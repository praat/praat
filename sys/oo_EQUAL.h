/* oo_EQUAL.h
 *
 * Copyright (C) 1994-2009 Paul Boersma
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
 * pb 2003/02/07 added oo_FILE and oo_DIR
 * pb 2003/06/11 made struct_equal global
 * pb 2006/05/29 added version to oo_OBJECT and oo_COLLECTION
 * pb 2006/08/08 reduced Lint warnings
 * pb 2007/06/09 wchar_t
 * pb 2009/01/22 consider empty strings and null strings identical
 * pb 2009/03/21 modern enums
 */

#include "oo_undef.h"

#define oo_SIMPLE(type,storage,x)  \
	if (my x != thy x) return 0;

#define oo_ARRAY(type,storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		if (my x [i] != thy x [i]) return 0; \

#define oo_SET(type,storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		if (my x [i] != thy x [i]) return 0; \

#define oo_VECTOR(type,t,storage,x,min,max)  \
	if (! my x != ! thy x || \
		(my x && ! NUM##t##vector_equal (my x, thy x, min, max))) return 0;

#define oo_MATRIX(type,t,storage,x,row1,row2,col1,col2)  \
	if (! my x != ! thy x || \
		(my x && ! NUM##t##matrix_equal (my x, thy x, row1, row2, col1, col2))) return 0;

#define oo_ENUMx(type,storage,Type,x)  \
	if (my x != thy x) return 0;

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	for (int i = 0; i < n; i ++) if (my x [i] != thy x [i]) return 0;

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) if (my x [i] != thy x [i]) return 0;

#define oo_ENUMx_VECTOR(type,t,storage,Type,x,min,max)  \
	if (! my x != ! thy x || \
		(my x && ! NUM##t##vector_equal (my x, thy x, min, max))) return 0;

#define oo_STRINGx(storage,x)  \
	if (! Melder_strequ (my x, thy x)) return 0;

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		if (! Melder_strequ (my x [i], thy x [i])) return 0;

#define oo_STRINGx_SET(storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		if (! Melder_strequ (my x [i], thy x [i])) return 0;

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	if (! my x != ! thy x) return 0; \
	if (my x) { \
		for (long i = min; i <= max; i ++) \
			if (! Melder_strequ (my x [i], thy x [i])) return 0; \
	}

#define oo_STRINGWx(storage,x)  \
	if (! Melder_wcsequ (my x, thy x)) return 0;

#define oo_STRINGWx_ARRAY(storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		if (! Melder_wcsequ (my x [i], thy x [i])) return 0;

#define oo_STRINGWx_SET(storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		if (! Melder_wcsequ (my x [i], thy x [i])) return 0;

#define oo_STRINGWx_VECTOR(storage,x,min,max)  \
	if (! my x != ! thy x) return 0; \
	if (my x) { \
		for (long i = min; i <= max; i ++) \
			if (! Melder_wcsequ (my x [i], thy x [i])) return 0; \
	}

#define oo_STRUCT(Type,x)  \
	if (! Type##_equal (& my x, & thy x)) return 0;

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		if (! Type##_equal (& my x [i], & thy x [i])) return 0;

#define oo_STRUCT_SET(Type,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		if (! Type##_equal (& my x [i], & thy x [i])) return 0;

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (! my x != ! thy x) return 0; \
	if (my x) { \
		for (long i = min; i <= max; i ++) \
			if (! Type##_equal (& my x [i], & thy x [i])) return 0; \
	}

#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  \
	if (! my x != ! thy x) return 0; \
	if (my x) { \
		for (long i = row1; i <= row2; i ++) \
			for (long j = col1; j <= col2; j ++) \
				if (! Type##_equal (& my x [i] [j], & thy x [i] [j])) return 0; \
	}

#define oo_WIDGET(x)  \
	!!!! Can never compare Widgets... !!!!

#define oo_WIDGET_ARRAY(x,cap,n)  \
	!!!! Can never compare Widgets... !!!!

#define oo_WIDGET_SET(x,setType)  \
	!!!! Can never compare Widgets... !!!!

#define oo_WIDGET_VECTOR_FROM(x,cap,min,max)  \
	!!!! Can never compare Widgets... !!!!



#define oo_OBJECT(Class,version,x)  \
	if (! my x != ! thy x || (my x && ! Data_equal (my x, thy x))) return 0;

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	if (! my x != ! thy x || (my x && ! Data_equal (my x, thy x))) return 0;

#define oo_FILE(x)  \
	if (! MelderFile_equal (& my x, & thy x)) return 0;

#define oo_DIR(x)  \
	if (! MelderDir_equal (& my x, & thy x)) return 0;



#define oo_DEFINE_STRUCT(Type)  \
	bool Type##_equal (Type me, Type thee) {

#define oo_END_STRUCT(Type)  \
		return 1; \
	}



#define oo_DEFINE_CLASS(Class,Parent)  \
	static bool class##Class##_equal (I, thou) { \
		iam (Class); \
		thouart (Class); \
		if (! inherited (Class) equal (me, thee)) return 0;

#define oo_END_CLASS(Class)  \
		return 1; \
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
