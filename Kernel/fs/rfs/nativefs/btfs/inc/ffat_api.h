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
 * @file		ffat_api.h
 * @brief		The file defines APIs for FFAT filesystem.
 *				FFAT filesystem is a 100% FAT compatible filesystem
 *				It provides all filesystem operation such as belows
 *					1. file / directory creation/deletion/lookup
 *					2. file read/write/change size/get status/set status
 *					3. read direcory
 *					4. get volume status
 *				It also provides addon module for performance imrpovement.
 *
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFAT_API_H_
#define _FFAT_API_H_

// includes
//	NOTICE.!!!
//	This header should not include any header file except below one for librarian.
#include "ffat_types.h"

// defines

// enum

// typedefs

// constant definitions

// external variable declarations

// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	FFAT_Init(t_boolean bForce);
	extern FFatErr	FFAT_Terminate(void);
	extern FFatErr	FFAT_Mount(FFatVol* pVol, FFatNode* pRoot, FFatMountFlag* pdwFlag,
								FFatLDevInfo* pLDevInfo, void* pDev);
	extern FFatErr	FFAT_Remount(FFatVol* pVol, FFatMountFlag* pdwFlag);
	extern FFatErr	FFAT_Umount(FFatVol* pVol, FFatMountFlag dwFlag);
	extern FFatErr	FFAT_GetBSInfoFromBS(void* pDev, t_int32 dwIOSize, t_int32* pdwSectorSize,
								t_int32* pdwClusterSize, t_uint32* pdwFirstDataSector);

	extern FFatErr	FFAT_GetVolumeStatus(FFatVol* pVol, FFatVolumeStatus* pStatus,
								t_int8* pBuff, t_int32 dwSize);

	extern FFatErr	FFAT_SetVolumeLabel(FFatVol* pVol, t_wchar* psVolLabel);
	extern FFatErr	FFAT_GetVolumeLabel(FFatVol* pVol, t_wchar* psVolLabel,
								t_int32 dwVolLabelLen);

	extern FFatErr	FFAT_Lookup(FFatNode* pNodeParent, FFatNode* pNodeChild,
								t_wchar* psName, FFatLookupFlag dwFlag, void* pAddonInfo);

	extern FFatErr	FFAT_FSCtl(FFatFSCtlCmd dwCmd, void* pParam0, void* pParam1, void* pParam2);
	extern FFatErr	FFAT_Makedir(FFatNode* pNodeParent, FFatNode* ppNodeChild, 
								t_wchar* psName, FFatCreateFlag dwFlag, void* pAddonInfo);
	extern FFatErr	FFAT_Removedir(FFatNode* pNodeParent, FFatNode* pNode, t_boolean bIsOpened);
	extern FFatErr	FFAT_Readdir(FFatNode* pNode, t_uint32 dwOffset, t_uint32* pdwOffsetNext,
								t_wchar* psName, t_int32 dwNameLen);

	extern FFatErr	FFAT_Create(FFatNode* pNodeParent, FFatNode* pNodeChild,
								t_wchar* psName, FFatCreateFlag dwFlag, void* pAddonInfo);
	extern FFatErr	FFAT_Open(FFatNode* pNode, void* pInode);
	extern FFatErr	FFAT_Close(FFatNode* pNode, FFatNodeCloseFlag dwFlag);

	extern t_int32	FFAT_Read(FFatNode* pNode, t_uint32 dwOffset, t_int8* pBuff,
								t_int32 dwSize, FFatReadFlag dwReadFlag);
	extern t_int32	FFAT_Write(FFatNode* pNode, t_uint32 dwOffset, t_int8* pBuff,
								t_int32 dwSize, FFatWriteFlag dwFlag);

	extern FFatErr	FFAT_CreateSymlink(FFatNode* pNodeParent, FFatNode* pNodeChild, t_wchar* psName,
										t_wchar* psPath, FFatCreateFlag dwFlag, void* pAddonInfo);
	extern FFatErr	FFAT_ReadSymlink(FFatNode* pNode, t_wchar* psPath, t_int32 dwLen, t_int32* pdwLen);

	extern FFatErr	FFAT_Unlink(FFatNode* pNodeParent, FFatNode* pNode, t_boolean bIsOpened);
	extern FFatErr	FFAT_SecureUnlink(FFatNode* pNodeParent, FFatNode* pNode);

	extern FFatErr	FFAT_ChangeSize(FFatNode* pNode, t_uint32 dwSize, FFatChangeSizeFlag dwFlag);

	extern FFatErr	FFAT_Rename(FFatNode* pNodeSrcParent, FFatNode* pNodeSrc,
								FFatNode* pNodeDesParent, FFatNode* pNodeDes,
								t_wchar* psName, FFatRenameFlag dwFlag);

	extern FFatErr	FFAT_SetNodeStatus(FFatNode* pNode, FFatNodeStatus* pStatus);
	extern FFatErr	FFAT_GetNodeStatus(FFatNode* pNode, FFatNodeStatus* pStatus);

	extern FFatErr	FFAT_SyncNode(FFatNode* pNode, t_uint32 dwSizeToBe);
	extern FFatErr	FFAT_SyncVol(FFatVol* pVol);

	extern FFatErr	FFAT_GetNodeClusters(FFatNode* pNode, t_uint32 dwOffset,
								t_uint32 dwSize, FFatVC* pVC);

	extern FFatErr	FFAT_GetNodeSFN(FFatNode* pNode, t_wchar* psName, t_int32 dwLen);
	extern FFatErr	FFAT_GetNodeName(FFatNode* pNode, t_wchar* psName, t_int32 dwLen);

	extern FFatVol*	FFAT_GetVol(FFatNode* pNode);
	extern FFatErr	FFAT_GetRootNode(FFatVol* pVol, FFatNode* pNode);
	extern FFatErr	FFAT_GetRootNodePtr(FFatVol* pVol, FFatNode** ppNode);
	extern void*	FFAT_GetInode(FFatNode* pNode);

	// Retrieve node information from FFatNode
	extern t_uint32	FFAT_GetSize(FFatNode* pNode);
	extern t_uint32	FFAT_GetCluster(FFatNode* pNode);
	extern t_uint8	FFAT_GetAttribute(FFatNode* pNode);
	extern t_uint32	FFAT_GetLastAccessDate(FFatNode* pNode);
	extern t_uint32	FFAT_GetLastAccessTime(FFatNode* pNode);
	extern t_uint32	FFAT_GetWriteDate(FFatNode* pNode);
	extern t_uint32	FFAT_GetWriteTime(FFatNode* pNode);
	extern t_uint32	FFAT_GetCreateDate(FFatNode* pNode);
	extern t_uint32	FFAT_GetCreateTime(FFatNode* pNode);
	extern t_uint32	FFAT_GetParentCluster(FFatNode* pNode);
	extern t_uint32	FFAT_GetCacheCluster(FFatNode* pNode);
	extern t_uint32	FFAT_GetCacheOffset(FFatNode* pNode);
	extern t_uint32 FFAT_GetDeStartCluster(FFatNode* pNode);
	extern t_uint32	FFAT_GetDeStartOffset(FFatNode* pNode);

	// Get volume information
	extern t_int32	FFAT_GetClusterSize(FFatVol* pVol);
	extern FFatErr	FFAT_GetClusterSizeBits(FFatVol* pVol, FFatNode* pNode, t_int32* pdwClusterSizeBit);
	extern FFatErr	FFAT_GetClusterSizeMask(FFatVol* pVol, FFatNode* pNode, t_uint32* pdwClusterSizeMask);
	extern t_int32	FFAT_GetSectorSize(FFatVol* pVol);
	extern t_int32	FFAT_GetSectorSizeBits(FFatVol* pVol);
	extern FFatErr	FFAT_GetSectorSizeMask(FFatVol* pVol, FFatNode* pNode, t_uint32* pdwSectorSizeMask);
	extern FFatErr	FFAT_GetSectorPerCluster(FFatVol* pVol, FFatNode* pNode, t_int32* pdwSPC);
	extern FFatErr	FFAT_GetSectorPerClusterBits(FFatVol* pVol, FFatNode* pNode, t_int32* pdwBits);
	extern FFatErr	FFAT_GetSectorOfCluster(FFatVol* pVol, t_uint32 dwCluster, t_uint32* pdwSector);
	extern t_uint32	FFAT_GetFirstDataSector(FFatVol* pVol);
	extern t_uint32	FFAT_GetClusterCountPerFATSectorBits(FFatVol* pVol);

	extern t_boolean	FFAT_VolIsFAT32(FFatVol* pVol);
	extern t_boolean	FFAT_VolIsReadOnly(FFatVol* pVol);

	extern t_boolean	FFAT_NodeIsFile(FFatNode* pNode);
	extern t_boolean	FFAT_NodeIsDir(FFatNode* pNode);
	extern t_boolean	FFAT_NodeIsSymbolicFile(FFatNode* pNode);
	extern t_boolean	FFAT_NodeIsRoot(FFatNode* pNode);
	extern t_boolean	FFAT_NodeIsDirtySize(FFatNode* pNode);
	extern t_boolean	FFAT_NodeIsDirtySizeRDOnly(FFatNode* pNode);
	extern void			FFAT_NodeSetInode(FFatNode* pNode, void* pInode);
	extern void*		FFAT_NodeGetInode(FFatNode* pNode);
	extern FFatNode*	FFAT_GetChildPtr(FFatNode* pNode, t_boolean bReset);

	extern t_uint32		FFAT_GetFatSectorOfCluster(FFatNode* pNode);
	extern t_uint32		FFAT_GetOffsetOfFatSector(FFatNode* pNode);

	extern FFatErr		FFAT_GetSpinLock(void);
	extern FFatErr		FFAT_PutSpinLock(void);

	extern void			FFAT_ResetNodeStruct(FFatNode* pNode);

	extern FFatLDevInfo*	FFAT_GetLDevInfo(FFatVol* pVol);
	extern void			FFAT_SetLDevInfo(FFatVol* pVol, FFatLDevInfo* pLDevInfo);

	// Retrieve ADDON information
	extern FFatErr		FFAT_GetGUIDFromNode(FFatNode* pNode, void* pstXDEInfo);

	extern FFatErr		FFAT_CheckPermission(FFatNode* pNode);

// debug begin
#ifdef FFAT_DEBUG
	extern FFatErr	FFAT_ChangeFatTableFat32(FFatVol* pVol);
#endif
// debug end

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_API_H_ */

/**
 * ======================================================================
 *
 * Features
 *		Many widgets on ADDON module
 *			-. FFAT can expand with many widgets.
 *			-. Widgets are added to ADDON module.
 *			-. It can improvement filesystem performance.
 *			-. Widgets : Free Cluster Cache, Directory Entry Cache.
 *
 *		NAND Support
 *			-. erase sector when clusters are deallocated.
 *				--. It is implemented at ffat_addon_deallocateCluster()
 *				--. ffat_addon_deallocateCluster() -> ffat_addon_nand_deleteClusterChain()
 *					--> ffat_al_eraseSector().
 *				--. ffat_al_eraseSector() is at Porting Layer. user must check the device is NAND or not
 *					and then call proper function that provided by FTL.
 *
 *		Directory Truncation while file creation
 *			-. deallocate cluster for directory when there is additional clusters for the directory.
 *			-. FFAT reduces node lookup time and disk usage.
 *
 *		Optimized node creation 
 *			-. FFAT lookup a directory just one time for node creation.
 *
 *		IO with data type
 *			-. FFAT gives data type on device IO operation.
 *			-. data type : FAT, DE, Normal Data
 *			-. block device can use this information with block management.
 *
 *		Separate clusters between normal data and meta data
 *			-. FFAT manages clusters for directory.
 *
 *		Secure File Unlink
 *			-. FFAT can delete a file with securely
 *			-. File data can not be recovered after secure unlink operation.
 *			-. FFAT_SecureUnlink().
 *
 *		Directory Entry Cache
 *			-. It caches directory entry to improve lookup performance
 *			-. Not implemented yet.
 *
 *		Free Cluster Cache
 *			-. It manages free clusters to improve cluster allocation performance
 *			-. It manages clusters on the contiguous count to provide best fit allocation
 *			-. It also manage clusters on cluster number to provide contiguous allocation.
 *
 */
