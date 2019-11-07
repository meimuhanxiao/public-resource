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
int cgiMain()
{
	cgiHeaderContentType("text/html");	
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>相机查询界面</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");

	FILE *fp = NULL;
	char *line = NULL;
	size_t len = 0;
	char cam_num[3] = {0};
	char cam_ip[1024] = {0};
	char cam_id[100] = {0};
	char cam_des[1024] = {0};
	
	fp = fopen("cam_info","r");
	if (fp == NULL){//没有phonenum文件
		fprintf(cgiOut,"没有配置过相机信息");
	}
	else{
		int n = getline(&line, &len, fp);//这行没有数据返回-1,系统分配len 120个字节，不够自动扩大字节数
		strcpy(cam_num,line);

		n = getline(&line, &len, fp);	
		strcpy(cam_ip,line);

		n = getline(&line, &len, fp);	
		strcpy(cam_id,line);

		n = getline(&line, &len, fp);	
		strcpy(cam_des,line);

	}

	char *s = strdup(cam_ip);
	char *ss = strdup(cam_id);
	int came_num = atoi(cam_num);
	char *ip[came_num];
	char *id[came_num];

	int i = 0;
	for(i = 0;i < came_num;i++){
		ip[i] = strsep(&s,",");
		fprintf(cgiOut,"第%d个相机ip = %s<br>",i+1,ip[i]);
	}

	for(i = 0;i < came_num;i++){
		id[i] = strsep(&ss,",");
		fprintf(cgiOut,"第%d个相机id = %s<br>",i+1,id[i]);
	}
	fprintf(cgiOut,"相机描述:%s<br>",cam_des);

	free(s);
	free(ss);
	free(line);
	fclose(fp);
	fprintf(cgiOut,"</body><html>");
	fprintf(cgiOut,"<br><a href=\"../cam_set.html\"><b>返回</b></a>");
	return 0;
}

