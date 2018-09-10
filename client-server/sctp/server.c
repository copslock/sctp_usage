//SCTPServer.C To compile - gcc sctpserver.c - o server - lsctp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
//#include <curl/curl.h>
#include <stdbool.h>

#define MAX_BUFFER 1024
#define MY_PORT_NUM 36412 /* This can be changed to suit the need and should be same in server and client */

bool function_test = false;
//char *path = "/users/sonika05/MobileControlPlanes/build/mymme/src/mymme";
char *path = "mymme";
struct string {
	char *ptr;
	size_t len;
};

void init_string(struct string *s) {
	s->len = 0;
	s->ptr = malloc(s->len+1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
	size_t new_len = s->len + size*nmemb;
	s->ptr = realloc(s->ptr, new_len+1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size*nmemb;
}

int get_bytes(uint8_t *bytes, char *ptr)
{
	printf("%s\n", ptr);
	char *token = strtok(ptr, " ");
	int i = 0;
	while (token != NULL)
	{
		bytes[i] = strtol(token, NULL, 16);
		printf("%x ", bytes[i]);
		i++;
		token = strtok(NULL, " ");
	}
	return i;
}
int main(int argc, char **argv)
{
	int listenSock, connSock, ret, in, flags, i;
	struct sockaddr_in servaddr;
	struct sctp_initmsg initmsg;
	struct sctp_event_subscribe events;
	struct sctp_sndrcvinfo sndrcvinfo;
	char buffer[MAX_BUFFER + 1];
	int c;
	char *cvalue = NULL;
/*	while ((c = getopt (argc, argv, "fp:")) != -1) {
		switch (c)
		{
			case 'f':
				function_test = true;
				break;
			case 'p':
				path = optarg;
				break;
			case '?':
				if (optopt == 'p')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr,
							"Unknown option character `\\x%x'.\n",
							optopt);
				return 1;
			default:
				abort ();
		}
	}*/
	listenSock = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	if(listenSock == -1)
	{
		printf("Failed to create socket\n");
		perror("socket()");
		exit(1);
	}

	bzero ((void *) &servaddr, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons (MY_PORT_NUM);

	ret = bind (listenSock, (struct sockaddr *) &servaddr, sizeof (servaddr));

	if(ret == -1 )
	{
		printf("Bind failed \n");
		perror("bind()");
		close(listenSock);
		exit(1);
	}

	/* Specify that a maximum of 5 streams will be available per socket */
	memset (&initmsg, 0, sizeof (initmsg));
	initmsg.sinit_num_ostreams = 5;
	initmsg.sinit_max_instreams = 5;
	initmsg.sinit_max_attempts = 4;
	ret = setsockopt (listenSock, IPPROTO_SCTP, SCTP_INITMSG,
			&initmsg, sizeof (initmsg));

	if(ret == -1 )
	{
		printf("setsockopt() failed \n");
		perror("setsockopt()");
		close(listenSock);
		exit(1);
	}

	ret = listen (listenSock, 5);
	if(ret == -1 )
	{
		printf("listen() failed \n");
		perror("listen()");
		close(listenSock);
		exit(1);
	}

	int len;


	printf ("Awaiting a new connection\n");

	connSock = accept (listenSock, (struct sockaddr *) NULL, (int *) NULL);
	if (connSock == -1)
	{
		printf("accept() failed\n");
		perror("accept()");
		close(connSock);
		return 1;
	}
	else
		printf ("New client connected....\n");

	while (1)
	{
		//Clear the buffer
		bzero (buffer, MAX_BUFFER + 1);
		in = recv(connSock, buffer, sizeof(buffer), 0);

		if( in == -1)
		{
			printf("Error in sctp_recvmsg\n");
			perror("sctp_recvmsg()");
			close(connSock);
			return 1;
		}
		else
		{
			//Add '\0' in case of text data
			buffer[in] = '\0';
			//printf (" Length of Data received: %d\n", in);
			send(connSock, buffer, in, 0);
		}
	}
	close (connSock);

	return 0;
}

