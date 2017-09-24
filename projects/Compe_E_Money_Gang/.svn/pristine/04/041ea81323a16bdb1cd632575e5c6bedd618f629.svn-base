#ifndef _MOUSE_H
#define _MOUSE_H

#define MOUSE_PORT 0x60
#define IRQ12 0x2C
#define NUM_COLS 80
#define NUM_ROWS 25
#define DIRECT_VID 0x4000

//variable that determines whether the mouse is enabled or not
extern int enable_mouse;

//initializes the mouse
void init_mouse();

//reads packets from the mouse port and changes mouse info accordingly
void mouse_handler();

//waits until port (0x64) or the mouse port (0x60) is ready to be written to
void m_wait(int type);

//waits until port the mouse port (0x60) is ready to be read
void m_read();

//manages the cursor for the mouse
void mouse_cursor_helper();

//mouse coordinate variables
extern int old_mouse_x;
extern int old_mouse_y;
extern int mouse_x;
extern int mouse_y;
extern int mouse_other;
extern int normal_cursor;

#endif /* _MOUSE_H */


