#include "defs.h"
#include "syscall.h"

// syscall0: no arguments
long syscall0(long n) {
    long ret;
    asm volatile (
        "syscall"
        : "=a" (ret)
        : "a" (n)
        : "rcx", "r11", "memory"
    );
    return ret;
}

// syscall1: 1 arg
long syscall1(long n, long a1) {
    long ret;
    asm volatile (
        "syscall"
        : "=a" (ret)
        : "a" (n), "D" (a1)
        : "rcx", "r11", "memory"
    );
    return ret;
}

// syscall2: 2 args
long syscall2(long n, long a1, long a2) {
    long ret;
    asm volatile (
        "syscall"
        : "=a" (ret)
        : "a" (n), "D" (a1), "S" (a2)
        : "rcx", "r11", "memory"
    );
    return ret;
}

// syscall3: 3 args
long syscall3(long n, long a1, long a2, long a3) {
    long ret;
    asm volatile (
        "syscall"
        : "=a" (ret)
        : "a" (n), "D" (a1), "S" (a2), "d" (a3)
        : "rcx", "r11", "memory"
    );
    return ret;
}

// syscall4: 4 args
long syscall4(long n, long a1, long a2, long a3, long a4) {
    long ret;
    asm volatile (
        "mov %5, %%r10\n\t"
        "syscall"
        : "=a" (ret)
        : "a" (n), "D" (a1), "S" (a2), "d" (a3), "r" (a4)
        : "rcx", "r11", "memory"
    );
    return ret;
}

// Write string (no null term needed)
void write_str(const char *s, long len) {
    syscall3(SYS_WRITE, 1, (long)s, len);
}
