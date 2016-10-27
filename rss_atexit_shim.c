#define _GNU_SOURCE
#include <dlfcn.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

extern char *program_invocation_name;
extern char *program_invocation_short_name;

static void (*real_exit)(int status) = NULL;

static char cmdline_buf[1024];

void log_my_rss() {
  struct rusage usage;
  int fd = 0;
  int cmd_fd = 0;
  int cmd_size = 0;
  getrusage(RUSAGE_SELF, &usage);

  cmd_fd = open("/proc/self/cmdline", O_RDONLY);
  cmd_size = read(cmd_fd, cmdline_buf, 1022);
  close(cmd_fd);
  
  fd = open("/tmp/rss_log", O_CREAT | O_APPEND | O_WRONLY, 0644);
  dprintf(fd, "%s ru_maxrss: %ld argv: ", program_invocation_short_name, usage.ru_maxrss);
  write(fd, cmdline_buf, cmd_size);
  dprintf(fd, "\n");
  close(fd);
}

void exit(int status) {
  /* first time through, find the real exit() */
  if (NULL == real_exit) {
    real_exit = dlsym(RTLD_NEXT, "exit");
  }

  /* perform logging operation */
  log_my_rss();

  /* call the real function */
  real_exit(status);
  /* not reached */
  abort();
}
