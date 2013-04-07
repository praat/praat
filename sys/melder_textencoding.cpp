/* melder_textencoding.cpp
 *
 * Copyright (C) 2007-2011 Paul Boersma
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
			unsigned short kar = *text;
			if (kar > 127) return false;
		} else {
			if (*text > 127) return false;
		}
	}
	return true;
}

bool Melder_isEncodable (const wchar_t *text, int outputEncoding) {
	switch (outputEncoding) {
		case kMelder_textOutputEncoding_ASCII: {
			for (; *text != '\0'; text ++) {
				if (sizeof (wchar_t) == 2) {
					unsigned short kar = *text;
					if (kar > 127) return false;
				} else {
					if (*text > 127) return false;
				}
			}
			return true;
		} break;
		case kMelder_textOutputEncoding_ISO_LATIN1: {
			for (; *text != '\0'; text ++) {
				if (sizeof (wchar_t) == 2) {
					unsigned short kar = *text;
					if (kar > 255) return false;
				} else {
					if (*text > 255) return false;
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
	for (const unsigned char *p = (const unsigned char *) & string [0]; *p != '\0'; p ++) {
		unsigned long kar = *p;
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
		if (*from == 13) {   /* Carriage return? */
			if (from [1] == '\n') {   /* Followed by linefeed? Must be a Windows text. */
				from ++;   /* Ignore carriage return. */
				*to = '\n';   /* Copy linefeed. */
			} else {   /* Bare carriage return? Must be a Macintosh text. */
				*to = '\n';   /* Change to linefeed. */
			}
		} else {
			*to = *from;
		}
	}
	*to = '\0';   /* Closing null byte. */
	return to - text;
}

long Melder_killReturns_inlineW (wchar_t *text) {
	const wchar_t *from;
	wchar_t *to;
	for (from = text, to = text; *from != '\0'; from ++, to ++) {
		if (*from == 13) {   /* Carriage return? */
			if (from [1] == '\n' || from [1] == 0x0085 /* NextLine */) {   /* Followed by linefeed? Must be a Windows text. */
				from ++;   /* Ignore carriage return. */
				*to = '\n';   /* Copy linefeed. */
			} else {   /* Bare carriage return? Must be a Macintosh text. */
				*to = '\n';   /* Change to linefeed (10). */
			}
		} else if (*from == 0x0085 /* NextLine */ || *from == 0x000C /* FormFeed */ ||
		    *from == UNICODE_LINE_SEPARATOR || *from == UNICODE_PARAGRAPH_SEPARATOR)
		{
			*to = '\n';
		} else {
			*to = *from;
		}
	}
	*to = '\0';   /* Closing null char. */
	return to - text;
}

wchar_t Melder_decodeMacRoman [256] = {
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

wchar_t Melder_decodeWindowsLatin1 [256] = {
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

void Melder_8bitToWcs_inline (const char *string, wchar_t *wcs, int inputEncoding) {
	wchar_t *q = & wcs [0];
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
			uint32_t kar = * p ++;
			if (kar <= 0x7F) {
				* q ++ = kar;
			} else if (kar <= 0xDF) {
				unsigned long kar2 = * p ++;
				* q ++ = ((kar & 0x1F) << 6) | (kar2 & 0x3F);
			} else if (kar <= 0xEF) {
				unsigned long kar2 = * p ++, kar3 = * p ++;
				* q ++ = ((kar & 0x0F) << 12) | ((kar2 & 0x3F) << 6) | (kar3 & 0x3F);
			} else if (kar <= 0xF4) {
				unsigned long kar2 = * p ++, kar3 = * p ++, kar4 = * p ++;
				kar = ((kar & 0x07) << 18) | ((kar2 & 0x3F) << 12) | ((kar3 & 0x3F) << 6) | (kar4 & 0x3F);
				if (sizeof (wchar_t) == 2) {
					/*
					 * Convert to UTF-16 surrogate pair.
					 */
					kar -= 0x10000;
					* q ++ = 0xD800 | (kar >> 10);
					* q ++ = 0xDC00 | (kar & 0x3FF);
				} else {
					* q ++ = kar;
				}
			}
		}
	} else if (inputEncoding == kMelder_textInputEncoding_ISO_LATIN1) {
		while (*p != '\0') {
			* q ++ = * p ++;
		}
	} else if (inputEncoding == kMelder_textInputEncoding_WINDOWS_LATIN1) {
		while (*p != '\0') {
			* q ++ = Melder_decodeWindowsLatin1 [* p ++];
		}
	} else if (inputEncoding == kMelder_textInputEncoding_MACROMAN) {
		while (*p != '\0') {
			* q ++ = Melder_decodeMacRoman [* p ++];
		}
	} else if (inputEncoding != kMelder_textInputEncoding_UTF8) {
		Melder_fatal ("Unknown text input encoding %d.", inputEncoding);
	}
	* q = '\0';
	(void) Melder_killReturns_inlineW (wcs);
}

wchar_t * Melder_8bitToWcs (const char *string, int inputEncoding) {
	if (string == NULL) return NULL;
	autostring result = Melder_malloc (wchar_t, strlen (string) + 1);
	Melder_8bitToWcs_inline (string, result.peek(), inputEncoding);
	return result.transfer();
}

wchar_t * Melder_utf8ToWcs (const char *string) {
	if (string == NULL) return NULL;
	autostring result = Melder_malloc (wchar_t, strlen (string) + 1);
	Melder_8bitToWcs_inline (string, result.peek(), kMelder_textInputEncoding_UTF8);
	return result.transfer();
}

wchar_t * Melder_peekUtf8ToWcs (const char *textA) {
	if (textA == NULL) return NULL;
	static MelderString buffers [11] = { { 0 } };
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderString_empty (& buffers [ibuffer]);
	unsigned long n = strlen (textA), i, j;
	for (i = 0, j = 0; i <= n; i ++) {
		unsigned char kar = textA [i];
		if (kar <= 0x7F) {
			MelderString_appendCharacter (& buffers [ibuffer], kar);
		} else if (kar <= 0xC1) {
			MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
		} else if (kar <= 0xDF) {
			unsigned char kar2 = textA [++ i];
			if (kar2 == '\0' || ! (kar2 & 0x80) || (kar2 & 0x40)) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			MelderString_appendCharacter (& buffers [ibuffer], ((kar & 0x1F) << 6) | (kar2 & 0x3F));
		} else if (kar <= 0xEF) {
			unsigned char kar2 = textA [++ i];
			if (kar2 == '\0' || ! (kar2 & 0x80) || (kar2 & 0x40)) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			unsigned char kar3 = textA [++ i];
			if (kar3 == '\0' || ! (kar3 & 0x80) || (kar3 & 0x40)) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			MelderString_appendCharacter (& buffers [ibuffer], ((kar & 0x0F) << 12) | ((kar2 & 0x3F) << 6) | (kar3 & 0x3F));
		} else if (kar <= 0xF4) {
			unsigned char kar2 = textA [++ i];
			if (kar2 == '\0' || ! (kar2 & 0x80) || (kar2 & 0x40)) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			unsigned char kar3 = textA [++ i];
			if (kar3 == '\0' || ! (kar3 & 0x80) || (kar3 & 0x40)) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			unsigned char kar4 = textA [++ i];
			if (kar4 == '\0' || ! (kar4 & 0x80) || (kar4 & 0x40)) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			unsigned long character = ((kar & 0x07) << 18) | ((kar2 & 0x3F) << 12) | ((kar3 & 0x3F) << 6) | (kar4 & 0x3F);
			if (sizeof (wchar_t) == 2) {
				/*
				 * Convert to UTF-16 surrogate pair.
				 */
				character -= 0x10000;
				MelderString_appendCharacter (& buffers [ibuffer], 0xD800 | (character >> 10));
				MelderString_appendCharacter (& buffers [ibuffer], 0xDC00 | (character & 0x3FF));
			} else {
				MelderString_appendCharacter (& buffers [ibuffer], character);
			}
		} else {
			MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
		}
	}
	return buffers [ibuffer]. string;
}

unsigned long wcslen_utf8 (const wchar_t *wcs, bool expandNewlines) {
	long length = 0;
	for (const wchar_t *p = & wcs [0]; *p != '\0'; p ++) {
		if (sizeof (wchar_t) == 2) {
			unsigned short kar = *p;
			if (kar <= 0x007F) {
				#ifdef _WIN32
					if (expandNewlines && kar == '\n') length ++;
				#else
					(void) expandNewlines;
				#endif
				length ++;
			} else if (kar <= 0x07FF) {
				length += 2;
			} else if (kar >= 0xD800 && kar <= 0xDBFF) {
				unsigned short kar2 = * ++ p;
				Melder_assert (kar2 >= 0xDC00 && kar2 <= 0xDFFF);
				length += 4;
			} else {
				Melder_assert (kar <= 0xD7FF || kar >= 0xE000);
				length += 3;
			}
		} else {
			unsigned long kar = *p;
			if (kar <= 0x00007F) {
				#ifdef _WIN32
					if (expandNewlines && kar == '\n') length ++;
				#else
					(void) expandNewlines;
				#endif
				length ++;
			} else if (kar <= 0x0007FF) {
				length += 2;
			} else if (kar <= 0x00FFFF) {
				length += 3;
			} else {
				Melder_assert (kar <= 0x10FFFF);
				length += 4;
			}
		}
	}
	return length;
}

unsigned long wcslen_utf16 (const wchar_t *wcs, bool expandNewlines) {
	long length = 0;
	for (const wchar_t *p = & wcs [0]; *p != '\0'; p ++) {
		if (sizeof (wchar_t) == 2) {
			#ifdef _WIN32
				unsigned short kar = *p;
				if (expandNewlines && kar == '\n') length ++;
			#else
				(void) expandNewlines;
			#endif
			length ++;
		} else {
			unsigned long kar = *p;
			if (kar <= 0x00007F) {
				#ifdef _WIN32
					if (expandNewlines && kar == '\n') length ++;
				#else
					(void) expandNewlines;
				#endif
				length ++;
			} else if (kar >= 0x10000) {
				length += 2;
			} else {
				length += 1;
			}
		}
	}
	return length;
}

void Melder_wcsToUtf8_inline (const wchar_t *wcs, char *utf8) {
	long n = wcslen (wcs), i, j;
	for (i = 0, j = 0; i < n; i ++) {
		unsigned long kar = sizeof (wchar_t) == 2 ? (unsigned short) wcs [i] : wcs [i];   // crucial cast: prevents sign extension
		if (kar <= 0x00007F) {
			#ifdef _WIN32
				if (kar == '\n') utf8 [j ++] = 13;
			#endif
			utf8 [j ++] = kar;
		} else if (kar <= 0x0007FF) {
			utf8 [j ++] = 0xC0 | (kar >> 6);
			utf8 [j ++] = 0x80 | (kar & 0x00003F);
		} else if (kar <= 0x00FFFF) {
			if (sizeof (wchar_t) == 2) {
				if ((kar & 0xF800) == 0xD800) {
					if (kar > 0xDBFF)
						Melder_fatal ("Incorrect Unicode value (first surrogate member %lX).", kar);
					unsigned long kar2 = (unsigned short) wcs [++ i];   // crucial cast: prevents sign extension
					if (kar2 < 0xDC00 || kar2 > 0xDFFF)
						Melder_fatal ("Incorrect Unicode value (second surrogate member %lX).", kar2);
					kar = (((kar & 0x3FF) << 10) | (kar2 & 0x3FF)) + 0x10000;   // decode UTF-16
					utf8 [j ++] = 0xF0 | (kar >> 18);
					utf8 [j ++] = 0x80 | ((kar >> 12) & 0x00003F);
					utf8 [j ++] = 0x80 | ((kar >> 6) & 0x00003F);
					utf8 [j ++] = 0x80 | (kar & 0x00003F);
				} else {
					utf8 [j ++] = 0xE0 | (kar >> 12);
					utf8 [j ++] = 0x80 | ((kar >> 6) & 0x00003F);
					utf8 [j ++] = 0x80 | (kar & 0x00003F);
				}
			} else {
				utf8 [j ++] = 0xE0 | (kar >> 12);
				utf8 [j ++] = 0x80 | ((kar >> 6) & 0x00003F);
				utf8 [j ++] = 0x80 | (kar & 0x00003F);
			}
		} else {
			utf8 [j ++] = 0xF0 | (kar >> 18);
			utf8 [j ++] = 0x80 | ((kar >> 12) & 0x00003F);
			utf8 [j ++] = 0x80 | ((kar >> 6) & 0x00003F);
			utf8 [j ++] = 0x80 | (kar & 0x00003F);
		}
	}
	utf8 [j] = '\0';
}

char * Melder_wcsToUtf8 (const wchar_t *string) {
	if (string == NULL) return NULL;
	autostring8 result = Melder_malloc (char, wcslen_utf8 (string, true) + 1);
	Melder_wcsToUtf8_inline (string, result.peek());
	return result.transfer();
}

char * Melder_peekWcsToUtf8 (const wchar_t *text) {
	if (text == NULL) return NULL;
	static char *buffer [11] = { NULL };
	static long bufferSize [11] = { 0 };
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	long sizeNeeded = wcslen (text) * 4 + 1;
	if ((bufferSize [ibuffer] - sizeNeeded) * sizeof (char) >= 10000) {
		Melder_free (buffer [ibuffer]);
		bufferSize [ibuffer] = 0;
	}
	if (sizeNeeded > bufferSize [ibuffer]) {
		sizeNeeded = sizeNeeded * 1.61803 + 100;
		buffer [ibuffer] = (char *) Melder_realloc_f (buffer [ibuffer], sizeNeeded * sizeof (char));
		bufferSize [ibuffer] = sizeNeeded;
	}
	Melder_wcsToUtf8_inline (text, buffer [ibuffer]);
	return buffer [ibuffer];
}

const MelderUtf16 * Melder_peekWcsToUtf16 (const wchar_t *text) {
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
	return buffers [ibuffer]. string;
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
