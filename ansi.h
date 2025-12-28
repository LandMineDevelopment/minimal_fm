#ifndef ANSI_H
#define ANSI_H

// #include "defs.h"
// #include "syscall.h"

// Clear
void clear_screen(void);
void clear_line(void);              // from cursor to end of line
void clear_to_screen_end(void);

// Cursor movement
void move_cursor(unsigned short col, unsigned short row);  // 1-based
void cursor_up(unsigned short n);
void cursor_down(unsigned short n);
void cursor_right(unsigned short n);
void cursor_left(unsigned short n);

// Cursor visibility
void hide_cursor(void);
void show_cursor(void);

// Cursor save/restore
void save_cursor(void);
void restore_cursor(void);

// Text attributes
void reset_attributes(void);
void set_bold(void);
void set_reverse(void);             // highlighted / selected
void set_fg_color(int color);       // 0-7 standard colors
void set_bg_color(int color);

#endif
