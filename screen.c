#include "defs.h"
#include "ansi.h"
#include "syscall.h"
#include "string.h"
#include "screen.h"
#include "dir.h"


char cwd[MAX_PATH];
unsigned short cwd_path_len;
char parent_dir[MAX_PATH];
unsigned short parent_path_len;
char child_obj[MAX_PATH];
unsigned short child_path_len;
char current_dir_items[MAX_DIRENT_BUF];
int current_item = 0;
int current_item_type = 0;
int num_dir_items = 0;
int size_of_dir_items;
int show_hidden = 0;

Cursor nav_cursor = {0};

void set_child_item_path(void){
    struct linux_dirent64 *d = 0;
    int pos = 0;
    for (int count = 0; count <= current_item; count++){
        d = (struct linux_dirent64 *)(current_dir_items + pos);

        if (d->d_name[0] == '.' && (d->d_name[1] == '\0' ||
        (d->d_name[1] == '.' && d->d_name[2] == '\0'))) {
                pos += d->d_reclen;
                count--;
                continue;
        }
        if (!show_hidden && d->d_name[0] == '.') {
                pos += d->d_reclen;
                count--;
                continue;
        }

        pos += d->d_reclen;
    }

    path_build(child_obj, cwd, d->d_name);

    current_item_type = d->d_type;
}

void set_dir_items(void){
    num_dir_items = 0;
    current_item = 0;
    size_of_dir_items = (unsigned short)list_dir_entries((long)cwd, current_dir_items, sizeof(current_dir_items));
    int pos = 0;
    while (pos < size_of_dir_items) {
        struct linux_dirent64 *d = (struct linux_dirent64 *)(current_dir_items + pos);

        if (d->d_name[0] == '.' && (d->d_name[1] == '\0' ||
        (d->d_name[1] == '.' && d->d_name[2] == '\0'))) {
                pos += d->d_reclen;
                continue;
        }
        if (!show_hidden && d->d_name[0] == '.') {
                pos += d->d_reclen;
                continue;
        }

        num_dir_items++;
        pos += d->d_reclen;
    }

    if (num_dir_items > 0) set_child_item_path();
}

void set_current_dir(void){
    cwd_path_len = syscall2(SYS_GETCWD, (long)&cwd, sizeof(cwd));
    syscall2(SYS_GETCWD, (long)&parent_dir, sizeof(parent_dir));
    syscall2(SYS_GETCWD, (long)&child_obj, sizeof(child_obj));
    go_to_parent_dir(parent_dir);
    parent_path_len = my_strlen(parent_dir);
    set_dir_items();
}


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

unsigned short draw_dir_listing(long path, unsigned short x_start, unsigned short x_stop,
                      unsigned short y_start, unsigned short y_stop)
{
    char buf[MAX_DIRENT_BUF];
    int nread = list_dir_entries((long)path, buf, sizeof(buf));
    if (nread <= 0) {
        move_cursor(x_start, y_start);
        write_str("[NO FILES]", 10);
        return 0;
    }

    save_cursor();
    unsigned short curr_row = y_start;
    unsigned short curr_col = x_start;
    int pos = 0;
    while (pos < nread && curr_row <= y_stop) {
        move_cursor(x_start, curr_row);

        struct linux_dirent64 *d = (struct linux_dirent64 *)(buf + pos);
        if (d->d_name[0] == '.' && (d->d_name[1] == '\0' ||
           (d->d_name[1] == '.' && d->d_name[2] == '\0'))) {
            pos += d->d_reclen;
            continue;
        }
        if (!show_hidden && d->d_name[0] == '.') {
            pos += d->d_reclen;
            continue;
        }


        unsigned short len_write = 0;
        if (my_strlen(d->d_name) > x_stop - x_start) len_write = x_stop - x_start;
        else len_write = my_strlen(d->d_name);
        write_str(d->d_name, len_write);

        if (d->d_type == DT_DIR) write_str("/", 1);

        pos += d->d_reclen;
        curr_row += 1;
    }

    restore_cursor();
    return curr_row;
}

void draw_parent_box() {
    draw_dir_listing((long)parent_dir,
              nav_screen.box_offset_x[0], nav_screen.box_offset_x[0] + nav_screen.box_width[0],
              nav_screen.box_offset_y[0], nav_screen.box_offset_y[0] + nav_screen.box_height[0]);
}

void draw_current_box() {
    nav_cursor.cursor_max_y = draw_dir_listing((long)cwd,
              nav_screen.box_offset_x[1] + 1, nav_screen.box_offset_x[1] + nav_screen.box_width[1],
              nav_screen.box_offset_y[1], nav_screen.box_offset_y[1] + nav_screen.box_height[1]);
    move_cursor(nav_screen.box_offset_x[1], nav_cursor.cursor_y);
    write_str(">",1);
}

void draw_child_box() {
    clear_section( nav_screen.box_offset_x[2], nav_screen.box_offset_x[2] + nav_screen.box_width[2], nav_screen.box_offset_y[2], nav_screen.box_offset_y[2] + nav_screen.box_height[2]);
    if (current_item_type == DT_DIR){
        draw_dir_listing((long)child_obj,
                nav_screen.box_offset_x[2], nav_screen.box_offset_x[2] + nav_screen.box_width[2],
                nav_screen.box_offset_y[2], nav_screen.box_offset_y[2] + nav_screen.box_height[2]);
    }
    else if (current_item_type == DT_REG) {
        long fd = syscall2(SYS_OPEN, (long)child_obj, O_RDONLY);
        if (fd < 0) {
            move_cursor( nav_screen.box_offset_x[2], nav_screen.box_offset_y[2]);
            write_str("Cannot open", 11);
            return;
        }

        char line_buf[nav_screen.box_width[2] * nav_screen.box_height[2]];
        for (int i = 0; i < nav_screen.box_width[2] * nav_screen.box_height[2]; i++) {
            line_buf[i] = ' ';
        }
        int line_count = 0;
        unsigned short pos = 0;
        char is_text = 1;
        int curr_buff_ind = 0;

        move_cursor( nav_screen.box_offset_x[2], nav_screen.box_offset_y[2] + line_count );

        while (line_count < nav_screen.box_height[2] ) {
            long ret = syscall3(SYS_READ, fd, (long)line_buf, sizeof(line_buf) - 1);
            if (ret <= 0) break;
            while (line_buf[curr_buff_ind] && line_count < nav_screen.box_height[2] ) {
                char c = line_buf[curr_buff_ind];
                if (pos > nav_screen.box_width[2]) {
                    while (line_buf[curr_buff_ind] && c != '\n') {
                        curr_buff_ind++;
                        c = line_buf[curr_buff_ind];
                    }
                }
                if (c == '\n') {
                    pos = 0;
                    line_count++;
                    move_cursor( nav_screen.box_offset_x[2], nav_screen.box_offset_y[2] + line_count );
                } else if (
                    ((unsigned char)c > 127 && c != '\t' && c != '\n' && c != '\r')) {
                    is_text = 0;
                    goto binary;
                } else if (c == '\t') {
                    write_str("    ", 4);
                    pos += 4;
                } else {
                    write_str(&c, 1);
                    pos++;
                }
                curr_buff_ind++;
            }
        }
        if (!is_text){
            binary:
            move_cursor( nav_screen.box_offset_x[2], nav_screen.box_offset_y[2]);
            write_str("[Binary file]", 13);
        }

        syscall1(SYS_CLOSE, fd);
    }

}

void nav_draw() {
    //draw parent box
    draw_dir_listing((long)parent_dir,
              nav_screen.box_offset_x[0], nav_screen.box_offset_x[0] + nav_screen.box_width[0],
              nav_screen.box_offset_y[0], nav_screen.box_offset_y[0] + nav_screen.box_height[0]);
    //draw current box
    nav_cursor.cursor_max_y = draw_dir_listing((long)cwd,
              nav_screen.box_offset_x[1] + 1, nav_screen.box_offset_x[1] + nav_screen.box_width[1],
              nav_screen.box_offset_y[1], nav_screen.box_offset_y[1] + nav_screen.box_height[1]);
    move_cursor(nav_screen.box_offset_x[1], nav_cursor.cursor_y);
    write_str(">",1);
    //draw child box
    if (current_item_type == DT_DIR){
        draw_dir_listing((long)child_obj,
                nav_screen.box_offset_x[2], nav_screen.box_offset_x[2] + nav_screen.box_width[2],
                nav_screen.box_offset_y[2], nav_screen.box_offset_y[2] + nav_screen.box_height[2]);
    }
}

// unsigned short nav_update(void *screen_ptr, unsigned short width, unsigned short height){
unsigned short nav_update(unsigned short width, unsigned short height){
    clear_screen();
    nav_screen.width = width;
    nav_screen.height = height;

    nav_screen.box_offset_x[0] = 0;
    // nav_screen.box_offset_x[1] = width/3;
    // nav_screen.box_offset_x[2] = width*2/3;
    nav_screen.box_offset_x[1] = width/4;
    nav_screen.box_offset_x[2] = width*1/2;

    nav_screen.box_offset_y[0] = 1;
    nav_screen.box_offset_y[1] = 1;
    nav_screen.box_offset_y[2] = 1;

    // nav_screen.box_width[0] = width/3;
    // nav_screen.box_width[1] = width/3;
    // nav_screen.box_width[2] = width/3;
    nav_screen.box_width[0] = width/4;
    nav_screen.box_width[1] = width/4;
    nav_screen.box_width[2] = width/2;

    nav_screen.box_height[0] = height;
    nav_screen.box_height[1] = height;
    nav_screen.box_height[2] = height;

    nav_cursor.cursor_min_y = nav_screen.box_offset_y[1];
    nav_cursor.cursor_max_y = nav_screen.box_offset_y[1] + nav_screen.box_height[1];
    if (nav_cursor.cursor_y < nav_cursor.cursor_min_y) nav_cursor.cursor_y = nav_cursor.cursor_min_y;

    // nav_draw();
    draw_parent_box();
    draw_current_box();
    draw_child_box();
}

unsigned short nav_keybind(char key) {
    if (key == 'q') return ACTION_EXIT;
    else if (key == 'r') {
        set_child_item_path();
        nav_update(nav_screen.width, nav_screen.height);
        return ACTION_REFRESH;
    }
    else if (key == 'k') {
        move_cursor(nav_screen.box_offset_x[1], nav_cursor.cursor_y);
        write_str(" ",1);

        nav_cursor.cursor_y = nav_cursor.cursor_y + 1;
        if (nav_cursor.cursor_y >= nav_cursor.cursor_max_y) nav_cursor.cursor_y = 1;
        move_cursor(nav_screen.box_offset_x[1], nav_cursor.cursor_y );
        write_str(">",1);

        current_item++;
        if (current_item > num_dir_items - 1) current_item = 0;

        set_child_item_path();
        draw_child_box();

        return ACTION_KEYBIND;
    }
    else if (key == 'i') {
        move_cursor(nav_screen.box_offset_x[1], nav_cursor.cursor_y );
        write_str(" ",1);

        nav_cursor.cursor_y = nav_cursor.cursor_y - 1;
        if (nav_cursor.cursor_y < nav_cursor.cursor_min_y) nav_cursor.cursor_y = nav_cursor.cursor_max_y - 1;
        move_cursor(nav_screen.box_offset_x[1], nav_cursor.cursor_y);
        write_str(">",1);

        current_item--;
        if (current_item < 0) current_item = num_dir_items -1;

        set_child_item_path();
        draw_child_box();

        return ACTION_KEYBIND;
    }
    else if (key == 'j') {
        my_strcpy(cwd, parent_dir);
        go_to_parent_dir(parent_dir);
        nav_cursor.cursor_y = 1;
        set_dir_items();
        nav_update(nav_screen.width, nav_screen.height);
    }
    else if (key == 'l' && current_item_type == DT_DIR) {
        my_strcpy(parent_dir, cwd);
        my_strcpy(cwd, child_obj);
        nav_cursor.cursor_y = 1;
        current_item = 0;
        set_child_item_path();
        set_dir_items();
        nav_update(nav_screen.width, nav_screen.height);
    }
    else if (key == '.') {
        show_hidden = !show_hidden;
        set_child_item_path();
        nav_update(nav_screen.width, nav_screen.height);
    }
    return ACTION_NOTHING;
}
