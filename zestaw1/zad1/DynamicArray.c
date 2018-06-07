//
// Created by rudeu on 14.03.18.
//

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "DynamicArray.h"

void createBlock(char** blockArray, int index, size_t blockSize){

    blockArray[index] = calloc(blockSize, sizeof(char));
    blockArray[index] = generateRandomString(blockSize);
}

void addToArray(char** blockArray, int* arraySize, size_t blockSize, int number){

    for(int i = *arraySize; i < (*arraySize + number); i++){
        createBlock(blockArray, i, blockSize);
    }

    *arraySize = (*arraySize + number);

}

void removeFromArray(char** blockArray, int* arraySize, int number){

    for(int i = *arraySize - 1; i >= (*arraySize - number); i--){
        free(blockArray[i]);
    }

    *arraySize = (*arraySize - number);

}

char** createBlockArray(int arraySize, size_t blockSize){

    char** blockArray = calloc((size_t) arraySize, sizeof(char*));
    for(int i =0; i< arraySize; i++){
        createBlock(blockArray, i, blockSize);
    }
    return blockArray;
}

void removeBlockArray(char** blockArray, int arraySize){

    for(int i = 0; i < arraySize; i++){
        free(blockArray[i]);
    }
    free(blockArray);
}

void printBlockArray(char** blockArray, int arraySize, size_t blockSize){

    for(int i = 0; i < arraySize; i++){
        printf("%d. ", i);
        for(int j = 0; j < blockSize; j++){
            printf("%c", blockArray[i][j]);
        }
        printf("\n");
    }
}

void removeAndAdd(char** blockArray, int arraySize, size_t blockSize, int index, int number){

    if(index >= arraySize || index < 0) return;
    for(int i = 0; i < number; i++){

        free(blockArray[index]);
        createBlock(blockArray, index, blockSize);
    }

}

int searchElement(char** blockArray, int arraySize, int index){

    if(index < 0 || index >= arraySize) return -1;
    int value = countValue(blockArray, index);
    int closestIndex = -1;
    int valueDifference = INT_MAX;
    int currentValue;

    for(int i = 0; i < arraySize; i++){

        if(i != index) {
            currentValue = countValue(blockArray, i);
            if (abs(currentValue - value) < valueDifference) {
                valueDifference = abs(currentValue - value);
                closestIndex = i;
            }
        }
    }
    return closestIndex;
}

int countValue(char** blockArray, int index){

    int j = 0;
    int value = 0;

    while(blockArray[index][j] != '\0'){

        value += blockArray[index][j];
        j++;
    }

    return value;
}

char* generateRandomString(size_t blockSize){

    static const char charset[] = "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    char* randomString = calloc(blockSize, sizeof(char));
    for (int i = 0; i < blockSize; i++){
        randomString[i] = charset[rand() % (sizeof(charset) -1)];
    }

    randomString[blockSize] = '\0';
    return randomString;

}