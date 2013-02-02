/* oo_EQUAL.h
 *
 * Copyright (C) 1994-2012,2013 Paul Boersma
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

#include "oo_undef.h"

#define oo_SIMPLE(type,storage,x)  \
	if (x != thy x) return false;

#define oo_ARRAY(type,storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		if (x [i] != thy x [i]) return false; \

#define oo_SET(type,storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		if (x [i] != thy x [i]) return false; \

#define oo_VECTOR(type,storage,x,min,max)  \
	if (! x != ! thy x || \
		(x && ! NUMvector_equal <type> (x, thy x, min, max))) return false;

#define oo_MATRIX(type,storage,x,row1,row2,col1,col2)  \
	if (! x != ! thy x || \
		(x && ! NUMmatrix_equal <type> (x, thy x, row1, row2, col1, col2))) return false;

#define oo_ENUMx(type,storage,Type,x)  \
	if (x != thy x) return false;

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	for (int i = 0; i < n; i ++) if (x [i] != thy x [i]) return false;

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) if (x [i] != thy x [i]) return false;

#define oo_ENUMx_VECTOR(type,storage,Type,x,min,max)  \
	if (! x != ! thy x || \
		(x && ! NUMvector_equal <type> (x, thy x, min, max))) return false;

#define oo_STRINGx(storage,x)  \
	if (! Melder_wcsequ (x, thy x)) return false;

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		if (! Melder_wcsequ (x [i], thy x [i])) return false;

#define oo_STRINGx_SET(storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		if (! Melder_wcsequ (x [i], thy x [i])) return false;

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	if (! x != ! thy x) return false; \
	if (x) { \
		for (long i = min; i <= max; i ++) \
			if (! Melder_wcsequ (x [i], thy x [i])) return false; \
	}

#define oo_STRUCT(Type,x)  \
	if (! x. equal (& thy x)) return false;

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		if (! x [i]. equal (& thy x [i])) return false;

#define oo_STRUCT_SET(Type,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		if (! x [i]. equal (& thy x [i])) return false;

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (! x != ! thy x) return false; \
	if (x) { \
		for (long i = min; i <= max; i ++) \
			if (! x [i]. equal (& thy x [i])) return false; \
	}

#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  \
	if (! x != ! thy x) return false; \
	if (x) { \
		for (long i = row1; i <= row2; i ++) \
			for (long j = col1; j <= col2; j ++) \
				if (! x [i] [j]. equal (& thy x [i] [j])) return false; \
	}

#define oo_OBJECT(Class,version,x)  \
	if (! x != ! thy x || (x && ! Data_equal (x, thy x))) return false;

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	if (! x != ! thy x || (x && ! Data_equal (x, thy x))) return false;

#define oo_FILE(x)  \
	if (! MelderFile_equal (& x, & thy x)) return false;

#define oo_DIR(x)  \
	if (! MelderDir_equal (& x, & thy x)) return false;

#define oo_DEFINE_STRUCT(Type)  \
	bool struct##Type :: equal (Type thee) {

#define oo_END_STRUCT(Type)  \
		return true; \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	bool struct##Class :: v_equal (thou) { \
		thouart (Class); \
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

#define oo_VERSION(version)

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_EQUALLING  1
#define oo_COMPARING  1
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_READING_CACHE  0
#define oo_READING_LISP  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_WRITING_CACHE  0
#define oo_WRITING_LISP  0
#define oo_DESCRIBING  0

/* End of file oo_EQUAL.h */
