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
 * @file		ffat_errno.h
 * @brief		The global configuration for FFAT
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */


#ifndef _FFAT_ERRNO_H_
#define _FFAT_ERRNO_H_

// includes
//	NOTICE.!!!
//	This header should not include any header.


/*!< 
	ERROR NUMBER BRIEF DESCRIPTION

	O or above	: success or partial success
	negative	: error
	==> Do not use positive number for errors

	Each error number occupies a bit.
	Do not assign new error number on already occupied bit.
*/

// defines
typedef signed int	FFatErr;

#define FFAT_DONE			0x00000004	/*!< success nothing to do additional work (special purpose) */
#define FFAT_OK2			0x00000002	/*!< success 2 (special purpose) */
#define FFAT_OK1			0x00000001	/*!< success 1 (special purpose) Never change value of this */
#define FFAT_OK				0x00000000	/*!< success (normal) Never change value of this */

#define FFAT_ENOENT			0x80000001	/*!< no such node (a specific node does not exist) */
#define FFAT_ENOMOREENT		0x80000002	/*!< no such node (any node does not exist after a specific offset) */
#define FFAT_ENOSPC			0x80000004	/*!< not enough free space */
#define FFAT_ENOMEM			0x80000008	/*!< not enough memory */
#define FFAT_EINVALID		0x80000010	/*!< invalid argument */
#define FFAT_EBUSY			0x80000020	/*!< busy state */
#define FFAT_ENOTEMPTY		0x80000040	/*!< not an empty directory */ 
#define FFAT_ENOTDIR		0x80000080	/*!< not a directory */
#define FFAT_EISDIR			0x80000100	/*!< is a directory */
#define FFAT_EEXIST			0x80000200	/*!< already exist node */
#define FFAT_EACCESS		0x80000400	/*!< invalid access */
#define FFAT_ENOSUPPORT		0x80000800	/*!< not support operation */
#define FFAT_EROFS			0x80001000	/*!< read only volume */
#define FFAT_EIO			0x80002000	/*!< IO error */
#define FFAT_ETOOLONG		0x80004000	/*!< too long name */
#define FFAT_EXDEV			0x80008000	/*!< media ejected (time stamp between volume and node is different) */
#define FFAT_EFAT			0x80010000	/*!< meta-data error */
#define FFAT_EPANIC			0x80020000	/*!< critical error */
#define FFAT_EINIT			0x80040000	/*!< not initialized yet */
#define FFAT_EINIT_ALREADY	0x80080000	/*!< already initialized */
#define FFAT_EDIRFULL		0x80100000	/*!< no more free space on the directory */
#define FFAT_ENOXATTR		0x80200000	/*!< no extended attribute */
#define FFAT_EFULLXATTR		0x80400000	/*!< too many extended extended attribute */
#define FFAT_ERANGE			0x80800000	/*!< the size of buffer is too small to hold the result (xattr) */
									/*!< offset is out of file size (map block) */
#define FFAT_EACCLOGFILE	0x81000000	/*!< access to log file */
#define FFAT_ERECOVERYFAIL	0x82000000	/*!< recovery is failed */

// INTERNAL USE
#define FFAT_EEOF			0x84000000	/*!< End Of File there is no more cluster (internal use only: may convert to #define FFAT_EFAT outside) */
#define FFAT_ERR1			0x88000000	/*!< general error type1 */
#define FFAT_EXDE			0xA0000000	/*!< error for extended directory entry */
#define FFAT_EPROG			0xC0000000	/*!< internal programming error */
#define FFAT_NO_VALUE		0x7FFFFFFF	/*!< have no value */


// enum

// typedefs

// constant definitions

// external variable declarations

// function declarations

#ifdef __cplusplus
	extern "C" {
#endif




#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_ERRNO_H_ */



