/**
*   @section Intro Intro
*	This project is to provide a programming base so that we can use it as a 
*	starting point of programming.
*    
*	@section  Copyright    Copyright
*            COPYRIGHT. 2008 SAMSUNG ELECTRONICS CO., LTD.                
*                            ALL RIGHTS RESERVED                              
*                                                                            
*	Permission is hereby granted to licensees of Samsung Electronics Co., Ltd. 
*	products to use or abstract this computer program only in  accordance with 
*	the terms of the NAND FLASH MEMORY SOFTWARE LICENSE AGREEMENT for the sole 
*	purpose of implementing  a product based  on Samsung Electronics Co., Ltd. 
*	products. No other rights to  reproduce, use, or disseminate this computer 
*	program, whether in part or in whole, are granted.                         
*                                                                            
*	Samsung Electronics Co., Ltd. makes no  representation or warranties  with 
*	respect to  the performance  of  this computer  program,  and specifically 
*	disclaims any  responsibility for  any damages,  special or consequential, 
*	connected with the use of this program.                                    
*                                                                            
*	@section Description Description
*	This project is an experimental project to develop a programming base for 
*	all projects in Flash S/W Group of Samsung Electronics.
*	We will define the basic debugging MACROs, version information, and types.
*
*/

/** 
* @file		btfs_debug.h
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
*				2. Protect the debug MACROs with _BTFS_LOCK_CONSOLE() and
*					_BTFS_UNLOCK_CONSOLE().
*				3. Add conditional message MACROs. The original MACROs are 
*					changed to non-conditional MACROs.
*				4. For assert check, BTFS_AS() (= BTFS_ASSERT()) and BTFS_ASZ() MACROs are added.
*/

#ifndef __BTFS_DEBUG_H__
#define __BTFS_DEBUG_H__

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
#ifndef __FUNCTION__
	#if defined(_MSC_VER) && (_MSC_VER < 1300)
		// Visual Studio 6.0 or under
		#define __FUNCTION__	"Unknown Func"
	#endif

	#if defined(__arm)
		#undef __FUNCTION__
		#define __FUNCTION__ __func__	// for ARM compiler
	#endif
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

///< CONF: you need to set _MY_BTFS_PRINTF here
#if (ENABLE_UNICODE >= 1) && (ENABLE_NUCLEUS == 0)
	#if defined(__CYGWIN__) || defined(__arm)
		// gcc or gcc in CYGWIN does not support L"__FUNCTION__"
		// armcc of ads1.2 and rvds2.x do not support L"__FUNCTION__"
		#define __TFILE__		_NsdMbsToWcs(__FILE__)
		#define __TFUNCTION__	_NsdMbsToWcsC(__FUNCTION__)
	#else
		#define __TFILE__		__WFILE__		///< define FILE name macro
		#define __TFUNCTION__	__WFUNCTION__	///< define FUNCTION macro
	#endif

	#define _T(x)			T2W(x)
	#define _BTFS_MY_PRINTF	OamPrintf			///< define printf function
#else
	#define _T(x)			x
	#define __TFILE__		__FILE__
	#define __TFUNCTION__	__FUNCTION__
	#define _BTFS_MY_PRINTF	OamPrintf			///< define printf function

	#if (ENABLE_LINUX_KERNEL >= 1)
		#undef _BTFS_MY_PRINTF
		#define _BTFS_MY_PRINTF	printk
	#endif
#endif

/** Define a clock function which returns the current time in second.
 *  Each message will print the current time so that you can refer
 *  how much time has passed since the start.
 *  CONF: define a time function which returns the current time in second.
 */
#define _BTFS_GET_TIME_SEC()	_NsdGetTimeSec()

/** Define a function which returns latest error number.
 *  Each message will print error number.
 *  CONF: define a error number returning function.
 */
#define _BTFS_GET_LAST_ERROR()	_NsdGetLastError()

/** Enable or disable console lock. If you use the debug MACROs with
 *  multi-thread env.,  you need to protect the debug MACRO with semaphore
 *  so that the messages are not mixed.
 *  CONF: you need to define your own _NsdLockConsole() and _NsdUnlockConsole()
 */
#if (ENABLE_MULTI_THREAD >= 1)
	#define _BTFS_LOCK_CONSOLE()	_NsdLockConsole()
	#define _BTFS_UNLOCK_CONSOLE()	_NsdUnlockConsole()
	#define _BTFS_GET_THREAD_ID()	_NsdGetThreadId()
#else
	#define _BTFS_LOCK_CONSOLE()
	#define _BTFS_UNLOCK_CONSOLE()
	#define _BTFS_GET_THREAD_ID()
#endif

#if (ENABLE_LINUX_KERNEL >= 1) && defined(RFS_FS_NATIVE_DEBUG_LEVEL)
	#define BTFS_DEBUG_LEVEL	RFS_FS_NATIVE_DEBUG_LEVEL
#elif (ENABLE_WINCE >= 1) && defined(PFS_FS_NATIVE_DEBUG_LEVEL)
	#define BTFS_DEBUG_LEVEL	PFS_FS_NATIVE_DEBUG_LEVEL
#else
	///< CONF: Change the DEBUG_LEVEL as you like.
	#define BTFS_DEBUG_LEVEL	1		///< set debug level
#endif

#if (BTFS_DEBUG_LEVEL <= 1) && (ENABLE_RFS_TOOLS >= 1)
	#undef BTFS_DEBUG_LEVEL
	#define BTFS_DEBUG_LEVEL	2		///< set debug level for rfs Tool
#endif

	#define BTFS_DL_PANIC		0		///< panic error (don't know the reason), system is halted, always printed
	#define BTFS_DL_CRITICAL	1		///< critical error (know the reason), system is not halted, always printed
	#define BTFS_DL_ERROR		2		///< error message of your package
	#define BTFS_DL_SYSTEM		3		///< system message
	#define BTFS_DL_INFO		4		///< information message
	#define BTFS_DL_DEBUG		5		///< debugging message
	#define BTFS_DL_VERBOSE		6		///< more information about debugging
	#define BTFS_DL_ASSERT		0		///< assert debug level

	///< CONF: enable or disable each debug level separately
	#define BTFS_DL_PANIC_ENABLE	1		///< enable PANIC debug message
	#define BTFS_DL_CRITICAL_ENABLE	1		///< enable CRITICAL debug message
	#define BTFS_DL_ERROR_ENABLE	1		///< enable ERROR debug message
	#define BTFS_DL_SYSTEM_ENABLE	0		///< enable SYSTEM debug message
	#define BTFS_DL_INFO_ENABLE		0		///< enable INFO debug message
	#define BTFS_DL_DEBUG_ENABLE	1		///< enable DEBUG debug message
	#define BTFS_DL_VERBOSE_ENABLE	1		///< enable VERBOSE debug message
	#define BTFS_DL_ASSERT_ENABLE	1		///< enable ASSERT debug message

// debug begin
	// just for kkaka test
	#ifdef BTFS_DETAILED_LOG
		#undef BTFS_DEBUG_LEVEL
		#define BTFS_DEBUG_LEVEL			BTFS_DL_VERBOSE
	#endif
// debug end

///< Define settings for the release mode
#if defined(NSD_RELEASE)
	// CONF: Setup the release mode settings. The default is to off all debug settings.
	#undef BTFS_DL_PANIC_ENABLE
	#undef BTFS_DL_CRITICAL_ENABLE
	#undef BTFS_DL_ERROR_ENABLE
	#undef BTFS_DL_SYSTEM_ENABLE
	#undef BTFS_DL_INFO_ENABLE
	#undef BTFS_DL_DEBUG_ENABLE
	#undef BTFS_DL_VERBOSE_ENABLE
	#undef BTFS_DL_ASSERT_ENABLE

	#define BTFS_DL_PANIC_ENABLE	1
	#define BTFS_DL_CRITICAL_ENABLE	1
	#define BTFS_DL_ERROR_ENABLE	0
	#define BTFS_DL_SYSTEM_ENABLE	0
	#define BTFS_DL_INFO_ENABLE		0
	#define BTFS_DL_DEBUG_ENABLE	0
	#define BTFS_DL_VERBOSE_ENABLE	0
	#define BTFS_DL_ASSERT_ENABLE	0
#endif

#define _BTFS_DL_DEBUG_STR_TEMPLATE(x)	\
	TEXT("[") TEXT(BTFS_NAME_STRING) TEXT("]") TEXT(x)	///< debug string template

#define BTFS_DL_PANIC_STR		_BTFS_DL_DEBUG_STR_TEMPLATE("[PAN]")	///< Panic error
#define BTFS_DL_CRITICAL_STR	_BTFS_DL_DEBUG_STR_TEMPLATE("[CRT]")	///< Critical error
#define BTFS_DL_ERROR_STR		_BTFS_DL_DEBUG_STR_TEMPLATE("[ERR]")	///< Error, but not critical
#define BTFS_DL_SYSTEM_STR		_BTFS_DL_DEBUG_STR_TEMPLATE("[SYS]")	///< System message
#define BTFS_DL_INFO_STR		_BTFS_DL_DEBUG_STR_TEMPLATE("[INF]")	///< Information message
#define BTFS_DL_DEBUG_STR		_BTFS_DL_DEBUG_STR_TEMPLATE("[DBG]")	///< Debug information
#define BTFS_DL_VERBOSE_STR		_BTFS_DL_DEBUG_STR_TEMPLATE("[VER]")	///< For more information about debugging
#define BTFS_DL_ASSERT_STR		_BTFS_DL_DEBUG_STR_TEMPLATE("[AST]")	///< For ASSERT message

/**
 * Debug message macros
 */
#define _MAMMOTH_STOP_STRING	\
	TEXT("\r\n<log P1=\"100\" P2=\"CRITICAL ERROR OCCURRED. CHECK IT!!!\" >\r\n")

///< print out the stop message of Mammoth SQA system
#define _BTFS_PRINT_MAMMOTH_STOP()	\
	do	\
	{	\
		_BTFS_MY_PRINTF(_MAMMOTH_STOP_STRING);	\
	} while (0)

///< print current code line information
#if (ENABLE_UNIX >= 1) || (ENABLE_RFS_TOOLS >= 1)
	#define _BTFS_TRIM_PARENT_DIRECTORY(x)	(x)
#else
	#define _BTFS_TRIM_PARENT_DIRECTORY(x)	_NsdGetStrRchrT((x), TIFS)
#endif

#if (ENABLE_MESSAGE_THREAD_ID >= 1)
	#define _BTFS_PRINT_THREAD_ID()	\
		_BTFS_MY_PRINTF(TEXT("[Tid:0x%05x]"),	_BTFS_GET_THREAD_ID())
#else
	#define _BTFS_PRINT_THREAD_ID()
#endif

#if (ENABLE_MESSAGE_TIME >= 1)
	#define _BTFS_PRINT_TIME()	\
		_BTFS_MY_PRINTF(TEXT("[Tim:%d]"),	_BTFS_GET_TIME_SEC())
#else
	#define _BTFS_PRINT_TIME()
#endif

#if (ENABLE_MESSAGE_ERROR_NUMBER >= 1)
	#define _BTFS_PRINT_ERROR_NUMBER()	\
		_BTFS_MY_PRINTF(TEXT("[Err:%05d]"),	_BTFS_GET_LAST_ERROR())
#else
	#define _BTFS_PRINT_ERROR_NUMBER()
#endif

#define _BTFS_PRINT_CODE_LINE()	\
	do	\
	{	\
		_BTFS_PRINT_TIME();	\
		_BTFS_PRINT_THREAD_ID();	\
		_BTFS_PRINT_ERROR_NUMBER();	\
		_BTFS_MY_PRINTF(TEXT("[%s(#%d)@%s]: "), \
			__TFUNCTION__,	\
			__LINE__,	\
			_BTFS_TRIM_PARENT_DIRECTORY(__TFILE__)	\
		); \
	} while (0)

///< print panic message. system is halted
#if (BTFS_DL_PANIC <= BTFS_DEBUG_LEVEL) && (BTFS_DL_PANIC_ENABLE >= 1)
	#define BTFS_D0(msg)	\
		do	\
		{	\
			_BTFS_LOCK_CONSOLE();	\
				_BTFS_MY_PRINTF(BTFS_DL_PANIC_STR);	\
				_BTFS_PRINT_CODE_LINE();	\
				_BTFS_MY_PRINTF msg ;	\
				_BTFS_MY_PRINTF(_T("\r\n"));	\
			_BTFS_UNLOCK_CONSOLE();	\
			\
			_NsdEnterEndlessLoop();	\
		} while (0)

	#define BTFS_D0C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			BTFS_D0(msg);	\
		} while (0)
#else
	#define BTFS_D0(msg)
	#define BTFS_D0C(cond, msg)
#endif

///< print critical error message. system is not halted
#if (BTFS_DL_CRITICAL <= BTFS_DEBUG_LEVEL) && (BTFS_DL_CRITICAL_ENABLE >= 1)
	#define BTFS_D1(msg)	\
		do	\
		{	\
			_BTFS_LOCK_CONSOLE();	\
				_BTFS_MY_PRINTF(BTFS_DL_CRITICAL_STR);	\
				_BTFS_PRINT_CODE_LINE();	\
				_BTFS_MY_PRINTF msg ;	\
				_BTFS_MY_PRINTF(_T("\r\n"));	\
			_BTFS_UNLOCK_CONSOLE();	\
		} while (0)

	#define BTFS_D1C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			BTFS_D1(msg);	\
		} while (0)
#else
	#define BTFS_D1(msg)
	#define BTFS_D1C(cond, msg)
#endif

///< print error message
#if (BTFS_DL_ERROR <= BTFS_DEBUG_LEVEL) && (BTFS_DL_ERROR_ENABLE >= 1)
	#define BTFS_D2(msg)	\
		do	\
		{	\
			_BTFS_LOCK_CONSOLE();	\
				_BTFS_MY_PRINTF(BTFS_DL_ERROR_STR);	\
				_BTFS_PRINT_CODE_LINE();	\
				_BTFS_MY_PRINTF msg ;	\
				_BTFS_MY_PRINTF(_T("\r\n"));	\
			_BTFS_UNLOCK_CONSOLE();	\
		} while (0)

	#define BTFS_D2C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			BTFS_D2(msg);	\
		} while (0)
#else
	#define BTFS_D2(msg)
	#define BTFS_D2C(cond, msg)
#endif

///< print system (error) message
#if (BTFS_DL_SYSTEM <= BTFS_DEBUG_LEVEL) && (BTFS_DL_SYSTEM_ENABLE >= 1)
	#define BTFS_D3(msg)	\
		do	\
		{	\
			_BTFS_LOCK_CONSOLE();	\
				_BTFS_MY_PRINTF(BTFS_DL_SYSTEM_STR);	\
				_BTFS_PRINT_CODE_LINE();	\
				_BTFS_MY_PRINTF msg ;	\
				_BTFS_MY_PRINTF(_T("\r\n"));	\
			_BTFS_UNLOCK_CONSOLE();	\
		} while (0)

	#define BTFS_D3C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			BTFS_D3(msg);	\
		} while (0)
#else
	#define BTFS_D3(msg)
	#define BTFS_D3C(cond, msg)
#endif

///< print normal information message
#if (BTFS_DL_INFO <= BTFS_DEBUG_LEVEL) && (BTFS_DL_INFO_ENABLE >= 1)
	#define BTFS_D4(msg)	\
		do	\
		{	\
			_BTFS_LOCK_CONSOLE();	\
				_BTFS_MY_PRINTF(BTFS_DL_INFO_STR);	\
				_BTFS_PRINT_CODE_LINE();	\
				_BTFS_MY_PRINTF msg ;	\
				_BTFS_MY_PRINTF(_T("\r\n"));	\
			_BTFS_UNLOCK_CONSOLE();	\
		} while (0)

	#define BTFS_D4C(cond, msg)						\
						do							\
						{							\
							if (!(cond))	break;	\
							BTFS_D4(msg);			\
						} while (0)
#else
	#define BTFS_D4(msg)
	#define BTFS_D4C(cond, msg)
#endif

///< print debug message
#if (BTFS_DL_DEBUG <= BTFS_DEBUG_LEVEL) && (BTFS_DL_DEBUG_ENABLE >= 1)
	#define BTFS_D5(msg)	\
		do	\
		{	\
			_BTFS_LOCK_CONSOLE();	\
				_BTFS_MY_PRINTF(BTFS_DL_DEBUG_STR);	\
				_BTFS_PRINT_CODE_LINE();	\
				_BTFS_MY_PRINTF msg ;	\
				_BTFS_MY_PRINTF(_T("\r\n"));	\
			_BTFS_UNLOCK_CONSOLE();	\
		} while (0)

	#define BTFS_D5C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			BTFS_D5(msg);	\
		} while (0)
#else
	#define BTFS_D5(msg)
	#define BTFS_D5C(cond, msg)
#endif

///< print more information about debugging
#if (BTFS_DL_VERBOSE <= BTFS_DEBUG_LEVEL) && (BTFS_DL_VERBOSE_ENABLE >= 1)
	#define BTFS_D6(msg)	\
	do	\
		{	\
			_BTFS_LOCK_CONSOLE();	\
				_BTFS_MY_PRINTF(BTFS_DL_VERBOSE_STR);	\
				_BTFS_MY_PRINTF msg ;	\
				_BTFS_MY_PRINTF(_T("\r\n"));	\
			_BTFS_UNLOCK_CONSOLE();	\
		} while (0)

	#define BTFS_D6C(cond, msg)	\
	do	\
		{	\
			if (!(cond))	break;	\
			BTFS_D6(msg);	\
		} while (0)
#else
	#define BTFS_D6(msg)
	#define BTFS_D6C(cond, msg)
#endif

///< print more information about debugging
#if (BTFS_DL_ASSERT <= BTFS_DEBUG_LEVEL) && (BTFS_DL_ASSERT_ENABLE >= 1)
	#define BTFS_D7(msg)												\
					do													\
						{												\
							_BTFS_LOCK_CONSOLE();						\
								_BTFS_MY_PRINTF(BTFS_DL_ASSERT_STR);	\
								_BTFS_PRINT_CODE_LINE();				\
								_BTFS_MY_PRINTF msg ;					\
								_BTFS_MY_PRINTF(_T("\r\n"));			\
							_BTFS_UNLOCK_CONSOLE();						\
																		\
							_NsdEnterEndlessLoop();						\
						} while (0)

	#define BTFS_D7C(cond, msg)	\
	do	\
		{	\
			if (cond)	break;	\
			BTFS_D7(msg);	\
		} while (0)

	#define BTFS_A7(cond)												\
					do													\
						{												\
							if (cond)	break;							\
							_BTFS_LOCK_CONSOLE();						\
								_BTFS_MY_PRINTF(BTFS_DL_ASSERT_STR);	\
								_BTFS_PRINT_CODE_LINE();				\
								_BTFS_MY_PRINTF(_T("\r\n"));			\
							_BTFS_UNLOCK_CONSOLE();						\
																		\
							_NsdEnterDebugBreak();						\
						} while (0)

#else
	#define BTFS_D7(msg)
	#define BTFS_D7C(cond, msg)
	#define BTFS_A7(cond)
#endif

/** Debug MACRO aliases #1.
 * Use these MACROs instead of BTFS_D#.
 * For example, you can use BTFS_DPANIC instead of BTFS_D0. 
 */
#define	BTFS_PANIC	BTFS_D0		///< Print panic message
#define BTFS_CRIT	BTFS_D1		///< Print critical message
#define BTFS_ERROR	BTFS_D2		///< Print error message
#define	BTFS_SYST	BTFS_D3		///< Print system message
#define BTFS_INFO	BTFS_D4		///< Print information message
#define BTFS_DEBUG	BTFS_D5		///< Print debug message
#define BTFS_VERB	BTFS_D6		///< Print verbose message

/** Debug MACRO aliases #2.
 * Use these MACROs instead of BTFS_D#.
 * For example, you can use BTFS_PM instead of BTFS_D0. 
 * PM means 'P'anic 'M'essage.
 */
#define	BTFS_PM		BTFS_D0		///< Print panic message
#define BTFS_CM		BTFS_D1		///< Print critical message
#define BTFS_EM		BTFS_D2		///< Prit error message
#define	BTFS_SM		BTFS_D3		///< Print system message
#define BTFS_IM		BTFS_D4		///< Print information message
#define BTFS_DM		BTFS_D5		///< Print debug message
#define BTFS_VM		BTFS_D6		///< Print verbose message

/** Debug MACRO aliases #3.
 * Use these MACROs if condition check is needed.
 * 'C' is added to the above MACRO alias. For example,
 * BTFS_PM with condition check MACRO is BTFS_PMC.
 */
#define	BTFS_PMC		BTFS_D0C	///< Print panic message with condition check
#define BTFS_CMC		BTFS_D1C	///< Print critical message with condition check
#define BTFS_EMC		BTFS_D2C	///< Print error message with condition check
#define	BTFS_SMC		BTFS_D3C	///< Print system message with condition check
#define BTFS_IMC		BTFS_D4C	///< Print information message with condition check
#define BTFS_DMC		BTFS_D5C	///< Print debug message with condition check
#define BTFS_VMC		BTFS_D6C	///< Print verbose message with condition check
#define BTFS_ASC		BTFS_D7C	///< Print assert  message with condition check

#define BTFS_AS			BTFS_A7		///< Assert MACRO
#define BTFS_ASSERT		BTFS_AS		///< Assert MACRO

/** Define debug zones here.
 * You can define several debug zones and enable and disable the debugging
 * messages according to the debug zones. Maximum 31 zones are supported.
 */
// CONF: Add your own Debug Zone Mask
enum {
	///< Add your own debug zone mask here
	///< Don't use 0x80000000
	eBTFS_DZM_DEFAULT	= 0x00000000U,	///< Default debug zone mask

	// CORE
	eBTFS_DZM_CORE_API			= 0x00000001U,	///< Debug Zone for CORE_API
	eBTFS_DZM_CORE_DIR			= 0x00000002U,	///< Debug Zone for CORE_DIR
	eBTFS_DZM_CORE_FILE			= 0x00000004U,	///< Debug Zone for CORE_FILE
	eBTFS_DZM_CORE_MAIN			= 0x00000008U,	///< Debug Zone for CORE_MAIN
	eBTFS_DZM_CORE_MISC			= 0x00000010U,	///< Debug Zone for CORE_MISC
	eBTFS_DZM_CORE_NODE			= 0x00000020U,	///< Debug Zone for CORE_NODE
	eBTFS_DZM_CORE_SHARE		= 0x00000040U,	///< Debug Zone for CORE_SHARE
	eBTFS_DZM_CORE_VOL			= 0x00000080U,	///< Debug Zone for CORE_VOL

	// FFATFS
	eBTFS_DZM_FFATFS_API		= 0x00000100U,	///< Debug Zone for FFATFS_API
	eBTFS_DZM_FFATFS_BS			= 0x00000200U,	///< Debug Zone for FFATFS_BS
	eBTFS_DZM_FFATFS_CACHE		= 0x00000400U,	///< Debug Zone for FFATFS_CACHE
	eBTFS_DZM_FFATFS_DE			= 0x00000800U,	///< Debug Zone for FFATFS_DE
	eBTFS_DZM_FFATFS_FAT		= 0x00001000U,	///< Debug Zone for FFATFS_MAIN
	eBTFS_DZM_FFATFS_MAIN		= 0x00002000U,	///< Debug Zone for FFATFS_MAIN
	eBTFS_DZM_FFATFS_MISC		= 0x00004000U,	///< Debug Zone for FFATFS_MISC

	// ADDON
	eBTFS_DZM_ADDON_API			= 0x00008000U,	///< Debug Zone for ADDON_API
	eBTFS_DZM_ADDON_CHKDSK		= 0x00010000U,	///< Debug Zone for ADDON_CHKDSK
	eBTFS_DZM_ADDON_DEC			= 0x00020000U,	///< Debug Zone for ADDON_DEC
	eBTFS_DZM_ADDON_FASTSEEK	= 0x00040000U,	///< Debug Zone for ADDON_FASTSEEK
	eBTFS_DZM_ADDON_FCC			= 0x00080000U,	///< Debug Zone for ADDON_FCC
	eBTFS_DZM_ADDON_FORMAT		= 0x00100000U,	///< Debug Zone for ADDON_FORMAT
	eBTFS_DZM_ADDON_HPA			= 0x00200000U,	///< Debug Zone for ADDON_HPA
	eBTFS_DZM_ADDON_LOG			= 0x00400000U,	///< Debug Zone for ADDON_LOG
	eBTFS_DZM_ADDON_MISC		= 0x00800000U,	///< Debug Zone for ADDON_MISC
	eBTFS_DZM_ADDON_NAND		= 0x01000000U,	///< Debug Zone for ADDON_NAND
	eBTFS_DZM_ADDON_SYMLINK		= 0x02000000U,	///< Debug Zone for ADDON_SYMLINK
	eBTFS_DZM_ADDON_XATTR		= 0x04000000U,	///< Debug Zone for ADDON_XATTR
	eBTFS_DZM_ADDON_XDE			= 0x0800000U,	///< Debug Zone for ADDON_XDE

	// Global
	eBTFS_DZM_GLOBAL_COMMON		= 0x1000000U,	///< Debug Zone for GLOBAL_COMMON

	// Vfsal
	eBTFS_DZM_VFSAL_NESTLE		= 0x2000000U,	///< Debug Zone for VFSAL_NESTLE
	eBTFS_DZM_VFSAL_BC			= 0x4000000U,	///< Debug Zone for VFSAL_BC

	///< Add your own debug zone mask here
	///< Don't use 0x80000000
	eBTFS_DZM_ALL				= 0x7FFFFFFFU	///< Enable all debug zone mask
};

/** Set default debug zone mask here.
 * Define the default (initial) debug zone to enable.
 */
/// Set default debug zone mask here
#define BTFS_DZM_CORE		(eBTFS_DZM_CORE_API | eBTFS_DZM_CORE_DIR | eBTFS_DZM_CORE_FILE |	\
							eBTFS_DZM_CORE_MAIN | eBTFS_DZM_CORE_MISC | eBTFS_DZM_CORE_NODE|	\
							eBTFS_DZM_CORE_SHARE | eBTFS_DZM_CORE_VOL)
#define BTFS_DZM_FFATFS		(eBTFS_DZM_FFATFS_API | eBTFS_DZM_FFATFS_BS | eBTFS_DZM_FFATFS_CACHE |	\
							eBTFS_DZM_FFATFS_DE | eBTFS_DZM_FFATFS_FAT | eBTFS_DZM_FFATFS_MAIN |	\
							eBTFS_DZM_FFATFS_MISC)
#define BTFS_DZM_ADDON		(eBTFS_DZM_ADDON_API | eBTFS_DZM_ADDON_CHKDSK | eBTFS_DZM_ADDON_DEC |		\
							eBTFS_DZM_ADDON_FASTSEEK | eBTFS_DZM_ADDON_FCC | eBTFS_DZM_ADDON_FORMAT |	\
							eBTFS_DZM_ADDON_HPA | eBTFS_DZM_ADDON_LOG | eBTFS_DZM_ADDON_MISC |			\
							eBTFS_DZM_ADDON_NAND | eBTFS_DZM_ADDON_SYMLINK | eBTFS_DZM_ADDON_XATTR |	\
							eBTFS_DZM_ADDON_XDE)
#define BTFS_DZM_GLOBAL		(eBTFS_DZM_GLOBAL_COMMON)
#define BTFS_DZM_VFSAL		(eBTFS_DZM_VFSAL_NESTLE | eBTFS_DZM_VFSAL_BC)

#define BTFS_GET_DEBUG_ZONE_MASK()	_BtfsGetDebugZoneMask()	///< Get debug mask
#define BTFS_SET_DEBUG_ZONE_MASK(x)	_BtfsSetDebugZoneMask(x)	///< Set debug mask

/// Add more debug zone to the debug zone mask
#define BTFS_ADD_DEBUG_ZONE_MASK(x)	\
	do	\
	{	\
		_BtfsGetDebugZoneMask(_BtfsGetDebugZoneMask() | x);	\
	} while (0)

/// Remove some debug zone from the debug zone mask
#define BTFS_DEL_DEBUG_ZONE_MASK(x)	\
	do	\
	{	\
		_BtfsSetDebugZoneMask(_BtfsGetDebugZoneMask() & (~x));	\
	} while (0)

/** Manage debug zone mask with several MACROs.
 * BTFS_GET_DZM() returns current debug zone mask.
 * BTFS_SET_DZM(x) updates the debug zone mask with x.
 * BTFS_ADD_DZM(x) adds new debug zone 'x' to the global debug zone mask.
 * BTFS_DEL_DZM(x) deletes some debug zone 'x" from the global debug zone mask.
 */
#define BTFS_GET_DZM()	BTFS_GET_DEBUG_ZONE_MASK()
#define BTFS_SET_DZM(x)	BTFS_SET_DEBUG_ZONE_MASK(x)
#define BTFS_ADD_DZM(x)	BTFS_ADD_DEBUG_ZONE_MASK(x)
#define BTFS_DEL_DZM(x)	BTFS_DEL_DEBUG_ZONE_MASK(x)

/** Debug MACROs with debug zone.
 * Use these MACROs if you want to use some debug message which can be
 * controlled with the debug zone.
 * For each source file (.c or .cpp), define the file's debug zone with
 * BTFS_FILE_ZONE_MASK. For example, if the source file is related with the 
 * memory setup and initialization, then define as follows.
 * #define BTFS_FILE_ZONE_MASK (eBTFS_DZM_INIT | eBTFS_DZM_MEMORY)
 * Then, use BTFS_XXZ inside the code. Every debug zone MACRO ends with 'Z'.
 */
#define BTFS_PMZ(msg)	\
	BTFS_PMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK)), msg)
#define BTFS_CMZ(msg)	\
	BTFS_CMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK)), msg)
#define BTFS_EMZ(msg)	\
	BTFS_EMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK)), msg)
#define BTFS_SMZ(msg)	\
	BTFS_SMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK)), msg)
#define BTFS_IMZ(msg)	\
	BTFS_IMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK)), msg)
#define BTFS_DMZ(msg)	\
	BTFS_DMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK)), msg)
#define BTFS_VMZ(msg)	\
	BTFS_VMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK)), msg)

#define BTFS_PMZC(cond, msg)	\
	BTFS_PMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK) && (cond)), msg)
#define BTFS_CMZC(cond, msg)	\
	BTFS_CMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK) && (cond)), msg)
#define BTFS_EMZC(cond, msg)	\
	BTFS_EMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK) && (cond)), msg)
#define BTFS_SMZC(cond, msg)	\
	BTFS_SMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK) && (cond)), msg)
#define BTFS_IMZC(cond, msg)	\
	BTFS_IMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK) && (cond)), msg)
#define BTFS_DMZC(cond, msg)	\
	BTFS_DMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK) && (cond)), msg)
#define BTFS_VMZC(cond, msg)	\
	BTFS_VMC(((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK) && (cond)), msg)

#define BTFS_ASZ(cond, msg)	\
	do	{	\
		if (!((BTFS_GET_DEBUG_ZONE_MASK() & BTFS_FILE_ZONE_MASK)))	break;	\
		BTFS_ASC(cond, msg);	\
	} while (0)

/// print out buffer
#define BTFS_PRINT_BOUNDARY		16
#define BTFS_PRINT_BOUNDARY_MASK	(BTFS_PRINT_BOUNDARY - 1)
#define BTFS_DUMP_BUFFER(pbuf, length)                               \
{                                                                   \
	unsigned int nBufferLeft = 0;                                         \
	unsigned char* pTmpBuf = (unsigned char*) pbuf;                                 \
																	\
	for (nBufferLeft = (length); nBufferLeft > 0; nBufferLeft--)    \
	{                                                               \
		if (0 == (nBufferLeft & BTFS_PRINT_BOUNDARY_MASK))           \
		{                                                           \
			_BTFS_MY_PRINTF(TEXT("\r\n"));                           \
		}                                                           \
		_BTFS_MY_PRINTF(TEXT("0x%02x "), *pTmpBuf++);                \
	}                                                               \
}

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	// Return last pointer of tCh
	extern PTCHAR _NsdGetStrRchrT(PTSTR ptszString, TCHAR tCh);
	// Return current debug zone mask
	extern unsigned int _BtfsGetDebugZoneMask(void);
	// Set new debug zone mask
	extern unsigned int _BtfsSetDebugZoneMask(unsigned int nNewDebugZoneMask);
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

#endif // end of #ifndef __BTFS_DEBUG_H__
