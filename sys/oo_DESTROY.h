/* oo_DESTROY.h
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

#define oo_SIMPLE(type,storage,x)

#define oo_ARRAY(type,storage,x,cap,n)

#define oo_SET(type,storage,x,setType)

#define oo_VECTOR(type,storage,x,min,max)  \
	NUMvector_free <type> (x, min);

#define oo_MATRIX(type,storage,x,row1,row2,col1,col2)  \
	NUMmatrix_free <type> (x, row1, col1);


#define oo_ENUMx(type,storage,Type,x)

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)

#define oo_ENUMx_SET(type,storage,Type,x,setType)

#define oo_ENUMx_VECTOR(type,storage,Type,x,min,max)  \
	NUMvector_free <type> (x, min);

#define oo_STRINGx(storage,x)  \
	Melder_free (x);

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		Melder_free (x [i]);

#define oo_STRINGx_SET(storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		Melder_free (x [i]);

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	if (x) { \
		for (long i = min; i <= max; i ++) \
			Melder_free (x [i]); \
		NUMvector_free <wchar_t*> (x, min); \
	}

#define oo_STRUCT(Type,x)  \
	x. destroy ();

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		x [i]. destroy ();

#define oo_STRUCT_SET(Type,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		x [i]. destroy ();

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (x) { \
		for (long i = min; i <= max; i ++) \
			x [i]. destroy (); \
		NUMvector_free <struct##Type> (x, min); \
	}

#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  \
	if (x) { \
		for (long i = row1; i <= row2; i ++) \
			for (long j = col1; j <= col2; j ++) \
				x [i] [j] -> destroy (); \
		NUMmatrix_free <struct##Type> (x, row1, col1); \
	}

#define oo_OBJECT(Class,version,x)  \
	forget (x);

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	forget (x);

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Struct)  \
	void struct##Struct :: destroy () {

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	void struct##Class :: v_destroy () { \

#define oo_END_CLASS(Class)  \
		Class##_Parent :: v_destroy (); \
	}

#define oo_IF(condition)  \
	if (condition) {

#define oo_ENDIF  \
	}

#define oo_FROM(from)

#define oo_ENDFROM

#define oo_VERSION(version)

#define oo_DECLARING  0
#define oo_DESTROYING  1
#define oo_COPYING  0
#define oo_EQUALLING  0
#define oo_COMPARING  0
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

/* End of file oo_DESTROY.h */
