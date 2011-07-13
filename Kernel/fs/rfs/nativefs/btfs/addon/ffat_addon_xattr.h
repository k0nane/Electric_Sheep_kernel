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
 * @file		ffat_addon_extended_attribute.h
 * @brief		Extended attribute Module for FFAT
 * @author		InHwan Choi (inhwan.choi@samsung.com)
 * @version		NOV-15-2007 [InHwan Choi] First writing
 * @version		JUN-23-2008 [GwangOk Go] Remove limitation of total x-attr size
 * @see			None
 */

#ifndef _FFAT_ADDON_EA_H_
#define _FFAT_ADDON_EA_H_

// includes

// defines
#define EA_SIG						((t_uint32)(0x1097))	//!< EA main signature

#define EA_TYPE_FLAG_SIZE			1
#define EA_COMPACTION_THRESHOLD		(FFAT_EA_LIST_SIZE_MAX << 1)
#define EA_VCE_BUFF_SIZE			((EA_COMPACTION_THRESHOLD >> 9) * sizeof(FFatVCE))	// cluster size가 512인 경우 가정

#if (FFAT_EA_NAME_SIZE_MAX > 512)
	#error "FFAT_EA_NAME_CHARS_MAX value is too high!"
#endif

#if (FFAT_EA_VALUE_SIZE_MAX > 64 * 1024)
	#error "FFAT_EA_VALUE_SIZE_MAX value is too high!"
#endif

#if (FFAT_EA_ATTR_NUMBER_MAX > 256)
	#error "FFAT_EA_ATTR_NUMBER_MAX value is too high!"
#endif

#if (FFAT_EA_LIST_SIZE_MAX > 128 * 1024)
	#error "FFAT_EA_LIST_SIZE_MAX value is too high!"
#endif

#if ((FFAT_EA_NAME_SIZE_MAX + FFAT_EA_VALUE_SIZE_MAX) > FFAT_EA_LIST_SIZE_MAX)
	#error "FFAT_EA_NAME_CHARS_MAX + FFAT_EA_VALUE_SIZE_MAX is greater than FFAT_EA_LIST_SIZE_MAX!"
#endif

#define FFAT_XATTR_USER_PREFIX				"user."
#define FFAT_XATTR_USER_PREFIX_LEN			(sizeof(FFAT_XATTR_USER_PREFIX) - 1)

#define FFAT_XATTR_SYSTEM_PREFIX			"system."
#define FFAT_XATTR_SYSTEM_PREFIX_LEN		(sizeof(FFAT_XATTR_SYSTEM_PREFIX) - 1)

#define FFAT_XATTR_POSIX_ACL_ACCESS			"system.posix_acl_access"
#define FFAT_XATTR_POSIX_ACL_ACCESS_LEN		(sizeof(FFAT_XATTR_POSIX_ACL_ACCESS) - 1)

#define FFAT_XATTR_POSIX_ACL_DEFAULT		"system.posix_acl_default"
#define FFAT_XATTR_POSIX_ACL_DEFAULT_LEN	(sizeof(FFAT_XATTR_POSIX_ACL_DEFAULT) - 1)

#define FFAT_XATTR_TRUSTED_PREFIX			"trusted."
#define FFAT_XATTR_TRUSTED_PREFIX_LEN		(sizeof(FFAT_XATTR_TRUSTED_PREFIX) - 1)

#define FFAT_XATTR_SECURITY_PREFIX			"security."
#define FFAT_XATTR_SECURITY_PREFIX_LEN		(sizeof(FFAT_XATTR_SECURITY_PREFIX) - 1)

#define FFAT_EA_SIGNATURE			".btfs?xattr."
#define FFAT_EA_SIGNATURE_SIZE		12

// typedefs

//!< EA main
typedef struct
{
	t_uint16	uwSig;					//!< head signature
	t_uint16	uwCrtTime;				//!< Create time of directory entrypEntry
	t_uint16	uwCrtDate;				//!< Create date of directory entry
	t_uint16	uwValidCount;			//!< Valid Attribute Count
	t_uint32	udwTotalSpace;			//!< Used space (including deleted entry)
	t_uint32	udwUsedSpace;			//!< Used space (not including deleted entry)
} EAMain;

//!< EA entry
typedef struct
{
	t_uint8		ubTypeFlag;				//!< EA entry type flag
										//!< do not change the order of this field
	t_uint8		ubNameSpaceID;			//!< EA name space id
	t_uint16	uwNameSize;				//!< EA name length (byte)
	t_uint32	udwEntryLength;			//!< EA entry length (byte)
	t_uint32	udwValueSize;			//!< EA value length (byte)
} EAEntry;

//!< Extended Directory entry
typedef struct _ExtendAttr
{
	t_uint8		szSignature[FFAT_EA_SIGNATURE_SIZE];
	t_uint32	dwEAFirstCluster;		//!< EA first cluster
} ExtendedAttr;

//!< EA information for INODE
typedef struct _EAInfo
{
	t_uint32	dwEAFirstCluster;		//!< EA first cluster
										//!< 0 : NO EA, else : it has valid EA
	t_uint32	dwTotalSize;			//!< Total size of EA
} EAInfo;

typedef signed int	EAEntryFlag;
enum _EAEntryFlag
{
	EA_ENTRY_NONE				= 0x00,		//!< No flag
	EA_ENTRY_USED				= 0x01,		//!< used mark
	EA_ENTRY_DELETE				= 0x02,		//!< delete mark

	EA_ENTRY_DUMMY				= 0x7FFFFFFF
};

// constant definitions

// external variable declarations

// function declarations
#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_ea_mount(Vol* pVol, FFatMountFlag dwFlag, ComCxt* pCxt);
	extern FFatErr	ffat_ea_set(Node* pNode, FFatXAttrInfo* pEAInfo, ComCxt* pCxt);
	extern FFatErr	ffat_ea_get(Node* pNode, FFatXAttrInfo* pEAInfo, t_uint32* pudwValueSize, ComCxt* pCxt);
	extern FFatErr	ffat_ea_delete(Node* pNode, FFatXAttrInfo* pEAInfo, ComCxt* pCxt);
	extern FFatErr	ffat_ea_list(Node* pNode, FFatXAttrInfo* pEAInfo, t_uint32* pudwListSize, ComCxt* pCxt);
	extern FFatErr	ffat_ea_deallocate(Node* pNode, ComCxt* pCxt);
	extern FFatErr	ffat_ea_getCreateTime(Node* pNode, FatDeSFN* pDE,
								t_uint16* puwCrtDate, t_uint16* puwCrtTime, ComCxt* pCxt);
	extern FFatErr	ffat_ea_getAllocSize(Node* pNode, t_uint32* pdwSize, ComCxt* pCxt);
	extern FFatErr	ffat_ea_setStatus(Node* pNode, FFatNodeStatus* pStatus, ComCxt* pCxt);
	extern FFatErr	ffat_ea_getEAFirstCluster(Node* pNode, t_uint32* pudwCluster, ComCxt* pCxt);
	extern void		ffat_ea_renameEA(Node* pNodeSrc, Node* pNodeDes);
	extern FFatErr	ffat_ea_initNode(Node* pNode);

	extern FFatErr	ffat_ea_getRootEAFirstCluster(Vol* pVol, t_uint32* pdwRootEACluster, ComCxt* pCxt);
	extern FFatErr	ffat_ea_setRootEAFirstCluster(Vol* pVol, t_uint32 dwRootEACluster,
												FFatCacheFlag dwCacheFlag, ComCxt* pCxt);
	
	extern FFatErr	ffat_ea_checkEA(Node* pNode, ComCxt* pCxt);

	extern FFatErr	ffat_ea_undoSetEA(Vol* pVol, t_uint32 udwFirstCluster, t_uint32 udwDelOffset,
								t_uint32 udwInsOffset, EAMain* pEAMain, EAEntry* pEAEntry, ComCxt* pCxt);
	extern FFatErr	ffat_ea_redoDeleteEA(Vol* pVol, t_uint32 udwFirstCluster, t_uint32 udwDelOffset,
								EAMain* pEAMain, EAEntry* pEAEntry, ComCxt* pCxt);
	
#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFAT_ADDON_EA_H_ */
