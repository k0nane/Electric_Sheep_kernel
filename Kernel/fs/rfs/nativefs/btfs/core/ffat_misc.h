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
 * @file		ffat_misc.h
 * @brief		The global configuration for FFAT
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */


#ifndef _FFAT_MISC_H_
#define _FFAT_MISC_H_

//!< includes
#include "ffat_main.h"
#include "ffat_vol.h"
#include "ffat_node.h"

//!< defines

//!< enum

//!< typedefs

//!< constant definitions

//!< external variable declarations


//!< function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_misc_getClusterOfOffset(Node* pNode, t_uint32 dwOffset,
							t_uint32* pdwCluster, NodePAL* pPAL, ComCxt* pCxt);
	extern FFatErr	ffat_misc_getNextCluster(Node* pNode, t_uint32 dwCluster,
							t_uint32* pdwNextCluster, ComCxt* pCxt);
	extern FFatErr	ffat_misc_getLastCluster(Vol* pVol, t_uint32 dwCluster,
							t_uint32* pdwLastCluster, t_uint32* pdwCount, ComCxt* pCxt);

	extern FFatErr	ffat_misc_getFreeClusterForDir(Node* pNode, FFatVC* pstVC,
							t_uint32 dwClusterCount, ComCxt* pCxt);
	extern FFatErr	ffat_misc_getFreeClusters(Node* pNode, t_uint32 dwCount,
							FFatVC* pVC, t_uint32 dwHint,
							t_uint32* pdwFreeCount,
							FatAllocateFlag dwAllocFlag, ComCxt* pCxt);
	extern FFatErr	ffat_misc_allocateCluster(Node* pNode, t_uint32 dwEOC, t_uint32 dwCount,
							FFatVC* pVC, FFatVCE* pLastVCE, FatAllocateFlag dwFlag,
							FFatCacheFlag dwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_misc_deallocateCluster(Node* pNode, t_uint32 dwNewEOC,
							t_uint32 dwFirstCluster, t_int32 dwCount,
							FFatVC* pVC, FatAllocateFlag dwFAFlag,
							FFatCacheFlag dwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_misc_makeClusterChain(Node* pNode, t_uint32 dwPrevEOF, t_int32 dwClusterCount, 
							t_uint32* pdwClusters, FatUpdateFlag dwFUFlag, FFatCacheFlag dwCacheFlag, ComCxt* pCxt);
	extern FFatErr	ffat_misc_makeClusterChainVC(Node* pNode, t_uint32 dwPrevEOF,
							FFatVC* pVC, FatUpdateFlag dwFUFlag,
							FFatCacheFlag dwCacheFlag, ComCxt* pCxt);

	extern FFatErr	ffat_misc_getVectoredCluster(Vol* pVol, Node* pNode, t_uint32 dwCluster,
							t_uint32 dwOffset, t_uint32 dwCount, FFatVC* pVC,
							NodePAL* pPAL, ComCxt* pCxt);
	extern void		ffat_misc_decFreeClusterCount(Node* pNode, t_uint32 dwCount, ComCxt* pCxt);

	extern FFatErr	ffat_misc_ldevIO(Vol* pVol, FFatLDevIO* pLDevIO, ComCxt* pCxt);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_MISC_H_ */



