/*
 * TFS4 2.0 File System Developed by Flash Planning Group.
 *
 * Copyright 2006-2007 by Memory Division, Samsung Electronics, Inc.,
 * San #16, Banwol-Ri, Taean-Eup, Hwasung-City, Gyeonggi-Do, Korea
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
 * @file		ess_unicode.h
 * @brief		ESS UNICODE(UTF16) Library
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _ESS_UNICODE_H_
#define _ESS_UNICODE_H_

// includes
#include <stdio.h>
#include "ess_types.h"

#ifndef WEOF
	#define WEOF	((t_uint16)0xFFFF)
#endif

#ifndef WNULL
	#define WNULL	((t_uint16)0L)
#endif

#define UNI_ISWUPPER(_c)			( uni_iswctype(_c, C2_UPPER) )
#define UNI_ISWLOWER(_c)			( uni_iswctype(_c, C2_LOWER) )
#define UNI_ISWALNUM(_c)			( uni_iswctype(_c, C2_ALPHA | C2_DIGIT) )
#define UNI_ISWALPHA(_c)			( uni_iswctype(_c, C2_ALPHA) )
#define UNI_ISWPRINT(_c)			( uni_iswctype(_c, C2_BLANK | C2_PUNCT | C2_ALPHA | C2_DIGIT))
#define UNI_ISWPUNCT(_c)			( uni_iswctype(_c, C2_PUNCT) )
#define UNI_ISWCNTRL(_c)			( uni_iswctype(_c, C2_CNTRL) )
#define UNI_ISWSPACE(_c)			( uni_iswctype(_c, C2_SPACE) )
#define UNI_ISWDIGIT(_c)			( uni_iswctype(_c, C2_DIGIT) )
#define UNI_ISWXDIGIT(_c)			( uni_iswctype(_c, C2_XDIGIT) )
#define UNI_ISGRAPH(_c)				( uni_iswctype(_c, C2_PUNCT | C2_ALPHA | C2_DIGIT) )
#define UNI_ISWASCII(_c)			( (t_uint32)(_c) < 0x80 )

#define	UNI_ISLEADBYTE(a)			uni_isleadbyte(a, NULL)

#define UNI_TOLOWER(a, b)			uni_tolower(a, b)
#define UNI_TOUPPER(a, b)			uni_toupper(a, b)
#define UNI_TOLOWER_STR(a, b, c)	uni_tolower_str(a, b, c)
#define UNI_TOUPPER_STR(a, b, c)	uni_toupper_str(a, b, c)
#define UNI_WCSLEN(a)				uni_wcslen((const t_uint16*)a)
#define UNI_TOWLOWER(a)				uni_towlower(a)
#define UNI_TOWUPPER(a)				uni_towupper(a)
#define UNI_WCSCPY(a, b)			uni_wcscpy((t_uint16*)a, (const t_uint16*)b)
#define UNI_WCSNCPY(a, b, c)		uni_wcsncpy((t_uint16*)a, (const t_uint16*)b, (t_uint32)c)

#define UNI_WCSCMP(a, b, c)			uni_wcscmp((const t_uint16*)a, (const t_uint16*)b, c)
#define UNI_WCSNCMP(a, b, c, d)		uni_wcsncmp((const t_uint16*)a, (const t_uint16*)b, (t_uint32)c, d)
#define UNI_WCSICMP(a, b, c)		uni_wcsicmp((const t_uint16*)a, (const t_uint16*)b, c)
#define UNI_WCSCHR(a, b, c)			uni_wcschr((const t_uint16*)a, (t_uint16)b, c)

#define UNI_WCSCAT(a, b)			uni_wcscat((t_uint16*)a, (const t_uint16*)b)

#define UNI_WCTOMB(a, b)			uni_wctomb((t_int8*)a, (t_uint16)b)
#define UNI_MBTOWC(a, b, c)			uni_mbtowc((t_uint16*)a, (const t_int8*)b, c)
#define UNI_MBSTOWCS(a, b, c)		uni_mbstowcs((t_uint16*)a,(const t_int8*) b, c)
#define UNI_WCSTOMBS(_mbs, _wcs, c)	uni_wcstombs((t_int8*)_mbs, (const t_uint16*)_wcs, c)
#define UNI_MBSCCH(_mbs)			uni_mbscch((const t_int8*)_mbs)

#define UNI_ITOW					uni_itow
#define UNI_WTOI					uni_wtoi

#define ESS_UNICODE_MAKEWORD(first, second, swap)	(swap) ? (((first) << 8) + (second)) : (((second) << 8) + (first))

typedef enum 
{
	C2_UPPER		= 0x0001,		/* upper case letter */
	C2_LOWER		= 0x0002,		/* lower case letter */
	C2_DIGIT		= 0x0004,		/* digit[0-9] */
	C2_SPACE		= 0x0008,		/* tab, carriage return, newline, */
	/* vertical tab or form feed */
	C2_PUNCT		= 0x0010,		/* punctuation character */
	C2_CNTRL		= 0x0020,		/* control character */
	C2_BLANK		= 0x0040,		/* space char */
	C2_XDIGIT		= 0x0080,		/* hexadecimal digit */
	C2_ALPHA		= 0x0100,		/* alphabetic character */
	C2_LEADBYTE		= 0x8000		/* multi-byte lead byte */
} WCTYPE;

struct cp_info
{
	t_uint32	codepage;
	t_uint32	char_size;
	t_uint16	def_char;
	t_uint16			def_unicode_char;
	t_uint8*	name;
};

struct sbcs_table
{
	struct cp_info	info;
	t_uint16*		cp2uni;
	t_uint8*	uni2cp_low;
	t_uint16*		uni2cp_high;
};

struct dbcs_table
{
	struct cp_info	info;
	const t_uint16*	cp2uni;
	t_uint8*		cp2uni_leadbytes;
	t_uint16*		uni2cp_low;
	t_uint16*		uni2cp_high;
};

union cp_table
{
	struct cp_info		info;
	struct sbcs_table	sbcs;
	struct dbcs_table	dbcs;
};

// external variable declarations


// function declarations

extern	t_uint16	uni_towupper(t_uint16);
extern	t_uint16	uni_towlower(t_uint16);

extern	t_int32		uni_isleadbyte(t_int32 c, t_uint8* plbIndex);
extern	t_int32		uni_iswctype(t_uint16	wc, WCTYPE	mask);

extern	t_int32		uni_tolower(t_int32, t_boolean bByteSwap);
extern	t_int32		uni_toupper(t_int32, t_boolean bByteSwap);
extern	void		uni_toupper_str(t_uint8 *, t_uint32, t_boolean bByteSwap);
extern	void		uni_tolower_str(t_uint8 *, t_uint32, t_boolean bByteSwap);
extern	void		uni_towupper_str(t_uint16 *, t_uint32);
extern	void		uni_towlower_str(t_uint16 *, t_uint32);

extern	t_uint32	uni_wcslen(const t_uint16* wcs);
extern	t_uint16*	uni_wcscpy(t_uint16 *, const t_uint16 *);
extern	t_uint16*	uni_wcsncpy (t_uint16*, const t_uint16*, t_uint32);

extern	t_int32		uni_wcsicmp(const t_uint16 *, const t_uint16 *, t_boolean bByteSwap);
extern	t_int32		uni_wcsncmp(const t_uint16 *, const t_uint16 *, t_uint32, t_boolean bByteSwap);
extern	t_int32		uni_wcscmp(const t_uint16 *, const t_uint16 *, t_boolean bByteSwap);
extern	t_uint16*	uni_wcschr(const t_uint16 *, t_uint16, t_boolean bByteSwap);

extern	t_uint16*	uni_wcscat(t_uint16 *, const t_uint16 *);

extern	t_uint32	uni_mbstowcs(t_uint16 *, const t_int8 *, t_uint32);
extern	t_int32		uni_mbtowc(t_uint16 *, const t_int8 *, t_uint32);
extern	t_uint32	uni_mbscch(const t_int8* mbs);

extern	t_uint32	uni_wcstombs(t_int8* mbs, const t_uint16* wcs, t_uint32 count);
extern	t_int32		uni_wctomb(t_int8 *, t_uint16);

extern	t_int32		uni_wtoi(const t_uint16* pwsz);
extern	t_uint16*	uni_itow(t_int32 value, t_uint16* pwsz);

extern	t_int32 	Unicode_Init(void);

#endif	/* #ifndef _UNICODE_H_*/



