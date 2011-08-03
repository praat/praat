/* oo_WRITE_CACHE.h
 *
 * Copyright (C) 1994-2011 Paul Boersma
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
	cacput##storage (my x, f);

#define oo_ARRAY(type,storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		cacput##storage (my x [i], f);

#define oo_SET(type,storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		cacput##storage (my x [i], f);

#define oo_VECTOR(type,t,storage,x,min,max)  \
	if (my x) \
		NUM##t##vector_writeCache_##storage (my x, min, max, f);

#define oo_MATRIX(type,t,storage,x,row1,row2,col1,col2)  \
	if (my x) \
		NUM##t##matrix_writeCache_##storage (my x, row1, row2, col1, col2, f);

#define oo_ENUMx(type,storage,Type,x)  \
	cacput##storage (my x, f, & enum_##Type);

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		cacput##storage (my x [i], f, & enum_##Type);

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		cacput##storage (my x [i], f, & enum_##Type);

#define oo_ENUMx_VECTOR(type,t,storage,Type,x,min,max)  \
	if (my x) \
		NUM##t##vector_writeCache_##storage (my x, min, max, f);

#define oo_STRINGx(storage,x)  \
	cacput##storage (my x, f);

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		cacput##storage (my x [i], f);

#define oo_STRINGx_SET(storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		cacput##storage (my x [i], f);

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	if (max >= min) \
		for (long i = min; i <= max; i ++) \
			cacput##storage (my x [i], f);

#define oo_STRUCT(Type,x)  \
	Type##_writeCache (& my x, f);

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		Type##_writeCache (& my x [i], f);

#define oo_STRUCT_SET(Type,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		Type##_writeCache (& my x [i], f);

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (max >= min) \
		for (long i = min; i <= max; i ++) \
			Type##_writeCache (& my x [i], f);

#define oo_OBJECT(Class,version,x)  \
	cacputex (my x != NULL, f); \
	if (my x) \
		Data_writeCache (my x, f);

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	cacputi4 (my x ? my x -> size : 0, f); \
	if (my x) { \
		for (long i = 1; i <= my x -> size; i ++) { \
			ItemClass data = (ItemClass) my x -> item [i]; \
			class##ItemClass -> writeCache (data, f); \
		} \
	}

#define oo_DEFINE_STRUCT(Type)  \
	static void Type##_writeCache (Type me, CACHE *f) {

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	static void class##Class##_writeCache (I, CACHE *f) { \
		iam (Class); \
		inherited (Class) writeCache (me, f);

#define oo_END_CLASS(Class)  \
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
#define oo_EQUALLING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_READING_CACHE  0
#define oo_READING_LISP  0
#define oo_WRITING  1
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_WRITING_CACHE  1
#define oo_WRITING_LISP  0
#define oo_DESCRIBING  0

/* End of file oo_WRITE_CACHE.h */
