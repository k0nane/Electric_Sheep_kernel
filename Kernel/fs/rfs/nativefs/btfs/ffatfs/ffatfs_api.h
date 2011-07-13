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
 * @file		ffatfs_api.h
 * @brief		API for FFATFS
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-28-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFATFS_API_H_
#define _FFATFS_API_H_

// includes
#include "ffat_config.h"
#include "ffat_al.h"

#include "ffatfs_main.h"
#include "ffatfs_types.h"
#include "ffatfs_bs.h"
#include "ffatfs_fat.h"
#include "ffatfs_de.h"


/* defines for API	*/
/* FAT module	*/
	/* check cluster validity	*/
#define FFATFS_IS_VALID_CLUSTER(_pVI, _dwCluster)		\
					((((_dwCluster) < ((_pVI)->dwClusterCount + 2)) && ((_dwCluster) >= 2)) ? FFAT_TRUE : FFAT_FALSE)

	/* get first sector of a cluster	*/
#define FFATFS_GET_FIRST_SECTOR(_pVI, dwC)	\
				(((dwC) < 2 || (dwC) > (VI_CC(_pVI)+ 1)) ? \
					0 : ( ((((dwC) - 2) << VI_SPCB(_pVI)) + VI_FDS(_pVI))))

/*
//
// NOTICE !!
//
//	아래 macro들에는 소문자가 포함되어 있다.
//	일반 API 처럼 쓰이는 macro 이므로 API와 통일 하기 위함이다.
*/

/* check validity of a cluster  */
/*	_pV		: VolInfo pointer	*/
/*	_dwC	: cluster number    */
#define FFATFS_IsValidCluster(_pVI, _dwC)				FFATFS_IS_VALID_CLUSTER(_pVI, _dwC)

/* get first sector of a cluster	*/
#define FFATFS_GetFirstSectorOfCluster(_pVI, _dwC)		FFATFS_GET_FIRST_SECTOR(_pVI, _dwC)

/* get sector offset from byte offset	*/
#define FFATFS_GetSectorOffset(_pVI, _dwO)				(_dwO >> VI_SSB(_pVI))

/* check free cluster count is valid or not	*/
#define FFATFS_IsValidFreeClusterCount(_pVI)			VI_IS_VALID_FCC(_pVI)
/* Set free cluster count */
#define FFATFS_SetFreeClusterCount(_pVI, _dwC)			VIC_SET_FCC(VI_VIC(_pVI), _dwC);

/* _dwC가 포함되어 있는 FAT Sector의 번호를 구한다.	*/
#define FFATFS_GetFatSectorOfCluster(_pVI, _dwC)		FFATFS_GET_FAT_SECTOR_OF_CLUSTER(_pVI, _dwC)

/* _dwC가 포함되어 있는 FAT Sector의 offset을 구한다.*/
#define FFATFS_GetOffsetOfFatSector(_pVI, _dwC)			FFATFS_GET_OFFSET_OF_FAT_SECTOR(_pVI, _dwC)

#define FFATFS_IS_FAT32(_pVI)							(((_pVI)->dwFatType == FFAT_FAT32) ? FFAT_TRUE : FFAT_FALSE)
#define FFATFS_IS_FAT16(_pVI)							(((_pVI)->dwFatType == FFAT_FAT16) ? FFAT_TRUE : FFAT_FALSE)

#define FFATFS_GetFatType(_pVI)						((_pVI)->dwFatType)
#define FFATFS_GetMaxFileSize(_pVI)					(FFATFS_IS_FAT32(_pVI) ? FAT32_MAX_FILE_SIZE : FAT16_MAX_FILE_SIZE)

#define FFATFS_IsDir(_pDE)							(((_pDE)->bAttr & FFAT_ATTR_DIR) ? FFAT_TRUE : FFAT_FALSE)
#define FFATFS_IsFile(_pDE)							(((_pDE)->bAttr & FFAT_ATTR_DIR) ? FFAT_FALSE : FFAT_TRUE)

#define FFATFS_IS_EOF(_pVI, _dwC)					((FFATFS_IS_FAT32(_pVI) == FFAT_TRUE) ? IS_FAT32_EOF(_dwC) : IS_FAT16_EOF(_dwC))
#define FFATFS_IS_BAD(_pVI, _dwC)					((FFATFS_IS_FAT32(_pVI) == FFAT_TRUE) ? IS_FAT32_BAD(_dwC) : IS_FAT16_BAD(_dwC))
//20100413_sks => Change to add the cluster notification function
#define INVALID_CLUSTER_COUNT		-1

//=============================================================================
//
//	DE Module 
//
//

/* get mode	*/
#define FFATFS_GetDeAttr(_pDE)			(((FatDeSFN*)(_pDE))->bAttr)
/* get creation time */
#define FFATFS_GetDeCTime(_pDE)			FFAT_BO_UINT16(((FatDeSFN*)(_pDE))->wCrtTime)
/* get creation date	*/
#define FFATFS_GetDeCDate(_pDE)			FFAT_BO_UINT16(((FatDeSFN*)(_pDE))->wCrtDate)
/* get creation time tenth	*/
#define FFATFS_GetDeCTimeTenth(_pDE)	(((FatDeSFN*)(_pDE))->bCrtTimeTenth)
/* get last access date	*/
#define FFATFS_GetDeADate(_pDE)			FFAT_BO_UINT16(((FatDeSFN*)(_pDE))->wLstAccDate)
/* get write time	*/
#define FFATFS_GetDeWTime(_pDE)			FFAT_BO_UINT16(((FatDeSFN*)(_pDE))->wWrtTime)
/* get write date	*/
#define FFATFS_GetDeWDate(_pDE)			FFAT_BO_UINT16(((FatDeSFN*)(_pDE))->wWrtDate)
/* get cluster number	*/
#define FFATFS_GetDeCluster(_pVI, _pDE)	(	\
										(FFATFS_IS_FAT32(_pVI) == FFAT_TRUE)	\
										? (((t_uint32)FFAT_BO_UINT16(((FatDeSFN*)(_pDE))->wFstClusLo)) |	\
											(((t_uint32)FFAT_BO_UINT16(((FatDeSFN*)(_pDE))->wFstClusHi)) << 16))	\
										: ((t_uint32)FFAT_BO_UINT16(((FatDeSFN*)(_pDE))->wFstClusLo))		\
										)

/* get size	*/
#define FFATFS_GetDeSize(_pDE)			FFAT_BO_UINT32(((FatDeSFN*)(_pDE))->dwFileSize)

/* enum		*/

/* typedefs	*/

/* constant definitions	*/

/* external variable declarations	*/

/* function declarations */


#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	FFATFS_Init(t_boolean bForce);
	extern FFatErr	FFATFS_Terminate(void);

	/* Boot Sector Functions ==================================================	*/
	extern FFatErr	FFATFS_Mount(FatVolInfo* pVI, FFatLDevInfo* pLDevInfo,
								void* pDev, ComCxt* pCxt, FFatMountFlag dwFlag);
	extern FFatErr	FFATFS_Umount(FatVolInfo* pVI, FFatCacheFlag dwFlag,
								ComCxt* pCxt);

	extern FFatErr	FFATFS_GetBSInfoFromBS(void* pDev, t_int32 dwIOSize,
								t_int32* pdwSectorSize, t_int32* pdwClusterSize,
								t_uint32* pdwFirstDataSector, ComCxt* pCxt);
	extern FFatErr	FFATFS_IsValidBootSector(t_int8* pBootSector);

	/* FFatfsCache functions. =================================================	*/
	extern FFatErr	FFATFS_SyncNode(FatVolInfo* pVI, void* pNode,
								FFatCacheFlag dwFlag, ComCxt* pCxt);
	extern FFatErr	FFATFS_SyncVol(FatVolInfo* pVI, FFatCacheFlag dwFlag, ComCxt* pCxt);
	extern FFatErr	FFATFS_FlushVol(FatVolInfo* pVI, FFatCacheFlag dwFlag, ComCxt* pCxt);
	extern FFatErr	FFATFS_AddCache(t_int8* pBuff, t_int32 dwSize,
								t_int32 dwSectorSize, ComCxt* pCxt);
	extern FFatErr	FFATFS_RemoveCache(t_int8* pBuff, ComCxt* pCxt);
	extern FFatErr	FFATFS_RegisterCacheCallback(PFN_CACHE_CALLBACK pFN,
								t_boolean bReg, ComCxt* pCxt);
	extern FFatErr	FFATFS_ChkCache(FFatCheckCache* pChkCache, ComCxt* pCxt);
	extern FFatErr	FFATFS_DiscardCache(FatVolInfo* pVI, t_uint32 dwSector,
								t_int32 dwCount, ComCxt* pCxt);

	/* FAT functions ==========================================================	*/
	extern FFatErr	FFATFS_GetClusterOfOffset(FatVolInfo* pVI, t_uint32 dwCluster,
								t_uint32 dwOffset, t_uint32* pdwCluster, ComCxt* pCxt);
	extern FFatErr	FFATFS_GetNextCluster(FatVolInfo* pVI, t_uint32 dwCluster,
								t_uint32* pdwNextCluster, ComCxt* pCxt);
	extern FFatErr	FFATFS_GetLastCluster(FatVolInfo* pVI, t_uint32 dwCluster, 
								t_uint32* pdwLastCluster, t_uint32* pdwCount, ComCxt* pCxt);

	extern FFatErr	FFATFS_GetFreeClusters(FatVolInfo* pVI, t_uint32 dwCount,
								FFatVC* pVC, t_uint32 dwHint, t_boolean bGetMoreCluster,
								ComCxt* pCxt);
	extern FFatErr	FFATFS_GetFreeClustersFromTo(FatVolInfo* pVI, t_uint32 dwHint, 
								t_uint32 dwFrom, t_uint32 dwTo, t_uint32 dwCount,
								FFatVC* pVC, t_uint32* pdwFreeCount, t_boolean bGetMoreCluster,
								ComCxt* pCxt);

	extern FFatErr	FFATFS_MakeClusterChain(FatVolInfo* pVI, t_uint32 dwPrevEOF,
								t_int32 dwClusterCount, t_uint32* pdwClusters,
								FatUpdateFlag dwFlag, FFatCacheFlag dwCacheFlag,
								void* pNode, ComCxt* pCxt);
	extern FFatErr	FFATFS_MakeClusterChainVC(FatVolInfo* pVI, t_uint32 dwPrevEOF, 
								FFatVC* pVC, FatUpdateFlag dwFUFlag,
								FFatCacheFlag dwCacheFlag, void* pNode, ComCxt* pCxt);

	extern void		FFATFS_IncFreeClusterCount(FatVolInfo* pVI, t_uint32 dwCount, ComCxt* pCxt);
	extern void		FFATFS_DecFreeClusterCount(FatVolInfo* pVI, t_uint32 dwCount, ComCxt* pCxt);

	extern FFatErr	FFATFS_GetVectoredCluster(FatVolInfo* pVI, t_uint32 dwCluster,
								t_int32 dwCount, FFatVC* pVC, t_boolean bGetContiguous,
								ComCxt* pCxt);

	extern FFatErr	FFATFS_AllocateCluster(FatVolInfo* pVI, t_int32 dwCount,
								FatAllocate* pAlloc, FatAllocateFlag dwFAFlag,
								FFatCacheFlag dwCacheFlag, void* pNode, ComCxt* pCxt);
	extern FFatErr	FFATFS_DeallocateCluster(FatVolInfo* pVI, t_int32 dwCount,
								FatAllocate* pAlloc, t_uint32* pdwDeallocCount,
								t_uint32* pdwFirstDealloc,
								FatAllocateFlag dwFAFlag, FFatCacheFlag dwCacheFlag,
								void* pNode, ComCxt* pCxt);
	extern FFatErr FFATFS_DeallocateClusterFromTo(FatVolInfo* pVI, t_uint32 dwPostEOF,
								t_uint32 dwCluster, t_uint32 dwCount,
								t_uint32* pdwDeallocCount, FatAllocateFlag dwFAFlag,
								FFatCacheFlag dwCacheFlag, void* pNode, ComCxt* pCxt);

	extern FFatErr	FFATFS_FreeClusters(FatVolInfo* pVI, t_int32 dwCount,
								t_uint32* pClusters, FFatCacheFlag dwFlag,
								FatAllocateFlag dwAllocFlag, void* pNode, ComCxt* pCxt);
	extern FFatErr	FFATFS_UpdateCluster(FatVolInfo* pVI, t_uint32 dwCluster,
								t_uint32 dwValue, FFatCacheFlag dwFlag,
								void* pNode, ComCxt* pCxt);

	extern FFatErr	FFATFS_UpdateVolumeStatus(FatVolInfo* pVI, t_int8* pBuff,
								t_int32 dwSize, ComCxt* pCxt);

	extern FFatErr	FFATFS_IsFreeFatSector(FatVolInfo* pVI, t_uint32 dwSector,
								FFatCacheFlag dwFlag, ComCxt* pCxt);
	extern FFatErr	FFATFS_GetFirstClusterOfFatSector(FatVolInfo* pVI,
								t_uint32 dwSector, t_uint32* pdwClusterNo);
	extern FFatErr	FFATFS_GetLastClusterOfFatSector(FatVolInfo* pVI,
								t_uint32 dwSector, t_uint32* pdwClusterNo);

	extern FFatErr	FFATFS_GetFCCOfSector(FatVolInfo* pVI, t_uint32 dwSector,
								t_uint32* pdwClusterCount, ComCxt* pCxt);

	/* Directory Entry functions ==============================================	*/
	extern FFatErr	FFATFS_GetNodeDirEntry(FatVolInfo* pVI, FatGetNodeDe* pNodeDE,
								ComCxt* pCxt);
	extern FFatErr	FFATFS_GenNameFromDirEntry(FatVolInfo* pVI, FatDeSFN* pDE,
								t_int32 dwEntryCount, t_wchar* psName,
								t_int32* pdwLen, FatGenNameFlag dwFlag);

	extern FFatErr	FFATFS_AdjustNameToFatFormat(FatVolInfo* pVI, t_wchar* psName,
								t_int32* pdwLen, t_int32* pdwNamePartLen,
								t_int32* pdwExtPartLen, t_int32* pdwSfnNameSize,
								FatNameType* pdwType, FatDeSFN* pDE);
	extern FFatErr	FFATFS_RemoveTrailingDotAndBlank(t_wchar* psName, t_int32* pdwLen);
	extern t_int32	FFATFS_GetNumericTail(FatDeSFN* pDE);

	extern FFatErr	FFATFS_SetDeTime(FatDeSFN* pDE, FatDeUpdateFlag dwFlag, FFatTime* pTime);
	extern FFatErr	FFATFS_SetDeSize(FatDeSFN* pDE, t_uint32 dwSize);
	extern FFatErr	FFATFS_SetDeAttr(FatDeSFN* pDE, t_uint8 bAttr);
	extern FFatErr	FFATFS_SetDeCluster(FatDeSFN* pDE, t_uint32 dwCluster);

	extern t_uint8	FFATFS_GetCheckSum(FatDeSFN* pDE);

#ifdef FFAT_VFAT_SUPPORT
	extern FFatErr	FFATFS_GenLFNE(t_wchar* psName, t_int16 wNameLen, FatDeLFN* pDE,
								t_int32* pdwEntryCount, t_uint8 bCheckSum);
	extern FFatErr	FFATFS_FindLFNEsInCluster(FatVolInfo* pVI, t_uint32 dwCluster,
								t_uint32 dwStartOffset, t_uint8 bCheckSum,
								t_uint8* pbPrevLFNOrder, t_uint32* pdwFoundLFNECount, ComCxt* pCxt);
#endif

	extern FFatErr	FFATFS_ReadWriteOnRoot(FatVolInfo* pVI, t_int32 dwOffset,
								t_int8* pBuff, t_int32 dwSize, FFatCacheFlag dwFlag,
								t_boolean bRead, void* pNode, ComCxt* pCxt);
	extern FFatErr	FFATFS_WriteDE(FatVolInfo* pVI, t_uint32 dwCluster, t_int32 dwOffset,
								t_int8* pBuff, t_int32 dwSize, FFatCacheFlag dwFlag,
								void* pNode, ComCxt* pCxt);
	extern FFatErr	FFATFS_DeleteDE(FatVolInfo* pVI, t_uint32 dwCluster, t_int32 dwOffset,
								t_int32 dwCount, t_boolean bLookupDelMark, FFatCacheFlag dwFlag,
								void* pNode, ComCxt* pCxt);

	/* MISC functions ==========================================================*/
	extern FFatErr	FFATFS_SetVolumeLabel(FatVolInfo* pVI, t_wchar* psVolLabel, ComCxt* pCxt);
	extern FFatErr	FFATFS_GetVolumeLabel(FatVolInfo* pVI, t_wchar* psVolLabel,
								t_int32 dwVolLabelLen, ComCxt* pCxt);

	extern FFatErr	FFATFS_FSCtl(FatFSCtlCmd dwCmd, void* pParam0, void* pParam1, void* pParam2);

	extern FFatErr	FFATFS_InitCluster(FatVolInfo* pVI, t_uint32 dwCluster, t_int32 dwCount,
								t_int8* pBuff, t_int32 dwSize, 
								FFatCacheFlag dwFlag, FFatCacheInfo* pCI, ComCxt* pCxt);
	extern FFatErr	FFATFS_InitPartialCluster(FatVolInfo* pVI, t_uint32 dwCluster, 
								t_int32 dwStartOffset, t_int32 dwSize, 
								FFatCacheFlag dwFlag, FFatCacheInfo* pCI, ComCxt* pCxt);

	extern t_int32	FFATFS_ReadWriteCluster(FatVolInfo* pVI, t_uint32 dwCluster, t_int8* pBuff,
								t_int32 dwCount, t_boolean bRead, 
								FFatCacheFlag dwFlag, FFatCacheInfo* pCI, ComCxt* pCxt);
	//2009.0716@chunum.kong_[fast read]_Cluster-unaligned-Read
	extern t_int32	FFATFS_ReadWritePartialCluster(FatVolInfo* pVI, t_uint32 dwCluster,
								t_int32 dwStartOffset, t_int32 dwSize, t_int8* pBuff,
								t_boolean bRead, FFatCacheFlag dwFlag, FFatCacheInfo* pCI,
								ComCxt* pCxt, t_boolean sectors);

	extern t_int32	FFATFS_ReadWriteSectors(FatVolInfo* pVI, t_uint32 dwSector,
								t_int32 dwCount, t_int8* pBuff, FFatCacheFlag dwFlag,
								FFatCacheInfo* pCI,t_boolean bRead, ComCxt* pCxt);
	extern t_int32	FFATFS_ReadWritePartialSector(FatVolInfo* pVI, t_uint32 dwSector, 
								t_int32 dwStartOffset, t_int32 dwSize, 
								t_int8* pBuff, FFatCacheFlag dwFlag, FFatCacheInfo* pCI,
								t_boolean bRead, ComCxt* pCxt);

	extern FFatErr	FFATFS_FreeClusterVC(FatVolInfo* pVI, FFatVC* pVC,
								t_uint32* pdwFreeCount, FatAllocateFlag dwFAFlag,
								FFatCacheFlag dwCacheFlag, void* pNode, ComCxt* pCxt);

	extern FFatErr	FFATFS_Lock(ComCxt* pCxt);
	extern FFatErr	FFATFS_UnLock(ComCxt* pCxt);

// debug begin
#ifdef FFAT_DEBUG
	extern FFatErr FFATFS_ChangeFatTableFAT32(FatVolInfo* pVI);
#endif
// debug end

#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFATFS_API_H_ */

