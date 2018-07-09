/* oo_WRITE_TEXT.h
 *
 * Copyright (C) 1994-2005,2007-2009,2011-2018 Paul Boersma
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

#define oo_SIMPLE(type,storage,x)  \
	texput##storage (file, our x, U"" #x, 0,0,0,0,0);

#define oo_ARRAY(type,storage,x,cap,n)  \
	{ \
		integer _n = (n); \
		texputintro (file, U"" #x U" []: ", n ? nullptr : U"(empty)", 0,0,0,0); \
		for (integer _i = 0; _i < _n; _i ++) { \
			texput##storage (file, our x [_i], U"" #x " [", Melder_integer (_i), U"]", 0,0,0); \
		} \
		texexdent (file); \
	}

#define oo_SET(type,storage,x,setType)  \
	texputintro (file, U"" #x U" []:", 0,0,0,0,0); \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		texput##storage (file, our x [_i], U"" #x U" [", setType##_getText ((setType) _i), U"]", 0,0,0); \
	} \
	texexdent (file);

#define oo_VECTOR(type,storage,x,min,max)  \
	{ \
		integer _min = (min), _max = (max); \
		if (our x) { \
			NUMvector_writeText_##storage (our x, _min, _max, file, U"" #x); \
		} \
	}

#define oo_MATRIX(type,storage,x,row1,row2,col1,col2)  \
	{ \
		integer _row1 = (row1), _row2 = (row2), _col1 = (col1), _col2 = (col2); \
		if (our x) { \
			NUMmatrix_writeText_##storage (our x, _row1, _row2, _col1, _col2, file, U"" #x); \
		} \
	}

#define oo_ENUMx(kType,storage,x)  \
	texput##storage (file, (int) our x, (const char32* (*) (int)) kType##_getText, U"" #x, 0,0,0,0,0);

//#define oo_ENUMx_ARRAY(kType,storage,x,cap,n)  \
//	{ \
//		integer _n = (n); \
//		texputintro (file, U"" #x U" []:", 0,0,0,0,0); \
//		for (integer _i = 0; _i < _n; _i ++) { \
//			texput##storage (file, (int) our x [_i], (const char32* (*) (int)) kType##_getText, U"" #x U" [", Melder_integer (_i), U"]", 0,0,0); \
//		} \
//		texexdent (file); \
//	}

//#define oo_ENUMx_SET(kType,storage,x,setType)  \
//	texputintro (file, U"" #x U" []: ", n ? nullptr : U"(empty)", 0,0,0,0); \
//	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
//		texput##storage (file, (int) our x [_i], (const char32* (*) (int)) kType##_getText, U"" #x U" [", setType##_getText ((setType) _i), U"]", 0,0,0); \
//	} \
//	texexdent (file);

//#define oo_ENUMx_VECTOR(kType,storage,x,min,max)  \
//	{ \
//		integer _min = (min), _max = (max); \
//		texputintro (file, U"" #x U" []: ", _max >= _min ? nullptr : U"(empty)", 0,0,0,0); \
//		for (integer _i = _min; _i <= _max; _i ++) { \
//			texput##storage (file, (int) our x [_i], (const char32* (*) (int)) kType##_getText, U"" #x U" [", Melder_integer (_i), U"]", 0,0,0); \
//		} \
//		texexdent (file); \
//	}

#define oo_STRINGx(storage,x)  \
	texput##storage (file, our x.get(), U""#x, 0,0,0,0,0);

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	{ \
		integer _n = (n); \
		texputintro (file, U"" #x U" []: ", _n ? nullptr : U"(empty)", 0,0,0,0); \
		for (integer _i = 0; _i < _n; _i ++) { \
			texput##storage (file, our x [_i].get(), U"" #x U" [", Melder_integer (_i), U"]", 0,0,0); \
		} \
		texexdent (file); \
	}

#define oo_STRINGx_SET(storage,x,setType)  \
	texputintro (file, U"" #x U" []:", 0,0,0,0,0); \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		texput##storage (file, our x [_i].get(), U"" #x U" [", setType##_getText ((setType) _i), U"]", 0,0,0); \
	} \
	texexdent (file);

#define oo_STRINGx_VECTOR(storage,x,size)  \
	{ \
		integer _size = (size); \
		texputintro (file, U"" #x U" []: ", _size >= 1 ? nullptr : U"(empty)", 0,0,0,0); \
		for (integer _i = 1; _i <= _size; _i ++) { \
			texput##storage (file, our x [_i].get(), U"" #x U" [", Melder_integer (_i), U"]", 0,0,0); \
		} \
		texexdent (file); \
	}

#define oo_STRUCT(Type,x)  \
	texputintro (file, U"" #x U":", 0,0,0,0,0); \
	our x. writeText (file); \
	texexdent (file);

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	{ \
		integer _n = (n); \
		texputintro (file, U"" #x U" []: ", n ? nullptr : U"(empty)", 0,0,0,0); \
		for (integer _i = 0; _i < _n; _i ++) { \
			texputintro (file, U"" #x " [", Melder_integer (_i), U"]:", 0,0,0); \
			our x [_i]. writeText (file); \
			texexdent (file); \
		} \
		texexdent (file); \
	}

#define oo_STRUCT_SET(Type,x,setType)  \
	texputintro (file, U"" #x U" []:", 0,0,0,0,0); \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		texputintro (file, U"" #x U" [", setType##_getText ((setType) _i), U"]:", 0,0,0); \
		our x [_i]. writeText (file); \
		texexdent (file); \
	} \
	texexdent (file);

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	{ \
		integer _min = (min), _max = (max); \
		texputintro (file, U"" #x U" []: ", _max >= _min ? nullptr : U"(empty)", 0,0,0,0); \
		for (integer _i = _min; _i <= _max; _i ++) { \
			texputintro (file, U"" #x U" [", Melder_integer (_i), U"]:", 0,0,0); \
			our x [_i]. writeText (file); \
			texexdent (file); \
		} \
		texexdent (file); \
	}

#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  \
	{ \
		integer _row1 = (row1), _row2 = (row2), _col1 = (col1), _col2 = (col2); \
		texputintro (file, U"" #x U" [] []: ", _row2 >= _row1 ? nullptr : U"(empty)", 0,0,0,0); \
		for (integer _irow = _row1; _irow <= _row2; _irow ++) { \
			texputintro (file, U"" #x U" [", Melder_integer (_irow), U"]:", 0,0,0); \
			for (integer _icol = _col1; _icol <= _col2; _icol ++) { \
				texputintro (file, U"" #x U" [", Melder_integer (_irow), U"] [", Melder_integer (_icol), U"]:", 0); \
				our x [_irow] [_icol]. writeText (file); \
				texexdent (file); \
			} \
			texexdent (file); \
		} \
		texexdent (file); \
	}

#define oo_OBJECT(Class,version,x)  \
	texputex (file, !! our x, U"" #x, 0,0,0,0,0); \
	if (our x) { \
		Data_writeText (our x.get(), file); \
	}

#define oo_COLLECTION_OF(Class,x,ItemClass,version)  \
	texputinteger (file, our x.size, U"" #x U": size", 0,0,0,0,0); \
	for (integer _i = 1; _i <= our x.size; _i ++) { \
		ItemClass data = our x.at [_i]; \
		texputintro (file, U"" #x U" [", Melder_integer (_i), U"]:", 0,0,0); \
		data -> struct##ItemClass :: v_writeText (file); \
		texexdent (file); \
	}

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	texputinteger (file, our x ? our x->size : 0, U"" #x U": size", 0,0,0,0,0); \
	if (our x) { \
		for (integer _i = 1; _i <= our x->size; _i ++) { \
			ItemClass data = our x->at [_i]; \
			texputintro (file, U"" #x U" [", Melder_integer (_i), U"]:", 0,0,0); \
			data -> struct##ItemClass :: v_writeText (file); \
			texexdent (file); \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type)  \
	void struct##Type :: writeText (MelderFile file) {

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	void struct##Class :: v_writeText (MelderFile file) { \
		Class##_Parent :: v_writeText (file);

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
