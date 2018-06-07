

#include "barbershop.h"

const char* shared_mem_path = "/shm";
const char* barber_path = "/barber";
const char* queue_path = "/queue";
const char* client_path = "/client";


void init_queue(queue* queue, int clients_number) {
    queue->wt_room_size = clients_number;
    queue->first = -1;
    queue->last = 0;
    queue->current_client = 0;
}

int is_empty(queue* queue) {
    if (queue->first == -1) return 1;
    else return 0;
}

int is_full(queue* queue) {
    if (queue->first == queue->last) return 1;
    else return 0;
}

pid_t get(queue* queue) {
    if (is_empty(queue) == 1) return -1;

    queue->current_client = queue->clients[queue->first];
    queue->first++;
    if (queue->first == queue->wt_room_size) queue->first = 0;
    if (queue->first == queue->last) queue->first = -1;

    return queue->current_client;
}

int put(queue* queue, pid_t clients_pid) {
    if (is_full(queue) == 1) return -1;
    if (is_empty(queue) == 1)
        queue->first = queue->last = 0;
    queue->clients[queue->last] = clients_pid;
    queue->last++;
    if (queue->last == queue->wt_room_size) queue->last = 0;
    return 0;
}

long get_timestamp() {
    struct timespec tp;
    if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1){
        printf("Getting time failed.");
        exit(EXIT_FAILURE);
    }
    return tp.tv_nsec / 1000;
}