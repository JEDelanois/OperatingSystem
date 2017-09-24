#ifndef _CONTEXT_SWITCH_H
#define _CONTEXT_SWITCH_H


#define NUM_PID 50 //Should be 32,768 if we weren't concerned with using up kernel memory
#define IRQ0 0x20	//idt entry value
#define VID_MEM_NO_FLAGS	0xFFFFF000		//masks out flags when and keeps video memory address when changing video memory pointers
#define FLAGS_WITHOUT_VID_MEM 0x00000FFF	//masks out video memory address to keep flags when changing video memory pointers

//x's, y's, no_new_line's, and caps_mode's for each shell
extern int pid_array[NUM_PID];
extern int x_of_term[3];
extern int y_of_term[3];
extern int no_new_line[3];

//colors for each shell
extern char red_text[3];
extern char blue_text[3];
extern char green_text[3];
extern char red_back[3];
extern char blue_back[3];
extern char green_back[3];

/* Checks if the given pid is currently in use. */
extern int check_pid(int pid);

/* Checks out the lowest unused pid. */
extern int new_pid_checkout();

/* Returns the lowest unused pid. */
extern int get_new_pid();

/* Marks the given pid as "not in use". */
extern int remove_pid(int pid);

/* Switches video memory and process data for scheduling and terminal switches */
void switch_shell(int old_shell);

/* updates xcreen_x and screen_y coordinates for context switching */
void context_paging();

/* called by idt when irq 0 is interupted to perform the actual context switch */
void context_switch_handler();

//sets up the idt to hold context_switch_handler as the interupt handler for IRQ0
void init_idt_context();

/* handles color changes for terminal switches */
void RGB_switch_helper(int old_shell);


#endif /* _CONTEXT_SWITCH_H */
