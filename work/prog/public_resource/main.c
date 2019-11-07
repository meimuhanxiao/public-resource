#include "data_global.h"

sqlite3 *db;
pthread_t  can_rcv_id, datato_server_id,ping_server_id,ping_device_id,\
			   query_showdata_id,read_socket_id;
extern int shmid;
extern int can_init();
pthread_mutex_t mutex_can_rcv_buf;
void create_db()
{
	//打开或者创建数据库
	int res = sqlite3_open("/home/linux/www/db/event_show.db",&db);
	if(res != SQLITE_OK){
		printf("can't open database\n");
		perror("open database");
		exit(1);
	}
	printf("open database ok !\n");

	char buf[1024] = {0};
	char *err_msg = 0;
	sprintf(buf,"create table if not exists event_table(设备ID varchar(10),事件序号 varchar(20),事件时间 varchar(60),事件对象 varchar(10),事件类型 varchar(20),设备方向 varchar(5));");
	res = sqlite3_exec(db,buf,0,0,&err_msg);
	if (res != SQLITE_OK){
		printf ("Error:%s",err_msg);
		sqlite3_close (db);
		exit(1);
	}
	printf ("create event_table OK!\n");

}
void ReleaseResource (int signo)
{
	pthread_cancel(can_rcv_id); //释放线程
	pthread_cancel(datato_server_id);
	pthread_cancel(ping_server_id);
	pthread_cancel(ping_device_id);
	pthread_cancel(query_showdata_id);
	pthread_cancel(read_socket_id);

	pthread_mutex_destroy (&mutex_datato_server);	
	pthread_cond_destroy (&cond_datato_server);
	pthread_mutex_destroy (&mutex_socket);
	pthread_mutex_destroy (&mutex_eventno);
	pthread_mutex_destroy(&mutex_can_rcv_buf);

	shmctl(shmid, IPC_RMID, NULL);
	sem_destroy(&sem);
	sem_destroy(&ping_data_sem);
	sem_destroy(&sem_socket_ok);
	sem_destroy(&sem_ser_res);
	sqlite3_close (db);
	exit(0);
}

int main(int argc, const char *argv[])
{
	create_db();
	can_fd = can_init();

	//初始化互斥锁 条件变量
	pthread_mutex_init (&mutex_datato_server, NULL);	
	pthread_cond_init (&cond_datato_server, NULL);
	pthread_mutex_init (&mutex_socket, NULL);	
	pthread_mutex_init(&mutex_eventno,NULL);
	pthread_mutex_init(&mutex_can_rcv_buf,NULL);
	sem_init(&sem, 0, 0);//初始化信号量为0
	sem_init(&ping_data_sem, 0, 1);
	sem_init(&sem_socket_ok,0,0);
	sem_init(&sem_ser_res,0,0);
	//注册SIGINI函数
	signal (SIGINT, ReleaseResource);
	//开启线程
	pthread_create (&can_rcv_id, 0, pthread_can_rcv, NULL);
	pthread_create(&datato_server_id,0,pthread_datato_server,NULL);
	pthread_create(&ping_server_id,0,pthread_ping_server,NULL);
	pthread_create(&ping_device_id,0,pthread_ping_device,NULL);
	pthread_create(&query_showdata_id,0,pthread_query_showdata,NULL);

	//阻塞等待回收线程资源
	pthread_join (can_rcv_id, NULL);
	printf ("can_rcv_pthread exit\n");
	pthread_join (datato_server_id, NULL);
	printf ("datato_server_pthread exit\n");
	pthread_join (ping_server_id, NULL);
	printf ("ping_server_pthread exit\n");
	pthread_join (ping_device_id, NULL);
	printf ("pthread_ping_device exit\n");
	pthread_join (query_showdata_id, NULL);
	printf ("pthread_query_showdata exit\n");
	pthread_join (read_socket_id, NULL);
	printf ("pthread_read_socket exit\n");
	printf("main thread exit\n");


	return 0;
}
