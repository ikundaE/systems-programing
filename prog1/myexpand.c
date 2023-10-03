/*
 cos350 experiment qsort
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
static int cmpstringp(const void *p1, const void *p2)
{
   return strcmp(* (char * const *) p1, * (char * const *) p2);
}


int main(int argc, char * argv[]){
  int  newCols = 8;
  int currentPos = 0;
  char ch;
  if ( argc == 3 ){
    if (argv [2] < 0){
      fprintf(stderr, "The number of tabs can't be negative");
      exit(1);
    }
    newCols = atoi (argv [2]);
  }else if ( argc != 1){
    fprintf(stderr, "argument too short");
    exit(1);
  }
  int temp = 0;
  while ( ch != EOF){
    ch = getchar();
    if ( ch == '\n'){
      putchar(ch);
      currentPos = 0;
    }
    else if ( ch == '\t'){
      temp = newCols - (currentPos%newCols);
      int i;
      for ( i = 0; i < temp; i++){
        putchar(' ');
        currentPos++;
      }
    }else {
      putchar(ch);
      currentPos++;
    }
  }
}