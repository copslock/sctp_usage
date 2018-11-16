#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>

#define MAX_BUFFER 1024
#define MY_PORT_NUM 62324 /* This can be changed to suit the need and should be same in server and client */
#define DEBUG 1

#ifdef DEBUG
#define debug_print(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define debug_print(fmt, ...) do {} while (0)
#endif

int num_ue;
struct sockaddr_in servaddr;

enum s1ap_message_type {
    ATTACH_REQ = 0,
    AUTH_REQ,
    AUTH_RES,
    SEC_MODE_COMMAND,
    SEC_MODE_COMPLETE,
    ATTACH_ACCEPT
};

enum ue_state {
    IDLE = 0,
    CONNECTED
};
#endif