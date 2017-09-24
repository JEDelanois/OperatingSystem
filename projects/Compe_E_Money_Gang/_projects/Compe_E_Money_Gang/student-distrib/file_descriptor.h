#ifndef _FILE_DESCRIPTOR_H
#define _FILE_DESCRIPTOR_H

#include "types.h"

typedef int32_t (*func_ptr)(void);		//type for our 'generic' function array pointer

//Structure for keeping track of open files for the given process
typedef struct file_entry {
	func_ptr * fop_table_ptr;		//File Operations Table Pointer
	uint8_t * inode_ptr;
	uint32_t file_pos;
	uint32_t flags;

} file_entry_t;


#endif /* _FILE_DESCRIPTOR_H */
