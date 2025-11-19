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

int main (int param, char* vargs [])
{
    int fd;
    unsigned long  numb = 32;
    unsigned long  value;
    printf("param =  %d\n", param);
    for(int i = 0; i < param; i++)
    {
        printf("vargs[%d] =  %s\n", i,vargs[i]);
    }

    fd = open("/dev/UART_STM", O_RDWR);

    if(fd < 0)
    {
        printf("Couldnt open /dev/ device file\n");
        return 1;
    }
    for( int i = 0 ; i < 2000; i++)
    {
        ioctl(fd, LED_ON, (unsigned long *) &numb); 
        //printf("LED turned ON\n");

        usleep(50000);
        
        ioctl(fd, LED_OFF, (unsigned long *) &value);
        //printf("LED turned OFF\n");

        usleep(50000);
    }
    if(close(fd) < 0)
    {
        printf("Close FAILED\n");
    }
 
    return 0;
}