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
 * @file		linux_bcache.h
 * @brief		This file includes APIs to NativeFS for accessing buffer cache.
 * @version		RFS_3.0.0_b035_RC
 * @see			none
 * @author		hayeong.kim@samsung.com
 */

#ifndef __BCACHE_H__
#define __BCACHE_H__

#include "rfs_linux.h"

/******************************************************************************/
/* NESTLE PUBLIC BCM FUNCTION                                                 */
/******************************************************************************/
/*
 * Prototypes
 */

/**  @brief		write buffer */
FERROR
BcmWriteBuffer(
	IN  PVOLUME_CONTROL_BLOCK		pVcb,
	IN  PVNODE			pVnode,
	IN  unsigned int	dwStartBlock,
	IN  unsigned int	dwBlockCount,
	IN  char*			pBuff,
	IN  BCM_OP_FLAG      dwFlag); 

/**  @brief		read buffer */
FERROR
BcmReadBuffer(
	IN  PVOLUME_CONTROL_BLOCK	pVcb,
	IN  PVNODE			pVnode,
	IN  unsigned int	dwStartBlock,
	IN  unsigned int	dwBlockCount,
	OUT char*			pBuff,
	IN  BCM_OP_FLAG      dwFlag);

/**  @brief		sync buffer */
FERROR
BcmSyncBuffer(
	IN PVOLUME_CONTROL_BLOCK	pVcb,
	IN PVNODE					pVnode);

/** @brief		discard buffer */
FERROR
BcmDiscardBuffer(
	IN PVOLUME_CONTROL_BLOCK	pVcb,
	IN PVNODE			pVnode,
	IN unsigned int		dwStartBlock,
	IN unsigned int		dwBlockCount,
	IN BCM_OP_FLAG		dwFlag);

/**
 * @brief        write vector buffer
 */
FERROR
BcmWriteBufferV(
	IN PVOLUME_CONTROL_BLOCK    pVcb,
	IN PVNODE           pVnode,
	IO PVECTOR_INFO     pVI,
	IN BCM_OP_FLAG      dwFlag);

/**
 * @brief        read vector buffer
 */
FERROR
BcmReadBufferV(
	IN PVOLUME_CONTROL_BLOCK    pVcb,
	IN PVNODE           pVnode,
	IO PVECTOR_INFO     pVI,
	IN BCM_OP_FLAG      dwFlag);

#endif /* __BCACHE_H__ */

// end of file
