#ifndef SCREEN_H
#define SCREEN_H

// Low-level buffer operations
void set_screen_char(int row, int col, char ch);
void clear_screen_buffer(void);

// Header and layout
void fill_header(void);
int get_left_width(void);
int get_middle_width(void);
int get_right_width(void);

// Pane fillers
// Left pane: parent directory listing
// Pseudo-code: Iterate parent_entries, print name + indicators (e.g., '/' if dir, 'e' if FLAG_EMPTY_DIR)
void fill_left_pane(void);

// Middle pane: current directory (or filtered in search mode)
// Pseudo-code:
//   Determine which list to use:
//     if (app_state.in_modal_input && app_state.modal_context == CTX_SEARCH)
//         use filtered_entries + num_filtered
//     else
//         use current_entries + num_current
//
//   For each entry in the list (up to visible rows):
//       char marker = ' ';
//       if (i == app_state.cursor_loc) marker = '>';  // Cursor
//       if (entry.flags & FLAG_SELECTED) marker = '*';  // Selected (overrides cursor if both)
//
//       Print line:
//         - marker
//         - name
//         - '/' if DT_DIR
//         - 'e' if DT_DIR and (flags & FLAG_EMPTY_DIR)
//         - '.' + extension if DT_REG and extension[0] != '\0'
//       Truncate to pane width
//       Handle alignment/padding
void fill_middle_pane(void);

// Right pane: dual-purpose preview
// Pseudo-code:
//   // Always start clean to avoid stale data from previous selection
//   app_state.num_preview = 0;
//   app_state.preview_mode = PREVIEW_NONE;
//
//   Get selected entry (filtered if search mode, else current)
//   If no selection → show "No selection"
//   Else build full_path = cwd + "/" + selected.name
//       If DT_DIR:
//           load_dir_into(full_path, preview_entries, &num_preview)
//           preview_mode = PREVIEW_DIR_CONTENTS
//           Render like middle pane (no selection support)
//       Else if DT_REG:
//           Try to open and read first MAX_PREVIEW_LINES lines
//           If text-like → display lines, preview_mode = PREVIEW_FILE_TEXT
//           Else → show "Binary file" or basic info
void fill_right_pane(void);

// Assemble everything
void build_screen(void);

// Output to terminal
void draw_screen(void);

// Full refresh (resize detect + reload + redraw)
void refresh_and_redraw(void);

// Utility
int count_selected(void);  // Counts FLAG_SELECTED in current view

#endif
