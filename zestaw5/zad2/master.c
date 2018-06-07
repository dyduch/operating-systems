//
// Created by rudeu on 19.04.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LINE_LENGTH 256


int main(int argc, char** argv){

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return -1;
    }

    char* path = argv[1];

    if(mkfifo(path, S_IWUSR | S_IWGRP | S_IRGRP | S_IRUSR) == -1) {
        perror("Couldn't create FIFO\n");
        exit(EXIT_FAILURE);
    }

    FILE* fifo = fopen(path, "r");
    if(fifo == NULL){
        perror("Couldn't open FIFO\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("FIFO is open.\n");

    char buf[MAX_LINE_LENGTH];
    printf("[MASTER] PID: %d\n", getpid());

    while(fgets(buf, MAX_LINE_LENGTH, fifo)){
        printf("Reading from fifo: %s\n", buf);
        fflush(stdout);
    }

    fclose(fifo);

    unlink(path);

    return 0;
}