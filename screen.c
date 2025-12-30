#include "defs.h"
#include "ansi.h"
#include "syscall.h"
#include "string.h"
#include "screen.h"
#include "dir.h"
Screen nav_screen = {
    .height = 0,
    .width = 0,
    .top_margin = 0,
    .bottom_margin = 0,
    .left_margin = 0,
    .right_margin = 0,
    .top_border = ' ',
    .bottom_border = ' ',
    .left_border = ' ',
    .right_border = ' ',

    //Grid
    .grid_cols = 3,
    .grid_rows = 1,
    .grid_dim_type = RELATIVE,
    .grid_cols_dims = {1, 1, 2},
    .grid_row_dims = {1},

    // .keybind = nav_keybinds,
    // .update = nav_update,
    //
    //Boxes
    .num_boxes = 3,
    .box_height = {0,0,0},
    .box_width = {0,0,0},
    // unsigned short box_width[MAX_BOXES_PER_SCREEN];
    // unsigned short box_top_margin[MAX_BOXES_PER_SCREEN];
    // unsigned short box_bottom_margin[MAX_BOXES_PER_SCREEN];
    // unsigned short box_left_margin[MAX_BOXES_PER_SCREEN];
    // unsigned short box_right_margin[MAX_BOXES_PER_SCREEN];
    // char box_top_border[MAX_BOXES_PER_SCREEN];
    // char box_bottom_border[MAX_BOXES_PER_SCREEN];
    // char box_left_border[MAX_BOXES_PER_SCREEN];
    // char box_right_border[MAX_BOXES_PER_SCREEN];
    // keybind_func box_keybind[MAX_BOXES_PER_SCREEN];
    // update_func box_update[MAX_BOXES_PER_SCREEN];
};

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
        // msleep(25);
    }
    for (int j = y_start; j < y_stop; j++){
        move_cursor(x_start,j);
        write_str("x", 1);
        move_cursor(x_stop,j);
        write_str("x", 1);
    }
}
void fill_x(int x_start, int x_stop, int y_start, int y_stop) {
    for (unsigned short row = y_start; row < y_stop; row++) {
        move_cursor(x_start, row);
        for (unsigned short col = x_start; col < x_stop; col++) {
            write_str("x", 1);
        }
    }
}
void fill_char(const char *c, unsigned short x_start, unsigned short x_stop, unsigned short y_start, unsigned short y_stop) {
    for (unsigned short row = y_start; row < y_stop; row++) {
        move_cursor(x_start, row);
        for (unsigned short col = x_start; col < x_stop; col++) {
            write_str(c,1);
        }
    }
}

void draw(int x_start, int x_stop, int y_start, int y_stop, void (*func)(int, int, int, int)) {
    func(x_start, x_stop, y_start, y_stop);
}

void nav_draw(Screen *screen_ptr) {
    //draw box 0
    draw_dir_listing((long)"..",
              screen_ptr->box_offset_x[0], screen_ptr->box_offset_x[0] + screen_ptr->box_width[0],
              screen_ptr->box_offset_y[0], screen_ptr->box_offset_y[0] + screen_ptr->box_height[0]);
    //draw box 1
    draw_dir_listing((long)".",
              screen_ptr->box_offset_x[1] + 1, screen_ptr->box_offset_x[1] + screen_ptr->box_width[1],
              screen_ptr->box_offset_y[1], screen_ptr->box_offset_y[1] + screen_ptr->box_height[1]);
}

// unsigned short nav_update(void *screen_ptr, unsigned short width, unsigned short height){
unsigned short nav_update(unsigned short width, unsigned short height){
    clear_screen();
    // Screen *screen = (Screen *)screen_ptr;
    Screen *screen = &nav_screen;
    screen->width = width;
    screen->height = height;

    screen->box_offset_x[0] = 0;
    screen->box_offset_x[1] = width/3;
    screen->box_offset_x[2] = width*2/3;

    screen->box_offset_y[0] = 1;
    screen->box_offset_y[1] = 1;
    screen->box_offset_y[2] = 1;

    screen->box_width[0] = width/3;
    screen->box_width[1] = width/3;
    screen->box_width[2] = width/3;

    screen->box_height[0] = height;
    screen->box_height[1] = height;
    screen->box_height[2] = height;

    nav_draw(screen);
}

unsigned short nav_keybind(char key, unsigned short width, unsigned short height){
    if (key == 'q') return ACTION_EXIT;
    else if (key == 'r') {
        nav_update(width, height);
        return ACTION_REFRESH;
    }
    return ACTION_NOTHING;
}


