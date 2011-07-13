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
 * @file		ns_nativefs.h
 * @brief		Declarations for interfaces that are exported to the native filesystem
 * This file contains declarations of interfaces and data structures used by the native filesystem.
 */
/**
 * @mainpage	NESTLE Native Filesystem Programmer's Guide
 * @brief		NESTLE Programmer's Guide For Native Filesystem Developers
 * @version		nestle 1.0.0
 * @since		2007
 * @author		ByungJune Song (byungjune.song@samsung.com)
 * @author		InHwan Choi (inhwan.choi@samsung.com)
 * @par Basic Rules
 
 * \li BR1 : Native file system must implement vnode, open file and volume operation that nestle framework defined.
 * \li BR2 : All of data structures and interfaces for native file system are defined by "ns_nativefs.h". Do not include any other header files except for "ns_nativefs.h".
 * \li BR3 : Nestle supports methods to access internal data structures. Do not access directly nestle's structures.
 * \li BR4 : All of inputs and outputs of data use buffer cache manager.
 * \li BR5 : Nestle synchronizes nestle's object by itself throughout native file system's call.
 * \li BR6 : Nestle does not support transaction. Transaction is optional with native file system. 
 */

#ifndef __NESTLE_H__
#define __NESTLE_H__

#include "ns_types.h"
#include "ns_misc.h"
 
#if defined(CONFIG_LINUX)
#include "linux_fcb.h"
#include "linux_vnode.h"
#include "linux_vcb.h"
#include "linux_logdisk.h"
#include "linux_bcache.h"
#endif

#if defined(CONFIG_RTOS)
#include "tfs4_ldev.h"
#endif 

#if defined(CONFIG_WINCE)
#include "logdisk.h"
#endif

#if defined(CONFIG_SYMBIAN)
#include "logdisk.h"
#endif

#if defined(CONFIG_RTOS)
#define NS_CONFIG_RTOS
#endif

#if defined(CONFIG_WINCE)
#define NS_CONFIG_WINCE
#endif

#if defined(CONFIG_LINUX)
#define NS_CONFIG_LINUX
#endif

#if defined(CONFIG_SYMBIAN)
#define NS_CONFIG_SYMBIAN
#endif 

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @defgroup nestle	NESTLE framework
 * @{
 */	

/******************************************************************************/
/* NESTLE Data Types					                                      */
/******************************************************************************/
/** 
 * @defgroup ns_datatype	data types
 * @{
 */	

#define NS_NO_CONCURRENCY					NO_CONCURRENCY
#define NS_SYSTEM_LEVEL_CONCURRENCY			SYSTEM_LEVEL_CONCURRENCY
#define NS_VOLUME_LEVEL_CONCURRENCY			VOLUME_LEVEL_CONCURRENCY
#define NS_FILE_LEVEL_CONCURRENCY			FILE_LEVEL_CONCURRENCY

/** @brief	concurrent level */
#define NS_CONCURRENT_LEVEL					CONCURRENT_LEVEL

#ifdef NATIVE_SUPPORT_SINGLE_LOCK
/** @brief Request for native to use a single lock
 *  Natives are obligated to support single lock
 */
#define NS_SUPPORT_SINGLE_LOCK
#endif

#ifdef MULTIPLE_CHARSET_SUPPORT
/** @brief Support different codepages
 *  Natives with one codepage read symbols from the used card with the other
 */
#define NS_MULTIPLE_CHARSET_SUPPORT
#endif

/** @brief	max dir, file, path length (not including the trailing NULL)*/
#define NS_MAX_DIR_NAME_LENGTH		MAX_DIR_NAME_LENGTH
#define NS_MAX_FILE_NAME_LENGTH		MAX_FILE_NAME_LENGTH
#define NS_MAX_PATH_NAME_LENGTH		MAX_PATH_NAME_LENGTH

/* Numeric tail restriction */
#ifdef MAX_FAT_NUMERIC_TAIL
#define	NS_MAX_FAT_NUMERIC_TAIL		MAX_FAT_NUMERIC_TAIL
#endif

/** @brief	endian byte order */
#define	NS_LITTLE_ENDIAN			LITTLE_ENDIAN
#define NS_BIG_ENDIAN				BIG_ENDIAN
#define NS_BYTE_ORDER				BYTE_ORDER

#if defined(CONFIG_WINCE) || defined(CONFIG_SYMBIAN) || defined(CONFIG_LINUX) //dynamic allocation
/** @brief	Does OS support dynamic allocation? */
#define NS_SUPPORT_DYNAMIC_ALLOC
#endif 

/**
* @brief	If OS does not support dynamic allocation, how many objects need?
*/
#ifndef	NS_SUPPORT_DYNAMIC_ALLOC
/** @brief	max vnode count in static allocation */
#define NS_TOTAL_VNODE_COUNT		TOTAL_VNODE_COUNT
/** @brief	max volume count in static allocation */
#define NS_MAX_VOLUMES				MAX_VOLUMES
/** @brief	max thread count in static allocation */
#define NS_MAX_CONCURRENT_THREADS	MAX_CONCURRENT_THREADS
#endif

/**
* @brief	native's log file name
*/
#define	NS_LOG_FILE_NAME			LOG_FILE_NAME						

#if defined(CONFIG_RTOS)
/**
* @brief	hidden protected area
*/
#define NS_HPA_ROOT_NAME			HPA_ROOT_NAME						
#define NS_HPA_MAX_VOLUME_SIZE		HPA_MAX_VOLUME_SIZE					
#define NS_HPA_MAX_VOLUME_COUNT		HPA_MAX_VOLUME_COUNT				
#endif /*defined(CONFIG_RTOS)*/

/**
* @brief	Common file attributes which save in Vnode
* @ref		_NS_FILE_ATTR
*/
typedef unsigned int			NS_FILE_ATTR;	

/**
* @brief	Common file attributes which save in Vnode
* @see		_FILE_ATTR
*/
enum _NS_FILE_ATTR
{

	NS_FILE_ATTR_READONLY		= FILE_ATTR_READONLY,
	NS_FILE_ATTR_HIDDEN			= FILE_ATTR_HIDDEN,
	NS_FILE_ATTR_SYSTEM			= FILE_ATTR_SYSTEM,
	NS_FILE_ATTR_DIRECTORY		= FILE_ATTR_DIRECTORY,
	NS_FILE_ATTR_ARCHIVE		= FILE_ATTR_ARCHIVE,
	NS_FILE_ATTR_NORMAL			= FILE_ATTR_NORMAL,

	NS_FILE_ATTR_DEVICE			= FILE_ATTR_DEVICE,
	NS_FILE_ATTR_ENCRYPTED		= FILE_ATTR_ENCRYPTED,
	NS_FILE_ATTR_LINKED			= FILE_ATTR_LINKED,
	NS_FILE_ATTR_NESTED			= FILE_ATTR_NESTED,

	NS_FILE_ATTR_FIFO			= FILE_ATTR_FIFO,
	NS_FILE_ATTR_SOCKET			= FILE_ATTR_SOCKET,
};

/**
 * @brief	For Linux, permissions of RWX of owner, group and other
 * @ref		_NS_ACL_MODE
 */
typedef unsigned short			NS_ACL_MODE;

/**
 * @brief	For Linux, permissions of RWX of owner, group and other
 * @see		_ACL_MODE
 */
enum _NS_ACL_MODE
{
	NS_OTHERS_EXECUTE			= OTHERS_EXECUTE,
	NS_OTHERS_WRITE				= OTHERS_WRITE,
	NS_OTHERS_READ				= OTHERS_READ,
	NS_OTHERS_RWX				= NS_OTHERS_EXECUTE | NS_OTHERS_WRITE | NS_OTHERS_READ,
	NS_GROUP_EXECUTE			= GROUP_EXECUTE,
	NS_GROUP_WRITE				= GROUP_WRITE,
	NS_GROUP_READ				= GROUP_READ,
	NS_GROUP_RWX				= NS_GROUP_EXECUTE | NS_GROUP_WRITE | NS_GROUP_READ,
	NS_OWNER_EXECUTE			= OWNER_EXECUTE,
	NS_OWNER_WRITE				= OWNER_WRITE,
	NS_OWNER_READ				= OWNER_READ,
	NS_OWNER_RWX				= NS_OWNER_EXECUTE | NS_OWNER_WRITE | NS_OWNER_READ,
	NS_STICKY					= ACL_STICKY,
	NS_SGID						= ACL_SGID,
	NS_SUID						= ACL_SUID,
	NS_ALL						= ACL_ALL,
};

/**
* @brief	NS_OPERATION_MODE indicates the permissions about the FileOps
* @ref		_NS_OPERATION_MODE
*/
typedef unsigned int			NS_OPERATION_MODE;

/**
* @brief	NS_OPERATION_MODE indicates the permissions about the FileOps
* @see		_OPERATION_MODE
*/
enum _NS_OPERATION_MODE
{
	NS_OP_EXEC					= OP_EXEC,
	NS_OP_WRITE					= OP_WRITE,
	NS_OP_READ					= OP_READ,
	NS_OP_APPEND				= OP_APPEND,
	NS_OP_RESIZE				= OP_RESIZE,
	NS_OP_DELETE				= OP_DELETE,
	NS_OP_METAUPDATE			= OP_METAUPDATE,
}; 

/**
 * @brief	The mount flags
 * @ref		_NS_MOUNT_FLAG
 */
typedef unsigned int			NS_MOUNT_FLAG, *NS_PMOUNT_FLAG;

/**
 * @brief	The mount flags
 * @see		_MOUNT_FLAG
 */
enum _NS_MOUNT_FLAG									
{	
	/* Nestle checks these values */
	NS_MOUNT_NORMAL					= MOUNT_NORMAL, 
	NS_MOUNT_READ_ONLY				= MOUNT_READ_ONLY,
	NS_MOUNT_NO_DEVICE				= MOUNT_NO_DEVICE,
	NS_MOUNT_NO_LOCK				= MOUNT_NO_LOCK,
	NS_MOUNT_ALLOW_DEVICE_IO		= MOUNT_ALLOW_DEVICE_IO,	// permit directly to access device(logical disk set)
	NS_MOUNT_IOCTL_ONLY				= MOUNT_IOCTL_ONLY,
	NS_MOUNT_EXTERNAL_DEVICE		= MOUNT_EXTERNAL_DEVICE,
	
	/* Native specific values */
	NS_MOUNT_FAT_MIRROR				= MOUNT_FAT_MIRROR,
	NS_MOUNT_LOG_INIT				= MOUNT_LOG_INIT,
	NS_MOUNT_TRANSACTION_OFF		= MOUNT_TRANSACTION_OFF,	
	NS_MOUNT_CLEAN_NAND				= MOUNT_CLEAN_NAND,
	NS_MOUNT_HPA					= MOUNT_HPA,
	NS_MOUNT_HPA_CREATE				= MOUNT_HPA_CREATE,
	NS_MOUNT_LLW					= MOUNT_LLW,
	NS_MOUNT_FULL_LLW				= MOUNT_FULL_LLW,
	NS_MOUNT_ERASE_SECTOR			= MOUNT_ERASE_SECTOR,
	NS_MOUNT_FILE_GUID				= MOUNT_FILE_GUID,
	NS_MOUNT_FILE_XATTR				= MOUNT_FILE_XATTR,
	NS_MOUNT_FILE_SPECIAL			= MOUNT_FILE_SPECIAL,
	NS_MOUNT_ALLOW_OS_NAMING_RULE	= MOUNT_ALLOW_OS_NAMING_RULE,
	NS_MOUNT_SYNC_META				= MOUNT_SYNC_META,
	NS_MOUNT_AUTO_ADAPTATION		= MOUNT_AUTO_ADAPTATION
};

/**
 * @brief	The unmount flags
 * @ref		_NS_UNMOUNT_FLAG
 */
typedef unsigned int			NS_UNMOUNT_FLAG;

/**
 * @brief	The unmount flags
 * @see		_UNMOUNT_FLAG
 */
enum _NS_UNMOUNT_FLAG
{
	NS_UNMOUNT_NORMAL			= UNMOUNT_NORMAL,
	NS_UNMOUNT_FORCE			= UNMOUNT_FORCE,	
	NS_UNMMOUNT_FLAG_MASK		= NS_UNMOUNT_NORMAL | NS_UNMOUNT_FORCE
} ;

/**
* @brief	The remount flags
* @ref		_NS_REMOUNT_FLAG
*/
typedef unsigned int			NS_REMOUNT_FLAG, *NS_PREMOUNT_FLAG;

/**
* @enum	_NS_REMOUNT_FLAG
* @brief	_REMOUNT_FLAG
*/
enum _NS_REMOUNT_FLAG								
{
	/* Nestle checks these values */
	NS_REMOUNT_NORMAL			= REMOUNT_NORMAL,				/**< normal mount */
	NS_REMOUNT_READ_ONLY		= REMOUNT_READ_ONLY,			/**< read only mount */

	/* Native specific values */
	NS_REMOUNT_LOG_INIT			= REMOUNT_LOG_INIT,				/**< initialize log */
	NS_REMOUNT_TRANSACTION_OFF	= REMOUNT_TRANSACTION_OFF,		/**< no transaction */
	NS_REMOUNT_CLEAN_NAND		= REMOUNT_CLEAN_NAND,			/**< clean nand */
	NS_REMOUNT_LLW				= REMOUNT_LLW,					/**< lazy log write */
	NS_REMOUNT_FULL_LLW			= REMOUNT_FULL_LLW,				/**< full lazy log write */
	NS_REMOUNT_FILE_GUID		= REMOUNT_FILE_GUID,			/**< guid support */
	NS_REMOUNT_FILE_XATTR		= REMOUNT_FILE_XATTR,			/**< extended attributes */
	NS_REMOUNT_FILE_SPECIAL		= REMOUNT_FILE_SEPCIAL,		/**< symlink support */
};


/**
* @brief Type to get a buffer entry.
* @ref		_NS_BCM_OP_FLAG
*/
typedef unsigned int			NS_BCM_OP_FLAG;

/**
 * @brief Type to get a buffer entry.
 * @see		_BCM_OP_FLAG
 */
enum _NS_BCM_OP_FLAG
{
	NS_BCM_OP_NONE				= BCM_OP_NONE,			/**< write case : write back, read case : uptodate*/
	NS_BCM_OP_SYNC				= BCM_OP_SYNC,			/**< write case : write through*/
	NS_BCM_OP_DIRECT			= BCM_OP_DIRECT,		/**< read / write case : write direct, read case : read direct */
	NS_BCM_OP_META				= BCM_OP_META,			/**< write case : meta data */
	NS_BCM_OP_HOT				= BCM_OP_HOT,			/**< write case : hot data */
	NS_BCM_OP_MASK				= BCM_OP_MASK,			/**< read / write flag mask */
	NS_BCM_OP_DUMMY				= BCM_OP_DUMMY	
};

/**
 * @brief NS_BE_STATUS indicates buffer entry status.
 * @ref		_NS_BE_STATUS
 */
typedef unsigned int			NS_BE_STATUS;

/**
 * @brief NS_BE_STATUS indicates buffer entry status.
 * @see		_BE_STATUS
 */
enum _NS_BE_STATUS {
	NS_BCM_BE_FREE				= BCM_BE_FREE,					/**< [AND]	Is Free */
	NS_BCM_BE_UPTODATE			= BCM_BE_UPTODATE,				/**< [AND]	Is uptodate data or valid */
	NS_BCM_BE_DIRTY				= BCM_BE_DIRTY,					/**< [OR]	Is dirty, (NOT clean) */
	NS_BCM_BE_META				= BCM_BE_META,					/**< [ONLY]	Is meta data (NOT user data) */
	NS_BCM_BE_HOT				= BCM_BE_HOT,					/**< [ONLY]	Is hot data (NOT cold data) */
	NS_BCM_BE_DUMMY				= BCM_BE_DUMMY
};

/**
 * @brief NS_LOOKUP_FLAG indicates Lookup flag for create or search
 * @ref		_NS_LOOKUP_FLAG
 */
typedef unsigned int			NS_LOOKUP_FLAG;

/**
 * @brief NS_LOOKUP_FLAG indicates Lookup flag for create or search
 * @see _LOOKUP_FLAG
 */
enum _NS_LOOKUP_FLAG
{
	NS_LOOKUP_CREATE			= LOOKUP_FOR_CREATE,			/**< for creating  */
	NS_LOOKUP_SEARCH			= LOOKUP_FOR_SEARCH,			/**< for search  */
	NS_LOOKUP_RENAME			= LOOKUP_FOR_RENAME				/**< for rename  */
};

/**
 * @brief	Decide to Ldev Read/write flag
 * @ref		_NS_DISK_OPERATE_TYPE
 */
typedef unsigned int			NS_DISK_OPERATE_TYPE;

/**
 * @brief	Decide to Ldev Read/write flag
 * @see		_DISK_OPERATE_TYPE
 */
enum _NS_DISK_OPERATE_TYPE
{
	NS_NONE						= LDEV_NONE,					/**< default flag */
	NS_READ						= LDEV_READ,					/**< Read sector Operation */
	NS_WRITE					= LDEV_WRITE,					/**< Write sector Operation */
	NS_META						= LDEV_META,					/**< Meta data operation */
	NS_HOT						= LDEV_HOT						/**< Hot data operation */
};

/**
 * @brief	CHKDSK (scanning) flag
 * @ref		_NS_CHKDSK_TYPE
 */
typedef unsigned int			NS_CHKDSK_TYPE;

/**
 * @brief	CHKDSK (scanning) flag
 * @see		_CHKDSK_TYPE
 */
enum _NS_CHKDSK_TYPE
{
	NS_CHKDSK_REPAIR				= CHKDSK_REPAIR,			/**< repair errors */
	NS_CHKDSK_SHOW					= CHKDSK_SHOW,				/**< print errors */
	NS_CHKDSK_REPAIR_INTERACTIVE	= CHKDSK_REPAIR_INTERACTIVE,/**< repair with confirm */
	NS_CHKDSK_CHECK_ONLY			= CHKDSK_CHECK_ONLY,		/**< check only */
	NS_CHKDSK_MASK					= CHKDSK_REPAIR | CHKDSK_SHOW | CHKDSK_REPAIR_INTERACTIVE |CHKDSK_CHECK_ONLY
};

/**
 * @brief	FAT Type
 * @ref		_NS_NFS_TYPE
 */
typedef unsigned int			NS_NFS_TYPE;

/**
 * @brief	FAT Type
 * @see		_NFS_TYPE
 */
enum _NS_NFS_TYPE
{
	NS_NFS_NONE						= NFS_NONE,
	NS_NFS_TYPE_FAT12				= NFS_TYPE_FAT12,
	NS_NFS_TYPE_FAT16				= NFS_TYPE_FAT16, /**< # of bit per cluster : 16 bit (65,524 clusters) */
	NS_NFS_TYPE_FAT32				= NFS_TYPE_FAT32, /**< # of bit per cluster : 32 bit (  2^28 clusters) */
	NS_NFS_TYPE_EXFAT				= NFS_TYPE_EXFAT
} ;

/**
 * @brief	Native Filesystem Type
 * @ref		_NS_NFS_NAME
 */
typedef unsigned int			NS_NFS_NAME;

/**
 * @brief	Native Filesystem Type
 * @see		_NFS_NAME
 */
enum _NS_NFS_NAME
{
	NS_NFS_NAME_BTFS				= NFS_NAME_BTFS,
	NS_NFS_NAME_ETF					= NFS_NAME_EXFAT
};

/**
 * @brief	How to covert name
 * @ref		_NS_CONVERT_TYPE
 */
typedef unsigned int			NS_CONVERT_TYPE;

/**
 * @brief	How to covert name
 * @see		_CONVERT_TYPE
 */
enum _NS_CONVERT_TYPE
{
	NS_FILE_NAME_SHORT_TO_LONG		= FILE_NAME_SHORT_TO_LONG,	/**< convert a short file name to a long file name */
	NS_FILE_NAME_LONG_TO_SHORT		= FILE_NAME_LONG_TO_SHORT	/**< convert a long file name to a short file name */
};

/**
 * @brief	Namespace of extended attributes
 * @ref		_NS_XATTR_NAMESPACE_ID
 */
typedef unsigned int		NS_XATTR_NAMESPACE_ID;

/**
 * @brief	Namespace of extended attributes
 * @see		_XATTR_NAMESPACE_ID
 */
enum _NS_XATTR_NAMESPACE_ID
{
	NS_XATTR_ID_USER				= XATTR_ID_USER,
	NS_XATTR_ID_POSIX_ACL_ACCESS	= XATTR_ID_POSIX_ACL_ACCESS,
	NS_XATTR_ID_POSIX_ACL_DEFAULT	= XATTR_ID_POSIX_ACL_DEFAULT,
	NS_XATTR_ID_TRUSTED				= XATTR_ID_TRUSTED,
	NS_XATTR_ID_SECURITY			= XATTR_ID_SECURITY
};

/**
 * @brief	Flag for setxattr()
 * @ref		_NS_XATTR_SET_FLAG
 */
typedef int		NS_XATTR_SET_FLAG;

/**
 * @brief	Flag for setxattr()
 * @see		_XATTR_SET_FLAG
 */
enum _NS_XATTR_SET_FLAG
{
	NS_XATTR_SET_FLAG_NONE			= XATTR_SET_FLAG_NONE,
	NS_XATTR_SET_FLAG_CREATE		= XATTR_SET_FLAG_CREATE,
	NS_XATTR_SET_FLAG_REPLACE		= XATTR_SET_FLAG_REPLACE
};

/**
 * @brief	Flag for disk control
 * @ref		_NS_DISK_FLAGS
 */
typedef unsigned int NS_DISK_FLAGS;

/**
 * @brief	Flag for disk control
 * @see		_DISK_FLAGS
 */
enum _NS_DISK_FLAGS
{
	NS_DISK_REMOVABLE_MEDIA			= DISK_REMOVABLE_MEDIA,
	NS_DISK_SUPPORT_ERASE_SECTORS	= DISK_SUPPORT_ERASE_SECTORS
};

/******************************************************************************/
/* I/O CONTROL                                                                */
/******************************************************************************/
/**
 * @brief	io control code list
 * @ref		_NS_IOCTL_CODE
 */
typedef unsigned int	NS_IOCTL_CODE;

/**
 * @brief	io control code list
 * @see		NS_IOCTL_CODE
 */
enum _NS_IOCTL_CODE
{
	NS_IOCTL_NATIVE_READ_SECTOR				= IOCTL_NATIVE_READ_SECTOR,				/**< read sector through native */
	NS_IOCTL_NATIVE_WRITE_SECTOR			= IOCTL_NATIVE_WRITE_SECTOR,			/**< write sector through native */
	NS_IOCTL_NATIVE_ERASE_SECTOR			= IOCTL_NATIVE_ERASE_SECTOR,			/**< erase sector through native */
	NS_IOCTL_NATIVE_FAT_CLEAN				= IOCTL_NATIVE_FAT_CLEAN,				/**< notify clean FAT information to device */
};

/**
* @brief	native filesystem control code list
* @ref		_NS_FSCTL_CODE
*/
typedef unsigned int	NS_FSCTL_CODE;

/**
* @brief	native filesystem control code list
* @see		NS_FSCTL_CODE
*/
enum _NS_FSCTL_CODE
{
	NS_FSCTL_NATIVE_GET_VOL_NAME			= FSCTL_NATIVE_GET_VOL_NAME,			/**< get volume name */
	NS_FSCTL_NATIVE_SET_VOL_NAME			= FSCTL_NATIVE_SET_VOL_NAME,			/**< set volume name */

	NS_FSCTL_NATIVE_ADD_CACHE				= FSCTL_NATIVE_ADD_CACHE,				/**< add cache to native */
	NS_FSCTL_NATIVE_REMOVE_CACHE			= FSCTL_NATIVE_REMOVE_CACHE,			/**< remove cache from native */
	NS_FSCTL_NATIVE_CHECK_CACHE				= FSCTL_NATIVE_CHECK_CACHE,				/**< check cache in native */

	NS_FSCTL_NATIVE_SET_DEC					= FSCTL_NATIVE_SET_DEC,					/**< set user directory entry cache */
	NS_FSCTL_NATIVE_RELEASE_DEC				= FSCTL_NATIVE_RELEASE_DEC,				/**< release user directory entry cache */
	NS_FSCTL_NATIVE_GET_DEC_INFO			= FSCTL_NATIVE_GET_DEC_INFO,			/**< set user directory entry cache information */

	NS_FSCTL_NATIVE_CHECK_BOOT_SECTOR		= FSCTL_NATIVE_CHECK_BOOT_SECTOR,		/**< check that first sector is boot-sector */

	NS_FSCTL_NATIVE_TEST_LOG_RECOVERY		= FSCTL_NATIVE_TEST_LOG_RECOVERY,		/**< for test log recovery */
	NS_FSCTL_NATIVE_TEST_INVALIDATE_FCCH	= FSCTL_NATIVE_TEST_INVALIDATE_FCCH,	/**< for test invalidate free cluster hint */
};

/**
* @enum		_FORMAT_METHOD
* @brief	format method using format_ex in RTOS
*/
typedef unsigned int		NS_FORMAT_METHOD;
enum _NS_FORMAT_METHOD 
{
	NS_FORMAT_SCAN_SKIP				= FORMAT_SCAN_SKIP,			/* skip bad sector scan */
	NS_FORMAT_SCAN_WRITE			= FORMAT_SCAN_WRITE,		/* fill sectors with the scanning buffer */
	NS_FORMAT_SCAN_READONLY			= FORMAT_SCAN_READONLY,		/* read sectors */

	NS_FORMAT_RECOVERY_OFF			= FORMAT_RECOVERY_OFF,		/* don't recover bad sectors */
	NS_FORMAT_RECOVERY_ON			= FORMAT_RECOVERY_ON,		/* automatically recover bad sectors(if found during the scan) */
	NS_FORMAT_RESERVED				= FORMAT_RESERVED			/* reserved (this makes this enum type 4 bytes in ADS) */
};


/**
* @brief	System Time Struct
* @par		NS_SYS_TIME
* @ref	PFN_NS_VNODE_OPS_SET_FILETIME, NS_GetFileTimes, NS_SetFileTimes
* @code
* typedef struct 					
* {
* 	unsigned short		wYear;
* 	unsigned short		wMonth;
* 	unsigned short		wDayOfWeek;
* 	unsigned short		wDay;
* 	unsigned short		wHour;
* 	unsigned short		wMinute;
* 	unsigned short		wSecond;
* 	unsigned short		wMilliseconds;
* } NS_SYS_TIME, *NS_PSYS_TIME;
* @endcode
* @ref _SYS_TIME
*/
typedef struct _SYS_TIME			NS_SYS_TIME, *NS_PSYS_TIME;

/**
* @brief	File Size
* @par		NS_FILE_SIZE, NS_PFILE_SIZE
* @code
* typedef unsigned long long		NS_FILE_SIZE, *NS_PFILE_SIZE;
* @endcode
*/
typedef unsigned long long			NS_FILE_SIZE, *NS_PFILE_SIZE;

/**
* @brief	File Offset
* @par		NS_FILE_OFFSET, NS_PFILE_OFFSET
* @code
* typedef unsigned long long		NS_FILE_OFFSET, *NS_PFILE_OFFSET;
* @endcode
*/
typedef unsigned long long			NS_FILE_OFFSET, *NS_PFILE_OFFSET;


/**
* @brief Data Structure of the directory entry
* @par NS_PDIR_ENTRY
* @code
* struct _DIRECTORY_ENTRY			
* {
*	unsigned long long	llVnodeID;
*	unsigned int		dwFileAttribute;	
*	SYS_TIME			ftCreated;
*	SYS_TIME			ftLastAccessed;
*	SYS_TIME			ftLastModified;
*	unsigned long long	llFileSize;
*	wchar_t				wszName[MAX_FILE_NAME_LENGTH+1];
*	wchar_t				wszAltName[MAX_ALT_FILE_NAME_LENGTH+1];
*	unsigned int		dwVolId;
*}
* @endcode
* @ref _DIRECTORY_ENTRY		
*/
typedef struct	_DIRECTORY_ENTRY			*NS_PDIR_ENTRY;

/**
* @struct	_FORMAT_PARAMETER
* @brief	format parameter
* @code
* struct _FORMAT_PARAMETER
* {
*	unsigned int		dwClusterSize;		
*	unsigned int		dwParamSize;		
*	void*				pFormatParam;		
* } ;
* @endcode
* @ref	_FORMAT_PARAMETER	
*/
typedef struct	_FORMAT_PARAMETER				*NS_PFORMAT_PARAMETER;

/**
* @struct	_FAT_CLEAN_PARAMETER
* @brief	fat cleansing parameter
* @code
* struct _FAT_CLEAN_PARAMETER
* {
*	unsigned int		dwStartCluster;
*	unsigned int		dwClusterCount;
* }
* @endcode
* @ref	_FAT_CLEAN_PARAMETER
*/
typedef struct	_FAT_CLEAN_PARAMETER			*NS_PFAT_CLEAN_PARAMETER;

/**
* @struct	_HPA_INFORMATION
* @brief	hidden protected area information
*
* struct _HPA_INFORMATION
* {
* 	t_uint32	dwClusterCount;	
* 	t_uint32	dwFreeClusterCount;
* 	t_uint32	dwAvailableBlockCount;
* }
*/
typedef	struct	_HPA_INFORMATION			*NS_PHPA_INFORMATION;

/**
* @brief Data Structure of the volume information Defined in TFS5.h
* @code
* struct _VOLUME_INFORMATION
* {
*	unsigned int		dwFsType;
*	unsigned int		dwFsID;
*	unsigned int		dwBlockSize;
*	unsigned int		dwNumBlocks;
*	unsigned int		dwNumFreeBlocks;
*	unsigned int		dwNumAvailClusters;
*	unsigned int		dwNumFiles;
*	unsigned int		dwNumFreeFiles;
*	unsigned int		dwMaxFileNameLen;
*   wchar_t				wszVolumeName[MAX_MOUNT_NAME_LENGTH+1];
*	HPA_INFORMATION		stHpa;
* }
* @endcode
* @ref	_VOLUME_INFORMATION
*/
typedef struct	_VOLUME_INFORMATION			*NS_PVOLUME_INFORMATION;

/**
* @brief Data Structure of volume name Defined in ns_types.h
* @par NS_PFS_VOLUME_NAME
* @code
* struct _FS_VOLUME_NAME			
* {
*	unsigned int	dwLen;
*	wchar_t			pwszVolumeName[MAX_MOUNT_NAME_LENGTH+1];
* } ;
* @endcode
* @ref	_FS_VOLUME_NAME
*/
typedef struct	_FS_VOLUME_NAME				*NS_PFS_VOLUME_NAME;

/**
* @brief NESTLE FILE_CONTROL_BLOCK
* @ref _FILE_CONTROL_BLOCK
*/
typedef struct	_FILE_CONTROL_BLOCK			*NS_PFCB;

/**
* @brief NESTLE VNODE
* @ref _VNODE
*/
typedef struct	_VNODE						*NS_PVNODE;

/**
* @brief NESTLE VOLUME_CONTROL_BLOCK
* @ref _VOLUME_CONTROL_BLOCK
*/
typedef struct	_VOLUME_CONTROL_BLOCK		*NS_PVCB;

/**
* @brief NESTLE BUFFER VECTOR ENTRY
* @ref _VECTOR_ENTRY
*/
typedef struct _VECTOR_ENTRY				*NS_PVE;

/**
* @brief NESTLE BUFFER VECTOR INFO
* @ref _VECTOR_INFO
*/
typedef struct	_VECTOR_INFO				*NS_PVI;

/**
* @brief NESTLE LOGICAL DISK
*/
typedef struct	_LOGICAL_DISK				*NS_PLOGICAL_DISK;
 
/**
* @brief DISK_SECTOR_RANGE struct
* @ref _DISK_SECTOR_RANGE
*/
typedef struct _DISK_SECTOR_RANGE			NS_DISK_SECTOR_RANGE, *NS_PDISK_SECTOR_RANGE;

/**
* @brief I/O request to logical sector by ioctl
* @ref _DISK_IO_REQUEST
*/
typedef struct	_DISK_IO_REQUEST			*NS_PDISK_IO_REQUEST;

/**
* @brief NAND_DEVICE_INFO struct
* @ref _NAND_DEVICE_INFO
*/
typedef struct _NAND_DEVICE_INFO			NS_NAND_DEVICE_INFO, *NS_PNAND_DEVICE_INFO;

/**
* @brief NAND_BLOCK_INFO struct
* @ref _NAND_BLOCK_INFO
*/
typedef struct _NAND_BLOCK_INFO				NS_NAND_BLOCK_INFO, *NS_PNAND_BLOCK_INFO;

/**
* @brief FS_DEC struct
* @ref _FS_DE_CACHE
*/
typedef struct	_FS_DE_CACHE				*NS_PFS_DE_CACHE;

/**
* @brief NATIVE_CACHE struct
* @ref _FS_NATIVE_CACHE
*/
typedef struct	_FS_NATIVE_CACHE			*NS_PFS_NATIVE_CACHE;

/**
* @brief SEMAPHORE struct
* @ref _SEMAPHORE
*/
typedef struct	_SEMAPHORE					NS_SEMAPHORE, *NS_PSEMAPHORE;

/**
* @brief RW_SEMAPHORE struct
* @ref _RW_SEMAPHORE
*/
typedef struct	_RW_SEMAPHORE				NS_RW_SEMAPHORE, *NS_PRW_SEMAPHORE;

/**
* @brief NESTLE BLOCK_DEVICE
* @ref _BLOCK_DEVICE
*/
typedef struct	_BLOCK_DEVICE				*NS_PBLOCK_DEVICE;

/**
* @brief NESTLE BLOCK_DEVICE
* @ref _FORMAT_DISK_INFO
*/
typedef struct	_FORMAT_DISK_INFO			*NS_PFORMAT_DISK_INFO;

/** @} */ //ns_datatype

/******************************************************************************/
/* Filesystem Interface						                                  */
/******************************************************************************/
/** 
 * @defgroup filesystem_interface	Filesystem Interface
 * Filesystem Interfaces implemented by Native Filesystem 
 * Filesystem interface
 * @{
 */	

/******************************************************************************/
/* NATIVE FILESYSTEM OPERATION                                                */
/******************************************************************************/
/** 
 * @defgroup NativefsOps	NativeFsOps
 * @brief	_NATIVEFS_OPERATIONS,  PFN_NS_NATIVEFS_OPS_MOUNT_DISK
 * @{
 */
/**
* @brief This function pointer is a declaration of the "pfnMountDisk" defined by native filesystem
* @param[in]	pVcb			Uninitialized vcb			
* @param[in]	pMountFlag		Mount flags pointer
* @return		FERROR
* @exception	FERROR_NO_ERROR		success
* @par	Implementation : Mandatory
* @li	Nativefs must handle wrong information in the partition table from a incorrect partitioning tool
* @par	To do : 
* @li	Alloc and init native vcb
* @li	Create root node (Make nestle vnode and native node and link them)
* @li	Init nestle vcb 
* @li	Set blocksize, blocksizebits, Root, pVolumeOps, pNativeVcb to nestle vcb
* @li	Return adopted mount flag. 
* @li	After calling NS_SetBlockSize(), native can do I/O throughout nestle's buffer cache
*/
typedef FERROR (*PFN_NS_NATIVEFS_OPS_MOUNT_DISK)(
	IN	NS_PVCB			pVcb,
	IO	NS_PMOUNT_FLAG	pMountFlag,
	OUT	NS_PVNODE*		ppVnodeRoot);

/**
* @brief This function pointer is a declaration of the "pfnFormat" defined by native filesystem
* @param[in]	pLogDisk			logical disk pointer		
* @param[in]	dwFileSystemType	fat type (FAT, BTFS)
* @param[in]	dwSectorsPerCluster	# of sectoer per cluster
* @return		FERROR
* @par	Implementation : Mandatory
* @par	To do :
* @li	format the device
*/ 
typedef FERROR (*PFN_NS_NATIVEFS_OPS_FORMAT)(
	IN	NS_PLOGICAL_DISK	pLogDisk,
	IN	NS_PFORMAT_PARAMETER	pFormatParam,
	IN	NS_PFORMAT_DISK_INFO		pDiskInfo);

/**
* @brief This function pointer is a declaration of the "pfnChkdsk" defined by native filesystem
* @param[in]	pLogDisk		logical disk
* @param[in]	pInBuf			chdsk flag which must has 4 bytes integer type
* @param[in]	dwInBufSize		the size of pInBuf (must be 4)
* @param[out]	pOutBuf			optional output passed by caller
* @param[in]	dwOutBufSize	the size of pOutBuf
* @return		FERROR
* @par	Implementation : Mandatory
* @par	To do : 
* @li	check validity of volume
* @li	Nestle does not protect reentrant for NativeFS, NativeFS has to get own lock.
*/
typedef FERROR (*PFN_NS_NATIVEFS_OPS_CHKDSK)(
	IN	NS_PVCB			pVcb,
	IN	void*			pInBuf,
	IN	unsigned int	dwInBufSize,
	OUT	void*			pOutBuf,
	IN	unsigned int	dwOutBufSize);

/**
* @brief This function pointer is a declaration of the "pfnInit" defined by native filesystem
* @return		FERROR
* @par	Implementation : Optional
* @par	To do : 
* @li	Initialize the native filesystem 
*/
typedef FERROR (*PFN_NS_NATIVEFS_OPS_INIT)(void);

/**
* @brief This function pointer is a declaration of the "pfnUninit" defined by native filesystem
* @return		FERROR
* @par	Implementation : Optional
* @par	To do : 
* @li	Uninitialize the native filesystem
*/
typedef FERROR (*PFN_NS_NATIVEFS_OPS_UNINIT)(void);

/**
* @brief This function pointer is a declaration of the "pfnFsctl" defined by native filesystem
* @param[in]	dwControlCode	filesystem control code
* @param[in]	pInBuf			optional input buffer
* @param[in]	dwInBufSize		the size of pInBuf
* @param[out]	pOutBuf			optional output passed by caller
* @param[in]	dwOutBufSize	the size of pOutBuf
* @return		FERROR
* @exception	FERROR_NOT_SUPPORTED	not supported fsctl control code
* @exception	FERROR_INVALID			invalid boot sector
* @par	Implementation : Mandatory
* @par	To do : 
* @li	Filesystem control
* @li	Nestle does not protect reentrant for NativeFS, NativeFS has to get own lock.
* @li	NS_IOCTL_NATIVE_CHECK_BOOT_SECTOR :
* @li		PFN_NS_NATIVEFS_OPS_FSCTL(NULL, NS_IOCTL_NATIVE_CHECK_BOOT_SECTOR, pInBuf, sizeof(pInBuf), NULL, NULL)
* @li		If pInBuf is suitable for boot sector of native filesystem, return FERROR_NO_ERROR 
* @li		else FERROR_INVALID
* @li
* @li	Native cache support : NS_IOCTL_NATIVE_ADD_CACHE, NS_IOCTL_NATIVE_REMOVE_CACHE, NS_IOCTL_NATIVE_CHECK_CACHE
* @li	typedef struct _FS_NATIVE_CACHE
* @li	{
* @li		char*				pBuff;			
* @li		int					dwSize;			
* @li		int					dwSectorSize;	
* @li	} FS_NATIVE_CACHE, *PFS_NATIVE_CACHE;
* @li
* @li	NS_IOCTL_NATIVE_ADD_CACHE		
* @li		*PFN_NS_VCB_OPS_FSIOCTL(NULL, NS_IOCTL_NATIVE_ADD_CACHE, NS_PFS_NATIVE_CACHE, sizeof(NS_PFS_NATIVE_CACHE),NULL, NULL)
* @li		Fill all value of FS_NATIVE_CACHE struct and set add cache.
* @li	NS_IOCTL_NATIVE_REMOVE_CACHE
* @li		*PFN_NS_VCB_OPS_FSIOCTL(NULL, NS_IOCTL_NATIVE_REMOVE_CACHE, NS_PFS_NATIVE_CACHE, sizeof(NS_PFS_NATIVE_CACHE),NULL, NULL)
* @li		remove cache size that has dwSize.
* @li	NS_IOCTL_NATIVE_CHECK_CACHE		
* @li		*PFN_NS_VCB_OPS_FSIOCTL(NULL, NS_IOCTL_NATIVE_CHECK_CACHE, NS_PFS_NATIVE_CACHE, sizeof(NS_PFS_NATIVE_CACHE),NULL, NULL)
* @li		check cache size that has dwSectorSize and fill dwSize and pBuff
*/
typedef FERROR (*PFN_NS_NATIVEFS_OPS_FSCTL)(
	IN	unsigned int		dwControlCode,
	IN	void*				pInBuf,
	IN	unsigned int		dwInBufSize,
	OUT	void*				pOutBuf,
	OUT	unsigned int		dwOutBufSize);

/**
* @brief define the filesystem and the fs-operations.
*/
typedef struct _NATIVEFS_OPERATIONS		NS_NATIVEFS_OPERATIONS, *NS_PNATIVEFS_OPERATIONS;

/** @} */ // end of NativefsOps


/******************************************************************************/
/* FILE OPERATIONS                                                            */
/******************************************************************************/
/** 
 * @defgroup FileOps	FileOps
 * @brief	File Operations
 * @{
 */
/**
* @brief Close a open file - pfnClose
* @param[in]	pFcb				fcb to close
* @return		FERROR				Error code
* @exception	FERROR_NO_ERROR		Success
* @exception	FERROR_IO_ERROR		Failed I/O
* @par	Implementation : Optional
* @par	To do : 
* @li	Perform the additional activity for closing the native open file object or flushing the buffers.
*/
typedef FERROR (*PFN_NS_FILE_OPS_CLOSE) (
	IN NS_PFCB				pFcb);

/**
* @brief Read a directory - pfnReadDirectory
* @param[in]	pFcb				fcb for parent directory
* @param[in]	pBuffer				file name to find (usually wild char)
* @param[out]	pEntry				dir entry to contain one child
* @param[out]	pdwNumberOfRead		how many the data read. nestle adds this value to directory offset
* @return		FERROR
* @exception	FERROR_NO_ERROR		success
* @exception	FERROR_NO_MORE_ENTRIES	No
* @par	Implementation : Mandatory
* @li	One call reads one entry
* @par	To do :  
* @li	Read a valid entry from the offset in a fcb, and save the read size in pdwNumberOfRead
* @li	Transform native entry to NS_PDIR_ENTRY
* @li	Native can access directly the fields of NS_PDIR_ENTRY
*/
typedef FERROR (*PFN_NS_FILE_OPS_READ_DIRECTORY) (
	IN	NS_PFCB				pFcb,
	IN	const wchar_t*		pwszFileNameToSearch,
	OUT	NS_PDIR_ENTRY		pEntry,
	OUT	unsigned int*		pdwNumberOfRead);

/**
* @brief This function pointer is a declaration of the "pfnWriteFile" defined by native filesystem
* @param[in]	pFcb					fcb to write
* @param[in]	pBuffer					write buffer					
* @param[in]	dwBytesToWrite			the size to write in byte
* @param[out]	pdwBytesWritten			the written size in byte
* @return		FERROR					Error code
* @exception	FERROR_NO_ERROR			Success
* @exception	FERROR_NO_FREE_SPACE	No free space
* @exception	FERROR_IO_ERROR			Failed I/O
* @par	Implementation : Mandatory
* @li	Native gets start offset from fcb by NS_GetOffsetFromFcb() and write data to buffer cache
* @par To do : 
* @li	Generic write procedures are as follow
* @li	Expand cluster
* @li	Mapping file block addr to volume block addr
* @li	Use local buffer to write misaligned head data
* @li	Pass aligned body to buffer cache (buffer cache also handles direct write or sync write)
* @li	Use local buffer to write misaligned tail data
* @li	If start offset is greater than file size, fill the hole with zero or garbage (NS_FillZero())
* @li	Update vnode's file size (NS_SetFileSize)
* @li	Update vnode's timestamp (NS_SetFileTimes)
* @li	If data remain dirty, mark vnode dirty (NS_MarkVnodeDirty())
*/
typedef FERROR (*PFN_NS_FILE_OPS_WRITE_FILE) (
	IN	NS_PFCB				pFcb,
	IN	unsigned char*		pBuffer,
	IN	unsigned int		dwBytesToWrite,
	OUT unsigned int*		pdwBytesWritten);

/**
* @brief This function pointer is a declaration of the "pfnWriteFileBegin" defined by native filesystem
*		 This funciton can be used to implement the transaction of write file.
*		 This function will notify the NativeFS the start of the write function.
*		 By using this info, NativeFS can start the transaction of write file.
* @param[in]	pFcb				fcb to write		
* @param[in]	llStartOffset		start offset to write
* @param[in]	dwBytesToWrite		the size to write in byte
* @return		FERROR				Error code
* @exception	FERROR_NO_ERROR		success
* @par	Implementation : Optional
* @li	Native can optimize write transaction if nestle uses this call
* @par	To do : 
* @li	write file transaction start
*/
typedef FERROR (*PFN_NS_FILE_OPS_WRITE_FILE_BEGIN) (
	IN	NS_PFCB				pFcb,
	IN	NS_FILE_OFFSET		llStartOffset,
	IN	unsigned int		dwBytesToWrite);

/**
* @brief This function pointer is a declaration of the "pfnWriteFileEnd" defined by native filesystem
*		 This funciton can be used to implement the transaction of write file.
*		 This function will notify the NativeFS the end of the write function.
*		 By using this info, NativeFS can commit the transaction of write file.
* @param[in]	pFcb				fcb to write
* @param[in]	dwBytesWritten		the written size in byte
* @param[in]	err					the result of write file
* @return		FERROR				Error code
* @exception	FERROR_NO_ERROR		success
* @par	Implementation : Optional
* @li	Native can optimize write transaction if nestle uses this call
* @par	To do : 
* @li	commit the transaction
* @see	PFN_NS_FILE_OPS_WRITE_FILE, PFN_NS_FILE_OPS_WRITE_FILE_BEGIN, NS_GetVnodeFromFcb
*/
typedef FERROR (*PFN_NS_FILE_OPS_WRITE_FILE_END) (
	IN	NS_PFCB				pFcb,
	IN	unsigned int		dwBytesWritten,
	IN	FERROR				err);

/**
* @brief This function pointer is a declaration of the "pfnReadFile" defined by native filesystem
* @param[in]	pFcb				fct to read									
* @param[in]	pBuffer				the buffer for reading data
* @param[in]	dwBytesToRead		the size to read in byte
* @param[out]	pdwBytesRead		the read size in byte
* @return		FERROR				Error code
* @exception	FERROR_NO_ERROR		success
* @exception	FERROR_IO_ERROR		Failed I/O
* @par	Implementation : Mandatory
* @li	Native gets start offset from fcb by NS_GetOffsetFromFcb() and reads data from buffer cache
* @par	To do :
* @li	Similar to PFN_NS_FILE_OPS_WRITE_FILE
*/
typedef FERROR (*PFN_NS_FILE_OPS_READ_FILE) (
	IN	NS_PFCB				pFcb,
	IN	unsigned char*		pBuffer,
	IN	unsigned int		dwBytesToRead,
	OUT unsigned int*		pdwBytesRead);

/**
* @brief This function pointer is a declaration of the "pfnSeekFile" defined by native filesystem
* @param[in,out]	pFcb				fcb to seek		
* @param[in]	llOffset			new offset
* @return		FERROR				Error code
* @exception	FERROR_NO_ERROR		success
* @par	Implementation : Optional
* @par	To do :
* @li	Before nestle updates file offset, native can does something
*/
typedef FERROR (*PFN_NS_FILE_OPS_SEEK_FILE) (
	IO NS_PFCB				pFcb,
	IN NS_FILE_OFFSET		llOffset);

/**
* @brief	File Operations
*/
typedef struct _FILE_OPERATIONS		NS_FILE_OPS, *NS_PFILE_OPS;

/** @} */ // end of FileOps

/******************************************************************************/
/* VNODE OPERATIONS                                                            */
/******************************************************************************/
/** 
 * @defgroup vnode_ops	VnodeOps
 * @brief	Vnode Operations
 * @{
 */	

/**
* @brief This function pointer is a declaration of the "pfnOpen" defined by native filesystem
* @param[in]	pFcb					fcb to open
* @return		FERROR
* @exception	FERROR_NO_ERROR			success
* @exception	FERROR_INSUFFICIENT_MEMORY	fail to allocate memory
* @par	Implementation : Optional
* @par	To do :
* @li	Do something with nestle fcb connected to vnode
*/
typedef FERROR (*PFN_NS_VNODE_OPS_OPEN) (
	IN	NS_PFCB				pFcb);

/**
* @brief This function pointer is a declaration of the "pfnLookup" defined by native filesystem
* @param[in]	pnParent		Parent Node
* @param[in]	pwsName			Relative path for search node
* @param[in]	dwNameLen		Name length for sear node
* @param[in]	dwLookupFlag	Lookup Flag - Search-only or Search For Creation
* @param[out]	ppVnode			Store node If node is exsist
* @return		FERROR
* @exception	FERROR_NO_ERROR					success
* @exception	FERROR_INSUFFICIENT_MEMORY		fail to allocate memory
* @par	Implementation : Mandatory
* @li	Lookup child
* @li	For creating, dwLookupFlag has NS_LOOKUP_CREATE
* @li	For search, dwLookupFlag has NS_LOOKUP_SEARCH
* @par	To do :
* @li	Find child by name in parent and get index
* @li	Get existing or new vnode by NS_FindOrCreateVnode()
* @li	If vnode is created, set Attr, uid, gid, acl, size, time, link count, pNative, ... into new vnode
*/
typedef FERROR (*PFN_NS_VNODE_OPS_LOOKUP) (
	IN	NS_PVNODE			pnParent,
	IN	const wchar_t*		pwsName,
	IN	unsigned int		dwNameLen,
	IN	NS_LOOKUP_FLAG		dwLookupFlag,
	OUT NS_PVNODE*			ppVnode);

/**
* @brief This function pointer is a declaration of the "pfnCreateDirectory" defined by native filesystem
* @param[in]	pParent			NS_PVNODE				
* @param[in]	pwsName			const wchar_t*
* @param[in]	dwNameLen		unsigned int			
* @param[in]	dwAttr			NS_FILE_ATTR			
* @param[in]	dwUid			unsigned int			
* @param[in]	dwGid			unsigned int			
* @param[in]	wPerm			NS_ACL_MODE				
* @param[out]	ppVnode			NS_PVNODE*				
* @return		FERROR
* @exception	FERROR_NO_ERROR					success
* @exception	FERROR_INSUFFICIENT_MEMORY		fail to allocate memory
* @exception	FERROR_ACCESS_DENIED			access denied
* @par	Implementation : Mandatory
* @li	Create directory with guid and mode
* @par	To do :
* @li	After directory is built successfully, create vnode (NS_FindOrCreateVnodeFromVcb). 
* @li   Set Attr, uid, gid, acl, size, time, link count, pNative, ... into new vnode
* @li	Update parent's time
*/
typedef FERROR (*PFN_NS_VNODE_OPS_CREATE_DIRECTORY) (
	IN	NS_PVNODE				pnParent,
	IN	const wchar_t*			pwsName,
	IN	unsigned int			dwNameLen,
	IN	NS_FILE_ATTR			dwAttr,
	IN	unsigned int			dwUid,
	IN	unsigned int			dwGid,
	IN	NS_ACL_MODE				wPerm,
	OUT NS_PVNODE*				ppVnode);

/**
* @brief This function pointer is a declaration of the "pfnCreateFile" defined by native filesystem
* @param[in]	pParent			NS_PVNODE				
* @param[in]	pwsName			const wchar_t*
* @param[in]	dwNameLen		unsigned int			
* @param[in]	dwAttr			NS_FILE_ATTR			
* @param[in]	dwUid			unsigned int			
* @param[in]	dwGid			unsigned int			
* @param[in]	wPerm			NS_ACL_MODE				
* @param[out]	ppVnode			NS_PVNODE*				
* @return		FERROR
* @exception	FERROR_NO_ERROR			success
* @par	Implementation : Mandatory 
* @li	Create file with guid and mode
* @par	To do :
* @li	After file is built successfully, create vnode (NS_FindOrCreateVnodeFromVcb). 
* @li   Set Attr, uid, gid, acl, size, time, link count, pNative, ... into new vnode
* @li	Update parent's time
*/
typedef FERROR (*PFN_NS_VNODE_OPS_CREATE_FILE) (
	IN	NS_PVNODE				pnParent,
	IN	const wchar_t*			pwsName,
	IN	unsigned int			dwNameLen,
	IN	NS_FILE_ATTR			dwAttr,
	IN	unsigned int			dwUid,
	IN	unsigned int			dwGid,
	IN	NS_ACL_MODE				wPerm,
	OUT NS_PVNODE*				ppVnode);

/**
* @brief This function pointer is a declaration of the "pfnCreateSymLink" defined by native filesystem
* @param[in]	pnSourceParent		NS_PVNODE
* @param[in]	pwsName				const wchar_t*
* @param[in]	dwNameLen			unsigned int
* @param[in]	dwAttr				NS_FILE_ATTR
* @param[in]	dwUid				unsigned int			
* @param[in]	dwGid				unsigned int			
* @param[in]	wPerm				NS_ACL_MODE	
* @param[in]	pwsTargetPath		const wchar_t*
* @param[out]	ppVnode				NS_PVNODE*	
* @return		FERROR
* @exception	FERROR_NO_ERROR			success
* @par	Implementation : Mandatory
* @li	Create symlink with guid and mode
* @par	To do :
* @li	After symlink is built successfully, create vnode (NS_FindOrCreateVnodeFromVcb). 
* @li   Set Attr, uid, gid, acl, size, time, link count, pNative, ... into new vnode
* @li	Update parent's time
*/
typedef FERROR (*PFN_NS_VNODE_OPS_CREATE_SYMLINK) (
	IN	NS_PVNODE			pnSourceParent,
	IN	const wchar_t*		pwsName,
	IN	unsigned int		dwNameLen,
	IN	NS_FILE_ATTR		dwAttr,
	IN	unsigned int		dwUid,
	IN	unsigned int		dwGid,
	IN	NS_ACL_MODE			wPerm,
	IN	const wchar_t*		pwsTargetPath,
	OUT	NS_PVNODE*			ppVnode);	

/**
* @brief This function pointer is a declaration of the "pfnReadSymlink" defined by native filesystem
* @param[in]	pVnode				NS_PVNODE
* @param[out]	pwszLinkBuff		wchar_t*
* @param[in]	dwLinkBuffSize		unsigned int  (byte size)
* @param[out]	pLinkLen			unsigned int* (string length)
* @return		FERROR
* @exception	FERROR_NO_ERROR			success
* @par	Implementation : Mandatory
* @li	Read symlink for lookup 
* @par	To do :
* @li	Read path into pwsPath
*/
typedef FERROR (*PFN_NS_VNODE_OPS_READ_SYMLINK) (
	IN	NS_PVNODE			pVnode,
	OUT	wchar_t*			pwszLinkBuff,
	IN	unsigned int		dwLinkBuffSize,
	OUT unsigned int*		pLinkLen);

/**
* @brief This function pointer is a declaration of the "pfnUnlink" defined by native filesystem
* @param[in]	pParent					NS_PVNODE				
* @param[in]	pTarget					NS_PVNODE				
* @param[in]	bIsOpened				BOOL					
* @return		FERROR
* @exception	FERROR_NO_ERROR			success
* @par	Implementation : Mandatory
* @li	Unlink file. If file is on open, bIsOpened has TRUE
* @par	To do :
* @li	Delete file entry
* @li	Decrease file's link count
* @li	Do something according to bIsOpened (ex, change vnode index)
* @li	Update parent's time
* @li	Make dirty mark to vnode and volume object
*/
typedef FERROR (*PFN_NS_VNODE_OPS_UNLINK) (
	IN	NS_PVNODE			pnParent,
	IN	NS_PVNODE			pnTarget,
	IN	BOOL				bIsOpened);

/**
* @brief This function pointer is a declaration of the "pfnMove" defined by native filesystem
* @param[in]	pSourceParent		NS_PVNODE				
* @param[in]	pSourceFile			NS_PVNODE				
* @param[in]	pTargetParent		NS_PVNODE				
* @param[in]	pwszNewName			const wchar_t*			
* @param[in]	dwNameLen			unsigned int			
* @param[in]	bIsSourceOpened		BOOL
* @return		FERROR
* @exception	FERROR_NO_ERROR			success
* @par	Implementation : Optional (Move or Move2)
* @li	Called by rename()
* @par	To do :
* @li	File move
* @li	Nestle make sure that no file which have the same name as pSourceFile exists
* @li	Update parent's time
* @li	Open rename is permitted
*/
typedef FERROR (*PFN_NS_VNODE_OPS_MOVE) (
	IN	NS_PVNODE			pnSourceParent,
	IN	NS_PVNODE			pnSourceFile,
	IN	NS_PVNODE			pnTargetParent,
	IN	const wchar_t*		pwszNewName,
	IN	unsigned int		dwNameLen,
	IN	BOOL				bIsSourceOpened);

/**
* @brief This function pointer is a declaration of the "pfnMove2" defined by native filesystem\n
*        There is a difference pfnMove and pfnMove2, \n
*        "pfnMove" assumes that the target is already deleted, but "pfnMove2" assumes that the target is not deleted so natives should delete target
* @param[in]	pSourceParent		NS_PVNODE				
* @param[in]	pSourceFile			NS_PVNODE				
* @param[in]	pTargetParent		NS_PVNODE	
* @param[in]	pTargetFile			NS_PVNODE
* @param[in]	pwszNewName			const wchar_t*			
* @param[in]	dwNameLen			unsigned int			
* @param[in]	bIsSourceOpened		BOOL
* @param[in]	bIsTargetOpened		BOOL
* @return		FERROR
* @exception	FERROR_NO_ERROR			success
* @par	Implementation : Optional (Move or Move2)
* @li	Called by rename()
* @par	To do :
* @li	If TargetFile don't exsist, Nestle make sure bIsTargetOpened has FALSE value
* @li	If TargetFile exists, native overwrites it by SourceFile
* @li	Update parent's time
* @li	Open rename is permitted, change both source and target's id number
* @li	Make dirty mark to vnode and volume object
*/
typedef FERROR (*PFN_NS_VNODE_OPS_MOVE2) (
	IN	NS_PVNODE			pnSourceParent,
	IN	NS_PVNODE			pnSourceFile,
	IN	NS_PVNODE			pnTargetParent,
	IN	NS_PVNODE			pnTargetFile,
	IN	const wchar_t*		pwszNewName,
	IN	unsigned int		dwNameLen,
	IN	BOOL				bIsSourceOpened,
	IN	BOOL				bIsTargetOpened);

/**
* @brief This function pointer is a declaration of the "pfnDeleteNode" defined by native filesystem
* @param[in]	pVnode				NS_PVNODE
* @return		FERROR
* @exception	FERROR_NO_ERROR			success
* @par	Implementation : Mandatory
* @li	Called when all the links and references are released
* @par	To do :
* @li	Delete physical resource on vnode
* @li	Recommend that native should implement pfnDestoryNode if native has in-memory resource on vnode
* @li	Consider open status unlinked node.
*/
typedef FERROR (*PFN_NS_VNODE_OPS_DELETE_NODE) (
	IN	NS_PVNODE		pVnode);

/**
* @brief This function pointer is a declaration of the "pfnDestroyNode" defined by native filesystem
* @param[in]	pVnode				NS_PVNODE
* @return		FERROR
* @exception	FERROR_NO_ERROR			success
* @par	Implementation : Optional
* @li	Called at no reference on vnode
* @li	If native has local resource on vnode, implements this
* @par	To do :
* @li	Release native vnode
* @li	Note that just release in-memory resource not physical file
*/
typedef FERROR (*PFN_NS_VNODE_OPS_DESTROY_NODE) (
	IN	NS_PVNODE		pVnode);

/**
* @brief This function pointer is a declaration of the "pfnSetAttributes" defined by native filesystem
* @param[in]	pVnode			NS_PVNODE
* @param[in]	dwAttr			unsigned int		
* @return		FERROR
* @exception	FERROR_NO_ERROR		success
* @par	Implementation : Optional
* @li	Set file attribute
* @par	To do :
* @li	After sucessful this call, nestle sets attributes into vnode
*/
typedef FERROR (*PFN_NS_VNODE_OPS_SET_ATTRIBUTES) (
	IN	NS_PVNODE		pVnode,
	IN	NS_FILE_ATTR	dwAttr);

/**
* @brief This function pointer is a declaration of the "pfnSetFileSize" defined by native filesystem
* @param[in]	pVnode			NS_PVNODE
* @param[in]	llFileSize		NS_FILE_SIZE
* @param[in]	bFillZero		BOOL
* @return		FERROR
* @exception	FERROR_NO_ERROR	success
* @par	Implementation : Mandatory
* @li	Called by trucate(), ftrucate(), SetEndOfFile()
* @par	To do :
* @li	Shrink or expand file
* @li	If file is expanded, native can ask nestle for init clusters
* @li	Update time and size of vnode and make dirty if needed
* @li	If bFillZero is true, zero fill operation is executed by native file system.
* @li	If free space was not enough to expand size, It makes FERROR_NO_FREE_SPACE error

*/
typedef FERROR (*PFN_NS_VNODE_OPS_SET_FILESIZE) (
	IN	NS_PVNODE			pVnode,
	IN	NS_FILE_SIZE		llFileSize,
	IN	BOOL				bFillZero);

/**
* @brief This function pointer is a declaration of the "pfnSetFileTime" defined by native filesystem
* @param[in]	pVnode			NS_PVNODE			
* @param[in]	ptmCreated		NS_PSYS_TIME		
* @param[in]	ptmLastAccessed	NS_PSYS_TIME		
* @param[in]	ptmLastModified	NS_PSYS_TIME		
* @return		FERROR
* @exception	FERROR_NO_ERROR		success
* @par	Implementation : Optional
* @li	Set filetime
* @par	To do :
* @li	Nestle sets times on vnode after successful this call
* @li	ptmCreated, ptmLastAccessed and ptmLastModified parameters can be set to NULL 
* @li	if the application does not need to set this information.

*/
typedef FERROR (*PFN_NS_VNODE_OPS_SET_FILETIME) (
	IN	NS_PVNODE			pVnode,
	IN	NS_PSYS_TIME		ptmCreated,
	IN	NS_PSYS_TIME		ptmLastAccessed,
	IN	NS_PSYS_TIME		ptmLastModified);

/**
* @brief Set user id, group id, and permission of user, group, and other
* @param[in]	pVnode		nestle's vnode of a file / dir / symlink
* @param[in]	dwUid		user id
* @param[in]	dwGid		group id
* @param[in]	wPerm		oring the following : NS_OTHERS_EXECUTE, NS_OTHERS_WRITE, NS_OTHERS_READ,
*												  NS_GROUP_EXECUTE, NS_GROUP_WRITE, NS_GROUP_READ,
*												  NS_OWNER_EXECUTE, NS_OWNER_WRITE, NS_OWNER_READ
*
* @par   Implementation : Optional (for Linux)
* @li    Set uid, gid, and RWXRWXRWX on user, group, and others
*/
typedef FERROR (*PFN_NS_VNODE_OPS_SET_GUID_MODE) (
	IN	NS_PVNODE			pVnode,
	IN	unsigned int		dwUid,
	IN	unsigned int		dwGid,
	IN	NS_ACL_MODE			wPerm);

/**
* @brief This function pointer is a declaration of the "pfnMapBlocks" defined by native filesystem
* @param[in]	NS_PVNODE		NS_PVNODE			
* @param[in]	dwBlockIndex	unsigned int		
* @param[in]	dwBlockCnt		unsigned int		
* @param[out]	pdwBlockNum		unsigned int		
* @param[out]	pdwContBlockCnt	unsigned int		
* @return		FERROR
* @exception	FERROR_NO_ERROR	success
* @par	Implementation : Mandatory
* @li	Get absolute block address from file relative address
* @par	To do :
* @li	Native should fill pdwBlockNum with absolute address
* @li	pdwContBlockCnt is filled with how many consecutive blocks are from pdwBlockNum
*/
typedef FERROR (*PFN_NS_VNODE_OPS_MAP_BLOCKS) (
	IN	NS_PVNODE			pVnode,
	IN	unsigned int		dwBlockIndex,
	IN	unsigned int		dwBlockCnt,
	OUT unsigned int*		pdwBlockNum,
	OUT unsigned int*		pdwContBlockCnt);

/**
* @brief This function pointer is a declaration of the "pfnSyncFile" defined by native filesystem
* @param[in]	pVnode				NS_PVNODE	
* @param[in]	llValidFileSize		NS_FILE_SIZE
* @param[in]	bUpdateAccessTime	BOOL	
* @param[in]	bUpdateModifyTime	BOOL	
* @return		FERROR
* @exception	FERROR_NO_ERROR	success
* @par	Implementation : Mandatory 
* @par	To do :
* @li	Sync the native dirty data
* @li	To flush dirty buffered data, use NS_FlushFileBuffers()
* @li	Nestle clears dirty mark after successful this call
* @li	bUpdateAccessTime and bUpdateModifyTime be used with Linux glue. 
* @li	If these value is TRUE, NativeFS must update time information of pVnode.
* @li	Except linux OS, these value must be FALSE.
*/
typedef FERROR (*PFN_NS_VNODE_OPS_SYNC_FILE) (
	IN	NS_PVNODE			pVnode,
	IN	NS_FILE_SIZE		llValidFileSize,
	IN	BOOL				bUpdateAccessTime,
	IN	BOOL				bUpdateModifyTime);

/**
* @brief This function pointer is a declaration of the "pfnPermission" defined by native filesystem
* @param[in]	pVnode			NS_PVNODE
* @param[in]	dwOperationMode	unsigned int		
* @return		FERROR
* @exception	FERROR_NO_ERROR		success
* @par	Implementation : Optional
* @li	Native file system want to check other permission except RW permission. Native file system can define its own check permission method
* @li	If native wants to check permission additionaly, implements this
* @par	To do :
* @li	check permission against operation
* @code
* typedef enum _OPERATION_MODE
* {
* 	NS_OPEXEC_LOOKUP	= 0x00000001UL,
* 	NS_OPWRITE			= 0x00000002UL,
* 	NS_OPREAD			= 0x00000004UL,
* 	NS_OPAPPEND			= 0x00000008UL,
* 	NS_OPRESIZE			= 0x00000010UL,
* 	NS_OPDELETE			= 0x00000020UL,
* 	NS_OPMETAUPDATE		= 0x00000040UL
* } NS_OPERATION_MODE; 
* @endcode
* @see	NS_OPERATION_MODE
*/
typedef FERROR (*PFN_NS_VNODE_OPS_PERMISSION) (
	IN	NS_PVNODE			pVnode,
	IN	NS_OPERATION_MODE	dwOperationMode);

/**
* @brief This function pointer is a declaration of the "pfnExpandClusters" defined by native filesystem
* @param[in]	pVnode			NS_VNODE		
* @param[in]	llSize			NS_FILE_SIZE		
* @return		FERROR
* @exception	FERROR_NO_ERROR		success
* @par	Implementation : Mandatory
* @li	Expand clusters as requested
* @par	To do 
* @li	Expand as many clusters as cover llSize
* @li	If free space is previously not enough, Expand cluster as soon as possible(FERROR_NO_ERROR). 
* @li	But free space is zero, It is FERROR_NO_FREE_SPACE case
* @li	Don't update vnode's size
*/
typedef FERROR (*PFN_NS_VNODE_OPS_EXPAND_CLUSTERS) (
	IN	NS_PVNODE		pVnode,
	IN	NS_FILE_SIZE	llSize);

/**
* @brief This function pointer is a declaration of the "pfnConvertName" defined by native filesystem
* @param[in]	pParent				NS_VNODE		
* @param[in]	pwszInputName		wchar_t*		
* @param[in]	pwszInputSize		unsigned int		
* @param[out]	pwsOutputName		wchar_t*		
* @param[io]	dwOutputNameSize	unsigned int		
* @param[in]	dwConvertType		unsigned int(NS_CONVERT_TYPE)		
* @return		FERROR
* @exception	FERROR_NO_ERROR		success
* @par	Implementation : Mandatory (symbian, FAT)
* @li	convert name
* @par	To do :
* @li	Convert file length  long filename -> short filename, short filename -> long filename
* @code
* enum _NS_CONVERT_TYPE
* {
*	NS_FILE_NAME_SHORT_TO_LONG		= FILE_NAME_SHORT_TO_LONG,	
*	NS_FILE_NAME_LONG_TO_SHORT		= FILE_NAME_LONG_TO_SHORT	
* };
* @endcode
* @see NS_CONVERT_TYPE
*/

typedef FERROR (*PFN_NS_VNODE_OPS_CONVERT_NAME) (
	IN NS_PVNODE			pParent,
	IN const wchar_t*		pwszInputName,
	IN const unsigned int	pwszInputSize,
	OUT wchar_t*			pwsOutputName,
	IO unsigned int			dwOutputNameSize,
	IN unsigned int			dwConvertType);

/**
* @brief This function pointer is a declaration of the "pfnIndexUnlink" defined by native filesystem
* @param[in]	pVnode				vnode pointer			
* @param[in]	pFcb				open file object (File Control Block)		
* @param[in]	llVnodeID			vnode index (id)
* @return		FERROR
* @exception	FERROR_NO_ERROR		success
* @par	Implementation : Optional (RTOS only)
* @li	
* @par	To do :
* @li	Called by tfs4_readdir_unlink()
* @li	Unlink the child which has llVnodeID
* @li	Nestle make sure that the child does not open
* @li	
*/
typedef FERROR (*PFN_NS_VNODE_OPS_INDEX_UNLINK) (
	IN	NS_PVNODE			pVnode,
	IN	NS_PFCB				pFcb,
	IN	unsigned long long	llVnodeID);

/**
* @brief This function pointer is a declaration of the "pfnCleanDirectory" defined by native filesystem
* @param[in]	pVnode			vnode pointer		
* @return		FERROR
* @exception	FERROR_NO_ERROR		success
* @par	Implementation : Optional (RTOS only)
* @li	
* @par	To do :
* @li	Called by tfs4_cleandir()
* @li	clean all children in the directory (represented by pVnode)
* @li	Nestle make sure that any child in directory does not open
*/
typedef FERROR (*PFN_NS_VNODE_OPS_CLEAN_DIRECTORY) (
	IN	NS_PVNODE		pVnode);

/**
* @brief Set an extended attribute to file / directory / symlink
* @param[in]	pVnode			nestle's vnode
* @param[in]	psName			the name of extended attribute
* @param[in]	pValue			the value of extended attribute
* @param[in]	dwValueSize		the size of pValue
* @param[in]	dwID			the ID of extended attribute namespace
* @param[in]	dwFlag			0, XATTR_SET_CREATE, or XATTR_SET_REPLACE
* @par	Implementation : Optional (Linux only)
* @li	
* @par	To do :
* @li	Called by setxattr()
* @li	
*/
typedef FERROR (*PFN_NS_VNODE_OPS_SET_XATTRIBUTE) (
	IN NS_PVNODE				pVnode,
	IN const char*				psName,
	IN const void*				pValue,
	IN unsigned int				dwValueSize,
	IN NS_XATTR_NAMESPACE_ID	dwID,
	IN NS_XATTR_SET_FLAG		dwFlag);

/**
* @brief Get a value coresponding to the name from file / directory / symlink
* @param[in]	pVnode			nestle's vnode
* @param[in]	psName			the name of extended attribute
* @param[in]	pValue			the value of extended attribute
* @param[in]	dwValueSize		the size of pValue
* @param[in]	dwID			the ID of extended attribute namespace
* @param[out]	dwSizeRead		the read size
* @par	Implementation : Optional (Linux only)
* @li	
* @par	To do :
* @li	Called by getxattr()
* @li	If dwValueSize is zero, dwSizeRead is filled with length of the value about psName
* @li
*/
typedef FERROR (*PFN_NS_VNODE_OPS_GET_XATTRIBUTE) (
	IN NS_PVNODE				pVnode,
	IN const char*				psName,
	IN const void*				pValue,
	IN unsigned int				dwValueSize,
	IN NS_XATTR_NAMESPACE_ID	dwID,
	OUT unsigned int*			dwSizeRead);

/**
* @brief List up all the names of file / directory / symlink
* @param[in]	pVnode			nestle's vnode
* @param[out]	pOutBuf			the user buffer to contain the list of names
* @param[in]	dwOutBufSize	the buffer size of pOutBuf
* @param[out]	dwSizeRead		the read size
* @par	Implementation : Optional (Linux only)
* @li	
* @par	To do :
* @li	Called by listxattr()
* @li	If dwOutButSize is zero, dwSizeRead is filled with total length of all the names
* @li
*/
typedef FERROR (*PFN_NS_VNODE_OPS_LIST_XATTRIBUTES) (
	IN NS_PVNODE				pVnode,
	OUT char*					pOutBuf,
	IN unsigned int				dwOutBufSize,
	OUT unsigned int*			dwSizeRead);

/**
* @brief Remove an extended attribute from file / directory / symlink
* @param[in]	pVnode			nestle's vnode
* @param[in]	psName			the name of extended attribute to remove
* @param[in]	dwID			the ID of extended attribute namespace
* @par	Implementation : Optional (Linux only)
* @li	
* @par	To do :
* @li	Called by removexattr() 
* @li
*/
typedef FERROR (*PFN_NS_VNODE_OPS_REMOVE_XATTRIBUTE)(
	IN NS_PVNODE				pVnode,
	IN const char*				psName,
	IN NS_XATTR_NAMESPACE_ID	dwID);

/**
* @brief Vnode Operations structure
*/
typedef struct _VNODE_OPERATIONS		NS_VNODE_OPS, *NS_PVNODE_OPS;

/** @} */ // end of vnode_ops

/******************************************************************************/
/* VCB Operations                                                             */
/******************************************************************************/
/** 
 * @defgroup vcb_ops	 VolumeOps
 * @brief	Volume operations
 * @{
 */

/**
* @brief This function pointer is a declaration of the "pfnWriteVolume" defined by native filesystem
* @param[in]	pVcb	NS_PVCB				
* @return		FERROR
* @exception	FERROR_NO_ERROR		success
* @par	Implementation : Optional
* @li	If there is permanent in vcb, native should implement this
* @par	To do :
* @li	Call by umount or sync
*/
typedef FERROR (*PFN_NS_VCB_OPS_WRITE_VOLUME) (
	IN	NS_PVCB				pVcb);

/**
* @brief This function pointer is a declaration of the "pfnGetVolumeInformation" defined by native filesystem
* @param[in]	pVcb		NS_PVCB					
* @param[in]	pVolumeInfo	NS_PVOLUME_INFORMATION	
* @return		FERROR
* @exception	FERROR_NO_ERROR			success
* @par	Implementation : Mandatory
* @par	To do :
* @li	Fill pVolumeInfo. Native accesses NS_PVOLUME_INFORMATION directly
* @code
* struct _VOLUME_INFORMATION
* {
*	unsigned int		dwFsType;
*	unsigned int		dwFsID;
*	unsigned int		dwBlockSize;
*	unsigned int		dwNumBlocks;
*	unsigned int		dwNumFreeBlocks;
*	unsigned int		dwNumAvailClusters;
*	unsigned int		dwNumFiles;
*	unsigned int		dwNumFreeFiles;
*	unsigned int		dwMaxFileNameLen;
*   wchar_t				wszVolumeName[MAX_MOUNT_NAME_LENGTH+1];
* } NS_PVOLUME_INFORMATION;
* @endcode
* @see	NS_PVOLUME_INFORMATION
*/
typedef FERROR (*PFN_NS_VCB_OPS_GET_VOLUME_INFORMATION) (
	IN	NS_PVCB					pVcb,
	OUT NS_PVOLUME_INFORMATION	pVolumeInfo);

/**
* @brief This function pointer is a declaration of the "pfnUnmountDisk" defined by native filesystem
* @param[in]	NS_PVCB					pVcb
* @param[in]	NS_UMOUNT_FLAG			dwUnmountFlags
* @return		FERROR
* @exception	FERROR_NO_ERROR			success
* @par	Implementation : Mandatory
* @par	To do :
* @li	Called during unmount()
* @li	Deallocate native vcb
* @li	Followed by call of fnDestroyNode for deallocating root vnode
* @li	If dwUnmountFlags has NS_UNMOUNT_FORCE, then continue umounting against error
*/
typedef FERROR (*PFN_NS_VCB_OPS_UNMOUNT_DISK)(
	IN	NS_PVCB				pVcb,
	IN	NS_UNMOUNT_FLAG		dwUnmountFlags);

/**
* @brief This function pointer is a declaration of the "pfnRemountDisk" defined by native filesystem
* @param[in]	NS_PVCB					pVcb
* @param[in]	NS_PREMOUNT_FLAG		pRemountFlags
* @return		FERROR
* @exception	FERROR_NO_ERROR			success
* @exception	FERROR_NOT_SUPPORTED	when native filesystem can not support remount flag.
* @par	Implementation : Mandatory(but linux only)
* @par	To do :
* @li	Called during remount operation for linux
* @li	Change and return adopted mount flag
* @li	
*/
typedef FERROR (*PFN_NS_VCB_OPS_REMOUNT_DISK)(
	IN	NS_PVCB				pVcb,
	IO	NS_PREMOUNT_FLAG	pRemountFlags);

/**
* @brief This function pointer is a declaration of the "pfnFsIoctl" defined by native filesystem
* @param[in]	NS_PVCB					pVcb
* @param[in]	NS_IOCTL_CODE			dwControlCode
* @param[in]	void*					pInBuf
* @param[in]	unsigned int			dwInBufSize
* @param[in]	void*					pOutBuf
* @param[in]	unsigned int			dwOutBufSize
* @return	FERROR_NO_ERROR
* @return	FERROR_NOT_SUPPORT			not available control code
* @return	FERROR_INVALID_PARAMETER	operation fail
* @par	Implementation : Mandatory 
* @par	To do :  
* @see	NS_IOCTL_CODE
* @li	file system control for volume
*/
typedef FERROR (*PFN_NS_VCB_OPS_FSIOCTL) (
	IN	NS_PVCB				pVcb,
	IN	unsigned int		dwControlCode,
	IN	void*				pInBuf,
	IN	unsigned int		dwInBufSize,
	OUT	void*				pOutBuf,
	OUT	unsigned int		dwOutBufSize);

/**
* @brief VCB Operations
*/
typedef struct _VCB_OPERATIONS		NS_VCB_OPS, *NS_PVCB_OPS;

/** @} */ // end of vcb_ops

/******************************************************************************/
/* Nestle Interface							                                  */
/******************************************************************************/
/** 
 * @defgroup	ns_interface	Nestle Interface
 * @brief		Nestle Interface for Native
 * @{
 */	
/******************************************************************************/
/* FCB								                                          */
/******************************************************************************/
/** 
 * @defgroup fcb_inf	FCB
 * @brief	FILE_CONTROL_BLOCK Interface
 * @{
 */	

/** 
 * @defgroup fcb_inf_redef		Original Interfaces - FCB
 * These are re-definitions of vnode interfaces in view of native filesystem
 * @{
 */	
#define NS_GetVnodeFromFcb			FcbGetVnode
#define NS_GetOffsetFromFcb			FcbGetOffset
#define NS_IsBypassMode				FcbIsBypassMode
#define NS_IsSyncMode				FcbIsSyncMode
#define NS_IsModtimeUpdate			FcbIsModtimeUpdate
#define NS_IsAcstimeUpdate			FcbIsAcstimeUpdate

/** @} */ // end of fcb_inf_redef

/**
 * This is a re-definition of the FCB interface - "FcbGetVnode" used by native filesystem
 * @brief		Get vnode from fcb
 * @param[in]	pFcb		Nestle Fcb
 * @returns		NS_PVNODE	vnode where fcb belongs
 */
NS_PVNODE
NS_GetVnodeFromFcb(
	IN NS_PFCB			pFcb);

/**
 * This is a re-definition of the FCB interface - "FcbGetOffset" used by native filesystem
 * @brief		Get fcb's offset (start offset at I/O)
 * @param[in]	pFcb		Nestle Fcb
 * @returns		Open file's offset
 */

NS_FILE_OFFSET
NS_GetOffsetFromFcb(
	IN	NS_PFCB			pFcb); 

/**
 * This is a re-definition of the FCB interface - "FcbIsBypassMode" used by native filesystem
 * @brief		Check whether fcb opened with sync mode (NS_FCB_ACCESS_NO_BUFFERING)
 * @param[in]	pFcb		Nestle Fcb
 * @returns		TRUE when fcb opend with NS_FCB_ACCESS_NO_BUFFERING, FALSE otherwise
 */
BOOL
NS_IsBypassMode(
	IN NS_PFCB			pFcb); 

/**
 * This is a re-definition of the FCB interface - "FcbIsSyncMode" used by native filesystem
 * @brief		Check whether fcb opened with sync mode (NS_FCB_ACCESS_SYNCHRONOUS)
 * @param[in]	pFcb		Nestle Fcb
 * @returns		TRUE when fcb opend with NS_FCB_ACCESS_SYNCHRONOUS, FALSE otherwise
 */
BOOL
NS_IsSyncMode(
	IN NS_PFCB			pFcb);

/**
 * This is a re-definition of the FCB interface - "FcbIsModtimeUpdate" used by native filesystem
 * @brief		Check whether fcb opened with modified time updation (NS_FCB_MODTIME_UPDATE)
 * @param[in]	pFcb	Nestle¿« Fcb pointer
 * @returns		TRUE when fcb opend with NS_FCB_MODTIME_UPDATE, FALSE otherwise
 * @remarks 
 */
BOOL
NS_IsModtimeUpdate(
	IN	NS_PFCB			pFcb);

/**
* This is a re-definition of the FCB interface - "FcbIsAcstimeUpdate" used by native filesystem
* @brief		Check whether fcb opened with access time updation (NS_FCB_ACSTIME_UPDATE)
* @param[in]	pFcb	Nestle Fcb
* @returns		TRUE when fcb opend with NS_FCB_ACSTIME_UPDATE, FALSE otherwise
* @remarks 
*/
BOOL
NS_IsAcstimeUpdate(
   IN	NS_PFCB			pFcb);


/** @} */ // end of fcb_inf

/******************************************************************************/
/* VIRTUAL NODE                                                               */
/******************************************************************************/

/** 
 * @defgroup vnode_inf	Vnode
 * @brief	VNODE Interface
 * @{
 */	

/** 
 * @defgroup vnode_inf_redefine		Original Interfaces - vnode
 * These are re-definitions of vnode interfaces in view of native filesystem.
 * @{
 */	

#define NS_GetVnodeIndex				VnodeGetIndex
#define NS_ChangeVnodeIndex				VnodeChangeIndex
#define NS_GetVnodeUid					VnodeGetUid
#define NS_SetVnodeUid					VnodeSetUid
#define	NS_GetVnodeGid					VnodeGetGid
#define	NS_SetVnodeGid					VnodeSetGid
#define NS_GetFileSize					VnodeGetSize
#define NS_SetFileSize					VnodeSetSize
#define NS_GetVnodeAttr					VnodeGetAttr
#define NS_SetVnodeAttr					VnodeSetAttr
#define NS_IsInitCluster				VnodeIsInitCluster
#define NS_GetVnodeLinkCnt				VnodeGetLinkCnt
#define NS_SetVnodeLinkCnt				VnodeSetLinkCnt
#define NS_GetVnodeAcl					VnodeGetAcl
#define NS_SetVnodeAcl					VnodeSetAcl
#define NS_GetFileTimes					VnodeGetTimes
#define NS_SetFileTimes					VnodeSetTimes
#define NS_GetVcbFromVnode				VnodeGetVcb
#define NS_GetNativeNode				VnodeGetNative
#define NS_LinkNativeNode				VnodeSetNative
#define NS_MarkVnodeMetaDirty			VnodeMarkMetaDirty
#define NS_MarkVnodeDataDirty			VnodeMarkDataDirty
#define	NS_FillZeroVnode				VnodeFillZero
#define NS_ReleaseVnode					VnodeRelease

#if defined(CONFIG_LINUX)
#define NS_GetVnodeBlocks				VnodeGetBlocks
#define NS_SetVnodeBlocks				VnodeSetBlocks
#endif
#if defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_SYMBIAN)
#define	NS_GetVnodeBlocks(pVnode)				do {} while(0)
#define	NS_SetVnodeBlocks(pVnode, llSize)		do {} while(0)
#endif /*defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_SYMBIAN)*/

/** @} */ // end of vnode_inf_redefine

/**
 * This is a re-definition of the VNODE interface - "VnodeGetIndex" used by native filesystem
 * @brief		Get Vnode's index
 * @param[in]	pVnode			Nestle Vnode
 * @returns		unsigned long long	Vnode ID
 * @remarks 
 * 
 */
unsigned long long
NS_GetVnodeIndex(
	IN	NS_PVNODE	pVnode);

/**
 * This is a re-definition of the VNODE interface - "VnodeChangeIndex" used by native filesystem
 * @brief		Change Vnode's index
 * @param[in]	pVnode		Nestle Vnode
 * @param[in]	llVnodeID		Vnode ID
 * @returns		void
 * @remarks 
 * 
 */
void
NS_ChangeVnodeIndex(
	IN	NS_PVNODE			pVnode, 
	IN	unsigned long long	llVnodeID);

/**
* This is a re-definition of the VNODE interface - "VnodeGetAcl" used by native filesystem.\n
* @brief		Get Vnode's Access level
* @param[in]	pVnode			Nestle Vnode
* @returns		unsigned short	read/write/execute of other, group, owner
* @remarks 
*/
unsigned short
NS_GetVnodeAcl(
	IN	NS_PVNODE	pVnode);

/**
 * This is a re-definition of the VNODE interface - "VnodeGetUid" used by native filesystem
 * @brief		Get Vnode's user ID
 * @param[in]	pVnode			Nestle Vnode
 * @returns		unsigned int	User ID
 * @remarks 
 * 
 */
unsigned int
NS_GetVnodeUid(
	IN	NS_PVNODE	pVnode);

/**
 * This is a re-definition of the VNODE interface - "VnodeSetUid" used by native filesystem
 * @brief		Set Vnode's user ID
 * @param[in]	pVnode		Nestle Vnode
 * @param[in]	dwUid		User ID
 * @returns		void
 * @remarks 
 * 
 */
void
NS_SetVnodeUid(
	IN	NS_PVNODE		pVnode,
	IN	unsigned int	dwUid);

/**
 * This is a re-definition of the VNODE interface - "VnodeGetGid" used by native filesystem
 * @brief		Get Vnode's group ID
 * @param[in]	pVnode			Nestle Vnode
 * @returns		unsigned int	Group ID
 * @remarks 
 * 
 */
unsigned int
NS_GetVnodeGid(
	IN	NS_PVNODE	pVnode);


/**
 * This is a re-definition of the VNODE interface - "VnodeSetGid" used by native filesystem
 * @brief		Set Vnode's group ID
 * @param[in]	pVnode		Nestle Vnode
 * @param[in]	dwGid		Group ID
 * @returns		void
 * @remarks 
 * 
 */
void
NS_SetVnodeGid(
	IN	NS_PVNODE		pVnode,
	IN	unsigned int	dwGid);

/**
 * This is a re-definition of the VNODE interface - "VnodeGetSize" used by native filesystem
 * @brief		Get Vnode's File Size
 * @param[in]	pVnode		Nestle Vnode
 * @returns		File size in pVnode
 * @remarks 
 * 
 */
FILE_SIZE
NS_GetFileSize(
	IN	NS_PVNODE	pVnode);

/**
 * This is a re-definition of the VNODE interface - "VnodeSetSize" used by native filesystem
 * @brief		Set Vnode's File size
 * @param[in]	pVnode		Nestle Vnode
 * @param[in]	llSize		File size to set
 * @returns		void
 * @remarks 
 * 
 */
void
NS_SetFileSize(
	IN	NS_PVNODE	pVnode,
	IN	FILE_SIZE	llSize);

#if defined(CONFIG_LINUX)
/**
 * This is a re-definition of the VNODE interface - "VnodeGetBlocks" used by native filesystem
 * @brief		Get Vnode's Blocks(Linux only)
 * @param[in]	pVnode		Nestle Vnode
 * @returns		vnode blocks in pVnode(bytes)
 * @remarks 
 * 
 */
unsigned int
NS_GetVnodeBlocks(
	IN	NS_PVNODE	pVnode);

/**
 * This is a re-definition of the VNODE interface - "VnodeSetBlocks" used by native filesystem
 * @brief		Set Vnode's Blocks(Linux only)
 * @param[in]	pVnode		Nestle Vnode
 * @param[in]	dwSize		Blocks to set(bytes)
 * @returns		void
 * @remarks 
 * 
 */
void
NS_SetVnodeBlocks(
	IN	NS_PVNODE	pVnode,
	IN	FILE_SIZE	llSize);

#endif /*defined(CONFIG_LINUX)*/

/**
 * This is a re-definition of the VNODE interface - "VnodeGetAttr" used by native filesystem
 * @brief		Get Vnode's atrribute
 * @param[in]	pVnode			Nestle Vnode
 * @returns		unsigned int	Attributes. See _NS_FILE_ATTR
 * @remarks 
 * 
 */
unsigned int
NS_GetVnodeAttr(
	IN	NS_PVNODE		pVnode);

/**
 * This is a re-definition of the VNODE interface - "VnodeSetAttr" used by native filesystem
 * @brief		Set Vnode's attribute
 * @param[in]	pVnode		Nestle Vnode
 * @param[in]	dwAttr		Attributes. See _NS_FILE_ATTR
 * @returns		void
 * @remarks 
 * 
 */
void
NS_SetVnodeAttr(
	IN	NS_PVNODE		pVnode,
	IN	unsigned int	dwAttr);

/**
 * This is a re-definition of the vnode interface - "VnodeIsInitCluster" used by native filesystem
 * @brief		Check whether native fills zero in hole or not
 * @param[in]	pVnode	Nestle vnode
 * @returns		TRUE on zerofill, FALSE on no zerofill
 * @remarks 
 *
 */
BOOL
NS_IsInitCluster(
	IN	NS_PVNODE		pVnode);

/**
 * This is a re-definition of the VNODE interface - "VnodeGetLinkCnt" used by native filesystem
 * @brief		Get Vnode's link count
 * @param[in]	pVnode			Nestle Vnode
 * @returns		unsigned short	Vnode's link count
 * @remarks 
 */
unsigned short
NS_GetVnodeLinkCnt(
	IN	NS_PVNODE		pVnode);

/**
 * This is a re-definition of the VNODE interface - "VnodeSetLinkCnt" used by native filesystem
 * @brief		Set Vnode's link count
 * @param[in,out]	pVnode			Nestle Vnode
 * @param[in]	wCnt			link count
 * @returns		unsigned short	Vnode's link count
 * @remarks 
 */
unsigned short
NS_SetVnodeLinkCnt(
	IO	NS_PVNODE		pVnode,
	IN	unsigned short	wCnt);

/**
 * This is a re-definition of the VNODE interface - "VnodeSetAcl" used by native filesystem
 * @brief		Set Vnode's ACL (RWX Permission)
 * @param[in]	pVnode		Nestle Vnode
 * @param[in]	wAcl		RWX permission (Refer to _ACL_MODE)
 * @returns		void
 * @remarks 
 */
void
NS_SetVnodeAcl(
	IN	NS_PVNODE		pVnode,
	IN	unsigned short	wAcl);

/**
 * This is a re-definition of the VNODE interface - "VnodeGetTimes" used by native filesystem
 * @brief		Get Vnode's times
 * @param[in]	pVnode		Nestle Vnode
 * @param[out]	pftCreate	create timestamp
 * @param[out]	pftAccess	access timestamp
 * @param[out]	pftUpdate	update timestamp
 * @returns		void
 * @remarks 
 *
 */
void
NS_GetFileTimes(
	IN	NS_PVNODE			pVnode,
	OUT	NS_PSYS_TIME		pftCreate,
	OUT	NS_PSYS_TIME		pftAccess,
	OUT	NS_PSYS_TIME		pftUpdate);

/**
 * This is a re-definition of the VNODE interface - "VnodeSetTimes" used by native filesystem
 * @brief		Set Vnode's time
 * @param[in]	pVnode		Nestle Vnode
 * @param[in]	pftCreate	create timestamp
 * @param[in]	pftAccess	access timestamp
 * @param[in]	pftUpdate	update timestamp
 * @returns		void
 * @remarks 
 *
 */
void
NS_SetFileTimes(
	IN	NS_PVNODE			pVnode,
	IN	NS_PSYS_TIME		pftCreate,
	IN	NS_PSYS_TIME		pftAccess,
	IN	NS_PSYS_TIME		pftUpdate);

/**
 * This is a re-definition of the VNODE interface - "VnodeGetVcb" used by native filesystem
 * @brief		Get Vnode's nestle volume
 * @param[in]	pVnode	Nestle Vnode
 * @returns		pVcb	Nestle Vcb
 * @remarks 
 */
NS_PVCB
NS_GetVcbFromVnode(
	IN	NS_PVNODE		pVnode);

/**
 * This is a re-definition of the VNODE interface - "VnodeGetNative" used by native filesystem
 * @brief		Get native node for Nestle Vnode
 * @param[in]	pVnode		Nestle Vnode
 * @returns		void*		Native node (Native should cast it by own type)
 * @remarks 
 */
void*
NS_GetNativeNode(
	IN	NS_PVNODE	pVnode);

/**
 * This is a re-definition of the VNODE interface - "VnodeSetNative" used by native filesystem
 * @brief		Set native node for nestle Vnode
 * @param[in]	pVnode		Nestle Vnode
 * @param[in]	pNative		Native node
 * @returns		void
 * @remarks 
 *
 */
void
NS_LinkNativeNode(
	IN	NS_PVNODE	pVnode,
	IN	void*		pNative);

/**
 * This is a re-definition of the VNODE interface - "VnodeMarkMetaDirty" used by native filesystem
 * @brief		Set Vnode dirty mark for meta-data
 * @param[in]	pVnode	Nestle Vnode
 * @returns		void
 * @remarks 
 *
 */
void
NS_MarkVnodeMetaDirty(
	IN	NS_PVNODE	pVnode);

/**
 * This is a re-definition of the VNODE interface - "VnodeMarkDataDirty" used by native filesystem
 * @brief		Set Vnode dirty mark for data area
 * @param[in]	pVnode	Nestle Vnode
 * @returns		void
 * @remarks 
 *
 */
void
NS_MarkVnodeDataDirty(
	IN	NS_PVNODE	pVnode);

/**
 * This is a re-definition of the VNODE interface - "VnodeFillZero" used by native filesystem
 * @brief		write all zeros in range
 * @param[in]	pVnode			Nestle Vnode
 * @param[in]	llOffset		Start offset
 * @param[in]	dwByteToFill			the size of zerofill in byte
 * @param[out]	pdwBytesFilled			the written size in byte
 * @returns		FERROR		error code
 */
FERROR
NS_FillZeroVnode(
	IN  NS_PVNODE		pVnode,
	IN  FILE_OFFSET		llOffset,
	IN  unsigned int	dwByteToFill,
	OUT	unsigned int*	pdwBytesFilled);

/**
 * This is a re-definition of the VNODE interface - "VnodeRelease" used by native filesystem
 * @brief		Release vnode. If there is no referencer, vnode can be destroyed
 * @param[in]	pVnode			Vnode to release
 * @returns		void
 */
void
NS_ReleaseVnode(
	IN NS_PVNODE pVnode);

/** @} */ // end of vnode_inf

/******************************************************************************/
/* VOLUME CONTROL BLOCK                                                       */
/******************************************************************************/
/** 
 * @defgroup vcb_inf	VCB
 * @brief	VOLUME_CONTROL_BLOCK Interface
 * @{
 */

/** 
 * @defgroup vcb_inf_redef Original Interfaces - VCB
 * NESTLE VCB module
 * @{
 */	
// public member functions of Vcb
#define NS_RegisterNativeVcb					VcbSetNative
#define NS_GetNativeVcb							VcbGetNative
#define NS_GetNativeVcbFromVnode(pVnode)		NS_GetNativeVcb(NS_GetVcbFromVnode(pVnode))
#define NS_GetRootFromVcb						VcbGetRoot
#define NS_RegisterVcbOperation					VcbSetOperation
#define NS_SetBlockSize							VcbSetBlockSize
#define NS_GetBlockSize							VcbGetBlockSize
#define NS_GetBlockSizeBits						VcbGetBlockSizeBits
#define NS_GetLogicalDisk						VcbGetLogicalDisk
#define	NS_FindOrCreateVnodeFromVcb				VcbFindOrCreateVnode
#define NS_FindVnodeFromVcb						VcbFindVnode
#define NS_MarkDirtyVcb							VcbMarkDirty
#define NS_SetCaseSensitivity					VcbSetCaseSensitivity
#define NS_NotifyFreeBlockCount					VcbNotifyFreeBlockCount
/** @} */ // end of vcb_inf_redef

/**
 * This is a re-definition of the VCB interface - "VcbSetNative" used by native filesystem
 * @brief		register native vcb into nestle vcb
 * @param[in]	pVcb		nestle vcb for mounting volume
 * @param[in]	pNativeVcb	native vcb (nestle registers the address of native vcb)
 * @returns		void
 * @remarks 
 * 
 */
void
NS_RegisterNativeVcb(
	IN	NS_PVCB		pVcb,
	IN	void*		pNativeVcb);

/**
 * This is a re-definition of the VCB interface - "VcbGetNative" used by native filesystem
 * @brief		get native vcb from nestle vcb
 *				native should cast its type from (void *)
 * @param[in]	pVcb	nestle vcb for mounted volume
 * @returns		void*	native vcb
 * @remarks 
 * 
 */
void*
NS_GetNativeVcb(
	IN	NS_PVCB		pVcb);


/**
 * This is a re-definition of the VCB interface - "VcbGetRoot" used by native filesystem
 * @brief		get root vnode of mounted volume
 * @param[in]	pVcb		vcb for mounted volume
 * @returns		NS_PVNODE	vnode for root directory
 * @remarks		
 * 
 */
NS_PVNODE
NS_GetRootFromVcb(
	IN	NS_PVCB		pVcb);


/**
 * This is a re-definition of the VCB interface - "VcbSetOperation" used by native filesystem
 * @brief		register volume operations
 * @param[in]	pVcb		vcb for mounting volume
 * @param[in]	pVolumeOps	volume operations
 * @returns		void
 * @remarks 
 * 
 */
void
NS_RegisterVcbOperation(
	IN	NS_PVCB		pVcb,
	IN	void*		pVolumeOps);


/**
 * This is a re-definition of the VCB interface - "VcbSetBlockSize" used by native filesystem
 * @brief		Set blocksize and blocksize bits on mounted volume
 * @param[in]	pVcb			vcb for mounted volume
 * @param[in]	dwBlockSize		blocksize to set
 * @returns		FERROR			error code if logical disk has greater atomic blocksize than dwBlockSize
 * @remarks 
 * 
 */
FERROR
NS_SetBlockSize(
	IN	NS_PVCB			pVcb,
	IN	unsigned int	dwBlockSize);

/**
 * This is a re-definition of the VCB interface - "VcbGetBlockSize" used by native filesystem
 * @brief		Get blocksize of mounted volume
 * @param[in]	pVcb			vcb for mounted volume
 * @returns		unsigned int	blocksize
 * @remarks 
 * 
 */
unsigned int
NS_GetBlockSize(
	IN	NS_PVCB		pVcb);


/**
 * This is a re-definition of the VCB interface - "VcbGetBlockSizeBits" used by native filesystem
 * @brief		Get blocksize bits of mounted volume
 * @param[in]	pVcb			vcb for mounted volume
 * @returns		unsigned int	blocksize bits
 * @remarks 
 * 
 */
unsigned int
NS_GetBlockSizeBits(
	IN	NS_PVCB		pVcb);

/**
 * This is a re-definition of the VCB interface - "VcbGetLogicalDisk" used by native filesystem
 * @brief		Get logical disk of mounted volume
 * @param[in]	pVcb		vcb for mounted volume
 * @returns		NS_PLOGICAL_DISK	logical dice of mounted volume
 * @remarks 
 * 
 */
NS_PLOGICAL_DISK
NS_GetLogicalDisk(
	IN	NS_PVCB		pVcb);

/**
 * This is a re-definition of the VCB interface - "VcbFindOrCreateVnode" used by native filesystem
 * @brief		Find exsting vnode or create new vnode
 * @param[in]	pVcb			vcb for mounted volume
 * @param[in]	llVnodeID		vnode ID to find or create
 * @param[in]	pParentVnode	parent vnode for create, If parent node is absent, input NULL
 * @param[in]	pFileOps		native file operations
 * @param[in]	pNodeOps		native vnode operations (file, dir, or symlink)
 * @param[out]	pbNew			TRUE on creating, FALSE on finding
 * @returns		NS_PVNODE	found / created vnode which has llVnodeID
 * @remarks 
 */
NS_PVNODE
NS_FindOrCreateVnodeFromVcb(
	IN	NS_PVCB				pVcb,
	IN	unsigned long long	llVnodeID,
	IN	NS_PVNODE			pParentVnode,
	IN	void*				pFileOps,
	IN	void*				pNodeOps,
	OUT	unsigned int*		pbNew);

/**
 * This is a re-definition of the VCB interface - "VcbFindVnode" used by native filesystem
 * @brief		Find a vnode and return it after increasing reference. If not exists, return null
 * @param[in]	pVcb		vcb for mounted volume
 * @param[in]	llVnodeID	vnode ID to find
 * @returns		pVnode		found vnode (NULL possible)
 * @remarks 
 */
NS_PVNODE
NS_FindVnodeFromVcb(
	IN	NS_PVCB				pVcb,
	IN	unsigned long long	llVnodeID);

/**
* This is a re-definition of the VCB interface - "VcbMarkDirty" used by native filesystem
* @brief		Mark dirty at volume
* @param[in]	pVcb		vcb for mounted volume
* @returns		FERROR_NO_ERROR
*				FERROR_READONLY_FS
* @remarks 
* 
*/
FERROR
NS_MarkDirtyVcb(
	IN	NS_PVCB		pVcb);

/**
 * This is a re-definition of the VCB interface - "VcbSetCaseSensitivity" used by native filesystem
 * @brief		Notify case-sensitivity to Nestle
 * @param[in]	pVcb			vcb for mounted volume
 * @param[in]	bIsSensitive	True if a native is sensitive; otherwise false
 * @remarks 
 */
void
NS_SetCaseSensitivity(
	IN	NS_PVCB		pVcb,
	IN	BOOL		bIsSensitive);

/**
 * Notify the change of free block count to Nestle
 * @brief		Notify the change of free block count to Nestle
 * @param[in]	pVcb			vcb for mounted volume
 * @param[in]	dwTotalCount	The total block count
 * @param[in]	dwFreeCount		The free block count
 * @param[in]	dwBlockSize		The block size
 */
void
NS_NotifyFreeBlockCount(
	IN	NS_PVCB			pVcb,
	IN	unsigned int	dwTotalCount,
	IN	unsigned int	dwFreeCount,
	IN	unsigned int	dwBlockSize);

/** @} */ // end of vcb_inf

/******************************************************************************/
/* Buffer Cache                                                               */
/******************************************************************************/
/** 
 * @defgroup bCache_inf	Buffer Cache
* @brief	Buffer Cache Interface
 * @{
 */

/** 
 * @defgroup bCache_inf_redef Original Interfaces - BCM
 * NESTLE BCM module
 * @{
 */	

#define	NS_ReadBuffer			BcmReadBuffer
#define	NS_WriteBuffer			BcmWriteBuffer
#define	NS_SyncBuffer			BcmSyncBuffer
#define	NS_DiscardBuffer		BcmDiscardBuffer
#define	NS_ReadBufferV			BcmReadBufferV
#define	NS_WriteBufferV			BcmWriteBufferV


/**
* @brief		read buffer
* @param[in]	pVcb			pointer of vcb
* @param[in]	pVnode			pointer of vnode 
* @param[in]	dwStartBlock	start block number
* @param[in]	dwBlockCount	blcok count
* @param[in]	pBuff			buffer pointer
* @param[in]	dwFlag			operation flag 
* @returns		FERROR_NO_ERROR
* @remarks		
* 
*/
FERROR
NS_ReadBuffer(
	IN	NS_PVCB				pVcb,
	IN	NS_PVNODE			pVnode,	
	IN	unsigned int		dwStartBlock,
	IN	unsigned int		dwBlockCount,
	OUT	char*				pBuff,
	IN	NS_BCM_OP_FLAG		dwFlag);

/**
* @brief		write buffer
* @param[in]	pVcb			pointer of vcb
* @param[in]	pVnode			pointer of vnode 
* @param[in]	dwStartBlock	start block number
* @param[in]	dwBlockCount	blcok count
* @param[in]	pBuff			buffer pointer
* @param[in]	dwFlag			operation flag 
* @returns		FERROR_NO_ERROR
* @remarks		
* 
*/
FERROR
NS_WriteBuffer(
	IN	NS_PVCB				pVcb,
	IN	NS_PVNODE			pVnode,
	IN	unsigned int 		dwStartBlock,
	IN	unsigned int		dwBlockCount,
	IN	char*				pBuff,
	IN	NS_BCM_OP_FLAG		dwFlag);


/**
* @brief		sync buffer
* @param[in]	pVcb		pointer of vcb
* @param[in]	pVnode		pointer of vnode
* @returns		FERROR_NO_ERROR
* @remarks		
* 
*/
FERROR
NS_SyncBuffer(
	IN NS_PVCB				pVcb,
	IN NS_PVNODE			pVnode);

/**
* @brief		discard buffer
*				find entry and discard in pVnode list.
*				If entry is only partially included discard range, don't discard entry but flush entry
*				It would be discard a whole entry in vnode by "BCM_OP_ALL" flag
* @param[in]	pVcb			mounted vcb
* @param[in]	pVnode			vode which owns discard data 
* @param[in]	dwStartBlock	start block number
* @param[in]	dwBlockCount	block count
* @param[in]	dwFlag			operation flag 
* @returns		FERROR_NO_ERROR
* @remarks		
* 
*/
FERROR
NS_DiscardBuffer(
	IN NS_PVCB				pVcb,
	IN NS_PVNODE			pVnode,
	IN unsigned int			dwStartBlock,
	IN unsigned int			dwBlockCount,
	IN NS_BCM_OP_FLAG		dwFlag);

/**
* @brief		vector read buffer
* @param[in]	pVcb			pointer of vcb
* @param[in]	pVnode			pointer of vnode 
* @param[in]	pVI				vector IO information
* @param[in]	dwFlag			operation flag 
* @returns		FERROR_NO_ERROR
* @remarks		
* 
*/
FERROR
NS_ReadBufferV(
	IN	NS_PVCB				pVcb,
	IN	NS_PVNODE			pVnode,
	IO	NS_PVI				pVI,
	IN	NS_BCM_OP_FLAG		dwFlag);

/**
* @brief		vector write buffer
* @param[in]	pVcb			pointer of vcb
* @param[in]	pVnode			pointer of vnode 
* @param[in]	pVI				vector IO information
* @param[in]	dwFlag			operation flag 
* @returns		FERROR_NO_ERROR
* @remarks		
* 
*/
FERROR
NS_WriteBufferV(
	IN NS_PVCB				pVcb,
	IN NS_PVNODE			pVnode,
	IO NS_PVI				pVI,
	IN NS_BCM_OP_FLAG		dwFlag);

/** @} */ // end of bCache_inf


/******************************************************************************/
/* LOGICAL DISK                                                               */
/******************************************************************************/
/** 
 * @defgroup logdisk_inf	Logical Disk
 * @brief	Logical Disk Interface
 * @{
 */
#define	NS_ReadWriteSectors			LdevReadWriteSectors
#define NS_ReadSectors(NS_PLOGICAL_DISK, dwSector, dwCount, pBuff, dwFlag)					\
	NS_ReadWriteSectors(NS_PLOGICAL_DISK, dwSector, dwCount, pBuff, dwFlag | NS_READ)
#define NS_WriteSectors(NS_PLOGICAL_DISK, dwSector, dwCount, pBuff, dwFlag)					\
	NS_ReadWriteSectors(NS_PLOGICAL_DISK, dwSector, dwCount, pBuff, dwFlag | NS_WRITE)
#define	NS_ReadWriteSectorsV		LdevReadWriteSectorsV 
#define NS_ReadSectorsV(NS_PLOGICAL_DISK, pVI, dwFlag)										\
	NS_ReadWriteSectorsV(NS_PLOGICAL_DISK, pVI, dwFlag | NS_READ)
#define NS_WriteSectorsV(NS_PLOGICAL_DISK, pVI, dwFlag)										\
	NS_ReadWriteSectorsV(NS_PLOGICAL_DISK, pVI, dwFlag | NS_WRITE)

#define NS_DeviceIoCtl				LdevIoControl
#define	NS_GetDiskInfo				LdevGetDiskInfo
#define	NS_EraseSectors				LdevEraseSectors
#define NS_GetDeviceName			LdevGetDeviceName
#define	NS_AttachNode				LdevAttachNode
#define	NS_DetachNode				LdevDetachNode
 
/**
 * This is a re-definition of the Logical Disk interface - "LdevGetDeviceName" used by native filesystem
 * @brief		get logical disk device name
 * @param[in]	pLogDisk		pointer to a logical disk.
 * @returns		device name
 * @see 
 * 
 */
wchar_t*
NS_GetDeviceName(
	IN	NS_PLOGICAL_DISK	pLogDisk);


/**
 * This is a re-definition of the Logical Disk interface - "LdevIoControl" used by native filesystem
 * @brief		Io Control for Logical Disk
 * @param[in]	pLogDisk		pointer to a logical disk.
 * @param[in]	dwControlCode	Control code for device io control
 * @param[in]	pInBuf			pointer to a Input buffer 
 * @param[in]	dwInBufSize		Input Buffer size
 * @param[out]	pOutBuf			pointer to a Output buffer 
 * @param[in]	dwOutBufSize	Output Buffer size
 * @returns		FERROR_INSUFFICIENT_BUFFER
 *				FERROR_NO_ERROR
 * @see 
 * 
 */
FERROR
NS_DeviceIoCtl(
	IN NS_PLOGICAL_DISK		pLogDisk,
	IN unsigned int			dwControlCode,
	IN void*				pInBuf,
	IN unsigned int			dwInBufSize,
	OUT void*				pOutBuf,
	IN	unsigned int		dwOutBufSize);


/**
 * This is a re-definition of the Logical Disk interface - "LdevGetDiskInfo" used by native filesystem
 * @brief		Get logical disk Info
 * @param[in]	pLogDisk		pointer to a logical disk.
 * @param[out]	pdwNumSectors	total number of sectors
 * @param[out]	pdwBytesPerSector	bytes per sector
 * @param[out]	pdwStartSectorNum	start sector number
 * @param[out]	pdwFlags			logical disk disk info flags (for ce)
 * @returns		void
 * @see 
 * 
 */
void
NS_GetDiskInfo(
	IN	NS_PLOGICAL_DISK	pLogDisk,
	OUT unsigned int*		pdwNumSectors,
	OUT unsigned int*		pdwBytesPerSector,
	OUT unsigned int*		pdwStartSectorNum,
	OUT	unsigned int*		pdwFlags);


/**
 * This is a re-definition of the Logical Disk interface. "LdevReadWriteSectors" used by native filesystem.
 * @brief		read sectors / write sectors from / to logical device directly
 * @param[in]	pLogDisk		pointer to a logical disk.
 * @param[in]	dwStartSector	start sector number for read / write
 * @param[in]	dwSectorCnt		sector count for read / write
 * @param[in,out]	pBuffer		pointer to user buffer
 * @param[in]	dwFlag			flag for data
 * @returns		void
 * @see 
 * 
 */
FERROR
NS_ReadWriteSectors(
	IN NS_PLOGICAL_DISK		pLogDisk,
	IN unsigned int			dwStartSector,
	IN unsigned int			dwSectorCnt,
	IO char*				pBuffer,
	IN unsigned int			dwFlag);


/**
 * This is a re-definition of the Logical Disk interface - "LdevEraseSectors" used by native filesystem.\n
 * @brief		erase sectors explicitly for secure deletion or something
 * @param[in]	pLogDisk		pointer to a logical disk.
 * @param[in]	pSectorRange	sector range structure pointer
 * @returns		FERROR
 * @remarks
 * @see 
 * 
 */
FERROR
NS_EraseSectors(
	IN	NS_PLOGICAL_DISK		pLogDisk,
	IN	NS_PDISK_SECTOR_RANGE	pSectorRange);


/**
* @brief		attach Node in logical disk (Device Node)
* @param[in]	pLogDisk	pointer to a logical disk.
* @param[in]	pVnode		pointer to device node
* @returns		void
* @remarks 
*
* @see 
* 
*/
void
NS_AttachNode(
	IN NS_PLOGICAL_DISK		pLogDisk,
	IN NS_PVNODE			pVnode);


/**
 * @brief		Detach Node in logical disk (Device Node)
 * @param[in]	pLogDisk	pointer to a logical disk.
 * @param[in]	pVnode		pointer to device node
 * @returns		void
 * @remarks 
 *
 * @see 
 * 
 */
void
NS_DetachNode(
	IN NS_PLOGICAL_DISK		pLogDisk,
	IN NS_PVNODE			pVnode);


/** @} */ // end of logdisk_inf


/******************************************************************************/
/* STORAGE  & Physical Device                                                 */
/******************************************************************************/
/** 
 * @defgroup storage_inf	Storage
 * @brief	Storage Interface	
 *			add for devfs. devfs uses storage and device module
 * @{
 */
#define	NS_GetLogicalDiskByName		StgGetLogicalDisk
#define	NS_GetPdevDiskCount			StgGetPdevDiskCount
#define	NS_FindPdevDisk				StgFindPdevDisk
#define	NS_LockStorageManager		StgLockStorageManager
#define	NS_UnlockStorageManager		StgUnlockStorageManager
#define	NS_FindLogicalDisk			PdevFindLogicalDisk

/**
 * @brief		NS_GetLogicalDiskByName gets a logical disk pointer
 * @param[in]	pwszName	block device name  (this is already removed "/dev/" by caller function)
 * @param[in]	cchName		count of pwszName
 * @returns		PLOGICAL_DISK	logical disk pointer
 * @see			_GetDevice, PdevFindLogicalDisk
 * 
 */
NS_PLOGICAL_DISK
NS_GetLogicalDiskByName(
	IN const wchar_t*	pwszName,
	IN unsigned int		cchName);

/**
 * @brief NS_GetPdevCount returns block device count
 * @returns unsigned int	count of block device in registerd device list
 */
unsigned int
NS_GetPdevDiskCount(void);

/**
 * @brief NS_FindPdevDisk finds block device and logical disk count.
 * @param[in]	dwDeviceIndex is a index of block device
 * @param[out]	pdwLogDiskCount is a count value of logical disk
 * @returns		NS_PBLOCK_DEVICE	Block device pointer
 */
NS_PBLOCK_DEVICE
NS_FindPdevDisk(
	IN	unsigned int	dwDeviceIndex,
	OUT	unsigned int*	pdwLogDiskCount);

/**
 * @brief		NS_LockStorageManager gets a lock
 * @returns		void
 * 
 */
void
NS_LockStorageManager(void);

/**
 * @brief		NS_UnlockStorageManager release a lock
 * @returns		void
 * 
 */
void
NS_UnlockStorageManager(void);

/**
 * @brief			NS_FindLogicalDisk finds logical disks with index
 * @param[in]		pDevice		physical device pointer
 * @param[in]		dwIndex		logical disk index
 * @returns			PLOGICAL_DISK	logical disk pointer
 */
NS_PLOGICAL_DISK
NS_FindLogicalDisk(
	IN NS_PBLOCK_DEVICE	pDevice,
	IN unsigned int		dwIndex);
	
/** @} */ // end of logdisk_inf


/******************************************************************************/
/* Debug                                            */
/******************************************************************************/
#define NS_ENTER_FUNCTION			ENTER_FUNCTION
#define NS_EXIT_FUNCTION			EXIT_FUNCTION

/** @} */ // end of util
/** @} */ // end of ns_interface

/** 
 * @defgroup Rtl Rtl Interface
 * @brief Rtl interface
 * @{
 */

/******************************************************************************/
/* Lock                                                         */
/******************************************************************************/
/** 
 * @defgroup lock Lock Management
 * @brief	Lock Interface
 * @{
 */
#define NS_CreateSem					RtlCreateSemaphore
#define NS_LockSem						RtlGetSemaphore
#define NS_TrylockSem					RtlTryGetSemaphore
#define NS_UnlockSem					RtlPutSemaphore
#define NS_DestroySem					RtlDestroySemaphore
#define NS_CreateRwSem					RtlCreateRwSemaphore
#define NS_ReadLockSem					RtlGetReadSemaphore
#define NS_ReadUnlockSem				RtlPutReadSemaphore
#define NS_WriteLockSem					RtlGetWriteSemaphore
#define NS_WriteUnlockSem				RtlPutWriteSemaphore
#define NS_DestroyRwSem					RtlDestroyRwSemaphore
#define NS_AtomicIncrement				RtlAtomicIncrement
#define NS_AtomicDecrement				RtlAtomicDecrement


/** @brief NS_CreateSemaphore function creates and returns a semaphore object. */
unsigned int			
NS_CreateSem(
	IO NS_SEMAPHORE*	pSemaphore,
	IN unsigned int		dwCount);

/** @brief NS_DestroySemaphore function destroys the semaphore and frees it from the
 * system. */
void
NS_DestroySem(
	IN NS_SEMAPHORE*	pSemaphore);

#ifdef TFS5_DEBUG
/** @brief NS_GetSemaphore function gets an exclusive rights to the semaphore. */
unsigned int
NS_LockSem(
	IN NS_SEMAPHORE*	pSemaphore);

/** @brief NS_TryGetSemaphore function tries to get semaphore and returns immediately
 * regardless of whether or not the request can be satisfied. */
unsigned int
NS_TrylockSem(
	IN NS_SEMAPHORE*	pSemaphore);

/** @brief NS_PutSemaphore function releases the semaphore. */
unsigned int
NS_UnlockSem(
	IN NS_SEMAPHORE*	pSemaphore);
#endif

/** @brief NS_CreateRwSemaphore function creates and returns a semaphore object. */
unsigned int
NS_CreateRwSem(
	IN NS_RW_SEMAPHORE*	pRwSemaphore);

/** @brief NS_DestroyRwSemaphore function destroys the semaphore and frees it from the
 * system. */
void
NS_DestroyRwSem(
	IN NS_RW_SEMAPHORE*	pRwSemaphore);

/** @brief NS_GetReadSemaphore function gets an exclusive rights to the semaphore. */
void
NS_ReadLockSem(
	IN NS_RW_SEMAPHORE*	pRwSemaphore);

/** @brief NS_PutReadSemaphore function releases the semaphore. */
void
NS_ReadUnlockSem(
	IN NS_RW_SEMAPHORE*	pRwSemaphore);

/** @brief NS_GetWriteSemaphore function gets an exclusive rights to the semaphore. */
void
NS_WriteLockSem(
	IN NS_RW_SEMAPHORE*	pRwSemaphore);

/** @brief NS_PutWriteSemaphore function releases the semaphore. */
void
NS_WriteUnlockSem(
	IN NS_RW_SEMAPHORE*	pRwSemaphore);

/** @brief */
unsigned int
NS_AtomicIncrement(
	IN volatile unsigned int*	pAtomicInt);

/** @brief */
unsigned int
NS_AtomicDecrement(
	IN volatile unsigned int*	pAtomicInt);

//typedef volatile unsigned int	ATOMIC_INT;

/** @} */ // end of lock

/******************************************************************************/
/* Memory Management                                                         */
/******************************************************************************/
/** 
 * @defgroup memory Memory Management
 * @brief	Memory Management Interface
 * @{
 */
#define NS_AllocateMemory				RtlAllocMem
#define NS_AllocPossibleMemory			RtlAllocPossibleMemory
#define NS_FreeMemory					RtlFreeMem
#define NS_FillMemory					RtlFillMem
#define NS_CompareMemory				RtlCmpMem
#define NS_CopyMemory					RtlCopyMem


/** @brief		Allocate memory blocks 
*	@param[in]	dwSize		Number of bytes to allocate
*	@return		The pointer of memory to allocate
*/
void*
NS_AllocateMemory(
	IN	unsigned int	dwSize);

/** @brief		Allocate memory blocks allowing as much 
*	@param[in]	dwMaxSize	Maixmum size
*	@param[in]	dwMinSize	Minimum size
*	@param[in]	pdwSize		Size to be allocated
*/
void*
NS_AllocPossibleMemory(
	IN unsigned int		dwMaxSize, 
	IN unsigned int		dwMinSize, 
	OUT unsigned int*	pdwSize);

/** @brief		Deallocates or frees a memory block. 
*	@param[in]	pMemory		Previously allocated memory block to be freed.
*	@return		void
*/
void
NS_FreeMemory(
	IN void*			pMemory);

/** @brief		Sets buffers to a specified character. 
*	@param[out]	pDest	New Buffer
*	@param[in]	value	Buffer to copy from
*	@param[in]	dwBytes	Number of charaters to copy
*	@return		The value of dest
*/
void
NS_FillMemory(
	OUT	void*			pDest,
	IN	unsigned char	value,
	IN	unsigned int	dwBytes);

/** @brief		Compare characters in two buffers.
*	@param[in]	pBuf1	New Buffer
*	@param[in]	pBuf2	Buffer to copy from
*	@param[in]	dwBytes	Number of charaters to copy
*	@return		The return value indicates the relationship between the buffers.
*	@par output
*	@li		returned value < 0 : buf1 less than buf2
*	@li		returned value = 0 : buf1 identical to buf2
*	@li		returned value > 0 : buf1 greater than buf2
*/
int
NS_CompareMemory(
	IN const void*		pBuf1,
	IN const void*		pBuf2,
	IN unsigned int		dwBytes);

/** @brief		Copies bytes between buffers
*	@param[out]	pDest	New Buffer
*	@param[in]	pSrc	Buffer to copy from
*	@param[in]	dwBytes	Number of charaters to copy
*	@return		The value of dest
*/
void
NS_CopyMemory(
	IN void*			pDest,
	IN const void*		pSrc,
	IN unsigned int		dwBytes);


/** @} */ // end of memory

/******************************************************************************/
/* Calculations		                                                          */
/******************************************************************************/

#define	NS_IsPow2		RtlIsPow2
#define NS_Pow2			RtlPow2
#define NS_Log2			RtlLog2

/******************************************************************************/
/* Process                                                                    */
/******************************************************************************/
/** 
 * @defgroup process Process Management
 * @brief	Process Management Interface
 * @{
 */
typedef unsigned int (*PFN_NS_TASK_PROC)(void);

#define NS_GetCurTaskID		RtlGetCurrentTaskId
#define NS_Sleep			RtlSleep


/** @brief RtlGetCurrentTaskId function returns the Task Id 
*	@return	void
*/
unsigned int
NS_GetCurTaskID(void);

/** @brief		RtlSleep function suspends the current thread until the specified amount of time has elapsed. 
 *	@param[in]	dwMilliseconds	Number of Milliseconds
 *	@return		void
 */
void
NS_Sleep(
	IN unsigned int dwMilliseconds);

/** @} */ // end of process

/******************************************************************************/
/* Time                                                                       */
/******************************************************************************/
/** 
* @defgroup Time Time Management
* @brief Time Management Interface
* @{
*/

#define NS_GetCurSysTime				RtlGetCurSysTime
#define NS_GetOffsetFromUtc				RtlGetOffsetFromUtc

/** @brief		NS_GetCurSysTime function retrieves the current local date and time.
*	@param[out]	pTime	Pointer to a SYSTEMTIME structure to receive the current local date and time. 
*	@return		void
*/
void
NS_GetCurSysTime(
	OUT NS_PSYS_TIME			pTime);

/** @brief		NS_GetOffsetFromUtc function returns minutes offset from UTC (e.g. In case of Korea, +540 returns)
*	@return		Minutes offset from UTC
*/
int
NS_GetOffsetFromUtc(void);
/** @} */ // end of Time

/******************************************************************************/
/* ENDIAN                                                                     */
/******************************************************************************/
/** 
* @defgroup Endian Endian Management
* @brief Endian Management Interface
* @{
*/

/** @brief translate host endian type into endian type in a unsigned two-byte type */
#define	NS_BOU16(x)			BOU16(x)
/** @brief translate host endian type into endian type in a unsigend four-byte type */
#define	NS_BOU32(x)			BOU32(x)
/** @brief translate host endian type into endian type in a unsigend eight-byte type */
#define	NS_BOU64(x)			BOU64(x)

/** @brief translate host endian type into endian type in a signed two-byte type */
#define	NS_BO16(x)			BO16(x)	
/** @brief translate host endian type into endian type in a sigend four-byte type */
#define	NS_BO32(x)			BO32(x)	
/** @brief translate host endian type into endian type in a sigend eight-byte type */
#define	NS_BO64(x)			BO64(x)	

/** @} */ // end of Endian
/******************************************************************************/
/* Etc				                                                          */
/******************************************************************************/
/** 
 * @defgroup char Character Management
 * @brief Character Management Interface
 * @{
 */

#define NS_GetUpperWideChar				RtlGetUpperWideChar
#define NS_GetLowerWideChar				RtlGetLowerWideChar
#define NS_WcsLen						RtlWcsLen
#define NS_CompareStrCaseInsensitive	RtlCompareStringCaseInsensitive
#define NS_ConvertUpperString			RtlConvertUpperString
#define NS_ConvertLowerString			RtlConvertLowerString
#define NS_GetLastNumeric				RtlGetLastNumeric
#define NS_StrCpy						RtlStrCpy
#define NS_StrNCpy						RtlStrNCpy
#define NS_StrCmp						RtlStrCmp
#define NS_StrICmp						RtlStrICmp
#define NS_StrNCmp						RtlStrNCmp
#define NS_StrLen						RtlStrLen
#define NS_StrCat						RtlStrCat
#define NS_ToLower						RtlToLower
#define NS_ToUpper						RtlToUpper
#define NS_IsLeadByte					RtlIsLeadByte
#define NS_WcsCpy						RtlWcsCpy
#define NS_WcsNCpy						RtlWcsNCpy
#define NS_WcsCmp						RtlWcsCmp
#define NS_WcsNCmp						RtlWcsNCmp
#define NS_WcsICmp						RtlWcsICmp
#define NS_MathRand						RtlMathRand

#define NS_MbsToWcs						RtlMbsToWcs
#define NS_MbToWc						RtlMbToWc
#define NS_WcsToMbs						RtlWcsToMbs
#define NS_WcToMb						RtlWcToMb


/**
 * @brief		covert multibyte string to wide string
 * @param[in]	pVcb			volume control block
 * @param[in]	pMultiByteStr	multibyte string (NULL is unacceptable)
 * @param[in]	cbMultiByte		the byte size of multibyte string to scan
 * @param[out]	pWideCharStr	widechar string (NULL is unacceptable)
 * @param[in]	cchWideChar		the char count allocated to pWideCharStr
 * @return		positive on success. "(unsigned int) -1" on failure (invalid buffer or invalid character)
 */
unsigned int	NS_MbsToWcs(
	IN	NS_PVCB					pVcb,
	IN	const char*				pMultiByteStr,
	IN	unsigned int			cbMultiByte,
	OUT	wchar_t*				pWideCharStr,
	IN	unsigned int			cchWideChar);

/**
 * @brief		covert multibyte char to widechar char
 * @param[in]	pVcb			volume control block
 * @param[in]	pMultiByteChar	multibyte char (NULL is unacceptable)
 * @param[in]	cbMultiByte		the byte size of multibyte char
 * @param[out]	pWideCharChar	widechar char (NULL is unacceptable)
 * @return		1 or 2 on success. -1 on failure (invalid buffer or invalid character)
 */
int		NS_MbToWc(
	IN	NS_PVCB					pVcb,
	IN	const char*				pMultiByteChar,
	IN	unsigned int			cbMultiByte,
	OUT	wchar_t*				pWideCharChar);

/**
 * @brief		covert widechar string to multibyte string
 * @param[in]	pVcb			volume control block
 * @param[in]	pWideCharStr	widechar string (NULL is unacceptable)
 * @param[in]	cchWideChar		the char count of widechar string to scan
 * @param[out]	pMultiByteStr	multibyte string (NULL is unacceptable)
 * @param[in]	cbMultiByte		the byte size allocated to pMultiByteChar
 * @return		positive on success. "(unsigned int) -1" on failure (invalid buffer or invalid character)
 */
unsigned int	NS_WcsToMbs(
	IN	NS_PVCB					pVcb,
	IN	const wchar_t*			pWideCharStr, 
	IN	unsigned int			cchWideChar,
	OUT	char*					pMultiByteStr,
	IN	unsigned int			cbMultiByte);

/**
 * @brief		covert widechar char to multibyte char
 * @param[in]	pVcb			volume control block
 * @param[in]	WideCharChar	widechar char
 * @param[out]	pMultiByteChar	multibyte char (NULL is unacceptable)
 * @param[in]	cbMultiByte		the byte size allocated to pMultiByteChar
 * @return		1 or 2 on success. -1 on failure (invalid buffer or invalid character)
 */
int		NS_WcToMb(
	IN	NS_PVCB					pVcb,
	IN	wchar_t					WideCharChar,
	OUT	char*					pMultiByteChar,
	IN	unsigned int			cbMultiByte);

/**
 * @brief		get the uppercase wide char
 * @param[in]	wc		input wide char
 * @return		upppercase wide char
 */
wchar_t
NS_GetUpperWideChar(
	IN	wchar_t wc);

/**
 * @brief		get the lowercase wide char
 * @param[in]	wc		input wide char
 * @return		lowercase wide char
 */
wchar_t
NS_GetLowerWideChar(
	wchar_t wc);

/**
 * @brief		get the string length of wide char string
 * @param[in]	wcs		input wide char string
 * @return		length
 */
unsigned int
NS_WcsLen(
	IN	const wchar_t*	wcs);

/**
 * @brief		compare two wide string with case-insensitive for cchCompare length
 * @param[in]	pwszName1	input wide char string1
 * @param[in]	pwszName2	input wide char string2
 * @param[in]	cchCompare	the length to compare
 * @return		compare result
 */
unsigned int
NS_CompareStrCaseInsensitive(
	IN	const wchar_t*		pwszName1,
	IN	const wchar_t*		pwszName2,
	IN	unsigned int		cchCompare);

/**
 * @brief		get the Uppercase wide string
 * @param[out]	pwszTarget		result string
 * @param[in]	pwszSource		input string
 * @return		void
 */
void
NS_ConvertUpperString(
	OUT	wchar_t*		pwszTarget,
	IN	const wchar_t*	pwszSource);

/**
 * @brief		get the Lowercase wide string
 * @param[out]	pwszTarget		result string
 * @param[in]	pwszSource		input string
 * @return		void
 */
void
NS_ConvertLowerString(
	OUT	wchar_t*		pwszTarget,
	IN	const wchar_t*	pwszSource);

/**
 * @brief		get the Numeric number from the string
 * @param[in]	pwszSource		input string
 * @param[in]	cchSource		input string length
 * @param[out]	pdwCount		start position of numeric number
 * @return		numeric number
 */
int
NS_GetLastNumeric(
	IN	const wchar_t*	pwszSource,
	IN	unsigned int	cchSource,
	OUT	unsigned int*	pdwCount);

/**
 * @brief		copy string
 * @param[out]	psDest			dest string
 * @param[in]	psSrc			src string
 * @return		result
 */
char*
NS_StrCpy(
	OUT	char*			psDest,
	IN	const char*		psSrc);

/**
 * @brief		copy string with n bytes
 * @param[out]	psDest			dest string
 * @param[in]	psSrc			src string
 * @param[in]	nNumber			number of bytes to copy
 * @return		result
 */
char*
NS_StrNCpy(
	OUT	char*			psDest,
	IN	const char*		psSrc,
	IN	int				nNumber);

/**
 * @brief		compare string
 * @param[in]	pS1				string1
 * @param[in]	pS2				string2
 * @return		result
 */
int
NS_StrCmp(
	IN	const char*	pS1,
	IN	const char*	pS2);

/**
 * @brief		compare string in insensitive mode
 * @param[in]	pS1				string1
 * @param[in]	pS2				string2
 * @return		result
 */
int
NS_StrICmp(
	IN	const char*	pS1,
	IN	const char*	pS2);

/**
 * @brief		compare string with n bytes
 * @param[in]	pS1				string1
 * @param[in]	pS2				string2
 * @param[in]	nNumber			number of bytes to copy
 * @return		result
 */
int
NS_StrNCmp(
	IN	const char*	pS1,
	IN	const char*	pS2,
	IN	int			nNumber);

/**
 * @brief		get the string length
 * @param[in]	pS				string
 * @return		string length
 */
int
NS_StrLen(
	IN	const char*	pS);

/**
 * @brief		cat src string to dest
 * @param[out]	psDest			dest string
 * @param[in]	psSrc			src string
 * @return		result
 */
char*
NS_StrCat(
	OUT	char*			psDest,
	IN	const char*		psSrc);

/**
 * @brief		covert to lowercase char
 * @param[in]	c			char
 * @return		lowercase char
 */
int
NS_ToLower(
	IN	int	c);

/**
 * @brief		covert to uppercase char
 * @param[in]	c			char
 * @return		uppercase char
 */
int
NS_ToUpper(
	IN	int	c);

/**
 * @brief		check LeadByte
 * @param[in]	pVcb		volume control block
 * @param[in]	c			char
 * @return		result
 */
int
NS_IsLeadByte(
	IN	NS_PVCB		pVcb,
	IN	int	c);

/**
 * @brief		copy wide string
 * @param[out]	pwsDst			dest string
 * @param[in]	pwsSrc			src string
 * @return		dest string
 */
wchar_t*
NS_WcsCpy(
	OUT	wchar_t*		pwsDst,
	IN	const wchar_t*	pwsSrc);

/**
 * @brief		copy wide string with n bytes
 * @param[out]	pwsDst			dest string
 * @param[in]	pwsSrc			src string
 * @param[in]	cch				number of length to copy
 * @return		dest string
 */
wchar_t*
NS_WcsNCpy(
	OUT	wchar_t*		pwsDst,
	IN	const wchar_t*	pwsSrc,
	IN	unsigned int	cch);

/**
 * @brief		compare wide string 
 * @param[in]	wcs1			string1
 * @param[in]	wcs2			string2
 * @return		result
 */
int
NS_WcsCmp(
	IN	const wchar_t*	wcs1,
	IN	const wchar_t*	wcs2);

/**
 * @brief		compare wide string with n bytes
 * @param[in]	wcs1			string1
 * @param[in]	wcs2			string2
 * @param[in]	cch				number of length to compare
 * @return		result
 */
int
NS_WcsNCmp(
	IN	const wchar_t*	wcs1,
	IN	const wchar_t*	wcs2,
	IN	unsigned int	cch);

/**
 * @brief		compare wide string with case insensitive
 * @param[in]	wcs1			string1
 * @param[in]	wcs2			string2
 * @return		result
 */
int
NS_WcsICmp(
	IN	const wchar_t*	wcs1,
	IN	const wchar_t*	wcs2);

/**
* @brief	This function returns a random value
* @return	a random value
*/
int
NS_MathRand(void);

/******************************************************************************/
/* string																	  */
/******************************************************************************/

/**
* @brief	write to the character string
*/
#define NS_Vsnprintf	RtlVsnprintf

/** @} */ // end of char

/** @} */ // end of Rtl

/** @} */ // end of filesystem_interface

/** @} */ //NESTLE


/******************************************************************************/
/* GLUE DEPENDENT API                                                         */
/******************************************************************************/

#if defined(CONFIG_RTOS)
#endif /*CONFIG_RTOS*/

#if defined(CONFIG_WINCE)
#endif /*CONFIG_WINCE*/

#if defined(CONFIG_SYMBIAN)
#endif /*CONFIG_SYMBIAN*/

#if defined(CONFIG_LINUX)
#endif /*CONFIG_LINUX*/

#if defined(CONFIG_UNUSED)
#endif /*CONFIG_UNUSED*/

#ifdef __cplusplus
}
#endif


#endif /* __NESTLE_H__ */

// end of file
