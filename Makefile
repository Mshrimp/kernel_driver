def_chip := h3

chip ?= $(def_chip)

ifeq ($(chip),hi3518ev200)
PLATFORM := hi3518ev200
endif

ifeq ($(chip),h3)
PLATFORM := allwinner-h3
endif

#########################################################

ifeq ($(PLATFORM),hi3518ev200)
KERNEL_DIR = ~/netview/nvc-hi3518ev200-kernel/linux-3.4.y
TARGET_DIR = /mnt/nfs/mcy/hi3518
CROSS_COMPLITE = arm-hisiv300-linux-

DRIVER_NAME = hi3518ev200
endif

ifeq ($(PLATFORM),allwinner-h3)
KERNEL_DIR = /home/mcy/kernel/linux-3.4
#KERNEL_DIR = ~/kernel/linux-3.4
TARGET_DIR = /mnt/nfs/mcy/hi3518
CROSS_COMPLITE = arm-cortexa9-linux-gnueabihf-

DRIVER_NAME = allwinner-h3
endif

#########################################################

DRV_SRC_DIR = driver
CHIP_SRC_DIR = chip/$(PLATFORM)

#########################################################

define check_chip
	if [ ! -d $(CHIP_SRC_DIR) ]; fi
endef

#########################################################

CC = $(CROSS_COMPLITE)gcc

#DRIVER_NAME = hi3518ev200

DEVICE_RELYON := driver/driver.o
DEVICE_RELYON += chip/$(PLATFORM)/gpio.o
DEVICE_RELYON += led/led.o
#DEVICE_RELYON += i2c/i2c_gpio.o
#DEVICE_RELYON += i2c/i2c.o
#DEVICE_RELYON += fm36/fm36.o

obj-m := $(DRIVER_NAME).o
$(DRIVER_NAME)-objs := $(DEVICE_RELYON)

all:
	make -C $(KERNEL_DIR) M=`pwd` modules

clean:
	make -C $(KERNEL_DIR) M=`pwd` clean
	cd test && make clean


app:
	cd test && make

install:
	-cp *.ko $(TARGET_DIR) -rf
	cd test && make install
