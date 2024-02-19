#include "types.h"
#include "riscv.h"
#include "defs.h"

// Syscall function code
uint64 sys_add(void) {
    int n, m;
    argint(0, &n);
    argint(1, &m);
    return n + m;
}