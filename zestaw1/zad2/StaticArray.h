#ifndef ZESTAW1_STATICARRAY_H
#define ZESTAW1_STATICARRAY_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_ARRAY_SIZE 1000000
#define MAX_BLOCK_SIZE 1001

void createBlockForStatic(int, size_t);

void addToStaticArray(int*, size_t, int);

void removeFromStaticArray(int*, int);

void createStaticBlockArray(int, size_t);

void removeStaticBlockArray();

void printStaticBlockArray();


void removeAndAddToStatic(int, size_t, int, int);


int searchElementInStatic(int, int);


int countValueForStatic(int);

char* generateRandomStringForStatic(size_t);

#endif