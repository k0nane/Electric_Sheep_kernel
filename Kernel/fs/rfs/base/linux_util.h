/******************************************************************************/
/*                                                                            */
/*               COPYRIGHT 2007 SAMSUNG ELECTRONICS CO., LTD.                 */
/*                          ALL RIGHTS RESERVED                               */
/*                                                                            */
/* Permission is hereby granted to licensees of Samsung Electronics Co., Ltd. */
/* products to use or abstract this computer program only in  accordance with */
/* the terms of the NAND FLASH MEMORY SOFTWARE LICENSE AGREEMENT for the sole */
/* purpose of implementing  a product based  on Samsung Electronics Co., Ltd. */
/* products. No other rights to  reproduce, use, or disseminate this computer */
/* program, whether in part or in whole, are granted.                         */
/*                                                                            */
/* Samsung Electronics Co., Ltd. makes no  representation or warranties  with */
/* respect to  the performance  of  this computer  program,  and specifically */
/* disclaims any  responsibility for  any damages,  special or consequential, */
/* connected with the use of this program.                                    */
/*                                                                            */
/******************************************************************************/
/**
 * @brief	linux utility for error, time, wcs/mbs
 * @author
 * @file	linux_util.h
 */

#ifndef __LINUX_UTILITY_H__
#define __LINUX_UTILITY_H__

#include "linux_vcb.h"

/**
 * @brief		convert Nestle error number to linux errno(asm/errno.h)
 */
int 
RtlLinuxError(
	IN	FERROR err);

/**
 * @brief		convert linux errno to Nestle error number
 */
FERROR
RtlNestleError(
	IN	int dwLinuxError);

/**
 * @brief		convert linux time (sec) to Nestle typed time
 */
void
RtlLinuxTimeToSysTime(
	IN	struct timespec*	pLxTime,
	OUT	PSYS_TIME			pTime);

/**
 * @brief		convert wide-character to multibyte char 
 */
int
RtlConvertToMb(
	IN	PVOLUME_CONTROL_BLOCK	pVcb, 
	IN	wchar_t					cWideChar, 
	OUT	char*					pMultiByteChar,
	IN	unsigned int			cbMultiByte);

/**
 * @brief		convert wide-character string to multibyte string (nNameLen: includeing null)
 */
int
RtlConvertToMbs(
	IN	PVOLUME_CONTROL_BLOCK	pVcb, 
	IN	const wchar_t*			pWideCharStr, 
	OUT	char*					pMultiByteStr, 
	IN	unsigned int			cbMultiByte);

/**
 * @brief	convert multi-character to wide char 
 */
int
RtlConvertToWc(
	IN	PVOLUME_CONTROL_BLOCK	pVcb, 
	OUT	wchar_t*				pWideChar, 
	IN	const char*				pMultiByteStr,
	IN	unsigned int			cbMultiByte);

/**
 * @brief	convert multibyte string to wide-character string (nNameLen: includeing null)
 */
int
RtlConvertToWcs(
	IN	PVOLUME_CONTROL_BLOCK	pVcb, 
	OUT	wchar_t*				pWideCharStr,
	IN	unsigned int			cchWideChar,
	IN	const char*				pMultiByteStr, 
	IN	unsigned int			cbMultiByte);

/**
 * @brief	allocate memory for unicode string & convert cstring(multi-bytes) to unicode(wide-char) string
 *
 * return char count of WcsString */
int
RtlGetWcsName(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	const char*				pMbsName,
	IN	unsigned int			cbMbsLen,
	OUT	wchar_t**				ppwszName);

/**
 * @brief	release memory for unicode string allocated by RtlGetWcsName
 */
void
RtlPutWcsName(wchar_t **pMemory);

#endif /* __LINUX_UTILITY_H__ */
