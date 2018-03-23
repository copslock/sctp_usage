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
#include <curl/curl.h>
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
	while ((c = getopt (argc, argv, "fp:")) != -1) {
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
	}
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

			printf (" Length of Data received: %d\n", in);
			char req[255];
			char *ptr = req;
			for (int i = 0; i < in; ++i) {
				printf("%x ", buffer[i]);
				ptr += sprintf(ptr, "%02X,", buffer[i]);

			}
			*ptr = '\0';
			printf("\n");
			printf("Request:%s\n", ptr);
			struct string s;
			init_string(&s);
			uint8_t bytes[100];
			int len;

			if (!function_test) {
				char command[1024];
				sprintf(command, "%s %s\n", path, req);
				printf("Command:%s\n", command);
				FILE *fp;

				fp = popen(command,"r"); 
				while (fgets(command, sizeof(command)-1, fp) != NULL) {
					printf("Output: %s\n", command);
				}
				len = get_bytes(bytes, command);
			} else {
				CURL *curl;
				CURLcode res;

				curl_global_init(CURL_GLOBAL_ALL);

				curl = curl_easy_init();
				if(curl) {
					//char *s1_req = "0x0,0x11,0x0,0x37,0x0,0x0,0x4,0x0,0x3b,0x0,0x8,0x0,0x2,0xf8,0x1,0x0,0x0,0xe0,0x0,0x0,0x3c,0x40,0x14,0x8,0x80,0x65,0x4e,0x42,0x5f,0x45,0x75,0x72,0x65,0x63,0x6f,0x6d,0x5f,0x4c,0x54,0x45,0x42,0x6f,0x78,0x0,0x40,0x0,0x7,0x0,0x0,0x0,0x40,0x2,0xf8,0x1,0x0,0x89,0x40,0x1,0x40";
					//char *attach_req = "0x00,0x0c,0x40,0x3f,0x00,0x00,0x05,0x00,0x08,0x00,0x02,0x00,0x07,0x00,0x1a,0x00,0x17,0x16,0x07,0x41,0x71,0x08,0x99,0x89,0x89,0x00,0x00,0x00,0x01,0x10,0x02,0x80,0xc0,0x00,0x05,0x02,0x00,0xd0,0x11,0xd1,0x00,0x43,0x00,0x06,0x00,0x99,0xf8,0x89,0x00,0x01,0x00,0x64,0x40,0x08,0x00,0x99,0xf8,0x89,0x00,0x01,0x1d,0x70,0x00,0x86,0x40,0x01,0x30";
					curl_easy_setopt(curl, CURLOPT_URL, "http://128.110.153.209:8080/function/parser");
					curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req);
					curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
					curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
					struct curl_slist *headers = NULL;
					curl_slist_append(headers, "Content-Type: text/plain");
					curl_slist_append(headers, "charsets: utf-8");
					curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
					printf("Befor curl perform\n");
					res = curl_easy_perform(curl);
					if(res != CURLE_OK)
						fprintf(stderr, "curl_easy_perform() failed: %s\n",
								curl_easy_strerror(res));
					printf("After curl, len:%d\n", (int)s.len);
					curl_easy_cleanup(curl);
					len = get_bytes(bytes, s.ptr);
					free(s.ptr);

				}
				curl_global_cleanup();
				//printf (" Data : %s\n", (char *) buffer);
				//uint8_t res_buf[] = {0x20, 0x11, 0x0, 0x17, 0x0, 0x0, 0x2, 0x0, 0x69, 0x0, 0xb, 0x0, 0x0, 0x2, 0xf8, 0x39, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1, 0x0, 0x57, 0x40, 0x1, 0xff};
			}
			if (len != 0) {
				send(connSock, bytes, len, 0);
			}
		}
	}
	close (connSock);

	return 0;
}

