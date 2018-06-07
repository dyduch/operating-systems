//
// Created by rudeu on 16.03.18.
//

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <sys/time.h>
#include "fileops.h"

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

void toFile(struct counted_times times, FILE *sp){

    fprintf(sp, "USER: %fs\t", (double) times.user_diff.tv_sec  + ((double) times.user_diff.tv_usec) / 1000000);
    fprintf(sp, "SYSTEM: %fs\n",(double) times.sys_diff.tv_sec  + ((double) times.sys_diff.tv_usec) / 1000000);
}


int main(int argc, char** argv){

    int c = 0;
    char action = 0;
    char* filepath = " ";
    char* source_file = " ";
    char* dest_file = " ";
    char* type = " ";
    int number = 0;
    size_t record_size = 0;
    srand((unsigned int) time(NULL));

    struct rusage start;
    struct rusage end;
    struct counted_times times;

    FILE* sp;

    while (1)
    {
        static struct option long_options[] =
                {
                        {"generate",                required_argument,       0, 'g'},
                        {"sort",                    required_argument,       0, 's'},
                        {"copy",                    required_argument,       0, 'c'},
                        {"number_of_blocks",        required_argument,       0, 'n'},
                        {"block_size",              required_argument,       0, 'b'},
                        {"destination",             required_argument,       0, 'd'},
                        {"type",                    required_argument,       0, 't'},
                        {0, 0, 0, 0}
                };

        int option_index = 0;
        c = getopt_long (argc, argv, "g:s:c:n:b:d:t:", long_options, &option_index);
        if (c == -1){
            switch (action){
                case 'g':
                    generate_data(filepath, number, record_size);
                    break;
                case 'c':
                    sp = fopen("wyniki.txt" , "a");
                    if(type[0] == 's') {
                        getrusage(RUSAGE_SELF, &start);
                        copy_data_sys(source_file, dest_file, number, record_size);
                        getrusage(RUSAGE_SELF, &end);
                        set_times(start, end, &times);
                        count_diff(&times);
                        fprintf(sp, "Copying %d records of length %zu using %s functions time:\n", number, record_size, type);
                        toFile(times, sp);

                    }
                    else if(type[0] == 'l') {
                        getrusage(RUSAGE_SELF, &start);
                        copy_data_lib(source_file, dest_file, number, record_size);
                        getrusage(RUSAGE_SELF, &end);
                        set_times(start, end, &times);
                        count_diff(&times);
                        fprintf(sp, "Copying %d records of length %zu using %s functions time:\n", number, record_size, type);
                        toFile(times, sp);
                    }
                    fclose(sp);
                    break;
                case 's':
                    sp = fopen("wyniki.txt" , "a");

                    if(type[0] == 's'){
                        getrusage(RUSAGE_SELF, &start);
                        sort_data_sys(filepath, number, record_size);
                        getrusage(RUSAGE_SELF, &end);
                        set_times(start, end, &times);
                        count_diff(&times);
                        fprintf(sp, "Sorting %d records of length %zu using %s functions time:\n", number, record_size, type);
                        toFile(times, sp);
                    }
                    else if(type[0] == 'l'){
                        getrusage(RUSAGE_SELF, &start);
                        sort_data_lib(filepath, number, record_size);
                        getrusage(RUSAGE_SELF, &end);
                        set_times(start, end, &times);
                        count_diff(&times);
                        fprintf(sp, "Sorting %d records of length %zu using %s functions time:\n", number, record_size, type);
                        toFile(times, sp);
                    }
                    fclose(sp);

                    break;
            }
        }

        switch (c)
        {

            case 'g':
                filepath = optarg;
                action = 'g';
                break;

            case 'c':
                source_file = optarg;
                action = 'c';
                break;

            case 'd':
                dest_file = optarg;
                break;

            case 'n':
                number = atoi(optarg);
                break;

            case 'b':
                record_size = (size_t) atoi(optarg);
                break;

            case 's':
                filepath = optarg;
                action = 's';
                break;

            case 't':
                type = optarg;
                break;


            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                return 0;
        }
    }


}