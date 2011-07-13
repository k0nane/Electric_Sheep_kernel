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
 * @file        linux_xattr.h
 * @brief       This file includes extended attribute operations.
 * @version     RFS_3.0.0_b035_RC
 * @see         none
 * @author      hayeong.kim@samsung.com
 */


#ifndef __XATTR_H__
#define __XATTR_H__

#include "rfs_linux.h"

/* xattr.h

struct xattr_handler {
   char *prefix;
   size_t (*list)(struct inode *inode, char *list, size_t list_size,
   const char *name, size_t name_len);
   int (*get)(struct inode *inode, const char *name, void *buffer,
   size_t size);
   int (*set)(struct inode *inode, const char *name, const void *buffer,
   size_t size, int flags);
};

*/

/* get extended attribute names for listxattr, or computed the buffer size required */
LINUX_SSIZE_T XattrListNames(
	PLINUX_DENTRY pDentry,
	char *pBuffer,
	LINUX_SIZE_T szBufferSize);

/*
 * handlers for security namespace
 */
#ifdef CONFIG_RFS_FS_XATTR
extern PLINUX_XATTR_HANDLER	g_stLinuxXattrHandlers[];
#else
#define g_stLinuxXattrHandlers		NULL
#endif

/* initialize security xattr for inode */
#ifdef CONFIG_RFS_FS_SECURITY
extern int XattrInitSecurity(
	PLINUX_INODE	pInode,
	PLINUX_INODE	pParentInode);
#else
static inline int XattrInitSecurity(
	PLINUX_INODE	pInode,
	PLINUX_INODE	pParentInode)
{
	return 0;
}
#endif

/*
 * common functions for xattrs
 */
extern int XattrCommGetValue(PVNODE, XATTR_NAMESPACE_ID, const char*,
		void*, LINUX_SIZE_T );
extern int XattrCommSetValue(PVNODE, XATTR_NAMESPACE_ID, const char*,
		const void*, LINUX_SIZE_T, int); 
extern int XattrCommRemoveName(PVNODE, XATTR_NAMESPACE_ID, const char*);

#ifdef CONFIG_RFS_FS_ACL
/*
 * handlers for access ACL namespace
 */
extern LINUX_XATTR_HANDLER	stXattrAclAccessHandler;
extern LINUX_XATTR_HANDLER	stXattrAclDefaultHandler;
#endif

#endif /* __XATTR_H__ */

// end of file
