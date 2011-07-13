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
 * @file		ffat_config.h
 * @brief		This file defines VFS dependent configurations
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFAT_CONFIG_VFS_H_
#define _FFAT_CONFIG_VFS_H_

#include "ns_nativefs.h"
#include "nsd_config.h"
#include "ffat_version.h"
#include "ffat_debug.h"

// Nestle does not have any definition for endian
#if (NS_BYTE_ORDER == NS_LITTLE_ENDIAN)
	#define FFAT_LITTLE_ENDIAN
#else
	#define FFAT_BIG_ENDIAN
#endif

#ifdef TFS5_DEBUG
	#define FFAT_DEBUG
#else
	#undef FFAT_DEBUG
#endif


/******************************************************************/
/*
	Lock Type / Reentrant level configuration
*/
/*	1. no lock		*/
/*	2. single lock	*/
/*	3. multiple lock -- fine grained lock */

#ifdef NS_SUPPORT_SINGLE_LOCK
	// nestle request single lock
	#define FFAT_LOCK_TYPE			2
#else
	#if (NS_CONCURRENT_LEVEL == NS_NO_CONCURRENCY)
		// not reentrant
		#define FFAT_LOCK_TYPE		1
	#else
		// nestle is reentrant
		#define FFAT_LOCK_TYPE		3
	#endif
#endif

#ifdef NS_MAX_CONCURRENT_THREADS
	// nestle define this definition only for RTOS and static memory allocation
	#define FFAT_TASK_COUNT					NS_MAX_CONCURRENT_THREADS
#endif



/******************************************************************/
/*
	Miscellaneous Functions Wrappers
*/
#define FFAT_MEMCPY(_des, _src, _size)		NS_CopyMemory((_des), (_src), (_size))
#define FFAT_MEMSET(_p, _c, _size)			NS_FillMemory((_p), (_c), (_size))
#define FFAT_MEMCMP(_p1, _p2, _size)		NS_CompareMemory((_p1), (_p2), (_size))

#define FFAT_STRCPY(dest, src)				NS_StrCpy (dest, src)
#define FFAT_STRNCPY(dest, src, n)			NS_StrNCpy ((char*)dest, (const char*)src, n)
#define FFAT_STRCMP(s1, s2)					NS_StrCmp(s1, s2)

#define FFAT_STRNCMP(s1, s2, n)				NS_StrNCmp(s1, s2, n)
#define FFAT_STRLEN(s)						NS_StrLen((char*)s)
#define FFAT_STRCAT(dest, src)				NS_StrCat (dest, src)
#define FFAT_TOLOWER(c)						NS_ToLower(c)
#define FFAT_TOUPPER(c)						NS_ToUpper(c)

#define FFAT_WCSLEN(s)						NS_WcsLen(s)
#define FFAT_TOWLOWER(wc)					NS_GetLowerWideChar(wc)
#define FFAT_TOWUPPER(wc)					NS_GetUpperWideChar(wc)
#define FFAT_WCSCPY(dest, src)				NS_WcsCpy(dest, src)
#define FFAT_WCSNCPY(dest, src, n)			NS_WcsNCpy(dest, src, n)
#define FFAT_WCSCMP(s1, s2)					NS_WcsCmp(s1, s2)
#define FFAT_WCSNCMP(s1, s2, n)				NS_WcsNCmp(s1, s2, n)
#define FFAT_WCSICMP(s1, s2)				NS_WcsICmp(s1, s2)

#define FFAT_ISLEADBYTE(a, _vol)			NS_IsLeadByte((NS_PVCB)_vol, a)
																			//!< Volume level NLS support
																			//!< 'dev' parameter is VCB pointer in TFS5(Nestle)
#define FFAT_WCTOMB(mb, cb, wc, _vol)			NS_WcToMb((NS_PVCB)_vol, wc, mb, cb)
																			//!< Volume level NLS support
																			//!< 'dev' parameter is VCB pointer in TFS5(Nestle)
#define FFAT_MBSTOWCS(dest, cch, src, cb, _vol)	NS_MbsToWcs((NS_PVCB)_vol, src, cb, dest, cch)
																			//!< Volume level NLS support
																			//!< 'dev' parameter is VCB pointer in TFS5(Nestle)
#define FFAT_WCSTOMBS(dest, cb, src, cch, _vol)	NS_WcsToMbs((NS_PVCB)_vol, src, cch, dest, cb)
																			//!< Volume level NLS support
																			//!< 'dev' parameter is VCB pointer in TFS5(Nestle)

// string length
#ifdef NS_MAX_DIR_NAME_LENGTH
	#define FFAT_DIR_NAME_MAX_LENGTH	NS_MAX_DIR_NAME_LENGTH		/*!< maximum dir name length */
#endif
#ifdef NS_MAX_FILE_NAME_LENGTH
	#define FFAT_FILE_NAME_MAX_LENGTH	NS_MAX_FILE_NAME_LENGTH		/*!< maximum file name length */
#endif
#ifdef NS_MAX_PATH_NAME_LENGTH
	#define FFAT_PATH_NAME_MAX_LENGTH	NS_MAX_PATH_NAME_LENGTH		/*!< maximum path name length */
#endif


	// for log module
#define FFAT_LOG_FILE_NAME_MAX_LENGTH	13				// NULL included

#if defined(NS_LOG_FILE_NAME)
	#define FFAT_LOG_FILE_NAME			NS_LOG_FILE_NAME	// constraints : alphabet and numeric character only
#endif

// for HPA (Hidden Protected Area)
#if defined(NS_HPA_MAX_VOLUME_COUNT)
	#define FFAT_HPA_MAX_VOLUME_COUNT	NS_HPA_MAX_VOLUME_COUNT
#endif // end of #if defined(NS_HPA_MAX_VOLUME_COUNT)

#if defined(NS_HPA_MAX_VOLUME_SIZE)
	#define FFAT_HPA_MAX_VOLUME_SIZE	NS_HPA_MAX_VOLUME_SIZE
#endif // end of #if defined(NS_HPA_MAX_VOLUME_SIZE)

#if defined(NS_HPA_ROOT_NAME)
	#define FFAT_HPA_ROOT_NAME			NS_HPA_ROOT_NAME
#endif // end of #if defined(NS_HPA_ROOT_NAME)

// for symbolic link
#define FFAT_SYMLINK_MAX_PATH_LEN		NS_MAX_PATH_NAME_LENGTH

// permission for extended directory entry
//2009.1029@chunum.kong_[Full_Permission]_use_sticky_bit
#define FFAT_EXTRA_STICKY			NS_STICKY				/**< others can read/write/execute. but only owner can delete */
#define FFAT_EXTRA_SETGID			NS_SGID					/**< group can execute */
#define FFAT_EXTRA_SETUID			NS_SUID					/**< user can execute */

#define FFAT_OTHERS_EXECUTE			NS_OTHERS_EXECUTE		/**< others can execute */
#define FFAT_OTHERS_WRITE			NS_OTHERS_WRITE			/**< others can write */
#define FFAT_OTHERS_READ			NS_OTHERS_READ			/**< others can read */

#define FFAT_GROUP_EXECUTE			NS_GROUP_EXECUTE		/**< group can execute */
#define FFAT_GROUP_WRITE			NS_GROUP_WRITE			/**< group can write */
#define FFAT_GROUP_READ				NS_GROUP_READ			/**< group can read */

#define FFAT_OWNER_EXECUTE			NS_OWNER_EXECUTE		/**< owner can execute */
#define FFAT_OWNER_WRITE			NS_OWNER_WRITE			/**< owner can write */
#define FFAT_OWNER_READ				NS_OWNER_READ			/**< owner can read */

// set memory allocation type
#ifdef NS_SUPPORT_DYNAMIC_ALLOC
	#define FFAT_DYNAMIC_ALLOC
#else
	// Node count
	#define FFAT_MAX_NODE_COUNT				NS_TOTAL_VNODE_COUNT

	// Volume count
	#define FFAT_MAX_VOL_COUNT				NS_MAX_VOLUMES
#endif

// WinCE, Symbian - reserved name 검사를 하지 않음
// RTOS - reserved name 검사를 함
// Linux - 커널 2.4는 reserved name 검사를 하고, 커널 2.6은 reserved name 검사를 하지 않음
// 해당 검사는 OS Glue에서 수행하므로 native filesystem에서는 reserved name 검사를 하지 않음 (Linux 제외)
#define FFAT_NO_CHECK_RESERVED_NAME

#ifdef NS_MAX_FAT_NUMERIC_TAIL
	// nestle limits the maximum numbere of numeric tail
	#define	FFAT_MAX_NUMERIC_TAIL	NS_MAX_FAT_NUMERIC_TAIL
#endif

#ifdef NS_CONFIG_WINCE
	// Directory Entry의 upper two bits(0x40, 0x80)를 사용
	#define FFAT_NO_CHECK_DIRECTORY_ENTRY_ATTR
#endif

#ifdef NS_CONFIG_LINUX
	// IO 성능을 고려하여 block size를 설정함 (block size를 가능한 크게 설정)
	#define FFAT_BLOCK_IO

	// Linux에서는 nativefs에서 reserved name 검사를 수행
	#undef FFAT_NO_CHECK_RESERVED_NAME

	// do now allow trailing space and dot for name
	#define FFAT_NOT_ALLOW_TRAILING_SPACE_AND_DOT
#endif

#ifdef NS_CONFIG_RTOS
	#ifdef NS_MULTIPLE_CHARSET_SUPPORT
		#define FFAT_CMP_NAME_MULTIBYTE
	#endif

	#if (ENABLE_RFS_TOOLS >= 1)
		#define FFAT_BLOCK_IO
	#endif
#endif

#define FFAT_INLINE			__inline

// SET BYTE ORDER CONVERSION ROUTINE
#define FFAT_BO_INT16(v)		NS_BO16(v)
#define FFAT_BO_UINT16(v)		NS_BOU16(v)
#define FFAT_BO_INT32(v)		NS_BO32(v)
#define FFAT_BO_UINT32(v)		NS_BOU32(v)

//!< configurations for print
#define	FFAT_ASSERT(_c)					BTFS_AS(_c)
#define	FFAT_ASSERTP(_c, _msg)			BTFS_ASZ(_c, _msg)

#define	FFAT_PRINT_PANIC(_msg)			BTFS_PMZ(_msg)	// logic error. programming error (this is printed on release mode)
#define	FFAT_PRINT_CRITICAL(_msg)		BTFS_CMZ(_msg)	// I/O ERROR, OS resource, chkdsk fix message (this is printed on release mode)
#define	FFAT_PRINT_ERROR(_msg)			BTFS_EMZ(_msg)	// normal error, no space, API level only, chkdsk error message
#define	FFAT_PRINT_SYSTEM(_msg)			BTFS_SMZ(_msg)	// do not use
#define	FFAT_PRINT_INFO(_msg)			BTFS_IMZ(_msg)	// do not use
#define	FFAT_PRINT_DEBUG(_msg)			BTFS_DMZ(_msg)	// internal module message
#define	FFAT_PRINT_VERBOSE(_msg)		BTFS_VMZ(_msg)	// detailed log, chkdsk progress message

#define	FFAT_CPRINT_PANIC(_c, _msg)		BTFS_PMZC(_c, _msg)
#define	FFAT_CPRINT_CRITICAL(_c, _msg)	BTFS_CMZC(_c, _msg)
#define	FFAT_CPRINT_ERROR(_c, _msg)		BTFS_EMZC(_c, _msg)
#define	FFAT_CPRINT_SYSTEM(_c, _msg)	BTFS_SMZC(_c, _msg)
#define	FFAT_CPRINT_INFO(_c, _msg)		BTFS_IMZC(_c, _msg)
#define	FFAT_CPRINT_DEBUG(_c, _msg)		BTFS_DMZC(_c, _msg)
#define	FFAT_CPRINT_VERBOSE(_c, _msg)	BTFS_VMZC(_c, _msg)

#define FFAT_LOG_PRINTF(_msg)			FFAT_PRINT_DEBUG(_msg)
#define FFAT_DEBUG_PRINTF(_msg)			FFAT_PRINT_DEBUG(_msg)

#define FFAT_VSNPRINTF					NS_Vsnprintf

//!< misc

//!< Character validity table for SFN & LFN
// JUN-17-2009 [JW.Park] Edit the macro about checking character validity by using table.
/********************************************************************************************************/
/******************************** Character validity table for LFN & SFN ********************************/
/********************************************************************************************************/
/*
(1) DEFAULT : FAT COMPATIBLE
	- INVALID for LFN =
		less than 0x20, 0x22("), 0x2A(*), 0x2F(/), 0x3A(:), 0x3C(<) 0x3E(>), 0x3F(?), 0x5C(\), 0x7C(|)
	- INVALID for SFN =
		INVALID for LFN + 0x2B(+), 0x2C(,), 0x3B(;), 0x3D(=), 0x5B([), 0x5D(])
*/

//	0 : Not allowed for SFN & LFN
//	1 : Not allowed for SFN but allowed for LFN
//	2 : Allowed both for SFN & LFN
static const t_uint8 _gsFFATValidCharTable[] =
{/*	0--1--2--3--4--5--6--7--8--9--A--B--C--D--E--F */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		/* 0x00 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		/* 0x10 */
	2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 1, 1, 2, 2, 0,		/* 0x20 */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1, 0, 1, 0, 0,		/* 0x30 */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,		/* 0x40 */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2,		/* 0x50 */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,		/* 0x60 */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2		/* 0x70 */
};

#if defined(NS_CONFIG_LINUX) || defined(NS_CONFIG_RTOS)	// rtos is no need to OS character set, just for test
/*
(2) LINUX : FAT INCOMPATIBLE
	- INVALID for LFN =
		less than 0x20, 0x2F(/)
	- INVALID for SFN =
		INVALID for LFN
*/

//	0 : Not allowed for SFN & LFN
//	1 : Not allowed for SFN but allowed for LFN
//	2 : Allowed both for SFN & LFN
static const t_uint8 _gsFFATValidCharTableForOS[] =
{/*	0--1--2--3--4--5--6--7--8--9--A--B--C--D--E--F */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		/* 0x00 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		/* 0x10 */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0,		/* 0x20 */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,		/* 0x30 */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,		/* 0x40 */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,		/* 0x50 */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,		/* 0x60 */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,		/* 0x70 */
};
#else
	#define	_gsFFATValidCharTableForOS	_gsFFATValidCharTable
#endif // end of CONFIG_LINUX

// [NOTICE]
// If you need more validity table for other OS, define another _gsValidCharTableForOS
/********************************************************************************************************/

#define FFAT_IS_VALID_CHAR_FOR_LFN(_c)	\
					(((_c) >= (t_wchar)0x7F) ? FFAT_TRUE : \
					((_gsFFATValidCharTable[(t_int32)(_c)] >= 1) ? FFAT_TRUE : FFAT_FALSE))
#define FFAT_IS_VALID_FOR_SFN(_c)	\
					(((_c) >= (t_wchar)0x7F) ? FFAT_TRUE : \
					((_gsFFATValidCharTable[(t_int32)(_c)] >= 2) ? FFAT_TRUE : FFAT_FALSE))

#define FFAT_IS_VALID_CHAR_FOR_LFN_IN_OS(_c)	\
					(((_c) >= (t_wchar)0x7F) ? FFAT_TRUE : \
					((_gsFFATValidCharTableForOS[(t_int32)(_c)] >= 1) ? FFAT_TRUE : FFAT_FALSE))
#define FFAT_IS_VALID_FOR_SFN_IN_OS(_c)	\
					(((_c) >= (t_wchar)0x7F) ? FFAT_TRUE : \
					((_gsFFATValidCharTableForOS[(t_int32)(_c)] >= 2) ? FFAT_TRUE : FFAT_FALSE))

#endif	/* _FFAT_CONFIG_VFS_H_ */

