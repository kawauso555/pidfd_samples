#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <poll.h>


#ifndef __NR_pidfd_open
#define __NR_pidfd_open 434   /* System call # on most architectures */
#endif

static int
pidfd_open(pid_t pid, unsigned int flags)
{
  return syscall(__NR_pidfd_open, pid, flags);
}

#define TIMEOUT (5 * 1000)	/* TimeOut = 5sec */

int main (int argc, char ** argv)
{
  pid_t pid = fork ();
  int status;
  sigset_t mask;
  int pfd;

  if (pid == 0)
    {
      /* Child Process */
      execv("/bin/sleep", argv);
    }
  else
    {
      struct pollfd pollfd;
      int pidfd, ready;

      printf ("Created process pid:%d\n", pid);
      
      pfd = pidfd_open(pid, 0);
      if (pfd == -1)
	{
	  perror("pidfd_open");
	  return -1;
	}

      pollfd.fd = pfd;
      pollfd.events = POLLIN;

      ready = poll (&pollfd, 1, TIMEOUT);
      if (ready < 0)
	{
	  perror("poll");
	  return -1;
	}
      else if (ready == 0)
	{
	  printf ("pid:%d Timeout\n",pid);
	}
      else
	{
	  printf ("process: %d done\n", pid);
	}
      /* Parent Process */
      pid_t dpid = waitpid (pid, &status, 0);
      printf ("pid:%d done, exit status: %d\n", dpid,  WEXITSTATUS(status));
      close (pfd);
    }
  return 0;
}
