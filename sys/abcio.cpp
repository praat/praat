/* abcio.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2003/05/19 accept percent signs in getReal
 * pb 2004/10/01 Melder_double instead of %.17g
 * pb 2006/02/17 support for Intel-based Macs
 * pb 2006/02/20 corrected bingeti3, bingeti3LE, binputi3, binputi3LE
 * pb 2006/03/28 support for systems where a long is not 32 bits and a short is not 16 bits
 * pb 2007/07/21 MelderReadString
 * pb 2007/08/14 check for NULL pointer before Melder_isValidAscii
 * pb 2009/03/18 modern enums
 * fb 2010/02/26 UTF-16 via bin(get|put)utf16()
 * pb 2010/03/09 more support for Unicode values above 0xFFFF
 * pb 2010/12/23 corrected bingeti3 and bingeti3LE for 64-bit systems
 * pb 2011/03/30 C++
 */

#include "melder.h"
#include "NUM.h"
#include <ctype.h>
#ifdef macintosh
	#include <TargetConditionals.h>
#endif
#include "abcio.h"

/********** ASCII I/O **********/

#define WCHAR_MINUS_1  (sizeof (wchar_t) == 2 ? 0xFFFF : 0xFFFFFFFF)

static long getInteger (MelderReadText me) {
	wchar_t buffer [41], c;
	/*
	 * Look for the first numeric character.
	 */
	for (c = MelderReadText_getChar (me); c != '-' && ! isdigit (c) && c != '+'; c = MelderReadText_getChar (me)) {
		if (c == 0)
			Melder_throw ("Early end of text detected while looking for an integer (line ", MelderReadText_getLineNumber (me), ").");
		if (c == '!') {   // end-of-line comment?
			while ((c = MelderReadText_getChar (me)) != '\n' && c != '\r') {
				if (c == 0)
					Melder_throw ("Early end of text detected in comment while looking for an integer (line ", MelderReadText_getLineNumber (me), ").");
			}
		}
		if (c == '\"')
			Melder_throw ("Found a string while looking for an integer in text (line ", MelderReadText_getLineNumber (me), ").");
		if (c == '<')
			Melder_throw ("Found an enumerated value while looking for an integer in text (line ", MelderReadText_getLineNumber (me), ").");
		while (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
			if (c == 0)
				Melder_throw ("Early end of text detected in comment (line ", MelderReadText_getLineNumber (me), ").");
			c = MelderReadText_getChar (me);
		}
	}
	int i = 0;
	for (; i < 40; i ++) {
		buffer [i] = c;
		c = MelderReadText_getChar (me);
		if (c == 0) { break; }   // this may well be OK here
		if (c == ' ' || c == '\n' || c == '\t' || c == '\r') break;
	}
	if (i >= 40)
		Melder_throw ("Found strange text while looking for an integer in text (line ", MelderReadText_getLineNumber (me), ").");
	buffer [i + 1] = '\0';
	return wcstol (buffer, NULL, 10);
}

static unsigned long getUnsigned (MelderReadText me) {
	wchar_t buffer [41], c;
	for (c = MelderReadText_getChar (me); ! isdigit (c) && c != '+'; c = MelderReadText_getChar (me)) {
		if (c == 0)
			Melder_throw ("Early end of text detected while looking for an unsigned integer (line ", MelderReadText_getLineNumber (me), ").");
		if (c == '!') {   // end-of-line comment?
			while ((c = MelderReadText_getChar (me)) != '\n' && c != '\r') {
				if (c == 0)
					Melder_throw ("Early end of text detected in comment while looking for an unsigned integer (line ", MelderReadText_getLineNumber (me), ").");
			}
		}
		if (c == '\"')
			Melder_throw ("Found a string while looking for an unsigned integer in text (line ", MelderReadText_getLineNumber (me), ").");
		if (c == '<')
			Melder_throw ("Found an enumerated value while looking for an unsigned integer in text (line ", MelderReadText_getLineNumber (me), ").");
		if (c == '-')
			Melder_throw ("Found a negative value while looking for an unsigned integer in text (line ", MelderReadText_getLineNumber (me), ").");
		while (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
			if (c == 0)
				Melder_throw ("Early end of text detected in comment (line ", MelderReadText_getLineNumber (me), ").");
			c = MelderReadText_getChar (me);
		}
	}
	int i = 0;
	for (i = 0; i < 40; i ++) {
		buffer [i] = c;
		c = MelderReadText_getChar (me);
		if (c == 0) { break; }   // this may well be OK here
		if (c == ' ' || c == '\n' || c == '\t' || c == '\r') break;
	}
	if (i >= 40)
		Melder_throw ("Found strange text while searching for an unsigned integer in text (line ", MelderReadText_getLineNumber (me), ").");
	buffer [i + 1] = '\0';
	return wcstoul (buffer, NULL, 10);
}

static double getReal (MelderReadText me) {
	int i;
	wchar_t buffer [41], c, *slash;
	do {
		for (c = MelderReadText_getChar (me); c != '-' && ! isdigit (c) && c != '+'; c = MelderReadText_getChar (me)) {
			if (c == 0)
				Melder_throw ("Early end of text detected while looking for a real number (line ", MelderReadText_getLineNumber (me), ").");
			if (c == '!') {   // end-of-line comment?
				while ((c = MelderReadText_getChar (me)) != '\n' && c != '\r') {
					if (c == 0)
						Melder_throw ("Early end of text detected in comment while looking for a real number (line ", MelderReadText_getLineNumber (me), ").");
				}
			}
			if (c == '\"')
				Melder_throw ("Found a string while looking for a real number in text (line ", MelderReadText_getLineNumber (me), ").");
			if (c == '<')
				Melder_throw ("Found an enumerated value while looking for a real number in text (line ", MelderReadText_getLineNumber (me), ").");
			while (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
				if (c == 0)
					Melder_throw ("Early end of text detected in comment while looking for a real number (line ", MelderReadText_getLineNumber (me), ").");
				c = MelderReadText_getChar (me);
			}
		}
		for (i = 0; i < 40; i ++) {
			buffer [i] = c;
			c = MelderReadText_getChar (me);
			if (c == 0) { break; }   // this may well be OK here
			if (c == ' ' || c == '\n' || c == '\t' || c == '\r') break;
		}
		if (i >= 40)
			Melder_throw ("Found strange text while searching for a real number in text (line ", MelderReadText_getLineNumber (me), ").");
	} while (i == 0 && buffer [0] == '+');   // guard against single '+' symbols, which occur in complex numbers
	buffer [i + 1] = '\0';
	slash = wcschr (buffer, '/');
	if (slash) {
		double numerator, denominator;
		*slash = '\0';
		numerator = Melder_atof (buffer), denominator = Melder_atof (slash + 1);
		if (numerator == HUGE_VAL || denominator == HUGE_VAL || denominator == 0.0)
			return HUGE_VAL;
		return numerator / denominator;
	}
	return Melder_atof (buffer);
}

static short getEnum (MelderReadText me, int (*getValue) (const wchar_t *)) {
	wchar_t buffer [41], c;
	for (c = MelderReadText_getChar (me); c != '<'; c = MelderReadText_getChar (me)) {
		if (c == 0)
			Melder_throw ("Early end of text detected while looking for an enumerated value (line ", MelderReadText_getLineNumber (me), ").");
		if (c == '!') {   /* End-of-line comment? */
			while ((c = MelderReadText_getChar (me)) != '\n' && c != '\r') {
				if (c == 0)
					Melder_throw ("Early end of text detected in comment while looking for an enumerated value (line ", MelderReadText_getLineNumber (me), ").");
			}
		}
		if (c == '-' || isdigit (c) || c == '+')
			Melder_throw ("Found a number while looking for an enumerated value in text (line ", MelderReadText_getLineNumber (me), ").");
		if (c == '\"')
			Melder_throw ("Found a string while looking for an enumerated value in text (line ", MelderReadText_getLineNumber (me), ").");
		while (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
			if (c == 0)
				Melder_throw ("Early end of text detected in comment while looking for an enumerated value (line ", MelderReadText_getLineNumber (me), ").");
			c = MelderReadText_getChar (me);
		}
	}
	int i = 0;
	for (; i < 40; i ++) {
		c = MelderReadText_getChar (me);   // read past first '<'
		if (c == 0)
			Melder_throw ("Early end of text detected while reading an enumerated value (line ", MelderReadText_getLineNumber (me), ").");
		if (c == ' ' || c == '\n' || c == '\t' || c == '\r')
			Melder_throw ("No matching '>' while reading an enumerated value (line ", MelderReadText_getLineNumber (me), ").");
		if (c == '>')
			break;   // the expected closing bracket; not added to the buffer
		buffer [i] = c;
	}
	if (i >= 40)
		Melder_throw ("Found strange text while reading an enumerated value in text (line ", MelderReadText_getLineNumber (me), ").");
	buffer [i] = '\0';
	int value = getValue (buffer);
	if (value < 0)
		Melder_throw ("\"", buffer, "\" is not a value of the enumerated type.");
	return value;
}

static wchar_t * getString (MelderReadText me) {
	static MelderString buffer = { 0 };
	MelderString_empty (& buffer);
	for (wchar_t c = MelderReadText_getChar (me); c != '\"'; c = MelderReadText_getChar (me)) {
		if (c == 0)
			Melder_throw ("Early end of text detected while looking for a string (line ", MelderReadText_getLineNumber (me), ").");
		if (c == '!') {   // end-of-line comment?
			while ((c = MelderReadText_getChar (me)) != '\n' && c != '\r') {
				if (c == 0)
					Melder_throw ("Early end of text detected in comment while looking for a string (line ", MelderReadText_getLineNumber (me), ").");
			}
		}
		if (c == '-' || isdigit (c) || c == '+')
			Melder_throw ("Found a number while looking for a string in text (line ", MelderReadText_getLineNumber (me), ").");
		if (c == '<')
			Melder_throw ("Found an enumerated value while looking for a string in text (line ", MelderReadText_getLineNumber (me), ").");
		while (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
			if (c == 0)
				Melder_throw ("Early end of text detected while looking for a string (line ", MelderReadText_getLineNumber (me), ").");
			c = MelderReadText_getChar (me);
		}
	}
	for (int i = 0; 1; i ++) {
		wchar_t c = MelderReadText_getChar (me);   // read past first '"'
		if (c == 0)
			Melder_throw ("Early end of text detected while reading a string (line ", MelderReadText_getLineNumber (me), ").");
		if (c == '\"') {
			wchar_t next = MelderReadText_getChar (me);
			if (next == 0) { break; }   // closing quote is last character in file: OK
			if (next != '\"') {
				if (next == ' ' || next == '\n' || next == '\t' || next == '\r') {
					// closing quote is followed by whitespace: it is OK to skip this whitespace (no need to "ungetChar")
				} else {
					wchar_t kar2 [2] = { next, '\0' };
					Melder_throw ("Character ", kar2, " following quote (line ", MelderReadText_getLineNumber (me), "). End of string or undoubled quote?");
				}
				break;   // the expected closing double quote; not added to the buffer
			}   // else: add only one of the two quotes to the buffer
		}
		MelderString_appendCharacter (& buffer, c);
	}
	return buffer. string;
}

#undef false
#undef true

#include "enums_getText.h"
#include "abcio_enums.h"
#include "enums_getValue.h"
#include "abcio_enums.h"

int texgeti1 (MelderReadText text) {
	try {
		long externalValue = getInteger (text);
		if (externalValue < -128 || externalValue > +127)
			Melder_throw ("Value (", externalValue, ") out of range (-128 .. +127).");
		return (int) externalValue;
	} catch (MelderError) {
		Melder_throw ("Signed small integer not read from text file.");
	}
}

int texgeti2 (MelderReadText text) {
	try {
		long externalValue = getInteger (text);
		if (externalValue < -32768 || externalValue > +32767)
			Melder_throw ("Value (", externalValue, ") out of range (-32768 .. +32767).");
		return (int) externalValue;
	} catch (MelderError) {
		Melder_throw ("Signed short integer not read from text file.");
	}
}

long texgeti4 (MelderReadText text) {
	try {
		long externalValue = getInteger (text);
		return externalValue;
	} catch (MelderError) {
		Melder_throw ("Signed integer not read from text file.");
	}
}

unsigned int texgetu1 (MelderReadText text) {
	try {
		long externalValue = getUnsigned (text);
		if (externalValue > 255)
			Melder_throw ("Value (", externalValue, ") out of range (0 .. 255).");
		return (unsigned int) externalValue;
	} catch (MelderError) {
		Melder_throw ("Unsigned small integer not read from text file.");
	}
}

unsigned int texgetu2 (MelderReadText text) {
	try {
		long externalValue = getUnsigned (text);
		if (externalValue > 65535)
			Melder_throw ("Value (", externalValue, ") out of range (0 .. 65535).");
		return (unsigned int) externalValue;
	} catch (MelderError) {
		Melder_throw ("Unsigned short integer not read from text file.");
	}
}

unsigned long texgetu4 (MelderReadText text) {
	try {
		long externalValue = getUnsigned (text);
		return externalValue;
	} catch (MelderError) {
		Melder_throw ("Unsigned integer not read from text file.");
	}
}

double texgetr4 (MelderReadText text) { return getReal (text); }
double texgetr8 (MelderReadText text) { return getReal (text); }
double texgetr10 (MelderReadText text) { return getReal (text); }
fcomplex texgetc8 (MelderReadText text) { fcomplex z; z.re = getReal (text); z.im = getReal (text); return z; }
dcomplex texgetc16 (MelderReadText text) { dcomplex z; z.re = getReal (text); z.im = getReal (text); return z; }

short texgete1 (MelderReadText text, int (*getValue) (const wchar_t *)) { return getEnum (text, getValue); }
short texgete2 (MelderReadText text, int (*getValue) (const wchar_t *)) { return getEnum (text, getValue); }
short texgeteb (MelderReadText text) { return getEnum (text, kBoolean_getValue); }
short texgeteq (MelderReadText text) { return getEnum (text, kQuestion_getValue); }
short texgetex (MelderReadText text) { return getEnum (text, kExistence_getValue); }
char *texgets2 (MelderReadText text) { return Melder_wcsToUtf8 (getString (text)); }
char *texgets4 (MelderReadText text) { return Melder_wcsToUtf8 (getString (text)); }
wchar_t *texgetw2 (MelderReadText text) { return Melder_wcsdup (getString (text)); }
wchar_t *texgetw4 (MelderReadText text) { return Melder_wcsdup (getString (text)); }

void texindent (MelderFile file) { file -> indent += 4; }
void texexdent (MelderFile file) { file -> indent -= 4; }
void texresetindent (MelderFile file) { file -> indent = 0; }

void texputintro (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	if (file -> verbose) {
		MelderFile_write (file, L"\n");
		for (int iindent = 1; iindent <= file -> indent; iindent ++) {
			MelderFile_write (file, L" ");
		}
		MelderFile_write (file,
			s1 && s1 [0] == 'd' && s1 [1] == '_' ? & s1 [2] : & s1 [0],
			s2 && s2 [0] == 'd' && s2 [1] == '_' ? & s2 [2] : & s2 [0],
			s3 && s3 [0] == 'd' && s3 [1] == '_' ? & s3 [2] : & s3 [0],
			s4 && s4 [0] == 'd' && s4 [1] == '_' ? & s4 [2] : & s4 [0],
			s5 && s5 [0] == 'd' && s5 [1] == '_' ? & s5 [2] : & s5 [0],
			s6 && s6 [0] == 'd' && s6 [1] == '_' ? & s6 [2] : & s6 [0]);
	}
	file -> indent += 4;
}

#define PUTLEADER  \
	MelderFile_write (file, L"\n"); \
	if (file -> verbose) { \
		for (int iindent = 1; iindent <= file -> indent; iindent ++) { \
			MelderFile_write (file, L" "); \
		} \
		MelderFile_write (file, \
			s1 && s1 [0] == 'd' && s1 [1] == '_' ? & s1 [2] : & s1 [0], \
			s2 && s2 [0] == 'd' && s2 [1] == '_' ? & s2 [2] : & s2 [0], \
			s3 && s3 [0] == 'd' && s3 [1] == '_' ? & s3 [2] : & s3 [0], \
			s4 && s4 [0] == 'd' && s4 [1] == '_' ? & s4 [2] : & s4 [0], \
			s5 && s5 [0] == 'd' && s5 [1] == '_' ? & s5 [2] : & s5 [0], \
			s6 && s6 [0] == 'd' && s6 [1] == '_' ? & s6 [2] : & s6 [0]); \
	}

void texputi1 (MelderFile file, int i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = " : NULL, Melder_integer (i), file -> verbose ? L" " : NULL);
}
void texputi2 (MelderFile file, int i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = " : NULL, Melder_integer (i), file -> verbose ? L" " : NULL);
}
void texputi4 (MelderFile file, long i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = " : NULL, Melder_integer (i), file -> verbose ? L" " : NULL);
}
void texputu1 (MelderFile file, unsigned int u, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = " : NULL, Melder_integer (u), file -> verbose ? L" " : NULL);
}
void texputu2 (MelderFile file, unsigned int u, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = " : NULL, Melder_integer (u), file -> verbose ? L" " : NULL);
}
void texputu4 (MelderFile file, unsigned long u, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = " : NULL, Melder_integer (u), file -> verbose ? L" " : NULL);
}
void texputr4 (MelderFile file, double x, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = " : NULL, Melder_single (x), file -> verbose ? L" " : NULL);
}
void texputr8 (MelderFile file, double x, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = " : NULL, Melder_double (x), file -> verbose ? L" " : NULL);
}
void texputc8 (MelderFile file, fcomplex z, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = " : NULL, Melder_single (z.re),
		file -> verbose ? L" + " : L" ", Melder_single (z.im), file -> verbose ? L" i " : NULL);
}
void texputc16 (MelderFile file, dcomplex z, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = " : NULL, Melder_double (z.re),
		file -> verbose ? L" + " : L" ", Melder_double (z.im), file -> verbose ? L" i " : NULL);
}
void texpute1 (MelderFile file, int i, const wchar_t * (*getText) (int), const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = <" : L"<", getText (i), file -> verbose ? L"> " : L">");
}
void texpute2 (MelderFile file, int i, const wchar_t * (*getText) (int), const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = <" : L"<", getText (i), file -> verbose ? L"> " : L">");
}
void texputeb (MelderFile file, bool i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = " : NULL, i ? L"<true>" : L"<false>", file -> verbose ? L" " : NULL);
}
void texputeq (MelderFile file, bool i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L"? " : NULL, i ? L"<yes>" : L"<no>", file -> verbose ? L" " : NULL);
}
void texputex (MelderFile file, bool i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L"? " : NULL, i ? L"<exists>" : L"<absent>", file -> verbose ? L" " : NULL);
}
void texputs1 (MelderFile file, const char *s, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = \"" : L"\"");
	if (s != NULL) {
		char c;
		while ((c = *s ++) != '\0') {
			MelderFile_writeCharacter (file, c);
			if (c == '\"') MelderFile_writeCharacter (file, c);   // double any internal quotes
		}
	}
	MelderFile_write (file, file -> verbose ? L"\" " : L"\"");
}
void texputs2 (MelderFile file, const char *s, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = \"" : L"\"");
	if (s != NULL) {
		char c;
		while ((c = *s ++) != '\0') {
			MelderFile_writeCharacter (file, c);
			if (c == '\"') MelderFile_writeCharacter (file, c);   // double any internal quotes
		}
	}
	MelderFile_write (file, file -> verbose ? L"\" " : L"\"");
}
void texputs4 (MelderFile file, const char *s, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = \"" : L"\"");
	if (s != NULL) {
		char c;
		while ((c = *s ++) != '\0') {
			MelderFile_writeCharacter (file, c);
			if (c == '\"') MelderFile_writeCharacter (file, c);   // double any internal quotes
		}
	}
	MelderFile_write (file, file -> verbose ? L"\" " : L"\"");
}
void texputw2 (MelderFile file, const wchar_t *s, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = \"" : L"\"");
	if (s != NULL) {
		wchar_t c;
		while ((c = *s ++) != '\0') {
			MelderFile_writeCharacter (file, c);
			if (c == '\"') MelderFile_writeCharacter (file, c);   // double any internal quotes
		}
	}
	MelderFile_write (file, file -> verbose ? L"\" " : L"\"");
}
void texputw4 (MelderFile file, const wchar_t *s, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? L" = \"" : L"\"");
	if (s != NULL) {
		wchar_t c;
		while ((c = *s ++) != '\0') {
			MelderFile_writeCharacter (file, c);
			if (c == '\"') MelderFile_writeCharacter (file, c);   // double any internal quotes
		}
	}
	MelderFile_write (file, file -> verbose ? L"\" " : L"\"");
}

/********** machine-independent binary I/O **********/

/* Optimizations for machines for which some of the formats are native. */

/* On which machines is "short" a two's complement Big-Endian (MSB-first) 2-byte word? */

#if defined (macintosh) && TARGET_RT_BIG_ENDIAN == 1
	#define binario_shortBE2 (sizeof (short) == 2)
	#define binario_shortLE2 0
#elif defined (_WIN32) || defined (macintosh) && TARGET_RT_LITTLE_ENDIAN == 1
	#define binario_shortBE2 0
	#define binario_shortLE2 (sizeof (short) == 2)
#else
	#define binario_shortBE2 0
	#define binario_shortLE2 0
#endif

/* On which machines is "long" a two's complement Big-Endian (MSB-first) 4-byte word? */

#if defined (macintosh) && TARGET_RT_BIG_ENDIAN == 1
	#define binario_longBE4 (sizeof (long) == 4)
	#define binario_longLE4 0
#elif defined (_WIN32) || defined (macintosh) && TARGET_RT_LITTLE_ENDIAN == 1
	#define binario_longBE4 0
	#define binario_longLE4 (sizeof (long) == 4)
#else
	#define binario_longBE4 0
	#define binario_longLE4 0
#endif

/* On which machines is "float" IEEE, four bytes, Most Significant Bit first? */

#if defined (macintosh) && TARGET_RT_BIG_ENDIAN == 1
	#define binario_floatIEEE4msb (sizeof (float) == 4)
	#define binario_floatIEEE4lsb 0
#elif defined (_WIN32) || defined (macintosh) && TARGET_RT_LITTLE_ENDIAN == 1
	#define binario_floatIEEE4msb 0
	#define binario_floatIEEE4lsb (sizeof (float) == 4)
#else
	#define binario_floatIEEE4msb 0
	#define binario_floatIEEE4lsb 0
#endif

/* On which machines is "double" IEEE, eight bytes, Most Significant Bit first? */

#if defined (macintosh) && TARGET_RT_BIG_ENDIAN == 1
	#define binario_doubleIEEE8msb (sizeof (double) == 8)
	#define binario_doubleIEEE8lsb 0
#elif defined (_WIN32) || defined (macintosh) && TARGET_RT_LITTLE_ENDIAN == 1
	#define binario_doubleIEEE8msb 0
	#define binario_doubleIEEE8lsb (sizeof (double) == 8)
#else
	#define binario_doubleIEEE8msb 0
	#define binario_doubleIEEE8lsb 0
#endif

/*
	The routines bingetr4, bingetr8, binputr4, and binputr8,
	were implemented by Paul Boersma from the descriptions of the IEEE floating-point formats,
	as found in the MC68881/MC68882 User's Manual by Motorola (second edition, 1989).
	The following copyright notice only refers to the code of bingetr10 and binputr10.
*/

/* Copyright (C) 1988-1991 Apple Computer, Inc.
 * All rights reserved.
 *
 * Warranty Information
 *  Even though Apple has reviewed this software, Apple makes no warranty
 *  or representation, either express or implied, with respect to this
 *  software, its quality, accuracy, merchantability, or fitness for a
 *  particular purpose.  As a result, this software is provided "as is,"
 *  and you, its user, are assuming the entire risk as to its quality
 *  and accuracy.
 *
 * This code may be used and freely distributed as long as it includes
 * this copyright notice and the above warranty information.
 *
 * Machine-independent I/O routines for IEEE floating-point numbers.
 *
 * NaN's and infinities are converted to HUGE_VAL or HUGE, which
 * happens to be infinity on IEEE machines.  Unfortunately, it is
 * impossible to preserve NaN's in a machine-independent way.
 * Infinities are, however, preserved on IEEE machines.
 *
 * These routines have been tested on the following machines:
 *    Apple Macintosh, MPW 3.1 C compiler
 *    Apple Macintosh, THINK C compiler
 *    Silicon Graphics IRIS, MIPS compiler
 *    Cray X/MP and Y/MP
 *    Digital Equipment VAX
 *
 *
 * Implemented by Malcolm Slaney and Ken Turkowski.
 *
 * Malcolm Slaney contributions during 1988-1990 include big- and little-
 * endian file I/O, conversion to and from Motorola's extended 80-bit
 * floating-point format, and conversions to and from IEEE single-
 * precision floating-point format.
 *
 * In 1991, Ken Turkowski implemented the conversions to and from
 * IEEE double-precision format, added more precision to the extended
 * conversions, and accommodated conversions involving +/- infinity,
 * NaN's, and denormalized numbers.
 */

// QUESTION: do the following work correctly if a long is 64 bits?

# define FloatToUnsigned(f)  \
	 ((unsigned long)(((long)((f) - 2147483648.0)) + 2147483647L + 1))

# define UnsignedToFloat(u)    \
	  (((double)((long)((u) - 2147483647L - 1))) + 2147483648.0)

/****************************************************************
 * Extended precision IEEE floating-point conversion routines.
 ****************************************************************/

/*
 * C O N V E R T   T O   I E E E   E X T E N D E D
 */

/*
 * C O N V E R T   F R O M   I E E E   E X T E N D E D  
 */

/*************** End of Apple Computer intermezzo. ***************/

static void readError (FILE *f, const char *text) {
	Melder_throw (feof (f) ? "Reached end of file" : "Error in file", " while trying to read ", text);
}

static void writeError (const char *text) {
	Melder_throw ("Error in file while trying to write ", text);
}

unsigned int bingetu1 (FILE *f) {
	try {
		int externalValue = getc (f);   // either -1 (EOF) or in the range 0..255
		if (externalValue < 0) readError (f, "a byte.");
		return (unsigned int) externalValue;
	} catch (MelderError) {
		Melder_throw ("Unsigned integer not read from 1 byte in binary file.");
	}
}

void binputu1 (unsigned int u, FILE *f) {
	try {
		if (putc (u, f) < 0) writeError ("a byte.");
	} catch (MelderError) {
		Melder_throw ("Unsigned integer not written to 1 byte in binary file.");
	}
}

int bingeti1 (FILE *f) {
	try {
		int externalValue = getc (f);
		if (externalValue < 0) readError (f, "a byte.");
		return (signed char) externalValue;   // this converts e.g. 200 to -56
	} catch (MelderError) {
		Melder_throw ("Signed integer not read from 1 byte in binary file.");
	}
}

void binputi1 (int u, FILE *f) {
	try {
		if (putc (u, f) < 0) writeError ("a byte.");
	} catch (MelderError) {
		Melder_throw ("Signed integer not written to 1 byte in binary file.");
	}
}

int bingete1 (FILE *f, int min, int max, const wchar_t *type) {
	try {
		int externalValue = getc (f);
		if (externalValue < 0) readError (f, "a byte.");
		int result = (signed char) externalValue;   // this converts e.g. 200 to -56, so the enumerated type is signed
		if (result < min || result > max)
			Melder_throw (result, " is not a value of enumerated type <", type, ">.");
		return result;
	} catch (MelderError) {
		Melder_throw ("Enumerated type not read from 1 byte in binary file.");
	}
}

void binpute1 (int value, FILE *f) {
	try {
		if (putc (value, f) < 0) writeError ("a byte.");
	} catch (MelderError) {
		Melder_throw ("Enumerated type not written to 1 byte in binary file.");
	}
}

static int bitsInReadBuffer = 0;
static unsigned char readBuffer;

#define macro_bingetb(nbits) \
unsigned int bingetb##nbits (FILE *f) { \
	unsigned char result; \
	if (bitsInReadBuffer < nbits) { \
		int externalValue = fgetc (f); \
		if (externalValue < 0) readError (f, "a bit."); \
		readBuffer = (unsigned char) externalValue; \
		bitsInReadBuffer = 8; \
	} \
	result = readBuffer << (8 - bitsInReadBuffer); \
	bitsInReadBuffer -= nbits; \
	return result >> (8 - nbits); \
}
macro_bingetb (1)
macro_bingetb (2)
macro_bingetb (3)
macro_bingetb (4)
macro_bingetb (5)
macro_bingetb (6)
macro_bingetb (7)

void bingetb (FILE *f) { (void) f; bitsInReadBuffer = 0; }

int bingeti2 (FILE *f) {
	try {
		if (binario_shortBE2 && Melder_debug != 18) {
			signed short s;
			if (fread (& s, sizeof (signed short), 1, f) != 1) readError (f, "a signed short integer.");
			return (int) s;   // with sign extension if an int is 4 bytes
		} else {
			unsigned char bytes [2];
			if (fread (bytes, sizeof (unsigned char), 2, f) != 2) readError (f, "two bytes.");
			uint16_t externalValue = ((uint16_t) bytes [0] << 8) | (uint16_t) bytes [1];
			return (int) (int16_t) externalValue;   // with sign extension if an int is 4 bytes
		}
	} catch (MelderError) {
		Melder_throw ("Signed integer not read from 2 bytes in binary file.");
	}
}

unsigned int bingetu2 (FILE *f) {
	try {
		if (binario_shortBE2 && Melder_debug != 18) {
			unsigned short s;
			if (fread (& s, sizeof (unsigned short), 1, f) != 1) readError (f, "an unsigned short integer.");
			return s;   // without sign extension
		} else {
			unsigned char bytes [2];
			if (fread (bytes, sizeof (unsigned char), 2, f) != 2) readError (f, "two bytes.");
			uint16_t externalValue = ((uint16_t) bytes [0] << 8) | (uint16_t) bytes [1];
			return (unsigned int) externalValue;
		}
	} catch (MelderError) {
		Melder_throw ("Unsigned integer not read from 2 bytes in binary file.");
	}
}

int bingete2 (FILE *f, int min, int max, const wchar_t *type) {
	try {
		short result;
		if (binario_shortBE2 && Melder_debug != 18) {
			if (fread (& result, sizeof (short), 1, f) != 1) readError (f, "a signed short integer.");
		} else {
			unsigned char bytes [2];
			if (fread (bytes, sizeof (unsigned char), 2, f) != 2) readError (f, "two bytes.");
			uint16_t externalValue = ((uint16_t) bytes [0] << 8) | (uint16_t) bytes [1];
			result = (short) (int16_t) externalValue;
		}
		if (result < min || result > max)
			Melder_throw (result, " is not a value of enumerated type \"", type, L"\".");
		return (int) result;
	} catch (MelderError) {
		Melder_throw ("Enumerated value not read from 2 bytes in binary file.");
	}
}

long bingeti3 (FILE *f) {
	try {
		unsigned char bytes [3];
		if (fread (bytes, sizeof (unsigned char), 3, f) != 3) readError (f, "three bytes.");
		uint32_t externalValue = ((uint32_t) bytes [0] << 16) | ((uint32_t) bytes [1] << 8) | (uint32_t) bytes [2];
		if ((bytes [0] & 128) != 0)   // is the 24-bit sign bit on?
			externalValue |= 0xFF000000;   // extend negative sign to 32 bits
		return (long) (int32_t) externalValue;   // first convert signedness, then perhaps extend sign to 64 bits!
	} catch (MelderError) {
		Melder_throw ("Signed long integer not read from 3 bytes in binary file.");
	}
}

long bingeti4 (FILE *f) {
	try {
		if (binario_longBE4 && Melder_debug != 18) {
			long l;
			if (fread (& l, sizeof (long), 1, f) != 1) readError (f, "a signed long integer.");
			return l;
		} else {
			unsigned char bytes [4];
			if (fread (bytes, sizeof (unsigned char), 4, f) != 4) readError (f, "four bytes.");
			uint32_t externalValue = 
				((uint32_t) bytes [0] << 24) | ((uint32_t) bytes [1] << 16) |
				((uint32_t) bytes [2] << 8) | (uint32_t) bytes [3];
			return (long) (int32_t) externalValue;   // first add signedness, then extend
		}
	} catch (MelderError) {
		Melder_throw ("Signed long integer not read from 4 bytes in binary file.");
	}
}

unsigned long bingetu4 (FILE *f) {
	try {
		if (binario_longBE4 && Melder_debug != 18) {
			unsigned long l;
			if (fread (& l, sizeof (unsigned long), 1, f) != 1) readError (f, "an unsigned long integer.");
			return l;
		} else {
			unsigned char bytes [4];
			if (fread (bytes, sizeof (unsigned char), 4, f) != 4) readError (f, "four bytes.");
			uint32_t externalValue = 
				((uint32_t) bytes [0] << 24) | ((uint32_t) bytes [1] << 16) |
				((uint32_t) bytes [2] << 8) | (uint32_t) bytes [3];
			return (unsigned long) externalValue;
		}
	} catch (MelderError) {
		Melder_throw ("Unsigned long integer not read from 4 bytes in binary file.");
	}
}

int bingeti2LE (FILE *f) {
	try {
		if (binario_shortLE2 && Melder_debug != 18) {
			signed short s;
			if (fread (& s, sizeof (signed short), 1, f) != 1) readError (f, "a signed short integer.");
			return (int) s;   // with sign extension if an int is 4 bytes
		} else {
			unsigned char bytes [2];
			if (fread (bytes, sizeof (unsigned char), 2, f) != 2) readError (f, "two bytes.");
			uint16_t externalValue = ((uint16_t) bytes [1] << 8) | (uint16_t) bytes [0];
			return (int) (int16_t) externalValue;   // with sign extension if an int is 4 bytes
		}
	} catch (MelderError) {
		Melder_throw ("Signed integer not read from 2 bytes in binary file.");
	}
}

unsigned int bingetu2LE (FILE *f) {
	try {
		if (binario_shortLE2 && Melder_debug != 18) {
			unsigned short s;
			if (fread (& s, sizeof (unsigned short), 1, f) != 1) readError (f, "an unsigned short integer.");
			return s;   // without sign extension
		} else {
			unsigned char bytes [2];
			if (fread (bytes, sizeof (unsigned char), 2, f) != 2) readError (f, "two bytes.");
			uint16_t externalValue = ((uint16_t) bytes [1] << 8) | (uint16_t) bytes [0];
			return (unsigned int) externalValue;
		}
	} catch (MelderError) {
		Melder_throw ("Unsigned integer not read from 2 bytes in binary file.");
	}
}

long bingeti3LE (FILE *f) {
	try {
		unsigned char bytes [3];
		if (fread (bytes, sizeof (unsigned char), 3, f) != 3) readError (f, "three bytes.");
		uint32_t externalValue = ((uint32_t) bytes [2] << 16) | ((uint32_t) bytes [1] << 8) | (uint32_t) bytes [0];
		if ((bytes [2] & 128) != 0)   // is the 24-bit sign bit on?
			externalValue |= 0xFF000000;   // extend negative sign to 32 bits
		return (long) (int32_t) externalValue;   // first convert signedness, then perhaps extend sign to 64 bits!
	} catch (MelderError) {
		Melder_throw ("Signed long integer not read from 3 bytes in binary file.");
	}
}

long bingeti4LE (FILE *f) {
	try {
		if (binario_longLE4 && Melder_debug != 18) {
			long l;
			if (fread (& l, sizeof (long), 1, f) != 1) readError (f, "a signed long integer.");
			return l;
		} else {
			unsigned char bytes [4];
			if (fread (bytes, sizeof (unsigned char), 4, f) != 4) readError (f, "four bytes.");
			uint32_t externalValue = ((uint32_t) bytes [3] << 24) | ((uint32_t) bytes [2] << 16) |
				((uint32_t) bytes [1] << 8) | (uint32_t) bytes [0];
			return (long) (int32_t) externalValue;   // first add signedness, then extend
		}
	} catch (MelderError) {
		Melder_throw ("Signed long integer not read from 4 bytes in binary file.");
	}
}

unsigned long bingetu4LE (FILE *f) {
	try {
		if (binario_longLE4 && Melder_debug != 18) {
			unsigned long l;
			if (fread (& l, sizeof (unsigned long), 1, f) != 1) readError (f, "an unsigned long integer.");
			return l;
		} else {
			unsigned char bytes [4];
			if (fread (bytes, sizeof (unsigned char), 4, f) != 4) readError (f, "four bytes.");
			uint32_t externalValue = ((uint32_t) bytes [3] << 24) | ((uint32_t) bytes [2] << 16) |
				((uint32_t) bytes [1] << 8) | (uint32_t) bytes [0];
			return (unsigned long) externalValue;
		}
	} catch (MelderError) {
		Melder_throw ("Unsigned long integer not read from 4 bytes in binary file.");
	}
}

double bingetr4 (FILE *f) {
	try {
		if (binario_floatIEEE4msb && Melder_debug != 18) {
			float x;
			if (fread (& x, sizeof (float), 1, f) != 1) readError (f, "a single-precision floating-point number.");
			return x;
		} else {
			unsigned char bytes [4];
			if (fread (bytes, sizeof (unsigned char), 4, f) != 4) readError (f, "four bytes.");
			long exponent = ((unsigned long) (bytes [0] & 0x7F) << 1) | ((unsigned long) (bytes [1] & 0x80) >> 7);   // 32 or 64 bits
			unsigned long mantissa = ((unsigned long) (bytes [1] & 0x7F) << 16) | ((unsigned long) bytes [2] << 8) | (unsigned long) bytes [3];   // 32 or 64 bits
			double x;
			if (exponent == 0)
				if (mantissa == 0) x = 0.0;
				else x = ldexp (UnsignedToFloat (mantissa), exponent - 149);   // denormalized
			else if (exponent == 0x00FF)   // Infinity or Not-a-Number
				x = HUGE_VAL;
			else   // finite
				x = ldexp (UnsignedToFloat (mantissa | 0x00800000), exponent - 150);
			return bytes [0] & 0x80 ? - x : x;
		}
	} catch (MelderError) {
		Melder_throw ("Floating-point number not read from 4 bytes in binary file.");
	}
}

double bingetr4LE (FILE *f) {
	try {
		if (binario_floatIEEE4lsb && Melder_debug != 18) {
			float x;
			if (fread (& x, sizeof (float), 1, f) != 1) readError (f, "a single-precision floating-point number.");
			return x;
		} else {
			unsigned char bytes [4];
			if (fread (bytes, sizeof (unsigned char), 4, f) != 4) readError (f, "four bytes.");
			long exponent = ((unsigned long) (bytes [3] & 0x7F) << 1) | ((unsigned long) (bytes [2] & 0x80) >> 7);   // 32 or 64 bits
			unsigned long mantissa = ((unsigned long) (bytes [2] & 0x7F) << 16) | ((unsigned long) bytes [1] << 8) | (unsigned long) bytes [0];   // 32 or 64 bits
			double x;
			if (exponent == 0)
				if (mantissa == 0) x = 0.0;
				else x = ldexp (UnsignedToFloat (mantissa), exponent - 149);   // denormalized
			else if (exponent == 0x00FF)   // Infinity or Not-a-Number. */
				x = HUGE_VAL;
			else   // finite
				x = ldexp (UnsignedToFloat (mantissa | 0x00800000), exponent - 150);
			return bytes [3] & 0x80 ? - x : x;
		}
	} catch (MelderError) {
		Melder_throw ("Floating-point number not read from 4 bytes in binary file.");
	}
}

double bingetr8 (FILE *f) {
	try {
		if (binario_doubleIEEE8msb && Melder_debug != 18) {
			double x;
			if (fread (& x, sizeof (double), 1, f) != 1) readError (f, "a double-precision floating-point number.");
			return x;
		} else {
			unsigned char bytes [8];
			if (fread (bytes, sizeof (unsigned char), 8, f) != 8) readError (f, "eight bytes.");
			Melder_assert (sizeof (long) >= 4);
			long exponent = ((unsigned long) (bytes [0] & 0x7F) << 4) | ((unsigned long) (bytes [1] & 0xF0) >> 4);
			unsigned long highMantissa = ((unsigned long) (bytes [1] & 0x0F) << 16) | ((unsigned long) bytes [2] << 8) | (unsigned long) bytes [3];
			unsigned long lowMantissa = ((unsigned long) bytes [4] << 24) | ((unsigned long) bytes [5] << 16) | ((unsigned long) bytes [6] << 8) | (unsigned long) bytes [7];
			double x;
			if (exponent == 0)
				if (highMantissa == 0 && lowMantissa == 0) x = 0.0;
				else x = ldexp (UnsignedToFloat (highMantissa), exponent - 1042) +
					ldexp (UnsignedToFloat (lowMantissa), exponent - 1074);   // denormalized
			else if (exponent == 0x07FF)   // Infinity or Not-a-Number
				x = HUGE_VAL;
			else
				x = ldexp (UnsignedToFloat (highMantissa | 0x00100000), exponent - 1043) +
					ldexp (UnsignedToFloat (lowMantissa), exponent - 1075);
			return bytes [0] & 0x80 ? - x : x;
		}
	} catch (MelderError) {
		Melder_throw ("Floating-point number not read from 8 bytes in binary file.");
	}
}

double bingetr10 (FILE *f) {
	try {
		unsigned char bytes [10];
		if (fread (bytes, sizeof (unsigned char), 10, f) != 10) readError (f, "ten bytes.");
		long exponent = ((bytes [0] & 0x7F) << 8) | bytes [1];
		unsigned long highMantissa = ((unsigned long) bytes [2] << 24) | ((unsigned long) bytes [3] << 16) | ((unsigned long) bytes [4] << 8) | (unsigned long) bytes [5];
		unsigned long lowMantissa = ((unsigned long) bytes [6] << 24) | ((unsigned long) bytes [7] << 16) | ((unsigned long) bytes [8] << 8) | (unsigned long) bytes [9];
		double x;
		if (exponent == 0 && highMantissa == 0 && lowMantissa == 0) x = 0;
		else if (exponent == 0x7FFF) x = HUGE_VAL;   /* Infinity or NaN */
		else {
			exponent -= 16383;
			x = ldexp (UnsignedToFloat (highMantissa), exponent - 31);
			x += ldexp (UnsignedToFloat (lowMantissa), exponent - 63);
		}
		return bytes [0] & 0x80 ? - x : x;
	} catch (MelderError) {
		Melder_throw ("Floating-point number not read from 10 bytes in binary file.");
	}
}

static int bitsInWriteBuffer = 0;
static unsigned char writeBuffer = 0;

#define macro_binputb(nbits) \
void binputb##nbits (unsigned int value, FILE *f) { \
	if (bitsInWriteBuffer + nbits > 8) { \
		if (fputc (writeBuffer, f) < 0) writeError ("a bit."); \
		bitsInWriteBuffer = 0; \
		writeBuffer = 0; \
	} \
	writeBuffer |= (value << (8 - nbits)) >> bitsInWriteBuffer; \
	bitsInWriteBuffer += nbits; \
}
macro_binputb (1)
macro_binputb (2)
macro_binputb (3)
macro_binputb (4)
macro_binputb (5)
macro_binputb (6)
macro_binputb (7)
void binputb (FILE *f) {
	if (bitsInWriteBuffer == 0) return;
	if (fputc (writeBuffer, f) < 0) writeError ("a bit.");   // flush
	bitsInWriteBuffer = 0;
	writeBuffer = 0;
}

void binputi2 (int i, FILE *f) {
	try {
		if (binario_shortBE2 && Melder_debug != 18) {
			short s = i;
			if (fwrite (& s, sizeof (short), 1, f) != 1) writeError ("a signed short integer.");
		} else {
			char bytes [2];
			bytes [0] = i >> 8;
			bytes [1] = i;
			if (fwrite (bytes, sizeof (char), 2, f) != 2) writeError ("two bytes.");
		}
	} catch (MelderError) {
		Melder_throw ("Signed integer not written to 2 bytes in binary file.");
	}
}

void binputu2 (unsigned int u, FILE *f) {
	try {
		if (binario_shortBE2 && Melder_debug != 18) {
			unsigned short s = u;
			if (fwrite (& s, sizeof (unsigned short), 1, f) != 1) writeError ("an unsigned short integer.");
		} else {
			char bytes [2];
			bytes [0] = u >> 8;
			bytes [1] = u;
			if (fwrite (bytes, sizeof (char), 2, f) != 2) writeError ("two bytes.");
		}
	} catch (MelderError) {
		Melder_throw ("Unsigned integer not written to 2 bytes in binary file.");
	}
}

void binpute2 (int value, FILE *f) {
	try {
		if (binario_shortBE2 && Melder_debug != 18) {
			short s = value;
			if (fwrite (& s, sizeof (short), 1, f) != 1) writeError ("a signed short integer");
		} else {
			char bytes [2];
			bytes [0] = value >> 8;
			bytes [1] = value;
			if (fwrite (bytes, sizeof (char), 2, f) != 2) writeError ("two bytes.");
		}
	} catch (MelderError) {
		Melder_throw ("Enumerated value not written to 2 bytes in binary file.");
	}
}

void binputi3 (long i, FILE *f) {
	try {
		char bytes [3];
		bytes [0] = i >> 16;
		bytes [1] = i >> 8;
		bytes [2] = i;
		if (fwrite (bytes, sizeof (char), 3, f) != 3) writeError ("three bytes");
	} catch (MelderError) {
		Melder_throw ("Signed integer not written to 3 bytes in binary file.");
	}
}

void binputi4 (long i, FILE *f) {
	try {
		if (binario_longBE4 && Melder_debug != 18) {
			if (fwrite (& i, sizeof (long), 1, f) != 1) writeError ("a signed long integer.");
		} else {
			char bytes [4];
			bytes [0] = i >> 24;
			bytes [1] = i >> 16;
			bytes [2] = i >> 8;
			bytes [3] = i;
			if (fwrite (bytes, sizeof (char), 4, f) != 4) writeError ("four bytes.");
		}
	} catch (MelderError) {
		Melder_throw ("Signed integer not written to 4 bytes in binary file.");
	}
}

void binputu4 (unsigned long u, FILE *f) {
	try {
		if (binario_longBE4 && Melder_debug != 18) {
			if (fwrite (& u, sizeof (unsigned long), 1, f) != 1) writeError ("an unsigned long integer.");
		} else {
			char bytes [4];
			bytes [0] = u >> 24;
			bytes [1] = u >> 16;
			bytes [2] = u >> 8;
			bytes [3] = u;
			if (fwrite (bytes, sizeof (char), 4, f) != 4) writeError ("four bytes.");
		}
	} catch (MelderError) {
		Melder_throw ("Unsigned integer not written to 4 bytes in binary file.");
	}
}

void binputi2LE (int i, FILE *f) {
	try {
		if (binario_shortLE2 && Melder_debug != 18) {
			short s = i;
			if (fwrite (& s, sizeof (short), 1, f) != 1) writeError ("a signed short integer.");
		} else {
			char bytes [2];
			bytes [1] = i >> 8;
			bytes [0] = i;
			if (fwrite (bytes, sizeof (char), 2, f) != 2) writeError ("two bytes.");
		}
	} catch (MelderError) {
		Melder_throw ("Signed integer not written to 2 bytes in binary file.");
	}
}

void binputu2LE (unsigned int u, FILE *f) {
	try {
		if (binario_shortLE2 && Melder_debug != 18) {
			unsigned short s = u;
			if (fwrite (& s, sizeof (unsigned short), 1, f) != 1) writeError ("an unsigned short integer.");
		} else {
			char bytes [2];
			bytes [1] = u >> 8;
			bytes [0] = u;
			if (fwrite (bytes, sizeof (char), 2, f) != 2) writeError ("two bytes.");
		}
	} catch (MelderError) {
		Melder_throw ("Unsigned integer not written to 2 bytes in binary file.");
	}
}

void binputi3LE (long i, FILE *f) {
	try {
		char bytes [3];
		bytes [2] = i >> 16;
		bytes [1] = i >> 8;
		bytes [0] = i;
		if (fwrite (bytes, sizeof (char), 3, f) != 3) writeError ("three bytes");
	} catch (MelderError) {
		Melder_throw ("Signed integer not written to 3 bytes in binary file.");
	}
}

void binputi4LE (long i, FILE *f) {
	try {
		if (binario_longLE4 && Melder_debug != 18) {
			if (fwrite (& i, sizeof (long), 1, f) != 1) writeError ("a signed long integer.");
		} else {
			char bytes [4];
			bytes [3] = i >> 24;
			bytes [2] = i >> 16;
			bytes [1] = i >> 8;
			bytes [0] = i;
			if (fwrite (bytes, sizeof (char), 4, f) != 4) writeError ("four bytes.");
		}
	} catch (MelderError) {
		Melder_throw ("Signed integer not written to 4 bytes in binary file.");
	}
}

void binputu4LE (unsigned long u, FILE *f) {
	try {
		if (binario_longLE4 && Melder_debug != 18) {
			if (fwrite (& u, sizeof (unsigned long), 1, f) != 1) writeError ("an unsigned long integer.");
		} else {
			char bytes [4];
			bytes [3] = u >> 24;
			bytes [2] = u >> 16;
			bytes [1] = u >> 8;
			bytes [0] = u;
			if (fwrite (bytes, sizeof (char), 4, f) != 4) writeError ("four bytes.");
		}
	} catch (MelderError) {
		Melder_throw ("Unsigned integer not written to 4 bytes in binary file.");
	}
}

void binputr4 (double x, FILE *f) {
	try {
		if (binario_floatIEEE4msb && Melder_debug != 18) {
			float x4 = x;
			if (fwrite (& x4, sizeof (float), 1, f) != 1) writeError ("a single-precision floating-point number.");
		} else {
			unsigned char bytes [4];
			int sign, exponent;
			double fMantissa, fsMantissa;
			unsigned long mantissa;
			if (x < 0.0) { sign = 0x0100; x *= -1; }
			else sign = 0;
			if (x == 0.0) { exponent = 0; mantissa = 0; }
			else {
				fMantissa = frexp (x, & exponent);
				if ((exponent > 128) || ! (fMantissa < 1))   // Infinity or Not-a-Number
					{ exponent = sign | 0x00FF; mantissa = 0; }   // Infinity
				else {   /* Finite. */
					exponent += 126;   // add bias
					if (exponent <= 0) {   // denormalized
						fMantissa = ldexp (fMantissa, exponent - 1);
						exponent = 0;
					}
					exponent |= sign;
					fMantissa = ldexp (fMantissa, 24);          
					fsMantissa = floor (fMantissa); 
					mantissa = FloatToUnsigned (fsMantissa) & 0x007FFFFF;
				}
			}
			bytes [0] = exponent >> 1;
			bytes [1] = (exponent << 7) | (mantissa >> 16);
			bytes [2] = mantissa >> 8;
			bytes [3] = mantissa;
			if (fwrite (bytes, sizeof (unsigned char), 4, f) != 4) writeError ("four bytes.");
		}
	} catch (MelderError) {
		Melder_throw ("Floating-point number not written to 4 bytes in binary file.");
	}
}

void binputr4LE (double x, FILE *f) {
	try {
		if (binario_floatIEEE4lsb && Melder_debug != 18) {
			float x4 = x;
			if (fwrite (& x4, sizeof (float), 1, f) != 1) writeError ("a single-precision floating-point number.");
		} else {
			unsigned char bytes [4];
			int sign, exponent;
			double fMantissa, fsMantissa;
			unsigned long mantissa;
			if (x < 0.0) { sign = 0x0100; x *= -1; }
			else sign = 0;
			if (x == 0.0) { exponent = 0; mantissa = 0; }
			else {
				fMantissa = frexp (x, & exponent);
				if ((exponent > 128) || ! (fMantissa < 1))   // Infinity or Not-a-Number
					{ exponent = sign | 0x00FF; mantissa = 0; }   // Infinity
				else {   /* Finite. */
					exponent += 126;   // add bias
					if (exponent <= 0) {   // denormalized
						fMantissa = ldexp (fMantissa, exponent - 1);
						exponent = 0;
					}
					exponent |= sign;
					fMantissa = ldexp (fMantissa, 24);          
					fsMantissa = floor (fMantissa); 
					mantissa = FloatToUnsigned (fsMantissa) & 0x007FFFFF;
				}
			}
			bytes [3] = exponent >> 1;
			bytes [2] = (exponent << 7) | (mantissa >> 16);
			bytes [1] = mantissa >> 8;
			bytes [0] = mantissa;
			if (fwrite (bytes, sizeof (unsigned char), 4, f) != 4) writeError ("four bytes.");
		}
	} catch (MelderError) {
		Melder_throw ("Floating-point number not written to 4 bytes in binary file.");
	}
}

void binputr8 (double x, FILE *f) {
	try {
		if (binario_doubleIEEE8msb && Melder_debug != 18) {
			if (fwrite (& x, sizeof (double), 1, f) != 1) writeError ("a double-precision floating-point number.");
		} else {
			unsigned char bytes [8];
			int sign, exponent;
			double fMantissa, fsMantissa;
			unsigned long highMantissa, lowMantissa;
			if (x < 0.0) { sign = 0x0800; x *= -1; }
			else sign = 0;
			if (x == 0.0) { exponent = 0; highMantissa = 0; lowMantissa = 0; }
			else {
				fMantissa = frexp (x, & exponent);
				if ((exponent > 1024) || ! (fMantissa < 1))   // Infinity or Not-a-Number
					{ exponent = sign | 0x07FF; highMantissa = 0; lowMantissa = 0; }   // Infinity
				else { /* Finite. */
					exponent += 1022;   // add bias
					if (exponent <= 0) {   // denormalized
						fMantissa = ldexp (fMantissa, exponent - 1);
						exponent = 0;
					}
					exponent |= sign;
					fMantissa = ldexp (fMantissa, 21);          
					fsMantissa = floor (fMantissa); 
					highMantissa = FloatToUnsigned (fsMantissa) & 0x000FFFFF;
					fMantissa = ldexp (fMantissa - fsMantissa, 32); 
					fsMantissa = floor (fMantissa); 
					lowMantissa = FloatToUnsigned (fsMantissa);
				}
			}
			bytes [0] = exponent >> 4;
			bytes [1] = (exponent << 4) | (highMantissa >> 16);
			bytes [2] = highMantissa >> 8;
			bytes [3] = highMantissa;
			bytes [4] = lowMantissa >> 24;
			bytes [5] = lowMantissa >> 16;
			bytes [6] = lowMantissa >> 8;
			bytes [7] = lowMantissa;
			if (fwrite (bytes, sizeof (unsigned char), 8, f) != 8) writeError ("eight bytes.");
		}
	} catch (MelderError) {
		Melder_throw ("Floating-point number not written to 8 bytes in binary file.");
	}
}

void binputr10 (double x, FILE *f) {
	try {
		unsigned char bytes [10];
		int sign, exponent;
		double fMantissa, fsMantissa;
		unsigned long highMantissa, lowMantissa;
		if (x < 0.0) { sign = 0x8000; x *= -1; }
		else sign = 0;
		if (x == 0.0) { exponent = 0; highMantissa = 0; lowMantissa = 0; }
		else {
			fMantissa = frexp (x, & exponent);
			if ((exponent > 16384) || ! (fMantissa < 1))   // Infinity or Not-a-Number
				{ exponent = sign | 0x7FFF; highMantissa = 0; lowMantissa = 0; }   // Infinity
			else {   /* Finite */
				exponent += 16382;   /* Add bias. */
				if (exponent < 0) {   /* Denormalized. */
					fMantissa = ldexp (fMantissa, exponent);
					exponent = 0;
				}
				exponent |= sign;
				fMantissa = ldexp (fMantissa, 32);          
				fsMantissa = floor (fMantissa); 
				highMantissa = FloatToUnsigned (fsMantissa);
				fMantissa = ldexp (fMantissa - fsMantissa, 32); 
				fsMantissa = floor (fMantissa); 
				lowMantissa = FloatToUnsigned (fsMantissa);
			}
		}
		bytes [0] = exponent >> 8;
		bytes [1] = exponent;
		bytes [2] = highMantissa >> 24;
		bytes [3] = highMantissa >> 16;
		bytes [4] = highMantissa >> 8;
		bytes [5] = highMantissa;
		bytes [6] = lowMantissa >> 24;
		bytes [7] = lowMantissa >> 16;
		bytes [8] = lowMantissa >> 8;
		bytes [9] = lowMantissa;
		if (fwrite (bytes, sizeof (unsigned char), 10, f) != 10) writeError ("ten bytes.");
	} catch (MelderError) {
		Melder_throw ("Floating-point number not written to 10 bytes in binary file.");
	}
}

fcomplex bingetc8 (FILE *f) {
	try {
		fcomplex result;
		result. re = bingetr4 (f);
		result. im = bingetr4 (f);
		return result;
	} catch (MelderError) {
		Melder_throw ("Complex number not read from 8 bytes in binary file.");
		fcomplex result = { 0 };
		return result;
	}
}

dcomplex bingetc16 (FILE *f) {
	try {
		dcomplex result;
		result. re = bingetr8 (f);
		result. im = bingetr8 (f);
		return result;
	} catch (MelderError) {
		Melder_throw ("Complex number not read from 16 bytes in binary file.");
		dcomplex result = { 0 };
		return result;
	}
}

void binputc8 (fcomplex z, FILE *f) {
	try {
		binputr4 (z. re, f);
		binputr4 (z. im, f);
	} catch (MelderError) {
		Melder_throw ("Complex number not written to 8 bytes in binary file.");
	}
}

void binputc16 (dcomplex z, FILE *f) {
	try {
		binputr8 (z. re, f);
		binputr8 (z. im, f);
	} catch (MelderError) {
		Melder_throw ("Complex number not written to 16 bytes in binary file.");
	}
}

char * bingets1 (FILE *f) {
	try {
		unsigned int length = bingetu1 (f);
		autostring8 result = Melder_malloc (char, length + 1);
		if (fread (result.peek(), sizeof (char), length, f) != length)
			Melder_throw (feof (f) ? "Reached end of file" : "Error in file", " while trying to read ", length, " one-byte characters.");
		result [length] = 0;   // trailing null byte
		return result.transfer();
	} catch (MelderError) {
		Melder_throw ("Text not read from a binary file.");
	}
}

char * bingets2 (FILE *f) {
	try {
		unsigned int length = bingetu2 (f);
		autostring8 result = Melder_malloc (char, length + 1);
		if (fread (result.peek(), sizeof (char), length, f) != length)
			Melder_throw (feof (f) ? "Reached end of file" : "Error in file", " while trying to read ", length, " one-byte characters.");
		result [length] = 0;   // trailing null byte
		return result.transfer();
	} catch (MelderError) {
		Melder_throw ("Text not read from a binary file.");
	}
}

char * bingets4 (FILE *f) {
	try {
		unsigned long length = bingetu4 (f);
		autostring8 result = Melder_malloc (char, length + 1);
		if (fread (result.peek(), sizeof (char), length, f) != length)
			Melder_throw (feof (f) ? "Reached end of file" : "Error in file", " while trying to read ", length, " one-byte characters.");
		result [length] = 0;   // trailing null byte
		return result.transfer();
	} catch (MelderError) {
		Melder_throw ("Text not read from a binary file.");
	}
}

wchar_t * bingetw1 (FILE *f) {
	try {
		autostring result = NULL;
		unsigned short length = bingetu1 (f);
		if (length == 0xFF) {   // an escape for encoding
			/*
			 * UTF-16
			 */
			length = bingetu1 (f);
			result.reset (Melder_malloc (wchar_t, length + 1));
			for (unsigned short i = 0; i < length; i ++) {
				if (sizeof (wchar_t) == 2) {
					result [i] = bingetu2 (f);
				} else {
					uint16_t kar = bingetu2 (f);
					if ((kar & 0xF800) == 0xD800) {
						if (kar > 0xDBFF)
							Melder_throw ("Incorrect Unicode value (first surrogate member ", kar, ").");
						uint16_t kar2 = bingetu2 (f);
						if (kar2 < 0xDC00 || kar2 > 0xDFFF)
							Melder_throw ("Incorrect Unicode value (second surrogate member ", kar2, ").");
						result [i] = (((kar & 0x3FF) << 10) | (kar2 & 0x3FF)) + 0x10000;
					} else {
						result [i] = kar;
					}
				}
			}
		} else {
			/*
			 * ASCII
			 */
			result.reset (Melder_malloc (wchar_t, length + 1));
			for (unsigned short i = 0; i < length; i ++) {
				result [i] = bingetu1 (f);
			}
		}
		result [length] = L'\0';
		return result.transfer();
	} catch (MelderError) {
		Melder_throw ("Text not read from a binary file.");
	}
}

wchar_t * bingetw2 (FILE *f) {
	try {
		autostring result = NULL;
		unsigned short length = bingetu2 (f);
		if (length == 0xFFFF) {   // an escape for encoding
			/*
			 * UTF-16
			 */
			length = bingetu2 (f);
			result.reset (Melder_malloc (wchar_t, length + 1));
			for (unsigned short i = 0; i < length; i ++) {
				if (sizeof (wchar_t) == 2) {
					result [i] = bingetu2 (f);
				} else {
					unsigned short kar = bingetu2 (f);
					if ((kar & 0xF800) == 0xD800) {
						if (kar > 0xDBFF)
							Melder_throw ("Incorrect Unicode value (first surrogate member ", kar, ").");
						unsigned short kar2 = bingetu2 (f);
						if (kar2 < 0xDC00 || kar2 > 0xDFFF)
							Melder_throw ("Incorrect Unicode value (second surrogate member ", kar2, ").");
						result [i] = (((kar & 0x3FF) << 10) | (kar2 & 0x3FF)) + 0x10000;
					} else {
						result [i] = kar;
					}
				}
			}
		} else {
			/*
			 * ASCII
			 */
			result.reset (Melder_malloc (wchar_t, length + 1));
			for (unsigned short i = 0; i < length; i ++) {
				result [i] = bingetu1 (f);
			}
		}
		result [length] = L'\0';
		return result.transfer();
	} catch (MelderError) {
		Melder_throw ("Text not read from a binary file.");
	}
}

wchar_t * bingetw4 (FILE *f) {
	try {
		autostring result = NULL;
		unsigned long length = bingetu4 (f);
		if (length == 0xFFFFFFFF) {   // an escape for encoding
			/*
			 * UTF-16
			 */
			length = bingetu4 (f);
			result.reset (Melder_malloc (wchar_t, length + 1));
			for (unsigned long i = 0; i < length; i ++) {
				if (sizeof (wchar_t) == 2) {
					result [i] = bingetu2 (f);
				} else {
					unsigned short kar = bingetu2 (f);
					if ((kar & 0xF800) == 0xD800) {
						if (kar > 0xDBFF)
							Melder_throw ("Incorrect Unicode value (first surrogate member ", kar, ").");
						unsigned short kar2 = bingetu2 (f);
						if (kar2 < 0xDC00 || kar2 > 0xDFFF)
							Melder_throw ("Incorrect Unicode value (second surrogate member ", kar2, ").");
						result [i] = (((kar & 0x3FF) << 10) | (kar2 & 0x3FF)) + 0x10000;
					} else {
						result [i] = kar;
					}
				}
			}
		} else {
			/*
			 * ASCII
			 */
			result.reset (Melder_malloc (wchar_t, length + 1));
			for (unsigned long i = 0; i < length; i ++) {
				result [i] = bingetu1 (f);
			}
		}
		result [length] = L'\0';
		return result.transfer();
	} catch (MelderError) {
		Melder_throw ("Text not read from a binary file.");
	}
}

void binputs1 (const char *s, FILE *f) {
	try {
		if (s == NULL) {
			binputu1 (0, f);
		} else {
			unsigned long length = strlen (s);
			if (length > 255) {
				Melder_warning ("Text of ", length, " characters truncated to 255 characters.");
				length = 255;
			}
			binputu1 (length, f);
			if (fwrite (s, sizeof (char), length, f) != length)
				Melder_throw ("Error in file while trying to write ", length, " one-byte characters.");
		}
	} catch (MelderError) {
		Melder_throw ("Text not written to a binary file.");
	}
}

void binputs2 (const char *s, FILE *f) {
	try {
		if (s == NULL) {
			binputu2 (0, f);
		} else {
			unsigned long length = strlen (s);
			if (length > 65535) {
				Melder_warning ("Text of ", length, " characters truncated to 65535 characters.");
				length = 65535;
			}
			binputu2 (length, f);
			if (fwrite (s, sizeof (char), length, f) != length)
				Melder_throw ("Error in file while trying to write ", length, " one-byte characters.");
		}
	} catch (MelderError) {
		Melder_throw ("Text not written to a binary file.");
	}
}

void binputs4 (const char *s, FILE *f) {
	try {
		if (s == NULL) {
			binputu4 (0, f);
		} else {
			unsigned long length = strlen (s);
			binputu4 (length, f);
			if (fwrite (s, sizeof (char), length, f) != length)
				Melder_throw ("Error in file while trying to write ", length, " one-byte characters.");
		}
	} catch (MelderError) {
		Melder_throw ("Text not written to a binary file.");
	}
}

static inline void binpututf16 (wchar_t character, FILE *f) {
	if (sizeof (wchar_t) == 2) {   // wchar_t is UTF-16?
		binputu2 (character, f);
	} else {   // wchar_t is UTF-32.
		MelderUtf32 kar = character;
		if (kar <= 0xFFFF) {
			binputu2 (character, f);
		} else if (kar <= 0x10FFFF) {
			kar -= 0x10000;
			binputu2 (0xD800 | (kar >> 10), f);
			binputu2 (0xDC00 | (kar & 0x3FF), f);
		} else {
			Melder_fatal ("Impossible Unicode value.");
		}
	}
}

void binputw1 (const wchar_t *s, FILE *f) {
	try {
		if (s == NULL) {
			binputu1 (0, f);
		} else {
			unsigned long length = wcslen (s);
			if (length > 254) {
				Melder_warning ("Text of ", length, " characters truncated to 254 characters.");
				length = 254;
			}
			if (Melder_isValidAscii (s)) {
				/*
				 * ASCII
				 */
				binputu1 (length, f);
				for (unsigned long i = 0; i < length; i ++) {
					binputu1 (s [i], f);
				}
			} else {
				/*
				 * UTF-16
				 */
				binputu1 (0xFF, f);   // an escape for multibyte encoding
				binputu1 (length, f);
				for (unsigned long i = 0; i < length; i ++) {
					binpututf16 (s [i], f);
				}
			}
		}
	} catch (MelderError) {
		Melder_throw ("Text not written to a binary file.");
	}
}

void binputw2 (const wchar_t *s, FILE *f) {
	try {
		if (s == NULL) {
			binputu2 (0, f);
		} else {
			unsigned long length = wcslen (s);
			if (length > 65534) {
				Melder_warning ("Text of ", length, " characters truncated to 65534 characters.");
				length = 65534;
			}
			if (Melder_isValidAscii (s)) {
				/*
				 * ASCII
				 */
				binputu2 (length, f);
				for (unsigned long i = 0; i < length; i ++) {
					binputu1 (s [i], f);
				}
			} else {
				/*
				 * UTF-16
				 */
				binputu2 (0xFFFF, f);   // an escape for multibyte encoding
				binputu2 (length, f);
				for (unsigned long i = 0; i < length; i ++) {
					binpututf16 (s [i], f);
				}
			}
		}
	} catch (MelderError) {
		Melder_throw ("Text not written to a binary file.");
	}
}

void binputw4 (const wchar_t *s, FILE *f) {
	try {
		if (s == NULL) {
			binputu4 (0, f);
		} else {
			unsigned long length = wcslen (s);
			if (Melder_isValidAscii (s)) {
				/*
				 * ASCII
				 */
				binputu4 (length, f);
				for (unsigned long i = 0; i < length; i ++) {
					binputu1 (s [i], f);
				}
			} else {
				/*
				 * UTF-16
				 */
				binputu4 (0xFFFFFFFF, f);   // an escape for multibyte encoding
				binputu4 (length, f);
				for (unsigned long i = 0; i < length; i ++) {
					binpututf16 (s [i], f);
				}
			}
		}
	} catch (MelderError) {
		Melder_throw ("Text not written to a binary file.");
	}
}

/********** machine-independent cache I/O **********/

#define my  me ->

#define START(x)  char *ptr = (char *) & (x);
#define READ  * ptr ++ = * f -> ptr ++;
#define WRITE  * f -> ptr ++ = * ptr ++;

CACHE * memopen (size_t nbytes) {
	CACHE *me;
	if (nbytes < 1) return NULL;
	me = Melder_malloc (CACHE, 1);
	my base = Melder_malloc (unsigned char, nbytes);
	my max = my base + nbytes;
	my ptr = my base;
	return me;
}

int memclose (CACHE *me) {
	if (! me || ! my base) return EOF;
	Melder_free (my base);
	Melder_free (me);
	return 0;
}

size_t memread (void *ptr, size_t size, size_t nmemb, CACHE *me) {
	size_t nbytes = size * nmemb;
	memcpy (ptr, my ptr, nbytes);
	my ptr += nbytes;
	return nmemb;
}

size_t memwrite (const void *ptr, size_t size, size_t nmemb, CACHE *me) {
	size_t nbytes = size * nmemb;
	Melder_assert (my ptr + nbytes <= my max);
	memcpy (my ptr, ptr, nbytes);
	my ptr += nbytes;
	return nmemb;
}

void memprint1 (CACHE *me, const char *s1) {
	(void) memwrite (s1, 1, strlen (s1), me);
}
void memprint2 (CACHE *me, const char *s1, const char *s2) {
	(void) memwrite (s1, 1, strlen (s1), me);
	(void) memwrite (s2, 1, strlen (s2), me);
}
void memprint3 (CACHE *me, const char *s1, const char *s2, const char *s3) {
	(void) memwrite (s1, 1, strlen (s1), me);
	(void) memwrite (s2, 1, strlen (s2), me);
	(void) memwrite (s3, 1, strlen (s3), me);
}
void memprint4 (CACHE *me, const char *s1, const char *s2, const char *s3, const char *s4) {
	(void) memwrite (s1, 1, strlen (s1), me);
	(void) memwrite (s2, 1, strlen (s2), me);
	(void) memwrite (s3, 1, strlen (s3), me);
	(void) memwrite (s4, 1, strlen (s4), me);
}
void memprint5 (CACHE *me, const char *s1, const char *s2, const char *s3, const char *s4, const char *s5) {
	(void) memwrite (s1, 1, strlen (s1), me);
	(void) memwrite (s2, 1, strlen (s2), me);
	(void) memwrite (s3, 1, strlen (s3), me);
	(void) memwrite (s4, 1, strlen (s4), me);
	(void) memwrite (s5, 1, strlen (s5), me);
}

unsigned int cacgetu1 (CACHE *me) { return * (unsigned char *) my ptr ++; }
void cacputu1 (unsigned int u, CACHE *me) { * (unsigned char *) my ptr ++ = u; }
int cacgeti1 (CACHE *me) { return * (signed char *) my ptr ++; }
void cacputi1 (int u, CACHE *me) { * (signed char *) my ptr ++ = u; }
int cacgete1 (CACHE *me, const wchar_t *type) {
	int result = * (signed char *) my ptr ++;
	if (result < 0)
		Melder_throw ("(cacgete1:) ", result, " is not a value of enumerated type \"", type, "\".");
	return result;
}
void cacpute1 (int value, CACHE *me) {
	* (signed char *) my ptr ++ = value;
}

int memseek (CACHE *me, long offset, int whence) {
	my ptr = whence == 0 ? my base + offset : my ptr + offset;
	return 0;
}

long memtell (CACHE *me) { return my ptr - my base; }

void memrewind (CACHE *me) { my ptr = my base; }

#define macro_cacgetb(nbits) \
unsigned int cacgetb##nbits (CACHE *f) { \
	unsigned char result; \
	if (bitsInReadBuffer < nbits) { readBuffer = * f -> ptr ++; bitsInReadBuffer = 8; } \
	result = readBuffer << (8 - bitsInReadBuffer); \
	bitsInReadBuffer -= nbits; \
	return result >> (8 - nbits); \
}
macro_cacgetb (1)
macro_cacgetb (2)
macro_cacgetb (3)
macro_cacgetb (4)
macro_cacgetb (5)
macro_cacgetb (6)
macro_cacgetb (7)
void cacgetb (CACHE *f) { (void) f; bitsInReadBuffer = 0; }

int cacgeti2 (CACHE *f) {
	if (binario_shortBE2) {
		short s;
		START (s) READ READ
		return s;   /* With sign extension if an int is 4 bytes. */
	} else {
		unsigned char bytes [2];
		START (bytes) READ READ
		return (signed short) (((unsigned short) bytes [0] << 8) | (unsigned short) bytes [1]);
	}
}

unsigned int cacgetu2 (CACHE *f) {
	if (binario_shortBE2) {
		unsigned short s;
		START (s) READ READ
		return s;   /* Without sign extension. */
	} else {
		unsigned char bytes [2];
		START (bytes) READ READ
		return ((unsigned short) bytes [0] << 8) | (unsigned short) bytes [1];
	}
}

int cacgete2 (CACHE *f, const wchar_t *type) {
	signed short s;
	if (binario_shortBE2) {
		START (s) READ READ
	} else {
		unsigned char bytes [2];
		START (bytes) READ READ
		s = ((unsigned short) bytes [0] << 8) | (unsigned short) bytes [1];
	}
	if (s < 0)
		Melder_throw ("(cacgete2:) ", s, " is not a value of enumerated type \"", type, "\".");
	return s;
}

long cacgeti4 (CACHE *f) {
	if (binario_longBE4) {
		long l;
		START (l) READ READ READ READ
		return l;
	} else {
		unsigned char bytes [4];
		START (bytes) READ READ READ READ
		return
			((unsigned long) bytes [0] << 24) |
			((unsigned long) bytes [1] << 16) |
			((unsigned long) bytes [2] << 8) |
			(unsigned long) bytes [3];
	}
}

unsigned long cacgetu4 (CACHE *f) {
	if (binario_longBE4) {
		unsigned long l;
		START (l) READ READ READ READ
		return l;
	} else {
		unsigned char bytes [4];
		START (bytes) READ READ READ READ
		return
			((unsigned long) bytes [0] << 24) |
			((unsigned long) bytes [1] << 16) |
			((unsigned long) bytes [2] << 8) |
			(unsigned long) bytes [3];
	}
}

int cacgeti2LE (CACHE *f) {
	unsigned char bytes [2];
	START (bytes) READ READ
	return (signed short) (((unsigned short) bytes [1] << 8) | (unsigned short) bytes [0]);
}

unsigned int cacgetu2LE (CACHE *f) {
	unsigned char bytes [2];
	START (bytes) READ READ
	return ((unsigned short) bytes [1] << 8) | (unsigned short) bytes [0];
}

long cacgeti4LE (CACHE *f) {
	unsigned char bytes [4];
	START (bytes) READ READ READ READ
	return
		((unsigned long) bytes [3] << 24) | ((unsigned long) bytes [2] << 16) |
		((unsigned long) bytes [1] << 8) | (unsigned long) bytes [0];
}

unsigned long cacgetu4LE (CACHE *f) {
	unsigned char bytes [4];
	START (bytes) READ READ READ READ
	return
		((unsigned long) bytes [3] << 24) | ((unsigned long) bytes [2] << 16) |
		((unsigned long) bytes [1] << 8) | (unsigned long) bytes [0];
}

double cacgetr4 (CACHE *f) {
	if (binario_floatIEEE4msb) {
		float x;
		START (x) READ READ READ READ
		return x;
	} else {
		unsigned char bytes [4];
		double x;
		long exponent;
		unsigned long mantissa;
		START (bytes) READ READ READ READ
		exponent = ((unsigned long) (bytes [0] & 0x7F) << 1) |
			((unsigned long) (bytes [1] & 0x80) >> 7);
		mantissa = ((unsigned long) (bytes [1] & 0x7F) << 16) |
			((unsigned long) bytes [2] << 8) | (unsigned long) bytes [3];
		if (exponent == 0)
			if (mantissa == 0) x = 0.0;
			else x = ldexp (UnsignedToFloat (mantissa), exponent - 149);   /* Denormalized. */
		else if (exponent == 0x00FF)   /* Infinity or Not-a-Number. */
			x = HUGE_VAL;
		else   /* Finite. */
			x  = ldexp (UnsignedToFloat (mantissa | 0x00800000), exponent - 150);
		return bytes [0] & 0x80 ? - x : x;
	}
}

double cacgetr8 (CACHE *f) {
	if (binario_doubleIEEE8msb) {
		double x;
		START (x) READ READ READ READ READ READ READ READ
		return x;
	} else {
		unsigned char bytes [8];
		double x;
		long exponent;
		unsigned long highMantissa, lowMantissa;
		START (bytes) READ READ READ READ READ READ READ READ
		exponent = ((unsigned long) (bytes [0] & 0x7F) << 4) |
			((unsigned long) (bytes [1] & 0xF0) >> 4);
		highMantissa = ((unsigned long) (bytes [1] & 0x0F) << 16) |
			((unsigned long) bytes [2] << 8) | (unsigned long) bytes [3];
		lowMantissa = ((unsigned long) bytes [4] << 24) | ((unsigned long) bytes [5] << 16) |
			((unsigned long) bytes [6] << 8) | (unsigned long) bytes [7];
		if (exponent == 0)
			if (highMantissa == 0 && lowMantissa == 0) x = 0.0;
			else x = ldexp (UnsignedToFloat (highMantissa), exponent - 1042) +
				ldexp (UnsignedToFloat (lowMantissa), exponent - 1074);   /* Denormalized. */
		else if (exponent == 0x07FF)   /* Infinity of Not-a-Number. */
			x = HUGE_VAL;
		else
			x = ldexp (UnsignedToFloat (highMantissa | 0x00100000), exponent - 1043) +
				ldexp (UnsignedToFloat (lowMantissa), exponent - 1075);
		return bytes [0] & 0x80 ? - x : x;
	}
}

double cacgetr10 (CACHE *f) {
	unsigned char bytes [10];
	double x;
	long exponent;
	unsigned long highMantissa, lowMantissa;
	START (bytes) READ READ READ READ READ READ READ READ READ READ
	exponent = ((unsigned long) (bytes [0] & 0x7F) << 8) | bytes [1];
	highMantissa = ((unsigned long) bytes [2] << 24) | ((unsigned long) bytes [3] << 16) |
		((unsigned long) bytes [4] << 8) | (unsigned long) bytes [5];
	lowMantissa = ((unsigned long) bytes [6] << 24) | ((unsigned long) bytes [7] << 16) |
		((unsigned long) bytes [8] << 8) | (unsigned long) bytes [9];
	if (exponent == 0 && highMantissa == 0 && lowMantissa == 0) x = 0;
	else if (exponent == 0x7FFF) x = HUGE_VAL;   /* Infinity or NaN */
	else {
		exponent -= 16383;
		x = ldexp (UnsignedToFloat (highMantissa), exponent - 31);
		x += ldexp (UnsignedToFloat (lowMantissa), exponent - 63);
	}
	return bytes [0] & 0x80 ? - x : x;
}

#define macro_cacputb(nbits) \
void cacputb##nbits (unsigned int value, CACHE *f) { \
	if (bitsInWriteBuffer + nbits > 8) { * f -> ptr ++ = writeBuffer; bitsInWriteBuffer = 0; writeBuffer = 0; } \
	writeBuffer |= (value << (8 - nbits)) >> bitsInWriteBuffer; \
	bitsInWriteBuffer += nbits; \
}
macro_cacputb (1)
macro_cacputb (2)
macro_cacputb (3)
macro_cacputb (4)
macro_cacputb (5)
macro_cacputb (6)
macro_cacputb (7)
void cacputb (CACHE *f) {
	if (bitsInWriteBuffer == 0) return;
	cacputu1 (writeBuffer, f);   /* Flush. */
	bitsInWriteBuffer = 0;
	writeBuffer = 0;
}

void cacputi2 (int i, CACHE *f) {
	if (binario_shortBE2) {
		short s = i;
		START (s) WRITE WRITE
	} else {
		char bytes [2];
		bytes [0] = i >> 8;
		bytes [1] = i;
		{ START (bytes) WRITE WRITE }
	}
}

void cacputu2 (unsigned int u, CACHE *f) {
	if (binario_shortBE2) {
		unsigned short s = u;
		START (s) WRITE WRITE
	} else {
		char bytes [2];
		bytes [0] = u >> 8;
		bytes [1] = u;
		{ START (bytes) WRITE WRITE }
	}
}

void cacpute2 (int value, CACHE *f) {
	if (binario_shortBE2) {
		signed short s = value;
		START (s) WRITE WRITE
	} else {
		char bytes [2];
		bytes [0] = value >> 8;
		bytes [1] = value;
		{ START (bytes) WRITE WRITE }
	}
}

void cacputi4 (long i, CACHE *f) {
	if (binario_longBE4) {
		START (i) WRITE WRITE WRITE WRITE
	} else {
		char bytes [4];
		bytes [0] = i >> 24;
		bytes [1] = i >> 16;
		bytes [2] = i >> 8;
		bytes [3] = i;
		{ START (bytes) WRITE WRITE WRITE WRITE }
	}
}

void cacputu4 (unsigned long u, CACHE *f) {
	if (binario_longBE4) {
		START (u) WRITE WRITE WRITE WRITE
	} else {
		char bytes [4];
		bytes [0] = u >> 24;
		bytes [1] = u >> 16;
		bytes [2] = u >> 8;
		bytes [3] = u;
		{ START (bytes) WRITE WRITE WRITE WRITE }
	}
}

void cacputi2LE (int i, CACHE *f) {
	char bytes [2];
	bytes [1] = i >> 8;
	bytes [0] = i;
	{ START (bytes) WRITE WRITE }
}

void cacputu2LE (unsigned int u, CACHE *f) {
	char bytes [2];
	bytes [1] = u >> 8;
	bytes [0] = u;
	{ START (bytes) WRITE WRITE }
}

void cacputi4LE (long i, CACHE *f) {
	char bytes [4];
	bytes [3] = i >> 24;
	bytes [2] = i >> 16;
	bytes [1] = i >> 8;
	bytes [0] = i;
	{ START (bytes) WRITE WRITE WRITE WRITE }
}

void cacputu4LE (unsigned long u, CACHE *f) {
	char bytes [4];
	bytes [3] = u >> 24;
	bytes [2] = u >> 16;
	bytes [1] = u >> 8;
	bytes [0] = u;
	{ START (bytes) WRITE WRITE WRITE WRITE }
}

void cacputr4 (double x, CACHE *f) {
	if (binario_floatIEEE4msb) {
		float x4 = x;
		START (x4) WRITE WRITE WRITE WRITE
	} else {
		unsigned char bytes [4];
		int sign, exponent;
		double fMantissa, fsMantissa;
		unsigned long mantissa;
		if (x < 0.0) { sign = 0x0100; x *= -1; }
		else sign = 0;
		if (x == 0.0) { exponent = 0; mantissa = 0; }
		else {
			fMantissa = frexp (x, & exponent);
			if ((exponent > 128) || ! (fMantissa < 1))   /* Infinity or Not-a-Number. */
				{ exponent = sign | 0x00FF; mantissa = 0; }   /* Infinity. */
			else {   /* Finite. */
				exponent += 126;   /* Add bias. */
				if (exponent <= 0) {   /* Denormalized. */
					fMantissa = ldexp (fMantissa, exponent - 1);
					exponent = 0;
				}
				exponent |= sign;
				fMantissa = ldexp (fMantissa, 24);          
				fsMantissa = floor (fMantissa); 
				mantissa = FloatToUnsigned (fsMantissa) & 0x007FFFFF;
			}
		}
		bytes [0] = exponent >> 1;
		bytes [1] = (exponent << 7) | (mantissa >> 16);
		bytes [2] = mantissa >> 8;
		bytes [3] = mantissa;
		{ START (bytes) WRITE WRITE WRITE WRITE }
	}
}

void cacputr8 (double x, CACHE *f) {
	if (binario_doubleIEEE8msb) {
		START (x) WRITE WRITE WRITE WRITE WRITE WRITE WRITE WRITE
	} else {
		unsigned char bytes [8];
		int sign, exponent;
		double fMantissa, fsMantissa;
		unsigned long highMantissa, lowMantissa;
		if (x < 0.0) { sign = 0x0800; x *= -1; }
		else sign = 0;
		if (x == 0.0) { exponent = 0; highMantissa = 0; lowMantissa = 0; }
		else {
			fMantissa = frexp (x, & exponent);
			if ((exponent > 1024) || ! (fMantissa < 1))   /* Infinity or Not-a-Number. */
				{ exponent = sign | 0x07FF; highMantissa = 0; lowMantissa = 0; }   /* Infinity. */
			else { /* Finite. */
				exponent += 1022;   /* Add bias. */
				if (exponent <= 0) {   /* Denormalized. */
					fMantissa = ldexp (fMantissa, exponent - 1);
					exponent = 0;
				}
				exponent |= sign;
				fMantissa = ldexp (fMantissa, 21);          
				fsMantissa = floor (fMantissa); 
				highMantissa = FloatToUnsigned (fsMantissa) & 0x000FFFFF;
				fMantissa = ldexp (fMantissa - fsMantissa, 32); 
				fsMantissa = floor (fMantissa); 
				lowMantissa = FloatToUnsigned (fsMantissa);
			}
		}
		bytes [0] = exponent >> 4;
		bytes [1] = (exponent << 4) | (highMantissa >> 16);
		bytes [2] = highMantissa >> 8;
		bytes [3] = highMantissa;
		bytes [4] = lowMantissa >> 24;
		bytes [5] = lowMantissa >> 16;
		bytes [6] = lowMantissa >> 8;
		bytes [7] = lowMantissa;
		{ START (bytes) WRITE WRITE WRITE WRITE WRITE WRITE WRITE WRITE }
	}
}

void cacputr10 (double x, CACHE *f) {
	unsigned char bytes [10];
	int sign, exponent;
	double fMantissa, fsMantissa;
	unsigned long highMantissa, lowMantissa;
	if (x < 0.0) { sign = 0x8000; x *= -1; }
	else sign = 0;
	if (x == 0.0) { exponent = 0; highMantissa = 0; lowMantissa = 0; }
	else {
		fMantissa = frexp (x, & exponent);
		if ((exponent > 16384) || ! (fMantissa < 1))   /* Infinity or Not-a-Number. */
			{ exponent = sign | 0x7FFF; highMantissa = 0; lowMantissa = 0; }   /* Infinity. */
		else {   /* Finite */
			exponent += 16382;   /* Add bias. */
			if (exponent < 0) {   /* Denormalized. */
				fMantissa = ldexp (fMantissa, exponent);
				exponent = 0;
			}
			exponent |= sign;
			fMantissa = ldexp (fMantissa, 32);          
			fsMantissa = floor (fMantissa); 
			highMantissa = FloatToUnsigned (fsMantissa);
			fMantissa = ldexp (fMantissa - fsMantissa, 32); 
			fsMantissa = floor (fMantissa); 
			lowMantissa = FloatToUnsigned (fsMantissa);
		}
	}
	bytes [0] = exponent >> 8;
	bytes [1] = exponent;
	bytes [2] = highMantissa >> 24;
	bytes [3] = highMantissa >> 16;
	bytes [4] = highMantissa >> 8;
	bytes [5] = highMantissa;
	bytes [6] = lowMantissa >> 24;
	bytes [7] = lowMantissa >> 16;
	bytes [8] = lowMantissa >> 8;
	bytes [9] = lowMantissa;
	{ START (bytes) WRITE WRITE WRITE WRITE WRITE WRITE WRITE WRITE WRITE WRITE }
}

fcomplex cacgetc8 (CACHE *f) {
	fcomplex result;
	result. re = cacgetr4 (f);
	result. im = cacgetr4 (f);
	return result;
}

dcomplex cacgetc16 (CACHE *f) {
	dcomplex result;
	result. re = cacgetr8 (f);
	result. im = cacgetr8 (f);
	return result;
}

void cacputc8 (fcomplex z, CACHE *f) {
	cacputr4 (z. re, f);
	cacputr4 (z. im, f);
}

void cacputc16 (dcomplex z, CACHE *f) {
	cacputr8 (z. re, f);
	cacputr8 (z. im, f);
}

char * cacgets1 (CACHE *f) {
	try {
		unsigned int length = (unsigned char) * f -> ptr ++;
		autostring8 result = Melder_malloc (char, length + 1);
		memread (result.peek(), 1, length, f);
		result [length] = 0;   // trailing null byte
		return result.transfer();
	} catch (MelderError) {
		Melder_throw ("Cannot read string from cache.");
	}
}

char * cacgets2 (CACHE *f) {
	try {
		unsigned int length = cacgetu2 (f);
		autostring8 result = Melder_malloc (char, length + 1);
		memread (result.peek(), 1, length, f);
		result [length] = 0;   // trailing null byte
		return result.transfer();
	} catch (MelderError) {
		Melder_throw ("Cannot read string from cache.");
	}
}

char * cacgets4 (CACHE *f) {
	try {
		unsigned long length = cacgetu4 (f);
		autostring8 result = Melder_malloc (char, length + 1);
		memread (result.peek(), 1, length, f);
		result [length] = 0;   // trailing null byte
		return result.transfer();
	} catch (MelderError) {
		Melder_throw ("Cannot read string from cache.");
	}
}

void cacputs1 (const char *s, CACHE *f) {
	unsigned int length = s ? strlen (s) : 0; if (length > 255) length = 255;
	* f -> ptr ++ = length; if (s) memwrite (s, 1, length, f);
}

void cacputs2 (const char *s, CACHE *f) {
	unsigned int length = s ? strlen (s) : 0; if (length > 65535) length = 65535;
	cacputu2 (length, f); if (s) memwrite (s, 1, length, f);
}

void cacputs4 (const char *s, CACHE *f) {
	unsigned long length = s ? strlen (s) : 0;
	cacputu4 (length, f); if (s) memwrite (s, 1, length, f);
}

/* End of file abcio.cpp */
