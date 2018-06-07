CC = gcc
CFLAGS = -std=gnu99 -Wall

all: barber client

barber:
	$(CC) $(CFLAGS) barbershop.c barber.c -o barber

client:
	$(CC) $(CFLAGS) barbershop.c client.c -o client


clean:
	rm -rf barber client