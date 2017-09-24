#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "wrapper.h"
#include "exception_handlers.h"
#include "keyboard.h"
#include "rtc.h"
#include "system_call_handler.h"
#include "system_jump_table.h"
#include "mouse.h"
#include "sound.h"


// MAGIC NUMBERS
#define SYSTEM_CALL 0x80
#define PIT_IRQ_PORT 0x40
#define PIT_MODE_PORT 0x43
#define PIT_INIT_COMMAND 0x30
#define PIT_COUNT_LO 0x7F
#define PIT_COUNT_HI 0xFF
#define NUM_EXCEPTIONS 20
#define USER_LEVEL 3
#define KERNEL_LEVEL 0



/*
 * initIDT
 *   DESCRIPTION: initializes the IDT
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */   
void initIDT()
{
	// populate idt
	exceptionEntries();

	interruptEntries();

	systemCallEntry();

	// load the idt
	lidt(idt_desc_ptr);
}

/*
 * exceptionEntries
 *   DESCRIPTION: map exception entries to IDT
 *   INPUTS: none
 *   OUTPUTS: none 
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes IDT
 */   
void exceptionEntries()
{
	// get params for other exceptions
	int idx = 0;
	while (idx < NUM_EXCEPTIONS) 
	{
		idt[idx].seg_selector = KERNEL_CS;
		idt[idx].present = 1;
		idt[idx].dpl = KERNEL_LEVEL;
		idt[idx].size = 1;
		idt[idx].reserved0 = 0;
		idt[idx].reserved1 = 1;
		idt[idx].reserved2 = 1;
		idt[idx].reserved3 = 1;

		if ((idx == 0) || (idx == 14) || (idx == 8))
			idx += 2;
		else
			idx++;
	}

	// get offset for all exceptions
	SET_IDT_ENTRY(idt[0],divideErrorWrapper);
	SET_IDT_ENTRY(idt[2],nmiIntWrapper);
	SET_IDT_ENTRY(idt[3],breakPointWrapper);
	SET_IDT_ENTRY(idt[4],overflowWrapper);
	SET_IDT_ENTRY(idt[5],rangeExceedWrapper);
	SET_IDT_ENTRY(idt[6],invalidOpWrapper);
	SET_IDT_ENTRY(idt[7],deviceNotAvailWrapper);
	SET_IDT_ENTRY(idt[8],doubleFaultWrapper);
	SET_IDT_ENTRY(idt[10],invalidTSSWrapper);
	SET_IDT_ENTRY(idt[11],segNotPresentWrapper);
	SET_IDT_ENTRY(idt[12],stackSegFaultWrapper);
	SET_IDT_ENTRY(idt[13],genProtectionWrapper);
	SET_IDT_ENTRY(idt[14],pageFaultWrapper);
	SET_IDT_ENTRY(idt[16],floatErrorWrapper);
	SET_IDT_ENTRY(idt[17],alignCheckWrapper);
	SET_IDT_ENTRY(idt[18],machineCheckWrapper);
	SET_IDT_ENTRY(idt[19],floatSIMDWrapper);
}

/*
 * interruptEntries
 *   DESCRIPTION: map interrupt entries to IDT
 *   INPUTS: none
 *   OUTPUTS: none 
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes IDT
 */   
void interruptEntries()
{

	// initialize keyboard
	init_keyboard();

	// initialize rtc
	init_rtc();
	
	init_PIT();
	
	init_mouse();

}

/*
 *   set_intrpt_gate_desc
 *   DESCRIPTION: updates IDT table for a specified vector
 *   INPUTS: int vector - vector of the IDT we wish to write to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Updates global IDT table 
 */   
void set_intrpt_gate_desc(int vector) {

	idt[vector].seg_selector = KERNEL_CS;		
	idt[vector].reserved3 = 0;
	idt[vector].reserved2 = 1;
	idt[vector].reserved1 = 1;
	idt[vector].size = 1;
	idt[vector].reserved0 = 0;
	idt[vector].dpl = KERNEL_LEVEL;
	idt[vector].present = 1;

	return;
}


/*
 * systemCallEntry
 *   DESCRIPTION: map system call entry to IDT
 *   INPUTS: none
 *   OUTPUTS: none 
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes IDT
 */   
void systemCallEntry()
{
	idt_desc_t systemCallStruct;

	systemCallStruct.seg_selector = KERNEL_CS;
	systemCallStruct.present = 1;
	systemCallStruct.dpl = USER_LEVEL;
	systemCallStruct.size = 1;
	systemCallStruct.reserved0 = 0;
	systemCallStruct.reserved1 = 1;
	systemCallStruct.reserved2 = 1;
	systemCallStruct.reserved3 = 1;

	idt[SYSTEM_CALL] = systemCallStruct;

	SET_IDT_ENTRY(idt[SYSTEM_CALL],systemCallHandler);

}

/*
 * init_PIT
 *   DESCRIPTION: Intializes the PIT to interrupt at 36.413 Hz for
 *					scheduling.
 *   INPUTS: none
 *   OUTPUTS: none 
 *   RETURN VALUE: none
 *   SIDE EFFECTS:
 */  

 
void init_PIT(){


	outb(PIT_INIT_COMMAND, PIT_MODE_PORT);
	outb(PIT_COUNT_LO, PIT_IRQ_PORT);
	outb(PIT_COUNT_HI, PIT_IRQ_PORT);

}




