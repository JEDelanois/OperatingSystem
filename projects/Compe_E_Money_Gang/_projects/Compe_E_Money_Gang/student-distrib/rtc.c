#include "lib.h"
#include "x86_desc.h" 
#include "keyboard.h"
#include "i8259.h"
#include "wrapper.h"
#include "idt.h"
#include "keyboard.h"
#include "rtc.h"
#include "process_data.h"

volatile int int_flag[3] = { 0, 0, 0 };

//Array that holds the counter values, use default count value corresponding to 1024Hz interrupt
volatile int process_counters[6] = { 1, 1, 1, 1, 1, 1 };
	
//Table for the file operations that can be performed on the RTC
func_ptr rtc_func_table[3] = { (func_ptr)rtc_open, (func_ptr)rtc_read, (func_ptr)rtc_write }; 

/*
 *   init_rtc
 *   DESCRIPTION: Initializes the RTC device, should only be called in kernel.c
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Updates IDT for the keyboard (0x28) and writes to register B to enable periodic interrupts 
 */  
void init_rtc() {

	set_intrpt_gate_desc(IRQ8);				//update idt struct

	SET_IDT_ENTRY(idt[IRQ8], rtc_wrapper);	//set table and offsets from struct
		
	//Enable periodic interrupts (Code modeled from OSDev)
	outb(RTC_REG_0, RTC_PORT_0);
	char temp = inb(RTC_PORT_1);
	outb(RTC_REG_0, RTC_PORT_0);
	outb((temp | 0x40), RTC_PORT_1);

	//Enable RTC interrupts on the PIC
	enable_irq(8);
	
	return;
}



/*
 * rtc_handler
 *   DESCRIPTION: Handler that is called for each RTC interrupt.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Clears each terminal's interrupt flag to signify that an interrupt has occured.
 */   
void rtc_handler() {

	//Must read register 0x0C to ensure interrupts continue
	outb(0x0C, RTC_PORT_0);	
	inb(RTC_PORT_1);		

	//Ensure that the PIC knows that the RTC interrupt can happen again
	send_eoi(8);

	int_flag[0] = 0;
	int_flag[1] = 0;
	int_flag[2] = 0;

	return;
}


/*
 * rtc_read
 *   DESCRIPTION: The function that is called by the read system call for RTC. Simply waits
 *					until the RTC generates an interrupt.
 *   INPUTS: All inputs are unused for this system call (they are leftover from the generalized read syscall)
 *   OUTPUTS: none
 *   RETURN VALUE: 0 after an RTC interrupt occurs (In all cases)
 *   SIDE EFFECTS: 
 */ 
int32_t rtc_read(int32_t fd,int8_t* buff, uint32_t offset, int32_t nbytes){
	PCB_struct_t* pcb = get_PCB();

	//Use a temporary value for the counting in the while loop
	int count = process_counters[pcb->pid];

	//Set the interrupt flag for the current terminal to 1
	int_flag[pcb->shell] = 1;
	while(count != 0){
		while(int_flag[pcb->shell] == 1){}		//Loop until the int_flag is set to 0 by the interrupt
		int_flag[pcb->shell] = 1;				//Reset the interrupt flag for the next interrupt	
		count--;								//Decrement the counter value
	}

	return 0;
}


/*
 * rtc_write
 *   DESCRIPTION: The function that is called by the write system call for RTC. It sets the value
 *					that rtc_read will count down from, which emulates a slower interrupt rate.
 *   INPUTS: nbytes -- must be 4 for proper functionality as per the system call's convention.
 *				buf -- void pointer to a 32 bit integer that holds the frequency requested.
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on successful change, -1 on invalid input.
 *   SIDE EFFECTS: 
 */ 
int32_t rtc_write(const void* buf, int32_t nbytes){

	int arg = *(int*)buf;				//Obtain the frequency that was requested
	PCB_struct_t* pcb = get_PCB();		//Get the current process's PCB to use it's pid
	
	if(nbytes != 4){
		return (int32_t) (-1);
	}

	//Map the argument to the proper number of counts at 1024Hz. (i.e. 2 counts @ 1024 Hz = 512 Hz)
	switch(arg){
		case 2: process_counters[pcb->pid] = 512;
				break;
		case 4: process_counters[pcb->pid] = 256;
				break;
		case 8: process_counters[pcb->pid] = 128;
				break;
		case 16: process_counters[pcb->pid] = 64;
				break;
		case 32: process_counters[pcb->pid] = 32;
				break;
		case 64: process_counters[pcb->pid] = 16;
				break;
		case 128: process_counters[pcb->pid] = 8;
				break;
		case 256: process_counters[pcb->pid] = 4;
				break;
		case 512: process_counters[pcb->pid] = 2;
				break;
		case 1024: process_counters[pcb->pid] = 1;
				break;
		default:		//If the argument is not correctly formatted, return an error
			return -1;
	}

	char rate = 6 & 0x0F;			//Set the interrupt frequency to max user value (1024 Hz)
	cli();							//Ensure that writing to the RTC is not interrupted
	outb(RTC_REG_1, RTC_PORT_0);
	char prev = inb(RTC_PORT_1);
	outb(RTC_REG_1, RTC_PORT_0);
	outb((prev & 0xF0) | rate, RTC_PORT_1);
	sti();							//Re-enable interrupts after communicating with RTC

	return 0;
}


/*
 * rtc_open
 *   DESCRIPTION: Changes the RTC interrupt frequency to 2 Hz regardless of input.
 *   INPUTS: Unused.
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 *   SIDE EFFECTS: 
 */ 
int32_t rtc_open(file_entry_t * file_desc_entry, dentry_t * dentry){

	//update file descriptor array
	file_desc_entry->fop_table_ptr = rtc_func_table;	//file op table pointer (function pointer)
	file_desc_entry->inode_ptr = NULL;	
	file_desc_entry->file_pos = 0;
	file_desc_entry->flags = 1;


	cli();									//Ensure that writing to the RTC is not interrupted
	outb(RTC_REG_1, RTC_PORT_0);						//Sets the RTC interrupt rate to 2Hz (MP 3.2 compliant)
	char prev = inb(RTC_PORT_1);						//  This rate is overwritten in both write and read
	outb(RTC_REG_1, RTC_PORT_0);
	outb((prev & 0xF0) | 0x0F, RTC_PORT_1);
	sti();									//Re-enable interrupts after communicating with RTC

	return 0;
}


/*
 * rtc_close
 *   DESCRIPTION: Currently only prints to demonstrate that it is called. (Artifact of MP 3.2)
 *   INPUTS: Unused.
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 *   SIDE EFFECTS: 
 */ 
int32_t rtc_close(int32_t fd){

	return 0;
}

	
