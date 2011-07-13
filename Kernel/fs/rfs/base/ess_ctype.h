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
 * @file		ess_ctype.h
 * @brief		migration from FSM/LIB/util.h
 * @author		Soojeong kim(sj003.kim@samsung.com)
 * @version	  	JEN-08-2007 [Soojeong Kim] First writing
 * @see			None
 */



#ifndef _ESS_CTPYE_H_
#define _ESS_CTPYE_H_

// includes
#include "ess_types.h"
#include "ess_unicode.h"

// defines
#define ESS_IsSpace(x)	(((x == ' ') || (x == '\t') || (x == '\r') || (x == '\n') || (x == '\v')) ? true : false)

#if (!defined(ESS_DUL))
	#define	ESS_STRICMP(a, b)				ESS_Stricmp(a, b)
#else
	#define	ESS_STRICMP(a, b, vol)			ESS_Stricmp(a, b, vol)	// Volume level NLS support
#endif

#ifdef __cplusplus
	extern "C" {
#endif

	extern void		ESS_Bincpy(void* pDst, const void* pSrc, t_uint32 dwLen);
	extern t_int32	ESS_Atoi(t_int8* str);
	extern t_int8*	ESS_Itoa(t_int32 nVal, t_int8* str);

	extern t_int32	ESS_Stricmp(const t_int8* s1, const t_int8* s2);

	extern t_int32	ESS_Isalnum(t_int8 c);

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _ESS_CTPYE_H_ */



