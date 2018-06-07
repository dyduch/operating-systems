CC = gcc -Wall -std=c99

all: compile run clean

compile:
	$(CC) -c fileops.c
	ar rcs libfiles.a fileops.o
	$(CC) -DTRACE -D_DEFAULT_SOURCE  -c main.c
	$(CC) -DTRACE -D_DEFAULT_SOURCE  -o main main.o fileops.o

run:
	./main --generate f1.txt -n 1000 -b 4
	./main --generate f2.txt -n 10000 -b 4
	./main --generate f3.txt -n 1000 -b 512
	./main --generate f4.txt -n 10000 -b 512
	./main --generate f5.txt -n 1000 -b 4096
	./main --generate f6.txt -n 10000 -b 4096
	./main --generate f7.txt -n 1000 -b 8192
	./main --generate f8.txt -n 10000 -b 8192

	./main --copy f1.txt -d f1sys.txt -n 1000 -b 4 -t sys
	./main --copy f2.txt -d f2sys.txt -n 10000 -b 4 -t sys
	./main --copy f3.txt -d f3sys.txt -n 1000 -b 512 -t sys
	./main --copy f4.txt -d f4sys.txt -n 10000 -b 512 -t sys
	./main --copy f5.txt -d f5sys.txt -n 1000 -b 4096 -t sys
	./main --copy f6.txt -d f6sys.txt -n 10000 -b 4096 -t sys
	./main --copy f7.txt -d f7sys.txt -n 1000 -b 8192 -t sys
	./main --copy f8.txt -d f8sys.txt -n 10000 -b 8192 -t sys

	./main --copy f1.txt -d f1lib.txt -n 1000 -b 4 -t lib
	./main --copy f2.txt -d f2lib.txt -n 10000 -b 4 -t lib
	./main --copy f3.txt -d f3lib.txt -n 1000 -b 512 -t lib
	./main --copy f4.txt -d f4lib.txt -n 10000 -b 512 -t lib
	./main --copy f5.txt -d f5lib.txt -n 1000 -b 4096 -t lib
	./main --copy f6.txt -d f6lib.txt -n 10000 -b 4096 -t lib
	./main --copy f7.txt -d f7lib.txt -n 1000 -b 8192 -t lib
	./main --copy f8.txt -d f8lib.txt -n 10000 -b 8192 -t lib

	./main --sort f1sys.txt -n 1000 -b 4 -t sys
	./main --sort f2sys.txt -n 10000 -b 4 -t sys
	./main --sort f3sys.txt -n 1000 -b 512 -t sys
	./main --sort f4sys.txt -n 10000 -b 512 -t sys
	./main --sort f5sys.txt -n 1000 -b 4096 -t sys
	./main --sort f6sys.txt -n 10000 -b 4096 -t sys
	./main --sort f7sys.txt -n 1000 -b 8192 -t sys
	./main --sort f8sys.txt -n 10000 -b 8192 -t sys

	./main --sort f1lib.txt -n 1000 -b 4 -t lib
	./main --sort f2lib.txt -n 10000 -b 4 -t lib
	./main --sort f3lib.txt -n 1000 -b 512 -t lib
	./main --sort f4lib.txt -n 10000 -b 512 -t lib
	./main --sort f5lib.txt -n 1000 -b 4096 -t lib
	./main --sort f6lib.txt -n 10000 -b 4096 -t lib
	./main --sort f7lib.txt -n 1000 -b 8192 -t lib
	./main --sort f8lib.txt -n 10000 -b 8192 -t lib

clean:
	rm -f *.o *.a *.so *.obj test