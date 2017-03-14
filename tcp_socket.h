#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include   <stdlib.h> 
#include   <string.h> 
#include   <sys/types.h> 
#include   <sys/socket.h> 
#include   <errno.h> 
#include   <unistd.h> 
#include   <netinet/in.h> 
#include   <limits.h> 
#include   <netdb.h> 
#include   <arpa/inet.h> 
#include   <ctype.h>

#define MAX_BUFFER_SIZE (1024 * 100)


char *mac_str = "000c291b2b1c";


int get_filesize(char* filename)  
{  
    FILE *fp=fopen(filename,"r");  
    if(!fp) return -1;  
    fseek(fp,0L,SEEK_END);  
    int size=ftell(fp);  
    fclose(fp);  
      
    return size;  
} 

int tcpclient_creat(const char* ip, int port)
{
	int socket_fd, flags;
	struct sockaddr_in server_addr;     
	
    server_addr.sin_family = AF_INET;  
    server_addr.sin_port = htons(port);  
	inet_pton(AF_INET, ip, &server_addr.sin_addr);
    //server_addr.sin_addr = inet_addr(ip);  
  
    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  
        return -1;  
    }  

	 /* Setting socket to nonblock */
  	flags = fcntl(socket_fd, F_GETFL, 0);
 	fcntl(socket_fd, flags|O_NONBLOCK);
	
    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {  
        return -1;  
    }  
    return socket_fd; 
}

static int send_data(int socket, char *buff, int size) 
{  
    int sent=0, tmpres=0;  
    while(sent < size) {  
        tmpres = send(socket,buff+sent,size-sent,0);  
        if(tmpres == -1){  
            return -1;  
        }
        sent += tmpres;  
    }  
    return sent;  
} 

static int recv_data(int socket, char *buff, int lenth)
{  
    int recvnum = 0;  
    recvnum = recv(socket, buff, lenth, 0);  
    return recvnum;  
} 

static int send_int_data(int val, int type, int socket) 
{  
    int ret = 0;
	int data_lenth = 4;
	char buff[128];
	bzero(buff, 128);
	int mac_str_len = strlen(mac_str);
	memcpy((void*)buff, (void*)mac_str, mac_str_len);				//MAC地址
	memcpy((void*)(buff + mac_str_len), (void*)&type, 4);			//type
	memcpy((void*)(buff + mac_str_len + 4), (void*)&data_lenth, 4);	//数据长度
	memcpy((void*)(buff + mac_str_len + 8), (void*)&val, 4);		//数据内容	
	ret = send_data(socket, buff, mac_str_len + 8 + 4);
	return ret;
} 


static int send_double_data(double val, int type, int socket) 
{  
    int ret = 0;
	int data_lenth = 8;
	char buff[128];
	bzero(buff, 128);
	int mac_str_len = strlen(mac_str);
	memcpy((void*)buff, (void*)mac_str, mac_str_len);				//MAC地址
	memcpy((void*)(buff + mac_str_len), (void*)&type, 4);			//type
	memcpy((void*)(buff + mac_str_len + 4), (void*)&data_lenth, 4);	//数据长度
	memcpy((void*)(buff + mac_str_len + 8), (void*)&val, 8);		//数据内容	
	ret = send_data(socket, buff, mac_str_len + 8 + 8);
	return ret;
} 

static int send_pic(char *picfile_name, int type, int socket)
{
	int ret, pic_size;
	char buff[MAX_BUFFER_SIZE];
	int mac_str_len = strlen(mac_str);
	FILE *file_fp;
	char *pic_buff;
	pic_size = get_filesize(picfile_name);
	pic_buff = (char*)malloc(pic_size * sizeof(char));
	file_fp = fopen(picfile_name,"r");
	ret = fread((void*)pic_buff, sizeof(char), pic_size, file_fp);
	if (ret != pic_size) {
		DEBUG("read file failed!\n");
		return -1;
	}
	memcpy((void*)buff, (void*)mac_str, mac_str_len);					//MAC地址
	memcpy((void*)(buff + mac_str_len), (void*)&type, 4);				//type
	memcpy((void*)(buff + mac_str_len + 4), (void*)&pic_size, 4);		//数据长度
	memcpy((void*)(buff + mac_str_len + 8), (void*)pic_buff, pic_size);	//数据内容
	ret = send_data(socket, buff, mac_str_len + 8 + pic_size);
	
	free(pic_buff);
	fclose(file_fp);
	return ret;
	
}



#endif
