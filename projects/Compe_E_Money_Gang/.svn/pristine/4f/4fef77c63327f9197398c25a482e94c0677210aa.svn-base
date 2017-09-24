#ifndef _RTC_H
#define _RTC_H

#include "dentry.h"
#include "file_descriptor.h"
#include "process_data.h"

//Magic Number Macros
#define IRQ1 0x21
#define IRQ8 0x28
#define key_board_port 0x60
#define RTC_PORT_0 0x70
#define RTC_PORT_1 0x71
#define RTC_REG_0 0x8B
#define RTC_REG_1 0x8A



//Array that holds the counter values set in rtc_write and used in rtc_read
extern volatile int process_counters[6];


//Initializes RTC
extern void init_rtc();

//Handler for the RTC Interrupt
extern void rtc_handler();

//Handler for the Read System Call (RTC)
extern int32_t rtc_read(int32_t fd,int8_t* buff, uint32_t offset, int32_t nbytes);

//Handler for the Write System Call (RTC)
extern int32_t rtc_write(const void* buf, int32_t nbytes);

//Handler for the Open System Call (RTC)
extern int32_t rtc_open(file_entry_t * file_desc_entry, dentry_t * dentry);

//Handler for the Close System Call
extern int32_t rtc_close(int32_t fd);

#endif /* _RTC_H */


