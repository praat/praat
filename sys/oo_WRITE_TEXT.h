/* oo_WRITE_TEXT.h
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

/*
 * pb 2002/03/07 GPL
 * pb 2003/02/07 added oo_FILE and oo_DIR (empty)
 * pb 2006/05/29 added version to oo_OBJECT and oo_COLLECTION
 * pb 2007/06/21 asc -> tex
 * pb 2007/08/14 enums wchar_t
 * pb 2008/01/19 NUM##storage
 * pb 2009/03/21 modern enums
 * pb 2011/03/03 removed oo_STRINGx
 * pb 2011/07/07 void
 */

#include "oo_undef.h"

#define oo_SIMPLE(type,storage,x)  \
	texput##storage (file, my x, L"" #x, 0,0,0,0,0);

#define oo_ARRAY(type,storage,x,cap,n)  \
	texputintro (file, L"" #x " []: ", n ? NULL : L"(empty)", 0,0,0,0); \
	for (int i = 0; i < n; i ++) \
		texput##storage (file, my x [i], L"" #x " [", Melder_integer (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_SET(type,storage,x,setType)  \
	texputintro (file, L"" #x " []:", 0,0,0,0,0); \
	for (int i = 0; i <= setType##_MAX; i ++) \
		texput##storage (file, my x [i], L"" #x " [", setType##_getText (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_VECTOR(type,t,storage,x,min,max)  \
	if (my x) \
		NUM##t##vector_writeText_##storage (my x, min, max, file, L"" #x);

#define oo_MATRIX(type,t,storage,x,row1,row2,col1,col2)  \
	if (my x) \
		NUM##t##matrix_writeText_##storage (my x, row1, row2, col1, col2, file, L"" #x);

#define oo_ENUMx(type,storage,Type,x)  \
	texput##storage (file, my x, Type##_getText, L"" #x, 0,0,0,0,0);

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	texputintro (file, L"" #x " []:", 0,0,0,0,0); \
	for (int i = 0; i < n; i ++) \
		texput##storage (file, my x [i], Type##_getText, L"" #x " [", Melder_integer (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	texputintro (file, L"" #x " []: ", n ? NULL : L"(empty)", 0,0,0,0); \
	for (int i = 0; i <= setType##_MAX; i ++) \
		texput##storage (file, my x [i], Type##_getText, L"" #x " [", setType##_getText (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_ENUMx_VECTOR(type,t,storage,Type,x,min,max)  \
	texputintro (file, L"" #x " []: ", max >= min ? NULL : L"(empty)", 0,0,0,0); \
	for (long i = min; i <= max; i ++) \
		texput##storage (file, my x [i], Type##_getText, L"" #x " [", Melder_integer (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_STRINGx(storage,x)  \
	texput##storage (file, my x, L""#x, 0,0,0,0,0);

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	texputintro (file, L"" #x " []: ", n ? NULL : L"(empty)", 0,0,0,0); \
	for (int i = 0; i < n; i ++) \
		texput##storage (file, my x [i], L"" #x " [", Melder_integer (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_STRINGx_SET(storage,x,setType)  \
	texputintro (file, L"" #x " []:", 0,0,0,0,0); \
	for (int i = 0; i <= setType##_MAX; i ++) \
		texput##storage (file, my x [i], L"" #x " [", setType##_getText (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	texputintro (file, L"" #x " []: ", max >= min ? NULL : L"(empty)", 0,0,0,0); \
	for (long i = min; i <= max; i ++) \
		texput##storage (file, my x [i], L"" #x " [", Melder_integer (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_STRUCT(Type,x)  \
	texputintro (file, L"" #x ":", 0,0,0,0,0); \
	Type##_writeText (& my x, file); \
	texexdent (file);

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	texputintro (file, L"" #x " []: ", n ? NULL : L"(empty)", 0,0,0,0); \
	for (int i = 0; i < n; i ++) { \
		texputintro (file, L"" #x " [", Melder_integer (i), L"]:", 0,0,0); \
		Type##_writeText (& my x [i], file); \
		texexdent (file); \
	} \
	texexdent (file);

#define oo_STRUCT_SET(Type,x,setType)  \
	texputintro (file, L"" #x " []:", 0,0,0,0,0); \
	for (int i = 0; i <= setType##_MAX; i ++) { \
		texputintro (file, L"" #x " [", setType##_getText (i), L"]:", 0,0,0); \
		Type##_writeText (& my x [i], file); \
		texexdent (file); \
	} \
	texexdent (file);

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	texputintro (file, L"" #x " []: ", max >= min ? NULL : L"(empty)", 0,0,0,0); \
	for (long i = min; i <= max; i ++) { \
		texputintro (file, L"" #x " [", Melder_integer (i), L"]:", 0,0,0); \
		Type##_writeText (& my x [i], file); \
		texexdent (file); \
	} \
	texexdent (file);

#define oo_OBJECT(Class,version,x)  \
	texputex (file, my x != NULL, L"" #x, 0,0,0,0,0); \
	if (my x) \
		Data_writeText (my x, file);

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	texputi4 (file, my x ? my x -> size : 0, L"" #x ": size", 0,0,0,0,0); \
	if (my x) { \
		for (long i = 1; i <= my x -> size; i ++) { \
			ItemClass data = (ItemClass) my x -> item [i]; \
			texputintro (file, L"" #x " [", Melder_integer (i), L"]:", 0,0,0); \
			class##ItemClass -> writeText (data, file); \
			texexdent (file); \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type)  \
	static void Type##_writeText (Type me, MelderFile file) {

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	static void class##Class##_writeText (I, MelderFile file) { \
		iam (Class); \
		inherited (Class) writeText (me, file);

#define oo_END_CLASS(Class)  \
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
#define oo_VALIDATING_ENCODING  0
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
