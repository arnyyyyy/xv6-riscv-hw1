#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define stderr 2

uint64 gen_next(uint64 value) {
    uint64 x = 1103515245;
    uint64 y = 12345;
    return value * x + y;
}

void print_usage(int fd) {
    fprintf(fd, "Usage: bigfile size_blocks first_value\n");
    fprintf(fd, "       size_blocks is in range 0..=MAXFILE\n");
}

uint8 expected[BSIZE], actual[BSIZE];

int main(int argc, char *argv[]) {
    // args
    int sz;
    uint64 init;
    
    int fd, r, i, j;
    uint64 value;
    char junk;
    
    if (argc != 3) {
        print_usage(stderr);
        exit(1);
    }
    sz = atoi(argv[1]);
    init = atoi(argv[2]);
    if (sz < 0 || sz > MAXFILE){
        fprintf(stderr, "size_blocks is out of range\n");
        print_usage(stderr);
        exit(1);
    }

    fd = open("largefile.dat", O_CREATE | O_RDWR | O_TRUNC);
    if (fd < 0) {
        fprintf(stderr, "cannot create largefile.dat\n");
        exit(1);
    }
    value = init;
    for (i = 0; i < sz; i++) {
        for (j = 0; j < BSIZE; j += sizeof(uint64)) {
            *(uint64 *)(expected + j) = value;
            value = gen_next(value);
        }
        if ((r = write(fd, &expected, BSIZE)) != BSIZE) {
            fprintf(stderr, "write of block %d failed\n", i);
            exit(1);
        }
    }
    close(fd);

    fd = open("largefile.dat", 0);
    if (fd < 0) {
        fprintf(stderr, "cannot open largefile.dat\n");
        exit(1);
    }
    value = init;
    for (i = 0; i < sz; i++) {
        for (j = 0; j < BSIZE; j += sizeof(uint64)) {
            *(uint64 *)(expected + j) = value;
            value = gen_next(value);
        }
        if ((r = read(fd, &actual, BSIZE)) != BSIZE) {
            fprintf(stderr, "read of block %i failed\n", i);
            exit(1);
        }
        if (memcmp(actual, expected, BSIZE) != 0) {
            fprintf(stderr, "block %d does not match\n", i);
            exit(1);
        }
    }

    r = read(fd, &junk, 1);
    if (r < 0) {
        fprintf(stderr, "EOF read failed\n");
        exit(1);
    }
    if (r > 0) {
        fprintf(stderr, "expected EOF but read a byte\n");
        exit(1);
    }

    printf("OK\n");
    
    close(fd);
    unlink("bigfile.dat");
    exit(0);
}
