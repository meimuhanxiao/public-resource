#CC=arm-none-linux-gnueabi-gcc
#CC=arm-linux-gnueabihf-gcc
CC=gcc
AFLAGS=-Wall -c -g
#LDFLAGS= -lpthread -L ~/work/sqlite/sqlite-arm/lib/ -lsqlite3
LDFLAGS= -lpthread  -lsqlite3

OBJS=main.o can_rcv_pthread.o datato_server.o pthread_ping_server.o func_crc.o\
	 pthread_ping_device.o pthread_query_showdata.o pthread_socket_read.o
public_res :$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJS):%.o:%.c
	$(CC) $(AFLAGS) $< -o $@


.PHONY:clean
clean:
	rm *.o app
