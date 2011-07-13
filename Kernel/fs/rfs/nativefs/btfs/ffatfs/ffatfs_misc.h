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
 * @file		ffatfs_misc.h
 * @brief		miscellaneous items for FFatfs 
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-12-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFATFS_MISC_H_
#define _FFATFS_MISC_H_

// includes
#include "ffatfs_types.h"


// defines

// enum

// typedefs

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_fs_misc_initPartialSector(FatVolInfo* pVolInfo, t_uint32 dwSector, 
						t_int32 dwStartOffset, t_int32 dwSize,
						t_int8* pBuff, FFatCacheFlag dwFlag, FFatCacheInfo* pCI);

	extern t_int32	ffat_fs_misc_readWritePartialSector(FatVolInfo* pVolInfo, t_uint32 dwSector, 
						t_int32 dwStartOffset, t_int32 dwSize, 
						t_int8* pBuff, FFatCacheFlag dwFlag, FFatCacheInfo* pCI,
						t_boolean bRead);
	
	extern t_int32	ffat_fs_misc_readWriteSectors(FatVolInfo* pVolInfo, t_uint32 dwSector,
						t_int32 dwCount, t_int8* pBuff,
						FFatCacheFlag dwFlag, FFatCacheInfo* pCI,t_boolean bRead);

	extern void		ffat_debug_printDE(t_int8* pDE);
	extern void		ffat_debug_printGetNodeDe(FatGetNodeDe* pNodeDe);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFATFS_MISC_H_ */



