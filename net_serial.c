/******************************************************************************
  A simple program of Hisilicon HI3520D video input and output implementation.
  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2012-12 Created
******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "debug.h"
#include "serial.h"
#include "tcp_socket.h"
#include "ipc_server.h"
#include "ipc_order.h"
#include "comm_head.h"

 
#define SERIAL_DEVICE "/dev/ttyUSB0"
#define PIC_FILE_NAME "snap.jpg"


//#define MAIN_SERVER_IP		"192.168.1.12"
#define MAIN_SERVER_IP		"192.168.1.199"
#define MAIN_SERVER_PORT	3800




/**************************************************************************
* 多线程共享变量
***************************************************************************/
unsigned short capture_flag;
int serial_fd, ipc_fd, server_fd;
pthread_mutex_t mutex;

/**************************************************************************/

/*************************************************************************
*线程内延时函数:毫秒级别
*************************************************************************/
void pthread_delay(unsigned long mSec)
{
    struct timeval tv;
    tv.tv_sec=mSec/1000;
    tv.tv_usec=(mSec%1000)*1000;
    int err;
    do{
       err=select(0,NULL,NULL,NULL,&tv);
    }while(err<0 && errno==EINTR);
}

/*************************************************************************
*线程函数:串口收数据，网口发数据
*************************************************************************/
void serialrecv_netsend()
{
	int i, n;
	char check_sum;
	unsigned short type;

	RecvDataFromSTM32 recvdata;

	//test data
	recvdata.longitude = 12.345;
	recvdata.latitude = 34.5422;
	recvdata.ecu_total_mileage = 34;
	recvdata.ecu_total_fuel_consumption = 23;
	recvdata.ecu_car_speed = 45;
	recvdata.ecu_engine_speed = 3451;
	recvdata.ecu_instantaneous_fuel_consumption = 456;
	recvdata.ecu_battery_voltage = 11;
	recvdata.ecu_water_temp = 34;
	recvdata.ecu_torque = 2;
	recvdata.ecu_throttle = 32;
	recvdata.ecu_brake_pedal = 32;
	recvdata.ecu_gear = 2;

	DEBUG("RecvDataFromSTM32 struct size = %d\n", sizeof(RecvDataFromSTM32));
	
	fd_set rd;
	FD_ZERO(&rd);
	FD_SET(serial_fd, &rd);	
	while(1)
    {    
        n = 0;
		FD_ZERO(&rd);
		FD_SET(serial_fd, &rd);
		
		/*pthread_mutex_lock(&mutex);
		
		send_double_data(recvdata.longitude, N_LONGITUDE, server_fd);
		send_double_data(recvdata.latitude, N_LATITUDE, server_fd);
		send_int_data(recvdata.ecu_total_mileage, N_TOTAL_MILEAGE, server_fd);
		send_int_data(recvdata.ecu_total_fuel_consumption, N_TOTAL_FUEL_CONS, server_fd);
		send_int_data(recvdata.ecu_car_speed, N_SPEED, server_fd);
		send_int_data(recvdata.ecu_engine_speed, N_ENGINE_SPEED, server_fd);
		send_int_data(recvdata.ecu_instantaneous_fuel_consumption, N_INSTANT_FUEL_CONS, server_fd);
		send_int_data(recvdata.ecu_battery_voltage, N_BATTERY_VOLTAGE, server_fd);
		send_int_data(recvdata.ecu_water_temp, N_WATER_TEMP, server_fd);
		send_int_data(recvdata.ecu_torque, N_TORQUE, server_fd);
		send_int_data(recvdata.ecu_throttle, N_THROTTLE, server_fd);
		send_int_data(recvdata.ecu_brake_pedal, N_BRAKE_PEDAL, server_fd);
		send_int_data(recvdata.ecu_gear, N_GEARL, server_fd);
		
		pthread_mutex_unlock(&mutex);
		DEBUG("send data finished\n");  
		pthread_delay(5000);*/
		
		switch (select(serial_fd + 1, &rd, NULL, NULL, NULL)) {
			case -1:
				break;
			case 0:
				break; //再次轮询 
			default:
				if (FD_ISSET(serial_fd, &rd)) {
					//usleep(1);
					pthread_delay(100);
					//pthread_mutex_lock(&mutex);
					n = read(serial_fd, (char*)&recvdata, sizeof(RecvDataFromSTM32));
					for (i = 0; i < sizeof(RecvDataFromSTM32) - 1; i++) {
						check_sum ^= ((char*)&recvdata)[i];
					}
					DEBUG("check_sum = %d  recvdata.checksum = %d\n",check_sum,  recvdata.checksum);
					//DEBUG("sizeof(RecvDataFromSTM32) = %d read from serial %d\n", sizeof(RecvDataFromSTM32),n);	
					if (n > 0 && check_sum == recvdata.checksum) {
						DEBUG("sizeof(RecvDataFromSTM32) = %d read from serial %d\n", sizeof(RecvDataFromSTM32),n);
						send_double_data(recvdata.longitude, N_LONGITUDE, server_fd);
						send_double_data(recvdata.latitude, N_LATITUDE, server_fd);
						send_int_data(recvdata.ecu_total_mileage, N_TOTAL_MILEAGE, server_fd);
						send_int_data(recvdata.ecu_total_fuel_consumption, N_TOTAL_FUEL_CONS, server_fd);
						send_int_data(recvdata.ecu_car_speed, N_SPEED, server_fd);
						send_int_data(recvdata.ecu_engine_speed, N_ENGINE_SPEED, server_fd);
						send_int_data(recvdata.ecu_instantaneous_fuel_consumption, N_INSTANT_FUEL_CONS, server_fd);
						send_int_data(recvdata.ecu_battery_voltage, N_BATTERY_VOLTAGE, server_fd);
						send_int_data(recvdata.ecu_water_temp, N_WATER_TEMP, server_fd);
						send_int_data(recvdata.ecu_torque, N_TORQUE, server_fd);
						send_int_data(recvdata.ecu_throttle, N_THROTTLE, server_fd);
						send_int_data(recvdata.ecu_brake_pedal, N_BRAKE_PEDAL, server_fd);
						send_int_data(recvdata.ecu_gear, N_GEARL, server_fd);
					}
					//pthread_mutex_unlock(&mutex);
				}			
		}	
    }
	
}

/*************************************************************************
*线程函数: 网口收数据(服务端命令接收等)
*************************************************************************/
void recvcmd_fromsever()
{
	int recv_count, recv_val, order_val;
	int pic_size;
	char recv_buff[128];
	while (1) {
		//pthread_mutex_lock(&mutex);
		recv_count = recv_data(server_fd, recv_buff, 128);
		//pthread_mutex_unlock(&mutex);
		if (recv_count > 0) {
			recv_val = *(int*)recv_buff;
			switch (recv_val) {
				case SCREENSHOT: {
					order_val = SCREENSHOT;
					DEBUG("recv sreenshot order\n");
					pthread_mutex_lock(&mutex);
					send_data(ipc_fd, (char*)&order_val, 4);
					recv_count = recv_data(ipc_fd ,recv_buff, 128);
					recv_val = *(int*)recv_buff;
					if (recv_val == SCREENSHOT_END) {
						send_pic(PIC_FILE_NAME, N_PIC, server_fd);
					}
					pthread_mutex_unlock(&mutex);
					break;
				}
				default: {
					break;
				}
			}
		}
	}
}


/******************************************************************************
* function    : main()
******************************************************************************/
int main(int argc, char *argv[])
{
	int ret, count, ipc_socket;
	char buff[100];
	//pthread_t sr_ns, nr_ns;


	
	//ipc_fd = create_ipc_server(&ipc_socket);

	server_fd = tcpclient_creat(MAIN_SERVER_IP, MAIN_SERVER_PORT);
		
	/*用默认属性初始化一个互斥锁对象*/
	//pthread_mutex_init(&mutex,NULL);

	serial_fd = open_serial(SERIAL_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == serial_fd) {
		DEBUG("open serial port failed\n");
	}
	set_serial(serial_fd, 115200, 8, 'N', 1);
	
	/*ret = pthread_create(&sr_ns, NULL, (void *)serialrecv_netsend, NULL);
	if (ret != 0) {
		DEBUG("create serialrecv_netsend pthread failed!\n");
	} else {
		DEBUG("create serialrecv_netsend pthread success!\n");
	}

	ret = pthread_create(&nr_ns, NULL, (void *)recvcmd_fromsever, NULL);
	if (ret != 0) {
		DEBUG("create recvcmd_fromsever pthread failed!\n");
	} else {
		DEBUG("create recvcmd_fromsever pthread success!\n");
	}*/

	while (1) {
		serialrecv_netsend();
	}

	close(serial_fd);
	close(server_fd);
	return 0;
	
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

