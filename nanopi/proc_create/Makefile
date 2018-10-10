
ifneq ($(KERNELRELEASE),)
	obj-m := proc_create.o

else
	KERNEL_DIR := /home/mcy/kernel/linux-3.4 
	CROSS_COMPILE := arm-cortexa9-linux-gnueabihf-

all:
	make ARCH=arm CROSS_COMPILE=arm-cortexa9-linux-gnueabihf- -C $(KERNEL_DIR) M=`pwd` modules

clean:
	make -C $(KERNEL_DIR) M=`pwd` clean

install:
	sudo cp *.ko /home/mcy/qemu/rootfs/mcy/ -rf
endif

