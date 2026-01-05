#include "ansi.h"
#include "syscall.h"

// Helper to write raw ANSI sequences
static void write_ansi(const char *seq, long len) {
    write_str(seq, len);
}

// Clear entire screen and move cursor home
void clear_screen(void) {
    write_ansi("\x1b[2J\x1b[H", 7);
}

// Clear from cursor to end of current line
void clear_line(void) {
    write_ansi("\x1b[K", 3);
}

// Clear from cursor to end of screen
void clear_to_screen_end(void) {
    write_ansi("\x1b[0J", 4);
}


// Move cursor to absolute position (row/col are 1-based)
void move_cursor(unsigned short col, unsigned short row) {
    char buf[32];
    long len = 0;

    buf[len++] = '\x1b';
    buf[len++] = '[';

    // row
    // if (row >= 100) { buf[len++] = '0' + (row / 100); row %= 100; }
    // if (row >= 10)  { buf[len++] = '0' + (row / 10); }
    if (row >= 100) { buf[len++] = '0' + (row / 100); }
    if (row >= 10)  { buf[len++] = '0' + ( (row / 10) % 10 ); }
    buf[len++] = '0' + (row % 10);

    buf[len++] = ';';

    // col
    // if (col >= 100) { buf[len++] = '0' + (col / 100); col %= 100; }
    // if (col >= 10)  { buf[len++] = '0' + (col / 10); }
    if (col >= 100) { buf[len++] = '0' + (col / 100); }
    if (col >= 10)  { buf[len++] = '0' + ( (col / 10) % 10 ); }
    buf[len++] = '0' + (col % 10);

    buf[len++] = 'H';

    write_ansi(buf, len);
}
// Clear retangular section
void clear_section(unsigned short x_start,unsigned short x_stop,unsigned short y_start,unsigned short y_stop) {
    // write_str("x", 1);
    // if (x_start >= x_stop) write_str("n",1);
    for (unsigned short row = y_start; row <= y_stop; ++row) {
        // write_str("y", 1);
        move_cursor(x_start, row);
        for (unsigned short col = x_start; col <= x_stop; ++col) {
            // write_str("z", 1);
            write_str(" ", 1);
        }
    }
}

// void move_cursor(unsigned short col, unsigned short row) {
//     char buf[32];
//     char col_buf[32];
//     char row_buf[32];
//     long len = 0;
//
//     buf[len++] = '\x1b';
//     buf[len++] = '[';
//
//     //row
//     while(row > 0){
//         row_buf[len++] = row % 10 + '0';
//         row /= 10;
//     }
//
//     buf[len++] = ';';
//
//     //col
//     while(col > 0){
//         buf[len++] = col % 10 + '0';
//         col /= 10;
//     }
//     // Null-terminate the string
//     str[i] = '\0';
//
//     // Reverse the string to get the correct order
//     for (int j = 0, k = i - 1; j < k; j++, k--) {
//         char temp = str[j];
//         str[j] = str[k];
//         str[k] = temp;
//     }
//
//     buf[len++] = 'H';
//     write_ansi(buf, len);
// }

// Relative cursor movement
void cursor_up(unsigned short n) {
    if (n == 0) return;
    char buf[16] = "\x1b[";
    long len = 2;
    if (n >= 10) { buf[len++] = '0' + (n / 10); }
    buf[len++] = '0' + (n % 10);
    buf[len++] = 'A';
    write_ansi(buf, len);
}

void cursor_down(unsigned short n) {
    if (n == 0) return;
    char buf[16] = "\x1b[";
    long len = 2;
    if (n >= 10) { buf[len++] = '0' + (n / 10); }
    buf[len++] = '0' + (n % 10);
    buf[len++] = 'B';
    write_ansi(buf, len);
}

void cursor_right(unsigned short n) {
    if (n == 0) return;
    char buf[16] = "\x1b[";
    long len = 2;
    if (n >= 10) { buf[len++] = '0' + (n / 10); }
    buf[len++] = '0' + (n % 10);
    buf[len++] = 'C';
    write_ansi(buf, len);
}

void cursor_left(unsigned short n) {
    if (n == 0) return;
    char buf[16] = "\x1b[";
    long len = 2;
    if (n >= 10) { buf[len++] = '0' + (n / 10); }
    buf[len++] = '0' + (n % 10);
    buf[len++] = 'D';
    write_ansi(buf, len);
}

// Cursor visibility
void hide_cursor(void)   { write_ansi("\x1b[?25l", 6); }
void show_cursor(void)   { write_ansi("\x1b[?25h", 6); }

// Save/restore cursor position
void save_cursor(void)    { write_ansi("\x1b[s", 3); }
void restore_cursor(void) { write_ansi("\x1b[u", 3); }

// Text attributes
void reset_attributes(void) { write_ansi("\x1b[m", 3); }
void set_bold(void)         { write_ansi("\x1b[1m", 4); }
void set_reverse(void)      { write_ansi("\x1b[7m", 4); }

// Standard foreground colors (30-37)
void set_fg_color(int color) {
    char seq[5] = "\x1b[3";
    seq[2] = '0' + (color & 7);
    seq[3] = 'm';
    write_ansi(seq, 4);
}

// Standard background colors (40-47)
void set_bg_color(int color) {
    char seq[5] = "\x1b[4";
    seq[2] = '0' + (color & 7);
    seq[3] = 'm';
    write_ansi(seq, 4);
}
