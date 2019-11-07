#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "cgic.h"
#include <stdint.h>
#include <net/if.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <linux/can.h> 
#include <linux/can/raw.h>
#include <signal.h>

#define RCV_CAN_OK         0x11
#define RCV_CAN_CRC_ERR    0x22
extern void func_crc(const uint8_t *buf,uint32_t len ,uint8_t *out_low, uint8_t *out_high);
int can_fd;
char send_data[1024] = {0};
uint32_t len = 0;//通过can发送数据长度 
int crc_err_no = 0;
void can_init()
{
	struct sockaddr_can addr;  
	struct ifreq ifr;  
	can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);      //创建套接字  
	strcpy(ifr.ifr_name, "vcan0" );  
	ioctl(can_fd, SIOCGIFINDEX, &ifr);                   //指定vcan0设备  
	addr.can_family = AF_CAN;  
	addr.can_ifindex = ifr.ifr_ifindex;  
	bind(can_fd, (struct sockaddr *)&addr, sizeof(addr));//将套接字与vcan0绑定  
}
int can_send()
{
	struct can_frame  frame = {0};
	frame.can_id = 0x032;
	int sended_count = 0;
	while(1) { 
		if(len >= 8){
			frame.can_dlc = 8;//这个参数是几 can_rcv接收几个字接
			memcpy(frame.data,send_data+sended_count,8);
			write(can_fd,&frame,sizeof(frame));
			sended_count += 8;
			len -= 8;
		}
		if(len < 8){
			frame.can_dlc = len;
			memcpy(frame.data,send_data+sended_count,len);
			write(can_fd,&frame,sizeof(frame));
			break;
		}
	}  
	
	return 0;
}
void alrm_handler(int sig)
{
	//超时没有接到can应答 提示报错退出程序
	fprintf(cgiOut,"can communication error\n");
	exit(1);
}

void can_rcv_resp()
{
	struct can_frame frame = {0};
	read(can_fd,&frame,sizeof(frame));
	alarm(0);//接收到了应答，关闭定时器
	uint8_t crc[2] = {0};
	uint8_t data_len = frame.data[2];
	func_crc(frame.data,data_len,crc,crc+1);
	if((crc[0]==frame.data[data_len])&&(crc[1]==frame.data[data_len+1])){
		if(frame.data[3] == RCV_CAN_CRC_ERR){//返回应答表示校验不对
			if(crc_err_no++ >= 3){//连续3次crc校验错误，直接退出程序
				fprintf(cgiOut,"can communication error\n");
				exit(1);
			}
			can_send();//再发一次数据
			alarm(1);
			can_rcv_resp();
		}
		if(frame.data[3] == RCV_CAN_OK)
			fprintf(cgiOut,"rcv response ok!\n");
	}

}
int cgiMain()
{
	signal(SIGALRM, alrm_handler);
	cgiHeaderContentType("text/html");	
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>相机设置界面</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	
	char cam_num[3] = {0};
	char cam_ip[1024] = {0};
	char cam_id[100] = {0};
	char cam_des[1024] = {0};

	cgiFormString("cam_num",cam_num,3);
	cgiFormString("cam_ip",cam_ip,1024);
	cgiFormString("cam_des",cam_des,1024);
	cgiFormString("cam_id",cam_id,100);
	FILE *fp = fopen("cam_info","w");
	if(fp == NULL)
		fprintf(cgiOut,"phonenum set error!<br>");
	fwrite(cam_num,strlen(cam_num),1,fp);
	fwrite("\n",1,1,fp);

	fwrite(cam_ip,strlen(cam_ip),1,fp);
	fwrite("\n",1,1,fp);

	fwrite(cam_id,strlen(cam_id),1,fp);
	fwrite("\n",1,1,fp);

	char *s = strdup(cam_ip);//申请空间复制cam_ip字符串
	char *ss = strdup(cam_id);
	int came_num = atoi(cam_num);
	char *ip[came_num];
	char *id[came_num];
		
	int i = 0;
	for(i = 0;i < came_num;i++){
		ip[i] = strsep(&s,",");//分割ip字符串
		fprintf(cgiOut,"第%d个相机ip = %s<br>",i+1,ip[i]);
	}

	for(i = 0;i < came_num;i++){
		id[i] = strsep(&ss,",");
		fprintf(cgiOut,"第%d个相机id = %s<br>",i+1,id[i]);
	}
	
	free(s);
	free(ss);
	fclose(fp);
	fprintf(cgiOut,"</body><html>");
	fprintf(cgiOut,"<br><a href=\"../cam_set.html\"><b>返回</b></a>");
	can_init();//初始化can设备
	//得到了数据通过can发给stm32
	send_data[0] = 0xfe;//起始标志
	send_data[1] = 0x32;//相机信息
	send_data[2] = 0xee;//先占个位,这条信息有多少个字节
	send_data[3] = 0xee;//两个字节表示数据长度,不算crc校验两个字节
	strcat(send_data,"|");
	strcat(send_data,cam_num);
	strcat(send_data,"|");
	strcat(send_data,cam_ip);
	strcat(send_data,"|");
	strcat(send_data,cam_id);
	strcat(send_data,"|");
	strcat(send_data,cam_des);
	strcat(send_data,"|");
	len = strlen(send_data);
	send_data[2] = len % 256;//数据都确定好了，最后校验
	send_data[3] = len / 256;
	func_crc((uint8_t*)send_data,len ,(uint8_t *)(send_data+len),(uint8_t *)(send_data+len+1));//校验写到数组里
	len += 2;//通过can发出去多少字节 包括两个crc校验字节
	can_send();//网页获得的数据通过can发出去
	alarm(1); //发送完数据开启定时器,如果1秒钟后没收到回应，再重发一次
	can_rcv_resp();//等待接收端回应
	close(can_fd);  
	return 0;
}


