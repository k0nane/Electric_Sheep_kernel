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
 * @file		ffatfs_de.h
 * @brief		Thils file defines FFatfsDE module
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFATFS_DE_H_
#define _FFATFS_DE_H_

// includes
#include "ffatfs_types.h"

// defines

// enum

// typedefs

// constant definitions

// external variable declarations

// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_fs_de_init(void);

	extern FFatErr	ffat_fs_de_setDeTime(FatDeSFN* pDe, FatDeUpdateFlag dwFlag, FFatTime* pTime);
	extern FFatErr	ffat_fs_de_setDeSize(FatDeSFN* pDE, t_uint32 dwSize);
	extern FFatErr	ffat_fs_de_setDeAttr(FatDeSFN* pDE, t_uint8 bAttr);
	extern FFatErr	ffat_fs_de_setDeCluster(FatDeSFN* pDE, t_uint32 dwCluster);

	extern FFatErr	ffat_fs_de_getNodeDE(FatVolInfo* pVI, FatGetNodeDe* pNodeDE);

	extern t_uint8	ffat_fs_de_genChecksum(FatDeSFN* pDE);

#ifdef FFAT_VFAT_SUPPORT
	extern FFatErr	ffat_fs_de_genLongFileName(FatVolInfo* pVI, FatDeLFN* pDE, t_int32 dwEntryCount, 
									t_wchar* psName, t_int32* pdwLen);
#endif
	extern FFatErr	ffat_fs_de_genShortFileName(FatVolInfo* pVI, FatDeSFN* pDE, t_wchar* psName, t_int32* pdwLen);
	extern FFatErr	ffat_fs_de_genShortVolumeLabel(FatVolInfo* pVI, FatDeSFN* pDE, t_wchar* psName);

	extern FFatErr	ffat_fs_de_adjustNameToFatFormat(FatVolInfo* pVI, t_wchar* psName,	t_int32* pdwLen,
									t_int32* pdwNamePartLen, t_int32* pdwExtPartLen, t_int32* pdwSfnNameSize,
									FatNameType* pdwNameType, FatDeSFN* pDE);
	extern FFatErr	ffat_fs_de_isValidVolumeLabel(t_wchar* psName);
	extern FFatErr	ffat_fs_de_adjustNameToVolumeLabel(FatVolInfo* pVI, t_wchar* psName, t_int32 dwLen, FatDeSFN* pDE);
	extern t_int32	ffat_fs_de_getNumericTail(FatDeSFN* pDE);

#ifdef FFAT_VFAT_SUPPORT
	extern FFatErr	ffat_fs_de_genLFNE(t_wchar* psName, t_int16 wNameLen, FatDeLFN* pDE, 
									t_int32* pdwEntryCount, t_uint8 bCheckSum);
	extern FFatErr	ffat_fs_de_findLFNEsInCluster(FatVolInfo* pVI, t_uint32 dwCluster,
									t_uint32 dwStartOffset, t_uint8 bCheckSum,
									t_uint8* pbPrevLFNOrder, t_uint32* pdwFoundLFNECount, ComCxt* pCxt);
#endif
	extern FFatErr	ffat_fs_de_genSFNE(FatVolInfo* pVI, t_wchar* psName, t_int32 dwNameLen, FatDeSFN* pDE);

	extern t_int32	ffat_fs_de_readWriteOnFat16Root(FatVolInfo* pVI, t_int32 dwOffset,
									t_int8* pBuff, t_int32 dwSize, FFatCacheFlag dwFlag,
									t_boolean bRead, void* pNode);
	extern FFatErr	ffat_fs_de_write(FatVolInfo* pVI, t_uint32 dwCluster, t_int32 dwOffset,
									t_int8* pBuff, t_int32 dwSize, FFatCacheFlag dwFlag,
									void* pNode);
	extern FFatErr	ffat_fs_de_delete(FatVolInfo* pVI, t_uint32 dwCluster, t_int32 dwOffset,
									t_int32 dwCount, t_boolean bLookupDelMark,
									FFatCacheFlag dwFlag, void* pNode);

	extern FFatErr	ffat_fs_de_getVolLabel(FatVolInfo* pVI, t_wchar* psVolLabel, t_int32 dwVolLabelLen);
	extern FFatErr	ffat_fs_de_setVolLabel(FatVolInfo* pVI, t_wchar* psVolLabel, t_boolean bFormat);
	extern FFatErr	ffat_fs_de_removeVolLabel(FatVolInfo* pVI);

	extern FFatErr	ffat_fs_de_removeTrailingDotAndBlank(t_wchar* psName, t_int32* pdwLen);

#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFATFS_DE_H_ */

