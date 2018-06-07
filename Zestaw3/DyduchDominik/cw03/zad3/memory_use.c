#include <stdio.h>
#include <stdlib.h>

int i =0;

void allocate_table(){

    int* table = malloc(1024);
    printf("%d\n", i++);
    allocate_table();
    free(table);
}

int main(){

    allocate_table();
    return 0;
}