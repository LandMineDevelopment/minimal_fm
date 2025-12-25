#include "defs.h"
#include "syscall.h"
#include "dir.h"
#include "screen.h"
#include "keybind.h"
#include "string.h"

typedef int (*ActionFunc)(char c);

typedef struct {
    char key;
    char key_alt;
    char ctrl_code;
    ActionFunc action;
} KeyBinding;

// Action declarations (all take char c)
static int action_quit(char c);
static int action_refresh(char c);
static int action_move_up(char c);
static int action_move_down(char c);
static int action_enter(char c);
static int action_parent(char c);
static int action_toggle_hidden(char c);
static int action_toggle_selection(char c);
static int action_start_add(char c);
static int action_start_rename(char c);
static int action_start_search(char c);
static int action_start_delete_confirm(char c);
static int action_copy(char c);
static int action_cut(char c);
static int action_paste(char c);
static int action_append_char(char c);
static int action_backspace(char c);
static int action_confirm_input(char c);
static int action_cancel_modal(char c);
static int action_search_append_and_filter(char c);
static int action_delete_yes(char c);
static int action_delete_no(char c);
static int action_delete_toggle_recursive(char c);

// ========================
// Universal keybindings — always active
// ========================
static const KeyBinding universal_bindings[] = {
    {0x1B, 0, 0x00, action_cancel_modal},
    {'q', 0, 0x00, action_quit},
    {0, 0, 0x12, action_refresh},
    {0, 0, 0x03, action_quit},
    {0, 0, 0x11, action_quit},
    {0, 0, 0x00, 0}
};

// ========================
// Context-specific tables
// ========================
static const KeyBinding normal_bindings[] = {
    {'i', 0, 0x00, action_move_up},
    {'k', 0, 0x00, action_move_down},
    {'l', 0, 0x00, action_enter},
    {'j', 0, 0x00, action_parent},
    {'.', 0, 0x00, action_toggle_hidden},
    {' ', 0, 0x00, action_toggle_selection},
    {'a', 0, 0x00, action_start_add},
    {'m', 0, 0x00, action_start_rename},
    {'d', 0, 0x00, action_start_delete_confirm},
    {'c', 0, 0x00, action_copy},
    {'x', 0, 0x00, action_cut},
    {'p', 0, 0x00, action_paste},
    {'/', 0, 0x00, action_start_search},
    {0, 0, 0x00, 0}
};

static const KeyBinding typing_bindings[] = {
    {0x7F, 0, 0x00, action_backspace},
    {'\r', 0, 0x00, action_confirm_input},
    {0, 0, 0x00, 0}
};

static const KeyBinding search_bindings[] = {
    {0x7F, 0, 0x00, action_backspace},
    {0, 0, 0x00, 0}
};

static const KeyBinding delete_bindings[] = {
    {'y', 'Y', 0x00, action_delete_yes},
    {'n', 'N', 0x00, action_delete_no},
    {'r', 'R', 0x00, action_delete_toggle_recursive},
    {0, 0, 0x00, 0}
};

// ========================
// Helper: lookup in table
// ========================
static int lookup_and_execute(const KeyBinding *table, char c) {
    for (int i = 0; table[i].action; i++) {
        if (c == table[i].key ||
            (table[i].key_alt && c == table[i].key_alt) ||
            (table[i].ctrl_code && c == table[i].ctrl_code)) {
            return table[i].action(c);
        }
    }
    return ACTION_NONE;
}

// ========================
// Unified dispatcher
// ========================
int handle_input(unsigned char context, char c) {
    const KeyBinding *table = 0;
    if (context == CTX_NORMAL) table = normal_bindings;
    else if (context == CTX_ADD || context == CTX_RENAME) table = typing_bindings;
    else if (context == CTX_SEARCH) table = search_bindings;
    else if (context == CTX_DELETE_CONFIRM) table = delete_bindings;
    if (!table) return ACTION_NONE;
    if ((context == CTX_ADD || context == CTX_RENAME || context == CTX_SEARCH) &&
        (c >= 0x20 && c <= 0x7E)) {
        if (context == CTX_SEARCH)
            return action_search_append_and_filter(c);
        else
            return action_append_char(c);
    }
    return lookup_and_execute(table, c);
}

// ========================
// Public handler
// ========================
int handle_key(char c) {
    int action = lookup_and_execute(universal_bindings, c);
    if (action != ACTION_NONE) return action;
    if (app_state.in_modal_input) {
        return handle_input(app_state.modal_context, c);
    } else {
        return handle_input(CTX_NORMAL, c);
    }
}

// ========================
// Actions (all take char c, ignore unused with (void)c)
// ========================
static int action_quit(char c) {
    (void)c;
    return ACTION_EXIT;
}
static int action_refresh(char c) {
    (void)c;
    refresh_and_redraw();
    return ACTION_NONE;
}
static int action_cancel_modal(char c) {
    (void)c;
    app_state.in_modal_input = 0;
    app_state.input_len = 0;
    build_screen();
    draw_screen();
    return ACTION_NONE;
}
static int action_move_up(char c) {
    (void)c;
    update_cursor_limits();
    if (app_state.cursor_max > 0) {
        if (app_state.cursor_loc == 0)
            app_state.cursor_loc = app_state.cursor_max;
        else
            app_state.cursor_loc--;
    }
    build_screen();
    draw_screen();
    return ACTION_NONE;
}
static int action_move_down(char c) {
    (void)c;
    update_cursor_limits();
    if (app_state.cursor_max > 0) {
        if (app_state.cursor_loc == app_state.cursor_max)
            app_state.cursor_loc = 0;
        else
            app_state.cursor_loc++;
    }
    build_screen();
    draw_screen();
    return ACTION_NONE;
}
static int action_enter(char c) {
    (void)c;

    Entry *selected = 0;
    if (app_state.in_modal_input && app_state.modal_context == CTX_SEARCH) {
        if (app_state.cursor_loc < app_state.num_filtered) {
            selected = &app_state.filtered_entries[app_state.cursor_loc];
        }
    } else if (app_state.cursor_loc < app_state.num_current) {
        selected = &app_state.current_entries[app_state.cursor_loc];
    }

    if (!selected) {
        my_strcpy(app_state.error_msg, "No selection");
        build_screen();
        draw_screen();
        return ACTION_NONE;
    }

    char full_path[MAX_PATH];
    path_build(full_path, ".", selected->name);

    if (selected->type == DT_DIR) {
        long ret = syscall1(SYS_CHDIR, (long)full_path);
        if (ret < 0) {
            my_strcpy(app_state.error_msg, "chdir failed");
        } else {
            refresh_and_redraw();
            return ACTION_NONE;
        }
    } else if (selected->type == DT_REG && app_state.editor_cmd) {
        // Fork and exec editor
        long pid = syscall0(SYS_FORK);
        if (pid == 0) {
            // Child
            const char *argv[3] = { app_state.editor_cmd, full_path, 0 };
            syscall3(SYS_EXECVE, (long)app_state.editor_cmd, (long)argv, 0);  // envp = 0
            // If execve fails, exit child
            syscall1(SYS_EXIT, 1);
        } else if (pid > 0) {
            // Parent — wait for child
            long wstatus;
            syscall4(SYS_WAIT4, pid, (long)&wstatus, 0, 0);
            refresh_and_redraw();
            return ACTION_NONE;
        } else {
            my_strcpy(app_state.error_msg, "fork failed");
        }
    } else {
        my_strcpy(app_state.error_msg, "No editor / not dir");
    }

    build_screen();
    draw_screen();
    return ACTION_NONE;
}
static int action_parent(char c) {
    (void)c;
    syscall1(SYS_CHDIR, (long)"..");
    refresh_and_redraw();
    return ACTION_NONE;
}
static int action_toggle_hidden(char c) {
    (void)c;
    app_state.show_hidden = !app_state.show_hidden;
    refresh_and_redraw();
    return ACTION_NONE;
}
static int action_toggle_selection(char c) {
    (void)c;
    Entry *entries = (app_state.in_modal_input && app_state.modal_context == CTX_SEARCH)
                     ? app_state.filtered_entries
                     : app_state.current_entries;
    int count = (app_state.in_modal_input && app_state.modal_context == CTX_SEARCH)
                ? app_state.num_filtered
                : app_state.num_current;
    if (app_state.cursor_loc < count) {
        entries[app_state.cursor_loc].flags ^= FLAG_SELECTED;
        action_move_down(0);
    }
    build_screen();
    draw_screen();
    return ACTION_NONE;
}
static int action_start_add(char c) {
    (void)c;
    app_state.in_modal_input = 1;
    app_state.modal_context = CTX_ADD;
    app_state.input_len = 0;
    build_screen();
    draw_screen();
    return ACTION_NONE;
}
static int action_start_rename(char c) {
    (void)c;
    if (app_state.cursor_loc >= app_state.num_current) return ACTION_NONE;
    my_strcpy(app_state.input_buffer, app_state.current_entries[app_state.cursor_loc].name);
    app_state.input_len = my_strlen(app_state.input_buffer);
    app_state.in_modal_input = 1;
    app_state.modal_context = CTX_RENAME;
    build_screen();
    draw_screen();
    return ACTION_NONE;
}
static int action_start_search(char c) {
    (void)c;
    app_state.in_modal_input = 1;
    app_state.modal_context = CTX_SEARCH;
    app_state.input_len = 0;
    app_state.num_filtered = 0;
    build_screen();
    draw_screen();
    return ACTION_NONE;
}
static int action_start_delete_confirm(char c) {
    (void)c;
    app_state.in_modal_input = 1;
    app_state.modal_context = CTX_DELETE_CONFIRM;
    app_state.delete_multi = count_selected() > 0;
    app_state.delete_recursive = 0;
    build_screen();
    draw_screen();
    return ACTION_NONE;
}
static int action_copy(char c) {
    (void)c;
    // TODO
    return ACTION_NONE;
}
static int action_cut(char c) {
    (void)c;
    // TODO
    return ACTION_NONE;
}
static int action_paste(char c) {
    (void)c;
    // TODO
    return ACTION_NONE;
}
static int action_append_char(char c) {
    if (app_state.input_len < MAX_NAME_LEN - 1) {
        app_state.input_buffer[app_state.input_len++] = c;
        app_state.input_buffer[app_state.input_len] = '\0';
        build_screen();
        draw_screen();
    }
    return ACTION_NONE;
}
static int action_backspace(char c) {
    (void)c;
    if (app_state.input_len > 0) {
        app_state.input_len--;
        app_state.input_buffer[app_state.input_len] = '\0';
        build_screen();
        draw_screen();
    }
    return ACTION_NONE;
}
static int action_confirm_input(char c) {
    (void)c;
    // TODO: create/rename based on context
    action_cancel_modal(0);
    return ACTION_REFRESH;
}
static int action_search_append_and_filter(char c) {
    action_append_char(c);
    // TODO: filter
    return ACTION_NONE;
}
static int action_delete_yes(char c) {
    (void)c;
    // TODO: delete
    action_cancel_modal(0);
    return ACTION_REFRESH;
}
static int action_delete_no(char c) {
    (void)c;
    action_cancel_modal(0);
    return ACTION_NONE;
}
static int action_delete_toggle_recursive(char c) {
    (void)c;
    app_state.delete_recursive = !app_state.delete_recursive;
    build_screen();
    draw_screen();
    return ACTION_NONE;
}
