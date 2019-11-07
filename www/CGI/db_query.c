#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "sqlite3.h"
#include "cgic.h"
int cgiMain()
{
	cgiHeaderContentType("text/html");	
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>net_check</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");

	char st_date[100] = {0};
	char st_time[100] = {0};
	cgiFormString("st_date",st_date,20);
	cgiFormString("st_time",st_time,20);
	strcat(st_date,"-");
	strcat(st_date,st_time);

	char end_date[100] = {0};
	char end_time[100] = {0};
	cgiFormString("end_date",end_date,20);
	cgiFormString("end_time",end_time,20);
	fprintf(cgiOut,"end_date = %s\n",end_date);
	if(strlen(end_date) < 10){//没有输入终止日期
		time_t t;
		struct tm* mydate;
		time (&t);   //获取现在的时间
		mydate = localtime (&t);
		sprintf(end_date,"%04d/%02d/%02d-%02d:%02d:%02d",mydate->tm_year+1900,mydate->tm_mon + 1,mydate->tm_mday,mydate->tm_hour,mydate->tm_min,mydate->tm_sec);
	}
	else{
		if(strlen(end_time) < 8){
			strcat(end_date,"-");
			strcat(end_date,"23:59:59");//选择日期没有选择时间
		}
		else{
			strcat(end_date,"-");
			strcat(end_date,end_time);
		}
	}

	char dev_id[3] = {0};
	cgiFormString("dev_id",dev_id,20);
	fprintf(cgiOut,"dev_id = %s\n",dev_id);

	char device[100] = {0};
	cgiFormString("device",device,100);
	fprintf(cgiOut,"device = %s\n",device);

	
	fprintf(cgiOut,"st_date = %s\n",st_date);
	fprintf(cgiOut,"end_date = %s\n",end_date);

	int res = system("cp /home/linux/www/html_data/basic.html /home/linux/www/html_data/data.html");
	                                                //where 事件时间 > %s and 事件时间 < %s
	char buf[1024] = {0};
	if(strlen(dev_id) == 0){//没有输入设备id
		if(strcmp(device,"所有设备") == 0)
			sprintf(buf,"sqlite3 -html /home/linux/www/db/event_show.db \"select * from event_table where 事件时间 > '%s' and 事件时间 < '%s';\" >> /home/linux/www/html_data/data.html",st_date,end_date);
		else
			sprintf(buf,"sqlite3 -html /home/linux/www/db/event_show.db \"select * from event_table where 事件时间 > '%s' and 事件时间 < '%s' and 事件对象 = '%s';\" >> /home/linux/www/html_data/data.html",st_date,end_date,device);
	}
	else{//输入了设备ID
		char dev_id_8bit[9] = {0};
		sprintf(dev_id_8bit,"%08d",atoi(dev_id));//8位设备id
		sprintf(buf,"sqlite3 -html /home/linux/www/db/event_show.db \"select * from event_table where 事件时间 > '%s' and 事件时间 < '%s' and 设备ID = '%s';\" >> /home/linux/www/html_data/data.html",st_date,end_date,dev_id);
	}
	res = system(buf);
	//html 显示返回
	system("echo \"</tbody></table> <br> <a href=\"../his_data_query.html\"><b>返回</b></a>\" >> /home/linux/www/html_data/data.html");
	//script 加入到网页中
	system("cat /home/linux/www/html_data/script.html >>  /home/linux/www/html_data/data.html");
	//输出html
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"0;url=../html_data/data.html\">");//跳转到data.html页面 data.html 有查询数据的结果
    
	return 0;
}


