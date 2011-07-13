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
 * @file		ffat_dir.h
 * @brief		This file defines FFatDir
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFAT_DIR_H_
#define _FFAT_DIR_H_

//!< includes
#include "ffat_node.h"
#include "ffat_vol.h"
#include "ffatfs_types.h"

//!< defines

//!< enum

/* readdir flag */
typedef signed int	ReaddirFlag;
enum _ReaddirFlag
{
	READDIR_NONE				= 0x00,		// no flag 
	READDIR_STAT				= 0x01,		// READDIR with stat 
	READDIR_GET_NODE			= 0x02,		// READDIR with fill node information
											//	Caution!!. Node must be terminated after using (Node has resource)
	READDIR_LFN					= 0x04,		// store long file name 
	READDIR_SFN					= 0x08,		// store short file name 
	READDIR_GET_SIZE_OF_DIR		= 0x10,		// get real size of directory
											// this does not work without READDIR_STAT
	READDIR_NO_LOCK		= 0x20,		// do not lock

	FFAT_READDIR_DUMMY	= 0x7FFFFFFF
};


//!< typedefs
/* readdir information */
typedef struct
{
	t_uint32			dwOffsetNext;	//!< [OUT] next readdir offset
	t_wchar*			psLFN;			//!< [IN/OUT]storage of long file name
										//!< may be NULL, unless it should be 256*2 byte
	t_wchar*			psSFN;			//!< [IN/OUT]storage of short file name
										//!< may be NULL, unless 12*2 byte
	t_int32				dwLenLFN;		//!< [IN ] character count can be stored at psLFN
										//!< [OUT] long file name length
	t_int32				dwLenSFN;		//!< [IN ] character count can be stored at psSFN
										//!< [OUT] long file name length
	FFatNodeStatus*		pNodeStatus;	//!< [OUT] node status
										//!< may be NULL
	Node*				pNode;			//!< [OUT] Node structure, it does not have lock
} ReaddirInfo;


//!< constant definitions

//!< external variable declarations

//!< function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_dir_init(void);
	extern FFatErr	ffat_dir_terminate(void);

	extern FFatErr	ffat_dir_readdir(Node* pNode, t_uint32 dwOffset, 
								ReaddirInfo* pRI, ReaddirFlag dwFlag, ComCxt* pCxt);
	extern FFatErr	ffat_dir_isEmpty(Node* pNode, ComCxt* pCxt);
	extern FFatErr	ffat_dir_remove(Node* pNodeParent, Node* pNode,
								NodeUnlinkFlag dwNUFlag, ComCxt* pCxt);

	extern FFatErr	ffat_dir_expand(Node* pNode, t_int32 dwNewClusterCount,
								t_uint32 dwLastCluster, t_uint32* pdwClusters, ComCxt* pCxt);
	extern t_int32	ffat_dir_write(Node* pNode, t_uint32 dwOffset, t_int8* pBuff, t_int32 dwSize,
								FFatVC* pVC, FFatCacheFlag dwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_dir_initCluster(Node* pNodeParent, Node* pNode, ComCxt* pCxt);

	extern FFatErr	ffat_dir_getDirEntry(Vol* pVol, Node* pNode, FatGetNodeDe* pNodeDE,
								t_boolean bIgnoreVolLabel, t_boolean bCheckAddon, ComCxt* pCxt);

	extern FFatErr	ffat_dir_getSize(Node* pNode, t_uint32* pdwSize, ComCxt* pCxt);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_DIR_H_ */



