#include "paging.h"

/*extern void loadPageDirectory(unsigned int*);
extern void enablePaging();*/

// Code skeleton found at OSDev Wiki @ http://wiki.osdev.org/Setting_Up_Paging
void initPaging()
{
	//Fill all entries in the page directory with nonpresent entries
	int i;
	for(i = 0; i < 1024; i++){
		page_dir[i] = 0x00000002;
	}

	//Fill the first page table with nonpresent pages
	for(i = 0; i < 1024; i++){
			first_page_table[i] = (i * 0x1000) | 2;
	}

	// fill page with video memory
	first_page_table[0xB8] |= 3;

	//Fill the first two directory entries with the addresses needed
	page_dir[0] = ((unsigned int) first_page_table) | 3;
	page_dir[1] = (0x400000 | 3) | 0x80;

	asm volatile("movl %0, %%eax\n\
		 movl %%eax, %%cr3\n\
		 movl %%cr4, %%eax\n\
 		 orl $0x00000010, %%eax\n\
 		 movl %%eax, %%cr4\n\
		 movl %%cr0, %%eax\n\
		 orl $0x80000000, %%eax\n\
		 movl %%eax, %%cr0"
		 :
		 : "r" (page_dir)
		 : "%eax","cc", "memory"
		 );	
}
