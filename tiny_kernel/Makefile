CC=../../toolchains/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc
OBJCOPY=../../toolchains/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-objcopy
LD=../../toolchains/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-ld

all: tiny_kernel.bin

tiny_kernel.bin: tiny_kernel.S
	$(CC)  -nostdlib -nostartfiles -nostdinc -c tiny_kernel.S -o tiny_kernel.o
	$(LD) -nostdlib tiny_kernel.o -T tiny_kernel.ld -o tiny_kernel.tmp.o
	$(OBJCOPY) -O binary tiny_kernel.tmp.o tiny_kernel.bin

clean:
	rm tiny_kernel.bin tiny_kernel.o tiny_kernel.tmp.o