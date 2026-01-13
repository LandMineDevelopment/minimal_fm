#include "string.h"

// String compare
// Returns: 0 if equal, <0 if a < b, >0 if a > b
int my_strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char *)a - *(const unsigned char *)b;
}

// String copy (dest must have room for src + null)
void my_strcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// String length (excludes null terminator)
int my_strlen(const char *s) {
    const char *start = s;
    while (*s) s++;
    return s - start;
}

// Build full path: base + "/" + name (skip double /)
void path_build(char *dest, const char *base, const char *name) {
    char *start = dest;
    // Copy base
    while (*base) {
        *dest++ = *base++;
    }

    // Append '/' if base doesn't end with one and name isn't empty
    if (dest != start &&
        *(dest - 1) != '/' &&
        *name) {
        *dest++ = '/';
    }

    // Append name
    while (*name) {
        *dest++ = *name++;
    }
    *dest = '\0';
}

// String concatenate
// result must have room for s1 + s2 + null
void str_concat(char *result, const char *s1, const char *s2) {
    // Copy s1
    while (*s1) {
        *result++ = *s1++;
    }
    // Copy s2
    while (*s2) {
        *result++ = *s2++;
    }
    *result = '\0';
}

// Fuzzy match (case-insensitive sequential, like Yazi)
// Returns 1 if pattern matches str in order (skips allowed in str)
int fuzzy_match(const char *pattern, const char *str) {
    if (!*pattern) return 1;  // empty pattern always matches

    while (*str) {
        // Manual lowercase for both chars
        char p = *pattern;
        char s = *str;
        if (p >= 'A' && p <= 'Z') p += 32;
        if (s >= 'A' && s <= 'Z') s += 32;

        if (p == s) {
            pattern++;
            if (!*pattern) return 1;  // all pattern chars matched
        }
        str++;
    }
    return 0;  // pattern not fully matched
}
