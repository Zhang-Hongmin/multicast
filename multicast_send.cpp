/*------------------------------------------------------------------------------
 * @file    MULTICAST_SEND.CPP
 * @author  Zhang Fei 90008227
 * @date    2024/10/24 16:36:15
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
#include <arpa/inet.h>
#include <netinet/udp.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MULTICAST_GROUP "239.0.0.88"//组播的ip地址
#define PORT 8888 // 6666
#define LO_IP "172.168.8.1"
// 运行客户端可执行文件   ./xxx  服务器端口  服务器地址

int main(int argc,char *argv[])
{
    //1.创建UDP套接字             
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1)
    {
        fprintf(stderr, "udp socket error,errno:%d,%s\n",errno,strerror(errno));
        exit(1);
    }

    //2.向目标主机发送消息,需要设置目标端口和目标地址
    char buf[300] = {0};
    
    struct sockaddr_in  dest_addr;
    dest_addr.sin_family         = AF_INET;                         //协议族，是固定的
    dest_addr.sin_addr.s_addr  = inet_addr(MULTICAST_GROUP);            //服务器地址 "224.88.88.88"  
    dest_addr.sin_port           = htons(PORT);            //服务器端口，必须转换为网络字节序

//    /*设置回环许可*/
//     int loop = 1;
//     int err = setsockopt(udp_socket, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)); 
//     if(err < 0)
//     { 
//         perror("setsockopt():IP_MULTICAST_LOOP"); 
//         return -3;
//     }
//     printf("设置回环许可成功\n");

    //3.发送客户端的上线时间
    int cout = 100;
    usleep(1000000);
    while (--cout) 
    {
        // fprintf(stdout, "请输入发送内容:\n");
        // if (fgets(buf, sizeof(buf), stdin)) {
            sprintf(buf, "%s", "================");
            sendto(udp_socket,buf,strlen(buf),0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            fprintf(stdout, "udp socket send:%s\n", buf);
            usleep(100000);
        // }
    }
    setsockopt(udp_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof(mreq));
    close(udp_socket);

    return 0;
}