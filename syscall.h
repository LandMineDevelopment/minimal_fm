#ifndef SYSCALL_H
#define SYSCALL_H

// Syscall wrappers  all return long (success >=0, error <0)
// On error, caller should handle (e.g., set error_msg)

// 0 args
long syscall0(long n);

// 1 arg
long syscall1(long n, long a1);

// 2 args
long syscall2(long n, long a1, long a2);

// 3 args
long syscall3(long n, long a1, long a2, long a3);

// 4 args
long syscall4(long n, long a1, long a2, long a3, long a4);

// Write string (no null term needed if len given)
void write_str(const char *s, long len);

#endif
