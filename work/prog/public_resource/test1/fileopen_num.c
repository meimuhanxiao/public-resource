#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, const char *argv[])
{
	while(1){
		FILE *fp = NULL;
		fp = fopen("1.txt","r");
		if(fp == NULL){
			perror("fopen");
			return 0;
		}
		//fclose(fp);
		sleep(1);
	}
	return 0;
}
