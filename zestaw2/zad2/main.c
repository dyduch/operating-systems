//
// Created by rudeu on 20.03.18.
//

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/stat.h>
#include <time.h>
#include <getopt.h>
#include <ftw.h>
#include <unistd.h>
#include <wait.h>


char compare_dates(time_t date1, time_t date2){  //1 if date1 is later, 0 if the same, -1 if date 2 is later

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

void get_all(char* path, char operand, time_t date){

    if(path == NULL) return;
    int compare_result;
    char* curr_path;
    struct stat* file_stat = malloc(sizeof(struct stat));
    DIR* directory = opendir(path);
    if(directory == NULL) return;
    struct dirent* current = readdir(directory);

    while(current != NULL){
        curr_path = calloc(200, sizeof(char));
        strcat(curr_path, path);
        strcat(curr_path, "/");
        strcat(curr_path, current-> d_name);
        lstat(curr_path, file_stat);
        compare_result = compare_dates(file_stat->st_mtime, date);
        if(strcmp(current->d_name, ".") != 0 && strcmp(current->d_name, "..") != 0){
            if(operand == compare_result) {
                printf("Path: %s", curr_path);
                printf("\tPermissions: ");
                print_permissions(file_stat);
                printf("\tSize: %li, Date: %s\n", file_stat->st_size, time_to_string(file_stat->st_mtime));
            }
            if(S_ISDIR(file_stat->st_mode)){
                pid_t pid = fork();
                if(pid == 0) {
                    get_all(curr_path, operand, date);
                    exit(0);
                }
                else{
                    waitpid(pid, 0, 0);
                }
            }
            current = readdir(directory);
            free(curr_path);
        }
        else current = readdir(directory);
    }
    closedir(directory);
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
    get_all(path, operand, t);

    return 0;
}
