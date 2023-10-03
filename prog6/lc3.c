/*
 *This Program uses pthreads to count the files. It creates simultaneous threads, one for each file.
 *Proj6/lc3: Enoch & Zachary Ouellette
 */
#include "lc.h"

int total=0;
pthread_mutex_t linecount_lock = PTHREAD_MUTEX_INITIALIZER;

void *linecount (void *);

struct report
{
  char *files;
  int lines;
};

int main ( int argc, char *argv[])
{
        if (argc < 2)
        {
                errno = EINVAL;
                perror("Please check your arguments");
                return -1;
        }

        int i =0 ;
        struct report files [argc-1];
        pthread_t threads [argc-1];
        int threadargs [argc-1];

        for(i=0; i < argc-1; i++)
        {
                files[i].files = argv[i+1];
                files[i].lines = 0;
                threadargs [i] = i;
                pthread_create(&threads[i], NULL, linecount, &files[i]);
        }

        for(i=0; i < argc-1; i++)
        {
                pthread_join(threads[i],NULL);
        }
        printf("The total number of lines are : %d\n", total);
        return 0;
}

void *linecount(void *info)
{
        struct report *file = info;
        char buf[BUFFERSIZE];
        int count, j, fd, size;
        count = j = fd = size = 0;
        if((fd = open(file->files, O_RDONLY)) == -1)
        {
                perror("Open failed!");
        }
        while (( size = read(fd, &buf, BUFFERSIZE)) > 0)
        {
                for(j=0; j < size; j++)
                {
                        if(buf[j] == '\n')
                        {
                                count++;
                        }
                }
        }
        file->lines = count;
        pthread_mutex_lock(&linecount_lock);
        total += count;
        pthread_mutex_unlock(&linecount_lock);
        printf("File: %s Lines: %d \n",file->files, file->lines);
        close(fd);
}//end of program