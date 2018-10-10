
ifneq ($(KERNELRELEASE),)
	obj-m := printk_test.o

else
	KERNEL_DIR := /home/mcy/qemu/kernel/linux-4.4.157
	CROSS_COMPILE := arm-linux-gnueabi-

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- -C $(KERNEL_DIR) M=`pwd` modules

clean:
	make -C $(KERNEL_DIR) M=`pwd` clean

install:
	sudo cp *.ko /home/mcy/qemu/rootfs/mcy/ -rf
endif

