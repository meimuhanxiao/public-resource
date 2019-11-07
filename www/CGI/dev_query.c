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
#include <dirent.h>

int cgiMain()
{
	cgiHeaderContentType("text/html");	
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>已设置设备查询</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");

	int filenum = 0;
	DIR *dirp;
	struct dirent *dir_ent;

	FILE *fp = NULL;
	char *line = NULL;
	size_t len = 0;
	char dev_id[3] = {0};
	char dev_type[20] = {0};
	char dev_dir[4] = {0};
	char dev_ip[20] = {0};	
	char filename[20] = {0};
	//打开目录，得到目录流指针
	if((dirp = opendir("dev_info")) == NULL)
	{
		fprintf(cgiOut,"fail to opendir");
		perror("fail to opendir");
		return -1;
	}

	//读取目录信息
	while((dir_ent = readdir(dirp)) != NULL)
	{
		if((!strcmp(dir_ent->d_name,"."))||(!strcmp(dir_ent->d_name,"..")))
			continue;	
		if(strncmp(dir_ent->d_name,"dev_",4) == 0)
			filenum++;
	}
	closedir(dirp);

	fprintf(cgiOut,"<h4>已设置%d个外接设备</h4><br>",filenum);

	int i;
	for(i = 1; i<= filenum;i++){
		bzero(filename,20);
		sprintf(filename,"dev_info/dev_%d",i);
		fp = fopen(filename,"r");
		if(fp == NULL){
			fprintf(cgiOut,"open dev_file error");
			perror("open dev_file error");
			return 0;
		}
		getline(&line, &len, fp);//这行没有数据返回-1,系统分配len 120个字节，不够自动扩大字节数
		strcpy(dev_id,line);

		getline(&line, &len, fp);	
		strcpy(dev_type,line);

		getline(&line, &len, fp);	
		strcpy(dev_dir,line);

		getline(&line, &len, fp);	
		strcpy(dev_ip,line);

		fprintf(cgiOut,"<br>设备 ID：%s &nbsp&nbsp,",dev_id);
		fprintf(cgiOut,"设备类型：%s  &nbsp&nbsp,",dev_type);
		fprintf(cgiOut,"设备方向：%s  &nbsp&nbsp,",dev_dir);
		fprintf(cgiOut,"设备 IP：%s ",dev_ip);
	//	fprintf(cgiOut,"ip 长度 %d,ip[0]=%d",strlen(dev_ip),dev_ip[0]);
		fprintf(cgiOut,"<br>");

		fclose(fp);
	}

	free(line);
	fprintf(cgiOut,"<br><a href=\"../dev_set.html\"><b>返回</b></a>");

	fprintf(cgiOut,"</body></html>");

	return 0;
}
