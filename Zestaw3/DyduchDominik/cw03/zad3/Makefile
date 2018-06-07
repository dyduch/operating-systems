CC = gcc -Wall -std=c11

all: compile clean

compile:
	$(CC) -D_GNU_SOURCE -c main.c
	$(CC) -D_GNU_SOURCE -o main main.o
	$(CC) -c memory_use.c
	$(CC) -o memory_use memory_use.o

clean:
	rm -f *.o *.a *.so *.obj test