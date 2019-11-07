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
#include "cgic.h"
int cgiMain()
{
	cgiHeaderContentType("text/html");	
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>设备功能设置界面</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	fprintf(cgiOut, "<BODY>");
	FILE *fp = NULL;
	char *line = NULL;
	size_t len = 0;
	fp = fopen("dev_func/dev_func.txt","r");
	if(fp == NULL)
		fprintf(cgiOut,"dev_set error\n");
	getline(&line, &len, fp);//
	uint8_t func = (uint8_t)atoi(line);//得到低8位
	getline(&line, &len, fp);
	uint8_t func_2 = (uint8_t)atoi(line);//8-15位
	getline(&line, &len, fp);//风扇温度
	char fan_temp[10] = {0};
	strcpy(fan_temp,line);
	getline(&line, &len, fp);//加热器温度
	char heat_temp[10] = {0};
	strcpy(heat_temp,line);

	getline(&line, &len, fp);//第一路电源检测方向
	char dir_1[5] = {0};
	strcpy(dir_1,line);

	getline(&line, &len, fp);//第2路电源检测方向
	char dir_2[5] = {0};
	strcpy(dir_2,line);

	getline(&line, &len, fp);//第3路电源检测方向
	char dir_3[5] = {0};
	strcpy(dir_3,line);

	getline(&line, &len, fp);//第4路电源检测方向
	char dir_4[5] = {0};
	strcpy(dir_4,line);
	fclose(fp);

	fprintf(cgiOut,"<FORM action=\"dev_func.cgi\" method=\"post\">");
	fprintf(cgiOut,"<h4>四路电源检测方向勿重复</h4>");
	fprintf(cgiOut,"第一路电源检测:");
	if(func & 0x01){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_1\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_1\" value=\"0\" >关闭  &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}
	else{//第一路电源检测关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_1\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_1\" value=\"0\" checked=\"checked\">关闭  &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}
	fprintf(cgiOut,"第一路电源检测方向:");
	fprintf(cgiOut,"<SELECT name=\"adc_check_1_dir\">");
	if(!strcmp(dir_1,"1\n"))//dir_1 东
		fprintf(cgiOut,"<option value=\"东\"selected = \"selected\">东</option>");
	else	
		fprintf(cgiOut,"<option value=\"东\">东</option>");

	if(!strcmp(dir_1,"2\n"))//dir_1 南
		fprintf(cgiOut,"<option value=\"南\"selected = \"selected\">南</option>");
	else	
		fprintf(cgiOut,"<option value=\"南\">南</option>");

	if(!strcmp(dir_1,"3\n"))//dir_1 西
		fprintf(cgiOut,"<option value=\"西\"selected = \"selected\">西</option>");
	else	
		fprintf(cgiOut,"<option value=\"西\">西</option>");

	if(!strcmp(dir_1,"4\n"))//dir_1 北
		fprintf(cgiOut,"<option value=\"北\"selected = \"selected\">北</option>");
	else	
		fprintf(cgiOut,"<option value=\"北\">北</option>");
	if(!strcmp(dir_1,"0\n"))//dir_1 无
		fprintf(cgiOut,"<option value=\"无\"selected = \"selected\">无</option>");
	else	
		fprintf(cgiOut,"<option value=\"无\">无</option>");

	fprintf(cgiOut,"</SELECT><p></p>");


	fprintf(cgiOut,"第二路电源检测:");
	if(func & 0x02){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_2\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_2\" value=\"0\" >关闭  &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}
	else{//第2路电源检测关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_2\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_2\" value=\"0\" checked=\"checked\">关闭  &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}
	fprintf(cgiOut,"第二路电源检测方向:");
	fprintf(cgiOut,"<SELECT name=\"adc_check_2_dir\">");
	if(!strcmp(dir_2,"1\n"))//dir_2 东
		fprintf(cgiOut,"<option value=\"东\"selected = \"selected\">东</option>");
	else	
		fprintf(cgiOut,"<option value=\"东\">东</option>");

	if(!strcmp(dir_2,"2\n"))//dir_2 南
		fprintf(cgiOut,"<option value=\"南\"selected = \"selected\">南</option>");
	else	
		fprintf(cgiOut,"<option value=\"南\">南</option>");

	if(!strcmp(dir_2,"3\n"))//dir_2 西
		fprintf(cgiOut,"<option value=\"西\"selected = \"selected\">西</option>");
	else	
		fprintf(cgiOut,"<option value=\"西\">西</option>");

	if(!strcmp(dir_2,"4\n"))//dir_2 北
		fprintf(cgiOut,"<option value=\"北\"selected = \"selected\">北</option>");
	else	
		fprintf(cgiOut,"<option value=\"北\">北</option>");

	if(!strcmp(dir_2,"0\n"))//dir_2 无
		fprintf(cgiOut,"<option value=\"无\"selected = \"selected\">无</option>");
	else	
		fprintf(cgiOut,"<option value=\"无\">无</option>");

	fprintf(cgiOut,"</SELECT><p></p>");


	fprintf(cgiOut,"第三路电源检测:");
	if(func & 0x04){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_3\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_3\" value=\"0\" >关闭  &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}
	else{//第3路电源检测关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_3\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_3\" value=\"0\" checked=\"checked\">关闭  &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}
	fprintf(cgiOut,"第三路电源检测方向:");
	fprintf(cgiOut,"<SELECT name=\"adc_check_3_dir\">");
	if(!strcmp(dir_3,"1\n"))//dir_3 东
		fprintf(cgiOut,"<option value=\"东\"selected = \"selected\">东</option>");
	else	
		fprintf(cgiOut,"<option value=\"东\">东</option>");

	if(!strcmp(dir_3,"2\n"))//dir_3 南
		fprintf(cgiOut,"<option value=\"南\"selected = \"selected\">南</option>");
	else	
		fprintf(cgiOut,"<option value=\"南\">南</option>");

	if(!strcmp(dir_3,"3\n"))//dir_3 西
		fprintf(cgiOut,"<option value=\"西\"selected = \"selected\">西</option>");
	else	
		fprintf(cgiOut,"<option value=\"西\">西</option>");

	if(!strcmp(dir_3,"4\n"))//dir_3 北
		fprintf(cgiOut,"<option value=\"北\"selected = \"selected\">北</option>");
	else	
		fprintf(cgiOut,"<option value=\"北\">北</option>");

	if(!strcmp(dir_3,"0\n"))//dir_3 无
		fprintf(cgiOut,"<option value=\"无\"selected = \"selected\">无</option>");
	else	
		fprintf(cgiOut,"<option value=\"无\">无</option>");

	fprintf(cgiOut,"</SELECT><p></p>");

	fprintf(cgiOut,"第四路电源检测:");
	if(func & 0x08){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_4\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_4\" value=\"0\" >关闭  &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}
	else{//第4路电源检测关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_4\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"adc_check_4\" value=\"0\" checked=\"checked\">关闭  &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}
	fprintf(cgiOut,"第四路电源检测方向:");
	fprintf(cgiOut,"<SELECT name=\"adc_check_4_dir\">");
	if(!strcmp(dir_4,"1\n"))//dir_4 东
		fprintf(cgiOut,"<option value=\"东\"selected = \"selected\">东</option>");
	else	
		fprintf(cgiOut,"<option value=\"东\">东</option>");

	if(!strcmp(dir_4,"2\n"))//dir_4 南
		fprintf(cgiOut,"<option value=\"南\"selected = \"selected\">南</option>");
	else	
		fprintf(cgiOut,"<option value=\"南\">南</option>");

	if(!strcmp(dir_4,"3\n"))//dir_4 西
		fprintf(cgiOut,"<option value=\"西\"selected = \"selected\">西</option>");
	else	
		fprintf(cgiOut,"<option value=\"西\">西</option>");

	if(!strcmp(dir_4,"4\n"))//dir_4 北
		fprintf(cgiOut,"<option value=\"北\"selected = \"selected\">北</option>");
	else	
		fprintf(cgiOut,"<option value=\"北\">北</option>");

	if(!strcmp(dir_4,"0\n"))//dir_4 无
		fprintf(cgiOut,"<option value=\"无\"selected = \"selected\">无</option>");
	else	
		fprintf(cgiOut,"<option value=\"无\">无</option>");

	fprintf(cgiOut,"</SELECT><p></p>");

	fprintf(cgiOut,"机柜前门检测:");
	if(func & 0x10){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"front_door\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"front_door\" value=\"0\" >关闭 &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}
	else{//关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"front_door\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"front_door\" value=\"0\" checked=\"checked\">关闭 &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}

	fprintf(cgiOut,"机柜后门检测:");
	if(func & 0x20){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"back_door\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"back_door\" value=\"0\" >关闭<p></p>");
	}
	else{//关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"back_door\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"back_door\" value=\"0\" checked=\"checked\">关闭<p></p>");
	}

	fprintf(cgiOut,"终端服务器220V检测:");
	if(func & 0x40){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"ter_ser_220v_check\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"ter_ser_220v_check\" value=\"0\" >关闭&nbsp&nbsp");
	}
	else{//关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"ter_ser_220v_check\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"ter_ser_220v_check\" value=\"0\" checked=\"checked\">关闭&nbsp&nbsp&nbsp");
	}

	fprintf(cgiOut,"机柜涉水检测:");
	if(func & 0x80){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_water\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_water\" value=\"0\" >关闭<p></p>");
	}
	else{//关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_water\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_water\" value=\"0\" checked=\"checked\">关闭<p></p>");
	}


	fprintf(cgiOut,"机柜撞击检测:");
	if(func_2 & 0x01){
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_hit\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_hit\" value=\"0\" >关闭  &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");	
	}
	else{
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_hit\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_hit\" value=\"0\" checked=\"checked\">关闭  &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");	
	}

	fprintf(cgiOut,"机柜风扇检测:");
	if(func_2 & 0x02){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_fan\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_fan\" value=\"0\" >关闭<p></p>");
	}
	else{//关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_fan\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_fan\" value=\"0\" checked=\"checked\">关闭<p></p>");
	}

	fprintf(cgiOut,"机柜加热器检测:");
	if(func_2 & 0x04){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_heat\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_heat\" value=\"0\" >关闭   &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}
	else{//关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_heat\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"cabinet_heat\" value=\"0\" checked=\"checked\">关闭    &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}

	fprintf(cgiOut,"拨打电话是否开启:");
	if(func_2 & 0x08){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"phone_func\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"phone_func\" value=\"0\" >关闭<p></p>");
	}
	else{//关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"phone_func\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"phone_func\" value=\"0\" checked=\"checked\">关闭<p></p>");
	}

	fprintf(cgiOut,"气象站是否开启:");
	if(func_2 & 0x10){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"weather_station\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"weather_station\" value=\"0\" >关闭  &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}
	else{//关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"weather_station\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"weather_station\" value=\"0\" checked=\"checked\">关闭  &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp");
	}

	fprintf(cgiOut,"能见度是否开启:");
	if(func_2 & 0x20){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"visibility\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"visibility\" value=\"0\" >关闭<p></p>");
	}
	else{//关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"visibility\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"visibility\" value=\"0\" checked=\"checked\">关闭<p></p>");
	}

	fprintf(cgiOut,"路感是否开启:");
	if(func_2 & 0x40){//打开的
		fprintf(cgiOut,"<input type=\"radio\" name=\"road_sense\" value=\"1\"checked=\"checked\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"road_sense\" value=\"0\" >关闭<p></p>");
	}
	else{//关闭的	
		fprintf(cgiOut,"<input type=\"radio\" name=\"road_sense\" value=\"1\"> 开启 ");
		fprintf(cgiOut,"<input type=\"radio\" name=\"road_sense\" value=\"0\" checked=\"checked\">关闭<p></p>");
	}

	fprintf(cgiOut,"风扇启动温度设定：");
	fprintf(cgiOut," <input type=\"TEXT\" name=fan_rotate_temp value=\"%s\" size=\"1\"> &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp",fan_temp);

	fprintf(cgiOut,"加热器启动温度设定：");
	fprintf(cgiOut," <input type=\"TEXT\" name=heater_heat_temp value=\"%s\" size=\"1\"><p></p>",heat_temp);

	fprintf(cgiOut,"<INPUT type=\"SUBMIT\" value=\"设置\">");
	fprintf(cgiOut, "</FORM></BODY></html>");
	return 0;
}
