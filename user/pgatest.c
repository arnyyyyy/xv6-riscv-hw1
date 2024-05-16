#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/stat.h"
#include "user/user.h"

#define stderr 2
#define assert(expr)                   \
  if(!(expr)){                         \
    fprintf(stderr, "assert: " #expr); \
    exit(1);                           \
  }
#define N 720

void
test_array(char *arr)
{
  // clearing the bit
  pgaccess(arr, N);

  assert(pgaccess(arr, N) == 0);

  arr[300] = 'a';

  assert(pgaccess(arr, N) == 1);
  assert(pgaccess(arr, N) == 0);

  arr[N - 1] = 'a';

  assert(pgaccess(arr, N) == 1);
  assert(pgaccess(arr, N) == 0);

  arr[0] = 'a';

  assert(pgaccess(arr, N) == 1);
  assert(pgaccess(arr, N) == 0);
}

void
test_stack(void)
{
  printf("stack test: ");

  char arr[N];
  test_array(arr);

  printf("OK\n");
}

void
test_heap(void)
{
  printf("heap test: ");

  char *arr = (char *)malloc(N);
  assert(arr);
  test_array(arr);
  free(arr);

  printf("OK\n");
}

int
main(void)
{
  test_stack();
  test_heap();
  printf("\n");
  vmprint();
}
