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
 * @file        linux_volume.h
 * @brief       This file includes volume control operations and superblock operations.
 * @version     RFS_3.0.0_b035_RC
 * @see         none
 * @author      hayeong.kim@samsung.com
 */

#ifndef __VOLUME_H__
#define __VOLUME_H__

#include "rfs_linux.h"

/*
 * extern variables and functions
 */
extern PLINUX_KMEM_CACHE	g_pVnodeCache;

/******************************************************************************/
/* Nestle PUBLIC API : Superblock FUNCTION                                    */
/******************************************************************************/
/*
 * Prototypes for Linux VFS
 */

/**
 * @brief		get statistics on a file system (super_operation: statfs)
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
LINUX_ERROR
VolGetVolumeInformation(PLINUX_DENTRY pDentry, PLINUX_KSTATFS pStat);
#else
LINUX_ERROR
VolGetVolumeInformation(PLINUX_SUPER pSb, PLINUX_KSTATFS pStat);
#endif

/**
 * @brief		release the super block (unmount)
 */
void
VolUnmountVolume(PLINUX_SUPER pSb);

/**
 * @brief		write the superblock
 */
void
VolWriteVolume(PLINUX_SUPER pSb);

/**
 * @brief		show mount options
 */
LINUX_ERROR
VolShowOptions(PLINUX_SEQ_FILE pSeq, PLINUX_VFS_MNT plxMnt);

/**
 * @brief		allow to remount to make a writable file system readonly
 */
LINUX_ERROR 
VolRemountVolume(
	IN	PLINUX_SUPER	pSb, 
	IO	int*			pFlag, 
	IN	char*			pData);
/**
 *  @brief		Function to build super block structure (Mount function)
 */
LINUX_ERROR
VolFillSuper(PLINUX_SUPER pSb, void* pData, int nSilent);

#endif	/* __VOLUME_H__ */

// end of file
