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
 * @file        rfs_linux.h
 * @brief       This file includes local header files for linux APIs
 * @version     RFS_3.0.0_b035_RC
 * @see         none
 * @author      hayeong.kim@samsung.com
 */

#ifndef __RFS_LINUX_H__
#define __RFS_LINUX_H__

#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/uio.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/time.h>
#include <linux/parser.h>
#include <linux/wait.h>
#include <linux/buffer_head.h>
#include <linux/byteorder/generic.h>
#include <linux/nls.h>
#include <linux/seq_file.h>
#include <linux/mount.h>
#include <linux/smp_lock.h>
#include <linux/fs_struct.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
#include <linux/semaphore.h>
#else
#include <asm/semaphore.h>
#endif


#include "ns_types.h"
#include "ns_misc.h"

/**
 * @brief	linux typedefs 
 */
typedef int					LINUX_ERROR;
typedef loff_t				LINUX_OFFSET;
typedef size_t				LINUX_SIZE_T;
typedef ssize_t				LINUX_SSIZE_T;
typedef sector_t			LINUX_SECTOR;
typedef mode_t				LINUX_MODE;
typedef dev_t				LINUX_DEV_T;
typedef time_t				LINUX_TIME;
typedef uid_t				LINUX_UID;
typedef gid_t				LINUX_GID;

typedef struct file_system_type		LINUX_FS_TYPE, *PLINUX_FS_TYPE;
typedef struct super_block			LINUX_SUPER, *PLINUX_SUPER;
typedef struct inode				LINUX_INODE, *PLINUX_INODE;
typedef struct file					LINUX_FILE, *PLINUX_FILE;
typedef struct dentry				LINUX_DENTRY, *PLINUX_DENTRY;
typedef struct qstr					LINUX_QSTR, *PLINUX_QSTR;
typedef struct address_space		LINUX_ADDRESS_SPACE, *PLINUX_ADDRESS_SPACE;
typedef struct nameidata			LINUX_NAMEIDATA, *PLINUX_NAMEIDATA;
typedef struct page					LINUX_PAGE, *PLINUX_PAGE;
typedef struct buffer_head			LINUX_BUF, *PLINUX_BUF;
typedef struct block_device			LINUX_BLOCK_DEVICE, *PLINUX_BLOCK_DEVICE;

typedef struct super_operations		LINUX_SUPER_OPS;
typedef struct address_space_operations	LINUX_ADDRESS_SPACE_OPS;
typedef struct file_operations		LINUX_FILE_OPS;
typedef struct inode_operations		LINUX_INODE_OPS;
typedef struct dentry_operations	LINUX_DENTRY_OPS;

typedef struct timespec				LINUX_TIMESPEC, *PLINUX_TIMESPEC;
typedef struct nls_table			LINUX_NLS_TABLE, *PLINUX_NLS_TABLE;
typedef struct iattr				LINUX_ATTR, *PLINUX_ATTR;
typedef struct semaphore			LINUX_SEMAPHORE, *PLINUX_SEMAPHORE;

typedef struct seq_file				LINUX_SEQ_FILE, *PLINUX_SEQ_FILE;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
typedef struct kmem_cache			LINUX_KMEM_CACHE, *PLINUX_KMEM_CACHE;
#else
typedef kmem_cache_t				LINUX_KMEM_CACHE, *PLINUX_KMEM_CACHE;
#endif
typedef struct vfsmount				LINUX_VFS_MNT, *PLINUX_VFS_MNT;
typedef struct kstatfs				LINUX_KSTATFS, *PLINUX_KSTATFS;
typedef struct kstat				LINUX_KSTAT, *PLINUX_KSTAT;
typedef struct writeback_control	LINUX_WB_CTL, *PLINUX_WB_CTL;
typedef struct kiocb				LINUX_KIOCB, *PLINUX_KIOCB;
typedef struct iovec				LINUX_IOVEC, *PLINUX_IOVEC;
typedef filldir_t					LINUX_FILLDIR;
typedef struct list_head			*PLINUX_LIST;
typedef substring_t					LINUX_SUBSTRING;
typedef struct hlist_head			LINUX_HLIST_HEAD, *PLINUX_HLIST_HEAD;
typedef struct hlist_node			LINUX_HLIST_NODE, *PLINUX_HLIST_NODE;
typedef match_table_t				LINUX_MATCH_TABLE;
typedef spinlock_t					LINUX_SPINLOCK;
typedef wait_queue_head_t			LINUX_WAIT_QUEUE_HEAD, *PLINUX_WAIT_QUEUE_HEAD;
typedef fl_owner_t					LINUX_FL_OWNER, *PLINUX_FL_OWNER;

#define PLINUX_CIOVEC				const struct iovec*

#ifdef RFS_CLUSTER_CHANGE_NOTIFY
typedef void (FP_CLUSTER_USAGE_NOTIFY)(unsigned int dwClusterSize, unsigned int dwTotalCluster, unsigned int dwFreeCluster, char *pSid);
extern FP_CLUSTER_USAGE_NOTIFY *gfpClusterUsageNotify;
#endif
/**
 * @brief	linux constant
 */
#define LINUX_PATH_MAX			PATH_MAX
#define LINUX_NAME_MAX			NAME_MAX
#define LINUX_MAX_OPT_ARGS		MAX_OPT_ARGS
#define LINUX_NLS_MAX_CHARSET_SIZE	NLS_MAX_CHARSET_SIZE
#define LINUX_LLONG_MAX			LLONG_MAX

#define LINUX_O_SYNC			O_SYNC	
#define LINUX_O_DIRECT			O_DIRECT

#define LINUX_S_SYNC			S_SYNC

#define LINUX_DT_DIR			DT_DIR
#define LINUX_DT_LNK			DT_LNK
#define LINUX_DT_BLK			DT_BLK
#define LINUX_DT_REG			DT_REG	
#define LINUX_DT_SOCK			DT_SOCK
#define LINUX_DT_FIFO			DT_FIFO

#define LINUX_ATTR_UID			ATTR_UID
#define LINUX_ATTR_GID			ATTR_GID
#define LINUX_ATTR_SIZE			ATTR_SIZE
#define LINUX_ATTR_MTIME		ATTR_MTIME
#define LINUX_ATTR_ATIME		ATTR_ATIME
#define LINUX_ATTR_MODE			ATTR_MODE

#define LINUX_S_IRWXUGO			S_IRWXUGO
#define LINUX_S_ISUID			S_ISUID
#define LINUX_S_ISGID			S_ISGID
#define LINUX_S_ISVTX			S_ISVTX
#define LINUX_S_IALLUGO			(S_ISUID|S_ISGID|S_ISVTX|S_IRWXUGO)
#define LINUX_S_IFMT			S_IFMT	/* bit mask for the file type bitfields */
#define LINUX_S_IFDIR			S_IFDIR	/* directory */
#define LINUX_S_IFLNK			S_IFLNK	/* symbolic link */
#define LINUX_S_IFBLK			S_IFBLK	/* block device */
#define LINUX_S_IFREG			S_IFREG	/* regular file */
#define LINUX_S_IFIFO			S_IFIFO /* fifo node */
#define LINUX_S_IFSOCK			S_IFSOCK /* socket node */
/*
 * The sticky bit (S_ISVTX) on a directory means 
 * that a file in that directory can be renamed or deleted
 * only by the owner of the file, by the owner of the directory,
 * and by a privileged process.
 */
#define LINUX_S_ISVTX			S_ISVTX	/* sticky bit */
/*
 * If files have set-group-ID bit,
 * files created there inherit their group ID from the directory,
 * not from the effective group ID of the creating process,
 * and directories created there will also get the S_ISGID bit set
 */
#define LINUX_S_ISGID			S_ISGID /* set-group-ID bit */

#define LINUX_I_LOCK			I_LOCK
#define LINUX__I_LOCK			__I_LOCK

#define LINUX_WRITE				WRITE
#define LINUX_SWRITE_SYNC		SWRITE_SYNC
#define LINUX_WRITE_SYNC		WRITE_SYNC
#define LINUX_READ				READ

#define LINUX_SEEK_CUR			SEEK_CUR
#define LINUX_SEEK_END			SEEK_END

#define LINUX_MS_NOATIME		MS_NOATIME
#define LINUX_MS_NODIRATIME		MS_NODIRATIME
#define LINUX_MS_RDONLY			MS_RDONLY
#define LINUX_MS_SYNCHRONOUS	MS_SYNCHRONOUS

#define LINUX_MAY_WRITE			MAY_WRITE
#define LINUX_MAY_APPEND		MAY_APPEND

#define LINUX_LOOKUP_CREATE		LOOKUP_CREATE
#define LINUX_LOOKUP_PARENT		LOOKUP_PARENT

#define LINUX_THIS_MODULE		THIS_MODULE
#define LINUX_FS_REQUIRES_DEV	FS_REQUIRES_DEV	
#define LINUX_WB_SYNC_ALL		WB_SYNC_ALL

#define LINUX_GFP_KERNEL		GFP_KERNEL
#define LINUX_KM_USER0			KM_USER0
#define LINUX_PF_MEMALLOC		PF_MEMALLOC

#define LINUX_PAGE_SIZE			PAGE_SIZE
#define LINUX_PAGE_CACHE_MASK	PAGE_CACHE_MASK
#define LINUX_PAGE_CACHE_SHIFT	PAGE_CACHE_SHIFT

#define LINUX_SLAB_CTOR_VERIFY		SLAB_CTOR_VERIFY
#define LINUX_SLAB_CTOR_CONSTRUCTOR	SLAB_CTOR_CONSTRUCTOR
#define LINUX_SLAB_RECLAIM_ACCOUNT	SLAB_RECLAIM_ACCOUNT
#define LINUX_SLAB_MEM_SPREAD		SLAB_MEM_SPREAD

#define LINUX_TASK_UNINTERRUPTIBLE	TASK_UNINTERRUPTIBLE
#define LINUX_TASK_RUNNING			TASK_RUNNING
				

/**
 * @brief	linux variable
 */
#define LINUX_g_CurTask				current
#define LINUX_g_stPageSymlinkOps	page_symlink_inode_operations
#define LINUX_g_InodeLock			inode_lock
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 25)
	#define LINUX_g_CurFsUid		current_fsuid()
	#define LINUX_g_CurFsGid		current_fsgid()
	#define LINUX_g_CurUid			current_uid()
	#define LINUX_g_CurGid			current_gid()
	#define LINUX_g_CurFsUmask		(current->fs->umask)
#else
	#define LINUX_g_CurFsUid		(current->fsuid)
	#define LINUX_g_CurFsGid		(current->fsgid)
	#define LINUX_g_CurUid			(current->uid)
	#define LINUX_g_CurGid			(current->gid)
	#define LINUX_g_CurFsUmask		(current->fs->umask)
#endif


/**
 * @brief	linux define
 */
#define	LINUX_IS_SYNC				IS_SYNC
#define LINUX_IS_RDONLY				IS_RDONLY
#define LINUX_IS_IMMUTABLE			IS_IMMUTABLE
#define LINUX_IS_APPEND				IS_APPEND
#define LINUX_IS_REG				S_ISREG
#define LINUX_IS_DIR				S_ISDIR
#define LINUX_IS_LNK				S_ISLNK
#define LINUX_IS_BLK				S_ISBLK
#define LINUX_IS_CHR				S_ISCHR
#define LINUX_IS_FIFO				S_ISFIFO
#define LINUX_IS_SOCK				S_ISSOCK

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
	#define LINUX_IS_NOATIME		IS_NOATIME
#else
	#define LINUX_IS_NOATIME(inode)		((inode)->i_sb->s_flags & (MS_RDONLY|MS_NOATIME))
#endif

#define LINUX_O_APPEND				O_APPEND

#define LINUX_CONTAINER_OF			container_of
#define LINUX_CURRENT_TIME			CURRENT_TIME

#define LINUX_INIT_HLIST_HEAD		INIT_HLIST_HEAD

#define LINUX_SET_CURRENT_STATE		set_current_state
#define LINUX__SET_CURRENT_STATE	__set_current_state

#define LINUX_KIOCB_IS_KICKED		kiocbIsKicked
#define LINUX_KIOCB_CLEAR_KICKED	kiocbClearKicked

/**
 * @brief	linux library functions
 */
#define LINUX_RegisterFilesystem		register_filesystem
#define LINUX_UnregisterFilesystem		unregister_filesystem

#define LINUX_GenericFileLlseek			generic_file_llseek
#define LINUX_DoSyncRead				do_sync_read
#define LINUX_DoSyncWrite				do_sync_write
#define LINUX_GenericFileAioRead		generic_file_aio_read
#define LINUX_GenericFileAioWrite		generic_file_aio_write
#define LINUX_GenericFileRead			generic_file_read
#define LINUX_GenericFileWrite			generic_file_write
#define LINUX_GenericFileWriteNolock	generic_file_write_nolock
#define LINUX_GenericFileAioWriteNolock	generic_file_aio_write_nolock
#define LINUX_GenericFileMmap			generic_file_mmap
#define LINUX_GenericPermission			generic_permission
#define LINUX_VfsPermission				vfs_permission
#define LINUX_GenericReadDir			generic_read_dir
#define LINUX_GenericSendFile			generic_file_sendfile
#define LINUX_GenericSpliceRead			generic_file_splice_read
#define LINUX_GenericSpliceWrite		generic_file_splice_write
#define LINUX_GenericReadLink			generic_readlink
#define LINUX_NdSetLink					nd_set_link
#define LINUX_KillBlockSuper			kill_block_super

#define	LINUX_GetSbBdev			get_sb_bdev
#define LINUX_GetUser			get_user

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 28)
	#define LINUX_SbIssueDiscard    sb_issue_discard
#endif

#define LINUX_SbBread			sb_bread
#define LINUX_SbGetblk			sb_getblk
#define LINUX_SyncDirtyBuffer	sync_dirty_buffer
#define LINUX_LLRwBlock			ll_rw_block
#define LINUX_WaitOnBuffer		wait_on_buffer
#define LINUX_Brelse			brelse
#define LINUX_MapBuffer			map_bh
#define LINUX_MarkBufferDirty	mark_buffer_dirty
#define LINUX_SyncDirtyBuffer	sync_dirty_buffer
#define LINUX_SetBufferDirty	set_buffer_dirty
#define LINUX_ClearBufferDirty	clear_buffer_dirty
#define LINUX_SetBufferUptodate	set_buffer_uptodate
#define LINUX_IsBufferUptodate	buffer_uptodate
#define LINUX_SetBufferNew		set_buffer_new
#define LINUX_ClearBit			clear_bit
#define LINUX_LockBuffer		lock_buffer
#define LINUX_UnlockBuffer		unlock_buffer
#define LINUX_GetBH				get_bh
#define LINUX_SubmitBH			submit_bh
#define LINUX_EndBufferWriteSync	end_buffer_write_sync

#define LINUX_NewInode			new_inode
#define LINUX_Iunique			iunique
#define LINUX_GrabInode			igrab
#define LINUX_InodeChangeOk		inode_change_ok
#define LINUX_InodeReadSize		i_size_read
#define LINUX_InodeWriteSize	i_size_write
#define LINUX_UnlockNewInode	unlock_new_inode
#define LINUX_InodeSetattr		inode_setattr
#define LINUX_InsertInodeHash	insert_inode_hash
#define LINUX_RemoveInodeHash	remove_inode_hash
#define LINUX_TruncInodePages	truncate_inode_pages
#define LINUX_FilemapWriteAndWait	filemap_write_and_wait
#define LINUX_FilemapDataWrite	filemap_fdatawrite
#define LINUX_FilemapDataWait	filemap_fdatawait
#define LINUX_WriteInodeNow		write_inode_now
#define LINUX_SyncInode			sync_inode
#define LINUX_MarkInodeDirty	mark_inode_dirty
#define LINUX_MarkInodeDirtySync	mark_inode_dirty_sync
#define LINUX_Iput				iput
#define LINUX_IsBadInode		is_bad_inode
#define LINUX_MakeBadInode		make_bad_inode
#define LINUX_ClearInode		clear_inode
#define LINUX_InodeInitOnce		inode_init_once
#define LINUX_InitSpecialInode	init_special_inode

#define LINUX_SbSetBlocksize	sb_set_blocksize
#define LINUX_BdevHardsectSize	bdev_hardsect_size

#define LINUX_IovLength			iov_length
#define LINUX_BlockdevDirectIO	blockdev_direct_IO

#define LINUX_DInstantiate		d_instantiate
#define LINUX_DSpliceAlias		d_splice_alias
#define LINUX_DAllocRoot		d_alloc_root
#define LINUX_DFindAlias		d_find_alias
#define LINUX_DInvalidate		d_invalidate
#define LINUX_Dput				dput
#define LINUX_InitNameHash		init_name_hash
#define LINUX_PartialNameHash	partial_name_hash
#define LINUX_EndNameHash		end_name_hash

#define LINUX_LoadNLS			load_nls
#define LINUX_NlsToUpper		nls_toupper
#define LINUX_NlsStrniCmp		nls_strnicmp

#define LINUX_MakeTime			mktime
#define LINUX_AtomicRead		atomic_read

#define LINUX_StrSep			strsep
#define LINUX_MatchToken		match_token
#define LINUX_MatchStrdup		match_strdup
#define LINUX_MatchInt			match_int
#define LINUX_MatchOctal		match_octal

#define LINUX_MpageReadpage			mpage_readpage
#define LINUX_MpageReadpages		mpage_readpages
#define LINUX_BlockWriteFullPage	block_write_full_page
#define LINUX_MpageWritePages		mpage_writepages
#define LINUX_ContPrepareWrite		cont_prepare_write
#define LINUX_BlockPrepareWrite		block_prepare_write
#define LINUX_GenericCommitWrite	generic_commit_write
#define LINUX_BlockCommitWrite		block_commit_write

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
#define LINUX_ContWriteBegin		cont_write_begin
#define LINUX_GenericWriteEnd		generic_write_end
#endif

#define LINUX_SyncPageRange			sync_page_range
#define LINUX_GrabCachePage			grab_cache_page
#define LINUX_PageHasBuffers		page_has_buffers
#define LINUX_PageBuffers			page_buffers
#define LINUX_CreateEmptyBuffers	create_empty_buffers
#define LINUX_SetPageUptodate		SetPageUptodate
#define LINUX_ClearPageUptodate		ClearPageUptodate
#define LINUX_FlushDcachePage		flush_dcache_page
#define LINUX_BlockSyncPage			block_sync_page
#define LINUX_UnlockPage			unlock_page
#define LINUX_PageCacheRelease		page_cache_release
#define LINUX_GetFreePage			__get_free_page
#define LINUX_FreePage				free_page
#define LINUX_PageAddress			page_address

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
#define LINUX_Lock			mutex_lock
#define LINUX_TryLock		mutex_trylock
#define LINUX_Unlock		mutex_unlock
#else
#define LINUX_Lock			down
#define LINUX_TryLock		down_trylock
#define LINUX_Unlock		up
#endif

#define LINUX_SemaInit		sema_init
#define LINUX_Down			down
#define LINUX_TryDown		down_trylock
#define LINUX_Up			up

#define LINUX_ReadLock		down_read
#define LINUX_ReadUnlock	up_read

#define LINUX_SpinLockInit	spin_lock_init
#define LINUX_SpinLock		spin_lock
#define LINUX_SpinUnlock	spin_unlock

#define	LINUX_LockKernel	lock_kernel
#define LINUX_UnlockKernel	unlock_kernel

#define LINUX_LockSuper		lock_super
#define LINUX_UnlockSuper	unlock_super

#define LINUX_BitWaitQueue	bit_waitqueue
#define LINUX_WaitOnBit		__wait_on_bit
#define LINUX_InodeWait		inode_wait
#define LINUX_WakeupInode	wake_up_inode

#define LINUX_HLIST_ADD_HEAD	hlist_add_head
#define LINUX_HLIST_DEL_INIT	hlist_del_init
#define LINUX_HLIST_FOR_EACH	hlist_for_each_entry

#define LINUX_KmapAtomic		kmap_atomic
#define LINUX_KunmapAtomic		kunmap_atomic

#define LINUX_KmemCacheCreate	kmem_cache_create
#define LINUX_KmemCacheDestroy	kmem_cache_destroy
#define LINUX_KmemCacheAlloc	kmem_cache_alloc
#define LINUX_KmemCacheFree		kmem_cache_free
#define	LINUX_Kmalloc			kmalloc
#define LINUX_Kfree				kfree

#define LINUX_Memset			memset
#define LINUX_Memcpy			memcpy

/*
 * special library for extended attributes
 */
#ifdef CONFIG_RFS_FS_XATTR
#include <linux/xattr.h>

typedef struct posix_acl			LINUX_POSIX_ACL, *PLINUX_POSIX_ACL;
typedef struct xattr_handler		LINUX_XATTR_HANDLER, *PLINUX_XATTR_HANDLER;

#define LINUX_XATTR_SECURITY_PREFIX		XATTR_SECURITY_PREFIX
#define LINUX_XATTR_TRUSTED_PREFIX		XATTR_TRUSTED_PREFIX
#define LINUX_XATTR_USER_PREFIX			XATTR_USER_PREFIX
#define LINUX_POSIX_ACL_XATTR_ACCESS	POSIX_ACL_XATTR_ACCESS
#define LINUX_POSIX_ACL_XATTR_DEFAULT	POSIX_ACL_XATTR_DEFAULT

#define LINUX_GenericSetXattr		generic_setxattr
#define LINUX_GenericGetXattr		generic_getxattr
#define LINUX_GenericListXattr		generic_listxattr
#define LINUX_GenericRemoveXattr	generic_removexattr

/* for SELinux, ACL */
#define LINUX_SecurityInodeInit		security_inode_init_security
#define LINUX_PosixAclEquivMode		posix_acl_equiv_mode
#define LINUX_PosixAclRelease		posix_acl_release
#define LINUX_PosixAclToXAttr		posix_acl_to_xattr
#define LINUX_PosixAclFromXAttr		posix_acl_from_xattr
#define LINUX_PosixAclValid			posix_acl_valid
#define LINUX_PosixAclDup			posix_acl_dup
#define LINUX_PosixAclCreateMasq	posix_acl_create_masq
#define	LINUX_PosixAclClone			posix_acl_clone

#endif /* CONFIG_RFS_FS_XATTR */

/**
 * @brief	basic inline functions for Linux VFS interface
 */

/* Get inode of 'struct file' */
static inline PLINUX_INODE LxFileToInode(PLINUX_FILE pFile)
{
	return pFile->f_dentry->d_inode;
}


/* set i_blocks - numof 512-byte blocks in inode */
static inline void LxSetBlocks(PLINUX_INODE pInode, LINUX_OFFSET llBytes)
{
	LINUX_SpinLock(&pInode->i_lock);
	pInode->i_blocks = (llBytes + (512 - 1)) >> 9;
	LINUX_SpinUnlock(&pInode->i_lock);
}

/**
 * @brief	Define constants
 */
#define RFS_ROOT_INO				1

#define RFS_ROOT_UID				0			/* root uid */
#define RFS_ROOT_GID				0			/* root gid */
#define RFS_DEFAULT_PERM_MASK		0000		/* default external permission mask */

/* ioctl commands */
#define	RFS_IOC_SET_ZEROFILL		_IOW(0xFA, 2, int)
#define	RFS_IOC_DO_CHKDSK			_IOW(0xFA, 3, int)

/**
 * @brief	Define Native FileSystems
 */
#define NATIVE_BTFS_NAME		(const wchar_t *)(L"btfs")
#define RFS_BTFS				("rfs")

/**
 * @brief	extern structures & functions
 */
extern struct super_operations g_stLinuxSuperOps;
extern struct address_space_operations g_stLinuxAddrOps;

extern struct file_operations g_stLinuxDirOps;
extern struct inode_operations g_stLinuxDirInodeOps;

extern struct file_operations g_stLinuxFileOps;
extern struct inode_operations g_stLinuxFileInodeOps;

extern struct inode_operations g_stLinuxSymlinkInodeOps;

extern struct inode_operations g_stLinuxSpecialInodeOps;

extern struct dentry_operations g_stLinuxDentryOpsI;

extern struct inode *LxAllocateInode (struct super_block *pSb);
extern void LxDestroyInode (struct inode *pInode);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
extern int LxGetSb(struct file_system_type *plxFsType, int dwFlag, const char* pDevName, void* pData, struct vfsmount *plxMnt);
#else
extern int LxGetSb(struct file_system_type *plxFsType, int dwFlag, const char* pDevName, void* pData);
#endif


/*
 * Project Base
 */
#include "nsd_debug.h"

/*
 * Debug Message
 */
#include "rfs_linux_debug.h"

/*
 * linux utility
 */
#include "linux_util.h"

/*
 * linux glue version
 */
#include "linux_version.h"
#endif /* __RFS_LINUX_H__ */

// end of file
