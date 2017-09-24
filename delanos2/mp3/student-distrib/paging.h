#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
/* This initializes the paging system. Creates a paging directory, fill it with nonpresent
paging tables, and loads the appropriate values to the processor registers for proper
paging functionality */

uint32_t page_dir[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

extern void initPaging();


#endif /* _PAGING_H */
