#Code used from OSDev: http://wiki.osdev.org/Setting_Up_Paging

#define ASM     1

.text
.globl loadPageDirectory
.globl enablePaging

#Use the page_directory address that was passed in and send it to the CR3
loadPageDirectory:
push %ebp
mov %esp, %ebp
mov 8(%esp), %eax
mov %eax, %cr3
mov %ebp, %esp
pop %ebp
ret

#Set the 32nd bit in the CR0 register to enable paging
enablePaging:
push %ebp
mov %esp, %ebp
mov %cr0, %eax
or $0x80000000, %eax
mov %eax, %cr0
mov %ebp, %esp
pop %ebp
ret