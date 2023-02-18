#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-
ROOTFS=${OUTDIR}/rootfs

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    #Clean
    echo "-----Cleaning-----"
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- mrproper
    #defconfig
    echo "-----defconfig-----"
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- defconfig
    # build vmlinux
    echo "-----vmlinux-----"
    make -j4 ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- all
    # build module
    echo "-----module-----"
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- modules
    # build devicetree
    echo "-----devicetree-----"
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- dtbs 
   
fi

echo "Copy resulting linux build image to ${OUTDIR}"
cp ${OUTDIR}/linux-stable/arch/arm64/boot/Image ${OUTDIR}


echo "Adding the Image in outdir"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
  mkdir ${OUTDIR}/rootfs
  cd ${OUTDIR}/rootfs
  mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
  mkdir -p usr/bin usr/lib usr/sbin
  mkdir -p var/log


cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    # Configure BusyBox
    
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig

    # Customize BusyBox configuration
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} menuconfig



else
    cd busybox
fi

# TODO: Make and install busybox
# Build BusyBox
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
# Install BusyBox
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} CONFIG_PREFIX=${OUTDIR}/rootfs install

echo "Library dependencies"
${CROSS_COMPILE}readelf -a ${OUTDIR}/rootfs/bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a ${OUTDIR}/rootfs/bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs

cd ${OUTDIR}/rootfs


SYSROOT=$(${CROSS_COMPILE}gcc -print-sysroot)
cp ${SYSROOT}/lib/ld-linux-aarch64.so.1 ./lib/
cp ${SYSROOT}/lib64/libm.so.6 ./lib64/
cp ${SYSROOT}/lib64/libresolv.so.2 ./lib64/
cp ${SYSROOT}/lib64/libc.so.6 ./lib64/


# TODO: Make device nodes
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

# TODO: Clean and build the writer utility

cd $FINDER_APP_DIR
make clean
make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE


# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
ROOTFS_HOME=$OUTDIR/rootfs/home
cp finder.sh finder-test.sh writer ${OUTDIR}/rootfs/home
mkdir $ROOTFS_HOME/conf
cp conf/username.txt $ROOTFS_HOME/conf
cp conf/assignment.txt $ROOTFS_HOME/conf
cp autorun-qemu.sh $ROOTFS_HOME

# TODO: Chown the root directory
sudo chown -R root:root *


# TODO: Create initramfs.cpio.gz
cd ${OUTDIR}/rootfs
echo "Create initramfs.cpio.gz"
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
gzip -f ${OUTDIR}/initramfs.cpio

