/*
 * BTFS(Beyond The FAT fileSystem) Developed by Flash Software Group.
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
 * @file		ffat_types.h
 * @brief		The global configuration for FFAT
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @version		FEB-02-2009 [DongYoung Seo] Remove code for user fast seek
 * @see			None
 */

#ifndef _FFAT_TYPES_H_
#define _FFAT_TYPES_H_

// includes
#include "ess_types.h"
#include "ess_list.h"

#include "ffat_config.h"
#include "ffat_errno.h"

// debug begin
#include "ess_debug.h"
// debug end

// defines
#define FFAT_NODE_SIZE	(188/4)		/*!< for external / internal data type interchange */
#define FFAT_VOL_SIZE	(228/4)		/*!< for external / internal data type interchange */

#ifdef USE_64_BIT_ADDRESS
	#undef FFAT_NODE_SIZE
	#undef FFAT_VOL_SIZE
	#define FFAT_NODE_SIZE	(224/4)
	#define FFAT_VOL_SIZE	(288/4)
#endif

//!< ==================================================
//!< initialize readdir information structure
//!<	pRI		: readdir information structure pointer
//!<	psLFN	: LFN string storage
//!<	psSFN	: SFN string storage
//!<	wLenLFN	: LFN length
//!<	wLenSFN	: SFN length
//!<	pNS		: Node Status storage pointer
//!<	pFP		: File Pointer Information */
#define FFAT_INIT_RI(_pRI, _psLFN, _psSFN, _wLenLFN, _wLenSFN, _pNS, _pNode)	\
							do {	\
									(_pRI)->psLFN		= _psLFN;	\
									(_pRI)->psSFN		= _psSFN;	\
									(_pRI)->dwLenLFN	= _wLenLFN;	\
									(_pRI)->dwLenSFN	= _wLenSFN;	\
									(_pRI)->pNodeStatus	= _pNS;	\
									(_pRI)->pNode		= _pNode;	\
							} while(0)


//!< initializes FFAT Vectored Cluster
#define FFAT_INIT_VC(_pVC, _dwOffset)	do {	\
										(_pVC)->dwValidEntryCount = (_pVC)->dwTotalClusterCount = 0;	\
										(_pVC)->dwClusterOffset = _dwOffset;	\
									} while (0)


// defines
#define FFAT_IS_SUCCESS(_r)			(((_r) < 0) ? FFAT_FALSE : FFAT_TRUE)	// check result is success or not


//=============================================================================
//
//	enum
//

/* flags for Cache */
typedef signed int	FFatCacheFlag;
enum _FFatCacheFlag
{
	FFAT_CACHE_NONE			= 0x00000000,	/*!< no flag, do not change this value */

	/* Flag Cache State */
	FFAT_CACHE_FREE			= 0x00000000,	/*!< free entry, equal to FFAT_CACHE_NONE */
	FFAT_CACHE_DIRTY		= 0x00000001,	/*!< dirty state */
	FFAT_CACHE_LOCK			= 0x00000002,	/*!< locked state */

	/* Flag for Cache Operation */
	FFAT_CACHE_UNLOCK		= 0x00000004,	/*!< remove cache lock */
	FFAT_CACHE_SYNC			= 0x00000008,	/*!< flush */
	FFAT_CACHE_DIRECT_IO	= 0x00000010 | FFAT_CACHE_SYNC,
											/*!< direct IO to media, do not caching, align이 안 맞아도, 그만큼만 내림*/
											/*!< do not remove sync flag */
	FFAT_CACHE_NOREAD		= 0x00000020,	/*!< do not read from media. */

	FFAT_CACHE_DISCARD		= 0x00000040,	/*!< discard dirty cache after sync operation */
	FFAT_CACHE_FORCE		= 0x00000080,	/*!< do operation even if there are some error */
	FFAT_CACHE_SYNC_CALLBACK= 0x00000100,	/*!< invoke call back function before sync */

	/* Flag for Data */
	FFAT_CACHE_DATA_FAT		= 0x00000200,	/*!< meta data for FAT area */
	FFAT_CACHE_DATA_DE		= 0x00000400,	/*!< meta data for Directory Entry */
	FFAT_CACHE_DATA_FS		= 0x00000800,	/*!< other meta data except FAT/DE */
	FFAT_CACHE_DATA_META	= (FFAT_CACHE_DATA_FAT | FFAT_CACHE_DATA_DE | FFAT_CACHE_DATA_FS),
											/*!< metadata, (include root directory) FFAT_CACHE_DATA_FAT or FFAT_CACHE_DATA_DE */
	FFAT_CACHE_DATA_LOG		= 0x00001000,	/*!< data is log */
	FFAT_CACHE_DATA_HOT		= FFAT_CACHE_DATA_LOG,

	/*< internal purpose flags */
	FFAT_CACHE_META_IO		= FFAT_CACHE_DIRECT_IO,		//!< this is base I/O policy for meta-data
														//!< All meta-data i/O must have this flag

	/*!< do not use any other bits over '00100000'h,
	 the area is occupied by individual module */
	FFAT_CACHE_SHARED		= 0x00010000,	/*!< This entry is shared by several node */

	FFAT_CACHE_DUMMY		= 0x7FFFFFFF
};


typedef signed int	FFatChangeSizeFlag;
enum _FFatChangeSizeFlag
{
	FFAT_CHANGE_SIZE_NONE 				= 0x00,		/*!< no flag */
	FFAT_CHANGE_SIZE_NO_LOCK 			= 0x01,		/*!< lock volume */
	FFAT_CHANGE_SIZE_NO_INIT_CLUSTER	= 0x02,		/*!< do not initialize cluster */

	/*!< INTERNAL PURPOSE ONLY ==================================================== */
	FFAT_CHANGE_SIZE_FOR_CREATE			= 0x08,		/*!< update all time information */
	FFAT_CHANGE_SIZE_AVAILABLE			= 0x10,		/*!< in case of expanding size, expand available size */
													/*!< even though requested size is larger than free space */

	FFAT_CHANGE_SIZE_LAZY_SYNC			= 0x20,		/*!< this operation may be lazy synchronization */
													/*!< for lazy log writer */

	FFAT_CHANGE_SIZE_RECORD_DIRTY_SIZE		= 0x40,	/*!< Do not update size in DE even if cluster is allocated */
													/*!< Only dirty bit will be recorded in DE */
													/*!< After sync, the size will be updated */

	FFAT_CHANGE_SIZE_RECOVERY_DIRTY_SIZE	= 0x80,	/*!< Do not update size in DE even if cluster is allocated */


	FFAT_CHANGE_SIZE_DUMMY				= 0x7FFFFFFF
};


/*!< flags for FFAT_Create() */
typedef signed int	FFatCreateFlag;
enum _FFatCreateFlag
{
	/*!< do not use value between 0x00 and 0xFF, reserved for attribute */
	/*!< do not use value between 0x100 and 0xF00, reserved for name type */

	FFAT_CREATE_NONE			= 0x00000000,		/*!< no attribute */
	FFAT_CREATE_ATTR_RO			= 0x00000001,		/*!< read only (FAT_ATTR_RO) */
	FFAT_CREATE_ATTR_HIDDEN		= 0x00000002,		/*!< hidden (FAT_ATTR_HIDDEN) */
	FFAT_CREATE_ATTR_SYS		= 0x00000004,		/*!< system (FAT_ATTR_SYS) */
	FFAT_CREATE_ATTR_VOLUME		= 0x00000008,		/*!< volume (FAT_ATTR_VOLUME) */
	FFAT_CREATE_ATTR_LONG_NAME	=					/*!< long file name */
									FFAT_CREATE_ATTR_RO | FFAT_CREATE_ATTR_HIDDEN
									| FFAT_CREATE_ATTR_SYS | FFAT_CREATE_ATTR_VOLUME,
	FFAT_CREATE_ATTR_DIR		= 0x00000010,		/*!< directory (FAT_ATTR_DIR) */
	FFAT_CREATE_ATTR_ARCH		= 0x00000020,		/*!< archive (FAT_ATTR_ARCH) */
	FFAT_CREATE_ATTR_DIR_ARCH	= FFAT_CREATE_ATTR_DIR | FFAT_CREATE_ATTR_ARCH,

	FFAT_CREATE_ATTR_MASK		= FFAT_CREATE_ATTR_LONG_NAME | FFAT_CREATE_ATTR_DIR_ARCH,

	FFAT_CREATE_LOOKUP			= 0x00010000,		/*!< do lookup before create */
													/*!< lookup before create . */
													/*!< no need to set this flag after lookup */
	FFAT_CREATE_EXCL			= 0x00020000,		/*!< if the node is exist it returns FFAT_EXIST */

	FFAT_CREATE_FIFO			= 0x00040000,		/*!< create fifo node */
	FFAT_CREATE_SOCKET			= 0x00080000,		/*!< create socket node */

	/*!< INTERNAL PURPOSE ONLY */
	FFAT_CREATE_NO_LOCK			= 0x00100000,		/*!< do not lock */

	FFAT_CREATE_DUMMY			= 0x7FFFFFFF		/*!< to enlarge 4byte size */
};


/*!< flags for FFAT_Mount() */
typedef signed int	FFatMountFlag;
enum _FFatMountFlag
{
	FFAT_MOUNT_NO_FLAG				= 0x00000000,	/*!< no flag */
	FFAT_MOUNT_RDONLY				= 0x00000001,	/*!< comment "read-only */
	FFAT_MOUNT_FAT_MIRROR			= 0x00000002,	/*!< mirroring FAT*/
	FFAT_MOUNT_FORCE				= 0x00000004,	/*!< force mount or force unmount */
	FFAT_UMOUNT_FORCE				= FFAT_MOUNT_FORCE,
													/*!< force mount or force unmount */
													/*!< even if there is an error*/
	FFAT_MOUNT_SYNC_META				= 0x00000008,	/*!< sync all meta data */

	/*!< for ADDON Module , base is 0x00000100 */
	FFAT_MOUNT_NO_LOG				= 0x00000100,	/*!< do not use log*/
	FFAT_MOUNT_LOG_INIT				= 0x00000200,	/*!< init log */
	FFAT_MOUNT_LOG_LLW				= 0x00000400,	/*!< Lazy Log Writer DEFAULT Mount flag */
													/* It improves filesystem writer performance */
													/* several latest write operations may be disappeared when abnormal power off */
	FFAT_MOUNT_LOG_FULL_LLW			= 0x00000800,	/*!< use LLW scheme for all meta-data area */
													/* It improves filesystem overall performance */
													/* several latest operations may be disappeared when abnormal power off */
	FFAT_MOUNT_LOG_MASK				= (FFAT_MOUNT_LOG_INIT | FFAT_MOUNT_LOG_LLW
										| FFAT_MOUNT_LOG_FULL_LLW),
													/*!< flag mask for HPA */
	FFAT_MOUNT_CLEAN_NAND			= 0x00001000,	/*!< Clean all of the unused area on the volume */
												/*!< This feature is available when the block device has FFAT_DEV_ERASE flag */
	FFAT_MOUNT_HPA					= 0x00002000,	/*!< mount with HPA */
	FFAT_MOUNT_HPA_CREATE			= 0x00004000,	/*!< create HPA */
	FFAT_MOUNT_HPA_SHOW				= 0x00008000,	/*!< show HPA, it makes a HPA to a normal area */
	FFAT_MOUNT_HPA_REMOVE			= 0x00010000,	/*!< remove HPA, it removes all data on HPA */
	FFAT_MOUNT_HPA_MASK				= (FFAT_MOUNT_HPA | FFAT_MOUNT_HPA_CREATE
										| FFAT_MOUNT_HPA_SHOW | FFAT_MOUNT_HPA_REMOVE),
													/*!< flag mask for HPA */
	FFAT_MOUNT_ERASE_SECTOR			= 0x00020000,	/*!< request block erase to block device while cluster deallocation */

	FFAT_MOUNT_XDE					= 0x00040000,	/*!< (FAT Incompatibility) support owner/group id, permission */
													/*!<	don't use this flag on volumes exposed to external */
													/*!<	can't guarantee compatibility */
	FFAT_MOUNT_XATTR				= 0x00080000,	/*!< (FAT Incompatibility) support extended attributes */
													/*!<	do not use this flag on volumes exposed to external */
													/*!<	can't guarantee compatibility */
	FFAT_MOUNT_SPECIAL_FILES		= 0x00100000,	/*!< (FAT partial Incompatibility) support special files */
													/*!<	(symbolic link, fifo, socket) */
													/*!<	Special files created internally can be mis-understood */
													/*!<	as normal file by external FAT file system */

	FFAT_MOUNT_CASE_SENSITIVE		= 0x00200000,	/*!< (FAT Incompatibility) Support naming rule of case sensitive */
	FFAT_MOUNT_OS_SPECIFIC_CHAR		= 0x00400000,	/*!< (FAT Incompatibility) Support naming rule of bad character set which is defined by OS */
													/*!< and Tail space and dot can be allowed if these are allowed in OS naming rule */

	FFAT_MOUNT_HPA_NO_CHECK			= 0x00800000,	/*!< when there is no HPA flag, don't check HPA on Mounting */
													/*!< this is temporary code used by RFS tools */
													/*!< if filenand support lseek for 64bit, this should be removed */

	FFAT_MOUNT_LOG_REPLAY			= 0x01000000,	/*!< restore log from backup area */

	FFAT_MOUNT_DUMMY				= 0x7FFFFFFF	/*!< to enlarge 4byte size */
};


// attribute of node
typedef signed int	FFatNodeAttr;
enum _FFatNodeAttr
{
	FFAT_ATTR_RO			= 0x01,		/*!< read only */
	FFAT_ATTR_HIDDEN		= 0x02,		/*!< hidden */
	FFAT_ATTR_SYS			= 0x04,		/*!<  system */
	FFAT_ATTR_VOLUME		= 0x08,		/*!< volume */
	FFAT_ATTR_LONG_NAME		= FFAT_ATTR_RO | FFAT_ATTR_HIDDEN 
								| FFAT_ATTR_SYS | FFAT_ATTR_VOLUME,
										/*!< long file name */
	FFAT_ATTR_DIR			= 0x10,		/*!<directory */
	FFAT_ATTR_ARCH			= 0x20,		/*!<  archive */
	FFAT_ATTR_DIR_ARCH		= FFAT_ATTR_DIR | FFAT_ATTR_ARCH,
										/*!< archive for directory */
	FFAT_ATTR_LONG_NAME_MASK = FFAT_ATTR_LONG_NAME | FFAT_ATTR_DIR_ARCH,

#ifndef FFAT_NO_CHECK_DIRECTORY_ENTRY_ATTR
	FFAT_ATTR_MASK			= FFAT_ATTR_LONG_NAME | FFAT_ATTR_DIR_ARCH,
#else
	FFAT_ATTR_MASK			= 0xFF,
#endif

	FFAT_ATTR_DUMMY			= 0x7FFFFFFF
};

/*! flags for Device */
typedef signed int	FFatDevFlag;
enum _FFatDevFlag
{
	FFAT_DEV_FLAG_NONE		= 0x00000000,	/*!< no flag */
	FFAT_DEV_FAT_CLEANING 	= 0x00000001,	/*!< cleaning NAND */
	FFAT_DEV_REMOVABLE		= 0x00000002,	/*!< removable device */
	FFAT_DEV_ERASE			= 0x00000004,	/*!< device support sector erase operation */

	FFAT_DEV_DUMMY			= 0x7FFFFFFF	/*!< removable device */
};

typedef signed int	FFatFatType;
enum _FFatFatType
{
	FFAT_FAT_UNKNOWN= 0x00,		/*!< unknown FAT type */
	FFAT_FAT12		= 0x01,		/*!< FAT12 */
	FFAT_FAT16		= 0x02,		/*!< FAT16 */
	FFAT_FAT32		= 0x04,		/*!< FAT32 */

	FFAT_FAT_DUMMY	= 0x7FFFFFFF
};

/*!< for lock */
typedef signed int	FFatLockFlag;
enum _FFatLockFlag
{
	LOCK_PRIOR		= 0x01,		/*!< priority */
	LOCK_FIFO		= 0x02,		/*!< serialize */
	LOCK_SUSPEND	= 0x04,		/*!< suspend if the lock is not available */
	LOCK_NO_SUSPEND	= 0x08		/*!< do not suspend */
};

/*!< flags for lookup */
typedef signed int	FFatLookupFlag;
enum _FFatLookupFlag
{
	FFAT_LOOKUP_NONE		= 0x0000,	/*!< lookup for node open */
										/*!< FFAT assign a lock for this operation */
	FFAT_LOOKUP_FOR_CREATE	= 0x0001,	/*!< lookup for node creation or rename */
										/*!< it gather free entry information , for log recovery */
	FFAT_LOOKUP_FOR_RENAME	= 0x0002,	/*!< lookup for rename operation */
	FFAT_LOOKUP_SET_CHILD	= 0x0004,	/*!< after lookup, set child node pointer of parent node */
	FFAT_LOOKUP_NO_LOCK		= 0x0008,	/*!< internal purpose */
										/*!< do not lock filesystem */
										/*!< do not use this flag */
	FFAT_LOOKUP_FREE_DE		= 0x0010,	/*!< just lookup for free directory entry */
										/*!< only for log recovery and rename */
	FFAT_LOOKUP_DUMMY		= 0x7FFFFFFF	/*!<to enlarge 4byte size */
};

/*!< directory attribute */
typedef signed int	FFatNodeFlag;
enum _FFatNodeFlag
{
	FFAT_NODE_RO			= FFAT_ATTR_RO,			/*!< read only */
	FFAT_NODE_HIDDEN		= FFAT_ATTR_HIDDEN,		/*!< hidden */
	FFAT_NODE_SYS			= FFAT_ATTR_SYS,		/*!<  system */
	FFAT_NODE_VOLUME		= FFAT_ATTR_VOLUME,		/*!< volume */
	FFAT_NODE_DIR			= FFAT_ATTR_DIR,		/*!< directory */
	FFAT_NODE_ARCH			= FFAT_ATTR_ARCH,		/*!< archive */

	// NOTICE!! DO NOT USE UNDER 0xFF
#if 0
	/*!< node open flag */
	FFAT_NODE_OPEN_DIRECT_IO		= 0x00000100, /*!< open with direct IO */
	FFAT_NODE_OPEN_NO_INIT_CLUSTER 	= 0x00000200, /*!< do not initialize cluster to 0x00 */
	FFAT_NODE_OPEN_IO_RANDOM		= 0x00000400, /*!< RANDOM IO, cache cluster chain */
	FFAT_NODE_OPEN_IO_SEQ			= 0x00000800, /*!< comment "sequential IO, read ahead */
	FFAT_NODE_OPEN_IO_NOREUSE		= 0x00001000, /*!< do not reused read data, no caching */
#endif

	FFAT_NODE_SYMLINK		= 0x00002000,
	FFAT_NODE_FIFO			= 0x00004000,
	FFAT_NODE_SOCKET		= 0x00008000,

	FFAT_NODE_SPECIAL_FILES	= FFAT_NODE_SYMLINK | FFAT_NODE_FIFO | FFAT_NODE_SOCKET,

	FFAT_NODE_DUMMY			= 0x7FFFFFFF
};


typedef signed int	FFatNodeCloseFlag;
enum _FFatNodeCloseFlag
{
	FFAT_NODE_CLOSE_NONE				= 0x00,		//!< no flag
	FFAT_NODE_CLOSE_SYNC				= 0x01,		//!< sync data
//	FFAT_NODE_CLOSE_NO_LOG				= 0x02,		//!< do not write close log
	FFAT_NODE_CLOSE_DEC_REFERENCE		= 0x04,		//!< decrease reference count
	FFAT_NODE_CLOSE_RELEASE_RESOURCE	= 0x08,		//!< release all resource for node.
													//!< this flag is used for FFatNode Destroy
	FFAT_NODE_CLOSE_DUMMY				= 0x7FFFFFFF
};


/*!< File System Control command */
typedef signed int	FFatFSCtlCmd;
enum _FFatFSCtlCmd
{
	/*!< base module */
	FFAT_FSCTL_BASE					= 0x00001000,	/*!< base of FACtl command */
	FFAT_FSCTL_VOL_IS_FAT32			= 0x00001001,	/*!< check volume is FAT32 or not */
													/*!< pParam0 : FFatVol pointer, pParam1,2 : don't care*/
	FFAT_FSCTL_GET_ROOT_CLUSTER_NO	= 0x00001002,	/*!< get root cluster number from vol */
													/*!< pParam0 : FFatVol pointer */
													/*!< pParam1 : cluster number storage, t_uint32 pointer */
													/*!< pParam2 : don't care*/
	FFAT_FSCTL_GET_ROOT				= 0x00001003,	/*!< get root directory from volume information */
	FFAT_FSCTL_SYNC_VOL				= 0x00001004,	/*!< sync a volume */
	FFAT_FSCTL_IS_VALID_BOOTSECTOR	= 0x00001005,	/*!< check pParam0 is valid boot sector or not */
	FFAT_FSCTL_ADD_CACHE			= 0x00001006,	/*!< add new cache for FFATFS */
	FFAT_FSCTL_REMOVE_CACHE			= 0x00001007,	/*!< remove cache for FFATFS */
	FFAT_FSCTL_CHK_CACHE			= 0x00001008,	/*!< check exist of cache for particular sector size for FFATFS */
													/*!<	return FFAT_OK : when there is no cache for the size */
													/*!<	return FFAT_OK1 : when there a cache for the size, and fill pointer of buffer */

	FFAT_FSCTL_LDEV_IO				= 0x00001009,	/*!< Logical Device sector IO */

	FFAT_FSCTL_CORE_END				= FFAT_FSCTL_LDEV_IO,
	FFAT_FSCTL_CORE_MASK			= 0x00000FFF,	/*!< mask for fsctl core */

	/*!< ADDON module */
	FFAT_FSCTL_ADDON_BASE			= 0x00002000,	/*!< base value of ADDON modUle */
	FFAT_FSCTL_CLEAN_NAND			= 0x00002001,	/*!< NAND cleaning */
	FFAT_FSCTL_READDIR_UNLINK		= 0x00002002,	/*!< unlink node while readdir */
	FFAT_FSCTL_READDIR_GET_NODE		= 0x00002003,	/*!< unlink node while readdir */
													/*!< CAUTION: Must be released resource of node after using */
	FFAT_FSCTL_READDIR_STAT			= 0x00002004,	/*!< get status while readdir */ 
	FFAT_FSCTL_DIR_ENTRY_CACHE		= 0x00002005,	/*!< directory entry cache */
	FFAT_FSCTL_FORMAT				= 0x00002006,	/*!< format a volume */
	FFAT_FSCTL_CLEAN_DIR			= 0x00002007,	/*!< delete all file in a directory */
	FFAT_FSCTL_CHKDSK				= 0x00002008,	/*!< check disk */
	FFAT_FSCTL_GET_SHORT_NAME		= 0x00002009,	/*!< get short file name from long file name */
	FFAT_FSCTL_GET_LONG_NAME		= 0x0000200A,	/*!< get long file name from short file name */
	FFAT_FSCTL_SET_GUID_PERM		= 0x0000200B,	/*!< set user/group id, permission */
	FFAT_FSCTL_EXTENDED_ATTRIBUTE	= 0x0000200C,	/*!< extended attribute set, get, delete */

	FFAT_FSCTL_ADDON_END			= FFAT_FSCTL_EXTENDED_ATTRIBUTE,		/*!< the last command for ADDON */

// debug begin
#ifdef FFAT_DEBUG
	//!< debug purpose for core module
	FFAT_FSCTL_DEBUG_BASE			= 0x00004000,		//!< base value for debug command 
	FFAT_FSCTL_PRINT_FAT_CAHIN		= 0x00004001,		//!< print fat chain 
	FFAT_FSCTL_GET_CLUSTERS_FOR_NODE= 0x00004002,		//!< get clusters for a node 
	FFAT_FSCTL_LOG_RECOVERY_TEST	= 0x00004003,		//!< for log recovery test
	FFAT_FSCTL_INVALIDATE_FCCH		= 0x00004004,		//!< invalidate free cluster count hint */
	FFAT_FSCTL_DEBUG_REMOUNT		= 0x00004005,		//!< for remount unit test

	//!< debug purpose for ADDON module
	FFAT_FSCTL_ADDON_DEBUG_BASE		= 0x00008000,		//!< base value for ADDON module debug command
	FFAT_FSCTL_FREE_CLUSTER_CACHE	= 0x00008001,		//!< free cluster cache*/
#endif
// debug end

	FFAT_FSCTL_DUMMY	= 0x7FFFFFFF
};

typedef signed int	FFatRenameFlag;
enum _FFatRenameFlag
{
	FFAT_RENAME_NONE			= 0x0000,
	FFAT_RENAME_LOOKUP			= 0x0001,		/*!< lookup target node */
	FFAT_RENAME_EXCL			= 0x0002,		/*!< rename if target does not exist */
	FFAT_RENAME_TARGET_OPENED	= 0x0004,		/*!< target node is in opened state */
	FFAT_RENAME_NO_LOCK			= 0x0008,		/*!< do not lock, internal purpose */

	FFAT_RENAME_DUMMY		=0x7FFFFFFF
};

typedef signed int	FFatReadFlag;
enum _FFatReadFlag
{
	FFAT_READ_NONE				= 0x00,		/*!< no flag */
	FFAT_READ_UPDATE_ADATE		= 0x01,		/*!< update access date */
	FFAT_READ_DIRECT_IO			= 0x02,

	FFAT_READ_DUMMY		= 0x7FFFFFFF
};

typedef signed int	FFatWriteFlag;
enum _FFatWriteFlag
{
	FFAT_WRITE_NONE				= 0x00,		/*!< no flag */
	FFAT_WRITE_NO_META_UPDATE	= 0x01,		/*!< update file data only. (no DE update) */
	FFAT_WRITE_SYNC				= 0x02,		/*!< synchronize data */
	FFAT_WRITE_DIRECT_IO		= 0x04,		/*!< do not cache data */
	FFAT_WRITE_NO_INIT_CLUSTER	= 0x08,		/*!< do not initialize cluster */

	/* internal purpose only */
	FFAT_WRITE_NO_LOCK			= 0x10,		/*!< do not lock node and volume, internal purpose */

	FFAT_WRITE_RECORD_DIRTY_SIZE	= 0x20,	/*!< Do not update size in DE even if cluster is allocated */
											/*!< Only dirty bit will be recorded in DE */
											/*!< After sync, the size will be updated */

	FFAT_WRITE_DUMMY	= 0x7FFFFFFF
};

// do not modify value of FFatIOType
// do not use or operation with this type !!
typedef signed int	FFatIOType;
enum _FFatIOType
{
	FFAT_IO_READ_SECTOR		=0x00,		/*!< read sector */
	FFAT_IO_WRITE_SECTOR	=0x01,		/*!< write sector */
	FFAT_IO_ERASE_SECTOR	=0x02,		/*!< erase sector */

	FFAT_LDEVIO_DUMMY	= 0x7FFFFFFF
};

// read/write/init flag
typedef signed int	FFatRWFlag;
enum _FFatRWFlag
{
	FFAT_RW_NONE		= 0x00,		/*!< no flag */
	FFAT_RW_READ		= 0x01,		/*!< read operation */
	FFAT_RW_WRITE		= 0x02,		/*!< write operation */
	FFAT_RW_INIT		= 0x04,		/*!< init operation */

	FFAT_RW_DUMMY		= 0x7FFFFFFF
};

//=============================================================================
//
//	type definitions
//

typedef void		FFatLock;			/*!< lock type definition */


typedef struct _FFatCacheInfo
{
	void*	pNode;	/*!< node pointer, it may be NULL */
	void*	pDev;	/*!< pointer for pointing device */
					/*!< device ID for FSM */
					/*!< VCB pointer for Nestle */
	void*	pPage;	/*!< buffer cache page pointer */
} FFatCacheInfo;


typedef struct _FFatNode
{
	t_int32		dwords[FFAT_NODE_SIZE];
} FFatNode;


typedef struct _FFatVol
{
	t_int32		dwords[FFAT_VOL_SIZE];
} FFatVol;


/*!< logical device information */
typedef struct _FFatLDevInfo
{
	FFatDevFlag		dwFlag;					/*!< device flag */
	t_uint16		wAlignBasis;			/*!< sector align count */
											/*!< if 0 , do not align */
	t_uint16		wDummy;					/*!< dummy for align */
	t_int32			dwDevSectorSize;		/*!< device sector size in bytes (not FAT sector) */
	t_int32			dwDevSectorSizeBits;	/*!< sector size in bit count */
	t_uint32		dwDevStartSector;		/*!< partition start sector number */
	t_uint32		dwDevSectorCount;		/*!< total sector count of device */

	t_uint32		dwStartBlock;			/*!< partition start block number */
	t_uint32		dwBlockCount;			/*!< total block count of device */
	t_int32			dwBlockSize;			/*!< block size in bytes */
	t_int32			dwBlockSizeBits;		/*!< block size in bit count */

	t_int32			dwFATSectorPerBlock;		/*!< FAT sector count per one block */
												/*!<  or block count per one FAT sector */
												/*!<     in this case value is negative */
												/*!< block must be always larger than FAT sector after mount */
												/*!< but block may be smaller than FAT sector before or during mount */
	t_int32			dwFATSectorPerBlockBits;	/*!< FAT sector count per one block in bit count */
												/*!<  or block count per one FAT sector in bit count */
	t_int32			dwFATSectorPerBlockMask;	/*!< FAT sector count per one block mask */
												/*!<  or block count per one FAT sector mask */

	t_int32			dwBlockPerCluster;		/*!< block count per one cluster */
	t_int32			dwBlockPerClusterBits;	/*!< block count per one cluster in bit count */
	t_int32			dwBlockPerClusterMask;	/*!< block count per one cluster mask */

} FFatLDevInfo;


/*!< Logical Device Sector IO */
typedef struct 
{
	FFatIOType	dwFlag;			/*!< LDEV IO Type */
	t_uint32	dwSectorNo;		/*!< start sector number */
	t_int32		dwCount;		/*!< sector count to read or to write */
	t_int8		*pBuff;			/*!< buffer pointer for storing read data or for writing buffer */
} FFatLDevIO;


/*!< Node Status */
typedef struct _FFatNodeStatus
{
	FFatNodeFlag	dwAttr;			/*!< node flag */
	t_uint32		dwCluster;		/*!< node cluster number */
	t_uint32		dwIno1;			/*!< INODE number1 (cluster number for directory entry) */
									/*!< notice!! this is not a parent first cluster !! */
	t_uint32		dwIno2;			/*!< INODE number2 (DE offset from begin of parent) */
	t_uint32		dwSize;			/*!< the file size in byte */
	t_uint32		dwATime;		/*!< time of last access */
	t_uint32		dwMTime;		/*!< time of last data modification */
	t_uint32		dwCTime;		/*!< time file was created */ 
	t_uint32		dwCTimeTenth;	/*!< 10 milliseconds (0.01sec) stamp of creation time */
									/*!< The granularity of dwCTime is 2 seconds */
									/*!< so the range of this field is 0-199 */
	t_uint32		dwAllocSize;	/*!< allocated size in byte */
} FFatNodeStatus;


/*!< time  */
typedef EssTime			FFatTime;


typedef struct
{
	// status for HPA
	t_uint32	dwClusterCount;			/*!< total cluster count for HPA */
} FFatVolumeStatusHPA;


typedef struct _FFatVolumeStatus
{
	FFatFatType	dwFatType;				/*!< filesystem type */
	t_uint32	dwClusterSize;			/*!< cluster size */
	t_uint32	dwClusterSizeBits;		/*!< cluster size in bit count */
	t_uint32	dwClusterCount;			/*!< Total cluster count */
	t_uint32	dwFreeClusterCount;		/*!< Total free cluster count */
										/*!< free cluster for (Normal + HPA) */
	t_uint32	dwMaxFileSize;			/*!< Maximum file size */
	t_uint16	dwMaxNameLen;			/*!< Maximum file name character count */

	FFatVolumeStatusHPA	stHPA;			/*!< status of HPA */
} FFatVolumeStatus;


// vectored cluster entry
typedef struct _FFatVCE
{
	t_uint32		dwCluster;			/*!< cluster number */
	t_uint32		dwCount;			/*!< contiguous cluster count */
} FFatVCE;


// structure for Vectored Cluster information
typedef struct _FFatVC
{
	t_int32		dwTotalEntryCount;		/*!< total entry count on pVCE */
	t_uint32	dwClusterOffset;		/*!< first cluster (in first entry) byte offset*/
										/*!< if not cluster aligned ==> no valid offset*/
										/*!< must be aligned to cluster size */
	t_uint32	dwTotalClusterCount;	/*!< total cluster count in pVCE */
	t_int32		dwValidEntryCount;		/*!< valid entry count */
										/*!< 0 : there is no data */
	FFatVCE*	pVCE;					/*!< vectored cluster storage */
} FFatVC;


// Vectored Sector entry
/*
	Caution !!
	Do not modify this structure. 
	This structure will be type casted to nestle type.
*/
typedef struct
{
	t_uint32		dwSector;			/*!< sector number */
	t_uint32		dwCount;			/*!< contiguous sector count */
	t_int8*			pBuff;				/*!< buffer pointer */
} FFatVSE;


// structure for Vectored Sector information
/*
	Caution !!
	Do not modify this structure. 
	This structure will be type casted to nestle type.
*/
typedef struct
{
	t_int32		dwTotalEntryCount;		/*!<  total entry count on pVSE */
	t_int32		dwValidEntryCount;		/*!<  valid entry count */
										/*!<  0 : there is no data */
	FFatVSE*	pVSE;					/*!<  vectored sector storage */
} FFatVS;


typedef struct _FFatAddCache
{
	t_int8*			pBuff;				/*! buffer pointer */
	t_int32			dwSize;				/*! buffer size */
	t_int32			dwSectorSize;		/*! sector size descriptor */
										/*! this buffer will be used for device 
											that sector size is dwSectorSize */
} FFatAddCache, FFatRemoveCache, FFatCheckCache;


typedef struct
{
	t_wchar			psFileName[FFAT_LOG_FILE_NAME_MAX_LENGTH];	/*!< Log file name*/
} FFatLogConfig;


typedef struct
{
	t_wchar		psHPARootName[FFAT_HPA_ROOT_NAME_MAX_LENGTH];
												/*!< root directory name for HPA */
	t_int16		wHPAVolCount;					/*!< FFAT_HPA_MAX_VOLUME_COUNT */
	t_int16		wHPABitmapSize;					/*!< FFAT_HPA_BITMAP_SIZE */
} FFatHPAConfig;


/*!< FFat common information */
typedef struct _FFatConfig
{
	/*!< for FFAT */
	t_int32		dwSectorSize;				/*!< default sector size */

	/*!< for FFATFS */
	t_int32		dwFFatfsCacheSize;			/*!< size of meta-data cache in byte */

	/*! for addon module */
	FFatLogConfig	stLog;					/*!< for log recovery module */
	FFatHPAConfig	stHPA;					/*!< for Hidden Protected Area */
} FFatConfig;

// debug begin

//=============================================================================
//
// debug part
//

#ifdef FFAT_DEBUG

	typedef signed int	FFatLogRecoveryTestFlag;
	enum _FFatLogRecoveryTestFlag
	{
		FFAT_LRT_NONE					= 0x00000000,	/* No LRT flag */
		FFAT_LRT_SET_VOLUME_NAME_BEFORE,				/* UNDO, before volume name change */
		FFAT_LRT_SET_VOLUME_NAME_AFTER,					/* REDO, before volume name change */

		FFAT_LRT_CREATE_EXPAND_PARENT_BEFORE,			/* UNDO, Parent expand before */
		FFAT_LRT_CREATE_EXPAND_PARENT_AFTER,			/* UNDO, Parent expand after */
		FFAT_LRT_CREATE_DE_UPDATE_BEFORE,				/* UNDO(new), REDO(re-create), before directory entry write */
		FFAT_LRT_CREATE_DE_UPDATE_AFTER,				/* UNDO(new), REDO(re-create), after directory entry write */

		FFAT_LRT_MKDIR_EXPAND_PARENT_BEFORE,			/* UNDO, Parent expand before */
		FFAT_LRT_MKDIR_EXPAND_PARENT_AFTER,				/* UNDO, Parent expand after */
		FFAT_LRT_MKDIR_GET_FREE_CLUSTER_BEFORE,			/* UNDO, before get Free Cluster for directory */
		FFAT_LRT_MKDIR_GET_FREE_CLUSTER_AFTER,			/* UNDO, after get Free Cluster for directory */
		FFAT_LRT_MKDIR_DE_UPDATE_BEFORE,				/* UNDO, before directory entry write */
		FFAT_LRT_MKDIR_DE_UPDATE_AFTER,					/* UNDO, after directory entry write */

		FFAT_LRT_RMDIR_DEALLOCATE_CLUSTER_BEFORE,		/* REDO, before deallocate clusters */
		FFAT_LRT_RMDIR_DEALLOCATE_CLUSTER_AFTER,		/* REDO, after deallocate clusters */	
		FFAT_LRT_RMDIR_DELETE_DE_BEFORE,				/* REDO, before delete de */
		FFAT_LRT_RMDIR_DELETE_DE_AFTER,					/* REDO, after delete de */

		FFAT_LRT_WRITE_ALLOCATE_CLUSTER_BEFORE,			/* UNDO, before cluster allocation */
		FFAT_LRT_WRITE_ALLOCATE_CLUSTER_AFTER,			/* UNDO, after cluster allocation */
		FFAT_LRT_WRITE_DATA_WRITE_BEFORE,				/* UNDO, before user data write */
		FFAT_LRT_WRITE_DATA_WRITE_AFTER,				/* UNDO, after data write */
		FFAT_LRT_WRITE_DE_UPDATE_BEFORE,				/* UNDO, before de update */
		FFAT_LRT_WRITE_DE_UPDATE_AFTER,					/* UNDO, after de update */

		FFAT_LRT_SET_STATUS_DE_UPDATE_BEFORE,			/* REDO, before de update */
		FFAT_LRT_SET_STATUS_DE_UPDATE_AFTER,			/* REDO, after de update */

		FFAT_LRT_RENAME_EXPAND_PARENT_BEFORE,			/* UNDO, before expand parent node */
		FFAT_LRT_RENAME_EXPAND_PARENT_AFTER,			/* UNDO, after expand parent node */
		FFAT_LRT_RENAME_DELETE_SRC_DE_BEFORE,			/* REDO, before delete source DE */
		FFAT_LRT_RENAME_DELETE_SRC_DE_AFTER,			/* REDO, after delete source DE */
		FFAT_LRT_RENAME_DELETE_DES_DE_BEFORE,			/* REDO, before delete destination DE */
		FFAT_LRT_RENAME_DELETE_DES_DE_AFTER,			/* REDO, after delete destination DE */
		FFAT_LRT_RENAME_WRITE_NEW_DE_BEFORE,			/* REDO, before write new DE */
		FFAT_LRT_RENAME_WRITE_NEW_DE_AFTER,				/* REDO, after write new DE */
		FFAT_LRT_RENAME_UPDATE_DOTDOT_BEFORE,			/* REDO, dir rename only, .. update before */
		FFAT_LRT_RENAME_UPDATE_DOTDOT_AFTER,			/* REDO, dir rename only, .. update after  */

		FFAT_LRT_TRUNCATE_EXTEND_ALLOCATE_CLUSTER_BEFORE,	/* UNDO, before cluster allocation */
		FFAT_LRT_TRUNCATE_EXTEND_ALLOCATE_CLUSTER_AFTER,	/* UNDO, after cluster allocation */
		FFAT_LRT_TRUNCATE_SHRINK_DEALLOCATE_CLUSTER_BEFORE,	/* REDO, before cluster deallocation */
		FFAT_LRT_TRUNCATE_SHRINK_DEALLOCATE_CLUSTER_AFTER,	/* REDO, after cluster deallocation */
		FFAT_LRT_TRUNCATE_UPDATE_DE_BEFORE,					/* UNDO(extend), REDO(shrink), before DE update */
		FFAT_LRT_TRUNCATE_UPDATE_DE_AFTER,					/* UNDO(extend), REDO(shrink), after DE update */

		FFAT_LRT_UNLINK_DEALLOCATE_CLUSTER_BEFORE,			/* REDO, before deallocate clusters */
		FFAT_LRT_UNLINK_DEALLOCATE_CLUSTER_AFTER,			/* REDO, after deallocate clusters */	
		FFAT_LRT_UNLINK_DELETE_DE_BEFORE,					/* REDO, before delete de */
		FFAT_LRT_UNLINK_DELETE_DE_AFTER,					/* REDO, after delete de */

		FFAT_LRT_CREATE_EA_MAKE_CHAIN_BEFORE,				/* UNDO, before make chain */
		FFAT_LRT_CREATE_EA_MAKE_CHAIN_AFTER,				/* UNDO, after make chain */
		FFAT_LRT_CREATE_EA_WRITE_MAIN_BEFORE,				/* UNDO, before write main */
		FFAT_LRT_CREATE_EA_WRITE_MAIN_AFTER,				/* UNDO, after write main */
		FFAT_LRT_CREATE_EA_WRITE_ENTRY_BEFORE,				/* UNDO, before write entry */
		FFAT_LRT_CREATE_EA_WRITE_ENTRY_AFTER,				/* UNDO, after write entry */
		FFAT_LRT_CREATE_EA_DE_UPDATE_BEFORE,				/* UNDO, before de update */
		FFAT_LRT_CREATE_EA_DE_UPDATE_AFTER,					/* UNDO, after de update */

		FFAT_LRT_SET_EA_MAKE_CHAIN_BEFORE,					/* UNDO, before make chain */
		FFAT_LRT_SET_EA_MAKE_CHAIN_AFTER,					/* UNDO, after make chain */
		FFAT_LRT_SET_EA_MARK_DELETE_BEFORE,					/* UNDO, before mark delete */
		FFAT_LRT_SET_EA_MARK_DELETE_AFTER,					/* UNDO, after mark delete */
		FFAT_LRT_SET_EA_WRITE_ENTRY_BEFORE,					/* UNDO, before write entry */
		FFAT_LRT_SET_EA_WRITE_ENTRY_AFTER,					/* UNDO, after write entry */
		FFAT_LRT_SET_EA_WRITE_MAIN_BEFORE,					/* UNDO, before write main */
		FFAT_LRT_SET_EA_WRITE_MAIN_AFTER,					/* UNDO, after write main */

		FFAT_LRT_COMPACT_EA_MAKE_CHAIN_BEFORE,				/* UNDO, before make chain */
		FFAT_LRT_COMPACT_EA_MAKE_CHAIN_AFTER,				/* UNDO, after make chain */
		FFAT_LRT_COMPACT_EA_DATA_WRITE_BEFORE,				/* UNDO, before user data write */
		FFAT_LRT_COMPACT_EA_DATA_WRITE_AFTER,				/* UNDO, after data write */
		FFAT_LRT_COMPACT_EA_DE_UPDATE_BEFORE,				/* UNDO, before de update */
		FFAT_LRT_COMPACT_EA_DE_UPDATE_AFTER,				/* UNDO, after de update */
		FFAT_LRT_COMPACT_EA_DEALLOCATE_CLUSTER_BEFORE,		/* UNDO, before deallocate cluster */
		FFAT_LRT_COMPACT_EA_DEALLOCATE_CLUSTER_AFTER,		/* UNDO, after deallocate cluster */

		FFAT_LRT_DELETE_EA_MARK_DELETE_BEFORE,				/* REDO, before mark delete */
		FFAT_LRT_DELETE_EA_MARK_DELETE_AFTER,				/* REDO, after mark delete */
		FFAT_LRT_DELETE_EA_WRITE_MAIN_BEFORE,				/* REDO, before write main */
		FFAT_LRT_DELETE_EA_WRITE_MAIN_AFTER,				/* REDO, after write main */

		FFAT_LRT_CREATE_SYMLINK_EXPAND_PARENT_BEFORE,		/* UNDO, before parent expand */
		FFAT_LRT_CREATE_SYMLINK_EXPAND_PARENT_AFTER,		/* UNDO, after parent expand */
		FFAT_LRT_CREATE_SYMLINK_ALLOCATE_CLUSTER_BEFORE,	/* UNDO, before cluster allocation */
		FFAT_LRT_CREATE_SYMLINK_ALLOCATE_CLUSTER_AFTER,		/* UNDO, after cluster allocation */
		FFAT_LRT_CREATE_SYMLINK_WRITE_SYMLINK_INFO_BEFORE,	/* UNDO, before write symlink info */
		FFAT_LRT_CREATE_SYMLINK_WRITE_SYMLINK_INFO_AFTER,	/* UNDO, after write symlink info */
		FFAT_LRT_CREATE_SYMLINK_WRITE_DE_BEFORE,			/* UNDO, before write DEs */
		FFAT_LRT_CREATE_SYMLINK_WRITE_DE_AFTER,				/* UNDO, after write DEs */


		// EIO test
		FFAT_EIO_TEST_WRITE_LOG_BEFORE,
		FFAT_EIO_TEST_WRITE_LOG_AFTER,
		FFAT_EIO_TEST_WRITE_DATA_BEFORE,
		FFAT_EIO_TEST_WRITE_DATA_AFTER,
		FFAT_EIO_TEST_WRITE_METADATA_BEFORE,
		FFAT_EIO_TEST_WRITE_METADATA_AFTER,

		FFAT_EIO_TEST_MKDIR_LOG_BEFORE,
		FFAT_EIO_TEST_MKDIR_LOG_AFTER,
		FFAT_EIO_TEST_MKDIR_UPDATE_BEFORE,
		FFAT_EIO_TEST_MKDIR_UPDATE_AFTER,
		FFAT_EIO_TEST_MKDIR_WRITEDE_BEFORE,
		FFAT_EIO_TEST_MKDIR_WRITEDE_AFTER,

		FFAT_EIO_TEST_CREATE_SYMLINK_LOG_BEFORE,
		FFAT_EIO_TEST_CREATE_SYMLINK_LOG_AFTER,

		FFAT_LRT_DUMMY			= 0x7FFFFFFF	/* last entry */
	};


	extern t_uint32 EssOsal_GetTick(void);

	#define FFAT_DEBUG_START_TIME()		\
				{	\
					t_uint32	dwStartTime, dwEndTime, dwElapseTime;	\
					dwStartTime = EssOsal_GetTick();

	#define FFAT_DEBUG_END_TIME()		\
			dwEndTime = EssOsal_GetTick();	\
			dwElapseTime = dwEndTime - dwStartTime;	\
			FFAT_DEBUG_PRINTF("Elapse time :%d \n", dwElapseTime);	\
		}
#endif	// #ifdef FFAT_DEBUG

// debug end

// constant definitions

// external variable declarations

// function declarations
#ifdef __cplusplus
	extern "C" {
#endif

	/* None */

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_TYPES_H_ */



