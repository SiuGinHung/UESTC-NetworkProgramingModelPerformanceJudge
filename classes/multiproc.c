#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

void
Doit(int sockfd)
{
    ssize_t n;
    char	buf[1024];
    while((n=read(sockfd,buf,sizeof(buf)))>0)
        write(sockfd,buf,n);
}

int
main(int argc , char ** argv)
{
    int 				listenfd,connfd;
    pid_t 				childpid;
    socklen_t			clilen;
    struct sockaddr_in 	cliaddr,servaddr;

    if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        fprintf(stderr,"socket:\n");
        exit(1);
    }

    bzero(&servaddr,sizeof(struct sockaddr_in));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(54324);

    if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))==-1)
    {
        fprintf(stderr,"bind:\n");
        exit(1);
    }

    if(listen(listenfd,5)==-1)
    {
        fprintf(stderr,"listen:\n");
        exit(1);
    }

    while(1)
    {
        clilen=sizeof(cliaddr);
        if((connfd=accept(listenfd,(struct sockaddr *)&cliaddr,&clilen))==-1)
        {
                fprintf(stderr,"accept:\n");
                exit(1);
        }
        childpid=fork();
        if(childpid==0)
        {
            close(listenfd);
            Doit(connfd);
            exit(0);
        }
        close(connfd);
    }

    return 0;
}



