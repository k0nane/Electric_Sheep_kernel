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
 * @file		ess_dlist.h
 * @brief		The file defines double liked list
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _ESS_DLIST_H_
#define _ESS_DLIST_H_

// includes
#include "ess_types.h"

// defines
#ifdef ESS_DEBUG
	#define ESS_DLIST_DEBUG
#endif


/* define and initialize list head */

/*
	ESS_DLIST_GET_ENTRY - get the struct for this entry
	ptr : EssDList pointer
	type : type of the member to access
	member : member name
*/
#define ESS_DLIST_GET_ENTRY(ptr, type, member) \
	((type *)((t_uint8 *)(ptr) - (t_uint8*)(&((type *)0)->member)))

/*
	ESS_DLIST_FOR_EACH - iterate over a DList
	pos : current EssDList pointer
	head : head pointer
	note : don't use it while removing a EssDList node
*/
#define ESS_DLIST_FOR_EACH(pos, head) \
		 for (pos = (head)->pNext; pos != (head); \
				 pos = pos->pNext)

/*
	ESS_DLIST_FOR_EACH_PREV - iterate over a EssDList backwardly
	pos : current EssDList pointer
	head : EssDList pointer
	note : don't use it while removing a EssDList node
*/
#define ESS_DLIST_FOR_EACH_PREV(pos, head) \
		 for (pos = (head)->pPrev; pos != (head); \
				 pos = pos->pPrev)

/*
	ESS_DLIST_FOR_EACH_SAFE - iterate over a EssDList safe
	pos : current EssDList pointer
	n :   another EssDList pointer to use as temporary storage
	head : EssDList pointer
*/
#define ESS_DLIST_FOR_EACH_SAFE(pos, n, head) \
		 for (pos = (head)->pNext, n = pos->pNext; pos != (head); \
				 pos = n, n = pos->pNext)

/*
	ESS_DLIST_FOR_EACH_ENTRY - iterate over EssDList of given type
	pos : current EssDList pointer
	head : head pointer
	member : name of member
	type : type of member
*/
#define ESS_DLIST_FOR_EACH_ENTRY(type, pos, head, member) \
		 for (pos = ESS_GET_ENTRY((head)->pNext, type, member);  \
			  &pos->member != (head); \
			  pos = ESS_GET_ENTRY(pos->member.pNext, type, member))

/*
	ESS_DLIST_FOR_EACH_ENTRY_SAFE - iterate over EssDList safe
	pos : current EssDList pointer
	n :   another EssDList pointer to use as temporary storage
	head : EssDList head pointer
	member : name of member
	type : type of member
*/
#define ESS_DLIST_FOR_EACH_ENTRY_SAFE(type, pos, n, head, member) \
		 for (pos = ESS_GET_ENTRY((head)->pNext, type, member), \
			  n = ESS_GET_ENTRY(pos->member.pNext, type, member);  \
			  &pos->member != (head); \
			  pos = n, n = ESS_GET_ENTRY(pos->member.pNext, type, member))

/*
	LIST_FOR_EACH_ENTRY_PREV - iterate over EssDList of given type backwardly
	pos : current EssDList pointer
	head : EssDList head pointer
	member : name of member
	type : type of member
*/
#define ESS_DLIST_FOR_EACH_ENTRY_PREV(type, pos, head, member) \
		 for (pos = ESS_GET_ENTRY((head)->pPrev, type, member);  \
			  &pos->member != (head); \
			  pos = ESS_GET_ENTRY(pos->member.pPrev, type, member))


/*
	ESS_DLLIST_FOR_EACH_ENTRY_PREV_SAFE - iterate over EssDList safe backwardly
	pos : current EssDList pointer
	n :   another EssDList pointer to use as temporary storage
	head : EssDList head pointer
	member : name of member
	type : type of member
*/
#define ESS_DLLIST_FOR_EACH_ENTRY_PREV_SAFE(type, pos, n, head, member) \
		 for (pos = ESS_GET_ENTRY((head)->pPrev, type, member), \
			n = ESS_GET_ENTRY(pos->member.pPrev, type, member);  \
			&pos->member != (head); \
			pos = n, n = ESS_GET_ENTRY(pos->member.pPrev, type, member))

#ifndef ESS_DLIST_DEBUG
	#define ESS_DLIST_INIT(p)			do { (p)->pPrev = (p); (p)->pNext = (p); } while (0)
	// p : previous, n : next, w : new
	#define ESS_DLIST_ADD(p, n, w)		{(w)->pPrev = (p); (w)->pNext = (n); (n)->pPrev = (w); (p)->pNext = (w); }
	#define ESS_DLIST_ADD_HEAD(h, n)	{ EssDList *pTmp = (h)->pNext; ESS_DLIST_ADD((h), pTmp, (n)); }
	#define ESS_DLIST_ADD_TAIL(h, n)	{ EssDList *pTmp = (h)->pPrev; ESS_DLIST_ADD(pTmp, (h), (n)); }

	#define ESS_DLIST_DEL(p, n)			{ (p)->pNext = (n); (n)->pPrev = (p); }
	#define ESS_DLIST_DEL_INIT(p)		{ ESS_DLIST_DEL((p)->pPrev, (p)->pNext); ESS_DLIST_INIT((p)); }
	#define ESS_DLIST_MOVE_HEAD(h, n)	{ ESS_DLIST_DEL((n)->pPrev, (n)->pNext); ESS_DLIST_ADD_HEAD((h), (n)); }
	#define ESS_DLIST_MOVE_TAIL(h, n)	{ ESS_DLIST_DEL((n)->pPrev, (n)->pNext); ESS_DLIST_ADD_TAIL((h), (n)); }

	#define ESS_DLIST_IS_EMPTY(h)		((h)->pNext == (h) ? ESS_TRUE : ESS_FALSE)

	#define ESS_DLIST_GET_NEXT(p)		((p)->pNext)		// get next entry
	#define ESS_DLIST_GET_PREV(p)		((p)->pPrev)		// get previous entry
	#define ESS_DLIST_GET_TAIL(h)		((h)->pPrev)		// get tail entry
	//!< move a list to another list head
	//!< h_from : head of list from.
	//!< h_from : head of list to
	#define ESS_DLIST_MERGE_LIST(_pHeadFrom, _pHeadTo)	\
										do {	\
											(_pHeadFrom)->pNext->pPrev	= (_pHeadTo);	\
											(_pHeadFrom)->pPrev->pNext	= (_pHeadTo)->pNext;	\
											(_pHeadTo)->pNext->pPrev	= (_pHeadFrom)->pPrev;	\
											(_pHeadTo)->pNext			= (_pHeadFrom)->pNext;	\
											ESS_DLIST_INIT(h_from);	\
										} while (0)
#endif

// enum

// typedefs
typedef struct _EssDList
{
	struct _EssDList*	pPrev;	//!< previous node
	struct _EssDList*	pNext;	//!< next node
} EssDList;


// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern void		EssDList_Del(EssDList* pNode);
	extern void		EssDList_Inverse(EssDList* pHead);
	extern t_int32	EssDList_Count(EssDList* pHead);
	extern t_int32	EssDList_IsEmptyCareFul(EssDList* pHead);

#ifdef ESS_DLIST_DEBUG
	extern void			ESS_DLIST_INIT(EssDList* p);
	extern void			ESS_DLIST_ADD(EssDList* p, EssDList* n, EssDList* w);
	extern void			ESS_DLIST_ADD_HEAD(EssDList* h, EssDList* n);
	extern void			ESS_DLIST_ADD_TAIL(EssDList* h, EssDList* n);
	extern void			ESS_DLIST_DEL(EssDList* p, EssDList* n);
	extern void			ESS_DLIST_DEL_INIT(EssDList* p);
	extern void			ESS_DLIST_MOVE_HEAD(EssDList* h, EssDList* n);
	extern void			ESS_DLIST_MOVE_TAIL(EssDList* h, EssDList* n);
	extern t_boolean	ESS_DLIST_IS_EMPTY(EssDList* h);
	extern EssDList*	ESS_DLIST_GET_NEXT(EssDList* p);
	extern EssDList*	ESS_DLIST_GET_PREV(EssDList* p);
	extern EssDList*	ESS_DLIST_GET_TAIL(EssDList* h);
	extern void			ESS_DLIST_MERGE_LIST(EssDList* pHeadFrom, EssDList* pHeadTo);
#endif


#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _ESS_DLIST_H_ */



