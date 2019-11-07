#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"

int cgiMain()
{
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<meta charset='UTF-8'>");    
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>com_show</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");
	fprintf(cgiOut,"<h3>串口参数设置</h3><p></p>");
	fprintf(cgiOut,"<FORM action=\"com_set.cgi\" method=\"post\">");
	fprintf(cgiOut,"<h4>串口1设置：</h4><p></p>");
	fprintf(cgiOut,"外接设备:");
	FILE *fp = NULL;
	fp = fopen("com_set/com_set.txt","r");
	if(fp == NULL)
		perror("read com_set.txt error");
	char *line = NULL;
	size_t len = 0;

	getline(&line,&len,fp);//串口1所接设备
	int n = atoi(line);
	switch(n){
	case 1:{//六要素
			   fprintf(cgiOut,"<SELECT name=\"device_1\">");	
			   fprintf(cgiOut,"<option value=\"1\"selected = \"selected\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\">无</option>");
			   fprintf(cgiOut,"</SELECT>");
			   break;
		   }
	case 2:{
			   fprintf(cgiOut,"<SELECT name=\"device_1\">");	
			   fprintf(cgiOut,"<option value=\"1\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\"selected = \"selected\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\">无</option>");
			   fprintf(cgiOut,"</SELECT>");
			   break;
		   }
	case 3:{
			   fprintf(cgiOut,"<SELECT name=\"device_1\">");	
			   fprintf(cgiOut,"<option value=\"1\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\"selected = \"selected\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\">无</option>");
			   fprintf(cgiOut,"</SELECT>");
			   break;
		   }
	case 0:{
			   fprintf(cgiOut,"<SELECT name=\"device_1\">");	
			   fprintf(cgiOut,"<option value=\"1\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\"selected = \"selected\">无</option>");
			   fprintf(cgiOut,"</SELECT>");
			   break;
		   }
	}

	getline(&line,&len,fp);//串口1数据位
	n = atoi(line);
	switch(n){
	case 6:{
			   fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp数据位:");	
			   fprintf(cgiOut,"<SELECT name=\"databit_1\">");
			   fprintf(cgiOut,"<option value=\"6\" selected = \"selected\">6</option>");
			   fprintf(cgiOut,"<option value=\"7\">7</option>");
			   fprintf(cgiOut,"<option value=\"8\" >8</option>");
			   fprintf(cgiOut,"</SELECT>");
			   break;
		   }
	case 7:{
			   fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp数据位:");	
			   fprintf(cgiOut,"<SELECT name=\"databit_1\">");
			   fprintf(cgiOut,"<option value=\"6\">6</option>");
			   fprintf(cgiOut,"<option value=\"7\"selected = \"selected\">7</option>");
			   fprintf(cgiOut,"<option value=\"8\">8</option>");
			   fprintf(cgiOut,"</SELECT>");
			   break;
		   }
	case 8:{
			   fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp数据位:");	
			   fprintf(cgiOut,"<SELECT name=\"databit_1\">");
			   fprintf(cgiOut,"<option value=\"6\">6</option>");
			   fprintf(cgiOut,"<option value=\"7\">7</option>");
			   fprintf(cgiOut,"<option value=\"8\"selected = \"selected\">8</option>");
			   fprintf(cgiOut,"</SELECT>");
			   break;
		   }
	}

	getline(&line,&len,fp);//串口1停止位
	if(!strcmp("1\n",line)){
		fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp停止位:");	
		fprintf(cgiOut,"<SELECT name=\"stopbit_1\">");
		fprintf(cgiOut,"<option value=\"1\" selected = \"selected\">1</option>");
		fprintf(cgiOut,"<option value=\"1.5\">1.5</option>");
		fprintf(cgiOut,"<option value=\"2\">2</option>");
		fprintf(cgiOut,"</SELECT> <p></p>");
	}
	else if(!strcmp("1.5\n",line)){
		fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp停止位:");	
		fprintf(cgiOut,"<SELECT name=\"stopbit_1\">");
		fprintf(cgiOut,"<option value=\"1\">1</option>");
		fprintf(cgiOut,"<option value=\"1.5\"selected = \"selected\">1.5</option>");
		fprintf(cgiOut,"<option value=\"2\">2</option>");
		fprintf(cgiOut,"</SELECT> <p></p>");
	}
	else if(!strcmp("2\n",line)){
		fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp停止位:");	
		fprintf(cgiOut,"<SELECT name=\"stopbit_1\">");
		fprintf(cgiOut,"<option value=\"1\">1</option>");
		fprintf(cgiOut,"<option value=\"1.5\">1.5</option>");
		fprintf(cgiOut,"<option value=\"2\" selected = \"selected\">2</option>");
		fprintf(cgiOut,"</SELECT> <p></p>");
	}

	getline(&line,&len,fp);//串口1校验位
	n = atoi(line);
	switch(n){
	case 0:{
				fprintf(cgiOut,"奇偶校验位:");
				fprintf(cgiOut,"<SELECT name=\"paritybit_1\">");
				fprintf(cgiOut,"<option value=\"0\" selected = \"selected\">无校验</option>");
				fprintf(cgiOut,"<option value=\"1\">奇校验</option>");
				fprintf(cgiOut,"<option value=\"2\">偶校验</option>");
				fprintf(cgiOut,"</SELECT>");
			   break;
		   }
	case 1:{
			   fprintf(cgiOut,"奇偶校验位:");
			   fprintf(cgiOut,"<SELECT name=\"paritybit_1\">");
			   fprintf(cgiOut,"<option value=\"0\" >无校验</option>");
			   fprintf(cgiOut,"<option value=\"1\"selected = \"selected\">奇校验</option>");
			   fprintf(cgiOut,"<option value=\"2\">偶校验</option>");
			   fprintf(cgiOut,"</SELECT>");
			   break;
		   }
	case 2:{
			   fprintf(cgiOut,"奇偶校验位:");
			   fprintf(cgiOut,"<SELECT name=\"paritybit_1\">");
			   fprintf(cgiOut,"<option value=\"0\" >无校验</option>");
			   fprintf(cgiOut,"<option value=\"1\">奇校验</option>");
			   fprintf(cgiOut,"<option value=\"2\"selected = \"selected\">偶校验</option>");
			   fprintf(cgiOut,"</SELECT>");
			   break;
		   }
	}
	getline(&line,&len,fp);//串口1波特率
	n = atoi(line);
	fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp波特率:");
	fprintf(cgiOut,"<SELECT name=\"baudrate_1\">");
	switch(n){
	case 9600:{
				  fprintf(cgiOut,"<option value=\"9600\" selected = \"selected\">9600</option>");
				  fprintf(cgiOut,"<option value=\"19200\">19200</option>");
				  fprintf(cgiOut,"<option value=\"38400\">38400</option>");
				  fprintf(cgiOut,"<option value=\"57600\">57600</option>");
				  fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				  break;
			  }

	case 19200:{
				  fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				  fprintf(cgiOut,"<option value=\"19200\" selected = \"selected\" >19200</option>");
				  fprintf(cgiOut,"<option value=\"38400\">38400</option>");
				  fprintf(cgiOut,"<option value=\"57600\">57600</option>");
				  fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				  break;
			  }
	case 38400:{
				   fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				   fprintf(cgiOut,"<option value=\"19200\"  >19200</option>");
				   fprintf(cgiOut,"<option value=\"38400\" selected = \"selected\">38400</option>");
				   fprintf(cgiOut,"<option value=\"57600\">57600</option>");
				   fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				   break;
			   }
	case 57600:{
				   fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				   fprintf(cgiOut,"<option value=\"19200\"  >19200</option>");
				   fprintf(cgiOut,"<option value=\"38400\" >38400</option>");
				   fprintf(cgiOut,"<option value=\"57600\" selected = \"selected\">57600</option>");
				   fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				   break;
			   }

	case 115200:{
				   fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				   fprintf(cgiOut,"<option value=\"19200\"  >19200</option>");
				   fprintf(cgiOut,"<option value=\"38400\" >38400</option>");
				   fprintf(cgiOut,"<option value=\"57600\" >57600</option>");
				   fprintf(cgiOut,"<option value=\"115200\" selected = \"selected\">115200</option>");
				   break;
			   }

	}
	fprintf(cgiOut,"</SELECT><p></p>");


	fprintf(cgiOut,"<h4>串口2设置：</h4><p></p>");
	fprintf(cgiOut,"外接设备:");
	getline(&line,&len,fp);//串口2所接设备
	n = atoi(line);
	fprintf(cgiOut,"<SELECT name=\"device_2\">");	
	switch(n){
	case 1:{//六要素
			   fprintf(cgiOut,"<option value=\"1\"selected = \"selected\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\">无</option>");
			   break;
		   }
	case 2:{
			   fprintf(cgiOut,"<option value=\"1\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\"selected = \"selected\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\">无</option>");
			   break;
		   }
	case 3:{
			   fprintf(cgiOut,"<option value=\"1\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\"selected = \"selected\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\">无</option>");
			   break;
		   }
	case 0:{
			   fprintf(cgiOut,"<option value=\"1\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\"selected = \"selected\">无</option>");
			   break;
		   }
	}
	fprintf(cgiOut,"</SELECT>");

	getline(&line,&len,fp);//串口2数据位
	n = atoi(line);
	fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp数据位:");	
	fprintf(cgiOut,"<SELECT name=\"databit_2\">");

	switch(n){
	case 6:{
			   fprintf(cgiOut,"<option value=\"6\" selected = \"selected\">6</option>");
			   fprintf(cgiOut,"<option value=\"7\">7</option>");
			   fprintf(cgiOut,"<option value=\"8\" >8</option>");
			   break;
		   }
	case 7:{
			   fprintf(cgiOut,"<option value=\"6\">6</option>");
			   fprintf(cgiOut,"<option value=\"7\"selected = \"selected\">7</option>");
			   fprintf(cgiOut,"<option value=\"8\">8</option>");
			   break;
		   }
	case 8:{
			   fprintf(cgiOut,"<option value=\"6\">6</option>");
			   fprintf(cgiOut,"<option value=\"7\">7</option>");
			   fprintf(cgiOut,"<option value=\"8\"selected = \"selected\">8</option>");
			   break;
		   }
	}
	fprintf(cgiOut,"</SELECT>");

	getline(&line,&len,fp);//串口2停止位
	fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp停止位:");	
	fprintf(cgiOut,"<SELECT name=\"stopbit_2\">");

	if(!strcmp("1\n",line)){
		fprintf(cgiOut,"<option value=\"1\" selected = \"selected\">1</option>");
		fprintf(cgiOut,"<option value=\"1.5\">1.5</option>");
		fprintf(cgiOut,"<option value=\"2\">2</option>");
	}
	else if(!strcmp("1.5\n",line)){
		fprintf(cgiOut,"<option value=\"1\">1</option>");
		fprintf(cgiOut,"<option value=\"1.5\"selected = \"selected\">1.5</option>");
		fprintf(cgiOut,"<option value=\"2\">2</option>");
	}
	else if(!strcmp("2\n",line)){
		fprintf(cgiOut,"<option value=\"1\">1</option>");
		fprintf(cgiOut,"<option value=\"1.5\">1.5</option>");
		fprintf(cgiOut,"<option value=\"2\" selected = \"selected\">2</option>");
	}
	fprintf(cgiOut,"</SELECT> <p></p>");

	getline(&line,&len,fp);//串口2校验位
	n = atoi(line);
	fprintf(cgiOut,"奇偶校验位:");
	fprintf(cgiOut,"<SELECT name=\"paritybit_2\">");

	switch(n){
	case 0:{
			   fprintf(cgiOut,"<option value=\"0\" selected = \"selected\">无校验</option>");
			   fprintf(cgiOut,"<option value=\"1\">奇校验</option>");
			   fprintf(cgiOut,"<option value=\"2\">偶校验</option>");
			   break;
		   }
	case 1:{
			   fprintf(cgiOut,"<option value=\"0\" >无校验</option>");
			   fprintf(cgiOut,"<option value=\"1\"selected = \"selected\">奇校验</option>");
			   fprintf(cgiOut,"<option value=\"2\">偶校验</option>");
			   break;
		   }
	case 2:{
			   fprintf(cgiOut,"<option value=\"0\" >无校验</option>");
			   fprintf(cgiOut,"<option value=\"1\">奇校验</option>");
			   fprintf(cgiOut,"<option value=\"2\"selected = \"selected\">偶校验</option>");
			   break;
		   }
	}
	fprintf(cgiOut,"</SELECT>");

	getline(&line,&len,fp);//串口2波特率
	n = atoi(line);
	fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp波特率:");
	fprintf(cgiOut,"<SELECT name=\"baudrate_2\">");
	switch(n){
	case 9600:{
				  fprintf(cgiOut,"<option value=\"9600\" selected = \"selected\">9600</option>");
				  fprintf(cgiOut,"<option value=\"19200\">19200</option>");
				  fprintf(cgiOut,"<option value=\"38400\">38400</option>");
				  fprintf(cgiOut,"<option value=\"57600\">57600</option>");
				  fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				  break;
			  }

	case 19200:{
				  fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				  fprintf(cgiOut,"<option value=\"19200\" selected = \"selected\" >19200</option>");
				  fprintf(cgiOut,"<option value=\"38400\">38400</option>");
				  fprintf(cgiOut,"<option value=\"57600\">57600</option>");
				  fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				  break;
			  }
	case 38400:{
				   fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				   fprintf(cgiOut,"<option value=\"19200\"  >19200</option>");
				   fprintf(cgiOut,"<option value=\"38400\" selected = \"selected\">38400</option>");
				   fprintf(cgiOut,"<option value=\"57600\">57600</option>");
				   fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				   break;
			   }
	case 57600:{
				   fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				   fprintf(cgiOut,"<option value=\"19200\"  >19200</option>");
				   fprintf(cgiOut,"<option value=\"38400\" >38400</option>");
				   fprintf(cgiOut,"<option value=\"57600\" selected = \"selected\">57600</option>");
				   fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				   break;
			   }

	case 115200:{
				   fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				   fprintf(cgiOut,"<option value=\"19200\"  >19200</option>");
				   fprintf(cgiOut,"<option value=\"38400\" >38400</option>");
				   fprintf(cgiOut,"<option value=\"57600\" >57600</option>");
				   fprintf(cgiOut,"<option value=\"115200\" selected = \"selected\">115200</option>");
				   break;
			   }

	}
	fprintf(cgiOut,"</SELECT><p></p>");

	fprintf(cgiOut,"<h4>串口3设置：</h4><p></p>");
	fprintf(cgiOut,"外接设备:");
	getline(&line,&len,fp);//串口3所接设备
	n = atoi(line);
	fprintf(cgiOut,"<SELECT name=\"device_3\">");	
	switch(n){
	case 1:{//六要素
			   fprintf(cgiOut,"<option value=\"1\"selected = \"selected\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\">无</option>");
			   break;
		   }
	case 2:{
			   fprintf(cgiOut,"<option value=\"1\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\"selected = \"selected\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\">无</option>");
			   break;
		   }
	case 3:{
			   fprintf(cgiOut,"<option value=\"1\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\"selected = \"selected\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\">无</option>");
			   break;
		   }
	case 0:{
			   fprintf(cgiOut,"<option value=\"1\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\"selected = \"selected\">无</option>");
			   break;
		   }
	}
	fprintf(cgiOut,"</SELECT>");

	getline(&line,&len,fp);//串口3数据位
	n = atoi(line);
	fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp数据位:");	
	fprintf(cgiOut,"<SELECT name=\"databit_3\">");

	switch(n){
	case 6:{
			   fprintf(cgiOut,"<option value=\"6\" selected = \"selected\">6</option>");
			   fprintf(cgiOut,"<option value=\"7\">7</option>");
			   fprintf(cgiOut,"<option value=\"8\" >8</option>");
			   break;
		   }
	case 7:{
			   fprintf(cgiOut,"<option value=\"6\">6</option>");
			   fprintf(cgiOut,"<option value=\"7\"selected = \"selected\">7</option>");
			   fprintf(cgiOut,"<option value=\"8\">8</option>");
			   break;
		   }
	case 8:{
			   fprintf(cgiOut,"<option value=\"6\">6</option>");
			   fprintf(cgiOut,"<option value=\"7\">7</option>");
			   fprintf(cgiOut,"<option value=\"8\"selected = \"selected\">8</option>");
			   break;
		   }
	}
	fprintf(cgiOut,"</SELECT>");

	getline(&line,&len,fp);//串口3停止位
	fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp停止位:");	
	fprintf(cgiOut,"<SELECT name=\"stopbit_3\">");

	if(!strcmp("1\n",line)){
		fprintf(cgiOut,"<option value=\"1\" selected = \"selected\">1</option>");
		fprintf(cgiOut,"<option value=\"1.5\">1.5</option>");
		fprintf(cgiOut,"<option value=\"2\">2</option>");
	}
	else if(!strcmp("1.5\n",line)){
		fprintf(cgiOut,"<option value=\"1\">1</option>");
		fprintf(cgiOut,"<option value=\"1.5\"selected = \"selected\">1.5</option>");
		fprintf(cgiOut,"<option value=\"2\">2</option>");
	}
	else if(!strcmp("2\n",line)){
		fprintf(cgiOut,"<option value=\"1\">1</option>");
		fprintf(cgiOut,"<option value=\"1.5\">1.5</option>");
		fprintf(cgiOut,"<option value=\"2\" selected = \"selected\">2</option>");
	}
	fprintf(cgiOut,"</SELECT> <p></p>");

	getline(&line,&len,fp);//串口3校验位
	n = atoi(line);
	fprintf(cgiOut,"奇偶校验位:");
	fprintf(cgiOut,"<SELECT name=\"paritybit_3\">");

	switch(n){
	case 0:{
			   fprintf(cgiOut,"<option value=\"0\" selected = \"selected\">无校验</option>");
			   fprintf(cgiOut,"<option value=\"1\">奇校验</option>");
			   fprintf(cgiOut,"<option value=\"2\">偶校验</option>");
			   break;
		   }
	case 1:{
			   fprintf(cgiOut,"<option value=\"0\" >无校验</option>");
			   fprintf(cgiOut,"<option value=\"1\"selected = \"selected\">奇校验</option>");
			   fprintf(cgiOut,"<option value=\"2\">偶校验</option>");
			   break;
		   }
	case 2:{
			   fprintf(cgiOut,"<option value=\"0\" >无校验</option>");
			   fprintf(cgiOut,"<option value=\"1\">奇校验</option>");
			   fprintf(cgiOut,"<option value=\"2\"selected = \"selected\">偶校验</option>");
			   break;
		   }
	}
	fprintf(cgiOut,"</SELECT>");

	getline(&line,&len,fp);//串口3波特率
	n = atoi(line);
	fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp波特率:");
	fprintf(cgiOut,"<SELECT name=\"baudrate_3\">");
	switch(n){
	case 9600:{
				  fprintf(cgiOut,"<option value=\"9600\" selected = \"selected\">9600</option>");
				  fprintf(cgiOut,"<option value=\"19200\">19200</option>");
				  fprintf(cgiOut,"<option value=\"38400\">38400</option>");
				  fprintf(cgiOut,"<option value=\"57600\">57600</option>");
				  fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				  break;
			  }

	case 19200:{
				  fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				  fprintf(cgiOut,"<option value=\"19200\" selected = \"selected\" >19200</option>");
				  fprintf(cgiOut,"<option value=\"38400\">38400</option>");
				  fprintf(cgiOut,"<option value=\"57600\">57600</option>");
				  fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				  break;
			  }
	case 38400:{
				   fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				   fprintf(cgiOut,"<option value=\"19200\"  >19200</option>");
				   fprintf(cgiOut,"<option value=\"38400\" selected = \"selected\">38400</option>");
				   fprintf(cgiOut,"<option value=\"57600\">57600</option>");
				   fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				   break;
			   }
	case 57600:{
				   fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				   fprintf(cgiOut,"<option value=\"19200\"  >19200</option>");
				   fprintf(cgiOut,"<option value=\"38400\" >38400</option>");
				   fprintf(cgiOut,"<option value=\"57600\" selected = \"selected\">57600</option>");
				   fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				   break;
			   }

	case 115200:{
				   fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				   fprintf(cgiOut,"<option value=\"19200\"  >19200</option>");
				   fprintf(cgiOut,"<option value=\"38400\" >38400</option>");
				   fprintf(cgiOut,"<option value=\"57600\" >57600</option>");
				   fprintf(cgiOut,"<option value=\"115200\" selected = \"selected\">115200</option>");
				   break;
			   }

	}
	fprintf(cgiOut,"</SELECT><p></p>");

	fprintf(cgiOut,"<h4>串口4设置：</h4><p></p>");
	fprintf(cgiOut,"外接设备:");
	getline(&line,&len,fp);//串口3所接设备
	 n = atoi(line);
	fprintf(cgiOut,"<SELECT name=\"device_4\">");	
	switch(n){
	case 1:{//六要素
			   fprintf(cgiOut,"<option value=\"1\"selected = \"selected\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\">无</option>");
			   break;
		   }
	case 2:{
			   fprintf(cgiOut,"<option value=\"1\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\"selected = \"selected\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\">无</option>");
			   break;
		   }
	case 3:{
			   fprintf(cgiOut,"<option value=\"1\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\"selected = \"selected\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\">无</option>");
			   break;
		   }
	case 0:{
			   fprintf(cgiOut,"<option value=\"1\">六要素</option>");
			   fprintf(cgiOut,"<option value=\"2\">能见度</option>");
			   fprintf(cgiOut,"<option value=\"3\">路感</option>");
			   fprintf(cgiOut,"<option value=\"0\"selected = \"selected\">无</option>");
			   break;
		   }
	}
	fprintf(cgiOut,"</SELECT>");

	getline(&line,&len,fp);//串口4数据位
	n = atoi(line);
	fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp数据位:");	
	fprintf(cgiOut,"<SELECT name=\"databit_4\">");

	switch(n){
	case 6:{
			   fprintf(cgiOut,"<option value=\"6\" selected = \"selected\">6</option>");
			   fprintf(cgiOut,"<option value=\"7\">7</option>");
			   fprintf(cgiOut,"<option value=\"8\" >8</option>");
			   break;
		   }
	case 7:{
			   fprintf(cgiOut,"<option value=\"6\">6</option>");
			   fprintf(cgiOut,"<option value=\"7\"selected = \"selected\">7</option>");
			   fprintf(cgiOut,"<option value=\"8\">8</option>");
			   break;
		   }
	case 8:{
			   fprintf(cgiOut,"<option value=\"6\">6</option>");
			   fprintf(cgiOut,"<option value=\"7\">7</option>");
			   fprintf(cgiOut,"<option value=\"8\"selected = \"selected\">8</option>");
			   break;
		   }
	}
	fprintf(cgiOut,"</SELECT>");

	getline(&line,&len,fp);//串口4停止位
	fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp停止位:");	
	fprintf(cgiOut,"<SELECT name=\"stopbit_4\">");

	if(!strcmp("1\n",line)){
		fprintf(cgiOut,"<option value=\"1\" selected = \"selected\">1</option>");
		fprintf(cgiOut,"<option value=\"1.5\">1.5</option>");
		fprintf(cgiOut,"<option value=\"2\">2</option>");
	}
	else if(!strcmp("1.5\n",line)){
		fprintf(cgiOut,"<option value=\"1\">1</option>");
		fprintf(cgiOut,"<option value=\"1.5\"selected = \"selected\">1.5</option>");
		fprintf(cgiOut,"<option value=\"2\">2</option>");
	}
	else if(!strcmp("2\n",line)){
		fprintf(cgiOut,"<option value=\"1\">1</option>");
		fprintf(cgiOut,"<option value=\"1.5\">1.5</option>");
		fprintf(cgiOut,"<option value=\"2\" selected = \"selected\">2</option>");
	}
	fprintf(cgiOut,"</SELECT> <p></p>");

	getline(&line,&len,fp);//串口4校验位
	n = atoi(line);
	fprintf(cgiOut,"奇偶校验位:");
	fprintf(cgiOut,"<SELECT name=\"paritybit_4\">");

	switch(n){
	case 0:{
			   fprintf(cgiOut,"<option value=\"0\" selected = \"selected\">无校验</option>");
			   fprintf(cgiOut,"<option value=\"1\">奇校验</option>");
			   fprintf(cgiOut,"<option value=\"2\">偶校验</option>");
			   break;
		   }
	case 1:{
			   fprintf(cgiOut,"<option value=\"0\" >无校验</option>");
			   fprintf(cgiOut,"<option value=\"1\"selected = \"selected\">奇校验</option>");
			   fprintf(cgiOut,"<option value=\"2\">偶校验</option>");
			   break;
		   }
	case 2:{
			   fprintf(cgiOut,"<option value=\"0\" >无校验</option>");
			   fprintf(cgiOut,"<option value=\"1\">奇校验</option>");
			   fprintf(cgiOut,"<option value=\"2\"selected = \"selected\">偶校验</option>");
			   break;
		   }
	}
	fprintf(cgiOut,"</SELECT>");

	getline(&line,&len,fp);//串口4波特率
	n = atoi(line);
	fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp波特率:");
	fprintf(cgiOut,"<SELECT name=\"baudrate_4\">");
	switch(n){
	case 9600:{
				  fprintf(cgiOut,"<option value=\"9600\" selected = \"selected\">9600</option>");
				  fprintf(cgiOut,"<option value=\"19200\">19200</option>");
				  fprintf(cgiOut,"<option value=\"38400\">38400</option>");
				  fprintf(cgiOut,"<option value=\"57600\">57600</option>");
				  fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				  break;
			  }

	case 19200:{
				  fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				  fprintf(cgiOut,"<option value=\"19200\" selected = \"selected\" >19200</option>");
				  fprintf(cgiOut,"<option value=\"38400\">38400</option>");
				  fprintf(cgiOut,"<option value=\"57600\">57600</option>");
				  fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				  break;
			  }
	case 38400:{
				   fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				   fprintf(cgiOut,"<option value=\"19200\"  >19200</option>");
				   fprintf(cgiOut,"<option value=\"38400\" selected = \"selected\">38400</option>");
				   fprintf(cgiOut,"<option value=\"57600\">57600</option>");
				   fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				   break;
			   }
	case 57600:{
				   fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				   fprintf(cgiOut,"<option value=\"19200\"  >19200</option>");
				   fprintf(cgiOut,"<option value=\"38400\" >38400</option>");
				   fprintf(cgiOut,"<option value=\"57600\" selected = \"selected\">57600</option>");
				   fprintf(cgiOut,"<option value=\"115200\">115200</option>");
				   break;
			   }

	case 115200:{
				   fprintf(cgiOut,"<option value=\"9600\" >9600</option>");
				   fprintf(cgiOut,"<option value=\"19200\"  >19200</option>");
				   fprintf(cgiOut,"<option value=\"38400\" >38400</option>");
				   fprintf(cgiOut,"<option value=\"57600\" >57600</option>");
				   fprintf(cgiOut,"<option value=\"115200\" selected = \"selected\">115200</option>");
				   break;
			   }

	}
	fclose(fp);
	fprintf(cgiOut,"</SELECT><p></p>");

	fprintf(cgiOut,"<INPUT type=\"SUBMIT\" value=\"设置\">");
	fprintf(cgiOut,"</FORM>");
	fprintf(cgiOut,"</BODY></HTML>");

	return 0;
}
