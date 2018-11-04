
ifneq ($(KERNELRELEASE),)
	obj-m := 01proc.o

else
	KERNEL_DIR := /home/mcy/kernel/linux-3.5
#	KERNEL_DIR := /home/mcy/qemu/kernel/linux-3.5
	CROSS_COMPILE := arm-linux-

all:
	make ARCH=arm -C $(KERNEL_DIR) M=`pwd` modules

clean:
	make -C $(KERNEL_DIR) M=`pwd` clean

install:
	sudo cp *.ko /home/mcy/qemu/rootfs/mcy/ -rf
endif

