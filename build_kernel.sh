#!/bin/bash
# Modifed from Samsung original by k0nane :: k0@k0nane.info

# Save current working directory
CWD=$PWD

# Set environment variables

KERNEL_DIR=$CWD/Kernel
BUILDS_DIR=~/android/builds
DEFCONFIG=indulge_recovery_defconfig
CROSS_COMPILER=/opt/toolchains/arm-2009q3/bin/arm-none-linux-gnueabi-
THREADS=4
GENERATE_ZIP=y
BUILD_SUFFIX=`date +%Y%m%d%H%M`

prepare_kernel()
{
	echo 
	echo "/*        Preparing kernel...        */"
	echo 
	echo

    make distclean
	make -j $THREADS -C $KERNEL_DIR ARCH=arm $DEFCONFIG
	if [ $? != 0 ] ; then
	    exit 1
	fi
	make -j $THREADS -C $KERNEL_DIR ARCH=arm prepare
	if [ $? != 0 ] ; then
	    exit 1
	fi
}

build_kernel()
{
	if [ ! -f $KERNEL_DIR/.config ] ; then
		if [ ! -f $KERNEL_DIR/scripts/mod/modprobe ] ; then
			prepare_kernel
		fi
	fi

	make -j $THREADS -C $KERNEL_DIR ARCH=arm $DEFCONFIG
	if [ $? != 0 ] ; then
	    exit 1
	fi

	echo 
	echo "/*        Building kernel...         */"
	echo
	
	cd $KERNEL_DIR

	make -j$THREADS CROSS_COMPILE="$CROSS_COMPILER"
	
	if [ "$GENERATE_ZIP" = y ] ; then
		if [ -f $KERNEL_DIR/arch/arm/boot/zImage ] ; then

			echo 
			echo "/*        Creating zip...         */"
			echo
		
			# Generate build zip from built zImage
			mv $KERNEL_DIR/arch/arm/boot/zImage $CWD/zipcreate
			cd $CWD/zipcreate
			zip -r zImage_$BUILD_SUFFIX.zip *
			rm zImage
			mv zImage_$BUILD_SUFFIX.zip $BUILDS_DIR
			echo 
			echo "zImage_$BUILD_SUFFIX.zip created."	
		fi
	fi
	
	if [ $? != 0 ] ; then
		exit $?
	fi
}

echo "**************************************"
echo "* Electric Sheep kernel build script *"
echo "*    by k0nane :: k0@k0nane.info     *"
echo "**************************************"
echo

build_kernel

exit 0
