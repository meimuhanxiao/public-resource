#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "cgic.h"
int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio,oldtio;
	if  (tcgetattr(fd,&oldtio)  !=  0) 
	{ 
		perror("SetupSerial 1");
		return -1;
	}
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag  |=  CLOCAL | CREAD; 
	newtio.c_cflag &= ~CSIZE; 

	switch( nBits )
	{
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |= CS8;
		break;
	}

	switch( nEvent )
	{
	case 'O':                     //奇校验
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E':                     //偶校验
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N':                    //无校验
		newtio.c_cflag &= ~PARENB;
		break;
	}

	switch( nSpeed )
	{
	case 300:
		cfsetispeed(&newtio, B300);
		cfsetospeed(&newtio, B300);
		break;
	case 600:
		cfsetispeed(&newtio, B600);
		cfsetospeed(&newtio, B600);
		break;
	case 1200:
		cfsetispeed(&newtio, B1200);
		cfsetospeed(&newtio, B1200);
		break;
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
		break;
	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	case 19200:
		cfsetispeed(&newtio, B19200);
		cfsetospeed(&newtio, B19200);
		break;       
	case 38400:
		cfsetispeed(&newtio, B38400);
		cfsetospeed(&newtio, B38400);
		break;  
	case 57600:
		cfsetispeed(&newtio, B57600);
		cfsetospeed(&newtio, B57600);
		break;      
	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	default:
		printf("sorry! [%d bps] is not support ,Set default 9600bps now!\n",nSpeed);
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	}
	if( nStop == 1 )
	{
		newtio.c_cflag &=  ~CSTOPB;
	}
	else if ( nStop == 2 )
	{
		newtio.c_cflag |=  CSTOPB;
	}
	newtio.c_cc[VTIME]  = 0;
	newtio.c_cc[VMIN] = 0;
	tcflush(fd,TCIFLUSH);
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		perror("com set error");
		return -1;
	}
	//printf("set done!\n");
	return 0;
}
int cgiMain()
{
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>com_set</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	char databit[2] = {0};
	char paritybit[2] = {0};
	char stopbit[2] = {0};
	char com_num[10] = {0};
	char baudrate[10] = {0}; 
	int fd;
    cgiFormString("com_num",com_num,10);
	if (strcmp(com_num,"com1") == 0 )
         fd = open("/dev/ttySAC0", O_RDWR|O_NOCTTY);
    
	else if (strcmp(com_num,"com2") == 0 )
		 fd = open("/dev/ttySAC1", O_RDWR|O_NOCTTY);
	if (fd < 0)
      fprintf(cgiOut,"<h4>不能设置%s,检查该串口是否备占用</h4>",com_num);
	cgiFormString("databit",databit,2);
	cgiFormString("paritybit",paritybit,2);
	cgiFormString("stopbit",stopbit,2);
	cgiFormString("baudrate",baudrate,10);
	fprintf(cgiOut,"<h4>%s</h4>",com_num);
	fprintf(cgiOut, "<h4>数据位:%s  奇偶校验位:%s  停止位:%s </H4>",databit,paritybit,stopbit);
	fprintf(cgiOut, "<h4>波特率:%s </H4>",baudrate);
	int res;
	res=set_opt(fd,atoi(baudrate),atoi(databit),paritybit[0],atoi(stopbit));
   // fprintf(cgiOut,"<h4>fd %d baudrate %d databit%d paritybit%c stopbit%d</h4>",fd,atoi(baudrate),atoi(databit),paritybit[0],atoi(stopbit));
	if (res == 0)
	  fprintf(cgiOut, "<h4>%s设置成功</H4>",com_num);
	if (res == -1)
      fprintf(cgiOut, "<h4>%s设置失败</H4>",com_num);
//	fprintf(cgiOut, "<a href='../com_set.html'>返回</a>");
	fprintf(cgiOut, "</BODY>\n"); 
	fprintf(cgiOut, "</HTML>\n");
	return 0;
}



