#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "unistd.h"
#include "stdarg.h"

#include <vector>
#include <thread>

const int MAX_THREAD = 100;
enum OUTPUT_TYPE
{
    DEBUG, ERROR, INFO
};
int LOG_LEVEL = INFO;

void mlog(OUTPUT_TYPE type, const char *fmt, ...);
void scan_address();

int main(int argc, char* argv[])
{
    int socketfd;
    sockaddr_in address;
    char remote_addrs[100];

    if(argc < 2)
    {
        // fprintf(stderr, "[ERROR] no IPv4 address range provided, use \"command address1 address2\", address should be like 192.168.1.*.\n");
        mlog(ERROR, "no IPv4 address range provided, use \"command address1 address2\", address should be like 192.168.1.*.");
        return 0;
    }

    for(int i = 1; i < argc; i++)
    {
        char cur_address[100];
        bool is_single = false;
        strcpy(remote_addrs, argv[i]);
        int addr_len = strlen(remote_addrs);
        for(int j = 0; j < addr_len; j++)
        {
            if(remote_addrs[j] == '*')
            {
                remote_addrs[j] = '%';
                remote_addrs[j+1] = 'd';
                remote_addrs[j+2] = '\0';
                // printf("[DEBUG] %s.\n", remote_addrs);
                break;
            }
            if(j == addr_len-1)
            {
                // printf("[DEBUG] %s.\n", remote_addrs);
                // printf("[INFO] scan single ip address.\n");
                mlog(INFO, "scan single ip address.");
                is_single = true;
                // return 0;
            }
        }
        int start = 1, end = 255;
        if(is_single)
        {
            start = end = 0;
        }
        for(int j = start; j <= end; j++)
        {
            strcpy(cur_address, remote_addrs);
            if(!is_single)
                sprintf(cur_address, remote_addrs, j);
            // printf("[DEBUG] qwq %s.\n", remote_addrs);
            for(int port = 1; port < 65536; port ++)
            {
                socketfd = socket(AF_INET, SOCK_STREAM, 0);
        
                if(socketfd < 0)
                {
                    // fprintf(stderr, "[ERROR] create socket error for address %s.\n", cur_address);
                    mlog(ERROR, "create socket error for address %s.", cur_address);
                    continue;
                }

                memset(&address, 0, sizeof(address));
                address.sin_family = AF_INET;
                address.sin_port = htons(port);
                if(inet_pton(AF_INET, cur_address, &address.sin_addr) < 0)
                {
                    // fprintf(stderr, "[ERROR] IPv4 address illegal.");
                    mlog(ERROR, "IPv4 address illegal.");
                    continue;
                }

                if(connect(socketfd, (sockaddr*)&address, sizeof(address)) < 0)
                {
                    // fprintf(stderr, "[ERROR] connect failed for address %s:%d.\n", cur_address, port);
                    mlog(ERROR, "connect failed for address %s:%d.", cur_address, port);
                    continue;
                }
                else
                {
                    // printf("[INFO] connect %s:%d successfully.\n", cur_address, port);
                    mlog(INFO, "connect %s:%d successfully.", cur_address, port);
                }
                close(socketfd);
            }
        }
    }

    return 0;
}

void scan_address()
{

}

void mlog(OUTPUT_TYPE type, const char *fmt, ...)
{
    char tmp[100], result[100];
    if(type < LOG_LEVEL)
        return;
    switch(type)
    {
        case INFO:
            sprintf(tmp, "[INFO] %s\n", fmt);
            break;
        
        case ERROR:
            sprintf(tmp, "[ERROR] %s\n", fmt);
            break;
        
        case DEBUG:
            sprintf(tmp, "[DEBUG] %s\n", fmt);
            break;
    }

    va_list args;
    int n;

    va_start(args, fmt);
    n = vsprintf(result, tmp, args);
    va_end(args);

    if(type == ERROR)
        fputs(result, stderr);
    else
        puts(result);
}
