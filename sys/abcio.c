/* abcio.c
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
 */

#include "melder.h"
#include <math.h>
#include <ctype.h>
#ifdef macintosh
	#include <TargetConditionals.h>
#endif
#include "enum.h"
#include "abcio.h"

/********** ASCII I/O **********/

static long getInteger (FILE *f) {
	int c, i;
	char buffer [41];
	for (c = fgetc (f); c != '-' && ! isdigit (c) && c != '+'; c = fgetc (f)) {
		if (feof (f)) {
			(void) Melder_error ("(ascio/getInteger:) Early end of file detected.");
			return 0;
		}
		if (c == '!') {   /* End-of-line comment? */
			while ((c = fgetc (f)) != '\n' && c != '\r') if (feof (f)) {
				(void) Melder_error ("(ascio/getInteger:) Early end of file detected in comment.");
				return 0;
			}
		}
		if (c == '\"') {
			(void) Melder_error ("(ascio/getInteger:) Found a string while searching for an integer.");
			return 0;
		}
		if (c == '<') {
			(void) Melder_error ("(ascio/getInteger:) Found an enumerated value while searching for an integer.");
			return 0;
		}
		while (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
			if (feof (f)) {
				(void) Melder_error ("(ascio/getInteger:) Early end of file detected in comment.");
				return 0;
			}
			c = fgetc (f);
		}
	}
	for (i = 0; i < 40; i ++) {
		buffer [i] = c;
		c = fgetc (f);
		if (feof (f)) { clearerr (f); break; }   /* This may well be OK here. */
		if (c == ' ' || c == '\n' || c == '\t' || c == '\r') break;
	}
	if (i >= 40) {
		(void) Melder_error ("(ascio/getInteger:) Found strange text while searching for an integer.");
		return 0;
	}
	buffer [i + 1] = '\0';
	return atol (buffer);
}

static unsigned long getUnsigned (FILE *f) {
	unsigned long result;
	int c, i;
	char buffer [41];
	for (c = fgetc (f); ! isdigit (c) && c != '+'; c = fgetc (f)) {
		if (feof (f)) {
			(void) Melder_error ("(ascio/getUnsigned:) Early end of file detected.");
			return 0;
		}
		if (c == '!') {   /* End-of-line comment? */
			while ((c = fgetc (f)) != '\n' && c != '\r') if (feof (f)) {
				(void) Melder_error ("(ascio/getUnsigned:) Early end of file detected in comment.");
				return 0;
			}
		}
		if (c == '\"') {
			(void) Melder_error ("(ascio/getUnsigned:) Found a string while searching for an unsigned integer.");
			return 0;
		}
		if (c == '<') {
			(void) Melder_error ("(ascio/getUnsigned:) Found an enumerated value while searching for an unsigned integer.");
			return 0;
		}
		if (c == '-') {
			(void) Melder_error ("(ascio/getUnsigned:) Found a negative value while searching for an unsigned integer.");
			return 0;
		}
		while (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
			if (feof (f)) {
				(void) Melder_error ("(ascio/getUnsigned:) Early end of file detected in comment.");
				return 0;
			}
			c = fgetc (f);
		}
	}
	for (i = 0; i < 40; i ++) {
		buffer [i] = c;
		c = fgetc (f);
		if (feof (f)) { clearerr (f); break; }   /* This may well be OK here. */
		if (c == ' ' || c == '\n' || c == '\t' || c == '\r') break;
	}
	if (i >= 40) {
		(void) Melder_error ("(ascio/getUnsigned:) Found strange text while searching for an unsigned integer.");
		return 0;
	}
	buffer [i + 1] = '\0';
	sscanf (buffer, "%lu", & result);
	return result;
}

static double getReal (FILE *f) {
	int c, i;
	char buffer [41], *slash;
	do {
		for (c = fgetc (f); c != '-' && ! isdigit (c) && c != '+'; c = fgetc (f)) {
			if (feof (f)) {
				(void) Melder_error ("(ascio/getReal:) Early end of file detected.");
				return 0;
			}
			if (c == '!') {   /* End-of-line comment? */
				while ((c = fgetc (f)) != '\n' && c != '\r') if (feof (f)) {
					(void) Melder_error ("(ascio/getReal:) Early end of file detected in comment.");
					return 0;
				}
			}
			if (c == '\"') {
				(void) Melder_error ("(ascio/getReal:) Found a string while searching for a real number.");
				return 0;
			}
			if (c == '<') {
				(void) Melder_error ("(ascio/getReal:) Found an enumerated value while searching for a real number.");
				return 0;
			}
			while (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
				if (feof (f)) {
					(void) Melder_error ("(ascio/getReal:) Early end of file detected in comment.");
					return 0;
				}
				c = fgetc (f);
			}
		}
		for (i = 0; i < 40; i ++) {
			buffer [i] = c;
			c = fgetc (f);
			if (feof (f)) { clearerr (f); break; }   /* This may well be OK here. */
			if (c == ' ' || c == '\n' || c == '\t' || c == '\r') break;
		}
		if (i >= 40) {
			(void) Melder_error ("(ascio/getReal:) Found strange text while searching for a real number.");
			return 0;
		}
	} while (i == 0 && buffer [0] == '+');   /* Guard against single '+' symbols, which occur in complex numbers. */
	buffer [i + 1] = '\0';
	slash = strchr (buffer, '/');
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

static short getEnum (FILE *f, void *enumerated) {
	int c, i;
	char buffer [41];
	for (c = fgetc (f); c != '<'; c = fgetc (f)) {
		if (feof (f)) {
			(void) Melder_error ("(ascio/getEnum:) Early end of file detected.");
			return 0;
		}
		if (c == '!') {   /* End-of-line comment? */
			while ((c = fgetc (f)) != '\n' && c != '\r') if (feof (f)) {
				(void) Melder_error ("(ascio/getEnum:) Early end of file detected in comment.");
				return 0;
			}
		}
		if (c == '-' || isdigit (c) || c == '+') {
			(void) Melder_error ("(ascio/getEnum:) Found an integer while searching for an enumerated value.");
			return 0;
		}
		if (c == '\"') {
			(void) Melder_error ("(ascio/getEnum:) Found a string while searching for an enumerated value.");
			return 0;
		}
		while (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
			if (feof (f)) {
				(void) Melder_error ("(ascio/getEnum:) Early end of file detected in comment.");
				return 0;
			}
			c = fgetc (f);
		}
	}
	for (i = 0; i < 40; i ++) {
		c = fgetc (f);   /* Read past first '<'. */
		if (feof (f)) {
			(void) Melder_error ("(ascio/getEnum:) Early end of file detected.");
			return 0;
		}
		if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
			(void) Melder_error ("(ascio/getEnum:) No matching '>'.");
			return 0;
		}
		if (c == '>')
			break;   /* The expected closing bracket; not added to the buffer. */
		buffer [i] = c;
	}
	if (i >= 40) {
		(void) Melder_error ("(ascio/getEnum:) Found strange text while searching for an enumerated value.");
		return 0;
	}
	buffer [i] = '\0';
	return enum_search (enumerated, buffer);
}

static char * getString (FILE *f) {
	int c, i;
	static char *buffer;
	static long capacity;
	if (! buffer) buffer = Melder_malloc (capacity = 100);
	for (c = fgetc (f); c != '\"'; c = fgetc (f)) {
		if (feof (f))
			return Melder_errorp ("(ascio/getString:) Early end of file detected.");
		if (c == '!') {   /* End-of-line comment? */
			while ((c = fgetc (f)) != '\n' && c != '\r') if (feof (f))
				return Melder_errorp ("(ascio/getString:) Early end of file detected in comment.");
		}
		if (c == '-' || isdigit (c) || c == '+')
			return Melder_errorp ("(ascio/getString:) Found a number while searching for a string.");
		if (c == '<')
			return Melder_errorp ("(ascio/getString:) Found an enumerated value while searching for a string.");
		while (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
			if (feof (f)) {
				(void) Melder_error ("(ascio/getString:) Early end of file detected in comment.");
				return 0;
			}
			c = fgetc (f);
		}
	}
	for (i = 0; 1; i ++) {
		if (i >= capacity && ! (buffer = Melder_realloc (buffer, capacity *= 2)))
			return Melder_errorp ("(ascio/getString:) No memory to extend string buffer by %ld bytes.",
				capacity /= 2);
		c = fgetc (f);   /* Read past first '"'. */
		if (feof (f))
			return Melder_errorp ("(ascio/getString:) Early end of file detected.");
		if (c == '\"') {
			int next = fgetc (f);
			if (feof (f)) { clearerr (f); break; }   /* Closing quote is last character in file: OK. */
			if (next != '\"') {
				if (next == ' ' || next == '\n' || next == '\t' || next == '\r')
					ungetc (next, f);   /* Put it back on the stream. */
				else
					return Melder_errorp ("(ascio/getString:) Character '%c' following quote. "
						"End of string or undoubled quote?", next);
				break;   /* The expected closing double quote; not added to the buffer. */
			} /* Else: add only one of the two quotes to the buffer. */
		}
		buffer [i] = c;
	}
	buffer [i] = '\0';
	return Melder_strdup (buffer);
}

#undef false
#undef true

enum_begin (ascio_Boolean, false)
	enum (true)
enum_end (ascio_Boolean)

enum_begin (ascio_Question, no)
	enum (yes)
enum_end (ascio_Question)

enum_begin (ascio_Existence, absent)
	enum (exists)
enum_end (ascio_Existence)

#include "enum_c.h"

enum_begin (ascio_Boolean, false)
	enum (true)
enum_end (ascio_Boolean)

enum_begin (ascio_Question, no)
	enum (yes)
enum_end (ascio_Question)

enum_begin (ascio_Existence, absent)
	enum (exists)
enum_end (ascio_Existence)

int ascgeti1 (FILE *f) { return getInteger (f); }   /* There should be out-of-bound checks here... */
int ascgeti2 (FILE *f) { return getInteger (f); }
long ascgeti4 (FILE *f) { return getInteger (f); }
unsigned int ascgetu1 (FILE *f) { return getInteger (f); }
unsigned int ascgetu2 (FILE *f) { return getUnsigned (f); }
unsigned long ascgetu4 (FILE *f) { return getUnsigned (f); }
double ascgetr4 (FILE *f) { return getReal (f); }
double ascgetr8 (FILE *f) { return getReal (f); }
double ascgetr10 (FILE *f) { return getReal (f); }
fcomplex ascgetc8 (FILE *f) { fcomplex z; z.re = getReal (f); z.im = getReal (f); return z; }
dcomplex ascgetc16 (FILE *f) { dcomplex z; z.re = getReal (f); z.im = getReal (f); return z; }
char ascgetc1 (FILE *f) { return getInteger (f); }
short ascgete1 (FILE *f, void *enumerated) { return getEnum (f, enumerated); }
short ascgete2 (FILE *f, void *enumerated) { return getEnum (f, enumerated); }
short ascgeteb (FILE *f) { return getEnum (f, & enum_ascio_Boolean); }
short ascgeteq (FILE *f) { return getEnum (f, & enum_ascio_Question); }
short ascgetex (FILE *f) { return getEnum (f, & enum_ascio_Existence); }
char *ascgets1 (FILE *f) { return getString (f); }
char *ascgets2 (FILE *f) { return getString (f); }
char *ascgets4 (FILE *f) { return getString (f); }

static int indent = 0;
void ascindent (void) { indent += 4; }
void ascexdent (void) { indent -= 4; }
void ascresetindent (void) { indent = 0; }

static int verbose = 1;

void ascio_verbose (int v) { verbose = v; }

#define PUTIN  const char *format, ...) { \
	va_list arg; int iindent; va_start (arg, format); fputc ('\n', f); \
	if (verbose) { for (iindent = 1; iindent <= indent; iindent ++) fputc (' ', f); \
	vfprintf (f, format, arg); }
#define PUTOUT  va_end (arg); }

void ascputintro (FILE *f, const char *format, ...) {
	if (verbose) {
		va_list arg; int iindent; va_start (arg, format); fputc ('\n', f);
		for (iindent = 1; iindent <= indent; iindent ++) fputc (' ', f);
		vfprintf (f, format, arg);
		va_end (arg);
	}
	indent += 4;
}

void ascputi1 (int i, FILE *f,
	PUTIN fprintf (f, verbose ? " = %d " : "%d", i); PUTOUT
void ascputi2 (int i, FILE *f,
	PUTIN fprintf (f, verbose ? " = %d " : "%d", i); PUTOUT
void ascputi4 (long i, FILE *f,
	PUTIN fprintf (f, verbose ? " = %ld " : "%ld", i); PUTOUT
void ascputu1 (unsigned int u, FILE *f,
	PUTIN fprintf (f, verbose ? " = %u " : "%u", u); PUTOUT
void ascputu2 (unsigned int u, FILE *f,
	PUTIN fprintf (f, verbose ? " = %u " : "%u", u); PUTOUT
void ascputu4 (unsigned long u, FILE *f,
	PUTIN fprintf (f, verbose ? " = %lu " : "%lu", u); PUTOUT
void ascputr4 (double x, FILE *f,
	PUTIN fprintf (f, verbose ? " = %s " : "%s", Melder_single (x)); PUTOUT
void ascputr8 (double x, FILE *f,
	PUTIN fprintf (f, verbose ? " = %s " : "%s", Melder_double (x)); PUTOUT
void ascputr10 (double x, FILE *f,
	PUTIN fprintf (f, x == HUGE_VAL ? verbose ? " = --undefined-- " : "--undefined--" : verbose ? " = %.20g " : "%.20g", x); PUTOUT
void ascputc8 (fcomplex z, FILE *f,
	PUTIN fprintf (f, verbose ? " = %.8g + %.8g i " : "%.8g %.8g", z.re, z.im); PUTOUT
void ascputc16 (dcomplex z, FILE *f,
	PUTIN fprintf (f, verbose ? " = %.17g + %.17g i " : "%.17g %.17g", z.re, z.im); PUTOUT
void ascputc1 (int i, FILE *f,
	PUTIN fprintf (f, verbose ? " = %d " : "%d", i); PUTOUT
void ascpute1 (int i, FILE *f, void *enumerated,
	PUTIN fprintf (f, verbose ? " = <%s> " : "<%s>", enum_string (enumerated, i)); PUTOUT
void ascpute2 (int i, FILE *f, void *enumerated,
	PUTIN fprintf (f, verbose ? " = <%s> " : "<%s>", enum_string (enumerated, i)); PUTOUT
void ascputeb (int i, FILE *f,
	PUTIN fprintf (f, i ? (verbose ? " = <true> " : "<true>") : (verbose ? " = <false> " : "<false>")); PUTOUT
void ascputeq (int i, FILE *f,
	PUTIN fprintf (f, i ? (verbose ? "? <yes> " : "<yes>") : (verbose ? "? <no> " : "<no>")); PUTOUT
void ascputex (int i, FILE *f,
	PUTIN fprintf (f, i ? (verbose ? "? <exists> " : "<exists>") : (verbose ? "? <absent> " : "<absent>")); PUTOUT
void ascputs1 (const char *s, FILE *f,
	PUTIN fprintf (f, verbose ? " = \"" : "\"");
	      if (s) { char c; while ((c = *s ++) != '\0') { fputc (c, f); if (c == '\"') fputc (c, f); } }
	      fprintf (f, verbose ? "\" " : "\""); PUTOUT
void ascputs2 (const char *s, FILE *f,
	PUTIN fprintf (f, verbose ? " = \"" : "\"");
	      if (s) { char c; while ((c = *s ++) != '\0') { fputc (c, f); if (c == '\"') fputc (c, f); } }
	      fprintf (f, verbose ? "\" " : "\""); PUTOUT
void ascputs4 (const char *s, FILE *f,
	PUTIN fprintf (f, verbose ? " = \"" : "\"");
	      if (s) { char c; while ((c = *s ++) != '\0') { fputc (c, f); if (c == '\"') fputc (c, f); } }
	      fprintf (f, verbose ? "\" " : "\""); PUTOUT

/********** machine-independent binary I/O **********/

/* Optimizations for machines for which some of the formats are native. */

/* On which machines is "short" a two's complement Big-Endian (MSB-first) 2-byte word? */

#if defined (sgi) || defined (macintosh) && TARGET_RT_BIG_ENDIAN == 1
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

#if defined (sgi) || defined (macintosh) && TARGET_RT_BIG_ENDIAN == 1
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

#if defined (sgi) || defined (macintosh) && TARGET_RT_BIG_ENDIAN == 1
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

#if defined (sgi) || defined (macintosh) && TARGET_RT_BIG_ENDIAN == 1
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

unsigned int bingetu1 (FILE *f) { return (unsigned char) getc (f); }
void binputu1 (unsigned int u, FILE *f) { putc (u, f); }
int bingeti1 (FILE *f) { return (signed char) getc (f); }
void binputi1 (int u, FILE *f) { putc (u, f); }

int bingete1 (FILE *f, void *enumerated) {
	int result = (signed char) getc (f);
	if (result < 0)
		(void) Melder_error ("(bingete1:) %d is not a value of enumerated type \"%s\".",
			result, enum_type (enumerated));
	return result;
}
void binpute1 (int value, FILE *f, void *enumerated) { (void) enumerated; putc (value, f); }

static int bitsInReadBuffer = 0;
static unsigned char readBuffer;

#define macro_bingetb(nbits) \
unsigned int bingetb##nbits (FILE *f) { \
	unsigned char result; \
	if (bitsInReadBuffer < nbits) { readBuffer = fgetc (f); bitsInReadBuffer = 8; } \
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
	if (binario_shortBE2 && Melder_debug != 18) {
		short s; fread (& s, 2, 1, f); return s;   /* With sign extension if an int is 4 bytes. */
	} else {
		unsigned char bytes [2]; fread (bytes, 1, 2, f);
		return (signed short) (((unsigned short) bytes [0] << 8) | (unsigned short) bytes [1]);
	}
}

unsigned int bingetu2 (FILE *f) {
	if (binario_shortBE2 && Melder_debug != 18) {
		unsigned short s; fread (& s, 2, 1, f); return s;   /* Without sign extension. */
	} else {
		unsigned char bytes [2]; fread (bytes, 1, 2, f);
		return ((unsigned short) bytes [0] << 8) | (unsigned short) bytes [1];
	}
}

int bingete2 (FILE *f, void *enumerated) {
	signed short result;
	if (binario_shortBE2 && Melder_debug != 18) {
		fread (& result, 2, 1, f);
	} else {
		unsigned char bytes [2]; fread (bytes, 1, 2, f);
		result = ((unsigned short) bytes [0] << 8) | (unsigned short) bytes [1];
	}
	if (result < 0)
		(void) Melder_error ("(bingete2:) %d is not a value of enumerated type \"%s\".",
			result, enum_type (enumerated));
	return result;
}

long bingeti3 (FILE *f) {
	unsigned long result;
	unsigned char bytes [3]; fread (bytes, 1, 3, f);
	result =
		((unsigned long) bytes [0] << 16) |
		((unsigned long) bytes [1] << 8) | (unsigned long) bytes [2];
	if ((bytes [0] & 128) != 0)
		result |= 0xFF000000;
	return result;
}

long bingeti4 (FILE *f) {
	if (binario_longBE4 && Melder_debug != 18) {
		long l; fread (& l, 4, 1, f); return l;
	} else {
		unsigned char bytes [4]; fread (bytes, 1, 4, f); return
			((unsigned long) bytes [0] << 24) | ((unsigned long) bytes [1] << 16) |
			((unsigned long) bytes [2] << 8) | (unsigned long) bytes [3];
	}
}

unsigned long bingetu4 (FILE *f) {
	if (binario_longBE4 && Melder_debug != 18) {
		unsigned long l; fread (& l, 4, 1, f); return l;
	} else {
		unsigned char bytes [4]; fread (bytes, 1, 4, f); return
			((unsigned long) bytes [0] << 24) | ((unsigned long) bytes [1] << 16) |
			((unsigned long) bytes [2] << 8) | (unsigned long) bytes [3];
	}
}

int bingeti2LE (FILE *f) {
	if (binario_shortLE2 && Melder_debug != 18) {
		short s; fread (& s, 2, 1, f); return s;   /* With sign extension if an int is 4 bytes. */
	} else {
		unsigned char bytes [2]; fread (bytes, 1, 2, f);
		return (signed short) (((unsigned short) bytes [1] << 8) | (unsigned short) bytes [0]);
	}
}

unsigned int bingetu2LE (FILE *f) {
	if (binario_shortLE2 && Melder_debug != 18) {
		unsigned short s; fread (& s, 2, 1, f); return s;   /* Without sign extension. */
	} else {
		unsigned char bytes [2]; fread (bytes, 1, 2, f);
		return ((unsigned short) bytes [1] << 8) | (unsigned short) bytes [0];
	}
}

long bingeti3LE (FILE *f) {
	unsigned long result;
	unsigned char bytes [3]; fread (bytes, 1, 3, f);
	result =
		((unsigned long) bytes [2] << 16) |
		((unsigned long) bytes [1] << 8) | (unsigned long) bytes [0];
	if ((bytes [2] & 128) != 0)
		result |= 0xFF000000;
	return result;
}

long bingeti4LE (FILE *f) {
	if (binario_longLE4 && Melder_debug != 18) {
		long l; fread (& l, 4, 1, f); return l;
	} else {
		unsigned char bytes [4]; fread (bytes, 1, 4, f); return
			((unsigned long) bytes [3] << 24) | ((unsigned long) bytes [2] << 16) |
			((unsigned long) bytes [1] << 8) | (unsigned long) bytes [0];
	}
}

unsigned long bingetu4LE (FILE *f) {
	if (binario_longLE4 && Melder_debug != 18) {
		unsigned long l; fread (& l, 4, 1, f); return l;
	} else {
		unsigned char bytes [4]; fread (bytes, 1, 4, f); return
			((unsigned long) bytes [3] << 24) | ((unsigned long) bytes [2] << 16) |
			((unsigned long) bytes [1] << 8) | (unsigned long) bytes [0];
	}
}

double bingetr4 (FILE *f) {
	if (binario_floatIEEE4msb && Melder_debug != 18) {
		float x; fread (& x, 4, 1, f); return x;
	} else {
		unsigned char bytes [4];
		double x;
		long exponent;
		unsigned long mantissa;
		fread (bytes, 1, 4, f);
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

double bingetr8 (FILE *f) {
	if (binario_doubleIEEE8msb && Melder_debug != 18) {
		double x; fread (& x, 8, 1, f); return x;
	} else {
		unsigned char bytes [8];
		double x;
		long exponent;
		unsigned long highMantissa, lowMantissa;
		fread (bytes, 1, 8, f);
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
		else if (exponent == 0x07FF)   /* Infinity or Not-a-Number. */
			x = HUGE_VAL;
		else
			x = ldexp (UnsignedToFloat (highMantissa | 0x00100000), exponent - 1043) +
				ldexp (UnsignedToFloat (lowMantissa), exponent - 1075);
		return bytes [0] & 0x80 ? - x : x;
	}
}

double bingetr10 (FILE *f) {
	unsigned char bytes [10];
	double x;
	long exponent;
	unsigned long highMantissa, lowMantissa;
	fread (bytes, 1, 10, f);
	exponent = ((bytes [0] & 0x7F) << 8) | bytes [1];
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

static int bitsInWriteBuffer = 0;
static unsigned char writeBuffer = 0;

#define macro_binputb(nbits) \
void binputb##nbits (unsigned int value, FILE *f) { \
	if (bitsInWriteBuffer + nbits > 8) { fputc (writeBuffer, f); bitsInWriteBuffer = 0; writeBuffer = 0; } \
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
	fputc (writeBuffer, f);   /* Flush. */
	bitsInWriteBuffer = 0;
	writeBuffer = 0;
}

void binputi2 (int i, FILE *f) {
	if (binario_shortBE2 && Melder_debug != 18) {
		short s = i; fwrite (& s, 2, 1, f);
	} else {
		char bytes [2];
		bytes [0] = i >> 8;
		bytes [1] = i;
		fwrite (bytes, 1, 2, f);
	}
}

void binputu2 (unsigned int u, FILE *f) {
	if (binario_shortBE2 && Melder_debug != 18) {
		unsigned short s = u; fwrite (& s, 2, 1, f);
	} else {
		char bytes [2];
		bytes [0] = u >> 8;
		bytes [1] = u;
		fwrite (bytes, 1, 2, f);
	}
}

void binpute2 (int value, FILE *f, void *enumerated) {
	(void) enumerated;
	if (binario_shortBE2 && Melder_debug != 18) {
		short s = value; fwrite (& s, 2, 1, f);
	} else {
		char bytes [2];
		bytes [0] = value >> 8;
		bytes [1] = value;
		fwrite (bytes, 1, 2, f);
	}
}

void binputi3 (long i, FILE *f) {
	char bytes [3];
	bytes [0] = i >> 16;
	bytes [1] = i >> 8;
	bytes [2] = i;
	fwrite (bytes, 1, 3, f);
}

void binputi4 (long i, FILE *f) {
	if (binario_longBE4 && Melder_debug != 18) {
		fwrite (& i, 4, 1, f);
	} else {
		char bytes [4];
		bytes [0] = i >> 24;
		bytes [1] = i >> 16;
		bytes [2] = i >> 8;
		bytes [3] = i;
		fwrite (bytes, 1, 4, f);
	}
}

void binputu4 (unsigned long u, FILE *f) {
	if (binario_longBE4 && Melder_debug != 18) {
		fwrite (& u, 4, 1, f);
	} else {
		char bytes [4];
		bytes [0] = u >> 24;
		bytes [1] = u >> 16;
		bytes [2] = u >> 8;
		bytes [3] = u;
		fwrite (bytes, 1, 4, f);
	}
}

void binputi2LE (int i, FILE *f) {
	if (binario_shortLE2 && Melder_debug != 18) {
		short s = i; fwrite (& s, 2, 1, f);
	} else {
		char bytes [2];
		bytes [1] = i >> 8;
		bytes [0] = i;
		fwrite (bytes, 1, 2, f);
	}
}

void binputu2LE (unsigned int u, FILE *f) {
	if (binario_shortLE2 && Melder_debug != 18) {
		unsigned short s = u; fwrite (& s, 2, 1, f);
	} else {
		char bytes [2];
		bytes [1] = u >> 8;
		bytes [0] = u;
		fwrite (bytes, 1, 2, f);
	}
}

void binputi3LE (long i, FILE *f) {
	char bytes [3];
	bytes [2] = i >> 16;
	bytes [1] = i >> 8;
	bytes [0] = i;
	fwrite (bytes, 1, 3, f);
}

void binputi4LE (long i, FILE *f) {
	if (binario_longLE4 && Melder_debug != 18) {
		fwrite (& i, 4, 1, f);
	} else {
		char bytes [4];
		bytes [3] = i >> 24;
		bytes [2] = i >> 16;
		bytes [1] = i >> 8;
		bytes [0] = i;
		fwrite (bytes, 1, 4, f);
	}
}

void binputu4LE (unsigned long u, FILE *f) {
	if (binario_longLE4 && Melder_debug != 18) {
		fwrite (& u, 4, 1, f);
	} else {
		char bytes [4];
		bytes [3] = u >> 24;
		bytes [2] = u >> 16;
		bytes [1] = u >> 8;
		bytes [0] = u;
		fwrite (bytes, 1, 4, f);
	}
}

void binputr4 (double x, FILE *f) {
	if (binario_floatIEEE4msb && Melder_debug != 18) {
		float x4 = x; fwrite (& x4, 4, 1, f);
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
		fwrite (bytes, 1, 4, f);
	}
}

void binputr8 (double x, FILE *f) {
	if (binario_doubleIEEE8msb && Melder_debug != 18) {
		fwrite (& x, 8, 1, f);
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
		fwrite (bytes, 1, 8, f);
	}
}

void binputr10 (double x, FILE *f) {
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
	fwrite (bytes, 1, 10, f);
}

fcomplex bingetc8 (FILE *f) {
	fcomplex result;
	result. re = bingetr4 (f);
	result. im = bingetr4 (f);
	return result;
}

dcomplex bingetc16 (FILE *f) {
	dcomplex result;
	result. re = bingetr8 (f);
	result. im = bingetr8 (f);
	return result;
}

void binputc8 (fcomplex z, FILE *f) {
	binputr4 (z. re, f);
	binputr4 (z. im, f);
}

void binputc16 (dcomplex z, FILE *f) {
	binputr8 (z. re, f);
	binputr8 (z. im, f);
}

char * bingets1 (FILE *f) {
	unsigned int length = bingetu1 (f);
	char *result = Melder_malloc (length + 1);
	if (! result)
		return Melder_errorp ("(bingets1:) Out of memory. Cannot create string of length %d.", length);
	if (fread (result, 1, length, f) != length) { Melder_free (result); return NULL; }
	result [length] = 0;   /* Trailing null byte. */
	return result;
}

char * bingets2 (FILE *f) {
	unsigned int length = bingetu2 (f);
	char *result = Melder_malloc (length + 1);
	if (! result)
		return Melder_errorp ("(bingets2:) Out of memory. Cannot create string of length %d.", length);
	if (fread (result, 1, length, f) != length) { Melder_free (result); return NULL; }
	result [length] = 0;   /* Trailing null byte. */
	return result;
}

char * bingets4 (FILE *f) {
	unsigned long length = bingetu4 (f);
	char *result = Melder_malloc (length + 1);
	if (! result)
		return Melder_errorp ("(bingets4:) Out of memory. Cannot create string of length %ld.", length);
	if (fread (result, 1, length, f) != length) { Melder_free (result); return NULL; }
	result [length] = 0;   /* Trailing null byte. */
	return result;
}

void binputs1 (const char *s, FILE *f) {
	unsigned int length = s ? strlen (s) : 0; if (length > 255) length = 255;
	binputu1 (length, f); if (s) fwrite (s, 1, length, f);
}

void binputs2 (const char *s, FILE *f) {
	unsigned int length = s ? strlen (s) : 0; if (length > 65535) length = 65535;
	binputu2 (length, f); if (s) fwrite (s, 1, length, f);
}

void binputs4 (const char *s, FILE *f) {
	unsigned long length = s ? strlen (s) : 0;
	binputu4 (length, f); if (s) fwrite (s, 1, length, f);
}

/********** machine-independent cache I/O **********/

#define my  me ->

#define START(x)  char *ptr = (char *) & (x);
#define READ  * ptr ++ = * f -> ptr ++;
#define WRITE  * f -> ptr ++ = * ptr ++;

CACHE * memopen (size_t nbytes) {
	CACHE *me;
	if (nbytes < 1) return NULL;
	if (! (me = Melder_malloc (sizeof (CACHE)))) return NULL;
	if (! (my base = Melder_malloc (nbytes))) { Melder_free (me); return NULL; }
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
int cacgete1 (CACHE *me, void *enumerated) {
	int result = * (signed char *) my ptr ++;
	if (result < 0)
		(void) Melder_error ("(cacgete1:) %d is not a value of enumerated type \"%s\".",
			result, enum_type (enumerated));
	return result;
}
void cacpute1 (int value, CACHE *me, void *enumerated) {
	(void) enumerated;
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

int cacgete2 (CACHE *f, void *enumerated) {
	signed short s;
	if (binario_shortBE2) {
		START (s) READ READ
	} else {
		unsigned char bytes [2];
		START (bytes) READ READ
		s = ((unsigned short) bytes [0] << 8) | (unsigned short) bytes [1];
	}
	if (s < 0)
		(void) Melder_error ("(cacgete2:) %d is not a value of enumerated type \"%s\".",
			s, enum_type (enumerated));
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

void cacpute2 (int value, CACHE *f, void *enumerated) {
	if (binario_shortBE2) {
		signed short s = value;
		START (s) WRITE WRITE
	} else {
		char bytes [2];
		bytes [0] = value >> 8;
		bytes [1] = value;
		{ START (bytes) WRITE WRITE }
	}
	(void) enumerated;
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
	unsigned int length = (unsigned char) * f -> ptr ++;
	char *result = Melder_malloc (length + 1);
	if (! result)
		return Melder_errorp ("(cacgets1:) Out of memory. Cannot create a string of length %d.", length);
	if (memread (result, 1, length, f) != length) { Melder_free (result); return NULL; }
	result [length] = 0;   /* Trailing null byte. */
	return result;
}

char * cacgets2 (CACHE *f) {
	unsigned int length = cacgetu2 (f);
	char *result = Melder_malloc (length + 1);
	if (! result)
		return Melder_errorp ("(cacgets2:) Out of memory. Cannot create a string of length %d.", length);
	if (memread (result, 1, length, f) != length) { Melder_free (result); return NULL; }
	result [length] = 0;   /* Trailing null byte. */
	return result;
}

char * cacgets4 (CACHE *f) {
	unsigned long length = cacgetu4 (f);
	char *result = Melder_malloc (length + 1);
	if (! result)
		return Melder_errorp ("(cacgets4:) Out of memory. Cannot create a string of length %ld.", length);
	if (memread (result, 1, length, f) != length) { Melder_free (result); return NULL; }
	result [length] = 0;   /* Trailing null byte. */
	return result;
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

/* End of file abcio.c */

