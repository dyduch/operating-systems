#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/time.h>


#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(j,l) \
   ({ __typeof__ (j) _j = (j); \
       __typeof__ (l) _l = (l); \
     _j < _l ? _j : _l; })


#define MAX_LINE_LENGTH 70

unsigned int c;
unsigned int W;
unsigned int H;
unsigned int M;


int** I;
float** K;
int** J;
int threads_num;


struct counted_times{
    struct timeval system_start_time, system_end_time, system_timediff,
            user_start_time, user_end_time, user_timediff,
            real_start_time, real_end_time, real_timediff;
};


void set_timediff(struct timeval* start_time, struct timeval* end_time, struct timeval* result){

    if (start_time->tv_sec > end_time->tv_sec)
        timersub(start_time, end_time, result);
    else if (start_time->tv_sec < end_time->tv_sec)
        timersub(end_time, start_time, result);
    else  // start_time.tv_sec == end_time.tv_sec
    {
        if (start_time->tv_usec >= end_time->tv_usec)
            timersub(start_time, end_time, result);
        else
            timersub(end_time, start_time, result);
    }
}

void count_difference(struct counted_times *times){

    set_timediff(&times->system_start_time, &times->system_end_time, &times->system_timediff);
    set_timediff(&times->user_start_time, &times->user_end_time, &times->user_timediff);
    set_timediff(&times->real_start_time, &times->real_end_time, &times->real_timediff);

}

void print_times(struct counted_times times){

    printf("\nREAL: %fs\t", (double) times.real_timediff.tv_sec  + ((double) times.real_timediff.tv_usec) / 1000000);
    printf("USER: %fs\t",(double) times.user_timediff.tv_sec  + ((double) times.user_timediff.tv_usec) / 1000000);
    printf("SYSTEM: %fs\n",(double) times.system_timediff.tv_sec  + ((double) times.system_timediff.tv_usec) / 1000000);
}

void set_times(struct rusage res_start_time, struct rusage res_end_time, struct counted_times* times){

    times->system_start_time = res_start_time.ru_stime;
    times->user_start_time = res_start_time.ru_utime;
    times->system_end_time = res_end_time.ru_stime;
    times->user_end_time = res_end_time.ru_utime;
}

void to_file(struct counted_times times, FILE *sp){

    fprintf(sp, "\nREAL: %fs\t", (double) times.real_timediff.tv_sec  + ((double) times.real_timediff.tv_usec) / 1000000);
    fprintf(sp, "USER: %fs\t", (double) times.user_timediff.tv_sec  + ((double) times.user_timediff.tv_usec) / 1000000);
    fprintf(sp, "SYSTEM: %fs\n",(double) times.system_timediff.tv_sec  + ((double) times.system_timediff.tv_usec) / 1000000);
}

int** allocate_table(){
    int** table = calloc(W, sizeof(int*));
    for(int i = 0; i < W; i++){
        table[i] = calloc(H, sizeof(int));
    }
    return table;
}

float** allocate_filter_table(int size){
    float** table = calloc((size_t) size, sizeof(int*));
    for(int i = 0; i < size; i++){
        table[i] = calloc((size_t) size, sizeof(int));
    }
    return table;
}


void picture_from_file(FILE* file){
    char* buffer = calloc(MAX_LINE_LENGTH, sizeof(char));
    I = NULL;
    int i = 1;
    int j = 0;
    while(fgets(buffer, MAX_LINE_LENGTH, file) != NULL){
        char* value;
        if(i == 1) {
            i++;
            continue;
        }
        if(i == 2){
            value = strsep(&buffer, " ");
            W = (unsigned int) strtol(value, NULL, 10);
            value = strsep(&buffer, "\n");
            H = (unsigned int) strtol(value, NULL, 10);
            I = allocate_table();
        }
        else if(i == 3){
            value = strsep(&buffer, " \n");
            M = (unsigned int) strtol(value, NULL, 10);
        }
        else{
            while((value=strsep(&buffer, "  \n")) != NULL){
                if(strcmp(value, "") == 0) continue;
                int w = j/W;
                int h = j%H;
                I[w][h] = (unsigned int) strtol(value, NULL, 10);
                j++;
            }
        }
        //free(&buffer);
        buffer = calloc(MAX_LINE_LENGTH, sizeof(char));
        i++;
    }
}


void filter_from_file(FILE* file){
    char* buffer = calloc(MAX_LINE_LENGTH, sizeof(char));
    K = NULL;
    int i = 1;
    int j = 0;
    while(fgets(buffer, MAX_LINE_LENGTH, file) != NULL){
        char* value;
        if(i == 1) {
            value = strsep(&buffer, " ");
            c = (unsigned int) strtol(value, NULL, 10);
            K = allocate_filter_table(c);
        }
        else{
            while((value=strsep(&buffer, "  \n")) != NULL){
                if(strcmp(value, "") == 0) continue;
                int w = j/c;
                int h = j%c;
                K[w][h] = strtof(value, NULL);
                j++;
            }
        }
        buffer = calloc(MAX_LINE_LENGTH, sizeof(char));

        i++;
    }
}

void result_to_file(FILE* file){
    char* buffer = calloc(MAX_LINE_LENGTH, sizeof(char));
    sprintf(buffer, "P2\n");
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    sprintf(buffer, "%d %d\n", W, H);
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    sprintf(buffer, "%d\n", M);
    fwrite(buffer, sizeof(char),  strlen(buffer), file);
    int num_counter = 0;
    for(int i = 0; i < W; i++){
        char* bufcpy = calloc(MAX_LINE_LENGTH, sizeof(char));
        for(int j = 0; j < H; j++){
            sprintf(buffer, "%d  ", J[i][j]);
            strcat(bufcpy, buffer);
            num_counter++;
            if(num_counter == 12 || j == W-1){
                strcat(bufcpy, " \n");
                fwrite(bufcpy, sizeof(char), strlen(bufcpy), file);
                num_counter = 0;
                free(bufcpy);
                bufcpy = calloc(MAX_LINE_LENGTH, sizeof(char));
            }
        }
        free(bufcpy);
    }
    free(buffer);
}


void* filter(void* args){

    long tid = (long) args;
    int start_width = (int) ((tid * W) / threads_num);
    int end_width = (int) (((tid + 1) * W) / threads_num);
    int c_ceil = (int) ceil(c/2);

    for(int x = start_width; x < end_width; x++){
        for(int y = 0; y < H; y++){
            double s_xy = 0;
            for(int i = 0; i < c; i++){
                for(int j = 0; j < c; j++){
                    int ix = min((W-1), max(0, x - c_ceil + i));
                    int iy = min((H-1), max(0, y - c_ceil + j));

                    s_xy += I[ix][iy] * K[i][j];
                }
            }
            J[x][y] = (int) round(s_xy);
        }
    }
    pthread_exit(NULL);
    return NULL;
}



int main(int argc, char** argv) {

    if(argc != 5) {
        printf("Wrong number of arguments!\n");
        exit(EXIT_FAILURE);
    }

    char* input_file_path = argv[2];
    char* filter_file_path = argv[3];
    char* output_file_path = argv[4];

    struct rusage res_start_time;
    struct rusage res_end_time;
    struct counted_times times;



    FILE* pic_file = fopen(input_file_path, "r");
    picture_from_file(pic_file);

    FILE* filter_file = fopen(filter_file_path, "r");
    filter_from_file(filter_file);

    FILE* result_file = fopen(output_file_path, "w+");
    J = allocate_table();

    threads_num = (int) strtol(argv[1], NULL, 10);
    pthread_t* threads = calloc((size_t) threads_num, sizeof(pthread_t));

    gettimeofday(&times.real_start_time, NULL);
    getrusage(RUSAGE_SELF, &res_start_time);
    for(long i = 0; i < threads_num; i++) {
        pthread_create(&threads[i], NULL, filter, (void *) i);
    }
    for(long i = 0; i < threads_num; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&times.real_end_time, NULL);
    getrusage(RUSAGE_SELF, &res_end_time);

    set_times(res_start_time, res_end_time, &times);
    count_difference(&times);
    FILE* sp;
    sp = fopen("Times.txt", "a+");
    fprintf(sp, "Filtering with filter of size %d, with usage of %d threads time:\t\n", c, threads_num);
    to_file(times, sp);
    result_to_file(result_file);
    fclose(pic_file);
    fclose(filter_file);
    fclose(result_file);
    fclose(sp);
    pthread_exit(NULL);
}
