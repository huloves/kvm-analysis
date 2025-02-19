KDIR := $(KERNEL_PATH)

# 检查 KERNEL_PATH 是否定义
ifeq ($(KDIR),)
$(error "KERNEL_PATH is not defined. Please specify the kernel path.")
endif

obj-m += virq_hwirq.o

all:
	make -C $(KDIR) M=$(shell pwd) modules
clean:
	make -C $(KDIR) M=$(shell pwd) clean
