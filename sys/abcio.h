#ifndef _abcio_h_
#define _abcio_h_
/* abcio.h
 *
 * Copyright (C) 1992-2011,2015,2017 Paul Boersma
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

#include "complex.h"
#include "melder.h"

#include "abcio_enums.h"

/* Numeric text input and output. */

int texgeti1 (MelderReadText text);
int16 texgeti16 (MelderReadText text);
#define texgeti2 texgeti16
long texgeti32 (MelderReadText text);
#define texgeti4 texgeti32
unsigned int texgetu1 (MelderReadText text);
unsigned int texgetu2 (MelderReadText text);
unsigned long texgetu4 (MelderReadText text);
double texgetr4 (MelderReadText text);
double texgetr8 (MelderReadText text);
double texgetr10 (MelderReadText text);
fcomplex texgetc8 (MelderReadText text);
dcomplex texgetc16 (MelderReadText text);
short texgete1 (MelderReadText text, int (*getValue) (const char32 *));
short texgete2 (MelderReadText text, int (*getValue) (const char32 *));
bool texgeteb (MelderReadText text);
bool texgeteq (MelderReadText text);
bool texgetex (MelderReadText text);
char *texgets2 (MelderReadText text);
char *texgets4 (MelderReadText text);
char32 *texgetw2 (MelderReadText text);
char32 *texgetw4 (MelderReadText text);

void texindent (MelderFile file);
void texexdent (MelderFile file);
void texresetindent (MelderFile file);
void texputintro (MelderFile file, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);

void texputi1 (MelderFile file, int i, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputi16 (MelderFile file, int i, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
#define texputi2 texputi16
void texputi32 (MelderFile file, long i, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
#define texputi4 texputi32
void texputu1 (MelderFile file, unsigned int u, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputu2 (MelderFile file, unsigned int u, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputu4 (MelderFile file, unsigned long u, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputr4 (MelderFile file, double x, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputr8 (MelderFile file, double x, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputc8 (MelderFile file, fcomplex z, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputc16 (MelderFile file, dcomplex z, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texpute1 (MelderFile file, int i, const char32 * (*getText) (int), const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texpute2 (MelderFile file, int i, const char32 * (*getText) (int), const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputeb (MelderFile file, bool i, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputeq (MelderFile file, bool i, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputex (MelderFile file, bool i, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputs1 (MelderFile file, const char *s, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputs2 (MelderFile file, const char *s, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputs4 (MelderFile file, const char *s, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputw2 (MelderFile file, const char32 *s, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputw2 (MelderFile file, const char32  *s, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);
void texputw4 (MelderFile file, const char32 *s, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6);

/* Portable device-independent binary input and output. */
/* Works on all machines with 8-bit bytes and chars, and 2's complement integers. */
/* Works independent of byte order or floating-point implementations. */
/*
	The 42 routines are analogous to fgetc and fputc, who read or write one character:
		int fgetc (FILE *f);   int fputc (int c, FILE *f);   // 0..255
*/
unsigned int bingetu1 (FILE *f);   void binputu1 (unsigned int i, FILE *f);   // 0..255
uint16 bingetu2 (FILE *f);   void binputu2 (uint16 i, FILE *f);   // 0..65535
uint32 bingetu4 (FILE *f);   void binputu4 (uint32 i, FILE *f);   // 0..4294967295

int bingeti1 (FILE *f);   void binputi1 (int i, FILE *f);   /* -128..127 */
int16 bingeti16 (FILE *f);   void binputi16 (int16 i, FILE *f);   // -32768..32767
int32 bingeti3 (FILE *f);   void binputi3 (int32 i, FILE *f);   // -8388608..8388607
int32 bingeti32 (FILE *f);   void binputi32 (int32 i, FILE *f);   // -2147483648..2147483647
#define bingeti2 bingeti16
#define binputi2 binputi16
#define bingeti4 bingeti32
#define binputi4 binputi32
/*
	Read or write signed or unsigned integers from or to 2 or 4 bytes in the stream 'f',
	in big-endian byte order (most significant byte first).
	This is the native integer format on Macintosh and Silicon Graphics Iris.
*/

int16 bingeti2LE (FILE *f);   void binputi2LE (int16 i, FILE *f);   // -32768..32767
int32 bingeti3LE (FILE *f);   void binputi3LE (int32 i, FILE *f);   // -8388608..8388607
int32 bingeti4LE (FILE *f);   void binputi4LE (int32 i, FILE *f);   // -2147483648..2147483647
uint16 bingetu2LE (FILE *f);   void binputu2LE (uint16 i, FILE *f);   // 0..65535
uint32 bingetu4LE (FILE *f);   void binputu4LE (uint32 i, FILE *f);   // 0..4294967295
/*
	Read or write signed or unsigned integers from or to 2 or 4 bytes in the stream 'f',
	in little-endian byte order (least significant byte first).
	This is the native integer format on Vax and IBM PC.
*/

unsigned int bingetb1 (FILE *f);   void binputb1 (unsigned int value, FILE *f);   // 0..1
unsigned int bingetb2 (FILE *f);   void binputb2 (unsigned int value, FILE *f);   // 0..3
unsigned int bingetb3 (FILE *f);   void binputb3 (unsigned int value, FILE *f);   // 0..7
unsigned int bingetb4 (FILE *f);   void binputb4 (unsigned int value, FILE *f);   // 0..15
unsigned int bingetb5 (FILE *f);   void binputb5 (unsigned int value, FILE *f);   // 0..31
unsigned int bingetb6 (FILE *f);   void binputb6 (unsigned int value, FILE *f);   // 0..63
unsigned int bingetb7 (FILE *f);   void binputb7 (unsigned int value, FILE *f);   // 0..127
void bingetb (FILE *f);   void binputb (FILE *f);
/*
	Read or write an unsigned integer from or to a number of bits in the stream 'f'.
	After an uninterrupted sequence of bingetbxxx or binputbxxx, call bingetb or binputb.
	Not reentrant: uses a static one-byte buffer. Do not call from interrupt routines.
	Example 1:
		int hasTitle = bingetb1 (f);   // one bit
		int category = bingetb3 (f);   // a number in the range 0..7
		int sense = bingetb2 (f);   // a number in the range 0..3
		bingetb (f);
	Example 2:
		binputb1 (hasTitle, f);   // one bit
		binputb3 (category, f);   // a number in the range 0..7
		binputb2 (sense, f);   // a number in the range 0..3
		binputb (f);
*/

int bingete1 (FILE *f, int min, int max, const char32 *type);
int bingete2 (FILE *f, int min, int max, const char32 *type);
bool bingeteb (FILE *f);
#define bingeteq bingeteb
#define bingetex bingeteb

void binpute1 (int value, FILE *f);
void binpute2 (int value, FILE *f);
void binputeb (bool value, FILE *f);
#define binputeq binputeb
#define binputex binputeb

double bingetr4 (FILE *f);   void binputr4 (double x, FILE *f);
/*
	Read or write a real number from or to 4 bytes in the stream `f`,
	in IEEE single-precision binary real format, with the most significant bit first.
	NaN's (Not-a-Numbers) and infinities get the value HUGE, which equals infinity on IEEE machines.
	Absolute values of normalized numbers (23 bits precision): 1.2e-38 .. 3.4e38.
	Denormalized: from 1.4e-45.
	This is the native format of a `float` on Macintosh and Silicon Graphics Iris.
*/
double bingetr4LE (FILE *f);   void binputr4LE (double x, FILE *f);   // least significant bit first

double bingetr8 (FILE *f);   void binputr8 (double x, FILE *f);
/*
	Read or write a real number from or to 8 bytes in the stream `f`,
	in IEEE double-precision binary real format, with the most significant bit first.
	NaN's (Not-a-Numbers) and infinities get the value HUGE, which equals infinity on IEEE machines.
	Absolute values of normalized numbers (52 bits precision): 2.2e-308 .. 1.8e308.
	Denormalized: from 4.9e-324.
	This is the native format of a `double` on Silicon Graphics Iris and PowerMac.
*/

double bingetr10 (FILE *f);   void binputr10 (double x, FILE *f);
/*
	Read or write a real number from or to 10 bytes in the stream `f`,
	in IEEE extended-precision binary real format, with the most significant bit first,
	as implemented in Motorola's MC68881 floating-point coprocessor.
	NaN's (Not-a-Numbers) and infinities get the value HUGE, which equals infinity on IEEE machines.
	Absolute values of normalized numbers (63 bits precision): 8e-4933 .. 6e4931.
	Denormalized: from 9e-4952.
	This format is used to encode the sampling rate in AIFF (Audio Interchange File Format) files,
	and is the native format of a `double` on 68k Macintosh.
*/

fcomplex bingetc8 (FILE *f);
dcomplex bingetc16 (FILE *f);
void binputc8 (fcomplex z, FILE *f);
void binputc16 (dcomplex z, FILE *f);

char * bingets1 (FILE *f);   void binputs1 (const char *s, FILE *f);   // 0..255 characters
char * bingets2 (FILE *f);   void binputs2 (const char *s, FILE *f);   // 0..65535 characters
char * bingets4 (FILE *f);   void binputs4 (const char *s, FILE *f);   // 0..4294967295 characters
/*
	Read or write a string from or to `str32len(s)` UTF-16LE or ASCII characters plus 1, 2, or 4 bytes in the stream `f`,
	in a Pascal-style format: first the length, then the characters, without a trailing null byte.
	bingetsxxx returns a new 'malloc'ed null-terminated C string (for the caller to `free` it).
	Fail if out of memory.
	binputsxxx expects a null-terminated C string whose `str32len` fits in 1, 2, or 4 bytes.
*/
char32 * bingetw1 (FILE *f);   void binputw1 (const char32 *s, FILE *f);
char32 * bingetw2 (FILE *f);   void binputw2 (const char32 *s, FILE *f);
char32 * bingetw4 (FILE *f);   void binputw4 (const char32 *s, FILE *f);

/* End of file abcio.h */
#endif
