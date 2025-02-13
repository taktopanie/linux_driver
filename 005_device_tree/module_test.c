#include<stdio.h>
#include<string.h>
#include <fcntl.h>
#include <unistd.h>

#define TRY_READ    10
#define SYSTEM_DEV_FOLDER "/dev/"



char buffer [2048];



int main(int argc, char* argv[])
{

    /* TEST SYSCALLS */

    //OPENING THE DEVICE FILE
    int file_descriptor;

    //int remaining = TRY_READ;
   

    //int total_read = 0;

    int n = 0;
    int ret = 0;

    if(argc != 2)
    {
        printf("Wrong usage!!\n");
        printf("Correct usage <filename> <readcount>\n");
    }

    char* device_name ; 

    device_name = argv[1];
    //remaining = atoi(argv[2]);

    char path [200];
    for(int i = 0 ; i < 200; i++)
    {
        path[i] = '\0';
    }

    strcat(path, SYSTEM_DEV_FOLDER);
    strcat(path, device_name);

    //printf("read requested = %d\n", remaining);

    printf("PATH: %s\n", path);

    file_descriptor = open(path, O_RDWR );

    if(file_descriptor < 0)
    {
        perror("open");
        return file_descriptor;
    }else
        printf("opening the device was successful\n");

    /* Now we can write to the device */

    char * message = "Hello my little device";
    int result; 

    result = write(file_descriptor, message, strlen(message));

    if(result < 0)
    {
        perror("write");
        return file_descriptor;
    }else
        printf("Successful writing to the device\n");

    /* change the file pointer */
    lseek(file_descriptor, 0, SEEK_SET);


    /* read buffer size from the device */
    result = read(file_descriptor, buffer, 2048);
    if(result < 0)
    {
        perror("read");
        return file_descriptor;
    }

    printf("Successful read from the device: %d bytes\n", result);

    printf ("message from the device: %s\n", buffer);

    printf("Closing the device\n");
    close(file_descriptor);
}