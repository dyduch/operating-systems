//
// Created by rudeu on 20.04.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define MAX_LINE_LENGTH 255


int main(int argc, char** argv) {

    srand((unsigned int) time(NULL));

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <path> <how_many_lines>\n", argv[0]);
        return -1;
    }

    char *path = argv[1];
    int N = (int) strtol(argv[2], NULL, 10);

    FILE* fifo = fopen(path, "w");

    if (fifo == NULL) {
        perror("Couldn't open FIFO\n");
        exit(EXIT_FAILURE);
    }

    printf("Starting slave %d ...\n", getpid());

    char buf[MAX_LINE_LENGTH];
    char date_buf[MAX_LINE_LENGTH];

    for (int i = 0; i < N; i++) {

        FILE* date = popen("date", "r");
        if(date==NULL){
            perror("Something wrong with getting date");
            exit(EXIT_FAILURE);
        }
        fgets(date_buf, MAX_LINE_LENGTH, date);
        fclose(date);
        sprintf(buf, "[SLAVE] PID: %d | %s", getpid(), date_buf);
        printf("Writing to FIFO: %s\n", buf);
        fputs(buf, fifo);

        sleep((unsigned int) (rand() % 4 + 2));
    }

    printf("Done writing to FIFO.\n");

    fclose(fifo);

    return 0;
}