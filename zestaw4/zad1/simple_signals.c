//
// Created by rudeu on 06.04.18.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>

bool is_paused = false;


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

void print_time(){

    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ( "\nCurrent local time and date: %s", asctime (timeinfo) );
}

int main(int argc, char** argv) {

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_sigtstp;


    while(1) {

        if(!is_paused) print_time();

        signal(SIGINT, handle_sigint);
        sigaction(SIGTSTP, &sa, NULL);

        sleep(1);
    }
    return 0;
}



