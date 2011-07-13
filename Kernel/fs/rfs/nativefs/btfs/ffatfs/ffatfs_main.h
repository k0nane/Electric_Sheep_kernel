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
 * @file		ffatfs_main.h
 * @brief		The main module for FFATFS
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFATFS_MAIN_H_
#define _FFATFS_MAIN_H_

// includes
#include "ffat_config.h"
#include "ffat_al.h"

#include "ffatfs_types.h"
#include "ffatfs_cache.h"

// defines
#define FFATFS_IS_INITIALIZED()		ffat_fs_isInitialized()

#define FFATFS_CHECK_INIT_RETURN()	if (FFATFS_IS_INITIALIZED() == FFAT_FALSE)	\
									{	\
										return FFAT_EINIT;	\
									}

#if (FFATFS_LOCK_TYPE == FFATFS_LOCK_NONE)
	#define FFATFS_LOCK_GET_FREE(_ppLock)	FFAT_OK
	#define FFATFS_LOCK_RELEASE(_pLock)		FFAT_OK
	#define FFATFS_LOCK_GET(_pLock)			FFAT_OK
	#define FFATFS_LOCK_PUT(_pLock)			FFAT_OK

	#define FFATFS_LOCK_INITIAL_COUNT		0		//!< do not need any lock
	#define FFATFS_LOCK_COUNT_MAX			1		//!< max lock count
#elif (FFATFS_LOCK_TYPE == FFATFS_LOCK_SINGLE)
	#define FFATFS_LOCK_GET_FREE(_ppLock)	ffat_al_getFreeLock(_ppLock)
	#define FFATFS_LOCK_RELEASE(_ppLock)	ffat_al_releaseLock(_ppLock)
	#define FFATFS_LOCK_GET(_pLock)			ffat_al_getLock(_pLock)
	#define FFATFS_LOCK_PUT(_pLock)			ffat_al_putLock(_pLock)

	#define FFATFS_LOCK_INITIAL_COUNT		1		//!< initialia lock creation count
	#define FFATFS_LOCK_COUNT_MAX			1		//!< max lock count
#else
	#error	"Invalid FFATFS_LOCK_TYPE configuration!!!"
#endif

#if (FFATFS_LOCK_TYPE == FFATFS_LOCK_NONE)
	#define FFATFS_LOCK(_pCxt)			FFAT_OK
	#define FFATFS_UNLOCK(_pCxt)		FFAT_OK
#else
	#define FFATFS_LOCK(_pCxt)			ffat_fs_getLock(_pCxt)
	#define FFATFS_UNLOCK(_pCxt)		ffat_fs_putLock(_pCxt)
#endif	/* #ifdef FFATFS_NOLOCK */


// enum

// typedefs

typedef signed int	FFatfsMainFlag;
enum _FFatfsMainFlag
{
	FFATFS_MAIN_NONE		= 0x00,		//!< no data
	FFATFS_MAIN_INIT		= 0x01,		//!< Initialized

	FFATFS_MAIN_DUMMY		= 0x7FFFFFFF
};

typedef struct
{
	FFatfsMainFlag		dwFlag;			//!< initialization state
	FFatLock*			pLock;			//!< FFatfs global lock
	t_uint16			wTimeStamp;		//!< Time stamp
	t_int16				wRefCount;		//!< reference count, re-entrant count
} FFatfsMainInfo;

// constant definitions

// external variable declarations
extern const FatVolOperation	gstVolOp16;
extern const FatVolOperation	gstVolOp32;
// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_fs_init(t_boolean bForce);
	extern FFatErr	ffat_fs_terminate(void);
	extern FFatErr	ffat_fs_fsctl(FatFSCtlCmd dwCmd, void* pParam0, void* pParam1, void* pParam2);

	// FFATFS API

	extern FFatfsCacheInfo* ffat_fs_getDefaultCache(void);
	extern FFatErr	ffat_fs_initVolInfo(FatVolInfo* pVolInfo);

	extern FFatErr	ffat_fs_getLock(ComCxt* pCxt);
	extern FFatErr	ffat_fs_putLock(ComCxt* pCxt);

	extern t_boolean	ffat_fs_isInitialized(void);
	extern void		ffat_fs_incTimeStamp(void);
	extern t_uint16	ffat_fs_getTimeStamp(void);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFATFS_MAIN_H_ */

