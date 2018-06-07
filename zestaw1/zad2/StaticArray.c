#include "StaticArray.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <limits.h>

char staticArray[MAX_ARRAY_SIZE][MAX_BLOCK_SIZE];


void createBlockForStatic(int index, size_t blockSize){

    char* randomString = generateRandomStringForStatic(blockSize);
    for(int i = 0; i < blockSize; i++){
        staticArray[index][i] = randomString[i];
    }
}

void addToStaticArray(int* arraySize, size_t blockSize, int number){

    for(int i = *arraySize; i < (*arraySize + number); i++){
        createBlockForStatic(i, blockSize);
    }
    *arraySize = (*arraySize + number);
}

void removeFromStaticArray(int* arraySize, int number){

    for(int i = *arraySize - 1; i >= (*arraySize - number); i--){
        int j = 0;
        while(staticArray[i][j] != '\0'){
            staticArray[i][j] = 0;
            j++;
        }
    }

    *arraySize = (*arraySize - number);
}

void createStaticBlockArray(int arraySize, size_t blockSize){

    for(int i = 0; i< arraySize; i++) {
        createBlockForStatic(i, blockSize);
    }
}

void removeStaticBlockArray(){

    int i = 0;
    int j = 0;

    while(staticArray[i][0] != 0){
        while(staticArray[i][j] != '\0'){
            staticArray[i][j] = 0;
            j++;
        }
        i++;
    }
}

void printStaticBlockArray(){

    int i = 0;
    int j = 0;

    while(staticArray[i][0] != 0){
        printf("%d. ", i);
        j = 0;
        while(staticArray[i][j] != '\0'){
            printf("%c", staticArray[i][j]);
            j++;
        }
        printf("\n");
        i++;
    }
}

void removeAndAddToStatic(int arraySize, size_t blockSize, int index, int number){

    if(index >= arraySize || index < 0) return;

    for(int i = 0; i < number; i++){
        for(int j = 0; j < blockSize; j++){
            staticArray[index][j] = 0;
        }
        createBlockForStatic(index, blockSize);
    }
}

int searchElementInStatic(int arraySize, int index){

    if(index < 0 || index >= arraySize) return -1;
    int value = countValueForStatic(index);
    int closestIndex = -1;
    int valueDifference = INT_MAX;
    int currentValue;

    for(int i = 0; i < arraySize; i++){

        if(i != index) {
            currentValue = countValueForStatic(i);
            if (abs(currentValue - value) < valueDifference) {
                valueDifference = abs(currentValue - value);
                closestIndex = i;
            }
        }
    }
    return closestIndex;
}

int countValueForStatic(int index){

    int j = 0;
    int value = 0;

    while(staticArray[index][j] != '\0'){

        value += staticArray[index][j];
        j++;
    }

    return value;
}

char* generateRandomStringForStatic(size_t blockSize){

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

