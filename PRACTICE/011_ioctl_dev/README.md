IOCTL usage simple driver

Old-style LED_ON/LED_OFF during mule is loaded/unloaded [GPIO_27 (RPI 539)]

/*
****************************************
    GPIO_17 controlled by IOCTL :  
        #define LED_ON _IO(0x10, 2)
        #define LED_OFF _IO(0x10, 3)
****************************************
/*

NOTE:
Remember to use free number Documentation/userspace-api/ioctl/ioctl-number.rst if use IOCTL functionality
