#include "terminal_driver.h"
#include "process_data.h"
#include "lib.h"
#include "context_switch.h"
#include "filesys_driver.h"
#include "sound.h"

#define TOTAL_HIST 10
#define UP_BUTTON 72
#define DOWN_BUTTON 80
#define BYTES_IN_FILENAME 32
#define DEL_CHAR 127


volatile int reading[3] = {0,0,0};

// test harness variable
int goOn = 0;
//rolling buffer for command line
uint8_t comand_his[3][TOTAL_HIST][KEY_BUFF_SIZE+1];
int Hidx_start[3] = {0,0,0};
int Hnum[3] = {0,0,0};
int Hcurr[3] = {0,0,0};
int hist_dist[3] = {0,0,0};

/*
function pointer array called by the system call handler for terminal stdin and stdout
*/
func_ptr stdin_func_table[3] = { (func_ptr)terminal_error_open, (func_ptr)terminal_read, (func_ptr)terminal_error_write }; 
func_ptr stdout_func_table[3] = { (func_ptr)terminal_error_open, (func_ptr)terminal_error_read, (func_ptr)terminal_write }; 
func_ptr terminal_func_table[3] = { (func_ptr)terminal_open, (func_ptr)terminal_read, (func_ptr)terminal_write }; 


/*terminal_open
 * 
 *   DESCRIPTION:  this is called by the system call handler and it is used to
					set the give file descriptor array entry used its not really called
					
 *   INPUTS: 	file_entry_t * file_desc_entry- FDE that is to be set for a file 
				dentry_t * dentry- dentry associated with the file
				
 *   OUTPUTS:  sets up file descrpor array
 *   RETURN VALUE:  returns zero on success
 *   SIDE EFFECTS: none
 */ 
int32_t terminal_open(file_entry_t * file_desc_entry, dentry_t * dentry){
	
		file_desc_entry->fop_table_ptr = terminal_func_table;	//file op table pointer (function pointer)
		file_desc_entry->inode_ptr = '\0';
		file_desc_entry->file_pos = 0;
		file_desc_entry->flags = 1;
	
	
	
		return 0;
}


/*stdin_open_file
 * 
 *   DESCRIPTION:  this is called by the system call handler and it is used to
					set the give file descriptor array entry used to set up stdin
					in file descriptor array
					
 *   INPUTS: 	file_entry_t * file_desc_entry- FDE that is to be set for a file 
				dentry_t * dentry- dentry associated with the file
				
 *   OUTPUTS:  sets up file descrpor array
 *   RETURN VALUE:  returns zero on success
 *   SIDE EFFECTS: none
 */ 
int32_t stdin_open_file(file_entry_t * file_desc_entry, dentry_t * dentry){
	
		file_desc_entry->fop_table_ptr = stdin_func_table;	//file op table pointer (function pointer)
		file_desc_entry->inode_ptr = '\0';
		file_desc_entry->file_pos = 0;
		file_desc_entry->flags = 1;
		
	return 0;
}


/*stdout_open_file
 * 
 *   DESCRIPTION:  this is called by the system call handler and it is used to
					set the give file descriptor array entry used to set up stdout
					in file descriptor array
					
 *   INPUTS: 	file_entry_t * file_desc_entry- FDE that is to be set for a file 
				dentry_t * dentry- dentry associated with the file
				
 *   OUTPUTS:  sets up file descrpor array
 *   RETURN VALUE:  returns zero on success
 *   SIDE EFFECTS: none
 */ 
int32_t stdout_open_file(file_entry_t * file_desc_entry, dentry_t * dentry){
	
		file_desc_entry->fop_table_ptr = stdout_func_table;	//file op table pointer (function pointer)
		file_desc_entry->inode_ptr = '\0';	
		file_desc_entry->file_pos = 0;
		file_desc_entry->flags = 1;
		
	return 0;
}


/*
 * terminal_close
 * 
 *   DESCRIPTION:  meant to close down a file descriptor array when 
					file is no longer in use. We went with a different
					implementation and made one generic file close
					so this function does nothing
 *   INPUTS:  none
 *   OUTPUTS:  none
 *   RETURN VALUE: returns 0x04
 *   SIDE EFFECTS: never actually called
 */ 
int32_t terminal_close(){
		/*
		clear any terminal specific value
		*/
		clear();
		return 0;
}


/*terminal_write
 * 
 *   DESCRIPTION: this function is used to write to the screen when the user makes the 
					coresponding system call. it will print the the memory associated 
					with B8000 and will print all non null characters
 *   INPUTS: 	const uint8_t* buf - buffer filled with data to print to screen
				int32_t nbytes - number of bytes to print to the screen
 *   OUTPUTS: characters to screen
 *   RETURN VALUE: number of bytes attempted to print to screen
 *   SIDE EFFECTS: places characters in video memory 
 */ 
int32_t terminal_write(const uint8_t* buf, int32_t nbytes)
{
	int idx;
	
	
	for (idx = 0; idx < nbytes; idx++)
	{
		// skip over null characters when printing
		if (buf[idx] == '\0')
			continue;
		putc(buf[idx]);
	}
	if(strncmp(buf,"no such command",15 ) == 0){
		set_speaker(600);
		bootsound();
	}
	return idx;
}


/*terminal_error_read
 * 
 *   DESCRIPTION: dummy function used in function pointer arrays in stdin/stdout
 *   INPUTS: 	uint32_t fd	- nothing
				uint8_t* buff- nothing
				uint32_t offset- nothing
				uint32_t nbytes- nothing
 *   OUTPUTS: none
 *   RETURN VALUE:  error -1
 *   SIDE EFFECTS: none
 */ 
int32_t terminal_error_read(uint32_t fd, uint8_t* buff, uint32_t offset, uint32_t nbytes){
return -1;
}


/*
 * 
 *   DESCRIPTION: dummy function used in function pointer arrays in stdin/stdout
 *   INPUTS: 	uint32_t fd	- nothing
				uint8_t* buff- nothing
				uint32_t offset- nothing
				uint32_t nbytes- nothing
 *   OUTPUTS: none
 *   RETURN VALUE:  error -1
 *   SIDE EFFECTS: none
 */ 
int32_t terminal_error_write(const void* buf, int32_t nbytes){
return -1;
}


/*terminal_error_open
 * 
 *   DESCRIPTION: dummy function used in function pointer arrays in stdin/stdout
 *   INPUTS: 	uint32_t fd	- nothing
				uint8_t* buff- nothing
				uint32_t offset- nothing
				uint32_t nbytes- nothing
 *   OUTPUTS: none
 *   RETURN VALUE:  error -1
 *   SIDE EFFECTS: none
 */ 
int32_t terminal_error_open(file_entry_t * file_desc_entry, dentry_t * dentry){
return -1;
}




/*terminal_read
 * 
 *   DESCRIPTION: When this function is called it waits for the user to press 
					enter then it fills the proved buffer with the keyboard input 
					and returns. it also places commands into the command history
					
 *   INPUTS: 	uint32_t fd - not used
				uint8_t* buff - buffer to be filled with user input
				uint32_t offset - not used
				uint32_t nbytes - number of bytes to copy to buffer
 *   OUTPUTS: keyboard buffer to the code that called it 
 *   RETURN VALUE: number of bytes read
 *   SIDE EFFECTS: clears and resets the keyboard buffer and changes command history
 */ 
int32_t terminal_read(uint32_t fd, uint8_t* buff, uint32_t offset, uint32_t nbytes)
{
	PCB_struct_t* pcb;
	pcb = get_PCB();
	reading[pcb->shell] = 1;
	uint32_t bytes_read;
	int a;
	//increment the number of entries up to 9
	if(Hnum[pcb->shell] < (TOTAL_HIST -1))
		Hnum[pcb->shell]++;
	//else make the begining of the list the next 
	//index to support the rolling buffer
	else
		Hidx_start[pcb->shell] = (Hidx_start[pcb->shell]+1)%TOTAL_HIST;
		
		
	//clears the keyboard buffer and reaset the index
	for(a = 0; a <= buffIdx[pcb->shell]; a++){
		keyboard_buff[pcb->shell][a] = (char)0;
	}
	buffIdx[pcb->shell] = -1;
		
	while(enter_state[pcb->shell] == 0){
	//wait and do nothing
	}
	//reset the enter state
	enter_state[pcb->shell] = 0;
	//reset the history offset
	hist_dist[pcb->shell] = 0;
	//copy buffer into buffer passed and save into command hist
	for(a = 0; (a < (buffIdx[pcb->shell] + 1))  && (a < nbytes); a++){
		comand_his[pcb->shell][(Hidx_start[pcb->shell] + Hnum[pcb->shell])%TOTAL_HIST][a]= buff[a] = keyboard_buff[pcb->shell][a];
	}
	comand_his[pcb->shell][(Hidx_start[pcb->shell] + Hnum[pcb->shell])%TOTAL_HIST][a-1] = NULL;
	bytes_read = a;
	
	//clears the keyboard buffer and reaset the index
	for(a = 0; a <= buffIdx[pcb->shell]; a++){
		keyboard_buff[pcb->shell][a] = (char)0;
	}
	
	buffIdx[pcb->shell] = -1;
	//if you get here then enter has been pressed
	//and the curr for the history should
	//always begin at the start of this history
	Hcurr[pcb->shell] = (Hidx_start[pcb->shell] + Hnum[pcb->shell])%TOTAL_HIST;
	
	if(Hcurr[pcb->shell] < 0)
		Hcurr[pcb->shell] = TOTAL_HIST + Hcurr[pcb->shell];
	
	reading[pcb->shell] = 0;
	return bytes_read;
}


/*acess_history
 * 
 *   DESCRIPTION: this function is used to acces the history when		
					the user presses the up or down arrows.
					the history is diplayed on the screen and placed into 
					the keyboard buffer
 *   INPUTS: int dir - is the direction pressed in on the keyboard
 *   OUTPUTS: screen displays history 
 *   RETURN VALUE: none
 *   SIDE EFFECTS: keyboard buffer is clobbered and filled with history
 */ 
void acess_history(int dir){
	int i;

	
	// if up was pressed
	if(dir == UP_BUTTON){	
		// dist is the indx of the history you are in
		//check distance into history
		if(hist_dist[shell_pid] + 1 == (Hnum[shell_pid]) ){
			return;
		}
		//increment distance into history
		hist_dist[shell_pid]++;
		
		// first delete the characters off the screen
		for(i = buffIdx[shell_pid]; i > -1; i--){
			//send delete character
			keyboardToScreen((char)DEL_CHAR);
		}
		
		//display the history command on the screen
		display_hist(&comand_his[shell_pid][Hcurr[shell_pid]%TOTAL_HIST][0]);
		
		//update the global variables so they print to screen properly
		Hcurr[shell_pid] = (Hcurr[shell_pid] - 1)%TOTAL_HIST;
	}
	//if down was pressed
	else{
		
		//check distance into history
		//if going past the bottom clear the keyboard buffer
		//and return
		
 		if((hist_dist[shell_pid] == 0)|| (hist_dist[shell_pid] == 1) ){
			for(i = buffIdx[shell_pid]; i > -1; i--){
				//send delete character
				keyboardToScreen((char)DEL_CHAR);
			}
			if(hist_dist[shell_pid] == 1)
				Hcurr[shell_pid] = (Hcurr[shell_pid] + 1)%TOTAL_HIST;
			hist_dist[shell_pid] = 0;
			return;
			
		} 
		//decrement distance into history
		hist_dist[shell_pid]--;
		
		// first delete the characters off the screen
		for(i = buffIdx[shell_pid]; i > -1; i--){
			//send delete character
			keyboardToScreen((char)DEL_CHAR);
		}
		
		//get to previouse history
		Hcurr[shell_pid] = (Hcurr[shell_pid]+2)%TOTAL_HIST;
		display_hist(&comand_his[shell_pid][Hcurr[shell_pid]%TOTAL_HIST][0]);
		
		//prime it for the next acces history call
		Hcurr[shell_pid] = (Hcurr[shell_pid] -1)%TOTAL_HIST;
		
	}
	
	// why dos % give you negative value
	//modify Hcurr to not provide any errors
	if(Hcurr[shell_pid] < 0)
		Hcurr[shell_pid] = TOTAL_HIST + Hcurr[shell_pid];
}


/*display_hist
 * 
 *   DESCRIPTION: this function takes in a string and adds it	
					to the keyboard buffer while also diplaying it 
					to the screen
 *   INPUTS: uint8_t * string - string to be added and dipslayed
 *   OUTPUTS: screen and keyboard buffer
 *   RETURN VALUE: none
 *   SIDE EFFECTS: screen and keyboard buffer
 */ 
void display_hist(uint8_t * string){
	int i;
;
	for(i = 0; string[i] != '\0'; i++){
		//print char to keyboard
		keyboardToScreen(string[i]);
		//keyboard_buff[shell_pid][buffIdx[shell_pid]] = string[i];
		
	}
}

//only looks at last word typed inorty dot be able to 
//tab complete multiple words in a command
/*
 * 
 *   DESCRIPTION: this function enables the tab completion for multiple words	
					in the command. it looks at the last word and send the 
					last word to get tab completed as opposed to the whole line
 *   INPUTS: none
 *   OUTPUTS: new tab completed command
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sceen and keyboard buffer are updated
 */ 
void pre_tab_complete(){
	int i;
	
	//if at last charcter in keyboard_buff then nothing to complete	
	// so return
	if((buffIdx[shell_pid] == (KEY_BUFF_SIZE -1)) || (reading[shell_pid] == 0))
		return;
		
	//make i corespond to the correct index
	for(i = buffIdx[shell_pid]; (i > -1) && (keyboard_buff[shell_pid][i] != ' '); i--){
		//do nothing
	}

	//call tab complete on the approprate inx in the keyboard buffer array
	tab_complete(&keyboard_buff[shell_pid][i + 1]);
}

/*tab_complete
 * 
 *   DESCRIPTION:  this function searches through the file system and finds possible matches	
					for command provided. This was cleaverly crafted to not use extra memory
					so it can be use with any sized file system! 
 *   INPUTS: unsigned char * buff - string to be tab completed and placed int the keyboard buffer
 *   OUTPUTS: new tab completed command	
 *   RETURN VALUE: none
 *   SIDE EFFECTS: screen and keyboard buffer
 */ 
void tab_complete(unsigned char * buff){

	int i,a;
	//this flag is zero untill the first match has been found. if a second match is
	//found it meas that there is more than one file name that can fit the given command
	//when tab was hit. if cut flag is 1 then it means that you should cur the current
	//new buff name to the be the least maximum common shared name between the exiting 
	//new name and the match just found
	int cut_flag = 0;
	int length_buff = (int) strlen((int8_t*) buff);
	unsigned char temp_name[KEY_BUFF_SIZE+1] = {(unsigned char)0};
	unsigned char new_buff[KEY_BUFF_SIZE+1] = {(unsigned char)0};
	
	//put current command into the new buffer
	for(i = 0; i < length_buff; i++){
		new_buff[i] = buff[i];
	}
	
	//check for matches in file system
	for(i = 0; 0 != tab_help((int8_t*) temp_name, i, BYTES_IN_FILENAME); i++){
		//if the file name matches
		if( 0 == strncmp((int8_t*) buff,(int8_t*) temp_name,strlen((int8_t*) buff))){
			//modify the current buffer to match the what tab should display
			if(cut_flag == 0){
				strcpy((int8_t*) new_buff,(int8_t*)  temp_name);
			}
			else{
				for(a = 0; a < BYTES_IN_FILENAME; a++){
					//if the charactss dont match break and delete rest of command
					if(temp_name[a] != new_buff[a]){
						break;
					}
				}
				for( ; a < BYTES_IN_FILENAME; a++){
					new_buff[a] = (unsigned char)0;
				}
			}
			cut_flag = 1;
		}
	}
	
	//clear keyboard buffer
	// first delete the characters off the screen
	for(i = 0; i < length_buff; i++){
		//send delete character
		keyboardToScreen((char)DEL_CHAR);
	}

	
	//dispay the new command
	display_hist(new_buff);
}

// int32_t terminal_read(char * string){
// 		/*
// 		reads bytes into terminal buffer 
// 		*/
// 		//if delete then delete only works for deletes from keyboard
// 		//will not work for delets in the middle of the string
// 		int i,a;
		
		
		
// 		if(string[0] ==  (char)DEL_CHAR){
// 			//if not at the beginning then delete 
// 			if((buffIdx[shell_pid] != -1)){
// 				keyboard_buff[buffIdx[shell_pid]] = (char)0;
// 				buffIdx[shell_pid]--;
// 			}
// 		}
		
// 		//handles case with 127 characters and enter push at the end
// 		else if(string[0] == '\n'){
// 					//MAKE FUNCTION CALL HERE TO HANDLE ENTERED COMMAND

// 					// test code for checkpoint 2
// 					int8_t * test = "continue";
// 					if (!strncmp(test, (int8_t *)keyboard_buff, strlen(test)+1))
// 						goOn = 1;
// 					else
// 						printf("\nType continue to go on: ");
					
// 					//this part clears the buffer
// 					for(a = 0; a <= buffIdx[shell_pid]; a++){
// 						keyboard_buff[a] = (char)0;
// 					}
// 					buffIdx[shell_pid] = -1;
// 		}
		
// 		else {
// 			//for all letters in string or untill buffer full
// 			for(i = 0;(string[i] != '\0') && (buffIdx[shell_pid] < (KEY_BUFF_SIZE-1)); i++){
// 				if(string[i] == '\n'){
// 					//MAKE FUNCTION CALL HERE TO HANDLE ENTERED COMMAND
					
// 					//this part clears the buffer
// 					for(a = 0; a <= buffIdx[shell_pid]; a++){
// 						keyboard_buff[a] = (char)0;
// 					}
// 					buffIdx[shell_pid] = -1;
// 				}
				
// 				else{
// 					//increment buffer position
// 					buffIdx[shell_pid]++;
// 					//copy character
// 					keyboard_buff[buffIdx[shell_pid]] = string[i];
// 				}
// 			}
// 		}
		
// 		//printf("\n BUFFER:%s \n INDEX:%d\n",keyboard_buff,buffIdx[shell_pid]);
// 		return 0;
// }

// int32_t terminal_write(char * string){
// 		/*
// 		writes count bytes to terminal screen
// 		keep track of verticle scrolling
// 		*/
// 		int i;
		
// 		//if delete then delete
// 		if(string[0] ==  (char)DEL_CHAR){
// 			if(buffIdx[shell_pid] != -1)
// 				deleteChar();
// 		}
		
// 		else {
// 			for(i = 0;(string[i] != '\0') && (buffIdx[shell_pid] < (KEY_BUFF_SIZE-1)); i++){
// 				printf("%c",string[i]);
// 			}
// 		}
// 		//printf(" \nINDEX:%d\n",buffIdx[shell_pid]);
// 		return 0;
// }

// void test_terminal_read()
// {
// 	printf("\nType continue to go on: ");
// 	while (!goOn);
// 	goOn = 0;
// 	clear();
// }
