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
 * @file		ffat_addon_debug.h
 * @brief		File Debug Modlue for FFAT
 * @author		GwangOk Go
 * @version		MAY-13-2009 [GwangOk Go]	First writing
 * @see			None
 */

#ifndef _FFAT_ADDON_DEBUG_H_
#define _FFAT_ADDON_DEBUG_H_

// includes
#include "ffat_types.h"
#include "ffat_vol.h"

// defines
#ifdef FFAT_DEBUG_FILE
	// TODO - fix lock
	#define FFAT_PRINT_FILE(vol, msg)	do	\
 										{	\
											t_int32		size;	\
											_BTFS_LOCK_CONSOLE();	\
 											if ((size = ffat_debug_copyMsgToBuffer msg) >= 0)		\
 											{	\
 												ffat_debug_writeMsgToFile(vol, size);			\
 											}	\
											_BTFS_UNLOCK_CONSOLE();	\
 										} while(0)
#endif

// enum

// typedefs
typedef struct _DebugFileInfo
{
	t_uint32			dwFirstCluster;
	t_int8*				pHeaderSectorBuffer;
	t_int8*				pCurSectorBuffer;
	t_uint32*			pdwSectorNumbers;
} DebugFileInfo;

// constant definitions

// external variable declarations


// function declarations

#ifdef __cplusplus
extern "C" {
#endif

	extern void		ffat_debug_init(void);
	extern void		ffat_debug_terminate(void);

	extern void		ffat_debug_mount(void* pVol, ComCxt* pCxt);
	extern void		ffat_debug_umount(Vol* pVol);

	extern t_int32	ffat_debug_copyMsgToBuffer(const char* pFmt, ...);
	extern void		ffat_debug_writeMsgToFile(Vol* pVol, t_int32 dwSize);

	extern t_boolean	ffat_debug_isDubugFile(Node* pNode);
	extern FFatErr		ffat_debug_isAccessible(Node* pNode);

#ifdef __cplusplus
	};
#endif

#endif	/* #ifndef _FFAT_ADDON_DEBUG_H_ */
