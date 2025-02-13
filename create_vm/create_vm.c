#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/errno.h>
#include <linux/kvm.h>

#define DEFAULT_KVM_DEV		"/dev/kvm"

int main(int argc, char *argv[])
{
	int sys_fd, vm_fd;
	int errno;
	long kvm_api_version;
	int ret;

	sys_fd = open(DEFAULT_KVM_DEV, O_RDWR);
	if (sys_fd < 0) {
		if (errno == ENOENT)
			printf("'%s' not found. Please make sure your kernel has CONFIG_KVM "
			       "enabled and that the KVM modules are loaded.", DEFAULT_KVM_DEV);
		else if (errno == ENODEV)
			printf("'%s' KVM driver not available.\n  # (If the KVM "
			       "module is loaded then 'dmesg' may offer further clues "
			       "about the failure.)", DEFAULT_KVM_DEV);
		else
			printf("Could not open %s: ", DEFAULT_KVM_DEV);
		
		return -errno;
	}

	kvm_api_version = ioctl(sys_fd, KVM_GET_API_VERSION, 0);
	if (kvm_api_version != KVM_API_VERSION) {
		printf("KVM_API_VERSION ioctl");
		close(sys_fd);
		return -errno;
	}

	printf("kvm_api_verison = %d\n", kvm_api_version);

	vm_fd = ioctl(sys_fd, KVM_CREATE_VM, 0);
	if (vm_fd < 0) {
		printf("KVM_CREATE_VM ioctl");
		ret = vm_fd;
		goto err_sys_fd;
	}

err_sys_fd:
	close(sys_fd);
	return ret;
}
