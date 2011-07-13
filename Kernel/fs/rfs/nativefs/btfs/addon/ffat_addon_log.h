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
 * @file	ffat_addon_log.h
 * @brief	log manager with write ahead log, WAL, scheme
 * @author	QingZhang
 * @version
 * @see		None
 */

#ifndef _FFAT_ADDON_LOG_H_
#define _FFAT_ADDON_LOG_H_

#include "ffat_types.h"
#include "ffatfs_types.h"
#include "ffat_node.h"
#include "ffat_vol.h"

#include "ffat_addon_xattr.h"

#define LOG_MAX_SLOT						64	// log file size in sector unit
#define LOG_EXT_SLOT						1	// extra log count in sector unit

#define LOG_OPEN_UNLINK_SLOT				64	// open unlink log in sector unit , It can be configurable.
												// must be power of 2
#define LOG_OPEN_UNLINK_SLOT_MASK			(LOG_OPEN_UNLINK_SLOT - 1)	// mask for slot

#define LOG_OPEN_UNLINK_ENTRY_SLOT			120	// open unlink log entry count in sector unit, fixed value
#define LOG_OPEN_UNLINK_BITMAP_BYTE			15	// open unlink log bitmap byte
#define LOG_OPEN_UNLINK_HEADER_SIZE			32	// open unlink header byte size in sector unit

#define LOG_MAX_OPEN_UNILNK_ENTRY			(LOG_OPEN_UNLINK_SLOT * LOG_OPEN_UNLINK_ENTRY_SLOT)	// 8 * 120

#define LOG_SLOT_COUNT						(LOG_MAX_SLOT + LOG_EXT_SLOT + LOG_OPEN_UNLINK_SLOT)

#define LOG_LLW_CACHE_SIZE					4096		// cache size for lazy log writer
														// it should be bigger than the sector size of block device
														// the maximum sector size for 

#define FFAT_IS_LOGGING_ENABLED(_pVol)		(((_pVol)->dwFlag & VOL_ADDON_LOGGING) ? FFAT_TRUE : FFAT_FALSE)

#define LOG_SECTOR_COUNT		(LOG_SLOT_COUNT << 1)	// sector count for log area
														// ADD BACKUP AREA (regardless of debug on/off)

#define FFAT_LOG_CREATE_INFO_SIGNATURE			".btfs?LogCre"
#define FFAT_LOG_CREATE_INFO_SIGNATURE_SIZE		12

#define LOG_IS_LOG(_pNode) ((NODE_ADDON_FLAG(_pNode) & ADDON_NODE_LOG) ? FFAT_TRUE : FFAT_FALSE)

// Log data structure
typedef unsigned int	LogType;
enum _LogType
{
	LM_LOG_NONE				= 0x00000000,		//!< No Log data, Do not change this value
	LM_LOG_CREATE_NEW		= 0x00000001,		//!< create a new node
	LM_LOG_UNLINK			= 0x00000002,		//!< unlink a node
	LM_LOG_EXTEND			= 0x00000004,		//!< extend size of a node
	LM_LOG_SHRINK			= 0x00000008,		//!< shrink size of a node
	LM_LOG_WRITE			= 0x00000010,		//!< write
	LM_LOG_RENAME			= 0x00000020,		//!< rename
	LM_LOG_SET_STATE		= 0x00000040,		//!< set state
	LM_LOG_TRUNCATE_DIR		= 0x00000080,		//!< truncate a directory
	LM_LOG_EXPAND_DIR		= 0x00000100,		//!< expand a directory
	LM_LOG_CONFIRM			= 0x00000200,		//!< confirm log for prevent undo operation
	LM_LOG_HPA_CREATE		= 0x00000400,		//!< for hidden protected area create, redo

	LM_LOG_EA_CREATE		= 0x00000800,		//!< set extended attribute (first)
	LM_LOG_EA_SET			= 0x00001000,		//!< set extended attribute
	LM_LOG_EA_DELETE		= 0x00002000,		//!< delete extended attribute
	LM_LOG_EA_COMPACTION	= 0x00004000,		//!< compaction extended attribute
	LM_LOG_CREATE_SYMLINK	= 0x00008000,		//!< symbolic link creation

	//!< common flag
	LM_LOG_OPEN_UNLINK		= 0x01000000,		//!< open unlink log
	LM_LOG_FLAG_LLW			= 0x02000000,		//!< log for lazy log writer
	LM_LOG_FLAG_LLW_BEGIN	= 0x04000000,		//!< the first lazy log entry
	LM_LOG_FLAG_BACKUP		= 0x08000000,		//!< flag for backup, debug only

	LM_LOG_FLAG_MASK		= LM_LOG_FLAG_LLW | LM_LOG_FLAG_BACKUP | LM_LOG_OPEN_UNLINK,
												//!< mask for common type
	LM_LOG_LLW_FLAG_MASK	= LM_LOG_WRITE | LM_LOG_EXTEND,
												//!< log type for LLW mask

	LM_LOG_MASK				= 0x7FFFFFFF
};


// do not exceed 2byte
typedef signed int	LogDirtyFlag;

enum _LogDirtyFlag
{
	LM_FLAG_NONE			= 0x0000,		//!< no flag
	LM_FLAG_SYNC			= 0x0001,		//!< need to be sync
	LM_FLAG_DE_DIRTY		= 0x0002,		//!< DE may be cached
	LM_FLAG_FAT_DIRTY		= 0x0004,		//!< FAT may be cached
	LM_FLAG_DIRTY			= (LM_FLAG_DE_DIRTY | LM_FLAG_FAT_DIRTY),	// DIRTY
	LM_FLAG_DUMMY			= 0x7FFFFFFF
};

typedef signed int LogInfoFlag;
enum _LogInfoFlag
{
	LI_FLAG_NONE			= 0x0000,		//!< no flag
	LI_FLAG_NO_LOG			= 0x0001,		//!< no valid log in this log info

	LI_FLAG_DUMMY			= 0x7FFFFFFF	//!< dummy
};


#ifdef LM_RECOVERY_SCAN_REMOVABLE_DEVICE
	typedef struct
	{
		Node*				pNode;			//!< Node
		FatGetNodeDe*		pNodeDE;		//!< DE for a Node
	} LogRmvDevMem;
#endif

typedef struct _OUInfo
{
	t_uint32		dwOUEntryIndex;			//!< open unlink node index in open unlink log area
											//!< upper 8byte is open unlink log slot index
											//!< lower 8byte is offset in log slot
	t_uint32		dwEAEntryIndex;			//!< extended attribute index in open unlink log area
} LogOUInfo;

// structure for LLW
typedef struct _LogLazyWriter
{
	Vol*			pVol;						//!< current volume
												//!< if NULL there is no LL
	t_int16			wLogSlot;					//!< log slot index
												//!<	-1 : slot for LLW did not assigned yet.
	t_int16			wRecentLLSize;				//!< recent LL size for undoing last one
	t_int8*			pCurPtr;					//!< current buffer pointer
	t_int8			pBuff[LOG_LLW_CACHE_SIZE];	//!< cache size for log
} LogLazyWriter;

// Log information manager
typedef struct _LogInfo
{
	LogInfoFlag		dwFlag;						//!< flag for log information
	t_uint32		udwSeqNum;					//!< sequence number of log record to write
	t_int16			wCurSlot;					//!< index of current log slot
	LogType			udwPrevLogType;				//!< previous log type (LogType)
												//!< QUIZ : Why do i use LogType type ?

	LogDirtyFlag	uwPrevDirtyFlag;			//!< dirty flag for previous log

#ifdef LM_RECOVERY_SCAN_REMOVABLE_DEVICE
	LogRmvDevMem*	pstRmvDevMem;				//!< memory used for scan removable device
#endif

													//!< QUIZ : (JAN-29-2008)
													//!<	why do I assign all sectors one to one
													//!<	instead of sector extent ?
	t_uint32		pdwSectors[LOG_SECTOR_COUNT];	//!< sectors for log area

	t_uint32		dwFirstCluster;				//!< the first cluster of log file.
												//!< 0 : log file is at the reserved area.

	t_uint32		udwOUFreeSectorHint;		//!< Sector hint number offset
												//!<	that have free open unlink log slot,
												//!<	start 0 sector

	LogLazyWriter*	pLLW;						//!< lazy log write for this volume
												//!< this if for dynamic memory allocation
} LogInfo;

//!< Information of log creation
typedef struct _LogCreatInfo
{
	t_uint8		szSignature[FFAT_LOG_CREATE_INFO_SIGNATURE_SIZE]; //!< Automatically be set in write function
	t_uint32	dwDeStartCluster;		//!< start cluster of log directory entry
	t_uint32	dwDeStartOffset;		//!< start offset of log directory entry
	t_uint32	dwDeCount;				//!< count of directory entries
	t_uint32	dwStartCluster;			//!< start cluster of Log
} LogCreatInfo;


// Log external API
#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr		ffat_log_init(void);
	extern FFatErr		ffat_log_terminate(void);

	extern FFatErr		ffat_log_operationFail(Vol* pVol, LogType dwType,
								Node* pNode, ComCxt* pCxt);

	extern FFatErr		ffat_log_expandDir(Node* pNode, t_uint32 udwPrevEOC,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);

	extern FFatErr		ffat_log_create(Node* pNodeParent, Node* pNodeChild,
								t_wchar* psName, FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);
	extern FFatErr		ffat_log_createSymlink(Node* pNode, t_wchar* psName,
								FFatVC* pVC, FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);

	extern FFatErr		ffat_log_unlink(Node* pNode, FFatVC* pVC, NodeUnlinkFlag dwNUFlag,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);

	extern FFatErr		ffat_log_unlinkOpenUnlinkedNode(Node* pNode, ComCxt* pCxt);
	extern FFatErr		ffat_log_afterUnlinkOpenUnlinkedNode(Node* pNode, ComCxt* pCxt);

	extern FFatErr		ffat_log_changeSize(Node* pNode, t_uint32 udwSize, t_uint32 dwEOF,
								FFatVC* pVC, FFatChangeSizeFlag dwCSFlag,
								FFatCacheFlag *pdwCacheFlag, ComCxt* pCxt);

	extern FFatErr		ffat_log_writeFile(Node* pNode, t_uint32 dwLastOffset, t_uint32 dwPrevEOC,
								FFatVC* pVC_Cur, FFatVC* pVC_New, FFatWriteFlag dwWriteFlag,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCx);

	extern FFatErr		ffat_log_createEA(Node* pNode, FFatVC* pVC, FFatCacheFlag* pdwCacheFlag,
								ComCxt* pCxt);
	extern FFatErr		ffat_log_setEA(Node* pNode, FFatVC* pVCOld, FFatVC* pVCNew, t_uint32 udwDelOffset,
								t_uint32 udwInsOffset, EAMain* pEAMain, EAEntry* pEAEntryOld,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);
	extern FFatErr		ffat_log_deleteEA(Node* pNode, t_uint32 udwFirstCluster,
								t_uint32 udwDelOffset, EAMain* pEAMain, EAEntry* pEAEntry,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);
	extern FFatErr		ffat_log_compactEA(Node* pNode, FFatVC* pOldVC, FFatVC* pNewVC,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);

	extern FFatErr		ffat_log_truncateDir(Node* pNode, t_uint32 udwPrevEOC,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);

	extern FFatErr		ffat_log_rename(Node* pNodeSrcParent, Node* pNodeSrc, Node* pNodeDesParent,
								Node* pNodeDes, Node* pNodeNewDes, t_wchar* psName,
								FFatRenameFlag dwFlag, FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);

	extern FFatErr		ffat_log_setStatus(Node* pNode, FFatNodeStatus* pStatus,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);
	extern FFatErr		ffat_log_updateXDE(Node* pNode, FFatExtendedDirEntryInfo* pNewXDEInfo,
								FFatCacheFlag* pdwCacheFlag, ComCxt* pCxt);

	extern FFatErr		ffat_log_confirm(Node* pNode, ComCxt* pCxt);

	extern FFatErr		ffat_log_mount(Vol* pVol, FFatMountFlag* pdwFlag, ComCxt* pCxt);
	extern FFatErr		ffat_log_remount(Vol* pVol, FFatMountFlag* pdwFlag, ComCxt* pCxt);
	extern FFatErr		ffat_log_umount(Vol* pVol, ComCxt* pCxt);

	extern FFatErr		ffat_log_syncVol(Vol* pVol, ComCxt* pCxt);
	extern FFatErr		ffat_log_afterSyncVol(Vol* pVol, FFatCacheFlag dwCacheFlag, ComCxt* pCxt);
	extern FFatErr		ffat_log_initNode(Node* pNode);

	extern FFatErr		ffat_log_cacheCallBack(Vol* pVol, ComCxt* pCxt);

	extern FFatErr		ffat_log_isAccessable(Node* pNode, NodeAccessFlag dwFlag);
	extern t_boolean	ffat_log_isLogNode(Node* pNode);

	extern FFatErr		ffat_log_initLogArea(Vol* pVol, void* pDevice, t_uint32 dwStartSector,
								t_uint32 dwEndSector, ComCxt* pCxt);

	// for HPA
	extern FFatErr		ffat_log_hpa(Vol* pVol, LogType dwLogType, ComCxt* pCxt);

	extern FFatErr		ffat_log_getClusterOfOUEntry(Vol* pVol, t_int32 dwIndex,
									t_uint32* pdwCluster, ComCxt* pCxt);

#ifdef __cplusplus
	};
#endif

#endif	/* _FFAT_ADDON_LOG_H_ */

