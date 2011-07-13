/**
*   @section Intro Intro
*	This project is to provide a programming base so that we can use it as a 
*	starting point of programming.
*    
*	@section  Copyright    Copyright
*            COPYRIGHT. 2008 SAMSUNG ELECTRONICS CO., LTD.                
*                            ALL RIGHTS RESERVED                              
*                                                                            
*	Permission is hereby granted to licensees of Samsung Electronics Co., Ltd. 
*	products to use or abstract this computer program only in  accordance with 
*	the terms of the NAND FLASH MEMORY SOFTWARE LICENSE AGREEMENT for the sole 
*	purpose of implementing  a product based  on Samsung Electronics Co., Ltd. 
*	products. No other rights to  reproduce, use, or disseminate this computer 
*	program, whether in part or in whole, are granted.                         
*                                                                            
*	Samsung Electronics Co., Ltd. makes no  representation or warranties  with 
*	respect to  the performance  of  this computer  program,  and specifically 
*	disclaims any  responsibility for  any damages,  special or consequential, 
*	connected with the use of this program.                                    
*                                                                            
*	@section Description Description
*	This project is an experimental project to develop a programming base for 
*	allprojects in Flash S/W Group of Samsung Electronics.
*	We will define the basic debugging MACROs, version information, and types.
*
*/

/** 
* @file		nsd_version.h
* @brief	Define Version-related MACROs and Type
* @author	Kwon Moon Sang
* @date		2008/08/26
* @remark	Define Samsung Sung Package Version Information structure type.
* @version	RFS_3.0.0_b035_RC
* @note
*			26-AUG-2008 [Moonsang Kwon]: First writing
*			10-SEP-2008 [Moonsang Kwon]: Add BTFS_NAME_STRING_FULL. 
*				BTFS_VERSION type is changed to include tszName.
*			19-SEP-2008 [Moonsang Kwon]: Change the prefix.
*				Every function and data type are changed so that BTFS or Nsd is
*				appeared first. For example, GetNsdVersion() is changed to
*				NsdGetVersion().
*/

#ifndef __BTFS_VERSION_H__
#define __BTFS_VERSION_H__

#include "nsd_types.h"

/*******************************************************************************
	Define Package Version Here
*******************************************************************************/
///< CONF: Set package version information
#define BTFS_NAME_STRING		"BTFS"					///< Package name
#define BTFS_NAME_STRING_FULL	"BTFS"				///< Full package name
#define BTFS_VERSION_STRING		"BTFS_1.0.1_b042"	///< Package release label

#define BTFS_VERSION_MAJOR		1		///< Major version, 1 ~ 15
#define BTFS_VERSION_MIDDLE		0		///< Middle version, 1 ~ 64
#define BTFS_VERSION_MINOR		1		///< Minor version, 1 ~ 255
#define BTFS_VERSION_PATCH		0		///< Patch version, 1 ~ 15
#define BTFS_VERSION_BUILD		42		///< Build number, 1 ~ 4095


// make package version number
#define BTFS_MAKE_VERSION_NUMBER(major, middle, minor, patch, build) \
		(	\
			(major << 4) |	\
			(middle << 4) |	\
			(minor << 8) |	\
			(patch << 4) |	\
			(build)	\
		)

// package version number
#define BTFS_GET_VERSION_NUMBER	\
	BTFS_MAKE_VERSION(	\
		BTFS_VERSION_MAJOR, \
		BTFS_VERSION_MIDDLE, \
		BTFS_VERSION_MINOR,	\
		BTFS_VERSION_PATCH, \
		BTFS_VERSION_BUILD)

/* BTFS version information */
#ifndef BTFS_VERSION_TYPE
	#define BTFS_VERSION_TYPE		1			///< To suppress warning

	#define BTFS_MAX_NAME_LENGTH	(24)			///< Maximum package name string length
	#define BTFS_MAX_DATE_LENGTH	(24)			///< Maximum date string length
	#define BTFS_MAX_TIME_LENGTH	(16)			///< Maximum time string length
	#define BTFS_MAX_TIMESTAMP_LENGTH	(32)	///< Maximum timestamp string length
	
	typedef struct
		{
			unsigned int	dwMajor;	///< Major version
			unsigned int	dwMiddle;	///< Middle version
			unsigned int	dwMinor;	///< Minor version
			unsigned int	dwPatch;	///< Patch number
			unsigned int	dwBuild;	///< Build number
			TCHAR			tszName[BTFS_MAX_NAME_LENGTH];			///< Package name such as PocketStoreIII
		} BTFS_VERSION, *PBTFS_VERSION, **PPBTFS_VERSION;
	#endif // end of #ifndef BTFS_VERSION_TYPE

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	// Return current package version information
	extern void BtfsGetVersion(char** ppszVersion, wchar_t** ppswzVersion, PPBTFS_VERSION ppNsdVersion);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // end of #ifndef __BTFS_VERSION_H__
