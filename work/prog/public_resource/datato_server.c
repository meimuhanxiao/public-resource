#include "data_global.h"
//can 收到数据 如果是事件 会通知这个线程 这个线程把can_rcv_buf里数据
//上传到服务器，如果发现服务器状态是false 把这事件存到文本里
int sd;
pthread_cond_t   cond_datato_server;
pthread_mutex_t  mutex_datato_server,mutex_socket;
extern bool reconnect_server , server_status_ok;
bool have_event_noto_server = false;
sem_t sem_ser_res;//阻塞等待服务器响应 由read_socket 线程解除阻塞
bool server_response;//由read_socket 线程通过接收的服务器数据来判断服务器接收数据是否正确
void *pthread_datato_server(void *arg)
{
	uint8_t buf[1024*5] = {0};
	FILE *fp = fopen(NOSENDTO_SERVER_EVENT,"r");
	if(fp == NULL)
		have_event_noto_server = false;
	else{
		have_event_noto_server = true;
		fclose(fp);
	}
	while(1){
		bzero(buf,1024*5);
		pthread_mutex_lock (&mutex_datato_server);
		pthread_cond_wait (&cond_datato_server, &mutex_datato_server);//阻塞等待can接收到实时事件
		printf("datato_server  start\n");
		memcpy(buf,can_rcv_buf,can_rcv_buf[8]*256+can_rcv_buf[7]+2);//拷贝出接收到事件 上传到服务器
		sem_post(&sem);//取到了数据 告诉can_rcv_pthread 可以清空can_rcv_buf
		if(server_status_ok){
			pthread_mutex_lock (&mutex_socket);
			int n = write(sd,buf,buf[8]*256+buf[7]+2);//服务器连接ok把收到事件数据发到服务器
			if( n <= 0){
				perror("socket write");
			
			}
			if(n == buf[8]*256+buf[7]+2)
				printf("send data to server successful\n");
			else
				printf("send data to server error");

			sem_wait(&sem_ser_res);//等待服务器回应
			if(server_response == true){
				printf("server response ok\n");
			}
			else{
				printf("server response error\n");
			}

			pthread_mutex_unlock (&mutex_socket);

		}
		else{//服务器连不上把接收到数据存到文本里
			int fd = open(NOSENDTO_SERVER_EVENT,O_CREAT|O_RDWR|O_APPEND,0664);
			if(fd < 0)
				perror("open NOSENDTO_SERVER_EVENT error");
			int n = write(fd,buf,buf[8]*256+buf[7]+2);
			if(n == buf[8]*256+buf[7]+2)
				printf("store nosendto_server_event sucessfull\n");
			else
				printf("store nosendto_server_event error");
			write(fd,"\n",1);
			close(fd);
			have_event_noto_server = true;//有事件没发到服务器
		}
		pthread_mutex_unlock (&mutex_datato_server);
	}	
//	close(sd);
}


