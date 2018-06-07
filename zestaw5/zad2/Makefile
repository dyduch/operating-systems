CC = gcc -Wall -std=c11 -D_GNU_SOURCE

all: compile clean

compile:
	$(CC) -c master.c
	$(CC) -o master master.o
	$(CC) -c slave.c
	$(CC) -o slave slave.o

clean:
	rm -f *.o *.a *.so *.obj