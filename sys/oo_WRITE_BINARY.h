/* oo_WRITE_BINARY.h
 *
 * Copyright (C) 1994-2008 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2003/02/07 added oo_FILE and oo_DIR (empty)
 * pb 2006/05/29 added version to oo_OBJECT and oo_COLLECTION
 * pb 2007/06/09 wchar_t
 * pb 2008/01/19 NUM##storage
 */

#include "oo_undef.h"


#define oo_SIMPLE(type,storage,x)  \
	binput##storage (my x, f);

#define oo_ARRAY(type,storage,x,cap,n)  \
	{ \
		int i; \
		for (i = 0; i < n; i ++) \
			binput##storage (my x [i], f); \
	}

#define oo_SET(type,storage,x,setType)  \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			binput##storage (my x [i], f); \
	}

#define oo_VECTOR(type,t,storage,x,min,max)  \
	if (my x && ! NUM##t##vector_writeBinary_##storage (my x, min, max, f)) return 0;

#define oo_MATRIX(type,t,storage,x,row1,row2,col1,col2)  \
	if (my x && ! NUM##t##matrix_writeBinary_##storage (my x, row1, row2, col1, col2, f)) return 0;


#define oo_ENUMx(type,storage,Type,x)  \
	binput##storage (my x, f, & enum_##Type);

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	{ \
		int i; \
		for (i = 0; i < n; i ++) \
			binput##storage (my x [i], f, & enum_##Type); \
	}

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			binput##storage (my x [i], f, & enum_##Type); \
	}

#define oo_ENUMx_VECTOR(type,t,storage,Type,x,min,max)  \
	if (my x && ! NUM##t##vector_writeBinary_##storage (my x, min, max, f)) return 0;


#define oo_STRINGx(storage,x)  \
	binput##storage (my x, f);

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	{ \
		int i; \
		for (i = 0; i < n; i ++) \
			binput##storage (my x [i], f); \
	}

#define oo_STRINGx_SET(storage,x,setType)  \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			binput##storage (my x [i], f); \
	}

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	if (max >= min) { \
		long i; \
		for (i = min; i <= max; i ++) \
			binput##storage (my x [i], f); \
	}


#define oo_STRINGWx(storage,x)  \
	binput##storage (my x, f);

#define oo_STRINGWx_ARRAY(storage,x,cap,n)  \
	{ \
		int i; \
		for (i = 0; i < n; i ++) \
			binput##storage (my x [i], f); \
	}

#define oo_STRINGWx_SET(storage,x,setType)  \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			binput##storage (my x [i], f); \
	}

#define oo_STRINGWx_VECTOR(storage,x,min,max)  \
	if (max >= min) { \
		long i; \
		for (i = min; i <= max; i ++) \
			binput##storage (my x [i], f); \
	}


#define oo_STRUCT(Type,x)  \
	Type##_writeBinary (& my x, f);

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	{ \
		int i; \
		for (i = 0; i < n; i ++) \
			Type##_writeBinary (& my x [i], f); \
	}

#define oo_STRUCT_SET(Type,x,setType)  \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			Type##_writeBinary (& my x [i], f); \
	}

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (max >= min) { \
		long i; \
		for (i = min; i <= max; i ++) \
			Type##_writeBinary (& my x [i], f); \
	}



#define oo_OBJECT(Class,version,x)  \
	binputex (my x != NULL, f); \
	if (my x && ! Data_writeBinary (my x, f)) return 0;

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	binputi4 (my x ? my x -> size : 0, f); \
	if (my x) { \
		long i; \
		for (i = 1; i <= my x -> size; i ++) { \
			ItemClass data = my x -> item [i]; \
			if (! class##ItemClass -> writeBinary (data, f)) return 0; \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)



#define oo_DEFINE_STRUCT(Type)  \
	static int Type##_writeBinary (Type me, FILE *f) {

#define oo_END_STRUCT(Type)  \
		return 1; \
	}



#define oo_DEFINE_CLASS(Class,Parent)  \
	static int class##Class##_writeBinary (I, FILE *f) { \
		iam (Class); \
		if (! inherited (Class) writeBinary (me, f)) return 0;

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
