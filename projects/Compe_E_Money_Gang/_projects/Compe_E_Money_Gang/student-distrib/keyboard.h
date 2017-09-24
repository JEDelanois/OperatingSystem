#ifndef _KEYBOARD_H
#define _KEYBOARD_H

//Descriptive Macros for Magic Numbers
#define IRQ1 0x21
#define IRQ8 0x28
#define KEYBOARD_PORT 0x60
#define KEY_BUFF_SIZE 128
#define SHIFT_OFFSET 256
#define TOP_ROW_HIGH 26
#define TOP_ROW_LOW 15
#define MID_ROW_HIGH 39
#define MID_ROW_LOW 29
#define BOT_ROW_HIGH 52
#define BOT_ROW_LOW 43

//Macros for scancodes
#define NUMB_LOCK 69
#define CAPS_LOCK 58
#define LEFT_SHIFT 42
#define RIGHT_SHIFT 54
#define L_SHIFT_REL 170
#define R_SHIFT_REL 182
#define ALT	56
#define ALT_REL 184
#define CTRL 29
#define CTRL_REL 157
#define UP 72
#define DOWN 80
#define TAB 15
#define F1 59
#define F2 60
#define F3 61
#define L_BUTTON 38
#define NUM_ASTERISK 55
#define NUM_PLUS 78
#define NUM_MINUS 74
#define NUM_ZERO 82
#define NUM_SEVEN 71
#define NUM_EIGHT 72
#define NUM_NINE 73
#define NUM_FOUR 75
#define NUM_FIVE 76
#define NUM_SIX 77
#define NUM_ONE 79
#define NUM_TWO 80
#define NUM_THREE 81
#define NUM_PERIOD 83

//Macros for video-related values
#define VGA_RW_PORT 0x03C9
#define WRITE_SET_PORT 0x03C8
#define READ_SET_PORT 0x03C7
#define BACK_ATTRIB 0x00
#define TEXT_ATTRIB 0x07
#define BLACK 0x00
#define DEF_GRAY 0x28
#define MIDDLE_VAL 0x20
#define INVERT_COLORS 0x12
#define FLIPPED_COLORS 0x70
#define INVERT_BACK_ATTRIB 0x01
#define INVERT_TEXT_ATTRIB 0x02
#define MAX_VAL 0x3F

//Global flags that keep track of keyboard information
extern int shift;
extern int control;
extern int capital_mode;

//Buffer and corresponding buffer information for each of the 3 terminals
extern unsigned char keyboard_buff[3][KEY_BUFF_SIZE+1];
extern int buffIdx[3];
extern volatile int enter_state[3];

//Initializes the keyboard for use
extern void init_keyboard();

//Helper function to change the RGB values corresponding to the button presses
void video_helper(int button);

//Handler that is called for a keyboard interrupt
extern void keyboard_handler();

//Properly write to the screen. (Do not mess up hidden terminals)
void keyboardToScreen(char keyPress);

#endif /* _KEYBOARD_H */
