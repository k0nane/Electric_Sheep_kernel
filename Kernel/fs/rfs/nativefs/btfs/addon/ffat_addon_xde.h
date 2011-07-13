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
 * @file		ffat_addon_extended_de.h
 * @brief		Extended Directory Entry
 * @author		GwangOk Go
 * @version		MAY-30-2008 [GwangOk Go] First writing
 * @see			None
 */

#ifndef _FFAT_ADDON_XDE_H_
#define _FFAT_ADDON_XDE_H_

// includes
#include "ffat_node.h"
#include "ffat_vol.h"

// defines

#define FFAT_XDE_SIGNATURE_STR			".btfs?xde*"	// signature는 string(10) + check sum(1)으로 구성됨
// 2010.05.26_chunum.kong_Bug Fix to compare with character "?" about xde. It occured the mount fail, 
#define FFAT_XDE_SIGNATURE_STR2         "btfs?xde*"

#define FFAT_XDE_SIGNATURE_STR_SIZE		10
#define FFAT_XDE_SIGNATURE_SIZE			(FFAT_XDE_SIGNATURE_STR_SIZE + 1)

// enum

// typedefs
typedef FFatExtendedDirEntryInfo		XDEInfo;

//!< Extended Directory entry
typedef struct _ExtednedDe
{
	t_uint8		szSignature[FFAT_XDE_SIGNATURE_STR_SIZE];
	t_uint8		bCheckSum1;		// SFNDE의 마지막 문자 자리
	t_uint8		bAttr;
	t_uint8		bType;
	t_uint8		bCheckSum2;		// LFNDE의 check sum 자리
	t_uint16	wPerm;			// Permission
	t_uint32	dwUID;			// User ID
	t_uint32	dwGID;			// Group ID
	t_uint32	dwReserved1;	// DE의 size를 맞추기 위함
	t_uint32	dwReserved2;	// DE의 size를 맞추기 위함
} ExtendedDe;

// constant definitions

// external variable declarations

// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_xde_mount(Vol* pVol, FFatMountFlag dwFlag, ComCxt* pCxt);
	extern void		ffat_xde_create(Node* pNodeChild, FatDeSFN* pDE, t_uint8 bCheckSum,
									XDEInfo* pXDEInfo);
	extern FFatErr	ffat_xde_afterCreate(Node* pNodeParent, Node* pNodeChild,
									t_uint32* pdwClustersDE, t_int32 dwClusterCountDE);
	extern void		ffat_xde_lookup(Node* pNodeChild);
	extern void		ffat_xde_fillNodeInfo(Node* pNode, FatGetNodeDe* pNodeDE, XDEInfo* pXDEInfo);

	extern void		ffat_xde_rename(Node* pNodeSrc, Node* pNodeDes);
	extern void		ffat_xde_renameUpdateDE(Node* pNodeSrc, Node* pNodeDes,
									FatDeSFN* pDE, t_uint8 bCheckSum);
	extern FFatErr	ffat_xde_afterRename(Node* pNodeParentDes, Node* pNodeNew,
									t_uint32* pdwClustersDE, t_int32 dwClusterCountDE);

	extern FFatErr	ffat_xde_updateXDE(Node* pNode, XDEInfo* pXDEInfo, ComCxt* pCxt);

	extern FFatErr	ffat_xde_getXDE(Vol* pVol, FatGetNodeDe* pNodeDE, ComCxt* pCxt);
	extern void		ffat_xde_generateXDE(Vol* pVol, FatDeSFN* pDeSFN, ExtendedDe* pXDE, t_uint32 dwUid,
										t_uint32 dwGid, t_uint16 wPerm, t_uint8 bCheckSum);
	extern FFatErr	ffat_xde_setStatus(Node* pNode, FFatNodeFlag dwAttr, ComCxt* pCxt);
	extern FFatErr	ffat_xde_getGUIDFromNode(Node* pNode, XDEInfo* pstXDEInfo);

#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFAT_ADDON_XDE_H_ */
