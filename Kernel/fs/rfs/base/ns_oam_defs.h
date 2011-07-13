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
* @brief	Declarations for OS Abstraction Layer
* @author	ByungJune Song (byungjune.song@samsung.com)
* @author	InHwan Choi (inhwan.choi@samsung.com)
* @file		ns_oam_defs.h
*/

#ifndef __NS_OAM_DEFS_H__
#define __NS_OAM_DEFS_H__

#include "glue_oam.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef TRUE
#define TRUE	(1UL)
#endif

#ifndef FALSE
#define FALSE	(0UL)
#endif
	
#define OAM_INVALID_TASK_ID		0xFFFFFFFF
#define OAM_DEFAULT_UID			0
#define OAM_DEFAULT_GID			0

typedef struct _OAM_TIME						// time 정보 표현
{
	unsigned short		wYear;
	unsigned short		wMonth;
	unsigned short		wDayOfWeek;
	unsigned short		wDay;
	unsigned short		wHour;
	unsigned short		wMinute;
	unsigned short		wSecond;
	unsigned short		wMilliseconds;
} OAM_TIME, *POAM_TIME;


/******************************************************************************/
/* TASK MANAGEMENT                                                            */
/******************************************************************************/

/** @brief OamGetCurrentTaskId function returns the Task Id */
unsigned int
OamGetCurrentTaskId(void);


/******************************************************************************/
/* TIME                                                                */
/******************************************************************************/
/** @brief OamGetSystemTime function returns the system time */
void
OamGetSystemTime(
	OUT POAM_TIME pTime);

unsigned int
OamGetMilliSec(void);

#ifndef CONFIG_ARIES_VER_B2
int
OamGetOffsetFromUtc(void);
#endif

/******************************************************************************/
/* USER ID                                                                    */
/******************************************************************************/
/** @brief OamGetUid function returns the user id in linux */
unsigned int
OamGetUid(void);

/** @brief OamGetGid function returns the group id in linux */
unsigned int
OamGetGid(void);


/******************************************************************************/
/* TASK SYNCHRONIZATION                                                       */
/******************************************************************************/

#define	OAM_WAIT_EVENT_INFINITELY	0xFFFFFFFFUL
#define	OAM_EVENT_DELETED			0x00000000UL
#define OAM_EVENT_SIGNALED			0x00000001UL
#define	OAM_EVENT_TIMEOUT			0x00000002UL

typedef unsigned int	LOCK_STATUS;

/** @brief OamCreateSemaphore function creates and returns a semaphore object. */
unsigned int
OamCreateSemaphore(
	IN OAM_SEMAPHORE*	pSemaphore,
	IN unsigned int		dwCount);


/** @brief OamDestroySemaphore function destroys the semaphore and frees it from the
 * system. */
void
OamDestroySemaphore(
	IN OAM_SEMAPHORE* pSemaphore);


/** @brief OamGetSemaphore function gets an exclusive rights to the semaphore. */
int
OamGetSemaphore(
	IN OAM_SEMAPHORE* pSemaphore);


/** @brief OamTryGetSemaphore function tries to get semaphore and returns immediately
 * regardless of whether or not the request can be satisfied. */
unsigned int
OamTryGetSemaphore(
	IN OAM_SEMAPHORE* pSemaphore);


/** @brief OamPutSemaphore function releases the semaphore. */
int
OamPutSemaphore(
	IN OAM_SEMAPHORE* pSemaphore);

/******************************************************************************/
/* MEMORY MANAGEMENT			                                              */
/******************************************************************************/


unsigned int	
OamInitMemory(void);

unsigned int
OamTerminateMemory(void);

/** @brief OamCopyMemory function copies specified amount of data from pSrc to
 * pDest. */
void
OamCopyMemory(
	IN void*			pDest,
	IN const void*		pSrc,
	IN unsigned int		dwBytes);


/** @brief OamFillMemory function fills memory spaces with a specified value. */
void
OamFillMemory(
	IN void*			pDest,
	IN unsigned char	value,
	IN unsigned int		dwBytes);


/** @brief OamCompareMemory function compare memory spaces with a specified value. */
int
OamCompareMemory(
	IN const void*		pBuf1,
	IN const void*		pBuf2,
	IN unsigned int		dwBytes);


#define OamAllocateMem(x)		OamAllocateMemory(x)
#define OamFreeMem(x)			OamFreeMemory(x)

/** @brief OamAllocateMemory function allocates specified amount of memory from
* the heap and returns the start address of it. */
#if defined(CONFIG_SYMBIAN)
	void*
	OamAllocateMemory(IN unsigned int dwSize, unsigned int dwMallocFlag);

	#undef OamAllocateMem 
	#define OamAllocateMem(x)   OamAllocateMemory(x, 0)
	#define OamAllocateMemory  EssOsal_AllocMem 
#else
	void*
	OamAllocateMemory(IN unsigned int dwSize);
#endif /*defined(CONFIG_SYMBIAN)*/


/** @brief OamFreeMemory function releases memory space allocated by
* OamAllocateMemory function */
void
OamFreeMemory(IN void* pMemory);

#ifdef TFS5_DEBUG
	
	#undef OamAllocateMem
	#undef OamFreeMem

	#define OamAllocateMem(x)		OamDbgAllocateMemory(x, __FILE__, __LINE__)
 	#define OamFreeMem(x)			OamDbgFreeMemory(x)

	void*
	OamDbgAllocateMemory(
		IN unsigned int		dwSize,
		IN const char*		filename,
		IN unsigned int		lineno);

	void
	OamDbgFreeMemory(
		IN void* pMemory);

	void 
	OamDbgPrintfHeapSize(void);

#endif


/******************************************************************************/
/* MISCELLANEOUS RUNTIME LIBRARY                                              */
/******************************************************************************/

/** @brief OamAtomicIncrement function increments the value of the variable
 * passed as the argument and returns the its value incremented. */
unsigned int
OamAtomicIncrement(
	IN volatile unsigned int* pValue);


/** @brief OamAtomicDecrement function decrements the value of the variable
 * passed as the argument and returns the its value incremented.*/
unsigned int
OamAtomicDecrement(
	IN volatile unsigned int* pValue);


/******************************************************************************/
/* DEBUGGING ENVIRONMENT                                                      */
/******************************************************************************/

#if defined(CONFIG_RTOS) 
	int	OamPrintf(const char* pFmt, ...);
#elif defined(CONFIG_LINUX)
	int	OamPrintf(const char* pFmt, ...);
#elif defined(CONFIG_SYMBIAN)
	#define OamPrintf		EssOsal_Printf
#elif defined(CONFIG_WINCE)
	#define OamPrintf		NKDbgPrintfW
#endif

#if defined(CONFIG_RTOS) 
	#define OamVsnprintf	vsnprintf
#elif defined(CONFIG_LINUX)
	#define OamVsnprintf	vsnprintf
#elif defined(CONFIG_SYMBIAN)
	#define OamVsnprintf	vsnprintf
#elif defined(CONFIG_WINCE)
	#define OamVsnprintf	_vsnprintf
#endif

#if defined(CONFIG_RTOS)
	int	
	OamVprintf(const char* pFmt, va_list ap);
#endif

#define OamDbgBreak()

#ifdef TFS5_DEBUG

#undef OamDbgBreak

#if defined(CONFIG_LINUX)
	#define OamDbgBreak()		BUG()
#elif defined(CONFIG_RTOS) 
	#define OamDbgBreak()		DebugBreak()
#elif defined(CONFIG_WINCE) 
	#define OamDbgBreak()		DebugBreak()
#elif defined(CONFIG_SYMBIAN) 
	#define OamDbgBreak()		DebugBreak()
#endif

#endif /*TFS5_DEBUG*/

int				
OamGetChar(void);

int				
OamGetInt(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __NS_OAM_DEFS_H__

