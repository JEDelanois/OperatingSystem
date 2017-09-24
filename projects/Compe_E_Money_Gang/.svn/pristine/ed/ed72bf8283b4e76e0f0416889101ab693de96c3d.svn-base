#include "system_call_handler.h"
#include "exception_handlers.h"
#include "lib.h"
#include "i8259.h"

/*
 * errorScreen
 *   DESCRIPTION: clears screen, prints exception, and halts system
 *   INPUTS: int index - determines which error to print to screen
 *   OUTPUTS: prints exception info onto screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: spins infinitely, effectively halting the program
 */   
void errorScreen(int index)
{
	// address where page fault occured
	int* wrong_address = (int*) 1;
	// clear the screen
	clear();
	switch (index)
	{
		case 0:
			printf("Divide By Zero");
			break;
		case 2:
			printf("Nonmaskable Interrupt");
			break;
		case 3:
			printf("INT 3: Breakpoint");
			break;
		case 4:
			printf("INTO: Overflow");
			break;
		case 5:
			printf("BOUND: Range Exceed");
			break;
		case 6:
			printf("UD2: Invalid Opcode");
			break;
		case 7:
			printf("Device Not Available");
			break;
		case 8:
			printf("Double Fault");
			break;
		case 10:
			printf("Invalid TSS");
			break;
		case 11:
			printf("Segment Not Present");
			break;
		case 12:
			printf("Stack Segment Fault");
			break;
		case 13:
			printf("General Protection");
			break;
		case 14:
			printf("Page Fault\n");
			asm volatile("movl %%cr2, %0\n"
		 		: "=r" (wrong_address)
		 		: 
		 		: "%eax", "memory"
		 		);
			printf("Attempted to access: %d", (int) wrong_address);
			break;
		case 16:
			printf("Floating Point Error");
			break;
		case 17:
			printf("Alignment Check");
			break;
		case 18:
			printf("Machine Check");
			break;
		case 19:
			printf("SIMD Floating Point Exception");
			break;
		default:
			printf("None of the Above %d", index);
			break;
	}
	// spin infinitely
	while (1);
}


/*
 * divideError
 *   DESCRIPTION: handles divide by zero exception
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void divideError ()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(0);
}

/*
 * nmiInt
 *   DESCRIPTION: handles non-maskable interrupt exception
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void nmiInt ()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(2);
}

/*
 * breakPoint
 *   DESCRIPTION: handles break point exception
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void breakPoint ()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(3);
}

/*
 * overflow
 *   DESCRIPTION: handles overflow exception
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void overflow ()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(4);
}

/*
 * rangeExceed
 *   DESCRIPTION: handles range exceed exception
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void rangeExceed()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(5);
}

/*
 * invalidOp
 *   DESCRIPTION: handles invalid Opcode exception
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void invalidOp()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(6);
}

/*
 * deviceNotAvail
 *   DESCRIPTION: handles device not available exception
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void deviceNotAvail()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(7);
}

/*
 * doubleFault
 *   DESCRIPTION: handles double fault exception
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void doubleFault()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(8);
}

/*
 * invalidTSS
 *   DESCRIPTION: handles invalid TSS exception
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void invalidTSS()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(10);
}

/*
 * segNotPresent
 *   DESCRIPTION: handles segment not present exception
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void segNotPresent()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(11);
}

/*
 * stackSegFault
 *   DESCRIPTION: handles stack seg fault exception
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void stackSegFault()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(12);
}

/*
 * genProtection
 *   DESCRIPTION: handles general protection exceptions
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void genProtection()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(13);
}

/*
 * pageFault
 *   DESCRIPTION: handles page fault exceptions
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void pageFault()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(14);
}

/*
 * floatError
 *   DESCRIPTION: handles float error exceptions
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void floatError()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(16);
}

/*
 * alignCheck
 *   DESCRIPTION: handles align check exceptions
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void alignCheck()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(17);
}

/*
 * machineCheck
 *   DESCRIPTION: handles machine check exceptions
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void machineCheck()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(18);
}

/*
 * floatSIMD
 *   DESCRIPTION: handles simd floating point exceptions
 *   INPUTS: none
 *   OUTPUTS: prints info to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */ 
void floatSIMD()
{
	// disable interrupts from occuring and go to errorScreen
	cli();
	errorScreen(19);
}
