#ifndef _abcio_h_
#define _abcio_h_
/* abcio.h
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2004/05/14 some three-byte routines
 */

#ifndef _complex_h_
	#include "complex.h"
#endif

/* Numeric ASCII input and output. */

int ascgeti1 (FILE *f);
int ascgeti2 (FILE *f);
long ascgeti4 (FILE *f);
unsigned int ascgetu1 (FILE *f);
unsigned int ascgetu2 (FILE *f);
unsigned long ascgetu4 (FILE *f);
double ascgetr4 (FILE *f);
double ascgetr8 (FILE *f);
double ascgetr10 (FILE *f);
fcomplex ascgetc8 (FILE *f);
dcomplex ascgetc16 (FILE *f);
char ascgetc1 (FILE *f);
short ascgete1 (FILE *f, void *enumerated);
short ascgete2 (FILE *f, void *enumerated);
short ascgeteb (FILE *f);
short ascgeteq (FILE *f);
short ascgetex (FILE *f);
char *ascgets1 (FILE *f);
char *ascgets2 (FILE *f);
char *ascgets4 (FILE *f);

void ascindent (void);
void ascexdent (void);
void ascresetindent (void);
void ascputintro (FILE *f, const char *format, ...);

void ascputi1 (int i, FILE *f, const char *format, ...);
void ascputi2 (int i, FILE *f, const char *format, ...);
void ascputi4 (long i, FILE *f, const char *format, ...);
void ascputu1 (unsigned int u, FILE *f, const char *format, ...);
void ascputu2 (unsigned int u, FILE *f, const char *format, ...);
void ascputu4 (unsigned long u, FILE *f, const char *format, ...);
void ascputr4 (double x, FILE *f, const char *format, ...);
void ascputr8 (double x, FILE *f, const char *format, ...);
void ascputr10 (double x, FILE *f, const char *format, ...);
void ascputc8 (fcomplex z, FILE *f, const char *format, ...);
void ascputc16 (dcomplex z, FILE *f, const char *format, ...);
void ascputc1 (int i, FILE *f, const char *format, ...);
void ascpute1 (int i, FILE *f, void *enumerated, const char *format, ...);
void ascpute2 (int i, FILE *f, void *enumerated, const char *format, ...);
void ascputeb (int i, FILE *f, const char *format, ...);
void ascputeq (int i, FILE *f, const char *format, ...);
void ascputex (int i, FILE *f, const char *format, ...);
void ascputs1 (const char *s, FILE *f, const char *format, ...);
void ascputs2 (const char *s, FILE *f, const char *format, ...);
void ascputs4 (const char *s, FILE *f, const char *format, ...);

void ascio_verbose (int verbose);

/* Portable device-independent binary input and output. */
/* Works on all machines with 8-bit bytes and chars, and 2's complement integers. */
/* Works independent of byte order or floating-point implementations. */
/*
	The 42 routines are analogous to fgetc and fputc, who read or write one character:
		int fgetc (FILE *f);   int fputc (int c, FILE *f);   // 0..255
*/
unsigned int bingetu1 (FILE *f);   void binputu1 (unsigned int i, FILE *f);   /* 0..255 */
unsigned int bingetu2 (FILE *f);   void binputu2 (unsigned int i, FILE *f);   /* 0..65535 */
unsigned long bingetu4 (FILE *f);   void binputu4 (unsigned long i, FILE *f);   /* 0..4294967295 */

int bingeti1 (FILE *f);   void binputi1 (int i, FILE *f);   /* -128..127 */
int bingeti2 (FILE *f);   void binputi2 (int i, FILE *f);   /* -32768..32767 */
long bingeti3 (FILE *f);   void binputi3 (long i, FILE *f);   /* -8388608..2148388607 */
long bingeti4 (FILE *f);   void binputi4 (long i, FILE *f);   /* -2147483648..2147483647 */
/*
	Read or write signed or unsigned integers from or to 2 or 4 bytes in the stream 'f',
	in big-endian byte order (most significant byte first).
	This is the native integer format on Macintosh and Silicon Graphics Iris.
*/

int bingeti2LE (FILE *f);   void binputi2LE (int i, FILE *f);   /* -32768..32767 */
long bingeti3LE (FILE *f);   void binputi3LE (long i, FILE *f);   /* -8388608..2148388607 */
long bingeti4LE (FILE *f);   void binputi4LE (long i, FILE *f);   /* -2147483648..2147483647 */
unsigned int bingetu2LE (FILE *f);   void binputu2LE (unsigned int i, FILE *f);   /* 0..65535 */
unsigned long bingetu4LE (FILE *f);   void binputu4LE (unsigned long i, FILE *f);   /* 0..4294967295 */
/*
	Read or write signed or unsigned integers from or to 2 or 4 bytes in the stream 'f',
	in little-endian byte order (least significant byte first).
	This is the native integer format on Vax and IBM PC.
*/

unsigned int bingetb1 (FILE *f);   void binputb1 (unsigned int value, FILE *f);   /* 0..1*/
unsigned int bingetb2 (FILE *f);   void binputb2 (unsigned int value, FILE *f);   /* 0..3 */
unsigned int bingetb3 (FILE *f);   void binputb3 (unsigned int value, FILE *f);   /* 0..7 */
unsigned int bingetb4 (FILE *f);   void binputb4 (unsigned int value, FILE *f);   /* 0..15 */
unsigned int bingetb5 (FILE *f);   void binputb5 (unsigned int value, FILE *f);   /* 0..31 */
unsigned int bingetb6 (FILE *f);   void binputb6 (unsigned int value, FILE *f);   /* 0..63 */
unsigned int bingetb7 (FILE *f);   void binputb7 (unsigned int value, FILE *f);   /* 0..127 */
void bingetb (FILE *f);   void binputb (FILE *f);
/*
	Read or write an unsigned integer from or to a number of bits in the stream 'f'.
	After an uninterrupted sequence of bingetbxxx or binputbxxx, call bingetb or binputb.
	Not reentrant: uses a static one-byte buffer. Do not call from interrupt routines.
	Example 1:
		int hasTitle = bingetb1 (f);   // One bit.
		int category = bingetb3 (f);   // A number in the range 0..7.
		int sense = bingetb2 (f);   // A number in the range 0..3.
		bingetb (f);
	Example 2:
		binputb1 (hasTitle, f);   // One bit.
		binputb3 (category, f);   // A number in the range 0..7.
		binputb2 (sense, f);   // A number in the range 0..3.
		binputb (f);
*/

int bingete1 (FILE *f, void *enumerated);
int bingete2 (FILE *f, void *enumerated);
#define bingeteb bingeti1
#define bingeteq bingeti1
#define bingetex bingeti1

void binpute1 (int value, FILE *f, void *enumerated);
void binpute2 (int value, FILE *f, void *enumerated);
#define binputeb binputi1
#define binputeq binputi1
#define binputex binputi1

double bingetr4 (FILE *f);   void binputr4 (double x, FILE *f);
/*
	Read or write a real number from or to 4 bytes in the stream 'f',
	in IEEE single-precision binary real format, with the most significant bit first.
	NaN's (Not-a-Numbers) and infinities get the value HUGE, which equals infinity on IEEE machines.
	Absolute values of normalized numbers (23 bits precision): 1.2e-38 .. 3.4e38.
	Denormalized: from 1.4e-45.
	This is the native format of a 'float' on Macintosh and Silicon Graphics Iris.
*/

double bingetr8 (FILE *f);   void binputr8 (double x, FILE *f);
/*
	Read or write a real number from or to 8 bytes in the stream 'f',
	in IEEE double-precision binary real format, with the most significant bit first.
	NaN's (Not-a-Numbers) and infinities get the value HUGE, which equals infinity on IEEE machines.
	Absolute values of normalized numbers (52 bits precision): 2.2e-308 .. 1.8e308.
	Denormalized: from 4.9e-324.
	This is the native format of a 'double' on Silicon Graphics Iris and PowerMac.
*/

double bingetr10 (FILE *f);   void binputr10 (double x, FILE *f);
/*
	Read or write a real number from or to 10 bytes in the stream 'f',
	in IEEE extended-precision binary real format, with the most significant bit first,
	as implemented in Motorola's MC68881 floating-point coprocessor.
	NaN's (Not-a-Numbers) and infinities get the value HUGE, which equals infinity on IEEE machines.
	Absolute values of normalized numbers (63 bits precision): 8e-4933 .. 6e4931.
	Denormalized: from 9e-4952.
	This format is used to encode the sampling rate in AIFF (Audio Interchange File Format) files,
	and is the native format of a 'double' on 68k Macintosh.
*/

fcomplex bingetc8 (FILE *f);
dcomplex bingetc16 (FILE *f);
void binputc8 (fcomplex z, FILE *f);
void binputc16 (dcomplex z, FILE *f);
#define bingetc1 bingeti1
#define binputc1 binputi1

char * bingets1 (FILE *f);   void binputs1 (const char *s, FILE *f);   /* 0..255 characters. */
char * bingets2 (FILE *f);   void binputs2 (const char *s, FILE *f);   /* 0..65535 characters. */
char * bingets4 (FILE *f);   void binputs4 (const char *s, FILE *f);   /* 0..4294967295 characters. */
/*
	Read or write a string from or to strlen (s) + 1, 2, or 4 bytes in the stream 'f',
	in a Pascal-style format: first the length, then the characters, without a trailing null byte.
	bingetsxxx returns a new 'malloc'ed null-terminated C string (for the caller to 'free' it),
	or NULL if out of memory.
	binputsxxx expects a null-terminated C string whose 'strlen' fits in 1, 2, or 4 bytes.
*/

/********** cache I/O **********/

typedef struct { unsigned char *ptr, *base, *max; } CACHE;

CACHE * memopen (size_t nbytes);

/* The following routines are completely analogous to fxxx, although they do not check for overflow. */

int memclose (CACHE *f);
size_t memread (void *ptr, size_t size, size_t nmemb, CACHE *m);
size_t memwrite (const void *ptr, size_t size, size_t nmemb, CACHE *m);
int memseek (CACHE *f, long offset, int whence);   /* No SEEK_END. */
long memtell (CACHE *f);
void memrewind (CACHE *me);

void memprint1 (CACHE *me, const char *s1);
void memprint2 (CACHE *me, const char *s1, const char *s2);
void memprint3 (CACHE *me, const char *s1, const char *s2, const char *s3);
void memprint4 (CACHE *me, const char *s1, const char *s2, const char *s3, const char *s4);
void memprint5 (CACHE *me, const char *s1, const char *s2, const char *s3, const char *s4, const char *s5);

/*
	Portable device-independent binary input and output from and to a CACHE.
	This rest of this header file is completely analogous to binario.h, with only the following changes:
		bingetxxx replaced by cacgetxxx;
		binputxxx replaced by cacputxxx;
		FILE replaced by CACHE;
*/
unsigned int cacgetb1 (CACHE *f);   void cacputb1 (unsigned int value, CACHE *f);
unsigned int cacgetb2 (CACHE *f);   void cacputb2 (unsigned int value, CACHE *f);
unsigned int cacgetb3 (CACHE *f);   void cacputb3 (unsigned int value, CACHE *f);
unsigned int cacgetb4 (CACHE *f);   void cacputb4 (unsigned int value, CACHE *f);
unsigned int cacgetb5 (CACHE *f);   void cacputb5 (unsigned int value, CACHE *f);
unsigned int cacgetb6 (CACHE *f);   void cacputb6 (unsigned int value, CACHE *f);
unsigned int cacgetb7 (CACHE *f);   void cacputb7 (unsigned int value, CACHE *f);
void cacgetb (CACHE *f);   void cacputb (CACHE *f);
unsigned int cacgetu1 (CACHE *f); void cacputu1 (unsigned int u, CACHE *f);
unsigned int cacgetu2 (CACHE *f);   void cacputu2 (unsigned int i, CACHE *f);
unsigned long cacgetu4 (CACHE *f);   void cacputu4 (unsigned long i, CACHE *f);
int cacgete1 (CACHE *f, void *enumerated); void cacpute1 (int value, CACHE *f, void *enumerated);
int cacgete2 (CACHE *f, void *enumerated);   void cacpute2 (int value, CACHE *f, void *enumerated);
#define cacgeteb cacgeti1
#define cacgeteq cacgeti1
#define cacgetex cacgeti1
#define cacputeb cacputi1
#define cacputeq cacputi1
#define cacputex cacputi1
int cacgeti1 (CACHE *f);   void cacputi1 (int i, CACHE *f);
int cacgeti2 (CACHE *f);   void cacputi2 (int i, CACHE *f);
long cacgeti4 (CACHE *f);   void cacputi4 (long i, CACHE *f);
int cacgeti2LE (CACHE *f);   void cacputi2LE (int i, CACHE *f);
double cacgetr4 (CACHE *f);   void cacputr4 (double x, CACHE *f);
double cacgetr8 (CACHE *f);   void cacputr8 (double x, CACHE *f);
double cacgetr10 (CACHE *f);   void cacputr10 (double x, CACHE *f);
fcomplex cacgetc8 (CACHE *f);
dcomplex cacgetc16 (CACHE *f);
void cacputc8 (fcomplex z, CACHE *f);
void cacputc16 (dcomplex z, CACHE *f);
#define cacgetc1 cacgeti1
#define cacputc1 cacputi1
long cacgeti4LE (CACHE *f);   void cacputi4LE (long i, CACHE *f);
unsigned int cacgetu2LE (CACHE *f);   void cacputu2LE (unsigned int i, CACHE *f);
unsigned long cacgetu4LE (CACHE *f);   void cacputu4LE (unsigned long i, CACHE *f);
char * cacgets1 (CACHE *f);   void cacputs1 (const char *s, CACHE *f);
char * cacgets2 (CACHE *f);   void cacputs2 (const char *s, CACHE *f);
char * cacgets4 (CACHE *f);   void cacputs4 (const char *s, CACHE *f);

/* End of file abcio.h */
#endif
