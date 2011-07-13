/*
 * BTFS(Beyond The FAT fileSystem) Developed by Flash Software Group.
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
 * @file		ffat_node.h
 * @brief		This fie defines functions and types for FFatNode
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFAT_NODE_H_
#define _FFAT_NODE_H_

// includes
#include "ffat_common.h"

#include "ffat_al.h"

#include "ffatfs_types.h"

#include "ffat_addon_types.h"


// defines

// Maximum new cluster count for creation
// it includes the last cluster of current directory.
#define NODE_MAX_CLUSTER_FOR_CREATE		3

#define NODE_SIG					0x7612						//!< signature for node validity
#define NODE_SIG_RESET				0x0000						//!< signature for node reset

#define NODE_TS(_pNode)				((_pNode)->wTimeStamp)		//!< get time stamp
#define NODE_VOL(_pNode)			((_pNode)->pVol)			//!< get volume

#define NODE_VI(_pNode)				(VOL_VI(NODE_VOL(_pNode)))	//!< get volume info
#define NODE_RC(_pNode)				((_pNode)->wRefCount)		//!< get reference count
#define NODE_COP(_pNode)			((_pNode)->dwClusterOfParent)	//!< get cluster of parent node
#define NODE_C(_pNode)				((_pNode)->dwCluster)		//!< get cluster
#define NODE_F(_pNode)				((_pNode)->dwFlag)			//!< get node flag
#define NODE_DE(_pNode)				(&(_pNode)->stDE)			//!< get directory entry
#define NODE_S(_pNode)				((_pNode)->dwSize)			//!< get size
#define NODE_INODE(_pNode)			((_pNode)->pInode)			//!< INODE of VFS

#define NODE_IS_ROOT(_pNode)		(((_pNode)->dwFlag & NODE_ROOT_DIR) ? FFAT_TRUE : FFAT_FALSE)
#define NODE_IS_DIR(_pNode)			FFATFS_IsDir(&(_pNode)->stDE)
#define NODE_IS_FILE(_pNode)		FFATFS_IsFile(&(_pNode)->stDE)

//!< NODE가 VALID 한 정보를 가지고 있음을 설정한다. LOOKUP, CREATE에서 설정됨.
#define NODE_SET_VALID(_pNode)		((_pNode)->dwFlag |= NODE_VALID)
#define NODE_IS_VALID(_pNode)		(((_pNode)->dwFlag & NODE_VALID) ? FFAT_TRUE : FFAT_FALSE)

//!< node가 초기화 되었음을 설정한다.. ffat_node_initNode()에서 설정됨.
#define NODE_SET_INIT(_pNode)		((_pNode)->dwFlag |= NODE_INIT)
#define NODE_IS_INIT(_pNode)		(((_pNode)->dwFlag & NODE_INIT) ? FFAT_TRUE : FFAT_FALSE)

//!< node가 open 되었음을 설정한다.. ffat_node_openNode()에서 설정됨.
#define NODE_SET_OPEN(_pNode)		((_pNode)->dwFlag |= NODE_OPEN)
#define NODE_CLEAR_OPEN(_pNode)		((_pNode)->dwFlag &= (~NODE_OPEN))
#define NODE_IS_OPEN(_pNode)		(((_pNode)->dwFlag & NODE_OPEN) ? FFAT_TRUE : FFAT_FALSE)

#define NODE_SET_UNLINK(_pNode)		((_pNode)->dwFlag |= NODE_UNLINKED)
#define NODE_CLEAR_UNLINK(_pNode)	((_pNode)->dwFlag &= (~NODE_UNLINKED))
#define NODE_IS_UNLINK(_pNode)		(((_pNode)->dwFlag & NODE_UNLINKED) ? FFAT_TRUE : FFAT_FALSE)

#define NODE_SET_OPEN_UNLINK(_pNode)	((_pNode)->dwFlag |= NODE_OPEN_UNLINK)
#define NODE_CLEAR_OPEN_UNLINK(_pNode)	((_pNode)->dwFlag &= (~NODE_OPEN_UNLINK))
#define NODE_IS_OPEN_UNLINK(_pNode)		(((_pNode)->dwFlag & NODE_OPEN_UNLINK) ? FFAT_TRUE : FFAT_FALSE)

#define NODE_SET_DIRTY_SIZE(_pNode)		((_pNode)->dwFlag |= NODE_DIRTY_SIZE)
#define NODE_CLEAR_DIRTY_SIZE(_pNode)	((_pNode)->dwFlag &= (~NODE_DIRTY_SIZE))
#define NODE_IS_DIRTY_SIZE(_pNode)		(((_pNode)->dwFlag & NODE_DIRTY_SIZE) ? FFAT_TRUE : FFAT_FALSE)

#define NODE_SET_DIRTY_SIZE_BEGIN(_pNode)	((_pNode)->dwFlag |= NODE_DIRTY_SIZE_BEGIN)
#define NODE_CLEAR_DIRTY_SIZE_BEGIN(_pNode)	((_pNode)->dwFlag &= (~NODE_DIRTY_SIZE_BEGIN))
#define NODE_IS_DIRTY_SIZE_BEGIN(_pNode)	(((_pNode)->dwFlag & NODE_DIRTY_SIZE_BEGIN) ? FFAT_TRUE : FFAT_FALSE)

#define NODE_SET_DIRTY_SIZE_RDONLY(_pNode)		((_pNode)->dwFlag |= NODE_DIRTY_SIZE_RDONLY)
#define NODE_CLEAR_DIRTY_SIZE_RDONLY(_pNode)	((_pNode)->dwFlag &= (~NODE_DIRTY_SIZE_RDONLY))
#define NODE_IS_DIRTY_SIZE_RDONLY(_pNode)		(((_pNode)->dwFlag & NODE_DIRTY_SIZE_RDONLY) ? FFAT_TRUE : FFAT_FALSE)

#ifdef FFAT_DEBUG
	// lock for read operation for node, read, get node state, readdir etc..
	#define NODE_GET_READ_LOCK(_pNode)	ffat_rwlock_getReadLock(&(_pNode)->stRWLock); pCxt->dwLockCount++		// get read lock
	#define NODE_PUT_READ_LOCK(_pNode)	ffat_rwlock_putReadLock(&(_pNode)->stRWLock); pCxt->dwUnlockCount++		// put release lock

	// write lock for node update, write, change size, set DEC ...
	#define NODE_GET_WRITE_LOCK(_pNode)	ffat_rwlock_getWriteLock(&(_pNode)->stRWLock); pCxt->dwLockCount++
	#define NODE_PUT_WRITE_LOCK(_pNode)	ffat_rwlock_putWriteLock(&(_pNode)->stRWLock); pCxt->dwUnlockCount++
#else
	// lock for read operation for node, read, get node state, readdir etc..
	#define NODE_GET_READ_LOCK(_pNode)	ffat_rwlock_getReadLock(&(_pNode)->stRWLock)		// get read lock
	#define NODE_PUT_READ_LOCK(_pNode)	ffat_rwlock_putReadLock(&(_pNode)->stRWLock)		// put release lock

	// write lock for node update, write, change size, set DEC ...
	#define NODE_GET_WRITE_LOCK(_pNode)	ffat_rwlock_getWriteLock(&(_pNode)->stRWLock)
	#define NODE_PUT_WRITE_LOCK(_pNode)	ffat_rwlock_putWriteLock(&(_pNode)->stRWLock)
#endif

//!< increase/decrease node reference(open)count
#define NODE_INC_REF_COUNT(_pNode)		FFAT_ATOMIC_INC16(&(_pNode)->wRefCount)		// increase
#define NODE_DEC_REF_COUNT(_pNode)		FFAT_ATOMIC_DEC16(&(_pNode)->wRefCount)		// decrease

// enum
typedef signed int	NodeFlag;
enum _NodeFlag
{
	//!< flag for node
	//!< do not use value between 0x00 and 0xFF, reserved for attribute
	//!< do not use value between 0x100 and 0xF00, reserved for name type
	//!< name type

	NODE_FLAG_NONE			= 0x00000000,	//!< NO FLAG

	NODE_NAME_SFN			= 0x00010000,	//!< short file name (SFNE only)
	NODE_NAME_NUMERIC_TAIL	= 0x00020000,	//!< should append numeric tail to SFNE
	NODE_NAME_UNDERSCORE	= 0x00040000,	//!< Node name has under score(_)

	NODE_ROOT_DIR			= 0x00100000,	//!< root directory
	NODE_LOCK				= 0x00200000,	//!< locked node

	NODE_UNLINKED			= 0x00400000,	//!< unlinked state
	NODE_VALID				= 0x00800000,	//!< valid entry, NODE에 대한 정상적인 정보를 가지고 있다.
											//!< lookup, rename 이후 설정된다.
	
	// [2009-10-19] new flag to protect the user data of append write
	NODE_DIRTY_SIZE			= 0x01000000,	//!< Node is in dirty-size state which is not matched
											//!< between cluster count and file size.
											//!< No need to write DE
	NODE_DIRTY_SIZE_BEGIN	= 0x02000000,	//!< Node begins the dirty size state.
											//!< At first time of expand, DE must be recorded with dirty bit.
											//!< After recording the DE, this flag will be removed.
	NODE_DIRTY_SIZE_RDONLY	= 0x04000000,	//!< the node is in dirty-size state in read-only volume.
											//!< the node structure was built at the time that volume is read-only.
											//!< In this case, the recovery about dirty-size node is not performed.
											//!< Then if the read-only attribute of volume is removed by REMOUNT,
											//!< this node must be recovered ahead of real operation
											//!< like write/rename/expand/unlink/sync

	NODE_INIT				= 0x10000000,	//!< initialized entry,
	NODE_OPEN				= 0x20000000,	//!< node is in opened state
	NODE_OPEN_UNLINK		= 0x40000000,	//!< open-unlinked state

	NODE_DUMMY				= 0x7FFFFFFF
};

typedef signed int	NodeUnlinkFlag;
enum _NodeUnlinkFlag
{
	NODE_UNLINK_NONE				= 0x00,		//!< no flag
	NODE_UNLINK_NO_LOG				= 0x01,		//!< do not write log
	NODE_UNLINK_SECURE				= 0x02,		//!< unlink with secure method.
	NODE_UNLINK_DISCARD_CACHE		= 0x04,		//!< discard data cache for this node
	NODE_UNLINK_NO_LOCK				= 0x08,		//!< does not lock
	NODE_UNLINK_OPEN				= 0x10,		//!< open unlink flag
	NODE_UNLINK_OPEN_UNLINKED_NODE	= 0x20, 	//!< final unlink after open unlink
	NODE_UNLINK_SYNC				= 0x40,		//!< do synchronous unlink (no write-back)

	NODE_UNLINK_DUMMY	= 0x7FFFFFFF
};

typedef signed int	NodeAccessFlag;
enum _NodeAccessFlag
{
	NODE_ACCESS_NONE		= 0x0000,		//!< no flag
	NODE_ACCESS_CREATE		= 0x0001,		//!< check create permission
	NODE_ACCESS_OPEN		= 0x0002,		//!< check open permission
	NODE_ACCESS_READ		= 0x0004,		//!< check read permission
	NODE_ACCESS_WRITE		= 0x0008,		//!< check write permission
	NODE_ACCESS_UNLINK		= 0x0010,		//!< check unlink permission
	NODE_ACCESS_SET_STAT	= 0x0020,		//!< check set status permission
	NODE_ACCESS_TRUNCATE	= 0x0040,		//!< check truncate permission
	NODE_ACCESS_RENAME		= 0x0080,		//!< check rename permission
	NODE_ACCESS_DELETE_NODE	= 0x0100,	

	NODE_ACCESS_MASK	= (NODE_ACCESS_CREATE | NODE_ACCESS_OPEN | NODE_ACCESS_READ
								| NODE_ACCESS_WRITE | NODE_ACCESS_UNLINK
								| NODE_ACCESS_SET_STAT | NODE_ACCESS_TRUNCATE
								| NODE_ACCESS_RENAME | NODE_ACCESS_DELETE_NODE),

	NODE_ACCESS_DUMMY	= 0x7FFFFFFF
};

typedef signed int	NodeSyncFlag;
enum _NodeSyncFlag
{
	NODE_SYNC_NONE			= 0x00,		//!< no flag
	NODE_SYNC_NO_LOCK		= 0x01,		//!< does not lock
	NODE_SYNC_NO_WRITE_DE	= 0x02,		//!< does not write directory entry
	
	NODE_SYNC_DUMMY		= 0x7FFFFFFF
};


typedef struct _NodeDeInfo
{
	t_uint32	dwDeStartCluster;	//!< cluster of directory entry
									//!< FAT16의 root directory 일 경우에는 FFATFS_FAT16_ROOT_CLUSTER(1)로 설정된다.
	t_uint32	dwDeStartOffset;	//!< Directory entry offset from begin of parent in byte
	t_uint32	dwDeEndCluster;		//!< directory entry end cluster number
									//!< may be 0 if there is no end cluster information
									//!< FAT16의 root directory 일 경우에는 FFATFS_FAT16_ROOT_CLUSTER(1)로 설정된다.
	t_uint32	dwDeEndOffset;		//!< directory entry end offset in byte
									//!< Maximum offset is (2MB - 32byte)
	t_uint32	dwDeClusterSFNE;	//!< cluster number of SFNE (need to support eXtended DE)
	t_uint32	dwDeOffsetSFNE;		//!< offset of SFNE(short file name entry)
	t_int32		dwDeCount;			//!< count of directory entry"
									//!< we can calculate with start and end offset.
									//!< Extended DE도 포함됨

	t_uint32	dwFreeCluster;		//!< free cluster number
									//!< FAT16의 root directory 일 경우에는 FFATFS_FAT16_ROOT_CLUSTER(1)로 설정된다.
	t_int32		dwFreeOffset;		//!< free entry offset from parent start in byte
	t_int32		dwFreeCount;		//!< free entry count
									//!< 0 일 경우에만 free DE 정보를 수집한다.
									//!< lookup 이후에도0일 경우에는 free DE가 없음을 의미함.
									//!< 이때는 create 과정에서 directory 의 크기를 확장해야함

	t_uint32	dwLastDeCluster;	//!< last cluster number for directory entry
									//!< to truncate directory
									//!< create flag로 lookup을 수행할 때만 설정이 된다.
									//!< FAT16의 root directory 일 경우에는 FFATFS_FAT16_ROOT_CLUSTER(1)로 설정된다.
	t_int32		dwLastDeOffset;		//!< last directory entry offset
} NodeDeInfo;


// information of Previous Access(read/write) Location
typedef struct _NodePAL
{
	t_uint32		dwOffset;		//!< start offset of location in previous write/read
									//!< FFAT_NO_OFFSET : no information
									//!< this must be cluster aligned offset
	t_uint32		dwCluster;		//!< start cluster of location in previous write/read
									//!< 0: no information
	t_uint32		dwCount;		//!< contiguous count of location in previous write/read
									//!< 0: no information
} NodePAL;

typedef struct _Node
{
	struct _Vol*	pVol;			//!< volume pointer
										// size(32 | 64bit) => 4 | 8
	ComRWLock		stRWLock;		//!< node lock
									//!< NULL일 경우 할당된 Lock이 없는 경우이다
										// size(32 | 64bit) => 12 | 24

	t_uint16		wTimeStamp;		//!< time stamp of parent(volume)
									//! volume의 time stamp와 다를 경우에는 
									//! 연관된 volume이 없다고 판단한다.
										// size(32 | 64bit) => 2 | 2

	t_int16			wNameLen;		//!< Node name length, may be NULL
									//!< 정보가 있을 경우에만 설정된다. 0일 경우는 정보가 없음.
										// size(32 | 64bit) => 2 | 2
	t_int16			wNamePartLen;	//!< character count of name part of long filename
										// size(32 | 64bit) => 2 | 2
	t_uint8			bExtPartLen;	//!< character count of extension part of long filename
										// size(32 | 64bit) => 1 | 1
	t_uint8			bSfnNameSize;	//!< byte count of name part of short filename
										// size(32 | 64bit) => 1 | 1

	t_uint32		dwCluster;		//!< cluster number for current node
									//!< 0 :there is no cluster" ;
										// size(32 | 64bit) => 4 | 4
	t_uint32		dwLastCluster;	//!< last cluster number of node
									//!< 0 : there is no last cluster information
										// size(32 | 64bit) => 4 | 4
	t_uint32		dwClusterOfParent;
									//!< parent cluster number of this node
									//!< FAT16 volume에서 FFATFS_FAT16_ROOT_CLUSTER 일 경우는 
									//!< parent가 root임을 의미
										// size(32 | 64bit) => 4 | 4

	t_uint32		dwSize;			//!< node size in byte, 0 for directory.
										// size(32 | 64bit) => 4 | 4

	FatDeSFN		stDE;			//!< directory entry
									//!< 해당 node의 directory entry 이다.
									//!< Device에 저장되는 Little Endian형태로 저장됨을 주의하라
										// size(32 | 64bit) => 32 | 32
	NodeDeInfo		stDeInfo;		//!< directory entry information for creation
									//!< 현재 node의 directory entry위 위치 정보를 갖고 있다.
									//!< creation 시에는 free directory entry까지 저장이된다.
										// size(32 | 64bit) => 48 | 48

	struct _Node*	pChildNode;		//!< create될 child에 대한 node
									//!< 이 정보는 Lookup for create에서 생성된 Node을
									//!< create/mkdir 등에서 사용하기 위함이다.
										// size(32 | 64bit) => 4 | 8

	FFatAddonNode	stAddon;		//!< data for add-on module
									//!< add-on module에서만 사용된다.. CORE에서는 접근하지 말것.!!
									//!< 이 변수는 32/64 bit Align이 맞게 시작되어야 함.
										// size(32 | 64bit) => 40 | 48

	void*			pInode;			//!< node pointer of correspondent INODE of VFS
										// size(32 | 64bit) => 4 | 8

	NodePAL			stPAL;			//!< previous access location
										// size(32 | 64bit) => 12 | 12

	NodeFlag		dwFlag;			//!< Flag of node
										// size(32 | 64bit) => 4 | 4

	t_atomic16		wRefCount;		//!< reference count, open count
										// size(32 | 64bit) => 2 | 2
	t_uint16		wSig;			//!< signature for node, NODE_SIG
										// size(32 | 64bit) => 2 | 2
#ifdef USE_64_BIT_ADDRESS
	t_uint32		dwPadd;				// size(32 | 64bit) => 0 | 4(padding)
#endif

										// total_size(byte)	=> 188 | 224
} Node;


/*
	structure for node creation operation
	It stores numeric tail information
*/
typedef struct _NodeNumericTail
{
	t_int8		pBitmap[FFAT_NUMERIC_TAIL_BUFF];
								// 빈 numeric tail을 저장하기 위한 bitmap
	t_uint16	wMin;			// minimum index to gather numeric tail information
	t_uint16	wMax;			// maximum index to gather numeric tail information
	t_uint16	wRand1;			// random으로 생성된 numeric tail number candidate 1
	t_uint16	wRand2;			// random으로 생성된 numeric tail number candidate 2
	t_uint16	wBottom;		// used numeric tail bottom
	t_uint16	wTop;			// used numeric tail top
} NodeNumericTail;


//!< constant definitions


//!< external variable declarations


//!< function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_node_init(void);
	extern FFatErr	ffat_node_terminate(void);

	extern FFatErr	ffat_node_open(Node* pNode, void* pInode, ComCxt* pCxt);
	extern FFatErr	ffat_node_close(Node* pNode, FFatNodeCloseFlag dwNCFlag, ComCxt* pCxt);

	extern FFatErr	ffat_node_initNode(struct _Vol* pVol, Node* pNodeParent,
							t_uint32 dwParentCluster, Node* pNodeChild,
							t_boolean bGetLock, ComCxt* pCxt);
	extern FFatErr	ffat_node_initRootNode(struct _Vol* pVol, Node* pNode, ComCxt* pCxt);
	extern FFatErr	ffat_node_terminateNode(Node* pNode, ComCxt* pCxt);

	extern FFatErr	ffat_node_getStatusFromDe(Node* pNode, FatDeSFN* pDE, t_uint32 dwDeCluster,
							t_uint32 dwDeOffset, FFatNodeStatus* pStatus);

	extern FFatErr	ffat_node_lookup(Node* pNodeParent, Node* pNodeChild, t_wchar* psName,
								t_int32 dwOffset, FFatLookupFlag dwFlag,
								void* pAddonInfo, ComCxt* pCxt);

	extern FFatErr	ffat_node_create(Node* pNodeParent, Node* pNodeChild, t_wchar* psName,
							FFatCreateFlag dwFlag, void* pAddonInfo, ComCxt* pCxt);
	extern t_int32	ffat_node_readWriteInit(Node* pNode, t_uint32 dwOffset, t_int8* pBuff,
							t_uint32 dwSize, FFatVC* pVC, t_uint32* pdwSizeDone,
							FFatCacheFlag dwFlag, FFatRWFlag bRWFalg, ComCxt* pCxt);
	extern FFatErr	ffat_node_rename(Node* pNodeSrcParent, Node* pNodeSrc,
							Node* pNodeDesParent, Node* pNodeDes,
							t_wchar* psName, FFatRenameFlag dwFlag, ComCxt* pCxt);

	extern FFatErr	ffat_node_unlink(Node* pNodeParent, Node* pNode,
							NodeUnlinkFlag dwNUFlag, ComCxt* pCxt);
	extern FFatErr	ffat_node_unlinkOpenUnlinked(Node* pNode, ComCxt* pCxt);

	extern FFatErr	ffat_node_setStatus(Node* pNode, FFatNodeStatus* pStatus, ComCxt* pCxt);
	extern FFatErr	ffat_node_getStatus(Node* pNode, FFatNodeStatus* pStatus, ComCxt* pCxt);
	extern FFatErr	ffat_node_sync(Node* pNode, t_uint32 dwSizeToBe, NodeSyncFlag dwFlag, ComCxt* pCxt);

	extern FFatErr	ffat_node_updateSFNE(Node* pNode, t_uint32 dwSize, t_uint8 bAttr,
					t_uint32 dwCluster, FatDeUpdateFlag dwDUFlag,
					FFatCacheFlag dwCacheFlag, ComCxt* pCxt);

	extern void		ffat_node_fillNodeInfo(Node* pNode, FatGetNodeDe* pNodeDE, void* pAddonInfo);
	extern t_boolean FFAT_FASTCALL
					ffat_node_isSameNode(Node* pNode1, Node* pNode2);

	extern FFatErr	ffat_node_createSymlink(Node* pNodeParent, Node* pNodeChild, t_wchar* psName,
								t_wchar* psPath, FFatCreateFlag dwFlag,
								void* pAddonInfo, ComCxt* pCxt);
	extern FFatErr	ffat_node_readSymlink(Node* pNode, t_wchar* psPath, t_int32 dwLen, t_int32* pdwLen, ComCxt* pCxt);
	extern FFatErr	ffat_node_isSymlink(Node* pNode);

	extern FFatErr	ffat_node_createExpandParent(Node* pNodeParent, Node* pNodeChild,
							t_int32* pdwClusterCount, t_uint32* pdwClustersDE,
							t_uint32 dwCount, ComCxt* pCxt);

	extern void		ffat_node_resetNodeStruct(Node* pNode);

	extern void		ffat_node_initPAL(Node* pNode);
	extern void		ffat_node_setPAL(Node* pNode, NodePAL* pPAL);
	extern void		ffat_node_getPAL(Node* pNode, NodePAL* pPAL);

	extern FFatErr	ffat_node_GetGUIDFromNode(Node* pNode, void* pstXDEInfo);

	extern FFatErr	ffat_node_isAccessible(Node* pNode, NodeAccessFlag dwFlag);

	// 적당한 곳으로 이동 ? ~
	extern FFatErr	ffat_node_writeDEs(Node* pNodeParent, Node* pNodeChild, FatDeSFN* pDE,
							t_int32 dwEntryCount, t_uint32* pdwClusters,
							t_int32 dwClusterCount, FFatCacheFlag dwCacheFlag, ComCxt* pCxt);
	extern void		ffat_node_initNumericTail(NodeNumericTail* pNumericTail, t_uint16 wBase);
	extern FFatErr	ffat_node_insertNumericTail(Node* pNode, NodeNumericTail* pNT);
	// ~ 적당한 곳으로 이동

// debug begin
//#ifdef FFAT_DEBUG
// 	extern FFatErr	ffat_node_lock(Node* pNode);
// 	extern FFatErr	ffat_node_unlock(Node* pNode);
//#endif /* #ifdef FFAT_DEBUG */
// debug end

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_NODE_H_ */



