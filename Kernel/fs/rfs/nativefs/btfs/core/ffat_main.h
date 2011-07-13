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
 * @file		ffat_main.h
 * @brief		This function defines FFatMain
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFAT_MAIN_H_
#define _FFAT_MAIN_H_

//!< includes

#include "ess_types.h"

#include "ffat_types.h"
#include "ffat_node.h"

#include "ffat_al.h"

#include "ffatfs_types.h"

//!< defines
#define FFAT_IS_INITIALIZED()		((ffat_main_getMainInfo()->dwFlag & FFAT_MAIN_INIT) ? FFAT_TRUE : FFAT_FALSE)

#define FFAT_INIT_ERROR_PRINT()

// debug begin
#ifdef FFAT_DEBUG
	#undef FFAT_INIT_ERROR_PRINT
	#define FFAT_INIT_ERROR_PRINT()		FFAT_LOG_PRINTF(_T("FFat is not initialized")
#endif
// debug end

#define FFAT_CHECK_INIT_RETURN()		do { \
										if (FFAT_IS_INITIALIZED() != FFAT_TRUE)	\
										{	\
											return FFAT_EINIT;	\
										}	\
									} while(0)

#define _MAIN_LOCK()			FFAT_LOCK_GET(ffat_main_getMainInfo()->pLock)
#define _MAIN_UNLOCK()			FFAT_LOCK_PUT(ffat_main_getMainInfo()->pLock)
#define FFAT_MAIN_LOCK()		_MAIN_LOCK()
#define FFAT_MAIN_UNLOCK()		_MAIN_UNLOCK()

#define GLOBAL_LOCK_ERR_PRINT()
#define GLOBAL_UNLOCK_ERR_PRINT()

// debug begin
#ifdef FFAT_DEBUG
	#undef GLOBAL_LOCK_ERR_PRINT
	#undef GLOBAL_UNLOCK_ERR_PRINT
	#define GLOBAL_LOCK_ERR_PRINT()		FFAT_LOG_PRINTF((_T("fail to lock global")))
	#define GLOBAL_UNLOCK_ERR_PRINT()	FFAT_LOG_PRINTF((_T("fail to un-lock global")))
#endif
// debug end

// global lock for single big lock configuration
#if (FFAT_LOCK_TYPE == FFAT_LOCK_NONE) || (FFAT_LOCK_TYPE == FFAT_LOCK_MULTIPLE)
	#define FFAT_LOCK_GLOBAL()				// FFAT global lock
	#define FFAT_UNLOCK_GLOBAL()			// FFAT global lock
	#define FFAT_LOCK_GLOBAL_RETURN()
	#define FFAT_UNLOCK_GLOBAL_RETURN()

// debug begin
	#ifdef FFAT_DEBUG
		#undef FFAT_MAIN_LOCK
		#undef FFAT_MAIN_UNLOCK
		#define FFAT_MAIN_LOCK()		ffat_main_lock()
		#define FFAT_MAIN_UNLOCK()		ffat_main_unlock()
	#endif
// debug end

#elif (FFAT_LOCK_TYPE == FFAT_LOCK_SINGLE)
	#undef FFAT_MAIN_LOCK
	#undef FFAT_MAIN_UNLOCK
	#define FFAT_MAIN_LOCK()		FFAT_OK		// FFAT main structrue lock
	#define FFAT_MAIN_UNLOCK()		FFAT_OK		// FFAT main structrue lock

	#define FFAT_LOCK_GLOBAL()			ffat_al_getLock(ffat_main_getMainInfo()->pLock)
	#define FFAT_UNLOCK_GLOBAL()		ffat_al_putLock(ffat_main_getMainInfo()->pLock)

	#define FFAT_LOCK_GLOBAL_RETURN()	do {	\
											if (FFAT_LOCK_GLOBAL() != FFAT_OK)	\
											{	\
												GLOBAL_LOCK_ERR_PRINT();	\
												return FFAT_EPANIC;	\
											}	\
										} while(0)

	#define FFAT_UNLOCK_GLOBAL_RETURN()	do {	\
											if (FFAT_UNLOCK_GLOBAL() != FFAT_OK)	\
											{	\
												GLOBAL_UNLOCK_ERR_PRINT();	\
												return FFAT_EPANIC;	\
											}	\
										} while(0)
#else
	#error Invalid Lock configuration !!!
#endif


#if (FFAT_LOCK_TYPE == FFAT_LOCK_NONE)
	#define FFAT_MAIN_GET_FREE_LOCK_FOR_MAIN(_ppLock)		FFAT_OK
#else
	#define FFAT_MAIN_GET_FREE_LOCK_FOR_MAIN(_ppLock)		ffat_al_getFreeLock(_ppLock)
#endif

#define FFAT_MAIN_CHECK_TIME_STAMP(_pVol, _pNode)	((VOL_TS(_pVol) == NODE_TS(_pNode)) ? FFAT_TRUE : FFAT_FALSE)

#define FFAT_MAIN_INC_REFCOUNT()		FFAT_ATOMIC_INC32(&(ffat_main_getMainInfo()->dwRefCount))
#define FFAT_MAIN_DEC_REFCOUNT()		FFAT_ATOMIC_DEC32(&(ffat_main_getMainInfo()->dwRefCount))
#define FFAT_MAIN_INC_TIMESTAMP()		FFAT_ATOMIC_INC16(&(ffat_main_getMainInfo()->wCurTimeStamp))
#define FFAT_MAIN_DEC_TIMESTAMP()		FFAT_ATOMIC_DEC16(&(ffat_main_getMainInfo()->wCurTimeStamp))

//!< enum
typedef signed int	FFatFlag;
enum _FFatFlag
{
	FFAT_MAIN_NONE		= 0x00000000,	//!< no flag
	FFAT_MAIN_INIT		= 0x00000001,	//!< initialized

	FFAT_MAIN_DUMMY		= 0x7FFFFFFF
};

//!< typedefs
typedef FFatErr	(*PFN_FSCTL)(void* pParam0, void* pParam1, void* pParam2, ComCxt* pCxt);

typedef struct _FFatMainInfo
{
	void*			pLock;				//!< FFAT global lock 
	t_atomic32		dwRefCount;			//!< reference count, mountµÈ volumeÀÇ ¼ö
	FFatFlag		dwFlag;				//!< FFAT main flag 
	
	t_atomic16		wCurTimeStamp;		//!< current time stamp */
										//!< it will increase after volume mount operation
										//!< and the number is store at volume time state
} FFatMainInfo;


//!< context for current task
typedef struct _CoreCxt
{
	void*			pPStack;			//!< Current PStack pointer
} CoreCxt;

//!< constant definitions

//!< external variable declarations

// debug begin
#ifdef FFAT_DEBUG
	#define FFAT_DEBUG_EIO_CHECK(_dwType)		if (ffat_main_LogRecoveryTestCheck(_dwType) < 0) { r = FFAT_EIO; goto out;}
	#define FFAT_DEBUG_LRT_CHECK(_dwType)		ffat_main_LogRecoveryTestCheck(_dwType)
#else
	#define FFAT_DEBUG_EIO_CHECK(_dwType)		((void)0)
	#define FFAT_DEBUG_LRT_CHECK(_dwType)		((void)0)
#endif
// debug end


//!< function declarations
#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_main_init(t_boolean bForce);
	extern FFatErr	ffat_main_terminate(void);

	extern FFatErr	ffat_main_fsctl(FFatFSCtlCmd dwCmd, void* pParam0, void* pParam1,
								void* pParam2, ComCxt* pCxt);
	extern FFatErr	ffat_main_cacheCallBack(FatVolInfo* pVolInfo, t_uint32 dwSector,
								FFatCacheFlag dwFlag, ComCxt* pCxt);

	extern FFatMainInfo*	ffat_main_getMainInfo(void);

#if (FFAT_LOCK_TYPE == FFAT_LOCK_MULTIPLE) || (FFAT_LOCK_TYPE == FFAT_LOCK_SINGLE)
	extern FFatErr	ffat_main_getFreeLockForMain(void** ppLock);
	extern FFatErr	ffat_main_getFreeLock(void** ppLock);
	extern FFatErr	ffat_main_releaseLock(void** ppLock);
#endif	/* end of #if (FFAT_LOCK_TYPE == FFAT_LOCK_MULTIPLE) */

	extern FFatErr	ffat_core_lock(ComCxt* pCxt);
	extern FFatErr	ffat_core_unlock(ComCxt* pCxt);

// debug begin
#ifdef FFAT_DEBUG
	extern FFatErr	ffat_main_lock(void);
	extern FFatErr	ffat_main_unlock(void);

	extern FFatErr	ffat_main_LogRecoveryTestCheck(FFatLogRecoveryTestFlag dwType);

	extern void	ffat_main_statisticMount(void);
	extern void	ffat_main_statisticUmount(void);
	extern void	ffat_main_statisticGetVolumeStatus(void);
	extern void	ffat_main_statisticSetVolumeLabel(void);
	extern void	ffat_main_statisticGetVoluemName(void);
	extern void	ffat_main_statisticLookup(void);
	extern void	ffat_main_statisticFSCtl(void);
	extern void	ffat_main_statisticMakedir(void);
	extern void	ffat_main_statisticRemovedir(void);
	extern void	ffat_main_statisticReaddir(void);
	extern void	ffat_main_statisticCreate(void);
	extern void	ffat_main_statisticOpen(void);
	extern void	ffat_main_statisticClose(void);
	extern void	ffat_main_statisticRead(void);
	extern void	ffat_main_statisticWrite(void);
	extern void	ffat_main_statisticUnlink(void);
	extern void	ffat_main_statisticSecureUnlink(void);
	extern void	ffat_main_statisticChangeSize(void);
	extern void	ffat_main_statisticRename(void);
	extern void	ffat_main_statisticSetNodeStatus(void);
	extern void	ffat_main_statisticGetNodeStatus(void);
	extern void	ffat_main_statisticSyncNode(void);
	extern void	ffat_main_statisticSyncVol(void);
	extern void	ffat_main_statisticSync(void);
	extern void	ffat_main_statisticGetNodeClusters(void);
#endif

// debug end 

#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFAT_MAIN_H_ */



