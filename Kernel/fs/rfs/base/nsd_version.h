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
* @file		nsd_version.h
* @brief	Define Version-related MACROs and Type
* @author	Kwon Moon Sang
* @date		2008/08/26
* @remark	Define Samsung Sung Package Version Information structure type.
* @version	RFS_3.0.0_b035_RC
* @note
*			26-AUG-2008 [Moonsang Kwon]: First writing
*			10-SEP-2008 [Moonsang Kwon]: Add NSD_NAME_STRING_FULL. 
*				NSD_VERSION type is changed to include tszName.
*			19-SEP-2008 [Moonsang Kwon]: Change the prefix.
*				Every function and data type are changed so that NSD or Nsd is
*				appeared first. For example, GetNsdVersion() is changed to
*				NsdGetVersion().
*/

#ifndef __NSD_VERSION_H__
	#define __NSD_VERSION_H__

#include "nsd_types.h"

/*******************************************************************************
	Define Package Version Here
*******************************************************************************/
///< CONF: Set package version information
#define NSD_NAME_STRING			"NSD"					///< Package name
#define NSD_NAME_STRING_FULL	"Nestle"				///< Full package name
#define NSD_VERSION_STRING		"Nestle_1.0.0_b017_100506"		///< Package release label

#define NSD_VERSION_MAJOR		1		///< Major version, 1 ~ 15
#define NSD_VERSION_MIDDLE		0		///< Middle version, 1 ~ 64
#define NSD_VERSION_MINOR		0		///< Minor version, 1 ~ 255
#define NSD_VERSION_PATCH		0		///< Patch version, 1 ~ 15
#define NSD_VERSION_BUILD		17		///< Build number, 1 ~ 4095


// make package version number
#define NSD_MAKE_VERSION_NUMBER(major, middle, minor, patch, build) \
		(	\
			(major << 4) |	\
			(middle << 4) |	\
			(minor << 8) |	\
			(patch << 4) |	\
			(build)	\
		)

// package version number
#define NSD_GET_VERSION_NUMBER	\
	NSD_MAKE_VERSION(	\
		NSD_VERSION_MAJOR, \
		NSD_VERSION_MIDDLE, \
		NSD_VERSION_MINOR,	\
		NSD_VERSION_PATCH, \
		NSD_VERSION_BUILD)

/* NSD version information */
#ifndef NSD_VERSION_TYPE
	#define NSD_VERSION_TYPE		1			///< To suppress warning

	#define NSD_MAX_NAME_LENGTH	(24)			///< Maximum package name string length
	#define NSD_MAX_DATE_LENGTH	(24)			///< Maximum date string length
	#define NSD_MAX_TIME_LENGTH	(16)			///< Maximum time string length
	#define NSD_MAX_TIMESTAMP_LENGTH	(32)	///< Maximum timestamp string length
	
	typedef struct
		{
			unsigned int	dwMajor;	///< Major version
			unsigned int	dwMiddle;	///< Middle version
			unsigned int	dwMinor;	///< Minor version
			unsigned int	dwPatch;	///< Patch number
			unsigned int	dwBuild;	///< Build number
			TCHAR			tszName[NSD_MAX_NAME_LENGTH];			///< Package name such as PocketStoreIII
		} NSD_VERSION, *PNSD_VERSION, **PPNSD_VERSION;
	#endif // end of #ifndef NSD_VERSION_TYPE

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	// Return current package version information
	extern void NsdGetVersion(char** ppszVersion, wchar_t** ppswzVersion, PPNSD_VERSION ppNsdVersion);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // end of #ifndef __NSD_VERSION_H__
