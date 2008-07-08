/* NUMarrays.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/07/21 readText and writeText API changes
 */

#include "NUM.h"
#include "melder.h"

static long theTotalNumberOfArrays;

long NUM_getTotalNumberOfArrays (void) { return theTotalNumberOfArrays; }

/*** Generic memory routines for vectors. ***/

void * NUMvector (long elementSize, long lo, long hi) {
	char *result;
	Melder_assert (sizeof (char) == 1);
	for (;;) { /* Not very infinite: 99.999 % of the time once, 0.001 % twice. */
		if (! (result = _Melder_calloc (hi - lo + 1, elementSize)))
			return Melder_errorp ("(NUMvector:) Not created.");
		if (result -= lo * elementSize) break;   /* This will normally succeed at the first try. */
		(void) Melder_realloc (result + lo * elementSize, 1);   /* Make sure that second try will succeed. */
	}
	theTotalNumberOfArrays += 1;
	return result;
}

void NUMvector_free (long elementSize, void *v, long lo) {
	char *dum;
	if (! v) return;
	dum = (char *) v + lo * elementSize;
	Melder_free (dum);
	theTotalNumberOfArrays -= 1;
}

void * NUMvector_copy (long elementSize, void *v, long lo, long hi) {
	char *result;
	long offset = lo * elementSize;
	if (! v) return NULL;
	if (! (result = NUMvector (elementSize, lo, hi))) return NULL;
	memcpy (result + offset, (char *) v + offset, (hi - lo + 1) * elementSize);
	return result;
}

void NUMvector_copyElements (long elementSize, void *v, void *to, long lo, long hi) {
	long offset = lo * elementSize;
	Melder_assert (v != NULL && to != NULL);
	if (hi >= lo) memcpy ((char *) to + offset, (char *) v + offset, (hi - lo + 1) * elementSize);
}

int NUMvector_equal (long elementSize, void *v1, void *v2, long lo, long hi) {
	long offset = lo * elementSize;
	Melder_assert (v1 != NULL && v2 != NULL);
	return ! memcmp ((char *) v1 + offset, (char *) v2 + offset, (hi - lo + 1) * elementSize);
}

/*** Generic memory routines for matrices. ***/

void * NUMmatrix (long elementSize, long row1, long row2, long col1, long col2) {
	long i, nrow = row2 - row1 + 1, ncol = col2 - col1 + 1, colSize = ncol * elementSize;
	char **result, **dum;
	Melder_assert (sizeof (char) == 1);
	for (;;) {
		if (! (result = _Melder_malloc (nrow * sizeof (char *))))   /* Assume all pointers have same size.*/
			return NULL;
		if (result -= row1) break;   /* This will normally succeed at the first try. */
		(void) Melder_realloc (result + row1, 1);   /* Make sure that second try will succeed. */
	}
	for (;;) {
		if (! (result [row1] = _Melder_calloc (nrow * ncol, elementSize))) {
			dum = result + row1;
			Melder_free (dum);
			return NULL;
		}
		if (result [row1] -= col1 * elementSize) break;   /* This will normally succeed at the first try. */
		(void) Melder_realloc (result [row1] + col1 * elementSize, 1);   /* Make sure that second try will succeed. */
	}
	for (i = row1 + 1; i <= row2; i++) result [i] = result [i - 1] + colSize;
	theTotalNumberOfArrays += 1;
	return result;
}

void NUMmatrix_free (long elementSize, void *m, long row1, long col1) {
	char *dum1, **dum2;
	if (! m) return;
	dum1 = ((char **) m) [row1] + col1 * elementSize;
	Melder_free (dum1);
	dum2 = (char **) m + row1;
	Melder_free (dum2);
	theTotalNumberOfArrays -= 1;
}

void * NUMmatrix_copy (long elementSize, void * m, long row1, long row2, long col1, long col2) {
	char **result;
	long colOffset = col1 * elementSize, dataSize = (row2 - row1 + 1) * (col2 - col1 + 1) * elementSize;
	if (! m) return NULL;
	result = NUMmatrix (elementSize, row1, row2, col1, col2);
	if (! result) return NULL;
	memcpy (result [row1] + colOffset, ((char **) m) [row1] + colOffset, dataSize);
	return result;
}

void NUMmatrix_copyElements (long elementSize, void *m, void *to, long row1, long row2, long col1, long col2) {
	long colOffset = col1 * elementSize, dataSize = (row2 - row1 + 1) * (col2 - col1 + 1) * elementSize;
	Melder_assert (m != NULL && to != NULL);
	memcpy (((char **) to) [row1] + colOffset, ((char **) m) [row1] + colOffset, dataSize);
}

int NUMmatrix_equal (long elementSize, void *m1, void *m2, long row1, long row2, long col1, long col2) {
	long colOffset = col1 * elementSize, dataSize = (row2 - row1 + 1) * (col2 - col1 + 1) * elementSize;
	Melder_assert (m1 != NULL && m2 != NULL);
	return ! memcmp (((char **) m1) [row1] + colOffset, ((char **) m2) [row1] + colOffset, dataSize);
}

/*** Typed memory and I/O routines for vectors and matrices. ***/

#define FUNCTION(t,type,storage)  \
	type * NUM##t##vector (long lo, long hi) \
		{ return NUMvector (sizeof (type), lo, hi); } \
	void NUM##t##vector_free (type *v, long lo) \
		{ NUMvector_free (sizeof (type), v, lo); } \
	type * NUM##t##vector_copy (const type *v, long lo, long hi) \
		{ return NUMvector_copy (sizeof (type), (void *) v, lo, hi); } \
	void NUM##t##vector_copyElements (const type *v, type *to, long lo, long hi) \
		{ NUMvector_copyElements (sizeof (type), (void *) v, to, lo, hi); } \
	int NUM##t##vector_equal (const type *v1, const type *v2, long lo, long hi) \
		{ return NUMvector_equal (sizeof (type), (void *) v1, (void *) v2, lo, hi); } \
	int NUM##t##vector_writeText (const type *v, long lo, long hi, MelderFile file, const wchar_t *name) { \
		texputintro (file, name, L" []: ", hi >= lo ? NULL : L"(empty)", 0,0,0); \
		for (long i = lo; i <= hi; i ++) \
			texput##storage (file, v [i], name, L" [", Melder_integer (i), L"]", 0,0); \
		texexdent (file); \
		if (feof (file -> filePointer) || ferror (file -> filePointer)) return 0; \
		return 1; \
	} \
	int NUM##t##vector_writeBinary (const type *v, long lo, long hi, FILE *f) { \
		long i; \
		for (i = lo; i <= hi; i ++) \
			binput##storage (v [i], f); \
		if (feof (f) || ferror (f)) return 0; \
		return 1; \
	} \
	int NUM##t##vector_writeCache (const type *v, long lo, long hi, CACHE *f) { \
		long i; \
		for (i = lo; i <= hi; i ++) \
			cacput##storage (v [i], f); \
		return 1; \
	} \
	type * NUM##t##vector_readText (long lo, long hi, MelderReadString *text, const char *name) { \
		type *result = NUM##t##vector (lo, hi); \
		if (! result) return NULL; \
		for (long i = lo; i <= hi; i ++) { \
			result [i] = texget##storage (text); \
			if (Melder_hasError ()) { \
				NUM##t##vector_free (result, lo); \
				return Melder_errorp ("(NUM" #t "vector_readText:) Could not read %s [%ld].", name, i); \
			} \
		} \
		return result; \
	} \
	type * NUM##t##vector_readBinary (long lo, long hi, FILE *f) { \
		long i; \
		type *result = NUM##t##vector (lo, hi); \
		if (! result) return NULL; \
		for (i = lo; i <= hi; i ++) { \
			result [i] = binget##storage (f); \
			if (feof (f)) { \
				NUM##t##vector_free (result, lo); \
				return NULL; \
			} \
		} \
		return result; \
	} \
	type * NUM##t##vector_readCache (long lo, long hi, CACHE *f) { \
		long i; \
		type *result = NUM##t##vector (lo, hi); \
		if (! result) return NULL; \
		for (i = lo; i <= hi; i ++) { \
			result [i] = cacget##storage (f); \
		} \
		return result; \
	} \
	type ** NUM##t##matrix (long row1, long row2, long col1, long col2) \
		{ return NUMmatrix (sizeof (type), row1, row2, col1, col2); } \
	void NUM##t##matrix_free (type **m, long row1, long col1) \
		{ NUMmatrix_free (sizeof (type), m, row1, col1); } \
	type ** NUM##t##matrix_copy (type **m, long row1, long row2, long col1, long col2) \
		{ return NUMmatrix_copy (sizeof (type), m, row1, row2, col1, col2); } \
	void NUM##t##matrix_copyElements (type **m, type **to, long row1, long row2, long col1, long col2) \
		{ NUMmatrix_copyElements (sizeof (type), m, to, row1, row2, col1, col2); } \
	int NUM##t##matrix_equal (type **m1, type **m2, long row1, long row2, long col1, long col2) \
		{ return NUMmatrix_equal (sizeof (type), m1, m2, row1, row2, col1, col2); } \
	int NUM##t##matrix_writeText (type **m, long row1, long row2, long col1, long col2, MelderFile file, const wchar_t *name) { \
		texputintro (file, name, L" [] []: ", row2 >= row1 ? NULL : L"(empty)", 0,0,0); \
		if (row2 >= row1) { \
			long row, col; \
			for (row = row1; row <= row2; row ++) { \
				texputintro (file, name, L" [", Melder_integer (row), L"]:", 0,0); \
				for (col = col1; col <= col2; col ++) { \
					texput##storage (file, m [row] [col], name, L" [", Melder_integer (row), L"] [", Melder_integer (col), L"]"); \
				} \
				texexdent (file); \
			} \
		} \
		texexdent (file); \
		if (feof (file -> filePointer) || ferror (file -> filePointer)) return 0; \
		return 1; \
	} \
	int NUM##t##matrix_writeBinary (type **m, long row1, long row2, long col1, long col2, FILE *f) { \
		if (row2 >= row1) { \
			long row, col; \
			for (row = row1; row <= row2; row ++) { \
				for (col = col1; col <= col2; col ++) \
					binput##storage (m [row] [col], f); \
			} \
		} \
		if (feof (f) || ferror (f)) return 0; \
		return 1; \
	} \
	int NUM##t##matrix_writeCache (type **m, long row1, long row2, long col1, long col2, CACHE *f) { \
		if (row2 >= row1) { \
			long row, col; \
			for (row = row1; row <= row2; row ++) { \
				for (col = col1; col <= col2; col ++) \
					cacput##storage (m [row] [col], f); \
			} \
		} \
		return 1; \
	} \
	type ** NUM##t##matrix_readText (long row1, long row2, long col1, long col2, MelderReadString *text, const char *name) { \
		type **result = NUM##t##matrix (row1, row2, col1, col2); \
		if (! result) return NULL; \
		for (long i = row1; i <= row2; i ++) for (long j = col1; j <= col2; j ++) { \
			result [i] [j] = texget##storage (text); \
			if (Melder_hasError ()) { \
				NUM##t##matrix_free (result, row1, col1); \
				return Melder_errorp ("(NUM" #t "matrix_readText:) " \
					"Could not read %s [%ld] [%ld].", name, i, j); \
			} \
		} \
		return result; \
	} \
	type ** NUM##t##matrix_readBinary (long row1, long row2, long col1, long col2, FILE *f) { \
		long i, j; \
		type **result = NUM##t##matrix (row1, row2, col1, col2); \
		if (! result) return NULL; \
		for (i = row1; i <= row2; i ++) for (j = col1; j <= col2; j ++) { \
			result [i] [j] = binget##storage (f); \
			if (feof (f)) { \
				NUM##t##matrix_free (result, row1, col1); \
				return NULL; \
			} \
		} \
		return result; \
	} \
	type ** NUM##t##matrix_readCache (long row1, long row2, long col1, long col2, CACHE *f) { \
		long i, j; \
		type **result = NUM##t##matrix (row1, row2, col1, col2); \
		if (! result) return NULL; \
		for (i = row1; i <= row2; i ++) for (j = col1; j <= col2; j ++) { \
			result [i] [j] = cacget##storage (f); \
		} \
		return result; \
	}

		
FUNCTION (b, signed char, i1)
FUNCTION (s, short, i2)
FUNCTION (i, int, i2)
FUNCTION (l, long, i4)
FUNCTION (ub, unsigned char, u1)
FUNCTION (us, unsigned short, u2)
FUNCTION (ui, unsigned int, u2)
FUNCTION (ul, unsigned long, u4)
FUNCTION (f, float, r4)
FUNCTION (d, double, r8)
FUNCTION (fc, fcomplex, c8)
FUNCTION (dc, dcomplex, c16)
FUNCTION (c, char, c1)

/* End of file NUMarrays.c */
