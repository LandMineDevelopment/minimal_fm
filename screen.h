#ifndef SCREEN_H
#define SCREEN_H
// Screen create_nav_screen(unsigned short height, unsigned short width);
extern Screen nav_screen;

void draw_x(int x_start, int x_stop, int y_start, int y_stop);
void fill_x(int x_start, int x_stop, int y_start, int y_stop);

void draw(int x_start, int x_stop, int y_start, int y_stop, void (*func)(int, int, int, int));

#endif
