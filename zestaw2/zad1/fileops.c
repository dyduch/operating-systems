#include "fileops.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <memory.h>

void generate_data(char* filepath, int number, size_t record_length){

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int file = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, mode);
    char* string;
    char* newline = calloc(1, sizeof(char));
    newline[0] = '\n';

    if(file == -1) return;

    for(int i = 0; i < number; i++){
        string = generate_random_string(record_length-1);
        write(file, string, record_length-1);
        write(file, newline, 1);
    }
    close(file);
}



void copy_data_sys(char* source, char* destination, int number, size_t record_size){

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int source_file = open(source, O_RDONLY, mode);
    int dest_file = open(destination, O_WRONLY | O_CREAT | O_TRUNC, mode);
    char* buf = calloc(record_size, sizeof(char));


    if(source_file == -1 || dest_file == -1) return;

    for(int i = 0; i < number; i++){
        if(read(source_file, buf, record_size) > 0){
            write(dest_file, buf, record_size);
        }
    }
    close(source_file);
    close(dest_file);
}


void copy_data_lib(char* source, char* destination, int number, size_t record_size){

    FILE* src;
    FILE* dest;
    char* buf = calloc(record_size, sizeof(char));

    src = fopen(source, "r");
    dest = fopen(destination, "w");

    if(src == NULL || dest == NULL) return;

    for(int i = 0; i < number; i++) {
        fread(buf, sizeof(char), record_size, src);
        fwrite(buf, sizeof(char), record_size, dest);
    }
    fclose(src);
    fclose(dest);

    free(buf);
}


void sort_data_sys(char* filepath, int number, size_t record_size){

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int file = open(filepath, O_RDWR | O_CREAT, mode);

    if(file == -1) return;

    char* record1 = calloc(record_size, sizeof(char));
    char* record2 = calloc(record_size, sizeof(char));

    for(int i = 0; i < number; i++){

        lseek(file, i*record_size, 0);
        read(file, record1, record_size);

        for(int j = 0; j < i; j++){

            lseek(file, j*record_size, 0);
            read(file, record2, record_size);

            if(record2[0] > record1[0]){
                lseek(file, j*record_size, 0);
                write(file, record1, record_size);
                lseek(file, i*record_size, 0);
                write(file, record2, record_size);

                char* temp = record1;
                record1 = record2;
                record2 = temp;
            }
        }
    }

    free(record1);
    free(record2);

    close(file);
}

void sort_data_lib(char* filepath, int number, size_t record_size){

    FILE* fileptr;

    fileptr = fopen(filepath, "r+");

    if(fileptr == NULL) return;

    char* record1 = calloc(record_size, sizeof(char));
    char* record2 = calloc(record_size, sizeof(char));
    char* temp = calloc(record_size, sizeof(char));

    for(int i = 0; i < number; i++){

        fseek(fileptr, i*record_size, 0);
        fread(record1, sizeof(char), record_size, fileptr);

        for(int j = 0; j < i; j++){

            fseek(fileptr, j*record_size, 0);
            fread(record2, sizeof(char), record_size, fileptr);

            if(record2[0] > record1[0]){
                fseek(fileptr, j*record_size, 0);
                fwrite(record1, sizeof(char), record_size, fileptr);
                fseek(fileptr, i*record_size, 0);
                fwrite(record2, sizeof(char), record_size, fileptr);

                temp = record1;
                record1 = record2;
                record2 = temp;
            }
        }
    }

    free(record1);
    free(record2);

    fclose(fileptr);

}


char* generate_random_string(size_t record_length){

    static const char charset[] = "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    char* random_string = calloc(record_length, sizeof(char));
    for (int i = 0; i < record_length; i++){
        random_string[i] = charset[rand() % (sizeof(charset) -1)];
    }

    random_string[record_length] = '\0';
    return random_string;
}