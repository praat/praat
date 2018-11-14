#ifndef _melder_tensorio_h_
#define _melder_tensorio_h_
/* melder_tensorio.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

/* Declaring 240 functions. */
#define FUNCTION(T,storage)  \
	void NUMvector_writeText_##storage (const T *v, integer lo, integer hi, MelderFile file, conststring32 name); \
	void vector_writeText_##storage (const constvector<T>& vec, MelderFile file, conststring32 name); \
	void NUMvector_writeBinary_##storage (const T *v, integer lo, integer hi, FILE *f); \
	void vector_writeBinary_##storage (const constvector<T>& vec, FILE *f); \
	T * NUMvector_readText_##storage (integer lo, integer hi, MelderReadText text, const char *name); \
	autovector<T> vector_readText_##storage (integer size, MelderReadText text, const char *name); \
	T * NUMvector_readBinary_##storage (integer lo, integer hi, FILE *f); \
	autovector<T> vector_readBinary_##storage (integer size, FILE *f); \
	void NUMmatrix_writeText_##storage (T **v, integer r1, integer r2, integer c1, integer c2, MelderFile file, conststring32 name); \
	void matrix_writeText_##storage (const constmatrix<T>& mat, MelderFile file, conststring32 name); \
	void NUMmatrix_writeBinary_##storage (T **v, integer r1, integer r2, integer c1, integer c2, FILE *f); \
	void matrix_writeBinary_##storage (const constmatrix<T>& mat, FILE *f); \
	T ** NUMmatrix_readText_##storage (integer r1, integer r2, integer c1, integer c2, MelderReadText text, const char *name); \
	automatrix<T> matrix_readText_##storage (integer nrow, integer ncol, MelderReadText text, const char *name); \
	T ** NUMmatrix_readBinary_##storage (integer r1, integer r2, integer c1, integer c2, FILE *f); \
	automatrix<T> matrix_readBinary_##storage (integer nrow, integer ncol, FILE *f); \
	void tensor3_writeText_##storage (const consttensor3<T>& mat, MelderFile file, conststring32 name); \
	void tensor3_writeBinary_##storage (const consttensor3<T>& mat, FILE *f); \
	autotensor3<T> tensor3_readText_##storage (integer ndim1, integer ndim2, integer ndim3, MelderReadText text, const char *name); \
	autotensor3<T> tensor3_readBinary_##storage (integer ndim1, integer ndim2, integer ndim3, FILE *f);

FUNCTION (signed char, i8)
FUNCTION (int, i16)
FUNCTION (long, i32)
FUNCTION (integer, integer32BE)
FUNCTION (integer, integer16BE)
FUNCTION (unsigned char, u8)
FUNCTION (unsigned int, u16)
FUNCTION (unsigned long, u32)
FUNCTION (double, r32)
FUNCTION (double, r64)
FUNCTION (dcomplex, c64)
FUNCTION (dcomplex, c128)
#undef FUNCTION

/*
void NUMvector_writeBinary_r64 (const double *v, integer lo, integer hi, FILE *f);   // etc
	write the vector elements v [lo..hi] as machine-independent
	binary data to the stream f.
	Throw an error message if anything went wrong.
	The vectors need not have been created by NUM...vector.
double * NUMvector_readText_r64 (integer lo, integer hi, MelderReadText text, const char *name);   // etc
	create and read a vector as text.
	Throw an error message if anything went wrong.
	Every element is supposed to be on the beginning of a line.
double * NUMvector_readBinary_r64 (integer lo, integer hi, FILE *f);   // etc
	create and read a vector as machine-independent binary data from the stream f.
	Throw an error message if anything went wrong.
void NUMvector_writeText_r64 (const double *v, integer lo, integer hi, MelderFile file, conststring32 name);   // etc
	write the vector elements v [lo..hi] as text to the open file,
	each element on its own line, preceded by "name [index]: ".
	Throw an error message if anything went wrong.
	The vectors need not have been created by NUMvector.
void NUMmatrix_writeText_r64 (double **m, integer r1, integer r2, integer c1, integer c2, MelderFile file, conststring32 name);   // etc
	write the matrix elements m [r1..r2] [c1..c2] as text to the open file.
	Throw an error message if anything went wrong.
	The matrices need not have been created by NUMmatrix.
void NUMmatrix_writeBinary_r64 (double **m, integer r1, integer r2, integer c1, integer c2, FILE *f);   // etc
	write the matrix elements m [r1..r2] [c1..c2] as machine-independent
	binary data to the stream f.
	Throw an error message if anything went wrong.
	The matrices need not have been created by NUMmatrix.
double ** NUMmatrix_readText_r64 (integer r1, integer r2, integer c1, integer c2, MelderReadText text, const char *name);   // etc
	create and read a matrix as text.
	Throw an error message if anything went wrong.
double ** NUMmatrix_readBinary_r64 (integer r1, integer r2, integer c1, integer c2, FILE *f);   // etc
	create and read a matrix as machine-independent binary data from the stream f.
	Throw an error message if anything went wrong.
*/

/* End of file melder_tensorio.h */
#endif
