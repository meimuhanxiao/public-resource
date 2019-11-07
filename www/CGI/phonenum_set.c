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
	fprintf(cgiOut, "<TITLE>电话号设置界面</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	
	char phonenum1[12] = {0};
	char phonenum2[12] = {0};
	char phonenum3[12] = {0};
	char phonenum4[12] = {0};
	char phonenum5[12] = {0};

	cgiFormString("phonenum1",phonenum1,12);
	cgiFormString("phonenum2",phonenum2,12);
	cgiFormString("phonenum3",phonenum3,12);
	cgiFormString("phonenum4",phonenum4,12);
	cgiFormString("phonenum5",phonenum5,12);
	FILE *fp = fopen("phonenum","w");//以只写方式打开，没有该文件则创建，如果存在文件清空定位到文件头
	if(fp == NULL)
		fprintf(cgiOut,"phonenum set error!<br>");
	fwrite(phonenum1,strlen(phonenum1),1,fp);
	fwrite("\n",1,1,fp);

	fwrite(phonenum2,strlen(phonenum2),1,fp);
	fwrite("\n",1,1,fp);

	fwrite(phonenum3,strlen(phonenum3),1,fp);
	fwrite("\n",1,1,fp);

	fwrite(phonenum4,strlen(phonenum4),1,fp);
	fwrite("\n",1,1,fp);

	fwrite(phonenum5,strlen(phonenum5),1,fp);
	fwrite("\n",1,1,fp);

	fprintf(cgiOut,"电话号码1: <b>%s<br></b>",phonenum1);
	fprintf(cgiOut,"<br>");

	fprintf(cgiOut,"电话号码2: <b>%s<br></b>",phonenum2);
	fprintf(cgiOut,"<br>");

	fprintf(cgiOut,"电话号码3: <b>%s<br></b>",phonenum3);
	fprintf(cgiOut,"<br>");

	fprintf(cgiOut,"电话号码4: <b>%s<br></b>",phonenum4);
	fprintf(cgiOut,"<br>");

	fprintf(cgiOut,"电话号码5: <b>%s<br></b>",phonenum5);

	fprintf(cgiOut,"<br><a href=\"phonenum_show.cgi\"><b>返回</b></a>");

	fprintf(cgiOut,"</body><html>");

	fclose(fp);
	return 0;
}

