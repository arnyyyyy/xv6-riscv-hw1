#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

#define NULL 0
#define BUF_SIZE 128
#define stderr 2

#define FAIL(cmd_name) \
    fprintf(stderr, cmd_name " failed\n"); \
    exit(1);

#define TRY(cmd) \
    if ((cmd) < 0) { \
        FAIL(#cmd); \
    }

int main(int argc, char **argv) {
    int p[2];
    TRY(pipe(p));

    int pid = fork();
    if (pid > 0) { // parent
        TRY(close(p[0])); // close read end

        for (int i = 0; i < argc; ++i) {
            TRY(write(p[1], argv[i], strlen(argv[i])));
            char nline = '\n';
            TRY(write(p[1], &nline, 1));
        }
        close(p[1]);

        wait(NULL);
        exit(0);
    } else if (pid == 0) { // child
        TRY(close(p[1])); // close write end

        // replace stdin with the read end
        TRY(close(0));
        TRY(dup(p[0]));
        TRY(close(p[0]));

        char *argv[] = {"/wc", NULL};
        TRY(exec("/wc", argv));

        exit(0);
    } else { // parent
        FAIL("fork()");
    }
}