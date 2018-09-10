#include <arpa/inet.h>
#include <netinet/in.h>
//#include <netinet/sctp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define MAX_BUFFER 1024
#define MY_PORT_NUM 36412 /* This can be changed to suit the need and should be same in server   \
			     and client */

uint8_t buffer[] = {0x00, 0x0c, 0x40, 0x3f, 0x00, 0x00, 0x05, 0x00, 0x08, 0x00,
	0x02, 0x00, 0x07, 0x00, 0x1a, 0x00, 0x17, 0x16, 0x07, 0x41,
	0x71, 0x08, 0x99, 0x89, 0x89, 0x00, 0x00, 0x00, 0x01, 0x10,
	0x02, 0x80, 0xc0, 0x00, 0x05, 0x02, 0x00, 0xd0, 0x11, 0xd1,
	0x00, 0x43, 0x00, 0x06, 0x00, 0x99, 0xf8, 0x89, 0x00, 0x01,
	0x00, 0x64, 0x40, 0x08, 0x00, 0x99, 0xf8, 0x89, 0x00, 0x01,
	0x1d, 0x70, 0x00, 0x86, 0x40, 0x01, 0x30};
int main(int argc, char *argv[]) {
	int connSock, in, i, ret, flags;
	struct sockaddr_in servaddr;
	//struct sctp_status status;
	int datalen = 0;

	datalen = sizeof(buffer);
	printf("Datalen:%d\n", datalen);

	for (int i = 0; i < 1; ++i) {
		connSock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

		if (connSock == -1) {
			printf("Socket creation failed\n");
			perror("socket()");
			exit(1);
		}

		bzero((void *)&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(MY_PORT_NUM);
		servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    double connect_time = 0, send_time = 0, receive_time = 0;
    clock_t t1 = clock();
		ret = connect(connSock, (struct sockaddr *)&servaddr, sizeof(servaddr));

		if (ret == -1) {
			printf("Connection failed\n");
			perror("connect()");
			close(connSock);
			exit(1);
		}
    clock_t t2 = clock();
    double t2t1 = (double)(t2 - t1) / CLOCKS_PER_SEC;
    connect_time += t2t1;
		buffer[28] += 2;
		int i = 0;
		while (i < 1000) {
    clock_t t2 = clock();
			ret = send(connSock, (void *)buffer, (size_t)datalen, 0);
			if (ret == -1) {
				printf("Error in sctp_sendmsg\n");
				perror("sctp_sendmsg()");
			} else {
    clock_t t3 = clock();
    double t3t2 = (double)(t3 - t2) / CLOCKS_PER_SEC;
    send_time += t3t2;
//				printf("Successfully sent %d bytes data to server\n", ret);
				bzero (buffer, sizeof(buffer));
				ret = recv(connSock, buffer, sizeof(buffer), 0);
    clock_t t4 = clock();
    double t4t3 = (double)(t4 - t3) / CLOCKS_PER_SEC;
    receive_time += t4t3;
//				printf("Received %d bytes data from server\n", ret);
			}
			++i;
		}
    printf("Total time taken for 1000: connect:%f, send:%f, receive:%f\n", connect_time, send_time, receive_time);
    printf("Average Time taken for: connect:%f, send:%f, receive:%f\n", connect_time, send_time/1000, receive_time/1000);
		close(connSock);
	}
	//while (1) {
	//}

	return 0;
}
