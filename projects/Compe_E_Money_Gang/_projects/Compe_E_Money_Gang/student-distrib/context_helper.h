#ifndef _CONTENT_HELPER_H
#define _CONTENT_HELPER_H


// injects artificial iret context onto appropriate stack and irets into user space
extern void execute_IRET(uint32_t eip, uint32_t cs, uint32_t esp, uint32_t ss);

#endif /* _CONTENT_HELPER_H */
