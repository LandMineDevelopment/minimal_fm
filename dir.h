#ifndef DIR_H
#define DIR_H

// Load directory contents into an Entry array
// Params:
//   path    - Full path to the directory to load
//   entries - Destination Entry array
//   count   - Pointer to store the number of valid entries
// Returns: void
// Pseudo-code:
//   fd = syscall2(SYS_OPEN, path, O_DIRECTORY)
//   if (fd < 0) {
//       // Set error and return early
//       my_strcpy(app_state.error_msg, "Open dir failed");
//       *count = 0;
//       return;
//   }
//   Loop SYS_GETDENTS64...
//   // On success, sort, etc.
//   syscall1(SYS_CLOSE, fd)
//   // Ignore close error
void load_dir_into(const char *path, Entry entries[], int *count);

// Load both current and parent directories
// Pseudo-code:
//   load_dir_into(".", app_state.current_entries, &app_state.num_current)
//   if (app_state.num_current == 0 && error_msg set) → keep message
//   load_dir_into("..", app_state.parent_entries, &app_state.num_parent)
//   Clear selections
void load_directories(void);

// Update cursor limits (unchanged)
void update_cursor_limits(void);

// Check if a directory is empty
// Returns: 1 if empty, 0 otherwise, -1 on error
// Pseudo-code:
//   fd = open path O_DIRECTORY
//   if (fd < 0) return -1
//   loop getdents, count non-./.. entries
//   close fd
//   return count == 0
int dir_is_empty(const char *path);

// Recursive delete/copy (pseudo unchanged — errors propagated via return)
long recursive_delete(const char *path);
long recursive_copy(const char *src, const char *dest);

#endif
