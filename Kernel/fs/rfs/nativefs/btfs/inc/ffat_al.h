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
 * @file		ffat_al.h
 * @brief		The file defines Abstraction layer for FFAT
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-20-2006 [DongYoung Seo] First writing
 * @see			None
 */


#ifndef _FFAT_AL_H_
#define _FFAT_AL_H_

// includes
#include "ffat_config.h"
#include "ffat_types.h"


// enum


// typedefs


// constant definitions


// external variable declarations

// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_al_init(void);
	extern FFatErr	ffat_al_terminate(void);

	// lock part
	extern FFatErr	ffat_al_initLock(void);
	extern FFatErr	ffat_al_terminateLock(void);
	extern FFatErr	ffat_al_getFreeLock(FFatLock** ppLock);
	extern FFatErr	ffat_al_releaseLock(FFatLock** ppLock);
	extern FFatErr	ffat_al_getLock(FFatLock* pLock);
	extern FFatErr	ffat_al_putLock(FFatLock* pLock);

// debug begin
#ifdef FFAT_DEBUG
	extern FFatErr	ffat_dbg_lock(FFatLock* pLock);
	extern FFatErr	ffat_dbg_unlock(FFatLock* pLock);
#endif
// debug end

	// TTY
	extern t_int32	ffat_getChar(void);

	// time
	extern FFatErr	ffat_localTime(FFatTime* pTime);

	// MEMORY
	extern FFatErr	ffat_al_initMem(void);
	extern FFatErr	ffat_al_terminateMem(void);
	extern void*	ffat_al_allocMem(t_int32 dwSize, EssMallocFlag dwMallocFlag);
	extern void		ffat_al_freeMem(void* p, t_int32 dwSize);
	extern t_int8*	ffat_al_getPStackBuf(void);
	extern t_int32	ffat_al_getPStackBufSize(void);

	// block device IO (buffer cache)
	extern FFatErr	ffat_al_initBlockIO(void);
	extern FFatErr	ffat_al_termianteBlockIO(void);

	extern t_int32	ffat_al_readSector(t_uint32 dwSector, t_int8* pBuff, t_int32 dwCount,
							FFatCacheFlag dwFlag, FFatCacheInfo* pCI);
	extern t_int32	ffat_al_readSector2(t_uint32 dwSector, t_int8* pBuff, t_int32 dwCount,
							FFatCacheFlag dwFlag, FFatCacheInfo* pCI);
	extern t_int32	ffat_al_readSectorVS(FFatVS* pVS, FFatCacheFlag dwFlag, FFatCacheInfo* pCI);

	extern t_int32	ffat_al_writeSector(t_uint32 dwSector, t_int8* pBuff, t_int32 dwCount,
							FFatCacheFlag dwFlag, FFatCacheInfo* pCI);
	extern t_int32	ffat_al_writeSectorVS(FFatVS* pVS, FFatCacheFlag dwFlag, FFatCacheInfo* pCI);

	extern t_int32	ffat_al_eraseSector(t_uint32 dwSector, t_int32 dwCount,
							FFatCacheFlag dwFlag, FFatCacheInfo* pCI);

	extern FFatErr	ffat_al_sync(FFatCacheFlag dwFlag, FFatCacheInfo* pCI);
	extern FFatErr	ffat_al_syncDev(FFatCacheFlag dwFlag, FFatCacheInfo* pCI);

	extern t_int32	ffat_ldev_writeSector(void* pDevice, t_uint32 dwSector,
							t_int8* pBuff, t_int32 dwCount);
	extern t_int32	ffat_ldev_eraseSector(void* pDevice, t_uint32 dwSector, t_int32 dwCount);

	// for dynamic configuration & tuning
	extern FFatErr	ffat_al_initConfig(void);
	extern FFatErr	ffat_al_terminateConfig(void);

	extern FFatConfig*	ffat_al_getConfig(void);
	//20100413_sks => Change to add the cluster notification function
	extern void ffat_al_cluster_notify(t_int32 dwFreeCount, t_int32 dwTotalCount, t_int32 dwClustersize,void* pDevice);


#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_AL_H_ */



