#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int main (void)
{
    int fd;
    int32_t numb = 32;
    int32_t value;

    if(fd = open("/dev/my_dev-0", O_RDWR) < 0)
    {
        printf("Couldnt open /dev/ device file\n");
        return 1;
    }

    printf("Write Value to Driver\n");
    ioctl(fd, WR_VALUE, (int32_t*) &numb); 
 
    printf("Reading Value from Driver\n");
    ioctl(fd, RD_VALUE, (int32_t*) &value);

    close(fd);
    return 0;
}