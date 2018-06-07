//
// Created by rudeu on 24.04.18.
//

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
#include <sys/stat.h>
#include "systemV.h"

char* home_path = NULL;
int server_queue_id = -1;

key_t queue_key;
int queue_id = -1;

int client_number = -2;

void create_queue(){
    queue_key = ftok(home_path, getpid());
    if(queue_key == -1){
        printf("[CLIENT] Could't generate queue key.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] Client queue key generated: %d.\n", queue_key);

    queue_id = msgget(queue_key, IPC_CREAT | IPC_EXCL | S_IWUSR | S_IRUSR);
    if(queue_id == -1){
        printf("[CLIENT] Couldn't create queue.CREATE_QUEUE\n");
        exit(EXIT_FAILURE);
    }

    printf("[CLIENT] Client queue id created: %d.\n", queue_id);

}

void remove_queue(){

    if(queue_id > -1){
        if(msgctl(queue_id, IPC_RMID, NULL) == -1){
            printf("[CLIENT] Couldn't remove client's queue.\n");
        }
        else{
            printf("[CLIENT] Client's queue removed.\n");
        }
    }
}

void handle_int(int sig_num){
    if(sig_num == SIGINT){
        printf("[CLIENT] SIGINT received.\n");
        exit(EXIT_SUCCESS);
    }
}

int get_server_queue_id(){
    int key = ftok(home_path, ID);
    if(key == -1){
        printf("[CLIENT] Could't generate queue key.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] Got server queue key: %d\n", key);

    int queue_id = msgget(key, 0);
    if(queue_id == -1) {
        printf("[CLIENT] Couldn't get server queue.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] Got server queue id: %d\n", queue_id);

    return queue_id;
}

void start_client(){
    msg_buf msg;
    msg.mtype = INIT;
    msg.pid = getpid();
    sprintf(msg.mtext, "%d", queue_key);

    if(msgsnd(server_queue_id, &msg, MSG_SIZE, 0) == -1){
        printf("[CLIENT] Couldn't send INIT request.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] INIT request sent to server.\n");
    if(msgrcv(queue_id, &msg, MSG_SIZE, 0, 0) == -1){
        printf("[CLIENT] Couldn't get INIT response.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] INIT response received from server.\n");

    if(sscanf(msg.mtext, "%d", &client_number) < 1){
        printf("[CLIENT] Couldn't scan INIT response.\n");
        exit(EXIT_FAILURE);
    }

    printf("[CLIENT] INIT response scanned.\n");

    if(client_number < 0){
        printf("[CLIENT] Too many clients.\n");
        exit(EXIT_FAILURE);
    }

    printf("[CLIENT] Client initialized with client's number: %d and pid: %d\n", client_number, getpid());
}

void request_time(msg_buf* msg){
    msg->mtype = TIME;
    if(msgsnd(server_queue_id, msg, MSG_SIZE, 0) == -1){
        printf("[CLIENT] TIME request failed.\n");
        exit(EXIT_FAILURE);
    }
    if(msgrcv(queue_id, msg, MSG_SIZE, 0, 0) == -1){
        printf("[CLIENT] TIME response failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] Time: %s\n", msg->mtext);
}

void request_mirror(msg_buf* msg){
    msg->mtype = MIRROR;
    printf("[CLIENT] Type in the text to mirror: ");
    if(fgets(msg->mtext, MAX_MSG_SIZE, stdin) == NULL){
        printf("[CLIENT] Text entering failure.\n");
        return;
    }
    if(msgsnd(server_queue_id, msg, MSG_SIZE, 0) == -1){
        printf("[CLIENT] MIRROR request failed.\n");
        exit(EXIT_FAILURE);
    }
    if(msgrcv(queue_id, msg, MSG_SIZE, 0, 0) == -1){
        printf("[CLIENT] MIRROR response failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] Mirrored text: %s", msg->mtext);

}

void request_calc(msg_buf* msg){
    msg->mtype = CALC;
    if(msgsnd(server_queue_id, msg, MSG_SIZE, 0) == -1){
        printf("[CLIENT] CALC request failed.\n");
        exit(EXIT_FAILURE);
    }
    if(msgrcv(queue_id, msg, MSG_SIZE, 0, 0) == -1){
        printf("[CLIENT] CALC response failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] Result: %s\n", msg->mtext);

}

void request_end(msg_buf* msg){
    msg->mtype = END;
    if(msgsnd(server_queue_id, msg, MSG_SIZE, 0) == -1){
        printf("[CLIENT] END request failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] Client's queue removed.\n");
}


int main(void){
    if(atexit(remove_queue) != 0){
        printf("[CLIENT] Couldn't register atexit function\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] \natexit function (remove_queue) registered.\n");
    signal(SIGINT, handle_int);
    printf("[CLIENT] SIGINT handler registered.\n");

    home_path = getenv("HOME");
    if(home_path == NULL){
        printf("[CLIENT] Couldn't get HOME environmental variable.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] HOME path set.\n");


    server_queue_id = get_server_queue_id();
    create_queue();

    start_client();

    char cmd[32];
    msg_buf msg;

    while(1){
        msg.pid = getpid();
        printf("[CLIENT] Enter your request: ");
        if(fgets(cmd, 32, stdin) == NULL){
            printf("[CLIENT] Request was not read.\n");
        }
        else{
            int cmd_len = (int) strlen(cmd);
            if(cmd[cmd_len - 1] == '\n') cmd[cmd_len - 1] = 0;
            char* cmd_cpy = calloc(32, sizeof(char));
            strncpy(cmd_cpy, cmd, (size_t) cmd_len);
            char* op = strtok(cmd_cpy, " ");
            if(strcmp(op, "END") == 0){
                request_end(&msg);
                exit(EXIT_SUCCESS);
            }
            else if(strcmp(op, "MIRROR") == 0){
                request_mirror(&msg);
            }
            else if(strcmp(op, "TIME") == 0){
                request_time(&msg);
            }
            else if(strcmp(op, "STOP") == 0){
                exit(EXIT_SUCCESS);
            }
            else if(strcmp(op, "CALC") == 0){
                sprintf(msg.mtext, "%s", cmd);
                request_calc(&msg);
            }
            else printf("[CLIENT] Wrong request.\n");
        }
    }
    return 0;
}