/* lib.h - Defines for useful library functions
 * vim:ts=4 noexpandtab
 */

#ifndef _LIB_H
#define _LIB_H

#include "types.h"
#include "process_data.h"
#include "context_switch.h"

// MAGIC NUMBERS
#define VIDEO 0xB8000
#define END_VIDEO 0xB9000
#define NUM_COLS 80
#define NUM_ROWS 25
#define ATTRIB 0x7
#define BACKGROUND_ATTRIB 0x70
#define INVERT_ATTRIB 0x12
#define VID_MEM_LINE_2 VIDEO + 160
#define VID_MEM_SHIFT_LINES 4000 - 160
#define DIRECT_VID 0x4000
#define kVID_MEM_LINE_2 DIRECT_VID + 160
#define CRTC_ADDR 0x3D4
#define CRTC_DATA 0x3D5

// Output a string to direct physical address of video memory.
int32_t dir_puts(int8_t* s);
// Standard printf that goes directly to physical address of video memory
int32_t dir_printf(int8_t *format, ...);

int32_t printf(int8_t *format, ...);
// Output a character to memory pointed to by video_mem
void putc(uint8_t c);
int32_t puts(int8_t *s);
int8_t *itoa(uint32_t value, int8_t* buf, int32_t radix);
int8_t *strrev(int8_t* s);
uint32_t strlen(const int8_t* s);
// clears video memory pointed to by video_mem
void clear(void);
// clears video memory pointed to by video_mem
void clear_init(void);
extern void test_interrupts();

// Updates the text-mode cursor to hold the place that the next char will be printed.
void updateCursor();
// Deletes the last character from the memory pointed to by video_mem
void deleteChar();

// global flags for newline and x,y coordinates
extern int noNewline;
int screen_x;
int screen_y;


void* memset(void* s, int32_t c, uint32_t n);
void* memset_word(void* s, int32_t c, uint32_t n);
void* memset_dword(void* s, int32_t c, uint32_t n);
void* memcpy(void* dest, const void* src, uint32_t n);
void* memmove(void* dest, const void* src, uint32_t n);
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n);
int8_t* strcpy(int8_t* dest, const int8_t*src);
int8_t* strncpy(int8_t* dest, const int8_t*src, uint32_t n);
// Output a character to direct physical address of video memory
extern void keyboard_putc(uint8_t c);
// Deletes the last character from the physical address of video memory
extern void keyboard_deleteChar();
// clears video memory in DIRECT_VID (0x4000) which always points to 0xB8000 (video memory) in physical address
extern void keyboard_clear(void);
/* Userspace address-check functions */
int32_t bad_userspace_addr(const void* addr, int32_t len);
int32_t safe_strncpy(int8_t* dest, const int8_t* src, int32_t n);

/* Port read functions */
/* Inb reads a byte and returns its value as a zero-extended 32-bit
 * unsigned int */
static inline uint32_t inb(port)
{
	uint32_t val;
	asm volatile("xorl %0, %0\n \
			inb   (%w1), %b0" 
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
} 

/* Reads two bytes from two consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them zero-extended
 * */
static inline uint32_t inw(port)
{
	uint32_t val;
	asm volatile("xorl %0, %0\n   \
			inw   (%w1), %w0"
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
}

/* Reads four bytes from four consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them */
static inline uint32_t inl(port)
{
	uint32_t val;
	asm volatile("inl   (%w1), %0"
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
}

/* Writes a byte to a port */
#define outb(data, port)                \
do {                                    \
	asm volatile("outb  %b1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Writes two bytes to two consecutive ports */
#define outw(data, port)                \
do {                                    \
	asm volatile("outw  %w1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Writes four bytes to four consecutive ports */
#define outl(data, port)                \
do {                                    \
	asm volatile("outl  %l1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Clear interrupt flag - disables interrupts on this processor */
#define cli()                           \
do {                                    \
	asm volatile("cli"                  \
			:                       \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Save flags and then clear interrupt flag
 * Saves the EFLAGS register into the variable "flags", and then
 * disables interrupts on this processor */
#define cli_and_save(flags)             \
do {                                    \
	asm volatile("pushfl        \n      \
			popl %0         \n      \
			cli"                    \
			: "=r"(flags)           \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Set interrupt flag - enable interrupts on this processor */
#define sti()                           \
do {                                    \
	asm volatile("sti"                  \
			:                       \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Restore flags
 * Puts the value in "flags" into the EFLAGS register.  Most often used
 * after a cli_and_save_flags(flags) */
#define restore_flags(flags)            \
do {                                    \
	asm volatile("pushl %0      \n      \
			popfl"                  \
			:                       \
			: "r"(flags)            \
			: "memory", "cc"        \
			);                      \
} while(0)

#endif /* _LIB_H */
