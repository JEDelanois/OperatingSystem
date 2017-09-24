#include "process_data.h"
#include "context_switch.h"
#include "x86_desc.h"
#include "paging.h"




/* This file contains the management of all of the data related to each process
including the PCB, TSS, and other information. This file doesn't include context switching.
(See context_switch.c for implementation) */


/*
 * create_PCB
 *   DESCRIPTION: Creates and Initializes relevant data for a process to be created
 *   INPUTS: parent_PCB -- Pointer to the parent process's PCB, or NULL for no parent.
 *			 args -- Arguments passed in and parsed by execute.
 *   OUTPUTS: none
 *   RETURN VALUE: returns the pointer to the newly created PCB.
 *   SIDE EFFECTS: none
 */  
PCB_struct_t* create_PCB(PCB_struct_t * parent_PCB, uint8_t* args){

	//get pointer for a new PCB
	int new_pid = new_pid_checkout();
	PCB_struct_t * new_PCB = (PCB_struct_t *) (END_KERNEL_MEM - KERNEL_STACK_SIZE*(new_pid + 1));
	
	
	//Start filling information
	(*new_PCB).pid = new_pid;
	(*new_PCB).user_stack_ptr = (uint32_t*) VIRT_USER_MEM;
	(*new_PCB).kernel_base_ptr = (uint32_t*) ((END_KERNEL_MEM - KERNEL_STACK_SIZE*(new_pid)) - 0x4);
	
		
	//We need to get the address of the page directory
	(*new_PCB).curr_page_dir = (uint32_t**)page_dir_array[new_pid];

	//Fill in the parent PCB pointer 
	(*new_PCB).parent_PCB_ptr = parent_PCB;

	//Use the parent's PCB to calculate the needed parent info.
	if (new_PCB->parent_PCB_ptr == NULL)
	{
		new_PCB->parent_page_dir = NULL;
		new_PCB->parent_kernel_stack = NULL;
	}
	else
	{
		(*new_PCB).parent_page_dir = (uint32_t**) (*(*new_PCB).parent_PCB_ptr).curr_page_dir;
		(*new_PCB).parent_kernel_stack = (*(*new_PCB).parent_PCB_ptr).kernel_base_ptr;
	}

	//update shell pids (value should only be 0, 1, or 2)
	if(new_pid < 3)
		(*new_PCB).shell = new_pid;
	else{
		(*new_PCB).shell = parent_PCB->shell;
		}
		
	//Initialize file array with stdin and stdout and clear all other indices
	set_file_array(new_PCB);

	//Set the arguments in the PCB based on the values passed in.
	int i;
	for(i = 0; i <KEYBOARD_BUFF_SIZE; i++){
		if(args[i] != NULL){
			(*new_PCB).arguments[i] = args[i];
		}
		else{
			(*new_PCB).arguments[i] = (uint32_t) 0;
		}
	}
	
	
	//update child and stack info for new PCB
	(*new_PCB).child_process_running = (uint32_t) 0;
	(*new_PCB).kernel_stack_ptr = (uint32_t*) ((END_KERNEL_MEM - KERNEL_STACK_SIZE*(new_pid)) - 0x4);

	return new_PCB;
}




/*
 * remove_PCB
 *   DESCRIPTION: Removes a PCB struct and checks out the pid
 *   INPUTS: pid -- the process id of the PCB needing to be removed
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */  
int remove_PCB(int pid){

	//free the pid in the pid array
	remove_pid(pid);
	return 0;
}

/*
 * set_file_array
 *   DESCRIPTION: Sets up a file array for a newly created PCB
 *   INPUTS: pcb -- PCB of the file array
 *   OUTPUTS: none
 *   RETURN VALUE: 0 
 *   SIDE EFFECTS: none
 */  
int32_t set_file_array(PCB_struct_t* pcb){
	
	//call helper functions to set up the file array
	stdin_set(pcb);
	stdout_set(pcb);
	clear_file_array(pcb);
	return 0;
}

/*
 * stdin_set
 *   DESCRIPTION: Sets up stdin index for the PCB's file array (called when new pcb created by set_file_array)
 *   INPUTS: pcb -- PCB of the file array
 *   OUTPUTS: none
 *   RETURN VALUE: 0 
 *   SIDE EFFECTS: none
 */  
int32_t stdin_set(PCB_struct_t* pcb){
	dentry_t dentry;
	stdin_open_file(&pcb->file_array[0], &dentry);		//call helper function to update file array
	return 0;
}

/*
 * stdout_set
 *   DESCRIPTION: Sets up stdout index for the PCB's file array (called when new pcb created by set_file_array)
 *   INPUTS: pcb -- PCB of the file array
 *   OUTPUTS: none
 *   RETURN VALUE: 0 
 *   SIDE EFFECTS: none
 */  
int32_t stdout_set(PCB_struct_t* pcb){
	dentry_t dentry;
	stdout_open_file(&pcb->file_array[1], &dentry);		//call helper function to update file array
	return 0;
}

/*
 * clear_file_array
 *   DESCRIPTION: Clears file array for given PCB
 *   INPUTS: pcb -- PCB of the file array
 *   OUTPUTS: none
 *   RETURN VALUE: 0 
 *   SIDE EFFECTS: none
 */  
int32_t clear_file_array(PCB_struct_t* pcb){
	int fd;
	
	//clear file array by setting proper values (doesn't need to be called on stdin or stdout)
	for(fd = 2; fd < 8; fd++){ 
		pcb->file_array[fd].fop_table_ptr = NULL;
		pcb->file_array[fd].inode_ptr = NULL;
		pcb->file_array[fd].file_pos = 0;
		pcb->file_array[fd].flags = 0;
	}
	return 0;
}


/*
 * get_PCB
 *   DESCRIPTION: Returns the address of the current PCB struct using the TSS's esp0.
 *   INPUTS:
 *   OUTPUTS: none
 *   RETURN VALUE: address of the current PCB
 *   SIDE EFFECTS: none
 */  
PCB_struct_t* get_PCB(){

	int address;								//Holds the value to be returned.
	int k_stack_ptr = (int) tss.esp0;			//Holds the kernel stack pointer.

	address = k_stack_ptr & 0xFFFFE000;			//Bitmask to jump to the correct kernel stack boundary
	return (PCB_struct_t*) address;
}


/*
 * get_PCB_via_pid
 *   DESCRIPTION: Returns the address of the PCB struct corresponding to the pid provided
 *   INPUTS:
 *   OUTPUTS: none
 *   RETURN VALUE: address of the PCB 
 *   SIDE EFFECTS: none
 */  
PCB_struct_t* get_PCB_via_pid(int pid){

	PCB_struct_t * address = (PCB_struct_t*) (END_KERNEL_MEM - KERNEL_STACK_SIZE*(pid + 1));

	return address;
}

/*
 * prep_PCB
 *   DESCRIPTION: Special create_PCB helper for the initialization of our terminals
 *   INPUTS:	new_pid - pid of the new PCB 
				parent_PCB - Pointer to the parent process's PCB, or NULL for no parent
				args - arguments for the new PCB
 *   OUTPUTS: none
 *   RETURN VALUE: returns the pointer to the newly created PCB
 *   SIDE EFFECTS: none
 */
PCB_struct_t* prep_PCB(int new_pid, PCB_struct_t * parent_PCB, uint8_t* args){

	//update pid_array and find a pointer to the new PCB
	pid_array[new_pid] = 1;
	PCB_struct_t * new_PCB = (PCB_struct_t *) (END_KERNEL_MEM - KERNEL_STACK_SIZE*(new_pid + 1));

	//Start filling information
	(*new_PCB).pid = new_pid;
	(*new_PCB).user_stack_ptr = (uint32_t*) VIRT_USER_MEM;
	(*new_PCB).kernel_base_ptr = (uint32_t*) ((END_KERNEL_MEM - KERNEL_STACK_SIZE*(new_pid)) - 0x4);
	(*new_PCB).shell = new_pid;
	
	//We need to get the address of the page directory
	(*new_PCB).curr_page_dir = (uint32_t**)page_dir_array[new_pid];

	//Fill in the parent PCB pointer (If we have global PCB pointer, use that and change it here)
	(*new_PCB).parent_PCB_ptr = parent_PCB;

	//Use the parent's PCB to calculate the needed parent info.
	if (new_PCB->parent_PCB_ptr == NULL)
	{
		new_PCB->parent_page_dir = NULL;
		new_PCB->parent_kernel_stack = NULL;
	}
	else
	{
		(*new_PCB).parent_page_dir = (uint32_t**) (*(*new_PCB).parent_PCB_ptr).curr_page_dir;
		(*new_PCB).parent_kernel_stack = (*(*new_PCB).parent_PCB_ptr).kernel_base_ptr;
	}

	//Initialize file array with stdin and stdout and clear all other idxs
	set_file_array(new_PCB);

	//Set the arguments in the PCB based on the values passed in.
	int i;
	for(i = 0; i <KEYBOARD_BUFF_SIZE; i++){
		if(args[i] != NULL){
			(*new_PCB).arguments[i] = args[i];
		}
		else{
			(*new_PCB).arguments[i] = (uint32_t) 0;
		}
	}

	//update fields of new PCB with appropriate values
	(*new_PCB).child_process_running = (uint32_t) 0;
	(*new_PCB).kernel_stack_ptr = (uint32_t*) ((END_KERNEL_MEM - KERNEL_STACK_SIZE*(new_pid)) - 0x4);

	return new_PCB;
}



