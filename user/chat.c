#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NULL 0
#define stderr 2

#define FAIL(reason)              \
    fprintf(stderr, reason "\n"); \
    exit(1);

#define TRY(cmd)               \
    if ((cmd) < 0) {           \
        FAIL(#cmd " failed");  \
    }

int main(int argc, char *argv[]) {
    if (argc != 2) {
        FAIL("expected an argument");
    }

    int p1[2], p2[2];
    TRY(pipe(p1));
    TRY(pipe(p2));

    int pid = fork();
    if (pid > 0) { // parent
        close(p1[0]); // close read end
        close(p2[1]); // close write end
        int ppid = getpid();

        TRY(write(p1[1], argv[1], strlen(argv[1])));

        close(p1[1]); // close write end and cause the child to finish

        for (;;) {
            char c;
            int r = read(p2[0], &c, 1);
            if (r < 0) {
                FAIL("parent read failed");
            } else if (r == 0) {
                break;
            }
            printf("%d: received \'%c\'\n", ppid, c);
        }

        close(p2[0]); // close read end

        wait(NULL);
        exit(0);
    } else if (pid == 0) { // child
        close(p1[1]); // close write end
        close(p2[0]); // close read end
        int cpid = getpid();

        for (;;) {
            char c;
            int r = read(p1[0], &c, 1);
            if (r < 0) {
                FAIL("child read failed");
            } else if (r == 0) {
                break;
            }
            printf("%d: received \'%c\'\n", cpid, c);
            TRY(write(p2[1], &c, 1));
        }

        close(p2[0]); // close read end
        close(p2[1]); // close write end
        
        exit(0);
    } else {
        FAIL("fork() failed");
    }
}
