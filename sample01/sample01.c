#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <poll.h>
#include <sys/signalfd.h>

#define TIMEOUT (5 * 1000)	/* TimeOut = 5sec */

int main (int argc, char ** argv)
{
  pid_t pid = fork ();
  int status;
  sigset_t mask;
  int sfd;

  sigemptyset (&mask);
  sigaddset (&mask, SIGCHLD);

  sigprocmask(SIG_BLOCK, &mask, NULL);
  sfd = signalfd(-1, &mask, 0);
  if (sfd < 0)
    {
      perror ("signalfd");
      return -1;
    }

  if (pid == 0)
    {
      /* Child Process */
      execv("/bin/sleep", argv);
    }
  else
    {
      struct pollfd pollfd;
      int pidfd, ready;
      struct signalfd_siginfo fdsi;
      ssize_t s;

      printf ("Created process pid:%d\n", pid);
      
      pollfd.fd = sfd;
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
	  s = read(sfd, &fdsi, sizeof(fdsi));
	  if (s != sizeof(fdsi))
	    {
	      perror ("read");
	      return -1;
	    }
	  printf ("Catch signal:%d, then do waitpid()\n", fdsi.ssi_signo);
	}
      /* Parent Process */
      pid_t dpid = waitpid (pid, &status, 0);
      printf ("pid:%d done, exit status: %d\n", dpid,  WEXITSTATUS(status));
    }
  return 0;
}
       
