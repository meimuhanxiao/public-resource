#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include "cgic.h"
#define GETLEN 	1024*5
#define PATH_NAME "/"
#define PRO_ID	100

uint8_t *mem_map()
{
	key_t key;
	int shmid;
	if((key = ftok(PATH_NAME, PRO_ID)) < 0)//使用ftok函数获取键值

	{
		perror("fail to ftok");
		exit(1);
	}

	if((shmid = shmget(key, GETLEN, IPC_CREAT | 0777)) < 0)//创建或者打开一个共享内存

	{
		perror("fail to shmget");
		exit(1);
	}
	uint8_t *p = shmat(shmid, NULL, 0);//内核里的共享内存映射到用户空间
	if(p == (void *)-1){
		perror("shmat");
		exit(1);
	}
	return p;
}


int cgiMain()
{
	uint8_t *map_p = mem_map();//共享内存起始地址
	uint16_t datalen = map_p[8]*256+map_p[7];

	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>");
	fprintf(cgiOut, "<head> <meta charset='UTF-8'>");
	//fprintf(cgiOut, "<title>显示界面</title>");
	fprintf(cgiOut,"<style>");
	fprintf(cgiOut,"#meteorologicalData{width: 100%%;}");
	fprintf(cgiOut,"#meteorologicalData .deviceAll{position: relative;width: 33.3%%;border: 1px solid #ccc;padding: 20px;}");
	fprintf(cgiOut,"#meteorologicalData .device{float: left;}");
	fprintf(cgiOut,"#meteorologicalData .device-img{padding-right: 20px;float: right;}");
	fprintf(cgiOut,"</style>");
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"5\"><TITLE>realdata</TITLE></HEAD>");
	fprintf(cgiOut, "<BODY>");

	fprintf(cgiOut,"<table  width=\"100%%\" >");
	fprintf(cgiOut,"<tr   width=\"100%%\" >");

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/平均能见度 瞬时能见度.png\">");
	fprintf(cgiOut,"<br>瞬时能见度<br>");
	uint32_t ins_vib = map_p[17]*16777216 + map_p[16]*65536+map_p[15]*256+map_p[14];
	fprintf(cgiOut,"%dm </td>",ins_vib);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/平均能见度 瞬时能见度.png\">");
	fprintf(cgiOut,"<br>平均能见度<br>");
	uint32_t ava_vib = map_p[21]*16777216 + map_p[20]*65536+map_p[19]*256+map_p[18];
	fprintf(cgiOut,"%dm </td>",ava_vib);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/能见度设备状态.png\">");
	fprintf(cgiOut,"<br>能见度设备状态<br>");
	switch(map_p[23]){
	case 0:{
			   if(map_p[22] == 0)
				   fprintf(cgiOut,"能见度平稳 正常 </td>");
			   else if(map_p[22] == 1)
				   fprintf(cgiOut,"能见度平稳 电源故障 </td>");
			   else if(map_p[22] == 2)
				   fprintf(cgiOut,"能见度平稳 接收器失明 </td>");
			   else if(map_p[22] == 3)
				   fprintf(cgiOut,"能见度平稳 接收器故障 </td>");
			   else if(map_p[22] == 4)
				   fprintf(cgiOut,"能见度平稳 发射器温度故障 </td>");
			   break;
		   }
	case 1:{
			   if(map_p[22] == 0)
				   fprintf(cgiOut,"能见度降低 正常 </td>");
			   else if(map_p[22] == 1)
				   fprintf(cgiOut,"能见度降低 电源故障 </td>");
			   else if(map_p[22] == 2)
				   fprintf(cgiOut,"能见度降低 接收器失明 </td>");
			   else if(map_p[22] == 3)
				   fprintf(cgiOut,"能见度降低 接收器故障 </td>");
			   else if(map_p[22] == 4)
				   fprintf(cgiOut,"能见度降低 发射器温度故障 </td>");
			   break;
		   }
	case 2:{
			   if(map_p[22] == 0)
				   fprintf(cgiOut,"能见度升高 正常 </td>");
			   else if(map_p[22] == 1)
				   fprintf(cgiOut,"能见度升高 电源故障 </td>");
			   else if(map_p[22] == 2)
				   fprintf(cgiOut,"能见度升高 接收器失明 </td>");
			   else if(map_p[22] == 3)
				   fprintf(cgiOut,"能见度升高 接收器故障 </td>");
			   else if(map_p[22] == 4)
				   fprintf(cgiOut,"能见度升高 发射器温度故障 </td>");
			   break;
		   }
	}

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/路面温度.png\">");
	fprintf(cgiOut,"<br>路面温度<br>");
	uint16_t road_tmp = map_p[25]*256+map_p[24];
	int16_t road_tmp_i = 0 ;
	memcpy(&road_tmp_i,&road_tmp,4);
	fprintf(cgiOut,"%.1fC </td>",road_tmp_i*0.1);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/水膜厚度.png\">");
	fprintf(cgiOut,"<br>水膜厚度<br>");
	uint16_t water_film = map_p[27]*256+map_p[26];
	fprintf(cgiOut,"%.2fmm </td>",water_film*0.01);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/雪膜厚度.png\">");
	fprintf(cgiOut,"<br>雪膜厚度<br>");
	uint16_t snow_film = map_p[29]*256+map_p[28];
	fprintf(cgiOut,"%.2fmm </td>",snow_film*0.01);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/冰膜厚度.png\">");
	fprintf(cgiOut,"<br>冰膜厚度<br>");
	uint16_t ice_film = map_p[31]*256+map_p[30];
	fprintf(cgiOut,"%.2fmm </td>",ice_film*0.01);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/路面摩擦系数.png\">");
	fprintf(cgiOut,"<br>路面摩擦系数<br>");
	uint16_t Friction_coefficient = map_p[33]*256+map_p[32];
	fprintf(cgiOut,"%.2f </td>",Friction_coefficient*0.01);

	fprintf(cgiOut,"</tr><p></p><p></p></p><p></p>");


	fprintf(cgiOut,"<tr height=120px  width=\"100%%\" >");

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/路况.png\">");
	fprintf(cgiOut,"<br>路况<br>");
	switch(map_p[35]){
	case 0:{
			   if(map_p[34] == 0)
				   fprintf(cgiOut,"无警告 出错 </td>");
			   else if(map_p[34] == 1)
				   fprintf(cgiOut,"无警告 干 </td>");
			   else if(map_p[34] == 2)
				   fprintf(cgiOut,"无警告 潮 </td>");
			   else if(map_p[34] == 3)
				   fprintf(cgiOut,"无警告 湿 </td>");
			   else if(map_p[34] == 6)
				   fprintf(cgiOut,"无警告 雪 </td>");
			   else if(map_p[34] == 7)
				   fprintf(cgiOut,"无警告 冰 </td>");
			   else if(map_p[34] == 9)
				   fprintf(cgiOut,"无警告 泥泞 </td>");
			   break;
		   }
	case 1:{
			   if(map_p[34] == 0)
				   fprintf(cgiOut,"警告 出错 </td>");
			   else if(map_p[34] == 1)
				   fprintf(cgiOut,"警告 干 </td>");
			   else if(map_p[34] == 2)
				   fprintf(cgiOut,"警告 潮 </td>");
			   else if(map_p[34] == 3)
				   fprintf(cgiOut,"警告 湿 </td>");
			   else if(map_p[34] == 6)
				   fprintf(cgiOut,"警告 雪 </td>");
			   else if(map_p[34] == 7)
				   fprintf(cgiOut,"警告 冰 </td>");
			   else if(map_p[34] == 9)
				   fprintf(cgiOut,"警告 泥泞 </td>");
			   break;
		   }
	case 2:{
			   if(map_p[34] == 0)
				   fprintf(cgiOut,"报警 出错 </td>");
			   else if(map_p[34] == 1)
				   fprintf(cgiOut,"报警 干 </td>");
			   else if(map_p[34] == 2)
				   fprintf(cgiOut,"报警 潮 </td>");
			   else if(map_p[34] == 3)
				   fprintf(cgiOut,"报警 湿 </td>");
			   else if(map_p[34] == 6)
				   fprintf(cgiOut,"报警 雪 </td>");
			   else if(map_p[34] == 7)
				   fprintf(cgiOut,"报警 冰 </td>");
			   else if(map_p[34] == 9)
				   fprintf(cgiOut,"报警 泥泞 </td>");
			   break;
		   }
	case 3:{
			   if(map_p[34] == 0)
				   fprintf(cgiOut,"霜警告 出错 </td>");
			   else if(map_p[34] == 1)
				   fprintf(cgiOut,"霜警告 干 </td>");
			   else if(map_p[34] == 2)
				   fprintf(cgiOut,"霜警告 潮 </td>");
			   else if(map_p[34] == 3)
				   fprintf(cgiOut,"霜警告 湿 </td>");
			   else if(map_p[34] == 6)
				   fprintf(cgiOut,"霜警告 雪 </td>");
			   else if(map_p[34] == 7)
				   fprintf(cgiOut,"霜警告 冰 </td>");
			   else if(map_p[34] == 9)
				   fprintf(cgiOut,"霜警告 泥泞 </td>");
			   break;
		   }		   
	}
	
	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/路感设备状态.png\">");
	fprintf(cgiOut,"<br>路感设备状态<br>");
	uint8_t road_dev = map_p[36];
	int fir = road_dev / 16;
	int sec = road_dev % 16;
	switch(fir){
	case 0:
		{
			if(sec == 0)
				fprintf(cgiOut,"接收器窗清洁 硬件OK </td>");
			else if(sec == 1)
				fprintf(cgiOut,"接收器窗清洁 CPU硬件警告 </td>");
			else if(sec == 2)
				fprintf(cgiOut,"接收器窗清洁 检测器硬件警告 </td>");
			else if(sec == 3)
				fprintf(cgiOut,"接收器窗清洁 其他 </td>");
			break;
		}
	case 1:
		{
			if(sec == 0)
				fprintf(cgiOut,"警告-接收器窗污染 硬件OK </td>");
			else if(sec == 1)
				fprintf(cgiOut,"警告-接收器窗污染 CPU硬件警告 </td>");
			else if(sec == 2)
				fprintf(cgiOut,"警告-接收器窗污染 检测器硬件警告 </td>");
			else if(sec == 3)
				fprintf(cgiOut,"警告-接收器窗污染 其他 </td>");
			break;
		}
	case 2:
		{
			if(sec == 0)
				fprintf(cgiOut,"报警-接收器窗污染严重 硬件OK </td>");
			else if(sec == 1)
				fprintf(cgiOut,"报警-接收器窗污染严重 CPU硬件警告 </td>");
			else if(sec == 2)
				fprintf(cgiOut,"报警-接收器窗污染严重 检测器硬件警告 </td>");
			else if(sec == 3)
				fprintf(cgiOut,"报警-接收器窗污染严重 其他 </td>");
			break;
		}
	}

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/最小风速 平均风速 最大风速.png\">");
	fprintf(cgiOut,"<br>最小风速<br>");
	uint16_t ws_min = map_p[38]*256+map_p[37];
	fprintf(cgiOut,"%.1fm/s </td>",ws_min*0.1);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/最小风速 平均风速 最大风速.png\">");
	fprintf(cgiOut,"<br>平均风速<br>");
	uint16_t ws_avg = map_p[40]*256+map_p[39];
	fprintf(cgiOut,"%.1fm/s </td>",ws_avg*0.1);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/最小风速 平均风速 最大风速.png\">");
	fprintf(cgiOut,"<br>最大风速<br>");
	uint16_t ws_max = map_p[42]*256+map_p[41];
	fprintf(cgiOut,"%.1fm/s </td>",ws_max*0.1);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/最小风向 平均风向 最大风向.png\">");
	fprintf(cgiOut,"<br>最小风向<br>");
	uint16_t wd_min = map_p[44]*256+map_p[43];
	fprintf(cgiOut,"%ddeg </td>",wd_min);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/最小风向 平均风向 最大风向.png\">");
	fprintf(cgiOut,"<br>平均风向<br>");
	uint16_t wd_avg = map_p[46]*256+map_p[45];
	fprintf(cgiOut,"%ddeg </td>",wd_avg);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/最小风向 平均风向 最大风向.png\">");
	fprintf(cgiOut,"<br>最大风向<br>");
	uint16_t wd_max = map_p[48]*256+map_p[47];
	fprintf(cgiOut,"%ddeg </td>",wd_max);

	fprintf(cgiOut,"</tr><p></p><p></p></p><p></p>");


	fprintf(cgiOut,"<tr height=120px  width=\"100%%\" >");

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/大气压力.png\">");
	fprintf(cgiOut,"<br>大气压力<br>");
	uint32_t air_pres =  map_p[52]*16777216 + map_p[51]*65536+map_p[50]*256+map_p[49];
	fprintf(cgiOut,"%0.1fhpa </td>",air_pres*0.1);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/大气温度.png\">");
	fprintf(cgiOut,"<br>大气温度<br>");
	int16_t air_tmp = map_p[54]*256+map_p[53];
	fprintf(cgiOut,"%0.1fC </td>",air_tmp*0.1);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/大气湿度.png\">");
	fprintf(cgiOut,"<br>大气湿度<br>");
	uint16_t air_moi = map_p[56]*256+map_p[55];
	fprintf(cgiOut,"%0.1f%%RH </td>",air_moi*0.1);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/日降雨量.png\">");
	fprintf(cgiOut,"<br>日降雨量<br>");
	uint16_t Daily_rainfall = map_p[58]*256+map_p[57];
	fprintf(cgiOut,"%0.1fmm </td>",Daily_rainfall*0.1);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/降雨持续时间.png\">");
	fprintf(cgiOut,"<br>降雨持续时间<br>");
	uint32_t Duration_of_rainfall = map_p[62]*16777216 + map_p[61]*65536+map_p[60]*256+map_p[59];
	uint8_t rain_day = Duration_of_rainfall / (3600*24);
	Duration_of_rainfall %= (3600*24);
	uint8_t rain_hour = Duration_of_rainfall / 3600;
	Duration_of_rainfall %= 3600;
	uint8_t rain_min = Duration_of_rainfall / 60;
	Duration_of_rainfall %= 60;
	fprintf(cgiOut,"%d天%d时%d分%d秒 </td>",rain_day,rain_hour,rain_min,Duration_of_rainfall);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/雨量强度.png\">");
	fprintf(cgiOut,"<br>雨量强度<br>");
	uint32_t Rainfall_intensity = map_p[66]*16777216 + map_p[65]*65536+map_p[64]*256+map_p[63];
	fprintf(cgiOut,"%0.1fmm </td>",Rainfall_intensity*0.1);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/雨强最大值.png\">");
	fprintf(cgiOut,"<br>雨强最大值<br>");
	uint32_t max_rain = map_p[70]*16777216 + map_p[69]*65536+map_p[68]*256+map_p[67];
	fprintf(cgiOut,"%0.1fmm </td>",max_rain*0.1);

	fprintf(cgiOut,"<td width=\"80\" height=\"80\" align=\"center\" ><img src=\"../24个气象数据/天气.png\">");
	fprintf(cgiOut,"<br>天气<br>");
	char weater[3] = {0};
	
//	uint16_t weater = map_p[72]*256+map_p[71];
	switch(map_p[71]){
	case 0x1B:{
				  strcpy(weater,"R");
				  if(map_p[72] == 0x2B)
					  strcat(weater,"+");
				  else if(map_p[72] == 0x2D)
					  strcat(weater,"-");
				  else if(map_p[72] == 0x20)
					  strcat(weater," ");
				  break;
			  }
	case 0x1C:
			  {
				  strcpy(weater,"S");
				  if(map_p[72] == 0x2B)
					  strcat(weater,"+");
				  else if(map_p[72] == 0x2D)
					  strcat(weater,"-");
				  else if(map_p[72] == 0x20)
					  strcat(weater," ");
				  break;
			  }
	case 0x19:
			  {
				  strcpy(weater,"P");
				  if(map_p[72] == 0x2B)
					  strcat(weater,"+");
				  else if(map_p[72] == 0x2D)
					  strcat(weater,"-");
				  else if(map_p[72] == 0x20)
					  strcat(weater," ");
				  break;
			  }
	case 0x0F:
			  {
				  strcpy(weater,"F");
				  if(map_p[72] == 0x2B)
					  strcat(weater,"+");
				  else if(map_p[72] == 0x2D)
					  strcat(weater,"-");
				  else if(map_p[72] == 0x20)
					  strcat(weater," ");
				  break;
			  }
	case 0x0C:
			  {
				  strcpy(weater,"C");
				  if(map_p[72] == 0x2B)
					  strcat(weater,"+");
				  else if(map_p[72] == 0x2D)
					  strcat(weater,"-");
				  else if(map_p[72] == 0x20)
					  strcat(weater," ");
				  break;
			  }
	}
	if(strcmp(weater,"R-") == 0)
		fprintf(cgiOut,"小雨 </td>");
	else if(strcmp(weater,"R ") == 0)
		fprintf(cgiOut,"中雨 </td>");
	else if(strcmp(weater,"R+") == 0)
		fprintf(cgiOut,"大雨 </td>");
	else if(strcmp(weater,"S-") == 0)
		fprintf(cgiOut,"小雪 </td>");
	else if(strcmp(weater,"S ") == 0)
		fprintf(cgiOut,"中雪 </td>");
	else if(strcmp(weater,"S+") == 0)
		fprintf(cgiOut,"大雪 </td>");
	else if(strcmp(weater,"P-") == 0)
		fprintf(cgiOut,"小混合降水 </td>");
	else if(strcmp(weater,"P ") == 0)
		fprintf(cgiOut,"中混合降水 </td>");
	else if(strcmp(weater,"P+") == 0)
		fprintf(cgiOut,"大混合降水 </td>");
	else if(strcmp(weater,"F-") == 0)
		fprintf(cgiOut,"轻雾 </td>");
	else if(strcmp(weater,"F ") == 0)
		fprintf(cgiOut,"雾 </td>");
	else if(strcmp(weater,"F+") == 0)
		fprintf(cgiOut,"浓雾 </td>");
	else if(strcmp(weater,"C ") == 0)
		fprintf(cgiOut,"无降水 </td>");
	else
		fprintf(cgiOut,"未知 </td>");


	fprintf(cgiOut,"</tr></table><p></p><br>");

	fprintf(cgiOut,"<table width= border=\"0\" id=\"meteorologicalData\" >");

	int i;
	for(i = 0; i < map_p[13];i++){//几个设备
		if((i % 3) == 0)
			fprintf(cgiOut,"<tr>");//新的一行
		fprintf(cgiOut,"<td class=\"deviceAll\">");//这一列样式
		fprintf(cgiOut,"<div class=\"device\">");//显示信息样式
		uint8_t dev_type = map_p[73+i*36];
		switch(dev_type){
		case 0x30:
			fprintf(cgiOut,"设备类型： 相机<br> ");
			break;
		case 0x31:
			fprintf(cgiOut,"设备类型： 终端服务器<br>");
			break;
		case 0x32:
			fprintf(cgiOut,"设备类型： 补光灯<br>");
			break;
		case 0x39:
			fprintf(cgiOut,"设备类型： 能见度<br>");
			break;
		case 0x3A:
			fprintf(cgiOut,"设备类型： 路感<br>");
			break;
		case 0x3B:
			fprintf(cgiOut,"设备类型： 气象站<br>");
			break;
		case 0x3C:
			fprintf(cgiOut,"设备类型： 机柜<br>");
			break;

		default:
			fprintf(cgiOut,"未知设备 <br>");
		}

		fprintf(cgiOut,"设备编号： %c%c%c%c%c%c%c%c<br>",map_p[74+i*36],map_p[75+i*36],map_p[76+i*36],map_p[77+i*36],map_p[78+i*36],map_p[79+i*36],map_p[80+i*36],map_p[81+i*36]);

		uint8_t now_state = map_p[82+i*36];
		switch(now_state){
		case 1:
			fprintf(cgiOut,"当前状态： 在线<br>");
			break;
		case 2:
			fprintf(cgiOut,"当前状态： 离线<br>");
			break;
		case 3:
			fprintf(cgiOut,"当前状态： 故障<br>");
			break;
		case 4:
			fprintf(cgiOut,"当前状态： 异常<br>");
			break;
		default:
			fprintf(cgiOut,"状态未知  <br>");
		}

		uint16_t fault_num = map_p[84+i*36]*256+map_p[83+i*36];
		fprintf(cgiOut,"故障次数： %d <br>",fault_num);

		//uint16_t fault_type = map_p[86+i*36]*256+map_p[85+i*36];
		if((map_p[86+i*36]== 0)&&(map_p[85+i*36]==0))
			fprintf(cgiOut,"故障类型：     无故障<br>");
		else if((map_p[85+i*36]== 0x70)&&(map_p[86+i*36]==0x02))
			fprintf(cgiOut,"故障类型：     交流电源故障<br>");
		else if((map_p[85+i*36]== 0xA0)&&(map_p[86+i*36]==0x02))
			fprintf(cgiOut,"故障类型：     网络故障<br>");
		else if((map_p[85+i*36]== 0x71)&&(map_p[86+i*36]==0x02))
			fprintf(cgiOut,"故障类型：     状态检测异常<br>");
		else if((map_p[85+i*36]== 0x02)&&(map_p[86+i*36]==0x00))
			fprintf(cgiOut,"故障类型：     离线<br>");
		else if((map_p[85+i*36]== 0x35)&&(map_p[86+i*36]==0x02))
			fprintf(cgiOut,"故障类型：     机柜进水<br>");
		else if((map_p[85+i*36]== 0x33)&&(map_p[86+i*36]==0x00))
			fprintf(cgiOut,"故障类型：     机柜撞击<br>");

		time_t t;
		struct tm* mydate;

		uint32_t fault_time = map_p[90+i*36]*16777216 + map_p[89+i*36]*65536+map_p[88+i*36]*256+map_p[87+i*36];

		if(fault_time == 0)
			fprintf(cgiOut,"故障时间：    无故障<br>");
		else{
			t = fault_time;
			t -= 8*60*60;//共享内存里故障时间是+8的
			mydate = localtime (&t);
			fprintf(cgiOut,"故障时间：     %04d/%02d/%02d-%02d:%02d:%02d<br>",mydate->tm_year+1900,mydate->tm_mon+1,mydate->tm_mday,mydate->tm_hour,mydate->tm_min,mydate->tm_sec);
		}

		uint32_t fault_time_total = 0;
		uint8_t fault_day = 0;
		uint8_t fault_hour = 0;
		uint8_t fault_min = 0;

		fault_time_total = map_p[94+i*36]*16777216 + map_p[93+i*36]*65536+map_p[92+i*36]*256+map_p[91+i*36];
		if(fault_time_total == 0)
			fprintf(cgiOut,"故障时间总长度：无故障 <br>");
		else{
			fault_day = fault_time_total/ (3600*24);
			fault_time_total %= (3600*24);
			fault_hour = fault_time_total / 3600;
			fault_time_total %= 3600;
			fault_min = fault_time_total / 60;
			fault_time_total %= 60;
			fprintf(cgiOut,"故障时间总长度：%d天%d时%d分%d秒 <br>",fault_day,fault_hour,fault_min,fault_time_total);
		}

		fault_time_total = map_p[98+i*36]*16777216 + map_p[97+i*36]*65536+map_p[96+i*36]*256+map_p[95+i*36];
		fault_day = fault_time_total/ (3600*24);
		fault_time_total %= (3600*24);
		fault_hour = fault_time_total / 3600;
		fault_time_total %= 3600;
		fault_min = fault_time_total / 60;
		fault_time_total %= 60;
		fprintf(cgiOut,"运行时间总长度：%d天%d时%d分%d秒 <br>",fault_day,fault_hour,fault_min,fault_time_total);

		switch(map_p[99+i*36]){
		case 0:
			fprintf(cgiOut,"设备所在方向：无 <br>");
			break;
		case 1:
			fprintf(cgiOut,"设备所在方向：东 <br>");
			break;
		case 2:
			fprintf(cgiOut,"设备所在方向：南 <br>");
			break;
		case 3:
			fprintf(cgiOut,"设备所在方向：西 <br>");
			break;
		case 4:
			fprintf(cgiOut,"设备所在方向：北 <br>");
			break;
		}
		fprintf(cgiOut,"</div>");

		fprintf(cgiOut,"<div class=\"device-img\">");//图片样式
		switch(dev_type){//最后设备类型图片
		case 0x30:{
					  if(now_state == 1)
						  fprintf(cgiOut,"<img src=\"../dev_ico/监控相机1.png\">");
					  else 
						  fprintf(cgiOut,"<img src=\"../dev_ico/监控相机2.png\" >");
					  break;
				  }
		case 0x31:{
					  if(now_state == 1)
						  fprintf(cgiOut,"<img src=\"../dev_ico/终端服务器1.png\" >");
					  else 
						  fprintf(cgiOut,"<img src=\"../dev_ico/终端服务器2.png\" >");
					  break;
				  }

		case 0x32:{
					  if(now_state == 1)
						  fprintf(cgiOut,"<img src=\"../dev_ico/补光灯1.png\" >");
					  else 
						  fprintf(cgiOut,"<img src=\"../dev_ico/补光灯2.png\" >");
					  break;
				  }
		case 0x39:{
					  if(now_state == 1)
						  fprintf(cgiOut,"<img src=\"../dev_ico/能见度1.png\" >");
					  else 
						  fprintf(cgiOut,"<img src=\"../dev_ico/能见度2.png\" >");
					  break;
				  }
		case 0x3A:{
					  if(now_state == 1)
						  fprintf(cgiOut,"<img src=\"../dev_ico/路感1.png\" >");
					  else 
						  fprintf(cgiOut,"<img src=\"../dev_ico/路感2.png\" >");
					  break;
				  }
		case 0x3B:{
					  if(now_state == 1)
						  fprintf(cgiOut,"<img src=\"../dev_ico/微型气象站1.png\" >");
					  else 
						  fprintf(cgiOut,"<img src=\"../dev_ico/微型气象站2.png\" >");
					  break;
				  }
		case 0x3C:{
					  if(now_state == 1)
						  fprintf(cgiOut,"<img src=\"../dev_ico/机柜1.png\" >");
					  else 
						  fprintf(cgiOut,"<img src=\"../dev_ico/机柜2.png\" >");
					  break;
				  }
		}
	
		fprintf(cgiOut,"</div>");
		fprintf(cgiOut,"</td>");
		if((i % 3) == 2)
			fprintf(cgiOut,"</tr>");
	}
	return 0;
}
