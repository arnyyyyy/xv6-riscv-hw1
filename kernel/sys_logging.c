#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"

uint64 sys_ltoggle(void) {
  int class, enable;
  argint(0, &class);
  argint(1, &enable);

  if(enable)
    return logging_enable(class);
  else
    return logging_disable(class);
}

uint64 sys_lenabled(void) {
  int class;
  argint(0, &class);
  return logging_enabled(class);
}
