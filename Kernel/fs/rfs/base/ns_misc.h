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
* @brief	Implementation of the run-time library routines for TFS5 and the natives.
* @author	ByungJune Song (byungjune.song@samsung.com)
* @author	InHwan Choi (inhwan.choi@samsung.com)
* @file		ns_misc.h
*/

#ifndef __NS_MISC_H__
#define __NS_MISC_H__

#include "ns_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/******************************************************************************/
/* Task Management															  */
/******************************************************************************/
/** @brief RtlGetCurrentTaskId function returns the Task Id */
unsigned int
RtlGetCurrentTaskId(void);

/** @brief RtlSleep function suspends the current thread until the specified
 * amount of time has elapsed. */
void
RtlSleep(
	IN unsigned int dwMilliseconds);

/** @brief RtlRelinquish function relinquishes remaining time slices of the
 * current thread to another. */
void
RtlRelinquish(void);



/******************************************************************************/
/* USER ID                                                                    */
/******************************************************************************/
/** @brief RtlGetUid function returns the user id in linux */
unsigned int
RtlGetUid(void);

/** @brief RtlGetGid function returns the group id in linux */
unsigned int
RtlGetGid(void);

/******************************************************************************/
/* TASK SYNCHRONIZATION                                                       */
/******************************************************************************/

/** @brief RtlCreateGate function creates and returns a Gate object. */
unsigned int
RtlCreateGate(
	IN GATE*			pGate,
	IN unsigned int		dwCount);

/** @brief RtlDestroyGate function destroys the Gate and frees it from the
 * system. */
void
RtlDestroyGate(
	IN GATE*	pGate);

/** @brief RtlEnterGate function gets an exclusive rights to the Gate. */
void
RtlEnterGate(
	IN GATE*	pGate);

/** @brief RtlExitGate function releases the Gate. */
void
RtlExitGate(
	IN GATE*	pGate);

/** @brief RtlCreateSemaphore function creates and returns a semaphore object. */
unsigned int
RtlCreateSemaphore(
	IN SEMAPHORE*	pSemaphore,
	IN unsigned int		dwCount);

/** @brief RtlDestroySemaphore function destroys the semaphore and frees it from the
 * system. */
void
RtlDestroySemaphore(
	IN SEMAPHORE* pSemaphore);

/** @brief RtlGetSemaphore function gets an exclusive rights to the semaphore. */
unsigned int
RtlGetSemaphore(
	IN SEMAPHORE* pSemaphore);

/** @brief RtlTryGetSemaphore function tries to get semaphore and returns immediately
 * regardless of whether or not the request can be satisfied. */
unsigned int
RtlTryGetSemaphore(
	IN SEMAPHORE* pSemaphore);

/** @brief RtlPutSemaphore function releases the semaphore. */
unsigned int
RtlPutSemaphore(
	IN SEMAPHORE* pSemaphore);

#ifdef TFS5_DEBUG
BOOL
RtlOwnSemaphore(
	IN SEMAPHORE* pSemaphore);

BOOL
RtlIsLocked(
	IN SEMAPHORE* pSemaphore);

BOOL
RtlOwnWriteSemaphore(
	IN RW_SEMAPHORE* pRwSemaphore);

BOOL
RtlIsWriteLocked(
	IN RW_SEMAPHORE* pRwSemaphore);
#endif /*TFS5_DEBUG*/

/** @brief RtlCreateRwSemaphore function creates and returns a semaphore object. */
unsigned int
RtlCreateRwSemaphore(
	IN RW_SEMAPHORE*	pRwSemaphore);

/** @brief RtlDestroyRwSemaphore function destroys the semaphore and frees it from the
 * system. */
void
RtlDestroyRwSemaphore(
	IN RW_SEMAPHORE* pRwSemaphore);

/** @brief RtlGetReadSemaphore function gets an exclusive rights to the semaphore. */
void
RtlGetReadSemaphore(
	IN RW_SEMAPHORE* pRwSemaphore);

/** @brief RtlPutReadSemaphore function releases the semaphore. */
void
RtlPutReadSemaphore(
	IN RW_SEMAPHORE* pRwSemaphore);

/** @brief RtlGetWriteSemaphore function gets an exclusive rights to the semaphore. */
void
RtlGetWriteSemaphore(
	IN RW_SEMAPHORE* pRwSemaphore);

/** @brief RtlTryGetWriteSemaphore function tries to get write semaphore and returns immediately
 * regardless of whether or not the request can be satisfied. */
unsigned int
RtlTryGetWriteSemaphore(
	IN RW_SEMAPHORE* pRwSemaphore);

/** @brief RtlPutWriteSemaphore function releases the semaphore. */
void
RtlPutWriteSemaphore(
	IN RW_SEMAPHORE* pRwSemaphore);


/** @brief RtlAtomicIncrement function increments the value of the variable
 * passed as the argument and returns the its value incremented. */
unsigned int
RtlAtomicIncrement(
	IN volatile unsigned int* pValue);

/** @brief RtlAtomicDecrement function decrements the value of the variable
 * passed as the argument and returns the its value incremented.*/
unsigned int
RtlAtomicDecrement(
	IN volatile unsigned int* pValue);


/******************************************************************************/
/* Memory Management														  */
/******************************************************************************/
/** @brief		Allocate memory blocks 
*	@param[in]	dwSize		Number of bytes to allocate
*	@return		The pointer of memory to allocate
*/
INLINE
void* 
RtlAllocMem(
	IN	unsigned int dwSize)
{
	return OamAllocateMem(dwSize);
}


/** @brief		Deallocates or frees a memory block. 
*	@param[in]	pMemory		Previously allocated memory block to be freed.
*	@return		void
*/
INLINE
void
RtlFreeMem(
	IN void* pMemory)
{
	OamFreeMem(pMemory);
}


/** @brief		Allocate memory blocks appropriately
*	@param[in]	dwMaxSize	max size to allocate
*	@param[in]	dwMinSize	min size to allocate
*	@param[out]	pdwSize		allocated size
*	@return		The pointer of memory to allocate
*/
INLINE
void* 
RtlAllocPossibleMemory(
	IN unsigned int		dwMaxSize, 
	IN unsigned int		dwMinSize, 
	OUT unsigned int*	pdwSize)
{
	void*	pBuffer = NULL;

	*pdwSize = dwMaxSize;
	do 
	{
		pBuffer = RtlAllocMem(*pdwSize);
		if (pBuffer)
		{
			break;
		}
		*pdwSize >>= 1;
	} while (*pdwSize >= dwMinSize);

	return pBuffer;
}

/** @brief		Copies bytes between buffers */
void
RtlCopyMem(
	OUT	void*			pDest,
	IN	const void*		pSrc,
	IN	unsigned int	dwBytes);

/** @brief		Sets buffers to a specified character. */
void
RtlFillMem(
	OUT	void*			pDest,
	IN	unsigned char	value,
	IN	unsigned int	dwBytes);

/** @brief		Compare characters in two buffers.*/
int
RtlCmpMem(
	IN const void*		pBuf1,
	IN const void*		pBuf2,
	IN unsigned int		dwBytes);

/** @brief		Initialize the memory */
unsigned int
RtlInitMemory(void);

/** @brief		Terminate the memory */
unsigned int
RtlTerminateMemory(void);
 

/******************************************************************************/
/* Time																		  */
/******************************************************************************/
/** @brief		RtlGetCurSystim function retrieves the current local date and time. */
void
RtlGetCurSysTime(
	OUT	PSYS_TIME pTime);

/** @brief		RtlGetCurSystim function convert SYS_TIME type to COMP_TIME type */
void
RtlSysTimeToCompTime(
	IN	PSYS_TIME			pSysTime,
	OUT	PCOMP_TIMESPEC		pCompTime);

/** @brief		RtlGetCurSystim function convert COMP_TIME type to SYS_TIME type */
void
RtlCompTimeToSysTime(
	IN	PCOMP_TIMESPEC		pCompTime,
	OUT	PSYS_TIME			pSysTime);


unsigned int
RtlGetMilliSec(void);

#ifndef CONFIG_ARIES_VER_B2
int
RtlGetOffsetFromUtc(void);
#endif

/******************************************************************************/
/* for misc.c    															  */
/******************************************************************************/

unsigned int
RtlCalculateCrc(
	IN const wchar_t*	pwsName,
	IN unsigned int		dwNameLen);

int
RtlIsPow2(
	IN unsigned int		dwNum);

unsigned int
RtlPow2(
	IN unsigned int		dwNum);

unsigned int
RtlLog2(
	IN unsigned int		dwNum);

wchar_t
RtlGetUpperWideChar(
	IN	const wchar_t	wc);

wchar_t
RtlGetLowerWideChar(
	IN const wchar_t	wc);

extern unsigned int
RtlWcsLen(
	IN	const wchar_t*	wcs);

extern unsigned int
RtlCompareString(
	IN const wchar_t*	pwszName1,
	IN const wchar_t*	pwszName2);

extern unsigned int
RtlCompareStringCaseInsensitive(
	IN	const wchar_t*	pwszName1,
	IN	const wchar_t*	pwszName2,
	IN	unsigned int	cchCompare);

extern void
RtlConvertUpperString(
	OUT	wchar_t*		pwszTarget,
	IN	const wchar_t*	pwszSource);

void
RtlConvertLowerString(
	OUT	wchar_t*		pwszTarget,
	IN	const wchar_t*	pwszSource);

int
RtlGetLastNumeric(
	IN	const wchar_t*	pwszSource,
	IN	unsigned int	cchSource,
	OUT	unsigned int*	pdwCount);

unsigned int 
RtlMbsToWcs(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	const char*				pMultiByteStr,
	IN	unsigned int			cbMultiByte,
	OUT	wchar_t*				pWideCharStr,
	IN	unsigned int			cchWideChar);

extern int	
RtlMbToWc(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	const char*				pMultiByteChar,
	IN	unsigned int			cbMultiByte,
	OUT	wchar_t*				pWideCharChar);

unsigned int 
RtlWcsToMbs(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	const wchar_t*			pWideCharStr, 
	IN	unsigned int			cchWideChar,
	OUT	char*					pMultiByteStr,
	IN	unsigned int			cbMultiByte);

int	
RtlWcToMb(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	wchar_t					pWideCharChar,
	OUT	char*					pMultiByteChar,
	IN	unsigned int			cbMultiByte);

extern char*
RtlStrCpy(
	OUT	char*		psDest,
	IN	const char*	psSrc);

extern char*
RtlStrNCpy(
	OUT	char*		psDest,
	IN	const char*	psSrc,
	IN	int				nNumber);

extern int
RtlStrCmp(
	IN	const char*	pS1,
	IN	const char*	pS2);

extern int
RtlStrICmp(
	IN	const char*	pS1,
	IN	const char*	pS2);

extern int
RtlStrNCmp(
	IN	const char*	pS1,
	IN	const char*	pS2,
	IN	int			nNumber);

extern int
RtlStrLen(
	IN	const char*	pS);

extern char*
RtlStrCat(
	OUT	char*		psDest,
	IN	const char*	psSrc);

extern int
RtlToLower(
	IN	int	c);

extern int
RtlToUpper(
	IN	int	c);

extern int
RtlIsLeadByte(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	int	c);

extern int
RtlStrCch(
	IN	const char*		pS);

extern wchar_t*
RtlWcsCpy(
	OUT	wchar_t*		pwsDst,
	IN	const wchar_t*	pwsSrc);

extern wchar_t*
RtlWcsNCpy(
	OUT	wchar_t*		pwsDst,
	IN	const wchar_t*	pwsSrc,
	IN	unsigned int	cch);

extern int
RtlWcsCmp(
	IN	const wchar_t*	wcs1,
	IN	const wchar_t*	wcs2);

int
RtlWcsNCmp(
	IN	const wchar_t*	wcs1,
	IN	const wchar_t*	wcs2,
	IN	unsigned int	cch);

extern int
RtlWcsICmp(
	IN	const wchar_t*	wcs1,
	IN	const wchar_t*	wcs2);

wchar_t*
RtlWcsChar(
	IN const wchar_t*	wcs,
	IN const wchar_t	wc);

wchar_t*
RtlWcsrChar(
	IN const wchar_t*	wcs, 
	IN wchar_t			wc);

wchar_t *
RtlWcsCat(
	OUT wchar_t*		wcs1, 
	IN const wchar_t*	wcs2);

int
RtlMathRand(void);

void
RtlBinCpy(
	IN	void*			pDst, 
	IN	const void*		pSrc, 
	IN	unsigned int	dwLen);


/******************************************************************************/
/* string																	  */
/******************************************************************************/

#define RtlVsnprintf	OamVsnprintf


/******************************************************************************/
/* Debugging																  */
/******************************************************************************/

INLINE int NOOP_TRUE(void)
{
	return TRUE;
}

#if	defined(CONFIG_PRINT_FUNC_INOUT) && (CONCURRENT_LEVEL == NO_CONCURRENCY)

	#define STACK_VAR	int stack_address;

	#define ENTER_API										\
	do														\
	{														\
		RtlEnterFunction(&stack_address, __FUNCTION__);		\
	} while (0)

	#define ENTER_FUNCTION									\
	do														\
	{														\
		int stack_address;									\
		RtlEnterFunction(&stack_address, __FUNCTION__);		\
	} while (0)

	#define EXIT_FUNCTION		RtlExitFunction(__FUNCTION__)

	void	RtlEnterFunction (int* pdwAddress, const char*	pszFunctionName);
	void	RtlExitFunction (const char*	pszFunctionName);
	void	RtlInitFunctionInfo(void);
	void	RtlDestroyFunctionInfo(void);
	void	RtlShowCallStack(void);
	void	RtlShowFunctionList(void);
	void	RtlInit(void);
	void	RtlTerminate(void);
	void	RtlResetFunctionList(void);

	#ifdef CONFIG_PRINT_QUIET
		#define RtlPrintMsg
	#else
		#define RtlPrintMsg	OamPrintf
	#endif

#else	/*!CONFIG_PRINT_FUNC_INOUT*/
	#define STACK_VAR
	#define ENTER_API	
	#define ENTER_FUNCTION
	#define EXIT_FUNCTION
	#define RtlEnterFunction(x)
	#define RtlExitFunction(x)
	#define RtlInitFunctionInfo()
	#define RtlDestroyFunctionInfo()
	#define RtlShowCallStack()
	#define RtlShowFunctionList()
	#define RtlInit()
	#define RtlTerminate()
	#define RtlResetFunctionList()

#endif /*CONFIG_PRINT_FUNC_INOUT*/

#if defined(__cplusplus)
}
#endif

#endif /* __NS_MISC_H__ */

// end of file

