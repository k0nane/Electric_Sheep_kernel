/*
 * TFS4 2.0 FFAT(Final FAT) filesystem Developed by Flash Planning Group.
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
 * @file		ffat_fsm.h
 * @brief		Interface module between NESTLE and FFAT
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version	  	OCT-11-2007 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFAT_NESTLE_H_
#define _FFAT_NESTLE_H_

// includes

// defines

// enum

// typedefs
typedef struct
{
	t_uint32		dwNestleFlag;
	t_uint32		dwFFatFlag;
} FlagConvert;

// constant definitions

// external variable declarations

// function declarations

#endif	/* #ifndef _FFAT_TEMPLATE_H_ */
