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
 * @file		linux_logdisk.h
 * @brief		This file includes APIs to NativeFS for accessing logical disk.
 * @version		RFS_3.0.0_b035_RC
 * @see			none
 * @author		hayeong.kim@samsung.com
 */


#ifndef __LOGDISK_H__
#define __LOGDISK_H__

#include "rfs_linux.h"

/******************************************************************************/
/* STRUCTURE                                                                  */
/******************************************************************************/

/**
 * @struct	_LOGICAL_DISK
 * @brief	Internal data structure for representing an logical disk.
 * @see		Related Ldev function
 *
 */
struct _LOGICAL_DISK
{
	struct block_device stLxBDev; /**< linux block device */
};


/******************************************************************************/
/* NESTLE PUBLIC LDEV FUNCTIONS                                               */
/******************************************************************************/
/**
 * @brief	logical device IO control 
 */
FERROR
LdevIoControl(
		IN PLOGICAL_DISK 	pLogDisk,
		IN unsigned int		dwControlCode,
		IN void*			pInBuf,
		IN unsigned int		dwInBufSize,
		OUT void*			pOutBuf,
		IN unsigned int		dwOutBufSize);


/**
 * @brief	get a disk Infomation
 */
void
LdevGetDiskInfo(
		IN PLOGICAL_DISK 	pLogDisk,
		OUT unsigned int*	pdwNumSectors,
		OUT unsigned int*	pdwBytesPerSector,
		OUT unsigned int*	pdwStartSectorNum,
		OUT	unsigned int*	pdwFlags);

/**
 * @brief	erases sectors
 */
FERROR
LdevEraseSectors(
	IN	PLOGICAL_DISK		pLogDisk,
	IN	PDISK_SECTOR_RANGE	pSectorRange);

/**
 * @brief	read/write sectors
 */
FERROR
LdevReadWriteSectors(
	IN PLOGICAL_DISK		pLogDisk,
	IN unsigned int			dwStartSector,
	IN unsigned int			dwSectorCnt,
	IN OUT char*			pBuffer,
	IN DISK_OPERATE_TYPE	dwFlag);

#endif /* __LOGDISK_H__ */

// end of file
