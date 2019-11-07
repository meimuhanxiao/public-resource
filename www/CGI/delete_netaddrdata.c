#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"
#include "sqlite3.h"
#include "stdlib.h"
#include <stdio.h>                                                                                                                  
#include <stdlib.h>
#include <unistd.h>
int cgiMain()
{
	sqlite3 *db;
	char *errmsg = NULL;
	int recode = sqlite3_open("/run/media/mmcblk1p1/app.db",&db);
	if(recode!=SQLITE_OK)
	{
		printf("Can't Open Database:%s!\n",sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	else  	{
		printf("/run/media/mmcblk1p1/app.db open OK!\n");
	}
//	recode = sqlite3_exec(db ,"delete from netaddr", NULL , NULL , &errmsg);
	recode = sqlite3_exec(db ,"DROP TABLE netaddr;", NULL , NULL , &errmsg);
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut,"<meta charset='UTF-8'>");
	fprintf(cgiOut, "<TITLE>delete</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	if (0 == recode)
		fprintf(cgiOut,"<h3>数据已删除</h3>");
	else		
		fprintf(cgiOut,"<h3>%s</h3>",errmsg);
	sqlite3_close(db);
	return 0;
}
