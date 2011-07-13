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
 * @file		ffat_vol.h
 * @brief		This function defines FFatVol
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */


#ifndef _FFAT_VOL_H_
#define _FFAT_VOL_H_

//!< includes
#include "ess_pstack.h"

#include "ffat_common.h"
#include "ffat_node.h"

#include "ffat_addon_types.h"

//!< defines
#ifdef FFAT_DEBUG
	#define VOL_GET_READ_LOCK(_pVol)	ffat_rwlock_getReadLock(&(_pVol)->stRWLock); pCxt->dwLockCount++		// get read lock
	#define VOL_PUT_READ_LOCK(_pVol)	ffat_rwlock_putReadLock(&(_pVol)->stRWLock); pCxt->dwUnlockCount++		// put release lock

	#define VOL_GET_WRITE_LOCK(_pVol)	ffat_rwlock_getWriteLock(&(_pVol)->stRWLock); pCxt->dwLockCount++
	#define VOL_PUT_WRITE_LOCK(_pVol)	ffat_rwlock_putWriteLock(&(_pVol)->stRWLock); pCxt->dwUnlockCount++
#else
	#define VOL_GET_READ_LOCK(_pVol)	ffat_rwlock_getReadLock(&(_pVol)->stRWLock)		// get read lock
	#define VOL_PUT_READ_LOCK(_pVol)	ffat_rwlock_putReadLock(&(_pVol)->stRWLock)		// put release lock

	#define VOL_GET_WRITE_LOCK(_pVol)	ffat_rwlock_getWriteLock(&(_pVol)->stRWLock)
	#define VOL_PUT_WRITE_LOCK(_pVol)	ffat_rwlock_putWriteLock(&(_pVol)->stRWLock)
#endif

// volume time stamp
#define VOL_FLAG(_pVol)					((_pVol)->dwFlag)		//!< volume flag
#define VOL_TS(_pVol)					((_pVol)->wTimeStamp)	//!< volume time stamp
#define VOL_MSD(_pVol)					((_pVol)->wMaxSizeDE)	//!< maximum directory entry size of volume
#define VOL_DI(_pVol)					(&((_pVol)->stDevInfo))	//!< get device information pointer
#define VOL_VI(_pVol)					(&((_pVol)->stVolInfo))	//!< get volume information pointer
#define VOL_SS(_pVol)					VI_SS(VOL_VI(_pVol))	//!< get sector size
#define VOL_CS(_pVol)					VI_CS(VOL_VI(_pVol))	//!< get cluster size
#define VOL_CSB(_pVol)					VI_CSB(VOL_VI(_pVol))	//!< get cluster size bit count
#define VOL_CSM(_pVol)					VI_CSM(VOL_VI(_pVol))	//!< get cluster size mask
#define VOL_SC(_pVol)					VI_SC(VOL_VI(_pVol))	//!< get volume total sector count
#define VOL_FCC(_pVol)					VI_FCC(VOL_VI(_pVol))	//!< get free cluster count
#define VOL_SSM(_pVol)					VI_SSM(VOL_VI(_pVol))	//!< get sector size mask
#define VOL_SSB(_pVol)					VI_SSB(VOL_VI(_pVol))	//!< get sector size bit count
#define VOL_SPC(_pVol)					VI_SPC(VOL_VI(_pVol))	//!< get sector per cluster
#define VOL_SPCB(_pVol)					VI_SPCB(VOL_VI(_pVol))	//!< get sector per cluster bit count
#define VOL_FRS(_pVol)					VI_FRS(VOL_VI(_pVol))	//!< get first root sector number
#define VOL_LRS(_pVol)					VI_LRS(VOL_VI(_pVol))	//!< get last root sector number
#define VOL_RSC(_pVol)					VI_RSC(VOL_VI(_pVol))	//!< get root sector count (FAT16 only)
#define VOL_RC(_pVol)					VI_RC(VOL_VI(_pVol))	//!< get root cluster number(FAT32 only)
#define VOL_CC(_pVol)					VI_CC(VOL_VI(_pVol))	//!< get cluster count
#define VOL_DEV(_pVol)					VI_DEV(VOL_VI(_pVol))	//!< get device
#define VOL_LCN(_pVol)					VI_LCN(VOL_VI(_pVol))	//!< get last cluster number
#define VOL_FFS(_pVol)					VI_FFS(VOL_VI(_pVol))	//!< get first sector number of FAT
#define VOL_LFS(_pVol)					VI_LFS(VOL_VI(_pVol))	//!< get last sector number of FAT
#define VOL_LFSFF(_pVol)				VI_LFSFF(VOL_VI(_pVol))	//!< get first sector number of 1st FAT
																//!< be careful!!!. this sector may not have valid cluster !!!
																//!< user VI_LVFSFF to get the last fat valid sector number
#define VOL_LVFSFF(_pVol)				VI_LVFSFF(VOL_VI(_pVol))
																//!< get the last Valid Fat Sector number on First FAT
#define VOL_FSC(_pVol)					VI_FSC(VOL_VI(_pVol))	//!< get FAT sector count
#define VOL_FC(_pVol)					VI_FC(VOL_VI(pVol))		//!< get FAT Count
#define VOL_TFSC(_pVol)					(VOL_FSC(_pVol) * VOL_FC(_pVol))
																//!< get total FAT sector count
#define VOL_ROOT(_pVol)					((_pVol)->pRoot)		//!< get root node
#define VOL_EOC(_pVol)					VI_EOC(VOL_VI(_pVol))	//!< get EOC value
#define VOL_BAD(_pVol)					VI_BAD(VOL_VI(_pVol))	//!< get BAD value
#define VOL_CCPFS(_pVol)				VI_CCPFS(VOL_VI(_pVol))	//!< get cluster count per a FAT sector
#define VOL_CCPFS_MASK(_pVol)			VI_CCPFS_MASK(VOL_VI(_pVol))	//!< get cluster count per a FAT sector mask
#define VOL_CCPFSB(_pVol)				VI_CCPFSB(VOL_VI(_pVol))		//!< cluster count per a FAT sector bit
#define VOL_CCLFS(_pVol)				VI_CCLFS(VOL_VI(_pVol))	//!< get cluster count on last fat sector
#define VOL_FDS(_pVol)					VI_FDS(VOL_VI(_pVol))	//!< get first data sector

#define VOL_IS_FAT32(_pVol)				FFATFS_IS_FAT32(&(_pVol)->stVolInfo)
#define VOL_IS_FAT16(_pVol)				FFATFS_IS_FAT16(&(_pVol)->stVolInfo)
#define VOL_IS_BUSY(_pVol)				(((_pVol)->wRefCount == 0) ? FFAT_FALSE : FFAT_TRUE)
#define VOL_IS_MOUNTED(_pVol)			((VOL_FLAG(_pVol) & VOL_MOUNTED) ? FFAT_TRUE : FFAT_FALSE)
#define VOL_IS_RDONLY(_pVol)			((VOL_FLAG(_pVol) & VOL_RDONLY) ? FFAT_TRUE : FFAT_FALSE)
#define VOL_IS_MOUNTING(_pVol)			((VOL_FLAG(_pVol) & VOL_MOUNTING) ? FFAT_TRUE : FFAT_FALSE)
#define VOL_IS_SYNC_META(_pVol)			((VOL_FLAG(_pVol) & VOL_SYNC_META) ? FFAT_TRUE : FFAT_FALSE)
#define VOL_IS_SYNC_USER(_pVol)			((VOL_FLAG(_pVol) & VOL_SYNC_USER) ? FFAT_TRUE : FFAT_FALSE)
#define VOL_IS_REMOVABLE(_pVol)			((VOL_DI(_pVol)->dwFlag & FFAT_DEV_REMOVABLE) ? FFAT_TRUE: FFAT_FALSE)

#define VOL_INC_REFCOUNT(_pVol)			FFAT_ATOMIC_INC16(&(_pVol)->wRefCount)
#define VOL_DEC_REFCOUNT(_pVol)			FFAT_ATOMIC_DEC16(&(_pVol)->wRefCount)

#define VOL_GET_FAT_UPDATE_FLAG(_pVol)		((VOL_FLAG(_pVol) & VOL_FAT_MIRROR) ? FAT_UPDATE_ALL : FAT_UPDATE_1ST)

// Check is there valid volume free cluster hint (VOLUME의 FREE CLUSTER HINT 정보가 VALID 한지 점검)
#define VOL_IS_VALID_FCC(_pVol)			FFATFS_IsValidFreeClusterCount(VOL_VI(_pVol))

// check volume has erase sector flag
#define VOL_IS_ERASE_SECTOR(_pVol)		((VOL_FLAG(_pVol) & VOL_ADDON_ERASE_SECTOR) ? FFAT_TRUE : FFAT_FALSE)

#define FFAT_FREE_CLUSTER_INVALID		(0xFFFFFFFF - 10)
												// free cluster count is invalid 
												// there is no cache free cluster count
												// do not change this value
												// same as FFATFS_FREE_CLUSTER_INVALID

//!< enum
typedef signed int	VolFlag;
enum _VolFlag
{
	VOL_FREE				= 0x00000000,	//!< free 
	VOL_MOUNTED				= 0x00000001,	//!< mounted volume
	VOL_FAT_MIRROR			= 0x00000002,	//!< enable FAT mirroring
	VOL_RDONLY				= 0x00000004,	//!< read only volume
	VOL_MOUNTING			= 0x00000008,	//!< volume is in mounting process
	VOL_SYNC_META			= 0x00000010,	//!< sync all meta date update
	VOL_SYNC_USER			= 0x00000020,	//!< sync all user date update

	VOL_CASE_SENSITIVE		= 0x00000040,	//!< (FAT incompatibility) Case sensitive naming rule
	VOL_OS_SPECIFIC_CHAR	= 0x00000080,	//!< (FAT incompatibility) Support OS specific character set

	// flags for ADDON module
	//		why do I write flags for ADDON at here ?
	//		this is not good design. but.. there are not may flags for volume
	//		Add a new data type at AddonVol when flags for ADDON are overflows~.
	VOL_ADDON_LOGGING		= 0x00001000,	//!< volume is mounted with log feature
	VOL_ADDON_LLW			= 0x00002000,	//!< lazy log writer feature (only for write operation)
	VOL_ADDON_FULL_LLW		= 0x00004000,	//!< full lazy log writer (for all operation)
	VOL_ADDON_FCC			= 0x00008000,	//!< FCC is enabled on current volume
	VOL_ADDON_XDE			= 0x00010000,	//!< (FAT incompatibility) support owner/group id, permission
	VOL_ADDON_XATTR			= 0x00020000,	//!< (FAT incompatibility) support extended attributes
	VOL_ADDON_SPECIAL_FILES	= 0x00040000,	//!< support special files (symbolic link, fifo, socket)
	VOL_ADDON_ERASE_SECTOR	= 0x00080000,	//!< request block erase to block device while cluster deallocation

	VOL_LFN_ONLY			= (VOL_CASE_SENSITIVE | VOL_OS_SPECIFIC_CHAR),
											//!< use only Long file name

	VOL_DUMMY				= 0x7FFFFFFF
};

//!< typedefs
typedef struct _Vol
{
	FFatLDevInfo	stDevInfo;		//!< device information
										// size(32 | 64bit) => 64 | 64

	FatVolInfo		stVolInfo;		//!< volume information
										// size(32 | 64bit) => 120 | 144

	ComRWLock		stRWLock;		//!< volume RW lock 
									//!< Caution, This is not a pseudo RW lock 
									//!<	to prevent concurrent volume update with CHKDSK
									//!<	All of the normal operation uses read lock only
									//!<	CHKDSK uses write lock
									//!<	to update some of Vol entry use fine-grain main lock
										// size(32 | 64bit) => 12 | 24

	Node*			pRoot;			//!< root node pointer
										// size(32 | 64bit) => 4 | 8

	FFatAddonVol	stAddon; 		//!< for ADDON module
									//!< 이 변수는 32/64 bit Align이 맞게 시작되어야 함.
										// size(32 | 64bit) => 16 | 32

	VolFlag			dwFlag;			//!< volume flag
										// size(32 | 64bit) => 4 | 4

	t_atomic16		wRefCount;		//!< reference count, current task count in the volume
										// size(32 | 64bit) => 2 | 2
	t_uint16		wTimeStamp;		//!< volume time stamp
										// size(32 | 64bit) => 2 | 2
	t_uint16		wMaxSizeDE;		//!< maximum directory entry size of volume
									//!< this variable is for XDE
									//!< The maximum DE count for a node on for XDE is (FAT_MAX_DE_COUNT_MAX + 1) 
										// size(32 | 64bit) => 2 | 2
	t_uint16		wDummy;			//!< dummy for byte alignment
										// size(32 | 64bit) => 2 | 2
#ifdef USE_64_BIT_ADDRESS
	t_uint32		dwPadd;				// size(32 | 64bit) => 0 | 4(padding)
#endif

										// total_size(byte)	=> 228 | 288
} Vol;

//!< constant definitions


//!< external variable declarations


//!< function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_vol_init(void);
	extern FFatErr	ffat_vol_terminate(void);

	extern FFatErr	ffat_vol_mount(Vol* pVol, Node* pRoot, FFatMountFlag* pdwFlag,
								FFatLDevInfo* pLDevInfo, void* pDev, ComCxt* pCxt);
	extern FFatErr	ffat_vol_umount(Vol* pVol, FFatMountFlag dwFlag, ComCxt* pCxt);
	extern FFatErr	ffat_vol_remount(Vol* pVol, FFatMountFlag* pdwFlag, ComCxt* pCxt);

	extern FFatErr	ffat_vol_getBSInfoFromBS(void* pDev, t_int32 dwIOSize,
								t_int32* pdwSectorSize, t_int32* pdwClusterSize,
								t_uint32* pdwFirstDataSector, ComCxt* pCxt);

	extern FFatErr	ffat_vol_sync(Vol* pVol, t_boolean bLock, ComCxt* pCxt);

	extern FFatErr	ffat_vol_getStatus(Vol* pVol, FFatVolumeStatus* pStatus,
								t_int8* pBuff, t_int32 dwSize, ComCxt* pCxt);

	extern FFatErr	ffat_vol_getVolumeLabel(Vol* pVol, t_wchar* psVolLabel,
								t_int32 dwVolLabelLen, ComCxt* pCxt);
	extern FFatErr	ffat_vol_setVolumeLabel(Vol* pVol, t_wchar* psVolLabel, ComCxt* pCxt);

// debug begin
#ifdef FFAT_DEBUG
	extern FFatErr	ffat_vol_lock(Vol* pVol);
	extern FFatErr	ffat_vol_unlock(Vol* pVol);
#endif
// debug end

#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFAT_VOL_H_ */



