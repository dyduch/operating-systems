CC = gcc -Wall -std=c11

all: compile clean

compile:
	$(CC) -c server.c
	$(CC) -o server server.o
	$(CC) -c client.c
	$(CC) -o client client.o

clean:
	rm -f *.o *.a *.so *.obj