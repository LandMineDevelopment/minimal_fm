#ifndef DRAW_H
#define DRAW_H

void draw_x(int x_start, int x_stop, int y_start, int y_stop);

void draw(int x_start, int x_stop, int y_start, int y_stop, void (*func)(int, int, int, int));

#endif
