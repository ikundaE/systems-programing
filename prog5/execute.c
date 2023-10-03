/* execute.c - code used by small shell to execute commands */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include "smsh.h"
int numargs (char ** arglist);
int nomore();

int execute(char *argv[])
/*
 * purpose: run a program passing it arguments
 * returns: status returned via wait, or -1 on error
 *  errors: -1 on fork() or wait() errors
 */
{
  int   pid=0 ;
  int   child_info = -1;
  int   bcground = 0;
  int   argcount=0;
  int   childsleep ;

  argcount = numargs (argv);//first get num of arguments for sleep
  if ( argv[0] == NULL )                /* nothing succeeds     */
    return 0;
  if(strcmp(argv[argcount - 1], "&") == 0)
    {
      bcground = 1;
      argv[argcount -1] = 0;
      argcount = numargs (argv);//uptade number of arguments
    }
  pid = getpid();
  if ( (pid = fork())  == -1 )
    perror("fork");
  else if ( pid == 0 ){
    pid = getpid();
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    execvp(argv[0], argv);
    perror("cannot execute command");
    exit(-1);
  }
  else {
    //if ( wait(&child_info) == -1 )
      //perror("wait");
      if(bcground == 0)
        {
          childsleep = waitpid(pid, &child_info, 0);
          if(childsleep == -1)
            {
              perror("execute: wait failed");
            }
        }
      else
        {
          printf("PID: %d -> started\n", pid);
        }
      childsleep = nomore();
    }
  return child_info;
}

int numargs (char ** arglist)
{
  int count;
  char ** variables;
  count = 0;
  variables = arglist;
  while (*variables)
    {
      count++;
      variables++;
    }
  return count;
}
int nomore(void)
{
  int sleep = 0;
  int state = 0;
  sleep = waitpid(-1, &state, WNOHANG);
  if( sleep == 0)
    {
      return(0);//still hasn't exited
    }
  if(sleep == -1)
    {
      if(errno != ECHILD)
        {
          perror("Error: child running ");
        }
    }
  else
    {
      if(WIFEXITED(state))
        {
          printf("Child exited successfuly: %d\n", sleep);
        }
      else
        {
          printf("Child exited with error: %d\n", sleep);
        }
    }
  return (sleep);
}