#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"
#include "sqlite3.h"
int cgiMain()
{
	system("sqlite3 -header -html /run/media/mmcblk1p1/app.db 'select * from netaddr;' > /www/html_data/raw_netaddr.html");
	system("cp /www/html_data/basic.html /www/html_data/netaddrdata.html");
	system("cat /www/html_data/raw_netaddr.html >> /www/html_data/netaddrdata.html");
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>query</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"0;url=../html_data/netaddrdata.html\">");
    return 0;
}
