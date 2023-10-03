/*
 *this is a word count but simply counts the number of lines in each file specified
 *Prog 6/lc1 : Enoch & Zachary Ouellette
 */
#include "lc.h"

int main(int argc, char *argv[])
{
  int i, j, count, total, fd, size; //declaring multiple variables(i & j for loops)
  i = j = count = total = fd = size = 0; //initializing
  char buf[BUFFERSIZE];

  if (argc < 2)
    {
      perror("Please try again, with more arguments\n");
      exit(0);
    }
  for (i = 1; i < argc; i++)//we skip 0 for prog name
    {
      if((fd = open(argv[i], O_RDONLY)) == -1)
        {
          perror ("Opening file failed!\n");
        }
      while ((size = read(fd, &buf, BUFFERSIZE)) > 0)
        {
          for (j = 0; j < size; j++)
            {
              if (buf[j] == '\n')//counts the number of new line characters
                {
                  count++;
                }
            }//end of for loop
        }//end of while loop
      printf("File: %s Lines: %d \n", argv[i], count);
      total += count;
      count = 0; //reset for new file
      close(fd);
    }//end of outer for loop
  printf("The total number of lines are : %d\n", total);
}//end of program