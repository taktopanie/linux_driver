obj-m := my_gpio.o 

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

KERNEL_DIR=/home/mac/Desktop/Moje/programowawnie/linux/rpi_kernel/linux-stable_20250127/

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) clean
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) help
host:
	make -C $(HOST_KERNEL_DIR) M=$(PWD) modules
target_mod_copy:
	scp  $(shell pwd)/*.ko pi@192.168.100.115:/home/pi/Desktop/from_host/
target_dt_copy:
	scp $(shell pwd)/*.dtb pi@192.168.100.115:/home/pi/Desktop/from_host/

