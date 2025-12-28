#include "defs.h"
#include "syscall.h"
#include "string.h"
#include "ansi.h"
// #include "draw.h"
//
struct timespec {
    long tv_sec;   // seconds
    long tv_nsec;  // nanoseconds
};
// Sleep for given milliseconds
void msleep(unsigned long milliseconds)
{
    struct timespec ts;
    ts.tv_sec  = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;  // convert ms → ns

    syscall2(SYS_nanosleep, (long)&ts, 0);
}

void draw_x(int x_start, int x_stop, int y_start, int y_stop) {
    move_cursor(1,y_start);
    for (int i = x_start; i < x_stop; i++){
        move_cursor(i,y_start);
        write_str("x", 1);
        move_cursor(i,y_stop);
        write_str("x", 1);
        msleep(25);
    }
    for (int j = y_start; j < y_stop; j++){
        move_cursor(x_start,j);
        write_str("x", 1);
        move_cursor(x_stop,j);
        write_str("x", 1);
    }
}

void draw(int x_start, int x_stop, int y_start, int y_stop, void (*func)(int, int, int, int)) {
    func(x_start, x_stop, y_start, y_stop);
}
