//
// Created by rudeu on 13.04.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <memory.h>
#include <errno.h>
#include <time.h>

volatile int N;
volatile int K;

struct parent_info{
    pid_t child_pid;
    int got_request;
    int sent_permission;
    int rt_sig_num;
    int child_status;
};

int received_signals = 0;

struct parent_info* info;

void print_data(){
    for(int i = 0; i < N; i++){
        printf("\nChild pid: %d\nGot request: %d\nSent permission: %d\nRT signum: %d\nChild status: %d\n",
                info[i].child_pid, info[i].got_request, info[i].sent_permission, info[i].rt_sig_num, info[i].child_status);
    }
}

void grant_permission(){
    for(int i = 0; i < N; i++) {
        if(info[i].got_request == 1 && info[i].sent_permission == 0){
            printf("Sending permission (SIGCONT) to child with pid: %d", info[i].child_pid);
            kill(SIGCONT, info[i].child_pid);
            info[i].sent_permission == 1;
        }
    }
}

void child_handler(int sig_num, siginfo_t* siginfo, void* context){

    if(sig_num == SIGCONT){
        printf("Child %d got permision.\n", getpid());
    }
}

void child_task(){

    srand(time(NULL));

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = child_handler;
    sigaction(SIGCONT, &sa, NULL);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGCONT);

    unsigned int sleep_time;
    sleep_time = (unsigned int) (rand() % 11);
    printf("Child with pid: %d, goes to sleep for: %d seconds...\n", getpid(), sleep_time);
    //fflush(stdout);
    sleep(sleep_time);
    printf("Child with pid: %d sends request (SIGUSR1)...\n", getpid());
    kill(getppid(), SIGUSR1);
    //sleep(1);
}

void parent_handler(int sig_num, siginfo_t* siginfo, void* context){

    if(received_signals < K) {
        if (sig_num == SIGUSR1) printf("Received request (SIGUSR1) from child with pid: %d. \n", siginfo->si_pid);
        for (int i = 0; i < N; i++) {
            if (info[i].child_pid == siginfo->si_pid) info[i].got_request = 1;
        }
        printf("\nReceived signals: \n%d", received_signals++);
    }
    else {
        grant_permission();
        if (sig_num == SIGUSR1) {
            printf("Received request (SIGUSR1) from child with pid: %d. \n", siginfo->si_pid);
            for (int i = 0; i < N; i++) {
                if (info[i].child_pid == siginfo->si_pid) info[i].got_request = 1;
                printf("Sending permission (SIGCONT) to child with pid: %d", info[i].child_pid);
                kill(SIGCONT, info[i].child_pid);
            }
            printf("\nReceived signals: %d\n", received_signals++);
        }
    }
}


void parent_action(){

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = parent_handler;
    sigaction(SIGUSR1, &sa, NULL);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);

    info = calloc((size_t) N, sizeof(struct parent_info));

    for(int i = 0; i < N; i++){
        info[i].child_pid = 0;
        info[i].got_request = 0;
        info[i].sent_permission = 0;
        info[i].rt_sig_num = -1;
        info[i].child_status = -1;
    }
    for(int i = 0; i < N; i++){
        info[i].child_pid = fork();
        sleep(1);
        if(info[i].child_pid == -1){
            printf("Error while forking.\n");
            exit(EXIT_FAILURE);
        }
        else if(info[i].child_pid == 0){
            child_task();
            //sleep(1);
            exit(EXIT_SUCCESS);
        }
    }
    while(received_signals < K){
        sigsuspend(&mask);
    }


    int status = 0;
    wait(&status);

};


int main(int argc, char** argv){

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <N(how many signals)> <K(how many to take action)>\n", argv[0]);
        return -1;
    }

    N = (int) strtol(argv[1], NULL, 10);
    K = (int) strtol(argv[2], NULL, 10);

    parent_action();
    print_data();

    return 0;
}
