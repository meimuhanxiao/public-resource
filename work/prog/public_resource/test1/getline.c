#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	FILE *fp = fopen("1.txt","r+");
	if(fp == NULL){
		perror("open 1.txt");
		return -1;
	}
	char *line = NULL;//指针要初始化NULL 要不段错误
	size_t len;
	getline(&line,&len,fp);
	printf("line = %s\n",line);
	fwrite("111\n",4,1,fp);
	fclose(fp);
	return 0;
}
