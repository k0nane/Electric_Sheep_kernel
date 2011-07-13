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
 * @file		ffatfs_fat.h
 * @brief		FFATFS FAT module
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFATFS_FAT_H_
#define _FFATFS_FAT_H_

// includes
#include "ffat_types.h"
#include "ffatfs_types.h"

// defines
#define FAT12_RESERVED		0x0FF0UL		// start of reserved value range 
#define FAT12_BAD			0x0FF7UL		// bad cluster(block) 
#define FAT12_EOF			0x0FF8UL		// start of EOF range 
#define FAT12_EOC			0x0FFFUL		// EOC of FAT12
#define FAT12_FREE			0x0000UL		// free cluster

#define FAT16_RESERVED		0xFFF0UL		// start of reserved value range 
#define FAT16_BAD			0xFFF7UL		// bad cluster (block) 
#define FAT16_EOF			0xFFF8UL		// start of EOF range 
#define FAT16_EOC			0xFFFFUL		// EOC of FAT16
#define FAT16_END			0xFFFFUL		// end of reserved range 
#define FAT16_FREE			0x0000UL		// free cluster

#define FAT32_RESERVED		0x0FFFFFF0UL	// start of reserved value range 
#define FAT32_BAD			0x0FFFFFF7UL	// bad cluster(block) 
#define FAT32_EOF			0x0FFFFFF8UL	// start of EOF range 
#define FAT32_EOC			0x0FFFFFFFUL	// EOC of FAT32
#define FAT32_END			0x0FFFFFFFUL	// end of reserved range 
#define FAT32_FREE			0x00000000UL	// free cluster
#define FAT32_MASK			0x0FFFFFFFUL	// fat32 mask (most 4-bit)

#define FAT_FREE			0x00000000		// free cluster mark

// End Of File
#define IS_FAT12_EOF(_dwV)	((_dwV >= FAT12_EOF) ? FFAT_TRUE : FFAT_FALSE)
#define IS_FAT16_EOF(_dwV)	((_dwV >= FAT16_EOF) ? FFAT_TRUE : FFAT_FALSE)
#define IS_FAT32_EOF(_dwV)	(((_dwV & FAT32_MASK) >= FAT32_EOF) ? FFAT_TRUE : FFAT_FALSE)

// bad cluster
#define IS_FAT12_BAD(_dwV)	((_dwV == FAT12_BAD) ? FFAT_TRUE : FFAT_FALSE)
#define IS_FAT16_BAD(_dwV)	((_dwV == FAT16_BAD) ? FFAT_TRUE : FFAT_FALSE)
#define IS_FAT32_BAD(_dwV)	(((_dwV & FAT32_MASK) == FAT32_BAD) ? FFAT_TRUE : FFAT_FALSE)

// check is EOF
//	_pVI	: volume info
//	_dwV	: value

// Max Sector per cluster * sector size * Max == MAX file size
#define FAT12_MAX_FILE_SIZE		((128 * 512) * 4084)	// about 255MB
#define FAT16_MAX_FILE_SIZE		2147483647UL			// 2GB -1
#define FAT32_MAX_FILE_SIZE		4294967295UL			// 4GB -1

#define FFATFS_FREE_CLUSTER_INVALID			(0xFFFFFFFFUL - 10)	// free cluster count is invalid 
																// there is no cache free cluster count
																// do not change this value (Never, it will make hazard !!!)
																// same as FFAT_FREE_CLUSTER_INVALID

// check is there valid free cluster hint
#define FFATFS_IS_VALID_FREE_CLUSTER_HINT(_pVI)	\
	((VIC_FCH(VI_VIC(_pVI)) <= VI_LCN(_pVI)) ? FFAT_TRUE : FFAT_FALSE)

#define FFATFS_INC_FREE_CLUSTER_COUNT(_pVI, _dwC)	\
	(FFATFS_IsValidFreeClusterCount(_pVI) == FFAT_TRUE) ? (VI_VIC(_pVI)->dwFreeClusterCount += (_dwC)) : 0

#define FFATFS_DEC_FREE_CLUSTER_COUNT(_pVI, _dwC)	\
	(FFATFS_IsValidFreeClusterCount(_pVI) == FFAT_TRUE) ? (VI_VIC(_pVI)->dwFreeClusterCount -= (_dwC)) : 0

// get FAT sector number from cluster number
#define FFATFS_GET_FAT_SECTOR_OF_CLUSTER(_pVI, _dwC)	\
						(((_dwC) >> VI_CCPFSB(_pVI)) + VI_FFS(_pVI))

// get sector in a cluster with offset
// _pVI : volume info, _dwC : cluster number, _dwO : offset in a cluster
#define FFATFS_GET_SECTOR_OF_CLUSTER(_pVI, _dwC, _dwO)	\
						(VI_FDS(_pVI) + (((_dwC) - 2) << VI_SPCB(_pVI)) \
							+ ((_dwO) >> VI_SSB(_pVI)))

// get offset in FAT sector from cluster number
#define FFATFS_GET_OFFSET_OF_FAT_SECTOR(_pVI, _dwC)	\
						((_dwC) % (VI_CCPFS(_pVI)))

// get cluster number of a sector
//	_pVl : volume info, 
//	_dwC : cluster number
#define FFATFS_GET_CLUSTER_OF_SECTOR(_pVI, _dwS)	\
						((((_dwS) - VI_FDS(_pVI)) >> (VI_CSB(_pVI) - VI_SSB(_pVI))) + 2)

// get the first cluster number of a FAT sector
#define FFATFS_GET_FIRST_CLUSTER_ON_SECTOR(_pVI, _dwSector)	\
				((FFATFS_IS_FAT32(_pVI) == FFAT_TRUE) ? FFATFS_GET_FIRST_CLUSTER_ON_SECTOR32(_pVI, _dwSector) \
															: FFATFS_GET_FIRST_CLUSTER_ON_SECTOR16(_pVI, _dwSector))

// get the first cluster number on a FAT sector for FAT32
#define FFATFS_GET_FIRST_CLUSTER_ON_SECTOR32(_pVI, _dwSector)	\
						((_dwSector - VI_FFS(_pVI)) << VI_CCPFSB(_pVI))

// get the first cluster number on a FAT sector for FAT16
#define FFATFS_GET_FIRST_CLUSTER_ON_SECTOR16(_pVI, _dwSector)	\
						((_dwSector - VI_FFS(_pVI)) << VI_CCPFSB(_pVI))

// get the last cluster of a FAT sector
#define FFATFS_GET_LAST_CLUSTER_ON_SECTOR(_pVI, _dwSector)	\
						(FFATFS_GET_FIRST_CLUSTER_ON_SECTOR(_pVI, _dwSector) - 1)

// mark EOC on FAT sector buffer
//	_pVI	: volume information
//	_pBuff	: sector buffer
//	_C		: cluster number
//	_Data	: Cluster Value
//	_CPFSM	: Cluster Per FAT Sector Mask
#define FFATFS_UPDATE_FAT_BUFFER_EOC(_pVI, _pBuff, _C, _CPFSM)	\
	((FFATFS_IS_FAT32(_pVI) == FFAT_TRUE) ? FFATFS_UPDATE_FAT32(_pBuff, _C, FAT32_EOC, _CPFSM) : FFATFS_UPDATE_FAT16(_pBuff, _C, FAT16_EOC, _CPFSM))

// update FAT sector buffer, FAT16/32
//	_pVI	: volume information
//	_pBuff	: sector buffer
//	_C		: cluster number
//	_Data	: Cluster Value
//	_CPFSM	: Cluster Per FAT Sector Mask
#define FFATFS_UPDATE_FAT_BUFFER(_pVI, _pBuff, _C, _Data, _CPFSM)	\
	((FFATFS_IS_FAT32(_pVI) == FFAT_TRUE) ? FFATFS_UPDATE_FAT32(_pBuff, _C, _Data, _CPFSM) : FFATFS_UPDATE_FAT16(_pBuff, _C, _Data, _CPFSM))

// update FAT16 sector buffer
// _p16		: buffer pointer for FAT16
// _C		: cluster number
// _Data	: Cluster Value
// _CPFSM	: Cluster Per FAT Sector Mask
#define FFATFS_UPDATE_FAT16(_p16, _C, _Data, _CPFSM)	\
				((((t_uint16*)(_p16))[(_C) & (_CPFSM)]) = FFAT_BO_UINT16((t_uint16)(_Data & 0xFFFF)))

// update FAT32 sector buffer
// _p32		: buffer pointer for FAT32
// _C		: cluster number
// _Data	: Cluster Value
// _CPFSM	: Cluster Per FAT Sector Mask
#define FFATFS_UPDATE_FAT32(_p32, _C, _Data, _CPFSM)							\
				(((t_uint32*)(_p32))[(_C) & (_CPFSM)] = FFAT_BO_UINT32((t_uint32)(_Data)))

#ifdef FFAT_RESERVE_MOST_4BIT_ON_FAT_UPDATING
	// FAT spec denotes about reserving most 4 bit on FAT area for FAT32.
	// But Windows XP does not reserve it.
	// define "FFAT_RESERVE_MOST_4BIT_ON_FAT_UPDATING" to reserve it

	#undef FFATFS_UPDATE_FAT32
	#define FFATFS_UPDATE_FAT32(_p32, _C, _Data, _CPFSM)							\
				((t_uint32*)(_p32))[(_C) & (_CPFSM)] = FFAT_BO_UINT32((t_uint32)(_Data) | ((((t_uint32*)(_p32))[(_C) & (_CPFSM)]) & (~FAT32_MASK)))
#endif

//!< update a cluster
//<! _pVI	: [IN] volume pointer
//<! _C		: [IN] cluster number
//<! _V		: [IN] cluster data
//<! _F		: [IN] cache flag
//<! _F		: [IN] node pointer
#define FFAT_FS_FAT_UPDATECLUSTER(_pVI, _C, _V, _F, _N)		(_pVI)->pVolOp->pfUpdateCluster(_pVI, _C, _V, _F, _N)


// check the cluster is free or not on the FAT Sector buffer, FAT16/32
//	_pVI	: volume information
//	_pBuff	: sector buffer
//	_C		: cluster number
//	_CPFSM	: Cluster Per FAT Sector Mask
#define FFATFS_IS_FREE_CLUSTER_BUFFER(_pVI, _pBuff, _C, _CPFSM)	\
	((FFATFS_IS_FAT32(_pVI) == FFAT_TRUE) ? FFATFS_IS_FREE_CLUSTER_BUFFER_FAT32(_pBuff, _C, _CPFSM) : FFATFS_IS_FREE_CLUSTER_BUFFER_FAT16(_pBuff, _C, _CPFSM))

// update FAT16 sector buffer
// _p16		: buffer pointer for FAT16
// _C		: cluster number
// _Data	: Cluster Value
// _CPFSM	: Cluster Per FAT Sector Mask
#define FFATFS_IS_FREE_CLUSTER_BUFFER_FAT16(_p16, _C, _CPFSM)	\
	(((((t_uint16*)(_p16))[(_C) & (_CPFSM)]) == FAT16_FREE) ? FFAT_TRUE : FFAT_FALSE)

// update FAT32 sector buffer
// _p32		: buffer pointer for FAT32
// _C		: cluster number
// _Data	: Cluster Value
// _CPFSM	: Cluster Per FAT Sector Mask
#define FFATFS_IS_FREE_CLUSTER_BUFFER_FAT32(_p32, _C, _CPFSM)	\
	((((t_uint32*)(_p32))[(_C) & (_CPFSM)] == FAT32_FREE) ? FFAT_TRUE : FFAT_FALSE)

// update FAT buffer

// enum


// typedefs
typedef struct
{
	t_int32		dwSC1Count;		// SC1 index max
	t_int32		dwSC2Count;		// SC2 index max

	t_uint32	pdwSC1[FFATFS_SPARSE_CLUSTER_COUNT_LV1];
												// sparse cluster data level1
	t_uint32	pdwSC2[FFATFS_SPARSE_CLUSTER_COUNT_LV2];
												// sparse cluster data level2
} FatSparseCluster;

// typedefs
typedef struct _FFatfsFatMain
{
	t_int32				dwCluster;		//!< start cluster number
	t_int32				dwCount;	//!< cluster count
	PFN_FAT_CALLBACK	pfCallBack[FFATFS_FAT_CALLBACK_COUNT];		//!< callback function entries
} FFatfsFatMain;

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr ffat_fs_fat_init(void);
	extern FFatErr	ffat_fs_fat_terminate(void);

	extern FFatErr	ffat_fs_fat_getClusterOfOffset(FatVolInfo* pVI, t_uint32 dwCluster,
							t_uint32 dwOffset, t_uint32* pdwCluster);

	extern FFatErr	ffat_fs_fat_forwardCluster(FatVolInfo* pVI, t_uint32 dwCluster, 
							t_uint32 dwCount, t_uint32* pdwCluster);

	extern FFatErr	ffat_fs_fat_getNextCluster16(FatVolInfo* pVI, 
							t_uint32 dwCluster, t_uint32* pdwCluster);
	extern FFatErr	ffat_fs_fat_getNextCluster32(FatVolInfo* pVI, 
							t_uint32 dwCluster, t_uint32* pdwCluster);
	extern FFatErr	ffat_fs_fat_getLastCluster(FatVolInfo* pVI, 
							t_uint32 dwCluster, t_uint32* pdwLastCluster, t_uint32* pdwCount);

	extern FFatErr	ffat_fs_fat_getVectoredCluster(FatVolInfo* pVI, t_uint32 dwCluster, t_int32 dwCount,
							t_boolean bGetContiguous, FFatVC* pVC);

	extern t_boolean	ffat_fs_fat_isEOF16(t_uint32 dwCluster);
	extern t_boolean	ffat_fs_fat_isEOF32(t_uint32 dwCluster);

	extern FFatErr	ffat_fs_fat_getFreeFromTo16(FatVolInfo* pVI,t_uint32 dwFrom, t_uint32 dwTo, 
							t_uint32 dwCount, FFatVC* pVC, t_uint32* pdwFreeCount, t_boolean bGetMoreCluster);
	extern FFatErr	ffat_fs_fat_getFreeFromTo32(FatVolInfo* pVI,t_uint32 dwFrom, t_uint32 dwTo, 
							t_uint32 dwCount, FFatVC* pVC, t_uint32* pdwFreeCount, t_boolean bGetMoreCluster);
	extern FFatErr	ffat_fs_fat_getFreeClusters(FatVolInfo* pVI, t_uint32 dwCount,
							FFatVC* pVC, t_uint32 dwHint, t_boolean bGetMoreCluster);
	extern FFatErr	ffat_fs_fat_getFreeClustersFromTo(FatVolInfo* pVI, t_uint32 dwHint,
							t_uint32 dwFrom, t_uint32 dwTo, t_uint32 dwCount,
							FFatVC* pVC, t_uint32* pdwFreeCount, t_boolean bGetMoreCluster);
	extern FFatErr	ffat_fs_fat_getFreeClusterCountAt(FatVolInfo* pVI, t_uint32 dwSector,
							t_int32* pdwFreeCount);

	extern FFatErr	ffat_fs_fat_makeClusterChain(FatVolInfo* pVI, t_uint32 dwPrevEOF,
							t_int32 dwClusterCount, t_uint32* pdwClusters,
							FatUpdateFlag dwFUFlag, FFatCacheFlag dwCacheFlag, void* pNode);
	extern FFatErr	ffat_fs_fat_makeClusterChainVC(FatVolInfo* pVI, t_uint32 dwPrevEOF, 
							FFatVC* pVC, FatUpdateFlag dwFUFlag,
							FFatCacheFlag dwCacheFlag, void* pNode);

	extern FFatErr	ffat_fs_fat_deallocateCluster(FatVolInfo* pVI, t_uint32 dwCount,
							FatAllocate* pAlloc, t_uint32* pdwDeallocCount,
							t_uint32* pdwFirstDealloc, FatAllocateFlag dwFAFlag,
							FFatCacheFlag dwCacheFlag, void* pNode);
	extern FFatErr	ffat_fs_fat_freeClusters(FatVolInfo* pVI, t_int32 dwCount,
							t_uint32* pClusters, FFatCacheFlag dwFlag,
							FatAllocateFlag dwAllocFlag, void* pNode);

	extern FFatErr	ffat_fs_fat_updateCluster(FatVolInfo* pVI, t_uint32 dwCluster, 
							t_uint32 dwValue, FFatCacheFlag dwFlag);
	extern FFatErr	ffat_fs_fat_updateCluster16(FatVolInfo* pVI, t_uint32 dwCluster, 
							t_uint32 dwValue, FFatCacheFlag dwFlag, void* pNode);
	extern FFatErr	ffat_fs_fat_updateCluster32(FatVolInfo* pVI, t_uint32 dwCluster, 
							t_uint32 dwValue, FFatCacheFlag dwFlag, void* pNode);

	extern FFatErr	ffat_fs_fat_initCluster(FatVolInfo* pVI, t_uint32 dwCluster, t_int32 dwCount, 
							t_int8* pBuff, t_int32 dwSize, 
							FFatCacheFlag dwFlag, FFatCacheInfo* pCI);
	extern FFatErr	ffat_fs_fat_initPartialCluster(FatVolInfo* pVI, t_uint32 dwCluster, 
							t_int32 dwStartOffset, t_int32 dwSize, 
							FFatCacheFlag dwFlag, FFatCacheInfo* pCI);

	extern t_int32	ffat_fs_fat_readWriteCluster(FatVolInfo* pVI, t_uint32 dwCluster,
							t_int8* pBuff, t_int32 dwCount, 
							FFatCacheFlag dwFlag, FFatCacheInfo* pCI, t_boolean bRead);
	//2009.0716@chunum.kong_[fast read]_Cluster-unaligned-Read
	extern t_int32	ffat_fs_fat_readWritePartialCluster(FatVolInfo* pVI, t_uint32 dwCluster, 
							t_int32 dwStartOffset, t_int32 dwEndOffset, 
							t_int8* pBuff, FFatCacheFlag dwFlag,
							FFatCacheInfo* pCI, t_boolean bRead, t_boolean sectors);

	extern FFatErr	ffat_fs_fat_isFreeFatSector(FatVolInfo* pVI,
							t_uint32 dwSector, FFatCacheFlag dwFlag);
	extern FFatErr	ffat_fs_fat_getFirstClusterOfFatSector(FatVolInfo* pVI, t_uint32 dwSector,
							t_uint32* pdwClusterNo);
	extern FFatErr	ffat_fs_fat_getLastClusterOfFatSector(FatVolInfo* pVI, t_uint32 dwSector,
							t_uint32* pdwClusterNo);
	extern FFatErr	ffat_fs_fat_getFCCOfSector(FatVolInfo* pVI, t_uint32 dwSector,
							t_uint32* pdwClusterCount);


	extern FFatErr	ffat_fs_fat_sync2ndTo1st(FatVolInfo* pVI, t_int32 dwCount, 
								FatAllocate* pAlloc, FatAllocateFlag dwFlag);

	extern FFatErr	ffat_fs_fat_updateStatus(FatVolInfo* pVI, t_int8* pBuff, t_int32 dwSize);

	extern FFatErr ffat_fs_fat_freeClustersVC(FatVolInfo* pVI, FFatVC* pVC, 
						t_uint32* pdwFreeCount, FatDeallocateFlag dwDAFlag,
						FFatCacheFlag dwCacheFlag, void* pNode);
	extern FFatErr ffat_fs_fat_deallocateWithSC(FatVolInfo* pVI, FFatVC* pVC,
						t_uint32 dwFirstCluster, t_uint32* pdwFreeCount,
						FatAllocateFlag dwFlag, FFatCacheFlag dwCacheFlag,
						void* pNode);

	extern FFatErr ffat_fs_fat_deallocateClusterFromTo(FatVolInfo* pVI, t_uint32 dwPostEOF, t_uint32 dwCluster, t_uint32 dwCount,
						t_uint32* pdwDeallocCount, FatAllocateFlag dwFAFlag, FFatCacheFlag dwCacheFlag, void* pNode);

// debug begin
#ifdef FFAT_DEBUG
	extern FFatErr	ffat_fs_fat_changeFAT32FatTable(FatVolInfo* pVI);
#endif
// debug end

#if 0	// 현재 사용되지 않는 함수들
	extern FFatErr	ffat_fs_fat_allocateCluster(FatVolInfo* pVI, t_uint32 dwCount,
							FatAllocate* pAlloc, FatAllocateFlag dwFAFlag,
							FFatCacheFlag dwCacheFlag, void* pNode);

	extern FFatErr	ffat_fs_fat_callback(PFN_FAT_CALLBACK pFN, t_boolean bReg);
#endif

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFATFS_FAT_H_ */



