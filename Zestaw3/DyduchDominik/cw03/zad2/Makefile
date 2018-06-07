CC = gcc -Wall -std=c11

all: compile run clean

compile:
	$(CC) -D_GNU_SOURCE -c main.c
	$(CC) -D_GNU_SOURCE -o main main.o

run:
	./main sample.txt

clean:
	rm -f *.o *.a *.so *.obj test