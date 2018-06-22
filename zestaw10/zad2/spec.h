//
// Created by rudeu on 21.06.18.
//

#ifndef ZESTAW10_SPEC_H
#define ZESTAW10_SPEC_H

#include <zconf.h>

#define MAX_LEN 128
#define MAX_CLIENTS 10
#define MAX_EVENTS 5
#define INET 0
#define LOCAL 1
#define START 2
#define REQUEST 3
#define PING 4
#define ADD 5
#define SUB 6
#define MUL 7
#define DIV 8
#define END 9

#define FAILURE_EXIT(code, format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(code);}

struct client{
    char name[MAX_LEN];
    struct sockaddr addr;
    socklen_t addr_size;
    int fd;
    int pings;
};

struct request{
    int operation;
    int val1;
    int val2;
};

struct msg{
    int id;
    int msg_type;
    char name[MAX_LEN];
    struct request msg_request;
};


#endif //ZESTAW10_SPEC_H
