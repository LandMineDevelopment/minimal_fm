#ifndef KEYBIND_H
#define KEYBIND_H

#define ACTION_NONE     0
#define ACTION_REDRAW   1
#define ACTION_REFRESH  2
#define ACTION_EXIT     3

// Action function now takes the input char
typedef int (*ActionFunc)(char c);

// Main entry point
int handle_key(char c);

// Unified dispatcher
int handle_input(unsigned char context, char c);

#endif
