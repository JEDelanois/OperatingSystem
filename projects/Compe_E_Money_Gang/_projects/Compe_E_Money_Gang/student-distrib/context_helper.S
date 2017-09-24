#define ASM     1

.data

	# Interrupts turned on flag in EFLAGS register
	INT_FLAG = 0x200

.text

.globl execute_IRET, halt_RET,process_prep

.align 4

# void execute_IRET(uint32_t eip, uint32_t cs, uint32_t esp, uint32_t ss);
#
# Description: Injects artificial iret context onto appropriate stack and irets into user space
#    Inputs: eax = eip
#			 ebx = cs
# 			 ecx = esp/eflags modifier
# 			 edx = ss
#   Outputs: irets into user space
# Registers: Clobbers EAX, EBX, ECX, EDX


execute_IRET:
	cli
	pushl	%ebp
	movl	%esp, %ebp

	movl	8(%ebp),%eax	#get eip
	movl	12(%ebp),%ebx	#get cs
	movl	16(%ebp),%ecx	#get esp
	movl	20(%ebp),%edx	#get ss

	pushl	%edx	#push ss
	pushl	%ecx	#push esp

	# turn on interrupt flag
	pushf
	popl	%ecx	#get flags
	orl		$INT_FLAG, %ecx	#modify flags
	pushl	%ecx	#push flags

	pushl	%ebx	#push cs
	pushl	%eax	#push eip
	
	# change data segment
	movw 	%dx, %ds
	iret

halt_RET:
	leave
	ret

