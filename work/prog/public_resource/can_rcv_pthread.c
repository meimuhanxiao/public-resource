#include "data_global.h"
#define PATH_NAME 	  "/"
#define PRO_ID		  100
//can 接收到数据 如果是事件 通知 发送到服务器线程 把事件发到服务器，然后把事件
//存入数据库，并且改写文本中设备状态，如果收到是展示数据把气象数据部分放到
//共享内存,展示数据设备状态部分由 query_showdata
//线程填写(打开对应文本把设备状态写道共享内存中相应位置)
pthread_cond_t cond_datato_server;
pthread_mutex_t mutex_eventno;//事件序号锁
uint8_t can_rcv_buf[LENGTH];
extern sqlite3 *db;
uint8_t crc_low,crc_high;
uint16_t datalen = 0;//发过来数据的长度不包括2字节crc
int count = 0; //每次从can得到几个字节数据
extern void func_crc( const uint8_t *buf,  uint32_t  len ,  uint8_t *out_low,  uint8_t *out_high );
sem_t sem;
int shmid;
int can_fd; 
bool can_rcv_flag = false;
uint8_t *map_p; //共享内存地址 

uint8_t *mem_map()
{
	key_t key;
	if((key = ftok(PATH_NAME, PRO_ID)) < 0)//使用ftok函数获取键值
	{
		perror("fail to ftok");
		exit(1);
	}
	if((shmid = shmget(key, 1024*5, IPC_CREAT | 0777)) < 0)//创建或者打开一个共享内存

	{
		perror("fail to shmget");
		exit(1);
	}
	uint8_t *p = shmat(shmid, NULL, 0);
	return p;
}

int can_init()
{
//	system("ifconfig can0 down");
//	system("ip link set can0 type can bitrate 20000 triple-sampling on ");
	//	system("ip link set can0 type can loopback on");
//	system("ifconfig can0 up");
//	system("sudo modprobe vcan");
//	system("sudo ip link add dev can0 type vcan");
//	system("sudo ip link set up can0");

	int can_fd;  
	struct sockaddr_can addr;  
	struct ifreq ifr;   
	can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);      //创建套接字  
	strcpy(ifr.ifr_name, "can0" );  
	ioctl(can_fd, SIOCGIFINDEX, &ifr);                   //指定can0设备  
	addr.can_family = AF_CAN;  
	addr.can_ifindex = ifr.ifr_ifindex;  
	bind(can_fd, (struct sockaddr *)&addr, sizeof(addr));            //将套接字与can0绑定  
	//禁用过滤规则，本进程不接收报文，只负责发送  
	//	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);  
	return can_fd;
	
}

void right_response_event()
{
	uint8_t event_right_response_data[15] = {
		0xFE,0xAA,0x02,0x01,0x00,0x02,0x00,0x0D,0x00,0xC8,0x70,0x00,0x00,0x2B,0x4F
	};
	struct can_frame r_frame = {0}; 
	r_frame.can_id = 0x010;
	memcpy(r_frame.data,event_right_response_data,8);
	r_frame.can_dlc = 8;
	write(can_fd,&r_frame,sizeof(r_frame));

	memcpy(r_frame.data,event_right_response_data+8,7);
	r_frame.can_dlc = 7;
	write(can_fd,&r_frame,sizeof(r_frame));
}

void wrong_response()
{
	uint8_t event_wrong_response_data[15] = {
		0xFE,0xAA,0x02,0x01,0x00,0x02,0x00,0x0D,0x00,0xC6,0x70,0x00,0x00,0x29,0xa7
	};
	struct can_frame w_frame = {0}; 
	w_frame.can_id = 0x010;
	memcpy(w_frame.data,event_wrong_response_data,8);
	w_frame.can_dlc = 8;
	write(can_fd,&w_frame,sizeof(w_frame));

	memcpy(w_frame.data,event_wrong_response_data+8,7);
	w_frame.can_dlc = 7;
	write(can_fd,&w_frame,sizeof(w_frame));
}

void can_rcv_timeout(int sig)
{
	bzero(can_rcv_buf,LENGTH);//can error 清空buf
	count = 0;
	datalen = 0;
	wrong_response();
	can_rcv_flag = false;
	printf("can timeout\n");
}

int read_eventno()
{
	FILE *fp = fopen("eventno.txt","r");
	if(fp == NULL){
		perror("eventno.txt");
		return -1;
	}
	char *line = NULL;
	size_t len = 0;

	getline(&line, &len, fp) ;
	int count = atoi(line);
	fclose(fp);
	free(line);
	return count;
}

void write_eventno(int eventno)
{
	char s[10] = {0};
	sprintf(s,"%d",eventno);//事件整数转成字符串
	FILE *fp = fopen("eventno.txt","r+");//写道文件里
	if(fp == NULL){
		perror("eventno.txt");
		return ;
	}

	fwrite(s,strlen(s),1,fp);
	fclose(fp);
}
void fill_can_rcv_buf(int eventno,int i)
{
	can_rcv_buf[49+i*57] = eventno /16777216; //填写事件序号
	can_rcv_buf[48+i*57] = eventno %16777216 /65536 ;
	can_rcv_buf[47+i*57] = eventno % 65536 / 256;
   	can_rcv_buf[46+i*57] = eventno %256;

	time_t t;
	time(&t);
	t += 8*60*60;//现在时间+8小时
	can_rcv_buf[53+i*57] = t /16777216; //填写事件时间
	can_rcv_buf[52+i*57] = t %16777216 /65536 ;
	can_rcv_buf[51+i*57] = t % 65536 / 256;
	can_rcv_buf[50+i*57] = t %256;	
	can_rcv_buf[72+i*57] = 0xa0; //把0x0a 替换调 在文本里存储会认为是换行符
}
void store_event_intodatabase()//存入事件到数据库并且根据事件改写设备状态
{
	char id[9] = {0};
	int c = 0,i;  //事件个数
	for(c = 0;c < can_rcv_buf[37];c++){

		for(i = 0; i < 8;i++)
			id[i] = can_rcv_buf[38+i+c*57];
		int id_int = atoi(id);
		char filename[100] = {0};
		sprintf(filename,"%s%d",DEVSTATUS_,id_int);
		FILE *fp = fopen(filename,"r");//读取设备状态信息
		if(fp == NULL){
			perror("read devstatus error");
			return;
		}
		char *line = NULL;
		size_t len = 0;
		char dev_status[5] = {0};
		char fault_count[5] = {0};
		char fault_type[5] = {0};
		getline(&line, &len, fp);//得到设备状态
		strcpy(dev_status,line);

		getline(&line,&len,fp);//得到故障次数
		strcpy(fault_count,line);

		getline(&line,&len,fp);//得到了故障类型
		strcpy(fault_type,line);
		fclose(fp);

		char event_no[10] = {0};
		sprintf(event_no,"%d",can_rcv_buf[49+c*57]*16777216+can_rcv_buf[48+c*57]*65536+can_rcv_buf[47+c*57]*256+can_rcv_buf[46+c*57]);

		char date[100] = {0};
		uint32_t fault_time = can_rcv_buf[53+c*57]*16777216 + can_rcv_buf[52+c*57]*65536+can_rcv_buf[51+c*57]*256+can_rcv_buf[50+c*57];
		time_t t = fault_time;
		t -= 8*60*60;//现在时间
		struct tm* mydate;
		mydate = localtime (&t);
		sprintf(date,"%04d/%02d/%02d-%02d:%02d:%02d",mydate->tm_year+1900,mydate->tm_mon + 1,mydate->tm_mday,mydate->tm_hour,mydate->tm_min,mydate->tm_sec);

		char direct[10] = {0};
		switch(can_rcv_buf[94+c*57]){
		case 0:
			strcpy(direct,"无");
			break;
		case 1:
			strcpy(direct,"东");
			break;
		case 2:
			strcpy(direct,"南");
			break;
		case 3:
			strcpy(direct,"西");
			break;
		case 4:
			strcpy(direct,"北");
			break;
		}

	
		fp = fopen(filename,"w");//根据事件重写设备状态
		if(fp == NULL){
			perror("write device status error");
			return ;
		}
		char obj[20] = {0};
		char event_type[50] = {0};
		char fault_occur_time[100] = {0};
		switch(can_rcv_buf[54+c*57]){//事件对象
		case 0x30:{
					  strcpy(obj,"相机");
					  switch(can_rcv_buf[55+c*57]){//事件类型
					  case 0x70:{
									if(can_rcv_buf[58+c*57] == 1){
										strcpy(event_type,"交流电正常");
										fwrite("2\n",2,1,fp);//电源故障排除把设备状态由故障改为离线
										fwrite(fault_count,strlen(fault_count),1,fp);//故障次数没变，原来的值写进去
										fwrite(fault_type,strlen(fault_type),1,fp);//故障类型为上一次故障类型
									}
									else if(can_rcv_buf[58+c*57] == 2){
										strcpy(event_type,"交流电故障");
										fwrite("3\n",2,1,fp);//电源故障
										int fault_count_int = atoi(fault_count);
										fault_count_int++;
										bzero(fault_count,5);
										sprintf(fault_count,"%d\n",fault_count_int);
										fwrite(fault_count,strlen(fault_count),1,fp);//故障次数写进文件
										
										fwrite("7002\n",5,1,fp);//故障类型 交流电故障

										if(atoi(dev_status) == 1){//之前设备状态正常，现在发生故障 建立一个文件记录故障
											bzero(filename,100);
											sprintf(filename,"fault/fault_%d",id_int);
											FILE *fault_fp = fopen(filename,"a");
											if(fault_fp == NULL){
												perror("record fault time error");
												return;
											}
											sprintf(fault_occur_time,"%d\n",fault_time);//把断电事件时间写进故障文本里
											fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
											fclose(fault_fp);
										}
									}
									break;
								}
					  case 0xA0:{
									if(can_rcv_buf[58+c*57] == 1){
										strcpy(event_type,"网络在线");
										fwrite("1\n",2,1,fp);//设备在线
										fwrite(fault_count,strlen(fault_count),1,fp);//故障次数没变，原来的值写进去
										fwrite(fault_type,strlen(fault_type),1,fp);//故障类型为上一次故障类型
										if(atoi(dev_status) == 2){//之前设备状态离线，现在在线,把恢复修好时间写进文件里
											bzero(filename,100);
											sprintf(filename,"fault/fault_%d",id_int);//打开相对应文件
											FILE * fault_fp = fopen(filename,"a");
											if(fault_fp == NULL){
												perror("record fault time error");
												return;
											}
											sprintf(fault_occur_time,"%d\n",fault_time);//把在线事件时间写进故障文本里
											fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
											fclose(fault_fp);
										}

									}
									else if(can_rcv_buf[58+c*57] == 2){
										strcpy(event_type,"网络离线");
										fwrite("2\n",2,1,fp);//网络离线
										int fault_count_int = atoi(fault_count);
										fault_count_int++;
										bzero(fault_count,5);
										sprintf(fault_count,"%d\n",fault_count_int);
										fwrite(fault_count,strlen(fault_count),1,fp);//故障次数写进文件
										
										fwrite("A002\n",5,1,fp);//故障类型 网络离线

										if(atoi(dev_status) == 1){//之前设备状态正常，现在发生故障 建立一个文件记录故障
											bzero(filename,100);
											sprintf(filename,"fault/fault_%d",id_int);
											FILE *fault_fp = fopen(filename,"a");
											if(fault_fp == NULL){
												perror("record fault time error");
												return;
											}
											sprintf(fault_occur_time,"%d\n",fault_time);//把断电事件时间写进故障文本里
											fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
											fclose(fault_fp);
										}

									}
									break;
								}
					  }
					  break;
				  }

		case 0x31:{
					  strcpy(obj,"终端服务器");
					  switch(can_rcv_buf[55+c*57]){//事件类型
					  case 0x72:{
									if(can_rcv_buf[58+c*57] == 1){
										strcpy(event_type,"直流12V正常");
										fwrite("1\n",2,1,fp);//电源故障排除把设备状态由故障改为离线
										fwrite(fault_count,strlen(fault_count),1,fp);//故障次数没变，原来的值写进去
										fwrite(fault_type,strlen(fault_type),1,fp);//故障类型为上一次故障类型
										if(atoi(dev_status) == 3){//之前设备断电，现在电源恢复,把恢复修好时间写进文件里
											bzero(filename,100);
											sprintf(filename,"fault/fault_%d",id_int);//打开相对应文件
											FILE * fault_fp = fopen(filename,"a");
											if(fault_fp == NULL){
												perror("record fault time error");
												return;
											}
											sprintf(fault_occur_time,"%d\n",fault_time);//把在线事件时间写进故障文本里
											fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
											fclose(fault_fp);
										}

									}
									else if(can_rcv_buf[58+c*57] == 2){
										strcpy(event_type,"直流12V故障");
										fwrite("3\n",2,1,fp);//电源故障
										int fault_count_int = atoi(fault_count);
										fault_count_int++;
										bzero(fault_count,5);
										sprintf(fault_count,"%d\n",fault_count_int);
										fwrite(fault_count,strlen(fault_count),1,fp);//故障次数写进文件
										
										fwrite("7202\n",5,1,fp);//故障类型 直流电故障

										if(atoi(dev_status) == 1){//之前设备状态正常，现在发生故障 建立一个文件记录故障
											bzero(filename,100);
											sprintf(filename,"fault/fault_%d",id_int);
											FILE * fault_fp = fopen(filename,"a");
											if(fault_fp == NULL){
												perror("record fault time error");
												return;
											}
											sprintf(fault_occur_time,"%d\n",fault_time);//把断电事件时间写进故障文本里
											fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
											fclose(fault_fp);
										}

									}
									break;
								}
					  case 0xA0:{
									if(can_rcv_buf[58+c*57] == 1)
										strcpy(event_type,"网络在线");
									else if(can_rcv_buf[58+c*57] == 2)
										strcpy(event_type,"网络离线");
									break;
								}
					  }
					  break;
				  }

		case 0x32:{
					  strcpy(obj,"补光灯");
					  switch(can_rcv_buf[55+c*57]){//事件类型
					  case 0x70:{
									if(can_rcv_buf[58+c*57] == 1){
										strcpy(event_type,"交流电正常");
										fwrite("1\n",2,1,fp);//电源恢复 状态为正常
										fwrite(fault_count,strlen(fault_count),1,fp);//故障次数没变，原来的值写进去
										fwrite(fault_type,strlen(fault_type),1,fp);//故障类型为上一次故障类型
										if(atoi(dev_status) == 3){//之前设备状态故障，现在电源恢复,把恢复修好时间写进文件里
											bzero(filename,100);
											sprintf(filename,"fault/fault_%d",id_int);//打开相对应文件
											FILE * fault_fp = fopen(filename,"a");
											if(fault_fp == NULL){
												perror("record fault time error");
												return;
											}
											sprintf(fault_occur_time,"%d\n",fault_time);//把来电事件时间写进故障文本里
											fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
											fclose(fault_fp);
										}

									}
									else if(can_rcv_buf[58+c*57] == 2){
										strcpy(event_type,"交流电故障");
										fwrite("3\n",2,1,fp);//电源故障
										int fault_count_int = atoi(fault_count);
										fault_count_int++;
										bzero(fault_count,5);
										sprintf(fault_count,"%d\n",fault_count_int);
										fwrite(fault_count,strlen(fault_count),1,fp);//故障次数写进文件

										fwrite("7002\n",5,1,fp);//故障类型 交流电故障

										if(atoi(dev_status) == 1){//之前设备状态正常，现在发生故障 建立一个文件记录故障
											bzero(filename,100);
											sprintf(filename,"fault/fault_%d",id_int);
											FILE * fault_fp = fopen(filename,"a");
											if(fault_fp == NULL){
												perror("record fault time error");
												return;
											}
											sprintf(fault_occur_time,"%d\n",fault_time);//把断电事件时间写进故障文本里
											fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
											fclose(fault_fp);
										}

									}
									break;
								}
					  case 0x71:{
									if(can_rcv_buf[58+c*57] == 1){
										strcpy(event_type,"状态检测正常");
										fwrite("1\n",2,1,fp);//电源恢复 状态为正常
										fwrite(fault_count,strlen(fault_count),1,fp);//故障次数没变，原来的值写进去
										fwrite(fault_type,strlen(fault_type),1,fp);//故障类型为上一次故障类型
					
									}
									else if(can_rcv_buf[58+c*57] == 2){
										strcpy(event_type,"状态检测异常");
										fwrite("4\n",2,1,fp);//状态检测异常
										//状态检测异常,但不知道哪个补光灯有异常，有一个补光灯异常，该方向补光灯都报这个异常
										fwrite(fault_count,strlen(fault_count),1,fp);//故障不加1了
										fwrite("7102\n",5,1,fp);//故障类型 状态检测异常
									}
									break;
								}
					  }
					  break;
				  }
		case 0x39:{
				  	strcpy(obj,"能见度");
					switch(can_rcv_buf[55+c*57]) 
					{
					case 1: {
								strcpy(event_type,"设备在线");
								fwrite("1\n",2,1,fp);//设备在线
								fwrite(fault_count,strlen(fault_count),1,fp);//故障次数没变，原来的值写进去
								fwrite(fault_type,strlen(fault_type),1,fp);//故障类型为上一次故障类型
								if(atoi(dev_status) == 2){//之前设备状态离线，现在在线,把恢复修好时间写进文件里
									bzero(filename,100);
									sprintf(filename,"fault/fault_%d",id_int);//打开相对应文件
									FILE * fault_fp = fopen(filename,"a");
									if(fault_fp == NULL){
										perror("record fault time error");
										return;
									}
									sprintf(fault_occur_time,"%d\n",fault_time);//把在线事件时间写进故障文本里
									fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
									fclose(fault_fp);
								}
								break;
							}
					case 2: {
								strcpy(event_type,"设备离线");
								fwrite("2\n",2,1,fp);//设备离线
								int fault_count_int = atoi(fault_count);
								fault_count_int++;
								bzero(fault_count,5);
								sprintf(fault_count,"%d\n",fault_count_int);
								fwrite(fault_count,strlen(fault_count),1,fp);//故障次数写进文件

								fwrite("0200\n",5,1,fp);//故障类型 离线

								if(atoi(dev_status) == 1){//之前设备状态正常，现在发生故障 建立一个文件记录故障
									bzero(filename,100);
									sprintf(filename,"fault/fault_%d",id_int);
									FILE * fault_fp = fopen(filename,"a");
									if(fault_fp == NULL){
										perror("record fault time error");
										return;
									}
									sprintf(fault_occur_time,"%d\n",fault_time);//把断电事件时间写进故障文本里
									fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
									fclose(fault_fp);
								}

								break;
							}
					}
					break;
				  }

		case 0x3A:{
					  strcpy(obj,"路感");
					  switch(can_rcv_buf[55+c*57]) 
					  {
					  case 1: {
								  strcpy(event_type,"设备在线");
								  fwrite("1\n",2,1,fp);//设备在线
								  fwrite(fault_count,strlen(fault_count),1,fp);//故障次数没变，原来的值写进去
								  fwrite(fault_type,strlen(fault_type),1,fp);//故障类型为上一次故障类型
								  if(atoi(dev_status) == 2){//之前设备状态离线，现在在线,把恢复修好时间写进文件里
									  bzero(filename,100);
									  sprintf(filename,"fault/fault_%d",id_int);//打开相对应文件
									  FILE * fault_fp = fopen(filename,"a");
									  if(fault_fp == NULL){
										  perror("record fault time error");
										  return;
									  }
									  sprintf(fault_occur_time,"%d\n",fault_time);//把在线事件时间写进故障文本里
									  fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
									  fclose(fault_fp);
								  }
									  break;
							  }
					  case 2: {	
								  strcpy(event_type,"设备离线");
								  fwrite("2\n",2,1,fp);//设备离线
								  int fault_count_int = atoi(fault_count);
								  fault_count_int++;
								  bzero(fault_count,5);
								  sprintf(fault_count,"%d\n",fault_count_int);
								  fwrite(fault_count,strlen(fault_count),1,fp);//故障次数写进文件

								  fwrite("0200\n",5,1,fp);//故障类型 离线

								  if(atoi(dev_status) == 1){//之前设备状态正常，现在发生故障 建立一个文件记录故障
									  bzero(filename,100);
									  sprintf(filename,"fault/fault_%d",id_int);
									  FILE * fault_fp = fopen(filename,"a");
									  if(fault_fp == NULL){
										  perror("record fault time error");
										  return;
									  }
									  sprintf(fault_occur_time,"%d\n",fault_time);//把断电事件时间写进故障文本里
									  fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
									  fclose(fault_fp);
								  }
								  break;
							  }
					  }
					  break;
				  }

		case 0x3B:{
					  strcpy(obj,"气象站");
					  switch(can_rcv_buf[55+c*57]) 
					  {
					  case 1: {
								  strcpy(event_type,"设备在线");
								  fwrite("1\n",2,1,fp);//设备在线
								  fwrite(fault_count,strlen(fault_count),1,fp);//故障次数没变，原来的值写进去
								  fwrite(fault_type,strlen(fault_type),1,fp);//故障类型为上一次故障类型
								  if(atoi(dev_status) == 2){//之前设备状态离线，现在在线,把恢复修好时间写进文件里
									  bzero(filename,100);
									  sprintf(filename,"fault/fault_%d",id_int);//打开相对应文件
									  FILE * fault_fp = fopen(filename,"a");
									  if(fault_fp == NULL){
										  perror("record fault time error");
										  return;
									  }
									  sprintf(fault_occur_time,"%d\n",fault_time);//把在线事件时间写进故障文本里
									  fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
									  fclose(fault_fp);
								  }
								  break;
							  }
					  case 2: {
								  strcpy(event_type,"设备离线");
								  fwrite("2\n",2,1,fp);//设备离线
								  int fault_count_int = atoi(fault_count);
								  fault_count_int++;
								  bzero(fault_count,5);
								  sprintf(fault_count,"%d\n",fault_count_int);
								  fwrite(fault_count,strlen(fault_count),1,fp);//故障次数写进文件

								  fwrite("0200\n",5,1,fp);//故障类型 离线

								  if(atoi(dev_status) == 1){//之前设备状态正常，现在发生故障 建立一个文件记录故障
									  bzero(filename,100);
									  sprintf(filename,"fault/fault_%d",id_int);
									  FILE * fault_fp = fopen(filename,"a");
									  if(fault_fp == NULL){
										  perror("record fault time error");
										  return;
									  }
									  sprintf(fault_occur_time,"%d\n",fault_time);//把断电事件时间写进故障文本里
									  fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
									  fclose(fault_fp);
								  }
								  break;
							  }
					  }
					  break;
				  }

		case 0x3C:{
					  strcpy(obj,"机柜");
					  switch(can_rcv_buf[55+c*57]){//事件类型
					  case 0x01:{
									if(can_rcv_buf[58+c*57] == 1)
										strcpy(event_type,"前门锁开");
									else if(can_rcv_buf[58+c*57] == 2)
										strcpy(event_type,"前门锁关");
									break;
								}
					  case 0x02:{
									if(can_rcv_buf[58+c*57] == 1)
										strcpy(event_type,"后门锁开");
									else if(can_rcv_buf[58+c*57] == 2)
										strcpy(event_type,"后门锁关");
									break;
								}

					  case 0x03:{
									if(can_rcv_buf[58+c*57] == 1)
										strcpy(event_type,"风扇开");
									else if(can_rcv_buf[58+c*57] == 2)
										strcpy(event_type,"风扇关");
									break;
								}

					  case 0x04:{
									if(can_rcv_buf[58+c*57] == 1)
										strcpy(event_type,"加热器开");
									else if(can_rcv_buf[58+c*57] == 2)
										strcpy(event_type,"加热器关");
									break;
								}

					  case 0x35:{
									if(can_rcv_buf[58+c*57] == 1){
										strcpy(event_type,"没进水");
										fwrite("1\n",2,1,fp);//设备在线
										fwrite(fault_count,strlen(fault_count),1,fp);//故障次数没变，原来的值写进去
										fwrite(fault_type,strlen(fault_type),1,fp);//故障类型为上一次故障类型
										bzero(filename,100);
										sprintf(filename,"fault/fault_%d",id_int);//打开相对应文件
										FILE * fault_fp = fopen(filename,"a");
										if(fault_fp == NULL){
											perror("record fault time error");
											return;
										}
										sprintf(fault_occur_time,"%d\n",fault_time);//把在线事件时间写进故障文本里
										fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
										fclose(fault_fp);

									}
									else if(can_rcv_buf[58+c*57] == 2){
										strcpy(event_type,"设备进水");
										fwrite("1\n",2,1,fp);//机柜总在线
										int fault_count_int = atoi(fault_count);
										fault_count_int++;
										bzero(fault_count,5);
										sprintf(fault_count,"%d\n",fault_count_int);
										fwrite(fault_count,strlen(fault_count),1,fp);//故障次数写进文件

										fwrite("3502\n",5,1,fp);//故障类型 进水

										bzero(filename,100);
										sprintf(filename,"fault/fault_%d",id_int);
										FILE * fault_fp = fopen(filename,"a");
										if(fault_fp == NULL){
											perror("record fault time error");
											return;
										}
										sprintf(fault_occur_time,"%d\n",fault_time);//把断电事件时间写进故障文本里
										fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
										fclose(fault_fp);

									}
									break;
								}

					  case 0x33:{
									fwrite("1\n",2,1,fp);//机柜总在线
									int fault_count_int = atoi(fault_count);
									fault_count_int++;
									bzero(fault_count,5);
									sprintf(fault_count,"%d\n",fault_count_int);
									fwrite(fault_count,strlen(fault_count),1,fp);//故障次数写进文件

									fwrite("3300\n",5,1,fp);//故障类型 撞击

									bzero(filename,100);
									sprintf(filename,"fault/fault_%d",id_int);
									FILE * fault_fp = fopen(filename,"a");
									if(fault_fp == NULL){
										perror("record fault time error");
										return;
									}
									sprintf(fault_occur_time,"%d\n",fault_time);//把断电事件时间写进故障文本里
									fwrite(fault_occur_time,strlen(fault_occur_time),1,fault_fp);
									fclose(fault_fp);

									if(can_rcv_buf[58+c*57] == 1)
										strcpy(event_type,"严重撞击1级");
									else if(can_rcv_buf[58+c*57] == 2)
										strcpy(event_type,"严重撞击2级");
									else if(can_rcv_buf[58+c*57] == 3)
										strcpy(event_type,"严重撞击3级");
									else if(can_rcv_buf[58+c*57] == 4)
										strcpy(event_type,"严重撞击4级");
									else if(can_rcv_buf[58+c*57] == 5)
										strcpy(event_type,"严重撞击5级");
									break;
								}
					  }
					  break;
				  }
		}
		fclose(fp);//不能写在switch语句里 case有满足的 就直接跳出去了，没有关闭fp

		char buf[1024] = {0};
		char *err_msg = NULL;
		sprintf(buf,"insert into event_table values('%d','%s','%s','%s','%s','%s');",id_int,event_no,date,obj,event_type,direct);
		int res = sqlite3_exec(db,buf,0,0,&err_msg);
		if (res != SQLITE_OK)
		{
			printf ("Error:%s",err_msg);
			sqlite3_close (db);
			exit(1);
		}

		printf ("insert event_table values OK!\n");
	}
	sem_wait(&sem);//datato_server_pthread 取完数据才能清空数据buf	
	//（解析完数据清空接收缓存）
	bzero(can_rcv_buf,LENGTH);// 清空buf
	count = 0;
	datalen = 0;
}



void *pthread_can_rcv(void *arg)
{
	struct can_frame frame = {0}; 
	signal(SIGALRM, can_rcv_timeout);
	map_p = mem_map();//得到共享内存地址
	int eventno = 0;
	while(1){
		bzero(&frame,sizeof(frame));
		read(can_fd, &frame, sizeof(frame));//等待can数据
		sem_init(&ping_data_sem, 0, 0);//占用can_rcv_buf 要用的阻塞等待
		can_rcv_flag = true;
		alarm(1); //来数据启动定时器
		memcpy(can_rcv_buf+count,frame.data,frame.can_dlc);//存储接收到的数据
		count+=frame.can_dlc;//已从can接收到了多少个字节
		if(count > 8)
			datalen = can_rcv_buf[8]*256+can_rcv_buf[7];//这一包数据会发过来这么多数据

		if((count-2) == datalen){//一包数据接收完成
			alarm(0);//一包数据接收完整 关闭定时器
			can_rcv_flag = false;
			func_crc(can_rcv_buf,datalen,&crc_low,&crc_high);//接收数据进行crc校验
			if((crc_low == can_rcv_buf[datalen]) && (crc_high == can_rcv_buf[datalen+1])){
				printf("can rcv data ok\n");
								//下面解析数据
				if(can_rcv_buf[10] == 0x70){//实时事件数据 存入数据库
					//添加事件序号  时间
					pthread_mutex_lock(&mutex_eventno);//对eventno加锁
					eventno = read_eventno();//得到了已经到第几个事件序号
					uint8_t rcv_eventnum = can_rcv_buf[37];//这包数据有多少事件
					int i;
					for(i = 0;i < rcv_eventnum;i++){// 填充can buf 把事件序号 时间写道相应字节
						eventno++;
						fill_can_rcv_buf(eventno,i);
					}
					func_crc(can_rcv_buf,datalen,can_rcv_buf+datalen,can_rcv_buf+datalen+1);//改变了数据，校验相应改变

					//把eventno写回 eventno.txt里
					write_eventno(eventno);
					pthread_mutex_unlock(&mutex_eventno);//对eventno操作完解锁
					//数据填写完成，给datato_server_pthread线程发消息，已接收到一包实时数据，把这实时数据发给服务器
					pthread_cond_signal(&cond_datato_server);
					//把实时事件存到数据库里
					store_event_intodatabase();//并清空Buf
					right_response_event();//发送接收正确应答

				}
				else if((can_rcv_buf[10] == 0x72) && (can_rcv_buf[9] == 0x81)){//接收到了展示数据
					memcpy(map_p,can_rcv_buf,73);//展示数据中的气象数据放到共享内存里
					bzero(can_rcv_buf,LENGTH);// 清空buf
					count = 0;
					datalen = 0;
				}

			}
			else
				wrong_response();
			sem_init(&ping_data_sem, 0, 1);//一包数据接收完毕，存入数据库，别的线程可以用can_rcv_buf
		}
	}//while
	
}
