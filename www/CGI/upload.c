#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cgic.h"
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
int cgiMain()
{

	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");
	fprintf(cgiOut, "<TITLE>上传文件</TITLE></HEAD>\n");
    
	fprintf(cgiOut, "<BODY>");
	cgiFilePtr file;
	int fd;
//	mode_t mode;
	char name[1024];
	char contentType[1024];
	char buffer[1024];
	int size;
	int got;
	//上传文件名存到name 里 . "upfilename" 代表上传的文件
	if (cgiFormFileName("upfilename", name, sizeof(name)) != cgiFormSuccess) {
		printf("<p>No file was uploaded.<p>\n");
		return 0;
	} 
	fprintf(cgiOut, "文件名: ");
	cgiHtmlEscape(name);// 输出文件名
	fprintf(cgiOut, "<p>\n");
	cgiFormFileSize("upfilename", &size);//得到分拣大小 存到 size里
	fprintf(cgiOut, "文件大小: %d bytes<p>\n", size);
	cgiFormFileContentType("upfilename", contentType, sizeof(contentType));//得到文件类型
	fprintf(cgiOut, "文件类型: ");
	cgiHtmlEscape(contentType);//输出文件类型
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "文件已上传<p>\n");
	if (cgiFormFileOpen("upfilename", &file) != cgiFormSuccess) {//打开文件
		fprintf(cgiOut, "Could not open the file.<p>\n");
		return 0;
	}

//	mode=S_IRWXU|S_IRGRP|S_IROTH; 
    char name1[100] = {0};
	strcpy(name1,name);
    strcpy(name,"../updatefile/");//放到这个目录下
	strcat(name,name1);
	fd = open(name,O_RDWR|O_CREAT|O_TRUNC,0777);
	if(fd < 0){
		fprintf(cgiOut, "<p>Could not create the file,error:%d<p>\n",fd);
	}
    //fprintf(cgiOut, "<pre>\n");
	//读取上传文件内容
	while (cgiFormFileRead(file, buffer, sizeof(buffer), &got) ==
			cgiFormSuccess)
	{   
			if(got>0)
				write(fd,buffer,got);//上传文件写到 updatefile/ 文件下

	//	cgiHtmlEscapeData(buffer,got);
	}
	cgiFormFileClose(file);
	close(fd);
	sprintf(name1,"chmod 777 %s",name);
	system(name1);//修改文件权限
	return 0;
}
