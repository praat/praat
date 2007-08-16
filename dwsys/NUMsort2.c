/* NUMsort.c
 *
 * Copyright (C) 1993-2003 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20030121 Initial version
 djmw 20030627 Removed bug in MACRO_NUMindex
*/

#include "NUM2.h"
#include "melder.h"

/*
	NUMsort2 uses heapsort to sort the second array in parallel with the first one.

	Algorithm follows p. 145 and 642 in:
	Donald E. Knuth (1998): The art of computer programming. Third edition. Vol. 3: sorting and searching.
		Boston: Addison-Wesley, printed may 2002.
	Modification: there is no distinction between record and key and 
	    Floyd's optimization (page 642) is used.
*/

#define MACRO_NUMsort2(TYPE,TYPE2) \
{ \
	long l, r, j, i, imin; \
	TYPE k, min; \
	TYPE2 kb, min2; \
	if (n < 2) return;   /* Already sorted. */ \
	if (n == 2) \
	{ \
		if (a[1] > a[2]) \
		{\
			min = a[2]; a[2] = a[1]; a[1] = min; \
			min2 = b[2]; b[2] = b[1]; b[1] = min2; \
		} \
		return; \
	} \
	if (n <= 12) \
	{ \
		for (i = 1; i < n; i++) \
		{ \
			min = a[i]; \
			imin = i; \
			for (j = i + 1; j <= n; j++) \
			{\
				if (a[j] < min)\
				{ \
					min = a [j]; \
					imin = j; \
				} \
			} \
			a[imin] = a[i]; a[i] = min; \
			min2 = b[imin]; b[imin] = b[i]; b[i] = min2; \
		} \
		return; \
	} \
	/* H1 */\
	l = (n >> 1) + 1; \
	r = n; \
	for (;;) /* H2 */\
	{ \
		if (l > 1) { \
			l--; \
			k = a[l]; kb = b[l]; \
		} else /* l == 1 */ { \
			k = a[r]; kb = b[r]; \
			a[r] = a[1]; b[r] = b[1]; \
			r--; \
			if (r == 1) \
			{ \
				a[1] = k; b[1] = kb; return; \
			} \
		} \
		/* H3 */ \
		j = l; \
		for (;;) { \
			/* H4 */ \
			i = j; \
			j = j << 1; \
			if (j > r) break; \
			if (j < r && a[j] < a[j + 1]) j++; /* H5 */\
			/* if (k >= a[j]) break; H6 */\
			a[i] = a[j]; b[i] = b[j]; /* H7 */\
		} \
		/* a[i] = k; b[i] = kb; H8 */\
		for (;;)  /*H8' */\
		{\
			j = i; \
			i = j >> 1; \
			/* H9' */ \
			if (j == l || k <= a[i]) \
			{ \
				a[j] = k; b[j] = kb; break; \
			} \
			a[j] = a[i]; b[j] = b[i]; \
		}\
	} \
}

void NUMsort2_fl (long n, float a[], long b[])
	MACRO_NUMsort2 (float, long)

void NUMsort2_ff (long n, float a[], float b[])
	MACRO_NUMsort2 (float, float)

void NUMsort2_dl (long n, double a[], long b[])
	MACRO_NUMsort2 (double, long)

void NUMsort2_dd (long n, double a[], double b[])
	MACRO_NUMsort2 (double, double)

void NUMsort2_ll (long n, long a[], long b[])
	MACRO_NUMsort2 (long, long)

#undef MACRO_NUMsort2

#define MACRO_NUMrank(TYPE)	\
{ \
	TYPE rank; \
	long i, jt, j = 1; \
	while (j < n) \
	{ \
		for (jt = j + 1; jt <= n && a[jt] == a[j]; jt++) {} \
		rank = (j + jt - 1) * 0.5; \
		for (i = j; i <= jt-1; i++) \
		{ \
			a[i] = rank; \
		} \
		j = jt;	\
	} \
	if (j == n) a[n] = n;	\
}

void NUMrank_f (long n, float a[])
	MACRO_NUMrank(float)

void NUMrank_d (long n, double a[])
	MACRO_NUMrank(double)

#undef MACRO_NUMrank

#define MACRO_NUMrankcolumns(TYPE,TS)			\
{	\
	long i, j, nr = re - rb + 1, *index = NULL;	\
	TYPE *v = NULL;	\
	\
	v = NUM##TS##vector (1, nr);	\
	if (v == NULL) return 0;	\
	index = NUMlvector (1, nr);	\
	if (index == NULL) goto end;	\
\
	for (j = cb; j <= ce; j++)	\
	{	\
		for (i = 1; i <= nr; i++) v[i] = m[rb + i - 1][j]; \
		for (i = 1; i <= nr; i++) index[i] = i;	\
		NUMsort2_##TS##l (nr, v, index);	\
		NUMrank_##TS (nr, v);	\
		for (i = 1; i <= nr; i++) m[rb + index[i] - 1][j] = v[i];	\
	}	\
end:	\
	NUM##TS##vector_free (v, 1);	\
	NUMlvector_free (index, 1);	\
	return ! Melder_hasError();	\
}

int NUMrankColumns_f (float **m, long rb, long re, long cb, long ce)
	MACRO_NUMrankcolumns(float,f)

int NUMrankColumns_d (double **m, long rb, long re, long cb, long ce)
	MACRO_NUMrankcolumns(double,d)

#undef MACRO_NUMrankcolumns

#define MACRO_NUMindex(TYPE) \
{ \
	long l, r, j, i, ii, k, imin; \
	TYPE min; \
	for (j = 1; j <= n; j++) index[j] = j;	\
	if (n < 2) return;   /* Already sorted. */ \
	if (n == 2) \
	{ \
		if (COMPARELT(a[2], a[1])) \
		{\
			index[1] = 2; index[2] = 1; \
		} \
		return; \
	} \
	if (n <= 12) \
	{ \
		for (i = 1; i < n; i++) \
		{ \
			imin = i; \
			min = a[index[imin]]; \
			for (j = i + 1; j <= n; j++) \
			{\
				if (COMPARELT(a[index[j]], min))\
				{ \
					imin = j; \
					min = a[index[j]]; \
				} \
			} \
			ii = index[imin]; index[imin] = index[i]; index[i] = ii; \
		} \
		return; \
	} \
	/* H1 */\
	l = n / 2 + 1; \
	r = n; \
	for (;;) /* H2 */\
	{ \
		if (l > 1) \
		{ \
			l--; \
			k = index[l]; \
		} \
		else /* l == 1 */ \
		{ \
			k = index[r]; \
			index[r] = index[1]; \
			r--; \
			if (r == 1) \
			{ \
				index[1] = k; break; \
			} \
		} \
		/* H3 */ \
		j = l; \
		for (;;) \
		{ \
			/* H4 */ \
			i = j; \
			j *= 2; \
			if (j > r) break; \
			if (j < r && COMPARELT (a[index[j]], a[index[j + 1]])) j++; /* H5 */\
			index[i] = index[j]; /* H7 */\
		} \
		for (;;)  /*H8' */\
		{\
			j = i; \
			i = j >> 1; \
			/* H9' */ \
			if (j == l || COMPARELT (a[k], a[index[i]])) \
			{ \
				index[j] = k; break; \
			} \
			index[j] = index[i]; \
		}\
	} \
}

#define COMPARELT(x,y) ((x) < (y))

void NUMindexx_f (const float a[], long n, long index[])
	MACRO_NUMindex(float)

void NUMindexx_d (const double a[], long n, long index[])
	MACRO_NUMindex(float)
	
#undef COMPARELT
#define COMPARELT(x,y) (NUMwcscmp (x,y) <  0)
void NUMindexx_s (wchar_t **a, long n, long index[])
	MACRO_NUMindex(wchar_t *)

#undef COMPARELT
#undef MACRO_INDEXX


/*
	Knuth's heapsort algorithm (vol. 3, page 145), 
	modified with Floyd's optimization (vol. 3, page 642).
*/
#define MACRO_NUMsortkf(TYPE)  { \
	long l, r, j, i, imin; \
	TYPE k, min; \
	if (n < 2) return;   /* Already sorted. */ \
	/* This n<2 step is absent from Press et al.'s implementation, */ \
	/* which will therefore not terminate on if(--ir==1). */ \
	/* Knuth's initial assumption is now fulfilled: n >= 2. */ \
	if (n == 2) \
	{ \
		if (a[1] > a[2]) \
		{\
			min = a[2]; a[2] = a[1]; a[1] = min; \
		} \
		return; \
	} \
	if (n <= 12) \
	{ \
		for (i = 1; i < n; i++) \
		{ \
			min = a[i]; \
			imin = i; \
			for (j = i + 1; j <= n; j++) \
			{\
				if (a[j] < min)\
				{ \
					min = a [j]; \
					imin = j; \
				} \
			} \
			a[imin] = a[i]; \
			a[i] = min; \
		} \
		return; \
	} \
	/* H1 */\
	l = (n >> 1) + 1; \
	r = n; \
	for (;;) /* H2 */\
	{ \
		if (l > 1) \
		{ \
			l--; \
			k = a[l]; \
		} \
		else /* l == 1 */ \
		{ \
			k = a[r]; \
			a[r] = a[1]; \
			r--; \
			if (r == 1) \
			{ \
				a[1] = k; return; \
			} \
		} \
		/* H3 */ \
		j = l; \
		for (;;) \
		{ \
			/* H4 */ \
			i = j; \
			j = j << 1; \
			if (j > r) break; \
			if (j < r && a[j] < a[j + 1]) j++; /* H5 */\
			/*if (k >= a[j]) break; */\
			a[i] = a[j]; /* H7 */\
		} \
/*		a[i] = k; H8 */\
		for (;;)  /*H8' */\
		{\
			j = i; \
			i = j >> 1; \
			/* H9' */ \
			if (j == l || k <= a[i]) \
			{ \
				a[j] = k; break; \
			} \
			a[j] = a[i]; \
		}\
	} \
}

void NUMsort1_d (long n, double a[]);
void NUMsort1_d (long n, double a[])
	MACRO_NUMsortkf (double)

#undef MACRO_NUMsortkf

/* End of file NUMsort.c */
