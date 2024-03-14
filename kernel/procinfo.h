// redefining procstate so including "kernel/proc.h"
// and all its dependencies is not required
enum procinfo_state {
  PINFO_UNUSED,
  PINFO_USED, 
  PINFO_SLEEPING, 
  PINFO_RUNNABLE, 
  PINFO_RUNNING, 
  PINFO_ZOMBIE
};

struct procinfo {
  enum procinfo_state state;
  char name[16];
  int pid; // process id
  int parent_pid; // parent process id
};
