#define ASM     1

.text

.globl keyboard_wrapper
.globl rtc_wrapper

.align 4

keyboard_wrapper:
	pusha
	call keyboard_handler
	popa
	iret
	
.align 4
rtc_wrapper:
	pusha
	call rtc_handler
	popa
	iret