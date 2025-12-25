#ifndef STRING_H
#define STRING_H

// String compare
// Params: a, b - Strings
// Returns: int - 0 if equal, >0 if a>b, <0 if a<b
// Pseudo-code logic:
//   Loop while *a == *b and !null, advance
//   Return *a - *b
int my_strcmp(const char *a, const char *b);

// String copy
// Params: dest - Destination
//         src - Source
// Returns: void
// Pseudo-code logic:
//   Loop copy *dest++ = *src++ until src null
//   *dest = 0
void my_strcpy(char *dest, const char *src);

// String length
// Params: s - String
// Returns: int - Length
// Pseudo-code logic:
//   Count until null
int my_strlen(const char *s);

// Build full path
// Params: dest - Buffer
//         base - Base path (e.g., cwd)
//         name - Name to append
// Returns: void
// Pseudo-code logic:
//   Copy base to dest
//   Append '/' if base not ends with '/'
//   Append name
//   Null terminate
void path_build(char *dest, const char *base, const char *name);

// Fuzzy match for search
// Params: pattern - Search pattern
//         str - String to match
// Returns: int - 1 if fuzzy match, 0 no
// Pseudo-code logic:
//   Case-insensitive: manually lowercase both pattern and str chars
//     (no <ctype.h> — use: if (c >= 'A' && c <= 'Z') c += 32;)
//   i=0 (pattern index), j=0 (str index)
//   while (str[j] != '\0'):
//       if (lowercase(str[j]) == lowercase(pattern[i])) i++
//       if (i == pattern length) return 1
//       j++
//   return 0
int fuzzy_match(const char *pattern, const char *str);

#endif
