#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
 
//receive it
#define MULTICAST_ADDR "239.1.1.1"
#define MULTICAST_PORT 8888
//组播地址以239.1.1.1 为例.
int main(int argc, char **argv)
{
 
    //1.创建socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
    {
        printf("socket creating err in udptalk\n");
        exit(EXIT_FAILURE);
    }
 
    //设置sock可选项, 可重复使用地址
    unsigned int yes = 1;
    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) 
    {
        perror("Reusing ADDR failed");
        exit(1);
    }
 
    //receive message 需要先bind socket, 说明从哪个地址,哪个端口接受. send 则不需要
    struct sockaddr_in myaddr;
    unsigned int socklen = sizeof(struct sockaddr_in);
    memset(&myaddr, 0, socklen);
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(MULTICAST_PORT);
    inet_pton(AF_INET, MULTICAST_ADDR, &myaddr.sin_addr);
 
    if (bind(sockfd, (struct sockaddr *) &myaddr,sizeof(struct sockaddr_in)) == -1) 
    {
        printf("Bind error\n");
        exit(EXIT_FAILURE);
    }
 
    //设置sock可选项，将当前sock加入到组播组
    struct ip_mreq mreq;
    bzero(&mreq, sizeof(struct ip_mreq));
    inet_pton(AF_INET, MULTICAST_ADDR, &mreq.imr_multiaddr.s_addr);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq,sizeof(struct ip_mreq)) == -1) 
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
 
 
    #define BUFLEN 255
    char recmsg[BUFLEN + 1];
    unsigned int n;
    struct sockaddr_in peeraddr;
    for (;;) 
    {
        bzero(recmsg, BUFLEN + 1);
        printf("waiting messge ... \n");
        n = recvfrom(sockfd, recmsg, BUFLEN, 0,(struct sockaddr *) &peeraddr, &socklen);
        if (n < 0) 
        {
            printf("recvfrom err!\n");
            exit(EXIT_FAILURE);
        } 
        else 
        {
 
            recmsg[n] = 0;
            printf("peer:%s", recmsg);
        }
    }
}