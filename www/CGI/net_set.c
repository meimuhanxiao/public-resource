#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"
#include <sys/stat.h>
#include <fcntl.h>
int cgiMain()
{
    cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>net_set</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	char ip[20] = {0};
    char netmask[20] = {0};
    char gateway[20] = {0};
	cgiFormString("netmask",netmask,20);
	cgiFormString("gateway",gateway,20);
	cgiFormString("ip",ip,20);
	fprintf(cgiOut,"<h4>ip:%s</h4>",ip);
	fprintf(cgiOut,"<h4>netmask:%s</h4>",netmask);
	fprintf(cgiOut,"<h4>gateway:%s</h4><br>",gateway);

	if (strlen(ip) < 11 || strlen(gateway) < 11)
		fprintf(cgiOut,"<h4>输入格式有误</h4><br>");
	else {
		int fd = open("net_set/net_info",O_RDWR|O_CREAT|O_TRUNC,0666);
		if (fd < 0)
			printf("open net_info failed\n");
		char buf[1024] = {0};
		sprintf(buf,"address %s\ngateway %s\nnetmask %s\n",ip,gateway,netmask);
		
		write(fd, buf, strlen(buf));
		close(fd);
		int n = system("cp net_set/basic_net  net_set/interfaces");
		n = system("cat net_set/net_info >> net_set/interfaces");
	//	n = system("rm net_info");
		n = system("cp net_set/interfaces /etc/network/interfaces");
		//	n = system("/etc/init.d/networking restart");
		if ( n == 0){
			fprintf(cgiOut,"<h4>设置ip成功</h4><br>");
			//	fprintf(cgiOut,"<h4>请重启设备</h4><br>");
		}
		bzero(buf,1024);
		sprintf(buf,"ifconfig eth0 %s netmask %s",ip,netmask);
		n = system(buf);

		bzero(buf,1024);
		strcpy(buf,"route add default gw ");
		strcat(buf,gateway);
		n = system(buf);
		system("echo \"nameserver 114.114.114.114 > /etc/resolv.conf\"");
		//	fprintf(cgiOut, "<a href='../com_set.html'>返回</a>");
	}
	return 0;

}
