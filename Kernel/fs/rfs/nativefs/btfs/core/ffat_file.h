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
 * @file		ffat_file.h
 * @brief		This function defines FFatFile
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFAT_FILE_H_
#define _FFAT_FILE_H_

// includes
#include "ffat_types.h"
#include "ffat_errno.h"

#include "ffat_node.h"

// defines

// enum

// typedefs

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_file_init(void);
	extern FFatErr	ffat_file_terminate(void);

	extern FFatErr	ffat_file_changeSize(Node* pNode, t_uint32 dwSize,
							FFatChangeSizeFlag dwCSFlag, FFatCacheFlag dwCacheFlag, ComCxt* pCxt);

	extern t_int32	ffat_file_read(Node* pNode, t_uint32 dwOffset, t_int8* pBuff, 
							t_int32 dwSize, FFatReadFlag dwReadFlag, ComCxt* pCxt);
	extern t_int32	ffat_file_write(Node* pNode, t_uint32 dwOffset, t_int8* pBuff, t_int32 dwSize,
							FFatWriteFlag dwWriteFlag, ComCxt* pCxt);
	extern FFatErr	ffat_file_unlink(Node* pNodeParent, Node* pNode,
							NodeUnlinkFlag dwNUFlag, ComCxt* pCxt);

	extern FFatErr	ffat_file_getClusters(Node* pNode, t_uint32 dwOffset, t_uint32 dwSize,
							FFatVC* pVC, ComCxt* pCxt);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_FILE_H_ */



