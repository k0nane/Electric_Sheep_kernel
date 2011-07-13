/*
 * RFS 3.0 Developed by Flash Software Group.
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
 * @file        natives.h
 * @brief       This file includes APIs for accessing Native Filesystems.
 * @version     RFS_3.0.0_b035_RC
 * @see         none
 * @author      hayeong.kim@samsung.com
 */

#ifndef __NATIVES_H__
#define __NATIVES_H__


#include "rfs_linux.h"

/******************************************************************************/
/* Native Operation Set                                                       */
/******************************************************************************/


/******************************************************************************/
/* Nestle Private API: Native Function                                        */
/******************************************************************************/
PNATIVEFS_OPERATIONS NativefsGetNative(const char *pName);

FERROR NativefsInitialize(IN PNATIVEFS_OPERATIONS pNativeFSOps);

FERROR NativefsUninitialize(IN PNATIVEFS_OPERATIONS pNativeFSOps);

FERROR
NativefsMountDisk(
		PLINUX_SUPER 			pSb,
		PVOLUME_CONTROL_BLOCK	pVcb,
		PMOUNT_FLAG 			pdwMountFlag,
		PVNODE *				ppRootVnode);

extern PNATIVEFS_OPERATIONS GetBTFS(void);
extern PNATIVEFS_OPERATIONS (*GetNativeBTFS)(void);

#endif

// end of file
