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
 * @file		ess_types.h
 * @brief		The primitive types for ESS team.
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _ESS_TYPES_H_
#define _ESS_TYPES_H_

// includes
#include "ess_base_config.h"

// defines
#ifndef ESS_TRUE
	#define	ESS_TRUE		1		//<! true, must be 1,
#endif

#ifndef ESS_FALSE
	#define ESS_FALSE		0		//<! false, must be 0,
#endif

#ifndef NULL
#define NULL	0
#endif

// enum

// typedefs

#ifndef t_int8
	typedef signed char			t_int8;
#endif

#ifndef t_uint8
	typedef unsigned char		t_uint8;
#endif

#ifndef t_int16
	typedef short int			t_int16;
#endif

#ifndef t_uint16
	typedef unsigned short int	t_uint16;
#endif

#ifndef t_int32
	typedef int					t_int32;
#endif

#ifndef t_uint32
	typedef unsigned int		t_uint32;
#endif

#ifndef t_wchar
	typedef t_uint16			t_wchar;		/* for UNICODE character */
#endif

#ifndef t_boolean
	typedef t_int32				t_boolean;
#endif

#ifndef t_int
	typedef int					t_int;			// pointer size
#endif

#ifndef t_uint
	typedef t_uint32			t_uint;
#endif

#ifndef t_int64
	typedef long long			t_int64;
#endif

#ifndef t_uint64
	typedef unsigned long long	t_uint64;
#endif

/* flags for malloc */
typedef t_int32	EssMallocFlag;
enum _EssMallocFlag
{
	ESS_MALLOC_NONE			= 0x00000000,	/*!< no flag */
	ESS_MALLOC_IO			= 0x00000001,	/*!< for I/O module*/

	ESS_MALLOC_DUMMY		= 0x7FFFFFFF
};

typedef struct _EssTime
{
	t_int32		tm_msec;	/*!< milliseconds after the seconds - [0,999] */
	t_int32		tm_sec;		/*!< seconds after the minute - [0,59] */
	t_int32		tm_min;		/*!< minutes after the hour - [0,59] */
	t_int32		tm_hour;	/*!< hours since midnight - [0,23] */
	t_int32		tm_mday;	/*!< day of the month - [1,31] */
	t_int32		tm_mon;		/*!< months since January - [0,11] */
	t_int32		tm_year;	/*!< years since 1900 */
	t_int32		tm_wday;	/*!< days since Sunday - [0,6] */
	t_int32		tm_yday;	/*!< days since January 1 - [0,365] */
	t_int32		tm_isdst;
} EssTime;

#ifdef _WIN32
	#if defined(CONFIG_RTOS) || defined(CONFIG_WINCE)
	// to suppress a warning message, "pointer truncation from ...".
	#pragma warning(disable: 4311)
	#pragma warning(disable: 4133)

	// to suppress a warning message, "conversion from 'x' to 'y' of greater size".
	#pragma warning(disable: 4312)
	#endif
#endif
// external variable declarations

// function declarations
#ifdef __cplusplus
	extern "C" {
#endif

	// None

#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _ESS_TYPES_H_ */


