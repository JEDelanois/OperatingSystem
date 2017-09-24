#ifndef _TERMINAL_DRIVER_H
#define _TERMINAL_DRIVER_H

#include "lib.h"
#include "keyboard.h"
#include "dentry.h"
#include "file_descriptor.h"
#include "process_data.h"

extern volatile int reading[3];

//function used by to prime the tab complete funtion
extern void pre_tab_complete();

//function that performs the tab completion
extern void tab_complete(unsigned char * buff);

//opens a file descriptor index to be set fro the terminal
int32_t terminal_open(file_entry_t * file_desc_array, dentry_t * dentry);

//closes the file descriptor index 
int32_t terminal_close();

//writs the given buffer to the video memeory
int32_t terminal_write(const uint8_t* buf, int32_t nbytes);

//reads in the keyboard input from the buffer
int32_t terminal_read(uint32_t fd, uint8_t* buff, uint32_t offset, uint32_t nbytes);

//acccesses the history when up or down buttons are pressed
void acess_history(int dir);

//displays the given string to the screen and places it into the keyboard buffer
void display_hist(uint8_t * string);

//used for error function in function pointer table
int32_t terminal_error_read(uint32_t fd, uint8_t* buff, uint32_t offset, uint32_t nbytes);

//used for error function in function pointer table
int32_t terminal_error_write(const void* buf, int32_t nbytes);

//used for error function in function pointer table
int32_t terminal_error_open(file_entry_t * file_desc_entry, dentry_t * dentry);

//opens file descriptor for stdin
int32_t stdin_open_file(file_entry_t * file_desc_entry, dentry_t * dentry);

//opens file descriptor for stdout
int32_t stdout_open_file(file_entry_t * file_desc_entry, dentry_t * dentry);

#endif /* _TERMINAL_DRIVER_H */

