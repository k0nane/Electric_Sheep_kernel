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
 * @file		ffat_common.h
 * @brief		Common library for FFAT_CORE and FFAT_FFATFS
 * @author		Seo Dong Young(dy76.seo@samsung.com)
 * @version		JUL-14-2006 [DongYoung Seo] First writing
 * @see			None
 */


//=============================================================================
//
// FFAT_COMMON에는 FFAT, ADDON and  FFATFS의 공통된 기능들이 구현된다.
//

//
// module prefix "Com"
//

#ifndef _FFAT_COMMON_H_
#define _FFAT_COMMON_H_

// includes
#include "ess_types.h"
#include "ess_pstack.h"

#include "ffat_config.h"
#include "ffat_types.h"

// defines
#define FFAT_MEM_ALIGN				sizeof(t_uint8*)
#define FFAT_MEM_ALIGN_MASK			(FFAT_MEM_ALIGN - 1)
#define FFAT_GET_ADDR(_p)			(t_uint)(_p)

#ifdef USE_64_BIT_ADDRESS
	#undef FFAT_GET_ADDR
	#define FFAT_GET_ADDR(_p)		(t_uint64)(_p)
#endif

//!< get byte aligned address
#define FFAT_GET_ALIGNED_ADDR(_p)	(t_int8*)((FFAT_GET_ADDR(_p) + FFAT_MEM_ALIGN_MASK) & (~FFAT_MEM_ALIGN_MASK))
//!< get byte offset of align
#define FFAT_GET_ALIGN_OFFSET(_p)	(t_uint)(FFAT_GET_ALIGNED_ADDR(_p) - FFAT_GET_ADDR(_p))

//!< Initialize Cache Information structure
#define FFAT_INIT_CI(_pCI, _pNode, _pDev)	do {	\
													(_pCI)->pNode = (void*)_pNode;	\
													(_pCI)->pDev = (void*)_pDev;	\
												} while (0)

//!< check cmd is for addon or not
#define FFAT_IS_FSCTL_CMD_FOR_ADDON(_dwCmd)		((_dwCmd & FFAT_FSCTL_ADDON_BASE) ? FFAT_TRUE : FFAT_FALSE)

#if (FFAT_LOCK_TYPE == FFAT_LOCK_NONE) || (FFAT_LOCK_TYPE == FFAT_LOCK_SINGLE)
	#define FFAT_ATOMIC_INC32(v)		(++(*v))
	#define FFAT_ATOMIC_DEC32(v)		(--(*v))
	#define FFAT_ATOMIC_INC16(v)		(++(*v))
	#define FFAT_ATOMIC_DEC16(v)		(--(*v))
#else
	#define FFAT_ATOMIC_INC32(v)		ffat_atomic_inc32(v)
	#define FFAT_ATOMIC_DEC32(v)		ffat_atomic_dec32(v)
	#define FFAT_ATOMIC_INC16(v)		ffat_atomic_inc16(v)
	#define FFAT_ATOMIC_DEC16(v)		ffat_atomic_dec16(v)
#endif

#if (FFAT_LOCK_TYPE == FFAT_LOCK_MULTIPLE)
	#define FFAT_GET_FREE_LOCK(_ppLock)				ffat_lock_getFreeLock(_ppLock)
	#define FFAT_RELEASE_LOCK(_pLock)				ffat_lock_releaseLock(_pLock)
#else
	#define FFAT_GET_FREE_LOCK(_ppLock)				FFAT_OK; (*_ppLock) = NULL;
	#define FFAT_RELEASE_LOCK(_pLock)				FFAT_OK
#endif

// check error and if err then out
#define FFAT_EOTO(_r, _msg, _where)		do {	\
											IF_UK (_r < 0)		goto _where;	\
										} while (0)


#define FFAT_EO(_r, _msg)	FFAT_EOTO(_r, _msg, out)
// check error and if err then return
#define FFAT_ER(_r, _msg)	do {	\
								IF_UK (_r < 0)	return _r;	\
							} while (0)


//!< for Node Location & VC
#define FFAT_NO_OFFSET		0xFFFFFFFF			// no offset information

//!< for VC
#define VC_NO_OFFSET		FFAT_NO_OFFSET

//!< initialize VC
#define VC_INIT(_pVC, _dwOffset)	FFAT_INIT_VC(_pVC, _dwOffset)

#define VC_O(_pVC)		((_pVC)->dwClusterOffset)		//!< get offset
#define VC_CC(_pVC)		((_pVC)->dwTotalClusterCount)	//!< get total cluster count in VC
#define VC_FC(_pVC)		((_pVC)->pVCE[0].dwCluster)		//!< get first cluster number
#define VC_FCO(_pVC)	VC_O(_pVC)						//!< get first cluster offset
#define VC_VEC(_pVC)	((_pVC)->dwValidEntryCount)		//!< get valid entry count
#define VC_TEC(_pVC)	((_pVC)->dwTotalEntryCount)		//!< get total entry count
#define VC_LEI(_pVC)	(VC_VEC(_pVC) - 1)				//!< get last index

#define VC_IS_FULL(_pVC)			((VC_VEC(_pVC) == VC_TEC(_pVC)) ? FFAT_TRUE : FFAT_FALSE)
#define VC_IS_EMPTY(_pVC)			((VC_VEC(_pVC) == 0) ? FFAT_TRUE : FFAT_FALSE)
#define VC_IS_VALID_OFFSET(_pVC)	((VC_O(_pVC) == VC_NO_OFFSET) ? FFAT_FALSE : FFAT_TRUE)

//!< get last cluster number
#define VC_LC(_pVC)				((_pVC)->pVCE[VC_VEC(_pVC) - 1].dwCluster + (_pVC)->pVCE[VC_VEC(_pVC) - 1].dwCount - 1)

//!< get last cluster number of a Entry
#define VC_LCE(_pVC, _dwId)		((_pVC)->pVCE[(_dwId)].dwCluster + (_pVC)->pVCE[(_dwId)].dwCount - 1)

//!< get last cluster offset
#define VC_LCO(_pVC, _pVol)		((VC_CC(_pVC) > 0) ? ((_pVC)->dwClusterOffset + ((VC_CC(_pVC) - 1) << VOL_CSB(_pVol))) : (_pVC)->dwClusterOffset)

#define VC_INSERT(_pVC, _dwCluster, _dwCount)		\
						do {	\
							if (VC_LC(_pVC) == ((_dwCluster) + 1))	\
							{	\
								(_pVC)->pVCE[VC_LEI(_pVC)].dwCount += (_dwCount);	\
							}	\
							else if (VC_VEC(_pVC) < VC_TEC(_pVC))	\
							{	\
								(_pVC)->dwValidEntryCount++;	\
								(_pVC)->pVCE[VC_LEI(_pVC)].dwCluster = (_dwCluster);	\
								(_pVC)->pVCE[VC_LEI(_pVC)].dwCount = (_dwCount);	\
							}	\
						} while (0)

//!< for FVS
#define VS_VEC(_pVS)		((_pVS)->dwValidEntryCount)	//!< get valid entry count
#define VS_TEC(_pVS)		((_pVS)->dwTotalEntryCount)	//!< get total entry count


// debug begin
#ifdef FFAT_DEBUG
	#undef FFAT_EOTO
	#define FFAT_EOTO(_r, _msg, _where)		\
							do {	\
								if (_r < 0)	\
								{	\
									if (_msg)	\
									{	\
										FFAT_LOG_PRINTF(_msg);	\
									}	\
									goto _where;	\
								}	\
							} while (0)

	#undef FFAT_EO
	#define FFAT_EO(_r, _msg)		FFAT_EOTO(_r, _msg, out)

	#undef FFAT_ER
	#define FFAT_ER(_r, _msg)		\
							do {	\
								if (_r < 0)	\
								{	\
									if (_msg)	\
									{	\
										FFAT_LOG_PRINTF(_msg);	\
									}	\
									return _r;	\
								}	\
							} while (0)
#endif	// end of #ifdef FFAT_DEBUG

// 아직 구현이 덜되었을 경우 사용하는 MACRO
#define NOT_IMPLEMENTED_YET		FFAT_LOG_PRINTF(_T("not implemented yet"))

// debug end

// type definition
typedef t_int32			t_atomic32;		//!< atomic type, this type guarantees atomic increase and decrease
										//!< Advisory lock
typedef t_int16			t_atomic16;		//!< atomic type, this type guarantees atomic increase and decrease
										//!< Advisory lock

typedef signed int	ComFlag;
enum _ComFlag
{
	COM_FLAG_NONE			= 0x00000000,		// no flag
	COM_FLAG_ADDON_LOCKED	= 0x00000001,		//!< ADDON is locked (Mutex)
	COM_FLAG_FFATFS_LOCKED	= 0x00000002,		//!< FFATFS is locked (Mutex)
	COM_FLAG_CORE_LOCKED	= 0x00000004,		//!< CORE is locked (Mutex)
	COM_FLAG_DUMMY			= 0x7FFFFFFF
};

typedef struct
{
	t_atomic32	dwRefCount;				//!< reference count for read lock
	FFatLock*	pLock;					//!< common lock
	FFatLock*	pLockRW;				//!< read/write lock
} ComRWLock;

//!< context for current task
typedef struct
{
	ComFlag			dwFlag;				//!< Status flag
	EssPStack*		pPStack;			//!< PSTACK pointer

	t_int32			dwLockCore;			//!< Count of locking core
	t_int32			dwLockAddon;		//!< count of Locking ADDON

// debug begin
#ifdef FFAT_DEBUG
	t_int32			dwLockCount;		//!< unlock count 
	t_int32			dwUnlockCount;		//!< lock count
#endif
// debug end
} ComCxt;


// debug begin
#ifdef FFAT_DEBUG
	// common debug struct
	typedef struct
	{
		FFatFSCtlCmd		dwCmd;		// FSCtl Command
		void*				pData;		// data for each command
										// this pointer will be type casted to another type
										//	according to dwCmd
	} ComDebug;
#endif
// debug end



//constant definitions

// function declarations

#ifdef __cplusplus
	extern "C" {
#endif

	extern FFatErr	ffat_common_init(void);
	extern FFatErr	ffat_common_terminate(void);

// ============================================================================
// Memory Module
	extern FFatErr	ffat_common_initMem(void);
	extern FFatErr	ffat_common_terminateMem(void);
	extern void*	ffat_common_allocMem(t_int32 dwSize, EssMallocFlag dwFlag);
	extern void		ffat_common_freeMem(void* p, t_int32 dwSize);
	extern void*	ffat_common_localMalloc(t_int32 dwSize, ComCxt* pCxt);
	extern void		ffat_common_localFree(void* p, t_int32 dwSize, ComCxt* pCxt);
	extern FFatErr	ffat_pstack_getFree(t_int32 dwSize, void** ppPStack);
	extern FFatErr	ffat_pstack_getDefault(t_int32 dwSize, void** ppPStack);
	extern FFatErr	ffat_pstack_release(void* pPStack);
// ============================================================================

// ============================================================================
// FFatAtomic Module
	extern FFatErr		ffat_atomic_init(void);
	extern FFatErr		ffat_atomic_terminate(void);
	extern t_atomic32	ffat_atomic_inc32(t_atomic32* pVal);
	extern t_atomic32	ffat_atomic_dec32(t_atomic32* pVal);
	extern t_atomic16	ffat_atomic_inc16(t_atomic16* pVal);
	extern t_atomic16	ffat_atomic_dec16(t_atomic16* pVal);
// ============================================================================

// ============================================================================
// Lock Module
	extern FFatErr	ffat_lock_getFreeLock(FFatLock** ppLock);
	extern FFatErr	ffat_lock_releaseLock(FFatLock** ppLock);
	extern FFatErr	ffat_lock_initRWLock(ComRWLock* pRWLock);
	extern FFatErr	ffat_lock_terminateRWLock(ComRWLock* pRWLock);
	extern FFatErr	ffat_rwlock_getReadLock(ComRWLock* pRWLock);
	extern FFatErr	ffat_rwlock_putReadLock(ComRWLock* pRWLock);
	extern FFatErr	ffat_rwlock_getWriteLock(ComRWLock* pRWLock);
	extern FFatErr	ffat_rwlock_putWriteLock(ComRWLock* pRWLock);
	extern FFatErr	ffat_lock_getAtomic(void);
	extern FFatErr	ffat_lock_putAtomic(void);
	extern FFatErr	ffat_lock_getSpin(void);
	extern FFatErr	ffat_lock_putSpin(void);
// ============================================================================

// ============================================================================
// Context Module
	extern FFatErr	ffat_cxt_create(ComCxt *pCxt);
	extern FFatErr	ffat_cxt_delete(ComCxt *pCxt);
// ============================================================================


// ============================================================================
// Utility functions 
	extern FFatErr	ffat_com_lookupOffsetInVC(t_uint32 dwOffset, FFatVC* pVC, t_int32* pdwIndex,
								t_uint32* pdwCluster, t_int32 dwCSB);
	extern FFatErr	ffat_com_lookupClusterInVC(t_uint32 dwCluster, FFatVC* pVC, t_int32* pdwIndex);
	extern FFatErr	ffat_com_lookupNextClusterInVC(t_uint32 dwCluster, FFatVC* pVC,
								t_int32* pdwIndex, t_uint32* pdwCluster);
	extern void		ffat_com_mergeVC(FFatVC* pVC_To, FFatVC* pVC_From);
	extern void		ffat_com_alignVC(FFatVC* pVC);

	extern t_int32	ffat_common_stricmp(const t_int8* s1, const t_int8* s2);
	extern FFatErr	ffat_common_wcsicmp(const t_wchar* wcs1, const t_wchar* wcs2, t_int32 dwLen);
	extern FFatErr	ffat_common_wcsucmp(const t_wchar* wcs1, const t_wchar* wcs2);
	extern FFatErr	ffat_common_wcsnucmp(const t_wchar* wcs1, const t_wchar* wcs2, t_int32 dwLen);
	extern void		ffat_common_towupper_str(t_wchar* pDesWcs, const t_wchar* pSrcWcs);
	//20100413_sks => Change to add the cluster notification function
	extern void ffat_common_cluster_change_notify(t_int32 dwFreeCount, t_int32 dwTotalCount, t_int32 dwClustersize,void* pDevice);
// ============================================================================

// debug begin
#ifdef FFAT_DEBUG
	extern t_int8*	ffat_debug_w2a(t_wchar* psStr, void* pVol);
	extern t_int8*	ffat_debug_w2a_2nd(t_wchar* psStr, void* pVol);
	extern FFatErr	ffat_debug_printBuffer(t_int8* pBuffer, t_int32 dwOffset, t_int32 dwSize);

	#if (FFAT_LOCK_TYPE != FFAT_LOCK_MULTIPLE)
		extern void ffat_com_debugMemCheckBegin(const char* psFuncName);
		extern void ffat_com_debugMemCheckEnd(void);
	#endif

#endif
// debug end

#ifdef __cplusplus
	};
#endif


// debug begin
#ifdef FFAT_DEBUG
	#ifdef __linux
		#define FFAT_DEBUG_GET_TASK_ID()			current->pid
	#else
		#define FFAT_DEBUG_GET_TASK_ID()			0
	#endif
#endif
// debug end


#endif	/* #ifndef _FFAT_COMMON_H_ */

