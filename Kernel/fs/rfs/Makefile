################################################################################
#
# Makefile for the linux rfs-filesystem
#
################################################################################

################################################################################
# Set flags (defined by nestle)
################################################################################

## where to run
EXTRA_CFLAGS += -DCONFIG_LINUX

ifeq ($(CONFIG_S5PV210_VICTORY), y)
################################################################################

CONFIG_RFS_FS_CORE_FAT = m
CONFIG_RFS_FS_FAT_VFAT = y
CONFIG_RFS_FS = m

EXTRA_CFLAGS += -DCONFIG_OND_4K
EXTRA_CFLAGS += -DCONFIG_RFS_FSR_MODULE
EXTRA_CFLAGS += -DCONFIG_RFS_FSR_STL_MODULE
EXTRA_CFLAGS += -DCONFIG_RFS_STL_DELETE
EXTRA_CFLAGS += -DCONFIG_FSR_FLASH_PHYS_ADDR=0xB0000000
EXTRA_CFLAGS += -DCONFIG_LINUSTOREIII_DEBUG_VERBOSE=0
EXTRA_CFLAGS += -DCONFIG_LINUSTOREIII_DEBUG

EXTRA_CFLAGS += -DCONFIG_RFS_FS_SPECIAL 
EXTRA_CFLAGS += -DCONFIG_RFS_FS_PERMISSION 
EXTRA_CFLAGS += -DCONFIG_RFS_FS_SYNC_ON_CLOSE 
EXTRA_CFLAGS += -DCONFIG_RFS_DEFAULT_IOCHARSET=\"utf8\"
EXTRA_CFLAGS += -DCONFIG_RFS_FS_CORE_FAT_MODULE
EXTRA_CFLAGS += -DCONFIG_RFS_FS_FAT_VFAT 
EXTRA_CFLAGS += -DCONFIG_RFS_FS_ERASE_SECTOR 
EXTRA_CFLAGS += -DCONFIG_RFS_FS_MODULE 
################################################################################
endif

## debugging mode
ifeq ($(CONFIG_RFS_FS_DEBUG), y)
include $(srctree)/fs/rfs/Makefile.debug
else
EXTRA_CFLAGS += -DRFS_RELEASE
endif

## Native Filesystem (BTFS)
ifneq ($(CONFIG_RFS_FS_FAT_VFAT), y)
EXTRA_CFLAGS += -DFFAT_VFAT_OFF
endif

EXTRA_CFLAGS += -D_4BYTE_VNODEID

## Notify the change of free block count
EXTRA_CFLAGS += -DRFS_CLUSTER_CHANGE_NOTIFY

## KFT Flag
ifdef CONFIG_KFT_FS_RFS
EXTRA_CFLAGS += -finstrument-functions
endif

################################################################################
# include path
################################################################################

## Base library
EXTRA_CFLAGS += -Ifs/rfs/inc -Ifs/rfs/oal -Ifs/rfs/base 

## Linux glue
EXTRA_CFLAGS += -Ifs/rfs/device -Ifs/rfs

## Native Filesystem
ifneq ($(CONFIG_FS_CORE_FAT), n)
EXTRA_CFLAGS += -Ifs/rfs/nativefs/btfs/inc  
EXTRA_CFLAGS += -Ifs/rfs/nativefs/btfs/core
EXTRA_CFLAGS += -Ifs/rfs/nativefs/btfs/ffatfs
EXTRA_CFLAGS += -Ifs/rfs/nativefs/btfs/global
EXTRA_CFLAGS += -Ifs/rfs/nativefs/btfs/addon
EXTRA_CFLAGS += -Ifs/rfs/nativefs/btfs/vfsal/nestle
endif

################################################################################
# Basic RFS glue module : Base library + Linux glue
################################################################################

obj-$(CONFIG_RFS_FS)    += rfs_glue.o

## Base library
rfs_glue-y	:= $(addprefix base/, \
		linux_util.o \
		ns_misc.o \
		ns_oam.o \
		ess_debug.o \
		ess_pstack.o \
		ess_rbtree2.o \
		ess_dlist.o \
		ess_bitmap.o \
		ess_list.o \
		ess_hash.o \
		ess_lru.o \
		ess_math.o \
		nsd_debug.o \
		nsd_version.o \
		)
	
## Linux glue
rfs_glue-y	+= linux_init_glue.o \
		   linux_rfs_api.o

rfs_glue-y	+= linux_dir.o \
		   linux_vcb.o \
		   linux_file.o \
		   linux_bcache.o \
		   linux_vnode.o \
		   linux_volume.o \
		   linux_fcb.o \
		   linux_debug.o \
		   natives.o

rfs_glue-y	+= device/linux_logdisk.o

rfs_glue-y	+= oal/oam_linux.o


ifeq ($(CONFIG_RFS_FS_XATTR), y)
rfs_glue-y	+= linux_xattr.o
endif

ifeq ($(CONFIG_RFS_FS_POSIX_ACL), y)
rfs_glue-y	+= linux_xattr_acl.o
endif


################################################################################
# Native Filesystems
################################################################################

## NativeFS - BTFS
obj-$(CONFIG_RFS_FS_CORE_FAT)    += rfs_fat.o

rfs_fat-y	:= linux_init_fat.o

rfs_fat-y += $(addprefix nativefs/btfs/, \
		addon/ffat_addon_api.o \
		addon/ffat_addon_dec.o \
		addon/ffat_addon_xattr.o \
		addon/ffat_addon_fastseek.o \
		addon/ffat_addon_fcc.o \
		addon/ffat_addon_hpa.o \
		addon/ffat_addon_log.o \
		addon/ffat_addon_misc.o \
		addon/ffat_addon_nand.o \
		addon/ffat_addon_spfile.o \
		addon/ffat_addon_xde.o \
		addon/ffat_addon_chkdsk.o \
		addon/ffat_addon_debug.o \
		core/ffat_api.o \
		core/ffat_file.o \
		core/ffat_dir.o \
		core/ffat_main.o \
		core/ffat_misc.o \
		core/ffat_node.o \
		core/ffat_share.o \
		core/ffat_vol.o \
		ffatfs/ffatfs_api.o \
		ffatfs/ffatfs_bs.o \
		ffatfs/ffatfs_cache.o \
		ffatfs/ffatfs_de.o \
		ffatfs/ffatfs_fat.o \
		ffatfs/ffatfs_main.o \
		ffatfs/ffatfs_misc.o \
		global/ffat_common.o \
		oam/ffat_al_nestle.o \
		vfsal/nestle/ffat_nestle.o \
		vfsal/nestle/ffat_buffer_cache.o \
		vfsal/nestle/ffat_debug.o \
		vfsal/nestle/ffat_version.o \
		)

