#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

#define VIDEO_MEM_IDX 0xB8
#define KERNEL_LOC 0x400000
#define MB_PAGE_BIT 0x80
#define NOT_PRESENT 0x2
#define PRESENT_SUPERVISOR 0x3
#define PRESENT_UNSUPERVISED 0x7
#define NUM_ENTRIES 1024
#define GLOBAL 0x100
#define USER_VIRT_IDX 32
#define USER_VID_IDX 34
#define FOUR_MB 0x400000
#define PAGE_OBJ_SIZE 1024
#define FOUR_KB_ALIGN 4096
#define FAKE_VID_MEM 0x1000

//These are each of the page directories that are used in this OS
uint32_t page_dir[PAGE_OBJ_SIZE] __attribute__((aligned(FOUR_KB_ALIGN)));		//Init kernel directory
uint32_t user_1_dir[PAGE_OBJ_SIZE] __attribute__((aligned(FOUR_KB_ALIGN)));		//User page directories
uint32_t user_2_dir[PAGE_OBJ_SIZE] __attribute__((aligned(FOUR_KB_ALIGN)));
uint32_t user_3_dir[PAGE_OBJ_SIZE] __attribute__((aligned(FOUR_KB_ALIGN)));
uint32_t user_4_dir[PAGE_OBJ_SIZE] __attribute__((aligned(FOUR_KB_ALIGN)));
uint32_t user_5_dir[PAGE_OBJ_SIZE] __attribute__((aligned(FOUR_KB_ALIGN)));
uint32_t user_6_dir[PAGE_OBJ_SIZE] __attribute__((aligned(FOUR_KB_ALIGN)));

//These are the three page tables that are used for video mapping purposes
uint32_t first_page_table[PAGE_OBJ_SIZE] __attribute__((aligned(FOUR_KB_ALIGN)));
uint32_t second_page_table[PAGE_OBJ_SIZE] __attribute__((aligned(FOUR_KB_ALIGN)));
uint32_t third_page_table[PAGE_OBJ_SIZE] __attribute__((aligned(FOUR_KB_ALIGN)));

//global array that holds the addresses for each user program, indexed by pid
uint32_t* page_dir_array[6];

/*Initializes paging and sets up kernel and video memory pages. */
extern void initPaging();


#endif /* _PAGING_H */
