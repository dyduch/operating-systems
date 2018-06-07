//
// Created by rudeu on 10.04.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <memory.h>
#include <errno.h>

volatile int parent_sig_count = 0;
volatile int child_received_count = 0;
volatile int parent_received_count = 0;
volatile pid_t pid = 0;
volatile int type;
volatile int L;

void parent_send_signals(){

    if(type != 3) {
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGINT);

        for (parent_sig_count; parent_sig_count < L; parent_sig_count++) {
            //sleep(1);
            printf("Sending signal SIGUSR1 #%d to child...\n", parent_sig_count);
            kill(pid, SIGUSR1);
            if (type == 2) sigsuspend(&mask);

        }
        printf("%d SIGUSR1 signals were sent to the child.\n", parent_sig_count);
        //sleep(1);
        printf("Sending signal SIGUSR2 to child...\n");
        kill(pid, SIGUSR2);
        parent_sig_count++;
    }

    else{
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGRTMIN);
        sigdelset(&mask, SIGINT);

        for (parent_sig_count; parent_sig_count < L; parent_sig_count++) {
            //sleep(1);
            printf("Sending signal SIGRTMIN #%d to child...\n", parent_sig_count);
            kill(pid, SIGRTMIN);
        }
        printf("%d SIGRTMIN signals were sent to the child.\n", parent_sig_count);
        //sleep(1);
        printf("Sending signal SIGRTMAX to child...\n");
        kill(pid, SIGRTMAX);
        parent_sig_count++;
    }

}

void parent_handler(int signum){

    if(signum == SIGINT){
        printf("Parent received SIGINT signal, sending SIGUSR2 to child...\n");
        kill(pid, SIGUSR2);
        exit(EXIT_SUCCESS);
    }

    if(type != 3) {
        if (signum == SIGUSR1) {
            printf("Parent received back SIGUSR1 #%d...\n", parent_received_count);
            parent_received_count++;
            printf("Parent received total of %d signals\n", parent_received_count);
        }
    }
    else{
        if (signum == SIGRTMIN) {
            printf("Parent received back SIGRTMIN #%d...\n", parent_received_count);
            parent_received_count++;
            printf("Parent received total of %d signals\n", parent_received_count);
        }
    }
}

void parent_action(){

    //sleep(1);

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = parent_handler;

    sigaction(SIGINT, &sa, NULL);
    if(type != 3) {
        sigaction(SIGUSR1, &sa, NULL);
    }
    else{
        sigaction(SIGRTMIN, &sa, NULL);
    }
    parent_send_signals();

    int status = 0;
    waitpid(pid, &status, 0);
    if (status) {
        printf("Error while executing.\n");
        exit(EXIT_FAILURE);
    }
}

void child_handler(int signum){

    if(type != 3) {
        if (signum == SIGUSR1) {
            printf("Child received SIGUSR1 #%d...\n", child_received_count);
            child_received_count++;
            printf("Child received total of %d signals\n", child_received_count);
            printf("Sending back SIGUSR1 signal to parent ...\n");
            if (kill(getppid(), SIGUSR1) == -1) {
                printf("Oh dear, something went wrong with sending signal! %s\n", strerror(errno));
            }
        } else if (signum == SIGUSR2) {
            printf("Child received SIGUSR2...\n");
            child_received_count++;
            printf("Child received total of %d signals\nShutting down...\n", child_received_count);
            exit(EXIT_SUCCESS);
        }
    }
    else{
        if (signum == SIGRTMIN) {
            printf("Child received SIGRTMIN #%d...\n", child_received_count);
            child_received_count++;
            printf("Child received total of %d signals\n", child_received_count);
            printf("Sending back SIGRTMIN signal to parent ...\n");
            if (kill(getppid(), SIGRTMIN) == -1) {
                printf("Oh dear, something went wrong with sending signal! %s\n", strerror(errno));
            }
        } else if (signum == SIGRTMAX) {
            printf("Child received SIGRTMAX...\n");
            child_received_count++;
            printf("Child received total of %d signals\nShutting down...\n", child_received_count);
            exit(EXIT_SUCCESS);
        }
    }

}

void child_catch_signals(){

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = child_handler;

    sigfillset(&sa.sa_mask);
    if(type != 3) {
        sigdelset(&sa.sa_mask, SIGUSR1);
        sigdelset(&sa.sa_mask, SIGUSR2);
        sigaction(SIGUSR1, &sa, NULL);
        sigaction(SIGUSR2, &sa, NULL);
    }
    else{
        sigdelset(&sa.sa_mask, SIGRTMIN);
        sigdelset(&sa.sa_mask, SIGRTMAX);
        sigaction(SIGRTMIN, &sa, NULL);
        sigaction(SIGRTMAX, &sa, NULL);
    }


    while(1){
        sleep(1);
    }
}


int main(int argc, char** argv){

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <L> <type>\n", argv[0]);
        return -1;
    }

    L = (int) strtol(argv[1], NULL, 10);
    type = (int) strtol(argv[2], NULL, 10);

    pid = fork();
    if(pid > 0){
        parent_action();
    }
    else if(pid == 0) child_catch_signals();
    else{
        printf("Fork failure, pid value: %d", pid);
        exit(EXIT_FAILURE);
    }


    return 0;

}