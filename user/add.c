#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
    printf("%d", add(atoi(argv[1]), atoi(argv[2]))); // do syscall
    exit(0);
}