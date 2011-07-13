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
 * @file		ffatfs_types.h
 * @brief		This defines FFatfs types.
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _FFATFS_TYPES_H_
#define _FFATFS_TYPES_H_

// includes
#include "ess_types.h"

#include "ffat_common.h"
#include "ffat_types.h"
#include "ffat_errno.h"

// defines

#define FAT_FAT_MAX_SIZE			(16*1024*1024 - 64*1024)
											//!< the maximum size of FAT area (16MB-64KB)
											//!< refer to http://support.microsoft.com/default.aspx?scid=KB;EN-US;Q314463&
#define FAT_SECTOR_MAX_SIZE			4096	//!< Maximum sector size. - 4KB.
#define FAT_CLUSTER_MAX_SIZE		65536	//!< Maximum cluster size - 64KB
#define FAT_FAT16_VOLUME_SIZE_MAX	2147483648U
											//!< The maximum volume size for FAT16

#define FAT_DE_COUNT_MAX			21		//!< Max Directory Entry Count
#define FAT_DE_LFN_COUNT_MAX		20		//!< Max Long File Name Entry Count
#define FAT_DE_SIZE					32		//!< size of a directory entry in byte
#define FAT_DE_SIZE_BITS			5		//!< bit count for directory entry size
#define FAT_DE_SIZE_MASK			(FAT_DE_SIZE - 1)	//!< mask of directory entry size
#define FAT_DE_MAX					65536	//!< Maximum directory entry count in a directory

#define FAT_DIR_SIZE_MAX			(FAT_DE_SIZE * FAT_DE_MAX)			//!< Max size of a directory 

#define FAT_DE_END_OF_DIR			0x00	//!< no more entry after this
#define FAT_DE_FREE					0xE5	//!< free entry
#define FAT_DE_LAST_LFNE_MASK		0x40	//!< mask for last LFN entry 

#define FAT_DE_CHAR_FOR_KANJI		0x05	//!< character for KANJI lead byte
											//!< what is KANJI ?? Chinese characters used in Japanese writing
											//!< 실제 SFNE의 1st character가 FAT_DE_FREE 일 경우 이 값으로 치환한다.

#define FAT_DE_DOT					".          "	//!< directory의 1st entry에 저장될 dot
#define FAT_DE_DOTDOT				"..         "	//!< directory의 2nd entry에 저장될 dotdot

//!< SFNE의 NTRes 부분에 저장될 값
#define FAT_DE_SFN_ALL_UPPER		0x00	//!< All character at SFNE are upper character
#define FAT_DE_SFN_NAME_UPPER		0x10	//!< characters at name part are upper character
#define FAT_DE_SFN_EXT_UPPER		0x08	//!< characters at ext part are upper character
#define FAT_DE_SFN_ALL_LOWER		0x18	//!< All character at SFNE are lower character

#define FAT_DE_DIRTY_SIZE			0x01	//!< [2009-10-19] To protect the user data of append write.
											//!< DE has dirty old size which is not matched with its cluster count
											//!< After user data is synchronized, this bit will be removed and
											//!< the real size will be updated to DE.
											//!< For the sudden-power-off, if bNTRes of some DE has this bit, 
											//!< the recovery to match between cluster count and size will be performed.
											

#define FAT_DE_SFN_MAX_LENGTH		12		//!< short file name의 최대 길이(dot 포함, Trailing NULL 제외)

#define FAT_SFN_NAME_PART_LEN		8		//!< name part의 길이
#define FAT_SFN_EXT_PART_LEN		3		//!< extension part의 길이
#define FAT_SFN_NAME_CHAR			11		//!< name character count at SFNE
#define FAT_LFN_NAME1_CHAR			5		//!< name1 character count at LFNE
#define FAT_LFN_NAME2_CHAR			6		//!< name2 character count at LFNE
#define FAT_LFN_NAME3_CHAR			2		//!< name3 character count at LFNE
#define FAT_LFN_NAME_CHAR			(FAT_LFN_NAME1_CHAR + FAT_LFN_NAME2_CHAR + FAT_LFN_NAME3_CHAR)

#define FAT_LFN_NAME1_BYTE			(FAT_LFN_NAME1_CHAR * 2)	//!< name1 character count at LFNE
#define FAT_LFN_NAME2_BYTE			(FAT_LFN_NAME2_CHAR * 2)	//!< name2 character count at LFNE
#define FAT_LFN_NAME3_BYTE			(FAT_LFN_NAME3_CHAR * 2)	//!< name3 character count at LFNE

#define FFATFS_FAT16_ROOT_CLUSTER	1		//!< ffatfs root cluster number,
											//!< 실제 FAT12/16에서 root dir의 cluster는 없다.
											//!< 하지만 1로 설정할 경우 FAT12/16의 root라고 가정한다.

#define FAT_DE_IS_DIR(_pDE)			(((_pDE)->bAttr & FFAT_ATTR_DIR) ? FFAT_TRUE : FFAT_FALSE)

//!< volume information macro (_pVI : pointer of FatVolinfo)
#define VI_SS(_pVI)			((_pVI)->dwSectorSize)				//!< get sector size
#define VI_SC(_pVI)			((_pVI)->dwSectorCount)				//!< get sector count
#define VI_SSB(_pVI)		((_pVI)->wSectorSizeBits)			//!< get sector size bits
#define VI_SSM(_pVI)		((_pVI)->wSectorSizeMask)			//!< get sector size mask
#define VI_SPC(_pVI)		((_pVI)->wSectorPerCluster)			//!< get sector count per a cluster
#define VI_SPCB(_pVI)		((_pVI)->wSectorPerClusterBits)		//!< get cluster size bits
#define VI_RSC(_pVI)		((_pVI)->dwRootSectorCount)			//!< get sector count for root (FAT16 only)
#define VI_REC(_pVI)		((_pVI)->wRootEntryCount)			//!< get direcctory entry count for root (FAT16 only)
#define VI_FRS(_pVI)		((_pVI)->dwFirstRootSector)			//!< get first root sector (FAT16 only)
#define VI_LRS(_pVI)		(VI_FRS(_pVI) + VI_RSC(_pVI) - 1)	//!< get last root sector number (FAT16 only)
#define VI_RC(_pVI)			((_pVI)->dwRootCluster)				//!< get cluster number for root dir
#define VI_CS(_pVI)			((_pVI)->dwClusterSize)				//!< get cluster size
#define VI_CSB(_pVI)		((_pVI)->dwClusterSizeBits)			//!< get cluster size in bit count
#define VI_CSM(_pVI)		((_pVI)->dwClusterSizeMask)			//!< get cluster size mask
#define VI_CC(_pVI)			((_pVI)->dwClusterCount)			//!< get cluster count
#define VI_CCPFS(_pVI)		((_pVI)->wClusterPerFatSector)		//!< cluster count per a FAT sector
#define VI_CCPFSB(_pVI)		((_pVI)->wClusterPerFatSectorBits)	//!< cluster count per a FAT sector bit
#define VI_CCPFS_MASK(_pVI)	(VI_CCPFS(_pVI) - 1)				//!< cluster count per a FAT sector mask value
#define VI_FDS(_pVI)		((_pVI)->dwFirstDataSector)			//!< get first data sector
#define VI_VIC(_pVI)		(&((_pVI)->stVolInfoCache))			//!< get volume information cache pointer

#define VI_EOC(_pVI)		((_pVI)->dwEOC)						//!< get EOC mark
#define VI_BAD(_pVI)		((_pVI)->dwBAD)						//!< get BAD mark
#define VI_FFS(_pVI)		((_pVI)->dwFirstFatSector)			//!< get first fat sector
#define VI_LFS(_pVI)		(VI_FFS(_pVI) + (VI_FSC(_pVI) * VI_FC(_pVI))- 1)
																// get last Fat Sector
#define VI_LFSFF(_pVI)		(VI_FFS(_pVI) + VI_FSC(_pVI) - 1)	//!< get last Fat Sector on 1st FAT
																//!< be careful!!!. this sector may not have valid cluster !!!
																//!< user VI_LVFSFF to get the last fat valid sector number
#define VI_LVFSFF(_pVI)		((_pVI)->dwLVFSOFF)					//!< get last Valid Fat Sector on 1st FAT
																//!< the last FAT Sector that has valid cluster
#define VI_FSC(_pVI)		((_pVI)->dwFatSize)					//!< get FAT sector count
#define VI_FC(_pVI)			((_pVI)->dwFatCount)				//!< get FAT count

#define VI_LCN(_pVI)		((_pVI)->dwClusterCount + 1)		//!< get last cluster number

#define	VI_CCLFS(_pVI)		((VI_LCN(_pVI) & VI_CCPFS_MASK(_pVI)) + 1)
																//!< Cluster Count On Last Fat Sector

#define VI_CXT(_pVI)			((_pVI)->pCxt)					//!< get pointer of context
#define VI_SET_CXT(_pVI, _pC)	((_pVI)->pCxt = _pC)			//!< set context
#define VI_DEV(_pVI)			((_pVI)->pDevice)				//!< get pointer for Device Io

#define VI_FCD(_pVI)		(&((_pVI)->stFatCacheDirty))		//!< get pointer for dirty list
#define VI_CACHE(_pVI)		((_pVI)->pFatCache)					//!< get cache pointer
#define VI_FCC(_pVI)		(VIC_FCC(VI_VIC(_pVI)))				//!< get free cluster count count

#define VI_SET_FCH(_pVI, _C)	(VIC_SET_FCH(VI_VIC(_pVI), _C))	//!< set free cluster hint

#define VI_FLAG(_pVI)		((_pVI)->dwVIFlag)					//!< get Mount Flag

#define VIC_FCC(_pVIC)			((_pVIC)->dwFreeClusterCount)	//!< get free cluster count hint
#define VIC_FCH(_pVIC)			((_pVIC)->dwFreeClusterHint)	//!< get free cluster hint

#define VIC_SET_FCH(_pVIC, _C)		((_pVIC)->dwFreeClusterHint = (_C))		//!< set free cluster hint
#define VIC_SET_FCC(_pVIC, _C)		((_pVIC)->dwFreeClusterCount = (_C))	//!< set free cluster count

//!< initialize volume information cache
#define VIC_INIT(_pVIC)			do {	\
									(_pVIC)->dwFreeClusterCount = FFATFS_FREE_CLUSTER_INVALID;	\
									(_pVIC)->dwFreeClusterHint	= FFATFS_FREE_CLUSTER_INVALID;	\
								} while(0)

//!< check free cluster count is valid or not
#define VI_IS_VALID_FCC(_pVI)		((VI_FCC(_pVI) == FFATFS_FREE_CLUSTER_INVALID) ? FFAT_FALSE : FFAT_TRUE)
//!< decrease free cluster count
#define VI_DEC_FCC(_pVI, _C)		(VI_IS_VALID_FCC(_pVI) ? (VI_FCC(_pVI) -= _C) : 0)
//!< increase free cluster count
#define VI_INC_FCC(_pVI, _C)		(VI_IS_VALID_FCC(_pVI) ? (VI_FCC(_pVI) += _C) : 0)

// enum

/* directory attribute */
typedef signed int	FatAllocateFlag, FatDeallocateFlag;
enum _FatAllocateFlag
{
	FAT_ALLOCATE_NONE				= 0x00000000,
	FAT_ALLOCATE_GET_FIRST_FREE		= 0x00000001,	//!< get first free cluster, it just returns fist free cluster and does not update FAT
													//!< this is information for log recovery
	FAT_ALLOCATE_INIT_CLUSTER		= 0x00000002,	//!< Initialize cluster to 0x000000
	FAT_ALLOCATE_FORCE				= 0x00000004,	//!< do allocation even if there is some error
	FAT_DEALLOCATE_FORCE			= FAT_ALLOCATE_FORCE,
													//!< deallocate cluster by force

	FAT_ALLOCATE_DIR				= 0x00000100,	//!< allocation for directory
	FAT_ALLOCATE_SECURE				= 0x00002000,	//!< Unlink cluster securely, deallocate시 이것만 거의 필요 

	FAT_DEALLOCATE_DISCARD_CACHE	= 0x00004000,	//!< discard cache for the data area

	FAT_ALLOCATE_DUMMY			= 0x7FFFFFFF
};

typedef signed int	FatDeUpdateFlag;
enum _FatDeUpdateFlag
{
	FAT_UPDATE_DE_NONE		= 0x00,
	FAT_UPDATE_DE_ATIME		= 0x01,		//!< update access time
	FAT_UPDATE_DE_CTIME		= 0x02,		//!< update creation time
	FAT_UPDATE_DE_MTIME		= 0x04,		//!< update modification time
	FAT_UPDATE_DE_ALL_TIME	= (FAT_UPDATE_DE_ATIME | 
									FAT_UPDATE_DE_CTIME | 
									FAT_UPDATE_DE_MTIME),
	FAT_UPDATE_DE_SIZE		= 0x08,		//!< update DE size
	FAT_UPDATE_DE_ATTR		= 0x10,		//!< update DE size
	FAT_UPDATE_DE_CLUSTER	= 0x20,		//!< update cluster information

	FAT_UPDATE_DE_ALL		= (FAT_UPDATE_DE_ALL_TIME |
									FAT_UPDATE_DE_SIZE |
									FAT_UPDATE_DE_ATTR |
									FAT_UPDATE_DE_CLUSTER),

	FAT_UPDATE_DE_WRITE_DE	= 0x40,		//!< write on device
	FAT_UPDATE_DE_FORCE		= 0x80,		//!< Never consider about error like IO error.
	
	FAT_UPDATE_DE_NEED_WRITE= 0x100,	//!< With FAT_UPDATE_DE_WRITE_DE, some case like dirty-size node do not write DE
										//!< With this even the case like dirty-size node, DE writing will be performed.

	FAT_UPDATE_REMOVE_DIRTY	= 0x200,	//!< remove the dirty bit in NTRes and update size

	FAT_UPDATE_ONLY_DE_SIZE	= 0x400,	//!< update DE size only, not node size

	FAT_UPDATE_DE_DUMMY		= 0x7FFFFFFF
};

//!< name generation flag for FFATFS_GenNameFromDirEntry()
typedef signed int	FatGenNameFlag;
enum _FatGenNameFlag
{
	FAT_GEN_NAME_LFN	= 0x01,		//!< generate long file name
	FAT_GEN_NAME_SFN	= 0x02,		//!< generate short file name

	FAT_GEN_NAME_MASK	= FAT_GEN_NAME_LFN | FAT_GEN_NAME_SFN,

	FAT_GEN_NAME_DUMMY	= 0x7FFFFFFF
};

typedef signed int	FatNameType;
enum _FatNameType
{
	// do not use value from 0x00 to 0xFF, reserved for FatAttribute
	FAT_NAME_SFN			= 0x00000100,	//!< short file name , SFNE만 생성
	FAT_NAME_LOWER			= 0x00000200,	//!< lower character is exist
	FAT_NAME_UPPER			= 0x00000400,	//!< upper character is exit
	FAT_NAME_DBCS			= 0x00000800,	//!< double byte character is exit
	FAT_NAME_UNKNOWN		= 0x00001000,	//!< there are some unknown characters.
	FAT_NAME_LFN_CHAR		= 0x00002000,	//!< there are characters must be be used in LFN but not SFN
	FAT_NAME_NUMERIC_TAIL	= 0x00004000,	//!< should append numeric tail to SFNE

	FAT_NAMETYPE_DUMMY		= 0x7FFFFFFF	//!< dummy for 4byte
};

typedef signed int	FatUpdateFlag;
enum _FatUpdateFlag
{
	FAT_UPDATE_NONE		= 0x00,		//!< no flag
	FAT_UPDATE_1ST		= 0x01,		//!< update 1st FAT only
	FAT_UPDATE_2ND		= 0x02,		//!< update 2nd FAT only
	FAT_UPDATE_ALL		= FAT_UPDATE_1ST | FAT_UPDATE_2ND,
	FAT_UPDATE_FORCE	= 0x04,		//!< update cluster by force
									//!< 오류가 있더라도 강제 수행.
									//!< error 후 deallocation에서 사용됨.
	FAT_UPDATE_DUMMY	= 0x7FFFFFFF
};

typedef signed int	FatFSCtlCmd;
enum _FatFSCtlCmd
{
	FAT_FSCTL_DEBUG_BASE	= 0x10000000,		//!< debug command base.
	FAT_FSCTL_IVIC			= 0x10000000,		//!< invalidate volume information cache
	FAT_FSCTL_IS_DIRTY_SECTOR_IN_CACHE
							= 0x10000001,		//!< check the sector is in the cache with dirty state

	FAT_FSCTL_DUMMY			= 0x7FFFFFFF
};

// flags for VolInfo structure
typedef signed int VolInfoFlag;
enum _VolInfoFlag
{
	VI_FLAG_NONE				= 0x00000000,	//!< No flag 
	VI_FLAG_FAT_MIRROR			= 0x00000001,	//!< enable FAT mirroring
	VI_FLAG_CASE_SENSITIVE		= 0x00000002,	//!< (FAT incompatibility) Case sensitive naming rule
	VI_FLAG_OS_SPECIFIC_CHAR	= 0x00000004,	//!< (FAT incompatibility) Support OS specific character set
												//!< and Tail space and dot can be allowed if these are allowed in OS naming rule
	
	VI_FLAG_LFN_ONLY			= (VI_FLAG_CASE_SENSITIVE | VI_FLAG_OS_SPECIFIC_CHAR),
												//!< (FAT incompatibility) Use only long file name
	VI_FLAG_DUMMY				= 0x7FFFFFFF
};

// typedefs and struct


// ***********************************
// Types for FAT FS Spec
typedef struct _FatDeSFN
{
	t_uint8		sName[FAT_SFN_NAME_CHAR];	//!< short name 
	t_uint8		bAttr;						//!< file attributes 
	t_uint8		bNTRes;						//!< Reserved for use by windows NT 
	t_uint8		bCrtTimeTenth;				//!< Millisecond stamp at file creation time 
	t_uint16	wCrtTime;					//!< Time file was created 
	t_uint16	wCrtDate;					//!< Data file was created  
	t_uint16	wLstAccDate;				//!< Last access date  
	t_uint16	wFstClusHi;					//!< High word of this entry's first cluster number 
	t_uint16	wWrtTime;					//!< Time of last write  
	t_uint16	wWrtDate;					//!< Data of last write  
	t_uint16	wFstClusLo;					//!< Low word of this entry's first cluster number 
	t_uint32	dwFileSize;					//!< 32-bit DWORD holding this file's size in byte 
} FatDeSFN;

//!< Directory entry for long file name
typedef struct _FatDeLFN
{
	t_uint8		bOrder;
	t_uint8		sName1[FAT_LFN_NAME1_BYTE];	//!< name part 1
	t_uint8		bAttr;
	t_uint8		bLongType;
	t_uint8		bChecksum;
	t_uint8		sName2[FAT_LFN_NAME2_BYTE];	//!< name part 2
	t_uint16	wFstClusLo;
	t_uint8		sName3[FAT_LFN_NAME3_BYTE];	//!< name part 3
} FatDeLFN;


// *****************************************
// Types for FFATFS


//!< directory entry information for a node.
//!< this structure is for FFATFS_GetNodeDirEntry()
typedef struct _FatGetNodeDe
{
	t_uint32	dwCluster;			//!< [IN] start cluster number of parent
									//!< 1(FFATFS_FAT16_ROOT_CLUSTER) ==> root directory of FAT12/16
	t_uint32	dwOffset;			//!< [IN] node lookup start offset in byte
									//!< offset base is dwCluster

	t_uint32	dwClusterOfOffset;	//!< [IN] node lookup start cluster
									//!< 0, 1 ==> there is no cluster information
									//!< FFATFS will lookup cluster for dwOffset

	t_uint32	dwDeStartCluster;	//!< [OUT] node entry start cluster
	t_uint32	dwDeStartOffset;	//!< [OUT] node entry start offset in byte
									//!< offset base is dwCluster
	t_uint32	dwDeEndCluster;		//!< [OUT] cluster number after lookup
									//!< this pointer may be NULL. 
									//!< FFAT does not store cluster info in this case
	t_uint32	dwDeEndOffset;		//!< [OUT] offset after lookup,
									//!< this pointer may be NULL. FFAT does not store in this case
									//!< offset base is dwCluster
	t_uint32	dwDeSfnCluster;		//!< [OUT] cluster number of SFNE (need to support extended DE)
	t_uint32	dwDeSfnOffset;		//!< [OUT] Offset of SFNE (need to support extended DE)
	FatDeSFN*	pDE;				//!< [IN/OUT] directory entry storage
									//!< it should be 21*32 byte
	t_int32		dwEntryCount;		//!< [OUT] directory entry count (LFNDE + SFNDE)
									//!< Extended DE는 포함되지 않음
	t_int32		dwTotalEntryCount;	//!< [OUT] directory entry count (LFNDE + SFNDE + XDE)
									//!< Extended DE도 포함됨

	//!< lookup condition, Node에 대한 DE를 찾는 조건을 설정한다.
	t_int32		dwTargetEntryCount;	//!< [IN] 찾을 entry의 수. 성능 개선을 위해 사용한다.
									//!< 0 일 경우는 모두 찾는다.
									//!< 1일 경우는 2까지 찾는다. 2일 경우에도 1인 경우도 찾는다.
									//!<   왜냐하면.. SFN일 경우도 LFN을 생성하는 경우가 있다.
									//!< Extended DE는 포함되지 않음
	t_wchar*	psName;				//!< [IN] uppercase name of directory entry to get
									//!< NULL일 경우 모두 찾는다.
	t_int32		dwNameLen;			//!< [IN] target node name length to find
	t_uint8*	psShortName;		//!< [IN] short file name (buffer of SFN entry)
	t_boolean	bExactOffset;		//!< [IN] dwStartOffset에서만 DE를 찾음 (directory의 끝까지 찾지 않음)
} FatGetNodeDe;

struct _FatVolInfo;

typedef FFatErr		(*PFN_CACHE_CALLBACK)(struct _FatVolInfo* pVolInfo, t_uint32 dwSector,
									FFatCacheFlag dwFlag, ComCxt* pCxt);

typedef FFatErr		(*PFN_GET_NEXT_CLUSTER)(struct _FatVolInfo* pVolInfo,
									t_uint32 dwCluster, t_uint32* pdwCluster);
typedef t_boolean	(*PFN_IS_EOF)(t_uint32 dwCluster);
typedef FFatErr		(*PFN_GET_FREE_FROM_TO)(struct _FatVolInfo* pVolInfo,t_uint32 dwFrom,
									t_uint32 dwTo, t_uint32 dwCount,
									FFatVC* pVC, t_uint32* pdwFreeCount, t_boolean bGetMoreCluster);
typedef FFatErr		(*PFN_UPDATE_CLUSTER)(struct _FatVolInfo* pVolInfo, t_uint32 dwCluster,
									t_uint32 dwValue, FFatCacheFlag dwFlag,
									void* pNode);

typedef FFatErr		(*PFN_FAT_CALLBACK)(struct _FatVolInfo* pVolInfo,
									FFatVC* pVC, ComCxt* pCxt);

struct _FFatfsCacheInfo;

typedef struct _FatVolOperation
{
	PFN_GET_NEXT_CLUSTER	pfGetNextCluster;		//!< get next cluster
	PFN_IS_EOF				pfIsEOF;				//!< check EOF
	PFN_GET_FREE_FROM_TO	pfGetFreeFromTo;		//!< get free cluster within some area.
	PFN_UPDATE_CLUSTER		pfUpdateCluster;		//!< update a cluster
} FatVolOperation;

typedef struct _FatVolInfoCache
{
	// volume information cache
	t_uint32	dwFreeClusterCount;		//!< free cluster count
										//!< volume의 free cluster갯수를 저장하고 있는다.
										//!< cluster 할당을 받을 경우 필요한 갯수의 cluster가 있는지 확인한다.
										//!< 초기에는 FFATFS_FREE_CLUSTER_INVALID 값을 갖고 있으며
										//!< 정보가 설정될 경우에는 정상적인 CLUSTER 갯수를 가진다.
	t_uint32	dwFreeClusterHint;		//!< free cluster hint
										//!< 다음의 FREE CLUSTER HINT 정보를 갖고 있다.
										//!< 주의 : 이 값은 실제 FREE CLUSTER 인지는 보장되지 않는다.
										//!< 초기에는 FFATFS_FREE_CLUSTER_INVALID 값을 갖고 있으며
										//!< 정보가 설정된 이후 FREE CLUSTER HINT를 가진다.
} FatVolInfoCache;

typedef struct _FatVolInfo
{
	t_int32				dwSectorSize;			//!< sector size in byte, MAX 64KB

	t_int16				wSectorSizeBits;		//!< sector size bit count
	t_int16				wSectorSizeMask;		//!< sector size mask value
	t_uint32			dwSectorCount;			//!< total sector count

	t_int16				wSectorPerCluster;		//!< cluster size in sector
	t_int16				wSectorPerClusterBits;	//!< cluster size in bits

	t_int16				wClusterPerFatSector;		//!< cluster count per a FAT sector
	t_int16				wClusterPerFatSectorBits;	//!< cluster count per a FAT sector

	t_int32				dwClusterSize;			//!< cluster size in byte
	t_int32				dwClusterSizeMask;		//!< cluster size mask in byte
	t_int32				dwClusterSizeBits;		//!< cluster size bit count

	t_uint32			dwClusterCount;			//!< total cluster count
												//!< 전체 사용가능한 cluster의 수.

	t_uint32			dwEOC;					//!< End of Chain value
	t_uint32			dwBAD;					//!< BAD sector mark
	t_uint32			dwFatSize;				//!< a FAT area size
	t_uint32			dwFatCount;				//!< Count of FAT
	t_uint32			dwFirstFatSector;		//!< 1'st sector number of FAT, This is same as ReservedSector
	t_uint32			dwLVFSOFF;				//!< the last valid FAT sector number on first FAT
												//!< FAT may have rest area over it's own valid clusters
												//!< this variable has FAT sector number of the last cluster

	t_uint32			dwFirstDataSector;		//!< first data sector number 
												//!< first sector of cluster 2
	t_int32				dwRootSectorCount;		//!< root directory sector count, only for FAT16
	t_uint32			dwFirstRootSector;		//!< first root sector number
	t_uint16			wRootEntryCount;		//!< root directory entry count, only for FAT16
	t_uint16			wTimeStamp;				//!< volume time stamp
	t_uint32			dwRootCluster;			//!< root cluster number, always 0 on FAT16

	VolInfoFlag			dwVIFlag;				//!< volume info flag
	FFatFatType			dwFatType;				//!< FAT type

	FatVolInfoCache			stVolInfoCache;		//!< volume cache
	const FatVolOperation*	pVolOp;				//!< volume operation function pointers

	// FFatfsCache
	struct _FFatfsCacheInfo*	pFatCache;		//!< FatCache pointer

	EssDList			dlFatCacheDirty;		//!< dirty FAT cache list corresponding with this volume

	void*				pDevice;				//!< reserved pointer for FFAT user1
												//!< this will be send for block device IO
												//!< TFS4 1.x sets this to logical device ID
												//!< TFS5 sets this to VCB pointer.
	ComCxt*				pCxt;					//!< pointer for current context
												//!< it hast lock state. memory fool
} FatVolInfo;


// structure for allocation/deallocation operation
typedef struct _FatAllocate
{
	t_int32		dwCount;			//!< allocated/deallocated cluster count
									//!< 0 : there is no information
	t_uint32	dwHintCluster;		//!< free cluster hint

	t_uint32	dwPrevEOF;			//!< [IN] previous End Of File
									//!< 0 으로 설정될 경우는 이전 cluster가 없는 경우이다.
									//!< 0 : there is no previous cluster.
	t_uint32	dwFirstCluster;		//!< first cluster of new chain
	t_uint32	dwLastCluster;		//!< last cluster of new chain

	FFatVC*		pVC;				//!< vectored cluster information
									//!< this will reduce FAT access on write operation
} FatAllocate, FatDeallocate;

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	// None 

#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFATFS_TYPES_H_ */



