#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>                      // �����ĸ�ͷ�ļ���linuxϵͳ������е�
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <linux/videodev2.h>                // ��������ͷ�豸

int main(int argc, char **argv)
{	
	int ret, i;
	int fd;

	/* ������ͷ�豸 */ 
	fd = open("/dev/video2", O_RDWR);       // ע��鿴����ͷ�豸��
	if (-1 == fd)
	{
		perror("open /dev/video0");
		return -1;
	}

	/* ��ѯ�򿪵��豸�Ƿ���������ͷ���豸video��һ��������ͷ*/
	struct v4l2_capability cap;
	ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
	if (-1 == ret)
	{
		perror("ioctl VIDIOC_QUERYCAP");
		close(fd);
	}
	if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
	{
		/* ���Ϊ����ͷ�豸���ӡ����ͷ�������� */
		printf("Driver    Name: %s\n", cap.driver);         
	}
	else
	{
		printf("open file is not video\n");
		close(fd);
		return -2;
	}

	/* ��ѯ����ͷ�ɲ�׽��ͼƬ���ͣ�VIDIOC_ENUM_FMT: ö������ͷ֡��ʽ */
	struct v4l2_fmtdesc fmt;
	int j;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // ָ����Ҫö�ٵ�����
	for (j = 0; ; j++)                      // �п�������ͷ֧�ֵ�ͼƬ��ʽ��ֹһ�� 
	{
		fmt.index = j;
		ret = ioctl(fd, VIDIOC_ENUM_FMT, &fmt);
	//	printf("i = %d\n",i);
		if (-1 == ret)                      // ��ȡ���и�ʽ���
		{
			break;	
		}
		
		/* ��ӡ����ͷͼƬ��ʽ */
		printf("Picture Format: %s\n", fmt.description);    

		/* ��ѯ��ͼ���ʽ��֧�ֵķֱ��� */	
		struct v4l2_frmsizeenum frmsize;
		frmsize.pixel_format = fmt.pixelformat;
		for (i = 0; ; i++)                  //���ø�ʽ֧�ֱַ��ʲ�ֹһ��
		{
			frmsize.index = i;
			ret = ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize);
			if (-1 == ret)                  // ��ȡ����ͼƬ�ֱ������
			{
				break;
			}
			
			/* ��ӡͼƬ�ֱ��� */
			printf("width: %d height: %d\n",
			        frmsize.discrete.width,frmsize.discrete.height);
		}

	}
	
	close(fd);                              // ��Ҫ���ǹرմ򿪵��豸

	return 0;
}
 
