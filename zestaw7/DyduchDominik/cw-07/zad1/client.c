
#define _GNU_SOURCE

#include "barbershop.h"

queue *q = NULL;
key_t queue_key;
sigset_t mask;
int shmid = -1;
int semid = -1;
volatile int client_counter = 0;


void sig_handler(int signo) {
    client_counter++;
}

void default_signal_handler(int signo) {
    printf("%d received\n", signo);
    exit(EXIT_SUCCESS);
}


void delete_all(void) {
    if (shmdt(q) == -1) printf("| %lo | [CLIENTS] detaching SEM_QUEUE shared memory failed.\n", get_timestamp());
    else printf("| %lo | [CLIENTS] detached SEM_QUEUE shared memory.\n", get_timestamp());
}


int get_on_chair() {
    int barber_status = semctl(semid, 0, GETVAL);
    if (barber_status == -1) {
        printf("| %lo | [CLIENTS] getting value of SEM_BARBER semaphore failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }


    pid_t clients_pid = getpid();

    if (barber_status == 0) {
        struct sembuf sops;
        sops.sem_num = SEM_BARBER;
        sops.sem_op = 1;
        sops.sem_flg = 0;

        if (semop(semid, &sops, 1) == -1) {
            printf("| %lo | [CLIENTS] awakening barber failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }

        printf("| %lo | [CLIENTS] Client %d has awakened barber!\n", get_timestamp(), clients_pid);
        fflush(stdout);
        if (semop(semid, &sops, 1) == -1) {
            printf("| %lo | [CLIENTS] awakening barber failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }
        q->current_client = clients_pid;
        return 1;

    } else
        {
            int res = put(q, clients_pid);
            if (res == -1) {
                printf("| %lo | [CLIENTS] Client %d couldn't find free place.\n", get_timestamp(), clients_pid);
                fflush(stdout);
                return -1;
            } else {
                printf("| %lo | [CLIENTS] Client %d took place in the waiting room.\n", get_timestamp(), clients_pid);
                fflush(stdout);
                return 0;
        }
    }
}


void get_cut(int shavings_number) {
    while (client_counter < shavings_number) {
        struct sembuf sops;
        sops.sem_num = SEM_CLIENT;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if (semop(semid, &sops, 1) == -1) {
            printf("| %lo | [CLIENTS] acquiring SEM_CLIENT semaphore failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }


        sops.sem_num = SEM_QUEUE;
        if (semop(semid, &sops, 1) == -1) {
            printf("| %lo | [CLIENTS] acquiring SEM_QUEUE semaphore failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }


        int res = get_on_chair();

        sops.sem_op = 1;
        if (semop(semid, &sops, 1) == -1) {
            printf("| %lo | [CLIENTS]releasing SEM_QUEUE semaphore failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }


        sops.sem_num = SEM_CLIENT;
        if (semop(semid, &sops, 1) == -1) {
            printf("| %lo | [CLIENTS] releasing SEM_CLIENT semaphore failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }


        if (res != -1) {
            sigsuspend(&mask);
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



    char *path = getenv("HOME");

    if (path == NULL) {
        printf("| %lo | [CLIENTS] getting environmental variable failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }


    queue_key = ftok(path, PROJECT_ID);
    if (queue_key == -1) {
        printf("| %lo | [CLIENTS] getting shared memory key failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }


    shmid = shmget(queue_key, 0, 0);
    if (shmid == -1) {
        printf("| %lo | [CLIENTS] opening shared memory failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }



    if ((q =shmat(shmid, NULL, 0)) == (void *) (-1)) {
        printf("| %lo | [CLIENTS] attaching shared memory failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }


    semid = semget(queue_key, 0, 0);
    if (semid == -1) {
        printf("| %lo | [CLIENTS] opening semaphores failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }


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



