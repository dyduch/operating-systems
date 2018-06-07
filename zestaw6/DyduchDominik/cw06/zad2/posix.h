//
// Created by rudeu on 22.04.18.
//

#ifndef ZAD2_POSIX_H
#define ZAD2_POSIX_H

#define MAX_CLIENTS 10
#define ID 420
#define MAX_MSG_SIZE 4096
#define MSG_BUF_SIZE sizeof(msg_buf)
#define MAX_MQ_SIZE 9

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

const char server_path[] = "/server";

#endif //ZAD2_POSIX_H
