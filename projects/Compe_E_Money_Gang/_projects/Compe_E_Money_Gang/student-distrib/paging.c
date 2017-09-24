#include "paging.h"
#include "context_switch.h"
#include "lib.h"

/*
 * initPaging
 *   DESCRIPTION: Initializes the paging for both the base kernel directory and user directories
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets paging scheme and initializes kernel and video memory pages
 */   
void initPaging()
{
	//Fill the global page directory array with pointers to the corresponding user page directories
	page_dir_array[0] = user_1_dir;
	page_dir_array[1] = user_2_dir;
	page_dir_array[2] = user_3_dir;
	page_dir_array[3] = user_4_dir;
	page_dir_array[4] = user_5_dir;
	page_dir_array[5] = user_6_dir;

	//Fill all entries in the page directory with nonpresent entries to initialize
	int i;
	for(i = 0; i < NUM_ENTRIES; i++){
		page_dir[i] = NOT_PRESENT;
		user_1_dir[i] = NOT_PRESENT;
		user_2_dir[i] = NOT_PRESENT;
		user_3_dir[i] = NOT_PRESENT;
		user_4_dir[i] = NOT_PRESENT;
		user_5_dir[i] = NOT_PRESENT;
		user_6_dir[i] = NOT_PRESENT;
	}

	//Fill the page tables with nonpresent page table entries
		//(do not map to the same physical, for futureproofed malloc)
	for(i = 0; i < NUM_ENTRIES/3; i++){
			first_page_table[i] = (((3*i) * FAKE_VID_MEM) | NOT_PRESENT);
			second_page_table[i] = (((3*i + 1) * FAKE_VID_MEM) | NOT_PRESENT);
			third_page_table[i] = (((3*i + 2) * FAKE_VID_MEM) | NOT_PRESENT);
	}

	//Set the public video memory for the first process as present.
	first_page_table[VIDEO_MEM_IDX] |= PRESENT_SUPERVISOR;
	
	//Set the private video memories to present
	first_page_table[1] |= PRESENT_UNSUPERVISED;
	second_page_table[1] |= PRESENT_UNSUPERVISED;
	third_page_table[1] |= PRESENT_UNSUPERVISED;
	
	//Make a page table entry that will always point to public video memory (for copy purposes)
	first_page_table[4] = ((VIDEO) | PRESENT_SUPERVISOR);
	second_page_table[4] = ((VIDEO) | PRESENT_SUPERVISOR);
	third_page_table[4] = ((VIDEO) | PRESENT_SUPERVISOR);

	//Set the virtual video memory to point to the private video memories
	first_page_table[VIDEO_MEM_IDX] = first_page_table[1];
	second_page_table[VIDEO_MEM_IDX] = second_page_table[1];
	third_page_table[VIDEO_MEM_IDX] = third_page_table[1];
	
	//The generalized kernel page directory initialization. Give it everything for copying purposes (kernel.c)
	page_dir[0] = ((unsigned int) first_page_table) | PRESENT_SUPERVISOR;
	page_dir[1] = ((KERNEL_LOC | PRESENT_SUPERVISOR) | GLOBAL) | MB_PAGE_BIT;
	page_dir[2] = ((unsigned int)first_page_table) | PRESENT_SUPERVISOR;
	page_dir[3] = ((unsigned int)second_page_table) | PRESENT_SUPERVISOR;
	page_dir[4] = ((unsigned int)third_page_table) | PRESENT_SUPERVISOR;

	//The three paging structures that will always be used by their corresponding terminals (shells)
	user_1_dir[0] = ((unsigned int) first_page_table) | PRESENT_UNSUPERVISED;			//Video memory
	user_1_dir[1] = ((KERNEL_LOC | PRESENT_SUPERVISOR) | GLOBAL) | MB_PAGE_BIT;			//Kernel space
	user_1_dir[USER_VIRT_IDX] = ((FOUR_MB*2) | PRESENT_UNSUPERVISED) | MB_PAGE_BIT;		//User space

	user_2_dir[0] = ((unsigned int) second_page_table) | PRESENT_UNSUPERVISED;			//Video memory
	user_2_dir[1] = ((KERNEL_LOC | PRESENT_SUPERVISOR) | GLOBAL) | MB_PAGE_BIT;			//Kernel space
	user_2_dir[USER_VIRT_IDX] = ((FOUR_MB*3) | PRESENT_UNSUPERVISED) | MB_PAGE_BIT;		//user space

	user_3_dir[0] = ((unsigned int) third_page_table) | PRESENT_UNSUPERVISED;			//Video memory
	user_3_dir[1] = ((KERNEL_LOC | PRESENT_SUPERVISOR) | GLOBAL) | MB_PAGE_BIT;			//Kernel space
	user_3_dir[USER_VIRT_IDX] = ((FOUR_MB*4) | PRESENT_UNSUPERVISED) | MB_PAGE_BIT;		//User space

	//Three paging structures that will have their 0th index done dynamically, as we don't know what terminal they
		//will be running on
	user_4_dir[1] = ((KERNEL_LOC | PRESENT_SUPERVISOR) | GLOBAL) | MB_PAGE_BIT;			//Kernel space
	user_4_dir[USER_VIRT_IDX] = ((FOUR_MB*5) | PRESENT_UNSUPERVISED) | MB_PAGE_BIT;		//user space

	user_5_dir[1] = ((KERNEL_LOC | PRESENT_SUPERVISOR) | GLOBAL) | MB_PAGE_BIT;			//Kernel space
	user_5_dir[USER_VIRT_IDX] = ((FOUR_MB*6) | PRESENT_UNSUPERVISED) | MB_PAGE_BIT;		//User space

	user_6_dir[1] = ((KERNEL_LOC | PRESENT_SUPERVISOR) | GLOBAL) | MB_PAGE_BIT;			//Kernel space
	user_6_dir[USER_VIRT_IDX] = ((FOUR_MB*7) | PRESENT_UNSUPERVISED) | MB_PAGE_BIT;		//User space


	// Set control registers to enable paging and enable 4MB paging
	// Sets CR3 (PDBR) to use the intial kernel page_dir
	// Last 3 command lines are there to enable global paging (global entries that are not flushed from the TLB)
	asm volatile("movl %0, %%eax\n\
		 movl %%eax, %%cr3\n\
		 movl %%cr4, %%eax\n\
 		 orl $0x00000010, %%eax\n\
 		 movl %%eax, %%cr4\n\
		 movl %%cr0, %%eax\n\
		 orl $0x80000000, %%eax\n\
		 movl %%eax, %%cr0\n\
		 movl %%cr4, %%eax\n\
 		 orl $0x00000080, %%eax\n\
 		 movl %%eax, %%cr4"
		 :
		 : "r" (page_dir)
		 : "%eax","cc", "memory"
		 );	

}

