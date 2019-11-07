#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
int sockfd;
void *pthread_rcv_data(void *arg)
{
	uint8_t rcv[20] = {0};
	while(1){
		int n = read(sockfd,rcv,20);
		int i;
		printf("pthread rcv data\n");
		for(i = 0;i<n;i++){
			printf("%02x ",rcv[i]);
		}
		puts("");
	}
}
int main(int argc, const char *argv[])
{
		struct sockaddr_in  peer_addr;
	socklen_t addrlen;
	uint8_t buff[20] = {0xff,0xAA,0xAA};
	int ret;
	pthread_t pthread_id;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd){
		perror("socket");
		return -1;
	} 

	peer_addr.sin_family = AF_INET;
	peer_addr.sin_port = htons(8000);
	peer_addr.sin_addr.s_addr = inet_addr("192.168.2.175");
	addrlen = sizeof(peer_addr);
	if (-1 == connect(sockfd, (struct sockaddr *)&peer_addr, addrlen)){
		perror("connect");
		return -1;
	}
		

	pthread_create(&pthread_id,0,pthread_rcv_data,NULL);
	while(1){
		sleep(5);

		write(sockfd,buff,3);
		int n = read(sockfd,buff,20);
		int i;
		printf("main rcv data\n");
		for(i = 0;i<n;i++){
			printf("%02x ",buff[i]);
		}
		puts("");

	}
	close(sockfd);
	return 0;
}
