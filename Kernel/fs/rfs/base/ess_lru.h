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
 * @file		ess_lru.h
 * @brief		The file defines for EssLru module
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-08-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _ESS_LRU_H_
#define _ESS_LRU_H_

// includes
#include "ess_dlist.h"

// defines
#ifdef ESS_STRICT_CHECK
	#define ESS_LRU_STRICT_CHECK		// CHECK VARIABLES 
#endif

#ifdef ESS_DEBUG
	#define ESS_LRU_DEBUG				// enable debug
#endif

// _MOVE TO LRU HEAD
//	_pLH	: LRU head
//	_pE		: LRU Entry
#define _ESS_LRU_ADD_TO_HEAD(_pLH, _pE)		ESS_DLIST_ADD_HEAD(&(_pLH)->stDListLru, &(_pE)->stDListLru)
#define _ESS_LRU_MOVE_TO_HEAD(_pLH, _pE)	ESS_DLIST_MOVE_HEAD(&(_pLH)->stDListLru, &(_pE)->stDListLru)
#define _ESS_LRU_REMOVE_INIT(_pE)			ESS_DLIST_DEL_INIT(&(_pE)->stDListLru)

#ifndef ESS_LRU_DEBUG
	#define ESS_LRU_REMOVE_INIT				_ESS_LRU_REMOVE_INIT
	#define ESS_LRU_MOVE_TO_HEAD			_ESS_LRU_MOVE_TO_HEAD
#else
	#define ESS_LRU_REMOVE_INIT				EssLru_RemoveInit
	#define ESS_LRU_MOVE_TO_HEAD			EssLru_MoveToHead
#endif

#define ESS_LRU_INIT						EssLru_Init
#define ESS_LRU_GET_HEAD(_pLru)				EssLru_GetHead(_pLru)
#define ESS_LRU_GET_TAIL(_pLru)				EssLru_GetTail(_pLru)
#define ESS_LRU_GET_PREV(_pLru, _pEntry)	EssLru_GetPrev(_pLru, _pEntry)
#define ESS_LRU_LOOKUP						EssLru_Lookup

#define ESS_LRU_INIT_ENTRY(_pEntry)			ESS_DLIST_INIT(&(_pEntry)->stDListLru)

// enum

// typedefs
typedef struct _EssLru
{
	EssDList	stDListLru;			//!< list for LRU
} EssLru;

typedef struct
{
	EssDList	stDListLru;			//!< for LRU
} EssLruEntry;

typedef t_boolean	(*PFN_LRU_CMP)(void* pTarget, EssLruEntry* pCurrent);

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef ESS_LRU_DEBUG
	extern void			EssLru_RemoveInit(EssLruEntry* pEntry);
	extern void			EssLru_MoveToHead(EssLru* pLru, EssLruEntry* pEntry);
#endif

	extern t_int32		EssLru_Init(EssLru* pLru);
	extern EssDList*	EssLru_Lookup(EssLru* pLru, void* pTarget, PFN_LRU_CMP pfCmp);
	extern EssDList*	EssLru_GetHead(EssLru* pLru);
	extern EssLruEntry*	EssLru_GetTail(EssLru* pLru);
	extern EssLruEntry*	EssLru_GetPrev(EssLru* pLru, EssLruEntry* pEntry);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _ESS_LRU_H_ */



