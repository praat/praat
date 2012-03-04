/* oo_WRITE_BINARY.h
 *
 * Copyright (C) 1994-2012 Paul Boersma
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
	binput##storage (x, f);

#define oo_ARRAY(type,storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		binput##storage (x [i], f);

#define oo_SET(type,storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		binput##storage (x [i], f);

#define oo_VECTOR(type,storage,x,min,max)  \
	if (x) \
		NUMvector_writeBinary_##storage (x, min, max, f);

#define oo_MATRIX(type,storage,x,row1,row2,col1,col2)  \
	if (x) \
		NUMmatrix_writeBinary_##storage (x, row1, row2, col1, col2, f);

#define oo_ENUMx(type,storage,Type,x)  \
	binput##storage (x, f);

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		binput##storage (x [i], f);

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		binput##storage (x [i], f);

#define oo_ENUMx_VECTOR(type,storage,Type,x,min,max)  \
	if (x) \
		NUMvector_writeBinary_##storage (x, min, max, f);

#define oo_STRINGx(storage,x)  \
	binput##storage (x, f);

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		binput##storage (x [i], f);

#define oo_STRINGx_SET(storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		binput##storage (x [i], f);

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	if (max >= min) \
		for (long i = min; i <= max; i ++) \
			binput##storage (x [i], f);

#define oo_STRUCT(Type,x)  \
	x. writeBinary (f);

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		x [i]. writeBinary (f);

#define oo_STRUCT_SET(Type,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		x [i]. writeBinary (f);

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (max >= min) \
		for (long i = min; i <= max; i ++) \
			x [i]. writeBinary (f);


#define oo_OBJECT(Class,version,x)  \
	binputex (x != NULL, f); \
	if (x) \
		Data_writeBinary (x, f);

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	binputi4 (x ? x -> size : 0, f); \
	if (x) { \
		for (long i = 1; i <= x -> size; i ++) { \
			ItemClass data = (ItemClass) x -> item [i]; \
			data -> struct##ItemClass :: v_writeBinary (f); \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type)  \
	void struct##Type :: writeBinary (FILE *f) {

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	void struct##Class :: v_writeBinary (FILE *f) { \
		Class##_Parent :: v_writeBinary (f);

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
#define oo_WRITING_BINARY  1
#define oo_WRITING_CACHE  0
#define oo_WRITING_LISP  0
#define oo_DESCRIBING  0

/* End of file oo_WRITE_BINARY.h */
