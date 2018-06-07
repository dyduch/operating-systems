CC = gcc -Wall -std=c99


all: compile run clean

static:
	$(CC) -c StaticArray.c DynamicArray.c
	ar rcs libstatic.a StaticArray.o DynamicArray.o
	$(CC) -DTRACE -D_DEFAULT_SOURCE -c test.c
	$(CC) -DTRACE -D_DEFAULT_SOURCE -o staticTest test.o StaticArray.o DynamicArray.o

shared:
	$(CC) -c -fPIC -o DynamicArray2.o DynamicArray.c
	$(CC) -c -fPIC -o StaticArray2.o StaticArray.c
	$(CC) -shared -fPIC -o libdynamic.so DynamicArray2.o StaticArray2.o
	$(CC) -DTRACE -D_DEFAULT_SOURCE -c test.c
	$(CC) -DTRACE -D_DEFAULT_SOURCE -o sharedTest test.o StaticArray.o DynamicArray.o

compile: static shared

run:
	rm -f raport3a.txt
	touch raport3a.txt
	echo "Static Libraries: " >> raport3a.txt
	./staticTest -c 100000 -b 500 -t s
	./staticTest -c 100000 -b 500 -t d
	./staticTest -c 100000 -b 500 -t s -a 10000

	./staticTest -c 100000 -b 500 -t s -r 10000
	./staticTest -c 100000 -b 500 -t d -r 10000
	./staticTest -c 100000 -b 500 -t s -m 10000
	./staticTest -c 100000 -b 500 -t d -m 10000
	./staticTest -c 100000 -b 500 -t s -s 25000
	./staticTest -c 100000 -b 500 -t d -s 25000

	echo "Shared Libraries: " >> raport3a.txt
	./sharedTest -c 100000 -b 500 -t s
	./sharedTest -c 100000 -b 500 -t d
	./sharedTest -c 100000 -b 500 -t s -a 10000

	./sharedTest -c 100000 -b 500 -t s -r 10000
	./sharedTest -c 100000 -b 500 -t d -r 10000
	./sharedTest -c 100000 -b 500 -t s -m 10000
	./sharedTest -c 100000 -b 500 -t d -m 10000
	./sharedTest -c 100000 -b 500 -t s -s 25000
	./sharedTest -c 100000 -b 500 -t d -s 25000


clean:
	rm -f *.o *.a *.so *.obj sharedTest staticTest
