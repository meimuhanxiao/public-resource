#include "data_global.h"
//每10s ping 服务器 ping 通了 将server_status_ok为true 如果以前有存的没发出去事件，把事件发出去
//不通 server_status_ok为false 

bool server_status_ok = false;
bool reconnect_server = true;
extern int sd;
struct sockaddr_in raddr;
extern bool have_event_noto_server;
extern pthread_mutex_t mutex_socket;
sem_t sem_socket_ok;
int socket_init()
{
	server_status_ok = false;
	sd = socket(AF_INET,SOCK_STREAM,0);//生成一个socket 用这个socket连接服务器
	if(sd < 0){
		perror("socket()");
		return -1;
	}
	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET,SERVER_IP,&raddr.sin_addr);
	return 0;
}
int connect_server()
{
	if(connect(sd,(void *)&raddr,sizeof(raddr)) < 0){
		perror("connect server");
		return -1;
	}
	server_status_ok = true;//socket连接到服务器了
	reconnect_server = false;
	printf("connect server successful\n");
	//sem_post(&sem_socket_ok);//连接服务器成功了置为1 read_socket 线程才启动,否则read_socket 线程阻塞等待
	return 0;
}
void send_remain_eventto_server()
{
	FILE *fp = fopen(NOSENDTO_SERVER_EVENT,"r");
	if(fp == NULL)
		perror("open nosendto_server_event");
	char *line = NULL;
	size_t len = 0;
	pthread_mutex_lock(&mutex_socket);
	int n = 0;
	int read_count = 0;
	uint8_t line_comp[1024] = {0};
	while((n=getline(&line, &len, fp)) != -1){//读一行数据 发一行数据
		if(((n-38-2-1)%57)!=0){//数据中有\n 读到一次事件一部分
			memcpy(line_comp+read_count,line,n);//读出来的放进buf里
			read_count += n;//读出的字节数累加
			if(((read_count-38-2-1)%57) == 0){//读出了完整一次事件 38头部 2 crc 1 \n
				write(sd,line_comp,read_count-1);//去掉换行符
				sem_wait(&sem_ser_res);//等待服务器回应
				if(server_response == true){
					printf("server response ok\n");
				}
				else{
					printf("server response error\n");
				}
				bzero(line_comp,1024);
				read_count = 0;
			}
		}
		else{//数据中没有\n 直接读出一次事件
		//	memcpy(line_comp,(uint8_t*)line,n-1);
			write(sd,(uint8_t*)line,n-1);//去掉换行符
			sem_wait(&sem_ser_res);//等待服务器回应
			if(server_response == true){
				printf("server response ok\n");
			}
			else{
				printf("server response error\n");
			}
		//	bzero(line_comp,1024);
		}
	}
	pthread_mutex_unlock(&mutex_socket);
	free(line);
	fclose(fp);
	system("cat nosendto_server_event >> nosendto_server_event_all");
	system("rm nosendto_server_event");//发送完已存的事件 删除
}
void ping_server()
{
	char str[100] = {0};
	sprintf(str,"ping -c 1 -w 1 %s > ping_server_status",SERVER_IP);
	system(str);
	sprintf(str,"ping -c 1 -w 1 %s >> ping_server_status",SERVER_IP);
	system(str);
	system("wc -l ping_server_status > ping_server_result");
	FILE *fp = fopen("ping_server_result","r");
	char *line = NULL;
	size_t len = 0;

	if(fp == NULL){
		perror("fopen()");
		
	}
	getline(&line, &len, fp);
//	printf("line = %s\n",line);
	if(atoi(line) == 12){//服务器正常
		server_status_ok = true;
		printf("server on line\n");

		if(reconnect_server == true){
			socket_init();//重新初始化socket
			while(connect_server()){//重新连接到服务器
				sleep(5);
				printf("正在连接服务器\n");
				server_status_ok = false;
			}
		}
		if(read_socket_id == 0)//连接服务器成功了，开启读取服务器返回数据线程
			pthread_create(&read_socket_id,0,pthread_socket_read,NULL);
		if(have_event_noto_server == true){
			send_remain_eventto_server();
			have_event_noto_server = false;//把文本里的事件发完 将标志置为false
		}
	}
	else{
		server_status_ok = false;
		if(reconnect_server == false)//sd有有效socket
			close(sd);//关闭socket

		reconnect_server = true;//ping不通服务器 下次ping同时,需要重新连接服务器
		if(read_socket_id != 0)	{//read_socket 线程正在运行
			pthread_cancel(read_socket_id);//销毁这个线程
		    read_socket_id = 0;
		}
		printf("server offline\n");
	}
	printf("server_status_ok = %d\n",server_status_ok);
	printf("reconnect_server = %d\n",reconnect_server);
	free(line);
	fclose(fp);
}

void *pthread_ping_server(void *arg)
{
	socket_init();
	while(connect_server()){//连接到服务器跳出循环
		sleep(10);
		printf("正在连接服务器\n");
		server_status_ok = false;
	}

	while(1){//连上服务器每10s检查服务器是否在线
		sleep(5);
		ping_server();
	}
}

