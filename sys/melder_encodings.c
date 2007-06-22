/* melder_encodings.c
 *
 * Copyright (C) 2007 Paul Boersma
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
 */

#include "melder.h"
#include "Preferences.h"
#include "UnicodeData.h"
#include "abcio.h"
#define my  me ->

static struct {
	int inputEncoding, outputEncoding;
} prefs = {
	#if defined (_WIN32)
		Melder_INPUT_ENCODING_UTF8_THEN_WINDOWS_LATIN1,
	#elif defined (macintosh)
		Melder_INPUT_ENCODING_UTF8_THEN_MACROMAN,
	#else
		Melder_INPUT_ENCODING_UTF8_THEN_ISO_LATIN1,
	#endif
	Melder_OUTPUT_ENCODING_ASCII_THEN_UTF16
};

void Melder_setInputEncoding (int encoding) { prefs. inputEncoding = encoding; }
int Melder_getInputEncoding (void) { return prefs. inputEncoding; }

void Melder_setOutputEncoding (int encoding) { prefs. outputEncoding = encoding; }
int Melder_getOutputEncoding (void) { return prefs. outputEncoding; }

void Melder_textEncoding_prefs (void) {
	Resources_addInt ("TextEncoding.inputEncoding", & prefs. inputEncoding);
	Resources_addInt ("TextEncoding.outputEncoding", & prefs. outputEncoding);
}

bool Melder_isValidAscii (const wchar_t *text) {
	for (; *text != '\0'; text ++) {
		if (*text > 127) return false;
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

static wchar_t decodeMacRoman [256] = {
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

static wchar_t decodeWindowsLatin1 [256] = {
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

int Melder_8bitToWcs_inline (const unsigned char *string, wchar_t *wcs, int inputEncoding) {
	if (inputEncoding == 0)
		inputEncoding = prefs. inputEncoding;
	long n = strlen ((char *) string), i, j;
	//Melder_casual ("Melder_8bitToWcs_inline: length before = %d", n);
	if (inputEncoding == Melder_INPUT_ENCODING_UTF8 ||
	    inputEncoding == Melder_INPUT_ENCODING_UTF8_THEN_ISO_LATIN1 ||
	    inputEncoding == Melder_INPUT_ENCODING_UTF8_THEN_WINDOWS_LATIN1 ||
		inputEncoding == Melder_INPUT_ENCODING_UTF8_THEN_MACROMAN)
	{
		bool isValidUtf8 = true;
		for (i = 0, j = 0; i < n; i ++) {
			unsigned long kar = (unsigned char) string [i];
			if (kar <= 0x7F) {
				wcs [j ++] = kar;
			} else if (kar <= 0xC1) {
				isValidUtf8 = false; break;
			} else if (kar <= 0xDF) {
				unsigned long kar2 = (unsigned char) string [++ i];
				if (kar2 == '\0' || ! (kar2 & 0x80) || (kar2 & 0x40)) { isValidUtf8 = false; break; }
				wcs [j ++] = ((kar & 0x3F) << 6) | (kar2 & 0x3F);
			} else if (kar <= 0xEF) {
				unsigned long kar2 = (unsigned char) string [++ i];
				if (kar2 == '\0' || ! (kar2 & 0x80) || (kar2 & 0x40)) { isValidUtf8 = false; break; }
				unsigned long kar3 = (unsigned char) string [++ i];
				if (kar3 == '\0' || ! (kar3 & 0x80) || (kar3 & 0x40)) { isValidUtf8 = false; break; }
				wcs [j ++] = ((kar & 0x3F) << 12) | ((kar2 & 0x3F) << 6) | (kar3 & 0x3F);
			} else if (kar <= 0xF4) {
				unsigned long kar2 = (unsigned char) string [++ i];
				if (kar2 == '\0' || ! (kar2 & 0x80) || (kar2 & 0x40)) { isValidUtf8 = false; break; }
				unsigned long kar3 = (unsigned char) string [++ i];
				if (kar3 == '\0' || ! (kar3 & 0x80) || (kar3 & 0x40)) { isValidUtf8 = false; break; }
				unsigned long kar4 = (unsigned char) string [++ i];
				if (kar4 == '\0' || ! (kar4 & 0x80) || (kar4 & 0x40)) { isValidUtf8 = false; break; }
				wcs [j ++] = ((kar & 0x3F) << 18) | ((kar2 & 0x3F) << 12) | ((kar3 & 0x3F) << 6) | (kar4 & 0x3F);
			} else {
				isValidUtf8 = false; break;
			}
		}
		if (isValidUtf8) {
			inputEncoding = Melder_INPUT_ENCODING_UTF8;
			wcs [j] = '\0';
			//Melder_casual ("Melder_8bitToWcs_inline: length after = %d", wcslen (wcs));
		} else if (inputEncoding == Melder_INPUT_ENCODING_UTF8) {
			wcs [0] = '\0';
			return Melder_error1 (L"Text is not valid UTF-8; please try a different text input encoding.");
		} else if (inputEncoding == Melder_INPUT_ENCODING_UTF8_THEN_ISO_LATIN1) {
			inputEncoding = Melder_INPUT_ENCODING_ISO_LATIN1;
		} else if (inputEncoding == Melder_INPUT_ENCODING_UTF8_THEN_WINDOWS_LATIN1) {
			inputEncoding = Melder_INPUT_ENCODING_WINDOWS_LATIN1;
		} else if (inputEncoding == Melder_INPUT_ENCODING_UTF8_THEN_MACROMAN) {
			inputEncoding = Melder_INPUT_ENCODING_MACROMAN;
		}
	}
	if (inputEncoding == Melder_INPUT_ENCODING_ISO_LATIN1) {
		for (i = 0; *string != '\0'; string ++, i ++) {
			wcs [i] = (unsigned char) *string;
		}
		wcs [i] = '\0';
	} else if (inputEncoding == Melder_INPUT_ENCODING_WINDOWS_LATIN1) {
		for (i = 0; *string != '\0'; string ++, i ++) {
			wcs [i] = decodeWindowsLatin1 [(unsigned char) *string];
		}
		wcs [i] = '\0';
	} else if (inputEncoding == Melder_INPUT_ENCODING_MACROMAN) {
		for (i = 0; *string != '\0'; string ++, i ++) {
			wcs [i] = decodeMacRoman [(unsigned char) *string];
		}
		wcs [i] = '\0';
	} else if (inputEncoding != Melder_INPUT_ENCODING_UTF8) {
		Melder_fatal ("Unknown text input encoding %d.", inputEncoding);
	}
	(void) Melder_killReturns_inlineW (wcs);
	return 1;
}

wchar_t * Melder_8bitToWcs (const unsigned char *string, int inputEncoding) {
	if (string == NULL) return NULL;
	wchar_t *result = Melder_malloc ((strlen ((char *) string) + 1) * sizeof (wchar_t));
	if (result == NULL) return NULL;
	if (! Melder_8bitToWcs_inline (string, result, inputEncoding)) {
		Melder_free (result);
	}
	return result;
}

void Melder_asciiToWcs_inline (const char *ascii, wchar_t *wcs) {
	const char *from = & ascii [0];
	wchar_t *to = & wcs [0];
	for (; *from != '\0'; from ++, to ++) {
		*to = (unsigned char) *from;
	}
	*to = L'\0';
}

wchar_t * Melder_asciiToWcs (const char *string) {
	if (string == NULL) return NULL;
	wchar_t *result = Melder_malloc ((strlen (string) + 1) * sizeof (wchar_t));
	if (result == NULL) return NULL;
	Melder_asciiToWcs_inline (string, result);
	return result;
}

void Melder_wcsToAscii_inline (const wchar_t *from, char *to) {
	for (; *from != '\0'; from ++, to ++) {
		*to = (char) (unsigned char) *from;
	}
	*to = L'\0';
}

char * Melder_wcsToAscii (const wchar_t *string) {
	if (! string) return NULL;
	char *result = malloc ((wcslen (string) + 1) * sizeof (char));
	if (result == NULL) return NULL;
	Melder_wcsToAscii_inline (string, result);
	return result;
}

wchar_t * Melder_peekAsciiToWcs (const char *textA) {
	if (textA == NULL) return NULL;
	static MelderStringW buffers [11] = { { 0 } };
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderStringW_copyA (& buffers [ibuffer], textA);
	return buffers [ibuffer]. string;
}

char * Melder_peekWcsToAscii (const wchar_t *textW) {
	if (textW == NULL) return NULL;
	static MelderStringA buffers [11] = { { 0 } };
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderStringA_copyW (& buffers [ibuffer], textW);
	return buffers [ibuffer]. string;
}

wchar_t * Melder_utf8ToWcs (const unsigned char *string) {
	wchar_t *result = Melder_malloc ((strlen ((char *) string) + 1) * sizeof (wchar_t));
	Melder_8bitToWcs_inline (string, result, Melder_INPUT_ENCODING_UTF8);
	return result;
}

void Melder_wcsToUtf8_inline (const wchar_t *wcs, unsigned char *utf8) {
	long n = wcslen (wcs), i, j;
	for (i = 0, j = 0; i < n; i ++) {
		unsigned long kar = wcs [i];
		if (kar <= 0x00007F) {
			#ifdef _WIN32
				if (kar == '\n') utf8 [j ++] = 13;
			#endif
			utf8 [j ++] = kar;
		} else if (kar <= 0x0007FF) {
			utf8 [j ++] = 0xC0 | (kar >> 6);
			utf8 [j ++] = 0x80 | (kar & 0x00003F);
		} else if (kar <= 0x00FFFF) {
			utf8 [j ++] = 0xE0 | (kar >> 12);
			utf8 [j ++] = 0x80 | ((kar >> 6) & 0x00003F);
			utf8 [j ++] = 0x80 | (kar & 0x00003F);
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
	char *result = Melder_malloc ((wcslen (string) * 6 + 1) * sizeof (char));
	Melder_wcsToUtf8_inline (string, (unsigned char *) result);
	return result;
}

wchar_t * Melder_peekUtf8ToWcs (const unsigned char *textA) {
	if (textA == NULL) return NULL;
	static MelderStringW buffers [11] = { { 0 } };
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderStringW_empty (& buffers [ibuffer]);
	unsigned long n = strlen ((char *) textA), i, j;
	for (i = 0, j = 0; i <= n; i ++) {
		unsigned char kar = textA [i];
		if (kar <= 0x7F) {
			MelderStringW_appendCharacter (& buffers [ibuffer], kar);
		} else if (kar <= 0xC1) {
			MelderStringW_appendCharacter (& buffers [ibuffer], '?');
		} else if (kar <= 0xDF) {
			unsigned char kar2 = textA [++ i];
			if (kar2 == '\0' || ! (kar2 & 0x80) || (kar2 & 0x40)) MelderStringW_appendCharacter (& buffers [ibuffer], '?');
			MelderStringW_appendCharacter (& buffers [ibuffer], ((kar & 0x3F) << 6) | (kar2 & 0x3F));
		} else if (kar <= 0xEF) {
			unsigned char kar2 = textA [++ i];
			if (kar2 == '\0' || ! (kar2 & 0x80) || (kar2 & 0x40)) MelderStringW_appendCharacter (& buffers [ibuffer], '?');
			unsigned char kar3 = textA [++ i];
			if (kar3 == '\0' || ! (kar3 & 0x80) || (kar3 & 0x40)) MelderStringW_appendCharacter (& buffers [ibuffer], '?');
			MelderStringW_appendCharacter (& buffers [ibuffer], ((kar & 0x3F) << 12) | ((kar2 & 0x3F) << 6) | (kar3 & 0x3F));
		} else if (kar <= 0xF4) {
			unsigned char kar2 = textA [++ i];
			if (kar2 == '\0' || ! (kar2 & 0x80) || (kar2 & 0x40)) MelderStringW_appendCharacter (& buffers [ibuffer], '?');
			unsigned char kar3 = textA [++ i];
			if (kar3 == '\0' || ! (kar3 & 0x80) || (kar3 & 0x40)) MelderStringW_appendCharacter (& buffers [ibuffer], '?');
			unsigned char kar4 = textA [++ i];
			if (kar4 == '\0' || ! (kar4 & 0x80) || (kar4 & 0x40)) MelderStringW_appendCharacter (& buffers [ibuffer], '?');
			MelderStringW_appendCharacter (& buffers [ibuffer], ((kar & 0x3F) << 18) | ((kar2 & 0x3F) << 12) | ((kar3 & 0x3F) << 6) | (kar4 & 0x3F));
		} else {
			MelderStringW_appendCharacter (& buffers [ibuffer], '?');
		}
	}
	return buffers [ibuffer]. string;
}

char * Melder_peekWcsToUtf8 (const wchar_t *text) {
	if (text == NULL) return NULL;
	static MelderStringA buffers [11] = { { 0 } };
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderStringA_empty (& buffers [ibuffer]);
	unsigned long n = wcslen (text);
	for (unsigned long i = 0; i <= n; i ++) {
		unsigned long kar = text [i];
		if (kar <= 0x00007F) {
			#ifdef _WIN32
				if (kar == '\n') MelderStringA_appendCharacter (& buffers [ibuffer], 13);
			#endif
			MelderStringA_appendCharacter (& buffers [ibuffer], kar);
		} else if (kar <= 0x0007FF) {
			MelderStringA_appendCharacter (& buffers [ibuffer], 0xC0 | (kar >> 6));
			MelderStringA_appendCharacter (& buffers [ibuffer], 0x80 | (kar & 0x00003F));
		} else if (kar <= 0x00FFFF) {
			MelderStringA_appendCharacter (& buffers [ibuffer], 0xE0 | (kar >> 12));
			MelderStringA_appendCharacter (& buffers [ibuffer], 0x80 | ((kar >> 6) & 0x00003F));
			MelderStringA_appendCharacter (& buffers [ibuffer], 0x80 | (kar & 0x00003F));
		} else {
			MelderStringA_appendCharacter (& buffers [ibuffer], 0xF0 | (kar >> 18));
			MelderStringA_appendCharacter (& buffers [ibuffer], 0x80 | ((kar >> 12) & 0x00003F));
			MelderStringA_appendCharacter (& buffers [ibuffer], 0x80 | ((kar >> 6) & 0x00003F));
			MelderStringA_appendCharacter (& buffers [ibuffer], 0x80 | (kar & 0x00003F));
		}
	}
	return buffers [ibuffer]. string;
}

void Melder_fwriteWcsAsUtf8 (const wchar_t *ptr, size_t n, FILE *f) {
	for (size_t i = 0; i < n; i ++) {
		unsigned long kar = ptr [i];
		if (kar <= 0x00007F) {
			#ifdef _WIN32
				if (kar == '\n') fputc (13, f);
			#endif
			fputc (kar, f);
		} else if (kar <= 0x0007FF) {
			fputc (0xC0 | (kar >> 6), f);
			fputc (0x80 | (kar & 0x00003F), f);
		} else if (kar <= 0x00FFFF) {
			fputc (0xE0 | (kar >> 12), f);
			fputc (0x80 | ((kar >> 6) & 0x00003F), f);
			fputc (0x80 | (kar & 0x00003F), f);
		} else {
			fputc (0xF0 | (kar >> 18), f);
			fputc (0x80 | ((kar >> 12) & 0x00003F), f);
			fputc (0x80 | ((kar >> 6) & 0x00003F), f);
			fputc (0x80 | (kar & 0x00003F), f);
		}
	}
}

char * MelderFile_readText (MelderFile file) {
	char *text;
	long length;
	FILE *f;
	if ((f = Melder_fopen (file, "rb")) == NULL) return NULL;
 	fseek (f, 0, SEEK_END);
 	length = ftell (f);
 	rewind (f);
	text = Melder_malloc (length + 1);
	if (! text) { Melder_fclose (file, f); return NULL; }
	fread (text, sizeof (char), length, f);
	if (! Melder_fclose (file, f)) {
		Melder_free (text);
		Melder_error3 (L"Error reading file \"", MelderFile_messageNameW (file), L"\".");
		return NULL;
	}
	text [length] = '\0';
	/*
	 * Convert Mac and DOS files to Unix text.
	 */
	(void) Melder_killReturns_inline (text);
	return text;
}

wchar_t * MelderFile_readTextW (MelderFile file) {
	int type = 0;   // 8-bit
	wchar_t *text = NULL;
	FILE *f = Melder_fopen (file, "rb");
	if (f == NULL) return NULL;
 	fseek (f, 0, SEEK_END);
 	unsigned long length = ftell (f);
 	rewind (f);
	if (length >= 2) {
		int firstByte = fgetc (f), secondByte = fgetc (f);
		if (firstByte == 0xfe && secondByte == 0xff) {
			type = 1;   // big-endian 16-bit
		} else if (firstByte == 0xff && secondByte == 0xfe) {
			type = 2;   // little-endian 16-bit
		}
	}
	if (type == 0) {
		rewind (f);   // length and type already set correctly.
		unsigned char *text8bit = Melder_malloc ((length + 1) * sizeof (char));
		if (! text8bit) { Melder_fclose (file, f); return NULL; }
		fread (text8bit, sizeof (char), length, f);
		if (! Melder_fclose (file, f)) {
			Melder_free (text8bit);
			Melder_error3 (L"Error reading file \"", MelderFile_messageNameW (file), L"\".");
			return NULL;
		}
		text8bit [length] = '\0';
		text = Melder_8bitToWcs (text8bit, 0);
		Melder_free (text8bit);
	} else {
		length = length / 2 - 1;
		text = Melder_malloc ((length + 1) * sizeof (wchar_t));
		if (! text) { Melder_fclose (file, f); return NULL; }
		if (type == 1) {
			for (unsigned long i = 0; i < length; i ++) {
				text [i] = bingetu2 (f);
			}
		} else {
			for (unsigned long i = 0; i < length; i ++) {
				text [i] = bingetu2LE (f);
			}
		}
		if (! Melder_fclose (file, f)) {
			Melder_free (text);
			Melder_error3 (L"Error reading file \"", MelderFile_messageNameW (file), L"\".");
			return NULL;
		}
		text [length] = '\0';

	}
	(void) Melder_killReturns_inlineW (text);
	return text;
}

/* BUG: the following two routines should be made system-independent,
   so that we can write DOS files from Unix etc., as determined by a user preference. */

int MelderFile_writeText (MelderFile file, const char *text) {
	FILE *f = Melder_fopen (file, "w");
	if (! f) return 0;
	/*
	 * On all systems, the number of bytes written (i.e. the return value of fwrite) equals strlen (text).
	 * On Windows, however, the resulting file length will be greater than this.
	 */
	fwrite (text, sizeof (char), strlen (text), f);   /* Not trailing null byte. */
	if (fclose (f))
		return Melder_error ("Error closing file \"%s\".", MelderFile_messageName (file));
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	return 1;
}

int MelderFile_writeTextW (MelderFile file, const wchar_t *text) {
	FILE *f = Melder_fopen (file, "wb");
	if (! f) return 0;
	if (prefs. outputEncoding == Melder_OUTPUT_ENCODING_UTF8) {
		Melder_fwriteWcsAsUtf8 (text, wcslen (text), f);
	} else if (prefs. outputEncoding == Melder_OUTPUT_ENCODING_UTF16) {
		binputu2 (0xfeff, f);
		long n = wcslen (text);
		for (long i = 0; i < n; i ++) {
			wchar_t kar = text [i];
			#ifdef _WIN32
				if (kar == '\n') binputu2 (13, f);
			#endif
			binputu2 (kar, f);
		}
	} else {
		if (Melder_isValidAscii (text)) {
			Melder_fwriteWcsAsUtf8 (text, wcslen (text), f);   // Because this includes CR/LF.
		} else {
			binputu2 (0xfeff, f);
			long n = wcslen (text);
			for (long i = 0; i < n; i ++) {
				wchar_t kar = text [i];
				#ifdef _WIN32
					if (kar == '\n') binputu2 (13, f);
				#endif
				binputu2 (kar, f);
			}
		}
	}
	if (fclose (f)) {
		Melder_error3 (L"Error closing file \"", MelderFile_messageNameW (file), L"\".");
		return 0;
	}
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	return 1;
}

int MelderFile_appendTextW (MelderFile file, const wchar_t *text) {
	FILE *f = Melder_fopen (file, "rb");
	if (f == NULL) {
		Melder_clearError ();
		return MelderFile_writeTextW (file, text);
	}
	/*
	 * The file already exists and is open. Determine its type.
	 */
	int firstByte = fgetc (f), secondByte = fgetc (f);
	fclose (f);
	int type = 0;
	if (firstByte == 0xfe && secondByte == 0xff) {
		type = 1;   // big-endian 16-bit
	} else if (firstByte == 0xff && secondByte == 0xfe) {
		type = 2;   // little-endian 16-bit
	}
	if (type == 0) {
		if (prefs. outputEncoding == Melder_OUTPUT_ENCODING_UTF8) {
			Melder_fwriteWcsAsUtf8 (text, wcslen (text), f);
		} else if (Melder_isValidAscii (text)) {
			/*
			 * Append ASCII text to ASCII file.
			 */
			FILE *f = Melder_fopen (file, "ab");
			if (! f) return 0;
			Melder_fwriteWcsAsUtf8 (text, wcslen (text), f);   // Because this includes CR/LF.
			if (fclose (f))
				return Melder_error3 (L"Error closing file \"", MelderFile_messageNameW (file), L"\".");
		} else {
			/*
			 * Convert to wide character file.
			 */
			wchar_t *oldText = MelderFile_readTextW (file);
			if (oldText == NULL) return 0;
			FILE *f = Melder_fopen (file, "wb");
			if (! f) return 0;
			binputu2 (0xfeff, f);
			unsigned long n = wcslen (oldText);
			for (unsigned long i = 0; i < n; i ++) {
				wchar_t kar = oldText [i];
				#ifdef _WIN32
					if (kar == '\n') binputu2 (13, f);
				#endif
				binputu2 (kar, f);
			}
			n = wcslen (text);
			for (unsigned long i = 0; i < n; i ++) {
				wchar_t kar = text [i];
				#ifdef _WIN32
					if (kar == '\n') binputu2 (13, f);
				#endif
				binputu2 (kar, f);
			}
			if (fclose (f))
				return Melder_error3 (L"Error closing file \"", MelderFile_messageNameW (file), L"\".");
		}
	} else {
		FILE *f = Melder_fopen (file, "ab");
		if (! f) return 0;
		unsigned long n = wcslen (text);
		for (unsigned long i = 0; i < n; i ++) {
			if (type == 1) {
				wchar_t kar = text [i];
				#ifdef _WIN32
					if (kar == '\n') binputu2 (13, f);
				#endif
				binputu2 (kar, f);
			} else {
				wchar_t kar = text [i];
				#ifdef _WIN32
					if (kar == '\n') binputu2LE (13, f);
				#endif
				binputu2LE (kar, f);
			}
		}
		if (fclose (f))
			return Melder_error3 (L"Error closing file \"", MelderFile_messageNameW (file), L"\".");
	}
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	return 1;
}

/* End of file melder_encodings.c */
