//
// Created by rudeu on 10.05.18.
//

#ifndef ZESTAW7_BARBERSHOP_H
#define ZESTAW7_BARBERSHOP_H


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define MAX_QUEUE_SIZE 512
#define PROJECT_ID 420

#define SEM_BARBER 0
#define SEM_QUEUE 1
#define SEM_CLIENT 2

typedef struct queue {
    int wt_room_size;
    int first;
    int last;
    pid_t current_client;
    pid_t clients[MAX_QUEUE_SIZE];
} queue;



int is_empty(queue* queue);

int is_full(queue* queue);

void init_queue(queue* queue, int clients_number);

pid_t get(queue* queue);

int put(queue* queue, pid_t clients_pid);

long get_timestamp();




#endif //ZESTAW7_BARBERSHOP_H
