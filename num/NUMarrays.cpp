/* NUMarrays.cpp
 *
 * Copyright (C) 1992-2012,2017 Paul Boersma
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

#include "melder.h"

static long theTotalNumberOfArrays;

long NUM_getTotalNumberOfArrays () { return theTotalNumberOfArrays; }

/*** Generic memory routines for vectors. ***/

void * NUMvector (long elementSize, long lo, long hi, bool zero) {
	try {
		if (hi < lo) return nullptr;   // not an error
		char *result;
		Melder_assert (sizeof (char) == 1);   // some say that this is true by definition
		for (;;) {   // not very infinite: 99.999 % of the time once, 0.001 % twice
			result = zero ?
				reinterpret_cast<char*> (_Melder_calloc (hi - lo + 1, elementSize)) :
				reinterpret_cast<char*> (_Melder_malloc ((hi - lo + 1) * elementSize));
			if (result -= lo * elementSize) break;   // this will normally succeed at the first try
			(void) Melder_realloc_f (result + lo * elementSize, 1);   // make "sure" that the second try will succeed (not *very* sure, because realloc might move memory even if it shrinks)
		}
		theTotalNumberOfArrays += 1;
		return result;
	} catch (MelderError) {
		Melder_throw (U"Vector of elements not created.");
	}
}

void NUMvector_free (long elementSize, void *v, long lo) noexcept {
	if (! v) return;   // no error
	char *dum = (char *) v + lo * elementSize;
	Melder_free (dum);
	theTotalNumberOfArrays -= 1;
}

void * NUMvector_copy (long elementSize, void *v, long lo, long hi) {
	try {
		if (! v) return nullptr;
		char *result = reinterpret_cast <char *> (NUMvector (elementSize, lo, hi, false));
		long offset = lo * elementSize;
		memcpy (result + offset, (char *) v + offset, (hi - lo + 1) * elementSize);
		return result;
	} catch (MelderError) {
		Melder_throw (U"Vector of elements not copied.");
	}
}

void NUMvector_copyElements (long elementSize, void *v, void *to, long lo, long hi) {
	long offset = lo * elementSize;
	Melder_assert (!! v && !! to);
	if (hi >= lo) memcpy ((char *) to + offset, (char *) v + offset, (hi - lo + 1) * elementSize);
}

bool NUMvector_equal (long elementSize, void *v1, void *v2, long lo, long hi) {
	long offset = lo * elementSize;
	Melder_assert (v1 && v2);
	return ! memcmp ((char *) v1 + offset, (char *) v2 + offset, (hi - lo + 1) * elementSize);
}

void NUMvector_append (long elementSize, void **v, long lo, long *hi) {
	try {
		char *result;
		if (! *v) {
			result = reinterpret_cast <char *> (NUMvector (elementSize, lo, lo, true));
			*hi = lo;
		} else {
			long offset = lo * elementSize;
			for (;;) {   // not very infinite: 99.999 % of the time once, 0.001 % twice
				result = reinterpret_cast <char *> (Melder_realloc ((char *) *v + offset, (*hi - lo + 2) * elementSize));
				if ((result -= offset) != nullptr) break;   // this will normally succeed at the first try
				(void) Melder_realloc_f (result + offset, 1);   // make "sure" that the second try will succeed
			}
			(*hi) ++;
			memset (result + *hi * elementSize, 0, elementSize);   // initialize the new element to zeroes
		}
		*v = result;
	} catch (MelderError) {
		Melder_throw (U"Vector: element not appended.");
	}
}

void NUMvector_insert (long elementSize, void **v, long lo, long *hi, long position) {
	try {
		char *result;
		if (! *v) {
			result = reinterpret_cast <char *> (NUMvector (elementSize, lo, lo, true));
			*hi = lo;
			Melder_assert (position == lo);
		} else {
			result = reinterpret_cast <char *> (NUMvector (elementSize, lo, *hi + 1, false));
			Melder_assert (position >= lo && position <= *hi + 1);
			NUMvector_copyElements (elementSize, *v, result, lo, position - 1);
			memset (result + position * elementSize, 0, elementSize);
			NUMvector_copyElements (elementSize, *v, result + elementSize, position, *hi);
			NUMvector_free (elementSize, *v, lo);
			(*hi) ++;
		}
		*v = result;
	} catch (MelderError) {
		Melder_throw (U"Vector: element not inserted.");
	}
}

/*** Generic memory routines for matrices. ***/

void * NUMmatrix (long elementSize, long row1, long row2, long col1, long col2, bool zero) {
	try {
		int64 numberOfRows = row2 - row1 + 1;
		int64 numberOfColumns = col2 - col1 + 1;
		int64 numberOfCells = numberOfRows * numberOfColumns;

		/*
		 * Allocate room for the row pointers.
		 */
		char **result;
		Melder_assert (sizeof (char) == 1);   // true by definition
		for (;;) {
			result = reinterpret_cast <char **> (_Melder_malloc_f (numberOfRows * (int64) sizeof (char *)));   // assume that all pointers have the same size
			result -= row1;
			if (result) break;   // this will normally succeed at the first try
			(void) Melder_realloc_f (result + row1, 1);   // make "sure" that the second try will succeed
		}
		/*
		 * Allocate room for the cells.
		 * The first row pointer points to below the first cell.
		 */
		for (;;) {
			try {
				result [row1] = zero ?
					reinterpret_cast <char *> (_Melder_calloc (numberOfCells, elementSize)) :
					reinterpret_cast <char *> (_Melder_malloc (numberOfCells * elementSize));
			} catch (MelderError) {
				result += row1;
				Melder_free (result);   // free the row pointers
				throw MelderError ();
			}
			if ((result [row1] -= col1 * elementSize) != nullptr) break;   // this will normally succeed at the first try
			(void) Melder_realloc_f (result [row1] + col1 * elementSize, 1);   // make "sure" that the second try will succeed
		}
		int64 columnSize = numberOfColumns * elementSize;
		for (long irow = row1 + 1; irow <= row2; irow ++) result [irow] = result [irow - 1] + columnSize;
		theTotalNumberOfArrays += 1;
		return result;
	} catch (MelderError) {
		Melder_throw (U"Matrix of elements not created.");
	}
}

void NUMmatrix_free (long elementSize, void *m, long row1, long col1) noexcept {
	if (! m) return;
	char *dummy1 = ((char **) m) [row1] + col1 * elementSize;
	Melder_free (dummy1);
	char **dummy2 = (char **) m + row1;
	Melder_free (dummy2);
	theTotalNumberOfArrays -= 1;
}

void * NUMmatrix_copy (long elementSize, void * m, long row1, long row2, long col1, long col2) {
	try {
		if (! m) return nullptr;
		char **result = reinterpret_cast <char **> (NUMmatrix (elementSize, row1, row2, col1, col2, false));
		if (! result) return nullptr;
		long columnOffset = col1 * elementSize;
		long dataSize = (row2 - row1 + 1) * (col2 - col1 + 1) * elementSize;
		memcpy (result [row1] + columnOffset, ((char **) m) [row1] + columnOffset, dataSize);
		return result;
	} catch (MelderError) {
		Melder_throw (U"Matrix of elements not copied.");
	}
}

void NUMmatrix_copyElements (long elementSize, void *m, void *to, long row1, long row2, long col1, long col2) {
	Melder_assert (m && to);
	long columnOffset = col1 * elementSize;
	long dataSize = (row2 - row1 + 1) * (col2 - col1 + 1) * elementSize;
	memcpy (((char **) to) [row1] + columnOffset, ((char **) m) [row1] + columnOffset, dataSize);
}

bool NUMmatrix_equal (long elementSize, void *m1, void *m2, long row1, long row2, long col1, long col2) {
	Melder_assert (m1 && m2);
	long columnOffset = col1 * elementSize;
	long dataSize = (row2 - row1 + 1) * (col2 - col1 + 1) * elementSize;
	return ! memcmp (((char **) m1) [row1] + columnOffset, ((char **) m2) [row1] + columnOffset, dataSize);
}

/*** Typed I/O routines for vectors and matrices. ***/

#define FUNCTION(type,storage)  \
	void NUMvector_writeText_##storage (const type *v, long lo, long hi, MelderFile file, const char32 *name) { \
		texputintro (file, name, U" []: ", hi >= lo ? nullptr : U"(empty)", 0,0,0); \
		for (long i = lo; i <= hi; i ++) \
			texput##storage (file, v [i], name, U" [", Melder_integer (i), U"]", 0,0); \
		texexdent (file); \
		if (feof (file -> filePointer) || ferror (file -> filePointer)) Melder_throw (U"Write error."); \
	} \
	void NUMvector_writeBinary_##storage (const type *v, long lo, long hi, FILE *f) { \
		for (long i = lo; i <= hi; i ++) \
			binput##storage (v [i], f); \
		if (feof (f) || ferror (f)) Melder_throw (U"Write error."); \
	} \
	type * NUMvector_readText_##storage (long lo, long hi, MelderReadText text, const char *name) { \
		type *result = nullptr; \
		try { \
			result = NUMvector <type> (lo, hi); \
			for (long i = lo; i <= hi; i ++) { \
				try { \
					result [i] = texget##storage (text); \
				} catch (MelderError) { \
					Melder_throw (U"Could not read ", Melder_peek8to32 (name), U" [", i, U"]."); \
				} \
			} \
			return result; \
		} catch (MelderError) { \
			NUMvector_free (result, lo); \
			throw; \
		} \
	} \
	type * NUMvector_readBinary_##storage (long lo, long hi, FILE *f) { \
		type *result = nullptr; \
		try { \
			result = NUMvector <type> (lo, hi); \
			for (long i = lo; i <= hi; i ++) { \
				result [i] = binget##storage (f); \
			} \
			return result; \
		} catch (MelderError) { \
			NUMvector_free (result, lo); \
			throw; \
		} \
	} \
	void NUMmatrix_writeText_##storage (type **m, long row1, long row2, long col1, long col2, MelderFile file, const char32 *name) { \
		texputintro (file, name, U" [] []: ", row2 >= row1 ? nullptr : U"(empty)", 0,0,0); \
		if (row2 >= row1) { \
			for (long irow = row1; irow <= row2; irow ++) { \
				texputintro (file, name, U" [", Melder_integer (irow), U"]:", 0,0); \
				for (long icol = col1; icol <= col2; icol ++) { \
					texput##storage (file, m [irow] [icol], name, U" [", Melder_integer (irow), U"] [", Melder_integer (icol), U"]"); \
				} \
				texexdent (file); \
			} \
		} \
		texexdent (file); \
		if (feof (file -> filePointer) || ferror (file -> filePointer)) Melder_throw (U"Write error."); \
	} \
	void NUMmatrix_writeBinary_##storage (type **m, long row1, long row2, long col1, long col2, FILE *f) { \
		if (row2 >= row1) { \
			for (long irow = row1; irow <= row2; irow ++) { \
				for (long icol = col1; icol <= col2; icol ++) \
					binput##storage (m [irow] [icol], f); \
			} \
		} \
		if (feof (f) || ferror (f)) Melder_throw (U"Write error."); \
	} \
	type ** NUMmatrix_readText_##storage (long row1, long row2, long col1, long col2, MelderReadText text, const char *name) { \
		type **result = nullptr; \
		try { \
			result = NUMmatrix <type> (row1, row2, col1, col2); \
			for (long irow = row1; irow <= row2; irow ++) for (long icol = col1; icol <= col2; icol ++) { \
				try { \
					result [irow] [icol] = texget##storage (text); \
				} catch (MelderError) { \
					Melder_throw (U"Could not read ", Melder_peek8to32 (name), U" [", irow, U"] [", icol, U"]."); \
				} \
			} \
			return result; \
		} catch (MelderError) { \
			NUMmatrix_free (result, row1, col1); \
			throw; \
		} \
	} \
	type ** NUMmatrix_readBinary_##storage (long row1, long row2, long col1, long col2, FILE *f) { \
		type **result = nullptr; \
		try { \
			result = NUMmatrix <type> (row1, row2, col1, col2); \
			for (long irow = row1; irow <= row2; irow ++) for (long icol = col1; icol <= col2; icol ++) \
				result [irow] [icol] = binget##storage (f); \
			return result; \
		} catch (MelderError) { \
			NUMmatrix_free (result, row1, col1); \
			throw; \
		} \
	}

FUNCTION (signed char, i8)
FUNCTION (int, i16)
FUNCTION (long, i32)
FUNCTION (unsigned char, u8)
FUNCTION (unsigned int, u16)
FUNCTION (unsigned long, u32)
FUNCTION (double, r32)
FUNCTION (double, r64)
FUNCTION (fcomplex, c64)
FUNCTION (dcomplex, c128)
#undef FUNCTION

/* End of file NUMarrays.cpp */
