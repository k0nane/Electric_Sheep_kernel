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
 * @file		ffat_addon_api.h
 * @brief		This file defines FFatAddon module.
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFAT_ADDON_API_H_
#define _FFAT_ADDON_API_H_


//=============================================================================
//
//	ADDON PROGRAMMERS GUIDE
//		1. Lock Scheme : single lock
//			lock addon module at the entrance (AddonAPI)
//			lock implementation is a simple MUTEX
//
//
//


// includes
// NOTICE : do not include headers for FFAT addon module !!!
#include "ffat_types.h"
#include "ffat_addon_types.h"
#include "ffat_common.h"

#include "ffat_vol.h"
#include "ffat_node.h"
#include "ffat_file.h"

// enum

// typedefs

// constant definitions

// external variable declarations

// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	// for init/terminate
	extern FFatErr	ffat_addon_init(t_boolean bForce);
	extern FFatErr	ffat_addon_terminate(ComCxt* pCxt);

	extern FFatErr	ffat_addon_fsctl(FFatFSCtlCmd dwCmd, void* pParam0,
								void* pParam1, void* pParam2, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterSyncNode(Node* pNode, ComCxt* pCxt);
	extern FFatErr	ffat_addon_cacheCallBack(Vol* pVol, t_uint32 dwSector,
								FFatCacheFlag dwFlag, ComCxt* pCxt);

	extern FFatErr	ffat_addon_ldevIO(Vol* pVol, FFatLDevIO* pLDevIO, ComCxt* pCxt);

	// for volume (mount/umount)
	extern FFatErr	ffat_addon_afterMount(Vol* pVol, FFatMountFlag* pdwFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_remount(Vol* pVol, FFatMountFlag* pdwFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_umount(Vol* pVol, FFatMountFlag dwFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_getVolumeStatus(Vol* pVol, FFatVolumeStatus* pStatus,
								t_int8* pBuff, t_int32 dwSize, ComCxt* pCxt);
	extern FFatErr	ffat_addon_syncVol(Vol* pVol, FFatCacheFlag dwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterSyncVol(Vol* pVol, FFatCacheFlag dwCacheFlag,
								ComCxt* pCxt);

	// for node
	extern FFatErr	ffat_addon_initNode(Vol* pVol, Node* pNodeParent, Node* pNodeChild,
								ComCxt* pCxt);
	extern FFatErr	ffat_addon_terminateNode(Node* pNode, ComCxt* pCxt);
	extern FFatErr	ffat_addon_openNode(Node* pNode, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterCloseNode(Node* pNode, ComCxt* pCxt);

	extern FFatErr	ffat_addon_expandDir(Node* pNode, t_uint32 dwPrevEOC,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_truncateDir(Node* pNode, t_uint32 dwPrevEOC, t_uint32 dwOffset,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);

	extern void		ffat_addon_createExpandParent(Node* pNodeChild, ComCxt* pCxt);

	extern void		ffat_addon_fillNodeInfo(Node* pNode, FatGetNodeDe* pNodeDE, void* pAddonInfo);
	extern FFatErr	ffat_addon_afterGetNodeDE(Vol* pVol,
								FatGetNodeDe* pNodeDE, ComCxt* pCxt);
	extern FFatErr	ffat_addon_GetGUIDFromNode(Node* pNode, void* pstXDEInfo);

	// for file
	extern FFatErr	ffat_addon_create(Node* pParent, Node* pNodeChild, t_wchar* psName,
								FatDeSFN* pDE, t_uint8 bCheckSum, void* pAddonInfo,
								FFatCreateFlag dwCreateFlag, FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterCreate(Node* pNodeParent, Node* pNodeChild,
								t_wchar* psName, t_uint32* pdwClusters, t_int32 dwClusterCountDE,
								t_boolean bSuccess, ComCxt* pCxt);

	extern FFatErr	ffat_addon_afterReadFile(Node* pNode, FFatVC* pVC, ComCxt* pCxt);

	extern t_int32	ffat_addon_writeFile(Node* pNode, t_uint32 dwLastOffset, t_uint32 dwPrevEOC,
								FFatVC* pVC_Cur, FFatVC* pVC_New, FFatWriteFlag dwWriteFlag,
								FFatCacheFlag *pdwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterWriteFile(Node* pNode, t_uint32 dwOriginalSize,
								FFatCacheFlag dwCacheFlag, t_boolean bSuccess, ComCxt* pCxt);

	extern FFatErr	ffat_addon_rename(Node* pNodeSrcParent, Node* pNodeSrc,
								Node* pNodeDesParent, Node* pNodeDes, Node* pNodeNewDes,
								t_wchar* psName, FFatRenameFlag dwFlag, 
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_renameUpdateDE(Node* pNodeSrc, Node* pNodeNewDes,
								FatDeSFN* pDE, t_uint8 bCheckSum, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterRename(Node* pNodeSrcParent, Node* pNodeSrc,
								Node* pNodeDesParent, Node* pNodeDes, Node* pNodeNew,
								t_wchar* psName, FFatRenameFlag dwFlag, t_boolean bSuccess,
								t_uint32* pdwClustersDE, t_int32 dwClusterCountDE,
								ComCxt* pCxt);

	extern FFatErr	ffat_addon_unlink(Node* pNodeParent, Node* pNode,
								FFatVC* pVC, NodeUnlinkFlag dwFlag,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterUnlink(Node* pNodeParent, Node* pNode, NodeUnlinkFlag dwNUFlag,
								t_boolean bSuccess, ComCxt* pCxt);

	extern FFatErr	ffat_addon_unlinkOpenUnlinkedNode(Node* pNode, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterUnlinkOpenUnlinkedNode(Node* pNode, ComCxt* pCxt);

	extern FFatErr	ffat_addon_changeSize(Node* pNode, t_uint32 dwSize, t_uint32 dwEOF,
								FFatVC *pVC, FFatChangeSizeFlag dwCSFlag,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterChangeSize(Node* pNode, FFatVC* pVC, t_uint32 dwOriginalSize,
								t_boolean bExpand, t_boolean bSuccess, ComCxt* pCxt);

	extern FFatErr	ffat_addon_setStatus(Node* pNode, FFatNodeStatus* pStatus,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterSetStatus(Node* pNode, t_boolean bSuccess, ComCxt* pCxt);

	extern FFatErr	ffat_addon_getStatusFromDe(Node* pNode, FatDeSFN* pDE,
								t_uint32 dwDeCluster, t_uint32 dwDeOffset, 
								FFatNodeStatus* pStatus, ComCxt* pCxt);

	// for symlink
	extern FFatErr	ffat_addon_createSymlink(Node* pNodeParent, Node* pNodeChild,
								t_wchar* psName, t_wchar* psPath,
								FFatCreateFlag dwFlag, void* pAddonInfo, ComCxt* pCxt);
	extern FFatErr	ffat_addon_readSymlink(Node* pNode, t_wchar* psPath, t_int32 dwLen, t_int32* pdwLen, ComCxt* pCxt);
	extern FFatErr	ffat_addon_isSymlink(Node* pNode);

	// for cluster
	extern FFatErr	ffat_addon_getAvailableClusterCountForNode(Node* pNode,
								t_uint32* pdwCount, ComCxt* pCxt);
	extern FFatErr	ffat_addon_getClusterOfOffset(Node* pNode, t_uint32 dwOffset,
								t_uint32* pdwCluster, t_uint32* pdwOffset,
								NodePAL* pPAL, ComCxt* pCxt);
	extern FFatErr	ffat_addon_getFreeClusters(Node* pNode, t_uint32 dwCount,
								FFatVC* pVC, t_uint32 dwHint,
								t_uint32* pdwFreeCount, FatAllocateFlag dwAllocFlag,
								ComCxt* pCxt);
	extern FFatErr	ffat_addon_addFreeClusters(Vol* pVol, t_uint32 dwCluster,
								t_uint32 dwCount, ComCxt* pCxt);
	extern FFatErr	ffat_addon_addFreeClustersVC(Vol* pVol, FFatVC* pVC, ComCxt* pCxt);
	extern FFatErr	ffat_addon_deallocateCluster(Node* pNode, t_uint32 dwNewEOC,
								t_uint32* pdwFirstCluster, t_uint32 dwCount,
								t_uint32* pdwDeallocCount,  FFatVC* pVC,
								FatAllocateFlag dwFAFlag, FFatCacheFlag dwCacheFlag,
								ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterDeallocateCluster(Node* pNode, FFatVC* pVC,
								t_uint32 dwDeallocCount, t_uint32 dwFirstCluster, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterMakeClusterChain(Node* pNode, t_uint32 dwPrevEOF,  t_int32 dwClusterCount, 
						t_uint32* pdwClusters, FatUpdateFlag dwFUFlag, FFatCacheFlag dwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterMakeClusterChainVC(Node* pNode, t_uint32 dwPrevEOF,
								FFatVC* pVC, FatUpdateFlag dwFUFlag,
								FFatCacheFlag dwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterSetVolumeLabel(Vol* pVol, ComCxt* pCxt);

	extern FFatErr	ffat_addon_getNextCluster(Node* pNode, t_uint32 dwCluster,
								t_uint32* pdwNextCluster);
	extern FFatErr	ffat_addon_getVectoredCluster(Node* pNode, t_uint32 dwCluster,
								t_uint32 dwOffset, t_uint32 dwCount, FFatVC* pVC,
								t_uint32* pdwNewCluster, t_uint32* pdwNewCount,
								NodePAL* pPAL, ComCxt* pCxt);

	// for lookup
	extern FFatErr	ffat_addon_lookup(Node* pNodeParent, Node* pNodeChild, t_wchar* psName, t_int32 dwLen,
								FFatLookupFlag dwFlag, FatGetNodeDe* pNodeDE,
								NodeNumericTail* pNumericTail, ComCxt* pCxt);
	extern FFatErr	ffat_addon_afterLookup(Node* pNodeParent, Node* pNodeChild,
								FFatLookupFlag dwFlag, ComCxt* pCxt);

	// for access control
	extern FFatErr	ffat_addon_isAccessable(Node* pNode, NodeAccessFlag dwFlag);

// debug begin
#ifdef FFAT_DEBUG
	extern FFatErr	ffat_addon_checkNodeDeInfo(Node* pNode);
#endif
// debug end


#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFAT_ADDON_API_H_ */


