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
#define FAULT_DIR   "/home/linux/work/prog/public_resource/fault"
int cgiMain()
{
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>dev_set</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	char dev_id[3] = {0};
	char dev_type[20] = {0};
	char dev_dir[4] = {0};
	char dev_ip[20] = {0};	
	FILE *fp;
	cgiFormString("dev_id",dev_id,3);
	cgiFormString("dev_type",dev_type,20);
	cgiFormString("dev_dir",dev_dir,4);
	cgiFormString("dev_ip",dev_ip,20);
	char filename[100] = {0};
	sprintf(filename,"dev_info/dev_%s",dev_id);
	fp = fopen(filename,"w");
	if(fp == NULL){
		fprintf(cgiOut,"设置外接设备错误");
		return 0;
	}
	fwrite(dev_id,strlen(dev_id),1,fp);
	fwrite("\n",1,1,fp);

	fwrite(dev_type,strlen(dev_type),1,fp);
	fwrite("\n",1,1,fp);

	fwrite(dev_dir,strlen(dev_dir),1,fp);
	fwrite("\n",1,1,fp);

	fwrite(dev_ip,strlen(dev_ip),1,fp);
	fwrite("\n",1,1,fp);

	time_t t;
	time(&t);//现在时间
	struct tm* mydate;
	mydate = localtime (&t);
	char now_time[15] = {0};
	sprintf(now_time,"%d",(uint32_t)t);//整形转成字符串
	fwrite(now_time,strlen(now_time),1,fp);//写到文件里
	fwrite("\n",1,1,fp);

	fprintf(cgiOut,"<h4>设置设备信息：</h4><br>");

	fprintf(cgiOut,"<br>设备 ID&nbsp&nbsp: <b>%s</b><br> ",dev_id);
	fprintf(cgiOut,"<br>设备类型: <b>%s</b><br> ",dev_type);
	fprintf(cgiOut,"<br>设备方向: <b>%s</b><br> ",dev_dir);
	fprintf(cgiOut,"<br>设备 IP&nbsp&nbsp: <b>%s</b><br> ",dev_ip);
	fprintf(cgiOut,"<br>设备接入时间： <b>%04d/%02d/%02d-%02d:%02d:%02d</b><br> ",mydate->tm_year+1900,mydate->tm_mon+1,mydate->tm_mday,mydate->tm_hour,mydate->tm_min,mydate->tm_sec);

//	fprintf(cgiOut,"ip 长度 %d,ip[0]=%d",strlen(dev_ip),dev_ip[0]);

	fclose(fp);
	bzero(filename,100);
	sprintf(filename,"chmod 666 dev_info/dev_%s",dev_id);
	system(filename);
	sprintf(filename,"dev_info/devstatus_%s",dev_id);//创建设备状态文件
	fp = fopen(filename,"w");
	if(fp == NULL){
		fprintf(cgiOut,"设置外接设备错误");
		return 0;
	}
	fwrite("1",1,1,fp);//第一行 设备状态 1 默认 在线
	fwrite("\n",1,1,fp);

	fwrite("0",1,1,fp);//第二行  故障次数 默认0次
	fwrite("\n",1,1,fp);

	fwrite("0",1,1,fp);//第三行   最新故障类型 默认没有
	fwrite("\n",1,1,fp);

	fclose(fp);
	bzero(filename,100);
	sprintf(filename,"chmod 666 dev_info/devstatus_%s",dev_id);
	system(filename);

	bzero(filename,100);
	//新建一个设备 没有故障 把故障文件删除
	sprintf(filename,"rm %s/fault_%s",FAULT_DIR,dev_id);
	system(filename);
	
	fprintf(cgiOut,"<br><a href=\"../dev_set.html\"><b>返回</b></a>");

	fprintf(cgiOut,"</body><html>");

	return 0;
}

