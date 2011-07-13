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
 * @file		ffat_config_vfs.h
 * @brief		The all and defaut configurations for BTFS
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		MAR-17-2009 [DongYoung Seo] move VFS dependent code to ffat_config_vfs.h
 * @see			None

 */

#ifndef _FFAT_CONFIG_H_
#define _FFAT_CONFIG_H_

#include "ffat_config_vfs.h"

/******************************************************************/
/*
	Endian Configuration
		For little-endian system.
			#define FFAT_LITTLE_ENDIAN
		For big-endian system.
			#define FFAT_BIG_ENDIAN
*/
/* define ENDIAN */
//#define FFAT_LITTLE_ENDIAN
//#define FFAT_BIG_ENDIAN

#if !defined(FFAT_LITTLE_ENDIAN) && !defined(FFAT_BIG_ENDIAN)
	#error "check configuration for endianess"
#endif

/*****************************************************************/
/*
	Default sector size configuration
*/
#ifndef FFAT_SECTOR_SIZE
	#define FFAT_SECTOR_SIZE			512		/*!<  default sector size,  */
	#define FFAT_SECTOR_SIZE_BITS		9		/*!<  default sector size bit count */
#endif

/*****************************************************************/
/*
	Max volume count
*/
#ifndef FFAT_MOUNT_COUNT_MAX
	#define FFAT_MOUNT_COUNT_MAX		8		/*!< the maximum concurrent volume mount count */
#endif

/*****************************************************************/
/*
	Max volume count
*/
#ifndef FFAT_FORMAT_ALIGN
	#define FFAT_FORMAT_ALIGN			16		/*!< cluster align for volume formating */
												/*!< The first cluster is aligned by this value */
#endif
/*****************************************************************/
/*
	VFAT On/Off
*/
#ifndef FFAT_VFAT_OFF
	#define FFAT_VFAT_SUPPORT				/*!< support long file name & FAT32 */
#endif

/******************************************************************/


/*****************************************************************/
/*
	Max Vol/Node Count
*/
#ifndef FFAT_MAX_VOL_COUNT
	#define FFAT_MAX_VOL_COUNT			8		/*!< The maximum volume count that can be mounted concurrently */
												/*!< Dynamic memory configuration does not limit this */
#endif

#ifndef FFAT_MAX_NODE_COUNT
	#define FFAT_MAX_NODE_COUNT			128		/*!< the maximum node count can be accessed concurrently*/
												/*!< Dynamic memory configuration does not limit this */
												// this value is changed by VFS configuration at below 
#endif


/******************************************************************/
/*
	Memory allocation type configuration

	Define FFAT_DYNAMIC_ALLOC for dynamic allocation for FFAT Work.
		FFAT allocates memory at the entrance of all APIs amount of Maximum usage.
		And create a PStack then do filesystem operation with the PStack.
	if FFAT_DYNAMIC_ALLOC is not defined
		1. lock type is FFAT_LOCK_SINGLE or FFAT_LOCK_NONE
			FFAT creates a PStack while it is initialize.
			And use it. (There is only on thread in the FFAT)
		2. lock type is FFAT_LOCK_MULTIPLE
			FFAT created defined count of PStack at FFAT_PSTACK_COUNT
			And use it for operation.
			When too many thread want to enter, they will be serialized.(by counting semaphore)

	#define FFAT_DYNAMIC_ALLOC				ffat uses dynamic memory allocation
											if not defined, FFAT uses static memory allocation
*/


/*****************************************************************/
/*
	Max THREAD COUNT = reentrant count
	
*/
#ifndef FFAT_TASK_COUNT
	#define FFAT_TASK_COUNT			1
#endif


/******************************************************************/
/*
	Lock Type / Reentrant level configuration
*/
/*	1. no lock		*/
/*	2. single lock	*/
/*	3. multiple lock -- fine grained lock */
#define FFAT_LOCK_NONE			1		/*!<  FFAT does not use lock, Lock is on duty of Upper layer */
#define FFAT_LOCK_SINGLE		2		/*!<  FFAT uses a lock(a big lock), not re-entrant */
#define FFAT_LOCK_MULTIPLE		3		/*!<  FFAT uses multiple lock, file level read, volume level write */

//#define FFAT_LOCK_TYPE		FFAT_LOCK_NONE			//!< for TFS4 1.x and TFS4 2.0
//#define FFAT_LOCK_TYPE		FFAT_LOCK_SINGLE		//!< FOR Nestle
//#define FFAT_LOCK_TYPE		FFAT_LOCK_MULTIPLE		//!< FOR Nestle

#ifndef FFAT_LOCK_TYPE
	#define FFAT_LOCK_TYPE		FFAT_LOCK_SINGLE		//!< not used yet. not tested
#endif

#if (FFAT_LOCK_TYPE == FFAT_LOCK_NONE) && (FFAT_TASK_COUNT != 1)
		#error "this must be no lock, - this is for RTOS & single thread, check config"
#endif


/******************************************************************/
/*
	Functions Wrappers
*/

/*!<  memory functions */
#define FFAT_MALLOC(_size, _flag)		ffat_common_allocMem(_size, _flag)
														/*!< memory allocation */
#define FFAT_FREE(_p, _size)			(_p) ? ffat_common_freeMem((_p), _size) : ((void)0)
														/*!< free memory */
/*!<  C-Library functions */
#define FFAT_RAND()						EssMath_Rand()

/*!<  String functions */
#define FFAT_STRICMP(s1, s2)			ffat_common_stricmp(s1, s2)
#define FFAT_WCSUCMP(s1, s2)			ffat_common_wcsucmp(s1, s2)		// 대문자비교 (s1은 대문자로 입력되어야 한다)
#define FFAT_WCSNUCMP(s1, s2, n)		ffat_common_wcsnucmp(s1, s2, n)	// 대문자비교 (s1은 대문자로 입력되어야 한다)
#define FFAT_TOWUPPER_STR(dest, src)	ffat_common_towupper_str(dest, src)

// time functions
#define FFAT_LOCALTIME				ffat_localTime

#define FFAT_GETCHAR				ffat_getChar

//20100413_sks => Change to add the cluster notification function
#define FFAT_CLUSTER_CHANGE_NOTI(dwFreeCount,dwTotalCount,dwClustersize,pDevice)	ffat_common_cluster_change_notify(dwFreeCount,dwTotalCount,dwClustersize,pDevice)
/*****************************************************************/
/*
	string length
*/
#ifndef FFAT_DIR_NAME_MAX_LENGTH
	#define FFAT_DIR_NAME_MAX_LENGTH	244		/*!< maximum directory name length (WinXP Compatible) */
#endif
#ifndef FFAT_FILE_NAME_MAX_LENGTH
	#define FFAT_FILE_NAME_MAX_LENGTH	255		/*!< maximum file name length */
#endif
#ifndef FFAT_PATH_NAME_MAX_LENGTH
	#define FFAT_PATH_NAME_MAX_LENGTH	259		/*!< maximum path name length */
#endif

#if (FFAT_DIR_NAME_MAX_LENGTH > FFAT_FILE_NAME_MAX_LENGTH)
	#define FFAT_NAME_MAX_LENGTH		FFAT_DIR_NAME_MAX_LENGTH
#else
	#define FFAT_NAME_MAX_LENGTH		FFAT_FILE_NAME_MAX_LENGTH
#endif

#if (FFAT_DIR_NAME_MAX_LENGTH > FFAT_PATH_NAME_MAX_LENGTH) || (FFAT_FILE_NAME_MAX_LENGTH > FFAT_PATH_NAME_MAX_LENGTH)
	#error	" Check configuration, maximum path length is shorter than max length for file or directory "
#endif

// SymLInk
#ifndef FFAT_SYMLINK_MAX_PATH_LEN
	#define	FFAT_SYMLINK_MAX_PATH_LEN		FFAT_MAX_PATH
#endif


/*****************************************************************/
/*
	 LOG configuration
*/
#ifndef FFAT_LOG_FILE_NAME_MAX_LENGTH
	#define FFAT_LOG_FILE_NAME_MAX_LENGTH	13				//!< the maximum name length for log file name
															//!< NULL included
#endif
#ifndef FFAT_LOG_FILE_NAME
	#define FFAT_LOG_FILE_NAME				"@btfs.log"		//!< log file name
#endif

#define FFAT_LOG_DEFAULT_MOUNT_FLAG			FFAT_MOUNT_LOG_LLW	//!< default log flag is LLW

/*****************************************************************/
/*
	Hidden Protected Area configuration
*/
#ifndef FFAT_HPA_MAX_VOLUME_COUNT
	#define FFAT_HPA_MAX_VOLUME_COUNT		1				//!<  The maximum HPA volume count
#endif
#ifndef FFAT_HPA_MAX_VOLUME_SIZE
	#define FFAT_HPA_MAX_VOLUME_SIZE		1024			//!< The maximum available volume size for HPA (MB)
#endif
#ifndef FFAT_HPA_ROOT_NAME
	#define FFAT_HPA_ROOT_NAME				"__hidden__"	//!< the root name of HPA, MAX Length is 12 character
#endif


/******************************************************************/
/*
	Misc. configurations
*/

/*!<  Include Headers for STD C */
// do not include any std C library for Linux 
// stdio.h string.h, stdlib.h ctype.h

/* The FFAT_FASTCALL calling convention specifies 
	that arguments to functions are to be passed in registers, when possible. 
	The following list shows the implementation of this calling convention.

	ex) on MS-Windows(WinXP, Win2000 etc.) system
		#define FFAT_FASTCALL		__fastcall

		if  fast call does not provides by compiler set it like below
		#define		FFAT_FASTCALL
*/
#define		FFAT_FASTCALL

/*!< configuration for removable device synchronization.
	If this definition is defined all meta-data for external device 
	will be synchronized before return.

	Notice.
		All meta-data will be synchronized by FFAT_MOUNT_SYNC_META flag.
*/
#define FFAT_SYNC_METADATA_ON_REMOVABLE_DEVICE

/*!< configuration for removable device synchronization.
	If this definition is defined all user-data for external device 
	will be synchronized before return.

	Notice.
		All user-data will be synchronized with removable device.
*/
#define FFAT_SYNC_USERDATA_ON_REMOVABLE_DEVICE


/*!< configuration for FAT mirroring for removable device 
	If this definition is defined all FAT update will be mirrored to another FAT (2nd, 3rd,...)

#define FFAT_MIRROR_FAT_ON_REMOVABLE_DEVICE
*/


/*
	Do not check Reserved Name for FAT filesystem.
	#define FFAT_NO_CHECK_RESERVED_NAME
*/


/*
	Directory Entry의 upper two bits(0x40, 0x80)를 사용
	#define FFAT_NO_CHECK_DIRECTORY_ENTRY_ATTR
*/


/*
	IO 성능을 고려하여 block size를 설정함 (block size를 가능한 크게 설정)
	#define FFAT_BLOCK_IO
*/


/*
	GET REAL SIZE OF DIRECTORY WHEN LOOKUP AND READDIR for Linux
	#define FFAT_GET_REAL_SIZE_OF_DIR
*/


/*
	#define FFAT_NO_LOG_NO_SYNC					// Does not sync meta-data for a volume 
											//	that mounted without log
*/


/*
	define below - to not allow trailing space and dot (This is LINUX Requirement)
	#define FFAT_NOT_ALLOW_TRAILING_SPACE_AND_DOT
*/
#ifdef FFAT_NOT_ALLOW_TRAILING_SPACE_AND_DOT
	#undef FFAT_ALLOW_TRAILING_SPACE_AND_DOT
#else
	#define FFAT_ALLOW_TRAILING_SPACE_AND_DOT
#endif


/*
	Limit the maximum number of numeric tail for Short Directory Entry
	#define FFAT_MAX_NUMERIC_TAIL		999			// set maximum number to 999
*/


/* compare name string both UNICODE and MULTIBYTE
	#define FFAT_CMP_NAME_MULTIBYTE
*/
// CAUTION!!, BE CAREFUL
//	upper feature is a some dangerous thing.
// user may change another file when the name string can not be converted to Multi-Byte successfully
// Code Page defines it's own character set with one or two byte.
// Some languages (Korean, Japanese etc.) use 2 byte for a character 
//	but other language use just 1 byte for a character.
//	So. some 2byte character can not be converted properly when code page is not same one.
//	some character is converted to '_' when that language does not have value for it.
// End user may get file name '_'
// So there may be several same named file on a directory
//	and user can not identify them with name.


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//
/*  internal configuration for FFAT CORE File system. do not modify from here */
//
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/


//=============================================================================
//
//!< internal configuration for development from here.
//
//

//!< definition for FFAT version
#define FFAT_VERSION_MAJOR	0x01		// major version	X.yy.zz-pp
#define FFAT_VERSION_MINOR1	0x00		// minor version	x.YY.zz-pp
#define FFAT_VERSION_MINOR2	0x01		// minor version	x.yy.ZZ-pp
#define FFAT_VERSION_PATCH	0x02		// build(RC)		x.yy.zz-PP

//!< FFAT_VERSION 0x01000104 means v1.0.1_patch4
#define FFAT_VERSION		(FFAT_VERSION_MAJOR << 24) |	\
							(FFAT_VERSION_MINOR1 << 16)	|	\
							(FFAT_VERSION_MINOR2 << 8) |	\
							FFAT_VERSION_PATCH

//2010.0406@shinho.oh_bug_fix_malloc
//to increase memory size for 32*1024 at _readwriteInit()
#define FFAT_MEM_SIZE_MAX			(96 * 1024)		/*!< FFAT Max Memory Usage */

#define FFAT_LOCAL_PSTACK_SIZE		(ESS_PSTACK_META_SIZE + FFAT_MEM_SIZE_MAX)
													/* PStack Size for a volume */
#define FFAT_PSTACK_SIZE			FFAT_MEM_SIZE_MAX


#define FFAT_NAME_BUFF_SIZE			(((13 * 20) + 1) * sizeof(t_wchar))

/* DEVICE COFIGURATION */
#define FFAT_LDEV_MIN_SECTOR_SIZE			512			/*!<  minimum sector size */
#define FFAT_LDEV_MAX_SECTOR_SIZE			8192		/*!<  maximum sector size */

/* SIGNATURE */
#define FFAT_VOLUME_SIG						0x00761225
#define FFAT_ROOT_NAME						"/"


/* true and false */
#define FFAT_TRUE		ESS_TRUE
#define FFAT_FALSE		ESS_FALSE


// #define FFAT_STRICT_CHECK		//!< watch on all parameter
#ifdef WIN32
	#undef		FFAT_FASTCALL
	#define		FFAT_FASTCALL		__fastcall
#endif


//!< branch prediction for gcc
#if defined(__GNUC__)
	#define IF_LK(x)		if (likely(x))		// probably 'if' is true
	#define IF_UK(x)		if (unlikely(x))	// probably 'if' is false
#else
	#define IF_LK(x)		if (x)
	#define IF_UK(x)		if (x)
#endif


#define FFAT_NUMERIC_TAIL_BUFF			1024							//!< byte for numeric tail buffer
																		//!< do not change this value - never
#define FFAT_NUMERIC_TAIL_MAX_INDEX		(FFAT_NUMERIC_TAIL_BUFF * 8)	//!< MAX numeric tail

#if (FFAT_NUMERIC_TAIL_MAX_INDEX > 65536)
	#error	Too big numerictail buffer.
#endif

#ifdef FFAT_MAX_NUMERIC_TAIL
	#if (FFAT_NUMERIC_TAIL_MAX_INDEX < FFAT_MAX_NUMERIC_TAIL)
		#error "check buffer size of FFAT_NUMERIC_TAIL_MAX_INDEX, or decrease FFAT_MAX_NUMERIC_TAIL"
		// FFAT_NUMERIC_TAIL_MAX_INDEX must be over FFAT_MAX_NUMERIC_TAIL for simple code
	#endif
#endif

#define FFAT_SECTOR_SIZE_MAX			(4*1024)		//!< max sector size

//!< temporary buffer 
#define FFAT_TEMP_BUFF_MAX_SIZE			(2*1024)		//!< max temporary buffer size, (should be over 1024)
#define FFAT_TEMP_BUFF_MAX_SIZE_BITS	11				//!< max temporary buffer size in bit

//!< allocation buffer
#define FFAT_ALLOC_BUFF_SIZE			(1*1024)		//!< 1024 byte buffer for cluster allocation/deallocation
														//!< minimum size is 1024

//!< definition for automatic directory truncation while node creation operation
#define FFAT_TRUNCATE_DIR

#define FFAT_COMMON_LOCK_COUNT			1				//!< lock count for common module 
														//!< for atomic type

// direct I/O trigger
//#define FFAT_DIRECT_IO_TRIGGER			(64*1024)	// trigger size for FFATFS caching.
														// this value is depended on VFS
														// so BTFS does not enforce DIRECT-IO

//!< Buffer size for cluster initialization
#define FFAT_CLUSTER_INIT_BUFF_SIZE		8192			//!< buffer size for cluster initialization */


//=============================================================================
//
// Configuration for Add-on Module
//

// for Free Cluster Cache
#define FFAT_FCC_BUFF_MIN					(440 + 120)		//!< minimum buffer size for free cluster cache(in byte)

// buffer size for format
#define FFAT_FORMAT_BUFF_SIZE				(512*32)		//!< buffer for FAT initialization

// memory usage for ADDON module
#define FFAT_ADDON_FCC_MEM_SIZE			(16 * 1024)			//!< FCC ADDON memory usage

// Global Directory Entry Cache
#define FFAT_GDEC						//!< Enable GDEC

#define FFAT_GDEC_MEM_SIZE				(32 * 1024)			//!< Memory size for GDEC
															//!< minimum size is 1KB
															//!< recommend size is over 8KB

#define FFAT_GDEC_BASE_ENTRY_COUNT		50					//!< entry count in one entry block
															//!< recommend 40~50
															//!< but if FFAT_GDEC_MEM_SIZE is small (less than 4KB)
															//!< recommend 20~30

// Extended Attribute
#define FFAT_EA_NAME_SIZE_MAX		255						//!< the maximum byte size of name
#define FFAT_EA_VALUE_SIZE_MAX		(16 * 1024)				//!< the maximum byte size of value
#define FFAT_EA_ATTR_NUMBER_MAX		128						//!< the maximum number of extended attributes
#define FFAT_EA_LIST_SIZE_MAX		(64 * 1024)				//!< the maximum byte size of total extended attributes

// default configurations
#ifndef FFAT_INLINE
	#define FFAT_INLINE					// define your own inline descriptor at each VFS confutation section
#endif

//!< CONFIGURATION FOR HPA
//!< Do not modify from here (library should be rebuilt) ==========================================
#define FFAT_HPA_BITMAP_SIZE			(FFAT_HPA_MAX_VOLUME_SIZE * 2)
														//!< bitmap size for FFAT_HPA_MAX_VOLUME_SIZE
														//!< Total bitmap size = FFAT_HPA_MAX_VOLUME_COUNT * FFAT_HPA_BITMAP_SIZE
#define FFAT_HPA_ROOT_NAME_MAX_LENGTH	13				//!< HPA root directory name maximum length
														//!< Should be power of 13, and NULL included.
														//!< Do not modify this
#define FFAT_HPA_ROOT_DE_COUNT			((FFAT_HPA_ROOT_NAME_MAX_LENGTH / FAT_LFN_NAME_CHAR) + 1)
														//!< DE count for HPA root directory

//!< description for bitmap size equation
//!< The equation for bitmap size FFAT_HPA_MAX_VOLUME_SIZE size volume
//!< (FFAT_HPA_MAX_VOLUME_SIZE * (((1024 * 1024 / TFS4_SECTOR_SIZE) / 128) / 8))
//!<								<---------->
//!<									1MB
//!<								<------------------------------>
//!<									change it to sector count
//!<								<-------------------------------------->
//!<									convert it to FAT sector count
//!<								<------------------------------------------->
//!<									change it to byte count == 2
//!<  <-------------------------------------------------------------------------->
//!<  Total bitmap memory size for FFAT_HPA_MAX_VOLUME_SIZE size volume



// Configuration for GFS
#define FFAT_ADDON_GFS_MEM_SIZE			(16 * 1024)				//!< GFS memory usage, minimum size 1KB
#define FFAT_ADDON_GFS_VCE_PER_ENTRY	8						//!< cluster chunks per a GFS Entry
																//!< must be power of 2, Max : 32768
#define FFAT_ADDON_GFS_READ_AHEAD_COUNT	64						//!< cluster chain read ahead FFATFS_FAT_CALLBACK_COUNT
																//!< caching clusters more than required count
#define FFAT_ADDON_GFS_NODE_COUNT		FFAT_MAX_NODE_COUNT
																//!< maximum node count for GFS
																//!< check this.. ugly hack.


// MEMORY

/*!< configuration for static memory allocation only */
/*!< this is maximum concurrently mountable count */
#if (FFAT_LOCK_TYPE == FFAT_LOCK_MULTIPLE)
	#define FFAT_PSTACK_COUNT			FFAT_TASK_COUNT			/*!< count of ESS_PStack */
#else
	#define FFAT_PSTACK_COUNT			1						/*!< count of ESS_PStack */
#endif

// General memory allocation for internal operation.
// It never return error.
//	_dwS	: size of memory
//	_p		: pointer of memory
//	_pCxt	: pointer of Context
#define FFAT_LOCAL_ALLOC(_dwS, _pCxt)		ffat_common_localMalloc(_dwS, _pCxt)	/*!<  local memory allocation,(for pstack) */
#define FFAT_LOCAL_FREE(_p, _dwS, _pCxt)	ffat_common_localFree(_p, _dwS, _pCxt)	/*!<  local memory free, (for pstack) */


//=============================================================================
//!< lock configuration
//

// LOCK
#define FFAT_LOCK_INIT_COUNT				1		//!< initialize resource count for lock

#if (FFAT_LOCK_TYPE == FFAT_LOCK_NONE) || (FFAT_LOCK_TYPE == FFAT_LOCK_SINGLE)
	#define FFAT_LOCK_GET_FREE(_ppLock)		FFAT_OK
	#define FFAT_LOCK_RELEASE(_pLock)		FFAT_OK
	#define FFAT_LOCK_GET(_pLock)			FFAT_OK
	#define FFAT_LOCK_PUT(_pLock)			FFAT_OK

	#define FFAT_LOCK_COUNT_MAX				1		//!< max lock count
#else
	#define FFAT_LOCK_GET_FREE(_ppLock)		ffat_al_getFreeLock(_ppLock)
	#define FFAT_LOCK_RELEASE(_ppLock)		ffat_al_releaseLock(_ppLock)
	#define FFAT_LOCK_GET(_pLock)			ffat_al_getLock(_pLock)
	#define FFAT_LOCK_PUT(_pLock)			ffat_al_putLock(_pLock)

	#define FFAT_LOCK_COUNT_MAX				(FFAT_MAX_NODE_COUNT * 2)		//!< max lock count
#endif	/* #ifdef FFAT_NOLOCK */

// end of lock configuration
//=============================================================================


//=============================================================================
// check byte order conversion routine

#ifndef FFAT_BO_INT16
	#error "check configuration, FFAT_BO_INT16 not defined "
#endif
#ifndef FFAT_BO_UINT16
	#error "check configuration, FFAT_BO_UINT16 not defined "
#endif
#ifndef FFAT_BO_INT32
	#error "check configuration, FFAT_BO_INT32 not defined "
#endif
#ifndef FFAT_BO_UINT32
	#error "check configuration, FFAT_BO_UINT32 not defined "
#endif
// end of byte order
//=============================================================================

//!< configurations for print

// for nestle support
#ifndef FFAT_ASSERT
	#error "check configuration, FFAT_ASSERT not defined "
#endif
#ifndef FFAT_ASSERTP
	#error "check configuration, FFAT_ASSERTP not defined "
#endif
#ifndef FFAT_PRINT_PANIC
	#error "check configuration, FFAT_PRINT_PANIC not defined "
#endif
#ifndef FFAT_PRINT_CRITICAL
	#error "check configuration, FFAT_PRINT_CRITICAL not defined "
#endif
#ifndef FFAT_PRINT_ERROR
	#error "check configuration, FFAT_PRINT_ERROR not defined "
#endif
#ifndef FFAT_PRINT_SYSTEM
	#error "check configuration, FFAT_PRINT_SYSTEM not defined "
#endif
#ifndef FFAT_PRINT_INFO
	#error "check configuration, FFAT_PRINT_INFO not defined "
#endif
#ifndef FFAT_PRINT_DEBUG
	#error "check configuration, FFAT_PRINT_DEBUG not defined "
#endif
#ifndef FFAT_PRINT_VERBOSE
	#error "check configuration, FFAT_PRINT_VERBOSE not defined "
#endif
#ifndef FFAT_LOG_PRINTF
	#error "check configuration, FFAT_LOG_PRINTF not defined "
#endif
#ifndef FFAT_DEBUG_PRINTF
	#error "check configuration, FFAT_DEBUG_PRINTF not defined "
#endif


#ifdef FFAT_STACK_CHECK
	#define FFAT_STACK_VAR			ESS_STACK_VAR
	#define FFAT_STACK_BEGIN		ESS_STACK_BEGIN
	#define FFAT_STACK_END			ESS_STACK_END
#else
	#define FFAT_STACK_VAR
	#define FFAT_STACK_BEGIN()			((void)0)
	#define FFAT_STACK_END()			((void)0)
#endif

// debug begin
#ifdef FFAT_DEBUG
	// file system status
	#define FFAT_STATUS			/*!< gather filesystem status */
	#undef FFAT_STRICT_CHECK
	#define FFAT_STRICT_CHECK
#endif
// debug end

//#define FFAT_DEBUG_FILE

/*****************************************************************************/
/*****************************************************************************/
/*
	Configurations for FFATFS
*/
#define FFATFS_LOCK_NONE		1		/*!< FFATFS does not use lock */
#define FFATFS_LOCK_SINGLE		2		/*!< FFATFS uses single lock */
										/* This is used when FFAT_LOCK_TYPE is FFAT_LOCK_MULTIPLE or FFAT_LOCK_NONE */

#if (FFAT_LOCK_TYPE == FFAT_LOCK_MULTIPLE)
	#define FFATFS_LOCK_TYPE		FFATFS_LOCK_SINGLE			//!< for NESTLE
#else
	#define FFATFS_LOCK_TYPE		FFATFS_LOCK_NONE			//!< for TFS4 1.x and 2.0
#endif

/* config for FFatfsCache */
#define FFATFS_CACHE_SECTOR		256		/*!<  default FAT cache sector count, 256 sector */

//!< configurations for FFatfsCache ====================================
#define FFATFS_MAX_CACHE_COUNT				8						//!< maximum cache count
#define FFATFS_CACHE_SIZE_MIN				2						//!< minimum FFATFS cache size in sector

//!< cache size in byte
#define FFATFS_CACHE_SIZE_IN_BYTE			(FFATFS_CACHE_SECTOR * FFAT_SECTOR_SIZE)

#define FFATFS_CACHE_HASH_BUCKET_COUNT		128						//!< bucket count for hash
#define FFATFS_CACHE_HASH_MASK				(FFATFS_CACHE_HASH_BUCKET_COUNT - 1)
																	//!< bucket count mask

#define FFATFS_CACHE_SECTOR_BITMAP_COUNT	256						//!< bitmap for sector IO, 
																	//!< default 256 sector
#define FFATFS_CACHE_SECTOR_BITMAP_BYTE		(FFATFS_CACHE_SECTOR_BITMAP_COUNT / 8)
																	//!< bitmap size in byte

#define FFATFS_CACHE_SECTOR_BITMAP_LOCAL_COUNT	64
#define FFATFS_CACHE_SECTOR_BITMAP_LOCAL_BYTE	(FFATFS_CACHE_SECTOR_BITMAP_LOCAL_COUNT / 8)
																	//!< local bitmap size in byte

#define FFATFS_CACHE_BYPASS_TRIGGER			(FFATFS_CACHE_SECTOR / 2)
													//!< trigger sector count for caching
													//!< if IO sector count is over this value,
													//!< the sectors are not stored at the cache

#define FFATFS_AUTO_CACHE_ADD_REMOVE				//!< add/remove FFATFS cache automatically

#define FFATFS_CACHE_CALLBACK_COUNT			1		//!< cache callback function registration count
#define FFATFS_FAT_CALLBACK_COUNT			1		//!< fat callback function registration count

//!< do not modify this value
//!< buffer size for backward cluster deallocation
#define FFATFS_SPARSE_CLUSTER_COUNT_LV1		256		//!< sparse cluster count for level1
#define FFATFS_SPARSE_CLUSTER_COUNT_LV2		128		//!< sparse cluster count for level2
#define FFATFS_CLUSTER_RM_COUNT				128		//!< minimum resource map(VC Entry) count

//!< buffer size for statfs
#define FFATFS_STATFS_BUFF_SIZE				16384		//!< buffer size for statfs */

//!< Lock count for FFATFS
#if (FFATFS_LOCK_TYPE == FFATFS_LOCK_SINGLE)
	#define FFATFS_LOCK_COUNT		1
#else
	#define FFATFS_LOCK_COUNT		0
#endif


// FAT spec denotes about reserving most 4 bit on FAT area for FAT32.
//	But Windows XP does not reserve it.
//	define "FFAT_RESERVE_MOST_4BIT_ON_FAT_UPDATING" to reserve it
//	refer to the FAT32 specification page 16
//	caution : this feature is not tested
//#define FFAT_RESERVE_MOST_4BIT_ON_FAT_UPDATING


// End of configurations for FFatfsCache =====================================
/*****************************************************************************/

// Total Lock Count
#define FFAT_LOCK_COUNT						(FFAT_LOCK_COUNT_MAX + FFAT_COMMON_LOCK_COUNT + FFATFS_LOCK_COUNT)

// configuration error checks
#if (FFAT_LOCK_TYPE == FFAT_LOCK_MULTIPLE)
	#if (FFATFS_LOCK_TYPE == FFATFS_LOCK_NONE)
		#error "FFATFS_LOCK_TYPE should be FFATFS_LOCK_SINGLE when FFAT_LOCK_TYPE is FFAT_LOCK_MULTIPLE!!"
	#endif
#endif

// check for fast seek
#if (FFAT_ADDON_GFS_MEM_SIZE < 1024)
	#error "invalid FFAT_ADDON_GFS_MEM_SIZE, too small size, should be over 1KB"
#endif

#if (FFAT_ADDON_GFS_VCE_PER_ENTRY < 1) || (FFAT_ADDON_GFS_VCE_PER_ENTRY > 65535)
	#error "invalid FFAT_ADDON_GFS_VCE_PER_ENTRY config !!, should be a value between 1 and 65535"
#endif


#if defined(__ARMCC_VERSION)
	#if (__ARMCC_VERSION >= 220000)
		// RVCT 2.2 patch0, build 000
		#pragma diag_remark 236		// warning #236-D: controlling expression is constant

		#ifdef FFAT_DEBUG
			#pragma	diag_remark	111		// warning #111-D: statement is unreachable,
										// FFAT_ASSERT has conditional expression .
										//	compiler will generate warning when the condition is constant.
			#pragma	diag_remark	128		// warning #128-D: loop is not reachable from preceding code
										// FFAT_ASSERT has conditional expression .
										//	compiler will generate warning when the condition is constant.
		#endif
	#endif
#endif


#if !defined(FFAT_MEMCPY)
	#error "Check configuration, FFAT_MEMCPY does not defined!!"
#endif
#if !defined(FFAT_MEMSET)
	#error "Check configuration, FFAT_MEMSET does not defined!!"
#endif
#if !defined(FFAT_MEMCMP)
	#error "Check configuration, FFAT_MEMCMP does not defined!!"
#endif
#if !defined(FFAT_STRCPY)
	#error "Check configuration, FFAT_MEMCPY does not defined!!"
#endif
#if !defined(FFAT_STRNCPY)
	#error "Check configuration, FFAT_STRNCPY does not defined!!"
#endif
#if !defined(FFAT_STRCMP)
	#error "Check configuration, FFAT_STRCMP does not defined!!"
#endif
#if !defined(FFAT_STRNCMP)
	#error "Check configuration, FFAT_STRNCMP does not defined!!"
#endif
#if !defined(FFAT_STRLEN)
	#error "Check configuration, FFAT_STRLEN does not defined!!"
#endif
#if !defined(FFAT_STRCAT)
	#error "Check configuration, FFAT_STRCAT does not defined!!"
#endif
#if !defined(FFAT_TOLOWER)
	#error "Check configuration, FFAT_TOLOWER does not defined!!"
#endif
#if !defined(FFAT_TOUPPER)
	#error "Check configuration, FFAT_TOUPPER does not defined!!"
#endif
#if !defined(FFAT_WCSLEN)
	#error "Check configuration, FFAT_WCSLEN does not defined!!"
#endif
#if !defined(FFAT_TOWLOWER)
	#error "Check configuration, FFAT_TOWLOWER does not defined!!"
#endif
#if !defined(FFAT_TOWUPPER)
	#error "Check configuration, FFAT_TOWUPPER does not defined!!"
#endif
#if !defined(FFAT_WCSCPY)
	#error "Check configuration, FFAT_WCSCPY does not defined!!"
#endif
#if !defined(FFAT_WCSNCPY)
	#error "Check configuration, FFAT_WCSNCPY does not defined!!"
#endif
#if !defined(FFAT_WCSCMP)
	#error "Check configuration, FFAT_WCSCMP does not defined!!"
#endif
#if !defined(FFAT_WCSNCMP)
	#error "Check configuration, FFAT_WCSNCMP does not defined!!"
#endif
#if !defined(FFAT_WCSICMP)
	#error "Check configuration, FFAT_WCSICMP does not defined!!"
#endif
#if !defined(FFAT_ISLEADBYTE)
	#error "Check configuration, FFAT_ISLEADBYTE does not defined!!"
#endif
#if !defined(FFAT_WCTOMB)
	#error "Check configuration, FFAT_WCTOMB does not defined!!"
#endif
#if !defined(FFAT_MBSTOWCS)
	#error "Check configuration, FFAT_MBSTOWCS does not defined!!"
#endif
#if !defined(FFAT_WCSTOMBS)
	#error "Check configuration, FFAT_WCSTOMBS does not defined!!"
#endif


#ifdef FFAT_MAX_NUMERIC_TAIL
	#if (FFAT_MAX_NUMERIC_TAIL == 0)
		#error "check FFAT_MAX_NUMERIC_TAIL configuration, it must not be 0"
	#endif
#endif


#if !defined(FFAT_OTHERS_EXECUTE)
	#error "Check configuration, FFAT_OTHERS_EXECUTE does not defined!!"
#endif
#if !defined(FFAT_OTHERS_WRITE)
	#error "Check configuration, FFAT_OTHERS_WRITE does not defined!!"
#endif
#if !defined(FFAT_OTHERS_READ)
	#error "Check configuration, FFAT_OTHERS_READ does not defined!!"
#endif
#if !defined(FFAT_GROUP_EXECUTE)
	#error "Check configuration, FFAT_GROUP_EXECUTE does not defined!!"
#endif
#if !defined(FFAT_GROUP_WRITE)
	#error "Check configuration, FFAT_GROUP_WRITE does not defined!!"
#endif
#if !defined(FFAT_GROUP_READ)
	#error "Check configuration, FFAT_GROUP_READ does not defined!!"
#endif
#if !defined(FFAT_OWNER_EXECUTE)
	#error "Check configuration, FFAT_OWNER_EXECUTE does not defined!!"
#endif
#if !defined(FFAT_OWNER_WRITE)
	#error "Check configuration, FFAT_OWNER_WRITE does not defined!!"
#endif
#if !defined(FFAT_OWNER_READ)
	#error "Check configuration, FFAT_OWNER_READ does not defined!!"
#endif


// debug begin
// =============================================================================
//	Design & Coding Principal.
//
//	-. Carefully check your code
//		- 1st, design
//		- 2nd, design review by whole team members
//		- 3rd. coding & testing
//		- 4th. code inspection
//		- 5th. merge to main code
//
//	-. be sure your code uses BTFS Coding Standard.
//		- strictly check it.
//
//	-. Do not use dynamic memory allocation
//		-. use PSTACK
//		-. dynamic memory allocation is allowed on below case.
//			. while FFAT_Init().
//			. PSTack creation.
//
//	-. Do not bit or mathematical operation on enum type variable
//		- SYMBIAN does now allow these operation
//
//	-. Role and Work
//		- focus on ROLE before make a new function
//		- never reference internal data type of another module.
//			. make some new interfaces to get the data.
//
//	-. No error No warning~
//
//

#ifdef ESS_BENCHMARK
	#ifdef FFAT_DEBUG
		#error invalid configuration, current configuration is benchmark mode. check FFAT_DEBUG definition
	#endif
#endif

#if (FFAT_MAX_NODE_COUNT <= 0)
	#error	"invalid FFAT_MAX_NODE_COUNT configuration"
#endif
// debug end

#endif	/* _FFAT_CONFIG_H_ */

