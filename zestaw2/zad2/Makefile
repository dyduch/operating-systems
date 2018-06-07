CC = gcc -Wall -std=c99

all: compile clean

compile:
	$(CC) -c main.c -D_GNU_SOURCE
	$(CC) -o main main.o -D_GNU_SOURCE
	$(CC) -c main_nftw.c -D_GNU_SOURCE
	$(CC) -o main_nftw main_nftw.o -D_GNU_SOURCE

clean:
	rm -rf *.o