#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char ** argv)
{
  pid_t pid = fork ();
  int status;
  
  if (pid == 0)
    {
      /* Child Process */
      execv("/bin/ls", argv);
    }
  else
    {
      /* Parent Process */
      waitpid (pid, &status, 0);
      printf ("pid:%d done, exit status: %d\n", pid,  WEXITSTATUS(status));
    }
  return 0;
}
      
