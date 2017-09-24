#include "lib.h"
#include "x86_desc.h" 
#include "keyboard.h"
#include "i8259.h"
#include "wrapper.h"
#include "idt.h"
#include "terminal_driver.h"
#include "system_call_handler.h"
#include "context_switch.h"
#include "process_data.h"
#include "mouse.h"

//The array that is used to map the keyboard's values to their ASCII equivalents. Capitals are offset by 256
//		which is outside of the keyboard's range. This is used for shifting cases.
unsigned char keycode_to_ascii[512] = {
	//Normal characters block											127 is delete
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', (unsigned char)127, 0, 'q', 'w', 'e', 'r',  
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 
	0, '`', 0, 0x5C, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' ', 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	//Capitals/symbols block (The empty space above is used by the keyboard
	// to signal that the key has been let go, thus we must create new space to use)
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0, 'Q', 'W', 'E', 'R', 
	'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', 
	0, '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//tab and enter is 28
//These flags keep track of whether or not their corresponding button is being held down
int shift = 0;
int control = 0;
int alt = 0;

//These flags keep track of the state that their corresponding buttons toggle
int capital_mode = 0;
int num_lock = 0;

//The keyboard buffer that keeps the history of each character that has been typed (one for each terminal)
unsigned char keyboard_buff[3][KEY_BUFF_SIZE+1];

//The index that points to the current place to put a new character in the corresponding keyboard_buff
int buffIdx[3];

//Keeps track of whether or not enter has been pressed for each terminal
volatile int enter_state[3] = {0,0,0};

//These keep track of the RGB states 
char large_change = 0;			//If set, change RGB values by a greater amount
char color_mode = 1;			//If set, change the color of the background, if clear, change text color


/*
 *   init_keyboard
 *   DESCRIPTION: initializes the keyboard device
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Updates IDT for the keyboard (0x21) 
 */   
void init_keyboard() {
	
	set_intrpt_gate_desc(IRQ1);						//update idt struct

	SET_IDT_ENTRY(idt[IRQ1], keyboard_wrapper);		//set table and offsets from struct

	enable_irq(1);									//enable on the pic
	
	//Initialize the buff indices to -1, signaling that they are empty
	buffIdx[0] = -1;
	buffIdx[1] = -1;
	buffIdx[2] = -1;
	
	return;
}


/*
 * keyboard_handler
 *   DESCRIPTION: Handles the interrupt from IRQ1, which is the keyboard. This is a very large handler, but it
 *					is all quite necessary for correct functionality (across multiple additions of code)
 *   INPUTS: none
 *   OUTPUTS: prints a character to the screen 
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Disables keyboard interrupts during process.
 */   
void keyboard_handler() {
	//Ensure that the PIC re-enables the keyboard interrupts
	send_eoi(1);
	
	int currX,currY,noNLine;				//Variables to hold the current screen information
	int old_pid;							//Contains the pid of the shell that we are switching away from
	PCB_struct_t* curr_PCB = get_PCB();		//Obtain the PCB of the currently running process
	
	//read keyboard stroke
	int button = inb(KEYBOARD_PORT);

	//This controls all of the RGB features, contains cases for the button presses that are related.
	video_helper(button);

	//Control all of the special cases (modifiers, mode management, and tab completion)
	switch(button){
		case NUMB_LOCK: num_lock = !num_lock;
				break;
		case CAPS_LOCK: capital_mode = !capital_mode;
				break;
		case LEFT_SHIFT: shift = 1;
				break;
		case RIGHT_SHIFT: shift = 1;
				break;
		case L_SHIFT_REL: shift = 0;
				break;
		case R_SHIFT_REL: shift = 0;
				break;
		case ALT: alt = 1;
				break;
		case ALT_REL: alt = 0;
				break;
		case CTRL: control = 1;
				break;
		case CTRL_REL: control = 0;
				break;
		case TAB: pre_tab_complete();
				return;
		default:break;
	}

	//If the up or down arrow is pressed while numlock is off, use command history
	if((num_lock == 0) && ((button == UP)||(button == DOWN))){
			acess_history(button);
		}

	//Checks if alt is held down while F% is pressed, which switches the shells
	if(alt){
		old_pid = shell_pid;					//Store the shell we're moving away from
		switch(button){
			case F1: shell_pid = 0;				//Change the current shell to the corresponding one
					switch_shell(old_pid);		//Call the helper function that switches to the new shell
					break;
			case F2: shell_pid = 1;
					switch_shell(old_pid);
					break;
			case F3: shell_pid = 2;
					switch_shell(old_pid);
					break;
			default: break;
		}
	}
	

	//This next section does the actual printing to the screen

	//Checks the scancodes to see if the button is a letter (affected by capslock - shift combo)
	int alpha = ((TOP_ROW_LOW < button)&&(button < TOP_ROW_HIGH)) || 
				((MID_ROW_LOW < button)&&(button < MID_ROW_HIGH)) ||
				((BOT_ROW_LOW < button)&&(button < BOT_ROW_HIGH));

	//If the button is unmapped, there is no need to traverse the rest of the handler
	if(keycode_to_ascii[button] == (char) 0)
		return;

	//Check if Control-L is pressed, and clear the screen and screen information as well as clearing the buffer
	if ((control == 1) && (button == L_BUTTON)){

		//save current screen information (process running, not what is shown on screen)
		currX = screen_x;
		currY = screen_y;
		noNLine = noNewline;
		
		//set screen info for the terminal we are looking at (if different terminals)
		if(shell_pid != curr_PCB->shell){
			screen_x = x_of_term[shell_pid];
			screen_y = y_of_term[shell_pid];
			noNewline = no_new_line[shell_pid];
		}

		//clear screen
		keyboard_clear();

		//save the changes to the screen info we are looking at
		x_of_term[shell_pid] = screen_x;
		y_of_term[shell_pid] = screen_y;
		no_new_line[shell_pid] = noNewline;
	
		//restore the screen info for the process running (if different terminals)
		if(shell_pid != curr_PCB->shell){
				screen_x = currX;
				screen_y = currY;
				noNewline = noNLine;
			}

		//clear keyboard buffer
		int a;
		for(a = 0; a <= buffIdx[shell_pid]; a++){
				keyboard_buff[shell_pid][a] = (char)0;
		}
		buffIdx[shell_pid] = -1;	//Signal that the keyboard buffer is empty
	}
	else if(capital_mode == 0){
			keyboardToScreen(keycode_to_ascii[button + shift*SHIFT_OFFSET]);		//Print while capslock off
	}
	else if(alpha){
			keyboardToScreen(keycode_to_ascii[button + (!shift)*SHIFT_OFFSET]);		//Print alpha characters with capslock on
	}
	else{
			keyboardToScreen(keycode_to_ascii[button + shift*SHIFT_OFFSET]);		//Print numbers/symbols with capslock on
	}
		
	return;
}





/*
 * keyboardToScreen
 *   DESCRIPTION: Helper function to write to the current terminal. It manages the screen position of 
 *					the current process as well as the terminal to write to.
 *   INPUTS: -- keyPress: The char that sould be written to the screen.
 *   OUTPUTS: prints a character to the screen 
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Saves changed screen information into the global variabes
 */   
void keyboardToScreen(char keyPress)
{ 
	int currX,currY,noNLine;
	PCB_struct_t* curr_PCB = get_PCB();

	//If the key to print is delete, then call the delete printing function (in lib.c)
	if(keyPress ==  (char)127){
		if(buffIdx[shell_pid] != -1){		//No need to delete if the buffer is empty
		
		currX = screen_x;					//save screen info for current process
		currY = screen_y;
		noNLine = noNewline;
		
		if(shell_pid != curr_PCB->shell){		//If the terminal to write to is different, then restore
			screen_x = x_of_term[shell_pid];		//the screen information for the displayed terminal.
			screen_y = y_of_term[shell_pid];
			noNewline = no_new_line[shell_pid];
		}	

		//Use delete character helper function to 'print' the delete (in lib.c)
		keyboard_deleteChar();

		x_of_term[shell_pid] = screen_x;		//save the changed screen information
		y_of_term[shell_pid] = screen_y;
		no_new_line[shell_pid] = noNewline;

		if(shell_pid != curr_PCB->shell){		//If the terminal to write to was different, then
			screen_x = currX;						//restore the saved screen information
			screen_y = currY;
			noNewline = noNLine;
		}
		
		//After printing to the screen, erase the character from the keyboard buffer
		keyboard_buff[shell_pid][buffIdx[shell_pid]] = (char)0;
		buffIdx[shell_pid]--;
		}
	}
	else if(keyPress == '\n'){	//Manage the case for a newline/return
		
		currX = screen_x;					//save screen info for current process
		currY = screen_y;
		noNLine = noNewline;
	
		if(shell_pid != curr_PCB->shell){		//If the terminal to write to is different, then restore
			screen_x = x_of_term[shell_pid];		//the screen information for the displayed terminal.
			screen_y = y_of_term[shell_pid];
			noNewline = no_new_line[shell_pid];
		}

		//Use the character helper function to print the newline to screen (in lib.c)
		keyboard_putc(keyPress);
		
		x_of_term[shell_pid] = screen_x;		//save the changed screen information
		y_of_term[shell_pid] = screen_y;
		no_new_line[shell_pid] = noNewline;
			
		if(shell_pid != curr_PCB->shell){		//If the terminal to write to was different, then
			screen_x = currX;						//restore the saved screen information
			screen_y = currY;
			noNewline = noNLine;
		}
	
		//Place the newline in the buffer and signal to terminal read that enter was pressed.
		buffIdx[shell_pid]++;
		keyboard_buff[shell_pid][buffIdx[shell_pid]] = '\n';
		if(reading[shell_pid])
			enter_state[shell_pid] = 1;

		//If the terminal that is written to (the displayed one) is not reading, clear the buffer.
		if (!reading[shell_pid])
		{
			int a;
			for(a = 0; a <= buffIdx[shell_pid]; a++){
				keyboard_buff[shell_pid][a] = (char)0;		//Clear the keyboard buffer
			}
			buffIdx[shell_pid] = -1;						//Reset the index to signal empty
		}
		reading[shell_pid] = 0;
	}
	else if (buffIdx[shell_pid] < (KEY_BUFF_SIZE - 1)){	//The rest of the characters can simply be printed

		currX = screen_x;					//save screen info for current process
		currY = screen_y;
		noNLine = noNewline;
	
		if(shell_pid != curr_PCB->shell){		//If the terminal to write to is different, then restore
			screen_x = x_of_term[shell_pid];		//the screen information for the displayed terminal.
			screen_y = y_of_term[shell_pid];
			noNewline = no_new_line[shell_pid];
		}

		//Use the character helper function to print the character to the screen (in lib.c)
		keyboard_putc(keyPress);

		x_of_term[shell_pid] = screen_x;		//save the changed screen information
		y_of_term[shell_pid] = screen_y;
		no_new_line[shell_pid] = noNewline;
		
		//restore lib c variables
		if(shell_pid != curr_PCB->shell){		//If the terminal to write to was different, then
			screen_x = currX;						//restore the saved screen information
			screen_y = currY;
			noNewline = noNLine;
		}
		
		//Increment the buffer index 
		buffIdx[shell_pid]++;
		//Place the character into the keyboard buffer
		keyboard_buff[shell_pid][buffIdx[shell_pid]] = keyPress;
	}
}




/*
 * video_helper
 *   DESCRIPTION: Helper function to manage the RGB suite (numpad buttons)
 *   INPUTS: -- button: The scancode of the button pressed, sent by the keyboard handler
 *   OUTPUTS: if the button is within the suite, affects the RGB values of the terminal(s)
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void video_helper(int button){

	int old_coord_y, old_coord_x, i;

	//Store the current screen RGB values in temporary variables
	outb(TEXT_ATTRIB, READ_SET_PORT);
	char temp_text_r = inb(VGA_RW_PORT);
	char temp_text_g = inb(VGA_RW_PORT);
	char temp_text_b = inb(VGA_RW_PORT);

	outb(BACK_ATTRIB, READ_SET_PORT);
	char temp_back_r = inb(VGA_RW_PORT);
	char temp_back_g = inb(VGA_RW_PORT);
	char temp_back_b = inb(VGA_RW_PORT);

	//The numpad period changes which coloring method to use for the mouse cursor
	if(button == NUM_PERIOD){				
		if(normal_cursor == 0){
			normal_cursor = 1;
			old_coord_y = old_mouse_y * NUM_COLS * 2;
			old_coord_x = old_mouse_x * 2;
			*(uint8_t *)(DIRECT_VID + old_coord_y + old_coord_x + 1) = INVERT_COLORS;
		}	
		else{
			normal_cursor = 0;
			old_coord_y = old_mouse_y * NUM_COLS * 2;
			old_coord_x = old_mouse_x * 2;
			*(uint8_t *)(DIRECT_VID + old_coord_y + old_coord_x + 1) = FLIPPED_COLORS;
		}

		return;
	}

	//Mask out the remaining functionality if numlock is off
	if(num_lock == 0)
		return;
	
	//Color mode decides whether you change the text or background color
	if(button == NUM_ASTERISK){
		if(color_mode == 0)
			color_mode = 1;				//Corresponds to changing background
		else
			color_mode = 0;				//Corresponds to changing text

		return;
	}

	//Enable the RGB changes to move at greater increments (8x larger increments)
	if(button == NUM_PLUS){	
		if(large_change == 0)
			large_change = 1;			//Change RGB quickly
		else
			large_change = 0;			//Fine tuning of RGB

		return;
	}

	//Copy the current color scheme to both of the other terminals by pressing the numberpad's minus
	if(button == NUM_MINUS){
		for(i = 0; i < 3; i++){
			red_text[i] = temp_text_r;
			green_text[i] = temp_text_g;
			blue_text[i] = temp_text_b;

			red_back[i] = temp_back_r;
			green_back[i] = temp_back_g;
			blue_back[i] = temp_back_b;
		}

	}

	if(button == NUM_ZERO){			//If numpad's zero is pressed, reset the terminal to default colors
		outb(TEXT_ATTRIB, WRITE_SET_PORT);		//Change the index to text, and then set RGB to default gray
		outb(DEF_GRAY, VGA_RW_PORT);
		outb(DEF_GRAY, VGA_RW_PORT);
		outb(DEF_GRAY, VGA_RW_PORT);

		outb(BACK_ATTRIB, WRITE_SET_PORT);		//Change the index to background and set RGB to default black
		outb(BLACK, VGA_RW_PORT);
		outb(BLACK, VGA_RW_PORT);
		outb(BLACK, VGA_RW_PORT);

		return;
	}


/*
	This large portion of code controls the behavior of the 9 numpad numbers in both text and background modes.
	Essentially each row corresponds to either R,G or B values, and the columns correspond to decrease, reset, and 
	increase. Simply described below:

		1,4,7 decrease the RGB value
		2,5,8 reset it to a neutral value
		3,6,9 increase the RGB value

		7,8,9 affect the Red values
		4,5,6 affect the Green values
		1,2,3 affect the Blue values
*/

	if(color_mode == 0){		//If we want to modify text color:
		outb(TEXT_ATTRIB, WRITE_SET_PORT);
		switch(button){
			case NUM_SEVEN: if((temp_text_r == (char) BLACK))				//Can't decrement below black
						break;
					if(((temp_text_r <= (char) 0x09)&&large_change))		//Cut off at black
						outb(BLACK, VGA_RW_PORT);
					else if(large_change)
						outb((temp_text_r - 0x08), VGA_RW_PORT);			//Change red value depending on
					else														//change speed
						outb((temp_text_r - 0x01), VGA_RW_PORT);
					outb(temp_text_g, VGA_RW_PORT);
					outb(temp_text_b, VGA_RW_PORT);	
					break;

			case NUM_EIGHT: outb(MIDDLE_VAL, VGA_RW_PORT);					//Reset red to a neutral value
					outb(temp_text_g, VGA_RW_PORT);
					outb(temp_text_b, VGA_RW_PORT);
					break;

			case NUM_NINE: if((temp_text_r == (char) MAX_VAL))				//Can't increment past max
						break;
					if(((temp_text_r >= (char) 0x36)&&large_change))		//Cut off at max
						outb(MAX_VAL, VGA_RW_PORT);
					else if(large_change)
						outb((temp_text_r + 0x08), VGA_RW_PORT);			//Change red value depending on
					else														//change speed
						outb((temp_text_r + 0x01), VGA_RW_PORT);
					outb(temp_text_g, VGA_RW_PORT);
					outb(temp_text_b, VGA_RW_PORT);
					break;


			case NUM_FOUR: if((temp_text_g == (char) BLACK))				//Can't decrement below black
						break;
					outb(temp_text_r, VGA_RW_PORT);
					if(((temp_text_g <= (char) 0x09)&&large_change))		//Cut off at black
						outb(BLACK, VGA_RW_PORT);
					else if(large_change)
						outb((temp_text_g - 0x08), VGA_RW_PORT);			//Change green value depending on
					else														//change speed
						outb((temp_text_g - 0x01), VGA_RW_PORT);
					
					outb(temp_text_b, VGA_RW_PORT);
					break;

			case NUM_FIVE: outb(temp_text_r, VGA_RW_PORT);				//Reset green to a neutral value
					outb(MIDDLE_VAL, VGA_RW_PORT);
					outb(temp_text_b, VGA_RW_PORT);
					break;

			case NUM_SIX: if((temp_text_g == (char) MAX_VAL))				//Can't increment past max
						break;
					outb(temp_text_r, VGA_RW_PORT);
					if(((temp_text_g >= (char) 0x36)&&large_change))		//Cut off at max
						outb(MAX_VAL, VGA_RW_PORT);
					else if(large_change)						
						outb((temp_text_g + 0x08), VGA_RW_PORT);			//Change green value depending on
					else														//change speed
						outb((temp_text_g + 0x01), VGA_RW_PORT);

					outb(temp_text_b, VGA_RW_PORT);
					break;


			case NUM_ONE: if((temp_text_b == (char) BLACK))					//Can't decrement below black
						break;
					outb(temp_text_r, VGA_RW_PORT);
					outb(temp_text_g, VGA_RW_PORT);
					if(((temp_text_b <= (char) 0x09)&&large_change))		//Cut off at black
						outb(BLACK, VGA_RW_PORT);
					else if(large_change)
						outb((temp_text_b - 0x08), VGA_RW_PORT);			//Change blue value depending on
					else														//change speed
						outb((temp_text_b - 0x01), VGA_RW_PORT);
					break;

			case NUM_TWO: outb(temp_text_r, VGA_RW_PORT);				//Reset blue to a neutral value
					outb(temp_text_g, VGA_RW_PORT);
					outb(MIDDLE_VAL, VGA_RW_PORT);
					break;

			case NUM_THREE: if((temp_text_b == (char) MAX_VAL))				//Can't inrement past max
						break;
					outb(temp_text_r, VGA_RW_PORT);
					outb(temp_text_g, VGA_RW_PORT);
					if(((temp_text_b >= (char) 0x36)&&large_change))		//Cut off at max
						outb(MAX_VAL, VGA_RW_PORT);
					else if(large_change)
						outb((temp_text_b + 0x08), VGA_RW_PORT);			//Change blue value depending on
					else														//change speed
						outb((temp_text_b + 0x01), VGA_RW_PORT);
					break;


			default: outb(temp_text_r, VGA_RW_PORT);		//If none of the buttons are pressed, just
					outb(temp_text_g, VGA_RW_PORT);					//restore the original values
					outb(temp_text_b, VGA_RW_PORT);
					break;
		}

	}
	else{				//If we want to modifiy background color:
		outb(BACK_ATTRIB, WRITE_SET_PORT);
		switch(button){
			case NUM_SEVEN: if((temp_back_r == (char) BLACK))				//Can't decrement below black
						break;
					if(((temp_back_r <= (char) 0x09)&&large_change))		//Cut off at black
						outb(BLACK, VGA_RW_PORT);
					else if(large_change)
						outb((temp_back_r - 0x08), VGA_RW_PORT);			//Change red value depending on
					else														//change speed
						outb((temp_back_r - 0x01), VGA_RW_PORT);
					outb(temp_back_g, VGA_RW_PORT);
					outb(temp_back_b, VGA_RW_PORT);	
					break;

			case NUM_EIGHT: outb(MIDDLE_VAL, VGA_RW_PORT);					//Reset red to a neutral value
					outb(temp_back_g, VGA_RW_PORT);
					outb(temp_back_b, VGA_RW_PORT);
					break;

			case NUM_NINE: if((temp_back_r == (char) MAX_VAL))				//Can't increment past max
						break;
					if(((temp_back_r >= (char) 0x36)&&large_change))		//Cut off at max
						outb(MAX_VAL, VGA_RW_PORT);
					else if(large_change)
						outb((temp_back_r + 0x08), VGA_RW_PORT);			//Change red value depending on
					else														//change speed
						outb((temp_back_r + 0x01), VGA_RW_PORT);
					outb(temp_back_g, VGA_RW_PORT);
					outb(temp_back_b, VGA_RW_PORT);
					break;


			case NUM_FOUR: if((temp_back_g == (char) BLACK))				//Can't decrement below black
						break;
					outb(temp_back_r, VGA_RW_PORT);
					if(((temp_back_g <= (char) 0x09)&&large_change))		//Cut off at black
						outb(BLACK, VGA_RW_PORT);
					else if(large_change)
						outb((temp_back_g - 0x08), VGA_RW_PORT);			//Change green value depending on
					else														//change speed
						outb((temp_back_g - 0x01), VGA_RW_PORT);
					
					outb(temp_back_b, VGA_RW_PORT);
					break;

			case NUM_FIVE: outb(temp_back_r, VGA_RW_PORT);				//Reset green to a neutral value
					outb(MIDDLE_VAL, VGA_RW_PORT);
					outb(temp_back_b, VGA_RW_PORT);
					break;

			case NUM_SIX: if((temp_back_g == (char) MAX_VAL))				//Can't increment past max
						break;
					outb(temp_back_r, VGA_RW_PORT);
					if(((temp_back_g >= (char) 0x36)&&large_change))		//Cut off at max
						outb(MAX_VAL, VGA_RW_PORT);
					else if(large_change)						
						outb((temp_back_g + 0x08), VGA_RW_PORT);			//Change green value depending on
					else														//change speed
						outb((temp_back_g + 0x01), VGA_RW_PORT);

					outb(temp_back_b, VGA_RW_PORT);
					break;


			case NUM_ONE: if((temp_back_b == (char) BLACK))					//Can't decrement below black
						break;
					outb(temp_back_r, VGA_RW_PORT);
					outb(temp_back_g, VGA_RW_PORT);
					if(((temp_back_b <= (char) 0x09)&&large_change))		//Cut off at black
						outb(BLACK, VGA_RW_PORT);
					else if(large_change)
						outb((temp_back_b - 0x08), VGA_RW_PORT);			//Change blue value depending on
					else														//change speed
						outb((temp_back_b - 0x01), VGA_RW_PORT);
					break;

			case NUM_TWO: outb(temp_back_r, VGA_RW_PORT);				//Reset blue to a neutral value
					outb(temp_back_g, VGA_RW_PORT);
					outb(MIDDLE_VAL, VGA_RW_PORT);
					break;

			case NUM_THREE: if((temp_back_b == (char) MAX_VAL))				//Can't inrement past max
						break;
					outb(temp_back_r, VGA_RW_PORT);
					outb(temp_back_g, VGA_RW_PORT);
					if(((temp_back_b >= (char) 0x36)&&large_change))		//Cut off at max
						outb(MAX_VAL, VGA_RW_PORT);
					else if(large_change)
						outb((temp_back_b + 0x08), VGA_RW_PORT);			//Change blue value depending on
					else														//change speed
						outb((temp_back_b + 0x01), VGA_RW_PORT);
					break;


			default: outb(temp_back_r, VGA_RW_PORT);		//If none of the buttons are pressed, just
					outb(temp_back_g, VGA_RW_PORT);					//restore the original values
					outb(temp_back_b, VGA_RW_PORT);
					break;
		}
	}


	//Re-obtain temporary values, as they may have changed above
	outb(TEXT_ATTRIB, READ_SET_PORT);	//Grab the color of the text
	temp_back_r = inb(VGA_RW_PORT);
	temp_text_g = inb(VGA_RW_PORT);
	temp_text_b = inb(VGA_RW_PORT);

	outb(BACK_ATTRIB, READ_SET_PORT);	//Grab the color of the background
	temp_back_r = inb(VGA_RW_PORT);
	temp_back_g = inb(VGA_RW_PORT);
	temp_back_b = inb(VGA_RW_PORT);

	//Calculate the inverted color for both text and background (used for one of the cursor modes)
	char inv_text_r = MAX_VAL - temp_text_r;
	char inv_text_g = MAX_VAL - temp_text_g;
	char inv_text_b = MAX_VAL - temp_text_b;
	char inv_back_r = MAX_VAL - temp_back_r;
	char inv_back_g = MAX_VAL - temp_back_g;
	char inv_back_b = MAX_VAL - temp_back_b;

	//Store the new inverted values into the attribute registers that are used by the inverted cursor
	outb(INVERT_TEXT_ATTRIB, WRITE_SET_PORT);
	outb(inv_text_r, VGA_RW_PORT);
	outb(inv_text_g, VGA_RW_PORT);
	outb(inv_text_b, VGA_RW_PORT);

	outb(INVERT_BACK_ATTRIB, WRITE_SET_PORT);
	outb(inv_back_r, VGA_RW_PORT);
	outb(inv_back_g, VGA_RW_PORT);
	outb(inv_back_b, VGA_RW_PORT);


}










