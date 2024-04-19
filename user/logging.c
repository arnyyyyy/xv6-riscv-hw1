#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/riscv.h"
#include "kernel/stat.h"
#include "kernel/logging.h"
#include "user/user.h"

#define stderr 2

char buf[DMESGBUF * PGSIZE + 1];

enum { SUB_ENABLE, SUB_DISABLE, SUB_SHOW, SUB_LIST, SUB_PRINT };

int parse_subcommand(const char *arg);
int parse_class(const char *arg);
void exec_dmesg(void) __attribute__((noreturn));

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "subcommand expected\n");
        exit(1);
    }

    int subcommand = parse_subcommand(argv[1]);
    int class = -1;

    switch (subcommand) {
    case SUB_ENABLE:
    case SUB_DISABLE:
    case SUB_SHOW:
        if (argc < 3) {
            fprintf(stderr, "logging class expected\n");
            exit(1);
        }
        class = parse_class(argv[2]);
        if (class < 0) {
            fprintf(stderr, "unknown logging class\n");
            exit(1);
        }
    default:
        break;
    }

    int enabled;
    switch (subcommand) {
    case SUB_ENABLE:
        ltoggle(class, 1);
        exit(0);
    case SUB_DISABLE:
        ltoggle(class, 0);
        exit(0);
    case SUB_SHOW:
        enabled = lenabled(class);
        printf("%d\n", enabled);
        exit(0);
    case SUB_LIST:
        enabled = lenabled(LOG_EXEC);
        printf("exec      %d\n", enabled);
        enabled = lenabled(LOG_SYSCALL);
        printf("syscall   %d\n", enabled);
        enabled = lenabled(LOG_INTERRUPT);
        printf("interrupt %d\n", enabled);
        enabled = lenabled(LOG_SWITCH);
        printf("switch    %d\n", enabled);
        exit(0);
    case SUB_PRINT:
        exec_dmesg();
    default:
        fprintf(stderr, "unknown subcommand\n");
        exit(1);
    }
}

int parse_subcommand(const char *arg) {
    if (strcmp(arg, "enable") == 0)
        return SUB_ENABLE;
    else if (strcmp(arg, "disable") == 0)
        return SUB_DISABLE;
    else if (strcmp(arg, "show") == 0)
        return SUB_SHOW;
    else if (strcmp(arg, "list") == 0)
        return SUB_LIST;
    else if (strcmp(arg, "print") == 0)
        return SUB_PRINT;
    return -1;
}

int parse_class(const char *arg) {
    if (strcmp(arg, "exec") == 0)
        return LOG_EXEC;
    else if (strcmp(arg, "syscall") == 0)
        return LOG_SYSCALL;
    else if (strcmp(arg, "interrupt") == 0)
        return LOG_INTERRUPT;
    else if (strcmp(arg, "switch") == 0)
        return LOG_SWITCH;
    return -1;
}

void exec_dmesg(void) {
    char *args[1] = { "dmesg" };
    exit(exec("/dmesg", args));
}
