//
// Created by rudeu on 05.04.18.
//

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <sys/time.h>


struct counted_times{
    struct timeval sys_start, sys_end, sys_diff,
            user_start, user_end, user_diff;
};

void set_diff(struct timeval* start, struct timeval* end, struct timeval* result){

    if (start->tv_sec > end->tv_sec)
        timersub(start, end, result);
    else if (start->tv_sec < end->tv_sec)
        timersub(end, start, result);
    else  // start.tv_sec == end.tv_sec
    {
        if (start->tv_usec >= end->tv_usec)
            timersub(start, end, result);
        else
            timersub(end, start, result);
    }
}

void count_diff(struct counted_times *times){

    set_diff(&times->sys_start, &times->sys_end, &times->sys_diff);
    set_diff(&times->user_start, &times->user_end, &times->user_diff);

}

void set_times(struct rusage start, struct rusage end, struct counted_times* times){

    times->sys_start = start.ru_stime;
    times->user_start = start.ru_utime;
    times->sys_end = end.ru_stime;
    times->user_end = end.ru_utime;
}

void print_times(struct counted_times times){

    printf("USER: %fs\t", (double) times.user_diff.tv_sec  + ((double) times.user_diff.tv_usec) / 1000000);
    printf("SYSTEM: %fs\n",(double) times.sys_diff.tv_sec  + ((double) times.sys_diff.tv_usec) / 1000000);
}

int set_limits(unsigned long cpu_time, unsigned long memory_size){

    struct rlimit cpu_limit;
    cpu_limit.rlim_max = (rlim_t) cpu_time;
    cpu_limit.rlim_cur = (rlim_t) cpu_time;
    if(setrlimit(RLIMIT_CPU, &cpu_limit) != 0){
        printf("Cannot set this limit.");
        return -1;
    }
    struct rlimit memory_limit;
    memory_limit.rlim_max = (rlim_t) (memory_size * 1024 * 1024);
    memory_limit.rlim_cur = (rlim_t) (memory_size * 1024 * 1024);
    if(setrlimit(RLIMIT_AS, &memory_limit) != 0){
        printf("Cannot set this limit.");
        return -1;
    }

    return 0;
}

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

    unsigned long cpu_time;
    unsigned long memory_size;

    if (argc < 2 || argc > 4) {
        fprintf(stderr, "Usage: %s <file> <cpu_time>(optional) <memory_usage>(optional)\n", argv[0]);
        return -1;
    }

    struct rlimit old_cpu_limits;
    getrlimit(RLIMIT_CPU, &old_cpu_limits);
    struct rlimit old_memory_limits;
    getrlimit(RLIMIT_CPU, &old_memory_limits);


    if(argv[2] != NULL && argv[3] != NULL){
        cpu_time = (unsigned long) strtol(argv[2], NULL, 10);
        memory_size = (unsigned long) strtol(argv[3], NULL, 10);
    }
    else{
        cpu_time = old_cpu_limits.rlim_max;
        memory_size = old_memory_limits.rlim_max / 1024 / 1024;
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


    struct rusage start;
    struct rusage end;
    struct counted_times times;

    while(getline(&command, &command_length, file) != -1){

        getrusage(RUSAGE_CHILDREN, &start);

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
            if(set_limits(cpu_time, memory_size) != 0){
                perror("Setting limits gone wrong.");
            }
            execvp(arguments[0], arguments);
            exit(1);
        }

        wait(&status);
        if(status != 0){
            printf("Command execution error: %s\n", command_tmp);
            return -1;
        }

        getrusage(RUSAGE_CHILDREN, &end);
        set_times(start, end, &times);
        count_diff(&times);

        printf("Command: %s usage times: \n", command_tmp);
        print_times(times);

        free(command_tmp);

        for(int i = 0; i < arg_count; i++){
            arguments[i] = "\0";
        }
    }

    fclose(file);
    free(command);
    free(arguments);

    return 0;
}