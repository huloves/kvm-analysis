# 如果未定义 TC_PATH，则设置默认值
ifndef TC_PATH
ROOT_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
TC_PATH := $(abspath $(ROOT_DIR)/../../toolchains/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu)
endif

# 如果未定义 BUILD_DIR，则设置默认值
ifndef BUILD_DIR
BUILD_DIR := $(abspath $(ROOT_DIR)/../../build)
endif

# 确保构建目录存在
$(shell mkdir -p $(BUILD_DIR))

CC ?= $(TC_PATH)/bin/aarch64-none-linux-gnu-gcc
LD ?= $(TC_PATH)/bin/aarch64-none-linux-gnu-ld
OBJCOPY ?= $(TC_PATH)/bin/aarch64-none-linux-gnu-objcopy

.PHONY: all clean

all: $(BUILD_DIR)/tiny_kernel.bin

# 将中间文件输出到 BUILD_DIR
$(BUILD_DIR)/tiny_kernel.bin: tiny_kernel.S tiny_kernel.ld
	@echo "Compiling tiny_kernel.S..."
	$(CC) -nostdlib -nostartfiles -nostdinc -c tiny_kernel.S -o $(BUILD_DIR)/tiny_kernel.o
	@echo "Linking tiny_kernel..."
	$(LD) -nostdlib $(BUILD_DIR)/tiny_kernel.o -T tiny_kernel.ld -o $(BUILD_DIR)/tiny_kernel.tmp.o
	@echo "Generating binary..."
	$(OBJCOPY) -O binary $(BUILD_DIR)/tiny_kernel.tmp.o $(BUILD_DIR)/tiny_kernel.bin

clean:
	rm -f \
		$(BUILD_DIR)/tiny_kernel.o \
		$(BUILD_DIR)/tiny_kernel.tmp.o \
		$(BUILD_DIR)/tiny_kernel.bin
