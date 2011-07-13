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
 * @file        rfs_linux_debug.h
 * @brief       This file includes debug operations for glue layer.
 * @version     RFS_3.0.0_b035_RC
 * @see         none
 * @author      hayeong.kim@samsung.com
 */

#ifndef __RFS_LINUX_DEBUG_H__
#define __RFS_LINUX_DEBUG_H__

/*
 * for debugging LinuxGlue
 */

/* Critical Error Message: printout without condition */
#define DPRINTK(format, args...)			\
	do										\
	{										\
		printk(KERN_CRIT "[%s: %d] " format "\n", __func__, __LINE__, ##args);	\
	} while (0)

#undef DEBUG

#undef DEBUG_LINUX

#if defined(TFS5_DEBUG)

#define LINUX_BUG(format, args...)		\
	do									\
	{									\
		DPRINTK(format, ##args);		\
		BUG();							\
	} while (0)

#define LINUX_BUG_ON(condition)			\
	do									\
	{									\
		BUG_ON(condition);				\
	} while (0)

#else

#define DEBUG_LINUX(n, arg...)		do { } while (0)
#define LINUX_BUG(format, args...)	DPRINTK(format, ##args)
#define LINUX_BUG_ON(condition)		do { } while (0)

#endif /* TFS5_DEBUG */

/*
 * Apply Debug Zone (2008/10/20)
 *
 * NSD_PMZ (panic message: D0)
 * NSD_CMZ (critical message: D1)
 * NSD_EMZ (error message: D2)
 * NSD_SMZ (system message: D3)
 * NSD_IMZ (information: D4)
 * NSD_DMZ (debug message: D5)
 * NSD_VMZ (verbose message: D6)
 * NSD_ASSERT (assert)
 */
#include "nsd_base.h"
#include "linux_debug.h"

#define LNX_PANIC(msg) RFS_PMZ(msg)
#define LNX_CMZ(msg) RFS_CMZ(msg)
#define LNX_EMZ(msg) RFS_EMZ(msg)
#define LNX_SMZ(msg) RFS_SMZ(msg)
#define LNX_IMZ(msg) RFS_IMZ(msg)
#define LNX_DMZ(msg) RFS_DMZ(msg)
#define LNX_VMZ(msg) RFS_VMZ(msg)

#if defined(TFS5_DEBUG)
#  define LNX_ASSERT_ARG(cond, err)		\
	do									\
	{									\
		if (cond)	break;				\
		LNX_DMZ(("Invalid Parameter"));	\
		RFS_ASSERT(cond);				\
	} while (0)
#  define LNX_ASSERT_ARGV(cond)	LNX_ASSERT_ARG(cond, -EINVAL)
#  define LNX_ASSERT_DEBUG(cond, err)	\
	do									\
	{									\
		RFS_ASSERT(cond);				\
		return err;						\
	} while (0);
#  define LNX_ASSERT_BUGS(cond)		RFS_ASSERT(cond);

#else
#  define LNX_ASSERT_ARG(cond, err)		\
	do									\
	{									\
		if (cond)	break;				\
		DPRINTK("Invalid Parameter");	\
		return err;						\
	} while (0)
#  define LNX_ASSERT_ARGV(cond)		\
	do									\
	{									\
		if (cond)	break;				\
		DPRINTK("Invalid Parameter");	\
		return;						\
	} while (0)
#  define LNX_ASSERT_DEBUG(cond, err)	\
	do									\
	{									\
		if (cond)	return 0;			\
		else		return err;			\
	} while (0);
#  define LNX_ASSERT_BUGS(cond)		RFS_ASSERT(cond);
#endif

#endif /* __RFS_LINUX_DEBUG_H__ */

// end of file
