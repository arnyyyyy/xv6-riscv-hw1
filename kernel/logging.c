#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "logging.h"

static int enabled[] =  {
  [LOG_EXEC] = 0,
  [LOG_SYSCALL] = 0,
  [LOG_INTERRUPT] = 0,
  [LOG_SWITCH] = 0
};
static struct spinlock lock;

void logging_init(void) {
  initlock(&lock, "logging_lock");
}

int logging_enable(int class) {
  if (class < 0 || class >= NELEM(enabled))
    return -1;
  acquire(&lock);
  enabled[class] = 1;
  release(&lock);
  return 0;
}

int logging_disable(int class) {
  if (class < 0 || class >= NELEM(enabled))
    return -1;
  acquire(&lock);
  enabled[class] = 0;
  release(&lock);
  return 0;
}

int logging_enabled(int class) {
  if (class < 0 || class >= NELEM(enabled))
    return -1;
  acquire(&lock);
  int e = enabled[class];
  release(&lock);
  return e;
}
