/* NUMarrays.cpp
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
 * pb 2007/07/21 readText and writeText API changes
 * pb 2008/01/19 include storage in I/O names
 * pb 2009/03/14 NUMvector_add
 * pb 2011/03/29 C++
 */

#include "NUM.h"
#include "melder.h"

static long theTotalNumberOfArrays;

long NUM_getTotalNumberOfArrays (void) { return theTotalNumberOfArrays; }

/*** Generic memory routines for vectors. ***/

void * NUMvector (long elementSize, long lo, long hi) {
	try {
		if (hi < lo) return NULL;   // not an error
		char *result;
		Melder_assert (sizeof (char) == 1);   // some say that this is true by definition
		for (;;) {   // not very infinite: 99.999 % of the time once, 0.001 % twice
			result = reinterpret_cast<char*> (_Melder_calloc_e (hi - lo + 1, elementSize)); therror
			if (result -= lo * elementSize) break;   // this will normally succeed at the first try
			(void) Melder_realloc_f (result + lo * elementSize, 1);   // make "sure" that the second try will succeed (not *very* sure, because realloc might move memory even if it shrinks)
		}
		theTotalNumberOfArrays += 1;
		return result;
	} catch (...) {
		rethrowmzero ("Vector not created.");
	}
}

void NUMvector_free (long elementSize, void *v, long lo) {
	if (v == NULL) return;   // no error
	char *dum = (char *) v + lo * elementSize;
	Melder_free (dum);
	theTotalNumberOfArrays -= 1;
}

void * NUMvector_copy (long elementSize, void *v, long lo, long hi) {
	try {
		if (v == NULL) return NULL;
		char *result = reinterpret_cast<char*> (NUMvector (elementSize, lo, hi)); therror
		long offset = lo * elementSize;
		memcpy (result + offset, (char *) v + offset, (hi - lo + 1) * elementSize);
		return result;
	} catch (...) {
		rethrowmzero ("Vector not copied.");
	}
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

void NUMvector_append_e (long elementSize, void **v, long lo, long *hi) {
	try {
		char *result;
		if (*v == NULL) {
			result = reinterpret_cast <char *> (NUMvector (elementSize, lo, lo)); therror
			*hi = lo;
		} else {
			long offset = lo * elementSize;
			for (;;) {   // not very infinite: 99.999 % of the time once, 0.001 % twice
				result = reinterpret_cast <char *> (Melder_realloc_e ((char *) *v + offset, (*hi - lo + 2) * elementSize)); therror
				if ((result -= offset) != NULL) break;   // this will normally succeed at the first try
				(void) Melder_realloc_f (result + offset, 1);   // make "sure" that the second try will succeed
			}
			(*hi) ++;
			memset (result + *hi * elementSize, 0, elementSize);   // initialize the new element to zeroes
		}
		*v = result;
	} catch (...) {
		rethrowm ("Vector: element not appended.");
	}
}

void NUMvector_insert_e (long elementSize, void **v, long lo, long *hi, long position) {
	try {
		char *result;
		if (*v == NULL) {
			result = reinterpret_cast <char *> (NUMvector (elementSize, lo, lo)); therror
			*hi = lo;
			Melder_assert (position == lo);
		} else {
			result = reinterpret_cast <char *> (NUMvector (elementSize, lo, *hi + 1)); therror
			Melder_assert (position >= lo && position <= *hi + 1);
			NUMvector_copyElements (elementSize, *v, result, lo, position - 1);
			NUMvector_copyElements (elementSize, *v, result + elementSize, position, *hi);
			NUMvector_free (elementSize, *v, lo);
			(*hi) ++;
		}
		*v = result;
	} catch (...) {
		rethrowm ("Vector: element not inserted.");
	}
}

/*** Generic memory routines for matrices. ***/

void * NUMmatrix (long elementSize, long row1, long row2, long col1, long col2) {
	try {
		/*
		 * Allocate room for the row pointers.
		 */
		long numberOfRows = row2 - row1 + 1;
		char **result;
		Melder_assert (sizeof (char) == 1);   // true by definition
		for (;;) {
			result = reinterpret_cast <char **> (_Melder_malloc_f (numberOfRows * sizeof (char *))); therror   // assume that all pointers have the same size
			result -= row1;
			if (result != NULL) break;   // this will normally succeed at the first try
			(void) Melder_realloc_f (result + row1, 1);   // make "sure" that the second try will succeed
		}
		/*
		 * Allocate room for the cells.
		 * The first row pointer points to below the first cell.
		 */
		long numberOfColumns = col2 - col1 + 1;
		for (;;) {
			try {
				result [row1] = reinterpret_cast <char *> (_Melder_calloc_e (numberOfRows * numberOfColumns, elementSize)); therror
			} catch (...) {
				result += row1;
				Melder_free (result);   // free the row pointers
				throw 1;
			}
			if ((result [row1] -= col1 * elementSize) != NULL) break;   // this will normally succeed at the first try
			(void) Melder_realloc_f (result [row1] + col1 * elementSize, 1);   // make "sure" that the second try will succeed
		}
		long columnSize = numberOfColumns * elementSize;
		for (long irow = row1 + 1; irow <= row2; irow ++) result [irow] = result [irow - 1] + columnSize;
		theTotalNumberOfArrays += 1;
		return result;
	} catch (...) {
		rethrowmzero ("Matrix not created.");
	}
}

void NUMmatrix_free (long elementSize, void *m, long row1, long col1) {
	if (m == NULL) return;
	char *dummy1 = ((char **) m) [row1] + col1 * elementSize;
	Melder_free (dummy1);
	char **dummy2 = (char **) m + row1;
	Melder_free (dummy2);
	theTotalNumberOfArrays -= 1;
}

void * NUMmatrix_copy (long elementSize, void * m, long row1, long row2, long col1, long col2) {
	try {
		if (m == NULL) return NULL;
		char **result = reinterpret_cast <char **> (NUMmatrix (elementSize, row1, row2, col1, col2));
		if (result == NULL) return NULL;
		long columnOffset = col1 * elementSize;
		long dataSize = (row2 - row1 + 1) * (col2 - col1 + 1) * elementSize;
		memcpy (result [row1] + columnOffset, ((char **) m) [row1] + columnOffset, dataSize);
		return result;
	} catch (...) {
		rethrowmzero ("Matrix not copied.");
	}
}

void NUMmatrix_copyElements (long elementSize, void *m, void *to, long row1, long row2, long col1, long col2) {
	Melder_assert (m != NULL && to != NULL);
	long columnOffset = col1 * elementSize;
	long dataSize = (row2 - row1 + 1) * (col2 - col1 + 1) * elementSize;
	memcpy (((char **) to) [row1] + columnOffset, ((char **) m) [row1] + columnOffset, dataSize);
}

int NUMmatrix_equal (long elementSize, void *m1, void *m2, long row1, long row2, long col1, long col2) {
	Melder_assert (m1 != NULL && m2 != NULL);
	long columnOffset = col1 * elementSize;
	long dataSize = (row2 - row1 + 1) * (col2 - col1 + 1) * elementSize;
	return ! memcmp (((char **) m1) [row1] + columnOffset, ((char **) m2) [row1] + columnOffset, dataSize);
}

/*** Typed memory and I/O routines for vectors and matrices. ***/

#define FUNCTION(t,type)  \
	type * NUM##t##vector (long lo, long hi) \
		{ return (type *) NUMvector (sizeof (type), lo, hi); } \
	void NUM##t##vector_free (type *v, long lo) \
		{ NUMvector_free (sizeof (type), v, lo); } \
	type * NUM##t##vector_copy (const type *v, long lo, long hi) \
		{ return (type *) NUMvector_copy (sizeof (type), (void *) v, lo, hi); } \
	void NUM##t##vector_copyElements (const type *v, type *to, long lo, long hi) \
		{ NUMvector_copyElements (sizeof (type), (void *) v, to, lo, hi); } \
	int NUM##t##vector_equal (const type *v1, const type *v2, long lo, long hi) \
		{ return NUMvector_equal (sizeof (type), (void *) v1, (void *) v2, lo, hi); } \
	void NUM##t##vector_append_e (type **v, long lo, long *hi) \
		{ NUMvector_append_e (sizeof (type), (void **) v, lo, hi); } \
	void NUM##t##vector_insert_e (type **v, long lo, long *hi, long position) \
		{ NUMvector_insert_e (sizeof (type), (void **) v, lo, hi, position); } \
	type ** NUM##t##matrix (long row1, long row2, long col1, long col2) \
		{ return (type **) NUMmatrix (sizeof (type), row1, row2, col1, col2); } \
	void NUM##t##matrix_free (type **m, long row1, long col1) \
		{ NUMmatrix_free (sizeof (type), m, row1, col1); } \
	type ** NUM##t##matrix_copy (type **m, long row1, long row2, long col1, long col2) \
		{ return (type **) NUMmatrix_copy (sizeof (type), m, row1, row2, col1, col2); } \
	void NUM##t##matrix_copyElements (type **m, type **to, long row1, long row2, long col1, long col2) \
		{ NUMmatrix_copyElements (sizeof (type), m, to, row1, row2, col1, col2); } \
	int NUM##t##matrix_equal (type **m1, type **m2, long row1, long row2, long col1, long col2) \
		{ return NUMmatrix_equal (sizeof (type), m1, m2, row1, row2, col1, col2); }

FUNCTION (b, signed char)
FUNCTION (s, short)
FUNCTION (i, int)
FUNCTION (l, long)
FUNCTION (ub, unsigned char)
FUNCTION (us, unsigned short)
FUNCTION (ui, unsigned int)
FUNCTION (ul, unsigned long)
FUNCTION (d, double)
FUNCTION (fc, fcomplex)
FUNCTION (dc, dcomplex)
FUNCTION (c, char)
#undef FUNCTION

#define FUNCTION(t,type,storage)  \
	int NUM##t##vector_writeText_##storage (const type *v, long lo, long hi, MelderFile file, const wchar_t *name) { \
		texputintro (file, name, L" []: ", hi >= lo ? NULL : L"(empty)", 0,0,0); \
		for (long i = lo; i <= hi; i ++) \
			texput##storage (file, v [i], name, L" [", Melder_integer (i), L"]", 0,0); \
		texexdent (file); \
		if (feof (file -> filePointer) || ferror (file -> filePointer)) return 0; \
		return 1; \
	} \
	int NUM##t##vector_writeBinary_##storage (const type *v, long lo, long hi, FILE *f) { \
		for (long i = lo; i <= hi; i ++) \
			binput##storage (v [i], f); \
		if (feof (f) || ferror (f)) return 0; \
		return 1; \
	} \
	int NUM##t##vector_writeCache_##storage (const type *v, long lo, long hi, CACHE *f) { \
		for (long i = lo; i <= hi; i ++) \
			cacput##storage (v [i], f); \
		return 1; \
	} \
	type * NUM##t##vector_readText_##storage (long lo, long hi, MelderReadText text, const char *name) { \
		type *result = NUM##t##vector (lo, hi); \
		if (! result) return NULL; \
		for (long i = lo; i <= hi; i ++) { \
			result [i] = texget##storage (text); \
			if (Melder_hasError ()) { \
				NUM##t##vector_free (result, lo); \
				return (type *) Melder_errorp ("(NUM" #t "vector_readText:) Could not read %s [%ld].", name, i); \
			} \
		} \
		return result; \
	} \
	type * NUM##t##vector_readBinary_##storage (long lo, long hi, FILE *f) { \
		type *result = NUM##t##vector (lo, hi); \
		if (! result) return NULL; \
		for (long i = lo; i <= hi; i ++) { \
			result [i] = binget##storage (f); \
			if (feof (f)) { \
				NUM##t##vector_free (result, lo); \
				return NULL; \
			} \
		} \
		return result; \
	} \
	type * NUM##t##vector_readCache_##storage (long lo, long hi, CACHE *f) { \
		type *result = NUM##t##vector (lo, hi); \
		if (! result) return NULL; \
		for (long i = lo; i <= hi; i ++) { \
			result [i] = cacget##storage (f); \
		} \
		return result; \
	} \
	int NUM##t##matrix_writeText_##storage (type **m, long row1, long row2, long col1, long col2, MelderFile file, const wchar_t *name) { \
		texputintro (file, name, L" [] []: ", row2 >= row1 ? NULL : L"(empty)", 0,0,0); \
		if (row2 >= row1) { \
			for (long irow = row1; irow <= row2; irow ++) { \
				texputintro (file, name, L" [", Melder_integer (irow), L"]:", 0,0); \
				for (long icol = col1; icol <= col2; icol ++) { \
					texput##storage (file, m [irow] [icol], name, L" [", Melder_integer (irow), L"] [", Melder_integer (icol), L"]"); \
				} \
				texexdent (file); \
			} \
		} \
		texexdent (file); \
		if (feof (file -> filePointer) || ferror (file -> filePointer)) return 0; \
		return 1; \
	} \
	int NUM##t##matrix_writeBinary_##storage (type **m, long row1, long row2, long col1, long col2, FILE *f) { \
		if (row2 >= row1) { \
			for (long irow = row1; irow <= row2; irow ++) { \
				for (long icol = col1; icol <= col2; icol ++) \
					binput##storage (m [irow] [icol], f); \
			} \
		} \
		if (feof (f) || ferror (f)) return 0; \
		return 1; \
	} \
	int NUM##t##matrix_writeCache_##storage (type **m, long row1, long row2, long col1, long col2, CACHE *f) { \
		if (row2 >= row1) { \
			for (long irow = row1; irow <= row2; irow ++) { \
				for (long icol = col1; icol <= col2; icol ++) \
					cacput##storage (m [irow] [icol], f); \
			} \
		} \
		return 1; \
	} \
	type ** NUM##t##matrix_readText_##storage (long row1, long row2, long col1, long col2, MelderReadText text, const char *name) { \
		type **result = NUM##t##matrix (row1, row2, col1, col2); \
		if (! result) return NULL; \
		for (long irow = row1; irow <= row2; irow ++) for (long icol = col1; icol <= col2; icol ++) { \
			result [irow] [icol] = texget##storage (text); \
			if (Melder_hasError ()) { \
				NUM##t##matrix_free (result, row1, col1); \
				return (type **) Melder_errorp ("(NUM" #t "matrix_readText:) " \
					"Could not read %s [%ld] [%ld].", name, irow, icol); \
			} \
		} \
		return result; \
	} \
	type ** NUM##t##matrix_readBinary_##storage (long row1, long row2, long col1, long col2, FILE *f) { \
		type **result = NUM##t##matrix (row1, row2, col1, col2); \
		if (! result) return NULL; \
		for (long irow = row1; irow <= row2; irow ++) for (long icol = col1; icol <= col2; icol ++) { \
			result [irow] [icol] = binget##storage (f); \
			if (feof (f)) { \
				NUM##t##matrix_free (result, row1, col1); \
				return NULL; \
			} \
		} \
		return result; \
	} \
	type ** NUM##t##matrix_readCache_##storage (long row1, long row2, long col1, long col2, CACHE *f) { \
		type **result = NUM##t##matrix (row1, row2, col1, col2); \
		if (! result) return NULL; \
		for (long irow = row1; irow <= row2; irow ++) for (long icol = col1; icol <= col2; icol ++) { \
			result [irow] [icol] = cacget##storage (f); \
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
FUNCTION (d, double, r4)
FUNCTION (d, double, r8)
FUNCTION (fc, fcomplex, c8)
FUNCTION (dc, dcomplex, c16)
FUNCTION (c, char, c1)
#undef FUNCTION

/* End of file NUMarrays.cpp */
