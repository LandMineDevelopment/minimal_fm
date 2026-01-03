#ifndef DIR_H
#define DIR_H
void draw_cwd(unsigned short x_start, unsigned short x_stop,
              unsigned short y_start, unsigned short y_stop);
int list_dir_entries(long dir_type, char *buf, int buf_size);
unsigned short draw_dir_listing(long dir_type, unsigned short x_start, unsigned short x_stop,
              unsigned short y_start, unsigned short y_stop);

#endif
