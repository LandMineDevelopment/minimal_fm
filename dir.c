#include "defs.h"
#include "syscall.h"
#include "string.h"
#include "dir.h"

// Linux dirent64 structure (from kernel headers, minimal version)
struct linux_dirent64 {
    unsigned long long d_ino;
    unsigned long long d_off;
    unsigned short     d_reclen;
    unsigned char      d_type;
    char               d_name[];
};

static void sort_entries(Entry entries[], int count) {
    // Simple bubble sort: directories first, then alpha by name
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            int swap = 0;
            if (entries[i].type != DT_DIR && entries[j].type == DT_DIR) {
                swap = 1;  // dir before non-dir
            } else if (entries[i].type == entries[j].type) {
                if (my_strcmp(entries[i].name, entries[j].name) > 0) {
                    swap = 1;  // alpha within same type
                }
            }
            if (swap) {
                Entry temp = entries[i];
                entries[i] = entries[j];
                entries[j] = temp;
            }
        }
    }
}

void load_dir_into(const char *path, Entry entries[], int *count) {
    *count = 0;

    long fd = syscall2(SYS_OPEN, (long)path, O_DIRECTORY);
    if (fd < 0) {
        my_strcpy(app_state.error_msg, "Open dir failed");
        return;
    }

    char buf[4096];
    while (1) {
        long bytes = syscall3(SYS_GETDENTS64, fd, (long)buf, sizeof(buf));
        if (bytes <= 0) break;

        char *p = buf;
        while (p < buf + bytes) {
            struct linux_dirent64 *d = (struct linux_dirent64 *)p;

            // Skip . and ..
            if (d->d_name[0] == '.' && (d->d_name[1] == '\0' ||
                                        (d->d_name[1] == '.' && d->d_name[2] == '\0'))) {
                p += d->d_reclen;
                continue;
            }

            // Skip hidden if !show_hidden
            if (!app_state.show_hidden && d->d_name[0] == '.') {
                p += d->d_reclen;
                continue;
            }

            // Copy name
            my_strcpy(entries[*count].name, d->d_name);

            // Set type and clear flags/extension
            entries[*count].type = d->d_type;
            entries[*count].flags = 0;
            entries[*count].extension[0] = '\0';

            // Set FLAG_HIDDEN
            if (d->d_name[0] == '.') {
                entries[*count].flags |= FLAG_HIDDEN;
            }

            // Extract extension (after last '.')
            const char *dot = 0;
            const char *c = d->d_name;
            while (*c) {
                if (*c == '.') dot = c;
                c++;
            }
            if (dot && dot != d->d_name && *(dot + 1)) {
                my_strcpy(entries[*count].extension, dot + 1);
            }

            // Set FLAG_EMPTY_DIR if directory
            if (d->d_type == DT_DIR) {
                char full_subpath[MAX_PATH];
                path_build(full_subpath, path, d->d_name);
                if (dir_is_empty(full_subpath)) {
                    entries[*count].flags |= FLAG_EMPTY_DIR;
                }
            }

            (*count)++;
            if (*count >= MAX_ENTRIES) goto done;

            p += d->d_reclen;
        }
    }

done:
    sort_entries(entries, *count);
    syscall1(SYS_CLOSE, fd);  // ignore error
}

void load_directories(void) {
    load_dir_into(".", app_state.current_entries, &app_state.num_current);
    load_dir_into("..", app_state.parent_entries, &app_state.num_parent);

    // Clear selections on reload
    for (int i = 0; i < app_state.num_current; i++) {
        app_state.current_entries[i].flags &= ~FLAG_SELECTED;
    }
}

void update_cursor_limits(void) {
    int count = (app_state.in_modal_input && app_state.modal_context == CTX_SEARCH)
                ? app_state.num_filtered
                : app_state.num_current;

    app_state.cursor_min = 0;
    app_state.cursor_max = count > 0 ? count - 1 : 0;
    if (app_state.cursor_loc < app_state.cursor_min) app_state.cursor_loc = app_state.cursor_min;
    if (app_state.cursor_loc > app_state.cursor_max) app_state.cursor_loc = app_state.cursor_max;
}

int dir_is_empty(const char *path) {
    long fd = syscall2(SYS_OPEN, (long)path, O_DIRECTORY);
    if (fd < 0) return -1;

    char buf[4096];
    int non_dot_count = 0;

    while (1) {
        long bytes = syscall3(SYS_GETDENTS64, fd, (long)buf, sizeof(buf));
        if (bytes <= 0) break;

        char *p = buf;
        while (p < buf + bytes) {
            struct linux_dirent64 *d = (struct linux_dirent64 *)p;

            if (d->d_name[0] != '.' ||
                (d->d_name[1] != '\0' && (d->d_name[1] != '.' || d->d_name[2] != '\0'))) {
                non_dot_count++;
            }
            p += d->d_reclen;
        }
    }

    syscall1(SYS_CLOSE, fd);
    return non_dot_count == 0;
}
