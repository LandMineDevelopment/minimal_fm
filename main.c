#include "defs.h"
#include "syscall.h"
#include "dir.h"
#include "screen.h"
#include "keybind.h"
#include "string.h"

// Global state instance
AppState app_state = {0};

// Possible editor paths to try
const char *editor_paths[] = {
    "/usr/bin/nvim",
    "/usr/local/bin/nvim",
    "/bin/nvim",
    "/opt/nvim/bin/nvim",
    "/home/linuxbrew/.linuxbrew/bin/nvim",
    0
};

void _start(void) {
    // 1. Initialize application state
    app_state.height = 24;
    app_state.width = 80;
    app_state.margin = 2;
    app_state.left_parts = 1;
    app_state.middle_parts = 1;
    app_state.right_parts = 2;
    app_state.show_hidden = 0;
    app_state.preview_mode = PREVIEW_NONE;
    app_state.cursor_loc = 0;
    app_state.cursor_min = 0;
    app_state.cursor_max = 0;
    app_state.editor_cmd = 0;  // Fixed: 0 instead of NULL
    app_state.num_clipboard = 0;
    app_state.is_cut = 0;
    app_state.in_modal_input = 0;
    app_state.modal_context = CTX_NORMAL;
    app_state.input_len = 0;
    app_state.delete_multi = 0;
    app_state.delete_recursive = 0;

    // 2. Detect available external editor
    for (int i = 0; editor_paths[i]; i++) {
        long fd = syscall2(SYS_OPEN, (long)editor_paths[i], O_RDONLY);
        if (fd >= 0) {
            syscall1(SYS_CLOSE, fd);
            app_state.editor_cmd = editor_paths[i];
            break;
        }
    }

    // 3. Initial refresh
    refresh_and_redraw();

    // 4. Raw mode
    long ret = syscall3(SYS_IOCTL, 0, TCGETS, (long)&app_state.oldt);
    if (ret == 0) {
        app_state.newt = app_state.oldt;
        app_state.newt.c_lflag &= ~(ICANON | ECHO);
        syscall3(SYS_IOCTL, 0, TCSETS, (long)&app_state.newt);
    }
    refresh_and_redraw();

    // 5. Main loop
    char c;
    while (1) {
        long bytes_read = syscall3(SYS_READ, 0, (long)&c, 1);
        if (bytes_read <= 0) break;

        // Resize poll
        struct winsize ws;
        ret = syscall3(SYS_IOCTL, 0, TIOCGWINSZ, (long)&ws);
        if (ret == 0 && (ws.ws_row != app_state.height || ws.ws_col != app_state.width)) {
            app_state.height = ws.ws_row;
            app_state.width = ws.ws_col;
            refresh_and_redraw();
        }

        int action = handle_key(c);

        if (action == ACTION_EXIT) break;
        if (action == ACTION_REFRESH) refresh_and_redraw();
        if (action == ACTION_REDRAW) {
            build_screen();
            draw_screen();
        }
    }

    // 6. Restore terminal
    syscall3(SYS_IOCTL, 0, TCSETS, (long)&app_state.oldt);

    // 7. Exit
    syscall1(SYS_EXIT, 0);
}
