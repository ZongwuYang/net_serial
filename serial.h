#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "debug.h"



typedef struct RecvDataFromSTM32
{
    char header[2]; 							//帧头

    float longitude;							//经度
    float latitude;								//纬度

    int ecu_total_mileage;						//[18FEC117] 总里程，factor:0.005 
    int ecu_total_fuel_consumption;				//[18FEE900] 总油耗，factor:0.5 
    short ecu_car_speed;						//[18FEF100] 车辆速度，factor:1/256 
    short ecu_engine_speed;						//[0CF00400] 发动机转速,factor:0.125 
    short ecu_instantaneous_fuel_consumption;	//[18FEF200] 瞬时油耗，factor:0.05 
    short ecu_battery_voltage;					//[18FEF700] 电瓶电压，factor:0.5 
    char ecu_water_temp;						//[18FEEE00] 水温，offset:-40 
    char ecu_torque;							//[0CF00400] 扭矩,offset:-125 
    char ecu_throttle;							//[0CF00300] 油门，factor:0.4 
    char ecu_brake_pedal;						//[18FEF100] 制动踏板
    char ecu_gear;								//[18F00503] 档位

    char checksum;//校验值
	
}__attribute__((packed)) RecvDataFromSTM32;


int open_serial(const char * device, int flags)
{
	int ret;
	ret = open(device, flags);
	return ret;
}

int set_serial(int fd, int nspeed, int nbits, char nevent, int nstop){
    struct termios old_state, new_state;
    if (tcgetattr(fd, &old_state)!=0){
        DEBUG("SetupSerial old\n");
        return -1;
    }
    bzero(&new_state,sizeof(new_state));
    new_state.c_cflag &= ~CSIZE; //将字符长度掩码

    switch(nspeed){
        case 2400:
            cfsetispeed(&new_state,B2400);
            cfsetospeed(&new_state,B2400);
            break;
        case 4800:
            cfsetispeed(&new_state,B4800);
            cfsetospeed(&new_state,B4800);
            break;
        case 9600:
            cfsetispeed(&new_state,B9600);
            cfsetospeed(&new_state,B9600);
            break;
        case 115200:
            cfsetispeed(&new_state,B115200);
            cfsetospeed(&new_state,B115200);
            break;
        case 460800:
            cfsetispeed(&new_state,B460800);
            cfsetospeed(&new_state,B460800);
            break;
        default:
            cfsetispeed(&new_state, B9600);
            cfsetospeed(&new_state, B9600);
            break;
    }

    switch(nbits){
        case 7:
            new_state.c_cflag |= CS7;
            break;
        case 8:
            new_state.c_cflag |= CS8;
            break;
    }

    switch(nevent){
        case 'O':
            new_state.c_cflag |= PARENB;
            new_state.c_cflag |= PARODD;
            new_state.c_cflag |= (INPCK | ISTRIP);
            break;
        case 'E':
            new_state.c_cflag |= PARENB;
            new_state.c_cflag |= (INPCK | ISTRIP);
            new_state.c_cflag &= ~PARODD;
            break;
        case 'N':
            new_state.c_cflag &= ~PARENB;
            break;
    }

    if(nstop==1)
        new_state.c_cflag &= ~CSTOPB;
    else if(nstop==2)
        new_state.c_cflag |= CSTOPB;
    new_state.c_cc[VTIME] = 0;
    new_state.c_cc[VMIN] = 0;
    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&new_state))!=0){
        DEBUG("serial set error\n");
        return -1;
    }
    DEBUG("serial set done!\n");
    return 0;
}
#endif
