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
 * @file        linux_fcb.h
 * @brief       This file includes APIs to NativeFS for accessing file structure.
 * @version     RFS_3.0.0_b035_RC
 * @see         none
 * @author      hayeong.kim@samsung.com
 */
 
#ifndef __FCB_H__
#define __FCB_H__

#include "rfs_linux.h"
#include "linux_vnode.h"

#undef RFS_FILE_ZONE_MASK
#define RFS_FILE_ZONE_MASK		(eRFS_DZM_FCB)

/******************************************************************************/
/* STRUCTURE                                                                  */
/******************************************************************************/
/**
 * @struct	_FILE_CONTROL_BLOCK
 * @brief	structure for an open file or directory.
 */
struct _FILE_CONTROL_BLOCK 
{
	LINUX_FILE		stLxFile;	/**< linux file structure */
};
 
/******************************************************************************/
/* NESTLE PUBLIC FUNCTIONS                                                    */
/******************************************************************************/
/**
 * @brief		get a vnode in fcb
 * @param[in]	pFcb	nestle Fcb pointer
 * @returns
 *		- PVNODE pointed by Fcb
 * @remarks 
 *
 */
static inline PVNODE 
FcbGetVnode(
	IN	PFILE_CONTROL_BLOCK		pFcb)
{
	LNX_ASSERT_ARG(pFcb, ERR_PTR(-EINVAL));

	return VnodeGetVnodeFromInode(pFcb->stLxFile.f_dentry->d_inode);
}

/**
 * @brief		get a file offset of fcb
 * @param[in]	pFcb	nestle Fcb pointer
 * @returns
 *		- FILE_OFFSET
 * @remarks 
 *
 */
static inline	FILE_OFFSET
FcbGetOffset(
	IN	PFILE_CONTROL_BLOCK		pFcb)
{
	LNX_ASSERT_ARG(pFcb, -EINVAL);

	return (FILE_OFFSET) (pFcb->stLxFile.f_pos);
}

/**
 * @brief		check whether the fcb's Access Mode is FILE_ACCESS_SYNCHRONOUS
 * @param[in]	pFcb		nestle Fcb pointer
 * @returns
 *		- BOOL : if FILE_ACCESS_SYNCHRONOUS, returns TRUE
 * @remarks 
 */
static inline	BOOL
FcbIsSyncMode(
	IN	PFILE_CONTROL_BLOCK		pFcb)
{
	PLINUX_INODE	pInode = NULL;

	LNX_ASSERT_ARG(pFcb, -EINVAL);

	pInode = pFcb->stLxFile.f_dentry->d_inode;

	if ((pFcb->stLxFile.f_flags & LINUX_O_SYNC) || 
			LINUX_IS_SYNC(pInode))
		return TRUE;
	else
		return FALSE;
}

/**
 * @brief		check whether the fcb's Access Mode is FILE_ACCESS_NO_BUFFERING
 * @param[in]	pFcb		nestle Fcb pointer
 * @returns
 *		- BOOL : if FILE_ACCESS_NO_BUFFERING, returns TRUE
 * @remarks 
 */
static inline	BOOL
FcbIsBypassMode(
	IN	PFILE_CONTROL_BLOCK		pFcb)
{
	LNX_ASSERT_ARG(pFcb, -EINVAL);

	if (pFcb->stLxFile.f_flags & LINUX_O_DIRECT)
		return TRUE;
	else
		return FALSE;
}

/**
 * @brief		check whether the fcb's Access Mode is FILE_NO_MODTIME_UPDATE
 * @param[in]	pFcb		nestle Fcb pointer
 * @returns
 *		- BOOL : if FILE_ACCESS_MODTIME_UPDATE, returns TRUE
 * @remarks 
 */
static inline	BOOL
FcbIsModtimeUpdate(
	IN	PFILE_CONTROL_BLOCK		pFcb)
{
	LNX_ASSERT_ARG(pFcb, -EINVAL);

	/* Do not update file ctime & mtime: S_NOCMTIME */
	if (IS_NOCMTIME(pFcb->stLxFile.f_dentry->d_inode)) 
		return FALSE;
	else
		return TRUE;
}

/**
 * @brief		check whether the fcb's Access Mode is FILE_NO_ACSTIME_UPDATE
 * @param[in]	pFcb		nestle Fcb pointer
 * @returns
 *		- BOOL : if FILE_ACCESS_MODTIME_UPDATE, returns TRUE
 * @remarks 
 */
static inline	BOOL
FcbIsAcstimeUpdate(
	IN	PFILE_CONTROL_BLOCK		pFcb)
{
	LNX_ASSERT_ARG(pFcb, -EINVAL);

	/* do not update access time: O_NOATIME for file, MS_NOATIME for sb */
	if (pFcb->stLxFile.f_flags & O_NOATIME)
		return FALSE;
	else
		return TRUE;
}


/******************************************************************************/
/* NESTLE PRIVATE FUNCTIONS                                                   */
/******************************************************************************/

/**
 * @brief		set fcb's file offset
 * @param[in]	pFcb		nestle Fcb pointer
 * @param[out]	llOffset	file offset to be set
 */
static inline void
FcbSetOffset(
	IN	PFILE_CONTROL_BLOCK		pFcb,
	OUT FILE_OFFSET				llOffset)
{
	if (pFcb == NULL)
	{
		RFS_ASSERT(0);
		return; /* invalid arguement */
	}

	pFcb->stLxFile.f_pos = (LINUX_OFFSET) llOffset;

	return;
}

#undef RFS_FILE_ZONE_MASK

#endif // __FCB_H__

// end of file
