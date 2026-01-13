#include <sys/syscall.h>

// Portable syscall numbers using system headers
#define SYS_READ __NR_read
#define SYS_WRITE __NR_write
#define SYS_EXIT __NR_exit
#define SYS_IOCTL __NR_ioctl
#define SYS_OPEN __NR_open
#define SYS_CLOSE __NR_close
#define SYS_GETDENTS64 __NR_getdents64
#define SYS_CHDIR __NR_chdir
#define SYS_GETCWD __NR_getcwd
#define SYS_FORK __NR_fork
#define SYS_EXECVE __NR_execve
#define SYS_WAIT4 __NR_wait4
#define SYS_MKDIR __NR_mkdir
#define SYS_CREAT __NR_creat
#define SYS_UNLINK __NR_unlink
#define SYS_RMDIR __NR_rmdir
#define SYS_RENAME __NR_rename

// IOCTL requests
#define TCGETS 0x5401
#define TCSETS 0x5402
#define TIOCGWINSZ 0x5413

// Open flags and dirent types are now defined in standard headers
typedef unsigned short (*update_func)(void *Screen, unsigned short, unsigned short);
typedef unsigned short (*keybind_func)(char);

// Term flags
#define ICANON 0000002
#define ECHO 0000010

// Limits
#define MAX_ROWS 100
#define MAX_COLS 300
#define MAX_ENTRIES 256
#define MAX_NAME_LEN 256
#define MAX_PATH 4096
#define MAX_CLIPBOARD 64
#define MAX_SEARCH_LEN 64
#define MAX_ERROR_LEN 128
#define MAX_EXT_LEN 16
#define MAX_PREVIEW_LINES 20
#define MAX_DIRENT_BUF 8192
#define MAX_BOXES_PER_SCREEN 15
#define NUM_SCREENS 1

// Modal contexts
#define CTX_NORMAL          0
#define CTX_ADD             1
#define CTX_RENAME          2
#define CTX_SEARCH          3
#define CTX_DELETE_CONFIRM  4

//Grid dim types
#define FIXED 0
#define RELATIVE 1

//Actions
typedef enum {
    ACTION_NOTHING,
    ACTION_EXIT,
    ACTION_REFRESH,
    ACTION_KEYBIND,
} Action;


// Preview modes
typedef enum {
    PREVIEW_NONE,
    PREVIEW_DIR_CONTENTS,
    PREVIEW_FILE_TEXT
} PreviewMode;

// Entry flags
#define FLAG_SELECTED  (1 << 0)
#define FLAG_EMPTY_DIR (1 << 1)
#define FLAG_HIDDEN    (1 << 2)
#define FLAG_PROTECTED (1 << 3)

struct linux_dirent64 {
    unsigned long long d_ino;
    unsigned long long d_off;
    unsigned short     d_reclen;
    unsigned char      d_type;
    char               d_name[];
};

typedef struct {
    char name[MAX_NAME_LEN];
    unsigned char type;
    unsigned char flags;
    char extension[MAX_EXT_LEN];
} Entry;

// Terminal structures are now defined in standard headers

typedef struct {
    unsigned short id;
    unsigned short column_offset;
    unsigned short row_offset;
    unsigned short height;
    unsigned short width;
    unsigned short dim_type;
    unsigned short top_margin;
    unsigned short bottom_margin;
    unsigned short left_margin;
    unsigned short right_margin;
    char top_border;
    char bottom_boarder;
    char left_border;
    char right_border;
} Box;

typedef struct {
    unsigned short height;
    unsigned short width;
    unsigned short top_margin;
    unsigned short bottom_margin;
    unsigned short left_margin;
    unsigned short right_margin;
    char top_border;
    char bottom_border;
    char left_border;
    char right_border;
    unsigned short column;

    //Grid
    unsigned short grid_cols;
    unsigned short grid_rows;
    unsigned short grid_dim_type;
    unsigned short grid_cols_dims[MAX_BOXES_PER_SCREEN];
    unsigned short grid_row_dims[MAX_BOXES_PER_SCREEN];

    keybind_func keybind;
    update_func update;

    //Boxes
    unsigned short num_boxes;
    unsigned short box_offset_x[MAX_BOXES_PER_SCREEN];
    unsigned short box_offset_y[MAX_BOXES_PER_SCREEN];
    unsigned short box_height[MAX_BOXES_PER_SCREEN];
    unsigned short box_width[MAX_BOXES_PER_SCREEN];
    unsigned short box_top_margin[MAX_BOXES_PER_SCREEN];
    unsigned short box_bottom_margin[MAX_BOXES_PER_SCREEN];
    unsigned short box_left_margin[MAX_BOXES_PER_SCREEN];
    unsigned short box_right_margin[MAX_BOXES_PER_SCREEN];
    char box_top_border[MAX_BOXES_PER_SCREEN];
    char box_bottom_border[MAX_BOXES_PER_SCREEN];
    char box_left_border[MAX_BOXES_PER_SCREEN];
    char box_right_border[MAX_BOXES_PER_SCREEN];
    keybind_func box_keybind[MAX_BOXES_PER_SCREEN];
    update_func box_update[MAX_BOXES_PER_SCREEN];

} Screen;

typedef struct {
    unsigned short cursor_x;
    unsigned short cursor_y;
    unsigned short cursor_max_x;
    unsigned short cursor_max_y;
    unsigned short cursor_min_x;
    unsigned short cursor_min_y;
} Cursor;
