//
// Created by rudeu on 13.04.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

size_t command_length = 2048;
const size_t max_arguments_number = 64;
const int max_commands_in_line = 10;

char** allocate_table(){
    char** arguments = calloc(max_arguments_number, sizeof(char*));
    for(int i = 0; i < max_arguments_number; i++){
        arguments[i]=calloc(command_length, sizeof(char));
    }
    return arguments;
}

int get_command_args(char** single_args, char* single_command){

    char* parameter;
    int command_count = 0;
    char* single_command_copy = strdup(single_command);

    while( (parameter = strsep(&single_command_copy," \n\t")) != NULL ) {

        if(strcmp(parameter, "")) {
            single_args[command_count] = parameter;
            command_count++;
        }
    }
    single_args[command_count] = NULL;
    free(single_command_copy);
    return command_count;
}


int get_single_commands(char** single_commands, char* line){

    char* parameter;
    int command_count = 0;

    char* line_cpy = strdup(line);

    while( (parameter = strsep(&line_cpy,"|")) != NULL ) {
        if(command_count == max_commands_in_line){
            printf("Too many commands in one line for: %s\n", single_commands[0]);
            return -1;
        }
        single_commands[command_count] = parameter;
        command_count++;
    }

    return command_count;
}



void close_pipes(int* pipes, int descriptors){
    for(int i = 0; i < descriptors; i++){
        close(pipes[i]);
    }
}


void execute_command(char** single_commands, int commands_count, int current_line){

    int descriptors = 2 * (commands_count - 1);
    int *pipes = calloc((size_t) descriptors, sizeof(int));
    for (int i = 0; i < descriptors; i += 2) {
        pipe(pipes + i);
    }


    int status;

    char **args = allocate_table();

    for (int i = 0; i < commands_count; i++) {

        get_command_args(args, single_commands[i]);

        if (fork() == 0) {
            if (i != 0) {
                if(dup2(pipes[2 * (i - 1)], 0) < 0){
                    printf("Error with dup2..\n");
                }
            }
            if (i != commands_count-1) {
                if(dup2(pipes[2 * i + 1], 1) < 0){
                    printf("Error with dup2..\n");
                }
            }

            close_pipes(pipes, descriptors);
            execvp(*args, args);
        }
    }
    close_pipes(pipes, descriptors);

    for (int i = 0; i < commands_count; i++)
        wait(&status);

}


int main(int argc, char** argv){

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return -1;
    }

    char* line;
    char** single_commands;

    single_commands = allocate_table();
    line = calloc(command_length, sizeof(char));

    FILE* file = fopen(argv[1], "r");

    if(file == NULL){
        printf("Could not open the file.\n");
        return -1;
    }

    int current_line = 0;

    while(fgets(line, (int) command_length, file) != NULL){

        char* line_tmp = calloc(command_length, sizeof(char));
        strcpy(line_tmp, line);

        int command_count = get_single_commands(single_commands, line);

        if(command_count < 0){
            printf("Something went wrong when parsing arguments.");
            return -1;
        }

        printf("\nExecuting command: %s\n", line_tmp);
        execute_command(single_commands, command_count, current_line);

        for(int i = 0; i < command_count; i++){
            single_commands[i] = "\0";
        }
        free(line_tmp);
    }

    fclose(file);
    free(line);
    free(single_commands);

    return 0;
}