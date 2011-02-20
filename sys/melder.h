#ifndef _melder_h_
#define _melder_h_
/* melder.h
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
 * pb 2010/12/28
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
	#define strequ  ! strcmp
	#define strnequ  ! strncmp
	#define wcsequ  ! wcscmp
	#define wcsnequ  ! wcsncmp
	#define Melder_strequ  ! Melder_strcmp
	#define Melder_strnequ  ! Melder_strncmp
	#define Melder_wcsequ  ! Melder_wcscmp
	#define Melder_wcsnequ  ! Melder_wcsncmp
#include <stdarg.h>
#include <stddef.h>
#include <wchar.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
	#define GLOBAL_C  extern "C"
	#define GLOBAL_C_BEGIN  extern "C" {
	#define GLOBAL_C_END  }
#else
	#define GLOBAL_C  extern
	#define GLOBAL_C_BEGIN
	#define GLOBAL_C_END
#endif

GLOBAL_C bool Melder_wcsequ_firstCharacterCaseInsensitive (const wchar_t *string1, const wchar_t *string2);

#include "enums.h"

#include "melder_enums.h"

#ifndef TRUE
	#define TRUE  1
#endif
#ifndef FALSE
	#define FALSE  0
#endif
#ifndef NULL
	#define NULL  ((void *) 0)
#endif

/********** NUMBER TO STRING CONVERSION **********/

/*
	The following routines return a static string, chosen from a circularly used set of 11 buffers.
	You can call at most 11 of them in one Melder_casual call, for instance.
*/
GLOBAL_C const wchar_t * Melder_integer (long value);
GLOBAL_C const wchar_t * Melder_bigInteger (double value);
GLOBAL_C const wchar_t * Melder_boolean (bool value);   // "yes" or "no"
GLOBAL_C const wchar_t * Melder_double (double value);   // "--undefined--" or something in the "%.15g", "%.16g", or "%.17g" formats
GLOBAL_C const wchar_t * Melder_single (double value);   // "--undefined--" or something in the "%.8g" format
GLOBAL_C const wchar_t * Melder_half (double value);   // "--undefined--" or something in the "%.4g" format
GLOBAL_C const wchar_t * Melder_fixed (double value, int precision);   // "--undefined--" or something in the "%.*f" format
GLOBAL_C const wchar_t * Melder_fixedExponent (double value, int exponent, int precision);
	/* if exponent is -2 and precision is 2:   67E-2, 0.00024E-2 */
GLOBAL_C const wchar_t * Melder_percent (double value, int precision);
	/* "--undefined--" or, if precision is 3: "0" or "34.400%" or "0.014%" or "0.001%" or "0.0000007%" */
GLOBAL_C const wchar_t * Melder_float (const wchar_t *number);
	/* turns 1e+4 into 10^^4, or -1.23456e-78 into -1.23456\.c10^^-78 */
GLOBAL_C const wchar_t * Melder_naturalLogarithm (double lnNumber);   // turns -10000 into "1.135483865315339e-4343"

/********** STRING TO NUMBER CONVERSION **********/

GLOBAL_C int Melder_isStringNumeric (const wchar_t *string);
GLOBAL_C double Melder_atof (const wchar_t *string);
	/*
	 * "3.14e-3" -> 3.14e-3
	 * "15.6%" -> 0.156
	 * "fghfghj" -> NUMundefined
	 */

/********** CONSOLE **********/

GLOBAL_C void Melder_writeToConsole (wchar_t *message, bool useStderr);

/********** ERROR **********/

GLOBAL_C int Melder_error (const char *format, ...);
GLOBAL_C bool Melder_error1 (const wchar_t *s1);
GLOBAL_C bool Melder_error2 (const wchar_t *s1, const wchar_t *s2);
GLOBAL_C bool Melder_error3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
GLOBAL_C bool Melder_error4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
GLOBAL_C bool Melder_error5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5);
GLOBAL_C bool Melder_error6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6);
GLOBAL_C bool Melder_error7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
GLOBAL_C bool Melder_error8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
GLOBAL_C bool Melder_error9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
GLOBAL_C bool Melder_error10 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10);
GLOBAL_C bool Melder_error11 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11);
GLOBAL_C bool Melder_error12 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12);
GLOBAL_C bool Melder_error13 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13);
GLOBAL_C bool Melder_error14 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14);
GLOBAL_C bool Melder_error15 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15);
GLOBAL_C bool Melder_error16 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16);
GLOBAL_C bool Melder_error17 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17);
GLOBAL_C bool Melder_error18 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17,
	const wchar_t *s18);
GLOBAL_C bool Melder_error19 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17,
	const wchar_t *s18, const wchar_t *s19);
GLOBAL_C void * Melder_errorp1 (const wchar_t *s1);
GLOBAL_C void * Melder_errorp2 (const wchar_t *s1, const wchar_t *s2);
GLOBAL_C void * Melder_errorp3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
GLOBAL_C void * Melder_errorp4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
GLOBAL_C void * Melder_errorp5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5);
GLOBAL_C void * Melder_errorp6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6);
GLOBAL_C void * Melder_errorp7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
GLOBAL_C void * Melder_errorp8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
GLOBAL_C void * Melder_errorp9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
GLOBAL_C void * Melder_errorp10 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10);
GLOBAL_C void * Melder_errorp11 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11);
GLOBAL_C void * Melder_errorp12 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12);
GLOBAL_C void * Melder_errorp13 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13);
GLOBAL_C void * Melder_errorp14 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14);
GLOBAL_C void * Melder_errorp15 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15);
GLOBAL_C void * Melder_errorp16 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16);
GLOBAL_C void * Melder_errorp17 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17);
GLOBAL_C void * Melder_errorp18 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17,
	const wchar_t *s18);
GLOBAL_C void * Melder_errorp19 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17,
	const wchar_t *s18, const wchar_t *s19);
#define error1(s1)  { Melder_error1 (s1); goto end; }
#define error2(s1,s2)  { Melder_error2 (s1,s2); goto end; }
#define error3(s1,s2,s3)  { Melder_error3 (s1,s2,s3); goto end; }
#define error4(s1,s2,s3,s4)  { Melder_error4 (s1,s2,s3,s4); goto end; }
#define error5(s1,s2,s3,s4,s5)  { Melder_error5 (s1,s2,s3,s4,s5); goto end; }
#define error6(s1,s2,s3,s4,s5,s6)  { Melder_error6 (s1,s2,s3,s4,s5,s6); goto end; }
#define error7(s1,s2,s3,s4,s5,s6,s7)  { Melder_error7 (s1,s2,s3,s4,s5,s6,s7); goto end; }
#define error8(s1,s2,s3,s4,s5,s6,s7,s8)  { Melder_error8 (s1,s2,s3,s4,s5,s6,s7,s8); goto end; }
#define error9(s1,s2,s3,s4,s5,s6,s7,s8,s9)  { Melder_error9 (s1,s2,s3,s4,s5,s6,s7,s8,s9); goto end; }
#define error10(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10)  { Melder_error10 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10); goto end; }
#define error11(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11)  { Melder_error11 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11); goto end; }
#define error12(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12)  { Melder_error12 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12); goto end; }
#define error13(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13)  { Melder_error13 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13); goto end; }
#define error14(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14)  { Melder_error14 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14); goto end; }
#define error15(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15)  { Melder_error15 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15); goto end; }
#define error16(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16)  { Melder_error16 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16); goto end; }
#define error17(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,s17)  { Melder_error17 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,s17); goto end; }
#define error18(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,s17,s18)  { Melder_error18 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,s17,s18); goto end; }
#define error19(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,s17,s18,s19)  { Melder_error19 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,s17,s18,s19); goto end; }
	/* Generate, but do not yet show, an error message; return 0. */

GLOBAL_C void * Melder_errorp (const char *format, ...);
GLOBAL_C void * Melder_errorpW (const wchar_t *format, ...);
	/* Generate, but do not yet show, an error message, return NULL. */

GLOBAL_C void Melder_flushError (const char *format, ...);
GLOBAL_C void Melder_flushErrorW (const wchar_t *format, ...);
	/* Send all deferred error messages to stderr (batch) or to an "Error" dialog, */
	/* including, if 'format' is not NULL, the error message generated by this routine. */

GLOBAL_C int Melder_hasError (void);
	/* Returns 1 if there is an error message in store, otherwise 0. */
#define iferror  if (Melder_hasError ())
#define cherror  iferror goto end;

GLOBAL_C void Melder_clearError (void);
	/* Cancel all stored error messages. */

GLOBAL_C wchar_t * Melder_getError (void);
	/* Returns the error string. Mainly used with wcsstr. */

GLOBAL_C int Melder_fatal (const char *format, ...);
	/* Give error message, abort program. */
	/* Should only be caused by programming errors. */

#ifdef NDEBUG
	#define Melder_assert(x)   ((void) 0)
#else
	/*
	#define Melder_assert(x)   ((void) ((x) || _Melder_assert (#x, __FILE__, __LINE__)))
	*/
	#define Melder_assert(x)   ((x) ? (void) (0) : (void) _Melder_assert (#x, __FILE__, __LINE__))
#endif
GLOBAL_C int _Melder_assert (const char *condition, const char *fileName, int lineNumber);
	/* Call Melder_fatal with a message based on the following template: */
	/*    "Assertion failed in file <fileName> on line <lineNumber>: <condition>" */

/********** MEMORY ALLOCATION ROUTINES **********/

/* These routines call malloc, free, realloc, and calloc. */
/* If out of memory, they return NULL; the _e versions also queue an error message (like "Out of memory"). */
/* These routines also maintain a count of the total number of blocks allocated. */

GLOBAL_C void Melder_alloc_init (void);   // to be called around program start-up
GLOBAL_C void * _Melder_malloc_e (unsigned long size);
#define Melder_malloc_e(type,size)  (type *) _Melder_malloc_e ((size) * sizeof (type))
GLOBAL_C void * _Melder_malloc_f (unsigned long size);
#define Melder_malloc_f(type,size)  (type *) _Melder_malloc_f ((size) * sizeof (type))
GLOBAL_C void * Melder_realloc_e (void *pointer, long size);
GLOBAL_C void * Melder_realloc_f (void *pointer, long size);
GLOBAL_C void * _Melder_calloc_e (long numberOfElements, long elementSize);
#define Melder_calloc_e(type,size)  (type *) _Melder_calloc_e (size, sizeof (type))
GLOBAL_C void * _Melder_calloc_f (long numberOfElements, long elementSize);
#define Melder_calloc_f(type,size)  (type *) _Melder_calloc_f (size, sizeof (type))
GLOBAL_C char * Melder_strdup_e (const char *string);
GLOBAL_C char * Melder_strdup_f (const char *string);
GLOBAL_C wchar_t * Melder_wcsdup_e (const wchar_t *string);
GLOBAL_C wchar_t * Melder_wcsdup_f (const wchar_t *string);
GLOBAL_C int Melder_strcmp (const char *string1, const char *string2);   // regards null string as empty string
GLOBAL_C int Melder_wcscmp (const wchar_t *string1, const wchar_t *string2);   // regards null string as empty string
GLOBAL_C int Melder_strncmp (const char *string1, const char *string2, unsigned long n);
GLOBAL_C int Melder_wcsncmp (const wchar_t *string1, const wchar_t *string2, unsigned long n);
GLOBAL_C wchar_t * Melder_wcstok (wchar_t *string, const wchar_t *delimiter, wchar_t **last);   // circumvents platforms where wcstok has only two arguments
GLOBAL_C wchar_t * Melder_wcsdecompose (const wchar_t *string);
GLOBAL_C wchar_t * Melder_wcsprecompose (const wchar_t *string);
GLOBAL_C wchar_t * Melder_wcsExpandBackslashSequences (const wchar_t *string);
GLOBAL_C wchar_t * Melder_wcsReduceBackslashSequences (const wchar_t *string);
GLOBAL_C void Melder_wcsReduceBackslashSequences_inline (const wchar_t *string);

/*
 * Text encodings.
 */
GLOBAL_C void Melder_textEncoding_prefs (void);
GLOBAL_C void Melder_setInputEncoding (enum kMelder_textInputEncoding encoding);
GLOBAL_C int Melder_getInputEncoding (void);
GLOBAL_C void Melder_setOutputEncoding (enum kMelder_textOutputEncoding encoding);
GLOBAL_C int Melder_getOutputEncoding (void);

/*
 * Some other encodings. Although not used in the above set/get functions,
 * these constants should stay separate from the above encoding constants
 * because they occur in the same fields of struct MelderFile.
 */
#define kMelder_textInputEncoding_FLAC  0x464C4143
#define kMelder_textOutputEncoding_ASCII  0x41534349
#define kMelder_textOutputEncoding_ISO_LATIN1  0x4C415401
#define kMelder_textOutputEncoding_FLAC  0x464C4143

typedef uint16_t MelderUtf16;
typedef uint32_t MelderUtf32;

GLOBAL_C bool Melder_isValidAscii (const wchar_t *string);
GLOBAL_C bool Melder_strIsValidUtf8 (const char *string);
GLOBAL_C bool Melder_isEncodable (const wchar_t *string, int outputEncoding);
extern wchar_t Melder_decodeMacRoman [256];
extern wchar_t Melder_decodeWindowsLatin1 [256];

GLOBAL_C long Melder_killReturns_inlineW (wchar_t *text);

GLOBAL_C unsigned long wcslen_utf8 (const wchar_t *wcs, bool expandNewlines);
GLOBAL_C unsigned long wcslen_utf16 (const wchar_t *wcs, bool expandNewlines);
GLOBAL_C unsigned long wcslen_utf32 (const wchar_t *wcs, bool expandNewlines);

GLOBAL_C void Melder_8bitToWcs_inline_e (const char *string, wchar_t *wcs, int inputEncoding);
	// errors: Text is not valid UTF-8.
GLOBAL_C wchar_t * Melder_8bitToWcs_e (const char *string, int inputEncoding);
	// errors: Out of memory; Text is not valid UTF-8.
GLOBAL_C wchar_t * Melder_utf8ToWcs_e (const char *string);
	// errors: Out of memory; Text is not valid UTF-8.

GLOBAL_C void Melder_wcsToUtf8_inline (const wchar_t *wcs, char *utf8);
GLOBAL_C char * Melder_wcsToUtf8_e (const wchar_t *string);
	// errors: Out of memory.
GLOBAL_C void Melder_wcsTo8bitFileRepresentation_inline (const wchar_t *wcs, char *utf8);
GLOBAL_C void Melder_8bitFileRepresentationToWcs_inline (const char *utf8, wchar_t *wcs);
GLOBAL_C wchar_t * Melder_peekUtf8ToWcs (const char *string);
GLOBAL_C char * Melder_peekWcsToUtf8 (const wchar_t *string);
GLOBAL_C const MelderUtf16 * Melder_peekWcsToUtf16 (const wchar_t *string);
GLOBAL_C const void * Melder_peekWcsToCfstring (const wchar_t *string);
GLOBAL_C void Melder_fwriteWcsAsUtf8 (const wchar_t *ptr, size_t n, FILE *f);

/*
 * Some often used characters.
 */
#define L_LEFT_SINGLE_QUOTE  L"\u2018"
#define L_RIGHT_SINGLE_QUOTE  L"\u2019"
#define L_LEFT_DOUBLE_QUOTE  L"\u201c"
#define L_RIGHT_DOUBLE_QUOTE  L"\u201d"
#define L_LEFT_GUILLEMET  L"\u00ab"
#define L_RIGHT_GUILLEMET  L"\u00bb"

#define Melder_free(pointer)  _Melder_free ((void **) & (pointer))
GLOBAL_C void _Melder_free (void **pointer);
/*
	Preconditions:
		none (*pointer may be NULL).
	Postconditions:
		*pointer == NULL;
*/

GLOBAL_C double Melder_allocationCount (void);
/*
	Returns the total number of successful calls to
	Melder_malloc, Melder_realloc (if 'ptr' is NULL), Melder_calloc, and Melder_strdup,
	since the start of the process. Mainly for debugging purposes.
*/

GLOBAL_C double Melder_deallocationCount (void);
/*
	Returns the total number of successful calls to Melder_free,
	since the start of the process. Mainly for debugging purposes.
*/

GLOBAL_C double Melder_allocationSize (void);
/*
	Returns the total number of bytes allocated in calls to
	Melder_malloc, Melder_realloc (if moved), Melder_calloc, and Melder_strdup,
	since the start of the process. Mainly for debugging purposes.
*/

GLOBAL_C double Melder_reallocationsInSituCount (void);
GLOBAL_C double Melder_movingReallocationsCount (void);

/********** FILES **********/

#if defined (_WIN32)
	#define Melder_DIRECTORY_SEPARATOR  '\\'
#else
	#define Melder_DIRECTORY_SEPARATOR  '/'
#endif

struct FLAC__StreamDecoder;
struct FLAC__StreamEncoder;

typedef struct {
	FILE *filePointer;
	wchar_t path [260];
	bool openForReading, openForWriting, verbose, requiresCRLF;
	unsigned long outputEncoding;
	int indent;
	struct FLAC__StreamEncoder *flacEncoder;
} structMelderFile, *MelderFile;
typedef struct {
	wchar_t path [260];
} structMelderDir, *MelderDir;

/********** STRINGS **********/

/* These are routines for never having to check string boundaries again. */

typedef struct {
	unsigned long length;
	unsigned long bufferSize;
	wchar_t *string;   // a growing buffer, never shrunk (can only be freed by MelderString_free)
} MelderString;
typedef struct {
	unsigned long length;
	unsigned long bufferSize;
	MelderUtf16 *string;   // a growing buffer, never shrunk (can only be freed by MelderString16_free)
} MelderString16;

GLOBAL_C void MelderString_free (MelderString *me);   // frees the "string" attribute only (and sets other attributes to zero)
GLOBAL_C void MelderString16_free (MelderString16 *me);   // frees the "string" attribute only (and sets other attributes to zero)
GLOBAL_C void MelderString_empty (MelderString *me);   // sets to empty string (buffer not freed)
GLOBAL_C void MelderString16_empty (MelderString16 *me);   // sets to empty string (buffer not freed)
GLOBAL_C bool MelderString_copy (MelderString *me, const wchar_t *source);
GLOBAL_C bool MelderString_ncopy (MelderString *me, const wchar_t *source, unsigned long n);
GLOBAL_C bool MelderString_append (MelderString *me, const wchar_t *source);
GLOBAL_C bool MelderString_append1 (MelderString *me, const wchar_t *s1);   // Identical to MelderString_append.
GLOBAL_C bool MelderString_append2 (MelderString *me, const wchar_t *s1, const wchar_t *s2);
GLOBAL_C bool MelderString_append3 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
GLOBAL_C bool MelderString_append4 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
GLOBAL_C bool MelderString_append5 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5);
GLOBAL_C bool MelderString_append6 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6);
GLOBAL_C bool MelderString_append7 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
GLOBAL_C bool MelderString_append8 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
GLOBAL_C bool MelderString_append9 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
GLOBAL_C bool MelderString_appendCharacter (MelderString *me, wchar_t character);
GLOBAL_C bool MelderString16_appendCharacter (MelderString16 *me, wchar_t character);
GLOBAL_C bool MelderString_get (MelderString *me, wchar_t *destination);   // performs no boundary checking
GLOBAL_C double MelderString_allocationCount (void);
GLOBAL_C double MelderString_deallocationCount (void);
GLOBAL_C double MelderString_allocationSize (void);
GLOBAL_C double MelderString_deallocationSize (void);

typedef struct structMelderReadText *MelderReadText;
GLOBAL_C MelderReadText MelderReadText_createFromFile (MelderFile file);
GLOBAL_C MelderReadText MelderReadText_createFromString (const wchar_t *string);
GLOBAL_C bool MelderReadText_isValid (MelderReadText text);
GLOBAL_C wchar_t MelderReadText_getChar (MelderReadText text);   // precondition: isValid
GLOBAL_C wchar_t * MelderReadText_readLine (MelderReadText text);
GLOBAL_C long MelderReadText_getNumberOfLines (MelderReadText me);
GLOBAL_C const wchar_t * MelderReadText_getLineNumber (MelderReadText text);
GLOBAL_C void MelderReadText_delete (MelderReadText text);

GLOBAL_C const wchar_t * Melder_wcscat2 (const wchar_t *s1, const wchar_t *s2);
GLOBAL_C const wchar_t * Melder_wcscat3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
GLOBAL_C const wchar_t * Melder_wcscat4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
GLOBAL_C const wchar_t * Melder_wcscat5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
GLOBAL_C const wchar_t * Melder_wcscat6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
GLOBAL_C const wchar_t * Melder_wcscat7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
GLOBAL_C const wchar_t * Melder_wcscat8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
GLOBAL_C const wchar_t * Melder_wcscat9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);

/********** NUMBER AND STRING COMPARISON **********/

GLOBAL_C int Melder_numberMatchesCriterion (double value, int which_kMelder_number, double criterion);
GLOBAL_C int Melder_stringMatchesCriterion (const wchar_t *value, int which_kMelder_string, const wchar_t *criterion);

/********** STRING PARSING **********/

/*
	These functions regard a string as a sequence of tokens,
	separated (and perhaps preceded and followed) by white space.
	The tokens cannot contain spaces themselves (there are no escapes).
	Typical use:
		for (token = Melder_firstToken (string); token != NULL; token = Melder_nextToken ()) {
			... do something with the token ...
		}
*/

GLOBAL_C long Melder_countTokens (const wchar_t *string);
GLOBAL_C wchar_t *Melder_firstToken (const wchar_t *string);
GLOBAL_C wchar_t *Melder_nextToken (void);
GLOBAL_C wchar_t ** Melder_getTokens (const wchar_t *string, long *n);
GLOBAL_C void Melder_freeTokens (wchar_t ***tokens);
GLOBAL_C long Melder_searchToken (const wchar_t *string, wchar_t **tokens, long n);

/********** MESSAGING ROUTINES **********/

/* These functions are called like printf ().
	Default Melder does fprintf to stderr,
	except Melder_information, which does fprintf to stdout.
	These functions show generic, native, and mixed strings correctly,
	and perform quote conversion, if that flag is not off; see under "NON-ASCII CHARACTERS".
	The alphabet is Roman, so that symbols from the Symbol and Phonetic alphabets
	are not translated (by default, \mu is shown as \mu and \as as a).
*/

GLOBAL_C void Melder_casual (const char *format, ...);
GLOBAL_C void Melder_casual1 (const wchar_t *s1);
GLOBAL_C void Melder_casual2 (const wchar_t *s1, const wchar_t *s2);
GLOBAL_C void Melder_casual3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
GLOBAL_C void Melder_casual4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
GLOBAL_C void Melder_casual5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
GLOBAL_C void Melder_casual6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
GLOBAL_C void Melder_casual7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
GLOBAL_C void Melder_casual8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
GLOBAL_C void Melder_casual9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
/*
	Function:
		Sends a message without user interference.
	Behaviour:
		Writes to stderr on Unix, otherwise to a special window.
*/

/* Give information to stdout (batch), or to an "Info" window (interactive), or to a diverted string. */

GLOBAL_C void MelderInfo_open (void);   /* Clear the Info window in the background. */
GLOBAL_C void MelderInfo_write1 (const wchar_t *s1);   /* Write a string to the Info window in the background. */
GLOBAL_C void MelderInfo_write2 (const wchar_t *s1, const wchar_t *s2);   /* Write two strings to the Info window in the background. */
GLOBAL_C void MelderInfo_write3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
GLOBAL_C void MelderInfo_write4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
GLOBAL_C void MelderInfo_write5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
GLOBAL_C void MelderInfo_write6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
GLOBAL_C void MelderInfo_write7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
GLOBAL_C void MelderInfo_write8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
GLOBAL_C void MelderInfo_write9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
GLOBAL_C void MelderInfo_writeLine1 (const wchar_t *s1);   /* Write a string to the Info window in the background; add a new-line. */
GLOBAL_C void MelderInfo_writeLine2 (const wchar_t *s1, const wchar_t *s2);
GLOBAL_C void MelderInfo_writeLine3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
GLOBAL_C void MelderInfo_writeLine4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
GLOBAL_C void MelderInfo_writeLine5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
GLOBAL_C void MelderInfo_writeLine6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
GLOBAL_C void MelderInfo_writeLine7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
GLOBAL_C void MelderInfo_writeLine8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
GLOBAL_C void MelderInfo_writeLine9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
GLOBAL_C void MelderInfo_close (void);   /* Flush the background info to the Info window. */

GLOBAL_C void Melder_information1 (const wchar_t *s1);
GLOBAL_C void Melder_information2 (const wchar_t *s1, const wchar_t *s2);
GLOBAL_C void Melder_information3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
GLOBAL_C void Melder_information4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
GLOBAL_C void Melder_information5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
GLOBAL_C void Melder_information6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
GLOBAL_C void Melder_information7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
GLOBAL_C void Melder_information8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
GLOBAL_C void Melder_information9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);

GLOBAL_C void Melder_informationReal (double value, const wchar_t *units);   /* %.17g or --undefined--; units may be NULL */

GLOBAL_C void Melder_divertInfo (MelderString *buffer);   /* NULL = back to normal. */

GLOBAL_C void Melder_print (const wchar_t *s);
	/* Write formatted text to the Info window without clearing it, and without adding a new-line symbol at the end. */

GLOBAL_C void Melder_clearInfo (void);   /* Clear the Info window. */
GLOBAL_C wchar_t * Melder_getInfo (void);
GLOBAL_C void Melder_help (const wchar_t *query);

GLOBAL_C void Melder_search (void);

/* Give warning to stderr (batch) or to a "Warning" dialog. */
GLOBAL_C void Melder_warning1 (const wchar_t *s1);
GLOBAL_C void Melder_warning2 (const wchar_t *s1, const wchar_t *s2);
GLOBAL_C void Melder_warning3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
GLOBAL_C void Melder_warning4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
GLOBAL_C void Melder_warning5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
GLOBAL_C void Melder_warning6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
GLOBAL_C void Melder_warning7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
GLOBAL_C void Melder_warning8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
GLOBAL_C void Melder_warning9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
GLOBAL_C void Melder_warningOff (void);
GLOBAL_C void Melder_warningOn (void);
	
GLOBAL_C void Melder_beep (void);

extern int Melder_debug;

/********** PROGRESS ROUTINES **********/

GLOBAL_C int Melder_progress1 (double progress, const wchar_t *s1);
GLOBAL_C int Melder_progress2 (double progress, const wchar_t *s1, const wchar_t *s2);
GLOBAL_C int Melder_progress3 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
GLOBAL_C int Melder_progress4 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
GLOBAL_C int Melder_progress5 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
GLOBAL_C int Melder_progress6 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
GLOBAL_C int Melder_progress7 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
GLOBAL_C int Melder_progress8 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
GLOBAL_C int Melder_progress9 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
GLOBAL_C void Melder_progressOff (void);
GLOBAL_C void Melder_progressOn (void);
/*
	Function:
		Show the progress of a time-consuming process.
	Arguments:
		Any of 's1' through 's9' may be NULL.
	Batch behaviour:
		Does nothing, always returns 1.
	Interactive behaviour:
		Shows the progress of a time-consuming process:
		- if 'progress' <= 0.0, show a window with text and a Cancel button, and return 1;
		- if 0.0 < 'progress' < 1.0, show text and a partially filled progress bar,
		  and return 0 if user interrupts, else return 1;
		- if 'progress' >= 1, hide the window.
	Usage:
		- call with 'progress' = 0.0 before the process starts:
			  (void) Melder_progress (0.0, "Starting work...");
		- at every turn in your loop, call with 'progress' between 0 and 1,
		  and check the return value to see if the user clicked the Cancel button:
			  if (! Melder_progress5 (i / (n + 1.0), L"Working on part ", Melder_integer (i), L" out of ", Melder_integer (n), L"...")) {
				  forget (me);   // Clean up.
				  return Melder_errorp1 (L"Work interrupted, not finished");   // Interrupt.
			  }
		- after the process has finished, call with 'progress' = 1.0:
			  (void) Melder_progress1 (1.0, NULL);
*/
	
GLOBAL_C void * Melder_monitor1 (double progress, const wchar_t *s1);
GLOBAL_C void * Melder_monitor2 (double progress, const wchar_t *s1, const wchar_t *s2);
GLOBAL_C void * Melder_monitor3 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
GLOBAL_C void * Melder_monitor4 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
GLOBAL_C void * Melder_monitor5 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
GLOBAL_C void * Melder_monitor6 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
GLOBAL_C void * Melder_monitor7 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
GLOBAL_C void * Melder_monitor8 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
GLOBAL_C void * Melder_monitor9 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
/*
	Function:
		Show the progress of a time-consuming process.
	Arguments:
		Any of 's1' through 's9' may be NULL.
	Batch behaviour:
		Does nothing, returns NULL if 'progress' <= 0.0 and a non-NULL pointer otherwise.
	Interactive behaviour:
		Shows the progress of a time-consuming process:
		- if 'progress' <= 0.0, show a window with text and a Cancel button and
		  room for a square drawing, and return a Graphics;
		- if 0.0 < 'progress' < 1.0, show text and a partially filled progress bar,
		  and return NULL if user interrupts, else return a non-NULL pointer;
		- if 'progress' >= 1, hide the window.
	Usage:
		- call with 'progress' = 0.0 before the process starts.
		- assign the return value to a Graphics:
			Graphics graphics = Melder_monitor1 (0.0, L"Starting work...");
		- at every turn of your loop, draw something in the Graphics:
			if (graphics) {   // Always check; might be batch.
				Graphics_clearWs (graphics);   // Only if you redraw all every time.
				Graphics_polyline (graphics, ...);
				Graphics_text1 (graphics, ...);
			}
		- immediately after this in your loop, call with 'progress' between 0 and 1,
		  and check the return value to see if the user clicked the Cancel button:
			if (! Melder_monitor5 (i / (n + 1.0), L"Working on part ", Melder_integer (i), L" out of ", Melder_integer (n), L"...")) {
				forget (me);   // Clean up.
				return Melder_errorp1 (L"Work interrupted, not finished");   // Interrupt.
			}
		- after the process has finished, call with 'progress' = 1.0:
			(void) Melder_monitor1 (1.0, NULL);
*/

/********** RECORD AND PLAY ROUTINES **********/

GLOBAL_C int Melder_publish (void *anything);

GLOBAL_C int Melder_record (double duration);
GLOBAL_C int Melder_recordFromFile (MelderFile fs);
GLOBAL_C void Melder_play (void);
GLOBAL_C void Melder_playReverse (void);
GLOBAL_C int Melder_publishPlayed (void);

/********** SYSTEM VERSION **********/

extern unsigned long Melder_systemVersion;
/*
	For Macintosh, this is set in the Motif emulator.
*/

/********** SCRATCH TEXT BUFFERS **********/

extern char Melder_buffer1 [30001], Melder_buffer2 [30001];
/*
	Every Melder routine uses both of these buffers:
	one for sprintfing the message,
	and one for translating this message to a native string.
	You can use these buffers,
	but be careful not to call any other routines that use them at the same time;
	the following routines are guaranteed not to call the Melder library:
	 - Mac Toolbox, XWindows, X Toolkit, Motif, and XVT routines,
		except those who dispatch events (XtDispatchEvent, xvt_process_pending_events).
	 - Longchar_*
	This means that you can use these buffers for reading and writing with
	the Longchar library.
*/

/********** ENFORCE INTERACTIVE BEHAVIOUR **********/

/* Procedures to enforce interactive behaviour of the Melder_XXXXXX routines. */

GLOBAL_C void MelderGui_create (/* XtAppContext* */ void *appContext, /* GuiObject */ void *parent);
/*
	'appContext' is the XtAppContext* output from Xt(Va)AppInitialize;
		if you used Xt(Va)Initialize it should be NULL.
	'parent' is the top-level widget returned by Xt(Va)(App)Initialize.
*/

extern int Melder_batch;   /* True if run from the batch or from an interactive command-line interface. */
extern int Melder_backgrounding;   /* True if running a script. */
extern bool Melder_consoleIsAnsi;
#ifndef CONTROL_APPLICATION
	extern void *Melder_appContext;   /* XtAppContext* */
	extern void *Melder_topShell;   /* GuiObject */
#endif

/********** OVERRIDE DEFAULT BEHAVIOUR **********/

/* Procedures to override default message methods. */
/* They may chage the string arguments. */
/* Many of these routines are called by MelderMotif_create and MelderXvt_create. */

GLOBAL_C void Melder_setCasualProc (void (*casualProc) (wchar_t *message));
GLOBAL_C void Melder_setProgressProc (int (*progressProc) (double progress, wchar_t *message));
GLOBAL_C void Melder_setMonitorProc (void * (*monitorProc) (double progress, wchar_t *message));
GLOBAL_C void Melder_setInformationProc (void (*informationProc) (wchar_t *message));
GLOBAL_C void Melder_setHelpProc (void (*help) (const wchar_t *query));
GLOBAL_C void Melder_setSearchProc (void (*search) (void));
GLOBAL_C void Melder_setWarningProc (void (*warningProc) (wchar_t *message));
GLOBAL_C void Melder_setErrorProc (void (*errorProc) (wchar_t *message));
GLOBAL_C void Melder_setFatalProc (void (*fatalProc) (wchar_t *message));
GLOBAL_C void Melder_setPublishProc (int (*publish) (void *));
GLOBAL_C void Melder_setRecordProc (int (*record) (double));
GLOBAL_C void Melder_setRecordFromFileProc (int (*recordFromFile) (MelderFile));
GLOBAL_C void Melder_setPlayProc (void (*play) (void));
GLOBAL_C void Melder_setPlayReverseProc (void (*playReverse) (void));
GLOBAL_C void Melder_setPublishPlayedProc (int (*publishPlayed) (void));

/********** FILE ROUTINES **********/

#if defined (macintosh)
	GLOBAL_C void Melder_machToFile (void *void_fsref, MelderFile file);
	GLOBAL_C void Melder_machToDir (void *void_fsref, MelderDir dir);
	GLOBAL_C int Melder_fileToMach (MelderFile file, void *void_fsref);
	GLOBAL_C int Melder_dirToMach (MelderDir dir, void *void_fsref);
#endif
GLOBAL_C wchar_t * MelderFile_name (MelderFile file);
GLOBAL_C wchar_t * MelderDir_name (MelderDir dir);
GLOBAL_C int Melder_pathToDir (const wchar_t *path, MelderDir dir);
GLOBAL_C int Melder_pathToFile (const wchar_t *path, MelderFile file);
GLOBAL_C int Melder_relativePathToFile (const wchar_t *path, MelderFile file);
GLOBAL_C wchar_t * Melder_dirToPath (MelderDir dir);
	/* Returns a pointer internal to 'dir', like "/u/paul/praats" or "D:\Paul\Praats" */
GLOBAL_C wchar_t * Melder_fileToPath (MelderFile file);
GLOBAL_C void MelderFile_copy (MelderFile file, MelderFile copy);
GLOBAL_C void MelderDir_copy (MelderDir dir, MelderDir copy);
GLOBAL_C int MelderFile_equal (MelderFile file1, MelderFile file2);
GLOBAL_C int MelderDir_equal (MelderDir dir1, MelderDir dir2);
GLOBAL_C void MelderFile_setToNull (MelderFile file);
GLOBAL_C int MelderFile_isNull (MelderFile file);
GLOBAL_C void MelderDir_setToNull (MelderDir dir);
GLOBAL_C int MelderDir_isNull (MelderDir dir);
GLOBAL_C void MelderDir_getFile (MelderDir parent, const wchar_t *fileName, MelderFile file);
GLOBAL_C void MelderDir_relativePathToFile (MelderDir dir, const wchar_t *path, MelderFile file);
GLOBAL_C void MelderFile_getParentDir (MelderFile file, MelderDir parent);
GLOBAL_C void MelderDir_getParentDir (MelderDir file, MelderDir parent);
GLOBAL_C int MelderDir_isDesktop (MelderDir dir);
GLOBAL_C int MelderDir_getSubdir (MelderDir parent, const wchar_t *subdirName, MelderDir subdir);
GLOBAL_C void Melder_rememberShellDirectory (void);
GLOBAL_C wchar_t * Melder_getShellDirectory (void);
GLOBAL_C void Melder_getHomeDir (MelderDir homeDir);
GLOBAL_C void Melder_getPrefDir (MelderDir prefDir);
GLOBAL_C void Melder_getTempDir (MelderDir tempDir);

GLOBAL_C int MelderFile_exists (MelderFile file);
GLOBAL_C int MelderFile_readable (MelderFile file);
GLOBAL_C long MelderFile_length (MelderFile file);
GLOBAL_C int MelderFile_delete (MelderFile file);

/* The following two should be combined with each other and with Windows extension setting: */
GLOBAL_C FILE * Melder_fopen (MelderFile file, const char *type);
#if defined (macintosh)
	GLOBAL_C void MelderFile_setMacTypeAndCreator (MelderFile file, long fileType, long creator);
	GLOBAL_C unsigned long MelderFile_getMacType (MelderFile file);
#else
	#define MelderFile_setMacTypeAndCreator(f,t,c)  (void) 0
#endif
GLOBAL_C int Melder_fclose (MelderFile file, FILE *stream);
GLOBAL_C void Melder_files_cleanUp (void);

/* So these will be the future replacements for the above, as soon as we rid of text files: */
GLOBAL_C void MelderFile_open (MelderFile file);
GLOBAL_C void MelderFile_append (MelderFile file);
GLOBAL_C void MelderFile_create (MelderFile file, const wchar_t *macType, const wchar_t *macCreator, const wchar_t *winExtension);
GLOBAL_C void * MelderFile_read (MelderFile file, long nbytes);
GLOBAL_C char * MelderFile_readLine (MelderFile file);
GLOBAL_C void MelderFile_writeCharacter (MelderFile file, wchar_t kar);
GLOBAL_C void MelderFile_write1 (MelderFile file, const wchar_t *s1);
GLOBAL_C void MelderFile_write2 (MelderFile file, const wchar_t *s1, const wchar_t *s2);
GLOBAL_C void MelderFile_write3 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
GLOBAL_C void MelderFile_write4 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
GLOBAL_C void MelderFile_write5 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
GLOBAL_C void MelderFile_write6 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
GLOBAL_C void MelderFile_write7 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
GLOBAL_C void MelderFile_write8 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
GLOBAL_C void MelderFile_write9 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
GLOBAL_C void MelderFile_rewind (MelderFile file);
GLOBAL_C void MelderFile_seek (MelderFile file, long position, int direction);
GLOBAL_C long MelderFile_tell (MelderFile file);
GLOBAL_C void MelderFile_close (MelderFile file);
/* If one of these routines fails, it closes the file and generates an error. */
/* It is often sufficient to call cherror only before other tests that could */
/* result in Melder_error (), and to call iferror at the end. */

/* Read and write whole text files. */
GLOBAL_C wchar_t * MelderFile_readText (MelderFile file);
GLOBAL_C int MelderFile_writeText (MelderFile file, const wchar_t *text);
GLOBAL_C int MelderFile_appendText (MelderFile file, const wchar_t *text);

GLOBAL_C int Melder_createDirectory (MelderDir parent, const wchar_t *subdirName, int mode);

GLOBAL_C void Melder_getDefaultDir (MelderDir dir);
GLOBAL_C void Melder_setDefaultDir (MelderDir dir);
GLOBAL_C void MelderFile_setDefaultDir (MelderFile file);

/* Use the following functions to pass unchanged text or file names to Melder_* functions. */
/* Backslashes are replaced by "\bs". */
/* The trick is that they return one of 11 cyclically used static strings, */
/* so you can use up to 11 strings in a single Melder_* call. */
GLOBAL_C wchar_t * Melder_peekExpandBackslashes (const wchar_t *message);
GLOBAL_C wchar_t * MelderFile_messageName (MelderFile file);   // Calls Melder_peekExpandBackslashes ().

GLOBAL_C double Melder_stopwatch (void);

GLOBAL_C long Melder_killReturns_inline (char *text);
/*
	 Replaces all bare returns (Mac) or return / linefeed sequences (Win) with bare linefeeds (generic = Unix).
	 Returns new length of string (equal to or less than old length).
*/

/********** AUDIO **********/

#if defined (macintosh) || defined (_WIN32) || defined (linux)
	#define kMelderAudio_inputUsesPortAudio_DEFAULT  true
		// Mac: in order to have CoreAudio (so that rogue applications cannot attack our sample rate anymore)
		// Win: in order to allow recording for over 64 megabytes (paMME)
		// Linux: in order to use ALSA and therefore be compatible with Ubuntu 10.10 and later
#else
	#define kMelderAudio_inputUsesPortAudio_DEFAULT  false
#endif
GLOBAL_C void MelderAudio_setInputUsesPortAudio (bool inputUsesPortAudio);
GLOBAL_C bool MelderAudio_getInputUsesPortAudio (void);
#if defined (macintosh) || defined (linux)
	#define kMelderAudio_outputUsesPortAudio_DEFAULT  true
		// Mac: in order to have CoreAudio (so that rogue applications cannot attack our sample rate anymore)
		// Linux: in order to use ALSA and therefore be compatible with Ubuntu 10.10 and later
#else
	#define kMelderAudio_outputUsesPortAudio_DEFAULT  false
		// Win: in order to reduce the long latencies of paMME and to avoid the incomplete implementation of paDirectSound
#endif
GLOBAL_C void MelderAudio_setOutputUsesPortAudio (bool outputUsesPortAudio);
GLOBAL_C bool MelderAudio_getOutputUsesPortAudio (void);
#if 1
	#define kMelderAudio_outputSilenceBefore_DEFAULT  0.0
		// Mac: in order to switch off the BOING caused by the automatic gain control
#endif
GLOBAL_C void MelderAudio_setOutputSilenceBefore (double silenceBefore);
GLOBAL_C double MelderAudio_getOutputSilenceBefore (void);
#if defined (macintosh)
	#define kMelderAudio_outputSilenceAfter_DEFAULT  0.0
		// Mac: in order to reduce the BOING caused by the automatic gain control when the user replays immediately after a sound has finished
#else
	#define kMelderAudio_outputSilenceAfter_DEFAULT  0.5
		// Win: in order to get rid of the click on some cards
		// Linux: in order to get rid of double playing of a sounding buffer
#endif
GLOBAL_C void MelderAudio_setOutputSilenceAfter (double silenceAfter);
GLOBAL_C double MelderAudio_getOutputSilenceAfter (void);
GLOBAL_C void MelderAudio_setOutputUsesBlocking (bool outputUsesBlocking);
GLOBAL_C bool MelderAudio_getOutputUsesBlocking (void);
GLOBAL_C void MelderAudio_setUseInternalSpeaker (bool useInternalSpeaker);   // for HP-UX and Sun
GLOBAL_C bool MelderAudio_getUseInternalSpeaker (void);
GLOBAL_C void MelderAudio_setOutputMaximumAsynchronicity (enum kMelder_asynchronicityLevel maximumAsynchronicity);
GLOBAL_C enum kMelder_asynchronicityLevel MelderAudio_getOutputMaximumAsynchronicity (void);
GLOBAL_C long MelderAudio_getOutputBestSampleRate (long fsamp);

extern int MelderAudio_isPlaying;
GLOBAL_C int MelderAudio_play16 (const short *buffer, long sampleRate, long numberOfSamples, int numberOfChannels,
	int (*playCallback) (void *playClosure, long numberOfSamplesPlayed), void *playClosure);
GLOBAL_C int MelderAudio_stopPlaying (bool isExplicit);   /* Returns 1 if sound was playing. */
#define MelderAudio_IMPLICIT  false
#define MelderAudio_EXPLICIT  true
GLOBAL_C long MelderAudio_getSamplesPlayed (void);
GLOBAL_C bool MelderAudio_stopWasExplicit (void);

GLOBAL_C void Melder_audio_prefs (void);   // in init file

/********** AUDIO FILES **********/

/* Audio file types. */
#define Melder_AIFF  1
#define Melder_AIFC  2
#define Melder_WAV  3
#define Melder_NEXT_SUN  4
#define Melder_NIST  5
#define Melder_SOUND_DESIGNER_TWO  6
#define Melder_FLAC 7
#define Melder_MP3 8
#define Melder_NUMBER_OF_AUDIO_FILE_TYPES  8
GLOBAL_C wchar_t * Melder_audioFileTypeString (int audioFileType);   /* "AIFF", "AIFC", "WAV", "NeXT/Sun", "NIST", "Sound Designer II", "FLAC", "MP3" */
GLOBAL_C wchar_t * Melder_macAudioFileType (int audioFileType);   /* "AIFF", "AIFC", "WAVE", "ULAW", "NIST", "Sd2f", "FLAC", "MP3" */
GLOBAL_C wchar_t * Melder_winAudioFileExtension (int audioFileType);   /* ".aiff", ".aifc", ".wav", ".au", ".nist", ".sd2", ".flac", ".mp3" */
/* Audio encodings. */
#define Melder_LINEAR_8_SIGNED  1
#define Melder_LINEAR_8_UNSIGNED  2
#define Melder_LINEAR_16_BIG_ENDIAN  3
#define Melder_LINEAR_16_LITTLE_ENDIAN  4
#define Melder_LINEAR_24_BIG_ENDIAN  5
#define Melder_LINEAR_24_LITTLE_ENDIAN  6
#define Melder_LINEAR_32_BIG_ENDIAN  7
#define Melder_LINEAR_32_LITTLE_ENDIAN  8
#define Melder_MULAW  9
#define Melder_ALAW  10
#define Melder_SHORTEN  11
#define Melder_POLYPHONE  12
#define Melder_IEEE_FLOAT_32_BIG_ENDIAN  13
#define Melder_IEEE_FLOAT_32_LITTLE_ENDIAN  14
#define Melder_FLAC_COMPRESSION 15
#define Melder_MPEG_COMPRESSION 16
GLOBAL_C int Melder_defaultAudioFileEncoding16 (int audioFileType);   /* BIG_ENDIAN, BIG_ENDIAN, LITTLE_ENDIAN, BIG_ENDIAN, LITTLE_ENDIAN, BIG_ENDIAN */
GLOBAL_C void MelderFile_writeAudioFileHeader16_e (MelderFile file, int audioFileType, long sampleRate, long numberOfSamples, int numberOfChannels);
GLOBAL_C int MelderFile_writeAudioFile16 (MelderFile file, int audioFileType, const short *buffer, long sampleRate, long numberOfSamples, int numberOfChannels);

GLOBAL_C int MelderFile_checkSoundFile (MelderFile file, int *numberOfChannels, int *encoding,
	double *sampleRate, long *startOfData, long *numberOfSamples);
/* Returns information about a just opened audio file.
 * The return value is the audio file type, or 0 if it is not a sound file or in case of error.
 * The data start at 'startOfData' bytes from the start of the file.
 */
GLOBAL_C int Melder_bytesPerSamplePoint (int encoding);
GLOBAL_C int Melder_readAudioToFloat (FILE *f, int numberOfChannels, int encoding, double **buffer, long numberOfSamples);
/* Reads channels into buffer [ichannel], which are base-1.
 */
GLOBAL_C int Melder_readAudioToShort (FILE *f, int numberOfChannels, int encoding, short *buffer, long numberOfSamples);
/* If stereo, buffer will contain alternating left and right values.
 * Buffer is base-0.
 */
GLOBAL_C int MelderFile_writeFloatToAudio (MelderFile file, int numberOfChannels, int encoding, double **buffer, long numberOfSamples, int warnIfClipped);
GLOBAL_C int MelderFile_writeShortToAudio (MelderFile file, int numberOfChannels, int encoding, const short *buffer, long numberOfSamples);

GLOBAL_C void Melder_audioTrigger (void);

/********** QUANTITY **********/

#define MelderQuantity_NONE  0
#define MelderQuantity_TIME_SECONDS  1
#define MelderQuantity_FREQUENCY_HERTZ  2
#define MelderQuantity_FREQUENCY_BARK  3
#define MelderQuantity_DISTANCE_FROM_GLOTTIS_METRES  4
#define MelderQuantity_NUMBER_OF_QUANTITIES  4
GLOBAL_C const wchar_t * MelderQuantity_getText (int quantity);   // e.g. "Time"
GLOBAL_C const wchar_t * MelderQuantity_getWithUnitText (int quantity);   // e.g. "Time (s)"
GLOBAL_C const wchar_t * MelderQuantity_getLongUnitText (int quantity);   // e.g. "seconds"
GLOBAL_C const wchar_t * MelderQuantity_getShortUnitText (int quantity);   // e.g. "s"

/********** MISCELLANEOUS **********/

GLOBAL_C wchar_t * Melder_getenv (const wchar_t *variableName);
GLOBAL_C int Melder_system (const wchar_t *command);   // spawn a system command; return 0 if error
GLOBAL_C double Melder_clock (void);   // seconds since 1969

/* End of file melder.h */
#endif
