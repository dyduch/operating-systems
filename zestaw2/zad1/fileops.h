#ifndef ZESTAW2_FILEOPS_H
#define ZESTAW2_FILEOPS_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void generate_data(char*, int, size_t);

char* generate_random_string(size_t);

void copy_data_sys(char*, char*, int, size_t);

void copy_data_lib(char*, char*, int, size_t);

void sort_data_sys(char*, int, size_t);

void sort_data_lib(char*, int, size_t);


#endif