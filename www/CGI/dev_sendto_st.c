#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <net/if.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <linux/can.h> 
#include <linux/can/raw.h>
#include <dirent.h>
#include "cgic.h"
#define DEV_   "dev_info/dev_"
#define DEV_INFO_DIR   "dev_info"


extern void func_crc(const uint8_t *buf,uint32_t len ,uint8_t *out_low, uint8_t *out_high);
int can_fd;

uint8_t can_send[820]={
0xFE,0xAA,0x02,0x01,0x00,0x02,0x00,0xFE,0x00,0xC9,0x04,0x00,0x00
};

void can_send_data(uint8_t *buf)
{
	struct can_frame frame = {0}; 
	int len = buf[8]*256+buf[7]+2;//buf里这么多字节要发出去
	int sended_count = 0;
	frame.can_id = 0x10;
	int i;
	while(1) { 
		if(len >= 8){
			frame.can_dlc = 8;//这个参数是几 can_rcv接收几个字接
			memcpy(frame.data,buf+sended_count,8);
			/*for(i=0;i<8;i++)
				printf("%02x ",frame.data[i]);*/
			write(can_fd,&frame,sizeof(frame));
			sended_count += 8;
			len -= 8;
			if(len == 0)
				break;
		}
		if(len < 8){
			frame.can_dlc = len;
			memcpy(frame.data,buf+sended_count,len);
			/*for(i=0;i<len;i++)
				printf("%02x ",frame.data[i]);
			puts("");*/
			write(can_fd,&frame,sizeof(frame));
			close(can_fd);
			break;
		}

	}  
}

void can_init()
{
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
	return ;
}

int cgiMain()
{
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>send dev to stm</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	int dev_num = 0;
	DIR *dirp;
	struct dirent *dir_ent;

	//这个目录存储所接设备信息
	if((dirp = opendir(DEV_INFO_DIR)) == NULL)
		perror("fail to opendir");
	while((dir_ent = readdir(dirp)) != NULL)
	{
		if((!strcmp(dir_ent->d_name,"."))||(!strcmp(dir_ent->d_name,"..")))
			continue;	
		if(strncmp(dir_ent->d_name,"dev_",4) == 0)//创建一个id 设备就会生成一个dev_id文件
			dev_num++;
	}
	closedir(dirp);
	can_send[13] = dev_num;//设备数
	uint16_t data_len = 40*dev_num + 14;//字节长度 
	can_send[8] = data_len / 256;
	can_send[7] = data_len % 256;
	FILE *fp = NULL;
	char *line;
	size_t len;
	char filename[100] = {0};
	int i;
	for(i=0;i<dev_num;i++){
		bzero(filename,100);
		sprintf(filename,"%s%d",DEV_,i+1);
		fp = fopen(filename,"r");
		if(fp == NULL){
			printf("open dev_ error\n");
			break;
		}
		getline(&line,&len,fp);//得到id
		int num = atoi(line);
		char id[9] = {0};
		sprintf(id,"%08d",num);
		memcpy(can_send+15+i*40,id,8);//填充can_send数组中的id

		getline(&line,&len,fp);//得到设备类型
		if(!strcmp(line,"相机\n"))
			can_send[14+i*40] = 0x30;
		else if(!strcmp(line,"终端服务器\n"))
			can_send[14+i*40] = 0x31;
		else if(!strcmp(line,"补光灯\n"))
			can_send[14+i*40] = 0x32;
		else if(!strcmp(line,"能见度\n"))
			can_send[14+i*40] = 0x39;
		else if(!strcmp(line,"路感\n"))
			can_send[14+i*40] = 0x3A;
		else if(!strcmp(line,"气象站\n"))
			can_send[14+i*40] = 0x3B;
		else if(!strcmp(line,"机柜\n"))
			can_send[14+i*40] = 0x3C;
		
		getline(&line,&len,fp);//得到设备方向
		if(!strcmp(line,"东\n"))
			can_send[44+i*40] = 1;
		else if(!strcmp(line,"南\n"))
			can_send[44+i*40] = 2;
		else if(!strcmp(line,"西\n"))
			can_send[44+i*40] = 3;
		else if(!strcmp(line,"北\n"))
			can_send[44+i*40] = 4;
		else if(!strcmp(line,"无\n"))
			can_send[44+i*40] = 0;

		getline(&line,&len,fp);//得到设备ip
		if(!strcmp(line,"\n")){//设备没有ip
			can_send[23+i*40] = 0;
			can_send[24+i*40] = 0;
			can_send[25+i*40] = 0;
			can_send[26+i*40] = 0;
		}
		else { // 设备有ip
			char ip[20] = {0};
			strncpy(ip,line,strlen(line)-1);//去掉\n
			int n = strlen(ip);
			int y;
			for(y=0;y<n;y++)
				if(ip[y] == 46)//"."
					ip[y] = 32; //" "
			char ip1[5] = {0};
			char ip2[5] = {0};
			char ip3[5] = {0};
			char ip4[5] = {0};
			sscanf(ip,"%s %s %s %s",ip1,ip2,ip3,ip4);
			can_send[23+i*40] = atoi(ip1);
			can_send[24+i*40] = atoi(ip2);
			can_send[25+i*40] = atoi(ip3);
			can_send[26+i*40] = atoi(ip4);
		}

		fclose(fp);
	}
	func_crc(can_send,data_len,can_send+data_len,can_send+data_len+1);
	can_init();
	can_send_data(can_send);
	fprintf(cgiOut,"存储设备信息成功！");
	fprintf(cgiOut,"<br><a href=\"../dev_set.html\"><b>返回</b></a>");
	fprintf(cgiOut,"</BODY></HTML>");
	return 0;
}
