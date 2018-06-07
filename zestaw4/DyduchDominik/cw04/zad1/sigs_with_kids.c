//
// Created by rudeu on 09.04.18.
//


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>

bool is_paused = false;
bool is_child_working = false;


void handle_sigtstp(int sig_num){

    if(!is_paused){
        printf("\nOdebrano sygnał SIGTSTP: %d \nCTRL+Z - kontynuacja\nCTRL+C - zakonczenie programu\n", sig_num);
        is_paused = true;
    }
    else{
        is_paused = false;
    }

}

void handle_sigint(int sig_num){

    printf("\nOdebrano sygnał SIGINT: %d\n", sig_num);
    exit(EXIT_SUCCESS);
}



int main(int argc, char** argv) {

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_sigtstp;

    pid_t pid = 0;

    while(1) {

        signal(SIGINT, handle_sigint);
        sigaction(SIGTSTP, &sa, NULL);

        if(!is_paused){
            if(!is_child_working) {
                is_child_working = true;

                pid = fork();
                if(pid == 0) {
                    execl("./date.sh", "./date.sh", NULL);
                    exit(EXIT_SUCCESS);
                }
            }
        }
        else{
            if(is_child_working){
                kill(pid, SIGKILL);
                is_child_working = false;
            }
        }
    }
    return 0;
}



