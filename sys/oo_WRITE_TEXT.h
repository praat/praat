/* oo_WRITE_TEXT.h
 *
 * Copyright (C) 1994-2012,2013,2014 Paul Boersma
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
	texput##storage (file, our x, L"" #x, 0,0,0,0,0);

#define oo_ARRAY(type,storage,x,cap,n)  \
	texputintro (file, L"" #x " []: ", n ? NULL : L"(empty)", 0,0,0,0); \
	for (int i = 0; i < n; i ++) \
		texput##storage (file, our x [i], L"" #x " [", Melder_integer (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_SET(type,storage,x,setType)  \
	texputintro (file, L"" #x " []:", 0,0,0,0,0); \
	for (int i = 0; i <= setType##_MAX; i ++) \
		texput##storage (file, our x [i], L"" #x " [", setType##_getText (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_VECTOR(type,storage,x,min,max)  \
	if (our x) \
		NUMvector_writeText_##storage (our x, min, max, file, L"" #x);

#define oo_MATRIX(type,storage,x,row1,row2,col1,col2)  \
	if (our x) \
		NUMmatrix_writeText_##storage (our x, row1, row2, col1, col2, file, L"" #x);

#define oo_ENUMx(type,storage,Type,x)  \
	texput##storage (file, our x, Type##_getText, L"" #x, 0,0,0,0,0);

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	texputintro (file, L"" #x " []:", 0,0,0,0,0); \
	for (int i = 0; i < n; i ++) \
		texput##storage (file, our x [i], Type##_getText, L"" #x " [", Melder_integer (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	texputintro (file, L"" #x " []: ", n ? NULL : L"(empty)", 0,0,0,0); \
	for (int i = 0; i <= setType##_MAX; i ++) \
		texput##storage (file, our x [i], Type##_getText, L"" #x " [", setType##_getText (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_ENUMx_VECTOR(type,storage,Type,x,min,max)  \
	texputintro (file, L"" #x " []: ", max >= min ? NULL : L"(empty)", 0,0,0,0); \
	for (long i = min; i <= max; i ++) \
		texput##storage (file, our x [i], Type##_getText, L"" #x " [", Melder_integer (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_STRINGx(storage,x)  \
	texput##storage (file, our x, L""#x, 0,0,0,0,0);

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	texputintro (file, L"" #x " []: ", n ? NULL : L"(empty)", 0,0,0,0); \
	for (int i = 0; i < n; i ++) \
		texput##storage (file, our x [i], L"" #x " [", Melder_integer (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_STRINGx_SET(storage,x,setType)  \
	texputintro (file, L"" #x " []:", 0,0,0,0,0); \
	for (int i = 0; i <= setType##_MAX; i ++) \
		texput##storage (file, our x [i], L"" #x " [", setType##_getText (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	texputintro (file, L"" #x " []: ", max >= min ? NULL : L"(empty)", 0,0,0,0); \
	for (long i = min; i <= max; i ++) \
		texput##storage (file, our x [i], L"" #x " [", Melder_integer (i), L"]", 0,0,0); \
	texexdent (file);

#define oo_STRUCT(Type,x)  \
	texputintro (file, L"" #x ":", 0,0,0,0,0); \
	our x. writeText (file); \
	texexdent (file);

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	texputintro (file, L"" #x " []: ", n ? NULL : L"(empty)", 0,0,0,0); \
	for (int i = 0; i < n; i ++) { \
		texputintro (file, L"" #x " [", Melder_integer (i), L"]:", 0,0,0); \
		our x [i]. writeText (file); \
		texexdent (file); \
	} \
	texexdent (file);

#define oo_STRUCT_SET(Type,x,setType)  \
	texputintro (file, L"" #x " []:", 0,0,0,0,0); \
	for (int i = 0; i <= setType##_MAX; i ++) { \
		texputintro (file, L"" #x " [", setType##_getText (i), L"]:", 0,0,0); \
		our x [i]. writeText (file); \
		texexdent (file); \
	} \
	texexdent (file);

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	texputintro (file, L"" #x " []: ", max >= min ? NULL : L"(empty)", 0,0,0,0); \
	for (long i = min; i <= max; i ++) { \
		texputintro (file, L"" #x " [", Melder_integer (i), L"]:", 0,0,0); \
		our x [i]. writeText (file); \
		texexdent (file); \
	} \
	texexdent (file);

#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  \
	texputintro (file, L"" #x " [] []: ", row2 >= row1 ? NULL : L"(empty)", 0,0,0,0); \
	for (long i = row1; i <= row2; i ++) { \
		texputintro (file, L"" #x " [", Melder_integer (i), L"]:", 0,0,0); \
		for (long j = col1; j <= col2; j ++) { \
			texputintro (file, L"" #x " [", Melder_integer (i), L"] [", Melder_integer (j), L"]:", 0); \
			our x [i] [j]. writeText (file); \
			texexdent (file); \
		} \
		texexdent (file); \
	} \
	texexdent (file);

#define oo_OBJECT(Class,version,x)  \
	texputex (file, our x != NULL, L"" #x, 0,0,0,0,0); \
	if (our x) \
		Data_writeText (our x, file);

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	texputi4 (file, our x ? our x -> size : 0, L"" #x ": size", 0,0,0,0,0); \
	if (our x) { \
		for (long i = 1; i <= our x -> size; i ++) { \
			ItemClass data = (ItemClass) our x -> item [i]; \
			texputintro (file, L"" #x " [", Melder_integer (i), L"]:", 0,0,0); \
			data -> struct##ItemClass :: v_writeText (file); \
			texexdent (file); \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type)  \
	void struct##Type :: writeText (MelderFile file) {

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	void struct##Class :: v_writeText (MelderFile file) { \
		Class##_Parent :: v_writeText (file);

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
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  1
#define oo_WRITING_TEXT  1
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo_WRITE_TEXT.h */
