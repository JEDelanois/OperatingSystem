#ifndef _IDT_H
#define _IDT_H

// put exceptions into IDT
void exceptionEntries();

// put interrupts into IDT
void interruptEntries();

// put system calls into IDT
void systemCallEntry();

// initialize IDT
extern void initIDT();

//Code to initialize IDT vector
extern void set_intrpt_gate_desc(int vector);

//Initialize the PIT for scheduling
extern void init_PIT();

#endif /* _IDT_H */
