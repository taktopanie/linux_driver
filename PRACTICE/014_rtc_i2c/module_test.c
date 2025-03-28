#include<stdio.h>
#include<string.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>

#define TRY_READ    10
#define SYSTEM_DEV_FOLDER "/dev/"


static time_t rtc_time_to_timestamp(struct rtc_time *rtc_time)
{
	struct tm tm_time = {
	       .tm_sec = rtc_time->tm_sec,
	       .tm_min = rtc_time->tm_min,
	       .tm_hour = rtc_time->tm_hour,
	       .tm_mday = rtc_time->tm_mday,
	       .tm_mon = rtc_time->tm_mon,
	       .tm_year = rtc_time->tm_year,
	};

	return mktime(&tm_time);
}



int main(int argc, char* argv[])
{

    /* TEST SYSCALLS */
    

    //OPENING THE DEVICE FILE
    int file_descriptor;

    int n = 0;
    int ret = 0;
    struct rtc_time my_time;
    time_t my_time_t;
    char* time_string;

    if(argc != 2)
    {
        printf("Wrong usage!!\n");
        printf("Correct usage <filename>\n");
        return -1;
    }

    char* device_name ; 

    device_name = argv[1];

    char path [200];
    for(int i = 0 ; i < 200; i++)
    {
        path[i] = '\0';
    }

    strcat(path, SYSTEM_DEV_FOLDER);
    strcat(path, device_name);

    printf("PATH: %s\n", path);

    file_descriptor = open(path, O_RDWR);

    if(file_descriptor < 0)
    {
        perror("open");
        return file_descriptor;
    }else
        printf("opening the device was successful\n");

    /* Now we can write to the device */
    ret = ioctl(file_descriptor, RTC_RD_TIME, &my_time);

    if(ret < 0)
    {
        printf("error\n");
        return -1;
    }

    my_time_t = rtc_time_to_timestamp(&my_time);

    time_string = ctime(&my_time_t);

    printf ("TIME: %s\n",time_string);

    printf("Closing the device\n");
    close(file_descriptor);
}