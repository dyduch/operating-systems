//
// Created by rudeu on 05.04.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char** allocate_table(size_t max_arguments_number, size_t command_length){
    char** arguments = calloc(max_arguments_number, sizeof(char*));
    for(int i = 0; i < max_arguments_number; i++){
        arguments[i]=calloc(command_length, sizeof(char));
    }
    return arguments;
}

int get_arguments(char** arguments, char* command, size_t max_arguments_number){

    char* parameter;
    int arg_count = 0;

    while( (parameter = strsep(&command," \n\t")) != NULL ) {
        if(arg_count == max_arguments_number){
            printf("Too many arguments for: %s\n", arguments[0]);
            return -1;
        }
        arguments[arg_count] = parameter;
        arg_count++;
    }
    arguments[arg_count-1] = NULL;
    return arg_count;
}

int main(int argc, char** argv){

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return -1;
    }

    size_t command_length = 255;
    size_t max_arguments_number = 64;

    char* command;
    char** arguments;
    int status = 0;

    arguments = allocate_table(max_arguments_number, command_length);
    command = calloc(command_length, sizeof(char));

    FILE* file = fopen(argv[1], "r");

    if(file == NULL){
        printf("Could not open the file.\n");
        return -1;
    }

    while(getline(&command, &command_length, file) != -1){

        char* command_tmp = calloc(command_length, sizeof(char));
        strcpy(command_tmp, command);

        if(command[strlen(command)-1] != '\n'){
            command[strlen(command)] = '\n';
        }

        int arg_count = get_arguments(arguments, command, max_arguments_number);

        if(arg_count == -1){
            printf("Something went wrong when parsing arguments.");
            return -1;
        }


        pid_t pid = fork();
        if(pid == -1){
            perror("Child process was not created\n");
        }
        else if(pid == 0){
            execvp(arguments[0], arguments);
            exit(1);
        }

        wait(&status);
        if(status){
            printf("Error while executing: %s\n", command_tmp);
            return -1;
        }
        for(int i = 0; i < arg_count; i++){
            arguments[i] = "\0";
        }

        free(command_tmp);
    }

    fclose(file);
    free(command);
    free(arguments);

    return 0;
}