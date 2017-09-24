/*
PROBLEM WITH PICS
should you enable or disable interupts in handler funcions?

when i do this we get an error because master pic is sending an interupt
for irq 2 when there is no interupts raised on the pic. WHY???
	
*/






/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

//index corresponds to what interrupt you would like a mask for
uint8_t irq_mask[8] = {	(uint8_t)0x01,
						(uint8_t)0x02,
						(uint8_t)0x04,
						(uint8_t)0x08,
						(uint8_t)0x10,
						(uint8_t)0x20,
						(uint8_t)0x40,
						(uint8_t)0x80};


/*
 * i8259_init
 *   DESCRIPTION: initializes PIC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initializes both pics and masks all interrupts except for IRQ2 on the master (for slave)
 */   
void
i8259_init(void)
{


//Write control words to ports/registers
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);
	
	outb(ICW2_MASTER,MASTER_DATA);
	outb(ICW2_SLAVE, SLAVE_DATA);
	
	outb(ICW3_MASTER,MASTER_DATA);
	outb(ICW3_SLAVE, SLAVE_DATA);
	
	outb(ICW4,MASTER_DATA);
	outb(ICW4, SLAVE_DATA);
	
	
	//write to mask interrupts for the slave and master
	outb((uint8_t)0xfb,MASTER_DATA);
	outb((uint8_t)0xff,SLAVE_DATA);	
	
	//mask all interrupts for master and slave
	master_mask = (uint8_t)0xfb;
	slave_mask = (uint8_t)0xff;
	

}


/*
 * enable_irq
 *   DESCRIPTION:	 Enable (unmask) the specified IRQ 
 *   INPUTS: irq_num - number of the interrupt we want to enable
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: unmasks specified IRQ only if it was currently masked
 */   
void
enable_irq(uint32_t irq_num)
{
	//enable irqs for master
	if(irq_num < 8){
		if(master_mask & irq_mask[irq_num]){			//if currently masked, unmask
			master_mask = master_mask - irq_mask[irq_num];	//set bit to 0
			
			outb(master_mask,MASTER_DATA);			//write to data port
		}
	}
	
	//enable irqs for slave
	else{		
		irq_num = irq_num - 8;			//subtract 8 to change slave's IRQ offset to use in irq_mask array
		if(slave_mask & irq_mask[irq_num]){
			slave_mask = slave_mask - irq_mask[irq_num];	//set bit to 0
			
			outb(slave_mask ,SLAVE_DATA);
		}
	}
}

/*
 * disable_irq
 *   DESCRIPTION:	 Disable (mask) the specified IRQ 
 *   INPUTS: irq_num - number of the interrupt we want to enable
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: masks specified IRQ only if it was currently unmasked
 */   
void
disable_irq(uint32_t irq_num)
{
	//send irq for master
	if(irq_num < 8){
		if(!(master_mask & irq_mask[irq_num])){			//if currently unmasked, mask
			master_mask = master_mask + irq_mask[irq_num];	//set bit to 1
			
			outb(master_mask,MASTER_DATA);
		}

	}
	
	//send irq for slave
	else{
		irq_num = irq_num - 8;				//create offset for slave IRQ to use in irq_mask array
		if(!(slave_mask & irq_mask[irq_num])){
			slave_mask = slave_mask + irq_mask[irq_num];		//set bit to 1
			
			outb(slave_mask ,SLAVE_DATA);
		}
	}




}


/*
 * disable_irq
 *   DESCRIPTION:	 Send end-of-interrupt signal for the specified IRQ 
 *   INPUTS: irq_num - number of the interrupt we want to send EOI for
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sends EOI signal to the master PIC
 */   
void
send_eoi(uint32_t irq_num)
{
	uint8_t eoi; 
	
	//EOI for master
	if( irq_num <8 ){
		eoi = (irq_num | EOI);			//or the EOI (0x60) with irq_num
		outb(eoi,MASTER_8259_PORT); 	//write specified EOI to master PIC
	}

	//EOI for slave
	else {
		eoi = ((irq_num-8) | EOI);
		outb(eoi,SLAVE_8259_PORT);
		
		eoi = ( 0x2| EOI);
		outb(eoi,MASTER_8259_PORT);
	
	}

	
}

