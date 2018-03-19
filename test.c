#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"

int main()
{
    pid_t origin = getpid();
    printf("%d\n", getpid());
    pid_t newpid = fork();
    printf("%d\n", getpid());
    return 0;
}
