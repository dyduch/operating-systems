//
// Created by rudeu on 14.03.18.
//

#ifndef ZESTAW1_DYNAMICARRAY_H
#define ZESTAW1_DYNAMICARRAY_H

#include <stddef.h>

void createBlock(char**, int, size_t);

void addToArray(char**, int*, size_t, int);

void removeFromArray(char**, int*, int);

char** createBlockArray(int, size_t);

void removeBlockArray(char **, int);

void printBlockArray(char**, int, size_t);

void removeAndAdd(char**, int, size_t, int, int);

int searchElement(char**, int, int);

int countValue(char**, int);

char* generateRandomString(size_t);

#endif //ZESTAW1_DYNAMICARRAY_H
