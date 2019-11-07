#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <stdint.h>
#include <unistd.h>
int main(int argc, char **argv)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == fd)
	{
		perror("socket");
		return -1;
	}

	struct sockaddr_in serveraddr,clientaddr = {0};
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("192.168.2.175");
	serveraddr.sin_port = htons(8000);
	int len = sizeof(serveraddr);
	if(-1 == bind(fd, (struct sockaddr*)&serveraddr, len) )
	{
		perror("connect");
		return -1;
	}

	listen(fd, 10);
	printf("listen...................\n");
	socklen_t clientaddr_len = sizeof(clientaddr);
	uint8_t buf[1024] = {0};
	int newsd;
	char ipstr[100] = {0};
lab:
	printf("wait client connect\n");
	 newsd = accept(fd,(void *)&clientaddr,&clientaddr_len);
	
	inet_ntop(AF_INET,&clientaddr.sin_addr,ipstr,100);
	printf("client:%s:%d\n",ipstr,ntohs(clientaddr.sin_port));

	while(1){
		bzero(buf,1024);
		int n = read(newsd,buf,1024);
		if(n <= 0){
			goto lab;
		}
		int i;
		for(i=0;i < n; i++){
			printf("0x%02x ",buf[i]);
		}
		puts("");

		uint8_t event_right_response_data[15] = {
			0xFE,0xAA,0x02,0x01,0x00,0x02,0x00,0x0D,0x00,0xC8,0x70,
			0x00,0x00,0x2B,0x4F
		};

		write(newsd,event_right_response_data,15);
	}
}
