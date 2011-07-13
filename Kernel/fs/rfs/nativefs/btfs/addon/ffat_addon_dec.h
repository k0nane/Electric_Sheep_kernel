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
 * @file		ffat_addon_dec.h
 * @brief		Directory Entry Cache Module for FFAT
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		SEP-26-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFAT_ADDON_DEC_H_
#define _FFAT_ADDON_DEC_H_

// includes
#include "ess_types.h"
#include "ess_dlist.h"

#include "ffat_types.h"
#include "ffat_node.h"
#include "ffat_vol.h"

#include "ffatfs_types.h"

// defines

// typedefs

//!< cache info of directory entries for a child node
typedef struct _DECEntry
{
	t_wchar		wLfnFirstChar;		//!< first character of name part of long filename
									//!< stored with upper character
									//!< _MASK_FIRST_NAME_CHAR : empty entry
	t_wchar		wLfnLastChar;		//!< last character of name part of long filename
									//!< stored with upper character
	t_wchar		wLfn2ndLastChar;	//!< second last character of name part of long filename
									//!< stored with upper character
									//!< store '0' in case that the length of name part is '1'
	t_uint8		bSfnFirstChar;		//!< first character of name part of short filename
									//!< stored with upper character
									//!< 0xFF	: volume label with wSfnLastChar
	t_uint8		bSfnLastChar;		//!< last character of name part of short filename
									//!< stored with upper character
									//!< 0xFF	: volume label
	t_uint8		bSfn2ndLastChar;	//!< second last character of name part of short filename
									//!< stored with upper character
									//!< store '0' in case that the length of name part is '1'
									//!< 0xFF	: volume label
	t_uint8		bFirstExtention;	//!< first character of extension part
									//!< stored with upper character
	t_uint16	wEntryIndex;		//!< entry index offset (index of directory entry, not byte offset)
	t_uint32	dwNameLength2NT;	//!< name length (character count) & name type & numeric tail
									//!< name length (8bits)
									//!< name Type (1bit) - LFN or SFN
									//!< numeric tail (23bits)
} DECEntry;


//!< cache info for a parent node. this has several DECInfo structures.
typedef struct _DECNode
{
// debug begin
#ifdef FFAT_DEBUG
	t_int32			dwSig;
#endif
// debug end
	t_uint32		dwCluster;			//!< start cluster of node
	t_int16			wValidEntryCount;	//!< Valid Entry Count
										//!< -1 : free DECNode
	t_uint16		wLastEntryIndex;	//!< entry index of last DECEntry

	DECEntry*		pLastDECEntry;		//!< pointer about last DECEntry

	EssDList		stListDECNode;		//!< if global dec, linked to stHeadDECNode of GlobalDEC
										//!< if user dec, linked to stList of DECVolInfo
	EssDList		stHeadDECInfo;		//!< Head of DECInfo List

	Vol*			pVol;
} DECNode;

// information of User Directory Entry Cache
typedef struct _UserDEC
{
	t_int8*			pBuff;					//!< user assigned buffer pointer
	t_int32			dwSize;					//!< size of buffer
	t_int32			dwMaxEntryCnt;			//!< ???
	DECNode*		pDECNode;				//!< ???
} UserDEC;


// constant definitions


// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_dec_init(void);
	extern FFatErr	ffat_dec_terminate(void);

	extern FFatErr	ffat_dec_mount(Vol* pVol);
	extern FFatErr	ffat_dec_umount(Vol* pVol);

	extern FFatErr	ffat_dec_initNode(Node* pNode);

	extern FFatErr	ffat_dec_setUDEC(Node* pNode, FFatDirEntryCacheInfo* pDECI);
	extern FFatErr	ffat_dec_releaseUDEC(Node* pNode);
	extern FFatErr	ffat_dec_getUDECInfo(Node* pNode, FFatDirEntryCacheInfo* pDECI);

	extern FFatErr	ffat_dec_deallocateGDEC(Node* pNode);

	extern FFatErr	ffat_dec_insertEntry(Node* pNodeParent, Node* pNodeChild, t_wchar* psName);
	extern FFatErr	ffat_dec_removeEntry(Node* pNodeParent, Node* pNodeChild);

	extern FFatErr	ffat_dec_lookup(Node* pNodeParent, Node* pNodeChild, t_wchar* psName,
							t_int32 dwLen,  FFatLookupFlag dwFlag, FatGetNodeDe* pNodeDE,
							NodeNumericTail* pNumericTail, ComCxt* pCxt);

#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFAT_ADDON_DEC_H_ */
