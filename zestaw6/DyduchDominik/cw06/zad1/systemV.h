//
// Created by rudeu on 22.04.18.
//

#ifndef ZAD1_SYSTEMV_H
#define ZAD1_SYSTEMV_H

#define MAX_CLIENTS 10
#define ID 420
#define MAX_MSG_SIZE 4096

typedef enum mtype{
    MIRROR  = 1,
    CALC    = 2,
    TIME    = 3,
    END     = 4,
    INIT    = 5,
    STOP    = 6
} mtype;

typedef struct msg_buf{
    long mtype;
    pid_t pid;
    char mtext[MAX_MSG_SIZE];
} msg_buf;

const size_t MSG_SIZE = sizeof(msg_buf) - sizeof(long);


#endif //ZAD1_SYSTEMV_H
