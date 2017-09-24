#ifndef _FILESYS_DRIVER_H
#define _FILESYS_DRIVER_H

#include "types.h"
#include "file_descriptor.h"
#include "dentry.h"
#include "process_data.h"

//pointer to the beginng of the file system
extern int FILE_SYSTEM; 

//used to return file names to the tab completion
int32_t tab_help(int8_t* buff, uint32_t offset, uint32_t length);

//used to open a file in the file descriptor array
int32_t file_open(file_entry_t * file_desc_entry, dentry_t* dentry);

//used to close a file in the file descriptor array
int32_t file_close();

//used to write to a file
int32_t file_write(const void* buf, int32_t nbytes);

//used to open a directory in the file descriptor array
int32_t directory_open(file_entry_t * file_desc_entry,dentry_t * dentry);

//used to close a directory in the file descriptor array
int32_t directory_close();

//used to add another file to the directory
int32_t directory_write(const void* buf, int32_t nbytes);
 

//prints out all files in the directory
//int32_t directory_read(int file_des, int8_t* buff, uint32_t length);
int32_t directory_read(uint32_t dir_idx, int8_t* buff, uint32_t offset, uint32_t length);

//read function used by system call
int32_t read_file_sys(uint32_t fd, uint8_t* buff, uint32_t offset, uint32_t length);

//reads file to buffer based on file name
int32_t read_file_to_buffer(uint8_t* name, uint8_t* buff, uint32_t offset, uint32_t length);

//reads file to the screen
extern int32_t read_file_to_mem(uint8_t* name, uint8_t* dest, uint32_t offset);

//populates a dentry with data coresponding to the given file name
extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);

//populates a dentry with data coresponding to the given index into the directory
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);

//reads data into the buffer specifed by the inode
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif /* _FILESYS_DRIVER_H */


/*
 * 
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   SIDE EFFECTS: none
 */ 

