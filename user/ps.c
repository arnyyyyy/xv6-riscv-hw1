#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "user/user.h"

#define NULL 0
#define stderr 2
#define assert(cond)                                      \
    if (!(cond)) {                                        \
        fprintf(stderr, "assertion failed: " #cond "\n"); \
        exit(1);                                          \
    }


static char *state_names[] = {
    [PINFO_USED]      "used  ",
    [PINFO_SLEEPING]  "sleep ",
    [PINFO_RUNNABLE]  "runble",
    [PINFO_RUNNING]   "run   ",
    [PINFO_ZOMBIE]    "zombie"
};

int main(void) {
    struct procinfo *plist;
    int lim = 1;
    int n;

    for(;;) {
        plist = malloc(sizeof(struct procinfo) * lim);
        assert(plist);
        n = ps_listinfo(plist, lim);
        if (n <= lim)
            break;
        free(plist);
        lim *= 2;
    }

    printf("pid ppid status name\n");
    struct procinfo *info;
    for (info = plist; info < &plist[n]; ++info) {
        printf("%d   %d    %s %s\n",
               info->pid,
               info->parent_pid,
               state_names[info->state],
               info->name);
    }

    exit(0);
}
