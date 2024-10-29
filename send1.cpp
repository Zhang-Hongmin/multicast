#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define MULTICAST_ADDR "239.1.1.1"
#define MULTICAST_PORT 8888
//组播, 发送与普通udp 发送并无不同,除了sendto的对端地址是一个组播地址.
//sendit
int main(int argc, char **argv)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);  
    if (sockfd < 0)
    {
        printf("socket creating error\n");
        exit(EXIT_FAILURE);
    }
    unsigned int socklen = sizeof(struct sockaddr_in);
    struct sockaddr_in peeraddr;
    memset(&peeraddr, 0, socklen);
    peeraddr.sin_family = AF_INET;
    peeraddr.sin_port = htons(MULTICAST_PORT);    
    if (inet_pton(AF_INET, MULTICAST_ADDR, &peeraddr.sin_addr) <= 0) {
        printf("wrong group address!\n");
        exit(EXIT_FAILURE);
    }
#define BUFLEN 255
    char recmsg[BUFLEN + 1];
    for (;;) {
        bzero(recmsg, BUFLEN + 1);
        printf("input message to send:");
        if (fgets(recmsg, BUFLEN, stdin) == (char *) EOF) 
            exit(EXIT_FAILURE);;
        if (sendto(sockfd, recmsg, strlen(recmsg), 0,(struct sockaddr *) &peeraddr,
                    sizeof(struct sockaddr_in)) < 0)      
        {
            printf("sendto error!\n");
            exit(EXIT_FAILURE);;
        }
        printf("send message:%s", recmsg);  
    }
}