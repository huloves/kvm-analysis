#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>

#define KVM_DEV  "/dev/kvm"
#define MEM_SIZE  0x1000
#define PHY_ADDR  0x80000

#define AARCH64_CORE_REG(x)        (KVM_REG_ARM64 | KVM_REG_SIZE_U64 | KVM_REG_ARM_CORE | KVM_REG_ARM_CORE_REG(x))

int main(void)
{
	struct kvm_one_reg reg;
	struct kvm_vcpu_init init; //using init the vcpu type
	struct kvm_vcpu_init preferred;
	int ret;

	__u64 guest_entry = PHY_ADDR;
	__u64 guest_pstate;

	int kvmfd = open(KVM_DEV, O_RDWR);

	//ioctl(kvmfd, KVM_GET_API_VERSION, NULL);
	//1. create vm and get the vm fd handler
	int vmfd =  ioctl(kvmfd, KVM_CREATE_VM, 0);

	//2. create vcpu
	int vcpufd = ioctl(vmfd, KVM_CREATE_VCPU, 0);
	if(vcpufd < 0) {
		printf("create vcpu failed\n");
		return -1;
	}

	//3. arm64 type vcpu type init
	//sample code can check the qemu/target/arm/kvm64.c
	memset(&init, 0, sizeof(init));
	//init.target = KVM_ARM_TARGET_GENERIC_V8; //here set KVM_ARM_TARGET_CORTEX_A57 meet failed
	init.target = -1;
	if (init.target == -1) {
		ret = ioctl(vmfd, KVM_ARM_PREFERRED_TARGET, &preferred);
		if(!ret) {
		init.target = preferred.target; //KVM_ARM_TARGET_GENERIC_V8
		printf("preferred vcpu type %d\n", init.target);
		}
	}

	ret = ioctl(vcpufd, KVM_ARM_VCPU_INIT, &init);

	if (ret  < 0) {
		printf("init vcpu type failed\n");
		return -1;
	}

	//4. get vcpu resouce map size and get vcpu  kvm_run status  
	int mmap_size = ioctl(kvmfd, KVM_GET_VCPU_MMAP_SIZE, NULL);

	if (mmap_size  < 0) {
		printf("get vcpu mmap size failed\n");
		return -1;
	}

	struct kvm_run *run = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpufd, 0);


	//5. load the vm running program to buffer 'ram'
	unsigned char *ram = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, 
		MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	int kfd = open("tiny_kernel.bin", O_RDONLY);
	read(kfd, ram, MEM_SIZE);

	struct kvm_userspace_memory_region mem = {
		.slot = 0,
		.flags = 0,
		.guest_phys_addr = PHY_ADDR,
		.memory_size = MEM_SIZE,
		.userspace_addr = (unsigned long)ram,
	};

	//6. set the vm userspace program ram to vm fd handler
	ret = ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &mem);

	if(ret < 0) {
		printf("set user memory region failed\n");
		return -1;
	}

	//7. change the vcpu register info, arm64 need change the pc value 
	// arm64 not support KVM_SET_REGS, and KVM_SET_SREGS only support at x86 ppc arch
	// arm64 using the KVM_SET_ONE_REG

	//sample code from qemu/target/arm/kvm64.c
	reg.id = AARCH64_CORE_REG(regs.pc);
	reg.addr = (__u64)&guest_entry;
	ret = ioctl(vcpufd, KVM_SET_ONE_REG, &reg);

	if(ret  < 0) {
		printf("change arm64 pc reg failed err %d\n", ret);
		return -1;
	} else {
		printf("set pc addr success\n");
	}

	//7. run the vcpu and get the vcpu result
	while(1) {
		ret = ioctl(vcpufd, KVM_RUN, NULL);
		if (ret == -1) {
			printf("exit unknow\n");
			return -1;
		}

		switch(run->exit_reason) {
		//when guest program meet io access error will trigger KVM_EXIT_MMIO event, 
		//using the event get guest program output
		case KVM_EXIT_MMIO:
			if (run->mmio.is_write && run->mmio.len == 1) {
			printf("%c", run->mmio.data[0]);
			}
			break;
		case KVM_EXIT_FAIL_ENTRY:
			puts("entry error");
			return -1;
		default:
			printf("exit_reason: %d\n", run->exit_reason);
			return -1;
		}
	}
	return 0;
}
