CC=../toolchains/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc

INCLUDES = -I./include

all: kvm_sample

kvm_sample: kvm_sample.c
	$(CC) $(INCLUDES) kvm_sample.c --static -g -o kvm_sample

get_version: get_version.c
	$(CC) $(INCLUDES) get_version.c -g -o get_version

clean:
	rm get_version kvm_sample
