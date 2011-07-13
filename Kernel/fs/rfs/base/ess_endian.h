/******************************************************************************/
/*                                                                            */
/*               COPYRIGHT 2007 SAMSUNG ELECTRONICS CO., LTD.                 */
/*                          ALL RIGHTS RESERVED                               */
/*                                                                            */
/* Permission is hereby granted to licensees of Samsung Electronics Co., Ltd. */
/* products to use or abstract this computer program only in  accordance with */
/* the terms of the NAND FLASH MEMORY SOFTWARE LICENSE AGREEMENT for the sole */
/* purpose of implementing  a product based  on Samsung Electronics Co., Ltd. */
/* products. No other rights to  reproduce, use, or disseminate this computer */
/* program, whether in part or in whole, are granted.                         */
/*                                                                            */
/* Samsung Electronics Co., Ltd. makes no  representation or warranties  with */
/* respect to  the performance  of  this computer  program,  and specifically */
/* disclaims any  responsibility for  any damages,  special or consequential, */
/* connected with the use of this program.                                    */
/*                                                                            */
/******************************************************************************/
/**
 * @brief	About the byte-ordering
 * @file	ess_endian.h
 * @author	
 */

#ifndef __ESS_ENDIAN_H__
#define	__ESS_ENDIAN_H__

#define BYTE_SWAP_16BIT(x)				\
			((((x) & 0xFF00) >> 8) |		\
			 (((x) & 0x00FF) << 8))

/** Swap the byte-order */
#define	BYTE_SWAP_32BIT(x)				\
			((((x) & 0xFF000000) >> 24) |	\
			 (((x) & 0x00FF0000) >>  8) |	\
			 (((x) & 0x0000FF00) <<  8) |	\
			 (((x) & 0x000000FF) << 24))	 

#define BYTE_SWAP_64BIT(x)				\
			(((x >> 56) | \
			((x >> 40) & 0xff00) | \
			((x >> 24) & 0xff0000) | \
			((x >> 8) & 0xff000000) | \
			((x << 8) & (0xfful << 32)) | \
			((x << 24) & (0xfful << 40)) | \
			((x << 40) & (0xfful << 48)) | \
			((x << 56))))			 

#endif // end of __ENDIAN_H__
