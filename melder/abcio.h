#ifndef _abcio_h_
#define _abcio_h_
/* abcio.h
 *
 * Copyright (C) 1992-2011,2015,2017-2020 Paul Boersma
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

/* Numeric text input and output. */

int texgeti8 (MelderReadText text);
int16 texgeti16 (MelderReadText text);
int32 texgeti32 (MelderReadText text);
integer texgetinteger (MelderReadText text);
#define texgetinteger16BE texgetinteger
#define texgetinteger32BE texgetinteger
unsigned int texgetu8 (MelderReadText text);
uint16 texgetu16 (MelderReadText text);
uint32 texgetu32 (MelderReadText text);
double texgetr32 (MelderReadText text);
double texgetr64 (MelderReadText text);
double texgetr80 (MelderReadText text);
dcomplex texgetc64 (MelderReadText text);
dcomplex texgetc128 (MelderReadText text);
int texgete8 (MelderReadText text, enum_generic_getValue getValue);
int texgete16 (MelderReadText text, enum_generic_getValue getValue);
bool texgeteb (MelderReadText text);
bool texgeteq (MelderReadText text);
bool texgetex (MelderReadText text);
autostring32 texgetw16 (MelderReadText text);
autostring32 texgetw32 (MelderReadText text);

void texindent (MelderFile file);
void texexdent (MelderFile file);
void texresetindent (MelderFile file);

#define texput_UP_TO_NINE_NULLABLE_STRINGS  \
	conststring32 s1 = nullptr, conststring32 s2 = nullptr, conststring32 s3 = nullptr, \
	conststring32 s4 = nullptr, conststring32 s5 = nullptr, conststring32 s6 = nullptr, \
	conststring32 s7 = nullptr, conststring32 s8 = nullptr, conststring32 s9 = nullptr
void texputintro   (MelderFile file,                                        texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputi8      (MelderFile file, int i,                                 texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputi16     (MelderFile file, int i,                                 texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputi32     (MelderFile file, long i,                                texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputinteger (MelderFile file, integer i,                             texput_UP_TO_NINE_NULLABLE_STRINGS);
#define texputinteger16BE texputinteger
#define texputinteger32BE texputinteger
void texputu8      (MelderFile file, unsigned int u,                        texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputu16     (MelderFile file, unsigned int u,                        texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputu32     (MelderFile file, unsigned long u,                       texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputr32     (MelderFile file, double x,                              texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputr64     (MelderFile file, double x,                              texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputc64     (MelderFile file, dcomplex z,                            texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputc128    (MelderFile file, dcomplex z,                            texput_UP_TO_NINE_NULLABLE_STRINGS);
void texpute8      (MelderFile file, int i, conststring32 (*getText) (int), texput_UP_TO_NINE_NULLABLE_STRINGS);
void texpute16     (MelderFile file, int i, conststring32 (*getText) (int), texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputeb      (MelderFile file, bool i,                                texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputeq      (MelderFile file, bool i,                                texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputex      (MelderFile file, bool i,                                texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputs8      (MelderFile file, conststring8 s,                        texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputs16     (MelderFile file, conststring8 s,                        texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputs32     (MelderFile file, conststring8 s,                        texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputw16     (MelderFile file, conststring32 s,                       texput_UP_TO_NINE_NULLABLE_STRINGS);
void texputw32     (MelderFile file, conststring32 s,                       texput_UP_TO_NINE_NULLABLE_STRINGS);
#undef texput_UP_TO_NINE_NULLABLE_STRINGS

/* Portable device-independent binary input and output. */
/* Works on all machines with 8-bit bytes and chars, and 2's complement integers. */
/* Works independent of byte order or floating-point implementations. */
/*
	The 42 routines are analogous to fgetc and fputc, who read or write one character:
		int fgetc (FILE *f);   int fputc (int c, FILE *f);   // 0..255
*/
unsigned int bingetu8 (FILE *f);   void binputu8 (unsigned int i, FILE *f);   // 0..255
uint16 bingetu16 (FILE *f);   void binputu16 (uint16 i, FILE *f);   // 0..65535
uint32 bingetu32 (FILE *f);   void binputu32 (uint32 i, FILE *f);   // 0..4294967295

int bingeti8 (FILE *f);   void binputi8 (int i, FILE *f);   /* -128..127 */
int16 bingeti16 (FILE *f);   void binputi16 (int16 i, FILE *f);   // -32768..32767
int32 bingeti24 (FILE *f);   void binputi24 (int32 i, FILE *f);   // -8388608..8388607
int32 bingeti32 (FILE *f);   void binputi32 (int32 i, FILE *f);   // -2147483648..2147483647
integer bingetinteger32BE (FILE *f);   void binputinteger32BE (integer i, FILE *f);
integer bingetinteger16BE (FILE *f);   void binputinteger16BE (integer i, FILE *f);
/*
	Read or write signed or unsigned integers from or to 2 or 4 bytes in the stream 'f',
	in big-endian byte order (most significant byte first).
	This is the native integer format on Macintosh and Silicon Graphics Iris.
*/

int16 bingeti16LE (FILE *f);   void binputi16LE (int16 i, FILE *f);   // -32768..32767
int32 bingeti24LE (FILE *f);   void binputi24LE (int32 i, FILE *f);   // -8388608..8388607
int32 bingeti32LE (FILE *f);   void binputi32LE (int32 i, FILE *f);   // -2147483648..2147483647
uint16 bingetu16LE (FILE *f);   void binputu16LE (uint16 i, FILE *f);   // 0..65535
uint32 bingetu32LE (FILE *f);   void binputu32LE (uint32 i, FILE *f);   // 0..4294967295
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

int bingete8 (FILE *f, int min, int max, conststring32 type);
int bingete16 (FILE *f, int min, int max, conststring32 type);
bool bingetbool8 (FILE *f);
#define bingeteb bingetbool8
#define bingeteq bingetbool8
#define bingetex bingetbool8

void binpute8 (int value, FILE *f);
void binpute16 (int value, FILE *f);
void binputbool8 (bool value, FILE *f);
#define binputeb binputbool8
#define binputeq binputbool8
#define binputex binputbool8

double bingetr32 (FILE *f);   void binputr32 (double x, FILE *f);
/*
	Read or write a real number from or to 4 bytes in the stream `f`,
	in IEEE single-precision binary real format, with the most significant bit first.
	NaN's (Not-a-Numbers) and infinities get the value HUGE, which equals infinity on IEEE machines.
	Absolute values of normalized numbers (23 bits precision): 1.2e-38 .. 3.4e38.
	Denormalized: from 1.4e-45.
	This is the native format of a `float` on Macintosh and Silicon Graphics Iris.
*/
double bingetr32LE (FILE *f);   void binputr32LE (double x, FILE *f);   // least significant bit first

double bingetr64 (FILE *f);   void binputr64 (double x, FILE *f);
/*
	Read or write a real number from or to 8 bytes in the stream `f`,
	in IEEE double-precision binary real format, with the most significant bit first.
	NaN's (Not-a-Numbers) and infinities get the value HUGE, which equals infinity on IEEE machines.
	Absolute values of normalized numbers (52 bits precision): 2.2e-308 .. 1.8e308.
	Denormalized: from 4.9e-324.
	This is the native format of a `double` on Silicon Graphics Iris and PowerMac.
*/
double bingetr64LE (FILE *f);   void binputr64LE (double x, FILE *f);   // least significant bit first

double bingetr80 (FILE *f);   void binputr80 (double x, FILE *f);
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

dcomplex bingetc64 (FILE *f);
dcomplex bingetc128 (FILE *f);
void binputc64 (dcomplex z, FILE *f);
void binputc128 (dcomplex z, FILE *f);

autostring8 bingets8 (FILE *f);   void binputs8 (const char *s, FILE *f);   // 0..255 characters
autostring8 bingets16 (FILE *f);   void binputs16 (const char *s, FILE *f);   // 0..65535 characters
autostring8 bingets32 (FILE *f);   void binputs32 (const char *s, FILE *f);   // 0..4294967295 characters
/*
	Read or write a string from or to `str32len(s)` UTF-16LE or ASCII characters plus 1, 2, or 4 bytes in the stream `f`,
	in a Pascal-style format: first the length, then the characters, without a trailing null byte.
	bingetsxxx returns a new 'malloc'ed null-terminated C string (for the caller to `free` it).
	Fail if out of memory.
	binputsxxx expects a null-terminated C string whose `str32len` fits in 1, 2, or 4 bytes.
*/
autostring32 bingetw8 (FILE *f);   void binputw8 (conststring32 s, FILE *f);
autostring32 bingetw16 (FILE *f);   void binputw16 (conststring32 s, FILE *f);
autostring32 bingetw32 (FILE *f);   void binputw32 (conststring32 s, FILE *f);

/* End of file abcio.h */
#endif
