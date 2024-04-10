#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/riscv.h"
#include "kernel/stat.h"
#include "user/user.h"

char buf[DMESGBUF * PGSIZE + 1];

int main(void) {
    dmesg(buf);
    printf("%s", buf);
    exit(0);
}
