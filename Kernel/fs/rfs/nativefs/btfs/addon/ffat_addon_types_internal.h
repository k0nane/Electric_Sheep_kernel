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
 * @file		ffat_addon_types_internal.h
 * @brief		Internal data type for ffat addon module
 *				this is only for add-on module.
 *				never be referenced by another module (core, ffatfs)
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @version		FEB-02-2009 [DongYoung Seo] Remove code for user fast seek
 * @see			None
 */

#ifndef _FFAT_ADDON_TYPES_INTERNAL_H_
#define _FFAT_ADDON_TYPES_INTERNAL_H_

//===========================================================================
//
// include
//
#include "ffat_types.h"
#include "ffat_addon_dec.h"
#include "ffat_addon_log.h"
#include "ffat_addon_fastseek.h"
#include "ffat_addon_fcc.h"
#include "ffat_addon_nand.h"
#include "ffat_addon_hpa.h"
#include "ffat_addon_xde.h"
#include "ffat_addon_debug.h"

//===========================================================================
//
// define
//

#define VOL_ADDON(_pVol)			((AddonVol*)(&((_pVol)->stAddon)))		//!< get ADDON pointer
#define NODE_ADDON(_pNode)			((AddonNode*)(&((_pNode)->stAddon)))	//!< get ADDON pointer
#define NODE_ADDON_FLAG(_pNode)		(NODE_ADDON(_pNode)->dwFlag)			//!< get ADDON node flag

// for AddonBPB
#define ADDON_BPB_START_OFFSET		(128)		//!< offset of AddonBPB in BPB
												// this value must be larger than 90(last field of FAT32 BPB)
												// and smaller than (510 - size of AddonBPB) (510, 511 is for mark of BPB)

#define ADDON_BPB_SIZE				(sizeof(AddonBPB))	// size of ADDON BPB

#define ADDON_BPB_START_OFFSET_LIMIT	(90)	//!< [DO NOT CHANGE] the limit of start offset of AddonBPB in BPB
												// The 89 offset is for the last field of FAT32 in BPB
#define ADDON_BPB_END_OFFSET_LIMIT		(509)	//!< [DO NOT CHANGE] the limit of end offset of AddonBPB in BPB
												// 510, 511 is for mark of BPB

#define	ADDON_BPB_XDE_OFFSET		(ADDON_BPB_START_OFFSET)	//!< start offset of root XDE in BPB
#define ADDON_BPB_EA_OFFSET			(ADDON_BPB_XDE_OFFSET + sizeof(ExtendedDe))		//!< start offset of root EA in BPB
#define ADDON_BPB_LOG_INFO_OFFSET	(ADDON_BPB_EA_OFFSET + sizeof(ExtendedAttr))	//!< start offset of log creation info in BPB

//===========================================================================
//
// enum
//

//===========================================================================
//
// type definition
//
typedef signed int	AddonNodeFlag;
enum _AddonNodeFlag
{
	ADDON_NODE_NONE					= 0x00000000,		//!< no flag
	ADDON_NODE_HPA					= 0x00000001,		//!< node is at HPA
	ADDON_NODE_GFS					= 0x00000002,		//!< node has Global Fast Seek
	ADDON_NODE_GDEC					= 0x00000004,		//!< node has Global DEC
	ADDON_NODE_UDEC					= 0x00000008,		//!< node has User DEC
	ADDON_NODE_DEC_MASK				= ADDON_NODE_GDEC | ADDON_NODE_UDEC,	//!< mask for DEC related flags
	ADDON_NODE_LOG					= 0x00000010,		//!< node is log file
	ADDON_NODE_SYMLINK				= 0x00000020,		//!< node is symlink
	ADDON_NODE_FIFO					= 0x00000040,		//!< node is fifo
	ADDON_NODE_SOCKET				= 0x00000080,		//!< node is socket
	ADDON_NODE_SPECIAL_FILES		= ADDON_NODE_SYMLINK | ADDON_NODE_FIFO | ADDON_NODE_SOCKET,
	ADDON_NODE_DUMMY				= 0x7FFFFFFF
};

// Short directory entryÀÇ NTResÀÇ fieldÀÇ flag
// do not use 0x10 and 0x08 bits
typedef unsigned char	AddonSFNEMark;
enum _AddonSFNEMark
{
	ADDON_SFNE_MARK_XATTR		= 0x80,			//!< this file has extended attributes
	ADDON_SFNE_MARK_XDE			= 0x40			//!< this file has extended DE (UID/GID, permission)
};

//!< Node structure for ADDON module
//!<	* Add node level data at here
typedef struct _AddonNode
{
	// all members in this structure may be NULL

	// be careful !!
	// do not store list at here!!. this structure may be copied.
	GFSNodeHead*		pGFS;			//!< head of Global Fast Seek
											// size(32 | 64bit) => 4 | 8
	UserDEC*			pUDEC;			//!< directory entry cache, NULL : DEC is not enabled.
											// size(32 | 64bit) => 4 | 8
	LogOUInfo			stOUI;			//!< deallocate cluster number storage for open unlink.
											// size(32 | 64bit) => 8 | 8	(4*2)
	XDEInfo				stXDE;			//!< Extended Directory Entry
											// size(32 | 64bit) => 12 | 12	(4*3)
	EAInfo				stEA;			//!< Extended Attribute information
											// size(32 | 64bit) => 8 | 8	(4*2)

	AddonNodeFlag		dwFlag;			//!< flag for ADDON node
											// size(32 | 64bit) => 4 | 4

											// total_size(byte)	=> 40 | 48
} AddonNode;


//!< volume structure for ADDON module
//!<	* Add volume level data at here
typedef struct _AddonVol
{
	// all members in this structure may be NULL
	LogInfo*			pLI;			//!< log context per volume
											// size(32 | 64bit) => 4 | 8
	FCCVolInfo*			pFCC;			//!< free cluster cache
											// size(32 | 64bit) => 4 | 8
	HPAInfo*			pHPA;			//!< Hidden Protected Area
											// size(32 | 64bit) => 4 | 8
	DebugFileInfo*		pDFile;			//!< debug file
											// size(32 | 64bit) => 4 | 8

											// total_size(byte)	=> 16 | 32
} AddonVol;


//!< Main structure on ADDON
//!<	* Add global data for ADDON at here
typedef struct 
{
	FFatLock*		pLock;			//!< lock pointer
	t_int32			dwRefCount;		//!< current reference count, lock count
} AddonMain;

//!< Addon BPB structure for read / write
//!<	* Add BPB data for ADDON at here
typedef struct 
{
	ExtendedDe		stRootXDE;		//!< Extended DE for Root
	ExtendedAttr	stRootEA;		//!< Extended attribute for Root
	LogCreatInfo	stLogCreatInfo;	//!< Information of log creation
} AddonBPB;

//===========================================================================
//
// constant definitions
//

//===========================================================================
//
// external variable declarations
//

//===========================================================================
//
// function declarations
//
#ifdef __cplusplus
	extern "C" {
#endif

	// Nothing

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_ADDON_TYPES_INTERNAL_H_ */
