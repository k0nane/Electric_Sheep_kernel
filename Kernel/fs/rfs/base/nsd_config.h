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
 * @file		nsd_config.h
 * @brief		Set configuration parameters
 * @author		Kwon Moon Sang
 * @date		2008/08/26
 * @remark		Some configurable parameters are here.
 *				You need to change the parameters appropriately for your project.
 * @version		RFS_3.0.0_b035_RC
 * @note
 *				26-AUG-2008 [Moonsang Kwon]: First writing
 *				19-SEP-2008 [Moonsang Kwon]: Add several features
 *					1. Add multi-thread env. support. You can enable or disable
 *						multi-thread env.
 *					2. Add message format configuration flag. You can control
 *						whether to include the time, thread id, and error number
 *						for each debug message.
 *				15-OCT-2008 [Moonsang Kwon]: Add _MSC_VER
 *						1000 : Visual C++ 4.x
 *						1100 : Visual C++ 5
 *						1200 : Visual C++ 6
 *						1300 : Visual C++ .NET
 *						1310 : Visual C++ .NET 2003
 *						1400 : Visual C++ .NET 2005
 *						1500 : Visual C++ .NET 2008
 */

#ifndef __NSD_CONFIG_H__
	#define __NSD_CONFIG_H__

///< CONF:set configurable parameters
#define ENABLE_UNICODE			1	///< define 1 if unicode as base string
#define ENABLE_WINCE			0	///< define 1 if WINCE is the target
#define ENABLE_UNIX				0	///< define 1 the target is Unix (Linux)
#define ENABLE_LINUX_KERNEL		0	///< define 1 if you want to use inside Linux Kernel
#define ENABLE_NUCLEUS			0	///< define 1 if you want to use inside Nucleus
#define ENABLE_SYMBIAN			0	///< define 1 if you want to use inside Symbian
#define ENABLE_MULTI_THREAD		1	///< define 1 if you use me in multi-thread env.
#define ENABLE_UNIT_TEST		1	///< define 1 if you want to do unit test
#define ENABLE_RFS_TOOLS		0	///< define 1 if you want to use inside RFS TOOLS

///< CONF: message information configuration
#define ENABLE_MESSAGE_THREAD_ID		1	///< define 1 if you want thread id for each message
#define ENABLE_MESSAGE_TIME				1	///< define 1 if you want time information for each message
#define ENABLE_MESSAGE_ERROR_NUMBER		1	///< define 1 if you want error number for each message

#ifdef TFS5_DEBUG
#else
	#define NSD_RELEASE
#endif

// For release mode, we disable all unit test code
#if defined(NSD_RELEASE)
	#undef ENABLE_UNIT_TEST
	#define ENABLE_UNIT_TEST	0
#endif

// We are in Windows (Visual Studio)
#if defined(_WIN32)
	#undef ENABLE_UNIX
	#undef ENABLE_LINUX_KERNEL

	#define ENABLE_UNIX				0
	#define ENABLE_LINUX_KERNEL		0
#endif

#if defined(__arm)
	#define _MSC_VER	1200	///< To supress warning error

	#undef ENABLE_UNICODE
	#define ENABLE_UNICODE  0	///< ADS1.2 does not support unicode or multibyte string
#endif

// MSVC defines _UNICODE if default string is "Unicode"
#if defined(_UNICODE)
	#undef ENABLE_UNICODE
	#define ENABLE_UNICODE  1
#endif

// For VS6.0, if _MBCS is defined, then disable ENABLE_UNICODE
#if defined(_MBCS)
	#undef ENABLE_UNICODE
	#define ENABLE_UNICODE  0
#endif

#if defined(_RFS_TOOLS)
	#undef ENABLE_RFS_TOOLS
	#define ENABLE_RFS_TOOLS	1

	#undef ENABLE_MULTI_THREAD
	#define ENABLE_MULTI_THREAD 0

	#undef ENABLE_UNICODE
	#define ENABLE_UNICODE  0
#endif

// If WINCE is defined, then define ENABLE_WINCE 1
#if defined(CONFIG_WINCE)
	#undef ENABLE_WINCE
	#define ENABLE_WINCE	1
#endif

#if defined(CONFIG_RTOS) && !defined(_WIN32) && (ENABLE_RFS_TOOLS == 0)
	#undef ENABLE_NUCLEUS
	#define ENABLE_NUCLEUS	1

	#undef ENABLE_MULTI_THREAD
	#define ENABLE_MULTI_THREAD 0
#endif

#if defined(CONFIG_SYMBIAN)
	#undef ENABLE_SYMBIAN
	#define ENABLE_SYMBIAN 1

	#undef ENABLE_UNICODE
	#define ENABLE_UNICODE  1

	#undef ENABLE_MULTI_THREAD
#define ENABLE_MULTI_THREAD 0
#endif

// If we are in Linux Kernel, disable ENABLE_UNICODE
#if defined(__KERNEL__)
	#undef ENABLE_UNICODE
	#undef ENABLE_WINCE
	#undef ENABLE_UNIX
	#undef ENABLE_LINUX_KERNEL

	#define ENABLE_UNICODE		0
	#define ENABLE_WINCE		0
	#define ENABLE_UNIX			1
	#define ENABLE_LINUX_KERNEL	1
#endif

// Under Unix, we use OEM code
#if (ENABLE_UNIX >= 1)
	#undef ENABLE_UNICODE
	#define ENABLE_UNICODE	0
#endif

#if (ENABLE_MULTI_THREAD == 0)
	// If we are not in multi-thread env, disable ENABLE_MESSAGE_THREAD_ID.
	#undef ENABLE_MESSAGE_THREAD_ID
	#define	ENABLE_MESSAGE_THREAD_ID	0
#endif

#endif  // end of #ifndef __NSD_CONFIG_H__
