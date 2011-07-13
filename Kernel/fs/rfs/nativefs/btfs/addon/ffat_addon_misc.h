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
 * @file		ffat_addon_misc.h
 * @brief		The miscellaneous function from addon module
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFAT_ADDON_MISC_H_
#define _FFAT_ADDON_MISC_H_

// includes
#include "ffat_api.h"
#include "ffat_node.h"
#include "ffat_vol.h"

// defines

// enum

// typedefs

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_addon_misc_readdirStat(Node* pNode, FFatReaddirStatInfo* pRSI,
								ComCxt* pCxt);
	extern FFatErr	ffat_addon_misc_readdirGetNode(Node* pNode,
								FFatReaddirGetNodeInfo* pRGNI, ComCxt* pCxt);
	extern FFatErr	ffat_addon_misc_cleanDir(Node* pNode, ComCxt* pCxt);
	extern FFatErr	ffat_addon_misc_readdirUnlink(Node* pNode,
								FFatReaddirUnlinkInfo* pRUI, ComCxt* pCxt);
	extern FFatErr	ffat_addon_misc_secureDeallocate(Vol* pVol, t_uint32 dwCluster,
								t_uint32 dwCount, ComCxt* pCxt);

#ifdef FFAT_VFAT_SUPPORT
	extern FFatErr	ffat_addon_misc_getShortLongName(FFatGetShortLongName* pGSLN,
								FFatFSCtlCmd dwCmd, ComCxt* pCxt);
#endif

	extern FFatErr	ffat_addon_misc_isFreeFATSector(Vol* pVol, t_uint32 dwSectorNo,
								ComCxt* pCxt);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_TEMPLATE_H_ */



