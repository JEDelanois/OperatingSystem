/* tuxctl-ioctl.c
 *
 * Driver (skeleton) for the mp2 tuxcontrollers for ECE391 at UIUC.
 *
 * Mark Murphy 2006
 * Andrew Ofisher 2007
 * Steve Lumetta 12-13 Sep 2009
 * Puskar Naha 2013
 */

#include <asm/current.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/miscdevice.h>
#include <linux/kdev_t.h>
#include <linux/tty.h>
#include <linux/spinlock.h>

#include "tuxctl-ld.h"
#include "tuxctl-ioctl.h"
#include "mtcp.h"


#define debug(str, ...) \
	////printk(KERN_DEBUG "%s: " str, __FUNCTION__, ## __VA_ARGS__)

/************************ Protocol Implementation *************************/

/* tuxctl_handle_packet()
 * IMPORTANT : Read the header for tuxctl_ldisc_data_callback() in 
 * tuxctl-ld.c. It calls this function, so all warnings there apply 
 * here as well.
 */
 
 /*
 here declare all of your button variables and clocks i thin
 
 download the tux test aswell
 */
 // bit masks used 
 #define bit0 1
 #define bit1 2
 #define bit2 4
 #define bit3 8
 #define bit4 16
 #define bit5 32
 #define bit6 64
 #define bit7 128
 
 //struct that represents the state of the controler
 typedef struct Tuxcont{
	int ack;
	int up;
	int down;
	int left; 
	int right;
	int start;
	int a;
	int b;
	int c;
	int reset;
	char leds[6];

 }tuxcont;
 tuxcont cont;
 
 spinlock_t cont_data_lock;
 

 /*
this function is responsible for
Inputs:	tty - passed throughout functions to refer to tux
		packet- pointer ot packed of infromation delivered 
				from the tux

Outputs: none

Side-Affects:
sets the global tux variables or resets the tux according to the 
command
*/
void tuxctl_handle_packet (struct tty_struct* tty, unsigned char* packet){
	
    unsigned a, b, c;

    a = packet[0]; /* Avoid ////printk() sign extending the 8-bit */
    b = packet[1]; /* values when printing them. */
    c = packet[2];
	
	
	//spin_lock_irq(&cont_data_lock);
	
	if(a == MTCP_RESET){ // if reset then change reset
		cont.ack = 1;
		tux_init(tty);
		//WHY does this function cause te kernel to cras?!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		 //tux_reset(tty);
		 cont.ack = 0;
		}
		
	else if(a == MTCP_ACK){// if ack signea
		cont.ack = 0;
		////printk("ack is now set!!!");
		}
	
	
	/*
;		Byte 0 - MTCP_BIOC_EVENT
;		byte 1  +-7-----4-+-3-+-2-+-1-+---0---+
;			| 1 X X X | C | B | A | START |
;			+---------+---+---+---+-------+
;		byte 2  +-7-----4-+---3---+--2---+--1---+-0--+
;			| 1 X X X | right | down | left | up |
;			+---------+-------+------+------+----+
	*/
	
	
	else if(a == MTCP_BIOC_EVENT){// if button press change buttons
		//ALL BUTTONS ARE ACTIVE LOW
		if( b & bit0)//if there is a one here button is not pressed
			cont.start = 0;
		else
			cont.start = 1;
		
		if( b & bit1)
			cont.a = 0;
		else
			cont.a = 1;
		
		if( b & bit2)
			cont.b = 0;
		else
			cont.b = 1;
		
		
		if( b & bit3)
			cont.c = 0;
		else
			cont.c = 1;
			
		
		
		if( c & bit0)
			cont.up = 0;
		else
			cont.up = 1;
			
		
		if( c & bit1)
			cont.left = 0;
		else
			cont.left = 1;
			
		if( c & bit2)
			cont.down = 0;
		else
			cont.down = 1;
			
		
		if( c & bit3)
			cont.right = 0;
		else
			cont.right = 1;
					
		}
		
	//printk("button is presse - %d",cont.down);
	
	//spin_unlock_irq(&cont_data_lock);
	

    /*////printk("packet : %x %x %x\n", a, b, c); */
}

/******** IMPORTANT NOTE: READ THIS BEFORE IMPLEMENTING THE IOCTLS ************
 *                                                                            *
 * The ioctls should not spend any time waiting for responses to the commands *
 * they send to the controller. The data is sent over the serial line at      *
 * 9600 BAUD. At this rate, a byte takes approximately 1 millisecond to       *
 * transmit; this means that there will be about 9 milliseconds between       *
 * the time you request that the low-level serial driver send the             *
 * 6-byte SET_LEDS packet and the time the 3-byte ACK packet finishes         *
 * arriving. This is far too long a time for a system call to take. The       *
 * ioctls should return immediately with success if their parameters are      *
 * valid.                                                                     *
 *                                                                            *
 ******************************************************************************/
 /*
 is called by mazegame and user level programs to send data to the tux controler
Inputs:	tty - passed throughout functions to refer to tux
		file - tux file 
		cmd - command the user would like to exicute
		

Outputs:

Side-Affects:
*/
int 
tuxctl_ioctl (struct tty_struct* tty, struct file* file, 
	      unsigned cmd, unsigned long arg)//arg 
{
    switch (cmd) {
	case TUX_INIT:
			return tux_init(tty);
	case TUX_BUTTONS:			
			return tux_buttons(arg);
	case TUX_SET_LED:
			return tux_set_led(tty,arg);
	
	//dont need to implement these three!!
	case TUX_LED_ACK:
	case TUX_LED_REQUEST:
	case TUX_READ_LED:
	default:
	    return -EINVAL;
    }
}
// why cant we use spin locks here?? so many pieces of code touching this data
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 /*
this function is responsible for initializing the tux control and global variables
Inputs:	tty - passed throughout functions to refer to tux

Outputs:ret zero if sucess

Side-Affects:
initializes everything!!! 
*/
int tux_init(struct tty_struct* tty){
	char op;
	int i;
	//spin_lock_irq(&cont_data_lock);
	cont.ack = 0;
	cont.up = 0;
	cont.down = 0;
	cont.left = 0; 
	cont.right = 0;
	cont.start = 0;
	cont.a = 0;
	cont.b = 0;
	cont.c = 0;
	for(i=0; i<6;i++)
		(cont.leds)[i] = (char)0;
	
	
	// possibly try in one instruction
	op = (char)MTCP_BIOC_ON;
	//tell the tux controler to go into interupt mode
	tuxctl_ldisc_put(tty, &op, 1);
	//spin_unlock_irq(&cont_data_lock);
	
	
	//tell the tux controler to go into user mode
	op = (char)MTCP_LED_USR; 
	tuxctl_ldisc_put(tty, &op, 1);
	

	
	return 0;
}


 /*
 function is called when the reset button is pressed. 
this function is responsible for
Inputs:	tty - passed throughout functions to refer to tux

Outputs:none

Side-Affects:
resets the tux controler to bioc mode and user set led mode
also restores the leds
*/
void tux_reset(struct tty_struct* tty){
	char op;
	char temp[6];
	int i;
	
	
	for(i=0; i<6;i++)
		temp[i] = (cont.leds)[i];
	
	tux_init(tty);
	
	for(i=0; i<6;i++)
		(cont.leds)[i] = temp[i];
		
	tuxctl_ldisc_put(tty, cont.leds, 6);
	
	
}
 /*
this function is responsible for returning the status of the buttons
to the user
Inputs:	arg - pointer to a interget the user wants to have chaingged

Outputs:returns 0 on sucess

Side-Affects:
gives sttus to ghe buttons to the user 
*/
int tux_buttons(unsigned long arg){
	// typecast the pointer
	int  p;
	unsigned int m;
	
	if((int*)arg == NULL)
		return -EINVAL;
	
	//clear what p is pointing to
	p = 0;
	
	//add bits where they need to go
	
	//spin_lock_irq(&cont_data_lock);
	
	if( cont.start == 1)
		p += bit0;
	
	if(cont.a == 1)
		p += bit1;
		
	if(cont.b == 1)
		p += bit2;
	
	if(cont.c == 1)
		p += bit3;
	
	if(cont.up == 1)
		p += bit4;
	
	if(cont.down == 1)
		p += bit5;
	
	if(cont.left == 1)
		p += bit6;
	
	if(cont.right ==1)
		p += bit7;
	
	//////printk("%x \n",p);

	//spin_unlock_irq(&cont_data_lock);
	p = ~p;
	p = p & 0xff;
	//printk("%x \n",p);
	m = (unsigned int)p;
	//printk("m = %x",m);
	copy_to_user((int*)arg, &m, (sizeof(unsigned int)));

	
	return 0;
}






//bit masks for corresponding components
//all powers of 2 coresponding the the correct bits
#define last_four_bits 15
#define dec0 16777216
#define dec1 33554432
#define dec2 67108864
#define dec3 134217728
#define led0 65536
#define led1 131072
#define led2 262144
#define led3 524288
/*
Inputs: arg - 32 bits 
				low 16 bits specity a hex value to be displayed
				low 4 bits 3rd byte specify leds to turn one
				low4 bits of highest byte 27:24 corespond to decimal points
Outputs: returns 0 when sucess and displays info on the LEDS

sideaffecs: sets the leds on the tux board
*/
int tux_set_led(struct tty_struct* tty, unsigned long arg){
//time intervol 50 ms
	int hex0, hex1, hex2, hex3,i;
	char command[6];
	static unsigned long prev = 0;
	//prevent spamming wait till ack is recived
	if(jiffies - prev < (HZ/20))
		return -EINVAL;
	prev = jiffies;
	
	if(cont.ack == 1)
		return -EINVAL;
	
	cont.ack = 1;

	////printk("This is the ack %d",cont.ack);
	/*
	while(1){
		spin_lock_irq(&cont_data_lock);
		if(cont.ack == 0)
			break;
		spin_unlock_irq(&cont_data_lock);
		
	}
	// now set ack feild to signify we are waiting on an ack again
	cont.ack = 1;
	spin_unlock_irq(&cont_data_lock);
	*/   
	//assign most recent parameters
	
	//get 4 individual hex values
	hex0 = arg & last_four_bits;
	hex1 = (arg >> 4) & last_four_bits;
	hex2 = (arg >> 8) & last_four_bits;
	hex3 = (arg >> 12) & last_four_bits;
	
	//set command to send
	command[0] = MTCP_LED_SET;
	command[1] = (char)last_four_bits;// always send all 4 led info
	
	if(arg & led0)
		command[2] = hex_to_dips(hex0,(int)(arg & dec0));
	else	
		command[2] = (char)0;
		
	if(arg & led1)
		command[3] = hex_to_dips(hex1,(int)(arg & dec1));
	else	
		command[3] = (char)0;
		
	if(arg & led2)
		command[4] = hex_to_dips(hex2,(int)(arg & dec2));
	else	
		command[4] = (char)0;
		
	if(arg & led3)
		command[5] = hex_to_dips(hex3,(int)(arg & dec3));
	else	
		command[5] = (char)0;
	
	for(i = 0;i <6; i++)
		(cont.leds)[i] = command[i];  
	
	return tuxctl_ldisc_put(tty, command, 6);
	
	
}


//led values that corespond to their places in the register
#define d 1
#define b 2
#define c 4
#define g 8
#define dp 16
#define f 32
#define e 64
#define a 128
/*
Input: an integer that represents a hexidecimal number
		if there is a decimal or not
		dec == 0 if no decimal
		decimal != 0 if there is a decimal
output: a char that coresponds to the leds that need to be turned on
		with the decimal points:
*/
char hex_to_dips(int hex, int decimal){
	int temp = 0;
	
	//all this converts the decimal value in hex into the display leds
	if(hex == 0)
		temp = a+b+c+d+e+f;
	
	else if(hex == 1)
		temp = b+c;
	
	else if(hex == 2)
		temp = a+b+g+e+d;
	
	else if(hex == 3)
		temp = a+b+c+d+g;
	
	else if(hex == 4)
		temp = f+g+b+c;
	
	else if(hex == 5)
		temp = a+f+g+c+d;
	
	else if(hex == 6)
		temp = f+g+c+d+e+a;
	
	else if(hex ==7)
		temp = a+b+c;
	
	else if(hex == 8)
		temp = a+b+c+d+e+f+g;
	
	else if(hex == 9)
		temp = a+f+g+b+c;
	
	else if(hex == 10)//A
		temp = a+b+c+e+f+g;
	
	else if(hex == 11)//b
		temp = f+g+c+d+e;
	
	else if(hex == 12)//C
		temp = a+f+e+d;
	
	else if(hex == 13)//d
		temp = b+c+d+e+g;
	
	else if(hex == 14)//E
		temp = a+f+g+e+d;
	
	else if(hex == 15)//F
		temp = a+f+g+e;
	
	
	if(decimal == 0)// if no decimal then return with no decimal added
		return (char)temp;
	
	else{// if decimal add the decimal
		temp += dp;
		return (char)temp;
		}
	
	
}






