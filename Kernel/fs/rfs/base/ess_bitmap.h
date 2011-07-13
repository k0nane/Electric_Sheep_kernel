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
 * @file		ess_bitmap.h
 * @brief		module for bitmap operation 
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version	  	JUL-18-2006 [DongYoung Seo] First writing
 * @see			None
 */

// includes
#include "ess_types.h"

#ifndef _ESS_BITMAP_H_
#define _ESS_BITMAP_H_

// defines

#ifdef ESS_STRICT_CHECK
	#define ESS_BITMAP_STRICT_CHECK
#endif

#define ESS_BITMAP_SET(pBitmap, _bit)	(((t_uint8*)pBitmap)[(_bit) >> 3] |= (0x01U << ((_bit) & 0x07)))
#define ESS_BITMAP_CLEAR(pBitmap, _bit)	(((t_uint8*)pBitmap)[(_bit) >> 3] &= ~(0x01U << ((_bit) & 0x07)))
#define ESS_BITMAP_CHANGE(pBitmap, _bit)	(((t_uint8*)pBitmap)[(_bit) >> 3] ^= ~(0x01U << ((_bit) & 0x07)))

#define ESS_BITMAP_IS_SET(pBitmap, _bit)		((((t_uint8*)pBitmap)[(_bit) >> 3] & (0x01U << ((_bit) & 0x07))) ? 1 : 0)
#define ESS_BITMAP_IS_CLEAR(pBitmap, _bit)	((((t_uint8*)pBitmap)[(_bit) >> 3] & (0x01U << ((_bit) & 0x07))) ? 0 : 1)


// enum

// typedefs

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern t_int32	EssBitmap_GetLowestBitZero(t_uint8* pBitmap, t_int32 dwSize);
	extern t_int32	EssBitmap_GetLowestBitOne(t_uint8* pBitmap, t_int32 dwSize);
	extern t_int32	EssBitmap_GetCountOfBitOne(t_uint8* pBitmap, t_int32 dwSize);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _ESS_BITMAP_H_ */



