/*
 *this is a word count but simply counts the number of lines in each file specified
 *Prog 6/lc2: Enoch & Zachary Ouellette
 */
#include "lc.h"

struct report
{
  char files[256];
  int lines;
}

totallines(char *);

int main(int argc, char *argv[])
{
  int childout;
  int total, i;
  total = i = 0;
  int pipefd[2];
  pid_t pid, childpid;
  if (argc < 2)
    {
      errno = EINVAL;
      perror("Please check your arguments");
      return -1;
    }
  char *stringarg[3];
  stringarg[0] = "lc1";
  stringarg[2] = NULL;
  for(i = 1; i < argc; i++)
    {
      //printf("Test: please work");
      stringarg[1] = argv[i];
      if( pipe(pipefd) == -1)
        {
          perror("Pipe failed!");
        }
      if((pid = fork()) == -1)
        {
          perror("for failed!");
        }
      else if(pid == 0)
        {
          printf("childpid successfull");
          childpid = getpid();

          close(pipefd[0]);
          if( dup2(pipefd[1], 1) == -1)
            {
              perror("STDOUT FAILED!");
            }
          close(pipefd[1]);
          execvp("lc1", stringarg);
        }
      else
        {

          childout = pipefd[0];
          close(pipefd[1]);
          //execvp("lc1", stringarg);
          char buf [BUFFERSIZE];
          int size = 0;
          struct report file_info;
          if((size = read(childout, &buf, BUFFERSIZE)) == 0)
            {
              perror("Read failed!");
            }
          buf[size] = 0;
          file_info = totallines(buf);
          printf("File: %s Lines: %d \n", file_info.files, file_info.lines);
          total += file_info.lines;
          int status;
          status = 0 ;
          waitpid(childpid, &status, WNOHANG);
        }
    }
  printf ("The total number of lines are : %d \n", total);
  return 0;
}//end of program

struct report totallines(char *buf)
{
  struct report info;
  int k;
  if (( k = sscanf(buf,"%*s %s %*s %d", info.files, &info.lines)) != 2)
    {
      perror("sscanf failed!");
    }
  return info;
}//end of program 