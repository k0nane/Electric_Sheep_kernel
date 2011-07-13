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
 * @file		ffat_common_io.h
 * @brief		Common library for FFAT_CORE, FFAT_ADDON and FFAT_FFATFS
 * @author		Seo Dong Young(dy76.seo@samsung.com)
 * @version		DEC-18-2007 [DongYoung Seo] First writing
 * @see			None
 */


//=============================================================================
//
// FFAT_SHARE에는 FFAT and ADDON의 공통된 기능들이 구현된다.
//

//
// module prefix "SH"
//

#ifndef _FFAT_SHARE_H_
#define _FFAT_SHARE_H_

// includes

// defines

// type definition

//constant definitions


// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_readWriteCluster(Vol* pVol, Node* pNode, t_uint32 dwCluster,
								t_int8* pBuff, t_int32 dwCount, t_boolean bRead,
								FFatCacheFlag dwFlag, ComCxt* pCxt);

	extern t_int32	ffat_readWritePartialCluster(Vol* pVol, Node* pNode, t_uint32 dwCluster,
								t_int32 dwOffset, t_int32 dwSize, t_int8* pBuff,
								t_boolean bRead, FFatCacheFlag dwFlag, ComCxt* pCxt);

	extern FFatErr	ffat_initCluster(Vol* pVol, Node* pNode, t_uint32 dwCluster, t_int32 dwCount,
								FFatCacheFlag dwFlag, ComCxt* pCxt);

	extern FFatErr	ffat_initPartialCluster(Vol* pVol, Node* pNode, t_uint32 dwCluster,
								t_int32 dwStartOffset, t_int32 dwSize,
								FFatCacheFlag dwFlag, ComCxt* pCxt);

	extern FFatErr	ffat_readWriteSectors(Vol* pVol, Node* pNode, t_uint32 dwSector, t_int32 dwCount,
								t_int8* pBuff, FFatCacheFlag dwFlag, t_boolean bRead, ComCxt* pCxt);
	extern FFatErr	ffat_readWritePartialSector(Vol* pVol, Node* pNode, t_uint32 dwSector, t_int32 dwOffset,
								t_int32 dwSize, t_int8* pBuff, FFatCacheFlag dwFlag,
								t_boolean bRead, ComCxt* pCxt);

	extern FFatErr	ffat_writeDEs(Vol* pVol, t_uint32 dwCluster, t_uint32 dwClusterOfOffset,
								t_int32 dwOffset, t_int8* pBuff, t_int32 dwSize,
								FFatCacheFlag dwFlag, Node* pNode, ComCxt* pCxt);
	extern FFatErr	ffat_deleteDEs(Vol* pVol, t_uint32 dwCluster, t_int32 dwOffset,
								t_uint32 dwClusterOfOffset,
								t_int32 dwCount, t_boolean bIntelligent,
								FFatCacheFlag dwFlag, Node* pNode, ComCxt* pCxt);

// debug begin
#ifdef FFAT_DEBUG
	extern void		ffat_share_printClusterChain(Vol* pVol, t_uint32 dwCluster, ComCxt* pCxt);
	extern FFatErr	ffat_share_checkNodePAL(Node* pNode, ComCxt* pCxt);
	extern FFatErr	ffat_share_checkNodeLastClusterInfo(Node* pNode, ComCxt* pCxt);
	extern FFatErr	ffat_share_checkDE(Node* pNode, t_wchar* psName, FatDeSFN* pDE, ComCxt* pCxt);
	extern FFatErr	ffat_share_checkNodeDeInfo(Node* pNode);
	extern FFatErr	ffat_share_checkClusterOfOffset(Vol* pVol, t_uint32 dwStartCluster,
								t_uint32 dwOffset, t_uint32 dwClusterOfOffset, ComCxt* pCxt);
#endif
// debug end

#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFAT_COMMON_IO_H_ */

