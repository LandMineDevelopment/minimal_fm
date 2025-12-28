#include "defs.h"
#include "syscall.h"
#include "string.h"
#include "init.h"

#define MAIN_SCREEN_BOX_COL 3
#define MAIN_SCREEN_BOX_ROW 1

Box init_box(unsigned short id, 
             unsigned short height, unsigned short width, 
             unsigned short x_offset, unsigned short y_offset, 
             unsigned short margin = 0, char top_bottom_border = ' ', char top_bottom_border = ' ') {
    Box box= {0};
    box.id = id;
    box.x_offset = x_offset;
    box.y_offset = y_offset;
    box.height = height;
    box.width = width;
    box.top_margin = margin;
    box.bottom_margin = margin;
    left_margin = margin;
    right_margin = margin;
    char top_bottom_border = top_bottom_border;
    char left_right_border = left_right_border;

    return box
}

Screen init_screen(unsigned short num_cols, unsigned short num_rows) {
    Screen screen = {0};
    screen.num_cols = num_cols;
    screen.num_rows = num_rows;

    return screen;
}

void calc_box_sizes((Screen)&screen) {
    for (int c = 0; c < screen.num_cols; c++) {
        for (int r = 0; c < screen.num_rows; r++) {
            height
        }
    }
}

AppState init_app(void) {
    AppState app_state = {0};

    app_state.preview_mode = PREVIEW_NONE;
    app_state.show_hidden = 0;
    app_state.editor_cmd = 0;
    app_state.num_clipboard = 0;
    app_state.is_cut = 0;
    app_state.in_modal_input = 0;
    app_state.modal_context = CTX_NORMAL;
    app_state.input_len = 0;
    app_state.delete_multi = 0;
    app_state.delete_recursive = 0;

    return app_state
}
