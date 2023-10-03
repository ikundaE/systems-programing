#
# makefile for smsh
#
CC=gcc
CFLAGS=-g

mysh: smsh1.o splitline.o execute.o
        $(CC) $(CFLAGS) -o mysh smsh1.o splitline.o execute.o
clean:
        rm *.o