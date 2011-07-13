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
 * @file        linux_dir.h
 * @brief       This file includes directory operations for Linux VFS.
 * @version     RFS_3.0.0_b035_RC
 * @see         none
 * @author      hayeong.kim@samsung.com
 */

#ifndef __DIR_H__
#define __DIR_H__

#include "rfs_linux.h"

extern FERROR FileCheckNativePermission(PVNODE pVnode, OPERATION_MODE dwPerm);
extern PVNODE VcbFindVnode(PVOLUME_CONTROL_BLOCK , unsigned long long);

/******************************************************************************/
/* NESTLE PRIVATE FUNCTIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* directory management apis (inode_operations, file_operations for dir)      */
/******************************************************************************/

/* file opeartions */
/**
 * @brief read all directory entries (file_operations: readdir)
 */
LINUX_ERROR
DirReadDirectory(
	IO	PLINUX_FILE		pFile, 
	OUT	void*			pBufDirent, 
	IN	LINUX_FILLDIR	pfFillDir);


/* inode opeartions */
/**
 * @brief create a new file (inode_operations: create)
 */
LINUX_ERROR
DirCreateFile(
	IO	PLINUX_INODE		pDir, 
	IO	PLINUX_DENTRY		pDentry, 
	IN	int					dwMode, 
	IN	PLINUX_NAMEIDATA	pstNd);

/**
 * @brief lookup a inode associated with dentry (inode_operations: lookup)
 */
PLINUX_DENTRY 
DirLookup(
	IO	PLINUX_INODE		pDir, 
	IO	PLINUX_DENTRY		pDentry, 
	IN	PLINUX_NAMEIDATA	pstNd);

/**
 * @brief make a symbolic link (inode_operations: symlink)
 */
LINUX_ERROR
DirSymbolicLink(
	IO	PLINUX_INODE	pDir, 
	IO	PLINUX_DENTRY	pDentry, 
	IN	const char*		pSymName);
/**
 * @brief create a directory (inode_operations: mkdir)
 */
LINUX_ERROR
DirCreateDirectory(
	IO	PLINUX_INODE	pDir, 
	IO	PLINUX_DENTRY	pDentry, 
	IN	int				dwMode);

/**
 * @brief remove a directory or a file 
 */
LINUX_ERROR
DirDelete(
	IO	PLINUX_INODE	pDir, 
	IN	PLINUX_DENTRY	pDentry);

/**
 * @brief change the name/location of a file or directory (inode_operations: rename)
 */
LINUX_ERROR
DirRename(
	IO	PLINUX_INODE	pOldDir, 
	IO	PLINUX_DENTRY	pOldDentry, 
	IO	PLINUX_INODE	pNewDir, 
	IO	PLINUX_DENTRY	pNewDentry);

/**
 * @brief create a special file (inode_operations: mknod)
 */
LINUX_ERROR
DirCreateNode(
	IO	PLINUX_INODE		pDir, 
	IO	PLINUX_DENTRY		pDentry, 
	IN	int					dwMode, 
	IN	LINUX_DEV_T			dwDev);

#endif /* __DIR_H__ */

// end of file
