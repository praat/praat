/* oo_READ_BINARY.h
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
 * pb 2006/04/12 guard against too new versions
 * pb 2006/05/29 added version to oo_OBJECT and oo_COLLECTION
 * pb 2007/06/09 wchar_t
 * pb 2008/01/19 NUM##storage
 */

#include "oo_undef.h"

#define oo_SIMPLE(type,storage,x)  \
	my x = binget##storage (f);

#define oo_ARRAY(type,storage,x,cap,n)  \
	if (n > cap) return Melder_error ("Number of \"%s\" (%d) greater than %d.", #x, n, cap); \
	{ \
		long i; \
		for (i = 0; i < n; i ++) \
			my x [i] = binget##storage (f); \
	}

#define oo_SET(type,storage,x,setType)  \
	{ \
		long i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			my x [i] = binget##storage (f); \
	}

#define oo_VECTOR(type,t,storage,x,min,max)  \
	if (max >= min && ! (my x = NUM##t##vector_readBinary_##storage (min, max, f))) return 0;

#define oo_MATRIX(type,t,storage,x,row1,row2,col1,col2)  \
	if (row2 >= row1 && col2 >= col1 && \
	    ! (my x = NUM##t##matrix_readBinary_##storage (row1, row2, col1, col2, f))) return 0;

#define oo_ENUMx(type,storage,Type,x)  \
	if ((my x = binget##storage (f, & enum_##Type)) < 0) return 0;

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	if (n > cap) return Melder_error ("Number of \"%s\" (%d) greater than %d.", #x, n, cap); \
	{ \
		long i; \
		for (i = 0; i < n; i ++) \
			if ((my x [i] = binget##storage (f, & enum_##Type)) < 0) return 0; \
	}

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	{ \
		long i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			if ((my x [i] = binget##storage (f, & enum_##Type)) < 0) return 0; \
	}

#define oo_ENUMx_VECTOR(type,t,storage,Type,x,min,max)  \
	if (max >= min) { \
		long i; \
		if (! (my x = NUM##t##vector (min, max))) return 0; \
		for (i = min; i <= max; i ++) \
			if ((my x [i] = binget##storage (f, & enum_##Type)) < 0) return 0; \
	}

#define oo_STRINGx(storage,x)  \
	if (! (my x = binget##storage (f))) return 0;

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	if (n > cap) return Melder_error ("Number of \"%s\" (%d) greater than %d.", #x, n, cap); \
	{ \
		long i; \
		for (i = 0; i < n; i ++) \
			if (! (my x [i] = binget##storage (f))) return 0; \
	}

#define oo_STRINGx_SET(storage,x,setType)  \
	{ \
		long i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			if (! (my x [i] = binget##storage (f))) return 0; \
	}

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	if (max >= min) { \
		long i; \
		if (! (my x = NUMvector (sizeof (char *), min, max))) return 0; \
		for (i = min; i <= max; i ++) \
			if (! (my x [i] = binget##storage (f))) return 0; \
	}

#define oo_STRINGWx(storage,x)  \
	if (! (my x = binget##storage (f))) return 0;

#define oo_STRINGWx_ARRAY(storage,x,cap,n)  \
	if (n > cap) return Melder_error ("Number of \"%s\" (%d) greater than %d.", #x, n, cap); \
	{ \
		long i; \
		for (i = 0; i < n; i ++) \
			if (! (my x [i] = binget##storage (f))) return 0; \
	}

#define oo_STRINGWx_SET(storage,x,setType)  \
	{ \
		long i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			if (! (my x [i] = binget##storage (f))) return 0; \
	}

#define oo_STRINGWx_VECTOR(storage,x,min,max)  \
	if (max >= min) { \
		long i; \
		if (! (my x = NUMvector (sizeof (wchar_t *), min, max))) return 0; \
		for (i = min; i <= max; i ++) \
			if (! (my x [i] = binget##storage (f))) return 0; \
	}

#define oo_STRUCT(Type,x)  \
	if (! Type##_readBinary (& my x, f)) return 0;

#define oo_STRUCT_ARRAY(Type,x,cap,n) \
	if (n > cap) return Melder_error ("Number of \"%s\" (%d) greater than %d.", #x, n, cap); \
	{ \
		long i; \
		for (i = 0; i < n; i ++) \
			if (! Type##_readBinary (& my x [i], f)) return 0; \
	}

#define oo_STRUCT_SET(Type,x,setType) \
	{ \
		long i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			if (! Type##_readBinary (& my x [i], f)) return 0; \
	}

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (max >= min) { \
		long i; \
		if (! (my x = NUMstructvector (Type, min, max))) return 0; \
		for (i = min; i <= max; i ++) \
			if (! Type##_readBinary (& my x [i], f)) return 0; \
	}

#define oo_OBJECT(Class,version,x)  \
	if (bingetex (f)) { \
		long saveVersion = Thing_version; \
		if ((my x = new (Class)) == NULL) return 0; \
		Thing_version = version; \
		if (! Data_readBinary (my x, f)) return 0; \
		Thing_version = saveVersion; \
	}

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	{ \
		long n = bingeti4 (f), i; \
		if ((my x = Class##_create ()) == NULL) return 0; \
		for (i = 1; i <= n; i ++) { \
			long saveVersion = Thing_version; \
			ItemClass item = new (ItemClass); \
			if (item == NULL) return 0; \
			Thing_version = version; \
			if (! item -> methods -> readBinary (item, f)) return 0; \
			Thing_version = saveVersion; \
			if (! Collection_addItem (my x, item)) return 0; \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type)  \
	static int Type##_readBinary (Type me, FILE *f) { \
		int localVersion = Thing_version; (void) localVersion;

#define oo_END_STRUCT(Type)  \
		return 1; \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	static int class##Class##_readBinary (I, FILE *f) { \
		iam (Class); \
		int localVersion = Thing_version; (void) localVersion; \
		if (localVersion > our version) \
			return Melder_error ("The format of this file is too new. Download a newer version of Praat."); \
		if (! inherited (Class) readBinary (me, f)) return 0;

#define oo_END_CLASS(Class)  \
		return 1; \
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
