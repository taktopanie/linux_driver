obj-m := pcd_platform_driver.o 

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

KERNEL_DIR=/home/maciej/Desktop/moje/programming/Linux/device_driver/rpi_kernel/linux_32_second
HOST_KERNEL_DIR=/lib/modules/5.15.0-127-generic/build/

filename=

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) clean
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) help
host:
	make -C $(HOST_KERNEL_DIR) M=$(PWD) modules
target_mod_copy:
	scp  $(PWD)/*.ko pi@192.168.3.215:/home/pi/Desktop/from_host/
target_dt_copy:
	scp $(PWD)/*.dtb pi@192.168.3.215:/home/pi/Desktop/from_host/
build_dtbo:
	dtc -@ -I dts -O dtb -o $(filename).dtbo $(filename).dts
