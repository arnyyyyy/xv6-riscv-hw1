#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"

uint64 sys_dmesg(void) {
  uint64 p;
  argaddr(0, &p);
  return dmesg(p);
}
