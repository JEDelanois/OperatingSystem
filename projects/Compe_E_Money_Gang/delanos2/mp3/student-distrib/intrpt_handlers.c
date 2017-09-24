
#include "lib.h"
#include "x86_desc.h" 
#include "intrpt_handlers.h"
#include "i8259.h"




static unsigned char keycode_to_ascii[250] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'q', 'w', 'e', 'r',  
	't', 'y', 'u', 'i', 'o', 'p', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	

void keyboard_handler() {
	disable_irq(1);

	
	//read keyboard stroke
	int button;
	char key;
	
	// what port do we need
	button = inb(key_board_port);
	if( button < 144){
//		printf("got button and it is %d\n",button);
	//get character from array using the stroke
	key = keycode_to_ascii[button];
//		printf("got key\n");
	//output it to the screen
	printf("%c", key);
	}
	else{
	printf("dumb compuer thinks this %d",button);
	}
	send_eoi(1);
	enable_irq(1);
	return;
}


void rtc_handler() {
	disable_irq(8);
	test_interrupts();			//call 'test_interrupts' handler from lib.c
	
	send_eoi(8);
	enable_irq(8);
	return;

}

