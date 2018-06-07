#define _GNU_SOURCE

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

int queue_id = -1;
char* home_path = NULL;
int waiting = 1;
key_t queue_key;
int clients_number = 0;
pid_t clients_pids[MAX_CLIENTS];
int clients_ids[MAX_CLIENTS];

void create_queue(){
    queue_key = ftok(home_path, ID);
    if(queue_key == -1){
        printf("[SERVER] Could't generate queue key.\n");
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Server queue key generated: %d.\n", queue_key);


    queue_id = msgget(queue_key, IPC_CREAT | IPC_EXCL | S_IWUSR | S_IRUSR);
    if(queue_id == -1){
        printf("[SERVER] Couldn't create queue.\n");
        exit(EXIT_FAILURE);
    }
    printf("[SERVER] Server queue id created: %d.\n", queue_id);

}

void remove_queue(){
    waiting = 0;
    msgctl(queue_id, IPC_RMID, NULL);
    printf("[SERVER] Server queue removed\n");
}

void handle_int(int sig_num){
    if(sig_num == SIGINT){
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
    key_t client_queue_key;
    if(sscanf(msg->mtext, "%d", &client_queue_key) < 0){
        printf("[SERVER] Reading clients_key failed.\n");
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Successfully read clients queue key: %d.\n", client_queue_key);


    int client_queue_id = msgget(client_queue_key, 0);
    if(client_queue_id == -1 ) {
        printf("[SERVER] Reading client_queue_id failed\n");
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Successfully read clients queue id: %d.\n", client_queue_id);


    int client_pid = msg->pid;
    msg->mtype = INIT;
    msg->pid = getpid();

    if(clients_number > MAX_CLIENTS - 1){
        printf("[SERVER] Maximum amount of clients reached!\n");
        sprintf(msg->mtext, "%d", -1);
    }else{
        clients_pids[clients_number] = client_pid;
        clients_ids[clients_number] = client_queue_id;
        sprintf(msg->mtext, "%d", clients_number);
        clients_number++;
    }

    printf("[SERVER] Client's data with pid: %d and id: %d registered to clients tables.\n",client_pid, client_queue_id);


    if(msgsnd(client_queue_id, msg, MSG_SIZE, 0) == -1){
        printf("[SERVER] INIT response sending failed.\n");
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] INIT response sent to client.\n");

}

void exec_mirror(msg_buf* msg){
    int client_queue_id = -1;
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients_pids[i] == msg->pid)
            client_queue_id = clients_ids[i];
    }
    if(client_queue_id == -1){
        printf("Couldn't get queue id for client with pid: %d\n", msg->pid);
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

    if(msgsnd(client_queue_id, msg, MSG_SIZE, 0) == -1){
        printf("[SERVER] MIRROR response sending failed.\n");
        exit(EXIT_FAILURE);
    }

}

void exec_time(msg_buf* msg){
    int client_queue_id = -1;
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients_pids[i] == msg->pid)
            client_queue_id = clients_ids[i];
    }
    if(client_queue_id == -1){
        printf("Couldn't get queue id for client with pid: %d\n", msg->pid);
        return;
    }

    time_t t;
    char buffer[26];
    struct tm* tm_info;

    time(&t);
    tm_info = localtime(&t);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    sprintf(msg->mtext, "%s", buffer);

    if(msgsnd(client_queue_id, msg, MSG_SIZE, 0) == -1){
        printf("[SERVER] TIME response sending failed.\n");
        exit(EXIT_FAILURE);
    }

}

void exec_calc(msg_buf* msg){

    int client_queue_id = -1;
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients_pids[i] == msg->pid)
            client_queue_id = clients_ids[i];
    }
    if(client_queue_id == -1){
        printf("Couldn't get queue id for client with pid: %d\n", msg->pid);
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
    if(msgsnd(client_queue_id, msg, MSG_SIZE, 0) == -1){
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
            break;
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

    home_path = getenv("HOME");
    if(home_path == NULL){
        printf("[SERVER] Couldn't get HOME environmental variable.\n");
        exit(EXIT_FAILURE);
    }
    printf("[SERVER] HOME path set.\n");

    create_queue();

    msg_buf buf;
    struct msqid_ds queue;

    while(1){
        if(waiting == 0){
            if(msgctl(queue_id, IPC_STAT, &queue) == -1){
                printf("[SERVER] Couldn't get queue's state.\n");
                exit(EXIT_FAILURE);
            }
            if(queue.msg_qnum == 0){
                printf("[SERVER] Queue is empty.\n");
                break;
            }
        }
        if(msgrcv(queue_id, &buf, MSG_SIZE, 0, 0) == -1){
            printf("[SERVER] Couldn't receive from queue.\n");
            exit(EXIT_FAILURE);
        }
        exec_msg(&buf);
    }

    return 0;

}