#include "data_global.h"
//向stm32 询问展示数据 并且根据设备事件总结出设备展示数据放到
//共享内存相应地方
uint8_t query_showdata[64] = {
	0xFE,0xAA,0x02,0x01,0x00,0x02,0x00,0x3E,0x00,0x80,0x72,0x00,0x00,0x00,
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,
	0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
	0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,
	0x43,0x44,0x45,0x46,0x47,0x48,0xC4,0xE3
};

void send_query_showdata()
{
	struct can_frame frame = {0};
	int len = query_showdata[8]*256+query_showdata[7]+2;//这么多字节要发出去
	int sended_count = 0;
	frame.can_id = 0x10;
	while(1) { 
		if(len >= 8){
			frame.can_dlc = 8;//这个参数是几 can_rcv接收几个字接
			memcpy(frame.data,query_showdata+sended_count,8);
			write(can_fd,&frame,sizeof(frame));
			sended_count += 8;
			len -= 8;
			if(len == 0)
				break;
		}
		if(len < 8){
			frame.can_dlc = len;
			memcpy(frame.data,query_showdata+sended_count,len);
			write(can_fd,&frame,sizeof(frame));
			break;
		}

	}  
}

void add_mem_show_device()
{
	map_p[13] = dev_num;//设备个数
	int datalen = dev_num * 36 + 73;
	map_p[7] = datalen % 256;
	map_p[8] = datalen / 256;
	int i;
	char dev_status[5] = {0};
	size_t len;
	char *line;
	FILE *fp;
	char filename[100] = {0};
	for(i = 0;i < dev_num;i++){
		char id[9] = {0};
		bzero(filename,100);
		sprintf(filename,"%s%d",DEV_,i+1);//存储设备信息的文件
		fp = fopen(filename,"r");
		if(fp == NULL){
			perror("query_showdata open dev_ error");
			break;
		}
		getline(&line,&len,fp);//得到id
		int id_int = atoi(line);
		sprintf(id,"%08d",id_int);
		strncpy((char*)map_p+74+i*36,id,8);//id 放到共享内存中

		getline(&line,&len,fp);//得到设备类型
		if(strcmp(line,"相机\n") == 0)//根据设备类型添加到共享内存里
			map_p[73+i*36] = 0x30;
		else if(!strcmp(line,"终端服务器\n"))
			map_p[73+i*36] = 0x31;
		else if(!strcmp(line,"补光灯\n"))
			map_p[73+i*36] = 0x32;
		else if(!strcmp(line,"能见度\n"))
			map_p[73+i*36] = 0x39;
		else if(!strcmp(line,"路感\n"))
			map_p[73+i*36] = 0x3A;
		else if(!strcmp(line,"气象站\n"))
			map_p[73+i*36] = 0x3B;
		else if(!strcmp(line,"机柜\n"))
			map_p[73+i*36] = 0x3C;
		
		getline(&line,&len,fp);//得到方向
		if(!strcmp(line,"东\n"))
			map_p[99+i*36] = 0x01;
		else if(!strcmp(line,"南\n"))
			map_p[99+i*36] = 0x02;
		else if(!strcmp(line,"西\n"))
			map_p[99+i*36] = 0x03;
		else if(!strcmp(line,"北\n"))
			map_p[99+i*36] = 0x04;
		else if(!strcmp(line,"无\n"))
			map_p[99+i*36] = 0x00;

		getline(&line,&len,fp);//得到ip
		getline(&line,&len,fp);//得到设备开始时间

		time_t t;
		time(&t);//得到现在时间
		int start_time = atoi(line);
	//	printf("start_time = %d\n",start_time);
	//	printf("t = %ld\n",t);
		int runtime = (int)t - atoi(line);
	//	printf("runtime = %d\n",runtime);
		map_p[98+i*36] = runtime / 16777216; //运行时间添到map里
		map_p[97+i*36] = runtime % 16777216 / 65536;
		map_p[96+i*36] = runtime % 65536 / 256 ;
		map_p[95+i*36] = runtime % 256;
		fclose(fp);// dev_%d 信息读取完了

		bzero(filename,100);
		sprintf(filename,"%s%d",DEVSTATUS_,i+1);//存储设备状态文本
		fp = fopen(filename,"r");
		if(fp == NULL){
			perror("open devstatus_");

		}
		getline(&line,&len,fp);//获得设备状态
		strcpy(dev_status,line);
		if(!strcmp(line,"1\n"))
			map_p[82+i*36] = 0x01;
		else if(!strcmp(line,"2\n"))
			map_p[82+i*36] = 0x02;
		else if(!strcmp(line,"3\n"))
			map_p[82+i*36] = 0x03;
		else if(!strcmp(line,"4\n"))
			map_p[82+i*36] = 0x04;

		getline(&line,&len,fp);//故障次数
		int fault_count_int = atoi(line);
		map_p[84+i*36] = fault_count_int / 256;
		map_p[83+i*36] = fault_count_int % 256;

		getline(&line,&len,fp);//故障类型
		if(!strcmp(line,"0\n")){
			map_p[85+i*36] = 0;
			map_p[86+i*36] = 0;
		}
		else if(!strcmp(line,"7002\n")){
			map_p[85+i*36] = 0x70;
			map_p[86+i*36] = 0x02;
		}
		else if(!strcmp(line,"A002\n")){
			map_p[85+i*36] = 0xA0;
			map_p[86+i*36] = 0x02;
		}
		else if(!strcmp(line,"7102\n")){
			map_p[85+i*36] = 0x71;
			map_p[86+i*36] = 0x02;
		}
		else if(!strcmp(line,"0200\n")){
			map_p[85+i*36] = 0x02;
			map_p[86+i*36] = 0x00;
		}
		else if(!strcmp(line,"3502")){
			map_p[85+i*36] = 0x35;
			map_p[86+i*36] = 0x02;
		}
		else if(!strcmp(line,"3300")){
			map_p[85+i*36] = 0x33;
			map_p[86+i*36] = 0x00;
		}
		fclose(fp);

		bzero(filename,100);
		sprintf(filename,"fault/fault_%d",id_int);//应用程序目录下
		int y;

		fp = fopen(filename,"r");
		if(fp == NULL){
		//	printf("%d device no fault\n",id_int);
			for(y=0;y<8;y++)
				map_p[87+i*36+y] = 0;//没有故障没有生成故障文件 故障时间添0	
		}
		else{//有故障文件
			int count = 0;
			uint32_t dev_time[100] = {0};
			while(getline(&line,&len,fp) != -1){
				char s[20] = {0};
				strncpy(s,line,strlen(line)-1);
				dev_time[count] = strtoul(s,NULL,10);	
				count++;
			}
			fclose(fp);
		//	printf("dev_time[0] = %d\n",dev_time[0]);
		//	printf("dev_time[1] = %d\n",dev_time[1]);


			uint32_t fault_time_total = 0;
			if(!strcmp(dev_status,"1\n")){//设备现在是好的 count 是个数 count-1下标是最近一次修好的时间，count-2是最近一次坏的时间
				map_p[90+i*36] = (dev_time[count-2]+8*60*60) / 16777216; //最新故障时间
				map_p[89+i*36] = (dev_time[count-2]+8*60*60) % 16777216 / 65536;
				map_p[88+i*36] = (dev_time[count-2]+8*60*60) % 65536 / 256 ;
				map_p[87+i*36] = (dev_time[count-2]+8*60*60) % 256;
				int c = count / 2;//现在是好的，是2的倍数
				for(y=0;y<c;y++){
					fault_time_total += (dev_time[1+y*2] - dev_time[y*2]);
			//		printf("fault_time_total = %d\n",fault_time_total);
				}
				map_p[94+i*36] = fault_time_total / 16777216; //故障时间总长度
				map_p[93+i*36] = fault_time_total % 16777216 / 65536;
				map_p[92+i*36] = fault_time_total % 65536 / 256 ;
				map_p[91+i*36] = fault_time_total % 256;
			}
			else{  //设备现在是坏的 count-1 最后一个是坏的时间
				map_p[90+i*36] = (dev_time[count-1]+8*60*60) / 16777216; //最新故障时间
				map_p[89+i*36] = (dev_time[count-1]+8*60*60) % 16777216 / 65536;
				map_p[88+i*36] = (dev_time[count-1]+8*60*60) % 65536 / 256 ;
				map_p[87+i*36] = (dev_time[count-1]+8*60*60) % 256;
				int c = count / 2;//现在是坏的，是2的倍数余出1 count 是奇数
				for(y=0;y<c;y++){
					fault_time_total += (dev_time[1+y*2] - dev_time[y*2]);
				}
				fault_time_total += ((int)t - dev_time[count-1]);//现在时间 - 最后一次故障时间
				map_p[94+i*36] = fault_time_total / 16777216; //故障时间总长度
				map_p[93+i*36] = fault_time_total % 16777216 / 65536;
				map_p[92+i*36] = fault_time_total % 65536 / 256 ;
				map_p[91+i*36] = fault_time_total % 256;
			}

				
		}
	}
		return;
}
void *pthread_query_showdata(void *arg)
{
	while(1){	
		sleep(1);
		if(dev_num == 0){//程序起来了，一个设备都没有,不往下运行了
			map_p[13] = 0;//设备个数0
			sleep(5);

			continue;
		}
		send_query_showdata();//can_rcv_pthread 接收到数据 放到共享内存里
		usleep(1000);
		add_mem_show_device();
		sleep(4);//每5s发一次询问展示数据
	}
	return NULL;
}
