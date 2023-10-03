/**  smsh1.c  small-shell version 1
 **             first really useful version after prompting shell
 **             this one parses the command line into strings
 **             uses fork, exec, wait, and ignores signals
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "smsh.h"

#define DFL_PROMPT      "Enoch/prog5> "
int returncmdvalue(char ** arglist);

int main()
{
  char  *cmdline, *prompt, **arglist;
  int   result;
  void  setup();

  prompt = DFL_PROMPT ;
  setup();

  while ( (cmdline = next_cmd(prompt, stdin)) != NULL ){
    if ( (arglist = splitline(cmdline)) != NULL  ){
      result = returncmdvalue(arglist);
      if(result != 0)
        {
          freelist(arglist);
          free(cmdline);
          result = nomore();
          continue;
        }
      result = execute(arglist);
      freelist(arglist);
      free(cmdline);
    }
    //
  }
  result = nomore();
  return 0;
}

int returncmdvalue(char ** arglist)
{
  int value = 0;
  if (arglist[0] == NULL)
    {
      return(-1);
    }
  if(strcmp(arglist[0], "exit") == 0) //first test that wasn't working
    {
      value = 1;//also check for the next value after exit
      if(arglist[1] == NULL)
        {
          exit(0);
        }
      else
        {
          value = atoi(arglist[1]);
          //printf("value = %d\n", value);
          exit (value);
        }
    }
  if(strcmp(arglist[0], "cd") == 0)
    {
      value = 2;
      if(arglist[1] == NULL)
        {
          chdir(getenv("HOME"));
          // printf("Changing directory to Home");
        }
      else
        {
          chdir(arglist[1]);
          if(errno < 0)
            {
              printf("cd failed! %s\n", strerror(errno));
              value = -2;
            }
        }
    }
  return value;
}

void setup()
/*
 * purpose: initialize shell
 * returns: nothing. calls fatal() if trouble
 */
{
  signal(SIGINT,  SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n)
{
  fprintf(stderr,"Error: %s,%s\n", s1, s2);
  exit(n);
}
