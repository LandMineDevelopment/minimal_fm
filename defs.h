// Syscall numbers for x86_64 Linux
#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_EXIT 60
#define SYS_IOCTL 16
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_GETDENTS64 217
#define SYS_CHDIR 80
#define SYS_GETCWD 79
#define SYS_FORK 57
#define SYS_EXECVE 59
#define SYS_WAIT4 61
#define SYS_MKDIR 83
#define SYS_CREAT 85
#define SYS_UNLINK 87
#define SYS_RMDIR 84
#define SYS_RENAME 82

// IOCTL requests
#define TCGETS 0x5401
#define TCSETS 0x5402
#define TIOCGWINSZ 0x5413

// Open flags
#define O_RDONLY 0
#define O_WRONLY 1
#define O_CREAT 0100
#define O_TRUNC 01000
#define O_DIRECTORY 0200000

// Dirent types
#define DT_UNKNOWN 0
#define DT_DIR 4
#define DT_REG 8
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
} Action;

#define SYS_nanosleep 35
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

// Unified entry structure
typedef struct {
    char name[MAX_NAME_LEN];
    unsigned char type;
    unsigned char flags;
    char extension[MAX_EXT_LEN];
} Entry;

// Terminal structures
struct termios {
    unsigned int c_iflag;
    unsigned int c_oflag;
    unsigned int c_cflag;
    unsigned int c_lflag;
    unsigned char c_line;
    unsigned char c_cc[19];
};

struct winsize {
    unsigned short ws_row;
    unsigned short ws_col;
    unsigned short ws_xpixel;
    unsigned short ws_ypixel;
};

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
    Screen *screen;
    unsigned short cursor_loc;
} NavScreen;

typedef struct {
    struct termios oldt;
    struct termios newt;

    Screen screens[NUM_SCREENS];
    const char *editor_cmd;

    char clipboard_paths[MAX_CLIPBOARD][MAX_PATH];
    unsigned char clipboard_types[MAX_CLIPBOARD];
    int num_clipboard;
    int is_cut;

    // Input state
    int in_modal_input;
    unsigned char modal_context;  // Changed from uint8_t

    char error_msg[MAX_ERROR_LEN];

    char input_buffer[MAX_NAME_LEN];
    int input_len;

    int delete_multi;
    int delete_recursive;
} AppState;


// Globals
extern AppState app_state;
extern const char *editor_paths[];
