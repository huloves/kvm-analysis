# 获取主目录的绝对路径
ROOT_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

# 定义工具链路径和下载 URL
TC_PATH := $(abspath $(ROOT_DIR)/../toolchains/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu)
CC := $(TC_PATH)/bin/aarch64-none-linux-gnu-gcc
OBJCOPY := $(TC_PATH)/bin/aarch64-none-linux-gnu-objcopy
LD := $(TC_PATH)/bin/aarch64-none-linux-gnu-ld
TC_URL := https://github.com/huloves/toolchains/releases/download/v0.1/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu.tar.xz

# 定义构建输出目录
BUILD_DIR := $(abspath $(ROOT_DIR)/../build)

# 确保构建目录存在
$(shell mkdir -p $(BUILD_DIR))

.PHONY: all clean toolchain

all: $(BUILD_DIR)/tiny_kvmtool

# 编译 tiny_kvmtool，并将中间文件和输出文件放入 BUILD_DIR
$(BUILD_DIR)/tiny_kvmtool: $(BUILD_DIR)/tiny_kvmtool.o $(BUILD_DIR)/tiny_kernel.bin
	@echo "Linking tiny_kvmtool..."
	$(CC) $(BUILD_DIR)/tiny_kvmtool.o --static -o $(BUILD_DIR)/tiny_kvmtool

# 编译 tiny_kvmtool.o
$(BUILD_DIR)/tiny_kvmtool.o: tiny_kvmtool.c toolchain
	@echo "Compiling tiny_kvmtool.c..."
	$(CC) -c tiny_kvmtool.c -o $(BUILD_DIR)/tiny_kvmtool.o

# 检查工具链是否存在
toolchain:
	if [ ! -f "$(CC)" ]; then \
		echo "Toolchain not found. Downloading..."; \
		mkdir -p $(dir $(TC_PATH)); \
		wget -O toolchain.tar.xz "$(TC_URL)"; \
		tar -xf toolchain.tar.xz -C $(dir $(TC_PATH)); \
		rm toolchain.tar.xz; \
	else \
		echo "Toolchain found at $(CC)"; \
	fi

# 构建 tiny_kernel.bin，并将中间文件放入 BUILD_DIR
$(BUILD_DIR)/tiny_kernel.bin: toolchain
	$(MAKE) -C tiny_kernel \
		CC="$(CC)" LD="$(LD)" OBJCOPY="$(OBJCOPY)" TC_PATH="$(TC_PATH)" BUILD_DIR="$(BUILD_DIR)" all

clean:
	rm -f $(BUILD_DIR)/tiny_kvmtool $(BUILD_DIR)/tiny_kvmtool.o
	$(MAKE) -C tiny_kernel clean BUILD_DIR="$(BUILD_DIR)"
