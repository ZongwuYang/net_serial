OBJ = net_serial.o
CC = arm-hisiv100nptl-linux-gcc
CFLAGS = -g -lpthread

net_serial:$(OBJ)
	$(CC) $(CFLAGS) -o net_serial $(OBJ)
	
net_serial.o:net_serial.c
	 $(CC) $(CFLAGS) -c net_serial.c
	 
.PHONY:clean
clean:  
	rm -rf $(OBJ) net_serial 

