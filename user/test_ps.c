#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/procinfo.h"
#include "user/user.h"

#define NULL 0
#define stderr 2
#define assert(cond)                                      \
    if (!(cond)) {                                        \
        fprintf(stderr, "assertion failed: " #cond "\n"); \
        exit(1);                                          \
    }

// we know that there are at least two processes: `init` and testing utility itself
void test_small_buf(void) {
    struct procinfo plist[1];
    
    // we know that there are at least two processes: `init` and testing utility itself
    assert(ps_listinfo(plist, 1) >= 2);
}

void test_bad_addr(void) {
    assert(ps_listinfo((struct procinfo *)10000000000, 5) == -1);
}

void test_null(void) {
    assert(ps_listinfo(NULL, -123) > 0);
}

void test_ok(void) {
    struct procinfo plist[NPROC];
    int n = ps_listinfo(plist, NPROC);
    assert(0 < n && n <= NPROC);
}

#define RUN_TEST(name)                      \
    printf("running test \"" #name "\"\n"); \
    test_##name();                          \
    printf("...ok\n");


int main(int argc, char *argv[]) {
    RUN_TEST(ok);
    RUN_TEST(small_buf);
    RUN_TEST(null);
    RUN_TEST(bad_addr);
    exit(0);
}
