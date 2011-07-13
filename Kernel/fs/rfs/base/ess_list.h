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
 * @file		ess_list.h
 * @brief		The file defines circular single liked list
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-28-2006 [DongYoung Seo] First writing
 * @see			None
 */


#ifndef _ESS_LIST_H_
#define _ESS_LIST_H_

// includes
#include "ess_types.h"

// defines


/* define and initialize list head */

/*
	ESS_LIST_GET_ENTRY - get the struct for this entry
	ptr		: EssList pointer
	type	: type of the member to access
	member	: member name
*/
#define ESS_LIST_GET_ENTRY(ptr, type, member) \
		((type *)((t_uint8 *)(ptr) - (t_uint8*)(&((type *)0)->member)))

/*
	ESS_LIST_FOR_EACH - iterate over a List
	pos		: current EssList pointer
	head	: head pointer
	note	: don't use it while removing a EssList node
*/
#define ESS_LIST_FOR_EACH(pos, head) \
		for (pos = (head)->pNext; pos != (head); \
				 pos = pos->pNext)


#define ESS_LIST_INIT(_p)					do { (_p)->pNext = NULL; } while (0)
#define ESS_LIST_ADD(_prev, _next, _new)	{ (_prev)->pNext = (_new); (_new)->pNext = (_next); }
#define ESS_LIST_ADD_HEAD(_head, _new)		{ EssList *_pTmp = (_head)->pNext; ESS_LIST_ADD((_head), _pTmp, (_new)); }
#define ESS_LIST_GET_HEAD(_head)			((_head)->pNext)
#define ESS_LIST_REMOVE_HEAD(_head)			((_head)->pNext = (_head)->pNext->pNext)

#define ESS_LIST_DEL(_prev, _next)			{(_prev)->pNext = (_next);}

#define ESS_LIST_IS_EMPTY(_head)			(((_head)->pNext == NULL) ? ESS_TRUE : ESS_FALSE)

#define ESS_LIST_GET_NEXT(p)				((p)->pNext)		// get next entry

// enum

// typedefs
typedef struct _EssList
{
	struct _EssList*	pNext;
} EssList;


// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern t_int32	EssList_Count(EssList* pHead);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _ESS_LIST_H_ */



