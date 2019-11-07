#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"
int cgiMain()
{
    cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>time_set</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	char date[20] = {0};
    char time[20] = {0};
	cgiFormString("date",date,20);
	cgiFormString("time",time,20);

	fprintf(cgiOut,"<h4>date: %s</h4>",date);
	fprintf(cgiOut,"<h4>time: %s</h4>",time);
	int n = strlen(date);
	int m = strlen(time);
    if ( n < 10 || m < 5)
      fprintf(cgiOut,"<h4>输入格式有误</h4>");
	else {
		char buf[300] = {0};
		sprintf(buf,"date -s '%s %s'",date,time);
		n = system(buf);
		n = system("hwclock --systohc");
		if (0 == n)
			fprintf(cgiOut,"<h4>设置成功</h4>");
		else
			fprintf(cgiOut,"<h4>设置失败，请与管理员联系</h4>"); 
		fprintf(cgiOut, "<br><br>");

		//	fprintf(cgiOut, "<br><a href='../time_set.html'>返回</a>");
	}
		return 0;
	
}
