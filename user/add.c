#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

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

char* my_gets(char *buf, int max) {
    int i, cc;
    char c;

    for (i = 0; i + 1 < max;) {
        cc = read(0, &c, 1);
        if (cc < 1) {
            break;
        }
        buf[i++] = c;
        if (c == '\n' || c == '\r') {
            break;
        }
    }
    buf[i] = '\0';
    return buf;
}

int main(int argc, char *argv[]) {
    char buf[BUF_SIZE];
    my_gets(buf, BUF_SIZE);

    if (strchr(buf, '\n') == NULL) {
        printf("Invalid args: buffer overflow\n");
        exit(1);
    }

    char *lhs_s = strtok(buf, DELM);
    char *rhs_s = strtok(NULL, DELM);
    if (lhs_s == NULL) {
        printf("Invalid number of args: two numbers expected, but none received\n");
        exit(1);
    }
    if (rhs_s == NULL) {
        printf("Invalid number of args: two numbers expected, but only one received\n");
        exit(1);
    }
    if (strtok(NULL, DELM) != NULL) {
        printf("Invalid number of args: two numbers expected, but more received\n");
        exit(1);
    }
    if (!all_digits(lhs_s)) {
        printf("Invalid args: unexpected symbols in the first argument\n");
        exit(1);
    }
    if (!all_digits(rhs_s)) {
        printf("Invalid args: unexpected symbols in the second argument\n");
        exit(1);
    }

    int lhs = atoi(lhs_s);
    int rhs = atoi(rhs_s);

    int sum = lhs + rhs;
    if (sum < 0) {
        printf("Addition overflow\n");
        exit(1);
    }

    printf("%d\n", sum);
    exit(0);
}
