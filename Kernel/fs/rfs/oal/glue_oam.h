/*
 * RFS 3.0 Developed by Flash Software Group.
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
 * @file		glue_oam.h
 * @brief		This file declares the OS abstraction Layer.
 * @version		RFS_3.0.0_b035_RC
 * @see			none
 * @author		hayeong.kim@samsung.com
 */

#ifndef __OAM_H__
#define __OAM_H__

#include <linux/sched.h>
#include <linux/rwsem.h>
#include <linux/slab.h>
#include <asm/atomic.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
#include <linux/semaphore.h>
#else
#include <asm/semaphore.h>
#endif

typedef struct semaphore OAM_SEMAPHORE;
typedef struct task_struct OAM_TASK;

/* redefine */
#undef OamCopyMemory
#undef OamFillMemory
#undef OamCompareMemory

#include <asm/byteorder.h>

#define OAM_LITTLE_ENDIAN	1234	//__LITTLE_ENDIAN
#define OAM_BIG_ENDIAN		4321	//__BIG_ENDIAN

#ifdef __LITTLE_ENDIAN
#define OAM_BYTE_ORDER		OAM_LITTLE_ENDIAN
#elif __BIG_ENDIAN
#define OAM_BYTE_ORDER		OAM_BIG_ENDIAN
#endif

#endif // __OAM_H__

// end of file
