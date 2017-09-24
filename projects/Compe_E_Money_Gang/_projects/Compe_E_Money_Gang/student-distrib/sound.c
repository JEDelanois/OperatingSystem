 #include "sound.h"
 #include "idt.h"
 #include "lib.h"

int sound_freq = 500;
//taken from OSDev
 
 //Play sound using built in speaker
  void set_speaker(uint32_t nFrequence) {
 	uint32_t Div;
 	
 
        //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outb(0xB6, 0x43);		//0xB6->1011 0110 'channel 2', 'square wave generator'
 	outb((uint8_t) (Div), 0x42);
 	outb( (uint8_t) (Div >> 8), 0x42);

 }
 

  void stop_sound() {
 	uint8_t tmp = inb(0x61) & 0xFC;	//1111 1100	CLEAR LAST TWO BITS (in position)
 
 	outb(tmp, 0x61);
 }

 void realsound(){
 uint8_t tmp;
 
 tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(tmp | 3, 0x61);
	}
	
}

 void bootsound(){
 uint8_t tmp;
 
 tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(tmp | 3, 0x61);
	}
	
	int i,j;
 
   for ( i = 1 ; i <= 5767 ; i++ )
       for ( j = 1 ; j <= 8767 ; j++ )
       {}
	 stop_sound();
 
}

