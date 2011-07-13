/******************************************************************************/
/*                                                                            */
/*               COPYRIGHT 2007 SAMSUNG ELECTRONICS CO., LTD.                 */
/*                          ALL RIGHTS RESERVED                               */
/*                                                                            */
/* Permission is hereby granted to licensees of Samsung Electronics Co., Ltd. */
/* products to use or abstract this computer program only in  accordance with */
/* the terms of the NAND FLASH MEMORY SOFTWARE LICENSE AGREEMENT for the sole */
/* purpose of implementing  a product based  on Samsung Electronics Co., Ltd. */
/* products. No other rights to  reproduce, use, or disseminate this computer */
/* program, whether in part or in whole, are granted.                         */
/*                                                                            */
/* Samsung Electronics Co., Ltd. makes no  representation or warranties  with */
/* respect to  the performance  of  this computer  program,  and specifically */
/* disclaims any  responsibility for  any damages,  special or consequential, */
/* connected with the use of this program.                                    */
/*                                                                            */
/******************************************************************************/
/**
 * @brief	List management
 * @author	
 * @file	ess_nlist.h
 */
#ifndef __ESS_NLIST_H__
#define __ESS_NLIST_H__

#if	defined(_WIN32) && defined(_MSC_VER) 
// to suppress a warning message, "conditional expression is constant".
#pragma warning(disable: 4127)
#endif

/******************************************************************************/
/*  DLIST																	  */
/******************************************************************************/

#if 0

/**
 * @brief		define dlist
 */
#define DLIST(type)							struct { type* pHead; type*	 pTail; }

/**
 * @brief		define dlist element
 */
#define DLIST_ENTRY(type)					struct { type* pNext; type* pPrev;}

/**
 * @brief		check the list is empty
 */
#define DLIST_IS_EMPTY(pList)				((pList)->pHead == NULL)

/**
 * @brief		get the list head
 */
#define DLIST_GET_HEAD(pList)				((pList)->pHead)

/**
 * @brief		get the list tail
 */
#define DLIST_GET_TAIL(pList)				((pList)->pTail)

/**
 * @brief		get the next element
 */
#define DLIST_GET_NEXT(elem, field)			((elem)->field.pNext)

/**
 * @brief		get the previous element
 */
#define DLIST_GET_PREV(elem, field)			((elem)->field.pPrev)

/**
* @brief		NULL
*/
#define DLIST_NULL(x)						{ NULL, NULL }

/**
 * @brief		initialize list
 */
#define DLIST_INIT(pList)																		\
		do {																					\
			DLIST_GET_HEAD(pList) = NULL;														\
			DLIST_GET_TAIL(pList) = NULL;														\
		} while(0)

/**
* @brief		initialize list entry
*/
#define DLIST_ENTRY_NULL(elem, filed)															\
		do {																					\
			DLIST_GET_PREV(elem, filed) = NULL;													\
			DLIST_GET_NEXT(elem, filed) = NULL;													\
		} while(0)

/**
* @brief		check entry is null
*/
#define DLIST_IS_ENTRY_NULL(elem, field)														\
	((DLIST_GET_PREV(elem, field) == NULL) && (DLIST_GET_NEXT(elem, field) == NULL))			\

/**
 * @brief		for each element of the list.. just for search
 */
#define DLIST_FOREACH(pList, field, elem)														\
		for (elem = DLIST_GET_HEAD(pList);														\
			elem;																				\
			elem = DLIST_GET_NEXT(elem, field))
	
/**
 * @brief		for each element of the list in reverse order.. just for search
 */
#define DLIST_FOREACH_PREV(pList, field, elem)													\
		for (elem = DLIST_GET_TAIL(pList);														\
			elem;																				\
			elem = DLIST_GET_PREV(elem, field))
	
/**
 * @brief		for each element of the list in safe mode.. for search & remove
 */
#define DLIST_FOREACH_SAFE(pList, field, elem, temp)											\
		for (elem = DLIST_GET_HEAD(pList);														\
			elem && (((temp = DLIST_GET_NEXT(elem, field)) != NULL) || 1);						\
			elem = temp)

/**
 * @brief		for each element of the list in reverse mode in safe mode.. for search & remove
 */
#define DLIST_FOREACH_PREV_SAFE(pList, field, elem, temp)										\
		for (elem = DLIST_GET_TAIL(pList);														\
			elem && (((temp = DLIST_GET_PREV(elem, field)) != NULL) || 1);						\
			elem = temp)

/**
 * @brief		add the element to the head of the list
 */
#define DLIST_ADD_HEAD(pList, elem, field)														\
		do {																					\
			if (!DLIST_GET_HEAD(pList))															\
				{NSD_AS(!DLIST_GET_TAIL(pList)); DLIST_GET_TAIL(pList) = elem;}					\
			else 																				\
				{DLIST_GET_PREV(DLIST_GET_HEAD(pList), field) = elem;}							\
			DLIST_GET_NEXT(elem, field) = DLIST_GET_HEAD(pList);								\
			DLIST_GET_PREV(elem, field) = NULL;													\
			DLIST_GET_HEAD(pList) = elem;														\
		} while(0)


/**
 * @brief		add the element to the tail of the list
 */
#define DLIST_ADD_TAIL(pList, elem, field)														\
		do {																					\
			if (!DLIST_GET_TAIL(pList))															\
				{NSD_AS(!DLIST_GET_HEAD(pList)); DLIST_GET_HEAD(pList) = elem;}					\
			else																				\
				{DLIST_GET_NEXT(DLIST_GET_TAIL(pList), field) = elem;}							\
			DLIST_GET_NEXT(elem, field) = NULL;													\
			DLIST_GET_PREV(elem, field) = DLIST_GET_TAIL(pList);								\
			DLIST_GET_TAIL(pList) = elem;														\
		} while(0)

/**
 * @brief		insert the element after the given point
 */
#define DLIST_INSERT_AFTER(pList, elem, elem_new, field)										\
		do {																					\
			if ((DLIST_GET_NEXT(elem_new, field) = DLIST_GET_NEXT(elem, field)) != NULL)		\
				{DLIST_GET_PREV(DLIST_GET_NEXT(elem, field), field) = elem_new;}				\
			else																				\
				{DLIST_GET_TAIL(pList) = elem_new;}												\
			DLIST_GET_NEXT(elem, field) = elem_new;												\
			DLIST_GET_PREV(elem_new, field) = elem;												\
		} while(0)

/**
 * @brief		insert the element before the given point
 */
#define DLIST_INSERT_BEFORE(pList, elem, elem_new, field)										\
		do {																					\
			if ((DLIST_GET_PREV(elem_new, field) = DLIST_GET_PREV(elem, field)) != NULL)		\
				{DLIST_GET_NEXT(DLIST_GET_PREV(elem, field), field) = elem_new;}				\
			else																				\
				{DLIST_GET_HEAD(pList) = elem_new;}												\
			DLIST_GET_PREV(elem, field) = elem_new;												\
			DLIST_GET_NEXT(elem_new, field) = elem;												\
		} while(0)

/**
 * @brief		remove the element 
 */
#define DLIST_REMOVE(pList, elem, field)														\
		do {																					\
			if ((DLIST_GET_HEAD(pList) == elem))												\
			{	if ((DLIST_GET_TAIL(pList) == elem))											\
				{																				\
					/* head and tail */															\
					NSD_AS(DLIST_GET_PREV(elem, field) == NULL);								\
					NSD_AS(DLIST_GET_NEXT(elem, field) == NULL);								\
					DLIST_INIT((pList));														\
				}																				\
				else																			\
				{																				\
					/* head */																	\
					NSD_AS(DLIST_GET_PREV(elem, field) == NULL);								\
					NSD_AS(DLIST_GET_NEXT(elem, field) != NULL);								\
					DLIST_GET_HEAD(pList) = DLIST_GET_NEXT(elem, field);						\
					DLIST_GET_PREV(DLIST_GET_HEAD(pList), field) = NULL;						\
				}																				\
			}																					\
			else																				\
			{																					\
				if ((DLIST_GET_TAIL(pList) == elem))											\
				{																				\
					/* tail */																	\
					NSD_AS(DLIST_GET_PREV(elem, field) != NULL);								\
					NSD_AS(DLIST_GET_NEXT(elem, field) == NULL);								\
					DLIST_GET_TAIL(pList) = DLIST_GET_PREV(elem , field);						\
					DLIST_GET_NEXT(DLIST_GET_TAIL(pList), field) = NULL;						\
				}																				\
				else																			\
				{																				\
					/* mid */																	\
					NSD_AS(DLIST_GET_PREV(elem, field) != NULL);								\
					NSD_AS(DLIST_GET_NEXT(elem, field) != NULL);								\
					DLIST_GET_NEXT(DLIST_GET_PREV(elem, field), field) = DLIST_GET_NEXT(elem, field);	\
					DLIST_GET_PREV(DLIST_GET_NEXT(elem, field), field) = DLIST_GET_PREV(elem, field);	\
				}																				\
			}																					\
			DLIST_GET_PREV(elem, field) = NULL;													\
			DLIST_GET_NEXT(elem, field) = NULL;													\
		} while(0)

/**
* @brief		move the element to the head of the list
*/
#define DLIST_MOVE_HEAD(pList, elem, field)														\
	do {																						\
		DLIST_REMOVE(pList, elem, field);														\
		DLIST_ADD_HEAD(pList, elem, field);														\
	} while(0)

/**
* @brief		move the element to the tail of the list
*/
#define DLIST_MOVE_TAIL(pList, elem, field)														\
	do {																						\
		DLIST_REMOVE(pList, elem, field);														\
		DLIST_ADD_TAIL(pList, elem, field);														\
	} while(0)

#else /*!ESS_DEBUG*/

/**
* @brief		define dlist
*/
#define DLIST(type)							struct { type* pHead; type*	 pTail; }

/**
* @brief		define dlist element
*/
#define DLIST_ENTRY(type)					struct { type* pNext; type* pPrev;}

/**
* @brief		check the list is empty
*/
#define DLIST_IS_EMPTY(pList)				((pList)->pHead == NULL)

/**
* @brief		get the list head
*/
#define DLIST_GET_HEAD(pList)				((pList)->pHead)

/**
* @brief		get the list tail
*/
#define DLIST_GET_TAIL(pList)				((pList)->pTail)

/**
* @brief		get the next element
*/
#define DLIST_GET_NEXT(elem, field)			((elem)->field.pNext)

/**
* @brief		get the previous element
*/
#define DLIST_GET_PREV(elem, field)			((elem)->field.pPrev)

/**
* @brief		NULL
*/
#define DLIST_NULL(x)						{ NULL, NULL }

/**
* @brief		initialize list
*/
#define DLIST_INIT(pList)																		\
	do {																					\
		DLIST_GET_HEAD(pList) = NULL;														\
		DLIST_GET_TAIL(pList) = NULL;														\
	} while(0)

/**
* @brief		initialize list entry
*/
#define DLIST_ENTRY_NULL(elem, filed)															\
	do {																					\
		DLIST_GET_PREV(elem, filed) = NULL;													\
		DLIST_GET_NEXT(elem, filed) = NULL;													\
	} while(0)

/**
* @brief		check entry is null
*/
#define DLIST_IS_ENTRY_NULL(elem, field)														\
	((DLIST_GET_PREV(elem, field) == NULL) && (DLIST_GET_NEXT(elem, field) == NULL))			\

/**
* @brief		for each element of the list.. just for search
*/
#define DLIST_FOREACH(pList, field, elem)														\
	for (elem = DLIST_GET_HEAD(pList);														\
		elem;																				\
		elem = DLIST_GET_NEXT(elem, field))

/**
* @brief		for each element of the list in reverse order.. just for search
*/
#define DLIST_FOREACH_PREV(pList, field, elem)													\
	for (elem = DLIST_GET_TAIL(pList);														\
		elem;																				\
		elem = DLIST_GET_PREV(elem, field))

/**
* @brief		for each element of the list in safe mode.. for search & remove
*/
#define DLIST_FOREACH_SAFE(pList, field, elem, temp)											\
	for (elem = DLIST_GET_HEAD(pList);														\
		elem && (((temp = DLIST_GET_NEXT(elem, field)) != NULL) || 1);						\
		elem = temp)

/**
* @brief		for each element of the list in reverse mode in safe mode.. for search & remove
*/
#define DLIST_FOREACH_PREV_SAFE(pList, field, elem, temp)										\
	for (elem = DLIST_GET_TAIL(pList);														\
		elem && (((temp = DLIST_GET_PREV(elem, field)) != NULL) || 1);						\
		elem = temp)

/**
* @brief		add the element to the head of the list
*/
#define DLIST_ADD_HEAD(pList, elem, field)														\
	do {																					\
	if (!DLIST_GET_HEAD(pList))															\
				{DLIST_GET_TAIL(pList) = elem;}					\
			else 																				\
				{DLIST_GET_PREV(DLIST_GET_HEAD(pList), field) = elem;}							\
				DLIST_GET_NEXT(elem, field) = DLIST_GET_HEAD(pList);								\
				DLIST_GET_PREV(elem, field) = NULL;													\
				DLIST_GET_HEAD(pList) = elem;														\
	} while(0)


/**
* @brief		add the element to the tail of the list
*/
#define DLIST_ADD_TAIL(pList, elem, field)														\
	do {																					\
	if (!DLIST_GET_TAIL(pList))															\
				{DLIST_GET_HEAD(pList) = elem;}					\
			else																				\
				{DLIST_GET_NEXT(DLIST_GET_TAIL(pList), field) = elem;}							\
				DLIST_GET_NEXT(elem, field) = NULL;													\
				DLIST_GET_PREV(elem, field) = DLIST_GET_TAIL(pList);								\
				DLIST_GET_TAIL(pList) = elem;														\
	} while(0)

/**
* @brief		insert the element after the given point
*/
#define DLIST_INSERT_AFTER(pList, elem, elem_new, field)										\
	do {																					\
			if ((DLIST_GET_NEXT(elem_new, field) = DLIST_GET_NEXT(elem, field)) != NULL)		\
				{DLIST_GET_PREV(DLIST_GET_NEXT(elem, field), field) = elem_new;}				\
			else																				\
				{DLIST_GET_TAIL(pList) = elem_new;}												\
				DLIST_GET_NEXT(elem, field) = elem_new;												\
				DLIST_GET_PREV(elem_new, field) = elem;												\
	} while(0)

/**
* @brief		insert the element before the given point
*/
#define DLIST_INSERT_BEFORE(pList, elem, elem_new, field)										\
	do {																					\
			if ((DLIST_GET_PREV(elem_new, field) = DLIST_GET_PREV(elem, field)) != NULL)		\
				{DLIST_GET_NEXT(DLIST_GET_PREV(elem, field), field) = elem_new;}				\
			else																				\
				{DLIST_GET_HEAD(pList) = elem_new;}												\
				DLIST_GET_PREV(elem, field) = elem_new;												\
				DLIST_GET_NEXT(elem_new, field) = elem;												\
	} while(0)

/**
* @brief		remove the element 
*/
#define DLIST_REMOVE(pList, elem, field)														\
	do {																					\
			if ((DLIST_GET_HEAD(pList) == elem))												\
			{	if ((DLIST_GET_TAIL(pList) == elem))											\
				{																				\
				DLIST_INIT((pList));														\
				}																				\
				else																			\
				{																				\
				DLIST_GET_HEAD(pList) = DLIST_GET_NEXT(elem, field);						\
				DLIST_GET_PREV(DLIST_GET_HEAD(pList), field) = NULL;						\
				}																				\
			}																					\
			else																				\
			{																					\
			if ((DLIST_GET_TAIL(pList) == elem))											\
				{																				\
				DLIST_GET_TAIL(pList) = DLIST_GET_PREV(elem , field);						\
				DLIST_GET_NEXT(DLIST_GET_TAIL(pList), field) = NULL;						\
				}																				\
				else																			\
				{																				\
				DLIST_GET_NEXT(DLIST_GET_PREV(elem, field), field) = DLIST_GET_NEXT(elem, field);	\
				DLIST_GET_PREV(DLIST_GET_NEXT(elem, field), field) = DLIST_GET_PREV(elem, field);	\
				}																				\
			}																					\
			DLIST_GET_PREV(elem, field) = NULL;													\
			DLIST_GET_NEXT(elem, field) = NULL;													\
	} while(0)

/**
* @brief		move the element to the head of the list
*/
#define DLIST_MOVE_HEAD(pList, elem, field)														\
	do {																						\
		DLIST_REMOVE(pList, elem, field);														\
		DLIST_ADD_HEAD(pList, elem, field);														\
	} while(0)

/**
* @brief		move the element to the tail of the list
*/
#define DLIST_MOVE_TAIL(pList, elem, field)														\
	do {																						\
		DLIST_REMOVE(pList, elem, field);														\
		DLIST_ADD_TAIL(pList, elem, field);														\
	} while(0)

#endif /*ESS_DEBUG*/

#endif /* __ESS_NLIST_H__ */
