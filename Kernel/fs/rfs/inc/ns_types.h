/*
* Nestle filesystem platform Developed by Flash Software Group.
*
* Copyright 2006-2009 by Memory Division, Samsung Electronics, Inc.,
* San #16, Banwol-Dong, Hwasung-City, Gyeonggi-Do, Korea
*
* All rights reserved.
*
* This software is the confidential and proprietary information
* of Samsung Electronics, Inc. ("Confidential Information").  You
* shall not disclose such Confidential Information and shall use
* it only in accordance with the terms of the license agreement
* you entered into with Samsung.
*/
/**
* @brief	Definition of nestle data types
* @author	ByungJune Song (byungjune.song@samsung.com)
* @author	InHwan Choi (inhwan.choi@samsung.com)
* @file		ns_types.h
*/

#ifndef __NS_TYPES_H__
#define __NS_TYPES_H__

#ifdef CONFIG_LINUX
#include <linux/types.h>
#include <linux/nls.h>
#endif

#ifdef CONFIG_WINCE
#include <windows.h>
#include <diskio.h>
#endif

#include "ns_oam_types.h"
#include "ns_config.h"
#include "ess_nlist.h"

/******************************************************************************/
/* DEFINE                                                                     */
/******************************************************************************/
 
#ifndef INLINE
#define INLINE		static __inline
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef IO
#define IO
#endif

#ifndef NULL
	#ifdef __cplusplus
		#define NULL    0
	#else
		#define NULL    ((void *)0)
	#endif
#endif

#if	defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_SYMBIAN)
	#if !defined(_WCHAR_T_DEFINED) && !defined(HAS_WCHAR_T)
		#if defined(CONFIG_SYMBIAN)
			#include <stddef.h> 
		#endif

		#if defined(CONFIG_RTOS) || defined(CONFIG_WINCE)
			typedef unsigned short wchar_t;
		#endif
	#endif
#endif /*defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_SYMBIAN)*/

#ifndef NS_BOOL
#define BOOL int
#define NS_BOOL
#endif

#if	defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_LINUX)
#ifndef TRUE
#define TRUE	(1L)
#endif 

#ifndef FALSE
#define FALSE	(0L)
#endif
#endif /*defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_LINUX)*/

/* Use this macro to avoid bullseye condition coverage when it case OS resource or device I/O error */
#define IF(x)		if(x)		// probably 'if' is true

/* nestle supports both / and \ using path delimiter */
#define	PATH_DELIMITER1					L'/'				/**< Slash */
#define	PATH_DELIMITER2					L'\\'				/**< Back Slash */

#define IS_PATH_DELIMITER(wc)		((wc) == PATH_DELIMITER1 || (wc) == PATH_DELIMITER2)

#if defined(CONFIG_LINUX) || defined(_RFS_TOOLS) || defined(CONFIG_TEST)
#undef IS_PATH_DELIMITER
#define IS_PATH_DELIMITER(wc)		((wc) == PATH_DELIMITER1)
#endif

/* nestle supports wild character lookup (read directory) */
#define WILD_CHARACTER1					L'*'				/**< Asterisk */
#define WILD_CHARACTER2					L'?'				/**< Question mark */

#define INVALID_FHANDLE					0xFFFFFFFF
#define BINARY_SEMAPHORE				1

#define	MAX_ALT_FILE_NAME_LENGTH		12
#define	MAX_VOLUME_LABEL_LENGTH			11

#define SIZEOF_ARRAY(arr)				(sizeof((arr)) / (sizeof(*(arr))))
#define IOCTL_TARGET(c)					(((c) >> 24) & 0xFF)
#define IOCTL_CODE(c)					(((c) >> 8) & 0xFFFF)
#define IOCTL_IN(c)						(((c) & 0x2) >> 1)
#define IOCTL_OUT(c)					((c) & 0x1)

#define CTLCODE(target, function, in, out)								\
	(((target) << 24) | ((function) << 8) | ((in) << 1) | (out))

#define TARGET_NESTLE	0x1
#define TARGET_NATIVE	0x2
#define TARGET_DEVICE	0x4


/******************************************************************************/
/* ENUMERATION                                                                */
/******************************************************************************/

typedef int							FERROR;
typedef unsigned int				FHANDLE;
typedef unsigned long long			FILE_OFFSET, FILE_SIZE, *PFILE_OFFSET, *PFILE_SIZE;
typedef	unsigned int				COMP_TIME;
typedef unsigned int				COMP_DATE;

/**
* @brief	io control code list
* @ref		_IOCTL_CODE
*/
typedef unsigned int	IOCTL_CODE;

/**
* @brief	io control code list
*/
enum _IOCTL_CODE
{
	// logical disk code
	IOCTL_LDEV_GET_INFO					= CTLCODE(TARGET_DEVICE, 100, FALSE, TRUE),	/**< get logical disk info */
	IOCTL_LDEV_READ_SECTOR				= CTLCODE(TARGET_DEVICE, 101, FALSE, TRUE),	/**< read directly from the logical disk */
	IOCTL_LDEV_WRITE_SECTOR				= CTLCODE(TARGET_DEVICE, 102, TRUE, FALSE),	/**< write directly to the logical disk */
	IOCTL_LDEV_ERASE_SECTOR				= CTLCODE(TARGET_DEVICE, 103, TRUE, FALSE),	/**< erase directly to the logical disk */
	
	IOCTL_PDEV_GET_INFO					= CTLCODE(TARGET_DEVICE, 104, FALSE, TRUE),	/**< get the physical device infomation */
	IOCTL_PDEV_LOW_FORMAT				= CTLCODE(TARGET_DEVICE, 105, TRUE, FALSE),	/**< physical device low format  */

	IOCTL_PDEV_READ_SECTOR				= CTLCODE(TARGET_DEVICE, 106, FALSE, TRUE),	/**< read directly from the physical device */
	IOCTL_PDEV_WRITE_SECTOR				= CTLCODE(TARGET_DEVICE, 107, TRUE, FALSE),	/**< write directly to the physical device */
	IOCTL_PDEV_ERASE_SECTOR				= CTLCODE(TARGET_DEVICE, 108, TRUE, FALSE),	/**< write directly to the physical device */

	IOCTL_PDEV_GET_NAND_BLOCKSIZE		= CTLCODE(TARGET_DEVICE, 109, FALSE, TRUE),	/**< get the nand block size */
	IOCTL_PDEV_GET_NAND_PAGESIZE		= CTLCODE(TARGET_DEVICE, 110, FALSE, TRUE),	/**< get the nand page size */
	IOCTL_PDEV_GET_NAND_ECNT			= CTLCODE(TARGET_DEVICE, 111, TRUE, TRUE),	/**< get nand erase count */
	IOCTL_PDEV_GET_FTL_VERSION			= CTLCODE(TARGET_DEVICE, 112, FALSE, TRUE),	/**< get FTL version */
	IOCTL_PDEV_GET_FTL_CONFIG			= CTLCODE(TARGET_DEVICE, 113, FALSE, TRUE),	/**< get FTL config */
	
	// nestle code
	IOCTL_NESTLE_FILE_DUMP				= CTLCODE(TARGET_NESTLE, 200, TRUE, FALSE),	/**< dump a file sector for debug */
	IOCTL_NESTLE_FLUSH					= CTLCODE(TARGET_NESTLE, 201, TRUE, FALSE),	/**< flush nestle buffer cache*/

	// nativefs code
	// ioctl : volume operation
	IOCTL_NATIVE_READ_SECTOR			= CTLCODE(TARGET_NATIVE, 300, FALSE, TRUE),	/**< read sector through native */
	IOCTL_NATIVE_WRITE_SECTOR			= CTLCODE(TARGET_NATIVE, 301, TRUE, FALSE),	/**< write sector through native */
	IOCTL_NATIVE_ERASE_SECTOR			= CTLCODE(TARGET_NATIVE, 302, TRUE, FALSE),	/**< erase sector through native */
	IOCTL_NATIVE_FAT_CLEAN				= CTLCODE(TARGET_NATIVE, 303, TRUE, FALSE),	/**< notify clean FAT information to device */
};


/**
* @brief	file system control code list
* @ref		_FSCTL_CODE
*/
typedef unsigned int	FSCTL_CODE;

/**
* @brief	filesystem control code list
*/
enum _FSCTL_CODE
{
	// nativefs code
	
	FSCTL_NATIVE_GET_VOL_NAME			= CTLCODE(TARGET_NATIVE, 311, FALSE, TRUE),	/**< get volume name */
	FSCTL_NATIVE_SET_VOL_NAME			= CTLCODE(TARGET_NATIVE, 312, TRUE, FALSE),	/**< set volume name */

	FSCTL_NATIVE_ADD_CACHE				= CTLCODE(TARGET_NATIVE, 313, TRUE, FALSE),	/**< add cache to native */
	FSCTL_NATIVE_REMOVE_CACHE			= CTLCODE(TARGET_NATIVE, 314, TRUE, FALSE),	/**< remove cache from native */
	FSCTL_NATIVE_CHECK_CACHE			= CTLCODE(TARGET_NATIVE, 315, TRUE, FALSE),	/**< check cache in native */

	FSCTL_NATIVE_SET_DEC				= CTLCODE(TARGET_NATIVE, 316, TRUE, FALSE),	/**< set user directory entry cache */
	FSCTL_NATIVE_RELEASE_DEC			= CTLCODE(TARGET_NATIVE, 317, TRUE, TRUE),	/**< release user directory entry cache */
	FSCTL_NATIVE_GET_DEC_INFO			= CTLCODE(TARGET_NATIVE, 318, FALSE, TRUE),	/**< set user directory entry cache information */

	FSCTL_NATIVE_CHECK_BOOT_SECTOR		= CTLCODE(TARGET_NATIVE, 319, TRUE, FALSE),	/**< check that first sector is boot-sector */

	// test code (internal use only)
	FSCTL_NATIVE_TEST_LOG_RECOVERY		= CTLCODE(TARGET_NATIVE, 320, TRUE, FALSE),	/**< for test log recovery */
	FSCTL_NATIVE_TEST_INVALIDATE_FCCH	= CTLCODE(TARGET_NATIVE, 321, FALSE, TRUE),	/**< for test invalidate free cluster hint */

};

/**
* @brief	chkdisk or format?
* @ref		_UTIL_CODE
*/
typedef unsigned int	UTIL_CODE;

/**
* @brief	chkdisk or format?
* @ref		_UTIL_CODE
*/
enum _UTIL_CODE
{
	// mandatory
	UTCTL_DISK_FORMAT				= CTLCODE(TARGET_NATIVE, 1, TRUE, FALSE),	/**< Format */
	UTCTL_DISK_CHKDSK				= CTLCODE(TARGET_NATIVE, 3, TRUE, FALSE),	/**< Chkdsk (scanning) */
};

/**
* @enum	_MOUNT_FLAG
* @brief	mount option
* @remarks	MOUNT_TRANSACTION_OFF and MOUNT_TRANSACTION_ON_MASK are exclusive relations
*			MOUNT_READ_ONLY and MOUNT_WRITE_ON_MASK are exclusive relations
*			MOUNT_TRANSACTION_OFF and MOUNT_READ_ONLY have higher priority than exclusive relation flags
*/
typedef unsigned int	MOUNT_FLAG, *PMOUNT_FLAG;
enum _MOUNT_FLAG						
{	
	MOUNT_NORMAL				= 0x00000000,		/**< normal mount */

	MOUNT_READ_ONLY				= 0x00000001,		/**< read only mount */
	MOUNT_SYNC_META				= 0x00000004,		/**< sync meta-data */
	MOUNT_FAT_MIRROR			= 0x00000008,		/**< fat mirror */

	MOUNT_NO_LOCK				= 0x00000010,		/**< mount no lock */
	MOUNT_LOG_INIT				= 0x00000020,		/**< initialize log */
	MOUNT_TRANSACTION_OFF		= 0x00000040,		/**< no transaction */
	MOUNT_TRANSACTION_ON_FORCE	= 0x00000080,		/**< transaction force*/

	MOUNT_CLEAN_NAND			= 0x00000100,		/**< clean nand */
	MOUNT_IOCTL_ONLY			= 0x00000200,		/**< only ioctl is allowed */
	MOUNT_HPA					= 0x00000400,		/**< hidden protected area mode */
	MOUNT_HPA_CREATE			= 0x00000800,		/**< create hidden protected area */

	MOUNT_LLW					= 0x00001000,		/**< lazy log write */
	MOUNT_FULL_LLW				= 0x00002000,		/**< full lazy log write */
	MOUNT_ERASE_SECTOR			= 0x00004000,		/**< erase sector */

	MOUNT_FILE_GUID				= 0x00010000,		/**< guid support */
	MOUNT_FILE_XATTR			= 0x00020000,		/**< extended attributes */
	MOUNT_FILE_SPECIAL			= 0x00040000,		/**< special file (symlink, FIFO, socket) support */
	MOUNT_ALLOW_OS_NAMING_RULE	= 0x00080000,		/**< allow to use ascii character in file or directory name for linux. */

	MOUNT_NO_DEVICE				= 0x00100000,		/**< mount without device */
	MOUNT_ALLOW_DEVICE_IO		= 0x00200000,		/**< direct access to logical disk is allowed */
	MOUNT_EXTERNAL_DEVICE		= 0x00400000,		/**< external(card) device */
	MOUNT_AUTO_ADAPTATION		= 0x00800000,		/**< auto adaptively mount for chkdsk */
 
	MOUNT_OS_PRIVATE			= 0x01000000,		/**< the least significant bit of OS private */
	MOUNT_CASE_INSENSITIVE		= 0x02000000,		/**< case insensitive, set by native */

	MOUNT_SYNC_FLAG_MASK		= MOUNT_EXTERNAL_DEVICE | MOUNT_FULL_LLW,
	MOUNT_TRANSACTION_ON_MASK	= MOUNT_LOG_INIT | MOUNT_LLW | MOUNT_FULL_LLW,	/** < available under transaction on */
	MOUNT_WRITE_ON_MASK			= MOUNT_CLEAN_NAND | MOUNT_LOG_INIT | MOUNT_ERASE_SECTOR, /** < available under NO read only  */
									/**< MOUNT_HPA_CREATE is used for mounting volume at already HPA exist */

	MOUNT_FLAG_MASK				= MOUNT_READ_ONLY | MOUNT_SYNC_META | MOUNT_FAT_MIRROR |
									MOUNT_NO_LOCK | MOUNT_LOG_INIT | MOUNT_TRANSACTION_OFF | MOUNT_TRANSACTION_ON_FORCE |
									MOUNT_CLEAN_NAND | MOUNT_IOCTL_ONLY | MOUNT_HPA | MOUNT_HPA_CREATE |
									MOUNT_LLW | MOUNT_FULL_LLW | MOUNT_ERASE_SECTOR |
									MOUNT_FILE_GUID | MOUNT_FILE_XATTR | MOUNT_FILE_SPECIAL | MOUNT_ALLOW_OS_NAMING_RULE |
									MOUNT_NO_DEVICE | MOUNT_ALLOW_DEVICE_IO | MOUNT_EXTERNAL_DEVICE | MOUNT_AUTO_ADAPTATION,

	MOUNT_NATIVE_FLAG_MASK		= MOUNT_FLAG_MASK
};	

/**
* @enum	_UNMOUNT_FLAG
* @brief	unmount option
*/
typedef unsigned int	UNMOUNT_FLAG;
enum _UNMOUNT_FLAG								
{
	UNMOUNT_NORMAL				= 0x00000000,		/**< normal unmount */
	UNMOUNT_FORCE				= 0x10000000,		/**< umount force */
	UMMOUNT_FLAG_MASK			= UNMOUNT_NORMAL | UNMOUNT_FORCE,
} ;

/**
* @enum	_REMOUNT_FLAG
* @brief	remount option
* @remark	refer to _MOUNT_FLAG
*/
typedef unsigned int	REMOUNT_FLAG, *PREMOUNT_FLAG;
enum _REMOUNT_FLAG								
{
	REMOUNT_NORMAL					= MOUNT_NORMAL,					/**< normal mount */
	
	REMOUNT_READ_ONLY				= MOUNT_READ_ONLY,				/**< read only mount */

	REMOUNT_LOG_INIT				= MOUNT_LOG_INIT,				/**< initialize log */
	REMOUNT_TRANSACTION_OFF			= MOUNT_TRANSACTION_OFF,		/**< no transaction */
	REMOUNT_CLEAN_NAND				= MOUNT_CLEAN_NAND,				/**< clean nand */

	REMOUNT_LLW						= MOUNT_LLW,					/**< lazy log write */
	REMOUNT_FULL_LLW				= MOUNT_FULL_LLW,				/**< full lazy log write */

	REMOUNT_FILE_GUID				= MOUNT_FILE_GUID,				/**< guid support */
	REMOUNT_FILE_XATTR				= MOUNT_FILE_XATTR,				/**< extended attributes */
	REMOUNT_FILE_SEPCIAL			= MOUNT_FILE_SPECIAL,			/**< special file (symlink, FIFO, socket) support */
	REMOUNT_ALLOW_OS_NAMING_RULE	= MOUNT_ALLOW_OS_NAMING_RULE,	/**< allow to use ascii character in file or directory name for linux. */

	REMOUNT_OS_PRIVATE				= MOUNT_OS_PRIVATE,			/**< the least significant bit of OS private */

	REMOUNT_TRANSACTION_ON_MASK	= REMOUNT_LOG_INIT | REMOUNT_LLW | REMOUNT_FULL_LLW,	/** < available under transaction on */

	REMOUNT_FLAG_MASK			= REMOUNT_TRANSACTION_OFF | REMOUNT_READ_ONLY | 
									REMOUNT_LOG_INIT | REMOUNT_CLEAN_NAND | REMOUNT_LLW | REMOUNT_FULL_LLW |
									REMOUNT_FILE_GUID | REMOUNT_FILE_XATTR | REMOUNT_FILE_SEPCIAL | REMOUNT_ALLOW_OS_NAMING_RULE
};	


/**
* @enum	_CREATION_MODE
* @brief	file create option
*/
typedef unsigned int	CREATION_MODE;
enum _CREATION_MODE						
{
	FILE_CREATE					= 0x00000001, /**< if file does not exist, we can create it */
	FILE_EXCL					= 0x00000002, /**< This is used with FILE_CREATE. if file exists, it returns error */
	FILE_TRUNCATE				= 0x00000004, /**< do truncate to 0 */

	FILE_DONT_CREATE			= 0x00000000, /**< pure open. if the file does not exist, the open will fail. */
	FILE_OPEN_ALWAYS			= FILE_CREATE,	/**< if file does not exist, do create */
	FILE_CREATE_NEW				= FILE_CREATE | FILE_EXCL,	/**< if the file already exists, the open fails. */
	FILE_TRUNCATE_EXISTING		= FILE_TRUNCATE,/**< if the file exists, it will be truncated. If the file does not exist, it will fail. */
	FILE_TRUNCATE_CREATE		= FILE_CREATE | FILE_TRUNCATE, /**< if the file exists, it is truncate. otherwise, create the file */
	FILE_TRUNCATE_CREATE_NEW	= FILE_CREATE | FILE_TRUNCATE | FILE_EXCL, /**< if the file exists, the open fails. create the file */
} ;
#define FILE_CREATE_EXCL		FILE_CREATE_NEW

/**
* @enum	_FILE_ACCESS_TYPES
* @brief	file open option
*/
typedef unsigned int	FILE_ACCESS_TYPES;
enum _FILE_ACCESS_TYPES
{
	// share
	FILE_ACCESS_SHARE_READ		= 0x00000001,	/**< allow another thread to re-open the file for reading */
	FILE_ACCESS_SHARE_WRITE		= 0x00000002,	/**< allow another thread to re-open the file for writing */

	// i/o
	FILE_ACCESS_RANDOM_ACCESS	= 0x00000100,	/**< the file will be accessed randomly. */
	FILE_ACCESS_NO_BUFFERING	= 0x00000200,	/**< don't cache any file i/o */
	FILE_ACCESS_SYNCHRONOUS		= 0x00000400,	/**< synchronous i/o */

	FILE_NO_MODTIME_UPDATE		= 0x00010000,	/**< modification time update */
	FILE_NO_ACSTIME_UPDATE		= 0x00020000,	/**< access time update */

	FILE_ACCESS_VOL				= 0x00100000,	/**< open volume */

	// RW
	FILE_ACCESS_APPEND			= 0x10000000,	/**< open files for append write */
	FILE_ACCESS_WRITE			= 0x20000000,	/**< open files for writing */
	FILE_ACCESS_READ			= 0x40000000,	/**< open files for reading */

	FILE_ACCESS_MASK			= FILE_ACCESS_SHARE_READ | FILE_ACCESS_SHARE_WRITE | FILE_ACCESS_RANDOM_ACCESS | 
								FILE_ACCESS_NO_BUFFERING | FILE_ACCESS_SYNCHRONOUS | FILE_NO_MODTIME_UPDATE |
								FILE_ACCESS_APPEND | FILE_ACCESS_WRITE | FILE_ACCESS_READ,
};

/**
* @enum	_FILE_ATTR
* @brief	file attribute
*/
typedef unsigned int	FILE_ATTR;
enum _FILE_ATTR
{
	FILE_ATTR_FILE				= 0x00000000,	/**< file */
	FILE_ATTR_READONLY			= 0x00000001,	/**< read only */
	FILE_ATTR_HIDDEN			= 0x00000002,	/**< hidden */
	FILE_ATTR_SYSTEM			= 0x00000004,	/**< system */
	FILE_ATTR_DIRECTORY			= 0x00000010,	/**< directory */
	FILE_ATTR_ARCHIVE			= 0x00000020,	/**< archive */
	FILE_ATTR_DIR_ARCHI			= (FILE_ATTR_DIRECTORY | FILE_ATTR_ARCHIVE),	/**< directory & archive */
	FILE_ATTR_NORMAL			= 0x00000080,	/**< normal file (for wince) */
	FILE_ATTR_FILE_ARCHI		= (FILE_ATTR_FILE | FILE_ATTR_ARCHIVE),	/**< file */

	FILE_ATTR_DEVICE			= 0x00000100,	/**< device file (devfs) */
	FILE_ATTR_ENCRYPTED			= 0x00004000,	/**< encrypted */
	FILE_ATTR_LINKED			= 0x00008000,	/**< symlink */
	FILE_ATTR_NESTED			= 0x00010000,	/**< nested file */
	FILE_ATTR_NO_INIT_CLUSTER	= 0x00100000,	/**< do not zerofill */

	FILE_ATTR_FIFO				= 0x01000000,	/**< fifo (for linux) */
	FILE_ATTR_SOCKET			= 0x02000000,	/**< socket (for linux) */

	FILE_SETTABLE_ATTR			= FILE_ATTR_READONLY | FILE_ATTR_HIDDEN | FILE_ATTR_SYSTEM | FILE_ATTR_ARCHIVE | FILE_ATTR_NORMAL,

	FILE_ATTR_MASK				= FILE_ATTR_FILE | FILE_ATTR_READONLY | FILE_ATTR_HIDDEN | FILE_ATTR_SYSTEM | FILE_ATTR_DIRECTORY |
								  FILE_ATTR_ARCHIVE | FILE_ATTR_NORMAL | FILE_ATTR_DEVICE | FILE_ATTR_ENCRYPTED |
								  FILE_ATTR_LINKED | FILE_ATTR_NESTED | FILE_ATTR_NO_INIT_CLUSTER | FILE_ATTR_FIFO | FILE_ATTR_SOCKET
} ;

/**
* @enum	_FCB_ACCESS_MODE
* @brief	Access to open file
*/
typedef unsigned int				FCB_ACCESS_MODE;
enum _FCB_ACCESS_MODE
{
	// share
	FCB_ACCESS_SHARE_READ			= FILE_ACCESS_SHARE_READ,	/**< allow another thread to re-open the file for reading */
	FCB_ACCESS_SHARE_WRITE			= FILE_ACCESS_SHARE_WRITE,	/**< allow another thread to re-open the file for writing */

	// i/o
  	FCB_ACCESS_RANDOM_ACCESS		= FILE_ACCESS_RANDOM_ACCESS,	/**< the file will be accessed randomly. */
	FCB_ACCESS_NO_BUFFERING			= FILE_ACCESS_NO_BUFFERING,	/**< don't cache any file i/o */
	FCB_ACCESS_SYNCHRONOUS			= FILE_ACCESS_SYNCHRONOUS,	/**< synchronous i/o */

	FCB_NO_MODTIME_UPDATE			= FILE_NO_MODTIME_UPDATE, /**< modification time update */
	FCB_NO_ACSTIME_UPDATE			= FILE_NO_ACSTIME_UPDATE, /**< access time update */

	FCB_ACCESS_VOL					= FILE_ACCESS_VOL,			/**< open volume */

	// RW
	FCB_ACCESS_APPEND				= FILE_ACCESS_APPEND, /**< open files for append write */
	FCB_ACCESS_WRITE				= FILE_ACCESS_WRITE,	/**< open files for writing */
	FCB_ACCESS_READ					= FILE_ACCESS_READ,	/**< open files for reading */

	FCB_RW_MASK						= FCB_ACCESS_APPEND | FCB_ACCESS_WRITE | FCB_ACCESS_READ,

	FCB_ACCESS_MASK					= FCB_ACCESS_SHARE_READ | FCB_ACCESS_SHARE_WRITE | FCB_ACCESS_RANDOM_ACCESS | 
										FCB_ACCESS_NO_BUFFERING | FCB_ACCESS_SYNCHRONOUS | FCB_NO_MODTIME_UPDATE |
										FCB_NO_ACSTIME_UPDATE| FCB_ACCESS_VOL |
										FCB_ACCESS_APPEND | FCB_ACCESS_WRITE | FCB_ACCESS_READ,
	
	FCB_DIR_OPEN					= FCB_ACCESS_SHARE_READ | FCB_ACCESS_READ
};

/**
* @enum	_ACL_MODE
* @brief	access permission info
*/
typedef unsigned short	ACL_MODE;
enum _ACL_MODE
{
	OTHERS_EXECUTE				= 0x0001,		/**< others can execute */
	OTHERS_WRITE				= 0x0002,		/**< others can write */
	OTHERS_READ					= 0x0004,		/**< others can read */
	OTHERS_RWX					= OTHERS_EXECUTE | OTHERS_WRITE | OTHERS_READ,

	GROUP_EXECUTE				= 0x0008,		/**< group can execute */
	GROUP_WRITE					= 0x0010,		/**< group can write */
	GROUP_READ					= 0x0020,		/**< group can read */
	GROUP_RWX					= GROUP_EXECUTE | GROUP_WRITE | GROUP_READ,

	OWNER_EXECUTE				= 0x0040,		/**< owner can execute */
	OWNER_WRITE					= 0x0080,		/**< owner can write */
	OWNER_READ					= 0x0100,		/**< owner can read */
	OWNER_RWX					= OWNER_EXECUTE | OWNER_WRITE | OWNER_READ,

	ACL_STICKY					= 0x0200,		/**< sticky bit */
	ACL_SGID					= 0x0400,		/**< set GID bit */
	ACL_SUID					= 0x0800,		/**< set UID bit */
	
	ACL_EXECUTES				= OWNER_EXECUTE | GROUP_EXECUTE | OTHERS_EXECUTE,
	ACL_READS					= OWNER_READ | GROUP_READ | OTHERS_READ,
	ACL_WRITES					= OWNER_WRITE | GROUP_WRITE | OTHERS_WRITE,

	ACL_RWX						= ACL_READS | ACL_WRITES | ACL_EXECUTES,
	ACL_ALL						= ACL_RWX | ACL_STICKY | ACL_SGID | ACL_SUID,
} ;

/**
* @enum	_OPERATION_MODE
* @brief	operation node
*/
typedef unsigned int	OPERATION_MODE;
enum _OPERATION_MODE
{
	OP_EXEC						= 0x00000001,		/**< exec */
	OP_WRITE					= 0x00000002,		/**< write */
	OP_READ						= 0x00000004,		/**< read */
	OP_APPEND					= 0x00000008,		/**< append */
	OP_RESIZE					= 0x00000010,		/**< resize */
	OP_DELETE					= 0x00000020,		/**< delete */
	OP_METAUPDATE				= 0x00000040,		/**< metadata update */

	OP_MASK						= OP_EXEC | OP_WRITE | OP_READ | OP_APPEND | 
	OP_RESIZE | OP_DELETE | OP_METAUPDATE,
} ;

/**
* @enum	_LOOKUP_FLAG
* @brief	lookup option
*/
typedef unsigned int	LOOKUP_FLAG;
enum _LOOKUP_FLAG
{
	LOOKUP_FOR_CREATE			= 0x00000001,		/**< lookup for create */
	LOOKUP_FOR_SEARCH			= 0x00000002,		/**< lookup for search. But, in linux this is just hint. it may be wrong */
	LOOKUP_FOR_RENAME			= 0x00000004,		/**< lookup for renmae. But, in linux this is just hint. it may be wrong */
} ;

/**
* @enum	_BCM_OP_FLAG
* @brief	buffer cache operation option 
*/
typedef unsigned int			BCM_OP_FLAG, *PBCM_OP_FLAG;
enum _BCM_OP_FLAG
{
	BCM_OP_NONE					= 0x00000000,			/**< write case : write back, read case : uptodate*/
	BCM_OP_SYNC					= 0x00000001,			/**< write case : write through*/
	BCM_OP_DIRECT				= 0x00000002,			/**< read / write case : write direct, read case : read direct */
	BCM_OP_META					= 0x00000010,			/**< write case : meta data, this value must be equal BCM_BE_META */
	BCM_OP_NORMAL				= 0x00000020,			/**< write case : normal data, not meta, this value must be equal BCM_BE_NORMAL */
	BCM_OP_HOT					= 0x00000040,			/**< write case : hot data, , this value must be equal BCM_BE_HOT */
	BCM_OP_COLD					= 0x00000080,			/**< write case : cold data, , not hot, this value must be equal BCM_BE_COLD */
	BCM_OP_READ					= 0x00010000,			/**< read case */
	BCM_OP_WRITE				= 0x00020000,			/**< write case */
	BCM_OP_VECTOR				= 0x00001000,			/**< vector operation case */

	BCM_OP_MASK_RW				=	BCM_OP_META | BCM_OP_NORMAL| 
									BCM_OP_HOT | BCM_OP_COLD |
									BCM_OP_READ | BCM_OP_WRITE,		/**< read / write flag mask */

	BCM_OP_MASK					=	BCM_OP_MASK_RW | BCM_OP_SYNC | BCM_OP_DIRECT,

	BCM_OP_ALL					= 0x00000100,			/**< discard case : discard all buffers in vnode*/	
	BCM_OP_MASK_DISCARD			= BCM_OP_ALL,			/**< discard flag mask */

	BCM_OP_DUMMY				= 0x7FFFFFFF
};

/**
* @enum	_BCM_BE_STATUS
* @brief	Buffer entry status 
* @remarks	[AND]	: If whole blocks have a condition, The Entry has the condition.
*			[OR]	: If a blocks just has a condition, The Entry has the condition.
*			[ONLY]	: The blocks cannot have the condition, The Entry can only.
*/
typedef unsigned int			BCM_BE_STATUS, *PBCM_BE_STATUS;
enum _BCM_BE_STATUS {
	BCM_BE_FREE					= 0x00000000,			/**< [AND]	Is Free */
	BCM_BE_UPTODATE				= 0x00000001,			/**< [AND]	Is uptodate data or valid */
	BCM_BE_DIRTY				= 0x00000002,			/**< [OR]	Is dirty, (NOT clean) */
	BCM_BE_META					= BCM_OP_META,			/**< [OR]	0x10, Is meta data (NOT user data), this value must be equal BCM_OP_META */
	BCM_BE_NORMAL				= BCM_OP_NORMAL,		/**< [AND]	0x20, Is user data (NOT meta data), this value must be equal BCM_OP_META */
	BCM_BE_HOT					= BCM_OP_HOT,			/**< [OR]	0x40, Is hot data (NOT cold data), this value must be equal BCM_OP_META */
	BCM_BE_COLD					= BCM_OP_COLD,			/**< [AND]	0x80, Is cold data (NOT hot data), this value must be equal BCM_OP_META */
	BCM_BE_VECTOR				= 0x00001000,			/**< [ONLY]	Vector entry */
	BCM_BE_DUMMY				= 0x7FFFFFFF
};
 
/**
* @enum	_DISK_OPERATE_TYPE
* @brief	disk operation type
* @remarks	Nestle had to use low 16bit, glue use high 16bit
*/
typedef unsigned int			DISK_OPERATE_TYPE;
enum _DISK_OPERATE_TYPE
{
	LDEV_NONE					= 0x00000000,			/**< ldev none */
	LDEV_SYNC					= BCM_OP_SYNC,			/**< ldev write through */
	LDEV_DIRECT					= BCM_OP_DIRECT,		/**< ldev direct I/O */
	LDEV_META					= BCM_OP_META,			/**< write case : meta data */
	LDEV_HOT					= BCM_OP_HOT,			/**< write case : hot data */
	LDEV_READ					= BCM_OP_READ,			/**< ldev read */
	LDEV_WRITE					= BCM_OP_WRITE,			/**< ldev write , write back is default */

};

/**
* @enum	_NFS_TYPE
* @brief	FAT type
*/
typedef unsigned int			NFS_TYPE;
enum _NFS_TYPE
{
	NFS_NONE					= 0x00,				/**< non type */
	NFS_TYPE_FAT12				= 0x01,				/**< fat12 */
	NFS_TYPE_FAT16				= 0x02,				/**< fat16 */
	NFS_TYPE_FAT32				= 0x04,				/**< fat32 */

	NFS_TYPE_EXFAT				= 0x05				/**< exFAT */
};

/**
* @enum	_NFS_NAME
* @brief	Native filesystems
*/
typedef unsigned int			NFS_NAME;
enum _NFS_NAME
{
	NFS_NAME_BTFS				= 0x0010,			/**< btfs */
	NFS_NAME_EXFAT				= 0x0020,			/**< exfat */
};

/**
* @enum	_DEVICE_ATTRIBUTE
* @brief	device attribute
*/
typedef unsigned int	DEVICE_ATTRIBUTE;
enum _DEVICE_ATTRIBUTE
{
	DEVICE_BLOCK_DEVICE			= 0x00000001,		/**< block device */
	DEVICE_READONLY				= 0x00000010,		/**< readonly device */
	DEVICE_REMOVABLE_MEDIA		= 0x00000100,		/**< removable device */
	DEVICE_MEDIA_INSERTED		= 0x00000200,		/**< device is just inserted */
	DEVICE_MEDIA_EJECTED		= 0x00000400		/**< device was removed */
} ;

/**
* @enum	_XATTR_NAMESPACE_ID
* @brief	Index for extended namespace (not bitwise flag) 
*/
typedef unsigned int		XATTR_NAMESPACE_ID;
enum _XATTR_NAMESPACE_ID
{
	XATTR_ID_USER				= 1,
	XATTR_ID_POSIX_ACL_ACCESS	= 2,
	XATTR_ID_POSIX_ACL_DEFAULT	= 3,
	XATTR_ID_TRUSTED			= 4,
	XATTR_ID_SECURITY			= 5
};

/**
* @enum	_XATTR_SET_FLAG
* @brief	create value or replace value 
*/
typedef int					XATTR_SET_FLAG;
enum _XATTR_SET_FLAG
{
	XATTR_SET_FLAG_NONE			= 0,
	XATTR_SET_FLAG_CREATE		= 1,
	XATTR_SET_FLAG_REPLACE		= 2
};


/**
 * @enum	_CONVERT_TYPE
 * @brief	get long/short name command
 */
typedef unsigned int			CONVERT_TYPE;
enum _CONVERT_TYPE
{
	FILE_NAME_SHORT_TO_LONG		= 0x01,	/**< get long name */
	FILE_NAME_LONG_TO_SHORT		= 0x02,	/**< get short name */
};


/**
 * @enum	_CHKDSK_TYPE
 * @brief	CHKDSK (scanning) flag
 */
typedef unsigned int			CHKDSK_TYPE;
enum _CHKDSK_TYPE
{
	CHKDSK_REPAIR				= 0x0001,	/**< repair errors */
	CHKDSK_SHOW					= 0x0002,	/**< print errors */
	CHKDSK_REPAIR_INTERACTIVE	= 0x0004,	/**< repair with confirm */
	CHKDSK_CHECK_ONLY			= 0x0008,	/**< check only */
	CHKDSK_MASK					= (CHKDSK_REPAIR | CHKDSK_SHOW | CHKDSK_REPAIR_INTERACTIVE | CHKDSK_CHECK_ONLY)
};

/**
 * @enum	_PART_INFO_ENUM
 * @brief	get parition info option
 */
typedef unsigned int			PART_INFO_ENUM;
enum _PART_INFO_ENUM
{
	PART_INFO_GET_ALL	= 0x00000001,		/* get all partition information */
	PART_INFO_GET_ONE	= 0x00000002,		/* get a partitition informatoin */
	PART_INFO_GET_COUNT	= 0x00000004,		/* get partition count */
	PART_INFO_GET_EXT	= 0x00000008,		/* get extended partition informatoin */
};

/**
* @enum	_SEEKFILE_METHOD
* @brief	seekfile option
*/
typedef enum _SEEKFILE_METHOD
{
	SEEKFILE_BEGIN			= 0,	/**< seek_begin */
	SEEKFILE_CURRENT		= 1,	/**< seek_current */
	SEEKFILE_END			= 2,	/**< seek_end */
} SEEKFILE_METHOD;

/**
 * @enum	_FLOCK_TYPE
 * @brief	flock type 
 */
typedef enum _FLOCK_TYPE
{
	FLOCK_SHARED_LOCK		= 0x0001,		/**< lock for read */
	FLOCK_EXCLUSIVE_LOCK	= 0x0002,		/**< lock for write */
	FLOCK_UNLOCK			= 0x0004,		/**< unlock */
	FLOCK_FAIL_IMMEDIATELY	= 0x0008,		/**< returns immediately if it is unable to acquire the requested lock */	
	FLOCK_SETLOCK			= 0x0010,		/**< set lock */
	FLOCK_GETLOCK			= 0x0020		/**< get lock */
} FLOCK_TYPE, *PFLOCK_TYPE;

/**
 * @enum	_DISK_FLAGS
 * @brief	Define disk flag in physical device
 * @see		it used _FORMAT_DISK_INFO
 */
typedef unsigned int	DISK_FLAGS;
enum _DISK_FLAGS
{
	DISK_NO_INFO				= 0x00000000,		/**< no info */
	DISK_WRITE_LOCKED			= 0x00000001,		/**< write locked */
	DISK_INTERNAL				= 0x00000002,		/**< device for file system internal use */
	DISK_SEMI_INTERNAL			= 0x00000004,		/**< semi internal */

	DISK_REMOVABLE_MEDIA		= 0x00000008,		/**< removable media */
	DISK_SUPPORT_ERASE_SECTORS	= 0x00000020,		/**< support erase sector */

	DISK_DEVICE_INIT			= 0x00000100,		/**< initialized */
	DISK_DEVICE_OPEN			= 0x00000100,		/**< opened */

	DISK_TYPE_HARDDISK			= 0x00001000,		/**< hard disk type */
	DISK_TYPE_MMC				= 0x00002000,		/**< mmc type */
	DISK_TYPE_SMMC				= 0x00004000,		/**< smmc type */
	DISK_TYPE_SD				= 0x00008000,		/**< sd type */
	DISK_TYPE_CF				= 0x00010000,		/**< cf type */
	DISK_TYPE_NAND_FLASH		= 0x00020000,		/**< nand flash type */
	DISK_TYPE_NOR_FLASH			= 0x00040000,		/**< nor flash type */
	DISK_TYPE_UNKNOWN			= 0x00080000,		/**< unknown type */

	DISK_TYPE_DUMMY				= 0x7FFFFFFF,
};


/**
* @enum	_VCB_STATUS
* @brief	Status of volume
* @see		
*
*/
typedef unsigned int			VCB_STATUS;
enum _VCB_STATUS
{
	VCB_STATUS_INITIALIZED		= 0x00000000,		/**< init status */
	VCB_STATUS_MOUNTING			= 0x00000001,		/**< On mounting */
	VCB_STATUS_MOUNTED			= 0x00000002,		/**< Mounting completed */
	VCB_STATUS_UNMOUNTING		= 0x00000010,		/**< On unmounting */
	VCB_STATUS_DIRTY			= 0x00000020,		/**< vcb dirty */
	VCB_STATUS_MASK				= VCB_STATUS_MOUNTING | VCB_STATUS_MOUNTED | VCB_STATUS_UNMOUNTING
};

/**
 * @enum	_VCB_SEARCH_FLAG
 * @brief	Flags to search a Vcb in Vcb list
 * @see		
 */
typedef unsigned int		VCB_SEARCH_FLAG;
enum _VCB_SEARCH_FLAG
{
	VCB_SEARCH_TARGET_STATUS	= 0x00000001, /**< search a specific status */
	VCB_SEARCH_ANY_STATUS		= 0x00000002,	/**< search any status */

	VCB_SEARCH_MASK				= VCB_SEARCH_TARGET_STATUS | VCB_SEARCH_ANY_STATUS,
};

/**
* @enum		_VNODE_STATUS
* @brief	vnode status
*/
typedef unsigned int			VNODE_STATUS;
enum _VNODE_STATUS
{
	VNODE_CLEAN					= 0x00000000,			/**< VNode Clean */
	VNODE_DIRTY_DATA			= 0x00000001,			/**< VNode Data Dirty */
	VNODE_DIRTY_META			= 0x00000002,			/**< VNode Meta Dirty */
	VNODE_DIRTY					= VNODE_DIRTY_DATA | VNODE_DIRTY_META, /**< VNode Meta & Data Dirty */
	VNODE_DIRECT				= 0x00000010
};

/**
* @enum		_VNODE_ATTR
* @brief	define vnode attribute
*/
typedef unsigned int			VNODE_ATTR;
enum _VNODE_ATTR
{
	VNODE_ATTR_READONLY			= FILE_ATTR_READONLY,	/**< read only */
	VNODE_ATTR_HIDDEN			= FILE_ATTR_HIDDEN,		/**< hidden */
	VNODE_ATTR_SYSTEM			= FILE_ATTR_SYSTEM,		/**< system */
	VNODE_ATTR_DIRECTORY		= FILE_ATTR_DIRECTORY,	/**< directory */
	VNODE_ATTR_ARCHIVE			= FILE_ATTR_ARCHIVE,	/**< archive */
	VNODE_ATTR_ENCRYPTED		= FILE_ATTR_ENCRYPTED,	/**< encrypted */
	VNODE_ATTR_LINKED			= FILE_ATTR_LINKED,		/**< symlink */
	VNODE_ATTR_NESTED			= FILE_ATTR_NESTED,		/**< nested file */
	VNODE_ATTR_NORMAL			= FILE_ATTR_NORMAL,		/**< normal file */

	VNODE_ATTR_DEVICE			= FILE_ATTR_DEVICE,		/**< device file */
	VNODE_ATTR_NO_INIT_CLUSTER	= FILE_ATTR_NO_INIT_CLUSTER, /**< do not init cluster */

	VNODE_ATTR_MASK				= VNODE_ATTR_READONLY | VNODE_ATTR_HIDDEN | VNODE_ATTR_SYSTEM |
								VNODE_ATTR_DIRECTORY | VNODE_ATTR_ARCHIVE | VNODE_ATTR_ENCRYPTED |
								VNODE_ATTR_LINKED | VNODE_ATTR_NESTED | VNODE_ATTR_NORMAL | 
								VNODE_ATTR_DEVICE
};

/**
* @enum		_FORMAT_METHOD
* @brief	format method using format_ex in RTOS
*/
typedef unsigned int		FORMAT_METHOD;
enum _FORMAT_METHOD 
{
	FORMAT_SCAN_SKIP		= 0x00000001,	/* skip bad sector scan */
	FORMAT_SCAN_WRITE		= 0x00000002,	/* fill sectors with the scanning buffer */
	FORMAT_SCAN_READONLY	= 0x00000004,	/* read sectors */

	FORMAT_RECOVERY_OFF		= 0x00000000,	/* don't recover bad sectors */
	FORMAT_RECOVERY_ON		= 0x00000010,	/* automatically recover bad sectors(if found during the scan) */
	FORMAT_RESERVED			= 0x0FFFFFFF	/* reserved (this makes this enum type 4 bytes in ADS) */
};

/**
* @enum		_LOGICAL_DISK_STATUS
* @brief	Flag in LogDiskStatus
*/
typedef t_uint32			LOGICAL_DISK_STATUS;
enum _LOGICAL_DISK_STATUS
{
	LOGICAL_DISK_INITIALIZED=	0x00000000,	/* logical disk available */
	LOGICAL_DISK_MOUNTING	=	0x00000001,	/* mounting process */
	LOGICAL_DISK_MOUNTED	=	0x00000002,	/* used by a volume */
	LOGICAL_DISK_FORMATTING	=	0x00000004,	/* formating process  */
	LOGICAL_DISK_SCANNING	=	0x00000008,	/* checking process */
};


/******************************************************************************/
/* STRUCTURE                                                                  */
/******************************************************************************/
typedef struct _VNODE					VNODE, *PVNODE;
typedef struct _FILE_CONTROL_BLOCK		FILE_CONTROL_BLOCK, *PFILE_CONTROL_BLOCK;
typedef struct _VOLUME_CONTROL_BLOCK	VOLUME_CONTROL_BLOCK, *PVOLUME_CONTROL_BLOCK;
typedef struct _LOGICAL_DISK			LOGICAL_DISK, *PLOGICAL_DISK;	

typedef struct _VN_BUFFER_INFO			VN_BUFFER_INFO, *PVN_BUFFER_INFO;
typedef struct _VOL_BUFFER_INFO			VOL_BUFFER_INFO, *PVOL_BUFFER_INFO;
typedef struct _PATH_ENTRY				PATH_ENTRY, *PPATH_ENTRY;

typedef struct _FILE_OPERATIONS			FILE_OPERATIONS, *PFILE_OPERATIONS;
typedef struct _VNODE_OPERATIONS		VNODE_OPERATIONS, *PVNODE_OPERATIONS;
typedef struct _VCB_OPERATIONS			VCB_OPERATIONS, *PVCB_OPERATIONS;
typedef struct _NATIVEFS_OPERATIONS		NATIVEFS_OPERATIONS, *PNATIVEFS_OPERATIONS;

/**
 * @struct	_COMP_TIMESPEC
 * @brief	compressed time
 */
typedef struct _COMP_TIMESPEC
{
	COMP_TIME						dwTime;		/**< hour, min, sec, millisec */
	COMP_DATE						dwDate;		/**< year, month, day, date */
} COMP_TIMESPEC, *PCOMP_TIMESPEC;

/**
 * @struct	_SYS_TIME
 * @brief	time
 */
typedef struct _SYS_TIME						
{
	unsigned short					wYear;			/**< year */
	unsigned short					wMonth;			/**< month */
	unsigned short					wDayOfWeek;		/**< day of week */
	unsigned short					wDay;			/**< day */
	unsigned short					wHour;			/**< hour */
	unsigned short					wMinute;		/**< minute */
	unsigned short					wSecond;		/**< second */
	unsigned short					wMilliseconds;	/**< milli second */
} SYS_TIME, *PSYS_TIME, FILE_TIME, *PFILE_TIME;

/**
* @struct	_HPA_INFORMATION
* @brief	hidden protected area information
*/
typedef struct _HPA_INFORMATION
{
	unsigned int	dwClusterCount;				/**< total cluster count for HPA */
	unsigned int	dwFreeClusterCount;			/**< total free cluster count for HPA */
	unsigned int	dwAvailableBlockCount;		/**< available Blocks can be assigned to HPA */
} HPA_INFORMATION, *PHPA_INFORMATION;

/**
 * @struct	_VOLUME_INFORMATION
 * @brief	volume information
 */
typedef struct _VOLUME_INFORMATION	
{
	unsigned int	dwFsName;				/**< fat or fatex or exfat? see NS_NFS_NAME */
	unsigned int	dwFsType;				/**< fat12, fat16, fat32, or non-fat? see NS_NFS_TYPE */
	unsigned int	dwClusterSize;			/**< cluster size byte */
	unsigned int	dwSectorsPerCluster;	/**< sector counts per cluster */
	unsigned int	dwBytesPerSector;		/**< sector size */
	unsigned int	dwNumClusters;			/**< total cluster count */
	unsigned int	dwNumFreeClusters;		/**< free cluster count */
	unsigned int	dwNumAvailClusters;		/**< available cluster count to non-super user */
	unsigned int	dwMaxFileNameLen;		/**< max file name length */
	unsigned int	ullMaxFileSize;			/**< max file size */
	wchar_t			wszVolumeLabel[MAX_VOLUME_LABEL_LENGTH+1];	/**< volume label */

	HPA_INFORMATION	stHpa;					/**< HPA infomation */
} VOLUME_INFORMATION, *PVOLUME_INFORMATION;

/**
 * @struct	_FS_VOLUME_NAME
 * @brief	volume name
 * @see		
 *
 */
typedef struct _FS_VOLUME_NAME
{
	PVOLUME_CONTROL_BLOCK	pVcb;								/**< volume information */
	unsigned int			dwLen;										/**< name length */
	wchar_t					wszVolumeName[MAX_VOLUME_LABEL_LENGTH + 1];	/**< name including NULL */
} FS_VOLUME_NAME, *PFS_VOLUME_NAME;

/**
 * @struct	_DIRECTORY_ENTRY
 * @brief	directory entry
 */
typedef struct _DIRECTORY_ENTRY			
{
	unsigned long long	llVnodeID;			/**< File ID */
	unsigned int		dwFileAttribute;	/**< attribute */
	
	SYS_TIME			ftCreated;			/**< creation time */
	SYS_TIME			ftLastAccessed;		/**< access time */
	SYS_TIME			ftLastModified;		/**< update time */

	unsigned long long	llFileSize;			/**< file size */

	wchar_t				wszName[MAX_FILE_NAME_LENGTH+1];			/**< file name including NULL */	
	wchar_t				wszAltName[MAX_ALT_FILE_NAME_LENGTH+1];	/**< alt file name including NULL */
	unsigned int		dwVolId;			/**< volume id */

} DIRECTORY_ENTRY, *PDIRECTORY_ENTRY;

/**
 * @struct	_FLOCK
 * @brief	Define flock structure 
 */
typedef struct _FLOCK
{
	int					l_type;			/**< lock type,  In WinCE, it's  dwFlag */
	int					l_pid;			/**< lock owner */
	int					l_whence;		/**< flag */
} FLOCK, *PFLOCK;

/**
 * @struct	_FS_DE_CACHE
 * @brief	structure for Directory Entry Cache
 */
typedef struct _FS_DE_CACHE
{
	char*				pBuff;			/**< buffer pointer */
	int					dwSize;			/**< size of buffer */
	void*				pFcb;			/**< pointer to FCB */
} FS_DE_CACHE, *PFS_DE_CACHE;

/**
* @struct	_FS_NATIVE_CACHE
* @brief	structure for add/remove Native filesystem internal Cache
*/
typedef struct _FS_NATIVE_CACHE
{
	char*				pBuff;			/**< buffer pointer */
	int					dwSize;			/**< buffer size */
	int					dwSectorSize;	/**< sector size descriptor */
} FS_NATIVE_CACHE, *PFS_NATIVE_CACHE;


/**
* @struct _VECTOR_ENTRY
* @brief I/O repository for vector I/O
*/
typedef struct _VECTOR_ENTRY
{
	unsigned int		dwBlock;		/**< logical block number */
	unsigned int		dwCount;		/**< block count */
	char*				pBuff;			/**< data buffer */
} VECTOR_ENTRY, *PVECTOR_ENTRY;

/**
* @struct _VECTOR_INFO
* @brief a batch of vector I/O 
*/
typedef struct _VECTOR_INFO
{
	unsigned int		dwTotalEntryCount;	/**< Total entry count */
	unsigned int		dwValidEntryCount;	/**< block count */
	PVECTOR_ENTRY		pVE;				/**< vector entry */
} VECTOR_INFO, *PVECTOR_INFO;

/**
 * @struct	_DISK_SECTOR_RANGE
 * @brief	disk sector range structure
 */
typedef struct _DISK_SECTOR_RANGE
{
	unsigned int		dwSectorNum;		/**< start number of disk sector */
	unsigned int		dwNumSectors;		/**< number of disk sector */
} DISK_SECTOR_RANGE, *PDISK_SECTOR_RANGE;	

/**
 * @struct	_DISK_IO_REQUEST
 * @brief	disk io request structure
 */
typedef struct _DISK_IO_REQUEST
{
	unsigned int		dwSectorNum;		/**< start number of disk sector */
	unsigned int		dwNumSectors;		/**< number of disk sector */
	char*				pBuf;				/**< user I/O buffer */
} DISK_IO_REQUEST, *PDISK_IO_REQUEST;	

/**
 * @struct	_NAND_DEVICE_INFO
 * @brief	nand device information structure
 */
struct _NAND_DEVICE_INFO
{
	unsigned int		dwSectorSize;				/**< sector size. should be multiple of 512 */
	unsigned int		dwSectorSizeBits;			/**< sector size bits. 512 -> 9 */

	unsigned int		dwSectorCount;				/**< total sector count */
	unsigned int		dwStartSector;				/**< start sector, normally 0 */
	unsigned int		dwEndSector;				/**< end sector, normally sector_count - 1 */
	unsigned int		dwOptimalIOSize;			/**< Optimal R/W size in sector */

	unsigned int		dwFlag;						/**< device flag.*/

	wchar_t				wszDeviceName[20 + 1];		/**< device name included null*/
};

/**
 * @struct	_NAND_BLOCK_INFO
 * @brief	nand block information structure
 */
struct _NAND_BLOCK_INFO
{
	unsigned int		dwNumBlocks;		/**< Block counts */
	unsigned int		dwPagesPerBlock;	/**< Pages Per Block */
};

/**
 * @struct	_FORMAT_PARAMETER
 * @brief	format parameter
 */
typedef struct _FORMAT_PARAMETER 
{
	NFS_TYPE			dwFileSystemType;	/**< filesystem type */
	unsigned int		dwClusterSize;		/**< cluster size */
	const wchar_t*		pwszLabel;			/**< volume label. */
	
} FORMAT_PARAMETER, *PFORMAT_PARAMETER;

/**
* @struct	_FORMAT_DISK_INFO
* @brief	disk information structure
* @see		
*
*/
typedef struct _FORMAT_DISK_INFO {
	unsigned int		dwNumSectors;				/**< total sectors */
	unsigned int		dwBytesPerSector;			/**< sector size */
	DISK_FLAGS			dwFlags;					/**< disk flag.. see DISK_FLAGS */
	unsigned int		dwStartSectorNum;			/**< physical sector number in the corresponding logical first sector */
} FORMAT_DISK_INFO, *PFORMAT_DISK_INFO;

/**
 * @struct	_FAT_CLEAN_PARAMETER
 * @brief	fat clensing parameter
 */
typedef struct _FAT_CLEAN_PARAMETER
{
	unsigned int		dwStartCluster;		/**< start cluster to clean */
	unsigned int		dwClusterCount;		/**< cluster count to be tested for cleaning */
} FAT_CLEAN_PARAMETER, *PFAT_CLEAN_PARAMETER;

/**
 * @struct	_FS_VOL_INFO
 * @brief	Filesystem volume information
 */
typedef struct _FS_VOL_INFO
{
	unsigned short	wVcbCnt;
	unsigned short	wNodeCnt;
	unsigned int	dwVcbMap;
} FS_VOL_INFO, *PFS_VOL_INFO;

/**
* @struct	_LOCKED_REGION
* @brief	Internal data structure for representing a file region-lock
*/
typedef struct _LOCKED_REGION
{
	unsigned int					dwTaskId;		/**< File region lock owner task id */
	PFILE_CONTROL_BLOCK				pFcb;			/**< File region lock owner FCB */
	unsigned int					dwLockType;		/**< File region lock type */
	FILE_OFFSET						llStartOffset;	/**< File region lock start offset */
	FILE_OFFSET						llEndOffset;	/**< File region lock end offset */
	DLIST_ENTRY(struct _LOCKED_REGION)		dleSiblings;	/**< Lock region linked list */
} LOCKED_REGION, *PLOCKED_REGION;


#if defined(CONFIG_RTOS) || defined(CONFIG_SYMBIAN) || defined(CONFIG_WINCE)

/**
 * @struct	_FILE_CONTROL_BLOCK
 * @brief	Internal data structure for representing an opened file or directory.
 * @see		FileHandleToFcb() FileFcbToHandle()
 *
 * An internal data structure in which the state of an opened file or directory
 * is maintained. The information in the FCB are not dependent on the native
 * file system.
 */
struct _FILE_CONTROL_BLOCK
{
#if defined(CONFIG_RTOS)
	unsigned short					wIndex;			/**< Unique index for this FCB */
	unsigned short					wChecksum;		/**< Checksum for verifying the file handle value */
#endif

	PVNODE							pVnode;			/**< Pointer to the node that this FCB refers\n Note that this can be NULL if the node was elliminated. */

	PFILE_OPERATIONS				pFileOps;		/**< File operations */
	FILE_OFFSET						llOffset;		/**< Current file offset (64bits) */

	FCB_ACCESS_MODE					dwAccessMode;	/**< Access mode specified when the file is opened\n If this FCB refers to a node of directory type, it will be ignored.
													     Refer to FCB_ACCESS_MODE */
	unsigned int					dwThreads;		/**< Thread count */	

	DLIST_ENTRY(struct _FILE_CONTROL_BLOCK)	dleSiblings;	/**< List entry for linking siblings referencing the same node */

	void*							pPrivate;		/**< Glue private data */
};

/**
* @struct	_VNODE
* @brief	Internal data structure to represent a file or directory.
*/
struct _VNODE
{
	unsigned long long				llVnodeID;		/**< unique index number */
	unsigned int					cRef;			/**< reference count */
	unsigned long long				llParentVnodeID;/**< parent unique index number */

	FILE_SIZE						llSize;			/**< file size */
	VNODE_ATTR						dwAttr;			/**< file attributes. */
	VNODE_STATUS					dwStatus;		/**< vnode status. */

	unsigned short					wLinkCnt;		/**< file link count. usually 1. 0 means deleted file */
	COMP_TIMESPEC					dwCreated;		/**< creation time */
	COMP_TIMESPEC					dwLastAccess;	/**< access time */
	COMP_TIMESPEC					dwLastModified;	/**< modification time */
#if defined(CONFIG_WINCE)
	FILETIME						dwftCreated;	/**< additional creation time */
	FILETIME						dwftLastAccess;	/**< additional last access time */
	FILETIME						dwftLastModified;/**< additional last modification time*/
#endif

	SEMAPHORE						bsLock;			/**< synchronization object for protecting itself */
	RW_SEMAPHORE					rwLock;			/**< synchronization object to prevent the race bet'n file shrink & file read */ 
	
	DLIST(FILE_CONTROL_BLOCK)		dlOpenedFcbs;	/**< list header for enumerating fcbs attached to this node */
	DLIST_ENTRY(VNODE)				dleSiblings;	/**< list entry for linking sibling nodes which belongs to the same volume */

	// file region lock
	DLIST(LOCKED_REGION)			dlLockedRegions;/**< list head containing information of the locked regions */

	PVOLUME_CONTROL_BLOCK			pVcb;			/**< the volume object belonging to */
	PPATH_ENTRY						pPathEntry;		/**< the path cache entry */ 
	PVNODE_OPERATIONS				pVnodeOps;		/**< the node operations */
	void*							pFileOps;		/**< the file operations */
	void*							pNative;		/**< the native inode */

	PVN_BUFFER_INFO					pVnBch;			/**< the buffer cache for Vnode */

	unsigned int					dwUID;			/**< user ID */
	unsigned int					dwGID;			/**< Group ID */
	unsigned short					wPerm;			/**< Access permission */

};

/**
 * @struct	_VOLUME_CONTROL_BLOCK
 * @brief	Structrue for handling a volume
 * @see		Related Vcb function
 *
 */
struct _VOLUME_CONTROL_BLOCK
{
	SEMAPHORE					bsRenameLock;	/**< Lock for cross-directory rename */

	wchar_t						wszName[MAX_MOUNT_NAME_LENGTH + 1];	/**< Access path to mounted volume */
	unsigned int				cchName;		/**< Char count of wszName */

	MOUNT_FLAG					dwFlag;			/**< vcb flag (transaction safe, case sensitive, ...) */
	VCB_STATUS					dwStatus;		/**< vcb status (mounting, active, ...) */

	unsigned int				dwBlockSize;	/**< Block size in Bytes */
	unsigned int				dwBlockSizeBits;/**< Block Size in Bit, 512 = 9 */

	PVNODE						pnRoot;			/**< Root Node Pointer */
	PLOGICAL_DISK				pDisk;			/**< Logical Disk Pointer */
	PVOL_BUFFER_INFO			pVolBch;		/**< buffer cache manager */

	PNATIVEFS_OPERATIONS		pNativeFSOps;		/**< native filesystem */
	PVCB_OPERATIONS				pVolumeOps;		/**< Volume Operation (function pointer list) */

	void*						pNativeVcb;		/**< Native Volume Control Block pointer */
	void*						pPrivate;		/**< Private Volume Control Block pointer */
	DLIST(VNODE)				dlVnodes;		/**< Vnode list in this volume */
	DLIST_ENTRY(VOLUME_CONTROL_BLOCK)	dleSiblings;	/**< List entry for Vcb list */

	FILE_SIZE					llVolumeSize;	/**< Volume's byte size */
	
};
#endif /*defined(CONFIG_RTOS) || defined(CONFIG_SYMBIAN) || defined(CONFIG_WINCE)*/


/******************************************************************************/
/* OPERATIONS                                                                 */
/******************************************************************************/

typedef FERROR (*PFN_NATIVEFS_OPS_MOUNT_DISK)(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IO	PMOUNT_FLAG				pMountFlags,
	OUT	PVNODE*					ppVnodeRoot);

typedef FERROR (*PFN_NATIVEFS_OPS_FORMAT)(
	IN	PLOGICAL_DISK			pLogDisk,
	IN	PFORMAT_PARAMETER		pFormatParam,
	IN	PFORMAT_DISK_INFO		pDiskInfo);

typedef FERROR (*PFN_NATIVEFS_OPS_CHKDSK)(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	void*					pInBuf,
	IN	unsigned int			dwInBufSize,
	OUT	void*					pOutBuf,
	IN	unsigned int			dwOutBufSize);

typedef FERROR (*PFN_NATIVEFS_OPS_INIT)(void);

typedef FERROR (*PFN_NATIVEFS_OPS_UNINIT)(void);

typedef FERROR (*PFN_NATIVEFS_OPS_FSCTL)(
	IN	unsigned int			dwControlCode,
	IN	void*					pInBuf,
	IN	unsigned int			dwInBufSize,
	OUT	void*					pOutBuf,
	OUT	unsigned int			dwOutBufSize);

/**
* @struct	_NATIVEFS_OPERATIONS
* @brief	native filesystem type (interface)
*/
struct _NATIVEFS_OPERATIONS
{
	wchar_t*						pwszName;		/**< file system type (ex : L"FAT") */
	PFN_NATIVEFS_OPS_MOUNT_DISK		pfnMountDisk;	/**< MountDisk Interface			*/
	PFN_NATIVEFS_OPS_FORMAT			pfnFormat;		/**< Format Interface               */
	PFN_NATIVEFS_OPS_CHKDSK			pfnChkdsk;		/**< Chkdsk (scan) Interface        */
	PFN_NATIVEFS_OPS_INIT			pfnNativeInit;	/**< Initialize a native filesystem */
	PFN_NATIVEFS_OPS_UNINIT			pfnNativeUninit;/**< Uninitialize a native filesystem */
	PFN_NATIVEFS_OPS_FSCTL			pfnFsctl;		/**< Fsctl of native filesystem, for checking if the first sector is BPB or not */
};


typedef FERROR (*PFN_VCB_OPS_WRITE_VOLUME) (
	IN	PVOLUME_CONTROL_BLOCK	pVcb);

typedef FERROR (*PFN_VCB_OPS_GET_VOLUME_INFORMATION) (
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	OUT	PVOLUME_INFORMATION		pVolumeInfo);

typedef FERROR (*PFN_VCB_OPS_UNMOUNT_DISK)(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	UNMOUNT_FLAG			dwUnmountFlags);

typedef FERROR (*PFN_VCB_OPS_REMOUNT_DISK)(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IO	PREMOUNT_FLAG			pRemountFlags);

typedef FERROR (*PFN_VCB_OPS_FSIOCTL) (
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	unsigned int			dwControlCode,
	IN	void*					pInBuf,
	IN	unsigned int			dwInBufSize,
	OUT	void*					pOutBuf,
	IN	unsigned int			dwOutBufSize);

/**
* @struct	_VCB_OPERATIONS
* @brief	VCB operations interface
*/
struct _VCB_OPERATIONS
{
	PFN_VCB_OPS_WRITE_VOLUME				pfnWriteVolume;			/**< write volume interface */
	PFN_VCB_OPS_GET_VOLUME_INFORMATION		pfnGetVolumeInformation;/**< get volume info interface */
	PFN_VCB_OPS_UNMOUNT_DISK				pfnUnmountDisk;			/**< unmount interface */
	PFN_VCB_OPS_REMOUNT_DISK				pfnRemountDisk;			/**< remount interface */
	PFN_VCB_OPS_FSIOCTL						pfnFsIoctl;				/**< fsctl interface */
};


typedef FERROR (*PFN_VNODE_OPS_OPEN) (
	IN	PFILE_CONTROL_BLOCK		pFcb);

typedef FERROR (*PFN_VNODE_OPS_LOOKUP) (
	IN	PVNODE					pnParent,
	IN	const wchar_t*			pwsName,
	IN	unsigned int			dwNameLen,
	IN	LOOKUP_FLAG				dwLookupFlag,
	OUT	PVNODE*					ppVnode);

typedef FERROR (*PFN_VNODE_OPS_CREATE_DIRECTORY) (
	IN	PVNODE					pnParent,
	IN	const wchar_t*			pwsName,
	IN	unsigned int			dwNameLen,
	IN	FILE_ATTR				dwAttr,
	IN	unsigned int			dwUid,
	IN	unsigned int			dwGid,
	IN	ACL_MODE				wAccessMode,
	OUT PVNODE*					ppVnode);

typedef FERROR (*PFN_VNODE_OPS_CREATE_FILE) (
	IN	PVNODE					pnParent,
	IN	const wchar_t*			pwsName,
	IN	unsigned int			dwNameLen,
	IN	FILE_ATTR				dwAttr,
	IN	unsigned int			dwUid,
	IN	unsigned int			dwGid,
	IN	ACL_MODE				wAccessMode,
	OUT	PVNODE*					ppVnode);

typedef FERROR (*PFN_VNODE_OPS_CREATE_SYMLINK) (
	IN	PVNODE					pnSourceParent,
	IN	const wchar_t*			pwsName,
	IN	unsigned int			dwNameLen,
	IN	FILE_ATTR				dwAttr,
	IN	unsigned int			dwUid,
	IN	unsigned int			dwGid,
	IN	ACL_MODE				wAccessMode,
	IN	const wchar_t*			pwsTargetPath,
	OUT	PVNODE*					ppVnode);

typedef FERROR (*PFN_VNODE_OPS_READ_SYMLINK) (
	IN	PVNODE					pVnode,
	OUT	wchar_t*				pwszLinkBuff,
	IN	unsigned int			dwLinkBuffSize,
	OUT	unsigned int*			pLinkLen);

typedef FERROR (*PFN_VNODE_OPS_UNLINK) (
	IN	PVNODE					pnParent,
	IN	PVNODE					pnTarget,
	IN	BOOL					bIsOpened);

typedef FERROR (*PFN_VNODE_OPS_MOVE) (
	IN	PVNODE					pnSourceParent,
	IN	PVNODE					pnSourceFile,
	IN	PVNODE					pnTargetParent,
	IN	const wchar_t*			pwszNewName,
	IN	unsigned int			dwNameLen,
	IN	BOOL					bIsSourceOpened);

typedef FERROR (*PFN_VNODE_OPS_MOVE2) (
	IN	PVNODE					pnSourceParent,
	IN	PVNODE					pnSourceFile,
	IN	PVNODE					pnTargetParent,
	IN	PVNODE					pnTargetFile,
	IN	const wchar_t*			pwszNewName,
	IN	unsigned int			dwNameLen,
	IN	BOOL					bIsSourceOpened,
	IN	BOOL					bIsTargetOpened);

typedef FERROR (*PFN_VNODE_OPS_DELETE_NODE) (
	IN	PVNODE					pVnode);

typedef FERROR (*PFN_VNODE_OPS_DESTROY_NODE) (
	IN	PVNODE					pVnode);

typedef FERROR (*PFN_VNODE_OPS_SET_ATTRIBUTES) (
	IN	PVNODE					pVnode,
	IN	FILE_ATTR				dwAttr);

typedef FERROR (*PFN_VNODE_OPS_SET_FILESIZE) (
	IN	PVNODE					pVnode,
	IN	FILE_SIZE				llFileSize,
	IN	BOOL					bFillZero);

typedef FERROR (*PFN_VNODE_OPS_SET_FILETIME) (
	IN	PVNODE					pVnode,
	IN	PSYS_TIME				ptmCreated,
	IN	PSYS_TIME				ptmLastAccessed,
	IN	PSYS_TIME				ptmLastModified);

typedef FERROR (*PFN_VNODE_OPS_SET_GUID_MODE) (
	IN	PVNODE					pVnode,
	IN	unsigned int			dwUid,
	IN	unsigned int			dwGid,
	IN	ACL_MODE				wPerm);

typedef FERROR (*PFN_VNODE_OPS_MAP_BLOCKS) (
	IN	PVNODE					pVnode,
	IN	unsigned int			dwBlockIndex,
	IN	unsigned int			dwBlockCnt,
	OUT	unsigned int*			pdwBlockNum,
	OUT	unsigned int*			pdwContBlockCnt);

typedef FERROR (*PFN_VNODE_OPS_SYNC_FILE) (
	IN	PVNODE					pVnode,
	IN	FILE_SIZE				llValidFileSize,
	IN	BOOL					bUpdateAccessTime,
	IN	BOOL					bUpdateModifyTime);

typedef FERROR (*PFN_VNODE_OPS_PERMISSION) (
	IN	PVNODE					pVnode,
	IN	OPERATION_MODE			dwOperationMode);

typedef FERROR (*PFN_VNODE_OPS_INDEX_UNLINK) (
	IN	PVNODE					pVnode,
	IN	PFILE_CONTROL_BLOCK		pFcb,
	IN	unsigned long long		llVnodeID);

typedef FERROR (*PFN_VNODE_OPS_CLEAN_DIRECTORY) (
	IN	PVNODE					pVnode);

typedef FERROR (*PFN_VNODE_OPS_EXPAND_CLUSTERS) (
	IN PVNODE					pVnode,
	IN FILE_SIZE				llSize);

typedef FERROR (*PFN_VNODE_OPS_CONVERT_NAME) (
	IN PVNODE					pParent,
	IN const wchar_t*			pwszInputName,
	IN const unsigned int		pwszInputSize,
	OUT wchar_t*				pwsOutputName,
	IO unsigned int				dwOutputNameSize,
	IN unsigned int				dwConvertType);

typedef FERROR (*PFN_VNODE_OPS_SET_XATTRIBUTE) (
	IN PVNODE					pVnode,
	IN const char*				psName,
	IN const void*				pValue,
	IN unsigned int				dwValueSize,
	IN XATTR_NAMESPACE_ID		dwID,
	IN XATTR_SET_FLAG			dwFlag);

typedef FERROR (*PFN_VNODE_OPS_GET_XATTRIBUTE) (
	IN PVNODE					pVnode,
	IN const char*				psName,
	IN const void*				pValue,
	IN unsigned int				dwValueSize,
	IN XATTR_NAMESPACE_ID		dwID,
	OUT unsigned int*			dwSizeRead);

typedef FERROR (*PFN_VNODE_OPS_LIST_XATTRIBUTES) (
	IN PVNODE					pVnode,
	OUT char*					pOutBuf,
	IN unsigned int				dwOutBufSize,
	OUT unsigned int*			dwSizeRead);

typedef FERROR (*PFN_VNODE_OPS_REMOVE_XATTRIBUTE)(
	IN PVNODE					pVnode,
	IN const char*				psName,
	IN XATTR_NAMESPACE_ID		dwID);

/**
* @struct	_VNODE_OPERATIONS
* @brief	vnode operations interface
*/
struct _VNODE_OPERATIONS
{
	PFN_VNODE_OPS_OPEN				pfnOpen;			/**< open interface */

	PFN_VNODE_OPS_LOOKUP			pfnLookupChild;		/**< lookup interface */
	PFN_VNODE_OPS_CREATE_DIRECTORY	pfnCreateDirectory;	/**< create dir interface */
	PFN_VNODE_OPS_CREATE_FILE		pfnCreateFile;		/**< create file interface */
	PFN_VNODE_OPS_CREATE_SYMLINK	pfnCreateSymlink;	/**< create symlink interface */
	PFN_VNODE_OPS_READ_SYMLINK		pfnReadSymlink;		/**< read symlink interface */
	PFN_VNODE_OPS_UNLINK			pfnUnlink;			/**< unlink interface */
	PFN_VNODE_OPS_MOVE				pfnMove;			/**< move interface */
	PFN_VNODE_OPS_MOVE2				pfnMove2;			/**< move2 interface */
	PFN_VNODE_OPS_INDEX_UNLINK		pfnIndexUnlink;		/**< index unlink interface for readdir_unlink */
	PFN_VNODE_OPS_CLEAN_DIRECTORY	pfnCleanDirectory;	/**< clean dir interface */

	PFN_VNODE_OPS_DELETE_NODE		pfnDeleteNode;		/**< delete node interface */
	PFN_VNODE_OPS_DESTROY_NODE		pfnDestroyNode;		/**< destroy node interface */

	PFN_VNODE_OPS_SET_ATTRIBUTES	pfnSetAttributes;	/**< set attribute interface */
	PFN_VNODE_OPS_SET_FILESIZE		pfnSetFileSize;		/**< set file size interface */
	PFN_VNODE_OPS_SET_FILETIME		pfnSetFileTime;		/**< set file time interface */
	PFN_VNODE_OPS_SET_GUID_MODE		pfnSetGuidMode;		/**< set uid, gid, and permission of user, group, other */

	PFN_VNODE_OPS_MAP_BLOCKS		pfnMapBlocks;		/**< map blocks interface */
	PFN_VNODE_OPS_SYNC_FILE			pfnSyncFile;		/**< sync file interface */

	PFN_VNODE_OPS_PERMISSION		pfnPermission;		/**< permission interface */
	PFN_VNODE_OPS_EXPAND_CLUSTERS	pfnExpandClusters;	/**< expand cluster interface */
	PFN_VNODE_OPS_CONVERT_NAME		pfnConvertName;		/**< convert name (long file name <-> short name interface */

	PFN_VNODE_OPS_SET_XATTRIBUTE	pfnSetXAttribute;	/**< set a value of an extended attribute */
	PFN_VNODE_OPS_GET_XATTRIBUTE	pfnGetXAttribute;	/**< get a value of an extended attribute */
	PFN_VNODE_OPS_LIST_XATTRIBUTES	pfnListXAttributes;	/**< list up the names of extended attributes */
	PFN_VNODE_OPS_REMOVE_XATTRIBUTE	pfnRemoveXAttribute;/**< remove an extended attribute */
};


typedef FERROR (*PFN_FILE_OPS_CLOSE) (
	IN	PFILE_CONTROL_BLOCK		pFcb);

typedef FERROR (*PFN_FILE_OPS_READ_DIRECTORY) (
	IN	PFILE_CONTROL_BLOCK		pFcb,
	IN	const wchar_t*			pwszFileNameToSearch,
	OUT	PDIRECTORY_ENTRY		pEntry,
	OUT	unsigned int*			pdwNumberOfRead);

typedef FERROR (*PFN_FILE_OPS_WRITE_FILE) (
	IN	PFILE_CONTROL_BLOCK		pFcb,
	IN	unsigned char*			pBuffer,
	IN	unsigned int			dwBytesToWrite,
	OUT	unsigned int*			pdwBytesWritten);

typedef FERROR (*PFN_FILE_OPS_WRITE_FILE_BEGIN) (
	IN	PFILE_CONTROL_BLOCK		pFcb,
	IN	FILE_OFFSET				llStartOffset,
	IN	unsigned int			dwBytesToWrite);

typedef FERROR (*PFN_FILE_OPS_WRITE_FILE_END) (
	IN	PFILE_CONTROL_BLOCK		pFcb,
	IN	unsigned int			dwBytesWritten,
	IN	FERROR					err);

typedef FERROR (*PFN_FILE_OPS_READ_FILE) (
	IN	PFILE_CONTROL_BLOCK		pFcb,
	IN	unsigned char*			pBuffer,
	IN	unsigned int			dwBytesToRead,
	OUT	unsigned int*			pdwBytesRead);

typedef FERROR (*PFN_FILE_OPS_SEEK_FILE) (
	IO	PFILE_CONTROL_BLOCK		pFcb,
	IN	FILE_OFFSET				llOffset);

typedef FERROR (*PFN_FILE_OPS_IOCTL) (
	IN	PFILE_CONTROL_BLOCK		pFcb,
	IN	PVNODE					pVnode,
	IN	IOCTL_CODE				dwControlCode,
	IN	void*					pInBuf,
	IN	unsigned int			dwInBufSize,
	OUT	void*					pOutBuf,
	IN	unsigned int			dwOutBufSize);

/**
* @struct	_FILE_OPERATIONS
* @brief	file operations interface
*/
struct _FILE_OPERATIONS
{
	PFN_FILE_OPS_CLOSE				pfnClose;			/**< close Interface			*/
	PFN_FILE_OPS_READ_DIRECTORY		pfnReadDirectory;	/**< readdir Interface			*/
	PFN_FILE_OPS_READ_FILE			pfnReadFile;		/**< read file Interface		*/
	PFN_FILE_OPS_WRITE_FILE			pfnWriteFile;		/**< write file Interface		*/
	PFN_FILE_OPS_WRITE_FILE_BEGIN	pfnWriteFileBegin;	/**< write_file_begin Interface	*/
	PFN_FILE_OPS_WRITE_FILE_END		pfnWriteFileEnd;	/**< write_file_end Interface	*/
	PFN_FILE_OPS_SEEK_FILE			pfnSeekFile;		/**< seek file Interface		*/
};
	

#ifdef TFS5_DEBUG
/**
* @struct	_FILE_SECTOR_DUMP
* @brief	dump a file sector
*/
typedef struct _FILE_SECTOR_DUMP
{
	PFILE_CONTROL_BLOCK		pFcb;
	unsigned int			dwFileSector;
	unsigned int			dwLogicalSector;
	unsigned int			dwPhysicalSector;
	char*					pBuffer;
} FILE_SECTOR_DUMP, *PFILE_SECTOR_DUMP;
#endif


/******************************************************************************/
/* ERROR TYPES                                                                */
/******************************************************************************/

/**
 * @enum	_FERROR
 * @brief	Nestle error type
 * @note	'[@ERROR]' mark - This error type do not exactly to match with POSIX. 
 */
enum _FERROR
{
	/**< The operation success, no error */
	FERROR_NO_ERROR					=	 0x00000000,		// 0
	
	/**< [ENOMEM] Not enough space. 
	The new process image requires more memory than is allowed by the hardware 
	or system-imposed memory management constraints. */
	FERROR_INSUFFICIENT_MEMORY		=	-0x00000001,		// 1

	/**< [EIO] Input/output error. 
	Some physical input or output error has occurred. 
	This error may be reported on a subsequent operation on the same file descriptor. 
	Any other error-causing operation on the same file descriptor may cause the 
	[EIO] error indication to be lost.*/
	FERROR_IO_ERROR					=	-0x00000002,		// 2

	/**< [@EINVAL]  An attempt was made to move the file pointer before 
	the beginning of the file. */
	FERROR_NEGATIVE_FILE_SEEK		=	-0x00000003,		// 3

	/**< [EINVAL] Invalid argument. Some invalid argument was supplied; 
	for example, specifying an undefined parameter, device name or filesystem name.*/
	FERROR_INVALID					=	-0x00000004,		// 4

	/**< [ELOOP] Symbolic link loop. 
	A loop exists in symbolic links encountered during pathname resolution. 
	This error may also be returned if more than {MAX_NESTED_SYMLINK} symbolic links 
	are encountered during pathname resolution. */
	FERROR_MAX_SYMLINK_LOOP			=	-0x00000005,		// 5

	/**< [ENOSYS] Function not implemented. An attempt was made to use a function 
	that is not available in this implementation. */
	FERROR_NOT_SUPPORTED			=	-0x00000006,		// 6

	/**< [EFALUT] Bad address. The system detected an invalid address in attempting 
	to use an argument of a	call. The reliable detection of this error cannot be 
	guaranteed, and when not detected may result in the generation of a signal, 
	indicating an address violation, which is sent to the process. */
	FERROR_SYSTEM_PANIC 			=	-0x00000007,		// 7

	/**< [ENODEV] No such device. An attempt was made to apply an inappropriate 
	function to a device. */
	FERROR_DEVICE_NOT_FOUND			=	-0x00000008,		// 8

	/**< [EBUSY] Resource busy. An attempt was made to make use of a system 
	resource that is not currently available, as it is being used by another 
	process in a manner that would have conflicted with	the request being 
	made by this process. */
	FERROR_BUSY						=	-0x00000009,		// 9

	/**< [EBADF] Bad file descriptor. A file descriptor argument is out of range,
	refers to no open file, or a read (write) request is made to a file 
	that is only open for writing (reading). */
	FERROR_INVALID_HANDLE			=	-0x0000000A,		// 10

	/**< [@EFALUT]  block size is smaller than sector size, block size 
	is bigger than buffer cache entry size or native file system did format 
	with page miss align. */
	FERROR_INVALID_BLOCK_SIZE		=	-0x0000000B,		// 11

	/**< [EEXIST] File exists. An existing file was mentioned in an inappropriate 
	context; for example, as a new link name in the link( ) function. */
	FERROR_ALREADY_EXISTS			=	-0x0000000C,		// 12

	/**< [ENOENT] No such file or directory. A component of a specified pathname 
	does not exist, or the pathname is an empty string. */
	FERROR_PATH_NOT_FOUND			=	-0x0000000D,		// 13

	/**< [@ENOENT] There are no more files. */
	FERROR_NO_MORE_ENTRIES			=	-0x0000000E,		// 14

	/**< [EACCES] Permission denied. An attempt was made to access a file in a 
	way forbidden by its file access permissions. */
	FERROR_ACCESS_DENIED			=	-0x0000000F,		// 15

	/**< [ENAMETOOLONG] Filename too long. The length of a pathname exceeds 
	{MAX_PATH_NAME_LENGTH}, or a pathname component is longer than {MAX_FILE_NAME_LENGTH}. 
	This error may also occur when pathname substitution, as a result of 
	encountering a symbolic link during pathname resolution, results in a pathname
	string the size of which exceeds {MAX_PATH_NAME_LENGTH}.*/
	FERROR_NAME_TOO_LONG			=	-0x00000010,		// 16

	/**< [EISDIR] Is a directory. An attempt was made to open a directory with 
	write mode specified. */ 
	FERROR_NOT_A_FILE				=	-0x00000011,		// 17

	/**< [EXDEV] Improper link. A link to a file on another file system was attempted */
	FERROR_MEDIA_EJECTED			=	-0x00000012,		// 18

	/**< [ENOTDIR] Not a directory. A component of the specified pathname exists, 
	but it is not a directory,
	when a directory was expected. */
	FERROR_NOT_A_DIRECTORY			=	-0x00000013,		// 19

	/**< [EMFILE] Too many open files. An attempt was made to open more than the 
	maximum number of file descriptors allowed in this process. */
	FERROR_TOO_MANY_OPEN_FILES		=	-0x00000014,		// 20

	/**< [EROFS] Read-only file system. An attempt was made to modify a file or 
	directory on a file system that is read-only. */
	FERROR_READONLY_FS				=	-0x00000015,		// 21

	/**< [ENOTEMPTY] Directory not empty. A directory other than an empty directory
	was supplied when an empty directory was expected. */
	FERROR_NOT_EMPTY				=	-0x00000016,		// 22

	/**< [@EACCES] Cannot across move operation in different volumes 
	(CAUTION!! Linux is allows)*/
	FERROR_EXCLUSIVE_VOLUME			=	-0x00000017,		// 23

	/**< [ENOSPC] No space left on a device. During the write( ) function on a 
	regular file or when extending a
	directory, there is no free space left on the device. */
	FERROR_NO_FREE_SPACE			=	-0x00000018,		// 24

	/**< [@EFALUT] The native filesystem fails a recovery unfinished operation 
	in mount process */
	FERROR_RECOVERY_FAILURE			=	-0x00000019,		// 25

	/**< [@EFALUT] A file system meta data is broken. */
	FERROR_FILESYSTEM_CORRUPT		=	-0x0000001A,		// 26

	/**< [@EFALUT] A filesystem already initialized, An attempt was made to 
	re-initialize. */
	FERROR_ALREADY_INITIALIZED		=	-0x0000001B,		// 27

	/**< [@EFALUT] filesystem not initialized. */
	FERROR_NOT_INITIALIZED			=	-0x0000001C,		// 28

	/**< [@ENOSPC] Too many file is exist in directory. cannot make file or 
	directory any more. */
	FERROR_DIR_FULL					=	-0x0000001D,		// 29

	/**< [@ENOSPC] Too many extended attribute created, a extended attribute has 
	limit size of repository. */
	FERROR_FULL_XATTR				=	-0x0000001E,		// 30

	/**< [@ENOENT] The named extended attribute does not exist. */
	FERROR_NO_XATTR					=	-0x0000001F,		// 31

	/**< [ERANGE] Result too large or too small. The result of the function is too
	large (overflow) or too small (underflow) to be represented in the available space */
	FERROR_RANGE					=	-0x00000020,		// 32

	/**< [@EPERM] The process cannot access the file because it is being used by 
	another process */
	FERROR_SHARING_VIOLATION		=	-0x00000021,		// 33

	/**< [@EPERM] Access a file that is restricted by native filesystem */
	FERROR_ACCESS_SYSTEMFILE		=	-0x00000022,		// 34

	/**< [@ACCES] Reached the end of the file. */
	FERROR_END_OF_FILE				=	-0x00000023,		// 35

	/**< [ENOENT] No leaf node of such file or directory. 
	A component of a specified pathname does not exist */
	FERROR_FILE_NOT_FOUND			=	-0x00000024,		// 36

	/**< [internal use only] try again to lookup for symbolic link */
	FERROR_RETRY					=	-0x00000100,
};

#endif /* __NS_TYPES_H__ */
