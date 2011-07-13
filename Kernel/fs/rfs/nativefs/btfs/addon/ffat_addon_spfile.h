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
 * @file		ffat_addon_spfile.h
 * @brief		Special File
 * @author		GwangOk Go
 * @version		DEC-05-2007 [GwangOk Go] First writing
 * @version		JUN-17-2009 [GwangOk Go] rename symlink into spfile
 * @see			None
 */

#ifndef _FFAT_ADDON_SPFILE_H_
#define _FFAT_ADDON_SPFILE_H_

// includes
#include "ffat_api.h"
#include "ffat_node.h"
//#include "ffat_vol.h"

// defines
#define SYMLINK_IS_SYMLINK(_pNode) ((NODE_ADDON_FLAG(_pNode) & ADDON_NODE_SYMLINK) ? FFAT_TRUE : FFAT_FALSE)

// enum

// typedefs

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_spfile_init(void);
	extern void		ffat_spfile_mount(Vol* pVol, FFatMountFlag dwFlag);
	extern FFatErr	ffat_spfile_createSymlink(Node* pNodeParent, Node* pNodeChild, t_wchar* psName,
									t_wchar* psPath, FFatCreateFlag dwFlag, void* pAddonInfo, ComCxt* pCxt);
	extern FFatErr	ffat_spfile_readSymlink(Node* pNode, t_wchar* psPath, t_int32 dwLen, t_int32* pdwLen,
									t_uint8* pbCrtTimeTenth, ComCxt* pCxt);
	extern void		ffat_spfile_create(Node* pNode, FFatCreateFlag dwCreateFlag);

	extern FFatErr	ffat_spfile_afterLookup(Node* pNode, t_uint8* pbCrtTimeTenth, ComCxt* pCxt);
	extern void		ffat_spfile_rename(Node* pNodeSrc, Node* pNodeDes);
	extern FFatErr	ffat_spfile_setStatus(Node* pNode, FFatNodeStatus* pStatus, ComCxt* pCxt);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_ADDON_SPFILE_H_ */
