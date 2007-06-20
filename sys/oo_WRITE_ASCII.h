/* oo_WRITE_ASCII.h
 *
 * Copyright (C) 1994-2007 Paul Boersma
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
 * pb 2007/06/09
 */

#include "oo_undef.h"



#define oo_SIMPLE(type,storage,x)  \
	ascput##storage (my x, f, #x);

#define oo_ARRAY(type,storage,x,cap,n)  \
	ascputintro (f, #x " []: %s", n ? "" : "(empty)"); \
	{ \
		int i; \
		for (i = 0; i < n; i ++) \
			ascput##storage (my x [i], f, #x " [%d]", i); \
	} \
	ascexdent ();

#define oo_SET(type,storage,x,setType)  \
	ascputintro (f, #x " []:"); \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			ascput##storage (my x [i], f, #x " [%s]", enumstring (setType, i)); \
	} \
	ascexdent ();

#define oo_VECTOR(type,t,storage,x,min,max)  \
	if (! NUM##t##vector_writeAscii (my x, min, max, f, #x)) return 0;

#define oo_MATRIX(type,t,storage,x,row1,row2,col1,col2)  \
	if (! NUM##t##matrix_writeAscii (my x, row1, row2, col1, col2, f, #x)) return 0;



#define oo_ENUMx(type,storage,Type,x)  \
	ascput##storage (my x, f, & enum_##Type, #x);

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	ascputintro (f, #x " []:"); \
	{ \
		int i; \
		for (i = 0; i < n; i ++) \
			ascput##storage (my x [i], f, & enum_##Type, #x " [%d]", i); \
	} \
	ascexdent ();

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	ascputintro (f, #x " []: %s", n ? "" : "(empty)"); \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			ascput##storage (my x [i], f, & enum_##Type, #x " [%s]", enumstring (setType, i)); \
	} \
	ascexdent ();

#define oo_ENUMx_VECTOR(type,t,storage,Type,x,min,max)  \
	ascputintro (f, #x " []: %s", max >= min ? "" : "(empty)"); \
	{ \
		long i; \
		for (i = min; i <= max; i ++) \
			ascput##storage (my x [i], f, & enum_##Type, #x " [%ld]", i); \
	} \
	ascexdent ();



#define oo_STRINGx(storage,x)  \
	ascput##storage (my x, f, #x);

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	ascputintro (f, #x " []: %s", n ? "" : "(empty)"); \
	{ \
		int i; \
		for (i = 0; i < n; i ++) \
			ascput##storage (my x [i], f, #x " [%d]", i); \
	} \
	ascexdent ();

#define oo_STRINGx_SET(storage,x,setType)  \
	ascputintro (f, #x " []:"); \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			ascput##storage (my x [i], f, #x " [%s]", enumstring (setType, i)); \
	} \
	ascexdent ();

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	ascputintro (f, #x " []: %s", max >= min ? "" : "(empty)"); \
	{ \
		long i; \
		for (i = min; i <= max; i ++) \
			ascput##storage (my x [i], f, #x " [%ld]", i); \
	} \
	ascexdent ();



#define oo_STRINGWx(storage,x)  \
	ascput##storage (my x, f, #x);

#define oo_STRINGWx_ARRAY(storage,x,cap,n)  \
	ascputintro (f, #x " []: %s", n ? "" : "(empty)"); \
	{ \
		int i; \
		for (i = 0; i < n; i ++) \
			ascput##storage (my x [i], f, #x " [%d]", i); \
	} \
	ascexdent ();

#define oo_STRINGWx_SET(storage,x,setType)  \
	ascputintro (f, #x " []:"); \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			ascput##storage (my x [i], f, #x " [%s]", enumstring (setType, i)); \
	} \
	ascexdent ();

#define oo_STRINGWx_VECTOR(storage,x,min,max)  \
	ascputintro (f, #x " []: %s", max >= min ? "" : "(empty)"); \
	{ \
		long i; \
		for (i = min; i <= max; i ++) \
			ascput##storage (my x [i], f, #x " [%ld]", i); \
	} \
	ascexdent ();



#define oo_STRUCT(Type,x)  \
	ascputintro (f, #x ":"); \
	Type##_writeAscii (& my x, f); \
	ascexdent ();

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	ascputintro (f, #x " []: %s", n ? "" : "(empty)"); \
	{ \
		int i; \
		for (i = 0; i < n; i ++) { \
			ascputintro (f, #x " [%d]:", i); \
			Type##_writeAscii (& my x [i], f); \
			ascexdent (); \
		} \
	} \
	ascexdent ();

#define oo_STRUCT_SET(Type,x,setType)  \
	ascputintro (f, #x " []:"); \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) { \
			ascputintro (f, #x " [%s]:", enumstring (setType, i)); \
			Type##_writeAscii (& my x [i], f); \
			ascexdent (); \
		} \
	} \
	ascexdent ();

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	ascputintro (f, #x " []: %s", max >= min ? "" : "(empty)"); \
	{ \
		long i; \
		for (i = min; i <= max; i ++) { \
			ascputintro (f, #x " [%ld]:", i); \
			Type##_writeAscii (& my x [i], f); \
			ascexdent (); \
		} \
	} \
	ascexdent ();


#define oo_OBJECT(Class,version,x)  \
	ascputex (my x != NULL, f, #x); \
	if (my x && ! Data_writeAscii (my x, f)) return 0;

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	ascputi4 (my x ? my x -> size : 0, f, #x ": size"); \
	if (my x) { \
		long i; \
		for (i = 1; i <= my x -> size; i ++) { \
			ItemClass data = my x -> item [i]; \
			ascputintro (f, #x " [%ld]:", i); \
			if (! class##ItemClass -> writeAscii (data, f)) return 0; \
			ascexdent (); \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)



#define oo_DEFINE_STRUCT(Type)  \
	static int Type##_writeAscii (Type me, FILE *f) { \

#define oo_END_STRUCT(Type)  \
		return 1; \
	}



#define oo_DEFINE_CLASS(Class,Parent)  \
	static int class##Class##_writeAscii (I, FILE *f) { \
		iam (Class); \
		if (! inherited (Class) writeAscii (me, f)) return 0; \

#define oo_END_CLASS(Class)  \
		return 1; \
	}



#define oo_FROM(from)

#define oo_ENDFROM



#define oo_VERSION(version)



#define oo_IF(condition)  if (condition) {
#define oo_ENDIF  }
#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_EQUALLING  0
#define oo_COMPARING  0
#define oo_READING  0
#define oo_READING_ASCII  0
#define oo_READING_BINARY  0
#define oo_READING_CACHE  0
#define oo_READING_LISP  0
#define oo_WRITING  1
#define oo_WRITING_ASCII  1
#define oo_WRITING_BINARY  0
#define oo_WRITING_CACHE  0
#define oo_WRITING_LISP  0
#define oo_DESCRIBING  0

/* End of file oo_WRITE_ASCII.h */
