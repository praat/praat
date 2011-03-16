/* oo_READ_TEXT.h
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
 * pb 2005/11/24 more informative error messages
 * pb 2006/04/12 guard against too new versions
 * pb 2006/05/29 added version to oo_OBJECT and oo_COLLECTION
 * pb 2007/06/09 wchar_t
 * pb 2007/06/21 asc -> tex
 * pb 2007/07/21 MelderReadString
 * pb 2007/07/22 improved reading of <exists>
 * pb 2008/01/19 NUM##storage
 * pb 2008/03/20 layout
 * pb 2008/11/04 MelderReadText
 * pb 2009/03/21 modern enums
 * pb 2011/03/03 removed oo_STRINGx
 */

#include "oo_undef.h"

#define oo_SIMPLE(type,storage,x)  \
	my x = texget##storage (text); \
	iferror return Melder_error3 (L"Trying to read \"", L"" #x, L"\".");

#define oo_ARRAY(type,storage,x,cap,n)  \
	if (n > cap) return Melder_error ("Number of \"%s\" (%d) greater than %d.", #x, n, cap); \
	for (long i = 0; i < n; i ++) { \
		my x [i] = texget##storage (text); \
		iferror return Melder_error ("Trying to read element %ld of \"%s\".", i+1, #x); \
	}

#define oo_SET(type,storage,x,setType)  \
	for (long i = 0; i <= setType##_MAX; i ++) { \
		my x [i] = texget##storage (text); \
		iferror return Melder_error ("Trying to read element %ld of \"%s\".", i+1, #x); \
	}

#define oo_VECTOR(type,t,storage,x,min,max)  \
	if (max >= min && ! (my x = NUM##t##vector_readText_##storage (min, max, text, #x))) return 0;

#define oo_MATRIX(type,t,storage,x,row1,row2,col1,col2)  \
	if (row2 >= row1 && col2 >= col1 && \
	    ! (my x = NUM##t##matrix_readText_##storage (row1, row2, col1, col2, text, #x))) return 0;

#define oo_ENUMx(type,storage,Type,x)  \
	if ((my x = texget##storage (text, Type##_getValue)) < 0) return 0;

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  \
	if (n > cap) return Melder_error ("Number of \"%s\" (%d) greater than %d.", #x, n, cap); \
	for (long i = 0; i < n; i ++) \
		if ((my x [i] = texget##storage (text, Type##_getValue)) < 0) return 0;

#define oo_ENUMx_SET(type,storage,Type,x,setType)  \
	for (long i = 0; i <= setType##_MAX; i ++) \
		if ((my x [i] = texget##storage (text, & Type##_getValue)) < 0) return 0;

#define oo_ENUMx_VECTOR(type,t,storage,Type,x,min,max)  \
	if (max >= min) { \
		if (! (my x = NUM##t##vector (min, max))) return 0; \
		for (long i = min; i <= max; i ++) \
			if ((my x [i] = texget##storage (text, & Type##_getValue)) < 0) return 0; \
	}

#define oo_STRINGWx(storage,x)  \
	if (! (my x = texget##storage (text))) return Melder_error ("Trying to read \"%s\".", #x);

#define oo_STRINGWx_ARRAY(storage,x,cap,n)  \
	if (n > cap) return Melder_error ("Number of \"%s\" (%d) greater than %d.", #x, n, cap); \
	for (long i = 0; i < n; i ++) \
		if (! (my x [i] = texget##storage (text))) return 0;

#define oo_STRINGWx_SET(storage,x,setType)  \
	for (long i = 0; i <= setType##_MAX; i ++) \
		if (! (my x [i] = texget##storage (text))) return 0;

#define oo_STRINGWx_VECTOR(storage,x,min,max)  \
	if (max >= min) { \
		if (! (my x = NUMvector (sizeof (wchar_t *), min, max))) return 0; \
		for (long i = min; i <= max; i ++) { \
			if (! (my x [i] = texget##storage (text))) \
				return Melder_error ("Trying to read element %ld of \"%s\".", i, #x); \
		} \
	}

#define oo_STRUCT(Type,x)  \
	if (! Type##_readText (& my x, text)) return 0;

#define oo_STRUCT_ARRAY(Type,x,cap,n) \
	if (n > cap) return Melder_error ("Number of \"%s\" (%d) greater than %d.", #x, n, cap); \
	for (long i = 0; i < n; i ++) \
		if (! Type##_readText (& my x [i], text)) return 0;

#define oo_STRUCT_SET(Type,x,setType) \
	for (long i = 0; i <= setType##_MAX; i ++) \
		if (! Type##_readText (& my x [i], text)) return 0;

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (max >= min) { \
		if (! (my x = NUMstructvector (Type, min, max))) return 0; \
		for (long i = min; i <= max; i ++) \
			if (! Type##_readText (& my x [i], text)) return 0; \
	}

#define oo_OBJECT(Class,version,x)  \
	if (texgetex (text) == 1) { \
		long saveVersion = Thing_version; \
		if ((my x = Thing_new (Class)) == NULL) return 0; \
		Thing_version = version; \
		if (! Data_readText (my x, text)) return 0; \
		Thing_version = saveVersion; \
	}

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	{ \
		long n = texgeti4 (text); \
		if ((my x = Class##_create ()) == NULL) return 0; \
		for (long i = 1; i <= n; i ++) { \
			long saveVersion = Thing_version; \
			ItemClass item = (ItemClass) Thing_new (ItemClass); \
			if (item == NULL) return 0; \
			Thing_version = version; \
			if (! item -> methods -> readText (item, text)) return 0; \
			Thing_version = saveVersion; \
			if (! Collection_addItem (my x, item)) return 0; \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type)  \
	static int Type##_readText (Type me, MelderReadText text) { \
		int localVersion = Thing_version; (void) localVersion;

#define oo_END_STRUCT(Type)  \
		return 1; \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	static int class##Class##_readText (I, MelderReadText text) { \
		iam (Class); \
		int localVersion = Thing_version; (void) localVersion; \
		if (localVersion > our version) \
			return Melder_error ("The format of this file is too new. Download a newer version of Praat."); \
		if (! inherited (Class) readText (me, text)) return 0;

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
#define oo_READING_TEXT  1
#define oo_READING_BINARY  0
#define oo_READING_CACHE  0
#define oo_READING_LISP  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_WRITING_CACHE  0
#define oo_WRITING_LISP  0
#define oo_DESCRIBING  0

/* End of file oo_READ_TEXT.h */
