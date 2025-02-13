obj-m := device.o

KERNEL_DIR = /usr/lib/modules/5.15.0-127-generic/build

all:
	make -C $(KERNEL_DIR) M=$(shell pwd) modules

clean:
	make -C $(KERNEL_DIR) M=$(shell pwd) clean
