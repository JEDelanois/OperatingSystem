#include "lib.h"
#include "x86_desc.h" 
#include "intrpt_idt_sets.h"
#include "intrpt_handlers.h"
#include "i8259.h"
#include "wrapper.h"


void set_intrpt_gate_desc(int vector) {

	idt[vector].seg_selector = 0x10;		
	//idt[vector].reserved4 = 
	idt[vector].reserved3 = 0;
	idt[vector].reserved2 = 1;
	idt[vector].reserved1 = 1;
	idt[vector].size = 1;
	idt[vector].reserved0 = 0;
	idt[vector].dpl = 0;
	idt[vector].present = 1;


	return;
}


//need to include file into kernel.c
void init_keyboard() {
	
	set_intrpt_gate_desc(IRQ1);				//update idt struct


	SET_IDT_ENTRY(idt[IRQ1], keyboard_wrapper);	//set table and offsets from struct

	enable_irq(1);							//enable on the pic
	
	return;
}




void init_rtc() {

	set_intrpt_gate_desc(IRQ8);		//update idt struct

	SET_IDT_ENTRY(idt[IRQ8], rtc_wrapper);	//set table and offsets from struct
	
	enable_irq(8);		//enable on the pic
	
	return;
}




















