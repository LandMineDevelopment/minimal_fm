#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/syscall.h>
#include "defs.h"
#include "ansi.h"      // for move_cursor
#include "string.h"


void draw_cwd(unsigned short x_start, unsigned short x_stop,
              unsigned short y_start, unsigned short y_stop)
{
    unsigned short max_path = x_stop - x_start;
    char cwd[max_path];
    char *ret = getcwd(cwd, max_path);

    if (ret == NULL) {
        const char *err = "[CWD ERROR]";
        int len = 10;
        for (int i = 0; i < len; i++) cwd[i] = err[i];
        cwd[len] = '\0';
        write(2, "Failed to get current working directory\n", 40);
    }

    unsigned short width = x_stop - x_start + 1;
    unsigned short height = y_stop - y_start + 1;

    save_cursor();

    unsigned short i = 0;
    for (unsigned short row = y_start; row <= y_stop; row++) {
        move_cursor(x_start,row);

        while (i < width && cwd[i] != '\0') {
            write(1, &cwd[i], 1);
            i++;
       }
    }

    restore_cursor();
}

int list_dir_entries(const char *dir_path, char *buf, int buf_size)
{
    int fd = open(dir_path, O_RDONLY);
    if (fd == -1) return -1;

    int total_size = 0;
    while (total_size < buf_size) {
        int nread = syscall(SYS_GETDENTS64, fd, buf + total_size, buf_size - total_size);
        if (nread <= 0) break;
        total_size += nread;
    }

    close(fd);
    return total_size;
}


unsigned short go_to_parent_dir(char *path) {
    if (path == 0) return 0;

    long len = strlen(path);

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
