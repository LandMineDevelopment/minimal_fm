#include "defs.h"
#include "syscall.h"
#include "dir.h"
#include "screen.h"
#include "string.h"
#include "ansi.h"
// #include "draw.h"

// Global state instance

// Possible editor paths to try
const char *editor_paths[] = {
    "/usr/bin/nvim",
    "/usr/local/bin/nvim",
    "/bin/nvim",
    "/opt/nvim/bin/nvim",
    "/home/linuxbrew/.linuxbrew/bin/nvim",
    0
};

// void pull_screen_size(void) {
//     struct winsize ws;
//     ret = syscall3(SYS_IOCTL, 0, TIOCGWINSZ, (long)&ws);
    // if (ret == 0 && (ws.ws_row != app_state.height || ws.ws_col != app_state.width)) {
    //     app_state.height = ws.ws_row;
    //     app_state.width = ws.ws_col;
    //     refresh_and_redraw();
    // }
// }


void _start(void) {
     // pull_screen_size();
    // 1. Initialize application state
    // 2. Detect available external editor
    // for (int i = 0; editor_paths[i]; i++) {
    //     long fd = syscall2(SYS_OPEN, (long)editor_paths[i], O_RDONLY);
    //     if (fd >= 0) {
    //         syscall1(SYS_CLOSE, fd);
    //         app_state.editor_cmd = editor_paths[i];
    //         break;
    //     }
    // }

    struct winsize ws;
    syscall3(SYS_IOCTL, 0, TIOCGWINSZ, (long)&ws);
    // nav_screen.height = ws.ws_row;
    // nav_screen.width = ws.ws_col;

    // 4. Raw mode
    struct termios oldt;
    struct termios newt;
    long ret = syscall3(SYS_IOCTL, 0, TCGETS, (long)&oldt);
    if (ret == 0) {
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        syscall3(SYS_IOCTL, 0, TCSETS, (long)&newt);
    }
    // nav_screen.width = ws.ws_col;
    // nav_screen.height = ws.ws_row;

    // 5. Main loop
    hide_cursor();
    clear_screen();
    set_current_dir();
    // move_cursor(5, 10);
    write_str("hello, world", 12);
    move_cursor(ws.ws_col, ws.ws_row);
    int curr_x = 0;
    int curr_y = 0;
    char c;
    unsigned short act = ACTION_NOTHING;
    while (1) {
        long bytes_read = syscall3(SYS_READ, 0, (long)&c, 1);
        if (bytes_read <= 0) break;

        syscall3(SYS_IOCTL, 0, TIOCGWINSZ, (long)&ws);
        nav_update(ws.ws_col, ws.ws_row);
        act = nav_keybind(c);

        if (act == ACTION_EXIT) break;

        if (c == 'd' && act == ACTION_NOTHING) {
            fill_x(0,ws.ws_col,0,ws.ws_row + 1);
        }
        if (c == 'c' && act == ACTION_NOTHING) {
            // clear_section(0,ws.ws_col,0,ws.ws_row + 1);
            clear_section(0,ws.ws_col,0,ws.ws_row + 1);
        }
        if (c == 'i' && act == ACTION_NOTHING) {
            if (curr_y > 0) curr_y--;
            move_cursor(curr_x,curr_y);
        }
        if (c == 'k' && act == ACTION_NOTHING) {
            if (curr_y < ws.ws_row) curr_y++;
            move_cursor(curr_x,curr_y);
        }
        if (c == 'j' && act == ACTION_NOTHING) {
            if (curr_x > 0) curr_x--;
            move_cursor(curr_x,curr_y);
        }
        if (c == 'l' && act == ACTION_NOTHING) {
            if (curr_x < ws.ws_col) curr_x++;
            move_cursor(curr_x,curr_y);
        }
   }

    // 6. Restore terminal
    syscall3(SYS_IOCTL, 0, TCSETS, (long)&oldt);

    // 7. Exit
    clear_screen();
    show_cursor();
    syscall1(SYS_EXIT, 0);
}
