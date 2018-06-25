/* oo_READ_TEXT.h
 *
 * Copyright (C) 1994-2009,2011-2018 Paul Boersma
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
	try { \
		our x = texget##storage (_textSource_); \
	} catch (MelderError) { \
		Melder_throw (U"\"" #x U"\" not read."); \
	}

#define oo_ARRAY(type,storage,x,cap,n)  \
	if (n > cap) Melder_throw (U"Number of \"" #x U"\" (", n, U") greater than ", cap, U"."); \
	for (integer _i = 0; _i < n; _i ++) { \
		try { \
			our x [_i] = texget##storage (_textSource_); \
		} catch (MelderError) { \
			Melder_throw (U"Element ", _i+1, U" of \"" #x U"\" not read."); \
		} \
	}

#define oo_SET(type,storage,x,setType)  \
	for (integer _i = 0; _i <= (int) setType::MAX; _i ++) { \
		try { \
			our x [_i] = texget##storage (_textSource_); \
		} catch (MelderError) { \
			Melder_throw (U"Element ", _i+1, U" of \"" #x, U"\" not read."); \
		} \
	}

#define oo_VECTOR(type,storage,x,min,max)  \
	if (max >= min) { \
		our x = NUMvector_readText_##storage (min, max, _textSource_, #x); \
	}

#define oo_MATRIX(type,storage,x,row1,row2,col1,col2)  \
	if (row2 >= row1 && col2 >= col1) { \
	    our x = NUMmatrix_readText_##storage (row1, row2, col1, col2, _textSource_, #x); \
	}

#define oo_ENUMx(kType,storage,x)  \
	our x = (kType) texget##storage (_textSource_, (enum_generic_getValue) kType##_getValue);

//#define oo_ENUMx_ARRAY(kType,storage,x,cap,n)  \
//	if (n > cap) Melder_throw (U"Number of \"" #x U"\" (", n, U") greater than ", cap, U"."); \
//	for (integer _i = 0; _i < n; _i ++) { \
//		our x [_i] = (kType) texget##storage (_textSource_, (enum_generic_getValue) kType##_getValue); \
//	}

//#define oo_ENUMx_SET(kType,storage,x,setType)  \
//	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
//		our x [_i] = (kType) texget##storage (_textSource_, (enum_generic_getValue) kType##_getValue); \
//	}

//#define oo_ENUMx_VECTOR(kType,storage,x,min,max)  \
//	if (max >= min) { \
//		our x = NUMvector <kType> (min, max); \
//		for (integer _i = min; _i <= max; _i ++) { \
//			our x [_i] = (kType) texget##storage (_textSource_, (enum_generic_getValue) kType##_getValue); \
//		} \
//	}

#define oo_STRINGx(storage,x)  \
	try { \
		our x = texget##storage (_textSource_); \
	} catch (MelderError) { \
		Melder_throw (U"String \"" #x U"\" not read."); \
	}

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	if (n > cap) Melder_throw (U"Number of \"" #x U"\" (", n, U") greater than ", cap, U"."); \
	for (integer _i = 0; _i < n; _i ++) { \
		our x [i] = texget##storage (_textSource_); \
	}

#define oo_STRINGx_SET(storage,x,setType)  \
	for (integer _i = 0; _i <= setType::MAX; _i ++) { \
		our x [_i] = texget##storage (_textSource_); \
	}

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	if (max >= min) { \
		our x = NUMvector <char32*> (min, max); \
		for (integer _i = min; _i <= max; _i ++) { \
			try { \
				our x [_i] = texget##storage (_textSource_); \
			} catch (MelderError) { \
				Melder_throw (U"Element ", _i, U" of \"" #x U"\" not read."); \
			} \
		} \
	}

#define oo_STRUCT(Type,x)  \
	our x. readText (_textSource_, formatVersion);

#define oo_STRUCT_ARRAY(Type,x,cap,n) \
	if (n > cap) Melder_throw (U"Number of \"" #x U"\" (", n, U") greater than ", cap, U"."); \
	for (integer _i = 0; _i < n; _i ++) { \
		our x [_i]. readText (_textSource_, formatVersion); \
	}

#define oo_STRUCT_SET(Type,x,setType) \
	for (integer _i = 0; _i <= (int) setType::MAX; _i ++) { \
		our x [_i]. readText (_textSource_, formatVersion); \
	}

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (max >= min) { \
		our x = NUMvector <struct##Type> (min, max); \
		for (integer _i = min; _i <= max; _i ++) { \
			our x [_i]. readText (_textSource_, formatVersion); \
		} \
	}

#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  \
	if (row2 >= row1 && col2 >= col1) { \
		our x = NUMmatrix <struct##Type> (row1, row2, col1, col2); \
		for (integer _irow = row1; _irow <= row2; _irow ++) { \
			for (integer _icol = col1; _icol <= col2; _icol ++) { \
				our x [_irow] [_icol]. readText (_textSource_, formatVersion); \
			} \
		} \
	}

#define oo_OBJECT(Class,formatVersion,x)  \
	if (texgetex (_textSource_) == 1) { \
		our x = Thing_new (Class); \
		our x -> v_readText (_textSource_, formatVersion); \
	}

#define oo_COLLECTION_OF(Class,x,ItemClass,formatVersion)  \
	{ \
		integer _n = texgetinteger (_textSource_); \
		for (integer _i = 1; _i <= _n; _i ++) { \
			auto##ItemClass _item = Thing_new (ItemClass); \
			_item -> v_readText (_textSource_, formatVersion); \
			our x.addItem_move (_item.move()); \
		} \
	}

#define oo_COLLECTION(Class,x,ItemClass,formatVersion)  \
	{ \
		integer _n = texgetinteger (_textSource_); \
		our x = Class##_create (); \
		for (integer _i = 1; _i <= _n; _i ++) { \
			auto##ItemClass _item = Thing_new (ItemClass); \
			_item -> v_readText (_textSource_, formatVersion); \
			our x -> addItem_move (_item.move()); \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type)  \
	void struct##Type :: readText (MelderReadText _textSource_, int formatVersion) { \
		(void) formatVersion;

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	void struct##Class :: v_readText (MelderReadText _textSource_, int formatVersion) { \
		if (formatVersion > our classInfo -> version) \
			Melder_throw (U"The format of this file is too new. Download a newer version of Praat."); \
		Class##_Parent :: v_readText (_textSource_, formatVersion);

#define oo_END_CLASS(Class)  \
	}

#define oo_IF(condition)  \
	if (condition) {

#define oo_ENDIF  \
	}

#define oo_FROM(from)  \
	if (formatVersion >= from) {

#define oo_ENDFROM  \
	}

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  1
#define oo_READING_TEXT  1
#define oo_READING_BINARY  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo_READ_TEXT.h */
