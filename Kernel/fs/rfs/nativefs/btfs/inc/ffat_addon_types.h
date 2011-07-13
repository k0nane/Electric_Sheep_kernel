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
 * @file		ffat_addon_types.h
 * @brief		The file is for FFatAddon module type.
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @version		FEB-02-2009 [DongYoung Seo] Remove code for user fast seek
 * @see			None
 */

#ifndef _FFAT_ADDON_TYPES_H_
#define _FFAT_ADDON_TYPES_H_

// includes
// includes
//	NOTICE.!!!
//	This header should not include any other header file.
#include "ffat_types.h"

//===========================================================================
//
// define
//
#define FFAT_ADDON_VOL_SIZE		(16/4)
#define FFAT_ADDON_NODE_SIZE	(40/4)

#ifdef USE_64_BIT_ADDRESS
	#undef FFAT_ADDON_VOL_SIZE
	#undef FFAT_ADDON_NODE_SIZE
	#define FFAT_ADDON_VOL_SIZE		(32/4)
	#define FFAT_ADDON_NODE_SIZE	(48/4)
#endif

#define FFAT_VOLUME_NAME_MAX_LENGTH		12		/*!< volume name length max */

/* default value for XDE */ 
#define FFAT_ADDON_DEFAULT_UID			0
#define FFAT_ADDON_DEFAULT_GID			0
#define FFAT_ADDON_DEFAULT_PERMISSION	(FFAT_XDE_RWX & ~(FFAT_XDE_GROUP_WRITE | FFAT_XDE_OTHERS_WRITE))	// 0755

#define FFAT_ADDON_LOG_FILE_UID			0				// root
#define FFAT_ADDON_LOG_FILE_GID			0				// root
#define FFAT_ADDON_LOG_FILE_PERMISSION	FFAT_XDE_READS	// read only (0444)

#define FFAT_ADDON_DEBUG_FILE_UID			0				// root
#define FFAT_ADDON_DEBUG_FILE_GID			0				// root
#define FFAT_ADDON_DEBUG_FILE_PERMISSION	FFAT_XDE_READS	// read only (0444)

//===========================================================================
//
// enum
//


/* Flag for Directory Entry Cache */
typedef signed int	FFatDECFlag;
enum _FFatDECFlag
{
	FFAT_DEC_SET				= 0x01,		/*!< set(enable) Directory Entry Cache */
	FFAT_DEC_RELEASE			= 0x02,		/*!< release(disable) Directory Entry Cache */
	FFAT_DEC_GET_INFO			= 0x04,		/*!< get Directory Entry Cache information */

	FFAT_DEC_DUMMY				= 0x7FFFFFFF
};

/* Flag for Free Cluster Cache */
typedef signed int	FFatFCCFlag;
enum _FFatFCCFlag
{
	FFAT_FCC_SET				= 0x01,		/*!< set(enable) Free Cluster Cache */
	FFAT_FCC_RELEASE			= 0x02,		/*!< release(disable) Free Cluster Cache */
	FFAT_FCC_GET_INFO			= 0x04,		/*!< get Free Cluster Cache information */

	FFAT_FCC_DUMMY				= 0x7FFFFFFF
};

/* Flags for chkdsk operation */
typedef signed int	FFatChkdskFlag;
enum _FFatChkdskFlag
{
	FFAT_CHKDSK_REPAIR				= 0x0001,	/*! repair errors */
	FFAT_CHKDSK_SHOW				= 0x0002,	/*! show errors */
	FFAT_CHKDSK_REPAIR_INTERACTIVE	= 0x0004,	/*! check filesystem and ask before repair */
	FFAT_CHKDSK_CHECK_ONLY			= 0x0008,	/*! Just check the file system is ok or not */
	FFAT_CHKDSK_MASK				= (FFAT_CHKDSK_REPAIR | FFAT_CHKDSK_SHOW | 
										FFAT_CHKDSK_REPAIR_INTERACTIVE | FFAT_CHKDSK_CHECK_ONLY),

	FFAT_CHKDSK_DUMMY				= 0x7FFFFFFF
};

/* Commands for extended attribute operation */
typedef signed int	FFatXAttrCmd;
enum _FFatXAttrCmd
{
	FFAT_XATTR_CMD_SET				= 0x0001,	/*! set an extended attribute value */
	FFAT_XATTR_CMD_GET				= 0x0002,	/*! get an extended attribute value */
	FFAT_XATTR_CMD_REMOVE			= 0x0004,	/*! delete an extended attribute */
	FFAT_XATTR_CMD_LIST				= 0x0008,	/*! list extended attribute names */

	FFAT_XATTR_CMD_DUMMY			= 0x7FFFFFFF
};

/* Flags for extended attribute set operation */
typedef signed int	FFatXAttrNSID;
enum _FFatXAttrNSID
{
	FFAT_XATTR_NS_USER				= 0x01,		/* user namespace */
	FFAT_XATTR_ID_POSIX_ACL_ACCESS	= 0x02,		/* POSIX ACL access name */
	FFAT_XATTR_ID_POSIX_ACL_DEFAULT	= 0x03,		/* POSIX ACL default name */
	FFAT_XATTR_NS_TRUSTED			= 0x04,		/* trusted namespace */
	FFAT_XATTR_NS_SECURITY			= 0x05,		/* security namespace */

	FFAT_XATTR_NS_DUMMY				= 0x7FFFFFFF
};

/* Flags for extended attribute set operation */
typedef signed int	FFatXAttrSetFlag;
enum _FFatXAttrSetFlag
{
	FFAT_XATTR_SET_FLAG_NONE		= 0x0000,		/* if ATTR already exists, replace and if doesn't exist, create */
	FFAT_XATTR_SET_FLAG_CREATE		= 0x0001,		/* if ATTR already exists, fail */
	FFAT_XATTR_SET_FLAG_REPLACE		= 0x0002,		/* if ATTR does not exist, fail */

	FFAT_XATTR_SET_FLAG_DUMMY		= 0x7FFFFFFF
};

/*!< permission for extended directory entry */
typedef unsigned short	_XDE_PERMISSION;
enum __XDE_PERMISSION
{
	//2009.1029@chunum.kong_[Full_Permission]_use_sticky_bit
	FFAT_XDE_EXTRA_STICKY			= FFAT_EXTRA_STICKY,		/**< others can read/write/execute. but only owner can delete */
	FFAT_XDE_EXTRA_SETGID			= FFAT_EXTRA_SETGID,		/**< group can execute */
	FFAT_XDE_EXTRA_SETUID			= FFAT_EXTRA_SETUID,		/**< user can execute */
	FFAT_XDE_EXTRA_ALL				= FFAT_EXTRA_STICKY | FFAT_EXTRA_SETGID | FFAT_EXTRA_SETUID,

	FFAT_XDE_OTHERS_EXECUTE			= FFAT_OTHERS_EXECUTE,		/**< others can execute */
	FFAT_XDE_OTHERS_WRITE			= FFAT_OTHERS_WRITE,		/**< others can write */
	FFAT_XDE_OTHERS_READ			= FFAT_OTHERS_READ,			/**< others can read */
	FFAT_XDE_OTHERS_RWX				= FFAT_OTHERS_EXECUTE | FFAT_OTHERS_WRITE | FFAT_OTHERS_READ,

	FFAT_XDE_GROUP_EXECUTE			= FFAT_GROUP_EXECUTE,		/**< group can execute */
	FFAT_XDE_GROUP_WRITE			= FFAT_GROUP_WRITE,			/**< group can write */
	FFAT_XDE_GROUP_READ				= FFAT_GROUP_READ,			/**< group can read */
	FFAT_XDE_GROUP_RWX				= FFAT_GROUP_EXECUTE | FFAT_GROUP_WRITE | FFAT_GROUP_READ,

	FFAT_XDE_OWNER_EXECUTE			= FFAT_OWNER_EXECUTE,		/**< owner can execute */
	FFAT_XDE_OWNER_WRITE			= FFAT_OWNER_WRITE,			/**< owner can write */
	FFAT_XDE_OWNER_READ				= FFAT_OWNER_READ,			/**< owner can read */
	FFAT_XDE_OWNER_RWX				= FFAT_OWNER_EXECUTE | FFAT_OWNER_WRITE | FFAT_OWNER_READ,

	FFAT_XDE_EXECUTES				= FFAT_OTHERS_EXECUTE | FFAT_GROUP_EXECUTE | FFAT_OWNER_EXECUTE,
	FFAT_XDE_WRITES					= FFAT_OTHERS_WRITE | FFAT_GROUP_WRITE | FFAT_OWNER_WRITE,
	FFAT_XDE_READS					= FFAT_OTHERS_READ | FFAT_GROUP_READ | FFAT_OWNER_READ,

	FFAT_XDE_RWX					= FFAT_XDE_EXECUTES | FFAT_XDE_WRITES | FFAT_XDE_READS
};


//===========================================================================
//
// type definition
//

typedef struct _FFatAddonVol
{
	t_int32		dwNode[FFAT_ADDON_VOL_SIZE];
} FFatAddonVol;

typedef struct _FFatAddonNode
{
	t_int32		dwNode[FFAT_ADDON_NODE_SIZE];
} FFatAddonNode;

/*!< this is a NAND specialized structure. */
/*!< To accelerate write performance clean NAND device while idle time. */
typedef struct _FFatCleanNand
{
	t_uint32	dwStartCluster;		/*!< clean start cluster number */
	t_uint32	dwClusterCount;		/*!< cluster count to be tested for cleaning */
} FFatCleanNand;

typedef struct _FFatFormatInfo
{
	const t_wchar*	psVolumeLabel;			/*!< volume name, max 11*/
	t_int8*			pBuff;					/*!< buffer for format performance improvement, it may be NULL */
	t_int32			dwBuffSize;				/*!< buffer size */
	t_int32			dwSectorsPerCluster;	/*!< cluster size in sector count */
	t_int32			dwRsvdSector;			/*!< reserved sector count, 0 : use default */
											/*!< default value : FFAT_BPB_RESERVED_SECTOR */
	FFatFatType		dwFatType;				/*!< fat type */

	t_uint16		wAlignBasis;			/*!< sector align count */
											/*!< if 0 , do not align */
	t_uint32		dwStartSector;			/*!< partition start sector number */
	t_uint32		dwSectorCount;			/*!< total sector count of device */
	t_int32			dwSectorSize;			/*!< logical device & FAT sector size in bytes */
											/*!< logical device & FAT sector size must be same */
	t_int32			dwSectorSizeBits;		/*!< sector size in bit count */

	void*			pDevice;				/*!< for device distinction */
											/*!< this will be sent on bock device IO */
} FFatFormatInfo;

typedef struct _FFatReaddirStatInfo
{
	t_uint32			dwOffset;			/*!< [IN] readdir start offset */
	t_uint32			dwOffsetNext;		/*!< [IN/OUT] next readdir byte offset */
	t_wchar*			psName;				/*!< name storage , should be 512 byte */
											/*!< if NULL, FFAT does not store long file name entry */
	t_wchar*			psShortFileName;	/*!< short file name storage, should be 16 byte */
											/*!< if NULL, FFAT does not store short file name entry */
	t_int32				dwNameLen;			/*!< character count can be stored at psName */
	t_int32				dwShortFileNameLen;	/*!< character count can be stored at psShortFileName */
	FFatNodeStatus		stStat;				/*!< node status storage */
	const t_wchar*		psNameToSearch;		/*< name to search, both "*" and "?" can be used */
											/*!< may be NULL ==> return all entry, same as "*" */
} FFatReaddirStatInfo;

typedef struct _FFatReaddirUnlinkInfo
{
	t_uint32			dwOffset;			/*!< [IN] readdir start offset */
	t_uint32			dwOffsetNext;		/*!< [IN/OUT] next readdir byte offset */
	FFatNodeFlag		dwMode;				/*!< deletion mode */
	FFatNode*			pNode;				/*!< node information storage */
} FFatReaddirUnlinkInfo;

typedef struct _FFatReaddirGetNodeInfo
{
	t_uint32			dwOffset;			/*!< [IN] readdir start offset */
	t_uint32			dwOffsetNext;		/*!< [IN/OUT] next readdir byte offset */
	t_wchar*			psName;				/*!< name storage , should be 512 byte */
											/*!< if NULL, FFAT does not store long file name entry */
	t_int32				dwNameLen;			/*!< character count can be storage at psName */
	FFatNode*			pNode;				/*!< child node information storage , it does not have lock*/
} FFatReaddirGetNodeInfo;

/*!< structure for Directory Entry Set */
typedef struct _FFatDirEntryCacheInfo
{
	t_int8*		pBuff;						/*!< buffer for User DEC , only for FFAT_DEC_SET */
	t_int32		dwSize;						/*!< size of User buffer, should be over _SIZE_MIN_UDEC */
											/*!< only for FFAT_DEC_SET */
} FFatDirEntryCacheInfo;

/*!< structure for Free Cluster Set */
typedef struct _FFatFreeClusterCacheInfo
{
	t_int8*		pBuff;						/*!< buffer for FCC */
	t_int32		dwSize;						/*!< size of buffer, should be over FFAT_FCC_BUFF_MIN(128) byte, */
} FFatFreeClusterCacheInfo;

/*!< structure for Extended attribute */
typedef struct _FFatXAttrInfo
{
	t_int8*				psName;				/*!< [IN] extended attribute name or list of name */
	t_int8*				pValue;				/*!< [IN/OUT] extended attribute value */
	t_int32				dwSize;				/*!< [IN/OUT] buffer size for extended attribute value or list */
											/*!<          if dwSize is 0, size of value or list will be returned */

	FFatXAttrNSID		dwNSID;				/*!< [IN] namespace id of extended attribute name */
	FFatXAttrSetFlag	dwSetFlag;			/*!< [IN] extended attribute set flag */
	FFatXAttrCmd		dwCmd;				/*!< [IN] extended attribute command */

} FFatXAttrInfo;

typedef struct _FFatExtendedDirEntryInfo
{
	t_uint32		dwUID;					/*!< User ID */
	t_uint32		dwGID;					/*!< Group ID */
	t_uint32		dwPerm;					/*!< Permission */
} FFatExtendedDirEntryInfo;

typedef struct _FFatGetShortLongName
{
	FFatNode*			pParent;

	t_wchar*			psLongName;			/*!< GetShortName -	[IN] long file name to get short file name */
											/*!< GetLongName -	[OUT] buffer to store long file name */
											/*!<					  should be more than 512 bytes ((255 + 1) * 2) */

	t_wchar*			psShortName;		/*!< GetShortName -	[OUT] buffer to store short file name */
											/*!<					  should be more than 36 bytes ((12 + 1) * 2) */
											/*!< GetLongName -	[IN] short file name to get long file name */

	t_int32				dwLongNameLen;		/*!< [IN] character count can be stored at psLongName */
											/*!< [OUT] length of long file name */

	t_int32				dwShortNameLen;		/*!< [IN] character count can be stored at psShortName */
											/*!< [OUT] length of short file name */
} FFatGetShortLongName;

// debug begin
#ifdef FFAT_DEBUG
	typedef struct _FFatFreeClusterCache
	{
		t_uint32		dwStartVolume;		/*!< buffer for FCC */
		t_uint32		dwEndVolume;		/*!< size of buffer, should be over FFAT_FCC_BUFF_MIN(128) byte, */
	} FFatFreeClusterCache;
#endif
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

#endif	/* #ifndef _FFAT_ADDON_TYPES_H_ */
