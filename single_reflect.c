#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAX_LEN 4096
#define true 1

void connecting(int socket_fd, const char* address, int port);
void listening(int socket_fd, int port);
void print_info();
void readmsg(char* buff, int max_len);

int main(int argc, char* argv[])
{
    // domain: AF_INET:IPv4 AF_INET6:IPv6 AF_LOCAL:Unix socket AF_ROUTE:...
    // type: OCK_STREAM、SOCK_DGRAM、SOCK_RAW、SOCK_PACKET、SOCK_SEQPACKET
    // protocol: IPPROTO_TCP、IPPTOTO_UDP、IPPROTO_SCTP、IPPROTO_TIPC
    // int socket(int domain, int type, int protocol);
    char address[30];
    int port = 0;
    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(argc < 2)
    {
        print_info();
        return 0;
    }

    if(!strcmp(argv[1], "--server"))
    {
        if(argc < 3)
        {
            print_info();
            return 0;
        }
        port = atoi(argv[2]);
        listening(socket_fd, port);

    }
    else if(!strcmp(argv[1], "--client"))
    {
        if(argc < 4)
        {
            print_info();
            return 0;
        }

        strcpy(address, argv[2]);
        port = atoi(argv[3]);

        connecting(socket_fd, address, port);
    }
    else
    {
        print_info();
        return 0;
    }
    return 0;
}

void connecting(int socket_fd, const char* address, int port)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if(inet_pton(AF_INET, address, &addr.sin_addr) < 0)
    {
        fprintf(stderr, "illegal ip address!\n");
        print_info();
    }
    if(connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        fprintf(stderr, "connect error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // working(socket_fd);
    // close(socket_fd);
    while(true)
    {
        char buff[MAX_LEN];
        printf("> ");
        readmsg(buff, MAX_LEN);
        if(strcmp(buff, "$exit")==0)
        {
            puts("now exiting...\n");
            int tmp = htonl(-1);
            send(socket_fd, (char*)&tmp, sizeof(int), 0);
            close(socket_fd);
            exit(0);
        }
        
        int length = htonl(strlen(buff));
        send(socket_fd, (char*)&length, sizeof(int), 0);

        if(send(socket_fd, buff, strlen(buff), 0) < 0)
        {
            fprintf(stderr, "network error, %s(errno: %d)\n", strerror(errno), errno);
            exit(0);
        }
    }
    puts("connection closed, now exiting...\n");
    exit(0);
}

void listening(int socket_fd, int port)
{
    struct sockaddr_in addr;
    int client_fd, length;
    char buff[MAX_LEN];
    

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1||listen(socket_fd, 10) == -1)
    {
        fprintf(stderr, "network error, try again please.\n");
        exit(0);
    }

    printf("waiting for client's request...\n");
    while(true)
    {
        if((client_fd = accept(socket_fd, NULL, NULL)) == -1)
        {
            fprintf(stderr, "accept socket error: %s(errno: %d)\n", strerror(errno), errno);
            continue;
        }

        // working(socket_fd);
        while(true)
        {
            int packet_len = 0, has_len = sizeof(int);
            while(has_len > 0)
            {
                int tmp = recv(client_fd, (char*)((&packet_len)+sizeof(int)-has_len), has_len, 0);
                // perror(strerror(errno));
                // printf("%d %d\n", has_len, tmp);
                if(tmp < has_len)
                {
                    has_len -= tmp;
                }
                else
                    break;
            }
            // printf("%d\n", packet_len);
            packet_len = ntohl(packet_len);
            if(packet_len == -1)
                break;
            // printf("%d\n", packet_len);
            recv(client_fd, buff, packet_len, 0);
            buff[packet_len] = '\0';
            printf("> %s\n", buff);
            // break;
        }
        puts("connection closed...\n");
        close(client_fd);
        // exit(0);
    }
}

void print_info()
{
    puts("Wrong command line options!\n");
    puts("use \"--server port\" to receive message and \"--client ip_address port\" to send message.\n");
}

void readmsg(char* buff, int max_len)
{
    char c = '\0';
    int length = 0;
    while((c=getchar())!='\n' && length <= max_len)
        buff[length++] = c;
    buff[length] = '\0';
}
