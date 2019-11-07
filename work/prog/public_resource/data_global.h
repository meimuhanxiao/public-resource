#ifndef __DATA_GLOBAL__H__
#define __DATA_GLOBAL__H__
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <termios.h>
#include <syscall.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <stdint.h>
#include <linux/input.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "sqlite3.h"
#include <dirent.h>
#include <sys/socket.h> 
#include <linux/can.h> 
#include <linux/can/raw.h>
#include <net/if.h> 

#define LENGTH    1024*5
#define SERVER_IP     "192.168.2.175"
#define SERVER_PORT   8000
#define NOSENDTO_SERVER_EVENT    "nosendto_server_event"
#define DEVSTATUS_     "/home/linux/www/cgi-bin/dev_info/devstatus_"
#define DEV_           "/home/linux/www/cgi-bin/dev_info/dev_"
#define DEV_INFO_DIR   "/home/linux/www/cgi-bin/dev_info"
extern uint8_t can_rcv_buf[LENGTH];
extern void *pthread_can_rcv (void *);
extern void *pthread_datato_server(void *);
extern void *pthread_ping_server(void *);
extern void *pthread_ping_device(void *);
extern void *pthread_query_showdata(void *);
extern void *pthread_socket_read(void *);
extern pthread_t read_socket_id;
extern sem_t sem, ping_data_sem,sem_socket_ok,sem_ser_res;
extern pthread_cond_t   cond_datato_server;
extern pthread_mutex_t  mutex_datato_server,mutex_socket,mutex_eventno,mutex_can_rcv_buf;
extern void func_crc(const uint8_t *buf,uint32_t len,uint8_t *out_low,uint8_t *out_high);
extern bool server_status_ok,can_rcv_flag;
extern int can_fd,sd;
extern uint8_t *map_p;//共享内存地址
extern int dev_num;//所接设备数量
extern bool server_response;


#endif
