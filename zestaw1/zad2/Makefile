CC = gcc -Wall -std=c99


all: compile run clean

static:
	$(CC) -c StaticArray.c DynamicArray.c
	ar rcs libstatic.a StaticArray.o DynamicArray.o


shared:
	$(CC) -c -fPIC -o DynamicArray2.o DynamicArray.c
	$(CC) -c -fPIC -o StaticArray2.o StaticArray.c
	$(CC) -shared -fPIC -o libdynamic.so DynamicArray2.o StaticArray2.o


compile: static shared
	$(CC) -DTRACE -D_DEFAULT_SOURCE -c test.c
	$(CC) -DTRACE -D_DEFAULT_SOURCE -o test test.o StaticArray.o DynamicArray.o

run:
	rm -f raport2.txt
	touch raport2.txt

	./test -c 100000 -b 500 -t s
	./test -c 100000 -b 500 -t d
	./test -c 100000 -b 500 -t s -a 10000

	./test -c 100000 -b 500 -t s -r 10000
	./test -c 100000 -b 500 -t d -r 10000
	./test -c 100000 -b 500 -t s -m 10000
	./test -c 100000 -b 500 -t d -m 10000
	./test -c 100000 -b 500 -t s -s 25000
	./test -c 100000 -b 500 -t d -s 25000


clean:
	rm -f *.o *.a *.so *.obj test
