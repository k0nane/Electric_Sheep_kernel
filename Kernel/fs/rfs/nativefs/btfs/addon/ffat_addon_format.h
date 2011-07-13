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
 * @file		ffat_addon_format.h
 * @brief		This file defines format operation
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-21-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFAT_ADDON_FORMAT_H_
#define _FFAT_ADDON_FORMAT_H_

// includes
#include "ffat_types.h"
#include "ffat_addon_api.h"

// defines

// enum

// typedefs

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_addon_format(FFatFormatInfo* pFormatInfo, ComCxt* pCxt);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_ADDON_FORMAT_H_ */



