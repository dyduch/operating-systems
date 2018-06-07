CC = gcc -Wall -std=gnu99 -pthread -lm


all: compile run

compile:
	$(CC) -o main main.c

run:
	./main 1 lena.ascii.pgm filter.txt res1th1fil.pgm
	./main 2 lena.ascii.pgm filter.txt res2th1fil.pgm
	./main 4 lena.ascii.pgm filter.txt res4th1fil.pgm
	./main 8 lena.ascii.pgm filter.txt res8th1fil.pgm
	./main 1 lena.ascii.pgm filter2.txt res1th2fil.pgm
	./main 2 lena.ascii.pgm filter2.txt res2th2fil.pgm
	./main 4 lena.ascii.pgm filter2.txt res4th2fil.pgm
	./main 8 lena.ascii.pgm filter2.txt res8th2fil.pgm
	./main 1 lena.ascii.pgm filter3.txt res1th3fil.pgm
	./main 2 lena.ascii.pgm filter3.txt res2th3fil.pgm
	./main 4 lena.ascii.pgm filter3.txt res4th3fil.pgm
	./main 8 lena.ascii.pgm filter3.txt res8th3fil.pgm

clean:
	rm -f *.o *.a *.so res* Times.txt
