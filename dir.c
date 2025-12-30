#include "defs.h"      // for SYS_GETCWD, MAX_PATH
#include "syscall.h"
#include "ansi.h"      // for move_cursor, write_str
#include "string.h"

#define MAX_DIRENT_BUF 8192

void draw_cwd(unsigned short x_start, unsigned short x_stop,
              unsigned short y_start, unsigned short y_stop)
{
    unsigned short max_path = x_stop - x_start;
    char cwd[max_path ];
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

// int list_dir_entries(char *buf, int buf_size)
// {
//     long fd = syscall2(SYS_OPEN, (long)".", O_DIRECTORY | O_RDONLY);
//     if (fd < 0) return -1;
//
//     long nread = syscall3(SYS_GETDENTS64, fd, (long)buf, buf_size);
//     syscall1(SYS_CLOSE, fd);
//
//     if (nread < 0) return -1;
//     return nread;  // number of bytes read
// }

int list_dir_entries(long dir_type, char *buf, int buf_size)
{
    long fd = syscall2(SYS_OPEN, (long)dir_type, O_DIRECTORY | O_RDONLY);
    if (fd < 0) return -1;

    long nread = syscall3(SYS_GETDENTS64, fd, (long)buf, buf_size);
    syscall1(SYS_CLOSE, fd);

    if (nread < 0) return -1;
    return nread;
}

void draw_dir_listing(long dir_type, unsigned short x_start, unsigned short x_stop,
                      unsigned short y_start, unsigned short y_stop)
{
    // char buf[32 + (1 + x_stop - x_start) * (1 + y_stop - y_start) ];
    char buf[MAX_DIRENT_BUF];
    int nread = list_dir_entries((long)dir_type, buf, sizeof(buf));
    if (nread <= 0) {
        move_cursor(x_start, y_start);
        write_str("[NO FILES]", 10);
        return;
    }

    save_cursor();
    unsigned short curr_row = y_start;
    unsigned short curr_col = x_start;
    int pos = 0;
    while (pos < nread && curr_row <= y_stop) {
    // while (pos < nread ) {
        move_cursor(x_start, curr_row);

        struct linux_dirent64 *d = (struct linux_dirent64 *)(buf + pos);

        unsigned short len_write = 0;
        if (my_strlen(d->d_name) > x_stop - x_start) len_write = x_stop - x_start;
        else len_write = my_strlen(d->d_name);
        write_str(d->d_name, len_write);

        pos += d->d_reclen;
        curr_row += 1;
    }

    restore_cursor();
}
