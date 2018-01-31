def_chip = hi3518ev200

ifeq ($(chip),)
chip = $(def_chip)
endif


ifeq ($(chip),hi3518ev100)
export NV_CROSS_COMPILE?=arm-hisiv100nptl-linux
endif

ifeq ($(chip),hi3518ev200)
export NV_CROSS_COMPILE?=arm-hisiv300-linux
endif

ifeq ($(chip),goke)
export NV_CROSS_COMPILE?=arm-goke-linux-uclibcgnueabi
endif

TARGET_DIR=/mnt/nfs/${USER}/hi3518
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
	@echo "***************** example ****************"
	@echo "make chip=xxx"
	@echo "xxx = hi3518ev100 or hi3518ev200 or goke"
	@echo "make clean"
	@echo "chip=$(chip)"
	@echo "******************************************"

head:
	@rm $(TEST_HEAD_LN_FILE) -rf
	#@ln -s $(COMMON_HEAD_FILE) $(TEST_HEAD_LN_FILE)

clean:
	-rm build -rf
	@echo clear done

install:
	-cp build/app_* $(TARGET_DIR) -rf
