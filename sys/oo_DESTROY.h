/* oo_DESTROY.h
 *
 * Copyright (C) 1994-2003 Paul Boersma
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
 * pb 2002/06/01
 * pb 2003/02/07 added oo_FILE and oo_DIR
 * pb 2003/06/11 made struct_destroy global
 */

#include "oo_undef.h"



#define oo_SIMPLE(type,storage,x)

#define oo_ARRAY(type,storage,x,cap,n)

#define oo_SET(type,storage,x,setType)

#define oo_VECTOR(type,t,storage,x,min,max)  \
	NUM##t##vector_free (my x, min);

#define oo_MATRIX(type,t,storage,x,row1,row2,col1,col2)  \
	NUM##t##matrix_free (my x, row1, col1);



#define oo_ENUMx(type,storage,Type,x)

#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)

#define oo_ENUMx_SET(type,storage,Type,x,setType)

#define oo_ENUMx_VECTOR(type,t,storage,Type,x,min,max)  \
	NUM##t##vector_free (my x, min);



#define oo_STRINGx(storage,x)  \
	Melder_free (my x);

#define oo_STRINGx_ARRAY(storage,x,cap,n)  \
	{ \
		int i; \
		for (i = 0; i < n; i ++) \
			Melder_free (my x [i]); \
	}

#define oo_STRINGx_SET(storage,x,setType)  \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			Melder_free (my x [i]); \
	}

#define oo_STRINGx_VECTOR(storage,x,min,max)  \
	if (my x) { \
		long i; \
		for (i = min; i <= max; i ++) \
			Melder_free (my x [i]); \
		NUMvector_free (sizeof (char *), my x, min); \
	}



#define oo_STRUCT(Type,x)  \
	Type##_destroy (& my x);

#define oo_STRUCT_ARRAY(Type,x,cap,n)  \
	{ \
		int i; \
		for (i = 0; i < n; i ++) \
			Type##_destroy (& my x [i]); \
	}

#define oo_STRUCT_SET(Type,x,setType)  \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			Type##_destroy (& my x [i]); \
	}

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	if (my x) { \
		long i; \
		for (i = min; i <= max; i ++) \
			Type##_destroy (& my x [i]); \
		NUMstructvector_free (Type, my x, min); \
	}

#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  \
	if (my x) { \
		long i, j; \
		for (i = row1; i <= row2; i ++) \
			for (j = col1; j <= col2; j ++) \
				Type##_destroy (& my x [i] [j]); \
		NUMstructmatrix_free (Type, my x, row1, col1); \
	}



#define oo_WIDGET(x)  \
	if (my x) XtDestroyWidget (my x);

#define oo_WIDGET_ARRAY(x,cap,n)  \
	{ \
		int i; \
		for (i = 0; i < n; i ++) \
			if (my x [i]) XtDestroyWidget (my x [i]); \
	}

#define oo_WIDGET_SET(x,setType)  \
	{ \
		int i; \
		for (i = 0; i <= enumlength (setType); i ++) \
			if (my x [i]) XtDestroyWidget (my x [i]); \
	}

#define oo_WIDGET_VECTOR_FROM(x,min,max)  \
	if (my x) { \
		long i; \
		for (i = min; i <= max; i ++) XtDestroyWidget (my x [i]); \
		NUMvector_free (sizeof (Widget), my x, min, 0); \
	}



#define oo_OBJECT(Type,x)  \
	forget (my x);

#define oo_COLLECTION(Type,x,ItemType)  \
	forget (my x);

#define oo_FILE(x)

#define oo_DIR(x)



#define oo_DEFINE_STRUCT(Type)  \
	void Type##_destroy (Type me) { (void) me;

#define oo_END_STRUCT(Type)  \
	}



#define oo_DEFINE_CLASS(Class,Parent)  \
	static void class##Class##_destroy (I) { \
		iam (Class);

#define oo_END_CLASS(Class)  \
		inherited (Class) destroy (me); \
	}



#define oo_IF(condition)  \
	if (condition) {

#define oo_ENDIF  \
	}



#define oo_FROM(from)

#define oo_ENDFROM



#define oo_VERSION(version)



#define oo_DECLARING  0
#define oo_DESTROYING  1
#define oo_COPYING  0
#define oo_EQUALLING  0
#define oo_COMPARING  0
#define oo_READING  0
#define oo_READING_ASCII  0
#define oo_READING_BINARY  0
#define oo_READING_CACHE  0
#define oo_READING_LISP  0
#define oo_WRITING  0
#define oo_WRITING_ASCII  0
#define oo_WRITING_BINARY  0
#define oo_WRITING_CACHE  0
#define oo_WRITING_LISP  0
#define oo_DESCRIBING  0

/* End of file oo_DESTROY.h */
