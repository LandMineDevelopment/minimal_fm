#ifndef SCREEN_H
#define SCREEN_H
// Screen create_nav_screen(unsigned short height, unsigned short width);
// extern Screen nav_screen;
void set_current_dir(void);
unsigned short go_to_parent_dir(char *path);

void draw_x(int x_start, int x_stop, int y_start, int y_stop);
void fill_x(int x_start, int x_stop, int y_start, int y_stop);

void draw(int x_start, int x_stop, int y_start, int y_stop, void (*func)(int, int, int, int));
// unsigned short nav_keybind(char key, Screen *screen, unsigned short width, unsigned short height);
unsigned short nav_update(unsigned short width, unsigned short height);
unsigned short nav_keybind(char key);

#endif
