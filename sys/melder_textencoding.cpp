/* melder_textencoding.cpp
 *
 * Copyright (C) 2007-2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "melder.h"
#include "Preferences.h"
#include "UnicodeData.h"
#include "abcio.h"
#include <math.h>

#if defined (macintosh)
	#include "macport_on.h"
	#include <CoreFoundation/CoreFoundation.h>
	#include "macport_off.h"
#endif

static struct {
	kMelder_textInputEncoding inputEncoding;
	kMelder_textOutputEncoding outputEncoding;
} preferences;

void Melder_setInputEncoding (kMelder_textInputEncoding encoding) { preferences. inputEncoding = encoding; }
kMelder_textInputEncoding Melder_getInputEncoding () { return preferences. inputEncoding; }

void Melder_setOutputEncoding (kMelder_textOutputEncoding encoding) { preferences. outputEncoding = encoding; }
kMelder_textOutputEncoding Melder_getOutputEncoding () { return preferences. outputEncoding; }

void Melder_textEncoding_prefs () {
	Preferences_addEnum (U"TextEncoding.inputEncoding", & preferences. inputEncoding, kMelder_textInputEncoding, kMelder_textInputEncoding::DEFAULT);
	Preferences_addEnum (U"TextEncoding.outputEncoding", & preferences. outputEncoding, kMelder_textOutputEncoding, kMelder_textOutputEncoding::DEFAULT);
}

bool Melder_isValidAscii (const char32 *text) {
	for (; *text != '\0'; text ++) {
		if (*text > 127) return false;
	}
	return true;
}

bool Melder_isEncodable (const char32 *text, int outputEncoding) {
	switch (outputEncoding) {
		case kMelder_textOutputEncoding_ASCII: {
			for (; *text != '\0'; text ++) {
				if (*text > 127) return false;
			}
			return true;
		} break;
		case kMelder_textOutputEncoding_ISO_LATIN1: {
			for (; *text != '\0'; text ++) {
				if (*text > 255) return false;
			}
			return true;
		} break;
		case (int) kMelder_textOutputEncoding::UTF8:
		case (int) kMelder_textOutputEncoding::UTF16:
		case (int) kMelder_textOutputEncoding::ASCII_THEN_UTF16:
		case (int) kMelder_textOutputEncoding::ISO_LATIN1_THEN_UTF16: {
			return true;
		}
	}
	return false;
}

bool Melder_str8IsValidUtf8 (const char *string) {
	for (const char8 *p = (const char8 *) & string [0]; *p != '\0'; p ++) {
		char32 kar = (char32) *p;
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

integer Melder_killReturns_inplace (char *text) {
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
integer Melder_killReturns_inplaceCHAR (CHAR *text) {
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
integer Melder_killReturns_inplace (char32 *text) {
	return Melder_killReturns_inplaceCHAR <char32> (text);
}

size_t str32len_utf8 (const char32 *string, bool nativizeNewlines) {
	size_t length = 0;
	for (const char32 *p = & string [0]; *p != U'\0'; p ++) {
		char32 kar = *p;
		if (kar <= 0x00007F) {
			#ifdef _WIN32
				if (nativizeNewlines && kar == U'\n') length ++;
			#else
				(void) nativizeNewlines;
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
	return length;
}

size_t str32len_utf16 (const char32 *string, bool nativizeNewlines) {
	size_t length = 0;
	for (const char32 *p = & string [0]; *p != U'\0'; p ++) {
		char32 kar = *p;
		if (kar <= 0x00007F) {
			#ifdef _WIN32
				if (nativizeNewlines && kar == U'\n') length ++;
			#else
				(void) nativizeNewlines;
			#endif
			length ++;
		} else if (kar >= 0x010000) {
			length += 2;
		} else {
			length += 1;
		}
	}
	return length;
}

char32 * Melder_peek8to32 (const char *textA) {
	if (! textA) return nullptr;
	static MelderString buffers [19] { };
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderString_empty (& buffers [ibuffer]);
	uinteger n = strlen (textA), i, j;
	for (i = 0, j = 0; i <= n; i ++) {
		char8 kar1 = (char8) textA [i];   // convert sign
		if (kar1 <= 0x7F) {
			MelderString_appendCharacter (& buffers [ibuffer],
				(char32) kar1);
		} else if (kar1 <= 0xC1) {
			MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
		} else if (kar1 <= 0xDF) {
			char8 kar2 = textA [++ i];
			if ((kar2 & 0xC0) != 0x80) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			MelderString_appendCharacter (& buffers [ibuffer],
				(char32) ((char32) ((char32) kar1 & 0x00001F) << 6) |
						  (char32) ((char32) kar2 & 0x00003F));
		} else if (kar1 <= 0xEF) {
			char8 kar2 = textA [++ i];
			if ((kar2 & 0xC0) != 0x80) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			char8 kar3 = textA [++ i];
			if ((kar3 & 0xC0) != 0x80) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			MelderString_appendCharacter (& buffers [ibuffer],
				(char32) ((char32) ((char32) kar1 & 0x00000F) << 12) |
				(char32) ((char32) ((char32) kar2 & 0x00003F) << 6) |
						  (char32) ((char32) kar3 & 0x00003F));
		} else if (kar1 <= 0xF4) {
			char8 kar2 = (char8) textA [++ i];
			if ((kar2 & 0xC0) != 0x80) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			char8 kar3 = (char8) textA [++ i];
			if ((kar3 & 0xC0) != 0x80) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			char8 kar4 = (char8) textA [++ i];
			if ((kar4 & 0xC0) != 0x80) MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			char32 character =
				(char32) ((char32) ((char32) kar1 & 0x000007) << 18) |
				(char32) ((char32) ((char32) kar2 & 0x00003F) << 12) |
				(char32) ((char32) ((char32) kar3 & 0x00003F) << 6) |
						  (char32) ((char32) kar4 & 0x00003F);
			MelderString_appendCharacter (& buffers [ibuffer], character);
		} else {
			MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
		}
	}
	return buffers [ibuffer]. string;
}

char32 Melder_decodeMacRoman [256] = {
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

char32 Melder_decodeWindowsLatin1 [256] = {
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

void Melder_8to32_inplace (const char *string8, char32 *string32, kMelder_textInputEncoding inputEncoding) {
	char32 *q = & string32 [0];
	if (inputEncoding == kMelder_textInputEncoding::UNDEFINED) {
		inputEncoding = preferences. inputEncoding;
		/*
		 * In case the preferences weren't initialized yet, use the platform defaults:
		 */
		if (inputEncoding == kMelder_textInputEncoding::UNDEFINED) {
			#if defined (macintosh)
				inputEncoding = kMelder_textInputEncoding::UTF8_THEN_MACROMAN;
			#elif defined (_WIN32)
				inputEncoding = kMelder_textInputEncoding::UTF8_THEN_WINDOWS_LATIN1;
			#else
				inputEncoding = kMelder_textInputEncoding::UTF8_THEN_ISO_LATIN1;
			#endif
		}
	}
	if (inputEncoding == kMelder_textInputEncoding::UTF8 ||
		inputEncoding == kMelder_textInputEncoding::UTF8_THEN_ISO_LATIN1 ||
		inputEncoding == kMelder_textInputEncoding::UTF8_THEN_WINDOWS_LATIN1 ||
		inputEncoding == kMelder_textInputEncoding::UTF8_THEN_MACROMAN)
	{
		if (Melder_str8IsValidUtf8 (string8)) {
			inputEncoding = kMelder_textInputEncoding::UTF8;
		} else if (inputEncoding == kMelder_textInputEncoding::UTF8_THEN_ISO_LATIN1) {
			inputEncoding = kMelder_textInputEncoding::ISO_LATIN1;
		} else if (inputEncoding == kMelder_textInputEncoding::UTF8_THEN_WINDOWS_LATIN1) {
			inputEncoding = kMelder_textInputEncoding::WINDOWS_LATIN1;
		} else if (inputEncoding == kMelder_textInputEncoding::UTF8_THEN_MACROMAN) {
			inputEncoding = kMelder_textInputEncoding::MACROMAN;
		} else {
			Melder_assert (inputEncoding == kMelder_textInputEncoding::UTF8);
			Melder_throw (U"Text is not valid UTF-8; please try a different text input encoding.");
		}
	}
	const char8 *p = (const char8 *) & string8 [0];
	if (inputEncoding == kMelder_textInputEncoding::UTF8) {
		while (*p != '\0') {
			char32 kar1 = * p ++;   // convert up without sign extension
			if (kar1 <= 0x00007F) {
				* q ++ = kar1;
			} else if (kar1 <= 0x0000DF) {
				char32 kar2 = * p ++;   // convert up without sign extension
				* q ++ = ((kar1 & 0x00001F) << 6) | (kar2 & 0x00003F);
			} else if (kar1 <= 0x0000EF) {
				char32 kar2 = * p ++, kar3 = * p ++;   // convert up without sign extension
				* q ++ = ((kar1 & 0x00000F) << 12) | ((kar2 & 0x00003F) << 6) | (kar3 & 0x00003F);
			} else if (kar1 <= 0x0000F4) {
				char32 kar2 = * p ++, kar3 = * p ++, kar4 = * p ++;   // convert up without sign extension
				char32 kar = ((kar1 & 0x000007) << 18) | ((kar2 & 0x00003F) << 12) | ((kar3 & 0x00003F) << 6) | (kar4 & 0x00003F);
				* q ++ = kar;
			}
		}
	} else if (inputEncoding == kMelder_textInputEncoding::ISO_LATIN1) {
		while (*p != '\0') {
			* q ++ = * p ++;
		}
	} else if (inputEncoding == kMelder_textInputEncoding::WINDOWS_LATIN1) {
		while (*p != '\0') {
			* q ++ = Melder_decodeWindowsLatin1 [* p ++];
		}
	} else if (inputEncoding == kMelder_textInputEncoding::MACROMAN) {
		while (*p != '\0') {
			* q ++ = Melder_decodeMacRoman [* p ++];
		}
	} else if (inputEncoding != kMelder_textInputEncoding::UTF8) {
		Melder_fatal (U"Unknown text input encoding ", (int) inputEncoding, U".");
	}
	* q = U'\0';   // closing null character
	(void) Melder_killReturns_inplaceCHAR <char32> (string32);
}

char32 * Melder_8to32 (const char *string, kMelder_textInputEncoding inputEncoding) {
	if (! string) return nullptr;
	autostring32 result = Melder_malloc (char32, (int64) strlen (string) + 1);
	Melder_8to32_inplace (string, result.peek(), inputEncoding);
	return result.transfer();
}

char32 * Melder_8to32 (const char *string) {
	if (! string) return nullptr;
	autostring32 result = Melder_malloc (char32, (int64) strlen (string) + 1);
	Melder_8to32_inplace (string, result.peek(), kMelder_textInputEncoding::UTF8);
	return result.transfer();
}

char32 * Melder_peek16to32 (const char16 *text) {
	if (! text) return nullptr;
	static MelderString buffers [19] { };
	static int ibuffer = 0;
	if (++ ibuffer == 19) ibuffer = 0;
	MelderString_empty (& buffers [ibuffer]);
	for (;;) {
		char16 kar1 = * text ++;
		if (kar1 == u'\0') return buffers [ibuffer]. string;
		if (kar1 < 0xD800) {
			MelderString_appendCharacter (& buffers [ibuffer], (char32) kar1);   // convert up without sign extension
		} else if (kar1 < 0xDC00) {
			char16 kar2 = * text ++;
			if (kar2 >= 0xDC00 && kar2 <= 0xDFFF) {
				MelderString_appendCharacter (& buffers [ibuffer],
					(char32) (0x010000 +
						(char32) (((char32) kar1 & 0x0003FF) << 10) +
						(char32)  ((char32) kar2 & 0x0003FF)));
			} else {
				MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
			}
		} else if (kar1 < 0xE000) {
			MelderString_appendCharacter (& buffers [ibuffer], UNICODE_REPLACEMENT_CHARACTER);
		} else {
			MelderString_appendCharacter (& buffers [ibuffer], (char32) kar1);   // convert up without sign extension
		}
	}
}

char32 * Melder_16to32 (const char16 *text) {
	return Melder_dup (Melder_peek16to32 (text));
}

void Melder_32to8_inplace (const char32 *string, char *utf8) {
	int64 n = str32len (string), i, j;
	for (i = 0, j = 0; i < n; i ++) {
		char32 kar = string [i];
		if (kar <= 0x00007F) {   // 7 bits
			#ifdef _WIN32
				if (kar == U'\n') utf8 [j ++] = 13;
			#endif
			utf8 [j ++] = (char) (char8) kar;   // guarded truncation
		} else if (kar <= 0x0007FF) {   // 11 bits
			utf8 [j ++] = (char) (char8) (0x0000C0 | (kar >> 6));   // the upper 5 bits yield a number between 0xC4 and 0xDF
			utf8 [j ++] = (char) (char8) (0x000080 | (kar & 0x00003F));   // the lower 6 bits yield a number between 0x80 and 0xBF
		} else if (kar <= 0x00FFFF) {   // 16 bits
			utf8 [j ++] = (char) (char8) (0x0000E0 | (kar >> 12));   // the upper 4 bits yield a number between 0xE0 and 0xEF
			utf8 [j ++] = (char) (char8) (0x000080 | ((kar >> 6) & 0x00003F));
			utf8 [j ++] = (char) (char8) (0x000080 | (kar & 0x00003F));
		} else {   // 21 bits
			utf8 [j ++] = (char) (char8) (0x0000F0 | (kar >> 18));   // the upper 3 bits yield a number between 0xF0 and 0xF4 (0x10FFFF >> 18 == 4)
			utf8 [j ++] = (char) (char8) (0x000080 | ((kar >> 12) & 0x00003F));   // the next 6 bits
			utf8 [j ++] = (char) (char8) (0x000080 | ((kar >> 6) & 0x00003F));   // the third 6 bits
			utf8 [j ++] = (char) (char8) (0x000080 | (kar & 0x00003F));   // the lower 6 bits
		}
	}
	utf8 [j] = '\0';
}

char * Melder_32to8 (const char32 *string) {
	if (! string) return nullptr;
	autostring8 result = Melder_malloc (char, (int64) str32len_utf8 (string, true) + 1);
	Melder_32to8_inplace (string, result.peek());
	return result.transfer();
}

char * Melder_peek32to8 (const char32 *text) {
	if (! text) return nullptr;
	static char *buffer [19] { nullptr };
	static int64 bufferSize [19] { 0 };
	static int ibuffer = 0;
	if (++ ibuffer == 19) ibuffer = 0;
	int64 sizeNeeded = str32len (text) * 4 + 1;
	if ((bufferSize [ibuffer] - sizeNeeded) * (int64) sizeof (char) >= 10000) {
		Melder_free (buffer [ibuffer]);
		bufferSize [ibuffer] = 0;
	}
	if (sizeNeeded > bufferSize [ibuffer]) {
		sizeNeeded = (int64) floor (sizeNeeded * 1.61803) + 100;
		buffer [ibuffer] = (char *) Melder_realloc_f (buffer [ibuffer], (int64) sizeNeeded * (int64) sizeof (char));
		bufferSize [ibuffer] = sizeNeeded;
	}
	Melder_32to8_inplace (text, buffer [ibuffer]);
	return buffer [ibuffer];
}

char16 * Melder_peek32to16 (const char32 *text, bool nativizeNewlines) {
	if (! text) return nullptr;
	static MelderString16 buffers [19] { };
	static int ibuffer = 0;
	if (++ ibuffer == 19) ibuffer = 0;
	MelderString16_empty (& buffers [ibuffer]);
	int64 n = str32len (text);
	if (nativizeNewlines) {
		for (int64 i = 0; i <= n; i ++) {
			#ifdef _WIN32
				if (text [i] == U'\n')
					MelderString16_appendCharacter (& buffers [ibuffer], (char32) 13);
			#endif
			MelderString16_appendCharacter (& buffers [ibuffer], text [i]);
		}
	} else {
		for (int64 i = 0; i <= n; i ++) {
			MelderString16_appendCharacter (& buffers [ibuffer], text [i]);
		}
	}
	return buffers [ibuffer]. string;
}
char16 * Melder_peek32to16 (const char32 *text) {
	return Melder_peek32to16 (text, false);
}

char16 * Melder_32to16 (const char32 *text) {
	char16 *text16 = Melder_peek32to16 (text);
	int64 length = str16len (text16);
	char16 *result = Melder_malloc (char16, length + 1);
	str16cpy (result, text16);
	return result;
}

#if defined (macintosh)
const void * Melder_peek32toCfstring (const char32 *text) {
	if (! text) return nullptr;
	static CFStringRef cfString [11];
	static int icfString = 0;
	if (++ icfString == 11) icfString = 0;
	if (cfString [icfString]) CFRelease (cfString [icfString]);
	cfString [icfString] = CFStringCreateWithCString (nullptr, (const char *) Melder_peek32to8 (text), kCFStringEncodingUTF8);
	return cfString [icfString];
}
#endif

/* End of file melder_textencoding.cpp */
