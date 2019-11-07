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
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>time_check</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	time_t t;
	struct tm* mydate;
	time (&t);
	mydate = localtime (&t);
	fprintf(cgiOut,"<h4>现在设备时间为:</h4><p></p>");
	fprintf(cgiOut,"<h4>日期: %04d-%02d-%02d<h4><p></p>",mydate->tm_year+1900,mydate->tm_mon + 1,mydate->tm_mday);
	fprintf(cgiOut,"<h4>时间: %02d:%02d:%02d<h4><br>",mydate->tm_hour,mydate->tm_min,mydate->tm_sec );
	//system("date +%Y-%m-%d__%H:%M:%S");
//	fprintf(cgiOut, "<br><a href='../com_set.html'>返回</a>");
	return 0;
}
