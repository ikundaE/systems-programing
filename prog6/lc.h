/* This is the header file for multiple file line counting
 * Enoch and Zachary Ouellette
 */

#include <pthread.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#define BUFFERSIZE 4096