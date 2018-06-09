//
// Created by rudeu on 09.06.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <memory.h>

int P, K, N, L, search_mode, print_mode, nk;
char filename[FILENAME_MAX];

char** buffer;
FILE* file;

int done = 0;

pthread_t* producer_threads;
pthread_t* consumer_threads;

int production_index = 0;
int consumption_index = 0;

pthread_mutex_t* mutexes;

pthread_cond_t r_cond;
pthread_cond_t w_cond;

void set_parameters(FILE* config){
    fscanf(config, "%d %d %d %s %d %d %d %d", &P, &K, &N, filename, &L, &search_mode, &print_mode, &nk);
}

void sig_handler(int signo){
    if(signo == SIGINT) printf("Received signal SIGINT!\n");
    else if(signo == SIGALRM) printf("Received signal SIGALRM!\n");
    printf("Canceling threads...");
    for(int i = 0 ; i < P; i++){
        pthread_cancel(producer_threads[i]);
    }
    for(int i = 0 ; i < K; i++){
        pthread_cancel(consumer_threads[i]);
    }
    exit(EXIT_SUCCESS);
}

int length_search(int line_length){
    return search_mode == (line_length > L ? 1 : line_length < L ? -1 : 0);
}

void init(){
    mutexes = calloc((size_t) (N + 2), sizeof(pthread_mutex_t));
    for(int i = 0; i < N+2; i++){
        pthread_mutex_init(&mutexes[i], NULL);
    }

    pthread_cond_init(&w_cond, NULL);
    pthread_cond_init(&r_cond, NULL);

    producer_threads = calloc((size_t) P, sizeof(pthread_t));
    consumer_threads = calloc((size_t) K, sizeof(pthread_t));
}

void destroy(){
    for (int i = 0; i < N; ++i)
        if (buffer[i]) free(buffer[i]);
    free(buffer);

    for (int j = 0; j < N + 2; ++j)
        pthread_mutex_destroy(&mutexes[j]);
    free(mutexes);

    pthread_cond_destroy(&w_cond);
    pthread_cond_destroy(&r_cond);
}

void* producer_routine(void* arg){
    int index;
    char line[LINE_MAX];
    while (fgets(line, LINE_MAX, file) != NULL) {
        if(print_mode) fprintf(stderr, "Producer[%ld]: taking file line\n", pthread_self());
        pthread_mutex_lock(&mutexes[N]);

        while (buffer[production_index] != NULL)
            pthread_cond_wait(&w_cond, &mutexes[N]);

        index = production_index;
        if(print_mode) fprintf(stderr, "Producer[%ld]: taking buffer index (%d)\n",  pthread_self(), index);
        production_index = (production_index + 1) % N;


        pthread_mutex_lock(&mutexes[index]);

        buffer[index] = malloc((strlen(line) + 1) * sizeof(char));
        strcpy(buffer[index], line);
        if(print_mode) fprintf(stderr, "Producer[%ld]: line copied to buffer at index (%d)\n",  pthread_self(), index);

        pthread_cond_broadcast(&r_cond);
        pthread_mutex_unlock(&mutexes[index]);
        pthread_mutex_unlock(&mutexes[N]);
    }
    if(print_mode) fprintf(stderr, "Producer[%ld]: Done\n", pthread_self());
    return NULL;
}

void* consumer_routine(void* arg) {
    char *line;
    int index;
    while (1) {
        pthread_mutex_lock(&mutexes[N + 1]);

        while (buffer[consumption_index] == NULL) {
            if (done) {
                pthread_mutex_unlock(&mutexes[N + 1]);
                if(print_mode) fprintf(stderr, "Consumer[%ld]: Done \n",  pthread_self());
                return NULL;
            }
            pthread_cond_wait(&r_cond, &mutexes[N + 1]);
        }

        index = consumption_index;
        if(print_mode) fprintf(stderr, "Consumer[%ld]: taking buffer index (%d)\n",  pthread_self(), index);
        consumption_index = (consumption_index + 1) % N;

        pthread_mutex_lock(&mutexes[index]);
        pthread_mutex_unlock(&mutexes[N + 1]);

        line = buffer[index];
        buffer[index] = NULL;
        if(print_mode) fprintf(stderr, "Consumer[%ld]: taking line from buffer at index (%d)\n",  pthread_self(), index);

        pthread_cond_broadcast(&w_cond);
        pthread_mutex_unlock(&mutexes[index]);

        if(length_search((int) strlen(line))){
            if(print_mode) fprintf(stderr, "Consumer[%ld]: found line with length %d %c %d\n",
                                pthread_self(), (int) strlen(line), search_mode == 1 ? '>' : search_mode == -1 ? '<' : '=', L);
            fprintf(stderr, "Consumer[%ld]: Index(%d), %s",  pthread_self(), index, line);
        }
        free(line);
        usleep(10);
    }
}

void run_threads(){
    for (int p = 0; p < P; ++p)
        pthread_create(&producer_threads[p], NULL, producer_routine, NULL);
    for (int k = 0; k < K; ++k)
        pthread_create(&consumer_threads[k], NULL, consumer_routine, NULL);
    if (nk > 0) alarm((unsigned int) nk);
}

void join_threads(){
    for (int p = 0; p < P; ++p)
        pthread_join(producer_threads[p], NULL);
    done = 1;
    pthread_cond_broadcast(&r_cond);
    for (int k = 0; k < K; ++k)
        pthread_join(consumer_threads[k], NULL);
}

int main(int argc, char** argv){
    if(argc != 2){
        printf("Wrong number of arguments!\n");
        exit(EXIT_FAILURE);
    }
    FILE* config = fopen(argv[1], "r");
    if(config == NULL){
        printf("Couldn't open config file!\n");
        exit(EXIT_FAILURE);
    }

    set_parameters(config);
    fclose(config);

    signal(SIGINT, sig_handler);
    if(nk > 0)
        signal(SIGALRM, sig_handler);

    file = fopen(argv[1], "r");
    if(file == NULL){
        printf("Couldn't open config file!\n");
        exit(EXIT_FAILURE);
    }

    buffer = calloc((size_t) N, sizeof(char*));

    init();

    run_threads();

    join_threads();

    fclose(file);

    destroy();

    return 0;
}