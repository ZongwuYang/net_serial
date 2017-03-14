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
 
    /* �������˿�ʼ����sockfd������ */ 
    if((*sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:IPV4;SOCK_STREAM:TCP
    { 
        DEBUG("Socket error:%s\n",strerror(errno)); 
        exit(1); 
    }

    /* ����������� sockaddr�ṹ */ 
    bzero(&server_addr,sizeof(struct sockaddr_in)); // ��ʼ��,��0
    server_addr.sin_family = AF_INET;                 // Internet
    //INADDR_ANY ��ʾ���Խ�������IP��ַ�����ݣ����󶨵����е�IP
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   
    server_addr.sin_port = htons(IPC_PORT);   
    /* ����sockfd��������IP��ַ */ 
    if(bind(*sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
    { 
        DEBUG("Bind error:%s\n",strerror(errno)); 
        exit(1); 
    }

    /* �����������ӵ����ͻ����� */ 
    if(listen(*sockfd,5)==-1) 
    { 
        DEBUG("Listen error:%s\n",strerror(errno)); 
        exit(1); 
    }
	/* ����������,ֱ���ͻ����������� */ 
    sin_size = sizeof(struct sockaddr_in); 
    if((ipc_fd = accept(*sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1) 
    { 
        DEBUG("Accept error:%s\n",strerror(errno)); 
        exit(1); 
    } 
	//�������ַת����.�ַ��� 
    DEBUG("Server get connection from %s\n",inet_ntoa(client_addr.sin_addr)); 
	return ipc_fd;
}

#endif

