#ifndef __IPC_SERVER_H__
#define __IPC_SERVER_H__

#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h>

#define IPC_PORT 6666

int create_ipc_server(int *sockfd)
{
	int ipc_fd, sin_size; 
    struct sockaddr_in server_addr, client_addr; 
 
    /* 服务器端开始建立sockfd描述符 */ 
    if((*sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:IPV4;SOCK_STREAM:TCP
    { 
        DEBUG("Socket error:%s\n",strerror(errno)); 
        exit(1); 
    }

    /* 服务器端填充 sockaddr结构 */ 
    bzero(&server_addr,sizeof(struct sockaddr_in)); // 初始化,置0
    server_addr.sin_family = AF_INET;                 // Internet
    //INADDR_ANY 表示可以接收任意IP地址的数据，即绑定到所有的IP
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   
    server_addr.sin_port = htons(IPC_PORT);   
    /* 捆绑sockfd描述符到IP地址 */ 
    if(bind(*sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
    { 
        DEBUG("Bind error:%s\n",strerror(errno)); 
        exit(1); 
    }

    /* 设置允许连接的最大客户端数 */ 
    if(listen(*sockfd,5)==-1) 
    { 
        DEBUG("Listen error:%s\n",strerror(errno)); 
        exit(1); 
    }
	/* 服务器阻塞,直到客户程序建立连接 */ 
    sin_size = sizeof(struct sockaddr_in); 
    if((ipc_fd = accept(*sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1) 
    { 
        DEBUG("Accept error:%s\n",strerror(errno)); 
        exit(1); 
    } 
	//将网络地址转换成.字符串 
    DEBUG("Server get connection from %s\n",inet_ntoa(client_addr.sin_addr)); 
	return ipc_fd;
}

#endif

