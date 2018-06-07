CC = gcc -Wall -std=c11 -D_GNU_SOURCE

all: compile clean

compile:
	$(CC) -c simple_signals.c
	$(CC) -c sigs_with_kids.c
	$(CC) -o simple_signals simple_signals.o
	$(CC) -o sigs_with_kids sigs_with_kids.o

clean:
	rm -f *.o *.a *.so *.obj