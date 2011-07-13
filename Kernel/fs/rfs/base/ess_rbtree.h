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
* @file			ess_rbtree.h
* @brief		The file defines for ESS_RBTree module
* @author		Zhang Qing(qing1.zhang@samsung.com)
* @version		JUL-26-2006 [Zhang Qing] First writing
* @see			None
*/

#ifndef _ESS_RBTREE_H_
#define _ESS_RBTREE_H_

// includes

// defines
#ifdef ESS_STRICT_CHECK
	#define ESS_RBT_STRICT_CHECK		// CHECK VARIABLES 
#endif

#ifdef ESS_DEBUG
	#define ESS_RBT_DEBUG				// enable debug
#endif

#define ESS_RBTREE_NODE_KEY(pRBNode)	((pRBNode)->dwRBNodeKey)
#define ESS_RBTREE_NODE_COLOR(pRBNode)	((pRBNode)->dwRBNodeColor)
#define ESS_RBTREE_NODE_LEFT(pRBNode)	((pRBNode)->pstRBNodeLeft)
#define ESS_RBTREE_NODE_RIGHT(pRBNode)	((pRBNode)->pstRBNodeRight)
#define ESS_RBTREE_NODE_PARENT(pRBNode) ((pRBNode)->pstRBNodeParent)

#define ESS_RBTREE_HEADER(pRBTree)		((EssRBNode*)(pRBTree))
#define ESS_RBTREE_ROOT(pRBTree)		((ESS_RBTREE_HEADER(pRBTree))->pstRBNodeParent)
#define ESS_RBTREE_LEFT_MOST(pRBTree)	((ESS_RBTREE_HEADER(pRBTree))->pstRBNodeLeft)
#define ESS_RBTREE_RIGHT_MOST(pRBTree)	((ESS_RBTREE_HEADER(pRBTree))->pstRBNodeRight)

// typedefs
typedef struct _EssRBNode
{
	t_uint32			dwRBNodeKey;		//!< key of node
	t_uint32			dwRBNodeColor;		//!< 0: red, 1: black
	struct _EssRBNode*	pstRBNodeLeft;		//!< left node
	struct _EssRBNode*	pstRBNodeRight;		//!< right node
	struct _EssRBNode*	pstRBNodeParent;	//!< parent node
} EssRBNode;

/*
typedef struct _EssRBTree
{
	struct _EssRBNode*	pstRBHeader;	//!< Header is used to represent a rbtree
										  //!< Header->parent points to the root of rbtree 	
} EssRBTree;
*/

typedef EssRBNode EssRBTree;

typedef t_int32	(*PFN_RBNODE_CMP)(void* pTarget, EssRBNode* pCurrent);

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern t_int32		EssRBTree_Init(EssRBTree* pRBTree);
	extern t_int32		EssRBTree_Delete(EssRBTree* pRBTree, EssRBNode* pRBNode);
	extern t_int32		EssRBTree_Insert(EssRBTree* pRBTree, EssRBNode* pRBNode);
	extern EssRBNode*	EssRBTree_Lookup(EssRBTree* pRBTree, t_uint32 dwRBNodeKey);
	extern EssRBNode*	EssRBTree_Lookup_WithCmp(EssRBTree* pRBTree, void* pTarget, PFN_RBNODE_CMP pfCmp);

#ifdef ESS_RBT_DEBUG
	extern t_int32 EssRBTree_Print(EssRBTree* pRBTree);
#endif

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _ESS_RBTREE_H_ */



