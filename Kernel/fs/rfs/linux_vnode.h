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
 * @file        linux_vnode.h
 * @brief       This file includes vnode operations.
 * @version     RFS_3.0.0_b035_RC
 * @see         none
 * @author      hayeong.kim@samsung.com
 */

#ifndef __VNODE_H__
#define __VNODE_H__

#include "rfs_linux.h"

#include <linux/dcache.h>
#include <linux/kernel.h>	

#undef RFS_FILE_ZONE_MASK
#define RFS_FILE_ZONE_MASK		(eRFS_DZM_VNODE)

extern struct timezone sys_tz;

/******************************************************************************/
/* STRUCTURE                                                                  */
/******************************************************************************/
/**
 * @struct	_VNODE
 * @brief	Internal data structure to represent a file or directory.
 */
struct _VNODE
{
	/* Nestle internals */
	unsigned long long			llVnodeID;		/**< vnode ID */
	LINUX_HLIST_NODE			hleHash;		/**< hash list entry */
	LINUX_OFFSET				llMappedSize;	/**< mapped size. This is used in fsGetBlock */	
	LINUX_SEMAPHORE				bsVnodeMetaSem;	/**< semaphore to protect file/dir metadata */


	/* supplementary field for inode */
	unsigned int				dwAttr;			/**< file attributes: refer to _NS_FILE_ATTR */
	COMP_TIMESPEC				dwCreationTime;	/**< creation time */

	/* for NativeFS */
	PVNODE_OPERATIONS			pVnodeOps;		/**< the node operations */
	PFILE_OPERATIONS			pFileOps;		/**< the file operations */
	void*						pNative;		/**< pointer to the native inode */
	PVOLUME_CONTROL_BLOCK 		pVcb;			/**< the volume object belonging to */

	LINUX_INODE					stLxInode;		/**< linux inode */

	PLINUX_DENTRY				pstLxDentry;	/**< linux dentry (debug) */

#ifdef CONFIG_RFS_FS_POSIX_ACL
	PLINUX_POSIX_ACL			pstLxAcl;
	PLINUX_POSIX_ACL			pstLxAclDefault;
#endif
};


/******************************************************************************/
/* Nestle PUBLIC API : VNODE FUNCTION                                         */
/******************************************************************************/

/**
 * @brief		down the metalock of vnode. 
 * @param[in]	pVnode : nestle vnode
 * @returns		void
 */
static inline void
VnodeMetaLock(PVNODE	pVnode)
{
	LNX_ASSERT_ARGV(pVnode);
	LINUX_Down(&pVnode->bsVnodeMetaSem);
}

/**
 * @brief		up the metalock of vnode. 
 * @param[in]	pVnode : nestle vnode
 * @returns		void
 */
static inline void
VnodeMetaUnlock(PVNODE	pVnode)
{
	LNX_ASSERT_ARGV(pVnode);
	LINUX_Up(&pVnode->bsVnodeMetaSem);
}


/*
 * Prototypes
 */
void
VnodeSetAttr(
	IN	PVNODE 			pVnode,
	IN	unsigned int	dwAttr);

void
VnodeGetTimes(
	IN	PVNODE 			pVnode,
	OUT	PSYS_TIME		pCreateTime,
	OUT	PSYS_TIME		pAccessTime,
	OUT	PSYS_TIME		pUpdateTime);

void
VnodeSetTimes(
	IN	PVNODE 			pVnode,
	IN	PSYS_TIME		pCreateTime,
	IN	PSYS_TIME		pAccessTime,
	IN	PSYS_TIME		pUpdateTime);

FERROR
VnodeFillZero(
	IN  PVNODE 					pVnode,
	IN  FILE_OFFSET				llOffset,
	IN  unsigned int			dwByteToFill,
	OUT	unsigned int*			pdwBytesFilled);

void
VnodeChangeIndex(
	IN	PVNODE				pVnode, 
	IN	unsigned long long	llVnodeID);

void
VnodeSetUid(
	IN	PVNODE 			pVnode,
	IN	unsigned int	dwUid);

void
VnodeSetGid(
	IN	PVNODE 			pVnode,
	IN	unsigned int	dwGid);

void
VnodeSetAcl(
	IN	PVNODE 			pVnode,
	IN	unsigned short	dwAcl);

void
VnodeSetBlocks(
	IN	PVNODE	pVnode,
	IN	FILE_SIZE	llSize);

unsigned int
VnodeGetBlocks(
	IN	PVNODE 			pVnode);

void
VnodeSetSize(
	IN	PVNODE 			pVnode,
	IN	FILE_SIZE		llSize);

/*
 * inline functions
 */

/**
 * @brief		Get Vnode's index
 * @param[in]	pVnode		nestle vnode
 * @returns		index of vnode
 */
static inline unsigned long long
VnodeGetIndex(
	IN	PVNODE 				pVnode)
{
	LNX_ASSERT_ARG(pVnode, ~0UL);
	return pVnode->llVnodeID;
}

/**
 * @brief		Get Vnode's user ID
 * @param[in]	pVnode		nestle vnode
 * @returns		user id of vnode
 */
static inline unsigned int
VnodeGetUid(
	IN	PVNODE 			pVnode)
{
	LNX_ASSERT_ARG(pVnode, ~0U);
	return pVnode->stLxInode.i_uid;
}

/**
 * @brief		Get Vnode's group ID
 * @param[in]	pVnode		nestle vnode
 * @returns		group id of vnode
 */
static inline unsigned int
VnodeGetGid(
	IN	PVNODE 			pVnode)
{
	LNX_ASSERT_ARG(pVnode, ~0U);
	return pVnode->stLxInode.i_gid;
}

/**
 * @brief		Get Vnode's File Size
 * @param[in]	pVnode : nestle vnode
 * @returns		file size of vnode
 */
static inline	FILE_SIZE
VnodeGetSize(
	IN	PVNODE 			pVnode)
{
	LNX_ASSERT_ARG(pVnode, ~0U);
	
	return (FILE_SIZE) LINUX_InodeReadSize(&pVnode->stLxInode);
}

/**
 * @brief		Get Vnode's atrribute
 * @param[in]	pVnode : nestle vnode
 * @returns		attribute of vnode
 */
static inline unsigned int	
VnodeGetAttr(
	IN	PVNODE 			pVnode)
{
	unsigned int dwAttr;

	LNX_ASSERT_ARG(pVnode, ~0U);

	VnodeMetaLock(pVnode);
	dwAttr = pVnode->dwAttr;
	VnodeMetaUnlock(pVnode);

	return dwAttr;
}

/**
 * @brief		Is vnode init cluster mode?
 * @param[in]	pVnode : nestle vnode
 * @returns		BOOL : TRUE: cluster init mode, FALSE: no init mode
 */
static inline	BOOL
VnodeIsInitCluster(
	IN	PVNODE			pVnode)
{
	BOOL	bIsInit;
	LNX_ASSERT_ARG(pVnode, FALSE);

	VnodeMetaLock(pVnode);
	bIsInit = (pVnode->dwAttr & FILE_ATTR_NO_INIT_CLUSTER)? FALSE: TRUE;
	VnodeMetaUnlock(pVnode);

	return bIsInit;
}


/**
 * @brief		Get Vnode's link count
 * @param[in]	pVnode : nestle vnode
 * @returns		link count of vnode
 */
static inline	unsigned short
VnodeGetLinkCnt(
	IN	PVNODE 			pVnode)
{
	LNX_ASSERT_ARG(pVnode, (unsigned short) ~0);

	return (unsigned short) (pVnode->stLxInode.i_nlink);
}

/**
 * @brief		Set Vnode's link count
 * @param[io]	pVnode : nestle vnode
 * @param[in]	wCnt : link count
 * @returns		link count of vnode
 */
static inline	unsigned short
VnodeSetLinkCnt(
	IN	PVNODE 			pVnode,
	IN	unsigned short	wCnt)
{
	LNX_ASSERT_ARG(pVnode, (unsigned short) ~0);

	pVnode->stLxInode.i_nlink = (unsigned int) wCnt;

	return (unsigned short) (pVnode->stLxInode.i_nlink);
}

/**
 * @brief		Get Vnode's Access level
 * @param[in]	pVnode : nestle vnode
 * @returns		ACL of vnode
 */
static inline	unsigned short
VnodeGetAcl(
	IN	PVNODE 			pVnode)
{
	unsigned short dwAcl;

	LNX_ASSERT_ARG(pVnode, (unsigned short) ~0);

	/* translate i_mode of linux inode to vnode's type */
	dwAcl = (pVnode->stLxInode.i_mode & ACL_ALL);

	return dwAcl;
}

/**
 * @brief		Get Vnode's VCB
 * @param[in]	pVnode : nestle vnode
 * @returns		pVcb
 */
static inline	PVOLUME_CONTROL_BLOCK 
VnodeGetVcb(
	IN	PVNODE 			pVnode)
{
	LNX_ASSERT_ARG(pVnode, NULL);

	return pVnode->pVcb;
}

/**
 * @brief		Get native node for nestle Vnode
 * @param[in]	pVnode : nestle vnode
 * @returns		void* : native node pointer
 */
static inline	void*
VnodeGetNative(
	IN	PVNODE 			pVnode)
{
	LNX_ASSERT_ARG(pVnode, NULL);
	return pVnode->pNative;
}

/**
 * @brief		Set native node for nestle Vnode
 * @param[in]	pVnode : nestle vnode
 * @param[in]	pNative : native node pointer
 * @returns		void
 */
static inline	void
VnodeSetNative(
	IN	PVNODE 			pVnode,
	IN	void*			pNative)
{
	LNX_ASSERT_ARGV(pVnode);

	pVnode->pNative = pNative;

	return;
}

/**
 * @brief		Set Vnode dirty mark for data in vnode (data)
 * @param[in]	pVnode : nestle vnode
 * @returns		void
 */
static inline	void
VnodeMarkDataDirty(
	IN	PVNODE 			pVnode)
{
	LNX_ASSERT_ARGV(pVnode);

	/* set I_DIRTY */
	LINUX_MarkInodeDirty(&pVnode->stLxInode);

	return;
}

/**
 * @brief		Set Vnode dirty for vnode itself (metadata)
 * @param[in]	pVnode : nestle vnode
 * @returns		void
 */
static inline	void
VnodeMarkMetaDirty(
	IN	PVNODE 			pVnode)
{
	LNX_ASSERT_ARGV(pVnode);

	/* set I_DIRTY_SYNC */
	LINUX_MarkInodeDirtySync(&pVnode->stLxInode);

	return;
}


/******************************************************************************/
/* Nestle PUBLIC API : Superblock FUNCTION                                    */
/******************************************************************************/
/*
 * Prototypes for Linux VFS
 */
void VnodeDeleteInode(PLINUX_INODE	pInode);
void VnodeClearInode(PLINUX_INODE	pInode);
int VnodeWriteInode(PLINUX_INODE pInode, int dwWait);

/******************************************************************************/
/* Nestle PRIVATE VNODE FUNCTION                                              */
/******************************************************************************/
/*
 * Glue internal functions
 */

/**
 * @brief		get vnode from LInux inode
 */
# define VnodeGetVnodeFromInode(pInode) LINUX_CONTAINER_OF(pInode, VNODE, stLxInode)

/**
 * @brief		Get vnode pointer from linux file structure (same as Fcb)
 */
# define VnodeGetVnodeFromFile(pFile) VnodeGetVnodeFromInode(pFile->f_dentry->d_inode)

/**
 * @brief		create a new inode and return vnode containing the inode
 */
PVNODE
VnodeCreate(PVOLUME_CONTROL_BLOCK	pVcb);

/**
 * @brief		Mark vnode bad in case of error after creating Vnode
 */
void 
VnodeMakeBad(PVNODE					pVnode);
/**
 * @brief		initialize new vnode
 */
void
VnodeInitialize(
	IN	PVNODE					pVnode,
	IN	unsigned long long		llVnodeID,
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	PVNODE					pParentVnode,
	IN	void*					pFileOps,
	IN	void*					pVnodeOps);


/**
 * @brief		release vnode: refernce count will be decreased.
 * @param[in]	pVnode : nestle vnode
 * @returns		void
 * @remarks		if the count becomes 0, destroy vnode.
 */
static inline	void
VnodeRelease(IN PVNODE			pVnode)
{
	LNX_ASSERT_ARGV(pVnode);

	pVnode->pstLxDentry = NULL;
	LINUX_Iput(&pVnode->stLxInode);
	return;
}

/**
 * @brief		set llMappedSize : file size aligned to block size.
 * @param[in]	pVnode : nestle vnode
 * @param[in]	llSize : file's size in bytes
 * @returns		void
 */
static inline void
VnodeSetMappedSize(PVNODE pVnode, LINUX_OFFSET llSize)
{
	LINUX_OFFSET	nBlkMask = ((1LU << pVnode->stLxInode.i_blkbits) - 1);

	if (llSize == 0)
	{
		pVnode->llMappedSize = 0;
	}
	else
	{
		/* llMappedSize = offset aligned to block size + 1 */
		pVnode->llMappedSize = (((LINUX_OFFSET) (llSize - 1)) | nBlkMask) + 1;
	}

	return;
}

#undef RFS_FILE_ZONE_MASK

#endif // __VNODE_H__

// end of file
