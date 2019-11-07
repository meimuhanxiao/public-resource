#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h> 
#include <linux/videodev2.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

struct buffer {
        void * start;
        size_t length;
};

static char * dev_name = "/dev/video2";//����ͷ�豸��
static int fd = -1;
struct buffer * buffers = NULL;
static unsigned int n_buffers = 0;

FILE *file_fd;
static unsigned long file_length;
static unsigned char *file_name;
//////////////////////////////////////////////////////
//��ȡһ֡����
//////////////////////////////////////////////////////
static int read_frame (void)
{
struct v4l2_buffer buf;
unsigned int i;

CLEAR (buf);
buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
buf.memory = V4L2_MEMORY_MMAP;

ioctl (fd, VIDIOC_DQBUF, &buf); //���вɼ���֡����

assert (buf.index < n_buffers);
   printf ("buf.index dq is %d,\n",buf.index);

fwrite(buffers[buf.index].start, buffers[buf.index].length, 1, file_fd); //����д���ļ���
  
ioctl (fd, VIDIOC_QBUF, &buf); //�ٽ�������

return 1;
}

int main (int argc,char ** argv)
{
struct v4l2_capability cap;
struct v4l2_format fmt;
unsigned int i;
enum v4l2_buf_type type;

file_fd = fopen("test-mmap.jpg", "w");//ͼƬ�ļ���

fd = open (dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);//���豸

ioctl (fd, VIDIOC_QUERYCAP, &cap);//��ȡ����ͷ����

CLEAR (fmt);
fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
fmt.fmt.pix.width = 1280;
fmt.fmt.pix.height = 960;
fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;
fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
ioctl (fd, VIDIOC_S_FMT, &fmt); //����ͼ���ʽ

file_length = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height; //����ͼƬ��С

struct v4l2_requestbuffers req;
CLEAR (req);
req.count = 4;
req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
req.memory = V4L2_MEMORY_MMAP;

ioctl (fd, VIDIOC_REQBUFS, &req); //���뻺�壬count�����������

if (req.count < 2)
   printf("Insufficient buffer memory\n");

buffers = calloc (req.count, sizeof (*buffers));//�ڴ��н�����Ӧ�ռ�

for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
{
	struct v4l2_buffer buf; //�����е�һ֡
	CLEAR (buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = n_buffers;

	if (-1 == ioctl (fd, VIDIOC_QUERYBUF, &buf)) //ӳ���û��ռ�
		printf ("VIDIOC_QUERYBUF error\n");

	buffers[n_buffers].length = buf.length;
	buffers[n_buffers].start =
		mmap (NULL /* start anywhere */, //ͨ��mmap����ӳ���ϵ
				buf.length,
				PROT_READ | PROT_WRITE /* required */,
				MAP_SHARED /* recommended */,
				fd, buf.m.offset);

	if (MAP_FAILED == buffers[n_buffers].start)
		printf ("mmap failed\n");
}

for (i = 0; i < n_buffers; ++i)
{
   struct v4l2_buffer buf;
   CLEAR (buf);

   buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   buf.memory = V4L2_MEMORY_MMAP;
   buf.index = i;

   if (-1 == ioctl (fd, VIDIOC_QBUF, &buf))//���뵽�Ļ�������ж�
    printf ("VIDIOC_QBUF failed\n");
}
               
type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

if (-1 == ioctl (fd, VIDIOC_STREAMON, &type)) //��ʼ��׽ͼ������
   printf ("VIDIOC_STREAMON failed\n");

for (;;) //��һ���漰���첽IO
{
   fd_set fds;
   struct timeval tv;
   int r;

   FD_ZERO (&fds);//��ָ�����ļ������������
   FD_SET (fd, &fds);//���ļ�����������������һ���µ��ļ�������

   /* Timeout. */
   tv.tv_sec = 2;
   tv.tv_usec = 0;

   r = select (fd + 1, &fds, NULL, NULL, &tv);//�ж��Ƿ�ɶ���������ͷ�Ƿ�׼���ã���tv�Ƕ�ʱ

   if (-1 == r) {
	   if (EINTR == errno)
		   continue;
	   printf ("select err\n");
   }
   if (0 == r) {
    fprintf (stderr, "select timeout\n");
    exit (EXIT_FAILURE);
                        }

   if (read_frame ())//����ɶ���ִ��read_frame ()������������ѭ��
   break;
}

unmap:
for (i = 0; i < n_buffers; ++i)
   if (-1 == munmap (buffers[i].start, buffers[i].length))
    printf ("munmap error");
close (fd);
fclose (file_fd);
exit (EXIT_SUCCESS);
return 0;
}
