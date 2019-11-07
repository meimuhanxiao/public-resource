#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"
#include <time.h>

int cgiMain()
{
	time_t t;
	struct tm* mydate;
	time (&t);
	mydate = localtime (&t);
	char date[100] = {0};
	sprintf(date,"%04d-%02d-%02d",mydate->tm_year+1900,mydate->tm_mon + 1,mydate->tm_mday);
	char time[100] = {0};
	sprintf(time,"%02d:%02d:%02d", mydate->tm_hour,mydate->tm_min,mydate->tm_sec);
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>time_show</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");

	fprintf(cgiOut,"<h3>时间设置</h3><p></p>");
	fprintf(cgiOut,"<FORM action=\"time_set.cgi\" method=\"post\" autofocus>");
	fprintf(cgiOut,"<h4>日期: <input type=\"text\" name=\"date\" value=\"%s\">",date);

	fprintf(cgiOut," </h4><p></p>");

	fprintf(cgiOut,"<h4>时间: <input type=\"text\" name=\"time\" value=\"%s\">",time);
	fprintf(cgiOut,"</h4><p></p>");
	fprintf(cgiOut,"<INPUT type=\"SUBMIT\" value=\"设置\">");

	fprintf(cgiOut,"</FORM><br>");
	fprintf(cgiOut,"<a href=\"time_check.cgi\"><h4>查询设备时间</h4> </a>");
	fprintf(cgiOut,"</BODY></HTML>");
	return 0;
}
