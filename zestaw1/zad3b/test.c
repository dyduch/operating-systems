//
// Created by rudeu on 15.03.18.
//

#include "StaticArray.h"
#include "DynamicArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/times.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>



struct countedTimes{
    struct timeval systemStartTime, systemEndTime, systemTimeDiff,
    userStartTime, userEndTime, userTimeDiff,
    realStartTime, realEndTime, realTimeDiff;
};

void setTimeDiff(struct timeval* startTime, struct timeval* endTime, struct timeval* result){

    if (startTime->tv_sec > endTime->tv_sec)
        timersub(startTime, endTime, result);
    else if (startTime->tv_sec < endTime->tv_sec)
        timersub(endTime, startTime, result);
    else  // startTime.tv_sec == endTime.tv_sec
    {
        if (startTime->tv_usec >= endTime->tv_usec)
            timersub(startTime, endTime, result);
        else
            timersub(endTime, startTime, result);
    }
}

void countDifference(struct countedTimes *times){

    setTimeDiff(&times->systemStartTime, &times->systemEndTime, &times->systemTimeDiff);
    setTimeDiff(&times->userStartTime, &times->userEndTime, &times->userTimeDiff);
    setTimeDiff(&times->realStartTime, &times->realEndTime, &times->realTimeDiff);

}


void printTimes(struct countedTimes times){

    printf("\nREAL: %fs\t", (double) times.realTimeDiff.tv_sec  + ((double) times.realTimeDiff.tv_usec) / 1000000);
    printf("USER: %fs\t",(double) times.userTimeDiff.tv_sec  + ((double) times.userTimeDiff.tv_usec) / 1000000);
    printf("SYSTEM: %fs\n",(double) times.systemTimeDiff.tv_sec  + ((double) times.systemTimeDiff.tv_usec) / 1000000);
}

void setTimes(struct rusage resStartTime, struct rusage resEndTime, struct countedTimes* times){

    times->systemStartTime = resStartTime.ru_stime;
    times->userStartTime = resStartTime.ru_utime;
    times->systemEndTime = resEndTime.ru_stime;
    times->userEndTime = resEndTime.ru_utime;
}

void toFile(struct countedTimes times, FILE *sp){

    fprintf(sp, "\nREAL: %fs\t", (double) times.realTimeDiff.tv_sec  + ((double) times.realTimeDiff.tv_usec) / 1000000);
    fprintf(sp, "USER: %fs\t", (double) times.userTimeDiff.tv_sec  + ((double) times.userTimeDiff.tv_usec) / 1000000);
    fprintf(sp, "SYSTEM: %fs\n",(double) times.systemTimeDiff.tv_sec  + ((double) times.systemTimeDiff.tv_usec) / 1000000);
}

void runAllocationTest(int arraySize, size_t blockSize, char typeOfAllocation){

    struct rusage resStartTime;
    struct rusage resEndTime;
    struct countedTimes times;

    if(typeOfAllocation == 's'){

        gettimeofday(&times.realStartTime, NULL);
        getrusage(RUSAGE_SELF, &resStartTime);
        createStaticBlockArray(arraySize, blockSize);
        gettimeofday(&times.realEndTime, NULL);
        getrusage(RUSAGE_SELF, &resEndTime);
        setTimes(resStartTime, resEndTime, &times);
        printf("Static Array of size: %d, blockSize: %zu, allocation time:\t", arraySize, blockSize);
        countDifference(&times);
        printTimes(times);
        FILE* sp;
        sp = fopen("raport3b.txt", "a");
        fprintf(sp, "Static Array of size: %d, blockSize: %zu, allocation time:\t", arraySize, blockSize);
        toFile(times, sp);
        fclose(sp);

    }

    if(typeOfAllocation == 'd'){

        gettimeofday(&times.realStartTime, NULL);
        getrusage(RUSAGE_SELF, &resStartTime);
        char** array = createBlockArray(arraySize, blockSize);
        gettimeofday(&times.realEndTime, NULL);
        getrusage(RUSAGE_SELF, &resEndTime);
        setTimes(resStartTime, resEndTime, &times);
        printf("Dynamic Array of size: %d, blockSize: %zu, allocation time:\t", arraySize, blockSize);
        countDifference(&times);
        printTimes(times);
        free(array);
        FILE* sp;
        sp = fopen("raport3b.txt", "a");
        fprintf(sp, "Dynamic Array of size: %d, blockSize: %zu, allocation time:\t", arraySize, blockSize);
        toFile(times, sp);
        fclose(sp);

    }
}

void runSearchTest(int arraySize, size_t blockSize, int index, char typeOfAllocation){

    struct rusage resStartTime;
    struct rusage resEndTime;
    struct countedTimes times;

    if(typeOfAllocation == 's'){

        createStaticBlockArray(arraySize, blockSize);
        gettimeofday(&times.realStartTime, NULL);
        getrusage(RUSAGE_SELF, &resStartTime);
        searchElementInStatic(arraySize, index);
        gettimeofday(&times.realEndTime, NULL);
        getrusage(RUSAGE_SELF, &resEndTime);
        setTimes(resStartTime, resEndTime, &times);
        printf("Searching element in static array of size: %d, blockSize: %zu, time:\t", arraySize, blockSize);
        countDifference(&times);
        printTimes(times);
        FILE* sp;
        sp = fopen("raport3b.txt", "a");
        fprintf(sp, "Searching element in static array of size: %d, blockSize: %zu, time:\t", arraySize, blockSize);
        toFile(times, sp);
        fclose(sp);

    }

    if(typeOfAllocation == 'd'){

        char** array = createBlockArray(arraySize, blockSize);
        gettimeofday(&times.realStartTime, NULL);
        getrusage(RUSAGE_SELF, &resStartTime);
        searchElement(array, arraySize, index);
        gettimeofday(&times.realEndTime, NULL);
        getrusage(RUSAGE_SELF, &resEndTime);
        setTimes(resStartTime, resEndTime, &times);
        printf("Searching element in dynamic array of size: %d, blockSize: %zu, time:\t", arraySize, blockSize);
        countDifference(&times);
        printTimes(times);
        free(array);
        FILE* sp;
        sp = fopen("raport3b.txt", "a");
        fprintf(sp, "Searching element in dynamic array of size: %d, blockSize: %zu, time:\t", arraySize, blockSize);
        toFile(times, sp);
        fclose(sp);
    }
}

void runAddTest(int arraySize, size_t blockSize, int numberOfAdded, char typeOfAllocation){

    struct rusage resStartTime;
    struct rusage resEndTime;
    struct countedTimes times;

    if(typeOfAllocation == 's'){

        createStaticBlockArray(arraySize, blockSize);
        gettimeofday(&times.realStartTime, NULL);
        getrusage(RUSAGE_SELF, &resStartTime);
        addToStaticArray(&arraySize, blockSize, numberOfAdded);
        gettimeofday(&times.realEndTime, NULL);
        getrusage(RUSAGE_SELF, &resEndTime);
        setTimes(resStartTime, resEndTime, &times);
        printf("Adding %d elements to static array of size: %d, blockSize: %zu, time:\t", numberOfAdded, arraySize - numberOfAdded, blockSize);
        countDifference(&times);
        printTimes(times);
        FILE* sp;
        sp = fopen("raport3b.txt", "a");
        fprintf(sp, "Adding %d elements to static array of size: %d, blockSize: %zu, time:\t", numberOfAdded, arraySize - numberOfAdded, blockSize);
        toFile(times, sp);
        fclose(sp);

    }

    if(typeOfAllocation == 'd'){

        char** array = createBlockArray(arraySize, blockSize);
        gettimeofday(&times.realStartTime, NULL);
        getrusage(RUSAGE_SELF, &resStartTime);
        array = addToArray(array, &arraySize, blockSize, numberOfAdded);
        gettimeofday(&times.realEndTime, NULL);
        getrusage(RUSAGE_SELF, &resEndTime);
        setTimes(resStartTime, resEndTime, &times);
        printf("Adding %d elements to dynamic array of size: %d, blockSize: %zu, time:\t", numberOfAdded, arraySize - numberOfAdded, blockSize);
        countDifference(&times);
        printTimes(times);
        //free(array);
        FILE* sp;
        sp = fopen("raport3b.txt", "a");
        fprintf(sp, "Adding %d elements to dynamic array of size: %d, blockSize: %zu, time:\t", numberOfAdded, arraySize - numberOfAdded, blockSize);
        toFile(times, sp);
        fclose(sp);
    }

}

void runRemoveTest(int arraySize, size_t blockSize, int numberOfRemoved, char typeOfAllocation){

    struct rusage resStartTime;
    struct rusage resEndTime;
    struct countedTimes times;

    if(typeOfAllocation == 's'){

        createStaticBlockArray(arraySize, blockSize);
        gettimeofday(&times.realStartTime, NULL);
        getrusage(RUSAGE_SELF, &resStartTime);
        removeFromStaticArray(&arraySize, numberOfRemoved);
        gettimeofday(&times.realEndTime, NULL);
        getrusage(RUSAGE_SELF, &resEndTime);
        setTimes(resStartTime, resEndTime, &times);
        printf("Removing %d elements from static array of size: %d, blockSize: %zu, time:\t",numberOfRemoved, arraySize + numberOfRemoved, blockSize);
        countDifference(&times);
        printTimes(times);
        FILE* sp;
        sp = fopen("raport3b.txt", "a");
        fprintf(sp, "Removing %d elements from static array of size: %d, blockSize: %zu, time:\t",numberOfRemoved, arraySize + numberOfRemoved, blockSize);
        toFile(times, sp);
        fclose(sp);

    }

    if(typeOfAllocation == 'd'){

        char** array = createBlockArray(arraySize, blockSize);
        gettimeofday(&times.realStartTime, NULL);
        getrusage(RUSAGE_SELF, &resStartTime);
        removeFromArray(array, &arraySize, numberOfRemoved);
        gettimeofday(&times.realEndTime, NULL);
        getrusage(RUSAGE_SELF, &resEndTime);
        setTimes(resStartTime, resEndTime, &times);
        printf("Removing %d elements from dynamic array of size: %d, blockSize: %zu, time:\t",numberOfRemoved, arraySize + numberOfRemoved, blockSize);
        countDifference(&times);
        printTimes(times);
        //free(array);
        FILE* sp;
        sp = fopen("raport3b.txt", "a");
        fprintf(sp, "Removing %d elements from dynamic array of size: %d, blockSize: %zu, time:\t",numberOfRemoved, arraySize + numberOfRemoved, blockSize);
        toFile(times, sp);
        fclose(sp);
    }

}

void runRemoveAndAddTest(int arraySize, size_t blockSize, int indexOfTried, int numberOfRepetitions, char typeOfAllocation){

    struct rusage resStartTime;
    struct rusage resEndTime;
    struct countedTimes times;

    if(typeOfAllocation == 's'){

        createStaticBlockArray(arraySize, blockSize);
        gettimeofday(&times.realStartTime, NULL);
        getrusage(RUSAGE_SELF, &resStartTime);
        removeAndAddToStatic(arraySize, blockSize, indexOfTried, numberOfRepetitions);
        gettimeofday(&times.realEndTime, NULL);
        getrusage(RUSAGE_SELF, &resEndTime);
        setTimes(resStartTime, resEndTime, &times);
        printf("Removing and adding an element %d times in static array of size: %d, blockSize: %zu, time:\t",numberOfRepetitions, arraySize, blockSize);
        countDifference(&times);
        printTimes(times);
        FILE* sp;
        sp = fopen("raport3b.txt", "a");
        fprintf(sp, "Removing and adding an element %d times in static array of size: %d, blockSize: %zu, time:\t",numberOfRepetitions, arraySize, blockSize);
        toFile(times, sp);
        fclose(sp);

    }

    if(typeOfAllocation == 'd'){

        char** array = createBlockArray(arraySize, blockSize);
        gettimeofday(&times.realStartTime, NULL);
        getrusage(RUSAGE_SELF, &resStartTime);
        removeAndAdd(array, arraySize, blockSize, indexOfTried, numberOfRepetitions);
        gettimeofday(&times.realEndTime, NULL);
        getrusage(RUSAGE_SELF, &resEndTime);
        setTimes(resStartTime, resEndTime, &times);
        printf("Removing and adding an element %d times in static array of size: %d, blockSize: %zu, time:\t",numberOfRepetitions, arraySize, blockSize);
        countDifference(&times);
        printTimes(times);
        //free(array);
        FILE* sp;
        sp = fopen("raport3b.txt", "a");
        fprintf(sp, "Removing and adding an element %d times in dynamic array of size: %d, blockSize: %zu, time:\t",numberOfRepetitions, arraySize, blockSize);
        toFile(times, sp);
        fclose(sp);
    }

}




int main (int argc, char **argv)
{
    int c;
    int arraySize = 0;
    size_t blockSize = 0;
    int index = 0;
    int numberOfAdded = 0;
    int numberOfRemoved = 0;
    int numberOfRepetitions = 0;
    int indexOfTried = 0;
    char typeOfAllocation = 's';


    while (1)
    {
        static struct option long_options[] =
                {
                        {"add_number",              required_argument,       0, 'a'},
                        {"block_size",              required_argument,       0, 'b'},
                        {"type_of_allocation",      required_argument,       0, 't'},
                        {"remove_number",           required_argument,       0, 'r'},
                        {"remove_and_add_number",   required_argument,       0, 'm'},
                        {"create_table",            required_argument,       0, 'c'},
                        {"search_element",          required_argument,       0, 's'},
                        {0, 0, 0, 0}
                };

        int option_index = 0;
        c = getopt_long (argc, argv, "a:b:t:r:m:c:s:", long_options, &option_index);
        if (c == -1) break;

        switch (c)
        {
            case 0:
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 't':
                typeOfAllocation = *optarg;
                if(argc <= 7) runAllocationTest(arraySize, blockSize, typeOfAllocation);
                break;

            case 'a':
                numberOfAdded = atoi(optarg);
                runAddTest(arraySize, blockSize, numberOfAdded, typeOfAllocation);
                break;

            case 'r':
                numberOfRemoved = atoi(optarg);
                runRemoveTest(arraySize, blockSize, numberOfRemoved, typeOfAllocation);
                break;

            case 'm':
                indexOfTried = rand()%arraySize;
                numberOfRepetitions = atoi(optarg);
                runRemoveAndAddTest(arraySize, blockSize, indexOfTried, numberOfRepetitions, typeOfAllocation);
                break;

            case 'c':
                arraySize = atoi(optarg);
                break;

            case 's':
                index = atoi(optarg);
                runSearchTest(arraySize, blockSize, index, typeOfAllocation);
                break;

            case 'b':
                blockSize = (size_t) atoi(optarg);
                break;

            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort ();
        }
    }

    return 0;
}

