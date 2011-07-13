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
* @file			ffat_addon_hpa.c
* @brief		Hidden Protected Area Module for FFAT
* @author		DongYoung Seo(dy76.seo@samsung.com)
* @version		MAY-23-2007 [DongYoung Seo] First writing
* @see			None
*/

#ifndef _FFAT_ADDON_HPA_H_
#define _FFAT_ADDON_HPA_H_

// includes

#include "ess_types.h"
#include "ess_dlist.h"

#include "ffat_types.h"
#include "ffat_node.h"
#include "ffat_vol.h"

#include "ffatfs_types.h"

// defines
#define VOL_HPA(_pVol)				(&((VOL_ADDON(_pVol))->pHPA))

#define HPA_SIG1					((t_uint32)(0x19761225))
#define HPA_SIG2					((t_uint32)(~HPA_SIG1))

#define HPA_BAD_SECTOR_INIT_VALUE	0xFFFFFFFF		//!< bad sector will be filled with this value

// typedefs

//!< The global information for HPA

//!< HPA information sector
typedef struct
{
	t_uint32	dwSig1;				//!< signature 1
	t_uint32	dwHPARootCluster;	//!< cluster number for HPA root directory
	t_uint32	dwFSMCluster;		//!< FAT sector map cluster number
	t_uint32	dwCMCluster;		//!< Cluster map cluster number
	t_int32		dwEntryCount;		//!< Root DE Count
	t_int32		dwDummy[3];			//!< dummy for SFNE alignment by 32byte
	FatDeSFN	pHPARootDE[FFAT_HPA_ROOT_DE_COUNT];
									//!< Root DE
	t_uint32	dwSig2;				//!< signature 2
} HPAInfoSector;

typedef signed int HPAFlag;
enum _HPAFlag
{
	HPA_NONE				= 0x00000000,		//!< No flag
	HPA_FREE_FAT_CHECKED	= 0x00000001,		//!< Free HPA FAT sector releasing work is done
												//!< All free HPA FAT Sector is changed to Normal Area
	HPA_VALID_FREE_BITMAP	= 0x00000002,		//!< Free FAT sector bitmap is available
												//!< this is set after the first update volume status
	HPA_VALID_UPDATE_BITMAP	= 0x00000004,		//!< FAT sector update bitmap is available
												//!< this is set after the first update volume status
	HPA_FAT_UPDATED			= 0x00000008,		//!< some part of FAT area is updated (on cluster alloc/deallocate)
												//!< this is used for clue to check update bitmap
	HPA_NO_FREE_CLUSTER		= 0x00000010,		//!< No more cluster is available for Full HPA
												//!< this is used for clue to allocate clusters for Full HPA
	HPA_MASK				= 0x7FFFFFFF
};

//!< the main structure for HPA
typedef struct _HPAInfo
{
	HPAFlag			dwFlag;					//!< Flag for HPA
	t_uint32		dwInfoCluster;			//!< HPA information cluster number
	t_uint32		dwRootCluster;			//!< HPA root directory cluster number
	t_uint32		dwFSMCluster;			//!< FAT sector bitmap cluster number for Full HPA 
	t_uint32		dwCMCluster;			//!< Cluster bitmap cluster number for Partial HPA 
	t_uint32		dwCMSector;				//!< Cluster bitmap sector number for Partial HPA 
	t_uint32		dwFreeClusterHint;		//!< The next free cluster number hint

	t_uint32		dwTotalClusterCount;	//!< total cluster count on HPA

	FatDeSFN		pRootDe[FFAT_HPA_ROOT_DE_COUNT];
											//!< root directory entry
	t_int32			dwEntryCount;			//!< root directory entry count

	t_uint8*		pBitmapFullHPA;				//!< FAT bitmap to distinguish HPA/Normal
											//!< 0 : Normal, 1: HPA
	t_uint8*		pBitmapPartialHPA;		//!< bitmap for check a Partial HPA Sector which contains both normal and HPA clusters.
											//!< 0: Normal or Full HPA, 1: Partial HPA
	t_uint8*		pBitmapFree;			//!< bitmap for free FAT sector on normal area
											//!< this FAT sector is wholly free
											//!< 1 : free
											//!< this also can be used to expand HPA area.
											//!< but update bitmap(pBitmapUpdated) must be checked before this.
											//!< it may not a free FAT sector even if pBitmapFree for FAT sector n is 1.
											//!<	because the FAT sector updated from free state.
											//!< FAT sector n is free when pBitmapFree is 1 and pBitmapUpdate is 0
	t_uint8*		pBitmapUpdated;			//!< bitmap for FAT updated FAT sector
											//!< this is used to enhance performance getting available space
											//!< CAUTION ! - this is only for checking pBitmapFree
											//!< 0: note updated, 1: updated
	t_uint32		dwFreeFATSectorsNormal;	//!< count of Free FAT Sectors on normal area
											//!< Caution !!, be careful
											//!< this data may be a sanity.
											//!<	it is not the correct one if there is any updated FAT sector
											//!<	use this value after checking updated bitmap

	// FAT Information
	t_uint32		dwLowestFullHPASector;			//!< the lowest Full HPA FAT Sector
											//!< for fast normal area allocation
	t_uint32		dwHighestFreeFullHPASector;	//!< the highest HPA FAT Sector that has free cluster

	EssList			slFree;					//!< list for free management
} HPAInfo;

// constant definitions

// external variable declarations

// function declarations
#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr		ffat_hpa_init(void);
	extern FFatErr		ffat_hpa_terminate(void);
	extern FFatErr		ffat_hpa_ldevIO(Vol* pVol, FFatLDevIO* pLDevIO, ComCxt* pCxt);

	extern	FFatErr		ffat_hpa_mount(Vol* pVol, FFatMountFlag dwFlag, ComCxt* pCxt);
	extern	FFatErr		ffat_hpa_umount(Vol* pVol, ComCxt* pCxt);
	extern FFatErr		ffat_hpa_getVolumeStatus(Vol* pVol, FFatVolumeStatus* pStatus, ComCxt* pCxt);
	extern FFatErr		ffat_hpa_getAvailableClusterCountForNode(Node* pNode,
									t_uint32* pdwCount, ComCxt* pCxt);

	// for log recovery
	extern FFatErr		ffat_hpa_undoHPACreate(Vol* pVol, ComCxt* pCxt);
	extern FFatErr		ffat_hpa_redoHPAShow(Vol* pVol);
	extern FFatErr		ffat_hpa_redoHPARemove(Vol* pVol);

	extern t_boolean	ffat_hpa_isActivated(Vol* pVol);

	extern FFatErr		ffat_hpa_lookup(Node* pNodeParent, Node* pNodeChild,
									t_wchar* psName, t_int32 dwLen, FFatLookupFlag dwFlag,
									FatGetNodeDe* pGetNodeDE);
	extern FFatErr		ffat_hpa_afterLookup(Node* pNodeParent, Node* pNodeChild);
	extern FFatErr		ffat_hpa_initNode(Node* pNodeParent, Node* pNodeChild);

	extern FFatErr		ffat_hpa_removeDir(Node* pNode);
	extern FFatErr		ffat_hpa_rename(Node* pNodeSrc, Node* pNodeDesParent,
									Node* pNodeDes, Node* pNodeDesNew);
	extern FFatErr		ffat_hpa_setStatus(Node* pNode);

	extern FFatErr		ffat_hpa_getFreeClusters(Node* pNode, t_uint32 dwCount, FFatVC* pVC,
									t_uint32 dwHint, t_uint32* pdwFreeCount,
									FatAllocateFlag dwAllocFlag, ComCxt* pCxt);
	extern FFatErr		ffat_hpa_makeClusterChainVCAfter(Node* pNode, FFatVC* pVC, FFatCacheFlag dwFlag, ComCxt* pCxt);
	extern FFatErr		ffat_hpa_deallocateClusters(Node* pNode, t_uint32 dwCluster, t_uint32 dwCount, FFatVC* pVC, FFatCacheFlag dwFlag, ComCxt* pCxt);
	extern FFatErr		ffat_hpa_afterDeallocateCluster(Node* pNode, FFatVC* pVC,
									t_uint32 dwDeallocCount, ComCxt* pCxt);

	extern t_int32		ffat_hpa_getBitmapSize(Vol* pVol);
	extern t_int32		ffat_hpa_getClusterBitmapSize(Vol* pVol);
	extern FFatErr		ffat_hpa_updatePartialHPA(Vol* pVol, Node* pNode, t_uint32 dwFirstCluster, t_uint32 dwCount,
													FFatVC* pVC, t_boolean bBitmapSet, FFatCacheFlag dwFlag, ComCxt* pCxt);

// debug begin
#ifdef FFAT_DEBUG
	extern FFatErr		ffat_hpa_fsctl(FFatFSCtlCmd dwCmd, Vol* pVol);
	extern FFatErr		ffat_hpa_checkNodeDeInfo(Node* pNode);
	extern FFatErr		ffat_hpa_checkTCCOfHPA(Vol* pVol, ComCxt* pCxt);
#endif
// debug end

#ifdef __cplusplus
	};
#endif


#endif	/* #ifndef _FFAT_ADDON_HPA_H_ */



