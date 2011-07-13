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
* @file		nsd_debug.h
* @brief	Define debug MACROs and functions
* @author	Kwon Moon Sang
* @date		2008/08/26
* @remark		
* @version	RFS_3.0.0_b035_RC
* @note
*			26-AUG-2008 [Moonsang Kwon]: First writing
*			18-SEP-2008 [Moonsang Kwon]: Fix Linux kernel header include problem
*				found by Hayoung, Kim
*				1. If Linux kernel version is less than or equal to 2.6.18, we
*					should include <linux/config.h>. Otherwise, we should use
*					<linux/autoconf.h>
*			19-SEP-2008 [Moonsang Kwon]: Add multi-thread mode support
*				1. Change _GetNsdTimeSec() with _NsdGetTimeSec().
*				2. Protect the debug MACROs with _NSD_LOCK_CONSOLE() and
*					_NSD_UNLOCK_CONSOLE().
*				3. Add conditional message MACROs. The original MACROs are 
*					changed to non-conditional MACROs.
*				4. For assert check, NSD_AS() (= NSD_ASSERT()) and NSD_ASZ() MACROs are added.
*/

#ifndef __NSD_DEBUG_H__
	#define __NSD_DEBUG_H__

#include "ns_oam_defs.h"
#include "nsd_types.h"
#include "nsd_version.h"

#if (ENABLE_LINUX_KERNEL >= 1)
	#include <linux/version.h>

	#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 18)
		#include <linux/config.h>
	#else
		#include <linux/autoconf.h>
	#endif

	#include <linux/module.h>
#endif

// MACRO for Unicode
#if defined(T2W)
#else
	// translate OEM string to Unicode string
	#define __T2W(x)	L##x
	#define T2W(x)		__T2W(x)
#endif

/*******************************************************************************
	Define __FILE__/__FUNCTION__ for Unicode
*******************************************************************************/
#if defined(_WIN32) && defined(_MSC_VER) && (_MSC_VER < 1300)
	// Visual Studio 6.0 or under
	#define __FUNCTION__	"UNSUPPORTED"
#endif

#if defined(__arm)
	#undef __FUNCTION__
	#define __FUNCTION__ __func__	// for ARM compiler
#endif

#if (ENABLE_SYMBIAN == 1)
#define __WFILE__		__FILE__
#define __WFUNCTION__	__FUNCTION__
#else 
#define __WFILE__		T2W(__FILE__)
#define __WFUNCTION__	T2W(__FUNCTION__)
#endif

// MACRO from Multi-byte string to TCHAR string
#if defined(_T)
	#undef _T
#endif

///< CONF: you need to set _MY_NSD_PRINTF here
#if (ENABLE_UNICODE >= 1) && (ENABLE_NUCLEUS == 0)
    #if defined(__CYGWIN__) || defined(__arm)
        // gcc or gcc in CYGWIN does not support L"__FUNCTION__"
		// armcc of ads1.2 and rvds2.x do not support L"__FUNCTION__"
        #define __TFILE__       _NsdMbsToWcs(__FILE__)
        #define __TFUNCTION__   _NsdMbsToWcsC(__FUNCTION__)
    #else
		#define __TFILE__		__WFILE__		///< define FILE name macro
		#define __TFUNCTION__	__WFUNCTION__	///< define FUNCTION macro
    #endif

	#define _T(x)			T2W(x)
	#define _NSD_MY_PRINTF	OamPrintf			///< define printf function
#else
	#define _T(x)			x
	#define __TFILE__		__FILE__
	#define __TFUNCTION__	__FUNCTION__
	#define _NSD_MY_PRINTF	OamPrintf			///< define printf function

	#if (ENABLE_LINUX_KERNEL >= 1)
		#undef _NSD_MY_PRINTF
		#define _NSD_MY_PRINTF	printk
	#endif
#endif

/** Define a clock function which returns the current time in second.
 *  Each message will print the current time so that you can refer
 *  how much time has passed since the start.
 *  CONF: define a time function which returns the current time in second.
 */
#define _NSD_GET_TIME_SEC()	_NsdGetTimeSec()

/** Define a function which returns latest error number.
 *  Each message will print error number.
 *  CONF: define a error number returning function.
 */
#define _NSD_GET_LAST_ERROR()	_NsdGetLastError()

/** Enable or disable console lock. If you use the debug MACROs with
 *  multi-thread env.,  you need to protect the debug MACRO with semaphore
 *  so that the messages are not mixed.
 *  CONF: you need to define your own _NsdLockConsole() and _NsdUnlockConsole()
 */
#if (ENABLE_MULTI_THREAD >= 1)
	#define _NSD_LOCK_CONSOLE()		_NsdLockConsole()
	#define _NSD_UNLOCK_CONSOLE()	_NsdUnlockConsole()
	#define _NSD_GET_THREAD_ID()	_NsdGetThreadId()
#else
	#define _NSD_LOCK_CONSOLE()
	#define _NSD_UNLOCK_CONSOLE()
	#define _NSD_GET_THREAD_ID()
#endif
 
#if (ENABLE_LINUX_KERNEL >= 1) && defined(RFS_FS_GLUE_DEBUG_LEVEL)
	#define NSD_DEBUG_LEVEL		RFS_FS_GLUE_DEBUG_LEVEL
#elif (ENABLE_WINCE >= 1) && defined(PFS_FS_GLUE_DEBUG_LEVEL)
	#define NSD_DEBUG_LEVEL		PFS_FS_GLUE_DEBUG_LEVEL
#else
	///< CONF: Change the DEBUG_LEVEL as you like.
	#define NSD_DEBUG_LEVEL		1		///< set debug level
#endif

	#define NSD_DL_PANIC		0		///< panic error (don't know the reason), system is halted, always printed
	#define NSD_DL_CRITICAL		1		///< critical error (know the reason), system is not halted, always printed
	#define NSD_DL_ERROR		2		///< error message of your package
	#define NSD_DL_SYSTEM		3		///< system message
	#define NSD_DL_INFO			4		///< information message
	#define NSD_DL_DEBUG		5		///< debugging message
	#define NSD_DL_VERBOSE		6		///< more information about debugging
	#define NSD_DL_ASSERT		0		///< assert debug level

	///< CONF: enable or disable each debug level separately
	#define NSD_DL_PANIC_ENABLE		1		///< enable PANIC debug message
	#define NSD_DL_CRITICAL_ENABLE	1		///< enable CRITICAL debug message
	#define NSD_DL_ERROR_ENABLE		1		///< enable ERROR debug message
	#define NSD_DL_SYSTEM_ENABLE	1		///< enable SYSTEM debug message
	#define NSD_DL_INFO_ENABLE		1		///< enable INFO debug message
	#define NSD_DL_DEBUG_ENABLE		1		///< enable DEBUG debug message
	#define NSD_DL_VERBOSE_ENABLE	1		///< enable VERBOSE debug message
	#define NSD_DL_ASSERT_ENABLE	1		///< enable ASSERT debug message
 
///< Define settings for the release mode
#if defined(NSD_RELEASE)
	// CONF: Setup the release mode settings. The default is to off all debug settings.
	#undef NSD_DL_PANIC_ENABLE
	#undef NSD_DL_CRITICAL_ENABLE
	#undef NSD_DL_ERROR_ENABLE
	#undef NSD_DL_SYSTEM_ENABLE
	#undef NSD_DL_INFO_ENABLE
	#undef NSD_DL_DEBUG_ENABLE
	#undef NSD_DL_VERBOSE_ENABLE
	#undef NSD_DL_ASSERT_ENABLE

	#define NSD_DL_PANIC_ENABLE		1
	#define NSD_DL_CRITICAL_ENABLE	1
	#define NSD_DL_ERROR_ENABLE		0
	#define NSD_DL_SYSTEM_ENABLE	0
	#define NSD_DL_INFO_ENABLE		0
	#define NSD_DL_DEBUG_ENABLE		0
	#define NSD_DL_VERBOSE_ENABLE	0
	#define NSD_DL_ASSERT_ENABLE	0
#endif

#define _NSD_DL_DEBUG_STR_TEMPLATE(x)	\
	TEXT("[") TEXT(NSD_NAME_STRING) TEXT("]") TEXT(x)	///< debug string template

#define NSD_DL_PANIC_STR	_NSD_DL_DEBUG_STR_TEMPLATE("[PAN]")	///< Panic error
#define NSD_DL_CRITICAL_STR	_NSD_DL_DEBUG_STR_TEMPLATE("[CRT]")	///< Critical error
#define NSD_DL_ERROR_STR	_NSD_DL_DEBUG_STR_TEMPLATE("[ERR]")	///< Error, but not critical
#define NSD_DL_SYSTEM_STR	_NSD_DL_DEBUG_STR_TEMPLATE("[SYS]")	///< System message
#define NSD_DL_INFO_STR		_NSD_DL_DEBUG_STR_TEMPLATE("[INF]")	///< Information message
#define NSD_DL_DEBUG_STR	_NSD_DL_DEBUG_STR_TEMPLATE("[DBG]")	///< Debug information
#define NSD_DL_VERBOSE_STR	_NSD_DL_DEBUG_STR_TEMPLATE("[VER]")	///< For more information about debugging
#define NSD_DL_ASSERT_STR	_NSD_DL_DEBUG_STR_TEMPLATE("[AST]")	///< For ASSERT message

/**
 * Debug message macros
 */
#define _MAMMOTH_STOP_STRING	\
	TEXT("\r\n<log P1=\"100\" P2=\"CRITICAL ERROR OCCURRED. CHECK IT!!!\" >\r\n")

///< print out the stop message of Mammoth SQA system
#define _NSD_PRINT_MAMMOTH_STOP()	\
	do	\
	{	\
		_NSD_MY_PRINTF(_MAMMOTH_STOP_STRING);	\
	} while (0)

///< print current code line information
#if (ENABLE_UNIX >= 1) || (ENABLE_RFS_TOOLS >= 1)
	#define _NSD_TRIM_PARENT_DIRECTORY(x)	(x)
#else
	#define _NSD_TRIM_PARENT_DIRECTORY(x)	_NsdGetStrRchrT((x), TIFS)
#endif

#if (ENABLE_MESSAGE_THREAD_ID >= 1)
	#define _NSD_PRINT_THREAD_ID()	\
		_NSD_MY_PRINTF(TEXT("[Tid:0x%05x]"),	_NSD_GET_THREAD_ID())
#else
	#define _NSD_PRINT_THREAD_ID()
#endif

#if (ENABLE_MESSAGE_TIME >= 1)
	#define _NSD_PRINT_TIME()	\
		_NSD_MY_PRINTF(TEXT("[Tim:%d]"),	_NSD_GET_TIME_SEC())
#else
	#define _NSD_PRINT_TIME()
#endif

#if (ENABLE_MESSAGE_ERROR_NUMBER >= 1)
	#define _NSD_PRINT_ERROR_NUMBER()	\
		_NSD_MY_PRINTF(TEXT("[Err:%05d]"),	_NSD_GET_LAST_ERROR())
#else
	#define _NSD_PRINT_ERROR_NUMBER()
#endif

#define _NSD_PRINT_CODE_LINE()	\
	do	\
	{	\
		_NSD_PRINT_TIME();	\
		_NSD_PRINT_THREAD_ID();	\
		_NSD_PRINT_ERROR_NUMBER();	\
		_NSD_MY_PRINTF(TEXT("[%s(#%d)@%s]: "), \
			__TFUNCTION__,	\
			__LINE__,	\
			_NSD_TRIM_PARENT_DIRECTORY(__TFILE__)	\
		); \
	} while (0)

///< print panic message. system is halted
#if (NSD_DL_PANIC <= NSD_DEBUG_LEVEL) && (NSD_DL_PANIC_ENABLE >= 1)
	#define NSD_D0(msg)	\
		do	\
		{	\
			_NSD_LOCK_CONSOLE();	\
				_NSD_MY_PRINTF(NSD_DL_PANIC_STR);	\
				_NSD_PRINT_CODE_LINE();	\
				_NSD_MY_PRINTF msg ;	\
				_NSD_PRINT_MAMMOTH_STOP();	\
				_NSD_MY_PRINTF(_T("\r\n"));	\
			_NSD_UNLOCK_CONSOLE();	\
			\
			_NsdEnterEndlessLoop();	\
		} while (0)

	#define NSD_D0C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			NSD_D0(msg);	\
		} while (0)
#else
	#define NSD_D0(msg)
	#define NSD_D0C(cond, msg)
#endif

///< print critical error message. system is not halted
#if (NSD_DL_CRITICAL <= NSD_DEBUG_LEVEL) && (NSD_DL_CRITICAL_ENABLE >= 1)
	#define NSD_D1(msg)	\
		do	\
		{	\
			_NSD_LOCK_CONSOLE();	\
				_NSD_MY_PRINTF(NSD_DL_CRITICAL_STR);	\
				_NSD_PRINT_CODE_LINE();	\
				_NSD_MY_PRINTF msg ;	\
				_NSD_MY_PRINTF(_T("\r\n"));	\
			_NSD_UNLOCK_CONSOLE();	\
		} while (0)

	#define NSD_D1C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			NSD_D1(msg);	\
		} while (0)
#else
	#define NSD_D1(msg)
	#define NSD_D1C(cond, msg)
#endif

///< print error message
#if (NSD_DL_ERROR <= NSD_DEBUG_LEVEL) && (NSD_DL_ERROR_ENABLE >= 1)
	#define NSD_D2(msg)	\
		do	\
		{	\
			_NSD_LOCK_CONSOLE();	\
				_NSD_MY_PRINTF(NSD_DL_ERROR_STR);	\
				_NSD_PRINT_CODE_LINE();	\
				_NSD_MY_PRINTF msg ;	\
				_NSD_MY_PRINTF(_T("\r\n"));	\
			_NSD_UNLOCK_CONSOLE();	\
		} while (0)

	#define NSD_D2C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			NSD_D2(msg);	\
		} while (0)
#else
	#define NSD_D2(msg)
	#define NSD_D2C(cond, msg)
#endif

///< print system (error) message
#if (NSD_DL_SYSTEM <= NSD_DEBUG_LEVEL) && (NSD_DL_SYSTEM_ENABLE >= 1)
	#define NSD_D3(msg)	\
		do	\
		{	\
			_NSD_LOCK_CONSOLE();	\
				_NSD_MY_PRINTF(NSD_DL_SYSTEM_STR);	\
				_NSD_PRINT_CODE_LINE();	\
				_NSD_MY_PRINTF msg ;	\
				_NSD_MY_PRINTF(_T("\r\n"));	\
			_NSD_UNLOCK_CONSOLE();	\
		} while (0)

	#define NSD_D3C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			NSD_D3(msg);	\
		} while (0)
#else
	#define NSD_D3(msg)
	#define NSD_D3C(cond, msg)
#endif

///< print normal information message
#if (NSD_DL_INFO <= NSD_DEBUG_LEVEL) && (NSD_DL_INFO_ENABLE >= 1)
	#define NSD_D4(msg)	\
		do	\
		{	\
			_NSD_LOCK_CONSOLE();	\
				_NSD_MY_PRINTF(NSD_DL_INFO_STR);	\
				_NSD_MY_PRINTF msg ;	\
				_NSD_MY_PRINTF(_T("\r\n"));	\
			_NSD_UNLOCK_CONSOLE();	\
		} while (0)

	#define NSD_D4C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			NSD_D4(msg);	\
		} while (0)
#else
	#define NSD_D4(msg)
	#define NSD_D4C(cond, msg)
#endif

///< print debug message
#if (NSD_DL_DEBUG <= NSD_DEBUG_LEVEL) && (NSD_DL_DEBUG_ENABLE >= 1)
	#define NSD_D5(msg)	\
		do	\
		{	\
			_NSD_LOCK_CONSOLE();	\
				_NSD_MY_PRINTF(NSD_DL_DEBUG_STR);	\
				_NSD_PRINT_CODE_LINE();	\
				_NSD_MY_PRINTF msg ;	\
				_NSD_MY_PRINTF(_T("\r\n"));	\
			_NSD_UNLOCK_CONSOLE();	\
		} while (0)

	#define NSD_D5C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			NSD_D5(msg);	\
		} while (0)
#else
	#define NSD_D5(msg)
	#define NSD_D5C(cond, msg)
#endif

///< print more information about debugging
#if (NSD_DL_VERBOSE <= NSD_DEBUG_LEVEL) && (NSD_DL_VERBOSE_ENABLE >= 1)
	#define NSD_D6(msg)	\
	do	\
		{	\
			_NSD_LOCK_CONSOLE();	\
				_NSD_MY_PRINTF(NSD_DL_VERBOSE_STR);	\
				_NSD_MY_PRINTF msg ;	\
				_NSD_MY_PRINTF(_T("\r\n"));	\
			_NSD_UNLOCK_CONSOLE();	\
		} while (0)

	#define NSD_D6C(cond, msg)	\
	do	\
		{	\
			if (!(cond))	break;	\
			NSD_D6(msg);	\
		} while (0)
#else
	#define NSD_D6(msg)
	#define NSD_D6C(cond, msg)
#endif

///< print more information about debugging
#if (NSD_DL_ASSERT <= NSD_DEBUG_LEVEL) && (NSD_DL_ASSERT_ENABLE >= 1)
	#define NSD_D7(msg)	\
	do	\
		{	\
			_NSD_LOCK_CONSOLE();	\
				_NSD_MY_PRINTF(NSD_DL_ASSERT_STR);	\
				_NSD_PRINT_CODE_LINE();	\
				_NSD_MY_PRINTF msg ;	\
				_NSD_MY_PRINTF(_T("\r\n"));	\
			_NSD_UNLOCK_CONSOLE();	\
			_NsdEnterEndlessLoop();	\
		} while (0)

	#define NSD_D7C(cond, msg)	\
	do	\
		{	\
			if (cond)	break;	\
			NSD_D7(msg);	\
		} while (0)

	#define NSD_A7(cond)	\
	do	\
		{	\
			if (cond)	break;	\
			_NSD_LOCK_CONSOLE();	\
				_NSD_MY_PRINTF(NSD_DL_ASSERT_STR);	\
				_NSD_PRINT_CODE_LINE();	\
				_NSD_MY_PRINTF(_T("\r\n"));	\
			_NSD_UNLOCK_CONSOLE();	\
			_NsdEnterDebugBreak();	\
		} while (0)

#else
	#define NSD_D7(msg)
	#define NSD_D7C(cond, msg)
	#define NSD_A7(cond)
#endif

/** Debug MACRO aliases #1.
 * Use these MACROs instead of NSD_D#.
 * For example, you can use NSD_DPANIC instead of NSD_D0. 
 */
#define	NSD_PANIC	NSD_D0	///< Print panic message
#define NSD_CRIT	NSD_D1	///< Print critical message
#define NSD_ERROR	NSD_D2	///< Print error message
#define	NSD_SYST	NSD_D3	///< Print system message
#define NSD_INFO	NSD_D4	///< Print information message
#define NSD_DEBUG	NSD_D5	///< Print debug message
#define NSD_VERB	NSD_D6	///< Print verbose message

/** Debug MACRO aliases #2.
 * Use these MACROs instead of NSD_D#.
 * For example, you can use NSD_PM instead of NSD_D0. 
 * PM means 'P'anic 'M'essage.
 */
#define	NSD_PM		NSD_D0	///< Print panic message
#define NSD_CM		NSD_D1	///< Print critical message
#define NSD_EM		NSD_D2	///< Prit error message
#define	NSD_SM		NSD_D3	///< Print system message
#define NSD_IM		NSD_D4	///< Print information message
#define NSD_DM		NSD_D5	///< Print debug message
#define NSD_VM		NSD_D6	///< Print verbose message

/** Debug MACRO aliases #3.
 * Use these MACROs if condition check is needed.
 * 'C' is added to the above MACRO alias. For example,
 * NSD_PM with condition check MACRO is NSD_PMC.
 */
#define	NSD_PMC		NSD_D0C	///< Print panic message with condition check
#define NSD_CMC		NSD_D1C	///< Print critical message with condition check
#define NSD_EMC		NSD_D2C	///< Print error message with condition check
#define	NSD_SMC		NSD_D3C	///< Print system message with condition check
#define NSD_IMC		NSD_D4C	///< Print information message with condition check
#define NSD_DMC		NSD_D5C	///< Print debug message with condition check
#define NSD_VMC		NSD_D6C	///< Print verbose message with condition check
#define NSD_ASC		NSD_D7C	///< Print assert  message with condition check

#define NSD_AS		NSD_A7	///< Assert MACRO
#define NSD_ASSERT	NSD_AS	///< Assert MACRO

/** Define debug zones here.
 * You can define several debug zones and enable and disable the debugging
 * messages according to the debug zones. Maximum 31 zones are supported.
 */
// CONF: Add your own Debug Zone Mask
enum {
	eNSD_DZM_DEFAULT	= 0x00000000U,	///< Default debug zone mask
	eNSD_DZM_BASE		= 0x00000001U,	///< Debug Zone for package base
	eNSD_DZM_ALWAYS		= 0x00000002U,	///< Print always
	eNSD_DZM_INIT		= 0x00000004U,	///< Debug Zone for Initialization
	eNSD_DZM_TEST		= 0x00000008U,	///< Debug Zone for Test

	///< Add your own debug zone mask here
	///< Don't use 0x80000000
	// OS glue
	eNSD_DZM_API		= 0x00000010U,	///< Debug Zone for API
	eNSD_DZM_DEV		= 0x00000020U,	///< Debug Zone for Device
	eNSD_DZM_OAM		= 0x00000040U,	///< Debug Zone for OS adoptive module

	// Nestle internal
	eNSD_DZM_VOL		= 0x00000100U,	///< Debug Zone for volume
	eNSD_DZM_FILE		= 0x00000200U,	///< Debug Zone for file
	eNSD_DZM_DIR		= 0x00000400U,	///< Debug Zone for dir
	eNSD_DZM_VCBLIST	= 0x00000800U,	///< Debug Zone for vcb list
	eNSD_DZM_VCB		= 0x00000800U,	///< Debug Zone for vcb = vcb list
	eNSD_DZM_FCB		= 0x00001000U,	///< Debug Zone for fcb
	eNSD_DZM_VNODE		= 0x00002000U,	///< Debug Zone for vnode
	eNSD_DZM_PATH		= 0x00004000U,	///< Debug Zone for path
	eNSD_DZM_PCACHE		= 0x00008000U,	///< Debug Zone for path cache
	eNSD_DZM_BCACHE		= 0x00010000U,	///< Debug Zone for buffer cache
	eNSD_DZM_NATIVE		= 0x00020000U,	///< Debug Zone for native manager
	eNSD_DZM_UTIL		= 0x00040000U,	///< Debug Zone for utility

	// native filesystem
	eNSD_DZM_DEVFS		= 0x00100000U,	///< Debug Zone for stack
	eNSD_DZM_BTFS		= 0x00200000U,	///< Debug Zone for stack

	///< Add your own debug zone mask here
	///< Don't use 0x80000000
	eNSD_DZM_ALL		= 0x7FFFFFFFU	///< Enable all debug zone mask
};

/** Set default debug zone mask here.
 * Define the default (initial) debug zone to enable.
 */
#define NSD_DZM_DEFAULT		eNSD_DZM_ALWAYS	///< Default debug zone mask

#define NSD_DZM_TEST		eNSD_DZM_TEST
#define NSD_DZM_GLUE		(eNSD_DZM_API | eNSD_DZM_DEV | eNSD_DZM_OAM)
#define NSD_DZM_NESTLE		(eNSD_DZM_VOL | eNSD_DZM_FILE | eNSD_DZM_DIR | eNSD_DZM_VCBLIST | eNSD_DZM_VCB | eNSD_DZM_FCB |			\
	eNSD_DZM_VNODE | eNSD_DZM_PATH | eNSD_DZM_PCACHE | eNSD_DZM_BCACHE | eNSD_DZM_NATIVE | eNSD_DZM_UTIL | eNSD_DZM_BASE)
#define NSD_DZM_NATIVEFS	(eNSD_DZM_DEVFS | eNSD_DZM_BTFS)

#define NSD_GET_DEBUG_ZONE_MASK()	_NsdGetDebugZoneMask()	///< Get debug mask
#define NSD_SET_DEBUG_ZONE_MASK(x)	_NsdSetDebugZoneMask(x)	///< Set debug mask

/// Add more debug zone to the debug zone mask
#define NSD_ADD_DEBUG_ZONE_MASK(x)	\
	do	\
	{	\
		_NsdSetDebugZoneMask(_NsdGetDebugZoneMask() | x);	\
	} while (0)

/// Remove some debug zone from the debug zone mask
#define NSD_DEL_DEBUG_ZONE_MASK(x)	\
	do	\
	{	\
		_NsdSetDebugZoneMask(_NsdGetDebugZoneMask() & (~x));	\
	} while (0)

/** Manage debug zone mask with several MACROs.
 * NSD_GET_DZM() returns current debug zone mask.
 * NSD_SET_DZM(x) updates the debug zone mask with x.
 * NSD_ADD_DZM(x) adds new debug zone 'x' to the global debug zone mask.
 * NSD_DEL_DZM(x) deletes some debug zone 'x" from the global debug zone mask.
 */
#define NSD_GET_DZM()	NSD_GET_DEBUG_ZONE_MASK()
#define NSD_SET_DZM(x)	NSD_SET_DEBUG_ZONE_MASK(x)
#define NSD_ADD_DZM(x)	NSD_ADD_DEBUG_ZONE_MASK(x)
#define NSD_DEL_DZM(x)	NSD_DEL_DEBUG_ZONE_MASK(x)

/** Debug MACROs with debug zone.
 * Use these MACROs if you want to use some debug message which can be
 * controlled with the debug zone.
 * For each source file (.c or .cpp), define the file's debug zone with
 * NSD_FILE_ZONE_MASK. For example, if the source file is related with the 
 * memory setup and initialization, then define as follows.
 * #define NSD_FILE_ZONE_MASK (eNSD_DZM_INIT | eNSD_DZM_MEMORY)
 * Then, use NSD_XXZ inside the code. Every debug zone MACRO ends with 'Z'.
 */
#define NSD_PMZ(msg)	\
	NSD_PMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK)), msg)
#define NSD_CMZ(msg)	\
	NSD_CMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK)), msg)
#define NSD_EMZ(msg)	\
	NSD_EMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK)), msg)
#define NSD_SMZ(msg)	\
	NSD_SMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK)), msg)
#define NSD_IMZ(msg)	\
	NSD_IMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK)), msg)
#define NSD_DMZ(msg)	\
	NSD_DMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK)), msg)
#define NSD_VMZ(msg)	\
	NSD_VMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK)), msg)

#define NSD_PMZC(cond, msg)	\
	NSD_PMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK) && (cond)), msg)
#define NSD_CMZC(cond, msg)	\
	NSD_CMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK) && (cond)), msg)
#define NSD_EMZC(cond, msg)	\
	NSD_EMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK) && (cond)), msg)
#define NSD_SMZC(cond, msg)	\
	NSD_SMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK) && (cond)), msg)
#define NSD_IMZC(cond, msg)	\
	NSD_IMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK) && (cond)), msg)
#define NSD_DMZC(cond, msg)	\
	NSD_DMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK) && (cond)), msg)
#define NSD_VMZC(cond, msg)	\
	NSD_VMC(((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK) && (cond)), msg)

#define NSD_ASZ(cond, msg)	\
	do	{	\
		if (!((NSD_GET_DEBUG_ZONE_MASK() & NSD_FILE_ZONE_MASK)))	break;	\
		NSD_ASC(cond, msg);	\
	} while (0)

/// print out buffer
#define NSD_PRINT_BOUNDARY		16
#define NSD_PRINT_BOUNDARY_MASK	(NSD_PRINT_BOUNDARY - 1)
#define NSD_DUMP_BUFFER(pbuf, length)                               \
{                                                                   \
	unsigned int nBufferLeft = 0;                                         \
	char* pTmpBuf = (char*) pbuf;                                 \
																	\
	for (nBufferLeft = (length); nBufferLeft > 0; nBufferLeft--)    \
	{                                                               \
		if (0 == (nBufferLeft & NSD_PRINT_BOUNDARY_MASK))           \
		{                                                           \
			_NSD_MY_PRINTF(TEXT("\r\n"));                           \
		}                                                           \
		_NSD_MY_PRINTF(TEXT("0x%02x "), *pTmpBuf++);                \
	}                                                               \
}

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	// Return last pointer of tCh
	extern PTCHAR _NsdGetStrRchrT(PTSTR ptszString, TCHAR tCh);
	// Return current debug zone mask
	extern unsigned int _NsdGetDebugZoneMask(void);
	// Set new debug zone mask
	extern unsigned int _NsdSetDebugZoneMask(unsigned int nNewDebugZoneMask);
	// Enter endless loop
	extern void _NsdEnterEndlessLoop(void);
	// Enter bebug break
	extern void _NsdEnterDebugBreak(void);
	// Return current time in second
	extern unsigned int _NsdGetTimeSec(void);
	// Return latest error number
	extern NSD_ERROR_TYPE _NsdGetLastError(void);

#if (ENABLE_MULTI_THREAD >= 1)
	// Get console lock
	extern void _NsdLockConsole(void);
	// Release console lock
	extern void _NsdUnlockConsole(void);
	// Return thread id
	extern THREAD_ID_TYPE _NsdGetThreadId(void);
#endif // end of #if (ENABLE_MULTI_THREAD >= 1)

    extern const wchar_t* _NsdMbsToWcsC(const char* pszString);
    extern wchar_t* _NsdMbsToWcs(const char* pszString);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // end of #ifndef __NSD_DEBUG_H__
