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

COLOR_NORMAL = \e[0m
COLOR_RED = \e[1;31m
COLOR_YELLOW = \e[1;33m
COLOR_BLUE = \e[1;34m

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

all: notice
	@echo -e "$(COLOR_YELLOW)make modules$(COLOR_NORMAL)"
	make -C $(KERNEL_DIR) M=`pwd` modules

clean:
	@echo -e "$(COLOR_YELLOW)make clean$(COLOR_NORMAL)"
	make -C $(KERNEL_DIR) M=`pwd` clean
	cd test && make clean
	-@rm chip/chip.h


app:
	@echo -e "$(COLOR_YELLOW)make app$(COLOR_NORMAL)"
	cd test && make

install:
	@echo -e "$(COLOR_YELLOW)make install$(COLOR_NORMAL)"
	-cp *.ko $(TARGET_DIR) -rf
	cd test && make install

notice:
	@echo -e "$(COLOR_RED)platform = $(PLATFORM)$(COLOR_NORMAL)"

