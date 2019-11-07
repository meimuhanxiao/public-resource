#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <linux/types.h>
#include <linux/videodev2.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <assert.h>
#include <asm/types.h>
#include <getopt.h>
#include <sys/time.h>
#define FILE_VIDEO  "/dev/video2"
#define JPG "./_image%d.jpg"
 
typedef struct{
    void *start;
	size_t length;
}BUFTYPE;
 
BUFTYPE *usr_buf = NULL;
static unsigned int n_buffer = 0;
 
 
/*set video capture ways(mmap)*/
int init_mmap(int fd)
{
	/*to request frame cache, contain requested counts*/
	struct v4l2_requestbuffers reqbufs;//
 
	memset(&reqbufs, 0, sizeof(reqbufs));
	reqbufs.count = 4; 	 							/*the number of buffer*/
	reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;    
	reqbufs.memory = V4L2_MEMORY_MMAP;				
 
	if(-1 == ioctl(fd,VIDIOC_REQBUFS,&reqbufs))//在内核空间请求摄像头缓存，一个count就是一帧数据缓存
	{
		perror("Fail to ioctl 'VIDIOC_REQBUFS'");
		exit(EXIT_FAILURE);
	}
	
	n_buffer = reqbufs.count;
	printf("n_buffer = %d\n", n_buffer);
	//usr_buf = calloc(reqbufs.count, sizeof(usr_buf));
	usr_buf = calloc(reqbufs.count, sizeof(BUFTYPE));//用户空间存放内核buf映射信息
	if(usr_buf == NULL)
	{
		printf("Out of memory\n");
		exit(-1);
	}
 
	/*map kernel cache to user process*/
	for(n_buffer = 0; n_buffer < reqbufs.count; ++n_buffer)
	{
		//stand for a frame
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffer;
		
		/*check the information of the kernel cache requested*/
		if(-1 == ioctl(fd,VIDIOC_QUERYBUF,&buf))//申请获得的缓存信息存在buf里，如缓存地址，缓存大小，index代表第几个缓存
		{
			perror("Fail to ioctl : VIDIOC_QUERYBUF");
			exit(EXIT_FAILURE);
		}
 
		usr_buf[n_buffer].length = buf.length;
		usr_buf[n_buffer].start = 
		mmap(NULL,
	    buf.length,
		PROT_READ | PROT_WRITE,
		MAP_SHARED,//buf里有编号第几个Buf，在fd摄像头偏移量是多少存在.m.offset里
		fd,buf.m.offset);//映射内核申请的缓存到用户空间
        printf("buf.length = %d,buf.bytesused = %d\n ",buf.length,buf.bytesused); 
		if(MAP_FAILED == usr_buf[n_buffer].start)
		{
			perror("Fail to mmap");
			exit(EXIT_FAILURE);
		}
 
	}
 
}
 
int open_camera(void)
{
	int fd;
	/*open video device with block */
	fd = open(FILE_VIDEO, O_RDWR);//要以O_RDWR 打开
	if(fd < 0)
	{	
		fprintf(stderr, "%s open err \n", FILE_VIDEO);
		exit(EXIT_FAILURE);
	};
	return fd;
}
 
int init_camera(int fd)
{
	struct v4l2_capability 	cap;	/* decive fuction, such as video input */
	struct v4l2_format 		tv_fmt; /* frame format */  
	struct v4l2_fmtdesc 	fmtdesc;  	/* detail control value */
	struct v4l2_control 	ctrl;
	int ret;
	
			/*show all the support format*/
	memset(&fmtdesc, 0, sizeof(fmtdesc));
	fmtdesc.index = 0 ;                 /* the number to check */
	fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
 
	/* check video decive driver capability */
	if(ret=ioctl(fd, VIDIOC_QUERYCAP, &cap)<0)//查询摄像头是否是capturevideo 
	{
		fprintf(stderr, "fail to ioctl VIDEO_QUERYCAP \n");
		exit(EXIT_FAILURE);
	}
	
	/*judge wherher or not to be a video-get device*/
	if(!(cap.capabilities & V4L2_BUF_TYPE_VIDEO_CAPTURE))
	{
		fprintf(stderr, "The Current device is not a video capture device \n");
		exit(EXIT_FAILURE);
	}
 
	/*judge whether or not to supply the form of video stream*/
	if(!(cap.capabilities & V4L2_CAP_STREAMING))
	{
		printf("The Current device does not support streaming i/o\n");
		exit(EXIT_FAILURE);
	}
	
	printf("\ncamera driver name is : %s\n",cap.driver);
	printf("camera device name is : %s\n",cap.card);
	printf("camera bus information: %s\n",cap.bus_info);
 
		/*display the format device support*/
	while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)//查询该设备capture支持的格式 //MJPEG , YUV 4:2:2 (YUYV)
	{	
		printf("\nsupport Picture %d.%s\n\n",fmtdesc.index+1,fmtdesc.description);
		struct v4l2_frmsizeenum frmsize;
		frmsize.pixel_format = fmtdesc.pixelformat;
		int i;
		for (i = 0; ; i++)                  //　该格式支持分辨率不止一种
		{
			frmsize.index = i;
			ret = ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize);
			if (-1 == ret)                  // 获取所有图片分辨率完成
			{
				break;
			}
			
			/* 打印图片分辨率 */
			printf("width: %d height: %d\n",
			        frmsize.discrete.width,frmsize.discrete.height);
		}
		fmtdesc.index++;
	}
 
 
	/*set the form of camera capture data*/
	tv_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;      /*v4l2_buf_typea,camera must use V4L2_BUF_TYPE_VIDEO_CAPTURE*/
	tv_fmt.fmt.pix.width = 640;
	tv_fmt.fmt.pix.height = 480;
	tv_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;	//设置照片像素格式
	//tv_fmt.fmt.pix.field = V4L2_FIELD_NONE;   		/*V4L2_FIELD_NONE*/
	if (ioctl(fd, VIDIOC_S_FMT, &tv_fmt)< 0) 
	{
		fprintf(stderr,"VIDIOC_S_FMT set err\n");
		exit(-1);
		close(fd);
	}
  printf("bytesperline = %d\n",tv_fmt.fmt.pix.bytesperline);
 init_mmap(fd);
}
 
int start_capture(int fd)
{
	unsigned int i;
	enum v4l2_buf_type type;
	
	/*place the kernel cache to a queue*/
	for(i = 0; i < n_buffer; i++)
	{
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
 
		if(-1 == ioctl(fd, VIDIOC_QBUF, &buf))//入列
		{
			perror("Fail to ioctl 'VIDIOC_QBUF'");
			exit(EXIT_FAILURE);
		}
	}
 
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//捕获了一帧图片通过poll告诉应用进程,拿buf
	if(-1 == ioctl(fd, VIDIOC_STREAMON, &type)) //启动摄像头caputure 模式,摄像头捕获了一帧图片存放到Buf里
	{
		printf("i=%d.\n", i);
		perror("VIDIOC_STREAMON");
		close(fd);
		exit(EXIT_FAILURE);
	}
 
	return 0;
}
 
 
int process_image(void *addr, int length)
{
	FILE *fp;
 
	static int num = 0;//相当于全局变量
 
	char image_name[20];
	sprintf(image_name, JPG, num++);
	if((fp = fopen(image_name, "w")) == NULL)
	{
		perror("Fail to fopen");
		exit(EXIT_FAILURE);
	}
	fwrite(addr, length, 1, fp);
	usleep(500);
	fclose(fp);
	return 0;
}
 
int read_frame(int fd)
{
	struct v4l2_buffer buf;
	unsigned int i;
	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	//put cache from queue
	if(-1 == ioctl(fd, VIDIOC_DQBUF,&buf))//出列，buf里带着是哪个缓存（申请了4个缓存）已经装了一帧图片
	{
		perror("Fail to ioctl 'VIDIOC_DQBUF'");
		exit(EXIT_FAILURE);
	}

	assert(buf.index < n_buffer);
	printf("index = %d\n",buf.index);
	printf("..buf.length = %d,buf.bytesused = %d\n ",buf.length,buf.bytesused); 
	//read process space's data to a file 第几个buffer有一帧图片
	process_image(usr_buf[buf.index].start, buf.bytesused);//保存一帧图片为.jpg文件
	if(-1 == ioctl(fd, VIDIOC_QBUF,&buf))//读取完一帧数据把缓存入列
	{
		perror("Fail to ioctl 'VIDIOC_QBUF'");
		exit(EXIT_FAILURE);
	}
	return 1;
}
 
 
int mainloop(int fd)
{
	int count = 1;
	while(count-- > 0)
	{
		for(;;)
		{
			fd_set fds;
			struct timeval tv;
			int r;
 
			FD_ZERO(&fds);
			FD_SET(fd,&fds);
 
			/*Timeout*/
			tv.tv_sec = 2;
			tv.tv_usec = 0;
			r = select(fd + 1,&fds,NULL,NULL,&tv);
			
			if(-1 == r)
			{
				 if(EINTR == errno)
					continue;
				perror("Fail to select");
				exit(EXIT_FAILURE);
			}
			if(0 == r)
			{
				fprintf(stderr,"select Timeout\n");
				exit(-1);
			}
 
			if(read_frame(fd))
			break;
		}
	}
	return 0;
}
 
void stop_capture(int fd)
{
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1 == ioctl(fd,VIDIOC_STREAMOFF,&type))//关闭捕获
	{
		perror("Fail to ioctl 'VIDIOC_STREAMOFF'");
		exit(EXIT_FAILURE);
	}
}
 
void close_camera_device(int fd)
{
	unsigned int i;
	for(i = 0;i < n_buffer; i++)
	{
		if(-1 == munmap(usr_buf[i].start,usr_buf[i].length))
		{
			exit(-1);
		}
	}
 
	free(usr_buf);
 
	if(-1 == close(fd))
	{
		perror("Fail to close fd");
		exit(EXIT_FAILURE);
	}
}
 
 
void main(void)
{
	int fd;
	fd = open_camera();
	init_camera(fd);
	start_capture(fd);//把四个缓冲Buff放进队列，开启streamon
	sleep(5);
	int i;
	for(i = 0;i < 3;i++)
		mainloop(fd);
	stop_capture(fd);
	close_camera_device(fd);
}
