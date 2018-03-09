SHELL = /bin/bash

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
KERNEL_DIR = /home/$(USER)/kernel/linux-3.4
#KERNEL_DIR = ~/kernel/linux-3.4
TARGET_DIR = /mnt
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

#########################################################

LED_DEVICE_RELYON := chip/$(PLATFORM)/gpio.o

obj-m += led/led_gpio.o
led/led_gpio-objs := $(LED_DEVICE_RELYON)

#########################################################

OLED_DEVICE_RELYON := chip/$(PLATFORM)/gpio.o
OLED_DEVICE_RELYON += i2c/i2c_gpio.o

obj-m += oled/oled_gpio.o
oled/oled_gpio-objs := $(OLED_DEVICE_RELYON)

#########################################################

MPU_DEVICE_RELYON := chip/$(PLATFORM)/gpio.o
MPU_DEVICE_RELYON += i2c/i2c_gpio.o

obj-m += mpu6050/mpu6050_gpio.o
mpu6050/mpu6050_gpio-objs := $(MPU_DEVICE_RELYON)

#########################################################

#COLOR_NORMAL = \e[0m
#COLOR_YELLOW = \e[1;33m
#COLOR_RED = \e[1;31m
#COLOR_BLUE = \e[1;34m
COLOR_NORMAL = \033[0m
COLOR_RED = \033[1;31m
COLOR_YELLOW = \033[1;33m
COLOR_BLUE = \033[1;34m

all: notice
	@echo "###############################################"
	@echo -e "$(COLOR_YELLOW)make modules$(COLOR_NORMAL)"
	make -C $(KERNEL_DIR) M=`pwd` modules
	-@mkdir -p build
	-find ./ -name "*.ko" | xargs -n1 -I {} sudo cp {} build -rf

clean:
	@echo "###############################################"
	@echo -e "$(COLOR_YELLOW)make clean$(COLOR_NORMAL)"
	-make -C $(KERNEL_DIR) M=`pwd` clean
	-rm build -rf
	-make -C test clean
	-make -C applicant clean

distclean:
	@echo "###############################################"
	@echo -e "$(COLOR_YELLOW)make clean$(COLOR_NORMAL)"
	-make -C $(KERNEL_DIR) M=`pwd` clean
	-cd test && make clean && cd ..
	-cd applicant && make clean && cd ..

app:
	@echo "###############################################"
	@echo -e "$(COLOR_YELLOW)make app$(COLOR_NORMAL)"
	-make -C applicant

demo:
	@echo "###############################################"
	@echo -e "$(COLOR_YELLOW)make test$(COLOR_NORMAL)"
	-make -C test

install:
	@echo "###############################################"
	@echo -e "$(COLOR_YELLOW)make install$(COLOR_NORMAL)"
	@#-find ./ -name "*.ko" | xargs -n1 -I {} chmod a+x {}
	-find ./ -name "*.ko" | xargs -n1 -I {} sudo cp {} $(TARGET_DIR) -rf
	-make -C test install
	-make -C applicant install

notice:
	@echo "###############################################"
	@echo -e "$(COLOR_RED)platform = $(PLATFORM)$(COLOR_NORMAL)"

