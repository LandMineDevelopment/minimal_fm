// ARM64 syscall numbers (hardcoded for no dependencies)
#define SYS_READ 63
#define SYS_WRITE 64
#define SYS_EXIT 93
#define SYS_IOCTL 29
#define SYS_OPEN 56  // openat, since open is deprecated
#define SYS_CLOSE 57
#define SYS_GETDENTS64 61
#define SYS_CHDIR 49
#define SYS_GETCWD 17
#define SYS_FORK 220
#define SYS_EXECVE 221
#define SYS_WAIT4 260

// Function declarations
long syscall0(long n);
long syscall1(long n, long a1);
long syscall2(long n, long a1, long a2);
long syscall3(long n, long a1, long a2, long a3);
long syscall4(long n, long a1, long a2, long a3, long a4);
void write_str(const char *s, long len);

// syscall0: no arguments
long syscall0(long n) {
    long ret;
    asm volatile (
        "mov x8, %1\n\t"
        "svc 0\n\t"
        "mov %0, x0"
        : "=r" (ret)
        : "r" (n)
        : "x0", "x8", "memory"
    );
    return ret;
}

// syscall1: 1 arg
long syscall1(long n, long a1) {
    long ret;
    asm volatile (
        "mov x8, %1\n\t"
        "mov x0, %2\n\t"
        "svc 0\n\t"
        "mov %0, x0"
        : "=r" (ret)
        : "r" (n), "r" (a1)
        : "x0", "x8", "memory"
    );
    return ret;
}

// syscall2: 2 args
long syscall2(long n, long a1, long a2) {
    long ret;
    asm volatile (
        "mov x8, %1\n\t"
        "mov x0, %2\n\t"
        "mov x1, %3\n\t"
        "svc 0\n\t"
        "mov %0, x0"
        : "=r" (ret)
        : "r" (n), "r" (a1), "r" (a2)
        : "x0", "x1", "x8", "memory"
    );
    return ret;
}

// syscall3: 3 args
long syscall3(long n, long a1, long a2, long a3) {
    long ret;
    asm volatile (
        "mov x8, %1\n\t"
        "mov x0, %2\n\t"
        "mov x1, %3\n\t"
        "mov x2, %4\n\t"
        "svc 0\n\t"
        "mov %0, x0"
        : "=r" (ret)
        : "r" (n), "r" (a1), "r" (a2), "r" (a3)
        : "x0", "x1", "x2", "x8", "memory"
    );
    return ret;
}

// syscall4: 4 args
long syscall4(long n, long a1, long a2, long a3, long a4) {
    long ret;
    asm volatile (
        "mov x8, %1\n\t"
        "mov x0, %2\n\t"
        "mov x1, %3\n\t"
        "mov x2, %4\n\t"
        "mov x3, %5\n\t"
        "svc 0\n\t"
        "mov %0, x0"
        : "=r" (ret)
        : "r" (n), "r" (a1), "r" (a2), "r" (a3), "r" (a4)
        : "x0", "x1", "x2", "x3", "x8", "memory"
    );
    return ret;
}

// Write string (no null term needed)
void write_str(const char *s, long len) {
    syscall3(SYS_WRITE, 1, (long)s, len);
}