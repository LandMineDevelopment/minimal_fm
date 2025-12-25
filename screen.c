#include "defs.h"
#include "syscall.h"
#include "string.h"
#include "dir.h"
#include "screen.h"

void set_screen_char(int row, int col, char ch) {
    if (row < 0 || row >= app_state.height || col < 0 || col >= app_state.width) return;
    app_state.screen[row * MAX_COLS + col] = ch;
}

void clear_screen_buffer(void) {
    for (int i = 0; i < MAX_ROWS * MAX_COLS; i++) {
        app_state.screen[i] = ' ';
    }
}

void fill_header(void) {
    const char *title = "File Manager";
    int title_len = my_strlen(title);
    int start_col = (app_state.width - title_len) / 2;
    if (start_col < 0) start_col = 0;

    for (int i = 0; i < app_state.width; i++) {
        set_screen_char(0, i, '-');
    }

    for (int i = 0; title[i]; i++) {
        if (start_col + i < app_state.width) {
            set_screen_char(0, start_col + i, title[i]);
        }
    }

    if (app_state.error_msg[0]) {
        int err_len = my_strlen(app_state.error_msg);
        int err_start = app_state.width - err_len - 2;
        if (err_start < 0) err_start = 0;
        for (int i = 0; i < err_len && err_start + i < app_state.width; i++) {
            set_screen_char(0, err_start + i, app_state.error_msg[i]);
        }
        app_state.error_msg[0] = '\0';
    }
}

int get_left_width(void) {
    int avail = app_state.width - 2 * app_state.margin;
    if (avail < 0) avail = 0;
    return (avail * app_state.left_parts) / (app_state.left_parts + app_state.middle_parts + app_state.right_parts);
}

int get_middle_width(void) {
    int avail = app_state.width - 2 * app_state.margin;
    if (avail < 0) avail = 0;
    return (avail * app_state.middle_parts) / (app_state.left_parts + app_state.middle_parts + app_state.right_parts);
}

int get_right_width(void) {
    int avail = app_state.width - 2 * app_state.margin;
    if (avail < 0) avail = 0;
    return (avail * app_state.right_parts) / (app_state.left_parts + app_state.middle_parts + app_state.right_parts);
}

static void draw_entry_line(int row, int col_start, int width, const Entry *e, char marker) {
    if (row >= app_state.height) return;

    int end_col = col_start + width;
    if (end_col > app_state.width) end_col = app_state.width;

    int pos = col_start;

    if (marker != '\0') {
        set_screen_char(row, col_start, marker);
        pos = col_start + 1;
    }

    const char *name = e->name;

    // for (int i = 0; name[i] && pos < end_col - 2; i++) {
    for (int i = 0; name[i] && pos < end_col; i++) {
        set_screen_char(row, pos++, name[i]);
    }

    if (name[my_strlen(name)] && pos < end_col) {
        set_screen_char(row, pos++, '~');
    }

    if (e->type == DT_DIR) {
        if (pos < end_col) set_screen_char(row, pos++, '/');
        if (e->flags & FLAG_EMPTY_DIR && pos < end_col) {
            set_screen_char(row, pos++, 'e');
        }
    }
}

void fill_left_pane(void) {
    int width = get_left_width();
    if (width <= 0) return;

    int start_row = 2;
    // int start_col = app_state.margin;
    int start_col = 0;

    for (int i = 0; i < app_state.num_parent && start_row + i < app_state.height; i++) {
        draw_entry_line(start_row + i, start_col, width, &app_state.parent_entries[i], '\0');
    }
}

void fill_middle_pane(void) {
    int width = get_middle_width();
    if (width <= 0) return;

    int start_row = 2;
    // int start_col = app_state.margin + get_left_width() + app_state.margin;
    int start_col = get_left_width() + app_state.margin;

    Entry *entries = (app_state.in_modal_input && app_state.modal_context == CTX_SEARCH)
                     ? app_state.filtered_entries
                     : app_state.current_entries;
    int count = (app_state.in_modal_input && app_state.modal_context == CTX_SEARCH)
                ? app_state.num_filtered
                : app_state.num_current;

    for (int i = 0; i < count && start_row + i < app_state.height; i++) {
        char marker = ' ';
        if (i == app_state.cursor_loc && entries[i].flags & FLAG_SELECTED) marker = '-';
        else if (i == app_state.cursor_loc) marker = '>';
        else if (entries[i].flags & FLAG_SELECTED) marker = '*';

        draw_entry_line(start_row + i, start_col, width, &entries[i], marker);
    }
}

void fill_right_pane(void) {
    int width = get_right_width();
    if (width <= 0) return;

    int start_row = 2;
    // int start_col = app_state.margin + get_left_width() + app_state.margin + get_middle_width() + app_state.margin;
    int start_col = get_left_width() + app_state.margin + get_middle_width() + app_state.margin;

    // Explicitly clear the right pane area in screen buffer
    for (int r = start_row; r < app_state.height; r++) {
        for (int c = start_col; c < start_col + width && c < app_state.width; c++) {
            set_screen_char(r, c, ' ');
        }
    }

    app_state.num_preview = 0;
    app_state.preview_mode = PREVIEW_NONE;
    for (int i = 0; i < MAX_ENTRIES; i++) {
        app_state.preview_entries[i].name[0] = '\0';
        app_state.preview_entries[i].type = 0;
        app_state.preview_entries[i].flags = 0;
        app_state.preview_entries[i].extension[0] = '\0';
    }

    Entry *selected = 0;

    if (app_state.in_modal_input && app_state.modal_context == CTX_SEARCH) {
        if (app_state.cursor_loc < app_state.num_filtered) {
            selected = &app_state.filtered_entries[app_state.cursor_loc];
        }
    } else if (app_state.cursor_loc < app_state.num_current) {
        selected = &app_state.current_entries[app_state.cursor_loc];
    }

    if (!selected) {
        const char *msg = "No selection";
        int msg_len = my_strlen(msg);
        int msg_col = start_col + (width - msg_len) / 2;
        if (msg_col < start_col) msg_col = start_col;
        for (int i = 0; msg[i] && msg_col + i < start_col + width; i++) {
            set_screen_char(start_row, msg_col + i, msg[i]);
        }
        return;
    }

    char full_path[MAX_PATH];
    path_build(full_path, ".", selected->name);

    if (selected->type == DT_DIR) {
        load_dir_into(full_path, app_state.preview_entries, &app_state.num_preview);
        app_state.preview_mode = PREVIEW_DIR_CONTENTS;

        for (int i = 0; i < app_state.num_preview && start_row + i < app_state.height; i++) {
            draw_entry_line(start_row + i, start_col, width, &app_state.preview_entries[i], '\0');
        }
    } else if (selected->type == DT_REG) {
        long fd = syscall2(SYS_OPEN, (long)full_path, O_RDONLY);
        if (fd < 0) {
            const char *msg = "Cannot open";
            for (int i = 0; msg[i] && start_col + i < start_col + width; i++) {
                set_screen_char(start_row, start_col + i, msg[i]);
            }
            return;
        }

        // char line_buf[MAX_COLS];
        char line_buf[width * MAX_PREVIEW_LINES];
        // initialize array with blank data
        for (int i = 0; i < width * MAX_PREVIEW_LINES; i++) {
            line_buf[i] = ' ';
        }
        int line_count = 0;
        int pos = 0;
        char is_text = 1;
        int curr_buff_ind = 0;

        while (line_count < MAX_PREVIEW_LINES && start_row + line_count < app_state.height) {
            long ret = syscall3(SYS_READ, fd, (long)line_buf, sizeof(line_buf) - 1);
            if (ret <= 0) break;
            // while (line_buf[curr_buff_ind] && pos < width) {
            while (line_buf[curr_buff_ind] && start_row + line_count < app_state.height) {
                char c = line_buf[curr_buff_ind];
                if (c == '\n') {
                    pos = 0;
                    line_count++;
                // } else if (c != '-' && (c == '\0' || (unsigned char)c < 0x20 || (unsigned char)c > 0x7E) ) {
                } else if (c == '\0' ||
                    ((unsigned char)c < 0x20 && c != '\t' && c != '\n' && c != '\r')) { 
                    is_text = 0;
                    goto binary;
                } else {
                    set_screen_char(start_row + line_count, start_col + pos, c);
                    pos++;
                }
                curr_buff_ind++;
                if (pos >= width) {
                   pos = 0;
                }
            }
        }

        if (is_text) {
            app_state.preview_mode = PREVIEW_FILE_TEXT;
        } else {
binary:
            const char *msg = "Binary file";
            for (int i = 0; msg[i] && start_col + i < start_col + width; i++) {
                set_screen_char(start_row, start_col + i, msg[i]);
            }
        }
        syscall1(SYS_CLOSE, fd);
    }
}

void build_screen(void) {
    clear_screen_buffer();
    fill_header();
    fill_left_pane();
    fill_middle_pane();
    fill_right_pane();
}

void draw_screen(void) {
    write_str("\x1b[?25l", 6);
    write_str("\x1b[H", 3);

    for (int row = 0; row < app_state.height; row++) {
        write_str(&app_state.screen[row * MAX_COLS], app_state.width);
        // write_str("\r\n", 2);
    }

    write_str("\x1b[?25h", 6);
}

void refresh_and_redraw(void) {
    struct winsize ws;
    long ret = syscall3(SYS_IOCTL, 0, TIOCGWINSZ, (long)&ws);
    if (ret == 0) {
        app_state.height = ws.ws_row;
        app_state.width = ws.ws_col;
    }

    load_directories();
    update_cursor_limits();
    build_screen();
    draw_screen();
}

int count_selected(void) {
    int count = 0;
    for (int i = 0; i < app_state.num_current; i++) {
        if (app_state.current_entries[i].flags & FLAG_SELECTED) count++;
    }
    return count;
}
