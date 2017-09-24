#define ASM     1

.data

	# Number of system calls
	NUM_SYS_CALLS = 10

.text

.globl systemCallHandler
/*
 * systemCallHandler
 *   DESCRIPTION: handles system call by going through jump table and then
 *				  returning control flow to user program
 *   INPUTS: none
 *   OUTPUTS: executes the system call
 *   RETURN VALUE: success/failure or the number of bytes read in the case of read
 *   SIDE EFFECTS: none
 */ 
 
#does not disable irq 1 any more but we shouldnt need it since interups are
#disables when a system call is made
/*
this file will contain the jump table so that when the int 0x80 instruction
is called the idt directs the call to this code

overview
inline assembly
*fist go into eax and find the syst call number from appendix B.
*should make a jump table in inline assembly and jump to approprate function
*be sure to save and store all registers
*need to push paramertsers on stack in the c calling convintion for all functions
*need to use iret
*acess PCB through bit masking the ESP

step by step 
make jump table and jump to coresponding syscall handler
each handler will call the correct function with the correct parameters.
ex read_handler will call the correct terminal/file/rtc read with the correct parameters


do open and open file based on the type of file it is. Make this in an if else format

for all other system calls make the approprate call from the file operations table 
pointer
*/


.align 4
	
systemCallHandler:
	cmp $NUM_SYS_CALLS, %eax	#check to make sure syscall is within bounds
	jbe zerocheck
	movl $-1, %eax	#return -1
	iret

zerocheck: 
	cmp $0, %eax	#check to make sure syscall is within bounds
	jne start
	movl $-1, %eax	#return -1
	iret

start:
	pusha						#push all registers onto stack
								#push all three paramters on the stack for the calling convention
	pushl %edx
	pushl %ecx
	pushl %ebx
	
	call *functions(,%eax,4) 	#jump to appropriate function 
	
	movl %eax, (store_return)		#save the value you want to return
	popl %ebx
	popl %ecx
	popl %edx
	popa						#pop all registers
	movl (store_return),%eax	#restore the return value
	iret
	
store_return:			
	.long 0				#memory used to store the return value
functions:
	.long 0, halt_handler, execute_handler, read_handler, write_handler, open_handler, close_handler,getargs_handler, vidmap_handler, set_handler_handler, sigreturn_handler


