#include "defs.h"
#include "syscall.h"
#include "string.h"
#include "screen.h"

unsigned short nav_keybinds(char key){
    if (key == 'q') return ACTION_EXIT;
    return ACTION_NOTHING;
}

// Screen create_nav_screen(unsigned short height, unsigned short width) {
    Screen nav_screen = {

        // .height = height,
        // .width = width,
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

        .keybind = nav_keybinds,
        // update_func update;
        //
        // //Boxes
        // unsigned short box_height[MAX_BOXES_PER_SCREEN];
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
// }
