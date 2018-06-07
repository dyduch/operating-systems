CC = gcc -Wall -std=c99


all: static shared clean

static:
	$(CC) -c StaticArray.c DynamicArray.c
	ar rcs libstatic.a StaticArray.o DynamicArray.o

shared:
	$(CC) -c -fPIC -o DynamicArray2.o DynamicArray.c
	$(CC) -c -fPIC -o StaticArray2.o StaticArray.c
	$(CC) -shared -fPIC -o libdynamic.so DynamicArray2.o StaticArray2.o


clean:
	rm -f *.o *.a *.so
