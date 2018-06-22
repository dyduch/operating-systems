//
// Created by rudeu on 21.06.18.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <sys/un.h>
#include <limits.h>
#include "spec.h"


char* name;
char* address;

unsigned short port;

int type;
int socketd;


void inthandler(int signo) {
    if (signo == SIGINT)
    {
        printf("Caught CTRL+C. Exiting.\n");
        exit(0);
    }
}

int calc(int op, int val1, int val2){
    switch(op){
        case ADD:
            return val1 + val2;
        case SUB:
            return val1 - val2;
        case MUL:
            return val1 * val2;
        case DIV:
            if(val2 == 0){
                fflush(stdout);
                printf("Cannot divide by zero.\n");
                return INT_MAX;
            }
            return val1/val2;
        default:
            printf("Wrong operand.\n");
            fflush(stdout);
            return INT_MIN;
    }
}

void clean_up(){
    shutdown(socketd, SHUT_RDWR);
    close(socketd);
}

void response_to_request(struct request req, int id) {
    struct msg msg;
    strcpy(msg.name, name);
    msg.msg_type = REQUEST;
    msg.id = id;
    int result = calc(req.operation, req.val1, req.val2);
    msg.msg_request.val1 = result;
    printf("Calculated expression and sending it's result(%d) to server.\n", msg.msg_request.val1);
    fflush(stdout);
    if(write(socketd, &msg, sizeof(msg)) < 0) FAILURE_EXIT(1, "Couldn't send calc result to server in client %s.\n", name);
}


void main_process_function() {
    while(1) {
        struct msg msg;
        ssize_t quantity = recv(socketd, &msg, sizeof(msg), MSG_WAITALL);
        if (quantity == 0) FAILURE_EXIT(0, "Server stopped.\n");
        switch (msg.msg_type)
        {
            case START:
            FAILURE_EXIT(1, "Shouldn't have received this. Client with that name already exists.\n");
            case REQUEST:
                fflush(stdout);
                response_to_request(msg.msg_request, msg.id);
                break;
            case PING:
                printf("Got pinged by server.\n");
                fflush(stdout);
                if(write(socketd, &msg, sizeof(msg)) <0) FAILURE_EXIT(1, "Couldn't send ping to server in client %s.\n", name);
                break;
            case END:
                printf("Server is down.\n");
                exit(0);
            default:
            FAILURE_EXIT(1, "I got something strange from server: %d. Exiting.\n", msg.msg_type);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4)
    {
        FAILURE_EXIT(1, "Pass at least 3 arguments.\n");
    }

    name = argv[1];
    type = (int) strtol(argv[2], NULL, 10);
    address = argv[3];

    if (type == INET) // type 0 in console
    {
        if(argc != 5) FAILURE_EXIT(1, "Pass 4 arguments if you want INET connection.\n");
        port = (unsigned short) strtoul(argv[4], NULL, 10);
        socketd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socketd < 0) FAILURE_EXIT(1, "Couldn't create INET socket.\n");
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        if (inet_pton(AF_INET, address, &addr.sin_addr) == 0) FAILURE_EXIT(1, "Couldn't pton in INET.\n");
        addr.sin_port = htons(port);
        if (connect(socketd, (const struct sockaddr *)&addr, sizeof(addr)) < 0) FAILURE_EXIT(1, "Couldn't connect INET.\n");
    }
    else if (type == LOCAL) // type 1 in console
    {
        socketd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (socketd < 0) FAILURE_EXIT(1, "Couldn't create LOCAL socket.\n");
        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, address);
        if(bind(socketd, (const struct sockaddr *)&addr, sizeof(sa_family_t)) < 0) FAILURE_EXIT(1, "Couldn't bind LOCAL.\n"); //w unix trzeba

        if(connect(socketd, (const struct sockaddr *)&addr, sizeof(addr)) <0) FAILURE_EXIT(1, "Couldn't connect LOCAL.\n");
    }

    atexit(clean_up);
    signal(SIGINT, inthandler);

    struct msg msg;
    strcpy(msg.name, name);
    msg.msg_type = START;
    if(write(socketd, &msg, sizeof(msg)) < 0) FAILURE_EXIT(1, "Couldn't start connection in client with name %s", name);
    printf("Should now be able to receive order in %s\n", name);
    fflush(stdout);

    main_process_function();

    return 0;
}