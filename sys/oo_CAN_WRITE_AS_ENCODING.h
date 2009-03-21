/* oo_CAN_WRITE_AS_ENCODING.h
 *
 * Copyright (C) 2007-2009 Paul Boersma
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
 * pb 2007/06/24 created as oo_CAN_WRITE_AS_ASCII.h
 * pb 2007/07/03 renamed to oo_CAN_WRITE_AS_ENCODING.h
 * pb 2007/10/01 struct_canWriteAsEncoding static
 * pb 2007/10/14 api
 * pb 2009/03/21 modern enums
 */

#include "oo_undef.h"

#define oo_SIMPLE(type,storage,x)

#define oo_ARRAY(type,storage,x,cap,n)

#define oo_SET(type,storage,x,setType)

#define oo_VECTOR(type,t,storage,x,min,max)

#define oo_MATRIX(type,t,storage,x,row1,row2,col1,col2)

#define oo_ENUMx(type,storage,Type,x)

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)

#define oo_ENUMx_SET(type,storage,Type,x,setType)

#define oo_ENUMx_VECTOR(type,t,storage,Type,x,min,max)

#define oo_STRINGx(storage,x)

#define oo_STRINGx_ARRAY(storage,x,cap,n)

#define oo_STRINGx_SET(storage,x,setType)

#define oo_STRINGx_VECTOR(storage,x,min,max)

#define oo_STRINGWx(storage,x)  \
	if (my x && ! Melder_isEncodable (my x, encoding)) return false;

#define oo_STRINGWx_ARRAY(storage,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		if (my x [i] && ! Melder_isEncodable (my x [i], encoding)) return false;

#define oo_STRINGWx_SET(storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		if (my x [i] && ! Melder_isEncodable (my x [i], encoding)) return false;

#define oo_STRINGWx_VECTOR(storage,x,min,max)  \
	if (my x) { \
		for (long i = min; i <= max; i ++) \
			if (my x [i] && ! Melder_isEncodable (my x [i], encoding)) return false; \
	}

#define oo_STRUCT(Type,x)  \
	if (! Type##_canWriteAsEncoding (& my x, encoding)) return false;

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	for (int i = 0; i < n; i ++) \
		if (! Type##_canWriteAsEncoding (& my x [i], encoding)) return false;

#define oo_STRUCT_SET(Type,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) \
		if (! Type##_canWriteAsEncoding (& my x [i], encoding)) return false;

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (my x) { \
		for (long i = min; i <= max; i ++) \
			if (! Type##_canWriteAsEncoding (& my x [i], encoding)) return false; \
	}

#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  \
	if (my x) { \
		for (long i = row1; i <= row2; i ++) \
			for (long j = col1; j <= col2; j ++) \
				if (! Type##_canWriteAsEncoding (& my x [i] [j], encoding)) return false; \
	}

#define oo_WIDGET(x)  \
	!!!! Can never write Widgets... !!!!

#define oo_WIDGET_ARRAY(x,cap,n)  \
	!!!! Can never write Widgets... !!!!

#define oo_WIDGET_SET(x,setType)  \
	!!!! Can never write Widgets... !!!!

#define oo_WIDGET_VECTOR_FROM(x,cap,min,max)  \
	!!!! Can never write Widgets... !!!!



#define oo_OBJECT(Class,version,x)  \
	if (my x && ! Data_canWriteAsEncoding (my x, encoding)) return false;

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	if (my x && ! Data_canWriteAsEncoding (my x, encoding)) return false;

#define oo_FILE(x)  \
	if (! Melder_isEncodable (my x. path, encoding)) return false;

#define oo_DIR(x)  \
	if (! Melder_isEncodable (my x. path, encoding)) return false;



#define oo_DEFINE_STRUCT(Type)  \
	static bool Type##_canWriteAsEncoding (Type me, int encoding) { \
		(void) me; (void) encoding;

#define oo_END_STRUCT(Type)  \
		return true; \
	}



#define oo_DEFINE_CLASS(Class,Parent)  \
	static bool class##Class##_canWriteAsEncoding (I, int encoding) { \
		iam (Class); \
		if (! inherited (Class) canWriteAsEncoding (me, encoding)) return false;

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
#define oo_EQUALLING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  1
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

/* End of file oo_CAN_WRITE_AS_ENCODING.h */
