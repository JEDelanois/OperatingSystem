#ifndef _SYSTEM_CALL_HANDLER_H
#define _SYSTEM_CALL_HANDLER_H
#include "types.h"
#include "dentry.h"
#include "file_descriptor.h"
#include "rtc.h"
#include "mouse.h"

#define CHUNK_SIZE 2000
#define USER_STACK 0x08400000 - 4
#define EXECUTABLE_SIZE 0x48000
#define MAX_CHARS 128
#define NUMBER_ARGS 10
#define MAX_FILE_IDX 7
#define MIN_FILE_IDX 2
#define END_VIRT_USER_MEM 0x8400000

#define ELF_SIZE 0x32

//Initializes the two extra shells that we need for multiple terminal support
extern int init_shell(int pid);

//All of the system calls that users can use. See function headers for details
int32_t halt_handler(uint8_t status);
int32_t execute_handler(const uint8_t* command);
int32_t read_handler(int32_t fd, void* buf, int32_t nbytes);
int32_t write_handler(int32_t fd, const void* buf, int32_t nbytes);
int32_t open_handler(const uint8_t* filename);
int32_t close_handler(int32_t fd);
int32_t getargs_handler(uint8_t* buf, int32_t nbytes);
int32_t vidmap_handler(uint8_t** screen_start);
int32_t set_handler_handler(int32_t signum, void* handler_address);
int32_t sigreturn_handler(void); 

//Helper functions to assist in the system calls
int32_t stdin_open();
int32_t stdout_open();
int32_t rm_white_space(const uint8_t* buff, int idx, int size);

//file system function pointers
typedef int32_t (*open_func)(file_entry_t * file_desc_array, dentry_t * dentry);
typedef int32_t (*read_func)(uint32_t fd, uint8_t* buff, uint32_t offset, uint32_t length);
typedef int32_t (*write_func)(const void* buf, int32_t nbytes);

#endif /* _SYSTEM_CALL_HANDLER_H */

