#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <linux/can.h> 
#include <linux/can/raw.h>
#include <linux/if.h>
#include <stdint.h>

extern void func_crc(const uint8_t *buf,uint32_t len ,uint8_t *out_low, uint8_t *out_high);

int can_fd;  
uint8_t can_send[47]={
0xFE,0xAA,0x02,0x01,0x00,0x02,0x00,0x2D,0x00,0xC9,0x01,0x00,0x00,
0x01,0x02,0x03,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,
0x00,0x4B,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x01,0x01,0x01,0x01,
0xAB,0x88
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
		/*	for(i=0;i<len;i++)
				printf("%02x ",frame.data[i]);*/
			puts("");
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
	FILE *fp = NULL;
	fp = fopen("com_set/com_set.txt","w");
	if(fp == NULL)
		perror("com_set.txt");

    cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>com_set</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	char dev[5] = {0};
	char databit[5] = {0};
	char stopbit[5] = {0};
	char paritybit[5] = {0};
	char baudrate[10] = {0};
	fprintf(cgiOut,"<h4>串口设置信息：</h4><br>");
	cgiFormString("device_1",dev,5);
	fprintf(cgiOut,"<h4>串口1配置信息：</h4><p></p>");
	if(!strcmp(dev,"1")){//六要素
		fwrite("1\n",2,1,fp);
		fprintf(cgiOut,"外接设备:六要素&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[13] = 1;
	}
	else if(!strcmp(dev,"2")){//能见度
		fwrite("2\n",2,1,fp);
		fprintf(cgiOut,"外接设备:能见度&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[13] = 2;
	}
	else if(!strcmp(dev,"3")){//路感
		fwrite("3\n",2,1,fp);
		fprintf(cgiOut,"外接设备:路感&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[13] = 3;
	}
	else if(!strcmp(dev,"0")){//无
		fwrite("0\n",2,1,fp);
		fprintf(cgiOut,"外接设备:无&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[13] = 0;
	}

	cgiFormString("databit_1",databit,5);//得到串口1的数据位
	if(!strcmp(databit,"6")){
		fwrite("6\n",2,1,fp);
		fprintf(cgiOut,"数据位:6位&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[37] = 6;
	}
	else if(!strcmp(databit,"7")){
		fwrite("7\n",2,1,fp);
		fprintf(cgiOut,"数据位:7位&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[37] = 7;
	}
	else if(!strcmp(databit,"8")){
		fwrite("8\n",2,1,fp);
		fprintf(cgiOut,"数据位:8位&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[37] = 8;
	}

	cgiFormString("stopbit_1",stopbit,5);//得到串口1的停止位
	if(!strcmp(stopbit,"1")){
		fwrite("1\n",2,1,fp);
		fprintf(cgiOut,"停止位:1位<p></p>");
		can_send[41] = 1;
	}
	else if(!strcmp(stopbit,"1.5")){
		fwrite("1.5\n",4,1,fp);
		fprintf(cgiOut,"停止位:1.5位<p></p>");
		can_send[41] = 3;
	}
	else if(!strcmp(stopbit,"2")){
		fwrite("2\n",2,1,fp);
		fprintf(cgiOut,"停止位:2位<p></p>");
		can_send[41] = 2;
	}

	cgiFormString("paritybit_1",paritybit,5);//得到串口1校验位
	if(!strcmp(paritybit,"0")){
		fwrite("0\n",2,1,fp);
		fprintf(cgiOut,"校验位:无校验&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[33] = 0;
	}
	else if(!strcmp(paritybit,"1")){
		fwrite("1\n",2,1,fp);
		fprintf(cgiOut,"校验位:奇校验&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[33] = 1;
	}
	else if(!strcmp(paritybit,"2")){
		fwrite("2\n",2,1,fp);
		fprintf(cgiOut,"校验位:偶校验&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[33] = 2;
	}
	cgiFormString("baudrate_1",baudrate,10);//得到串口1的波特率
	int baud = atoi(baudrate);
	can_send[20] = baud / 16777216;
	can_send[19] = baud %16777216 /65536;
	can_send[18] = baud % 65536 / 256;
	can_send[17] = baud %256;

	if(!strcmp(baudrate,"9600")){
		fwrite("9600\n",5,1,fp);
		fprintf(cgiOut,"波特率:9600<p></p>");
	}
	else if(!strcmp(baudrate,"19200")){
		fwrite("19200\n",6,1,fp);
		fprintf(cgiOut,"波特率:19200<p></p>");
	}
	else if(!strcmp(baudrate,"38400")){
		fwrite("38400\n",6,1,fp);
		fprintf(cgiOut,"波特率:38400<p></p>");
	}
	else if(!strcmp(baudrate,"57600")){
		fwrite("57600\n",6,1,fp);
		fprintf(cgiOut,"波特率:57600<p></p>");
	}
	else if(!strcmp(baudrate,"115200")){
		fwrite("115200\n",7,1,fp);
		fprintf(cgiOut,"波特率:115200<p></p>");
	}
	fprintf(cgiOut,"<p></p>");
/*	printf("dev = %s ",dev);
	printf("databit=%s ",databit);
	printf("stopbit=%s ",stopbit);
	printf("paritybit=%s ",paritybit);
	printf("baudrate=%s ",baudrate);*/

	fprintf(cgiOut,"<h4>串口2配置信息：</h4><p></p>");
	bzero(dev,5);
	bzero(databit,5);
	bzero(stopbit,5);
	bzero(paritybit,5);
	bzero(baudrate,10);
	cgiFormString("device_2",dev,5);
	if(!strcmp(dev,"1")){//六要素
		fwrite("1\n",2,1,fp);
		fprintf(cgiOut,"外接设备:六要素&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[14] = 1;
	}
	else if(!strcmp(dev,"2")){//能见度
		fwrite("2\n",2,1,fp);
		fprintf(cgiOut,"外接设备:能见度&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[14] = 2;
	}
	else if(!strcmp(dev,"3")){//路感
		fwrite("3\n",2,1,fp);
		fprintf(cgiOut,"外接设备:路感&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[14] = 3;
	}
	else if(!strcmp(dev,"0")){//无
		fwrite("0\n",2,1,fp);
		fprintf(cgiOut,"外接设备:无&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[14] = 0;
	}

	cgiFormString("databit_2",databit,5);//得到串口2的数据位
	if(!strcmp(databit,"6")){
		fwrite("6\n",2,1,fp);
		fprintf(cgiOut,"数据位:6位&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[38] = 6;
	}
	else if(!strcmp(databit,"7")){
		fwrite("7\n",2,1,fp);
		fprintf(cgiOut,"数据位:7位&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[38] = 7;
	}
	else if(!strcmp(databit,"8")){
		fwrite("8\n",2,1,fp);
		fprintf(cgiOut,"数据位:8位&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[38] = 8;
	}

	cgiFormString("stopbit_2",stopbit,5);//得到串口2的停止位
	if(!strcmp(stopbit,"1")){
		fwrite("1\n",2,1,fp);
		fprintf(cgiOut,"停止位:1位<p></p>");
		can_send[42] = 1;
	}
	else if(!strcmp(stopbit,"1.5")){
		fwrite("1.5\n",4,1,fp);
		fprintf(cgiOut,"停止位:1.5位<p></p>");
		can_send[42] = 3;
	}
	else if(!strcmp(stopbit,"2")){
		fwrite("2\n",2,1,fp);
		fprintf(cgiOut,"停止位:2位<p></p>");
		can_send[42] = 2;
	}

	cgiFormString("paritybit_2",paritybit,5);//得到串口2的停止位
	if(!strcmp(paritybit,"0")){
		fwrite("0\n",2,1,fp);
		fprintf(cgiOut,"校验位:无校验&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[34] = 0;
	}
	else if(!strcmp(paritybit,"1")){
		fwrite("1\n",2,1,fp);
		fprintf(cgiOut,"校验位:奇校验&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[34] = 1;
	}
	else if(!strcmp(paritybit,"2")){
		fwrite("2\n",2,1,fp);
		fprintf(cgiOut,"校验位:偶校验&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[34] = 2;
	}
	cgiFormString("baudrate_2",baudrate,10);//得到串口2的波特率
	baud = atoi(baudrate);
	can_send[24] = baud / 16777216;
	can_send[23] = baud %16777216 /65536;
	can_send[22] = baud % 65536 / 256;
	can_send[21] = baud %256;

	if(!strcmp(baudrate,"9600")){
		fwrite("9600\n",5,1,fp);
		fprintf(cgiOut,"波特率:9600<p></p>");
	}
	else if(!strcmp(baudrate,"19200")){
		fwrite("19200\n",6,1,fp);
		fprintf(cgiOut,"波特率:19200<p></p>");
	}
	else if(!strcmp(baudrate,"38400")){
		fwrite("38400\n",6,1,fp);
		fprintf(cgiOut,"波特率:38400<p></p>");
	}
	else if(!strcmp(baudrate,"57600")){
		fwrite("57600\n",6,1,fp);
		fprintf(cgiOut,"波特率:57600<p></p>");
	}
	else if(!strcmp(baudrate,"115200")){
		fwrite("115200\n",7,1,fp);
		fprintf(cgiOut,"波特率:115200<p></p>");
	}
	fprintf(cgiOut,"<p></p>");


	fprintf(cgiOut,"<h4>串口3配置信息：</h4><p></p>");
	bzero(dev,5);
	bzero(databit,5);
	bzero(stopbit,5);
	bzero(paritybit,5);
	bzero(baudrate,10);
	cgiFormString("device_3",dev,5);
	if(!strcmp(dev,"1")){//六要素
		fwrite("1\n",2,1,fp);
		fprintf(cgiOut,"外接设备:六要素&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[15] = 1;
	}
	else if(!strcmp(dev,"2")){//能见度
		fwrite("2\n",2,1,fp);
		fprintf(cgiOut,"外接设备:能见度&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[15] = 2;
	}
	else if(!strcmp(dev,"3")){//路感
		fwrite("3\n",2,1,fp);
		fprintf(cgiOut,"外接设备:路感&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[15] = 3;
	}
	else if(!strcmp(dev,"0")){//无
		fwrite("0\n",2,1,fp);
		fprintf(cgiOut,"外接设备:无&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[15] = 0;
	}

	cgiFormString("databit_3",databit,5);//得到串口3的数据位
	if(!strcmp(databit,"6")){
		fwrite("6\n",2,1,fp);
		fprintf(cgiOut,"数据位:6位&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[39] = 6;
	}
	else if(!strcmp(databit,"7")){
		fwrite("7\n",2,1,fp);
		fprintf(cgiOut,"数据位:7位&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[39] = 7;
	}
	else if(!strcmp(databit,"8")){
		fwrite("8\n",2,1,fp);
		fprintf(cgiOut,"数据位:8位&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[39] = 8;
	}

	cgiFormString("stopbit_3",stopbit,5);//得到串口3的停止位
	if(!strcmp(stopbit,"1")){
		fwrite("1\n",2,1,fp);
		fprintf(cgiOut,"停止位:1位<p></p>");
		can_send[43] = 1;
	}
	else if(!strcmp(stopbit,"1.5")){
		fwrite("1.5\n",4,1,fp);
		fprintf(cgiOut,"停止位:1.5位<p></p>");
		can_send[43] = 3;
	}
	else if(!strcmp(stopbit,"2")){
		fwrite("2\n",2,1,fp);
		fprintf(cgiOut,"停止位:2位<p></p>");
		can_send[43] = 2;
	}

	cgiFormString("paritybit_3",paritybit,5);//得到串口3的校验位
	if(!strcmp(paritybit,"0")){
		fwrite("0\n",2,1,fp);
		fprintf(cgiOut,"校验位:无校验&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[35] = 0;
	}
	else if(!strcmp(paritybit,"1")){
		fwrite("1\n",2,1,fp);
		fprintf(cgiOut,"校验位:奇校验&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[35] = 1;
	}
	else if(!strcmp(paritybit,"2")){
		fwrite("2\n",2,1,fp);
		fprintf(cgiOut,"校验位:偶校验&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[35] = 2;
	}
	cgiFormString("baudrate_3",baudrate,10);//得到串口3的波特率
	baud = atoi(baudrate);
	can_send[28] = baud / 16777216;
	can_send[27] = baud %16777216 /65536;
	can_send[26] = baud % 65536 / 256;
	can_send[25] = baud %256;

	if(!strcmp(baudrate,"9600")){
		fwrite("9600\n",5,1,fp);
		fprintf(cgiOut,"波特率:9600<p></p>");
	}
	else if(!strcmp(baudrate,"19200")){
		fwrite("19200\n",6,1,fp);
		fprintf(cgiOut,"波特率:19200<p></p>");
	}
	else if(!strcmp(baudrate,"38400")){
		fwrite("38400\n",6,1,fp);
		fprintf(cgiOut,"波特率:38400<p></p>");
	}
	else if(!strcmp(baudrate,"57600")){
		fwrite("57600\n",6,1,fp);
		fprintf(cgiOut,"波特率:57600<p></p>");
	}
	else if(!strcmp(baudrate,"115200")){
		fwrite("115200\n",7,1,fp);
		fprintf(cgiOut,"波特率:115200<p></p>");
	}
	fprintf(cgiOut,"<p></p>");

	fprintf(cgiOut,"<h4>串口4配置信息：</h4><p></p>");
	bzero(dev,5);
	bzero(databit,5);
	bzero(stopbit,5);
	bzero(paritybit,5);
	bzero(baudrate,10);
	cgiFormString("device_4",dev,5);
	if(!strcmp(dev,"1")){//六要素
		fwrite("1\n",2,1,fp);
		fprintf(cgiOut,"外接设备:六要素&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[16] = 1;
	}
	else if(!strcmp(dev,"2")){//能见度
		fwrite("2\n",2,1,fp);
		fprintf(cgiOut,"外接设备:能见度&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[16] = 2;
	}
	else if(!strcmp(dev,"3")){//路感
		fwrite("3\n",2,1,fp);
		fprintf(cgiOut,"外接设备:路感&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[16] = 3;
	}
	else if(!strcmp(dev,"0")){//无
		fwrite("0\n",2,1,fp);
		fprintf(cgiOut,"外接设备:无&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[16] = 0;
	}

	cgiFormString("databit_4",databit,5);//得到串口4的数据位
	if(!strcmp(databit,"6")){
		fwrite("6\n",2,1,fp);
		fprintf(cgiOut,"数据位:6位&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[40] = 6;
	}
	else if(!strcmp(databit,"7")){
		fwrite("7\n",2,1,fp);
		fprintf(cgiOut,"数据位:7位&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[40] = 7;
	}
	else if(!strcmp(databit,"8")){
		fwrite("8\n",2,1,fp);
		fprintf(cgiOut,"数据位:8位&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[40] = 8;
	}

	cgiFormString("stopbit_4",stopbit,5);//得到串口4的停止位
	if(!strcmp(stopbit,"1")){
		fwrite("1\n",2,1,fp);
		fprintf(cgiOut,"停止位:1位<p></p>");
		can_send[44] = 1;
	}
	else if(!strcmp(stopbit,"1.5")){
		fwrite("1.5\n",4,1,fp);
		fprintf(cgiOut,"停止位:1.5位<p></p>");
		can_send[44] = 3;
	}
	else if(!strcmp(stopbit,"2")){
		fwrite("2\n",2,1,fp);
		fprintf(cgiOut,"停止位:2位<p></p>");
		can_send[44] = 2;
	}

	cgiFormString("paritybit_4",paritybit,5);//得到串口4的校验位
	if(!strcmp(paritybit,"0")){
		fwrite("0\n",2,1,fp);
		fprintf(cgiOut,"校验位:无校验&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[36] = 0;
	}
	else if(!strcmp(paritybit,"1")){
		fwrite("1\n",2,1,fp);
		fprintf(cgiOut,"校验位:奇校验&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[36] = 1;
	}
	else if(!strcmp(paritybit,"2")){
		fwrite("2\n",2,1,fp);
		fprintf(cgiOut,"校验位:偶校验&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
		can_send[36] = 2;
	}
	cgiFormString("baudrate_4",baudrate,10);//得到串口4的波特率
	baud = atoi(baudrate);
	can_send[32] = baud / 16777216;
	can_send[31] = baud %16777216 /65536;
	can_send[30] = baud % 65536 / 256;
	can_send[29] = baud %256;

	if(!strcmp(baudrate,"9600")){
		fwrite("9600\n",5,1,fp);
		fprintf(cgiOut,"波特率:9600<p></p>");
	}
	else if(!strcmp(baudrate,"19200")){
		fwrite("19200\n",6,1,fp);
		fprintf(cgiOut,"波特率:19200<p></p>");
	}
	else if(!strcmp(baudrate,"38400")){
		fwrite("38400\n",6,1,fp);
		fprintf(cgiOut,"波特率:38400<p></p>");
	}
	else if(!strcmp(baudrate,"57600")){
		fwrite("57600\n",6,1,fp);
		fprintf(cgiOut,"波特率:57600<p></p>");
	}
	else if(!strcmp(baudrate,"115200")){
		fwrite("115200\n",7,1,fp);
		fprintf(cgiOut,"波特率:115200<p></p>");
	}
	fprintf(cgiOut,"<p></p>");
	func_crc(can_send,45,can_send+45,can_send+46);
	can_send_data(can_send);

	fprintf(cgiOut,"</BODY></HTML>");
	return 0;
}
