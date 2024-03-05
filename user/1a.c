#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

#define stderr 2

int main(void) {
    int pid = fork();
    if (pid > 0) { // parent
        int ppid = getpid();
        printf("parent pid: %d\n", ppid);
        printf("child pid: %d\n", pid);

        int status;
        int cpid = wait(&status);
        printf("child with pid %d exited with status %d\n", cpid, status);
        exit(0);
    } else if (pid == 0) { // child
        sleep(50);
        exit(1);
    } else { // parent
        fprintf(stderr, "fork failed");
        exit(1);
    }
}