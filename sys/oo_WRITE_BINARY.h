/* oo_WRITE_BINARY.h
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
	binput##storage (our x, f);

#define oo_ARRAY(type,storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		binput##storage (our x [i], f);

#define oo_SET(type,storage,x,setType)  \
	for (int i = 0; i <= (int) setType::MAX; i ++) \
		binput##storage (our x [i], f);

#define oo_VECTOR(type,storage,x,min,max)  \
	if (our x) \
		NUMvector_writeBinary_##storage (our x, min, max, f);

#define oo_MATRIX(type,storage,x,row1,row2,col1,col2)  \
	if (our x) \
		NUMmatrix_writeBinary_##storage (our x, row1, row2, col1, col2, f);

#define oo_ENUMx(kType,storage,x)  \
	binput##storage ((int) our x, f);

//#define oo_ENUMx_ARRAY(kType,storage,x,cap,n)  \
//	for (int i = 0; i < n; i ++) \
//		binput##storage ((int) our x [i], f);

//#define oo_ENUMx_SET(kType,storage,x,setType)  \
//	for (int i = 0; i <= setType::MAX; i ++) \
//		binput##storage ((int) our x [i], f);

//#define oo_ENUMx_VECTOR(kType,storage,x,min,max)  \
//	if (our x) \
//		NUMvector_writeBinary_##storage ((int) our x, min, max, f);

#define oo_STRINGx(storage,x)  \
	binput##storage (our x, f);

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		binput##storage (our x [i], f);

#define oo_STRINGx_SET(storage,x,setType)  \
	for (int i = 0; i <= setType::MAX; i ++) \
		binput##storage (our x [i], f);

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	for (integer i = min; i <= max; i ++) \
		binput##storage (our x [i], f);

#define oo_STRUCT(Type,x)  \
	our x. writeBinary (f);

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		our x [i]. writeBinary (f);

#define oo_STRUCT_SET(Type,x,setType)  \
	for (int i = 0; i <= (int) setType::MAX; i ++) \
		our x [i]. writeBinary (f);

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	for (integer i = min; i <= max; i ++) \
		our x [i]. writeBinary (f);

#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  \
	for (integer i = row1; i <= row2; i ++) \
		for (integer j = col1; j <= col2; j ++) \
			our x [i] [j]. writeBinary (f);

#define oo_AUTO_OBJECT(Class,version,x)  \
	binputex ((bool) our x, f); \
	if (our x) \
		Data_writeBinary (our x.get(), f);

#define oo_COLLECTION_OF(Class,x,ItemClass,version)  \
	binputinteger (our x.size, f); \
	for (integer i = 1; i <= our x.size; i ++) { \
		ItemClass data = our x.at [i]; \
		data -> struct##ItemClass :: v_writeBinary (f); \
	}

#define oo_AUTO_COLLECTION(Class,x,ItemClass,version)  \
	binputinteger (our x ? our x->size : 0, f); \
	if (our x) { \
		for (integer i = 1; i <= our x->size; i ++) { \
			ItemClass data = our x->at [i]; \
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

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  1
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  1
#define oo_DESCRIBING  0

/* End of file oo_WRITE_BINARY.h */
