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
 * @file        linux_file.h
 * @brief       This file includes file operations for Linux VFS.
 * @version     RFS_3.0.0_b035_RC
 * @see         none
 * @author      hayeong.kim@samsung.com
 */

#ifndef __FILE_H__
#define __FILE_H__

#include "rfs_linux.h"

extern LINUX_ERROR VolChkDisk(PLINUX_SUPER	pSb);

/******************************************************************************/
/* NESTLE PRIVATE FUNCTIONS                                                   */
/******************************************************************************/
/**
 * @brief		check additional permission of inode
 */
FERROR
FileCheckNativePermission(
	IN	PVNODE				pVnode, 
	IN	OPERATION_MODE		dwPerm);

/******************************************************************************/
/* file management apis (file/inode_operations for file)                      */
/******************************************************************************/

/**
 * @brief		open file (file_operations: open)
 */
LINUX_ERROR
FileOpen(
	IN	PLINUX_INODE		pInode, 
	IN	PLINUX_FILE			pFile);

/**
 * @brief		close file (file_operations: release)
 */
LINUX_ERROR
FileClose(
	IN	PLINUX_INODE		pInode, 
	IN	PLINUX_FILE			pFile);

/**
 * @brief		flush in-core data and metadata
 */
LINUX_ERROR
FileFlush(
	IN	PLINUX_FILE		pFile,
	IN	fl_owner_t		pId);

/**
 * @brief		move file offset to new position
 */
LINUX_OFFSET
FileSeek(
	IO	PLINUX_FILE			pFile, 
	IN	LINUX_OFFSET		llOffset, 
	IN	int					nFlagSeek);


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19)
/**
 * @brief		write up to count bytes to file at speicified position
 */
LINUX_SSIZE_T
FileAioWrite(
	IN	struct kiocb*		pIocb, 
	IN	const struct iovec*	pIovec,
	IN	unsigned long		nSegments,
	IN	LINUX_OFFSET		llPos);
#endif

/**
 * @brief		read up to count bytes to file at speicified position
 */
LINUX_SSIZE_T
FileRead(
	IO	PLINUX_FILE		pFile, 
	OUT	char*			pBuf, 
	IN	LINUX_SIZE_T	nCount, 
	IO	LINUX_OFFSET*	pPos);

/**
 * @brief		write up to count bytes to file at speicified position
 */
LINUX_SSIZE_T
FileWrite(
	IO	PLINUX_FILE			pFile, 
	IN	const char*			pBuf, 
	IN	LINUX_SIZE_T		nCount, 
	IO	LINUX_OFFSET*		pPos);

/**
 * @brief		flush all dirty buffers of inode include data and meta data
 */
LINUX_ERROR
FileSync(
	IN	PLINUX_FILE		pFile, 
	IN	PLINUX_DENTRY	pDentry, 
	IN	int				nDataSync);

/**
 * @brief		check a permission of inode
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 27) // 2.6.0 ~ 2.6.26
LINUX_ERROR
FilePermission(
	IN	PLINUX_INODE		pInode, 
	IN	int					dwLxOpMode,
	IN	PLINUX_NAMEIDATA	pNd);
#else
LINUX_ERROR
FilePermission(
	IN	PLINUX_INODE		pInode, 
	IN	int					dwLxOpMode);
#endif


/**
 * @brief		change an attribute of inode
 */
LINUX_ERROR
FileSetAttribute(
	IN	PLINUX_DENTRY	pDentry, 
	IO	PLINUX_ATTR		pAttr);

/**
 * @brief		get an attribute of inode
 */
LINUX_ERROR
FileGetAttribute(
	IN	PLINUX_VFS_MNT		pMnt, 
	IN	PLINUX_DENTRY		pDentry, 
	OUT	PLINUX_KSTAT		pStat);


/**
 * @brief		truncate a file to a specified size
 */
void
FileTruncate(
	IO	PLINUX_INODE	pInode);

/**
 *  @brief		function excuting direct I/O operation
 */
LINUX_SSIZE_T 
FileDirectRW(
	IN	int					nRwFlag, 
	IN	PLINUX_KIOCB		pIocb,
	IN	PLINUX_CIOVEC		pIov, 
	IN	LINUX_OFFSET		llOffset, 
	IN	unsigned long		nNumSegs);

/**
 * @brief		read symlink
 */
LINUX_ERROR
FileReadLink(
	IN	PLINUX_DENTRY		pDentry, 
	OUT	char __user*		pBuffer, 
	IN	int					dwBufLen);

/**
 * @brief		follow symlink
 */
void*
FileFollowLink(
	IN	PLINUX_DENTRY		pDentry, 
	IN	PLINUX_NAMEIDATA	pNd);

/**
 * @brief		put symlink
 */
void
FilePutLink(
	IN	PLINUX_DENTRY		pDentry, 
	IN	PLINUX_NAMEIDATA	pNd, 
	IO	void*				pSymlinkPath);

/**
 * @brief		translate index into a logical block
 */
LINUX_ERROR
FileGetBlock(
	PLINUX_INODE	pInode,
	LINUX_SECTOR	dwBlock,
	PLINUX_BUF 		pBuf,
	int				create);

/**
 * @brief ioctl command
 */
int
FileIoctl(
	PLINUX_INODE 	pInode,
	PLINUX_FILE 	pFile,
	unsigned int	dwCmd,
	unsigned long	dwArgAddr);

#endif // __FILE_H__

// end of file
