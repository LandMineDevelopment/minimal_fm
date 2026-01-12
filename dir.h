#ifndef DIR_H
#define DIR_H
void draw_cwd(unsigned short x_start, unsigned short x_stop,
              unsigned short y_start, unsigned short y_stop);
int list_dir_entries(long dir_type, char *buf, int buf_size);
unsigned short go_to_parent_dir(char *path);

// Delete a file or directory recursively
void delete_recursive(const char *path);

// Create a file if it does not exist
// Returns 1 if created, 0 if already exists
int create_file(const char *path);

#endif
