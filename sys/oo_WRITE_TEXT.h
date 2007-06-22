/* oo_WRITE_TEXT.h
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
 * pb 2007/06/21 asc -> tex
 */

#include "oo_undef.h"



#define oo_SIMPLE(type,storage,x)  \
	texput##storage (my x, file, #x);

#define oo_ARRAY(type,storage,x,cap,n)  \
	texputintro (file, #x " []: %s", n ? "" : "(empty)"); \
	for (int i = 0; i < n; i ++) \
		texput##storage (my x [i], file, #x " [%d]", i); \
	texexdent (file);

#define oo_SET(type,storage,x,setType)  \
	texputintro (file, #x " []:"); \
	for (int i = 0; i <= enumlength (setType); i ++) \
		texput##storage (my x [i], file, #x " [%s]", enumstring (setType, i)); \
	texexdent (file);

#define oo_VECTOR(type,t,storage,x,min,max)  \
	if (! NUM##t##vector_writeText (my x, min, max, file, #x)) return 0;

#define oo_MATRIX(type,t,storage,x,row1,row2,col1,col2)  \
	if (! NUM##t##matrix_writeText (my x, row1, row2, col1, col2, file, #x)) return 0;



#define oo_ENUMx(type,storage,Type,x)  \
	texput##storage (my x, file, & enum_##Type, #x);

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	texputintro (file, #x " []:"); \
	for (int i = 0; i < n; i ++) \
		texput##storage (my x [i], file, & enum_##Type, #x " [%d]", i); \
	texexdent (file);

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	texputintro (file, #x " []: %s", n ? "" : "(empty)"); \
	for (int i = 0; i <= enumlength (setType); i ++) \
		texput##storage (my x [i], file, & enum_##Type, #x " [%s]", enumstring (setType, i)); \
	texexdent (file);

#define oo_ENUMx_VECTOR(type,t,storage,Type,x,min,max)  \
	texputintro (file, #x " []: %s", max >= min ? "" : "(empty)"); \
	for (long i = min; i <= max; i ++) \
		texput##storage (my x [i], file, & enum_##Type, #x " [%ld]", i); \
	texexdent (file);



#define oo_STRINGx(storage,x)  \
	texput##storage (my x, file, #x);

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	texputintro (file, #x " []: %s", n ? "" : "(empty)"); \
	for (int i = 0; i < n; i ++) \
		texput##storage (my x [i], file, #x " [%d]", i); \
	texexdent (file);

#define oo_STRINGx_SET(storage,x,setType)  \
	texputintro (file, #x " []:"); \
	for (int i = 0; i <= enumlength (setType); i ++) \
		texput##storage (my x [i], file, #x " [%s]", enumstring (setType, i)); \
	texexdent (file);

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	texputintro (file, #x " []: %s", max >= min ? "" : "(empty)"); \
	for (long i = min; i <= max; i ++) \
		texput##storage (my x [i], file, #x " [%ld]", i); \
	texexdent (file);



#define oo_STRINGWx(storage,x)  \
	texput##storage (my x, file, #x);

#define oo_STRINGWx_ARRAY(storage,x,cap,n)  \
	texputintro (file, #x " []: %s", n ? "" : "(empty)"); \
	for (int i = 0; i < n; i ++) \
		texput##storage (my x [i], file, #x " [%d]", i); \
	texexdent (file);

#define oo_STRINGWx_SET(storage,x,setType)  \
	texputintro (file, #x " []:"); \
	for (int i = 0; i <= enumlength (setType); i ++) \
		texput##storage (my x [i], file, #x " [%s]", enumstring (setType, i)); \
	texexdent (file);

#define oo_STRINGWx_VECTOR(storage,x,min,max)  \
	texputintro (file, #x " []: %s", max >= min ? "" : "(empty)"); \
	for (long i = min; i <= max; i ++) \
		texput##storage (my x [i], file, #x " [%ld]", i); \
	texexdent (file);



#define oo_STRUCT(Type,x)  \
	texputintro (file, #x ":"); \
	Type##_writeText (& my x, file); \
	texexdent (file);

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	texputintro (file, #x " []: %s", n ? "" : "(empty)"); \
	for (int i = 0; i < n; i ++) { \
		texputintro (file, #x " [%d]:", i); \
		Type##_writeText (& my x [i], file); \
		texexdent (file); \
	} \
	texexdent (file);

#define oo_STRUCT_SET(Type,x,setType)  \
	texputintro (file, #x " []:"); \
	for (int i = 0; i <= enumlength (setType); i ++) { \
		texputintro (file, #x " [%s]:", enumstring (setType, i)); \
		Type##_writeText (& my x [i], file); \
		texexdent (file); \
	} \
	texexdent (file);

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	texputintro (file, #x " []: %s", max >= min ? "" : "(empty)"); \
	for (long i = min; i <= max; i ++) { \
		texputintro (file, #x " [%ld]:", i); \
		Type##_writeText (& my x [i], file); \
		texexdent (file); \
	} \
	texexdent (file);


#define oo_OBJECT(Class,version,x)  \
	texputex (my x != NULL, file, #x); \
	if (my x && ! Data_writeText (my x, file)) return 0;

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	texputi4 (my x ? my x -> size : 0, file, #x ": size"); \
	if (my x) { \
		for (long i = 1; i <= my x -> size; i ++) { \
			ItemClass data = my x -> item [i]; \
			texputintro (file, #x " [%ld]:", i); \
			if (! class##ItemClass -> writeText (data, file)) return 0; \
			texexdent (file); \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)



#define oo_DEFINE_STRUCT(Type)  \
	static int Type##_writeText (Type me, MelderFile file) {

#define oo_END_STRUCT(Type)  \
		return 1; \
	}



#define oo_DEFINE_CLASS(Class,Parent)  \
	static int class##Class##_writeText (I, MelderFile file) { \
		iam (Class); \
		if (! inherited (Class) writeText (me, file)) return 0;

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
#define oo_VALIDATING_ASCII  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_READING_CACHE  0
#define oo_READING_LISP  0
#define oo_WRITING  1
#define oo_WRITING_TEXT  1
#define oo_WRITING_BINARY  0
#define oo_WRITING_CACHE  0
#define oo_WRITING_LISP  0
#define oo_DESCRIBING  0

/* End of file oo_WRITE_TEXT.h */
