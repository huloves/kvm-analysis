# 获取主目录的绝对路径
ROOT_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

# 定义工具链路径和下载 URL
TC_PATH := $(abspath $(ROOT_DIR)/../toolchains/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu)
CC := $(TC_PATH)/bin/aarch64-none-linux-gnu-gcc
TC_URL := https://github.com/huloves/toolchains/releases/download/v0.1/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu.tar.xz

# 定义包含目录和构建输出目录
INCLUDE_DIR := $(abspath $(ROOT_DIR)/../include)
INCLUDES := -I$(INCLUDE_DIR)
BUILD_DIR := $(abspath $(ROOT_DIR)/../build)

# 确保构建目录存在
$(shell mkdir -p $(BUILD_DIR))

.PHONY: all clean toolchain

all: $(BUILD_DIR)/get_version

# 编译 get_version 并将输出放入 BUILD_DIR
$(BUILD_DIR)/get_version: get_version.c toolchain
	@echo "Compiling get_version.c..."
	$(CC) $(INCLUDES) get_version.c -g -o $(BUILD_DIR)/get_version

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

clean:
	rm -f $(BUILD_DIR)/get_version
