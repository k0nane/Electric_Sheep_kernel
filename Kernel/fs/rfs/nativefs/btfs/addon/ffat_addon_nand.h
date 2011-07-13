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
 * @file		ffat_addon_nand.h
 * @brief		Module for NAND Flash Support 
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */


#ifndef _FFAT_ADDON_NAND_H_
#define _FFAT_ADDON_NAND_H_

// includes

// defines

// enum

// typedefs

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_nand_mount(Vol* pVol, FFatMountFlag dwFlag, ComCxt* pCxt);

	extern FFatErr	ffat_nand_cleanNand(Vol* pVol, FFatCleanNand* pCN, ComCxt* pCxt);
	extern FFatErr	ffat_nand_deleteClusterChain(Vol* pVol, FFatVC* pVC,
								t_uint32 dwCluster, t_uint32 dwCount, ComCxt* pCxt);
	extern FFatErr	ffat_nand_cleanVolume(Vol* pVol, ComCxt* pCxt);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_TEMPLATE_H_ */



