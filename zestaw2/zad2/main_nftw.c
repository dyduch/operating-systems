//
// Created by rudeu on 22.03.18.
//



#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/stat.h>
#include <time.h>
#include <getopt.h>
#include <ftw.h>

time_t date_global;
char operand_global;


char compare_dates(time_t date1, time_t date2){

    double seconds = difftime(date1, date2);
    if (seconds > 0) return '>';
    else if(seconds == 0) return '=';
    else return '<';

}

void print_permissions(struct stat* file_stat){

    printf( (S_ISDIR(file_stat->st_mode)) ? "d" : "-");
    printf( (file_stat->st_mode & S_IRUSR) ? "r" : "-");
    printf( (file_stat->st_mode & S_IWUSR) ? "w" : "-");
    printf( (file_stat->st_mode & S_IXUSR) ? "x" : "-");
    printf( (file_stat->st_mode & S_IRGRP) ? "r" : "-");
    printf( (file_stat->st_mode & S_IWGRP) ? "w" : "-");
    printf( (file_stat->st_mode & S_IXGRP) ? "x" : "-");
    printf( (file_stat->st_mode & S_IROTH) ? "r" : "-");
    printf( (file_stat->st_mode & S_IWOTH) ? "w" : "-");
    printf( (file_stat->st_mode & S_IXOTH) ? "x" : "-");

}

char* time_to_string(time_t st_mtim){

    struct tm *tm;
    char* buf = calloc(200, sizeof(char));
    int buf_size = 200;
    tm = localtime(&st_mtim);
    strftime(buf, (size_t) buf_size, "%d.%m.%Y %H:%M:%S", tm);
    return buf;
}

int nftw_get_all(const char* path, const struct stat* file_stat, int typeflag, struct FTW *ftwbuf){

    int compare_result = compare_dates(file_stat->st_mtime, date_global);
    if(compare_result == operand_global){
        printf("Path: %s", path);
        printf("\tPermissions: ");
        print_permissions((struct stat *) file_stat);
        printf("\tSize: %li, Date: %s\n", file_stat->st_size, time_to_string(file_stat->st_mtime));
        return 0;
    }
    return 0;
}


int main(int argc, char** argv){

    char* path = NULL;
    char operand = 0;
    char* date = NULL;
    int c = 0;

    struct tm tm;
    time_t t;

    while (1)
    {
        static struct option long_options[] =
                {
                        {"path",                    required_argument,       0, 'p'},
                        {"operand",                 required_argument,       0, 'o'},
                        {"date",                    required_argument,       0, 'd'},
                        {0, 0, 0, 0}
                };

        int option_index = 0;
        c = getopt_long (argc, argv, "p:o:d:", long_options, &option_index);
        if (c == -1) break;

        switch (c)
        {

            case 'p':
                path = optarg;
                break;

            case 'o':
                operand = optarg[0];
                break;

            case 'd':

                date = optarg;
                break;

            default:
                return 0;
        }
    }

    if(path == NULL || operand == 0 || date == NULL){
        printf("Something went terribly wrong with your arguments :(\n");
        printf("Path: %s, Operand: %c, Date: %s\n", path, operand, date);
        return 0;
    }

    strptime(date, "%d.%m.%Y %H:%M:%S", &tm);
    t = mktime(&tm);
    date_global = t;
    operand_global = operand;
    nftw(path, nftw_get_all, 15, FTW_PHYS);


    return 0;
}
