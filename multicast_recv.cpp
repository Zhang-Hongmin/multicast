/*------------------------------------------------------------------------------
 * @file    MULTICAST_RECV.CPP
 * @author  Zhang Fei 90008227
 * @date    2024/10/24 16:37:21
 * @brief   
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024, 三一集团重卡产品研究院电气技术所
 * All rights reserved.
------------------------------------------------------------------------------*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */

#define MULTICAST_GROUP "239.0.0.88" // 组播的ip地址 239.0.0.88
#define LO_IP "172.168.8.1"
#define PORT 8888

int main(int argc,char *argv[])
{
    //1.创建UDP套接字             
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1)
    {
        fprintf(stderr, "udp socket error,errno:%d,%s\n",errno,strerror(errno));
        exit(1);
    }

    // 地址和端口重用，多个进程监听同一端口
    int opt = 1;
    int err = setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void  *)&opt, sizeof(opt));
    if(err < 0)
    { 
        perror("setsockopt():SO_REUSEADDR"); 
        return -3;
    }
    printf("设置ip和端口重用成功\n");

    // /*设置回环许可*/
    // int loop = 1;
    // err = setsockopt(udp_socket, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)); 
    // if(err < 0)
    // { 
    //     perror("setsockopt():IP_MULTICAST_LOOP"); 
    //     return -3;
    // }
    // printf("设置回环许可成功\n");

    /*初始化地址*/
    struct sockaddr_in  host_addr;
    host_addr.sin_family         = AF_INET;                               //协议族，是固定的
    host_addr.sin_port           = htons(PORT);                    //目标端口，必须转换为网络字节序
    host_addr.sin_addr.s_addr   = htonl(INADDR_ANY); // inet_addr(LO_IP); // 

   //绑定端口和地址
    if (bind(udp_socket,(struct sockaddr *)&host_addr, sizeof(host_addr)) == -1) {
        printf("bind failed.\n");
        return -1;
    }
    printf("绑定成功\n");

    //加入组播
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); // inet_addr(LO_IP); // htonl(INADDR_ANY);
    if (setsockopt(udp_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) == -1) {
        printf("setsockopt failed.\n");
        return -1;
    }
    printf("加入多播组[%s : %d] 成功\n", MULTICAST_GROUP, PORT);


    // 调用recvfrom等待接收数据，并且接收客户端的网络信息
    char buf[128] = {0};
    struct sockaddr_in  client;
    socklen_t client_len = sizeof(client);

    while(1)
    {
        int bytes_received = recvfrom(udp_socket, buf, sizeof(buf),0, (struct sockaddr*)&client, &client_len);
        if(bytes_received == -1)
        {
            perror("recvfrom error");
            continue;
        }

        printf("Received %d bytes from [%s::%d]: %s\n", bytes_received, inet_ntoa(client.sin_addr), PORT, buf);
        bzero(buf, sizeof(buf));
    }

    setsockopt(udp_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof(mreq));
    close(udp_socket);
    return 0;
}

