#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"
#include <dirent.h>
#include <net/if.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <linux/can.h> 
#include <linux/can/raw.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int cgiMain()
{
	unsigned char buf[2048] = {};//存储文件字节用于计算crc
	int can_fd;  
	struct sockaddr_can addr;  
	struct ifreq ifr;  
	struct can_frame frame = {0};  
	frame.can_id = 0x11;
	can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);      //创建套接字  
	strcpy(ifr.ifr_name, "can0" );  
	ioctl(can_fd, SIOCGIFINDEX, &ifr);                   //指定can0设备  
	addr.can_family = AF_CAN;  
	addr.can_ifindex = ifr.ifr_ifindex;  
	bind(can_fd, (struct sockaddr *)&addr, sizeof(addr));      

	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>update_stm32</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
    DIR *dir;
	struct dirent *dirp;

	int flag = 0;
	if((dir = opendir("../updatefile")) == NULL)//打开目录，返回目录指针
	{
		printf("fail to OPEN updatefile dir");
		return -1;
	}
	if(dir != NULL)
	{
		while((dirp = readdir(dir)) != NULL)
		{
			if(strcmp(dirp->d_name ,".") == 0) continue;
			if (strcmp(dirp->d_name,"stm.zip") == 0){
				system("unzip -o -d ../updatefile/stm/  ../updatefile/stm.zip");
				//	system("mv ../updatefile/www /etc -r");//更新web
				flag = 1;
				int stm_fd = open("../updatefile/stm.zip",O_RDONLY);
				int n,total = 0;
				while((n = read(stm_fd,frame.data,8))!= 0){
					int i;
					for(i = 0;i<n;i++)
						buf[total+i] = frame.data[i];//读到数据存到buf里用于crc校验
					total += n;
					frame.can_dlc = n;
					write(can_fd,&frame,sizeof(frame));//文件从can发出去
					//write two bytes crc 
				}
				close(stm_fd);
			}
		}
	}
	if (0 == flag)
		fprintf(cgiOut,"<h4>没有STM32升级程序</h4>");
	close(can_fd);
	return 0;
}

