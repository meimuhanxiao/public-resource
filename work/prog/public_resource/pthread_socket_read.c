#include "data_global.h"

uint8_t socket_rcv_data[100] = {0};
uint8_t socket_send_data[1024] = {0};//服务器询问展示数据，把展示数据发给服务器
extern bool server_response;
void *pthread_socket_read(void *arg)
{ 
label_wait_serok:
	//sem_wait(&sem_socket_ok);//等待连接到服务器
	printf("socket_read start\n");
	while(1){
		int n = read(sd,socket_rcv_data,100);
		if(n < 0){
			perror("read socket");
			printf("read socket error goto wait\n");
			goto label_wait_serok;//服务器断了 等待重新连接
		}
		if((socket_rcv_data[10] == 0x72) && (socket_rcv_data[9] == 0x81)){//查询展示数据
			int datalen = map_p[8]*256+map_p[7];
			memcpy(socket_send_data,map_p,datalen);//共享内存数据拷贝出来

			func_crc(socket_send_data,datalen,socket_send_data+datalen,socket_send_data+datalen+1);
			pthread_mutex_lock(&mutex_socket);	
			n = write(sd,socket_send_data,datalen+2);//展示数据发给服务器
			pthread_mutex_unlock(&mutex_socket);
			if(n < 0)
				perror("pthread_socket_read write");
		}
		else if((socket_rcv_data[10] == 0x70) && (socket_rcv_data[9] == 0xC8)){
			//有线程发送事件，服务器返回收到正确应答
			server_response = true;
			sem_post(&sem_ser_res);
		}
		else if((socket_rcv_data[10] == 0x70) && (socket_rcv_data[9] == 0xC6)){
			//有线程发送事件，服务器返回错误应答
			server_response = false;
			sem_post(&sem_ser_res);
		}

	}
	return NULL;
}
