#define _GNU_SOURCE

#include "barbershop.h"

int shmid = -1;
int semid = -1;

queue* q = NULL;
key_t queue_key;

void default_signal_handler(int sig_num) {
    printf("%d recived.", sig_num);
    exit(EXIT_SUCCESS);
}


void delete_all() {
    if (shmdt(q) == -1) printf("| %lo | [BARBER] detaching queue shared memory failed.\n", get_timestamp());
    else printf("| %lo | [BARBER] detached queue shared memory.\n", get_timestamp());

    if (shmctl(shmid, IPC_RMID, NULL) == -1) printf("| %lo | [BARBER] deleting queue shared memory failed.\n", get_timestamp());
    else printf("| %lo | [BARBER] deleted queue shared memory.\n", get_timestamp());

    if (semctl(semid, 0, IPC_RMID) == -1) printf("| %lo | [BARBER] deleting semaphores failed.", get_timestamp());
    else printf("| %lo | [BARBER] deleted semaphores!\n", get_timestamp());
}


pid_t get_on_chair(struct sembuf *sops) {
    sops->sem_num = SEM_QUEUE;
    sops->sem_op = -1;
    if (semop(semid, sops, 1) == -1) printf("| %lo | [BARBER] taking SEM_QUEUE semaphore failed.", get_timestamp());

    pid_t curr = q->current_client;

    sops->sem_op = 1;
    if (semop(semid, sops, 1) == -1) printf("| %lo | [BARBER] releasing SEM_QUEUE semaphore failed.", get_timestamp());

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

    char *path = getenv("HOME");

    if (path == NULL) {
        printf("| %lo | [BARBER] Getting environmental variable failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    queue_key = ftok(path, PROJECT_ID);
    if (queue_key == -1) {
        printf("| %lo | [BARBER] getting shm key failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    shmid = shmget(queue_key, sizeof(queue), IPC_CREAT | IPC_EXCL | 0666);
    if (shmid == -1) {
        printf("| %lo | [BARBER] creating shm failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }


    if ((q = shmat(shmid, NULL, 0)) == (void *) (-1)){
        printf("| %lo | [BARBER] attaching shm failed.", get_timestamp());
        exit(EXIT_FAILURE);
    }

    init_queue(q, chair_number);

    semid = semget(queue_key, 4, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1) {
        printf("| %lo | [BARBER] creating semaphores failed.", get_timestamp());
        exit(EXIT_FAILURE);

    }

    for (int i = 1; i < 3; i++)
        if (semctl(semid, i, SETVAL, 1) == -1) {
        printf("| %lo | [BARBER] setting semaphores failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }

    if (semctl(semid, 0, SETVAL, 0) == -1) {
        printf("| %lo | [BARBER] setting semaphores failed.", get_timestamp());
        exit(EXIT_FAILURE);

    }


    struct sembuf sops;
    sops.sem_flg = 0;

    while (1) {
        sops.sem_num = SEM_BARBER;
        sops.sem_op = -1;

        if (semop(semid, &sops, 1) == -1){
            printf("| %lo | [BARBER] taking SEM_BARBER semaphore failed.", get_timestamp());
            exit(EXIT_FAILURE);
        }

        pid_t current_client = get_on_chair(&sops);
        cut(current_client);

        while (1) {
            sops.sem_num = SEM_QUEUE;
            sops.sem_op = -1;
            if (semop(semid, &sops, 1) == -1) {
                printf("| %lo | [BARBER] taking SEM_QUEUE semaphore failed.", get_timestamp());
                exit(EXIT_FAILURE);
            }
            current_client = get(q);

            if (current_client != -1) {
                sops.sem_op = 1;
                if (semop(semid, &sops, 1) == -1) {
                    printf("| %lo | [BARBER] releasing SEM_QUEUE semaphore failed.", get_timestamp());
                    exit(EXIT_FAILURE);
                }
                cut(current_client);
            } else {
                printf("| %lo | [BARBER] going to sleep...\n", get_timestamp());
                fflush(stdout);
                sops.sem_num = SEM_BARBER;
                sops.sem_op = -1;
                if (semop(semid, &sops, 1) == -1) {
                    printf("| %lo | [BARBER] taking SEM_BARBER semaphore failed.", get_timestamp());
                    exit(EXIT_FAILURE);
                }

                sops.sem_num = SEM_QUEUE;
                sops.sem_op = 1;
                if (semop(semid, &sops, 1) == -1) {
                    printf("| %lo | [BARBER] releasing SEM_QUEUE semaphore failed.", get_timestamp());
                    exit(EXIT_FAILURE);

                }
                break;
            }
        }
    }

    return 0;
}



