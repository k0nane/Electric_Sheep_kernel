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
 * @file		ffatfs_cache.h
 * @brief		The file defines FFatCache
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-08-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFATFS_CACHE_H_
#define _FFATFS_CACHE_H_

// includes
#include "ess_hash.h"
#include "ess_lru.h"

#include "ffat_types.h"

#include "ffatfs_types.h"

// defines
#define FFATFS_CACHE_SET_DIRTY(_pE)		(_pE)->dwFlag |= FFAT_CACHE_DIRTY

// typedefs
typedef struct _FFatfsCacheMain
{
	EssDList			dlCaches;		//!< created cache list
	t_int16				wCachecount;	//!< FFatfs cache count
	t_int16				wDummy;			//1< dummy
	PFN_CACHE_CALLBACK	pfCallBack[FFATFS_CACHE_CALLBACK_COUNT];	//!< callback function entries
	t_int8*				pCacheBuff[FFATFS_MAX_CACHE_COUNT];			//!< buffer pointer array for FFATFS cache
} FFatfsCacheMain;


typedef t_int16		FFatfsCacheInfoFlag;	// flag for Cache info
enum _FFatfsCacheInfoFlag
{
	FFATFS_CIFLAG_NONE			= 0x00,
	FFATFS_CIFLAG_USER			= 0x01,		// buffer for this cache is comes from user
	FFATFS_CIFLAG_AUTO_ADD		= 0x02		// This cache is added by BTFS automatically.
											// it will be freed by the BTFS
};

typedef struct _FFatfsCacheInfo
{
	t_uint16	wSectorSize;		//!< sector size in byte
	t_uint8		bSectorSizeBits;	//!< sector size in bit count
	t_uint8		bSig;				//!< signature
	t_int16		wRefCount;			//!< cache reference count
	t_int16		wFlag;				//!< flag for cache info

	EssHash		stHash;				//!< hash structure for cache management
	EssLru		stLru;				//!< LRU structure for cache management
	EssDList	dlDirty;			//!< dirty list
	EssDList	dlCache;			//!< for created cache list (connected to FFatfsCacheMain)
} FFatfsCacheInfo;

struct _FatVolInfo;

typedef struct _FFatfsCacheEntry
{
	EssDList 				stDListVol;		//!< for volume separation 
											//!< for volume level sync,
											//!< CAUTION!! -- do not move this entry to another place !!!
											//!< (It will makes a significant error)
	FFatCacheFlag			dwFlag;			//!< cache flag

	EssDList				dlDirty;
	EssHashEntry			stHashEntry;	//!< for HASH and LRU list
	EssLruEntry				stLruEntry;		//!< for HASH and LRU list

	struct _FatVolInfo*		pVolInfo; 		//!< volume information
											//!< we can get entry size and cache head from volume info.
	t_uint32				dwSectorNo;		//!< sector number of this entry
	t_int8* 				pBuff;			//!< buffer pointer
											//!< sector size
	t_uint16				wTimeStamp;		//!< time stamp to check volume change
	t_uint16				wDummy;

	void*					pNode;			//!< pointer for node 
											//!< NULL : this entry is occupied by two or more nodes
											//!<		No node information (check flag)
} FFatfsCacheEntry;

// constant definitions

// external variable declarations

// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr			ffat_fs_cache_init(void);
	extern FFatErr			ffat_fs_cache_terminate(void);

	extern t_boolean		ffat_fs_cache_entryCompare(void* pTarget, EssHashEntry* pHashEntry);

	extern FFatfsCacheInfo*	ffat_fs_cache_getCache(t_int32 wSectorSize);
	extern FFatErr			ffat_fs_cache_putCache(FFatfsCacheInfo* pCache);
	extern t_boolean		ffat_fs_cache_checkCache(t_int32 dwSectorSize, t_int8** ppBuff);

	extern FFatErr	ffat_fs_cache_addCache(t_int8* pBuff, t_int32 dwSize, t_int32 dwSectorSize);
	extern FFatErr	ffat_fs_cache_removeCache(t_int8* pBuff, ComCxt* pCxt);

	extern FFatErr	ffat_fs_cache_syncVol(FatVolInfo* pVolInfo, FFatCacheFlag dwFlag, ComCxt* pCxt);
	extern FFatErr	ffat_fs_cache_sync(FFatfsCacheInfo* pCache, FFatCacheFlag dwFlag, ComCxt* pCxt);
	extern FFatErr	ffat_fs_cache_syncAll(FFatCacheFlag dwFlag, ComCxt* pCxt);
	extern FFatErr	ffat_fs_cache_syncNode(FatVolInfo* pVolInfo, void* pNode, FFatCacheFlag dwFlag);
	extern FFatErr	ffat_fs_cache_syncEntry(FFatfsCacheInfo* pCache, struct _FatVolInfo* pVolInfo,
							FFatfsCacheEntry* pEntry, FFatCacheFlag dwFlag, ComCxt* pCxt);
	extern FFatErr	ffat_fs_cache_flushVol(FatVolInfo* pVolInfo, FFatCacheFlag dwFlag);

	extern t_int32	ffat_fs_cache_readSector(t_uint32 dwSector, t_int8* pBuff, t_int32 dwCount,
							FFatCacheFlag dwFlag, FatVolInfo* pVolInfo);
	extern t_int32	ffat_fs_cache_writeSector(t_uint32 dwSector, t_int8* pBuff, t_int32 dwCount,
							FFatCacheFlag dwFlag, FatVolInfo* pVI, void* pNode);

	extern FFatErr	ffat_fs_cache_getSector(t_uint32 dwSector, FFatCacheFlag dwFlag,
							FFatfsCacheEntry** ppEntry, FatVolInfo* pVI);
	extern FFatErr	ffat_fs_cache_putSector(FatVolInfo* pVolInfo, FFatfsCacheEntry* pEntry, FFatCacheFlag dwFlag, void* pNode);

	extern void		ffat_fs_cache_discard(FatVolInfo* pVolInfo, t_uint32 dwSector, t_int32 dwCount);

	extern FFatErr	ffat_fs_cache_callback(PFN_CACHE_CALLBACK pFN, t_boolean bReg);

	extern FFatErr	ffat_fs_cache_update(FatVolInfo* pVI, t_uint32 dwStartSector,
							t_int8* pBuff, t_int32 dwCount, FFatCacheFlag dwFlag);

// debug begin
#ifdef FFAT_DEBUG
	extern FFatfsCacheEntry*	ffat_fs_cache_lookupSector(FatVolInfo* pVolInfo, t_uint32 dwSector);
#endif
// debug end

#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFATFS_CACHE_H_ */

