#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"
#include "sqlite3.h"
#include "stdlib.h"
#include <stdio.h>                                                                                                                  
#include <stdlib.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <stdint.h>

#define KB 1024.0// 2^10
#define MB 1048576.0// 2^20 
#define GB 1073741824.0 
//app.db数据库中的 netaddr表单 转换成 csv格式文件 //网页自动提示下载该csv文件
int cgiMain()
{	
	int res = system("sqlite3 -header -csv /home/linux/www/db/event_show.db 'select * from event_table;' > /home/linux/www/csv_data/dbdownload.csv");
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut,"<meta charset='UTF-8'>");
	fprintf(cgiOut, "<TITLE>数据下载</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	/*struct statfs diskInfo;  
	statfs("/run/media/mmcblk1p1", &diskInfo);  
	uint64_t blocksize = diskInfo.f_bsize;  // 每个block里包含的字节数
	uint64_t availableDisk = diskInfo.f_bavail * blocksize; // 可用空间大小

    fprintf(cgiOut,"<h3>剩余存储空间 %.4f MB</h3><br>",availableDisk / MB);
    fprintf(cgiOut,"<h3>删除数据（确保数据已保存后再删除数据，数据一旦删除无法恢复!）</h3>");
	fprintf(cgiOut, "<a href='delete_netaddrdata.cgi'>删除数据</a>");*/

	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"0;url=../csv_data/dbdownload.csv\">");//网页跳转到这个.csv
	return 0;
}
