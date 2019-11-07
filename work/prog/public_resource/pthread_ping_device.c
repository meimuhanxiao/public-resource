#include "data_global.h"
//ping 相机 如果相机原来是在线的 Ping不通报相机离线事件
//如果相机原来是离线的，现在Ping通了，报上线事件
int dev_num = 0;//设备数量
int cam_num = 0;//相机数量
sem_t ping_data_sem;
extern int read_eventno();
extern void write_eventno(int eventno);
extern void store_event_intodatabase();

void *pthread_ping_device(void *arg)
{
	while(1){
		sem_init(&ping_data_sem, 0, 1);//该线程默认是可以使用can_rcv_buf
		dev_num = 0;
		char filename[100] = {0};
		DIR *dirp;
		struct dirent *dir_ent;
		sprintf(filename,"%s",DEV_INFO_DIR);
		//这个目录存储所接设备信息
		if((dirp = opendir(filename)) == NULL)
		{
			perror("fail to opendir");
			continue;
		}

		while((dir_ent = readdir(dirp)) != NULL)
		{
			if((!strcmp(dir_ent->d_name,"."))||(!strcmp(dir_ent->d_name,"..")))
				continue;	
			if(strncmp(dir_ent->d_name,"dev_",4) == 0)//创建一个id 设备就会生成一个dev_id文件
				dev_num++;
		}
		if(dev_num == 0){//程序起来了，没有设置设备,不往下运行了
			sleep(20);
			continue;
		}
		//printf("dev_num = %d\n",dev_num);
		closedir(dirp);
		int i;
		FILE *fp = NULL;
		char *line;
		size_t len;
		char ip[20] = {0};
		//char dev_type[20] = {0};
		char direct[5] = {0};
		char id[9] = {0};
		char dev_status[5] = {0};
		char fault_count[5] = {0};
		char fault_type[5] = {0};
		uint8_t ping_event[97] = {
			0xFE,0xAA,0x02,0x02,0x00,0x01,0x00,0x5F,0x00,0xC7,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x01,0x00,0x00,0x00,0x01, 
			0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x34,0x0B,0x00,0x00,0x00,0x42,0x0D,0x55,0x5D,0x32,0x0A,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xA0,0x0B,0x0C,0x0D,
			0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x03,0x36,0x4B 
		};
		for(i = 1; i <= dev_num;i++){
			bzero(filename,100);
			sprintf(filename,"%s%d",DEV_,i);
			fp = fopen(filename,"r");
			if(fp == NULL){
				perror("ping device thread open file error");
				break;
			}
			getline(&line,&len,fp);
			strcpy(id,line); //得到id
		//	printf("id = %s",id);
			getline(&line,&len,fp);//得到设备类型
		//	printf("dev_type = %s",line);
			if(strcmp(line,"相机\n") == 0){//设备类型是相机
				getline(&line,&len,fp);
				strcpy(direct,line);//得到方向
				getline(&line,&len,fp);
				strncpy(ip,line,strlen(line)-1);//把\n去掉,得到ip地址
		//		printf("ip = %s\n",ip);
				bzero(filename,100);
				sprintf(filename,"%s%d",DEVSTATUS_,i);
				FILE *fault_fp = fopen(filename,"r");
				if(fault_fp == NULL){
					perror("open devstatus_");
					//return NULL;
					break;
				}
				getline(&line,&len,fault_fp);//获得设备状态
				strcpy(dev_status,line);
				getline(&line,&len,fault_fp);
				strcpy(fault_count,line);//获得故障次数
				//printf("fault_count = %s\n",fault_count);
				getline(&line,&len,fault_fp);
				strcpy(fault_type,line);//故障类型
				//printf("fault_type = %s\n",fault_type);
				fclose(fault_fp);

				if(strcmp(dev_status,"3\n") == 0){}//设备是故障的 没电 不用ping了
				else if(strcmp(dev_status,"1\n") == 0){//如果设备是好的
					bzero(filename,100);
					sprintf(filename,"ping -c 1 -w 1 %s > ping_cam.txt",ip);
					system(filename);
					sprintf(filename,"ping -c 1 -w 1 %s >> ping_cam.txt",ip);
					system(filename);

					system("wc -l ping_cam.txt > ping_cam_result.txt");
					FILE *ping_fp = fopen("ping_cam_result.txt","r");
					if(ping_fp == NULL)
						perror("open ping_cam_result.txt error");
					getline(&line,&len,ping_fp);
					char line_num[10] = {0};
					strncpy(line_num,line,2);
					int line_num_int = atoi(line_num);
					printf("ping device ping line num = %d\n",line_num_int);
					if(line_num_int < 12){// 认为设备不在线 之前是好的现在不在线 报事件
						int id_int = atoi(id);
						char id_str[9] = {0};
						sprintf(id_str,"%08d",id_int);
						strncpy((char*)ping_event+38,id_str,8);//id 放进数据里
						//printf("id = %x\n",ping_event[45]);

						pthread_mutex_lock(&mutex_eventno);//对eventno加锁
						int eventno = read_eventno();//得到了已经到第几个事件序号
						eventno ++;
					//	printf("eventno = %d\n",eventno);
						ping_event[49] = eventno /16777216; //填写事件序号
						ping_event[48] = eventno %16777216 /65536 ;
						ping_event[47] = eventno % 65536 / 256;
						ping_event[46] = eventno %256;

						//把eventno写回 eventno.txt里
						write_eventno(eventno);
						pthread_mutex_unlock(&mutex_eventno);//对eventno操作完解锁

						time_t t;
						time(&t);
						t += 8*60*60;
						ping_event[53] = t /16777216; //填写事件时间
						ping_event[52] = t %16777216 /65536 ;
						ping_event[51] = t % 65536 / 256;
						ping_event[50] = t %256;	

						ping_event[54] = 0x30;//相机
						ping_event[55] = 0xA0;//事件类型以太网
						ping_event[58] = 0x02;//断网
						if(strcmp(direct,"东\n") == 0)
							ping_event[94] = 1;
						else if(!strcmp(direct,"南\n"))
							ping_event[94] = 2;
						else if(!strcmp(direct,"西\n"))
							ping_event[94] = 3;
						else if(!strcmp(direct,"北\n"))
							ping_event[94] = 4;
						func_crc(ping_event,ping_event[7],ping_event+95,ping_event+96);//填充crc校验码
				    	sem_wait(&ping_data_sem);//can 没占用buf 直接运行 如果占用，阻塞等待
						memcpy(can_rcv_buf,ping_event,ping_event[7]+2);//把ping事件放到buf里
						pthread_cond_signal(&cond_datato_server);//通知 datato_server线程有数据了，把数据发给服务器
						//把实时事件存到数据库里
						store_event_intodatabase();//把can_rcv_buf里数据存到数据库并且更新设备状态

						//修改设备状态
						//报事件
						//存事件到数据库
					}
					else if(line_num_int == 12){}//设备本来就是好的，现在Ping也通 不用发送事件
					fclose(ping_fp);
				}

				else if(strcmp(dev_status,"2\n") == 0){//设备不在线
					bzero(filename,100);
					sprintf(filename,"ping -c 1 -w 1 %s > ping_cam.txt",ip);
					system(filename);

					sprintf(filename,"ping -c 1 -w 1 %s >> ping_cam.txt",ip);
					system(filename);

					system("wc -l ping_cam.txt > ping_cam_result.txt");
					FILE *ping_fp = fopen("ping_cam_result.txt","r");
					if(ping_fp == NULL)
						perror("open ping_cam_result.txt error");
					getline(&line,&len,ping_fp);
					char line_num[10] = {0};
					strncpy(line_num,line,2);
					int line_num_int = atoi(line_num);
					printf("ping device ping line num = %d\n",line_num_int);
					if(line_num_int == 12){// 认为设备在线 之前设备是离线的现在在线了 报事件
						int id_int = atoi(id);
						char id_str[9] = {0};
						sprintf(id_str,"%08d",id_int);
						strncpy((char*)ping_event+38,id_str,8);//id 放进数据里

						pthread_mutex_lock(&mutex_eventno);//对eventno加锁
						int eventno = read_eventno();//得到了已经到第几个事件序号
						eventno ++;
						ping_event[49] = eventno /16777216; //填写事件序号
						ping_event[48] = eventno %16777216 /65536 ;
						ping_event[47] = eventno % 65536 / 256;
						ping_event[46] = eventno %256;

						//把eventno写回 eventno.txt里
						write_eventno(eventno);
						pthread_mutex_unlock(&mutex_eventno);//对eventno操作完解锁

						time_t t;
						time(&t);
						t += 8*60*60;
						ping_event[53] = t /16777216; //填写事件时间
						ping_event[52] = t %16777216 /65536 ;
						ping_event[51] = t % 65536 / 256;
						ping_event[50] = t %256;	

						ping_event[54] = 0x30;//相机
						ping_event[55] = 0xA0;//事件类型以太网
						ping_event[58] = 0x01;//在线
						if(strcmp(direct,"东\n") == 0)
							ping_event[94] = 1;
						else if(!strcmp(direct,"南\n"))
							ping_event[94] = 2;
						else if(!strcmp(direct,"西\n"))
							ping_event[94] = 3;
						else if(!strcmp(direct,"北\n"))
							ping_event[94] = 4;
						func_crc(ping_event,ping_event[7],ping_event+95,ping_event+96);//填充crc校验码
						sem_wait(&ping_data_sem);//can 没占用buf 直接运行 如果占用，阻塞等待
						memcpy(can_rcv_buf,ping_event,ping_event[7]+2);//把ping事件放到buf里
						pthread_cond_signal(&cond_datato_server);//通知 datato_server线程有数据了，把数据发给服务器
						//把实时事件存到数据库里
						store_event_intodatabase();//把can_rcv_buf里数据存到数据库并且更新设备状态
					}
					else if(line_num_int < 12){}//之前就是离线 现在还ping不通 不报事件
					fclose(ping_fp);
				}
			
			}//cam
			fclose(fp);
			usleep(1000);
		}
		sleep(10);//10s   ping 一次所有相机
		//printf("ping thread is running\n");
	}
	return NULL;
}


