#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"
#include <dirent.h>
#include <pthread.h>
//把运行文件从下载的目录移到根目录，重启板子，板子自动运行新程序
int n;
int cgiMain()
{
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>update_system</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
    DIR *dir;
	struct dirent *dirp;

	int flag = 0;
	if((dir = opendir("../updatefile")) == NULL)//打开目录，返回目录指针
	{
		printf("fail to OPEN updatefile dir");
		return -1;
	}
	if(dir != NULL)
	{
		while((dirp = readdir(dir)) != NULL)
		{
		//	fprintf(cgiOut,"%s<br>",dirp->d_name);
			if(strcmp(dirp->d_name ,".") == 0) continue;
			if (strcmp(dirp->d_name,"aaa") == 0){
				system("mv ../updatefile/aaa /");
				flag = 1;
            } 
			if (strcmp(dirp->d_name,"www.tar") == 0){
				system("tar xvf ../updatefile/www.tar -C ../updatefile/" );
				//	system("mv ../updatefile/www /etc -r");//更新web
                flag = 1;
			}
		}
	}
	if (0 == flag)
		fprintf(cgiOut,"<h4>没有升级文件</h4>");
	else if (1 == flag){
        system("reboot");
	}
		return 0;
}

