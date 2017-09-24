#include "system_call_handler.h"
#include "paging.h"
#include "process_data.h"
#include "filesys_driver.h"
#include "lib.h"
#include "context_switch.h"
#include "x86_desc.h"
#include "context_helper.h"
#include "terminal_driver.h"



/*
 * init_shell
 *   DESCRIPTION: This function sets up a shell to be picked up by scheduling once it is initialized. It is
 *					very similar to execute, however it never actually executes it, only sets it up.
 *   INPUTS: pid -- The pid of the shell you want to create
 *   OUTPUTS: none
 *   RETURN VALUE: always 0
 *   SIDE EFFECTS: none
 */ 
int init_shell(int pid){

	//We only want to initialized shells 1 or 2
	if((pid != 1)&&(pid != 2))
		return -1;
	//Make a fake command of shell, so that we can use the code from execute
	uint8_t command[MAX_CHARS] = "shell";
	uint8_t name[MAX_CHARS];
	uint8_t argz[MAX_CHARS];

	//The virtual address of the process image (code to be executed)
	uint8_t * image_loc = (uint8_t*) (VIRT_USER_MEM + EXECUTABLE_SIZE);

	//Need to parse the command line
	int i,a;
	for(i = rm_white_space( command, 0, MAX_CHARS),a = 0; i < MAX_CHARS; i++, a++){
		//when at the end of the name place a null character
		if((command[i] == ' ')||(command[i] == NULL)){
			name[a] = NULL;
			break;
		}
		//else copy the caracter into the name
		else{
			name[a] = command[i];
		}
	}
		
	// pick up where you left off for the "i" value
	for(i = rm_white_space( command, i, MAX_CHARS),a = 0; i < MAX_CHARS; i++, a++){
		//when at the end of the name place a null character
		if(command[i] == NULL){
			argz[a] = NULL;
			break;
		}
		//else copy the caracter into the argz
		else{
			argz[a] = command[i];
		}
	}
	
	//fill rest of argz with null characters
	for(;a < MAX_CHARS; a++){
		argz[a] = NULL;
	}
	

	//Set up paging for the new program by loading the correct page into CR3
	asm volatile("movl %0, %%eax\n\
		 movl %%eax, %%cr3\n"
		 :
		 : "r" (page_dir_array[pid])
		 : "%eax", "memory"
		 );

	//Grab the ELF header
	uint8_t ELF_header[ELF_SIZE];
	if (read_file_to_buffer(name, ELF_header, 0, ELF_SIZE) == -1)
		return -1;
	//Check if proper executable (ELF)
	if( (ELF_header[0] != 0x7f) ||
		(ELF_header[1] != 'E') ||
		(ELF_header[2] != 'L') ||
		(ELF_header[3] != 'F')){
		return -1;
	}

	//File Loading (put contents of executable in user space)
	read_file_to_mem(name, image_loc, 0);

	//Set the instruction pointer to the proper place requested by the program image (by convention)
	uint32_t * eip = (uint32_t*)(image_loc+24);

	//New PCB for the process
	PCB_struct_t* new_PCB;
	
	//Initialize the PCB for the specific case of a shell
	new_PCB = prep_PCB(pid, NULL, argz);

	//Load the TSS with the correct new kernel stack pointer (done at the end of the inline)
	asm volatile("movl %1, (%5)\n\
		movl %2, -4(%5)\n\
		push %%eax\n\
		pushf\n\
		pop %%eax\n\
		orl $0x200, %%eax\n\
		movl %%eax, -8(%5)\n\
		pop %%eax\n\
		movl %3, -12(%5)\n\
		movl %4, -16(%5)\n\
		pushl %%eax\n\
		movl %5, %%eax\n\
		subl $48, %%eax\n\
		movl $context_pop, -4(%%eax)\n\
		movl $context_pop, -8(%%eax)\n\
		subl $8, %%eax\n\
		movl %%eax, %0\n\
		popl %%eax\n"
	 : "=r" (new_PCB->kernel_stack_ptr)
	 : "r" (USER_DS), "r" (USER_STACK), "r" (USER_CS), "r" (*eip), "r" (new_PCB->kernel_base_ptr)
	 : "%eax", "memory"
	 );

	new_PCB->context_ebp = new_PCB->kernel_stack_ptr;

	tss.esp0 = (uint32_t) new_PCB->kernel_base_ptr;

	return 0;

}



/*
 * halt
 *   DESCRIPTION: Attempts to halt a program by decomissioning relevant structures and
 *					returning control flow to the process's parent program.
 *   INPUTS: status -- Contains the condition in which the halt was called.
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if cannot be executed, 256 if dies by exception, 0 to 255 for 
 *							if it executes a halt (the value that halt returns)
 *   SIDE EFFECTS: none
 */ 
int32_t halt_handler(uint8_t status){

	// Grab the current PCB to be decommissioned
	PCB_struct_t* halt_PCB;
	halt_PCB = get_PCB();
	int i;

	//If the last shell is trying to exit, restart a shell program at the same pid
	if(halt_PCB->pid < 3){
		remove_PCB(halt_PCB->pid);
		clear();
		execute_handler((uint8_t*) "shell");
	}
	
	//close all files that have been opened by the halting process
	for( i = 2; i < 8; i++){
		if(halt_PCB->file_array[i].flags == 1)
			close_handler(i);
	}
	//Set the parent PCB to no longer having a child out
	(halt_PCB->parent_PCB_ptr)->child_process_running = (uint32_t) 0;

	//Need to reset the process's RTC count value to default (for the next process to use the pid)
	process_counters[halt_PCB->pid] = 1;

	//Change esp0 to the PCB of the parent
	tss.esp0 = (uint32_t) (halt_PCB->parent_kernel_stack);

	//Restore parent's paging to CR3/PDBR
	asm volatile("movl %0, %%eax\n\
	 	movl %%eax, %%cr3\n"
	 :
	 : "r" (halt_PCB->parent_page_dir)
	 : "%eax", "memory"
	 );

	//Removed PCB entry from active list
	remove_PCB(halt_PCB->pid);

	//Transfer halt condition to be used by the inline below
	uint32_t asm_status = (uint32_t) status;

	//Set return value as the halt condition, and jmp out of the halt_handler
	asm volatile("movl %0, %%eax\n\
	 	movl %%eax, %%ebp\n\
	 	movl %1, %%eax\n\
	 	jmp halt_RET\n"
	 :
	 : "r" (halt_PCB->execute_ebp), "r" (asm_status)
	 : "%eax", "memory"
	 );

	//This will never be reached, as the inline jumps
	return 0;
}



/*
 * execute
 *   DESCRIPTION: Attempts to load and execute a new program by setting necessary PCB information
 *   INPUTS: command -- space separated sequence of words that are parsed to piece
 *							together the proper program and arguments.
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if cannot be executed, 256 if dies by exception, 0 to 255 for 
 *							if it executes a halt (the value that halt returns)
 *   SIDE EFFECTS: none
 *	TODO: parse the arguments from the command passed by the system call
 */  
int32_t execute_handler(const uint8_t* command){
	//Use get_new_pid() to get the pid of the new process (declaring this after create_PCB will provide the wrong pid)
	int new_pid = get_new_pid();

	// check if using all available processes
	if ((new_pid == 6) || (command == NULL))
		return -1;

	uint8_t name[MAX_CHARS];
	uint8_t argz[MAX_CHARS];
	//The virtual address of the process image (code to be executed)
	uint8_t * image_loc = (uint8_t*) (VIRT_USER_MEM + EXECUTABLE_SIZE);

	//Parse the command into an instruction name and arguments to that instruction
	int i,a;	
	for(i = rm_white_space( command, 0, MAX_CHARS),a = 0; i < MAX_CHARS; i++, a++){
		//when at the end of the name place a null character
		if((command[i] == ' ')||(command[i] == NULL)){
			name[a] = NULL;
			break;
		}
		//else copy the caracter into the name
		else{
			name[a] = command[i];
		}
	}
	
	// pick up where you left off for the "i" value
	for(i = rm_white_space( command, i, MAX_CHARS),a = 0; i < MAX_CHARS; i++, a++){
		//when at the end of the name place a null character
		if(command[i] == NULL){
			argz[a] = NULL;
			break;
		}
		//else copy the caracter into the argz
		else{
			argz[a] = command[i];
		}
	}
	
	//fill rest of argz with null characters
	for(;a < MAX_CHARS; a++){
		argz[a] = NULL;
	}
	
	//Grab the ELF header
	uint8_t ELF_header[ELF_SIZE];
	if (read_file_to_buffer(name, ELF_header, 0, ELF_SIZE) == -1)
		return -1;
	//Check if proper executable (ELF)
	if( (ELF_header[0] != 0x7f) ||
		(ELF_header[1] != 'E') ||
		(ELF_header[2] != 'L') ||
		(ELF_header[3] != 'F')){
		return -1;
	}

	//Set the parent PCB to null, will be replaced later if needed
	PCB_struct_t* parent_PCB = (PCB_struct_t*) NULL;

	//Take paren't video paging and give it to the child process
	if (new_pid > 2)
	{
		parent_PCB = get_PCB();
		page_dir_array[new_pid][0] = page_dir_array[parent_PCB->pid][0];
	}

	//Set up paging for the new program by loading the correct page into CR3
	asm volatile("movl %0, %%eax\n\
		 movl %%eax, %%cr3\n"
		 :
		 : "r" (page_dir_array[new_pid])
		 : "%eax", "memory"
		 );

	//File Loading (put contents of executable in user space)
	read_file_to_mem(name, image_loc, 0);

	//Set the instruction pointer to the proper place requested by the program image (by convention)
	uint32_t * eip = (uint32_t*)(image_loc+24);

	//New PCB for the process (Pass in either parent or NULL depending on pid.)
	PCB_struct_t* new_PCB;
	if(new_pid < 3)
		new_PCB = create_PCB(NULL, argz);
	else
	{
		parent_PCB = get_PCB();
		new_PCB = create_PCB(parent_PCB, argz);
	}

	//Set parent to having a child out
	if(parent_PCB != (PCB_struct_t*) NULL)
		parent_PCB->child_process_running = (uint32_t) 1;

	//Change the video memory to the correct paging.
	((uint32_t*) ( (uint32_t)(new_PCB->curr_page_dir[0]) & VID_MEM_NO_FLAGS ))[VIDEO_MEM_IDX] = ((uint32_t*) ( (uint32_t)(new_PCB->curr_page_dir[0]) & VID_MEM_NO_FLAGS ))[VIDEO_MEM_IDX] & FLAGS_WITHOUT_VID_MEM;		//clear top 20 bits
	if(new_PCB->shell == shell_pid)
	 	((uint32_t*) ( (uint32_t)(new_PCB->curr_page_dir[0]) & VID_MEM_NO_FLAGS ))[VIDEO_MEM_IDX] |= VIDEO;	//This is public video memory
	else
	 	((uint32_t*) ( (uint32_t)(new_PCB->curr_page_dir[0]) & VID_MEM_NO_FLAGS ))[VIDEO_MEM_IDX] |= ((uint32_t*) ( (uint32_t)(new_PCB->curr_page_dir[0]) & VID_MEM_NO_FLAGS ))[1];		//This is private video memory

	//Flush the TLB
	asm volatile("movl %%cr3, %%eax\n\
		 movl %%eax, %%cr3\n"
		 :
		 :
		 : "%eax", "memory"
		 );

	//Place ebp into the PCB for use in the halt system call
	asm volatile("movl %%ebp, %0\n"
		 : "=r" (new_PCB->execute_ebp)
		 : 
		 : "%eax", "memory"
		 );

	//Load the TSS with the correct new kernel stack pointer
	tss.esp0 = (uint32_t) new_PCB->kernel_base_ptr;

	//IRET --> Go to the program officially
	execute_IRET(*eip,USER_CS,USER_STACK,USER_DS);

	//This will never be reached, as execute_IRET begins processing for the new program
	return 0;
}


/*
 * read_handler 
 *   DESCRIPTION: Accomplishes the system call for read
 *   INPUTS:	fd - file descriptor number for the PCB's file descriptor array
 *				buf - the buffer to be read from
 *				nbytes - number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: The number of bytes read, or -1 on error
 *   SIDE EFFECTS: none
 */ 
int32_t read_handler(int32_t fd, void* buf, int32_t nbytes){


	PCB_struct_t* pcb;
	pcb = get_PCB();		//get PCB of running process
	int32_t ret_val;		//variable to hold return value
	
	//check descriptor boundaries and that file is opened
	if((fd < 0) || (fd > MAX_FILE_IDX) || (pcb->file_array[fd].flags == 0) || (buf == NULL))
		return -1;
	
	//call the 'read' helper function by using global function pointer array
	ret_val = ((read_func)pcb->file_array[fd].fop_table_ptr[1])(fd, buf, pcb->file_array[fd].file_pos, nbytes);

	//return the number of bytes read
	return ret_val;
	
}



/*
 * write_handler 
 *   DESCRIPTION: Accomplishes the system call for write
 *   INPUTS:	fd - file descriptor number for the PCB's file descriptor array
 *				buf - the buffer to write to
 *				nbytes - number of bytes to print to the terminal or frequency for the rtc	
 *   OUTPUTS: none
 *   RETURN VALUE: Number of bytes written (terminal write), 0 (rtc), or -1 on error
 *   SIDE EFFECTS: none
 */ 
int32_t write_handler(int32_t fd, const void* buf, int32_t nbytes){

	PCB_struct_t* pcb;
	pcb = get_PCB();

	//check descriptor boundaries
	if((fd < 0) || (fd > 7) || (pcb->file_array[fd].flags == 0) || (buf == NULL))
		return -1;

	//return bytes written (by terminal write), 0 (rtc), or -1
	return ((write_func)pcb->file_array[fd].fop_table_ptr[2])(buf, nbytes);	

}



/*
 * open_handler 
 *   DESCRIPTION: Accomplishes the system call for open
 *   INPUTS:	filename - pointer of file to be opened
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, or -1 on error
 *   SIDE EFFECTS: none
 */ 
int32_t open_handler(const uint8_t* filename){

PCB_struct_t* pcb;
pcb = get_PCB();		//get current PCB

int i=0;
int fda_idx = -1;	
int32_t error;
dentry_t dentry;

	//update dentry struct
	if (filename == NULL)
		return -1;

	error = read_dentry_by_name(filename,&dentry);

	if(error == -1)
		return -1;
	
	//choose open array entry
	for(i = MIN_FILE_IDX; i <= MAX_FILE_IDX; i++) {
		if(pcb->file_array[i].flags == 0) {
			fda_idx = i;
			break;
		}		
	}
	
	//if an open entry couldn't be found, return error
	if (fda_idx == -1)
		return -1;
		
		
	//check what kind of open to call
	if(dentry.fileType == 0) 
		{ rtc_open(&pcb->file_array[fda_idx], &dentry); }
		
	else if(dentry.fileType == 1) 
		{ directory_open(&pcb->file_array[fda_idx], &dentry); }
				
	else { file_open(&pcb->file_array[fda_idx], &dentry); }	


return fda_idx;
}



/*
 * close_handler
 *   DESCRIPTION: Closes a file in the file array of the current process.
 *   INPUTS:	fd - file descriptor number for the PCB's file descriptor array
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, or -1 on error
 *   SIDE EFFECTS: none
 */ 
int32_t close_handler(int32_t fd){

	PCB_struct_t* pcb;
	pcb = get_PCB();		//get current PCB

	//check for a valid fd entry
	if((fd < MIN_FILE_IDX) || (fd > MAX_FILE_IDX) || (pcb->file_array[fd].flags == 0) )
		return -1;
	
	//if valid file descriptor, free up the file array entry
	pcb->file_array[fd].fop_table_ptr = NULL;
	pcb->file_array[fd].inode_ptr = NULL;
	pcb->file_array[fd].file_pos = 0;
	pcb->file_array[fd].flags = 0;
	

return 0;
}


/*
 * getargs_handler
 *   DESCRIPTION: Gets the arguments of a process
 *   INPUTS:	buf - buffer we're getting args from
 *  			nbytes - number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, or -1 on error
 *   SIDE EFFECTS: none
 */ 
int32_t getargs_handler(uint8_t* buf, int32_t nbytes){
	PCB_struct_t* pcb;
	pcb = get_PCB();
	int i;

	
	for(i = 0; (i < nbytes) && (i < MAX_CHARS); i++){
		buf[i] = pcb->arguments[i];
	}

	// if at least last char is null then you know string was null terminated
	// i-1 since i will be incremented one last time after final index is filled
	if (pcb->arguments[i-1] == NULL)
		return 0;
	else
		return -1;
}



/*
 * vidmap_handler
 *   DESCRIPTION: Creates a map and returns the address in the supplied argument.
 *   INPUTS: screen_start -- Is a variable that is expected to be filled with the
 *								virtual address and then returned after paging.
 *   OUTPUTS: Maps 4KB of new user memory to map to the video memory.
 *   RETURN VALUE: 0 on success, or -1 on error
 *   SIDE EFFECTS: none
 */ 
int32_t vidmap_handler(uint8_t** screen_start){
	
	//Check for argument validity
	int screen_check = (int) screen_start;

	//Does it fall within User Space?
	if((screen_check >= END_VIRT_USER_MEM)&&(screen_check <= VIRT_USER_MEM))
		return -1;
	//Does it fall within Kernel Space?
	if((screen_check >= KERNEL_LOC)&&(screen_check <= END_KERNEL_MEM))
		return -1;
	//Does it fall within Video Memory?
	if((screen_check >= VIDEO)&&(screen_check <= END_VIDEO))
		return -1;
	//Is it a NULL pointer?
	if(screen_check == NULL)
		return -1;

	//Set User Video Memory to 136MB (4KB page)
	(*screen_start) = (uint8_t*) VIDEO;

return 0;
}



/*
 * set_handler_handler 
 *   DESCRIPTION: Handler for the set_handler system call. (EXTRA CREDIT)
 *   INPUTS: -- Unused
 *   OUTPUTS: none
 *   RETURN VALUE: -1, as it is not implemented
 *   SIDE EFFECTS: none
 */ 
int32_t set_handler_handler(int32_t signum, void* handler_address){
return -1;
}



/*
 * sigreturn_handler 
 *   DESCRIPTION: Handler for the sigreturn system call. (EXTRA CREDIT)
 *   INPUTS: -- Unused
 *   OUTPUTS: none
 *   RETURN VALUE: -1, as it is not implemented
 *   SIDE EFFECTS: none
 */ 
int32_t sigreturn_handler(void){
return -1;
}


////////////////////////////////////////
////////// HELPER FUNCTIONS ////////////
////////////////////////////////////////

/*
 * stdin_open
 *   DESCRIPTION: Helper function to open STDIN in the current process's file array
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: Always 0
 *   SIDE EFFECTS: none
 */ 
int32_t stdin_open(){
	PCB_struct_t* pcb;
	pcb = get_PCB();		//Get the current function's PCB
	dentry_t dentry;
	stdin_open_file(&pcb->file_array[0], &dentry);	//open STDIN in the file array
return 0;
}



/*
 * stdout_open 
 *   DESCRIPTION: Helper function to open STDOUT in the current process's file array
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: Always 0
 *   SIDE EFFECTS: none
 */ 
int32_t stdout_open(){
	PCB_struct_t* pcb;
	pcb = get_PCB();		//Get the current function's PCB
	dentry_t dentry;
	stdout_open_file(&pcb->file_array[1], &dentry);		//open STDOUT in the file array
return 0;
}



/*
 * rm_white_space
 *   DESCRIPTION:returns the index of the first non white space character after
 *				the given index
 *   INPUTS: --buff: the buffer to check for white spaces
 *			 --idx: the index of the buffer
 *			 --size: the size of the buffer
 *							
 *   OUTPUTS: none
 *   RETURN VALUE: 
 *							
 *   SIDE EFFECTS:
 *	TODO: 
 */  
int32_t rm_white_space(const uint8_t* buff, int idx, int size){
	
	while(idx < size){
		// if not at a white space then return current idx 
		if(buff[idx] != ' ')
			break;
		//look at next index
		idx++;
	}
	
	//if idx == to size then there is no white space at all in the buffer
	if(idx == size)
		return -1;
		
	else	
		return idx;
		
}















