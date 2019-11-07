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
/*
int can_creat()
{
	int can_fd;  
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
	struct can_frame frame = {0};
	frame.can_id = 10;
	frame.can_dlc = 8;
	frame.data[0] = 0xFF;
	frame.data[1] = 0xAA;
	write(can_fd,&frame,sizeof(frame));
	int count = 0,datalen = 0;;
	while(1){
		read(can_fd,&frame,sizeof(frame));
		count += frame.dlc;
		if((count-2) == )
	}
	return 0;
}*/

int cgiMain()
{

	cgiHeaderContentType("text/html");	
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>电话号码显示界面</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	FILE *fp = NULL;
	char *line = NULL;
	size_t len = 0;
	char phonenum1[12] = {0};
	char phonenum2[12] = {0};
	char phonenum3[12] = {0};
	char phonenum4[12] = {0};
	char phonenum5[12] = {0};
	
	fp = fopen("phonenum","r");
	if (fp == NULL){//没有phonenum文件
		bzero(phonenum1,12);
		bzero(phonenum2,12);
		bzero(phonenum3,12);
		bzero(phonenum4,12);
		bzero(phonenum5,12);
	}
	else{
		getline(&line, &len, fp);//这行没有数据返回-1,系统分配len 120个字节，不够自动扩大字节数
		strcpy(phonenum1,line);//如果没有数据会把换行符拷贝到字符串里

		getline(&line, &len, fp);	
		strcpy(phonenum2,line);

		getline(&line, &len, fp);	
		strcpy(phonenum3,line);

		getline(&line, &len, fp);	
		strcpy(phonenum4,line);

		getline(&line, &len, fp);	
		strcpy(phonenum5,line);
	}

	fprintf(cgiOut, "<FORM action=\"./phonenum_set.cgi\" method=\"post\" >");

	fprintf(cgiOut,"<h4>电话号码1: <input type=\"text\"name=\"phonenum1\" size=\"15\"");
	fprintf(cgiOut,"onkeyup=\"this.value=value.replace(/[^\a1234567890]/g,'');if(this.value=='')(this.value='');\" ");
	fprintf(cgiOut,"placeholder=\"11位手机号码\" maxlength=\"11\" ");
	fprintf(cgiOut,"value=\"%s\"",phonenum1);//换行符打印不出来
	fprintf(cgiOut," /></h4><p></p>");
	

	fprintf(cgiOut,"<h4>电话号码2: <input type=\"text\"name=\"phonenum2\" size=\"15\"");
	fprintf(cgiOut,"onkeyup=\"this.value=value.replace(/[^\a1234567890]/g,'');if(this.value=='')(this.value='');\" ");
	fprintf(cgiOut,"placeholder=\"11位手机号码\" maxlength=\"11\" ");
	fprintf(cgiOut,"value=\"%s\"",phonenum2);
	fprintf(cgiOut," /></h4><p></p>");

	fprintf(cgiOut,"<h4>电话号码3: <input type=\"text\"name=\"phonenum3\" size=\"15\"");
	fprintf(cgiOut,"onkeyup=\"this.value=value.replace(/[^\a1234567890]/g,'');if(this.value=='')(this.value='');\" ");
	fprintf(cgiOut,"placeholder=\"11位手机号码\" maxlength=\"11\" ");
	fprintf(cgiOut,"value=\"%s\"",phonenum3);
	fprintf(cgiOut," /></h4><p></p>");

	fprintf(cgiOut,"<h4>电话号码4: <input type=\"text\"name=\"phonenum4\" size=\"15\"");
	fprintf(cgiOut,"onkeyup=\"this.value=value.replace(/[^\a1234567890]/g,'');if(this.value=='')(this.value='');\" ");
	fprintf(cgiOut,"placeholder=\"11位手机号码\" maxlength=\"11\" ");
	fprintf(cgiOut,"value=\"%s\"",phonenum4);
	fprintf(cgiOut," /></h4><p></p>");

	fprintf(cgiOut,"<h4>电话号码5: <input type=\"text\"name=\"phonenum5\" size=\"15\"");
	fprintf(cgiOut,"onkeyup=\"this.value=value.replace(/[^\a1234567890]/g,'');if(this.value=='')(this.value='');\" ");
	fprintf(cgiOut,"placeholder=\"11位手机号码\" maxlength=\"11\" ");
	fprintf(cgiOut,"value=\"%s\"",phonenum5);
	fprintf(cgiOut," /></h4><p></p>");

	free(line);
	fclose(fp);
	
	fprintf(cgiOut,"<INPUT type=\"SUBMIT\" value=\"设置\">	</FORM><br></body><html>");

	fprintf(cgiOut,"<br>提示：输入框里有电话号码表示已存的电话号码");
	return 0;
}

