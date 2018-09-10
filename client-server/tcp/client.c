// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    int i = 0;
    double connect_time = 0, send_time = 0, receive_time = 0;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "192.168.4.20", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    clock_t t1 = clock();
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed, i:%d\n", i);
        perror("connect failed");
        return -1;
    }
    clock_t t2 = clock();
    double t2t1 = (double)(t2 - t1) / CLOCKS_PER_SEC;
    connect_time += t2t1;
    while (i < 1000) {
    clock_t t2 = clock();
    send(sock , hello , strlen(hello) , 0 );
    clock_t t3 = clock();
    double t3t2 = (double)(t3 - t2) / CLOCKS_PER_SEC;
    send_time += t3t2;
    //printf("Hello message sent\n");
    valread = read( sock , buffer, 1024);
    clock_t t4 = clock();
    double t4t3 = (double)(t4 - t3) / CLOCKS_PER_SEC;
    receive_time += t4t3;
    ++i;
    }
    //printf("t1:%ld, t2:%ld, t3:%ld, t4:%ld\n", t1, t2, t3, t4);
    printf("Total time taken for 1000: connect:%f, send:%f, receive:%f\n", connect_time, send_time, receive_time);
    printf("Average Time taken for: connect:%f, send:%f, receive:%f\n", connect_time, send_time/1000, receive_time/1000);
    //printf("%s\n",buffer );
    return 0;
}

