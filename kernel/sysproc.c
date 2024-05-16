#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_pgaccess(void)
{
  uint64 addr;
  int n, va, accessed;
  pte_t *pte;
  struct proc *p = myproc();

  argaddr(0, &addr);
  argint(1, &n);

  if(n < 0 || addr + n >= MAXVA)
    return -1;

  accessed = 0;
  for(va = PGROUNDDOWN(addr); va < addr + n; va += PGSIZE){
    if((pte = walk(p->pagetable, va, 0)) == 0)
      return -1;
    if(*pte & PTE_A)
      accessed = 1;
    *pte &= ~PTE_A;
  }

  return accessed;
}

uint64
sys_vmprint(void)
{
  struct proc *p = myproc();
  vmprint(p->pagetable);
  return 0;
}
