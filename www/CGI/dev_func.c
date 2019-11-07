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

#include "cgic.h"

extern void func_crc(const uint8_t *buf,uint32_t len ,uint8_t *out_low, uint8_t *out_high);

int can_fd;  
uint8_t can_send[47]={
0xFE,0xAA,0x02,0x01,0x00,0x02,0x00,0x2D,0x00,0xC9,0x01,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x01,0x02
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
			for(i=0;i<8;i++)
			//	printf("%02x ",frame.data[i]);
			write(can_fd,&frame,sizeof(frame));
			sended_count += 8;
			len -= 8;
			if(len == 0)
				break;
		}
		if(len < 8){
			frame.can_dlc = len;
			memcpy(frame.data,buf+sended_count,len);
			for(i=0;i<len;i++)
			//	printf("%02x ",frame.data[i]);
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
	uint8_t func_1[2] = {0};
	FILE *fp;
	fp = fopen("dev_func/dev_func.txt","w");
	if(fp == NULL)
		fprintf(cgiOut,"open dev_func.txt error\n");
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>dev_func</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	fprintf(cgiOut,"<h4>设置功能信息：</h4><br>");
	char adc_check_1[2] = {0};
	cgiFormString("adc_check_1",adc_check_1,2);
	if(!strcmp(adc_check_1,"1")){//开启
		func_1[0] |= 0x01; //bit0 1
		fprintf(cgiOut,"第一路电源检测开启<p></p>");
	}
	else{
		func_1[0] &= ~0x01; // bit 0 0;
		fprintf(cgiOut,"第一路电源检测关闭<p></p>");
	}

	char adc_check_2[2] = {0};
	cgiFormString("adc_check_2",adc_check_2,2);
	if(!strcmp(adc_check_2,"1")){//开启
		func_1[0] |= 0x02; //bit1 1
		fprintf(cgiOut,"第二路电源检测开启<p></p>");
	}
	else{
		func_1[0] &= ~0x02; // bit1 0;
		fprintf(cgiOut,"第二路电源检测关闭<p></p>");
	}

	char adc_check_3[2] = {0};
	cgiFormString("adc_check_3",adc_check_3,2);
	if(!strcmp(adc_check_3,"1")){//开启
		func_1[0] |= 0x04; //bit2 1
		fprintf(cgiOut,"第三路电源检测开启<p></p>");
	}
	else{
		func_1[0] &= ~0x04; // bit2 0;
		fprintf(cgiOut,"第三路电源检测关闭<p></p>");
	}
	char adc_check_4[2] = {0};
	cgiFormString("adc_check_4",adc_check_4,2);
	if(!strcmp(adc_check_4,"1")){//开启
		func_1[0] |= 0x08; //bit3 1
		fprintf(cgiOut,"第四路电源检测开启<p></p>");
	}
	else{
		func_1[0] &= ~0x08; // bit3 0;
		fprintf(cgiOut,"第四路电源检测关闭<p></p>");
	}

	char ter_ser_220v_check[2] = {0};
	cgiFormString("ter_ser_220v_check",ter_ser_220v_check,2);
	if(!strcmp(ter_ser_220v_check,"1")){//开启
		func_1[0] |= 0x10; //bit4 1
		fprintf(cgiOut,"终端服务器220V检测开启<p></p>");

	}
	else{
		func_1[0] &= ~0x10; //bit4 0;
		fprintf(cgiOut,"终端服务器220V检测关闭<p></p>");
	}

	char front_door[2] = {0};
	cgiFormString("front_door",front_door,2);
	if(!strcmp(front_door,"1")){//开启
		func_1[0] |= 0x20; //bit5 1
		fprintf(cgiOut,"机柜前门检测开启<p></p>");
	}
	else{
		func_1[0] &= ~0x20; // bit5 0;
		fprintf(cgiOut,"机柜前门检测关闭<p></p>");
	}

	char back_door[2] = {0};
	cgiFormString("back_door",back_door,2);
	if(!strcmp(back_door,"1")){//开启
		func_1[0] |= 0x40; //bit6 1
		fprintf(cgiOut,"机柜后门检测开启<p></p>");
	}
	else{
		func_1[0] &= ~0x40; // bit6 0;
		fprintf(cgiOut,"机柜后门检测关闭<p></p>");
	}

	char cabinet_water[2] = {0};
	cgiFormString("cabinet_water",cabinet_water,2);
	if(!strcmp(cabinet_water,"1")){//开启
		func_1[0] |= 0x80; //bit7 1
		fprintf(cgiOut,"机柜涉水检测开启<p></p>");
	}
	else{
		func_1[0] &= ~0x80; // bit7 0;
		fprintf(cgiOut,"机柜涉水检测关闭<p></p>");
	}

	can_send[17] = func_1[0];//功能写到相应位置
	char inttostr[10] = {0};
	sprintf(inttostr,"%d",func_1[0]);
	fwrite(inttostr,strlen(inttostr),1,fp);//32位整数 低8位写到文件里
	fwrite("\n",1,1,fp);

	char cabinet_hit[2] = {0};
	cgiFormString("cabinet_hit",cabinet_hit,2);
	if(!strcmp(cabinet_hit,"1")){//开启
		func_1[1] |= 0x01; //bit8 1
		fprintf(cgiOut,"机柜撞击检测开启<p></p>");
	}
	else{
		func_1[1] &= ~0x01; // bit8 0;
		fprintf(cgiOut,"机柜撞击检测关闭<p></p>");
	}

	char cabinet_fan[2] = {0};
	cgiFormString("cabinet_fan",cabinet_fan,2);
	if(!strcmp(cabinet_fan,"1")){//开启
		func_1[1] |= 0x02; //bit9 1
		fprintf(cgiOut,"机柜风扇检测开启<p></p>");
	}
	else{
		func_1[1] &= ~0x02; // bit9 0;
		fprintf(cgiOut,"机柜风扇检测关闭<p></p>");
	}

	char cabinet_heat[2] = {0};
	cgiFormString("cabinet_heat",cabinet_heat,2);
	if(!strcmp(cabinet_heat,"1")){//开启
		func_1[1] |= 0x04; //bit10 1
		fprintf(cgiOut,"机柜加热器检测开启<p></p>");
	}
	else{
		func_1[1] &= ~0x04; // bit10 0;
		fprintf(cgiOut,"机柜加热器检测关闭<p></p>");
	}

	char phone_func[2] = {0};
	cgiFormString("phone_func",phone_func,2);
	if(!strcmp(phone_func,"1")){//开启
		func_1[1] |= 0x08; //bit11 1
		fprintf(cgiOut,"拨打电话功能开启<p></p>");
	}
	else{
		func_1[1] &= ~0x08; // bit11 0;
		fprintf(cgiOut,"拨打电话功能关闭<p></p>");
	}

	char weather_station[2] = {0};
	cgiFormString("weather_station",weather_station,2);
	if(!strcmp(weather_station,"1")){//开启
		func_1[1] |= 0x10; //bit12 1
		fprintf(cgiOut,"气象站开启<p></p>");
	}
	else{
		func_1[1] &= ~0x10; // bit12 0;
		fprintf(cgiOut,"气象站关闭<p></p>");
	}

	char visibility[2] = {0};
	cgiFormString("visibility",visibility,2);
	if(!strcmp(visibility,"1")){//开启
		func_1[1] |= 0x20; //bit13 1
		fprintf(cgiOut,"能见度开启<p></p>");
	}
	else{
		func_1[1] &= ~0x20; // bit13 0;
		fprintf(cgiOut,"能见度关闭<p></p>");
	}

	char road_sense[2] = {0};
	cgiFormString("road_sense",road_sense,2);
	if(!strcmp(road_sense,"1")){//开启
		func_1[1] |= 0x40; //bit14 1
		fprintf(cgiOut,"路感开启<p></p>");
	}
	else{
		func_1[1] &= ~0x40; // bit14 0;
		fprintf(cgiOut,"路感关闭<p></p>");
	}

	can_send[18] = func_1[1];
	bzero(inttostr,10);
	sprintf(inttostr,"%d",func_1[1]);
	fwrite(inttostr,strlen(inttostr),1,fp);
	fwrite("\n",1,1,fp);//第二个u32 8-15bit

	char fan_rotate_temp[10] = {0};
	cgiFormString("fan_rotate_temp",fan_rotate_temp,5);
	fprintf(cgiOut,"风扇启动温度:%s 度<p></p>",fan_rotate_temp);


	fwrite(fan_rotate_temp,strlen(fan_rotate_temp),1,fp);
	fwrite("\n",1,1,fp);
	uint16_t temp = 0;
	temp = atoi(fan_rotate_temp);
	can_send[22] = temp/256;
	can_send[21] = temp%256;

	char heat_temp[10] = {0};
	cgiFormString("heater_heat_temp",heat_temp,10);
	fprintf(cgiOut,"加热器启动温度:%s 度<p></p>",heat_temp);

	fwrite(heat_temp,strlen(heat_temp),1,fp);
	fwrite("\n",1,1,fp);
	if(heat_temp[0] == '-'){//输入负温度
		char heat_temp_1[10] = {0};
		strcpy(heat_temp_1,heat_temp+1);//去掉负号
		temp = atoi(heat_temp_1);
		can_send[24] = 0x01;
		can_send[23] = temp;
	}
	else{
		temp = atoi(heat_temp);
		can_send[24] = 0x00;
		can_send[23] = temp;
	}
	char adc_check_1_dir[10] = {0};
	cgiFormString("adc_check_1_dir",adc_check_1_dir,10);
	if(!strcmp(adc_check_1_dir,"东")){
		can_send[25] = 1;
		fwrite("1\n",2,1,fp);
		if(!strcmp(adc_check_1,"1"))
			fprintf(cgiOut,"第一路电源检测方向:东<p></p>");
	}
	else if(!strcmp(adc_check_1_dir,"南")){
		can_send[25] = 2;
		fwrite("2\n",2,1,fp);
		if(!strcmp(adc_check_1,"1"))
			fprintf(cgiOut,"第一路电源检测方向:南<p></p>");
	}
	else if(!strcmp(adc_check_1_dir,"西")){
		can_send[25] = 3;
		fwrite("3\n",2,1,fp);
		if(!strcmp(adc_check_1,"1"))
			fprintf(cgiOut,"第一路电源检测方向:西<p></p>");
	}
	else if(!strcmp(adc_check_1_dir,"北")){
		can_send[25] = 4;
		fwrite("4\n",2,1,fp);
		if(!strcmp(adc_check_1,"1"))
			fprintf(cgiOut,"第一路电源检测方向:北<p></p>");
	}
	else if(!strcmp(adc_check_1_dir,"无")){
		can_send[25] = 0;
		fwrite("0\n",2,1,fp);
		if(!strcmp(adc_check_1,"1"))
			fprintf(cgiOut,"第一路电源检测方向:无<p></p>");
	}


	char adc_check_2_dir[10] = {0};
	cgiFormString("adc_check_2_dir",adc_check_2_dir,10);
	if(!strcmp(adc_check_2_dir,"东")){
		can_send[26] = 1;
		fwrite("1\n",2,1,fp);
		if(!strcmp(adc_check_2,"1"))
			fprintf(cgiOut,"第二路电源检测方向:东<p></p>");
	}
	else if(!strcmp(adc_check_2_dir,"南")){
		can_send[26] = 2;
		fwrite("2\n",2,1,fp);
		if(!strcmp(adc_check_2,"1"))
			fprintf(cgiOut,"第二路电源检测方向:南<p></p>");
	}
	else if(!strcmp(adc_check_2_dir,"西")){
		can_send[26] = 3;
		fwrite("3\n",2,1,fp);
		if(!strcmp(adc_check_2,"1"))
			fprintf(cgiOut,"第二路电源检测方向:西<p></p>");
	}
	else if(!strcmp(adc_check_2_dir,"北")){
		can_send[26] = 4;
		fwrite("4\n",2,1,fp);
		if(!strcmp(adc_check_2,"1"))
			fprintf(cgiOut,"第二路电源检测方向:北<p></p>");
	}
	else if(!strcmp(adc_check_2_dir,"无")){
		can_send[26] = 0;
		fwrite("0\n",2,1,fp);
		if(!strcmp(adc_check_2,"1"))
			fprintf(cgiOut,"第二路电源检测方向:无<p></p>");
	}


	char adc_check_3_dir[10] = {0};
	cgiFormString("adc_check_3_dir",adc_check_3_dir,10);
	if(!strcmp(adc_check_3_dir,"东")){
		can_send[27] = 1;
		fwrite("1\n",2,1,fp);
		if(!strcmp(adc_check_3,"1"))
			fprintf(cgiOut,"第三路电源检测方向:东<p></p>");
	}
	else if(!strcmp(adc_check_3_dir,"南")){
		can_send[27] = 2;
		fwrite("2\n",2,1,fp);
		if(!strcmp(adc_check_3,"1"))
			fprintf(cgiOut,"第三路电源检测方向:南<p></p>");
	}
	else if(!strcmp(adc_check_3_dir,"西")){
		can_send[27] = 3;
		fwrite("3\n",2,1,fp);
		if(!strcmp(adc_check_3,"1"))
			fprintf(cgiOut,"第三路电源检测方向:西<p></p>");
	}
	else if(!strcmp(adc_check_3_dir,"北")){
		can_send[27] = 4;
		fwrite("4\n",2,1,fp);
		if(!strcmp(adc_check_3,"1"))
			fprintf(cgiOut,"第三路电源检测方向:北<p></p>");
	}
	else if(!strcmp(adc_check_3_dir,"无")){
		can_send[27] = 0;
		fwrite("0\n",2,1,fp);
		if(!strcmp(adc_check_3,"1"))
			fprintf(cgiOut,"第三路电源检测方向:无<p></p>");
	}


	char adc_check_4_dir[10] = {0};
	cgiFormString("adc_check_4_dir",adc_check_4_dir,10);
	if(!strcmp(adc_check_4_dir,"东")){
		can_send[28] = 1;
		fwrite("1\n",2,1,fp);
		if(!strcmp(adc_check_4,"1"))
			fprintf(cgiOut,"第四路电源检测方向:东<p></p>");
	}
	else if(!strcmp(adc_check_4_dir,"南")){
		can_send[28] = 2;
		fwrite("2\n",2,1,fp);
		if(!strcmp(adc_check_4,"1"))
			fprintf(cgiOut,"第四路电源检测方向:南<p></p>");
	}
	else if(!strcmp(adc_check_4_dir,"西")){
		can_send[28] = 3;
		fwrite("3\n",2,1,fp);
		if(!strcmp(adc_check_4,"1"))
			fprintf(cgiOut,"第四路电源检测方向:西<p></p>");
	}
	else if(!strcmp(adc_check_4_dir,"北")){
		can_send[28] = 4;
		fwrite("4\n",2,1,fp);
		if(!strcmp(adc_check_4,"1"))
			fprintf(cgiOut,"第四路电源检测方向:北<p></p>");
	}
	else if(!strcmp(adc_check_4_dir,"无")){
		can_send[28] = 0;
		fwrite("0\n",2,1,fp);
		if(!strcmp(adc_check_4,"1"))
			fprintf(cgiOut,"第四路电源检测方向:无<p></p>");
	}


	func_crc(can_send,45,can_send+45,can_send+46);

	can_send_data(can_send);
	
	fclose(fp);
	char filename[100] = {0};
	sprintf(filename,"chmod 666 dev_func/dev_func.txt");
	system(filename);
	

	fprintf(cgiOut,"<br><a href=\"dev_func_show.cgi\"><b>返回</b></a>");
	fprintf(cgiOut,"</body><html>");

	return 0;
}
