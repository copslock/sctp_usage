/* http-client.c
 *
 * Copyright (c) 2000 Sean Walton and Macmillan Publishers.  Use may be in
 * whole or in part in accordance to the General Public License (GPL).
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*****************************************************************************/
/*** http-client.c                                                         ***/
/***                                                                       ***/
/*** This program shows what the HTTP server sends to the client.  First,  ***/
/*** it opens a TCP socket to the server.  Then, it sends the request      ***/
/*** "GET <resource> HTTP/1.0\n\n" (the second newline is needed for the   ***/
/*** "message-end" message.  Lastly it prints out the reply.               ***/
/*****************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <errno.h>
#include <time.h>

#define MAXBUF  1024
void PANIC(char *msg);
#define PANIC(msg)  {perror(msg); abort();}

int main(int Count, char *Strings[])
{   int sockfd, bytes_read;
	struct sockaddr_in dest;
	char buffer[MAXBUF];

	/*---Make sure we have the right number of parameters---*/
	if ( Count != 3 )
		PANIC("usage: testport <IP-addr> <send-msg>\n");
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
		PANIC("Socket");

	/*---Initialize server address/port struct---*/
	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(8000); /*default HTTP Server port */
	if ( inet_addr(Strings[1], &dest.sin_addr.s_addr) == 0 )
		PANIC(Strings[1]);

	double connect_time = 0, send_time = 0, receive_time = 0;
	int i = 0;
	sprintf(buffer, "GET %s HTTP/1.0\n\n", Strings[2]);
	while (i < 10) {
	/*---Connect to server---*/
	if ( connect(sockfd, (struct sockaddr*)&dest, sizeof(dest)) != 0 )
		PANIC("Connect");

		clock_t t2 = clock();
		int b = send(sockfd, buffer, strlen(buffer), 0);
		clock_t t3 = clock();
		double t3t2 = (double)(t3 - t2) / CLOCKS_PER_SEC;
		send_time += t3t2;
		printf("Sent :%d th GET, bytes sent: %d\n", i, b);
		/*---While there's data, read and print it---*/
		do
		{
			printf("Will read response\n");
			bzero(buffer, sizeof(buffer));
			bytes_read = recv(sockfd, buffer, sizeof(buffer), 0);
			if ( bytes_read > 0 )
			    printf("%s", buffer);
		}
		while ( bytes_read > 0 );
		bytes_read = 0;
		clock_t t4 = clock();
		double t4t3 = (double)(t4 - t3) / CLOCKS_PER_SEC;
		receive_time += t4t3;
		++i;
	close(sockfd);
	}
	printf("Total time taken for 1000: connect:%f, send:%f, receive:%f\n", connect_time, send_time, receive_time);
	printf("Average Time taken for: connect:%f, send:%f, receive:%f\n", connect_time, send_time/1000, receive_time/1000);
	/*---Clean up---*/
	return 0;
}

