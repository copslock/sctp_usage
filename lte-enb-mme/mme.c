#include <netinet/in.h>
#include <netinet/sctp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "common.h"
#include "ue.h"
#include "mme.h"

struct sockaddr_in addr = {0};
socklen_t from_len;
int socket_fd = -1;
int num_ue;
struct sockaddr_in servaddr;
struct ue_info *ue_info_arr;

int main(int argc, char *argv[])
{
    int flags, in;
    uint8_t buffer[MAX_BUFFER + 1] = {0};
    struct sctp_sndrcvinfo sndrcvinfo = {0};

    if (argc != 2)
    {
        printf("Usage: ./mme <num_ue>\n");
        return 0;
    }
    num_ue = atoi(argv[1]);

    create_connection();
    while (1)
    {
        struct sctp_sndrcvinfo sinfo = {0};
        int n;
        bzero(buffer, MAX_BUFFER + 1);
        flags = 0;
        memset((void *)&addr, 0, sizeof(struct sockaddr_in));
        from_len = (socklen_t)sizeof(struct sockaddr_in);
        memset((void *)&sinfo, 0, sizeof(struct sctp_sndrcvinfo));

        n = sctp_recvmsg(socket_fd, (void *)buffer, MAX_BUFFER, (struct sockaddr *)&addr, &from_len, &sinfo, &flags);
        if (-1 == n)
        {
            printf("Error with sctp_recvmsg: -1... waiting\n");
            perror("Description: ");
            sleep(1);
            continue;
        }
        else
        {
            handle_received_message(buffer);
        }
    }

    getchar();
clean:
    cleanup();
    return 0;
}
void create_connection()
{
    int SctpScocket, n, flags;
    socklen_t from_len;

    struct sockaddr_in addr = {0};
    struct sctp_sndrcvinfo sinfo = {0};
    struct sctp_event_subscribe event = {0};

    char *szAddress;
    int iPort;

    int iMsgSize;
    ue_info_arr = (struct ue_info *)malloc(num_ue * sizeof(struct ue_info));
    //get the arguments
    szAddress = "127.0.0.1";
    iPort = MY_PORT_NUM;

    //here we may fail if sctp is not supported
    socket_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
    debug_print("socket created...\n");

    //make sure we receive MSG_NOTIFICATION
    setsockopt(socket_fd, IPPROTO_SCTP, SCTP_EVENTS, &event, sizeof(struct sctp_event_subscribe));
    debug_print("setsockopt succeeded...\n");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(iPort);
    addr.sin_addr.s_addr = inet_addr(szAddress);

    //bind to specific server address and port
    bind(socket_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    debug_print("bind succeeded...\n");

    //wait for connections
    listen(socket_fd, 1);
    debug_print("listen succeeded...\n");
}

void send_sctp_message(long id)
{
    int ret;

    ret = sctp_sendmsg(socket_fd, (const void *)&ue_info_arr[id].message, (size_t)ue_info_arr[id].datalen,
                       (struct sockaddr *)&addr, from_len, htonl(MY_PORT_NUM), 0, 0, 0, 0);

    if (ret == -1)
    {
        printf("Error in sctp_sendmsg\n");
        perror("sctp_sendmsg()");
        return;
    }
    else
        debug_print("Successfully sent %d bytes data to ENB\n", ret);
}
void cleanup()
{
    close(socket_fd);
    free(ue_info_arr);
}