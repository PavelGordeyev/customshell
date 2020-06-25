CC = gcc
CFLAGS = -g -Wall -std=c99

all : smallsh

smallsh : smallsh.o dynArrPID.o dynArrChar.o
	$(CC) $(CFLAGS) -o $@ $^

smallsh.o : smallsh.c

dynArrPID.o : dynArrPID.c

dynArrChar.o : dynArrChar.c

clean :
	-rm *.o
	-rm smallsh
