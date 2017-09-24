#define ASM     1

.text


.globl context_wrapper
.globl context_pop
.globl keyboard_wrapper
.globl rtc_wrapper
.globl mouse_wrapper
.globl null_wrapper
.globl divideErrorWrapper
.globl nmiIntWrapper
.globl breakPointWrapper
.globl overflowWrapper
.globl rangeExceedWrapper
.globl invalidOpWrapper
.globl deviceNotAvailWrapper
.globl doubleFaultWrapper
.globl invalidTSSWrapper
.globl segNotPresentWrapper
.globl stackSegFaultWrapper
.globl genProtectionWrapper
.globl pageFaultWrapper
.globl floatErrorWrapper
.globl alignCheckWrapper
.globl machineCheckWrapper
.globl floatSIMDWrapper

.align 4


context_wrapper:
	pusha						#push all registers onto stack  	used to store current context
	call context_switch_handler		#call keyboard handler
context_pop:
	popa						#pop all registers			used to restore next context
	iret						#interrupt return			used to ret to next context

keyboard_wrapper:
	pusha						#push all registers onto stack
	call keyboard_handler		#call keyboard handler
	popa						#pop all registers
	iret						#interrupt return
	

rtc_wrapper:
	pusha						#push all registers onto stack
	call rtc_handler			#call rtc handler
	popa						#pop all registers
	iret						#interrupt return
	
mouse_wrapper:
	pusha						#push all registers onto stack
	call mouse_handler			#call mouse handler
	popa						#pop all registers
	iret						#interrupt return

#exception wrappers

divideErrorWrapper:
	pusha						#push all registers onto stack
	call divideError			#call divideError
	popa						#pop all registers
	iret						#interrupt return

nmiIntWrapper:
	pusha						#push all registers onto stack
	call nmiInt					#call nmiInt
	popa						#pop all registers
	iret						#interrupt return

breakPointWrapper:
	pusha						#push all registers onto stack
	call breakPoint				#call breakPoint
	popa						#pop all registers
	iret						#interrupt return

overflowWrapper:
	pusha						#push all registers onto stack
	call overflow				#call overflow
	popa						#pop all registers
	iret						#interrupt return

rangeExceedWrapper:
	pusha						#push all registers onto stack
	call rangeExceed			#call rangeExceed
	popa						#pop all registers
	iret						#interrupt return

invalidOpWrapper:
	pusha						#push all registers onto stack
	call invalidOp				#call invalidOp
	popa						#pop all registers
	iret						#interrupt return

deviceNotAvailWrapper:
	pusha						#push all registers onto stack
	call deviceNotAvail			#call deviceNotAvail
	popa						#pop all registers
	iret						#interrupt return

doubleFaultWrapper:
	pusha						#push all registers onto stack
	call doubleFault			#call doubleFault
	popa						#pop all registers
	iret						#interrupt return

invalidTSSWrapper:
	pusha						#push all registers onto stack
	call rtc_handler			#call rtc handler
	popa						#pop all registers
	iret						#interrupt return

segNotPresentWrapper:
	pusha						#push all registers onto stack
	call segNotPresent			#call segNotPresent
	popa						#pop all registers
	iret						#interrupt return

stackSegFaultWrapper:
	pusha						#push all registers onto stack
	call stackSegFault			#call stackSegFault
	popa						#pop all registers
	iret						#interrupt return

genProtectionWrapper:
	pusha						#push all registers onto stack
	call genProtection			#call genProtection
	popa						#pop all registers
	iret						#interrupt return

pageFaultWrapper:
	pusha						#push all registers onto stack
	call pageFault				#call pageFault
	popa						#pop all registers
	iret						#interrupt return

floatErrorWrapper:
	pusha						#push all registers onto stack
	call floatError				#call floatError
	popa						#pop all registers
	iret						#interrupt return

alignCheckWrapper:
	pusha						#push all registers onto stack
	call alignCheck				#call alignCheck
	popa						#pop all registers
	iret						#interrupt return

machineCheckWrapper:
	pusha						#push all registers onto stack
	call machineCheck			#call machineCheck
	popa						#pop all registers
	iret						#interrupt return

floatSIMDWrapper:
	pusha						#push all registers onto stack
	call floatSIMD				#call floatSIMD
	popa						#pop all registers
	iret						#interrupt return
