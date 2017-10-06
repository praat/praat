/* oo_EQUAL.h
 *
 * Copyright (C) 1994-2012,2013,2014,2015,2017 Paul Boersma
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
	if (our x != thy x) return false;

#define oo_ARRAY(type,storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		if (our x [i] != thy x [i]) return false; \

#define oo_SET(type,storage,x,setType)  \
	for (int i = 0; i <= (int) setType::MAX; i ++) \
		if (our x [i] != thy x [i]) return false; \

#define oo_VECTOR(type,storage,x,min,max)  \
	if (! our x != ! thy x || \
		(our x && ! NUMvector_equal <type> (our x, thy x, min, max))) return false;

#define oo_MATRIX(type,storage,x,row1,row2,col1,col2)  \
	if (! our x != ! thy x || \
		(our x && ! NUMmatrix_equal <type> (our x, thy x, row1, row2, col1, col2))) return false;

#define oo_ENUMx(kType,storage,x)  \
	if (our x != thy x) return false;

//#define oo_ENUMx_ARRAY(kType,storage,x,cap,n)  \
//	for (int i = 0; i < n; i ++) if (our x [i] != thy x [i]) return false;

//#define oo_ENUMx_SET(kType,storage,x,setType)  \
//	for (int i = 0; i <= (int) setType::MAX; i ++) if (our x [i] != thy x [i]) return false;

//#define oo_ENUMx_VECTOR(kType,storage,x,min,max)  \
//	if (! our x != ! thy x || \
//		(our x && ! NUMvector_equal <type> (our x, thy x, min, max))) return false;

#define oo_STRINGx(storage,x)  \
	if (! Melder_equ (our x, thy x)) return false;

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		if (! Melder_equ (our x [i], thy x [i])) return false;

#define oo_STRINGx_SET(storage,x,setType)  \
	for (int i = 0; i <= setType::MAX; i ++) \
		if (! Melder_equ (our x [i], thy x [i])) return false;

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	if (! our x != ! thy x) return false; \
	if (our x) { \
		for (integer i = min; i <= max; i ++) \
			if (! Melder_equ (our x [i], thy x [i])) return false; \
	}

#define oo_STRUCT(Type,x)  \
	if (! our x. equal (& thy x)) return false;

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		if (! our x [i]. equal (& thy x [i])) return false;

#define oo_STRUCT_SET(Type,x,setType)  \
	for (int i = 0; i <= (int) setType::MAX; i ++) \
		if (! our x [i]. equal (& thy x [i])) return false;

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (! our x != ! thy x) return false; \
	if (our x) { \
		for (integer i = min; i <= max; i ++) \
			if (! our x [i]. equal (& thy x [i])) return false; \
	}

#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  \
	if (! our x != ! thy x) return false; \
	if (our x) { \
		for (integer i = row1; i <= row2; i ++) \
			for (integer j = col1; j <= col2; j ++) \
				if (! our x [i] [j]. equal (& thy x [i] [j])) return false; \
	}

#define oo_AUTO_OBJECT(Class,version,x)  \
	if (! our x != ! thy x || (our x && ! Data_equal (our x.get(), thy x.get()))) return false;

#define oo_COLLECTION_OF(Class,x,ItemClass,version)  \
	for (integer i = 1; i <= our x.size; i ++) { \
		if (! our x.at [i] != ! thy x.at [i] || (our x.at [i] && ! Data_equal (our x.at [i], thy x.at [i]))) return false; \
	}

#define oo_AUTO_COLLECTION(Class,x,ItemClass,version)  \
	if (! our x != ! thy x || (our x && ! Data_equal (our x.get(), thy x.get()))) return false;

#define oo_FILE(x)  \
	if (! MelderFile_equal (& our x, & thy x)) return false;

#define oo_DIR(x)  \
	if (! MelderDir_equal (& our x, & thy x)) return false;

#define oo_DEFINE_STRUCT(Type)  \
	bool struct##Type :: equal (Type thee) {

#define oo_END_STRUCT(Type)  \
		return true; \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	bool struct##Class :: v_equal (Daata thee_Daata) { \
		Class thee = static_cast <Class> (thee_Daata); \
		if (! Class##_Parent :: v_equal (thee)) return false;

#define oo_END_CLASS(Class)  \
		return true; \
	}

#define oo_IF(condition)  \
	if (condition) {

#define oo_ENDIF  \
	}

#define oo_FROM(from)

#define oo_ENDFROM

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_COMPARING  1
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo_EQUAL.h */
