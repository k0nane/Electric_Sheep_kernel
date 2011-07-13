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
*				1. Change _GetRFSTimeSec() with _RFSGetTimeSec().
*				2. Protect the debug MACROs with _RFS_LOCK_CONSOLE() and
*					_RFS_UNLOCK_CONSOLE().
*				3. Add conditional message MACROs. The original MACROs are 
*					changed to non-conditional MACROs.
*				4. For assert check, RFS_AS() (= RFS_ASSERT()) and RFS_ASZ() MACROs are added.
*/

#ifndef __RFS_DEBUG_H__
	#define __RFS_DEBUG_H__

#include "nsd_types.h"

#define RFS_NAME_STRING "RFS"

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
#if defined(__arm)
	#undef __FUNCTION__
	#define __FUNCTION__ __func__	// for ARM compiler
#endif

// MACRO from Multi-byte string to TCHAR string
#if defined(_T)
	#undef _T
#endif

///< CONF: you need to set _MY_RFS_PRINTF here
//#if (ENABLE_UNICODE == 0)
	#define _T(x)			x
	#define __TFILE__		__FILE__
	#define __TFUNCTION__	__FUNCTION__
	#define _RFS_MY_PRINTF	printk	///< define printf function
//#endif

/** Define a clock function which returns the current time in second.
 *  Each message will print the current time so that you can refer
 *  how much time has passed since the start.
 *  CONF: define a time function which returns the current time in second.
 */
#define _RFS_GET_TIME_SEC()	_RFSGetTimeSec()

/** Enable or disable console lock. If you use the debug MACROs with
 *  multi-thread env.,  you need to protect the debug MACRO with semaphore
 *  so that the messages are not mixed.
 *  CONF: you need to define your own _RFSLockConsole() and _RFSUnlockConsole()
 */
#if (ENABLE_MULTI_THREAD >= 1)
	#define _RFS_LOCK_CONSOLE()		_RFSLockConsole()
	#define _RFS_UNLOCK_CONSOLE()	_RFSUnlockConsole()
	#define _RFS_GET_THREAD_ID()	_RFSGetThreadId()
#else
	#define _RFS_LOCK_CONSOLE()
	#define _RFS_UNLOCK_CONSOLE()
	#define _RFS_GET_THREAD_ID()
#endif
 
#if (ENABLE_LINUX_KERNEL >= 1) && defined(RFS_FS_GLUE_DEBUG_LEVEL)
	#define RFS_DEBUG_LEVEL		RFS_FS_GLUE_DEBUG_LEVEL
#else
	///< CONF: Change the DEBUG_LEVEL as you like.
	#define RFS_DEBUG_LEVEL		1		///< set debug level
#endif

	#define RFS_DL_PANIC		0		///< panic error (don't know the reason), system is halted, always printed
	#define RFS_DL_CRITICAL		1		///< critical error (know the reason), system is not halted, always printed
	#define RFS_DL_ERROR		2		///< error message of your package
	#define RFS_DL_SYSTEM		3		///< system message
	#define RFS_DL_INFO			4		///< information message
	#define RFS_DL_DEBUG		5		///< debugging message
	#define RFS_DL_VERBOSE		6		///< more information about debugging
	#define RFS_DL_ASSERT		0		///< assert debug level

	///< CONF: enable or disable each debug level separately
	#define RFS_DL_PANIC_ENABLE		1		///< enable PANIC debug message
	#define RFS_DL_CRITICAL_ENABLE	1		///< enable CRITICAL debug message
	#define RFS_DL_ERROR_ENABLE		1		///< enable ERROR debug message
	#define RFS_DL_SYSTEM_ENABLE	1		///< enable SYSTEM debug message
	#define RFS_DL_INFO_ENABLE		1		///< enable INFO debug message
	#define RFS_DL_DEBUG_ENABLE		1		///< enable DEBUG debug message
	#define RFS_DL_VERBOSE_ENABLE	1		///< enable VERBOSE debug message
	#define RFS_DL_ASSERT_ENABLE	1		///< enable ASSERT debug message
 
///< Define settings for the release mode
#if defined(RFS_RELEASE)
	// CONF: Setup the release mode settings. The default is to off all debug settings.
	#undef RFS_DL_PANIC_ENABLE
	#undef RFS_DL_CRITICAL_ENABLE
	#undef RFS_DL_ERROR_ENABLE
	#undef RFS_DL_SYSTEM_ENABLE
	#undef RFS_DL_INFO_ENABLE
	#undef RFS_DL_DEBUG_ENABLE
	#undef RFS_DL_VERBOSE_ENABLE
	#undef RFS_DL_ASSERT_ENABLE

	#define RFS_DL_PANIC_ENABLE		1
	#define RFS_DL_CRITICAL_ENABLE	1
	#define RFS_DL_ERROR_ENABLE		0
	#define RFS_DL_SYSTEM_ENABLE	0
	#define RFS_DL_INFO_ENABLE		0
	#define RFS_DL_DEBUG_ENABLE		0
	#define RFS_DL_VERBOSE_ENABLE	0
	#define RFS_DL_ASSERT_ENABLE	0
#endif

#define _RFS_DL_DEBUG_STR_TEMPLATE(x)	\
	TEXT(RFS_NAME_STRING) TEXT(":") TEXT(x)						///< debug string template

#define RFS_DL_PANIC_STR	_RFS_DL_DEBUG_STR_TEMPLATE("PAN")	///< Panic error
#define RFS_DL_CRITICAL_STR	_RFS_DL_DEBUG_STR_TEMPLATE("CRT")	///< Critical error
#define RFS_DL_ERROR_STR	_RFS_DL_DEBUG_STR_TEMPLATE("ERR")	///< Error, but not critical
#define RFS_DL_SYSTEM_STR	_RFS_DL_DEBUG_STR_TEMPLATE("SYS")	///< System message
#define RFS_DL_INFO_STR		_RFS_DL_DEBUG_STR_TEMPLATE("INF")	///< Information message
#define RFS_DL_DEBUG_STR	_RFS_DL_DEBUG_STR_TEMPLATE("DBG")	///< Debug information
#define RFS_DL_VERBOSE_STR	_RFS_DL_DEBUG_STR_TEMPLATE("VER")	///< For more information about debugging
#define RFS_DL_ASSERT_STR	_RFS_DL_DEBUG_STR_TEMPLATE("AST")	///< For ASSERT message

/**
 * Debug message macros
 */
#define _MAMMOTH_STOP_STRING	\
	TEXT("\r\n<log P1=\"100\" P2=\"CRITICAL ERROR OCCURRED. CHECK IT!!!\" >\r\n")

///< print current code line information
#if (ENABLE_UNIX >= 1) || (ENABLE_RFS_TOOLS >= 1)
	#define _RFS_TRIM_PARENT_DIRECTORY(x)	strrchr(x, TIFS)
#else
	#define _RFS_TRIM_PARENT_DIRECTORY(x)	_RFSGetStrRchrT((x), TIFS)
#endif

#if defined(RFS_RELEASE)
	#define _RFS_DUMP_STACK()
#else
	#define _RFS_DUMP_STACK()	dump_stack()
#endif

///< print panic message. system is halted
///< print out the stop message of Mammoth SQA system
#if (RFS_DL_PANIC <= RFS_DEBUG_LEVEL) && (RFS_DL_PANIC_ENABLE >= 1)
	#define RFS_D0(format, msg...)	\
		do	\
		{	\
			_RFS_LOCK_CONSOLE();	\
			_RFS_MY_PRINTF("[" RFS_DL_PANIC_STR ":%d:%s@%s#%d]" format _MAMMOTH_STOP_STRING "\r\n", \
					_RFS_GET_THREAD_ID(), __TFUNCTION__,	\
					_RFS_TRIM_PARENT_DIRECTORY(__TFILE__),	\
					__LINE__, ##msg); \
			_RFS_UNLOCK_CONSOLE();	\
			dump_stack();\
			_RFSEnterEndlessLoop();	\
		} while (0)

	#define RFS_D0C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			RFS_D0 msg ;	\
		} while (0)
#else
	#define RFS_D0(msg)
	#define RFS_D0C(cond, msg)
#endif

///< print critical error message. system is not halted
#if (RFS_DL_CRITICAL <= RFS_DEBUG_LEVEL) && (RFS_DL_CRITICAL_ENABLE >= 1)
	#define RFS_D1(format, msg...)	\
		do	\
		{	\
			_RFS_LOCK_CONSOLE();	\
			_RFS_MY_PRINTF("[" RFS_DL_CRITICAL_STR ":%d:%s@%s#%d]" format "\r\n", \
					_RFS_GET_THREAD_ID(), __TFUNCTION__,	\
					_RFS_TRIM_PARENT_DIRECTORY(__TFILE__),	\
					__LINE__, ##msg); \
			_RFS_UNLOCK_CONSOLE();	\
		#ifdef CONFIG_ARIES_VER_B2	
			dump_stack();\
		#else
			_RFS_DUMP_STACK();\
		#endif
		} while (0)

	#define RFS_D1C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			RFS_D1 msg ;	\
		} while (0)
#else
	#define RFS_D1(msg)
	#define RFS_D1C(cond, msg)
#endif

///< print error message
#if (RFS_DL_ERROR <= RFS_DEBUG_LEVEL) && (RFS_DL_ERROR_ENABLE >= 1)
	#define RFS_D2(format, msg...)	\
		do	\
		{	\
			_RFS_LOCK_CONSOLE();	\
			_RFS_MY_PRINTF("[" RFS_DL_ERROR_STR ":%d:%s@%s#%d]" format "\r\n", \
					_RFS_GET_THREAD_ID(), __TFUNCTION__,	\
					_RFS_TRIM_PARENT_DIRECTORY(__TFILE__),	\
					__LINE__, ##msg); \
			_RFS_UNLOCK_CONSOLE();	\
		} while (0)

	#define RFS_D2C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			RFS_D2 msg ;	\
		} while (0)
#else
	#define RFS_D2(msg)
	#define RFS_D2C(cond, msg)
#endif

///< print system (error) message
#if (RFS_DL_SYSTEM <= RFS_DEBUG_LEVEL) && (RFS_DL_SYSTEM_ENABLE >= 1)
	#define RFS_D3(format, msg...)	\
		do	\
		{	\
			_RFS_LOCK_CONSOLE();	\
			_RFS_MY_PRINTF("[" RFS_DL_SYSTEM_STR ":%d:%s@%s#%d]" format "\r\n", \
					_RFS_GET_THREAD_ID(), __TFUNCTION__,	\
					_RFS_TRIM_PARENT_DIRECTORY(__TFILE__),	\
					__LINE__, ##msg); \
			_RFS_UNLOCK_CONSOLE();	\
		} while (0)

	#define RFS_D3C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			RFS_D3 msg ;	\
		} while (0)
#else
	#define RFS_D3(msg)
	#define RFS_D3C(cond, msg)
#endif

///< print normal information message
#if (RFS_DL_INFO <= RFS_DEBUG_LEVEL) && (RFS_DL_INFO_ENABLE >= 1)
	#define RFS_D4(format, msg...)	\
		do	\
		{	\
			_RFS_LOCK_CONSOLE();	\
			_RFS_MY_PRINTF("[" RFS_DL_INFO_STR ":%d]" format "\r\n", _RFS_GET_THREAD_ID(), ##msg); \
			_RFS_UNLOCK_CONSOLE();	\
		} while (0)

	#define RFS_D4C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			RFS_D4 msg ;	\
		} while (0)
#else
	#define RFS_D4(msg)
	#define RFS_D4C(cond, msg)
#endif

///< print debug message
#if (RFS_DL_DEBUG <= RFS_DEBUG_LEVEL) && (RFS_DL_DEBUG_ENABLE >= 1)
	#define RFS_D5(format, msg...)	\
		do	\
		{	\
			_RFS_LOCK_CONSOLE();	\
			_RFS_MY_PRINTF("[" RFS_DL_DEBUG_STR ":%d:%s@%s#%d]" format "\r\n", \
					_RFS_GET_THREAD_ID(), __TFUNCTION__,	\
					_RFS_TRIM_PARENT_DIRECTORY(__TFILE__),	\
					__LINE__, ##msg); \
			_RFS_UNLOCK_CONSOLE();	\
		} while (0)

	#define RFS_D5C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			RFS_D5 msg ;	\
		} while (0)
#else
	#define RFS_D5(msg)
	#define RFS_D5C(cond, msg)
#endif

///< print more information about debugging
#if (RFS_DL_VERBOSE <= RFS_DEBUG_LEVEL) && (RFS_DL_VERBOSE_ENABLE >= 1)
	#define RFS_D6(format, msg...)	\
		do	\
		{	\
			_RFS_LOCK_CONSOLE();	\
			_RFS_MY_PRINTF("[" RFS_DL_VERBOSE_STR ":%d:%s@%s#%d]" format "\r\n", \
					_RFS_GET_THREAD_ID(), __TFUNCTION__,	\
					_RFS_TRIM_PARENT_DIRECTORY(__TFILE__),	\
					__LINE__, ##msg); \
			_RFS_UNLOCK_CONSOLE();	\
		} while (0)

	#define RFS_D6C(cond, msg)	\
		do	\
		{	\
			if (!(cond))	break;	\
			RFS_D6 msg ;	\
		} while (0)
#else
	#define RFS_D6(msg)
	#define RFS_D6C(cond, msg)
#endif

///< print more information about debugging
#if (RFS_DL_ASSERT <= RFS_DEBUG_LEVEL) && (RFS_DL_ASSERT_ENABLE >= 1)
	#define RFS_D7(format, msg...)	\
		do	\
		{	\
			_RFS_LOCK_CONSOLE();	\
			_RFS_MY_PRINTF("[" RFS_DL_ASSERT_STR ":%d:%s@%s#%d]" format "\r\n", \
					_RFS_GET_THREAD_ID(), __TFUNCTION__,	\
					_RFS_TRIM_PARENT_DIRECTORY(__TFILE__),	\
					__LINE__, ##msg); \
			_RFS_UNLOCK_CONSOLE();	\
			_RFSEnterEndlessLoop();	\
		} while (0)

	#define RFS_D7C(cond, msg)	\
		do	\
		{	\
			if (cond)	break;	\
			RFS_D7 msg ;	\
		} while (0)

	#define RFS_A7(cond)	\
		do	\
		{	\
			if (cond)	break;	\
			_RFS_LOCK_CONSOLE();	\
			_RFS_MY_PRINTF("[" RFS_DL_ASSERT_STR ":%d:%s@%s#%d]\r\n", \
					_RFS_GET_THREAD_ID(), __TFUNCTION__,	\
					_RFS_TRIM_PARENT_DIRECTORY(__TFILE__),	\
					__LINE__); \
			_RFS_UNLOCK_CONSOLE();	\
			_RFSEnterDebugBreak();	\
		} while (0)

#else
	#define RFS_D7(msg)
	#define RFS_D7C(cond, msg)
	#define RFS_A7(cond)
#endif

/** Debug MACRO aliases #1.
 * Use these MACROs instead of RFS_D#.
 * For example, you can use RFS_DPANIC instead of RFS_D0. 
 */
#define	RFS_PANIC	RFS_D0	///< Print panic message
#define RFS_CRIT	RFS_D1	///< Print critical message
#define RFS_ERROR	RFS_D2	///< Print error message
#define	RFS_SYST	RFS_D3	///< Print system message
#define RFS_INFO	RFS_D4	///< Print information message
#define RFS_DEBUG	RFS_D5	///< Print debug message
#define RFS_VERB	RFS_D6	///< Print verbose message

/** Debug MACRO aliases #2.
 * Use these MACROs instead of RFS_D#.
 * For example, you can use RFS_PM instead of RFS_D0. 
 * PM means 'P'anic 'M'essage.
 */
#define	RFS_PM		RFS_D0	///< Print panic message
#define RFS_CM		RFS_D1	///< Print critical message
#define RFS_EM		RFS_D2	///< Prit error message
#define	RFS_SM		RFS_D3	///< Print system message
#define RFS_IM		RFS_D4	///< Print information message
#define RFS_DM		RFS_D5	///< Print debug message
#define RFS_VM		RFS_D6	///< Print verbose message

/** Debug MACRO aliases #3.
 * Use these MACROs if condition check is needed.
 * 'C' is added to the above MACRO alias. For example,
 * RFS_PM with condition check MACRO is RFS_PMC.
 */
#define	RFS_PMC		RFS_D0C	///< Print panic message with condition check
#define RFS_CMC		RFS_D1C	///< Print critical message with condition check
#define RFS_EMC		RFS_D2C	///< Print error message with condition check
#define	RFS_SMC		RFS_D3C	///< Print system message with condition check
#define RFS_IMC		RFS_D4C	///< Print information message with condition check
#define RFS_DMC		RFS_D5C	///< Print debug message with condition check
#define RFS_VMC		RFS_D6C	///< Print verbose message with condition check
#define RFS_ASC		RFS_D7C	///< Print assert  message with condition check

#define RFS_AS		RFS_A7	///< Assert MACRO
#define RFS_ASSERT	RFS_AS	///< Assert MACRO

/** Define debug zones here.
 * You can define several debug zones and enable and disable the debugging
 * messages according to the debug zones. Maximum 31 zones are supported.
 */
// CONF: Add your own Debug Zone Mask
enum {
	eRFS_DZM_DEFAULT	= 0x00000000U,	///< Default debug zone mask
	eRFS_DZM_BASE		= 0x00000001U,	///< Debug Zone for package base
	eRFS_DZM_ALWAYS		= 0x00000002U,	///< Print always
	eRFS_DZM_INIT		= 0x00000004U,	///< Debug Zone for Initialization
	eRFS_DZM_TEST		= 0x00000008U,	///< Debug Zone for Test

	///< Add your own debug zone mask here
	///< Don't use 0x80000000
	// OS glue
	eRFS_DZM_API		= 0x00000010U,	///< Debug Zone for API
	eRFS_DZM_DEV		= 0x00000020U,	///< Debug Zone for Device
	eRFS_DZM_OAM		= 0x00000040U,	///< Debug Zone for OS adoptive module

	// Nestle internal
	eRFS_DZM_VOL		= 0x00000100U,	///< Debug Zone for volume
	eRFS_DZM_FILE		= 0x00000200U,	///< Debug Zone for file
	eRFS_DZM_DIR		= 0x00000400U,	///< Debug Zone for dir
	eRFS_DZM_VCBLIST	= 0x00000800U,	///< Debug Zone for vcb list
	eRFS_DZM_VCB		= 0x00000800U,	///< Debug Zone for vcb = vcb list
	eRFS_DZM_FCB		= 0x00001000U,	///< Debug Zone for fcb
	eRFS_DZM_VNODE		= 0x00002000U,	///< Debug Zone for vnode
	eRFS_DZM_PATH		= 0x00004000U,	///< Debug Zone for path
	eRFS_DZM_PCACHE		= 0x00008000U,	///< Debug Zone for path cache
	eRFS_DZM_BCACHE		= 0x00010000U,	///< Debug Zone for buffer cache
	eRFS_DZM_NATIVE		= 0x00020000U,	///< Debug Zone for native manager
	eRFS_DZM_UTIL		= 0x00040000U,	///< Debug Zone for utility
	eRFS_DZM_XATTR		= 0x00080000U,	///< Debug Zone for xattr

	// native filesystem
	eRFS_DZM_DEVFS		= 0x00100000U,	///< Debug Zone for stack
	eRFS_DZM_BTFS		= 0x00200000U,	///< Debug Zone for stack

	///< Add your own debug zone mask here
	///< Don't use 0x80000000
	eRFS_DZM_ALL		= 0x7FFFFFFFU	///< Enable all debug zone mask
};

/** Set default debug zone mask here.
 * Define the default (initial) debug zone to enable.
 */
#define RFS_DZM_DEFAULT		eRFS_DZM_ALWAYS	///< Default debug zone mask

#define RFS_DZM_TEST		eRFS_DZM_TEST
#define RFS_DZM_GLUE		(eRFS_DZM_API | eRFS_DZM_DEV | eRFS_DZM_OAM)
#define RFS_DZM_NESTLE		(eRFS_DZM_VOL | eRFS_DZM_FILE | eRFS_DZM_DIR | eRFS_DZM_VCBLIST | eRFS_DZM_VCB | eRFS_DZM_FCB |			\
	eRFS_DZM_VNODE | eRFS_DZM_PATH | eRFS_DZM_PCACHE | eRFS_DZM_BCACHE | eRFS_DZM_NATIVE | eRFS_DZM_UTIL | eRFS_DZM_BASE)
#define RFS_DZM_NATIVEFS	(eRFS_DZM_DEVFS | eRFS_DZM_BTFS)

#define RFS_GET_DEBUG_ZONE_MASK()	_RFSGetDebugZoneMask()	///< Get debug mask
#define RFS_SET_DEBUG_ZONE_MASK(x)	_RFSSetDebugZoneMask(x)	///< Set debug mask

/// Add more debug zone to the debug zone mask
#define RFS_ADD_DEBUG_ZONE_MASK(x)	\
	do	\
	{	\
		_RFSSetDebugZoneMask(_RFSGetDebugZoneMask() | x);	\
	} while (0)

/// Remove some debug zone from the debug zone mask
#define RFS_DEL_DEBUG_ZONE_MASK(x)	\
	do	\
	{	\
		_RFSSetDebugZoneMask(_RFSGetDebugZoneMask() & (~x));	\
	} while (0)

/** Manage debug zone mask with several MACROs.
 * RFS_GET_DZM() returns current debug zone mask.
 * RFS_SET_DZM(x) updates the debug zone mask with x.
 * RFS_ADD_DZM(x) adds new debug zone 'x' to the global debug zone mask.
 * RFS_DEL_DZM(x) deletes some debug zone 'x" from the global debug zone mask.
 */
#define RFS_GET_DZM()	RFS_GET_DEBUG_ZONE_MASK()
#define RFS_SET_DZM(x)	RFS_SET_DEBUG_ZONE_MASK(x)
#define RFS_ADD_DZM(x)	RFS_ADD_DEBUG_ZONE_MASK(x)
#define RFS_DEL_DZM(x)	RFS_DEL_DEBUG_ZONE_MASK(x)

/** Debug MACROs with debug zone.
 * Use these MACROs if you want to use some debug message which can be
 * controlled with the debug zone.
 * For each source file (.c or .cpp), define the file's debug zone with
 * RFS_FILE_ZONE_MASK. For example, if the source file is related with the 
 * memory setup and initialization, then define as follows.
 * #define RFS_FILE_ZONE_MASK (eRFS_DZM_INIT | eRFS_DZM_MEMORY)
 * Then, use RFS_XXZ inside the code. Every debug zone MACRO ends with 'Z'.
 */
#define RFS_PMZ(msg)	\
	RFS_PMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK)), msg)
#define RFS_CMZ(msg)	\
	RFS_CMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK)), msg)
#define RFS_EMZ(msg)	\
	RFS_EMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK)), msg)
#define RFS_SMZ(msg)	\
	RFS_SMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK)), msg)
#define RFS_IMZ(msg)	\
	RFS_IMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK)), msg)
#define RFS_DMZ(msg)	\
	RFS_DMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK)), msg)
#define RFS_VMZ(msg)	\
	RFS_VMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK)), msg)

#define RFS_PMZC(cond, msg)	\
	RFS_PMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK) && (cond)), msg)
#define RFS_CMZC(cond, msg)	\
	RFS_CMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK) && (cond)), msg)
#define RFS_EMZC(cond, msg)	\
	RFS_EMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK) && (cond)), msg)
#define RFS_SMZC(cond, msg)	\
	RFS_SMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK) && (cond)), msg)
#define RFS_IMZC(cond, msg)	\
	RFS_IMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK) && (cond)), msg)
#define RFS_DMZC(cond, msg)	\
	RFS_DMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK) && (cond)), msg)
#define RFS_VMZC(cond, msg)	\
	RFS_VMC(((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK) && (cond)), msg)

#define RFS_ASZ(cond, msg)	\
	do	{	\
		if (!((RFS_GET_DEBUG_ZONE_MASK() & RFS_FILE_ZONE_MASK)))	break;	\
		RFS_ASC(cond, msg);	\
	} while (0)

/// print out buffer
#define RFS_PRINT_BOUNDARY		16
#define RFS_PRINT_BOUNDARY_MASK	(RFS_PRINT_BOUNDARY - 1)
#define RFS_DUMP_BUFFER(pbuf, length)                               \
{                                                                   \
	unsigned int nBufferLeft = 0;                                         \
	char* pTmpBuf = (char*) pbuf;                                 \
																	\
	for (nBufferLeft = (length); nBufferLeft > 0; nBufferLeft--)    \
	{                                                               \
		if (0 == (nBufferLeft & RFS_PRINT_BOUNDARY_MASK))           \
		{                                                           \
			_RFS_MY_PRINTF(TEXT("\r\n"));                           \
		}                                                           \
		_RFS_MY_PRINTF(TEXT("0x%02x "), *pTmpBuf++);                \
	}                                                               \
}

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if (ENABLE_UNIX >= 1) || (ENABLE_RFS_TOOLS >= 1)
#else
	// Return last pointer of tCh
	extern PTCHAR _RFSGetStrRchrT(PTSTR ptszString, TCHAR tCh);
#endif

	// Return current debug zone mask
	extern unsigned int _RFSGetDebugZoneMask(void);
	// Set new debug zone mask
	extern unsigned int _RFSSetDebugZoneMask(unsigned int nNewDebugZoneMask);
	// Enter endless loop
	extern void _RFSEnterEndlessLoop(void);
	// Enter bebug break
	extern void _RFSEnterDebugBreak(void);
	// Return current time in second
	extern unsigned int _RFSGetTimeSec(void);

#if (ENABLE_MULTI_THREAD >= 1)
	// Get console lock
	extern void _RFSLockConsole(void);
	// Release console lock
	extern void _RFSUnlockConsole(void);
	// Return thread id
	extern THREAD_ID_TYPE _RFSGetThreadId(void);
#endif // end of #if (ENABLE_MULTI_THREAD >= 1)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // end of #ifndef __RFS_DEBUG_H__
