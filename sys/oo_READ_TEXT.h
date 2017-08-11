/* oo_READ_TEXT.h
 *
 * Copyright (C) 1994-2012,2013,2014,2015,2016,2017 Paul Boersma
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
		our x = texget##storage (a_text); \
	} catch (MelderError) { \
		Melder_throw (U"\"" #x U"\" not read."); \
	}

#define oo_ARRAY(type,storage,x,cap,n)  \
	if (n > cap) Melder_throw (U"Number of \"" #x U"\" (", n, U") greater than ", cap, U"."); \
	for (long i = 0; i < n; i ++) { \
		try { \
			our x [i] = texget##storage (a_text); \
		} catch (MelderError) { \
			Melder_throw (U"Element ", i+1, U" of \"" #x U"\" not read."); \
		} \
	}

#define oo_SET(type,storage,x,setType)  \
	for (long i = 0; i <= setType##_MAX; i ++) { \
		try { \
			our x [i] = texget##storage (a_text); \
		} catch (MelderError) { \
			Melder_throw (U"Element ", i+1, U" of \"" #x, U"\" not read."); \
		} \
	}

#define oo_VECTOR(type,storage,x,min,max)  \
	if (max >= min) { \
		our x = NUMvector_readText_##storage (min, max, a_text, #x); \
	}

#define oo_MATRIX(type,storage,x,row1,row2,col1,col2)  \
	if (row2 >= row1 && col2 >= col1) { \
	    our x = NUMmatrix_readText_##storage (row1, row2, col1, col2, a_text, #x); \
	}

#define oo_ENUMx(type,storage,Type,x)  \
	our x = texget##storage (a_text, Type##_getValue);

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	if (n > cap) Melder_throw (U"Number of \"" #x U"\" (", n, U") greater than ", cap, U"."); \
	for (long i = 0; i < n; i ++) { \
		our x [i] = texget##storage (a_text, Type##_getValue); \
	}

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	for (long i = 0; i <= setType##_MAX; i ++) { \
		our x [i] = texget##storage (a_text, & Type##_getValue); \
	}

#define oo_ENUMx_VECTOR(type,storage,Type,x,min,max)  \
	if (max >= min) { \
		our x = NUMvector <type> (min, max); \
		for (long i = min; i <= max; i ++) { \
			our x [i] = texget##storage (a_text, & Type##_getValue); \
		} \
	}

#define oo_STRINGx(storage,x)  \
	try { \
		our x = texget##storage (a_text); \
	} catch (MelderError) { \
		Melder_throw (U"String \"" #x U"\" not read."); \
	}

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	if (n > cap) Melder_throw (U"Number of \"" #x U"\" (", n, U") greater than ", cap, U"."); \
	for (long i = 0; i < n; i ++) { \
		our x [i] = texget##storage (a_text); \
	}

#define oo_STRINGx_SET(storage,x,setType)  \
	for (long i = 0; i <= setType##_MAX; i ++) { \
		our x [i] = texget##storage (a_text); \
	}

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	if (max >= min) { \
		our x = NUMvector <char32*> (min, max); \
		for (long i = min; i <= max; i ++) { \
			try { \
				our x [i] = texget##storage (a_text); \
			} catch (MelderError) { \
				Melder_throw (U"Element ", i, U" of \"" #x U"\" not read."); \
			} \
		} \
	}

#define oo_STRUCT(Type,x)  \
	our x. readText (a_text, formatVersion);

#define oo_STRUCT_ARRAY(Type,x,cap,n) \
	if (n > cap) Melder_throw (U"Number of \"" #x U"\" (", n, U") greater than ", cap, U"."); \
	for (long i = 0; i < n; i ++) { \
		our x [i]. readText (a_text, formatVersion); \
	}

#define oo_STRUCT_SET(Type,x,setType) \
	for (long i = 0; i <= setType##_MAX; i ++) { \
		our x [i]. readText (a_text, formatVersion); \
	}

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (max >= min) { \
		our x = NUMvector <struct##Type> (min, max); \
		for (long i = min; i <= max; i ++) { \
			our x [i]. readText (a_text, formatVersion); \
		} \
	}

#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  \
	if (row2 >= row1 && col2 >= col1) { \
		our x = NUMmatrix <struct##Type> (row1, row2, col1, col2); \
		for (long i = row1; i <= row2; i ++) { \
			for (long j = col1; j <= col2; j ++) { \
				our x [i] [j]. readText (a_text, formatVersion); \
			} \
		} \
	}

#define oo_AUTO_OBJECT(Class,formatVersion,x)  \
	if (texgetex (a_text) == 1) { \
		our x = Thing_new (Class); \
		our x -> v_readText (a_text, formatVersion); \
	}

#define oo_COLLECTION_OF(Class,x,ItemClass,formatVersion)  \
	{ \
		long n = texgeti32 (a_text); \
		for (long i = 1; i <= n; i ++) { \
			auto##ItemClass item = Thing_new (ItemClass); \
			item -> v_readText (a_text, formatVersion); \
			our x.addItem_move (item.move()); \
		} \
	}

#define oo_AUTO_COLLECTION(Class,x,ItemClass,formatVersion)  \
	{ \
		long n = texgeti32 (a_text); \
		our x = Class##_create (); \
		for (long i = 1; i <= n; i ++) { \
			auto##ItemClass item = Thing_new (ItemClass); \
			item -> v_readText (a_text, formatVersion); \
			our x -> addItem_move (item.move()); \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type)  \
	void struct##Type :: readText (MelderReadText a_text, int formatVersion) { \
		(void) formatVersion;

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	void struct##Class :: v_readText (MelderReadText a_text, int formatVersion) { \
		if (formatVersion > our classInfo -> version) \
			Melder_throw (U"The format of this file is too new. Download a newer version of Praat."); \
		Class##_Parent :: v_readText (a_text, formatVersion);

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
