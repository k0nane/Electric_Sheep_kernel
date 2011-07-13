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
 * @file		ffat_addon_fcc.h
 * @brief		Free Cluster Cache Module
 *				It manages free cluster information for fast cluster allocation
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		OCT-31-2006 [DongYoung Seo] First writing
 * @see			None
 */
 
#ifndef _FFAT_ADDON_FCC_H_
#define _FFAT_ADDON_FCC_H_

// includes
#include "ess_rbtree2.h"
#include "ess_dlist.h"

#include "ffat_vol.h"
#include "ffatfs_types.h"

// defines

// enum

// typedefs
typedef struct
{
	t_uint32		dwFreeClusterCount;
	EssRBTree2		stRBTreeClusterHead; 			//!< tree head for cluster number, contiguous allocation
	EssRBTree2		stRBTreeContCountHead;		//!< tree head for contiguous count, best fit allocation

	EssDList		dlDirtyList;			//!< list for dirty list
	EssList			slFree;					//!< list for free FCC Info List
} FCCVolInfo;


// constant definitions


// external variable declarations


// function declarations

#ifdef __cplusplus
 extern "C" {
#endif

	extern FFatErr  ffat_fcc_init(void);
	extern FFatErr  ffat_fcc_terminate(void);

	extern FFatErr  ffat_fcc_mount(Vol* pVol);
	extern FFatErr  ffat_fcc_umount(Vol* pVol, FFatMountFlag dwFlag, ComCxt* pCxt);

	extern FFatErr  ffat_fcc_syncVol(Vol* pVol, FFatCacheFlag dwCFlag, ComCxt* pCxt);
	extern FFatErr	ffat_fcc_deallocateCluster(Node* pNode, t_uint32 dwNewEOC, 
								t_uint32* pdwFirstCluster, t_uint32 dwCount,
								t_uint32* pdwDeallocCount, FFatVC* pVC,
								FatAllocateFlag dwFAFlag, FFatCacheFlag dwCacheFlag,
								ComCxt* pCxt);
	extern FFatErr	ffat_fcc_getFreeClusters(Node* pNode, t_uint32 dwCount, FFatVC* pVC,
								t_uint32 dwHint, t_uint32* pdwFreeCount, ComCxt* pCxt);
	extern FFatErr	ffat_fcc_getFreeClustersFromTo(Vol* pVol, t_uint32 dwHint, t_uint32 dwFrom, 
								t_uint32 dwTo, t_uint32 dwCount, FFatVC* pVC,
								t_uint32* pdwFreeCount, ComCxt* pCxt);
	extern FFatErr	ffat_fcc_getFCCOfSector(Vol* pVol, t_uint32 dwSector, t_uint32* pdwFCC,
								ComCxt* pCxt);
	extern FFatErr	ffat_fcc_getVolumeStatus(Vol* pVol, t_int8* pBuff, t_int32 dwSize,
								ComCxt* pCxt);
	extern FFatErr	ffat_fcc_addFreeClusters(Vol* pVol, t_uint32 dwCluster, t_uint32 dwCount,
								ComCxt* pCxt);
	extern FFatErr	ffat_fcc_addFreeClustersVC(Vol* pVol, FFatVC* pVC, ComCxt* pCxt);

// debug begin
#ifdef FFAT_DEBUG
	extern FFatErr  ffat_fcc_addFreeClustersFromTo(Vol* pVol, t_uint32 dwStartVolume, 
								t_uint32 dwEndVolume, ComCxt* pCxt);
#endif
// debug end

#ifdef __cplusplus
 };
#endif

#endif  /* #ifndef _FFAT_ADDON_FCC_H_ */
