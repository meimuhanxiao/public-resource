#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cgic.h"
#include <string.h>
int cgiMain()
{
	char username[10] = {0}, password[10] = {0}; 
	cgiFormString("username",username,10);
	cgiFormString("password",password,10);
	
	if (!strcmp(username,"root") && !strcmp(password,"123")){
		printf("Set-Cookie:username=%s;path=/;\n",username);
		cgiHeaderContentType("text/html"); 
		fprintf(cgiOut, "<HTML>\n");
		fprintf(cgiOut, "<meta charset='UTF-8'>");
		fprintf(cgiOut, "<HTML><HEAD>\n");
		fprintf(cgiOut, "<TITLE>logining</TITLE></HEAD>\n");
		fprintf(cgiOut, "<BODY>");
	//	fprintf(cgiOut, "<H3>%s<H3>","login successfully,skipping...");		
		fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"0;url=../show.html\">");//跳到主页面
	}
	else{
		cgiHeaderContentType("text/html");
		fprintf(cgiOut, "<HTML>\n");
		fprintf(cgiOut, "<meta charset='UTF-8'>");
		fprintf(cgiOut, "<HTML><HEAD>\n");
		fprintf(cgiOut, "<TITLE>logining</TITLE></HEAD>\n");
		fprintf(cgiOut, "<BODY>");
		fprintf(cgiOut, "<H3>%s<H3>","用户名或者密码不正确");		
        fprintf(cgiOut, "<a href='../index.html'>Return</a>");
        fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"2;url=../index.html\">");
       	}
	return 0;
}
