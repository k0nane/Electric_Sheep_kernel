/*
 * BTFS(Beyond The FAT fileSystem) Developed by Flash Software Group.
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
* @file			ffat_addon_fastseek.h
* @brief		definitions for fast seek module
* @author		DongYoung Seo(dy76.seo@samsung.com)
* @version		JUL-04-2006 [DongYoung Seo] First writing
* @version		FEB-02-2009 [DongYoung Seo] Remove code for user fast seek
* @version		JAN-14-2010 [ChunUm Kong]	Modifying comment (English/Korean)
* @see			None
*/



#ifndef _FFAT_ADDON_FASTSEEK_H_
#define _FFAT_ADDON_FASTSEEK_H_

// includes

// enum

// typedefs

// global fast seek head 
typedef struct
{
	EssDList	stDListNode;		// head for global fast seek
} GFSNodeHead;


// constant definitions

// external variable declarations

// function declarations

#ifdef __cplusplus
extern "C" {
#endif

	extern FFatErr		ffat_fastseek_init(t_boolean bForce);
	extern FFatErr		ffat_fastseek_terminate(void);

	extern FFatErr		ffat_fastseek_initNode(struct _Node* pNode);
	extern FFatErr		ffat_fastseek_terminateNode(Node* pNode);
	extern FFatErr		ffat_fastseek_rename(Node* pNodeSrc, Node* pNodeDes, Node* pNodeNewDes);
	extern FFatErr		ffat_fastseek_afterRename(Node* pNodeSrc, Node* pNodeDes,
									Node* pNodeNewDes);
	extern FFatErr		ffat_fastseek_deallocateGFSE(Node* pNode);

	extern FFatErr		ffat_fastseek_getClusterOfOffset(Node* pNode, t_uint32 dwOffset,
									t_uint32* pdwCluster, t_uint32* pdwOffset,
									NodePAL* pPAL, ComCxt* pCxt);
	extern FFatErr		ffat_fastseek_getVectoredCluster(Node* pNode, t_uint32 dwCount,
									FFatVC* pVC, t_uint32 dwOffset, t_uint32* pdwNewCluster,
									t_uint32* pdwNewCount, NodePAL* pPAL, ComCxt* pCxt);

	extern FFatErr		ffat_fastseek_resetFrom(Node* pNode, t_uint32 dwOffset);

	// GFS functions
	extern FFatErr		ffat_fastseek_umount(Vol* pVol);

#ifdef __cplusplus
};
#endif


#endif	/* #ifndef _FFAT_ADDON_FASTSEEK_H_ */



