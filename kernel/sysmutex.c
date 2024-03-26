#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"
#include "mutex.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int
argmd(int n, int *pmd, struct mutex **pm)
{
  int md;
  struct mutex *m;

  argint(n, &md);
  if(md < 0 || md >= NOMUTEX || (m=myproc()->mutex[md]) == 0)
    return -1;
  if(pmd)
    *pmd = md;
  if(pm)
    *pm = m;
  return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
mdalloc(struct mutex *m)
{
  int md;
  struct proc *p = myproc();

  for(md = 0; md < NOMUTEX; md++){
    if(p->mutex[md] == 0){
      p->mutex[md] = m;
      return md;
    }
  }
  return -1;
}

uint64
sys_mclose(void)
{
  int md;
  struct mutex *m;

  if(argmd(0, &md, &m) < 0)
    return -1;
  myproc()->mutex[md] = 0;
  mutexclose(m);
  return 0;
}

uint64
sys_mcreate(void)
{
  int md;
  struct mutex *m;

  if((m=mutexalloc()) == 0)
    return -1;
  if((md=mdalloc(m)) < 0) {
    mutexclose(m);
    return -1;
  }
  return md;
}

uint64
sys_mlock(void)
{
  int md;
  struct mutex *m;

  if(argmd(0, &md, &m) < 0)
    return -1;
  mutexlock(m);
  return 0;
}

uint64
sys_munlock(void)
{
  int md;
  struct mutex *m;

  if(argmd(0, &md, &m) < 0)
    return -1;
  mutexunlock(m);
  return 0;
}
