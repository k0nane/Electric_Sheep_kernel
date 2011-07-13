/*
 * TFS4 2.0 File System Developed by Flash Planning Group.
 *
 * Copyright 2006-2007 by Memory Division, Samsung Electronics, Inc.,
 * San #16, Banwol-Ri, Taean-Eup, Hwasung-City, Gyeonggi-Do, Korea
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
 * @file		ess_debug.h
 * @brief		Base modue for FFAT debug
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _ESS_DEBUG_H_
#define _ESS_DEBUG_H_

#include "ess_types.h"
#include "ess_dlist.h"

#ifdef ESS_DEBUG
	#define ESS_DEBUG_PRT
#else
	#undef ESS_DEBUG_PRT
#endif

#if defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_SYMBIAN)
#ifndef __FUNCTION__
	#if (__CC_ARM == 1)
		#define __FUNCTION__ __func__	// for ARM ADS1.2
	#else
		#define __FUNCTION__ "[]"
	#endif
#endif
#endif /*defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_SYMBIAN)*/

#ifdef ESS_DEBUG_PRT
	#define	ESS_ASSERTP(f, msg)		((f) ? ((void)0) : ESS_Assert( __FILE__, __LINE__, msg, __FUNCTION__))
	#define	ESS_ASSERT(f)			ESS_ASSERTP(f, NULL)
	#define ESS_LOG_PRINTF(msg)		ESS_PrintLog((const char*)msg, __FILE__, __LINE__, __FUNCTION__)
	#define ESS_LOG_PRINTFW(msg)	ESS_PrintLogWcs(msg, __FILE__, __LINE__)
	#define ESS_DEBUG_PRINTF		ESS_Printf
	#define ESS_SRC(msg)			ESS_Src(msg, __FILE__, __LINE__, __FUNCTION__)
// debug begin
#ifdef _RFS_TOOLS
	#undef	ESS_DEBUG_PRINTF
	#undef	ESS_LOG_PRINTF
	#define	ESS_DEBUG_PRINTF		printf
	#define	ESS_LOG_PRINTF(msg)		printf("[%s:%d]"msg"\n", __FILE__, __LINE__)
#endif
// debug end
#else
	#define ESS_ASSERTP(f, msg)		
	#define ESS_ASSERT(f)		

	#define ESS_LOG_PRINTF(msg)
	#define ESS_LOG_PRINTFW(msg)	
	#define ESS_SRC(msg)

	#define ESS_DEBUG_PRINTF		ESS_DebugNullPrintf
#endif

#if defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_LINUX)
#define ESS_ABORT()		do{ *(int *)0 = 0; }while(1)
#else
#define ESS_ABORT()
#endif /*defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_LINUX)*/

// for stack check
#ifdef ESS_STACK_CHECK
	#define ESS_STACK_VAR			t_uint32 dwStackVar;
	#define ESS_STACK_BEGIN()		ESS_StackStart(&dwStackVar, __FUNCTION__)
	#define ESS_STACK_END()			ESS_StackEnd(&dwStackVar, __FUNCTION__)

	#ifdef __cplusplus
		extern "C" {
	#endif ////	__cplusplus
		extern void	ESS_StackShow(void);
		extern void	ESS_StackStart(void *pdwStartAddress, const char *pszMsg);
		extern void	ESS_StackEnd(void *pdwEndAddress, const char *pszMsg);
	#ifdef __cplusplus
		};
	#endif ////	__cplusplus
#else
	#define ESS_STACK_VAR
	#define ESS_STACK_BEGIN()
	#define ESS_STACK_END()
#endif

// s : structure name
// t : type, alignment byte
// m : member variable name
// msg : print message
#define ESS_DEBUG_BA_ASSERT(s, t, m, msg)	\
				EssAssert((( (t_uint32)(&((s*)0)->m)	&(sizeof(t) - 1)) == 0), "Invalid byte alignent" msg);

typedef struct _EssMemUsageEntry
{
	void*		pPtr;		// allocated memory pointer
	t_int32		dwSize;		// size of memory
	EssDList	stList;		// list for management
} EssMemUsageEntry;

typedef struct _EssMemUsage
{
	EssDList			stAllocList;	// allocated list
	EssDList			stFreeList;		// free entry list
	EssMemUsageEntry	*pAlloc;
	t_int32				dwMaxAllocSize;	// maximum allocation size
	t_int32				dwMaxUsagesage;	// maximum memory usage
} EssMemUsage;

#ifdef __cplusplus
	extern "C" {
#endif

	extern void				ESS_DebugInit(t_int32 (*_printf)(const char* pFmt,...), t_int32 (*_get_char)(void));
	extern void				ESS_Assert(const char* psFileName, t_int32 dwLine,
										char* pszMsg, const char* strFuncName);
	extern void				ESS_DumpBuffer(t_int8* pBuff, t_int32 dwLength);
	extern char*			ESS_PathStart(const char* psFileName);

	extern void				ESS_PrintLog(const char* msg, const char* strFileName,
										t_int32 nLineNumber, const char* strFuncName);
	extern const t_int8*	ESS_Src(const char* msg, const char* psFileName,
										t_int32 nLineNumber, const char* strFuncName);
	extern void				ESS_DumpSector(t_int8* buf);

	extern t_int32			ESS_Printf(char* pFmt, ...);

	extern t_int32			ESS_DebugNullPrintf(char* pFmt,...);
	extern t_int32			ESS_DebugNullGetChar(void);

#ifdef __cplusplus
	};
#endif

#endif /* #ifndef _ESS_DEBUG_H_ */

