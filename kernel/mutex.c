#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "mutex.h"
#include "proc.h"

struct {
  struct spinlock lock;
  struct mutex mutex[NMUTEX];
} mtable;

void
mutexinit(void)
{
  struct mutex *m;

  initlock(&mtable.lock, "mtable");
  for(m = mtable.mutex; m < mtable.mutex + NMUTEX; ++m){
    initsleeplock(&m->lock, "mutex");
    m->ref = 0;
  }
}

struct mutex*
mutexalloc(void)
{
  struct mutex *m;

  acquire(&mtable.lock);
  for(m = mtable.mutex; m < mtable.mutex + NMUTEX; ++m){
    if(m->ref == 0){
      m->ref = 1;
      release(&mtable.lock);
      return m;
    }
  }
  release(&mtable.lock);
  return 0;
}

void
mutexclose(struct mutex *m)
{
  if(m->ref < 1)
    panic("mutexclose");
  m->ref--;
  if(holdingsleep(&m->lock))
    releasesleep(&m->lock);
}

struct mutex*
mutexdup(struct mutex *m)
{
  if(m->ref < 1)
    panic("mutexdup");
  m->ref++;
  return m;
}

void
mutexlock(struct mutex *m)
{
  if(m->ref < 1)
    panic("mutexlock");
  acquiresleep(&m->lock);
}

void
mutexunlock(struct mutex *m)
{
  if(m->ref < 1)
    panic("mutexunlock");
  releasesleep(&m->lock);
}
