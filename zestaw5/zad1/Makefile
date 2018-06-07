CC = gcc -Wall -std=c11 -D_GNU_SOURCE

all: compile clean

compile:
	$(CC) -c main.c
	$(CC) -o main main.o

clean:
	rm -f *.o *.a *.so *.obj