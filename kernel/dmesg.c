#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define BUFSIZE (DMESGBUF * PGSIZE)

static struct {
  char buf[BUFSIZE];
  uint64 head, tail;
  struct spinlock lock; 
} dbuf;

void
dinit()
{
  dbuf.head = dbuf.tail = 0;
  initlock(&dbuf.lock, "dbuf");
}

// dbuf.lock must be held
static void
dputc(char c)
{
  if (dbuf.tail - dbuf.head == BUFSIZE)
    dbuf.head++;
  dbuf.buf[dbuf.tail % BUFSIZE] = c;
  dbuf.tail++;
}

static char digits[] = "0123456789abcdef";

static void
pr_int(int xx, int base, int sign)
{
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    dputc(buf[i]);
}

static void
pr_ptr(uint64 x)
{
  int i;
  dputc('0');
  dputc('x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    dputc(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the console. only understands %d, %x, %p, %s.
void
pr_msg(const char *fmt, ...)
{
  va_list ap;
  int i, c;
  char *s;

  if (fmt == 0)
    panic("null fmt");

  acquire(&dbuf.lock);

  // acquire(&tickslock);
  dputc('[');
  pr_int(ticks, 10, 1);
  dputc(']');
  // release(&tickslock);

  dputc(' ');

  va_start(ap, fmt);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      dputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      pr_int(va_arg(ap, int), 10, 1);
      break;
    case 'x':
      pr_int(va_arg(ap, int), 16, 1);
      break;
    case 'p':
      pr_ptr(va_arg(ap, uint64));
      break;
    case 's':
      if((s = va_arg(ap, char*)) == 0)
        s = "(null)";
      for(; *s; s++)
        dputc(*s);
      break;
    case '%':
      dputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      dputc('%');
      dputc(c);
      break;
    }
  }
  va_end(ap);

  dputc('\n');

  release(&dbuf.lock);
}

// addr is a userspace address to a buffer of size at least BUFSIZE + 1
int
dmesg(uint64 addr)
{
  struct proc *p = myproc();

  if (addr == 0)
    return -1;

  acquire(&dbuf.lock);

  int align = dbuf.head / BUFSIZE * BUFSIZE;
  int head = dbuf.head - align;
  int tail = dbuf.tail - align;
  if (tail <= BUFSIZE) {
    if (copyout(p->pagetable, addr, dbuf.buf + head, tail - head) != 0) {
      release(&dbuf.lock);
      return -1;
    }
  } else {
    if (copyout(p->pagetable, addr, dbuf.buf + head, BUFSIZE - head) != 0) {
      release(&dbuf.lock);
      return -1;
    }
    if (copyout(p->pagetable, addr + (BUFSIZE - head), dbuf.buf, tail - BUFSIZE) != 0) {
      release(&dbuf.lock);
      return -1;
    }
  }

  char null = '\0';
  if (copyout(p->pagetable, addr + (tail - head), &null, 1) != 0) {
    release(&dbuf.lock);
    return -1;
  }
  
  release(&dbuf.lock);
  return 0;
}
