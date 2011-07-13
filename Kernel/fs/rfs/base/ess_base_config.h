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
 * @file		ess_base_config.h
 * @brief		This file has configurations for ESS_BASE
 * @author		DongYoung Seo(dy76.seo@samsung.com)
 * @version		JUL-04-2006 [DongYoung Seo] First writing
 * @see			None
 */

#ifndef _ESS_BASE_CONFIG_H_
#define _ESS_BASE_CONFIG_H_

// debug begin
#ifdef TFS5_DEBUG
	#define ESS_STRICT_CHECK		// Check parameters and variables strictly.
	#define ESS_DEBUG				// debug mode
	#define ESS_STACK_CHECK			// check stack usage
#endif
#ifdef CONFIG_LINUX
	#undef ESS_STRICT_CHECK			// Check parameters and variables strictly.
	#undef ESS_DEBUG				// debug mode
	#undef ESS_STACK_CHECK			// check stack usage
#endif
// debug end

/*
	Define codepage that the target system will use.
	949:KOREAN, 437:US, 850:Multilingual Latin I, 852:Multilingual Latin II,
	855:Cyrillic, 857:Turkish, 858:Multilingual Latin I + Euro,
	862:Hebrew, 866:Russian, 874:Thai, 932:Japanese Shift-JIS,
	936:Simplified Chinese GBK, 950:Traditional Chinese Big5,
	1250:Central Europe, 1251:Cyrillic, 1250:Central Europe,
	1251:Cyrillic, 1252:Latin I, 1253:Greek, 1254:Turkish,
	1255:Hebrew, 1256:Arabic, 1257:Baltic, 1258:Vietnam,
	28591:IS08859_1 Latin 1, 28592:IS08859_2 Latin 2, 28593:IS08859_3 Latin 3,
	28594:IS08859_4 Baltic, 28595:IS08859_5 Cyrillic2, 28596:IS08859_6 Arabic,
	28597:IS08859_7 Greek, 28598:IS08859_8 Hebrew, 28599:IS08859_9 Turkish,
	28605:IS08859_15 Latin 9
	are supported now.
*/
#define ESS_CODEPAGE				949		//!< set code page

#if defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_SYMBIAN)
#define ESS_MEMSET(_p, _v, _s)		memset((_p), (_v), (_s))
#define ESS_STRLEN(_s)				strlen((const char*)_s)
#define ESS_SPRINTF					sprintf
#endif /*defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_SYMBIAN)*/

//// error number
#define ESS_OK				 0x0000
#define ESS_EERROR			-0x0001		// general error
#define ESS_EINVALID		-0x0002		// invalid parameter
#define ESS_EPROG			-0x0004		// programming error
#define ESS_EPANIC			-0x0005		// Critical error
#define ESS_ENOENT			-0x0006		// no entry

//// Global definitions
#define ESS_GET_ENTRY(ptr, type, member) \
			((type *)((t_uint8*)(ptr) - (t_uint)(&((type *)0)->member)))

#if defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_SYMBIAN)
	//// includes for STD C
	#include <stdio.h>
	#include <ctype.h>
	#include <string.h>
	#include <stdarg.h>
#endif /*defined(CONFIG_RTOS) || defined(CONFIG_WINCE) || defined(CONFIG_SYMBIAN)*/

//===============================================================================
//
// DO NOT MODIFY FROM HERE

//// FOR BYTE ALIGNMENT
#if defined(__arm)	// for ADS 1.x
	#define ESS_ALIGN_MACRO(x)		__align(x)
#else
	#define ESS_ALIGN_MACRO(x)
#endif

#if defined(__CC_ARM) && defined(__EDG_VERSION__) && (__EDG_VERSION >= 300)
	// for ARM RealView
	#define ESS_ALIGN_BYTE			8
#else
	#define ESS_ALIGN_BYTE			4
#endif

#define ESS_ALIGN_VAR			ESS_ALIGN_MACRO(ESS_ALIGN_BYTE)

// debug begin
// check for configuration
#ifdef ESS_BENCHMARK
	#if defined(ESS_STRICT_CHECK) || defined(ESS_STRICT_CHECK) || defined(ESS_STRICT_CHECK)
		#error INVALID DEBUG CONFIGURAITON
	#endif
#endif
// debug end

//=============================================================================
/*
	Code Page & UNICODE

	949		: KOREAN
	437		: US
	850		: Multilingual Latin I
	852		: Multilingual Latin II
	855		: Cyrillic
	857		: Turkish
	858		: Multilingual Latin I + Euro
	862		: Hebrew
	866		: Russian
	874		: Thai
	932		: Japanese Shift-JIS
	936		: Simplified Chinese GBK
	949		: Korean
	950		: Traditional Chinese Big5
	1258	: Vietnam
	1250	: Central Europe
	1251	: Cyrillic
	1250	: Central Europe
	1251	: Cyrillic
	1252	: Latin	I
	1253	: Greek
	1254	: Turkish
	1255	: Hebrew
	1256	: Arabic
	1257	: Baltic
	1258	: Vietnam
	28591	: IS08859_1 Latin 1
	28592	: IS08859_2 Latin 2
	28593	: IS08859_3 Latin 3
	28594	: IS08859_4 Baltic
	28595	: IS08859_5 Cyrillic2
	288596	: IS08859_6 Arabic
	28597	: IS08859_7 Greek
	28598	: IS08859_8 Hebrew
	28599	: IS08859_9 Turkish
	28605	: IS08859_15 Latin 9

	reference page : http://www.microsoft.com/globaldev/reference/cphome.mspx

	=============================================================================
	Code Page Table

	Language			Locale							ANSI		OEM
	codepage			codepage
	===========================================================================
	Afrikaans			Afrikaans						1252		850
	Albanian			Albanian						1250		852
	Arabic				Arabic (Algeria)				1256		720
	Arabic				Arabic (Bahrain)				1256		720
	Arabic				Arabic (Egypt)					1256		720
	Arabic				Arabic (Iraq)					1256		720
	Arabic				Arabic (Jordan)					1256		720
	Arabic				Arabic (Kuwait)					1256		720
	Arabic				Arabic (Lebanon)				1256		720
	Arabic				Arabic (Libya)					1256		720
	Arabic				Arabic (Morocco)				1256		720
	Arabic				Arabic (Oman)					1256		720
	Arabic				Arabic (Qatar)					1256		720
	Arabic				Arabic (Saudi Arabia)			1256		720
	Arabic				Arabic (Syria)					1256		720
	Arabic				Arabic (Tunisia)				1256		720
	Arabic				Arabic (U.A.E.)					1256		720
	Arabic				Arabic (Yemen)					1256		720
	Armenian			Armenian						0			1
	Azeri	(Cyrillic)	Azeri (Cyrillic)				1251		866
	Azeri	(Latin)		Azeri (Latin)					1254		857
	Basque				Basque							1252		850
	Belarusian			Belarusian						1251		866
	Bulgarian			Bulgarian						1251		866
	Catalan				Catalan							1252		850
	Chinese				Chinese	(Hong Kong S.A.R.)		950			950
	Chinese				Chinese	(Macau S.A.R.)			950			950
	Chinese				Chinese	(PRC)					936			936
	Chinese				Chinese	(Singapore)				936			936
	Chinese				Chinese	(Taiwan)				950			950
	Croatian			Croatian						1250		852
	Czech				Czech							1250		852
	Danish				Danish							1252		850
	Divehi				Divehi							0			1
	Dutch				Dutch (Belgium)					1252		850
	Dutch				Dutch (Netherlands)				1252		850
	English				English	(Australia)				1252		850
	English				English	(Belize)				1252		850
	English				English	(Canada)				1252		850
	English				English	(Caribbean)				1252		850
	English				English	(Ireland)				1252		850
	English				English	(Jamaica)				1252		850
	English				English	(New Zealand)			1252		850
	English				English	(Philippines)			1252		437
	English				English	(South Africa)			1252		437
	English				English	(Trinidad)				1252		850
	English				English	(United	Kingdom)		1252		850
	English				English	(United	States)			1252		437
	English				English	(Zimbabwe)				1252		437
	Estonian			Estonian						1257		775
	Faroese				Faroese							1252		850
	Farsi				Farsi							1256		720
	Finnish				Finnish							1252		850
	French				French (Belgium)				1252		850
	French				French (Canada)					1252		850
	French				French (France)					1252		850
	French				French (Luxembourg)				1252		850
	French				French (Monaco)					1252		850
	French				French (Switzerland)			1252		850
	FYRO				Macedonian	FYRO Macedonian		1251		866
	Galician			Galician						1252		850
	Georgian			Georgian						0			1
	German				German (Austria)				1252		850
	German				German (Germany)				1252		850
	German				German (Liechtenstein)			1252		850
	German				German (Luxembourg)				1252		850
	German				German (Switzerland)			1252		850
	Greek				Greek							1253		737
	Gujarati			Gujarati						0			1
	Hebrew				Hebrew							1255		862
	Hindi				Hindi							0			1
	Hungarian			Hungarian						1250		852
	Icelandic			Icelandic						1252		850
	Indonesian			Indonesian						1252		850
	Italian				Italian	(Italy)					1252		850
	Italian				Italian	(Switzerland)			1252		850
	Japanese			Japanese						932			932
	Kannada				Kannada							0			1
	Kazakh				Kazakh							1251		866
	Konkani				Konkani							0			1
	Korean				Korean							949			949
	Kyrgyz				Kyrgyz (Cyrillic)				1251		866
	Latvian				Latvian							1257		775
	Lithuanian			Lithuanian						1257		775
	Malay				Malay (Brunei Darussalam)		1252		850
	Malay				Malay (Malaysia)				1252		850
	Marathi				Marathi							0			1
	Mongolian			Mongolian (Cyrillic)			1251		866
	Norwegian(Bokmal)	Norwegian (Bokmal)				1252		850
	Norwegian(Nynorsk)	Norwegian (Nynorsk)				1252		850
	Polish				Polish							1250		852
	Portuguese			Portuguese (Brazil)				1252		850
	Portuguese			Portuguese (Portugal)			1252		850
	Punjabi				Punjabi							0			1
	Romanian			Romanian						1250		852
	Russian				Russian							1251		866
	Sanskrit			Sanskrit						0			1
	Serbian (Cyrillic)	Serbian	(Cyrillic)				1251		855
	Serbian (Latin)		Serbian	(Latin)					1250		852
	Slovak				Slovak							1250		852
	Slovenian			Slovenian						1250		852
	Spanish				Spanish	(Argentina)				1252		850
	Spanish				Spanish	(Bolivia)				1252		850
	Spanish				Spanish	(Chile)					1252		850
	Spanish				Spanish	(Colombia)				1252		850
	Spanish				Spanish	(Costa Rica)			1252		850
	Spanish				Spanish	(Dominican Republic)	1252		850
	Spanish				Spanish	(Ecuador)				1252		850
	Spanish				Spanish	(El	Salvador)			1252		850
	Spanish				Spanish	(Guatemala)				1252		850
	Spanish				Spanish	(Honduras)				1252		850
	Spanish				Spanish	(International Sort)	1252		850
	Spanish				Spanish	(Mexico)				1252		850
	Spanish				Spanish	(Nicaragua)				1252		850
	Spanish				Spanish	(Panama)				1252		850
	Spanish				Spanish	(Paraguay)				1252		850
	Spanish				Spanish	(Peru)					1252		850
	Spanish				Spanish	(Puerto	Rico)			1252		850
	Spanish				Spanish	(Traditional Sort)		1252		850
	Spanish				Spanish	(Uruguay)				1252		850
	Spanish				Spanish	(Venezuela)				1252		850
	Swahili				Swahili							1252		437
	Swedish				Swedish							1252		850
	Swedish				Swedish	(Finland)				1252		850
	Syriac				Syriac							0			1
	Tamil				Tamil							0			1
	Tatar				Tatar							1251		866
	Telugu				Telugu							0			1
	Thai				Thai							874			874
	Turkish				Turkish							1254		857
	Ukrainian			Ukrainian						1251		866
	Urdu				Urdu							1256		720
	Uzbek	(Cyrillic)	Uzbek (Cyrillic)				1251		866
	Uzbek	(Latin)		Uzbek (Latin)					1254		857
	Vietnamese			Vietnamese						1258		1258
*/

#endif	/* #ifndef _ESS_BASE_CONFIG_H_ */


