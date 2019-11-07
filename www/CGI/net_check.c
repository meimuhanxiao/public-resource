#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"

int cgiMain()
{
    FILE *stream;
	char *line = NULL;
    size_t len = 0;
//	ssize_t read;
	cgiHeaderContentType("text/html");	
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>net_check</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	system("ifconfig eth0 > net_set/netpar.txt") ;
	stream = fopen("net_set/netpar.txt","r");
	if (stream == NULL)
		fprintf(cgiOut,"open netpar.txt error\n");
    getline(&line, &len, stream);//line 指向读出一行的字符串 
	
	getline(&line, &len, stream);//要第二行的字符串  第二行的是ip地址
	char s1[100] = {0};
	char s2[100] = {0};
	char s3[100] = {0};
	char s4[100] = {0};
	sscanf(line,"%s %s %s %s",s1,s2,s3,s4);
	//s1 = inet s2 = addr:192.168.2.175 s3 = Bcast:192.168.2.255 s4 = Mask:255.255.255.0 
	fprintf(cgiOut, "<h4>Ip:%s</h4><p></p>",s2+5);//读取ip
	fprintf(cgiOut, "<h4>%s</h4><p></p>",s4);//读取ip

	free(line);
	fclose(stream);
	system("ip route show > net_set/gateway.txt") ;
	FILE *fp = fopen("net_set/gateway.txt","r");
	if(fp == NULL)
		perror("open gateway.txt");
	getline(&line,&len,fp);
	//default via 192.168.2.1 dev eth0  proto static  metric 100
	bzero(s1,100);
	bzero(s2,100);
	bzero(s3,100);
	bzero(s4,100);
	sscanf(line,"%s %s %s %s",s1,s2,s3,s4);
	/*printf("s1 = %s\n",s1);
	printf("s2 = %s\n",s2);
	printf("s3 = %s\n",s3);
	printf("s4 = %s\n",s4);*/
	fprintf(cgiOut, "<h4>Gateway:%s</h4><p></p>",s3);
	fclose(fp);
	fprintf(cgiOut, "<a href='../net_set.html'>返回</a>");
	fprintf(cgiOut, "</BODY>\n"); 
	fprintf(cgiOut, "</HTML>\n");
	return 0;
}

