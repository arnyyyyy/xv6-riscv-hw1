#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define stderr 2
#define NULL 0
#define BUF_SIZE 64
#define DELM " \n\r"

int all_digits(const char *s) {
    while (*s != '\0') {
        if (*s < '0' || '9' < *s) {
            return 0;
        }
        s++;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    char buf[BUF_SIZE] = {};
    gets(buf, BUF_SIZE);

    if (buf[BUF_SIZE - 2] != '\0') {
        fprintf(stderr, "Buffer overflow\n");
        exit(1);
    }

    char *lhs_s = strtok(buf, DELM);
    char *rhs_s = strtok(NULL, DELM);
    if (lhs_s == NULL) {
        fprintf(stderr, "Invalid number of args: two numbers expected, but none received\n");
        exit(1);
    }
    if (rhs_s == NULL) {
        fprintf(stderr, "Invalid number of args: two numbers expected, but only one received\n");
        exit(1);
    }
    if (strtok(NULL, DELM) != NULL) {
        fprintf(stderr, "Invalid number of args: two numbers expected, but more received\n");
        exit(1);
    }
    if (!all_digits(lhs_s)) {
        fprintf(stderr, "Invalid args: unexpected symbols in the first argument\n");
        exit(1);
    }
    if (!all_digits(rhs_s)) {
        fprintf(stderr, "Invalid args: unexpected symbols in the second argument\n");
        exit(1);
    }

    int lhs = atoi(lhs_s);
    int rhs = atoi(rhs_s);

    int sum = add(lhs, rhs);
    if (sum < 0) {
        fprintf(stderr, "Addition overflow\n");
        exit(1);
    }

    printf("%d\n", sum);
    exit(0);
}
