obj-m := my_driver.o my_device.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

KERNEL_DIR=/home/maciej/Desktop/moje/programming/Linux/device_driver/rpi_kernel/linux_32_second
HOST_KERNEL_DIR=/lib/modules/$(shell uname -r)/build/

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) clean
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) help
host:
	make -C $(HOST_KERNEL_DIR) M=$(PWD) modules
