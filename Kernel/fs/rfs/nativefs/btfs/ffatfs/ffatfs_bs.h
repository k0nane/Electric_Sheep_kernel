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
 * @file		ffatfs_bs.h
 * @brief		This file defines FFATFS BPB module
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFATFS_BS_H_
#define _FFATFS_BS_H_

// includes
#include "ffat_types.h"
#include "ffat_errno.h"

#include "ffatfs_types.h"

// defines
#define BS_FSI_LEAD_SIG				0x41615252	// Lead signature
#define BS_FSI_STRUCT_SIG			0x61417272	// Localized signature
#define BS_FSI_TRAIL_SIG			0xAA550000	// Trail signature

#define BS_SIG0_OFFSET				510
#define BS_SIG1_OFFSET				511
#define BS_SIG0						0x55
#define BS_SIG1						0xAA

#define BS_OEM_NAME					"MSWIN4.1"
#define BS_OEM_NAME_LENGTH			8

#define BS_SIGN_FAT12				"FAT12   "
#define BS_SIGN_FAT16				"FAT16   "
#define BS_SIGN_FAT32				"FAT32   "

#define BPB_MAX_SECTOR_PER_CLUSTER	128
#define BPB_MAX_CLUSTER_SIZE		(32 * 1024)		// Max cluster size, 32Kb

#define BS_FAT1216_OFFSET			36
#define BS_FAT32_OFFSET				36

#define BS_FSI_LEAD_SIG				0x41615252
#define BS_FSI_STRUC_SIG			0x61417272
#define BS_FSI_TRAIN_SIG			0xAA550000

#define BS_FAT12_MAX_CLUSTER_COUNT	4085
#define BS_FAT16_MAX_CLUSTER_COUNT	65525

#define BS_VOLUME_LABEL_MAX_LENGTH	11	// volume name max length
#define BS_FILESYSTEM_TYPE_LENGTH	8	// file system type length

#define FFAT_BPB_RESERVED_SECTOR	32
#define FFAT_BPB_DEFAULT_FAT_COUNT	2
#define FFAT_ROOT_DIR_ENTRY_COUNT	512

// enum
typedef signed int	BSType;
enum _BSType
{
	FAT_JMP_INSTRUCTION0	= 0xEB,
	FAT_JMP_INSTRUCTION2	= 0x90,
	FAT_SIG_OFFSET			= 510,
	FAT_BACKUP_BOOT			= 6,
	FAT_SIGNATURE			= 0xAA55,
	FAT_SIG_FFAT1			= 0x12,
	FAT_SIG_FFAT2			= 0x25,
	FAT_FAT_TYPE_12			= 0x0001,
	FAT_FAT_TYPE_16			= 0x0002,
	FAT_FAT_TYPE_32			= 0x0003,
	FAT_MEDIA_FIXED			= 0xF8,		// fixed media
	FAT_MEDIA_REMOVABLE		= 0xF0,		// removable media
	FAT_SECTOR_PER_TRACK	= 0x10,		// 16
	FAT_NUM_HEADS			= 0x02,		// 2
	FAT_SIG_EXT				= 0x29,

	FAT_BS_TYPE_END			= 0x7FFFFFFF

};

// typedefs
/*  common part of BOOTSECTOR. */
typedef struct
{
	t_uint8		bJmpBoot[3];		// Jump Instruction to boot code
	t_uint8		pOemName[8];		// OEM Name \"MSWIN4.1\"
	t_uint8		wBytesPerSec[2];	// bytes per sector, 512, 1024, 2048, 4096 are possible
	t_uint8		bSectorsPerClus;	// number of sectors per allocation unit
									// must be power of two: 1, 2, 4, ..., 128"
									// nBytesPerSec * nSectorsPerClus <= 32 KB"
	t_uint16	wRsvdSecCnt;		// FAT12, FAT16 --> 1, FAT32 > 0, normally 32
	t_uint8		bNumFATs;			// number of FAT data structure on the volume. 2 is recommended
	t_uint8		wRootEntCnt[2];		// FAT12, FAT16 --> the count of 32-byte directory entries
									// nBytesPerSec x even = nRootEntCnt * 32
									// FAT16 --> 512
									// FAT32 --> 0
	t_uint8		wTotSec16[2];		// old 16-bit total count of sectors on the volume
									// 0 --> TotSec32 should be non-zero
									// FAT32 --> 0
									// > 0 and fits --> TotSec32 = 0
	t_uint8		bMedia;				// 0xF8 --> fixed, 0xF0 --> removable
	t_uint16	wFATSz16;			// count of sectors occupied by one FAT. FAT32 --> 0
	t_uint16	wSecPerTrack;		// sectors per track
	t_uint16	wNumHeads;			// number of heads
	t_uint32	dwHiddSec;			// count of hidden sectors preceding the partition that
									// contains this FAT volume
	t_uint32	dwTotSec32;			// 32-bit total count of sectors on the volume 
} FatBSCommon;


// FAT12/16-specific, start at offset 36
typedef struct
{
	t_uint8		bDrvNum;		// drive number 0x00 for floppy, 0x80 for hard disks
	t_uint8		bReserved1;		// reserved, 0
	t_uint8		bBootSig;		// extended boot signature 0x29
	t_uint8		dwVolID[4];		// Volume serial number
	t_uint8		sVolLab[11];	// Volume label. "NO NAME   "
	t_uint8		sFileSysType[8];// "FAT12  ", "FAT16  ", "FAT    "
} FatBS16;


// FAT32-specific, start at offset 36
typedef struct 
{
	t_uint32	dwFATSz32;		// 32-bit count of sectors occupied by one FAT
	t_uint16	wExtFlags;		// 0-3 : zero-based number of active FAT
								//      only valid if mirroring is disabled
								// 4-6 : Reserved
								// 7   : 0 means the FAT is mirrored at runtime into all FATs
								// 1   : 1 means only one FAT is active; it is the one referenced
								//          in bits 0-3
								// 8-15 : reserved
	t_uint16	wFSVer;			// High Byte : major revision number, Low Byte :
								// minor revision number
								// 0x0000
	t_uint32	dwRootClus;		// Cluster number of the first cluster of the root
								// directory. Usually 2
	t_uint16	wFSInfo;		// Sector number of FSINFO structure in the
								// reserved area. Usually 1
	t_uint16	wBkBootSec;		// > 0 : sector number in the reserved area of
								// the volume of a copy
								// of the boot record. Usually 6.
	t_uint8		sReserved[12];	// Reserved. initialized to zero at format
	t_uint8		bDrvNum;		// drive number 0x00 for floppy, 0x80 for hard disks
	t_uint8		bReserved1;		// reserved, 0
	t_uint8		bBootSig;		// extended boot signature 0x29
	t_uint8		dwVolID[4];		// Volume serial number
	t_uint8		sVolLab[11];	// Volume label. "NO NAME   "
	t_uint8		sFileSysType[8];// "FAT12   ", "FAT16   ", "FAT     "
} FatBS32;

typedef struct 
{
	t_uint32	dwFSI_LeadSig;			// lead signature
	t_uint8		sFSI_Reserved1[480];	// reserved for future expansion
	t_uint32	dwFSI_StrucSig;			// local signature
	t_uint32	dwFSI_Free_Count;		// free cluster count hint
	t_uint32	dwFSI_Nxt_Free;			// next free cluster hint
	t_uint8		sReserved2[12];			// reserved for future expansion
	t_uint32	dwFSI_TrailSig;			// trail signature
} Fat32FSInfo;


// constant definitions

// external variable declarations

// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_fs_bs_mount(FatVolInfo* pVolInfo, FFatLDevInfo* pLDevInfo,
									void* pDev, ComCxt* pCxt, FFatMountFlag dwFlag);

	extern FFatErr	ffat_fs_bs_retrieveCommon(FatVolInfo* pVolInfo, t_int8* pBootSector,
									t_uint32 dwLDevSectorCount, t_int32 dwLDevSectorSizeBits);
	extern FFatErr	ffat_fs_bs_retrieveFat32FSInfo(FatVolInfo* pVolInfo,
									t_int8* pBootSector);

	extern FFatErr	ffat_fs_bs_getBSInfoFromBS(void* pDev, t_int32 dwIOSize,
									t_int32* pdwSectorSize, t_int32* pdwClusterSize,
									t_uint32* pdwFirstDataSector, ComCxt* pCxt);

	extern FFatErr	ffat_fs_bs_getVolLabel(FatVolInfo* pVolInfo, t_wchar* psVolLabel,
									t_int32 dwVolLabelLen);

	extern FFatErr	ffat_fs_bs_isValidBootSector(t_int8* pBootSector);

#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFATFS_BS_H_ */



