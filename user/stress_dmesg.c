#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/riscv.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NULL 0

void exec_echo(void) {
    int pid = fork();    
    if (pid > 0) { // parent
        wait(NULL);
    } else if (pid == 0) {
        // ignore IO
        close(0);
        close(1);
        close(2);

        char *args[1] = { "echo" };
        exec("/echo", args); // ignore errors
    } else {
        // ignore
    }
}

int main(void) {
    for (int i = 0; i < 1024; ++i) {
        exec_echo();
    }

    char *args[1] = { "dmesg" };
    exit(exec("/dmesg", args));
}
