/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

//index coresponds to what interupt you would like a mask for
uint8_t irq_mask[8] = {	(uint8_t)0x01,
						(uint8_t)0x02,
						(uint8_t)0x04,
						(uint8_t)0x08,
						(uint8_t)0x10,
						(uint8_t)0x20,
						(uint8_t)0x40,
						(uint8_t)0x80};

/* Initialize the 8259 PIC */
void
i8259_init(void)
{

 //outb(data, port)
//send all icw use out b command
/*
when do we write to which port 
online is says sometimes we write to ptc_1_data and PIC_!_CTRL
some students only write to PIC_1_DATA even for the 


when do we write to data port
when do write to control port?
are there any other ports?
why are there two instead of one?
*/

//Write control words to ports/registers
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);
	
	outb(ICW2_MASTER,MASTER_DATA);
	outb(ICW2_SLAVE, SLAVE_DATA);
	
	outb(ICW3_MASTER,MASTER_DATA);
	outb(ICW3_SLAVE, SLAVE_DATA);
	
	outb(ICW4,MASTER_DATA);
	outb(ICW4, SLAVE_DATA);

	// mask all interupts
	//are we writing to the correct ports here
	
	
	//write to mask to mask interups for the slave and master
	outb((uint8_t)0xfb,MASTER_DATA);
	outb((uint8_t)0xff,SLAVE_DATA);	
	
	//set current master and slave masks
	master_mask = (uint8_t)0xfb;
	slave_mask = (uint8_t)0xff;
	

}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
//send bits to imr of proper tux
	
	//change from 1 to 0
	//first check wich pic you need to be writing to
	if(irq_num < 8){
		if(master_mask & irq_mask[irq_num]){			//if this bit is 1
			master_mask = master_mask - irq_mask[irq_num];	//set bit to 0
			
			outb(master_mask,MASTER_DATA);
		}
	}
	
	else{
		irq_num = irq_num - 8;
		if(slave_mask & irq_mask[irq_num]){
			slave_mask = slave_mask - irq_mask[irq_num];	//set bit to 0
			
			outb(slave_mask ,SLAVE_DATA);
		}
	}
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
//change from 0 to 1
	if(irq_num < 8){
		if(!(master_mask & irq_mask[irq_num])){			//if this bit is 0
			master_mask = master_mask + irq_mask[irq_num];	//set bit to 1
			
			outb(master_mask,MASTER_DATA);
		}

	}
	
	else{
		irq_num = irq_num - 8;
		if(!(slave_mask & irq_mask[irq_num])){
			slave_mask = slave_mask + irq_mask[irq_num];		//set bit to 1
			
			outb(slave_mask ,SLAVE_DATA);
		}
	}




}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	uint8_t eoi; 
	
	if( irq_num <8 ){
		eoi = (irq_num | EOI);
		outb(eoi,MASTER_8259_PORT); 
	}

	
	else {
	
		eoi = ((irq_num-8) | EOI);
		outb(eoi,SLAVE_8259_PORT);
		
		eoi = ( 0x2| EOI);
		outb(eoi,MASTER_8259_PORT);
	
	}

	
}

