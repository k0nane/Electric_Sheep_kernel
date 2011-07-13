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
 * @brief	nestle configurations
 * @author	ByungJune Song (byungjune.song@samsung.com)
 * @author	InHwan Choi (inhwan.choi@samsung.com)
 * @file	ns_config.h
 *
 *
 */
#ifndef __NS_CONFIG_H__
#define __NS_CONFIG_H__

#ifdef CONFIG_RTOS
#include "tfs4_config.h"
#endif

#ifdef CONFIG_WINCE
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define NO_CONCURRENCY					1
#define SYSTEM_LEVEL_CONCURRENCY		2
#define VOLUME_LEVEL_CONCURRENCY		3
#define FILE_LEVEL_CONCURRENCY			4

#undef	LITTLE_ENDIAN
#undef	BIG_ENDIAN
#undef	BYTE_ORDER

#define LITTLE_ENDIAN					1
#define BIG_ENDIAN						2

#if !(defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_SYMBIAN) || defined(CONFIG_LINUX))
#error "Choose one operating system what you want. And add OS MACRO such as 'CONFIG_RTOS' in your make file or project file "
#endif 


/*=============================================================================*/
/* RTOS CONFIG																   */
/*=============================================================================*/
#ifdef CONFIG_RTOS

	#undef CONFIG_WINCE
	#undef CONFIG_LINUX
	#undef CONFIG_SYMBIAN

	/* Byte order */
	#if (TFS4_BYTE_ORDER == TFS4_LITTLE_ENDIAN)
		#define BYTE_ORDER					LITTLE_ENDIAN
	#elif (TFS4_BYTE_ORDER == TFS4_LITTLE_ENDIAN)
		#define BYTE_ORDER					BIG_ENDIAN
	#else
		#error "Choose endian"
	#endif

	/* Reserved name */
	#define	RSVD_NAME_COUNT				TFS4_RSVD_NAME_COUNT				/**< the count of reserved names */
	#define	RSVD_NAME_LEN				TFS4_RSVD_NAME_LEN					/**< the length for each name */

	/** volume */
	#define MAX_CONCURRENT_THREADS		TFS4_MAX_CONCURRENT_THREADS			/**< max concurrent threads */
	#define MAX_MOUNT_NAME_LENGTH		5									/**< the length(5) of "/dev" is the one */

	/** File and Directory */
	#define	MAX_DIR_NAME_LENGTH			TFS4_DIR_NAME_MAX_LENGTH			/**< max dir name length */
	#define	MAX_FILE_NAME_LENGTH		TFS4_FILE_NAME_MAX_LENGTH			/**< max file name length */
	#define	MAX_PATH_NAME_LENGTH		TFS4_PATH_NAME_MAX_LENGTH			/**< max path name length */

	#ifdef _RFS_TOOLS
		#undef MAX_DIR_NAME_LENGTH
		#undef MAX_FILE_NAME_LENGTH
		#undef MAX_PATH_NAME_LENGTH

		#include <linux/limits.h>
		
		#define	MAX_DIR_NAME_LENGTH			NAME_MAX		/**< max dir name length */
		#define	MAX_FILE_NAME_LENGTH		NAME_MAX		/**< max name length */
		#define	MAX_PATH_NAME_LENGTH		PATH_MAX		/**< max path length */
	#endif

	/* Path & Path Cache */
	#define	PCACHE_ENTRY_COUNT			TFS4_NUM_PATH_ENTRY					/**< the number of path caches */
	#define	PCACHE_MAX_NAME_LENGTH		TFS4_MAX_NAME_LENGTH_PCACHE			/**< the maximum name length to decide to put the name into pcache */
 
	/* Buffer Cache */
	#define	BCACHE_SIZE					TFS4_BUFFER_CACHE_SIZE				/**< Total buffer cache size */
	#define BCACHE_MIN_BLOCK_SIZE		TFS4_SECTOR_SIZE					/**< sector size (typically 512B) */
	#define	BCACHE_OPTIMAL_IO												/**< optimal I/O size operation */

	/* for static allocation (RTOS additional configuration) */
	#define	MAX_VOLUMES					TFS4_VOLUME_COUNT
	#define	MAX_OPEN_FILES				TFS4_MAX_FILE_OPEN					/**< file + directory(a directory is a file) */
	#define	MAX_LOGICAL_DISKS			((MAX_VOLUMES) + (TFS4_PDEV_COUNT))

	/**< MAX_LOGICAL_DISKS (1 for each physical + 1 for each logical) + MAX_VOLUMES (1 for root) +  */
	/**< PCACHE_ENTRY_COUNT (1 for each pcache) + MAX_OPEN_FILES (maximum to open) + MAX_CONCURRENT_THREAD * 2 (lookup new path + extra)) */
	#define	TOTAL_VNODE_COUNT			((MAX_LOGICAL_DISKS) +				\
										(MAX_VOLUMES) +						\
										(PCACHE_ENTRY_COUNT) +				\
										(MAX_OPEN_FILES) +					\
										((MAX_CONCURRENT_THREADS) * 2))

	/* for concurrent access */
	#define CONCURRENT_LEVEL			NO_CONCURRENCY							/**< no concurrent access, or FILE_LEVEL_CONCURRENCY*/

	/*
	 * Support different codepages.
	 * e.g. TFS4 with cp949 reads symbols from the used card with cp436 from PC
	 */
	#ifdef TFS4_MULTIPLE_CHARSET_SUPPORT
		#define MULTIPLE_CHARSET_SUPPORT
	#endif

	/* stack checking */
	/**< defined : show the usage of the stack, undefined : disable this feature. */
#ifdef TFS5_DEBUG
	#define	CONFIG_STACK_CHECK
#endif

	#ifdef	CONFIG_STACK_CHECK
		#define	CONFIG_PRINT_FUNC_INOUT
//		#define	CONFIG_PRINT_QUIET
//		#define	CONFIG_ENABLE_CALL_STACK
//		#define	CONFIG_ENABLE_PROFILING
	#endif

	/* native filesystem config */

	/* define the native filesystem */
	#define CONFIG_TFS5_BTFS
	#define CONFIG_TFS5_DEVFS

	/* log file name */
	#define	LOG_FILE_NAME				TFS4_LOG_FILE_NAME					/**< log(system) file name */

	#ifdef _RFS_TOOLS
		#undef LOG_FILE_NAME
		#define	LOG_FILE_NAME			"rfs.log"							/**< log(system) file name in RFS tool */
	#endif

	/* hidden protected area */
	#define HPA_ROOT_NAME				TFS4_HPA_ROOT_NAME					/**< the root name of HPA, MAX Length is 12 character */
	#define HPA_MAX_VOLUME_SIZE			TFS4_HPA_MAX_VOLUME_SIZE			/**< The maximum available volume size for HPA (MB) */
	#define HPA_MAX_VOLUME_COUNT		TFS4_HPA_MAX_VOLUME_COUNT			/**< The maximum HPA volume count */

#endif


/*=============================================================================*/
/* WINCE CONFIG																   */
/*=============================================================================*/
#ifdef CONFIG_WINCE

	#ifdef WINCE_DEBUG
		#if !defined(TFS5_DEBUG)
			#define TFS5_DEBUG
		#endif
	#endif

	#define PFS_FS_GLUE_DEBUG_LEVEL		1
	#define PFS_FS_NATIVE_DEBUG_LEVEL	1

	#undef CONFIG_RTOS
	#undef CONFIG_LINUX
	#undef CONFIG_SYMBIAN

	/**< Byte order */
	#define BYTE_ORDER					LITTLE_ENDIAN

	/**< Reserved name */
	#define	RSVD_NAME_COUNT				0					/**< the count of reserved names */
	#define	RSVD_NAME_LEN				1					/**< the length for each name */

	/**< volume */
	#define MAX_CONCURRENT_THREADS		4		
	#define MAX_MOUNT_NAME_LENGTH		64	

	/**< File and Directory */
	#define	MAX_DIR_NAME_LENGTH			259					/**< max dir name length */
	#define MAX_FILE_NAME_LENGTH		259					/**< max file name length */
	#define	MAX_PATH_NAME_LENGTH		259					/**< max path name length */ 
	
	/**< Path & Path Cache */
	#define PCACHE_ENTRY_COUNT			1024				/**< the number of pcaches */
	#define PCACHE_MAX_NAME_LENGTH		50	 				/**< caching name length ( <= MAX_FILE_NAME_LENGTH) */

	/**< Buffer Cache */
	#define BCACHE_SIZE					(3*1024*1024)
	#define BCACHE_MIN_BLOCK_SIZE		(512)				/**< sector size (typically 512B) */

	/**< request for native to use single lock */
	#define NATIVE_SUPPORT_SINGLE_LOCK

	/**< for concurrent access */
	#define CONCURRENT_LEVEL			FILE_LEVEL_CONCURRENCY		/**< multiple threads can access the same file */

	/* native filesystem config */

	/**< define the native filesystem */
	#define CONFIG_TFS5_BTFS

	/* log file name */
	#define	LOG_FILE_NAME				"POCKETFS.SYS"		/**< log(system) file name */

	/* Numeric tail restriction */
	#define	MAX_FAT_NUMERIC_TAIL		999
#endif


/*=============================================================================*/
/* SYMBIAN CONFIG															   */
/*=============================================================================*/
#ifdef CONFIG_SYMBIAN

	#undef CONFIG_RTOS
	#undef CONFIG_LINUX
	#undef CONFIG_WINCE

	/**< Byte order */
	#define BYTE_ORDER					LITTLE_ENDIAN

	/**< Reserved name */
	#define	RSVD_NAME_COUNT				22					/**< the count of reserved names */
	#define	RSVD_NAME_LEN				5					/**< the length for each name */

	/**< volume */
	#define MAX_CONCURRENT_THREADS		4		
	#define MAX_MOUNT_NAME_LENGTH		3

	/**< File and Directory */
	#define	MAX_DIR_NAME_LENGTH			243					/**< max dir name length */
	#define MAX_FILE_NAME_LENGTH		255					/**< max file name length */
	#define	MAX_PATH_NAME_LENGTH		259					/**< max path name length */ 
	
	/**< Path & Path Cache */
	#define PCACHE_ENTRY_COUNT			256
	#define PCACHE_MAX_NAME_LENGTH		260

	/**< Buffer Cache */
	#define BCACHE_SIZE					(64*1024)			/**< Total buffer cache size */
	#define BCACHE_MIN_BLOCK_SIZE		(512)				/**< sector size (typically 512B) */

	/**< for concurrent access */
	#define CONCURRENT_LEVEL			 VOLUME_LEVEL_CONCURRENCY		/**< multiple threads can access the same filesystem */
																		/**< only one thread can access the one volume */

	/* native filesystem config */

	/**< define the native filesystem */
	#define CONFIG_TFS5_BTFS

	/* log file name */
	#define	LOG_FILE_NAME				"unifs.sys"			/**< log(system) file name */
#endif


/*=============================================================================*/
/* LINUX CONFIG																   */
/*=============================================================================*/
#ifdef CONFIG_LINUX
	#include <linux/limits.h>

	#undef CONFIG_RTOS
	#undef CONFIG_WINCE
	#undef CONFIG_SYMBIAN
	
	/**< Byte order */
	#define BYTE_ORDER					LITTLE_ENDIAN

	/**< request for native to use single lock */
	#define NATIVE_SUPPORT_SINGLE_LOCK

	#define	MAX_DIR_NAME_LENGTH			NAME_MAX		/**< max dir name length */
	#define	MAX_FILE_NAME_LENGTH		NAME_MAX		/**< max name length */
	#define	MAX_PATH_NAME_LENGTH		PATH_MAX		/**< max path length */

	#define	RSVD_NAME_COUNT				22
	#define	RSVD_NAME_LEN				8
	#define MAX_CONCURRENT_THREADS		4

	/* for concurrent access */
	#define CONCURRENT_LEVEL			FILE_LEVEL_CONCURRENCY		/**< multiple threads can access the same file */

	/* native filesystem config */

	/**< define the native filesystem */
	#define CONFIG_TFS5_BTFS

	/* log file name */
	#define	LOG_FILE_NAME				"rfs.log"			/**< log(system) file name */

	/* Numeric tail restriction */
	#define	MAX_FAT_NUMERIC_TAIL		2000
#endif

#if (CONCURRENT_LEVEL != SYSTEM_LEVEL_CONCURRENCY) && (CONCURRENT_LEVEL != VOLUME_LEVEL_CONCURRENCY) && \
	(CONCURRENT_LEVEL != FILE_LEVEL_CONCURRENCY) && (CONCURRENT_LEVEL != NO_CONCURRENCY)

#error "Please configure right concurrency level"
#endif

#ifdef __cplusplus
}
#endif


#endif // __NS_CONFIG_H__
// end of file

