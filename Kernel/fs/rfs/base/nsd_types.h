/*
* Nestle filesystem platform Developed by Flash Software Group.
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
* @file		nsd_types.h
* @brief	Define basic types
* @author	Kwon Moon Sang
* @date		2008/08/26
* @remark	Just base of programming
* @version	RFS_3.0.0_b035_RC
* @note
*			26-AUG-2008 [Moonsang Kwon]: First writing
*			18-SEP-2008 [Moonsang Kwon]: Fix wchar_t definition problem in Linux
*				Kernel
*					1. Inside Linux kernel, we can use <linux/types.h> and
*					<linux/nls.h> to use wchar_t data type.
*			19-SEP-2008 [Moonsang Kwon]: Add two data types.
*					1. THREAD_ID_TYPE for thread id type
*					2. NSD_ERROR_TYPE for error number type
*			23-NOV-2008 [Moonsang Kwon]: Add unit test function type.
*					1. UTEST_FUNCTION for unit test function definition
*/

#ifndef __NSD_TYPES_H__
	#define __NSD_TYPES_H__

#include "nsd_config.h"

#if defined(_WIN32)
	#include <windows.h>
#endif

#if defined(__KERNEL__)
	///< blank
#elif (ENABLE_UNIX >= 1)
	#include <wchar.h>
#else
	///< blank
#endif

#if defined(_WIN32)
	#include <tchar.h>
#else
	#undef _TCHAR_DEFINED
#endif

#if defined(VS6)
#else
#endif

#if (ENABLE_LINUX_KERNEL >= 1)
	#include <linux/types.h>
	#include <linux/nls.h>
	#define _WCHAR_T_DEFINED
#endif

#if (ENABLE_RFS_TOOLS >= 1)
	#include <wchar.h>
	#define _WCHAR_T_DEFINED
#endif 

// Define wchar_t if it is not defined before
#if defined(__arm)
	// ARM does not have <wchar.h>
	#if (ENABLE_SYMBIAN == 0)
		typedef unsigned short	wchar_t;
	#endif
#else
	#ifndef _WCHAR_T_DEFINED
		#if (ENABLE_UNIX >= 1) && defined(__KERNEL__)
			typedef int	wchar_t;	///< For Linux, it is 4-byte
		#elif (ENABLE_UNIX >= 1)
			///< blank
		#else
			typedef short	wchar_t;
		#endif

		#define _WCHAR_T_DEFINED
	#endif
#endif

#if (ENABLE_UNICODE >= 1)
	#ifndef _TCHAR_DEFINED
		typedef wchar_t TCHAR;
		#define _TCHAR_DEFINED
	#endif

	#define TNULL	L'\0'
	#ifndef TEXT
		#define __TEXT(x)	L##x
		#define TEXT(x)		__TEXT(x)
	#endif
#else
	#ifndef _TCHAR_DEFINED
		typedef char TCHAR;
		#define _TCHAR_DEFINED
	#endif

	#define TNULL	'\0'
	#ifndef TEXT
		#define __TEXT(x)	x
		#define TEXT(x)		__TEXT(x)
	#endif
#endif

typedef TCHAR		*PTCHAR;	///< define TCHAR string pointer
typedef PTCHAR		*PPTCHAR;	///< define pointer of PTCHAR
typedef const TCHAR	*PCTSTR;	///< define constant TCHAR string pointer

// define intermediate field separator
#if defined(IFS)
	#undef IFS
	#define IFS	'\\'
#else
	#define IFS	'\\'
#endif

///< CONF: set up IFS. For Unix, set ENABLE_UNIX at nsd_config.h
#if (ENABLE_UNIX >= 1)
	#undef IFS
	#define IFS	'/'
#endif
#define TIFS	TEXT(IFS)

typedef PTCHAR	PTSTR, *PPTSTR;

#if (ENABLE_MULTI_THREAD >= 1)
	///< CONF: define thread type here.
	#if defined(_WIN32)
		typedef DWORD THREAD_ID_TYPE;
		#define __THREAD_ID_TYPE_DEFINED
	#elif defined(__arm)
		typedef int	THREAD_ID_TYPE;
		#define __THREAD_ID_TYPE_DEFINED
	#else
		typedef int	THREAD_ID_TYPE;
		#define __THREAD_ID_TYPE_DEFINED
	#endif
#endif

///< CONF: define error number type used in your system
#if defined(_WIN32)
	typedef DWORD NSD_ERROR_TYPE;
	#define __NSD_ERROR_TYPE_DEFINED
#elif defined(__arm)
	typedef int NSD_ERROR_TYPE;
	#define __NSD_ERROR_TYPE_DEFINED
#else
	typedef int NSD_ERROR_TYPE;
	#define __NSD_ERROR_TYPE_DEFINED
#endif

#endif // end of #ifndef __NSD_TYPES_H__
