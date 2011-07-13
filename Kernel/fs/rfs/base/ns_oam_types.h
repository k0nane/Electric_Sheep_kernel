/*
* Nestle filesystem platform Developed by Flash Software Group.
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
* @brief	Nestle OAM types
* @author	ByungJune Song (byungjune.song@samsung.com)
* @author	InHwan Choi (inhwan.choi@samsung.com)
* @file		ns_oam_types.h
*/

#ifndef __NS_OAM_TYPES_H__
#define __NS_OAM_TYPES_H__

#include "ns_config.h"
#include "ns_oam_defs.h"
#include "nsd_base.h"
#include "ess_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/******************************************************************************/
/* Time	Macros																  */
/******************************************************************************/
#define TIME_HOUR(v)			(((v) & 0x07C00000U) >> 22)
#define TIME_MIN(v)				(((v) & 0x003F0000U) >> 16)
#define TIME_SEC(v)				(((v) & 0x0000FC00U) >> 10)
#define TIME_MILISEC(v)			((v) &  0x000003FFU)

#define DATE_YEAR(v)			((((v) & 0x0000FE00U) >> 9) + 1980)
#define DATE_MONTH(v)			(((v) &  0x000001E0U) >> 5)
#define DATE_DAY(v)				((v) &	 0x0000001FU)

#define MAKE_TIME(hour, min, sec, milisec)										\
								(((milisec) &		0x000003FFU) |				\
								(((sec) << 10) &	0x0000FC00U) |				\
								(((min) << 16) &	0x003F0000U) |				\
								(((hour) << 22) &	0x07C00000U))		
#define MAKE_DATE(year, mon, day)												\
								(((day) &				0x0000001FU) |			\
								(((mon) << 5) &			0x000001E0U) |			\
								((((year)-1980) << 9) & 0x0000FE00U))

/******************************************************************************/
/* MISC. Macros																  */
/******************************************************************************/
#define ULONGLONG_TO_LOW_UINT(eight)		((unsigned int) ((eight) & 0x00000000FFFFFFFFULL))		
#define ULONGLONG_TO_HIGH_UINT(eight)		((unsigned int) (((eight) >> 32) & 0x00000000FFFFFFFFULL))
#define UINT_TO_ULONGLONG(high, low)		((((unsigned long long) (high)) << 32) | (unsigned int)(low))
#define INT_TO_LONGLONG(high, low)			((high) ? ((((long long) (high)) << 32) | (unsigned int)(low)) : (long long)(low))

#define MIN(x, y)							(((x) < (y)) ? (x) : (y))
#define	MAX(x, y)							(((x) > (y)) ? (x) : (y))
#define ABS(n)								( ((n) < 0) ? -1*(n) : (n) )

#if (BYTE_ORDER == LITTLE_ENDIAN)
	#define RTL_UNICODE_MAKEWORD(first, second)		(((second) << 8) + (first))
#else
	#define RTL_UNICODE_MAKEWORD(first, second)		(((first) << 8) + (second))
#endif

#if (BYTE_ORDER == LITTLE_ENDIAN)
#define	BOU16(x)		((unsigned short)(x))
#define	BOU32(x)		((unsigned int)(x))
#define	BOU64(x)		((unsigned long long)(x))
#define	BO16(x)			((short)(x))
#define	BO32(x)			((int)(x))
#define	BO64(x)			((long long)(x))
#else
#define	BOU16(x)		BYTE_SWAP_16BIT(x)
#define	BOU32(x)		BYTE_SWAP_32BIT(x)
#define	BOU64(x)		BYTE_SWAP_64BIT(x)
#define	BO16(x)			BYTE_SWAP_16BIT(x)
#define	BO32(x)			BYTE_SWAP_32BIT(x)
#define	BO64(x)			BYTE_SWAP_64BIT(x)
#endif

/******************************************************************************/
/* OAM Data types															  */
/******************************************************************************/
/**
 * @struct	_GATE
 * @brief	Redefinition of OAM Semaphore
 */
typedef struct _GATE
{
	OAM_SEMAPHORE*					pSem;				/**< OAM semaphore */

#ifdef TFS5_DEBUG
	int								bLocked;			/**< lock status for debug mode */
	unsigned int					dwOwnerId;			/**< Owner ID for debug mode */
#endif

} GATE, PGATE;
	
/**
 * @struct	_SEMAPHORE
 * @brief	Redefinition of OAM Semaphore.. it is defined as null semaphore in single thread system
 */
typedef struct _SEMAPHORE
{
	OAM_SEMAPHORE*					pSem;				/**< OAM semaphore */

#ifdef TFS5_DEBUG
	int								bLocked;			/**< lock status for debug mode */
	unsigned int					dwOwnerId;			/**< Owner ID for debug mode */
#endif

} SEMAPHORE, *PSEMAPHORE;

#define PFN_TASK_PROC		PFN_OAM_TASK_PROC
#define RTL_INVALID_TASK_ID	0xFFFFFFFF
#define RTL_DEFAULT_UID		0
#define RTL_DEFAULT_GID		0

/** 
 * @struct	_RW_SEMAPHORE
 * @brief	Redefinition of OAM Read / Write semaphore 
 */
typedef struct _RW_SEMAPHORE
{
	SEMAPHORE			bsRW;				/**< read-write semaphore handle */
	SEMAPHORE			bsPreemption;		/**< Preemption semaphore handle */
	unsigned int		dwReadCount;		/**< read count */

#ifdef TFS5_DEBUG
	unsigned int		dwWriteOwnerId;		/**< (TFS5_DEBUG purpose only) owner of the mtxLock if locked */
	int					bWriteLocked;		/**< (TFS5_DEBUG purpose only) lock status of mtxLock */
#endif

} RW_SEMAPHORE, *PRW_SEMAPHORE;


#if defined(__cplusplus)
}
#endif

#endif /* __NS_OAM_TYPES_H__ */

// end of file
