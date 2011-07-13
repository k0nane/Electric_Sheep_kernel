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
 * @file		ess_hash.h
 * @brief		The file defines hash algorithm
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-09-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _ESS_HASH_H_
#define _ESS_HASH_H_

// includes
#include "ess_base_config.h"
#include "ess_dlist.h"

// defines
#ifdef ESS_STRICT_CHECK
	#define ESS_HASH_STRICT_CHECK
#endif

#ifdef ESS_DEBUG
	#define ESS_LRU_DEBUG
#endif

	//!< add a node to hash table
	#define _ESS_HASH_ADD(_pHash, _dwHashVal, _pEntry)		\
					ESS_DLIST_ADD_HEAD(&(_pHash)->pHashTable[_dwHashVal], &(_pEntry)->stDListHash)
	//!< remove a node from hash table
	#define _ESS_HASH_REMOVE(_pEntry)	\
					ESS_DLIST_DEL((_pEntry)->stDListHash.pPrev, (_pEntry)->stDListHash.pNext)
	//!< move a node to head of table
	#define _ESS_HASH_MOVE_HEAD(_pHash, _dwHashVal, _pEntry)	\
					ESS_DLIST_MOVE_HEAD(&(_pHash)->pHashTable[_dwHashVal], &(_pEntry)->stDListHash)
	//!< add a node to free list
	#define _ESS_HASH_ADD_TO_FREE(_pHash, _pEntry)	\
					ESS_DLIST_ADD_HEAD(&(_pHash)->stDListFree, &(_pEntry)->stDListHash)
	//!< move a not to free list
	#define _ESS_HASH_MOVE_TO_FREE(_pHash, _pEntry)	\
						do {	\
							ESS_DLIST_DEL((_pEntry)->stDListHash.pPrev, (_pEntry)->stDListHash.pNext);	\
							ESS_DLIST_ADD_HEAD(&(_pHash)->stDListFree, &(_pEntry)->stDListHash);	\
						} while(0)

#ifndef ESS_LRU_DEBUG
	//!< add a node to hash table
	#define ESS_HASH_ADD			_ESS_HASH_ADD
	//!< remove a node from hash table
	#define ESS_HASH_REMOVE			_ESS_HASH_REMOVE
	//!< move a node to head of table
	#define ESS_HASH_MOVE_TO_HEAD	_ESS_HASH_MOVE_HEAD
	//!< add a node to free list
	#define ESS_HASH_ADD_TO_FREE	_ESS_HASH_ADD_TO_FREE
	//!< move a not to free list
	#define ESS_HASH_MOVE_TO_FREE	_ESS_HASH_MOVE_TO_FREE
#else
	#define ESS_HASH_ADD			EssHash_Add
	#define ESS_HASH_REMOVE			EssHash_Remove
	#define ESS_HASH_MOVE_TO_HEAD	EssHash_MoveToHead
	#define ESS_HASH_ADD_TO_FREE	EssHash_AddToFree
	#define ESS_HASH_MOVE_TO_FREE	EssHash_MoveToFree
#endif

#define ESS_HASH_INIT				EssHash_Init
#define ESS_HASH_LOOKUP				EssHash_Lookup
#define ESS_HASH_GET_FREE			EssHash_GetFree

#define ESS_HASH_INIT_ENTRY(_pEntry)	ESS_DLIST_INIT(&(_pEntry)->stDListHash)

// enum

// typedefs
typedef struct _EssHash 
{
	EssDList*	pHashTable;		//!< pointer for hash management
	t_int32		dwBucketMax;	//!< hash max value
	EssDList	stDListFree;	//!< list head for free node
} EssHash;

typedef struct 
{
	EssDList	stDListHash;			// hash entry
} EssHashEntry;

typedef t_boolean	(*PFN_HASH_CMP)(void* pTarget, EssHashEntry* pCurrent);

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

#ifndef ESS_HASH_DEBUG
	extern void			EssHash_Add(EssHash* pHash, t_int32 dwHashVal, EssHashEntry* pEntry);
	extern void			EssHash_Remove(EssHashEntry* pEntry);
	extern void			EssHash_MoveToHead(EssHash* pHash, t_int32 dwHashVal, EssHashEntry* pEntry);
	extern void			EssHash_AddToFree(EssHash* pHash, EssHashEntry* pEntry);
	extern void			EssHash_MoveToFree(EssHash* pHash, EssHashEntry* pEntry);
#endif

	extern t_int32			EssHash_Init(EssHash* pHash, EssDList* pHashTable, t_int32 dwBucketCount);
	extern EssHashEntry*	EssHash_Lookup(EssHash* pHash, t_int32 dwHashVal, void* pTarget,PFN_HASH_CMP pfCmp);
	extern EssHashEntry*	EssHash_GetFree(EssHash* pHash);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _ESS_HASH_H_ */



