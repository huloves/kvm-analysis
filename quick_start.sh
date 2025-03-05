#!/bin/bash

PWD="`pwd`"

if [ -z $ARCH ]; then
	ARCH="arm64"
fi

# 获取脚本所在目录的绝对路径
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

# 定义工具链路径和下载 URL
TOOLCHAIN_DIR="${SCRIPT_DIR}/toolchains/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu"
TOOLCHAIN_URL="https://github.com/huloves/toolchains/releases/download/v0.1/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu.tar.xz"
CC="${TOOLCHAIN_DIR}/bin/aarch64-none-linux-gnu-gcc"

# 检查工具链是否存在，如果不存在则下载
if [ ! -f "$CC" ]; then
	echo "Toolchain not found. Downloading..."
	mkdir -p "$(dirname "$TOOLCHAIN_DIR")"
	wget -O toolchain.tar.xz "$TOOLCHAIN_URL"
	tar -xf toolchain.tar.xz -C "$(dirname "$TOOLCHAIN_DIR")"
	rm toolchain.tar.xz
	echo "Toolchain installed at $TOOLCHAIN_DIR"
else
	echo "Toolchain found at $CC"
fi

if [ "$ARCH" == "arm64" ]; then
	# 拼接绝对路径
	TOOLCHAIN="${SCRIPT_DIR}/toolchains/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-"
fi

function linux_vm()
{
	# 编译linux内核
	cd linux-5.9
	ARCH=$ARCH CROSS_COMPILE=$TOOLCHAIN make defconfig
	ARCH=$ARCH CROSS_COMPILE=$TOOLCHAIN make all -j32
	cd -

	# 解压rootfs.cpio
	rm -rf rootfs
	mkdir -p rootfs/temp/
	cd rootfs/temp/
	cp ../../rootfs.cpio ../
	cpio -idmv < ../rootfs.cpio
	cd -

	qemu-system-aarch64 \
		-cpu cortex-a53 \
		-m 512M \
		-machine type=virt,virtualization=on \
		-nographic \
		-smp 1 \
		-kernel linux-5.9/arch/arm64/boot/Image  \
		-initrd rootfs/rootfs.cpio   \
		-append "rdinit=/linuxrc console=ttyAMA0"
	
	# 虚拟机内执行该命令
	# ./lkvm run -k Image -i rootfs.kvm.cpio -p "rdinit=/linuxrc console=ttyAMA0" -m 320 -c 1 --name guest-18
}

function mini_vm()
{
	# 编译mini_vm应用程序
	cd mini_vm/
	make clean
	make all
	cd -

	# 编译linux内核
	cd linux-5.9/
	ARCH=$ARCH CROSS_COMPILE=$TOOLCHAIN make defconfig
	ARCH=$ARCH CROSS_COMPILE=$TOOLCHAIN make all -j32
	cd -

	# 解压rootfs.cpio到rootfs/temp目录
	rm -rf rootfs
	mkdir -p rootfs/temp/
	cd rootfs/temp/
	cp ../../rootfs.cpio ../
	cpio -idmv < ../rootfs.cpio

	# 将tiny_kvmtool和tiny_kernel打包进cpio中
	cp ${SCRIPT_DIR}/build/tiny_kvmtool ${SCRIPT_DIR}/build/tiny_kernel.bin ./
	cd ..
	( cd temp && find . | cpio -o -H newc ) > rootfs.cpio
	cd ..

	qemu-system-aarch64 \
		-cpu cortex-a53 \
		-m 512M \
		-machine type=virt,virtualization=on \
		-nographic \
		-smp 1 \
		-kernel linux-5.9/arch/arm64/boot/Image  \
		-initrd rootfs/rootfs.cpio   \
		-append "rdinit=/linuxrc console=ttyAMA0"
	
	# 虚拟机内执行该命令
	# ./tiny_kvmtool
}

function virq_hwirq()
{
	ABS_LINUX_PATH=$(realpath linux-5.9/)

	# 交叉编译内核模块准备工作
	cd linux-5.9/
	ARCH=$ARCH CROSS_COMPILE=$TOOLCHAIN make defconfig
	ARCH=$ARCH CROSS_COMPILE=$TOOLCHAIN make prepare
	ARCH=$ARCH CROSS_COMPILE=$TOOLCHAIN make scripts
	ARCH=$ARCH CROSS_COMPILE=$TOOLCHAIN make modules
	cd -
	
	# 交叉编译内核模块
	cd vgicv2/virq_hwirq_module/
	ARCH=$ARCH CROSS_COMPILE=$TOOLCHAIN KERNEL_PATH=$ABS_LINUX_PATH make all
	cd -

	# 解压rootfs.cpio到rootfs/temp目录
	rm -rf rootfs
	mkdir -p rootfs/temp/
	cd rootfs/temp/
	cp ../../rootfs.cpio ../
	cpio -idmv < ../rootfs.cpio

	# 将hwirq_module打包进cpio中
	cp -r ${SCRIPT_DIR}/vgicv2/virq_hwirq_module ./
	cd ..
	( cd temp && find . | cpio -o -H newc ) > rootfs.cpio
	cd ..

	cd vgicv2/virq_hwirq_module/
	ARCH=$ARCH CROSS_COMPILE=$TOOLCHAIN KERNEL_PATH=$ABS_LINUX_PATH make clean
	cd -

	qemu-system-aarch64 \
		-cpu cortex-a53 \
		-m 512M \
		-machine type=virt,virtualization=on \
		-nographic \
		-smp 1 \
		-kernel linux-5.9/arch/arm64/boot/Image  \
		-initrd rootfs/rootfs.cpio   \
		-append "rdinit=/linuxrc console=ttyAMA0"
}

#
# 将字符串转换为函数，并调用函数
#
function call_sub_cmd()
{
	#
	# 通过第一个参数，获得想要调用的函数名
	# 例如 check 函数
	#
	func=$1
	#
	# 函数名不支持”-“，因此将参数中的”-“转换为”_“
	#
	func=${func//-/_}
	#
	# 从参数列表中移除第一个参数，例如 check，将剩余的参数传给函数
	#
	shift 1
	eval "$func $*"
}

#
# 主函数
#
function main()
{
	#
	# 如果没有任何参数，默认调用all函数
	#
	if [ $# -eq 0 ]; then
		all
		exit 0
	fi

	#
	# 带参数运行，看看相应的函数是否存在
	#
	SUB_CMD=$1
	type ${SUB_CMD//-/_} > /dev/null 2>&1
	#
	# 要调用的子函数不存在，说明用法错误
	#
	if [ $? -ne 0 ]; then
		usage
		exit
	else
		#
		# 要调用的子函数存在，执行子函数
		#
		shift 1;
		call_sub_cmd $SUB_CMD $*
		exit $?
	fi

	usage
}

#
# 调用主函数
#
main $*
