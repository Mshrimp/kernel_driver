def_chip = h3

ifeq ($(chip),)
chip = $(def_chip)
endif

ifeq ($(chip),hi3518ev200)
export NV_CROSS_COMPILE?=arm-hisiv300-linux
TARGET_DIR=/mnt/nfs/${USER}/hi3518
endif

ifeq ($(chip),h3)
export NV_CROSS_COMPILE?=arm-cortexa9-linux-gnueabihf
TARGET_DIR=/mnt
endif

BIN_SOURCE:=$(wildcard ./app_*.c)
BIN_TARGET:=$(patsubst ./app_%.c, ./build/app_%, $(BIN_SOURCE))

export ARCH = arm
export CC = $(NV_CROSS_COMPILE)-gcc

all:notice $(BIN_TARGET)

./build/app_%:./app_%.c
	@mkdir -p build
	$(CC) -o $@ $<
	@chmod a+x $@

notice:
	@echo ""
	@echo "***************** make app ****************"
	@echo "make chip=xxx"
	@echo "xxx = hi3518ev200 or allwinner-h3"
	@echo "make clean"
	@echo "chip=$(chip)"
	@echo "Cross_Compile=$(NV_CROSS_COMPILE)"
	@echo "******************************************"

clean:
	-rm build -rf
	@echo clear done

install:
	-sudo cp build/app_* $(TARGET_DIR) -rf
