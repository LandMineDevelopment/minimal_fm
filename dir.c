#include "defs.h"      // for SYS_GETCWD, MAX_PATH
#include "syscall.h"
#include "ansi.h"      // for move_cursor, write_str
#include "string.h"


void draw_cwd(unsigned short x_start, unsigned short x_stop,
              unsigned short y_start, unsigned short y_stop)
{
    unsigned short max_path = x_stop - x_start;
    char cwd[max_path];
    long ret = syscall2(SYS_GETCWD, (long)&cwd, max_path);

    if (ret < 0) {
        const char *err = "[CWD ERROR]";
        ret = 10;
        for (int i = 0; i < ret; i++) cwd[i] = err[i];
        cwd[ret] = '\0';
    } else {
        cwd[ret] = '\0';
    }

    unsigned short width = x_stop - x_start + 1;
    unsigned short height = y_stop - y_start + 1;

    save_cursor();

    unsigned short i = 0;
    for (unsigned short row = y_start; row <= y_stop; row++) {
        move_cursor(x_start,row);

        while (i < width && cwd[i] != '\0') {
            write_str(&cwd[i], 1);
            i++;
       }
    }

    restore_cursor();
}

int list_dir_entries(long dir_type, char *buf, int buf_size)
{
    long fd = syscall2(SYS_OPEN, (long)dir_type, O_DIRECTORY | O_RDONLY);
    if (fd < 0) return -1;

    long nread = syscall3(SYS_GETDENTS64, fd, (long)buf, buf_size);
    syscall1(SYS_CLOSE, fd);

    if (nread < 0) return -1;
    return nread;
}


unsigned short go_to_parent_dir(char *path) {
    if (path == 0) return 0;

    long len = my_strlen(path);

    if (len == 0 || (len == 1 && path[0] == '/')) {
        return 0;
    }
    // Remove trailing slash if present (except for root)
    if (len > 1 && path[len - 1] == '/') {
        path[--len] = '\0';
    }
    // Find the last '/' — that's the end of the current dir name
    long i = len - 1;
    while (i > 0 && path[i] != '/') {
        i--;
    }
    // If we found a '/', truncate there
    if (i > 0) {
        path[i] = '\0';  // truncate to parent
        return 1;
    }
    // If we're at something like "dir" with no '/', go to root
    if (i == 0 && path[0] != '/') {
        path[0] = '/';
        path[1] = '\0';
        return 1;
    }
    // Already at root
    return 0;
}

// Delete a file or directory recursively
void delete_recursive(const char *path) {
    char buf[MAX_DIRENT_BUF];
    int nread = list_dir_entries((long)path, buf, sizeof(buf));
    if (nread >= 0) {
        // It's a directory, delete contents first
        int pos = 0;
        while (pos < nread) {
            struct linux_dirent64 *d = (struct linux_dirent64 *)(buf + pos);
            if (d->d_name[0] == '.' && (d->d_name[1] == '\0' ||
                (d->d_name[1] == '.' && d->d_name[2] == '\0'))) {
                pos += d->d_reclen;
                continue;
            }
            // Build full path
            char full_path[MAX_PATH];
            path_build(full_path, path, d->d_name);
            // Recurse
            delete_recursive(full_path);
            pos += d->d_reclen;
        }
        // Now remove the directory
        syscall1(SYS_RMDIR, (long)path);
    } else {
        // It's a file, unlink
        syscall1(SYS_UNLINK, (long)path);
    }
}

// Create a file if it does not exist
// Returns 1 if created, 0 if already exists or error
int create_file(const char *path) {
    long ret = syscall3(SYS_OPEN, (long)path, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (ret >= 0) {
        syscall1(SYS_CLOSE, ret);
        return 1; // created
    } else {
        return 0; // exists or error
    }
}
