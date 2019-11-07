#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, const char *argv[])
{
	FILE *fp = fopen("2.txt","r");
	char *line = NULL;
	size_t len = 0;
	int n = 0;
	n=getline(&line, &len, fp) ;
	printf("n = %d\n",n);
	printf("line[0] = %d\n",line[0]);
	printf("line[1] = %d\n",line[1]);
	int c = atoi(line);
	printf("c = %d\n",c);

	fclose(fp);

	c += 5;
	char s[10] = {0};
	sprintf(s,"%d",c);
	fp = fopen("2.txt","r+");
	fwrite(s,strlen(s),1,fp);
	fclose(fp);
	


	return 0;
}
