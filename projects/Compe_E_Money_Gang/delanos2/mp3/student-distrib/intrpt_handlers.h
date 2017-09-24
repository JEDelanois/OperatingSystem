#ifndef _INTRPT_HANDLERS_H
#define _INTRPT_HANDLERS_H

#define IRQ1 0x21
#define IRQ8 0x28
#define key_board_port 0x60



extern void keyboard_handler();

extern void rtc_handler();


#endif /* _INTRPT_HANDLERS_H */

