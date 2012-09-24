/* oo_READ_BINARY.h
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
	x = binget##storage (f);

#define oo_ARRAY(type,storage,x,cap,n)  \
	if (n > cap) Melder_throw ("Number of \"", #x, "\" (", n, ") greater than ", cap, "."); \
	for (int i = 0; i < n; i ++) { \
		x [i] = binget##storage (f); \
	}

#define oo_SET(type,storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) { \
		x [i] = binget##storage (f); \
	}

#define oo_VECTOR(type,storage,x,min,max)  \
	if (max >= min) { \
		x = NUMvector_readBinary_##storage (min, max, f); \
	}

#define oo_MATRIX(type,storage,x,row1,row2,col1,col2)  \
	if (row2 >= row1 && col2 >= col1) { \
	    x = NUMmatrix_readBinary_##storage (row1, row2, col1, col2, f); \
	}

#define oo_ENUMx(type,storage,Type,x)  \
	x = binget##storage (f, Type##_MIN, Type##_MAX, L"" #Type);

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	if (n > cap) Melder_throw ("Number of \"", #x, "\" (", n, ") greater than ", cap, "."); \
	for (int i = 0; i < n; i ++) { \
		x [i] = binget##storage (f, Type##_MIN, Type##_MAX, L"" #Type); \
	}

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) { \
		x [i] = binget##storage (f, Type##_MIN, Type##_MAX, L"" #Type); \
	}

#define oo_ENUMx_VECTOR(type,storage,Type,x,min,max)  \
	if (max >= min) { \
		x = NUMvector <type> (min, max); \
		for (long i = min; i <= max; i ++) { \
			x [i] = binget##storage (f, Type##_MIN, Type##_MAX, L"" #Type); \
	}

#define oo_STRINGx(storage,x)  \
	x = binget##storage (f);

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	if (n > cap) Melder_throw ("Number of \"", #x, "\" (", n, ") greater than ", cap, "."); \
	for (int i = 0; i < n; i ++) { \
		x [i] = binget##storage (f); \
	}

#define oo_STRINGx_SET(storage,x,setType)  \
	for (int i = 0; i <= setType##_MAX; i ++) { \
		x [i] = binget##storage (f); \
	}

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	if (max >= min) { \
		x = NUMvector <wchar_t *> (min, max); \
		for (long i = min; i <= max; i ++) { \
			x [i] = binget##storage (f); \
		} \
	}

#define oo_STRUCT(Type,x)  \
	x. readBinary (f);

#define oo_STRUCT_ARRAY(Type,x,cap,n) \
	if (n > cap) Melder_throw ("Number of \"", #x, "\" (", n, ") greater than ", cap, "."); \
	for (int i = 0; i < n; i ++) { \
		x [i]. readBinary (f); \
	}

#define oo_STRUCT_SET(Type,x,setType) \
	for (int i = 0; i <= setType##_MAX; i ++) { \
		x [i]. readBinary (f); \
	}

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (max >= min) { \
		x = NUMvector <struct##Type> (min, max); \
		for (long i = min; i <= max; i ++) { \
			x [i]. readBinary (f); \
		} \
	}

#define oo_OBJECT(Class,version,x)  \
	if (bingetex (f)) { \
		long saveVersion = Thing_version; \
		x = Thing_new (Class); \
		Thing_version = version; \
		x -> v_readBinary (f); \
		Thing_version = saveVersion; \
	}

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	{ \
		long n = bingeti4 (f); \
		x = Class##_create (); \
		for (long i = 1; i <= n; i ++) { \
			long saveVersion = Thing_version; \
			auto##ItemClass item = (ItemClass) Thing_new (ItemClass); \
			Thing_version = version; \
			item.peek() -> v_readBinary (f); \
			Thing_version = saveVersion; \
			Collection_addItem (x, item.transfer()); \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type)  \
	void struct##Type :: readBinary (FILE *f) { \
		int localVersion = Thing_version; (void) localVersion;

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	void struct##Class :: v_readBinary (FILE *f) { \
		int localVersion = Thing_version; (void) localVersion; \
		if (localVersion > this -> classInfo -> version) \
			Melder_throw ("The format of this file is too new. Download a newer version of Praat."); \
		Class##_Parent :: v_readBinary (f);

#define oo_END_CLASS(Class)  \
	}

#define oo_IF(condition)  \
	if (condition) {

#define oo_ENDIF  \
	}

#define oo_FROM(from)  \
	if (localVersion >= from) {

#define oo_ENDFROM  \
	}

#define oo_VERSION(version)  \
	Thing_version = version;

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_EQUALLING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  1
#define oo_READING_TEXT  0
#define oo_READING_BINARY  1
#define oo_READING_CACHE  0
#define oo_READING_LISP  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_WRITING_CACHE  0
#define oo_WRITING_LISP  0
#define oo_DESCRIBING  0

/* End of file oo_READ_BINARY.h */
