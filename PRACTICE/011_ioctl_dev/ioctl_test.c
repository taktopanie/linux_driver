#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define WR_VALUE _IOW(0x10, 0, unsigned long *)
#define RD_VALUE _IOR(0x10, 1, unsigned long *)
#define LED_ON _IO(0x10, 2)
#define LED_OFF _IO(0x10, 3)

int main (void)
{
    int fd;
    unsigned long  numb = 32;
    unsigned long  value;

    fd = open("/dev/my_gpio-0", O_RDWR);

    if(fd < 0)
    {
        printf("Couldnt open /dev/ device file\n");
        return 1;
    }

    ioctl(fd, LED_ON, (unsigned long *) &numb); 
    printf("LED turned ON\n");

    sleep(2);
    
    ioctl(fd, LED_OFF, (unsigned long *) &value);
    printf("LED turned OFF\n");

    if(close(fd) < 0)
    {
        printf("Close FAILED\n");
    }
 
    return 0;
}