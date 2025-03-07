#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main (void)
{
    int fd;
    
    if(fd = open("/dev/my_dev-0", O_RDWR) < 0)
    {
        printf("Couldnt open /dev/ device file\n");
        return 1;
    }

    if(ioctl(fd, 'a', NULL))
	{
		printf("Ioctl failed\n");
	}else
	{
		printf("Ioctl success\n");
	}

    close(fd);
    return 0;
}