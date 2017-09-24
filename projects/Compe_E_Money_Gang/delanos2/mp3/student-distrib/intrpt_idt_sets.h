#ifndef _INTRPT_IDT_SETS_H
#define _INTRPT_IDT_SETS_H



#define IRQ1 0x21
#define IRQ8 0x28



extern void set_intrpt_gate_desc(int vector);

extern void init_rtc();

extern void init_keyboard();


#endif /* _INTRPT_IDT_SETS_H */

