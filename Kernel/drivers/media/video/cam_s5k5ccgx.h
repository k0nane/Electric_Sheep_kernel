#ifndef __S5K5CCGX_H__
#define __S5K5CCGX_H__
/***********************************************************************
* Driver Header for S5K5CCGX (3MP Camera) from SAMSUNG SYSTEM LSI
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
***********************************************************************/

#define MAX_VALUE 0x0A20
#define MAX_VALUE_FLASH 0x069F//10*1024 / 6.04
#define AWB_BLUEOFFSET 0x01f4   //0813

#define MOVIEMODE_FLASH 3
#define FLASHMODE_AUTO	2
#define FLASHMODE_ON	3
#define FLASHMODE_OFF	1


//	AE parameter
#define	ERRSCL_AUTO			0x0284
#define	ERRSCL_NOW			0x0286
#define	AESCL_AUTO			0x0288
#define	AESCL_NOW			0x028A
#define	CAP_HALF_AE_CTRL	0x027D
#define	CAP_GAINOFFSET		0x0282
#define	AEINDEADBAND		0x400B
#define	AEOUTDEADBAND		0x400C
#define	AE_SPEED_INIT		0x402F
#define	AE_OFFSETVAL 0x076c          //0813

//	AWB parameter
#define	RATIO_R_AUTO		0x026A
#define	RATIO_B_AUTO		0x026C
#define	RATIO_R_NOW			0x026E
#define	RATIO_B_NOW			0x0270
#define	HALF_AWB_CTRL		0x028C
#define	STB_CONT_SHIFT_R	0x445C
#define	STB_CONT_SHIFT_B	0x445E
#define	CONT_SHIFT			0x0097

//	AF parameter
#define	AF_STATE			0x6D76

//	SENS STS&MODE
#define	HALF_MOVE_STS	0x0294
#define	MODESEL			0x0011
#define	LED_ON			0x0069
#define	FAST_MODECHG_EN	0x3208

#define	AE_MAXDIFF		5000
#define	N_RATIO			2	//	luminous ratio of pre flash and fullflash 0:x2, 1:x3, 2:x4, 3:x5, 4:x6
#define	AWB_TBLMIN		50

/* Enumeration for AF status */
enum
{
  CAMERA_AF_STATUS_IN_PROGRESS = 0,
  CAMERA_AF_STATUS_SUCCESS,
  CAMERA_AF_STATUS_FAIL,
  CAMERA_AF_STATUS_1ST_SUCCESS,
  CAMERA_AF_STATUS_MAX
};

enum
{
  CAMERA_AF_CMD_START = 0,
  CAMERA_AF_CMD_STOP,
  CAMERA_AF_CMD_CANCEL_STATUS
};

#endif /* __S5K5CCGX_H__ */
