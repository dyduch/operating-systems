#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <sys/time.h>

int i =0;

void allocate_table(){

    int* table = malloc(1024);
    printf("%d\n", i++);
    allocate_table();
    free(table);
}

int main(){

    struct rlimit old_cpu_limits;
    getrlimit(RLIMIT_CPU, &old_cpu_limits);
    struct rlimit old_memory_limits;
    getrlimit(RLIMIT_CPU, &old_memory_limits);


    //sleep(5);
    allocate_table();
    return 0;
}