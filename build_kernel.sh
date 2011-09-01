#!/bin/bash
# modified by k0nane

#Set CPU Environment Variable
if [ "$CPU_JOB_NUM" = "" ] ; then
        CPU_JOB_NUM=4
fi


Usage()
{
echo "build_kernel.sh - building script android kernel"
echo "  Usage: ./build_kernel.sh "
echo

exit 1
}

OPTION=-k
PRODUCT=r910

case "$PRODUCT" in
	
    r910)		
                MODULES="onedram_svn onedram dpram_recovery samsung"
                KERNEL_DEF_CONFIG=k0nane_defconfig
                ;;
    
	*)
		Usage
		;;
esac 

if [ ! $PWD_DIR ] ; then
	PWD_DIR=$(pwd)
fi

KERNEL_DIR=$PWD_DIR/Kernel
MODULES_DIR=$PWD_DIR/modules
BUILDS_DIR=~/android/builds


prepare_kernel()
{
	echo "*************************************"
	echo "*          prepare kernel           *"
	echo "*************************************"
	echo

        make distclean
	make -C $KERNEL_DIR ARCH=arm $KERNEL_DEF_CONFIG
	if [ $? != 0 ] ; then
	    exit 1
	fi
	make -C $KERNEL_DIR ARCH=arm prepare
	if [ $? != 0 ] ; then
	    exit 1
	fi
}

build_modules()
{
	echo "*************************************"
	echo "*      module building disabled     *"
	echo "*************************************"
	echo

	# make -C $KERNEL_DIR ARCH=arm $KERNEL_DEF_CONFIG	
	# if [ $? != 0 ] ; then
	#    exit 1
	# fi
	# make -C $KERNEL_DIR ARCH=arm KBUILD_MODPOST_WARN=1 modules
	# if [ $? != 0 ] ; then
	#    exit 1
	# fi

	# for module in $MODULES
	# do
	# 	echo cd $MODULES_DIR/$module
	#	cd $MODULES_DIR/$module
	#	make KDIR=$KERNEL_DIR
	#done 
	
	# Disabled by k0nane - we don't need these
}


build_kernel()
{
	if [ ! -f $KERNEL_DIR/.config ] ; then
		if [ ! -f $KERNEL_DIR/scripts/mod/modprobe ] ; then
			prepare_kernel
		fi
	fi

	echo "make " -C $KERNEL_DIR ARCH=arm $KERNEL_DEF_CONFIG
	make -C $KERNEL_DIR ARCH=arm $KERNEL_DEF_CONFIG
	if [ $? != 0 ] ; then
	    exit 1
	fi

	build_modules

	echo "*************************************"
	echo "*           build kernel            *"
	echo "*************************************"
	echo
	
	cd $KERNEL_DIR

	make -j$CPU_JOB_NUM
	
	# Copy zImage to builds directory
	if [ -e $KERNEL_DIR/arch/arm/boot/zImage ] ; then
		cp $KERNEL_DIR/arch/arm/boot/zImage $BUILDS_DIR/zImage_`date +%Y%m%d%H%M`
	fi
	
	echo Built image is zImage_`date +%Y%m%d%H%M`
	if [ $? != 0 ] ; then
		exit $?
	fi
}



case "$OPTION" in
	-k)
		build_kernel
		;;
	*)
		Usage
		;;
esac 

exit 0
