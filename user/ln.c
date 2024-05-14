#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int sym;
  const char *old, *new;

  if(argc < 3 || argc > 4)
    goto bad;

  old = argv[argc - 2];
  new = argv[argc - 1];
  if(*old == '-' || *new == '-')
    goto bad;

  if(argc == 4 && strcmp(argv[1], "-s") == 0)
    sym = 1;
  else if(argc == 3)
    sym = 0;
  else
    goto bad;
  
  if(sym){
    if(symlink(old, new) < 0)
      fprintf(2, "symlink %s %s: failed\n", old, new);
  } else{
    if(link(old, new) < 0)
      fprintf(2, "link %s %s: failed\n", old, new);
  }
  exit(0);

bad:
  fprintf(2, "Usage: ln [-s] old new\n");
  exit(1);
}
