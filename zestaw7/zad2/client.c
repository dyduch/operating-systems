

#include "barbershop.h"

queue *q = NULL;
sem_t* SEM_BARBER;
sem_t* SEM_QUEUE;
sem_t* SEM_CLIENT;
volatile int client_counter = 0;
sigset_t f_mask;


void sig_handler(int signo) {
    client_counter++;
}

void default_signal_handler(int signo) {
    printf("%d received\n", signo);
    exit(EXIT_SUCCESS);
}


void delete_all(void) {
    if (munmap(q, sizeof(q)) == -1) printf("| %lo | [CLIENT] detaching queue shared memory failed.\n", get_timestamp());
    else printf("| %lo | [CLIENT] detached queue shared memory.\n", get_timestamp());

    if (sem_close(SEM_BARBER) == -1) printf("| %lo | [CLIENT] closing semaphore SEM_BARBER failed.", get_timestamp());
    else printf("| %lo | [CLIENT] closed semaphore SEM_BARBER.\n", get_timestamp());

    if (sem_close(SEM_QUEUE) == -1) printf("| %lo | [CLIENT] closing semaphore SEM_QUEUE failed.", get_timestamp());
    else printf("| %lo | [CLIENT] closed semaphore SEM_QUEUE.\n", get_timestamp());

    if (sem_close(SEM_CLIENT) == -1) printf("| %lo | [CLIENT] closing semaphore SEM_CLIENT failed.", get_timestamp());
    else printf("| %lo | [CLIENT] closed semaphore SEM_CLIENT.\n", get_timestamp());

}


int get_on_chair() {
    int barber_status;
    if (sem_getvalue(SEM_BARBER, &barber_status) == -1)  {
        printf("| %lo | [CLIENTS] getting value of SEM_BARBER semaphore failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    pid_t client_pid = getpid();

    if (barber_status == 0) {
        if (sem_post(SEM_BARBER) == -1) {
            printf("| %lo | [CLIENTS] awakening barber failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }
        printf("| %lo | [CLIENTS] Client %d has awakened barber!\n", get_timestamp(), client_pid);
        fflush(stdout);
        q->current_client = client_pid;
        return 1;

    }
    else {
        int res = put(q, client_pid);
        if (res == -1) {
            printf("| %lo | [CLIENTS] Client %d couldn't find free place.\n", get_timestamp(), client_pid);
            fflush(stdout);
            return -1;
        }
        else {
            printf("| %lo | [CLIENTS] Client %d took place in the waiting room.\n", get_timestamp(), client_pid);
            fflush(stdout);
            return 0;
        }
    }
}


void get_cut(int shavings_number) {
    while (client_counter < shavings_number) {

        if (sem_wait(SEM_CLIENT) == -1) {
            printf("| %lo | [CLIENTS] acquiring SEM_CLIENT semaphore failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }

        if (sem_wait(SEM_QUEUE) == -1) {
            printf("| %lo | [CLIENTS] acquiring SEM_QUEUE semaphore failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }

        int res = get_on_chair();


        if (sem_post(SEM_QUEUE) == -1) {
            printf("| %lo | [CLIENTS] releasing SEM_QUEUE semaphore failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }



        if (sem_post(SEM_CLIENT) == -1) {
            printf("| %lo | [CLIENTS] releasing SEM_CLIENT semaphore failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }


        if (res != -1) {
            sigsuspend(&f_mask);
            printf("| %lo | [CLIENTS] Client %d just got shaved.\n", get_timestamp(), getpid());
            fflush(stdout);
        }
    }
}


int main(int argc, char **argv) {
    if (argc != 3) {
        printf("[CLIENTS] wrong arguments number.");
        exit(EXIT_FAILURE);
    }

    int clients_number = (int) strtol(argv[1], '\0', 10);
    int shavings_number = (int) strtol(argv[2], '\0', 10);
    if (clients_number < 1) {
        printf("[CLIENTS] Wrong number of clients.");
        exit(EXIT_FAILURE);
    }

    if (shavings_number < 1) {
        printf("[CLIENTS] Wrong number of shavings.");
        exit(EXIT_FAILURE);
    }


    if (atexit(delete_all) == -1) {
        printf("| %lo | [CLIENTS] setting atexit failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    if (signal(SIGINT, default_signal_handler) == SIG_ERR) {
        printf("| %lo | [CLIENTS] setting SIGINT handler failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTERM, default_signal_handler) == SIG_ERR) {
        printf("| %lo | [CLIENTS] setting SIGINT handler failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }


    if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
        printf("| %lo | [CLIENTS] setting SIGUSR1 handler failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }


    int shmid = shm_open(shared_mem_path, O_RDWR, 0);
    if (shmid == -1) {
        printf("| %lo | [CLIENTS] opening shared memory failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }


    if ((q = mmap(NULL, sizeof(q), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0)) == (void *) (-1)){
        printf("| %lo | [BARBER] attaching shared memory failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }


    SEM_BARBER = sem_open(barber_path, O_RDWR);
    if (SEM_BARBER == SEM_FAILED) {
        printf("| %lo | [BARBER] creating SEM_BARBER semaphore failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }
    SEM_QUEUE = sem_open(queue_path,  O_RDWR);
    if (SEM_QUEUE == SEM_FAILED) {
        printf("| %lo | [BARBER] creating SEM_QUEUE semaphore failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    SEM_CLIENT = sem_open(client_path, O_RDWR);
    if (SEM_CLIENT == SEM_FAILED) {
        printf("| %lo | [BARBER] creating SEM_CLIENT semaphore failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    sigfillset(&f_mask);
    sigdelset(&f_mask, SIGUSR1);
    sigdelset(&f_mask, SIGINT);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        printf("| %lo | [CLIENTS] blocking masked signals failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }


    for (int i = 0; i < clients_number; i++) {
        pid_t id = fork();
        if (id == -1) {
            printf("| %lo | [CLIENTS] Fork failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }

        if (id == 0) {
            get_cut(shavings_number);
            return 0;
        }
    }

    printf("| %lo | [CLIENTS]All clients created.\n", get_timestamp());

    while (1) {
        wait(NULL);
        if (errno == ECHILD) break;
    }

    return 0;
}



