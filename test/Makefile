def_chip = hi3518ev200

ifeq ($(chip),)
chip = $(def_chip)
endif


ifeq ($(chip),hi3518ev100)
export NV_CROSS_COMPILE?=arm-hisiv100nptl-linux
TARGET_DIR=/mnt/nfs/${USER}/hi3518
endif

ifeq ($(chip),hi3518ev200)
export NV_CROSS_COMPILE?=arm-hisiv300-linux
TARGET_DIR=/mnt/nfs/${USER}/hi3518
endif

ifeq ($(chip),goke)
export NV_CROSS_COMPILE?=arm-goke-linux-uclibcgnueabi
endif

ifeq ($(chip),h3)
export NV_CROSS_COMPILE?=arm-cortexa9-linux-gnueabihf
TARGET_DIR=/mnt
endif

COMMON_HEAD_FILE=../common.h
TEST_HEAD_LN_FILE=common.h
BIN_SOURCE:=$(wildcard ./app_*.c)
BIN_TARGET:=$(patsubst ./app_%.c, ./build/app_%, $(BIN_SOURCE))

export ARCH = arm
export CC = $(NV_CROSS_COMPILE)-gcc

all:notice head $(BIN_TARGET)

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

head:
	@rm $(TEST_HEAD_LN_FILE) -rf
	#@ln -s $(COMMON_HEAD_FILE) $(TEST_HEAD_LN_FILE)

clean:
	-rm build -rf
	@echo clear done

install:
	-sudo cp build/app_* $(TARGET_DIR) -rf
