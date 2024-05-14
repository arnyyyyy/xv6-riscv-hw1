#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "user/user.h"

#define NULL 0

#define FAIL(reason) \
  printf(reason), exit(1);

#define TRY(cmd) \
  if((cmd) < 0) \
    FAIL(#cmd " failed\n");

static char *tname;

void
create_file(char *p)
{
  int fd;
  TRY(fd = open(p, O_CREATE | O_TRUNC | O_WRONLY));
  TRY(write(fd, tname, strlen(tname)));
  close(fd);
}

void
check_link(char *p)
{
  static char buf[512];
  struct stat st;
  int n, fd;

  n = strlen(tname);
  TRY(fd = open(p, O_RDONLY));
  TRY(fstat(fd, &st));
  if(st.type != T_FILE)
    FAIL("expected file\n");
  TRY(read(fd, buf, n));
  if(memcmp(buf, tname, n) != 0)
    FAIL("memcmp != 0\n");
  if(read(fd, buf, 1) != 0)
    FAIL("read: not EOF\n");
  close(fd);
}

void
check_error(char *p)
{
  if((open(p, O_RDONLY | O_NOFOLLOW)) < 0)
    FAIL("expected link to exist\n");
  if((open(p, O_RDONLY)) > 0)
    FAIL("expected open to fail\n");
}

void
abs_simple(void)
{
  create_file("f");
  TRY(symlink("/abs_simple/f", "l"));
  check_link("l");
}

void
rel_to_same_lvl(void)
{
  create_file("f");
  TRY(symlink("f", "l"));
  check_link("l");
}

void
rel_to_higher_lvl(void)
{
  TRY(mkdir("a"));
  TRY(mkdir("a/b"));
  TRY(mkdir("a/b/c"));
  create_file("f");
  TRY(symlink("../../../f", "a/b/c/l"));
  check_link("a/b/c/l");
}

void
rel_to_lower_lvl(void)
{
  TRY(mkdir("a"));
  TRY(mkdir("a/b"));
  TRY(mkdir("a/b/c"));
  create_file("a/b/c/f");
  TRY(symlink("a/b/c/f", "l"));
  check_link("l");
}

void
abs_to_abs(void)
{
  create_file("f");
  TRY(symlink("/abs_to_abs/f", "l1"));
  TRY(symlink("/abs_to_abs/l1", "l2"));
  TRY(symlink("/abs_to_abs/l2", "l3"));
  check_link("l3");
}

void
abs_to_rel(void)
{
  create_file("f");
  TRY(symlink("f", "l1"));
  TRY(symlink("/abs_to_rel/l1", "l2"));
  check_link("l2");
}

void
rel_to_rel(void)
{
  create_file("f");
  TRY(symlink("f", "l1"));
  TRY(symlink("l1", "l2"));
  TRY(symlink("l2", "l3"));
  check_link("l3");
}

void
direct_self(void)
{
  TRY(symlink("l", "l"));
  check_error("l");
}

void
indirect_self(void)
{
  TRY(symlink("l2", "l1"));
  TRY(symlink("l3", "l2"));
  TRY(symlink("l1", "l3"));
  check_error("l1");
}

void
abs_to_none(void)
{
  TRY(symlink("/abs_to_none/f", "l"));
  check_error("l");
}

void
rel_to_none_same_lvl(void)
{
  TRY(mkdir("a"));
  TRY(mkdir("a/b"));
  TRY(mkdir("a/b/c"));
  create_file("f");
  TRY(symlink("f", "a/b/c/l"));
  check_error("a/b/c/l");
}

void
rel_to_none_higher_lvl(void)
{
  TRY(mkdir("a"));
  TRY(mkdir("a/b"));
  TRY(mkdir("a/b/c"));
  create_file("a/b/c/f");
  TRY(symlink("../../../f", "a/b/c/l"));
  check_error("a/b/c/l");
}

void
rel_to_none_lower_lvl(void)
{
  TRY(mkdir("a"));
  TRY(mkdir("a/b"));
  TRY(mkdir("a/b/c"));
  create_file("f");
  TRY(symlink("a/b/c/f", "l"));
  check_error("l");
}

struct test {
  void (*f)(void);
  char *s;
} tests[] = {
  {abs_simple, "abs_simple"},
  {rel_to_same_lvl, "rel_to_same_lvl"},
  {rel_to_higher_lvl, "rel_to_higher_lvl"},
  {rel_to_lower_lvl, "rel_to_lower_lvl"},
  {abs_to_abs, "abs_to_abs"},
  {abs_to_rel, "abs_to_rel"},
  {rel_to_rel, "rel_to_rel"},
  {direct_self, "direct_self"},
  {indirect_self, "indirect_self"},
  {abs_to_none, "abs_to_none"},
  {rel_to_none_same_lvl, "rel_to_none_same_lvl"},
  {rel_to_none_higher_lvl, "rel_to_none_higher_lvl"},
  {rel_to_none_lower_lvl, "rel_to_none_lower_lvl"},
  {NULL, NULL}
};

int
unlink_r(char *s)
{
  char buf[DIRSIZ+1];

  int fd, r;
  struct stat st;
  struct dirent de;

  if((fd = open(s, O_RDONLY | O_NOFOLLOW)) < 0)
    return -1;
  if(fstat(fd, &st) < 0)
    return -1;

  if(st.type == T_DIR){
    if(chdir(s) < 0)
      return -1;
    while((r = read(fd, &de, sizeof(de))) == sizeof(de)){
      memcpy(buf, de.name, DIRSIZ);
      buf[DIRSIZ] = '\0';
      if(strcmp(buf, ".") == 0 || strcmp(buf, "..") == 0)
        continue;
      if((r = unlink_r(buf)) < 0)
        break;
    }
    chdir("..");
    if(r != 0)
      return -1;
  }
  close(fd);
  if(unlink(s) < 0)
    return -1;
  return 0;
}

int
exec_ls(char *path)
{
  int pid, xstatus;
  pid = fork();
  if(pid < 0)
    return -1;
  if(pid == 0){
    char *argv[] = { "ls", path, NULL };
    return exec("ls", argv);
  } else{
    wait(&xstatus);
    return xstatus == 0 ? 0 : -1;
  }
}

int
ls_r_impl(char buf[])
{
  int n = strlen(buf);
  int fd, r;
  struct stat st;
  struct dirent de;

  if((fd = open(buf, O_RDONLY | O_NOFOLLOW)) < 0)
    return -1;
  if(fstat(fd, &st) < 0){
    close(fd);
    return -1;
  }
  printf("\nls %s:\n", buf);
  if(exec_ls(buf) != 0){
    close(fd);
    return -1;
  }

  if(st.type == T_DIR){
    if(n + 1 + DIRSIZ + 1 > MAXPATH) {
      close(fd);
      return -1;
    }
    while((r = read(fd, &de, sizeof(de))) == sizeof(de)){
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      buf[n] = '/';
      memcpy(buf + n + 1, de.name, DIRSIZ);
      buf[n + 1 + DIRSIZ] = '\0';
      r = ls_r_impl(buf);
      buf[n] = '\0';
      if(r != 0)
        break;
    }
    if(r != 0) {
      close(fd);
      return -1;
    }
  }
  close(fd);
  return 0;
}

int
ls_r(char *path)
{
  char buf[MAXPATH];
  strcpy(buf, path);
  return ls_r_impl(buf);
}

int
runtest(struct test *t)
{
  int pid;
  int xstatus;

  printf("test %s: ", t->s);

  unlink_r(t->s);
  if(mkdir(t->s) < 0){
    printf("runtest: mkdir error\n");
    exit(1);
  }

  if((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }

  if(pid == 0) {
    tname = t->s;
    chdir(t->s);
    t->f();
    chdir("..");
    exit(0);
  } else {
    wait(&xstatus);
    if(xstatus != 0) 
      printf("FAILED\n");
    else
      printf("OK\n");
    return xstatus != 0;
  }
}

int
main(void)
{
  chdir("/"); // ensure we are at root
  struct test *t;
  for(t = tests; t->s != NULL; ++t){
    if(runtest(t) != 0)
      exit(1);
  }
  printf("ALL TESTS PASSED\n");
  for(t = tests; t->s != NULL; ++t){
    ls_r(t->s);
    unlink_r(t->s);
  }
  exit(0);
}
