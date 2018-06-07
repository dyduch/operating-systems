CC = gcc -Wall -std=c99


all: cleanrap O0 O1 O2 O3 Os clean

compile: static shared

static:
	$(CC)  -c StaticArray.c DynamicArray.c $(OPTLVL)
	ar rcs libstatic.a StaticArray.o DynamicArray.o
	$(CC)  -DTRACE -D_DEFAULT_SOURCE -c test.c $(OPTLVL)
	$(CC)  -DTRACE -D_DEFAULT_SOURCE -o staticTest test.o StaticArray.o DynamicArray.o $(OPTLVL)

shared:
	$(CC)  -c -fPIC -o DynamicArray2.o DynamicArray.c $(OPTLVL)
	$(CC)  -c -fPIC -o StaticArray2.o StaticArray.c $(OPTLVL)
	$(CC)  -shared -fPIC -o libdynamic.so DynamicArray2.o StaticArray2.o $(OPTLVL)
	$(CC)  -DTRACE -D_DEFAULT_SOURCE -c test.c $(OPTLVL)
	$(CC)  -DTRACE -D_DEFAULT_SOURCE -o sharedTest test.o StaticArray.o DynamicArray.o $(OPTLVL)

O0:
	make compile OPTLVL=-O0
	echo "OPTLVL = -O0 " >> raport3b.txt
	echo "Static Libraries: " >> raport3b.txt
	./staticTest -c 100000 -b 500 -t s
	./staticTest -c 100000 -b 500 -t d
	./staticTest -c 100000 -b 500 -t s -a 10000

	./staticTest -c 100000 -b 500 -t s -r 10000
	./staticTest -c 100000 -b 500 -t d -r 10000
	./staticTest -c 100000 -b 500 -t s -m 10000
	./staticTest -c 100000 -b 500 -t d -m 10000
	./staticTest -c 100000 -b 500 -t s -s 25000
	./staticTest -c 100000 -b 500 -t d -s 25000

	echo "Shared Libraries: " >> raport3b.txt
	./sharedTest -c 100000 -b 500 -t s
	./sharedTest -c 100000 -b 500 -t d
	./sharedTest -c 100000 -b 500 -t s -a 10000

	./sharedTest -c 100000 -b 500 -t s -r 10000
	./sharedTest -c 100000 -b 500 -t d -r 10000
	./sharedTest -c 100000 -b 500 -t s -m 10000
	./sharedTest -c 100000 -b 500 -t d -m 10000
	./sharedTest -c 100000 -b 500 -t s -s 25000
	./sharedTest -c 100000 -b 500 -t d -s 25000

O1:
	make compile OPTLVL=-O1
	echo "OPTLVL = -O1 " >> raport3b.txt
	echo "Static Libraries: " >> raport3b.txt
	./staticTest -c 100000 -b 500 -t s
	./staticTest -c 100000 -b 500 -t d
	./staticTest -c 100000 -b 500 -t s -a 10000

	./staticTest -c 100000 -b 500 -t s -r 10000
	./staticTest -c 100000 -b 500 -t d -r 10000
	./staticTest -c 100000 -b 500 -t s -m 10000
	./staticTest -c 100000 -b 500 -t d -m 10000
	./staticTest -c 100000 -b 500 -t s -s 25000
	./staticTest -c 100000 -b 500 -t d -s 25000

	echo "Shared Libraries: " >> raport3b.txt
	./sharedTest -c 100000 -b 500 -t s
	./sharedTest -c 100000 -b 500 -t d
	./sharedTest -c 100000 -b 500 -t s -a 10000

	./sharedTest -c 100000 -b 500 -t s -r 10000
	./sharedTest -c 100000 -b 500 -t d -r 10000
	./sharedTest -c 100000 -b 500 -t s -m 10000
	./sharedTest -c 100000 -b 500 -t d -m 10000
	./sharedTest -c 100000 -b 500 -t s -s 25000
	./sharedTest -c 100000 -b 500 -t d -s 25000


O2:
	make compile OPTLVL=-O2
	echo "OPTLVL = -O2 " >> raport3b.txt
	echo "Static Libraries: " >> raport3b.txt
	./staticTest -c 100000 -b 500 -t s
	./staticTest -c 100000 -b 500 -t d
	./staticTest -c 100000 -b 500 -t s -a 10000

	./staticTest -c 100000 -b 500 -t s -r 10000
	./staticTest -c 100000 -b 500 -t d -r 10000
	./staticTest -c 100000 -b 500 -t s -m 10000
	./staticTest -c 100000 -b 500 -t d -m 10000
	./staticTest -c 100000 -b 500 -t s -s 25000
	./staticTest -c 100000 -b 500 -t d -s 25000

	echo "Shared Libraries: " >> raport3b.txt
	./sharedTest -c 100000 -b 500 -t s
	./sharedTest -c 100000 -b 500 -t d
	./sharedTest -c 100000 -b 500 -t s -a 10000

	./sharedTest -c 100000 -b 500 -t s -r 10000
	./sharedTest -c 100000 -b 500 -t d -r 10000
	./sharedTest -c 100000 -b 500 -t s -m 10000
	./sharedTest -c 100000 -b 500 -t d -m 10000
	./sharedTest -c 100000 -b 500 -t s -s 25000
	./sharedTest -c 100000 -b 500 -t d -s 25000

O3:
	make compile OPTLVL=-O3
	echo "OPTLVL = -O3 " >> raport3b.txt
	echo "Static Libraries: " >> raport3b.txt
	./staticTest -c 100000 -b 500 -t s
	./staticTest -c 100000 -b 500 -t d
	./staticTest -c 100000 -b 500 -t s -a 10000

	./staticTest -c 100000 -b 500 -t s -r 10000
	./staticTest -c 100000 -b 500 -t d -r 10000
	./staticTest -c 100000 -b 500 -t s -m 10000
	./staticTest -c 100000 -b 500 -t d -m 10000
	./staticTest -c 100000 -b 500 -t s -s 25000
	./staticTest -c 100000 -b 500 -t d -s 25000

	echo "Shared Libraries: " >> raport3b.txt
	./sharedTest -c 100000 -b 500 -t s
	./sharedTest -c 100000 -b 500 -t d
	./sharedTest -c 100000 -b 500 -t s -a 10000

	./sharedTest -c 100000 -b 500 -t s -r 10000
	./sharedTest -c 100000 -b 500 -t d -r 10000
	./sharedTest -c 100000 -b 500 -t s -m 10000
	./sharedTest -c 100000 -b 500 -t d -m 10000
	./sharedTest -c 100000 -b 500 -t s -s 25000
	./sharedTest -c 100000 -b 500 -t d -s 25000

Os:
	make compile OPTLVL=-Os
	echo "OPTLVL = -Os " >> raport3b.txt
	echo "Static Libraries: " >> raport3b.txt
	./staticTest -c 100000 -b 500 -t s
	./staticTest -c 100000 -b 500 -t d
	./staticTest -c 100000 -b 500 -t s -a 10000

	./staticTest -c 100000 -b 500 -t s -r 10000
	./staticTest -c 100000 -b 500 -t d -r 10000
	./staticTest -c 100000 -b 500 -t s -m 10000
	./staticTest -c 100000 -b 500 -t d -m 10000
	./staticTest -c 100000 -b 500 -t s -s 25000
	./staticTest -c 100000 -b 500 -t d -s 25000

	echo "Shared Libraries: " >> raport3b.txt
	./sharedTest -c 100000 -b 500 -t s
	./sharedTest -c 100000 -b 500 -t d
	./sharedTest -c 100000 -b 500 -t s -a 10000

	./sharedTest -c 100000 -b 500 -t s -r 10000
	./sharedTest -c 100000 -b 500 -t d -r 10000
	./sharedTest -c 100000 -b 500 -t s -m 10000
	./sharedTest -c 100000 -b 500 -t d -m 10000
	./sharedTest -c 100000 -b 500 -t s -s 25000
	./sharedTest -c 100000 -b 500 -t d -s 25000

cleanrap:
	rm -f raport3b.txt

clean:
	rm -f *.o *.a *.so *.obj sharedTest staticTest
