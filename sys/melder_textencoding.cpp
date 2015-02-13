/* melder_textencoding.cpp
 *
 * Copyright (C) 2007-2011,2014 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2007/06/02 utf8 <-> wcs
 * pb 2007/06/14 separated from melder_strings.c and melder_alloc.c
 * pb 2007/06/16 text encoding preferences
 * pb 2007/08/12 prefs in wchar_t
 * pb 2007/09/04 Melder_malloc rather than malloc in Melder_wcsToAscii (had caused an error in counting memory leaks)
 * pb 2007/10/06 Melder_peekWcsToCfstring
 * pb 2007/12/09 made MelderFile_writeCharacter compatible with the ISO Latin-1 preference
 * pb 2007/12/09 made MelderFile_readText ignore null bytes
 * pb 2008/11/05 split off from melder_encodings.c
 * pb 2010/03/08 support for Unicode values above 0xFFFF
 * pb 2011/04/05 C++
 * pb 2011/07/04 C++
 */

#include "melder.h"
#include "Preferences.h"
#include "UnicodeData.h"
#include "abcio.h"

#if defined (macintosh)
	#include "macport_on.h"
    #if useCarbon
        #include <Carbon/Carbon.h>
    #else
        #include <CoreFoundation/CoreFoundation.h>
    #endif
	#include "macport_off.h"
#endif

static struct {
	enum kMelder_textInputEncoding inputEncoding;
	enum kMelder_textOutputEncoding outputEncoding;
} preferences;

void Melder_setInputEncoding (enum kMelder_textInputEncoding encoding) { preferences. inputEncoding = encoding; }
int Melder_getInputEncoding (void) { return preferences. inputEncoding; }

void Melder_setOutputEncoding (enum kMelder_textOutputEncoding encoding) { preferences. outputEncoding = encoding; }
enum kMelder_textOutputEncoding Melder_getOutputEncoding (void) { return preferences. outputEncoding; }

void Melder_textEncoding_prefs (void) {
	Preferences_addEnum (L"TextEncoding.inputEncoding", & preferences. inputEncoding, kMelder_textInputEncoding, kMelder_textInputEncoding_DEFAULT);
	Preferences_addEnum (L"TextEncoding.outputEncoding", & preferences. outputEncoding, kMelder_textOutputEncoding, kMelder_textOutputEncoding_DEFAULT);
}

bool Melder_isValidAscii (const wchar_t *text) {
	for (; *text != '\0'; text ++) {
		if (sizeof (wchar_t) == 2) {
			if ((char16_t) *text > 127) return false;   // make unsigned
		} else {
			if ((char32_t) *text > 127) return false;   // make unsigned
		}
	}
	return true;
}

bool Melder_isValidAscii (const char32_t *text) {
	for (; *text != '\0'; text ++) {
		if (*text > 127) return false;
	}
	return true;
}

bool Melder_isEncodable (const wchar_t *text, int outputEncoding) {
	switch (outputEncoding) {
		case kMelder_textOutputEncoding_ASCII: {
			for (; *text != '\0'; text ++) {
				if (sizeof (wchar_t) == 2) {
					if ((char16_t) *text > 127) return false;   // make unsigned
				} else {
					if ((char32_t) *text > 127) return false;   // make unsigned
				}
			}
			return true;
		} break;
		case kMelder_textOutputEncoding_ISO_LATIN1: {
			for (; *text != '\0'; text ++) {
				if (sizeof (wchar_t) == 2) {
					if ((char16_t) *text > 255) return false;   // make unsigned
				} else {
					if ((char32_t) *text > 255) return false;   // make unsigned
				}
			}
			return true;
		} break;
		case kMelder_textOutputEncoding_UTF8:
		case kMelder_textOutputEncoding_UTF16:
		case kMelder_textOutputEncoding_ASCII_THEN_UTF16:
		case kMelder_textOutputEncoding_ISO_LATIN1_THEN_UTF16: {
			return true;
		}
	}
	return false;
}

bool Melder_strIsValidUtf8 (const char *string) {
	for (const char8_t *p = (const char8_t *) & string [0]; *p != '\0'; p ++) {
		char32_t kar = (char32_t) *p;
		if (kar <= 0x7F) {
			;
		} else if (kar <= 0xC1) {
			return false;
		} else if (kar <= 0xDF) {
			if ((* ++ p & 0xC0) != 0x80) return false;
		} else if (kar <= 0xEF) {
			if ((* ++ p & 0xC0) != 0x80) return false;
			if ((* ++ p & 0xC0) != 0x80) return false;
		} else if (kar <= 0xF4) {
			if ((* ++ p & 0xC0) != 0x80) return false;
			if ((* ++ p & 0xC0) != 0x80) return false;
			if ((* ++ p & 0xC0) != 0x80) return false;
		} else {
			return false;
		}
	}
	return true;
}

long Melder_killReturns_inline (char *text) {
	const char *from;
	char *to;
	for (from = text, to = text; *from != '\0'; from ++, to ++) {
		if (*from == 13) {   // carriage return?
			if (from [1] == '\n') {   // followed by linefeed? Must be a Windows text
				from ++;   // ignore carriage return
				*to = '\n';   // copy linefeed
			} else {   // bare carriage return? Must be a Macintosh text
				*to = '\n';   // change to linefeed
			}
		} else {
			*to = *from;
		}
	}
	*to = '\0';   // closing null byte
	return to - text;
}

template <class CHAR>
long Melder_killReturns_inlineCHAR (CHAR *text) {
	const CHAR *from;
	CHAR *to;
	for (from = text, to = text; *from != '\0'; from ++, to ++) {
		if (*from == 13) {   // carriage return?
			if (from [1] == '\n' || from [1] == 0x0085 /* NextLine */) {   // followed by linefeed? Must be a Windows text
				from ++;   // ignore carriage return
				*to = '\n';   // copy linefeed
			} else {   // bare carriage return? Must be a Macintosh text
				*to = '\n';   // change to linefeed (10)
			}
		} else if (*from == 0x0085 /* NextLine */ || *from == 0x000C /* FormFeed */ ||
		    *from == UNICODE_LINE_SEPARATOR || *from == UNICODE_PARAGRAPH_SEPARATOR)
		{
			*to = '\n';
		} else {
			*to = *from;
		}
	}
	*to = '\0';   // closing null character
	return to - text;
}
long Melder_killReturns_inlineW (wchar_t *text) {
	return Melder_killReturns_inlineCHAR <wchar_t> (text);
}
long Melder_killReturns_inline32 (char32_t *text) {
	return Melder_killReturns_inlineCHAR <char32_t> (text);
}

char32_t Melder_decodeMacRoman [256] = {
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
	 20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
	 40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
	 60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,
	100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
	120, 121, 122, 123, 124, 125, 126, 127,
	UNICODE_LATIN_CAPITAL_LETTER_A_WITH_DIAERESIS, UNICODE_LATIN_CAPITAL_LETTER_A_WITH_RING_ABOVE,
	UNICODE_LATIN_CAPITAL_LETTER_C_WITH_CEDILLA, UNICODE_LATIN_CAPITAL_LETTER_E_WITH_ACUTE,
	UNICODE_LATIN_CAPITAL_LETTER_N_WITH_TILDE, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_DIAERESIS,
	UNICODE_LATIN_CAPITAL_LETTER_U_WITH_DIAERESIS, UNICODE_LATIN_SMALL_LETTER_A_WITH_ACUTE,
	UNICODE_LATIN_SMALL_LETTER_A_WITH_GRAVE, UNICODE_LATIN_SMALL_LETTER_A_WITH_CIRCUMFLEX,
	UNICODE_LATIN_SMALL_LETTER_A_WITH_DIAERESIS, UNICODE_LATIN_SMALL_LETTER_A_WITH_TILDE,
	UNICODE_LATIN_SMALL_LETTER_A_WITH_RING_ABOVE, UNICODE_LATIN_SMALL_LETTER_C_WITH_CEDILLA,
	UNICODE_LATIN_SMALL_LETTER_E_WITH_ACUTE, UNICODE_LATIN_SMALL_LETTER_E_WITH_GRAVE,

	UNICODE_LATIN_SMALL_LETTER_E_WITH_CIRCUMFLEX, UNICODE_LATIN_SMALL_LETTER_E_WITH_DIAERESIS,
	UNICODE_LATIN_SMALL_LETTER_I_WITH_ACUTE, UNICODE_LATIN_SMALL_LETTER_I_WITH_GRAVE,
	UNICODE_LATIN_SMALL_LETTER_I_WITH_CIRCUMFLEX, UNICODE_LATIN_SMALL_LETTER_I_WITH_DIAERESIS,
	UNICODE_LATIN_SMALL_LETTER_N_WITH_TILDE, UNICODE_LATIN_SMALL_LETTER_O_WITH_ACUTE,
	UNICODE_LATIN_SMALL_LETTER_O_WITH_GRAVE, UNICODE_LATIN_SMALL_LETTER_O_WITH_CIRCUMFLEX,
	UNICODE_LATIN_SMALL_LETTER_O_WITH_DIAERESIS, UNICODE_LATIN_SMALL_LETTER_O_WITH_TILDE,
	UNICODE_LATIN_SMALL_LETTER_U_WITH_ACUTE, UNICODE_LATIN_SMALL_LETTER_U_WITH_GRAVE,
	UNICODE_LATIN_SMALL_LETTER_U_WITH_CIRCUMFLEX, UNICODE_LATIN_SMALL_LETTER_U_WITH_DIAERESIS,

	UNICODE_DAGGER, UNICODE_DEGREE_SIGN, UNICODE_CENT_SIGN, UNICODE_POUND_SIGN,
	UNICODE_SECTION_SIGN, UNICODE_BULLET, UNICODE_PILCROW_SIGN, UNICODE_LATIN_SMALL_LETTER_SHARP_S,
	UNICODE_REGISTERED_SIGN, UNICODE_COPYRIGHT_SIGN, UNICODE_TRADE_MARK_SIGN, UNICODE_ACUTE_ACCENT,
	UNICODE_DIAERESIS, UNICODE_NOT_EQUAL_TO,
	UNICODE_LATIN_CAPITAL_LETTER_AE, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_STROKE,

	UNICODE_INFINITY, UNICODE_PLUS_MINUS_SIGN, UNICODE_LESS_THAN_OR_EQUAL_TO, UNICODE_GREATER_THAN_OR_EQUAL_TO,
	UNICODE_YEN_SIGN, UNICODE_MICRO_SIGN, UNICODE_PARTIAL_DIFFERENTIAL, UNICODE_N_ARY_SUMMATION,
	UNICODE_N_ARY_PRODUCT, UNICODE_GREEK_SMALL_LETTER_PI, UNICODE_INTEGRAL,
	UNICODE_FEMININE_ORDINAL_INDICATOR, UNICODE_MASCULINE_ORDINAL_INDICATOR,
	UNICODE_GREEK_CAPITAL_LETTER_OMEGA, UNICODE_LATIN_SMALL_LETTER_AE,
	UNICODE_LATIN_SMALL_LETTER_O_WITH_STROKE,

	UNICODE_INVERTED_QUESTION_MARK, UNICODE_INVERTED_EXCLAMATION_MARK, UNICODE_NOT_SIGN, UNICODE_SQUARE_ROOT,
	UNICODE_LATIN_SMALL_LETTER_F_WITH_HOOK, UNICODE_ALMOST_EQUAL_TO, UNICODE_INCREMENT,
	UNICODE_LEFT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK, UNICODE_RIGHT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK,
	UNICODE_HORIZONTAL_ELLIPSIS, UNICODE_NO_BREAK_SPACE, UNICODE_LATIN_CAPITAL_LETTER_A_WITH_GRAVE,
	UNICODE_LATIN_CAPITAL_LETTER_A_WITH_TILDE, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_TILDE,
	UNICODE_LATIN_CAPITAL_LIGATURE_OE, UNICODE_LATIN_SMALL_LIGATURE_OE,

	UNICODE_EN_DASH, UNICODE_EM_DASH, UNICODE_LEFT_DOUBLE_QUOTATION_MARK, UNICODE_RIGHT_DOUBLE_QUOTATION_MARK,
	UNICODE_LEFT_SINGLE_QUOTATION_MARK, UNICODE_RIGHT_SINGLE_QUOTATION_MARK, UNICODE_DIVISION_SIGN, UNICODE_LOZENGE,
	UNICODE_LATIN_SMALL_LETTER_Y_WITH_DIAERESIS, UNICODE_LATIN_CAPITAL_LETTER_Y_WITH_DIAERESIS,
	UNICODE_FRACTION_SLASH, UNICODE_EURO_SIGN,
	UNICODE_SINGLE_LEFT_POINTING_ANGLE_QUOTATION_MARK, UNICODE_SINGLE_RIGHT_POINTING_ANGLE_QUOTATION_MARK,
	UNICODE_LATIN_SMALL_LIGATURE_FI, UNICODE_LATIN_SMALL_LIGATURE_FL,

	UNICODE_DOUBLE_DAGGER, UNICODE_MIDDLE_DOT,
	UNICODE_SINGLE_LOW_9_QUOTATION_MARK, UNICODE_DOUBLE_LOW_9_QUOTATION_MARK, UNICODE_PER_MILLE_SIGN,
	UNICODE_LATIN_CAPITAL_LETTER_A_WITH_CIRCUMFLEX, UNICODE_LATIN_CAPITAL_LETTER_E_WITH_CIRCUMFLEX,
	UNICODE_LATIN_CAPITAL_LETTER_A_WITH_ACUTE, UNICODE_LATIN_CAPITAL_LETTER_E_WITH_DIAERESIS,
	UNICODE_LATIN_CAPITAL_LETTER_E_WITH_GRAVE, UNICODE_LATIN_CAPITAL_LETTER_I_WITH_ACUTE,
	UNICODE_LATIN_CAPITAL_LETTER_I_WITH_CIRCUMFLEX, UNICODE_LATIN_CAPITAL_LETTER_I_WITH_DIAERESIS,
	UNICODE_LATIN_CAPITAL_LETTER_I_WITH_GRAVE, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_ACUTE,
	UNICODE_LATIN_CAPITAL_LETTER_O_WITH_CIRCUMFLEX,

	0xf8ff /* Apple logo */, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_GRAVE, UNICODE_LATIN_CAPITAL_LETTER_U_WITH_ACUTE,
	UNICODE_LATIN_CAPITAL_LETTER_U_WITH_CIRCUMFLEX, UNICODE_LATIN_CAPITAL_LETTER_U_WITH_GRAVE,
	UNICODE_LATIN_SMALL_LETTER_DOTLESS_I, UNICODE_MODIFIER_LETTER_CIRCUMFLEX_ACCENT, UNICODE_SMALL_TILDE,
	UNICODE_MACRON, UNICODE_BREVE, UNICODE_DOT_ABOVE, UNICODE_RING_ABOVE, UNICODE_CEDILLA,
	UNICODE_DOUBLE_ACUTE_ACCENT, UNICODE_OGONEK, UNICODE_CARON };

char32_t Melder_decodeWindowsLatin1 [256] = {
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
	 20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
	 40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
	 60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,
	100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
	120, 121, 122, 123, 124, 125, 126, 127,
	UNICODE_EURO_SIGN, 129, UNICODE_SINGLE_LOW_9_QUOTATION_MARK, UNICODE_LATIN_SMALL_LETTER_F_WITH_HOOK,
	UNICODE_DOUBLE_LOW_9_QUOTATION_MARK, UNICODE_HORIZONTAL_ELLIPSIS, UNICODE_DAGGER, UNICODE_DOUBLE_DAGGER,
	UNICODE_MODIFIER_LETTER_CIRCUMFLEX_ACCENT, UNICODE_PER_MILLE_SIGN, UNICODE_LATIN_CAPITAL_LETTER_S_WITH_CARON,
	UNICODE_SINGLE_LEFT_POINTING_ANGLE_QUOTATION_MARK, UNICODE_LATIN_CAPITAL_LIGATURE_OE, 141,
	UNICODE_LATIN_CAPITAL_LETTER_Z_WITH_CARON, 143,
	144, UNICODE_LEFT_SINGLE_QUOTATION_MARK, UNICODE_RIGHT_SINGLE_QUOTATION_MARK, UNICODE_LEFT_DOUBLE_QUOTATION_MARK,
	UNICODE_RIGHT_DOUBLE_QUOTATION_MARK, UNICODE_BULLET, UNICODE_EN_DASH, UNICODE_EM_DASH,
	UNICODE_SMALL_TILDE, UNICODE_TRADE_MARK_SIGN, UNICODE_LATIN_SMALL_LETTER_S_WITH_CARON,
	UNICODE_SINGLE_RIGHT_POINTING_ANGLE_QUOTATION_MARK, UNICODE_LATIN_SMALL_LIGATURE_OE, 157,
	UNICODE_LATIN_SMALL_LETTER_Z_WITH_CARON, UNICODE_LATIN_CAPITAL_LETTER_Y_WITH_DIAERESIS,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
	180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
	200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
	220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 };

template <class CHAR>
void Melder_8bitToCHAR_inline (const char *string, CHAR *wcs, int inputEncoding) {
	CHAR *q = & wcs [0];
	if (inputEncoding == 0) {
		inputEncoding = preferences. inputEncoding;
		/*
		 * In case the preferences weren't initialized yet, use the platform defaults:
		 */
		if (inputEncoding == 0) {
			#if defined (macintosh)
				inputEncoding = kMelder_textInputEncoding_UTF8_THEN_MACROMAN;
			#elif defined (_WIN32)
				inputEncoding = kMelder_textInputEncoding_UTF8_THEN_WINDOWS_LATIN1;
			#else
				inputEncoding = kMelder_textInputEncoding_UTF8_THEN_ISO_LATIN1;
			#endif
		}
	}
	if (inputEncoding == kMelder_textInputEncoding_UTF8 ||
		inputEncoding == kMelder_textInputEncoding_UTF8_THEN_ISO_LATIN1 ||
		inputEncoding == kMelder_textInputEncoding_UTF8_THEN_WINDOWS_LATIN1 ||
		inputEncoding == kMelder_textInputEncoding_UTF8_THEN_MACROMAN)
	{
		if (Melder_strIsValidUtf8 (string)) {
			inputEncoding = kMelder_textInputEncoding_UTF8;
		} else if (inputEncoding == kMelder_textInputEncoding_UTF8_THEN_ISO_LATIN1) {
			inputEncoding = kMelder_textInputEncoding_ISO_LATIN1;
		} else if (inputEncoding == kMelder_textInputEncoding_UTF8_THEN_WINDOWS_LATIN1) {
			inputEncoding = kMelder_textInputEncoding_WINDOWS_LATIN1;
		} else if (inputEncoding == kMelder_textInputEncoding_UTF8_THEN_MACROMAN) {
			inputEncoding = kMelder_textInputEncoding_MACROMAN;
		} else {
			Melder_assert (inputEncoding == kMelder_textInputEncoding_UTF8);
			Melder_throw ("Text is not valid UTF-8; please try a different text input encoding.");
		}
	}
	const unsigned char *p = (const unsigned char *) & string [0];
	if (inputEncoding == kMelder_textInputEncoding_UTF8) {
		while (*p != '\0') {
			char32_t kar1 = * p ++;
			if (kar1 <= 0x7F) {
				* q ++ = (CHAR) kar1;
			} else if (kar1 <= 0xDF) {
				char32_t kar2 = * p ++;
				* q ++ = (CHAR) (((kar1 & 0x1F) << 6) | (kar2 & 0x3F));
			} else if (kar1 <= 0xEF) {
				char32_t kar2 = * p ++, kar3 = * p ++;
				* q ++ = (CHAR) (((kar1 & 0x0F) << 12) | ((kar2 & 0x3F) << 6) | (kar3 & 0x3F));
			} else if (kar1 <= 0xF4) {
				char32_t kar2 = * p ++, kar3 = * p ++, kar4 = * p ++;
				char32_t kar = ((kar1 & 0x07) << 18) | ((kar2 & 0x3F) << 12) | ((kar3 & 0x3F) << 6) | (kar4 & 0x3F);
				if (sizeof (CHAR) == 2) {
					/*
					 * Convert to UTF-16 surrogate pair.
					 */
					kar -= 0x10000;
					* q ++ = 0xD800 | (kar >> 10);
					* q ++ = 0xDC00 | (kar & 0x3FF);
				} else {
					* q ++ = (CHAR) kar;
				}
			}
		}
	} else if (inputEncoding == kMelder_textInputEncoding_ISO_LATIN1) {
		while (*p != '\0') {
			* q ++ = * p ++;
		}
	} else if (inputEncoding == kMelder_textInputEncoding_WINDOWS_LATIN1) {
		while (*p != '\0') {
			* q ++ = (CHAR) Melder_decodeWindowsLatin1 [* p ++];
		}
	} else if (inputEncoding == kMelder_textInputEncoding_MACROMAN) {
		while (*p != '\0') {
			* q ++ = (CHAR) Melder_decodeMacRoman [* p ++];
		}
	} else if (inputEncoding != kMelder_textInputEncoding_UTF8) {
		Melder_fatal ("Unknown text input encoding %d.", inputEncoding);
	}
	* q = '\0';
	(void) Melder_killReturns_inlineCHAR <CHAR> (wcs);
}
void Melder_8bitToWcs_inline (const char *string, wchar_t *wcs, int inputEncoding) {
	Melder_8bitToCHAR_inline <wchar_t> (string, wcs, inputEncoding);
}
void Melder_8bitToUtf32_inline (const char *string, char32_t *wcs, int inputEncoding) {
	Melder_8bitToCHAR_inline <char32_t> (string, wcs, inputEncoding);
}

wchar_t * Melder_8bitToWcs (const char *string, int inputEncoding) {
	if (string == NULL) return NULL;
	autostring result = Melder_malloc (wchar_t, (int64_t) strlen (string) + 1);
	Melder_8bitToWcs_inline (string, result.peek(), inputEncoding);
	return result.transfer();
}
char32_t * Melder_8bitToUtf32 (const char *string, int inputEncoding) {
	if (string == NULL) return NULL;
	autostring32 result = Melder_malloc (char32_t, (int64_t) strlen (string) + 1);
	Melder_8bitToUtf32_inline (string, result.peek(), inputEncoding);
	return result.transfer();
}

wchar_t * Melder_utf8ToWcs (const char *string) {
	if (string == NULL) return NULL;
	autostring result = Melder_malloc (wchar_t, (int64_t) strlen (string) + 1);
	Melder_8bitToWcs_inline (string, result.peek(), kMelder_textInputEncoding_UTF8);
	return result.transfer();
}
char32_t * Melder_utf8ToUtf32 (const char *string) {
	if (string == NULL) return NULL;
	autostring32 result = Melder_malloc (char32_t, (int64_t) strlen (string) + 1);
	Melder_8bitToUtf32_inline (string, result.peek(), kMelder_textInputEncoding_UTF8);
	return result.transfer();
}

char32_t * Melder_utf8ToUtf32_f (const char *string) {
	char32_t *result = Melder_malloc_f (char32_t, (int64_t) strlen (string) + 1);
	Melder_assert (Melder_strIsValidUtf8 (string));
	const char8_t *p = (const char8_t *) & string [0];
	char32_t *q = & result [0];
	while (*p != '\0') {
		char32_t kar1 = (char32_t) * p ++;   // convert up
		if (kar1 <= 0x7F) {
			* q ++ = kar1;
		} else if (kar1 <= 0xDF) {
			char32_t kar2 = (char32_t) * p ++;   // convert up
			* q ++ = ((kar1 & 0x1F) << 6) | (kar2 & 0x3F);
		} else if (kar1 <= 0xEF) {
			char32_t kar2 = (char32_t) * p ++, kar3 = (char32_t) * p ++;   // convert up
			* q ++ = ((kar1 & 0x0F) << 12) | ((kar2 & 0x3F) << 6) | (kar3 & 0x3F);
		} else if (kar1 <= 0xF4) {
			char32_t kar2 = (char32_t) * p ++, kar3 = (char32_t) * p ++, kar4 = (char32_t) * p ++;   // convert up
			* q ++ = ((kar1 & 0x07) << 18) | ((kar2 & 0x3F) << 12) | ((kar3 & 0x3F) << 6) | (kar4 & 0x3F);
		}
	}
	* q = '\0';   // closing null character
	return result;
}

wchar_t * Melder_peekUtf8ToWcs (const char *textA) {
	if (textA == NULL) return NULL;
	static MelderString buffers [11] = { { 0 } };
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderString_empty (& buffers [ibuffer]);
	unsigned long n = strlen (textA), i, j;
	for (i = 0, j = 0; i <= n; i ++) {
		char8_t kar1 = (char8_t) textA [i];   // convert sign
		if (kar1 <= 0x7F) {
			MelderString_appendCharacter (& buffers [ibuffer],
				(char32_t) kar1);
		} else if (kar1 <= 0xC1) {
			MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
		} else if (kar1 <= 0xDF) {
			char8_t kar2 = (char8_t) textA [++ i];
			if ((kar2 & 0xC0) != 0x80) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			MelderString_appendCharacter (& buffers [ibuffer],
				(char32_t) ((char32_t) ((char32_t) kar1 & 0x1F) << 6) |
				            (char32_t) ((char32_t) kar2 & 0x3F));
		} else if (kar1 <= 0xEF) {
			char8_t kar2 = (char8_t) textA [++ i];
			if ((kar2 & 0xC0) != 0x80) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			char8_t kar3 = (char8_t) textA [++ i];
			if ((kar3 & 0xC0) != 0x80) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			MelderString_appendCharacter (& buffers [ibuffer],
				(char32_t) ((char32_t) ((char32_t) kar1 & 0x0F) << 12) |
				(char32_t) ((char32_t) ((char32_t) kar2 & 0x3F) << 6) |
				            (char32_t) ((char32_t) kar3 & 0x3F));
		} else if (kar1 <= 0xF4) {
			char8_t kar2 = (char8_t) textA [++ i];
			if ((kar2 & 0xC0) != 0x80) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			char8_t kar3 = (char8_t) textA [++ i];
			if ((kar3 & 0xC0) != 0x80) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			char8_t kar4 = (char8_t) textA [++ i];
			if ((kar4 & 0xC0) != 0x80) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			char32_t character =
				(char32_t) ((char32_t) ((char32_t) kar1 & 0x07) << 18) |
				(char32_t) ((char32_t) ((char32_t) kar2 & 0x3F) << 12) |
				(char32_t) ((char32_t) ((char32_t) kar3 & 0x3F) << 6) |
			                (char32_t) ((char32_t) kar4 & 0x3F);
			if (sizeof (wchar_t) == 2) {
				/*
				 * Convert to UTF-16 surrogate pair.
				 */
				character -= 0x010000;
				MelderString_appendCharacter (& buffers [ibuffer], 0x00D800 | (character >> 10));
				MelderString_appendCharacter (& buffers [ibuffer], 0x00DC00 | (character & 0x0003FF));
			} else {
				MelderString_appendCharacter (& buffers [ibuffer], (wchar_t) character);   // convert sign
			}
		} else {
			MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
		}
	}
	return buffers [ibuffer]. string;
}

size_t wcslen_utf8 (const wchar_t *wcs, bool expandNewlines) {
	size_t length = 0;
	for (const wchar_t *p = & wcs [0]; *p != '\0'; p ++) {
		if (sizeof (wchar_t) == 2) {
			char16_t kar1 = (char16_t) *p;   // convert sign
			if (kar1 <= 0x007F) {
				#ifdef _WIN32
					if (expandNewlines && kar1 == '\n') length ++;
				#else
					(void) expandNewlines;
				#endif
				length ++;
			} else if (kar1 <= 0x07FF) {
				length += 2;
			} else if (kar1 >= 0xD800 && kar1 <= 0xDBFF) {
				char16_t kar2 = (char16_t) * ++ p;
				Melder_assert (kar2 >= 0xDC00 && kar2 <= 0xDFFF);
				length += 4;
			} else {
				Melder_assert (kar1 <= 0xD7FF || kar1 >= 0xE000);
				length += 3;
			}
		} else {
			char32_t kar = (char32_t) *p;   // convert sign
			if (kar <= 0x0000007F) {
				#ifdef _WIN32
					if (expandNewlines && kar == '\n') length ++;
				#else
					(void) expandNewlines;
				#endif
				length ++;
			} else if (kar <= 0x000007FF) {
				length += 2;
			} else if (kar <= 0x0000FFFF) {
				length += 3;
			} else {
				Melder_assert (kar <= 0x0010FFFF);
				length += 4;
			}
		}
	}
	return length;
}

size_t wcslen_utf16 (const wchar_t *wcs, bool expandNewlines) {
	size_t length = 0;
	for (const wchar_t *p = & wcs [0]; *p != '\0'; p ++) {
		if (sizeof (wchar_t) == 2) {
			#ifdef _WIN32
				char16_t kar = (char16_t) *p;   // convert sign
				if (expandNewlines && kar == '\n') length ++;
			#else
				(void) expandNewlines;
			#endif
			length ++;
		} else {
			char32_t kar = (char32_t) *p;   // convert sign
			if (kar <= 0x0000007F) {
				#ifdef _WIN32
					if (expandNewlines && kar == '\n') length ++;
				#else
					(void) expandNewlines;
				#endif
				length ++;
			} else if (kar >= 0x00010000) {
				length += 2;
			} else {
				length += 1;
			}
		}
	}
	return length;
}

void Melder_wcsToUtf8_inline (const wchar_t *wcs, char *utf8) {
	size_t n = wcslen (wcs), i, j;
	for (i = 0, j = 0; i < n; i ++) {
		char32_t kar = sizeof (wchar_t) == 2 ?
			(char16_t) wcs [i] :   // crucial cast: prevents sign extension
			(char32_t) wcs [i];
		if (kar <= 0x0000007F) {
			#ifdef _WIN32
				if (kar == '\n') utf8 [j ++] = 13;
			#endif
			utf8 [j ++] = (char) (char8_t) kar;   // guarded truncation
		} else if (kar <= 0x000007FF) {
			utf8 [j ++] = (char) (char8_t) (0xC0 | (kar >> 6));
			utf8 [j ++] = (char) (char8_t) (0x80 | (kar & 0x00003F));
		} else if (kar <= 0x0000FFFF) {
			if (sizeof (wchar_t) == 2) {
				if ((kar & 0xF800) == 0xD800) {
					if (kar > 0xDBFF)
						Melder_fatal ("Incorrect Unicode value (first surrogate member %X).", kar);
					char32_t kar2 = (char16_t) wcs [++ i];   // crucial cast: prevents sign extension
					if (kar2 < 0xDC00 || kar2 > 0xDFFF)
						Melder_fatal ("Incorrect Unicode value (second surrogate member %X).", kar2);
					kar = (((kar & 0x3FF) << 10) | (kar2 & 0x3FF)) + 0x10000;   // decode UTF-16
					utf8 [j ++] = (char) (char8_t) (0xF0 | (kar >> 18));
					utf8 [j ++] = (char) (char8_t) (0x80 | ((kar >> 12) & 0x00003F));
					utf8 [j ++] = (char) (char8_t) (0x80 | ((kar >> 6) & 0x00003F));
					utf8 [j ++] = (char) (char8_t) (0x80 | (kar & 0x00003F));
				} else {
					utf8 [j ++] = (char) (char8_t) (0xE0 | (kar >> 12));
					utf8 [j ++] = (char) (char8_t) (0x80 | ((kar >> 6) & 0x00003F));
					utf8 [j ++] = (char) (char8_t) (0x80 | (kar & 0x00003F));
				}
			} else {
				utf8 [j ++] = (char) (char8_t) (0xE0 | (kar >> 12));
				utf8 [j ++] = (char) (char8_t) (0x80 | ((kar >> 6) & 0x00003F));
				utf8 [j ++] = (char) (char8_t) (0x80 | (kar & 0x00003F));
			}
		} else {
			utf8 [j ++] = (char) (char8_t) (0xF0 | (kar >> 18));
			utf8 [j ++] = (char) (char8_t) (0x80 | ((kar >> 12) & 0x00003F));
			utf8 [j ++] = (char) (char8_t) (0x80 | ((kar >> 6) & 0x00003F));
			utf8 [j ++] = (char) (char8_t) (0x80 | (kar & 0x00003F));
		}
	}
	utf8 [j] = '\0';
}

void Melder_str32ToUtf8_inline (const char32 *string, char *utf8) {
	int64 n = str32len (string), i, j;
	for (i = 0, j = 0; i < n; i ++) {
		char32 kar = string [i];
		if (kar <= 0x00007F) {   // 7 bits
			#ifdef _WIN32
				if (kar == '\n') utf8 [j ++] = 13;
			#endif
			utf8 [j ++] = (char) (char8_t) kar;   // guarded truncation
		} else if (kar <= 0x0007FF) {   // 11 bits
			utf8 [j ++] = (char) (char8_t) (0xC0 | (kar >> 6));   // the upper 5 bits yield a number between 0xC4 and 0xDF
			utf8 [j ++] = (char) (char8_t) (0x80 | (kar & 0x00003F));   // the lower 6 bits yield a number between 0x80 and 0xBF
		} else if (kar <= 0x00FFFF) {   // 16 bits
			utf8 [j ++] = (char) (char8_t) (0xE0 | (kar >> 12));   // the upper 4 bits yield a number between 0xE0 and 0xEF
			utf8 [j ++] = (char) (char8_t) (0x80 | ((kar >> 6) & 0x00003F));
			utf8 [j ++] = (char) (char8_t) (0x80 | (kar & 0x00003F));
		} else {   // 21 bits
			utf8 [j ++] = (char) (char8_t) (0xF0 | (kar >> 18));   // the upper 3 bits yield a number between 0xF0 and 0xF4 (0x10FFFF >> 18 == 4)
			utf8 [j ++] = (char) (char8_t) (0x80 | ((kar >> 12) & 0x00003F));   // the next 6 bits
			utf8 [j ++] = (char) (char8_t) (0x80 | ((kar >> 6) & 0x00003F));   // the third 6 bits
			utf8 [j ++] = (char) (char8_t) (0x80 | (kar & 0x00003F));   // the lower 6 bits
		}
	}
	utf8 [j] = '\0';
}

char * Melder_wcsToUtf8 (const wchar_t *string) {
	if (string == NULL) return NULL;
	autostring8 result = Melder_malloc (char, (int64_t) wcslen_utf8 (string, true) + 1);
	Melder_wcsToUtf8_inline (string, result.peek());
	return result.transfer();
}

char * Melder_peekWcsToUtf8 (const wchar_t *text) {
	if (text == NULL) return NULL;
	static char *buffer [11] = { NULL };
	static size_t bufferSize [11] = { 0 };
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	size_t sizeNeeded = wcslen (text) * 4 + 1;
	if ((bufferSize [ibuffer] - sizeNeeded) * sizeof (char) >= 10000) {
		Melder_free (buffer [ibuffer]);
		bufferSize [ibuffer] = 0;
	}
	if (sizeNeeded > bufferSize [ibuffer]) {
		sizeNeeded = (size_t) (sizeNeeded * 1.61803) + 100;
		buffer [ibuffer] = (char *) Melder_realloc_f (buffer [ibuffer], (int64) sizeNeeded * (int64) sizeof (char));
		bufferSize [ibuffer] = sizeNeeded;
	}
	Melder_wcsToUtf8_inline (text, buffer [ibuffer]);
	return buffer [ibuffer];
}

const uint16_t * Melder_peekWcsToUtf16 (const wchar_t *text) {
	if (text == NULL) return NULL;
	static MelderString16 buffers [11] = { { 0 } };
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderString16_empty (& buffers [ibuffer]);
	unsigned long n = wcslen (text);
	for (unsigned long i = 0; i <= n; i ++) {
		#ifdef _WIN32
			if (text [i] == '\n') MelderString16_appendCharacter (& buffers [ibuffer], 13);
		#endif
		MelderString16_appendCharacter (& buffers [ibuffer], text [i]);
	}
	return (uint16_t *) buffers [ibuffer]. string;   // return type is such that this routine can be called by C
}

#if defined (macintosh)
const void * Melder_peekWcsToCfstring (const wchar_t *text) {
	if (text == NULL) return NULL;
	static CFStringRef cfString [11];
	static int icfString = 0;
	if (++ icfString == 11) icfString = 0;
	if (cfString [icfString] != NULL) CFRelease (cfString [icfString]);
	cfString [icfString] = CFStringCreateWithCString (NULL, Melder_peekWcsToUtf8 (text), kCFStringEncodingUTF8);
	return cfString [icfString];
}
#endif

/* End of file melder_textencoding.cpp */
