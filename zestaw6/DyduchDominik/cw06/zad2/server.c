#define _GNU_SOURCE


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
#include <sys/stat.h>
#include "posix.h"

mqd_t mqueue_id = -1;
int waiting = 1;
int clients_number = 0;
pid_t clients_pids[MAX_CLIENTS];
int clients_ids[MAX_CLIENTS];

void create_queue(struct mq_attr posix_attr){
    mqueue_id = mq_open(server_path, O_RDONLY | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR, &posix_attr);
    if(mqueue_id == -1){
        printf("[SERVER] Couldn't create queue.\n");
        exit(EXIT_FAILURE);
    }
    printf("[SERVER] Server queue id created: %d.\n", mqueue_id);
}

void remove_queue(){
    for(int i = 0; i < clients_number; i++){
        if(mq_close(clients_ids[i]) == -1){
            printf("[SERVER] Couldn't close %d client queue\n", i);
        }
        if(kill(clients_pids[i], SIGINT) == -1){
            printf("[SERVER] Couldn't kill %d client.\n", i);
        }
    }
    if(mqueue_id > -1){
        if(mq_close(mqueue_id) == -1){
            printf("[SERVER] Couldn't close public queue.\n");
        }
        else printf("[SERVER] Server queue closed.\n");

        if(mq_unlink(server_path) == -1) printf("[SERVER] Couldn't delete public queue.\n");
        else printf("[SERVER] Server queue removed.\n");
    }
}

void handle_int(int sig_num){
    if(sig_num == SIGINT){
        waiting = 0;
        printf("[SERVER] SIGINT received.\n");
        exit(EXIT_SUCCESS);
    }
}

char* calc_add(char* op){
    char *first, *second;
    first = strtok(op, "+");
    second = strtok(NULL, "+");

    int a = (int) strtol(first, NULL, 10);
    int b = (int) strtol(second, NULL, 10);

    int result = a+b;
    char* res = calloc(32, sizeof(char));
    sprintf(res, "%d", result);
    return res;
}

char* calc_sub(char* op){
    char *first, *second;
    first = strtok(op, "-");
    second = strtok(NULL, "-");

    int a = (int) strtol(first, NULL, 10);
    int b = (int) strtol(second, NULL, 10);

    int result = a-b;
    char* res = calloc(32, sizeof(char));
    sprintf(res, "%d", result);
    return res;
}
char* calc_mul(char* op){
    char *first, *second;
    first = strtok(op, "*");
    second = strtok(NULL, "*");

    int a = (int) strtol(first, NULL, 10);
    int b = (int) strtol(second, NULL, 10);

    int result = a*b;
    char* res = calloc(32, sizeof(char));
    sprintf(res, "%d", result);
    return res;
}

char* calc_div(char* op){
    char *first, *second;
    first = strtok(op, "/");
    second = strtok(NULL, "/");

    int a = (int) strtol(first, NULL, 10);
    int b = (int) strtol(second, NULL, 10);

    int result = a/b;
    int change = a%b;
    char* res = calloc(32, sizeof(char));
    sprintf(res, "%d %d/%d", result, change, b);
    return res;
}


char* get_calc_result(char* op){
    char* result;
    char* p = strrchr(op, '+');
    if(p != NULL){
        result = calc_add(op);
        return result;
    }
    p = strrchr(op, '-');
    if(p != NULL){
        result = calc_sub(op);
        return result;
    }
    p = strrchr(op, '*');
    if(p != NULL){
        result = calc_mul(op);
        return result;
    }
    p = strrchr(op, '/');
    if(p != NULL){
        result = calc_div(op);
        return result;
    }
    return NULL;
}

void exec_init(msg_buf* msg){

    int client_pid = msg->pid;
    char client_path[20];
    sprintf(client_path, "/%d", client_pid);

    int client_mqueue_id = mq_open(client_path, O_WRONLY);
    if(client_mqueue_id == -1 ) {
        printf("[SERVER] Reading client_queue_id failed\n");
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Successfully read clients queue id: %d.\n", client_mqueue_id);

    msg->mtype = INIT;
    msg->pid = getpid();

    if(clients_number > MAX_CLIENTS - 1){
        printf("[SERVER] Maximum amount of clients reached!\n");
        sprintf(msg->mtext, "%d", -1);
        if(mq_send(client_mqueue_id, (char*) msg, MSG_BUF_SIZE, 1) == -1) {
            printf("[SERVER] Couldn't send INIT.\n");
            exit(EXIT_FAILURE);
        }
        if(mq_close(client_mqueue_id) == -1){
            printf("[SERVER] Couldn't close client's queue.\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        clients_pids[clients_number] = client_pid;
        clients_ids[clients_number] = client_mqueue_id;
        sprintf(msg->mtext, "%d", clients_number);
        clients_number++;
        if(mq_send(client_mqueue_id, (char*) msg, MSG_BUF_SIZE, 1) == -1) {
            printf("[SERVER] Couldn't send INIT.\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("[SERVER] Client's data with pid: %d and id: %d registered to clients tables.\n",client_pid, client_mqueue_id);

}

void exec_mirror(msg_buf* msg){
    int client_mqueue_id = -1;
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients_pids[i] == msg->pid)
            client_mqueue_id = clients_ids[i];
    }
    if(client_mqueue_id == -1){
        printf("[SERVER] Couldn't get queue id for client with pid: %d\n", msg->pid);
        return;
    }

    int len = (int) strlen(msg->mtext);
    if(msg->mtext[len-1] == '\n')
        len--;
    for(int i = 0; i < len/2; i++){
        char tmp = msg->mtext[i];
        msg->mtext[i] = msg->mtext[len - 1 - i];
        msg->mtext[len - 1 - i] = tmp;
    }

    if(mq_send(client_mqueue_id,(char*) msg, MSG_BUF_SIZE, 1) == -1){
        printf("[SERVER] MIRROR response sending failed.\n");
        exit(EXIT_FAILURE);
    }
}

void exec_time(msg_buf* msg){
    int client_mqueue_id = -1;
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients_pids[i] == msg->pid)
            client_mqueue_id = clients_ids[i];
    }
    if(client_mqueue_id == -1){
        printf("[SERVER] Couldn't get queue id for client with pid: %d\n", msg->pid);
        return;
    }

    time_t t;
    char buffer[26];
    struct tm* tm_info;

    time(&t);
    tm_info = localtime(&t);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    sprintf(msg->mtext, "%s", buffer);

    if(mq_send(client_mqueue_id,(char*) msg, MSG_BUF_SIZE, 1) == -1){
        printf("[SERVER] TIME response sending failed.\n");
        exit(EXIT_FAILURE);
    }

}

void exec_calc(msg_buf* msg){

    int client_mqueue_id = -1;
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients_pids[i] == msg->pid)
            client_mqueue_id = clients_ids[i];
    }
    if(client_mqueue_id == -1){
        printf("[SERVER] Couldn't get queue id for client with pid: %d\n", msg->pid);
        return;
    }

    char* full_op = calloc(32, sizeof(char));
    sprintf(full_op, "%s", msg->mtext);
    char *op;
    strtok(full_op, " ");
    op = strtok(NULL, " ");
    char* res = get_calc_result(op);
    if(res == NULL){
        sprintf(msg->mtext, "%s", "Wrong operand.");
    }
    else{
        sprintf(msg->mtext, "%s", res);
    }
    if(mq_send(client_mqueue_id,(char*) msg, MSG_BUF_SIZE, 1) == -1){
        printf("[SERVER] CALC response sending failed.\n");
        exit(EXIT_FAILURE);
    }
}

void exec_stop(msg_buf* msg){
    int i;
    for(i = 0; i < clients_number; i++){
        if(clients_pids[i] == msg->pid) break;
    }
    if(i == clients_number){
        printf("[SERVER] Couldn't find client.\n");
        return;
    }
    if(mq_close(clients_ids[i]) == -1) {
        printf("[SERVER] Couldn't close client in STOP.\n");
    }
    for(; i+1 < clients_number; i++){
        clients_pids[i] = clients_pids[i+1];
        clients_ids[i] = clients_ids[i+1];
    }
    clients_number--;
}


void exec_msg(msg_buf* msg){
    if(msg == NULL){
        printf("[SERVER] Received NULL.\n");
        return;
    }
    switch(msg->mtype){
        case MIRROR:
            exec_mirror(msg);
            break;
        case CALC:
            exec_calc(msg);
            break;
        case TIME:
            exec_time(msg);
            break;
        case END:
            waiting = 0;
            break;
        case INIT:
            exec_init(msg);
            break;
        case STOP:
            exec_stop(msg);
        default:
            break;
    }
}


int main(void){
    if(atexit(remove_queue) != 0){
        printf("[SERVER] Couldn't register atexit function\n");
        exit(EXIT_FAILURE);
    }
    printf("\n[SERVER] atexit function (remove_queue) registered.\n");

    signal(SIGINT, handle_int);
    printf("[SERVER] SIGINT handler registered.\n");

    struct mq_attr current_state;

    struct mq_attr posix_attr;
    posix_attr.mq_maxmsg = MAX_MQ_SIZE;
    posix_attr.mq_msgsize = MSG_BUF_SIZE;

    create_queue(posix_attr);

    msg_buf buf;

    while(1){
        if(waiting == 0){
            if(mq_getattr(mqueue_id, &current_state) == -1){
                printf("[SERVER] Couldn't get queue's state.\n");
                exit(EXIT_FAILURE);
            }
            if(current_state.mq_curmsgs == 0){
                printf("[SERVER] Queue is empty.\n");
                exit(EXIT_SUCCESS);
            }
        }
        if(mq_receive(mqueue_id, (char*) &buf, MSG_BUF_SIZE, NULL) == -1){
            printf("[SERVER] Couldn't receive from queue.\n");
            exit(EXIT_FAILURE);
        }
        exec_msg(&buf);
    }

    return 0;

}