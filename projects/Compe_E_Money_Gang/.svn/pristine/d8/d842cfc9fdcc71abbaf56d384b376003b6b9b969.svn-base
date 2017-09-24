#ifndef _PROCESS_DATA_H
#define _PROCESS_DATA_H

#include "x86_desc.h"
#include "file_descriptor.h"
#include "terminal_driver.h"
#include "dentry.h"

//buffer sizes
#define KEYBOARD_BUFF_SIZE 128
#define FILE_DESCRIP_ARRAY_SIZE 8	

//memory locations
#define VIRT_USER_MEM 0x8000000		//128MB
#define END_KERNEL_MEM 0x800000		//8MB
#define KERNEL_STACK_SIZE 0x2000	//8KB

int shell_pid;		//pid for the current shell

//The entire PCB, housing all required process data.
typedef struct PCB_struct {
	int pid;
	uint32_t* execute_ebp;						//ebp associated with execute and halt
	uint32_t* context_ebp;						//ebp used for context switching
	uint32_t* user_stack_ptr;					//bottom of the user stack
	uint32_t* kernel_base_ptr; 					//bottom of kernel stack
	uint32_t* kernel_stack_ptr;					//top of the kernel stack (used for context switching)
	uint32_t** curr_page_dir;					//page directory of PCB
	uint32_t** parent_page_dir;					//page directory of parent PCB
	struct PCB_struct* parent_PCB_ptr;			//pointer to parent PCB
	struct PCB_struct* child_PCB_ptr;			//pointer to the child process
	uint32_t* parent_kernel_stack;				//kernel stack pointer for parent
	uint32_t child_process_running;				// 0 if the process does not have a child, 1 if the process has a child
	file_entry_t file_array[FILE_DESCRIP_ARRAY_SIZE];	//file entry array of the process
	uint8_t arguments[KEYBOARD_BUFF_SIZE];		//arguments passed in and parsed by execute
	int shell;	//shell that process is called from

} PCB_struct_t;


//Process data manipulation functions:

//creates a new process structure. Requires a pointer to the parent PCB and all the arguments that
//were passed in when the process was called with execute.
extern PCB_struct_t* create_PCB(PCB_struct_t * parent_PCB, uint8_t* args);

//Special create_PCB helper for the initialization of our terminals
extern PCB_struct_t* prep_PCB(int new_pid, PCB_struct_t * parent_PCB, uint8_t* args);

/* Removes a PCB from use, used when a process needs to be squashed. */
extern int remove_PCB(int pid);

/* Returns the address of the current PCB based off of the TSS information. */
PCB_struct_t* get_PCB();

/* Gets a PCB struct pointer based on the pid provided*/
extern PCB_struct_t* get_PCB_via_pid(int pid);

//sets entire file array at initialization
int32_t set_file_array(PCB_struct_t* pcb);

//sets the stdin file array entry
int32_t stdin_set(PCB_struct_t* pcb);

//sets the stdout file array entry
int32_t stdout_set(PCB_struct_t* pcb);

//clears all 2-7 indices
int32_t clear_file_array(PCB_struct_t* pcb);


#endif /* _PROCESS_DATA_H */

