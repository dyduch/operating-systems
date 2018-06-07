
#include "barbershop.h"

queue* q = NULL;
sem_t* SEM_BARBER;
sem_t* SEM_QUEUE;
sem_t* SEM_CLIENT;


void default_signal_handler(int sig_num) {
    printf("%d received.", sig_num);
    exit(EXIT_SUCCESS);
}


void delete_all() {
    if (munmap(q, sizeof(q)) == -1) printf("| %lo | [BARBER] detaching queue shared memory failed.\n", get_timestamp());
    else printf("| %lo | [BARBER] detached queue shared memory.\n", get_timestamp());

    if (shm_unlink(shared_mem_path) == -1) printf("| %lo | [BARBER] deleting queue shared memory failed.\n", get_timestamp());
    else printf("| %lo | [BARBER] deleted queue shared memory.\n", get_timestamp());

    if (sem_close(SEM_BARBER) == -1) printf("| %lo | [BARBER] closing semaphore SEM_BARBER failed.", get_timestamp());
    else printf("| %lo | [BARBER] closed semaphore SEM_BARBER.\n", get_timestamp());

    if (sem_unlink(barber_path) == -1) printf("| %lo | [BARBER] deleting barber semaphores failed.", get_timestamp());
    else printf("| %lo | [BARBER] deleted barber semaphores.\n", get_timestamp());

    if (sem_close(SEM_QUEUE) == -1) printf("| %lo | [BARBER] closing semaphore SEM_QUEUE failed.", get_timestamp());
    else printf("| %lo | [BARBER] closed semaphore SEM_QUEUE.\n", get_timestamp());

    if (sem_unlink(queue_path) == -1) printf("| %lo | [BARBER] deleting queue semaphores failed.", get_timestamp());
    else printf("| %lo | [BARBER] deleted queue semaphores.\n", get_timestamp());

    if (sem_close(SEM_CLIENT) == -1) printf("| %lo | [BARBER] closing semaphore SEM_CLIENT failed.", get_timestamp());
    else printf("| %lo | [BARBER] closed semaphore SEM_CLIENT.\n", get_timestamp());

    if (sem_unlink(client_path) == -1) printf("| %lo | [BARBER] deleting client semaphores failed.", get_timestamp());
    else printf("| %lo | [BARBER] deleted client semaphores.\n", get_timestamp());
}


pid_t get_on_chair() {

    if(sem_wait(SEM_QUEUE) == -1){
        printf("| %lo | [BARBER] acquiring SEM_QUEUE semaphore failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }
    pid_t curr = q->current_client;
    if(sem_post(SEM_QUEUE) == -1){
        printf("| %lo | [BARBER] releasing SEM_QUEUE semaphore failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }
    return curr;
}

void cut(pid_t pid) {
    printf("| %lo | [BARBER] preparing to cut %d\n", get_timestamp(), pid);
    fflush(stdout);
    kill(pid, SIGUSR1);
    printf("| %lo | [BARBER] finished cutting %d\n", get_timestamp(), pid);
    fflush(stdout);
}


int main(int argc, char *argv[]) {
    if (argc != 2){
        printf("[BARBER] Wrong number of arguments.");
        exit(EXIT_FAILURE);
    }
    int chair_number = (int) strtol(argv[1], '\0', 10);
    if (chair_number < 1 || chair_number > MAX_QUEUE_SIZE) {
        printf("[BARBER] Wrong number of chairs.");
        exit(EXIT_FAILURE);
    }

    if (atexit(delete_all) == -1){
        printf("| %lo | [BARBER] setting atexit failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }
    if (signal(SIGINT, default_signal_handler) == SIG_ERR) {
        printf("| %lo | [BARBER] setting SIGINT handler failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    if (signal(SIGTERM, default_signal_handler) == SIG_ERR) {
        printf("| %lo | [BARBER] setting SIGTERM handler failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    int shmid = shm_open(shared_mem_path, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shmid == -1) {
        printf("| %lo | [BARBER] creating shared memory failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    if(ftruncate(shmid, sizeof(q)) == -1){
        printf("| %lo | [BARBER] truncating shared memory failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    if ((q = mmap(NULL, sizeof(q), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0)) == (void *) (-1)){
        printf("| %lo | [BARBER] attaching shared memory failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    init_queue(q, chair_number);

    SEM_BARBER = sem_open(barber_path, O_CREAT | O_EXCL | O_RDWR, 0666, 0);
    if (SEM_BARBER == SEM_FAILED) {
        printf("| %lo | [BARBER] creating SEM_BARBER semaphore failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }
    SEM_QUEUE = sem_open(queue_path, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    if (SEM_QUEUE == SEM_FAILED) {
        printf("| %lo | [BARBER] creating SEM_QUEUE semaphore failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    SEM_CLIENT = sem_open(client_path, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    if (SEM_CLIENT == SEM_FAILED) {
        printf("| %lo | [BARBER] creating SEM_CLIENT semaphore failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }


    while (1) {
        if(sem_wait(SEM_BARBER) == -1) {
            printf("| %lo | [BARBER] acquiring SEM_BARBER semaphore failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }
        if(sem_post(SEM_BARBER) == -1){
            printf("| %lo | [BARBER] setting BARBER as wake failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }

        pid_t current_client = get_on_chair();
        cut(current_client);

        while (1) {
           if(sem_wait(SEM_QUEUE) == -1) {
               printf("| %lo | [BARBER] acquiring SEM_QUEUE semaphore failed.", get_timestamp());
               exit(EXIT_FAILURE);
           }
           current_client = get(q);

           if(current_client != -1) {
               if (sem_post(SEM_QUEUE) == -1) {
                   printf("| %lo | [BARBER] releasing SEM_QUEUE semaphore failed.", get_timestamp());
                   exit(EXIT_FAILURE);
               }
               cut(current_client);
           }
           else {
               printf("| %lo | [BARBER] going to sleep.", get_timestamp());
               fflush(stdout);
               if(sem_wait(SEM_QUEUE) == -1) {
                   printf("| %lo | [BARBER] acquiring SEM_BARBER semaphore failed.", get_timestamp());
                   exit(EXIT_FAILURE);
               }

               if (sem_post(SEM_QUEUE) == -1) {
                   printf("| %lo | [BARBER] releasing SEM_QUEUE semaphore failed.", get_timestamp());
                   exit(EXIT_FAILURE);
               }
               break;
           }
        }
    }

    return 0;
}



