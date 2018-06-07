//
// Created by rudeu on 24.04.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mqueue.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "posix.h"

mqd_t server_mqueue_id = -1;
mqd_t mqueue_id = -1;
int client_number = -2;
char path[20];

void create_queue(struct mq_attr posix_attr){

    mqueue_id = mq_open(path, O_RDONLY | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR, &posix_attr);
    if(mqueue_id == -1){
        printf("[CLIENT] Couldn't create queue.\n");
        exit(EXIT_FAILURE);
    }

    printf("[CLIENT] Client queue id created: %d.\n", mqueue_id);

}


void handle_int(int sig_num){
    if(sig_num == SIGINT){
        printf("[CLIENT] SIGINT received.\n");
        exit(EXIT_SUCCESS);
    }
}


void start_client(){
    msg_buf msg;
    msg.mtype = INIT;
    msg.pid = getpid();

    if(mq_send(server_mqueue_id, (char *) &msg, MSG_BUF_SIZE, 1) == -1){
        printf("[CLIENT] Couldn't send INIT request.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] INIT request sent to server.\n");

    if(mq_receive(mqueue_id, (char*) &msg, MSG_BUF_SIZE, NULL) == -1){
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
    if(mq_send(server_mqueue_id, (char*) msg, MSG_BUF_SIZE, 1) == -1){
        printf("[CLIENT] TIME request failed.\n");
        exit(EXIT_FAILURE);
    }
    if(mq_receive(mqueue_id,(char*) msg, MSG_BUF_SIZE, NULL) == -1){
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
    if(mq_send(server_mqueue_id, (char*) msg, MSG_BUF_SIZE, 1) == -1){
        printf("[CLIENT] MIRROR request failed.\n");
        exit(EXIT_FAILURE);
    }
    if(mq_receive(mqueue_id,(char*) msg, MSG_BUF_SIZE, NULL) == -1){
        printf("[CLIENT] MIRROR response failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] Mirrored text: %s", msg->mtext);

}

void request_calc(msg_buf* msg){
    msg->mtype = CALC;
    if(mq_send(server_mqueue_id, (char*) msg, MSG_BUF_SIZE, 1) == -1){
        printf("[CLIENT] CALC request failed.\n");
        exit(EXIT_FAILURE);
    }
    if(mq_receive(mqueue_id,(char*) msg, MSG_BUF_SIZE, NULL) == -1){
        printf("[CLIENT] CALC response failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] Result: %s\n", msg->mtext);

}

void request_end(msg_buf* msg){
    msg->mtype = END;
    if(mq_send(server_mqueue_id, (char*) msg, MSG_BUF_SIZE, 1) == -1){
        printf("[CLIENT] Couldn't send END request.\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] Client's queue removed.\n");
}

void request_stop(msg_buf* msg){
    msg->mtype = STOP;
    if(mq_send(server_mqueue_id, (char*) msg, MSG_BUF_SIZE, 1) == -1)
        printf("[CLIENT] Couldn't send STOP request.\n");
    fflush(stdout);
}

void remove_queue(){

    if(mqueue_id > -1) {
        if (client_number >= 0) {
            printf("[CLIENT] Sending STOP request to server.\n");
            msg_buf msg;
            msg.pid = getpid();
            request_stop(&msg);
        }

        if(mq_close(server_mqueue_id) == -1){
            printf("[CLIENT] Couldn't close server's queue.\n");
        }
        else printf("[CLIENT] Servers's queue closed successfully.\n");

        if(mq_close(mqueue_id) == -1){
            printf("[CLIENT] Couldn't close clients's queue.\n");
        }
        else printf("[CLIENT] Client's queue closed successfully.\n");

        if(mq_unlink(path) == -1){
            printf("[CLIENT] Couldn't delete clients's queue.\n");
        }
        else printf("[CLIENT] Client's queue removed.\n");
    }
}



int main(void){
    if(atexit(remove_queue) != 0){
        printf("[CLIENT] Couldn't register atexit function\n");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] \natexit function (remove_queue) registered.\n");
    signal(SIGINT, handle_int);
    printf("[CLIENT] SIGINT handler registered.\n");

    sprintf(path, "/%d", getpid());

    server_mqueue_id = mq_open(server_path, O_WRONLY);
    if(server_mqueue_id == -1){
        printf("[CLIENT] Couldn't open server queue.\n");
        exit(EXIT_FAILURE);
    }

    struct mq_attr posix_attr;
    posix_attr.mq_maxmsg = MAX_MQ_SIZE;
    posix_attr.mq_msgsize = MSG_BUF_SIZE;

    create_queue(posix_attr);
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
            else if(strcmp(op, "CALC") == 0){
                sprintf(msg.mtext, "%s", cmd);
                request_calc(&msg);
            }
            else if(strcmp(op, "STOP") == 0){
                exit(EXIT_SUCCESS);
            }
            else printf("[CLIENT] Wrong request.\n");
        }
    }
    return 0;
}