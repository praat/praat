/* oo_WRITE_TEXT.h
 *
 * Copyright (C) 1994-2005,2007-2009,2011-2020 Paul Boersma
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

#include "oo_undef.h"

#define oo_SIMPLE(type, storage, x)  \
	texput##storage (_file_, our x, U"" #x);

#define oo_SET(type, storage, x, setType)  \
	texputintro (_file_, U"" #x U" []:"); \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		texput##storage (_file_, our x [_i], U"" #x U" [", setType##_getText ((setType) _i), U"]"); \
	} \
	texexdent (_file_);

#define oo_ANYVEC(type, storage, x, sizeExpression)  \
	{ \
		integer _size = (sizeExpression); \
		Melder_assert (our x.size == _size); \
		vector_writeText_##storage (our x.get(), _file_, U"" #x); \
	}

#define oo_ANYMAT(type, storage, x, nrowExpression, ncolExpression)  \
	{ \
		integer _nrow = (nrowExpression), _ncol = (ncolExpression); \
		Melder_assert (our x.nrow == _nrow && our x.ncol == _ncol); \
		matrix_writeText_##storage (our x.get(), _file_, U"" #x); \
	}

#define oo_ANYTEN3(type, storage, x, ndim1Expression, ndim2Expression, ndim3Expression)  \
	{ \
		integer _ndim1 = (ndim1Expression), _ndim2 = (ndim2Expression), _ndim3 = (ndim3Expression); \
		tensor3_writeText_##storage (our x.get(), _file_, U"" #x); \
	}

#define oo_ENUMx(kType, storage, x)  \
	texput##storage (_file_, (int) our x, (const char32* (*) (int)) kType##_getText, U"" #x);

//#define oo_ENUMx_SET(kType, storage, x, setType)  \
//	texputintro (_file_, U"" #x U" []: ", n ? nullptr : U"(empty)"); \
//	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
//		texput##storage (_file_, (int) our x [_i], (const char32* (*) (int)) kType##_getText, U"" #x U" [", setType##_getText ((setType) _i), U"]"); \
//	} \
//	texexdent (file);

#define oo_STRINGx(storage, x)  \
	texput##storage (_file_, our x.get(), U""#x);

#define oo_STRINGx_SET(storage, x, setType)  \
	texputintro (_file_, U"" #x U" []:"); \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		texput##storage (_file_, our x [_i].get(), U"" #x U" [", setType##_getText ((setType) _i), U"]"); \
	} \
	texexdent (_file_);

#define oo_STRINGx_VECTOR(storage, x, n)  \
	{ \
		integer _size = (n); \
		Melder_assert (_size == our x.size); \
		texputintro (_file_, U"" #x U" []: ", _size >= 1 ? nullptr : U"(empty)"); \
		for (integer _i = 1; _i <= _size; _i ++) { \
			texput##storage (_file_, our x [_i].get(), U"" #x U" [", Melder_integer (_i), U"]"); \
		} \
		texexdent (_file_); \
	}

#define oo_STRUCT(Type, x)  \
	texputintro (_file_, U"" #x U":"); \
	our x. writeText (_file_); \
	texexdent (_file_);

#define oo_STRUCT_SET(Type, x, setType)  \
	texputintro (_file_, U"" #x U" []:"); \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		texputintro (_file_, U"" #x U" [", setType##_getText ((setType) _i), U"]:"); \
		our x [_i]. writeText (_file_); \
		texexdent (_file_); \
	} \
	texexdent (_file_);

#define oo_STRUCTVEC(Type, x, n)  \
{ \
	integer _size = (n); \
	Melder_assert (our x.size == _size); \
	texputintro (_file_, U"" #x U" []: ", _size >= 1 ? nullptr : U"(empty)"); \
	for (integer _i = 1; _i <= _size; _i ++) { \
		texputintro (_file_, U"" #x U" [", Melder_integer (_i), U"]:"); \
		our x [_i]. writeText (_file_); \
		texexdent (_file_); \
	} \
	texexdent (_file_); \
}

#define oo_OBJECT(Class, version, x)  \
	texputex (_file_, !! our x, U"" #x); \
	if (our x) { \
		Data_writeText (our x.get(), _file_); \
	}

#define oo_COLLECTION_OF(Class, x, ItemClass, version)  \
	texputinteger (_file_, our x.size, U"" #x U": size"); \
	for (integer _i = 1; _i <= our x.size; _i ++) { \
		ItemClass data = our x.at [_i]; \
		texputintro (_file_, U"" #x U" [", Melder_integer (_i), U"]:"); \
		data -> struct##ItemClass :: v_writeText (_file_); \
		texexdent (_file_); \
	}

#define oo_COLLECTION(Class, x, ItemClass, version)  \
	texputinteger (_file_, our x ? our x->size : 0, U"" #x U": size"); \
	if (our x) { \
		for (integer _i = 1; _i <= our x->size; _i ++) { \
			ItemClass data = our x->at [_i]; \
			texputintro (_file_, U"" #x U" [", Melder_integer (_i), U"]:"); \
			data -> struct##ItemClass :: v_writeText (_file_); \
			texexdent (_file_); \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type)  \
	void struct##Type :: writeText (MelderFile _file_) {

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class, Parent)  \
	void struct##Class :: v_writeText (MelderFile _file_) { \
		Class##_Parent :: v_writeText (_file_);

#define oo_END_CLASS(Class)  \
	}

#define oo_FROM(from)

#define oo_ENDFROM

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  1
#define oo_WRITING_TEXT  1
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo_WRITE_TEXT.h */
