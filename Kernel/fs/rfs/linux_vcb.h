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
 * @file        linux_vcb.h
 * @brief       This file includes volume control operations.
 * @version     RFS_3.0.0_b035_RC
 * @see         none
 * @author      hayeong.kim@samsung.com
 */

#ifndef __VCB_H__
#define __VCB_H__

#include "rfs_linux.h"

#undef RFS_FILE_ZONE_MASK
#define RFS_FILE_ZONE_MASK		(eRFS_DZM_VCB)

#define VNODE_HASH_SIZE_BITS	(8)
#define VNODE_HASH_SIZE			(1UL << VNODE_HASH_SIZE_BITS)
#define VNODE_HASH_MASK			(VNODE_HASH_SIZE - 1)

/******************************************************************************/
/* Enumeration                                                               */
/******************************************************************************/

enum _LINUX_PRIVATE_FLAG
{
	/* private flag */
	PRIVATE_VCB_VFAT_OFF	= 0x00000001,
	PRIVATE_VCB_SELINUX		= 0x00000002,
	PRIVATE_VCB_INTERNAL	= 0x00000004,
	PRIVATE_VCB_EXTERNAL	= 0x00000008,
	PRIVATE_VCB_SETATTR		= 0x00000010,	// compatible with linux vfat's uid, gid, umask option

	PRIVATE_VCB_ALL			= (PRIVATE_VCB_VFAT_OFF | PRIVATE_VCB_SELINUX | PRIVATE_VCB_INTERNAL | PRIVATE_VCB_EXTERNAL | PRIVATE_VCB_SETATTR),
};

// external private mount option in GLUE layer
struct _EXTENDED_MOUNT_OPTION
{
	LINUX_UID		dwFsUid;	/* uid */
	LINUX_GID		dwFsGid;	/* gid */
	LINUX_MODE		wFsFmask;	/* fmask */
	LINUX_MODE		wFsDmask;	/* dmask */
};
typedef struct _EXTENDED_MOUNT_OPTION	EXT_MOUNT_OPT, *PEXT_MOUNT_OPT;

/******************************************************************************/
/* STRUCTURE                                                                  */
/******************************************************************************/
/**
 * @struct	_VOLUME_CONTROL_BLOCK
 * @brief	Structure to control the volume
 * @see		Related Vcb function
 *
 */
struct _VOLUME_CONTROL_BLOCK
{
	PLINUX_SUPER 		pSb;			/**< Linux super_block */
	PLINUX_NLS_TABLE	pNlsTableIo;	/**< NLS table for iocharset */
	PLINUX_NLS_TABLE 	pNlsTableDisk;	/**< NLS table for disk codepage*/

	LINUX_SPINLOCK		spinHash;		/**< spin lock for vnode hash */
	LINUX_HLIST_HEAD	aHashTable[VNODE_HASH_SIZE]; /** Vnode Hash TAble */
	PVNODE 				pRoot;			/**< Root Vnode */
	PLOGICAL_DISK 		pDisk;			/**< logical disk */

	unsigned int		dwFlag;			/**< enum MOUNT_FLAG */
	unsigned int		dwPrvFlag;		/**< enum _LINUX_PRIVATE_FLAG */

	EXT_MOUNT_OPT		stExtOpt;		/**< mount option for owner and authority */

	PVCB_OPERATIONS		pVolumeOps;		/**< Volume Operation set */
	void*				pNative;		/**< Native Volume Control Block */
}; 
	
/*
 * Macro
 */
#define clear_mnt_opt(o, opt)	(o &= ~MOUNT_##opt)
#define set_mnt_opt(o, opt)		(o |= MOUNT_##opt)
#define test_mnt_opt(o, opt)	(o & MOUNT_##opt)

#define clear_prv_opt(o, opt)	(o &= ~PRIVATE_VCB_##opt)
#define set_prv_opt(o, opt)		(o |= PRIVATE_VCB_##opt)
#define test_prv_opt(o, opt)	(o & PRIVATE_VCB_##opt)

#define IS_VCB_INTERNAL(pVcb)	test_prv_opt(pVcb->dwPrvFlag, INTERNAL)
#define IS_VCB_EXTERNAL(pVcb)	test_prv_opt(pVcb->dwPrvFlag, EXTERNAL)
#define IS_VCB_SETATTR(pVcb)	test_prv_opt(pVcb->dwPrvFlag, SETATTR)

/******************************************************************************/
/* PRIVATE Inline VCB FUNCTION												  */
/******************************************************************************/

static inline PVOLUME_CONTROL_BLOCK RFS_SB_VCB(PLINUX_SUPER pSb)
{
	return pSb->s_fs_info;
}

/**
 * @brief		whether the volume need particular dentry operation or not.
 * @param[in]	pVcb	vcb pointer to check
 * @return		0 if use Linux VFS's dentry operation, or 1
 */
static inline int
VcbIsAliasDentry(
	IN	PVOLUME_CONTROL_BLOCK	pVcb)
{
	PLINUX_SUPER	pSb = pVcb->pSb;

	if (!strcmp(pSb->s_type->name, RFS_BTFS) &&
			!(pVcb->dwFlag & MOUNT_ALLOW_OS_NAMING_RULE))
	{
		// need dentry ops of case-insensitive comparision
		return 1;
	}

	/* TODO: SUPPORT_VFAT_OFF */

	// don't need own dentry ops
	return 0;
}
/******************************************************************************/
/* NESTLE PUBLIC VCB FUNCTION												  */
/******************************************************************************/

/**
 * @brief		Set operation set of Volume Control Block
 */
void
VcbSetOperation(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	void*					pVolumeOps);
/**
 * @brief		Set the block's size(in bytes) and the bit size of Volume Control Block
 */
FERROR
VcbSetBlockSize(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	unsigned int			dwBlockSize);

/**
 * @brief		Set the Root Vnode of Volume Control Block
 */
FERROR
VcbSetRoot(
	IN	PVOLUME_CONTROL_BLOCK 	pVcb,
	IN	PVNODE 	pVnode);

/**
 * @brief		Find the vnode with index, or create new vnode from index
 */
PVNODE 
VcbFindOrCreateVnode(
	IN	PVOLUME_CONTROL_BLOCK 	pVcb,
	IN	unsigned long long		llVnodeId,
	IN	PVNODE					pParentVnode,
	IN	void*					pFileOps,
	IN	void*					pNodeOps,
	OUT	unsigned int*			pNew);

/**
 * @brief		Find the vnode with index
 */
PVNODE
VcbFindVnode(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	unsigned long long		llVnodeID);
/**
 * @brief		Mark dirty flag at volume
 */
FERROR
VcbMarkDirty(
	IN  PVOLUME_CONTROL_BLOCK	pVcb);

/**
 * @brief		Notify the change of free block count to Nestle
 */
void
VcbNotifyFreeBlockCount(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	unsigned int	dwTotalCount,
	IN	unsigned int	dwFreeCount,
	IN	unsigned int	dwBlockSize);


/**
 * @brief		Set the native volume data of Volume Control Block
 * @param[in]	pVcb		VFS Volume Control Block
 * @param[in]	pNative	Native Volume
 * @returns
 *		- void
 * @remarks 
 * 
 */
static inline	void
VcbSetNative(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	void*					pNative)
{
	LNX_ASSERT_ARGV(pVcb);

	pVcb->pNative = (void *) pNative;
	pVcb->pSb->s_fs_info = pVcb;

	return;
}

/**
 * @brief		Get Native volume of Volume Control Block
 * @param[in]	pVcb		VFS Volume Control Block
 * @returns
 *		- void*		pointer of Native Volume
 * @remarks 
 * 
 */
static inline	void*
VcbGetNative(
	IN	PVOLUME_CONTROL_BLOCK	pVcb)
{
	LNX_ASSERT_ARG(pVcb, NULL);

	return pVcb->pNative;
}

/**
 * @brief		Get Root Vnode of Volume Control Block
 * @param[in]	pVcb			VFS Volume Control Block
 * @returns
 *		- PVNODE 		Root Node Pointer
 * @remarks 
 * 
 */
static inline	PVNODE 
VcbGetRoot(
	IN	PVOLUME_CONTROL_BLOCK	pVcb)
{
	LNX_ASSERT_ARG(pVcb, NULL);

	return pVcb->pRoot;
}
 
/**
 * @brief		Get the block's size of Volume Control Block
 * @param[in]	pVcb			VFS Volume Control Block
 * @returns
 *		- unsigned int	the block's size (in bytes)
 * @remarks 
 * 
 */
static inline	unsigned int
VcbGetBlockSize(
	IN	PVOLUME_CONTROL_BLOCK	pVcb)
{
	LNX_ASSERT_ARG(pVcb, 0);
	LNX_ASSERT_ARG(pVcb->pSb, 0);

	return pVcb->pSb->s_blocksize;
}


/**
 * @brief		Get the block's bitsize of Volume Control Block
 * @param[in]	pVcb			VFS Volume Control Block
 * @returns
 *		- unsigned int	the block's bitsize
 * @remarks 
 * 
 */
static inline	unsigned int
VcbGetBlockSizeBits(
	IN	PVOLUME_CONTROL_BLOCK	pVcb)
{
	LNX_ASSERT_ARG(pVcb, 0);
	LNX_ASSERT_ARG(pVcb->pSb, 0);

	return pVcb->pSb->s_blocksize_bits;
}


/**
 * @brief		Get the logical disk of Volume Control Block
 * @param[in]	pVcb			VFS Volume Control Block
 * @returns
 *		- PLOGICAL_DISK Logical Disk
 * @remarks 
 * 
 */
static inline	PLOGICAL_DISK 
VcbGetLogicalDisk(
	IN	PVOLUME_CONTROL_BLOCK	pVcb)
{
	LNX_ASSERT_ARG(pVcb, NULL);

	return	pVcb->pDisk;
}

/******************************************************************************/
/* NESTLE PRIVATE VCB FUNCTION												  */
/******************************************************************************/

/**
 * @brief		Create a Volume Control Block
 */
PVOLUME_CONTROL_BLOCK 
VcbCreate(
	IO	PLINUX_SUPER		pSb,
	IN	MOUNT_FLAG			dwMntFlag,
	IN	unsigned int		dwPrvFlag,
	IN	PLINUX_NLS_TABLE	pNlsIo,
	IN	PEXT_MOUNT_OPT		pstExtOpt);

/**
 * @brief		Destroy a Volume Control Block
 */
void
VcbDestroy(PVOLUME_CONTROL_BLOCK	pVcb);

/**
 * @brief		Insert a Vnode into the hash table in Vcb
 */
void
VcbInsertHash(PVOLUME_CONTROL_BLOCK pVcb, PVNODE pVnode);

/**
 * @brief		Remove a Vnode from the hash table in Vcb
 */
void
VcbRemoveHash(PVOLUME_CONTROL_BLOCK pVcb, PVNODE pVnode);

unsigned long
VcbHash(
	IN	PVOLUME_CONTROL_BLOCK	pVcb, 
	IN	unsigned long long		llVnodeID);

int
VcbSetDentryOps(
	IN	PVOLUME_CONTROL_BLOCK	pVcb);

#undef RFS_FILE_ZONE_MASK

#endif // __VCB_H__

// end of file
