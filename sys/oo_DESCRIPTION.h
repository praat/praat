/* oo_DESCRIPTION.h
 *
 * Copyright (C) 1994-2006 Paul Boersma
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
 * pb 2003/02/07 added oo_FILE and oo_DIR (empty)
 * pb 2004/10/16 replaced structType with theStructType, struct Type with struct structType
 * pb 2006/05/29 added version to oo_OBJECT and oo_COLLECTION
 */

#include "oo_undef.h"

#undef oo_BYTE
#define oo_BYTE(x)  { #x, 1, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char) },
#undef oo_SHORT
#define oo_SHORT(x)  { #x, 2, (int) & ((ooSTRUCT) 0) -> x, sizeof (short) },
#undef oo_INT
#define oo_INT(x)  { #x, 3, (int) & ((ooSTRUCT) 0) -> x, sizeof (int) },
#undef oo_LONG
#define oo_LONG(x)  { #x, 4, (int) & ((ooSTRUCT) 0) -> x, sizeof (long) },
#undef oo_UBYTE
#define oo_UBYTE(x)  { #x, 5, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned char) },
#undef oo_USHORT
#define oo_USHORT(x)  { #x, 6, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned short) },
#undef oo_UINT
#define oo_UINT(x)  { #x, 7, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned int) },
#undef oo_ULONG
#define oo_ULONG(x)  { #x, 8, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned long) },
#undef oo_FLOAT
#define oo_FLOAT(x)  { #x, 9, (int) & ((ooSTRUCT) 0) -> x, sizeof (float) },
#undef oo_DOUBLE
#define oo_DOUBLE(x)  { #x, 10, (int) & ((ooSTRUCT) 0) -> x, sizeof (double) },
#undef oo_FCOMPLEX
#define oo_FCOMPLEX(x)  { #x, 11, (int) & ((ooSTRUCT) 0) -> x, sizeof (fcomplex) },
#undef oo_DCOMPLEX
#define oo_DCOMPLEX(x)  { #x, 12, (int) & ((ooSTRUCT) 0) -> x, sizeof (dcomplex) },
#undef oo_CHAR
#define oo_CHAR(x)  { #x, 13, (int) & ((ooSTRUCT) 0) -> x, sizeof (char) },

#undef oo_BYTE_ARRAY
#define oo_BYTE_ARRAY(x,cap,n)  { #x, 1, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, - cap, 0, #n },
#undef oo_SHORT_ARRAY
#define oo_SHORT_ARRAY(x,cap,n)  { #x, 2, (int) & ((ooSTRUCT) 0) -> x, sizeof (short), 0, 0, - cap, 0, #n },
#undef oo_INT_ARRAY
#define oo_INT_ARRAY(x,cap,n)  { #x, 3, (int) & ((ooSTRUCT) 0) -> x, sizeof (int), 0, 0, - cap, 0, #n },
#undef oo_LONG_ARRAY
#define oo_LONG_ARRAY(x,cap,n)  { #x, 4, (int) & ((ooSTRUCT) 0) -> x, sizeof (long), 0, 0, - cap, 0, #n },
#undef oo_UBYTE_ARRAY
#define oo_UBYTE_ARRAY(x,cap,n)  { #x, 5, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned char), 0, 0, - cap, 0, #n },
#undef oo_USHORT_ARRAY
#define oo_USHORT_ARRAY(x,cap,n)  { #x, 6, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned short), 0, 0, - cap, 0, #n },
#undef oo_UINT_ARRAY
#define oo_UINT_ARRAY(x,cap,n)  { #x, 7, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned int), 0, 0, - cap, 0, #n },
#undef oo_ULONG_ARRAY
#define oo_ULONG_ARRAY(x,cap,n)  { #x, 8, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned long), 0, 0, - cap, 0, #n },
#undef oo_FLOAT_ARRAY
#define oo_FLOAT_ARRAY(x,cap,n)  { #x, 9, (int) & ((ooSTRUCT) 0) -> x, sizeof (float), 0, 0, - cap, 0, #n },
#undef oo_DOUBLE_ARRAY
#define oo_DOUBLE_ARRAY(x,cap,n)  { #x, 10, (int) & ((ooSTRUCT) 0) -> x, sizeof (double), 0, 0, - cap, 0, #n },
#undef oo_FCOMPLEX_ARRAY
#define oo_FCOMPLEX_ARRAY(x,cap,n)  { #x, 11, (int) & ((ooSTRUCT) 0) -> x, sizeof (fcomplex), 0, 0, - cap, 0, #n },
#undef oo_DCOMPLEX_ARRAY
#define oo_DCOMPLEX_ARRAY(x,cap,n)  { #x, 12, (int) & ((ooSTRUCT) 0) -> x, sizeof (dcomplex), 0, 0, - cap, 0, #n },
#undef oo_CHAR_ARRAY
#define oo_CHAR_ARRAY(x,cap,n)  { #x, 13, (int) & ((ooSTRUCT) 0) -> x, sizeof (char), 0, 0, - cap, 0, #n },

#undef oo_BYTE_SET
#define oo_BYTE_SET(x,setType)  { #x, 1, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_SHORT_SET
#define oo_SHORT_SET(x,setType)  { #x, 2, (int) & ((ooSTRUCT) 0) -> x, sizeof (short), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_INT_SET
#define oo_INT_SET(x,setType)  { #x, 3, (int) & ((ooSTRUCT) 0) -> x, sizeof (int), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_LONG_SET
#define oo_LONG_SET(x,setType)  { #x, 4, (int) & ((ooSTRUCT) 0) -> x, sizeof (long), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_UBYTE_SET
#define oo_UBYTE_SET(x,setType)  { #x, 5, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned char), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_USHORT_SET
#define oo_USHORT_SET(x,setType)  { #x, 6, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned short), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_UINT_SET
#define oo_UINT_SET(x,setType)  { #x, 7, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned int), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_ULONG_SET
#define oo_ULONG_SET(x,setType)  { #x, 8, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned long), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_FLOAT_SET
#define oo_FLOAT_SET(x,setType)  { #x, 9, (int) & ((ooSTRUCT) 0) -> x, sizeof (float), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_DOUBLE_SET
#define oo_DOUBLE_SET(x,setType)  { #x, 10, (int) & ((ooSTRUCT) 0) -> x, sizeof (double), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_FCOMPLEX_SET
#define oo_FCOMPLEX_SET(x,setType)  { #x, 11, (int) & ((ooSTRUCT) 0) -> x, sizeof (fcomplex), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_DCOMPLEX_SET
#define oo_DCOMPLEX_SET(x,setType)  { #x, 12, (int) & ((ooSTRUCT) 0) -> x, sizeof (dcomplex), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_CHAR_SET
#define oo_CHAR_SET(x,setType)  { #x, 13, (int) & ((ooSTRUCT) 0) -> x, sizeof (char), 0, 0, 3, 0, (char *) & enum_##setType },

#undef oo_BYTE_VECTOR_FROM
#define oo_BYTE_VECTOR_FROM(x,min,max)  { #x, 1, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, 1, #min, #max },
#undef oo_SHORT_VECTOR_FROM
#define oo_SHORT_VECTOR_FROM(x,min,max)   { #x, 2, (int) & ((ooSTRUCT) 0) -> x, sizeof (short), 0, 0, 1, #min, #max },
#undef oo_INT_VECTOR_FROM
#define oo_INT_VECTOR_FROM(x,min,max)  { #x, 3, (int) & ((ooSTRUCT) 0) -> x, sizeof (int), 0, 0, 1, #min, #max },
#undef oo_LONG_VECTOR_FROM
#define oo_LONG_VECTOR_FROM(x,min,max)  { #x, 4, (int) & ((ooSTRUCT) 0) -> x, sizeof (long), 0, 0, 1, #min, #max },
#undef oo_UBYTE_VECTOR_FROM
#define oo_UBYTE_VECTOR_FROM(x,min,max)  { #x, 5, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned char), 0, 0, 1, #min, #max },
#undef oo_USHORT_VECTOR_FROM
#define oo_USHORT_VECTOR_FROM(x,min,max)   { #x, 6, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned short), 0, 0, 1, #min, #max },
#undef oo_UINT_VECTOR_FROM
#define oo_UINT_VECTOR_FROM(x,min,max)  { #x, 7, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned int), 0, 0, 1, #min, #max },
#undef oo_ULONG_VECTOR_FROM
#define oo_ULONG_VECTOR_FROM(x,min,max)  { #x, 8, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned long), 0, 0, 1, #min, #max },
#undef oo_FLOAT_VECTOR_FROM
#define oo_FLOAT_VECTOR_FROM(x,min,max)  { #x, 9, (int) & ((ooSTRUCT) 0) -> x, sizeof (float), 0, 0, 1, #min, #max },
#undef oo_DOUBLE_VECTOR_FROM
#define oo_DOUBLE_VECTOR_FROM(x,min,max)  { #x, 10, (int) & ((ooSTRUCT) 0) -> x, sizeof (double), 0, 0, 1, #min, #max },
#undef oo_FCOMPLEX_VECTOR_FROM
#define oo_FCOMPLEX_VECTOR_FROM(x,min,max)  { #x, 11, (int) & ((ooSTRUCT) 0) -> x, sizeof (fcomplex), 0, 0, 1, #min, #max },
#undef oo_DCOMPLEX_VECTOR_FROM
#define oo_DCOMPLEX_VECTOR_FROM(x,min,max)  { #x, 12, (int) & ((ooSTRUCT) 0) -> x, sizeof (dcomplex), 0, 0, 1, #min, #max },
#undef oo_CHAR_VECTOR_FROM
#define oo_CHAR_VECTOR_FROM(x,min,max)  { #x, 13, (int) & ((ooSTRUCT) 0) -> x, sizeof (char), 0, 0, 1, #min, #max },

#undef oo_BYTE_MATRIX_FROM
#define oo_BYTE_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 1, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, 2, #r1, #r2, #c1, #c2 },
#undef oo_SHORT_MATRIX_FROM
#define oo_SHORT_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 2, (int) & ((ooSTRUCT) 0) -> x, sizeof (short), 0, 0, 2, #r1, #r2, #c1, #c2 },
#undef oo_INT_MATRIX_FROM
#define oo_INT_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 3, (int) & ((ooSTRUCT) 0) -> x, sizeof (int), 0, 0, 2, #r1, #r2, #c1, #c2 },
#undef oo_LONG_MATRIX_FROM
#define oo_LONG_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 4, (int) & ((ooSTRUCT) 0) -> x, sizeof (long), 0, 0, 2, #r1, #r2, #c1, #c2 },
#undef oo_UBYTE_MATRIX_FROM
#define oo_UBYTE_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 5, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned char), 0, 0, 2, #r1, #r2, #c1, #c2 },
#undef oo_USHORT_MATRIX_FROM
#define oo_USHORT_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 6, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned short), 0, 0, 2, #r1, #r2, #c1, #c2 },
#undef oo_UINT_MATRIX_FROM
#define oo_UINT_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 7, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned int), 0, 0, 2, #r1, #r2, #c1, #c2 },
#undef oo_ULONG_MATRIX_FROM
#define oo_ULONG_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 8, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned long), 0, 0, 2, #r1, #r2, #c1, #c2 },
#undef oo_FLOAT_MATRIX_FROM
#define oo_FLOAT_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 9, (int) & ((ooSTRUCT) 0) -> x, sizeof (float), 0, 0, 2, #r1, #r2, #c1, #c2 },
#undef oo_DOUBLE_MATRIX_FROM
#define oo_DOUBLE_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 10, (int) & ((ooSTRUCT) 0) -> x, sizeof (double), 0, 0, 2, #r1, #r2, #c1, #c2 },
#undef oo_FCOMPLEX_MATRIX_FROM
#define oo_FCOMPLEX_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 11, (int) & ((ooSTRUCT) 0) -> x, sizeof (fcomplex), 0, 0, 2, #r1, #r2, #c1, #c2 },
#undef oo_DCOMPLEX_MATRIX_FROM
#define oo_DCOMPLEX_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 12, (int) & ((ooSTRUCT) 0) -> x, sizeof (dcomplex), 0, 0, 2, #r1, #r2, #c1, #c2 },
#undef oo_CHAR_MATRIX_FROM
#define oo_CHAR_MATRIX_FROM(x,r1,r2,c1,c2)  { #x, 13, (int) & ((ooSTRUCT) 0) -> x, sizeof (char), 0, 0, 2, #r1, #r2, #c1, #c2 },

#undef oo_BYTE_VECTOR
#define oo_BYTE_VECTOR(x,n)  { #x, 1, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, 1, 0, #n },
#undef oo_SHORT_VECTOR
#define oo_SHORT_VECTOR(x,n)   { #x, 2, (int) & ((ooSTRUCT) 0) -> x, sizeof (short), 0, 0, 1, 0, #n },
#undef oo_INT_VECTOR
#define oo_INT_VECTOR(x,n)  { #x, 3, (int) & ((ooSTRUCT) 0) -> x, sizeof (int), 0, 0, 1, 0, #n },
#undef oo_LONG_VECTOR
#define oo_LONG_VECTOR(x,n)  { #x, 4, (int) & ((ooSTRUCT) 0) -> x, sizeof (long), 0, 0, 1, 0, #n },
#undef oo_UBYTE_VECTOR
#define oo_UBYTE_VECTOR(x,n)  { #x, 5, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned char), 0, 0, 1, 0, #n },
#undef oo_USHORT_VECTOR
#define oo_USHORT_VECTOR(x,n)   { #x, 6, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned short), 0, 0, 1, 0, #n },
#undef oo_UINT_VECTOR
#define oo_UINT_VECTOR(x,n)  { #x, 7, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned int), 0, 0, 1, 0, #n },
#undef oo_ULONG_VECTOR
#define oo_ULONG_VECTOR(x,n)  { #x, 8, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned long), 0, 0, 1, 0, #n },
#undef oo_FLOAT_VECTOR
#define oo_FLOAT_VECTOR(x,n)  { #x, 9, (int) & ((ooSTRUCT) 0) -> x, sizeof (float), 0, 0, 1, 0, #n },
#undef oo_DOUBLE_VECTOR
#define oo_DOUBLE_VECTOR(x,n)  { #x, 10, (int) & ((ooSTRUCT) 0) -> x, sizeof (double), 0, 0, 1, 0, #n },
#undef oo_FCOMPLEX_VECTOR
#define oo_FCOMPLEX_VECTOR(x,n)  { #x, 11, (int) & ((ooSTRUCT) 0) -> x, sizeof (fcomplex), 0, 0, 1, 0, #n },
#undef oo_CCOMPLEX_VECTOR
#define oo_CCOMPLEX_VECTOR(x,n)  { #x, 12, (int) & ((ooSTRUCT) 0) -> x, sizeof (dcomplex), 0, 0, 1, 0, #n },
#undef oo_CHAR_VECTOR
#define oo_CHAR_VECTOR(x,n)  { #x, 13, (int) & ((ooSTRUCT) 0) -> x, sizoef (char), 0, 0, 1, 0, #n },

#undef oo_BYTE_MATRIX
#define oo_BYTE_MATRIX(x,nrow,ncol)  { #x, 1, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, 2, 0, #nrow, 0, #ncol },
#undef oo_SHORT_MATRIX
#define oo_SHORT_MATRIX(x,nrow,ncol)  { #x, 2, (int) & ((ooSTRUCT) 0) -> x, sizeof (short), 0, 0, 2, 0, #nrow, 0, #ncol },
#undef oo_INT_MATRIX
#define oo_INT_MATRIX(x,nrow,ncol)  { #x, 3, (int) & ((ooSTRUCT) 0) -> x, sizeof (int), 0, 0, 2, 0, #nrow, 0, #ncol },
#undef oo_LONG_MATRIX
#define oo_LONG_MATRIX(x,nrow,ncol)  { #x, 4, (int) & ((ooSTRUCT) 0) -> x, sizeof (long), 0, 0, 2, 0, #nrow, 0, #ncol },
#undef oo_UBYTE_MATRIX
#define oo_UBYTE_MATRIX(x,nrow,ncol)  { #x, 5, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned char), 0, 0, 2, 0, #nrow, 0, #ncol },
#undef oo_USHORT_MATRIX
#define oo_USHORT_MATRIX(x,nrow,ncol)  { #x, 6, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned short), 0, 0, 2, 0, #nrow, 0, #ncol },
#undef oo_UINT_MATRIX
#define oo_UINT_MATRIX(x,nrow,ncol)  { #x, 7, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned int), 0, 0, 2, 0, #nrow, 0, #ncol },
#undef oo_ULONG_MATRIX
#define oo_ULONG_MATRIX(x,nrow,ncol)  { #x, 8, (int) & ((ooSTRUCT) 0) -> x, sizeof (unsigned long), 0, 0, 2, 0, #nrow, 0, #ncol },
#undef oo_FLOAT_MATRIX
#define oo_FLOAT_MATRIX(x,nrow,ncol)  { #x, 9, (int) & ((ooSTRUCT) 0) -> x, sizeof (float), 0, 0, 2, 0, #nrow, 0, #ncol },
#undef oo_DOUBLE_MATRIX
#define oo_DOUBLE_MATRIX(x,nrow,ncol)  { #x, 10, (int) & ((ooSTRUCT) 0) -> x, sizeof (double), 0, 0, 2, 0, #nrow, 0, #ncol },
#undef oo_FCOMPLEX_MATRIX
#define oo_FCOMPLEX_MATRIX(x,nrow,ncol)  { #x, 11, (int) & ((ooSTRUCT) 0) -> x, sizeof (fcomplex), 0, 0, 2, 0, #nrow, 0, #ncol },
#undef oo_DCOMPLEX_MATRIX
#define oo_DCOMPLEX_MATRIX(x,nrow,ncol)  { #x, 12, (int) & ((ooSTRUCT) 0) -> x, sizeof (dcomplex), 0, 0, 2, 0, #nrow, 0, #ncol },
#undef oo_CHAR_MATRIX
#define oo_CHAR_MATRIX(x,nrow,ncol)  { #x, 13, (int) & ((ooSTRUCT) 0) -> x, sizeof (char), 0, 0, 2, 0, #nrow, 0, #ncol },

#undef oo_ENUM
#define oo_ENUM(Type,x)  { #x, 14, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), #Type, & enum_##Type },
#undef oo_LENUM
#define oo_LENUM(Type,x)  { #x, 15, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed short), #Type, & enum_##Type },
#undef oo_ENUM_ARRAY
#define oo_ENUM_ARRAY(Type,x,cap,n)  { #x, 14, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), #Type, & enum_##Type, - cap, 0, #n },
#undef oo_LENUM_ARRAY
#define oo_LENUM_ARRAY(Type,x,cap,n)  { #x, 15, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed short), #Type, & enum_##Type, - cap, 0, #n },
#undef oo_ENUM_SET
#define oo_ENUM_SET(Type,x,setType)  { #x, 14, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), #Type, & enum_##Type, 3, 0, (char *) & enum_##setType },
#undef oo_LENUM_SET
#define oo_LENUM_SET(Type,x,setType)  { #x, 15, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed short), #Type, & enum_##Type, 3, 0, (char *) & enum_##setType },
#undef oo_ENUM_VECTOR_FROM
#define oo_ENUM_VECTOR_FROM(Type,x,min,max)  { #x, 14, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), #Type, & enum_##Type, 1, #min, #max },
#undef oo_LENUM_VECTOR_FROM
#define oo_LENUM_VECTOR_FROM(Type,x,min,max)  { #x, 15, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed short), #Type, & enum_##Type, 1, #min, #max },
#undef oo_ENUM_VECTOR
#define oo_ENUM_VECTOR(Type,x,n)  { #x, 14, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), #Type, & enum_##Type, 1, 0, #n },
#undef oo_LENUM_VECTOR
#define oo_LENUM_VECTOR(Type,x,n)  { #x, 15, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed short), #Type, & enum_##Type, 1, 0, #n },
#undef oo_BOOLEAN
#define oo_BOOLEAN(x)  { #x, 16, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char) },
#undef oo_BOOLEAN_ARRAY
#define oo_BOOLEAN_ARRAY(x,cap,n)  { #x, 16, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, - cap, 0, #n },
#undef oo_BOOLEAN_SET
#define oo_BOOLEAN_SET(x,setType)  { #x, 16, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_BOOLEAN_VECTOR_FROM
#define oo_BOOLEAN_VECTOR_FROM(x,min,max)  { #x, 16, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, 1, #min, #max },
#undef oo_BOOLEAN_VECTOR
#define oo_BOOLEAN_VECTOR(x,n)  { #x, 16, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, 1, 0, #n },
#undef oo_QUESTION
#define oo_QUESTION(x)  { #x, 17, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char) },
#undef oo_QUESTION_ARRAY
#define oo_QUESTION_ARRAY(x,cap,n)  { #x, 17, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, - cap, 0, #n },
#undef oo_QUESTION_SET
#define oo_QUESTION_SET(x,setType)  { #x, 17, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_QUESTION_VECTOR_FROM
#define oo_QUESTION_VECTOR_FROM(x,min,max)  { #x, 17, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, 1, #min, #max },
#undef oo_QUESTION_VECTOR
#define oo_QUESTION_VECTOR(x,n)  { #x, 17, (int) & ((ooSTRUCT) 0) -> x, sizeof (signed char), 0, 0, 1, 0, #n },

#undef oo_SSTRING
#define oo_SSTRING(x)  { #x, 18, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *) },
#undef oo_STRING
#define oo_STRING(x)  { #x, 19, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *) },
#undef oo_LSTRING
#define oo_LSTRING(x)  { #x, 20, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *) },
#undef oo_SSTRING_ARRAY
#define oo_SSTRING_ARRAY(x,cap,n)  { #x, 18, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *), 0, 0, - cap, 0, #n },
#undef oo_STRING_ARRAY
#define oo_STRING_ARRAY(x,cap,n)  { #x, 19, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *), 0, 0, - cap, 0, #n },
#undef oo_LSTRING_ARRAY
#define oo_LSTRING_ARRAY(x,cap,n)  { #x, 20, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *), 0, 0, - cap, 0, #n },
#undef oo_SSTRING_SET
#define oo_SSTRING_SET(x,setType)  { #x, 18, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_STRING_SET
#define oo_STRING_SET(x,setType)  { #x, 19, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_LSTRING_SET
#define oo_LSTRING_SET(x,setType)  { #x, 20, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *), 0, 0, 3, 0, (char *) & enum_##setType },
#undef oo_SSTRING_VECTOR_FROM
#define oo_SSTRING_VECTOR_FROM(x,min,max)  { #x, 18, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *), 0, 0, 1, #min, #max },
#undef oo_STRING_VECTOR_FROM
#define oo_STRING_VECTOR_FROM(x,min,max)  { #x, 19, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *), 0, 0, 1, #min, #max },
#undef oo_LSTRING_VECTOR_FROM
#define oo_LSTRING_VECTOR_FROM(x,min,max)  { #x, 20, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *), 0, 0, 1, #min, #max },
#undef oo_SSTRING_VECTOR
#define oo_SSTRING_VECTOR(x,n)  { #x, 18, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *), 0, 0, 1, 0, #n },
#undef oo_STRING_VECTOR
#define oo_STRING_VECTOR(x,n)  { #x, 19, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *), 0, 0, 1, 0, #n },
#undef oo_LSTRING_VECTOR
#define oo_LSTRING_VECTOR(x,n)  { #x, 20, (int) & ((ooSTRUCT) 0) -> x, sizeof (char *), 0, 0, 1, 0, #n },

#define oo_STRUCT(Type,x)  { #x, 21, (int) & ((ooSTRUCT) 0) -> x, sizeof (struct struct##Type), #Type, Type##_description },
#define oo_STRUCT_ARRAY(Type,x,cap,n)  { #x, 21, (int) & ((ooSTRUCT) 0) -> x, sizeof (struct struct##Type), #Type, Type##_description, - cap, 0, #n },
#define oo_STRUCT_SET(Type,x,setType)  { #x, 21, (int) & ((ooSTRUCT) 0) -> x, sizeof (struct struct##Type), #Type, Type##_description, 3, 0, (char *) & enum_##setType },
#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  { #x, 21, (int) & ((ooSTRUCT) 0) -> x, sizeof (struct struct##Type), #Type, Type##_description, 1, #min, #max },
#undef oo_STRUCT_VECTOR
#define oo_STRUCT_VECTOR(Type,x,n)  { #x, 21, (int) & ((ooSTRUCT) 0) -> x, sizeof (struct struct##Type), #Type, Type##_description, 1, 0, #n },

#define oo_WIDGET(x)  { #x, 22, (int) & ((ooSTRUCT) 0) -> x, sizeof (Widget) },
#define oo_WIDGET_ARRAY(x,cap,n)  { #x, 22, (int) & ((ooSTRUCT) 0) -> x, sizeof (Widget), 0, 0, - cap, 0, #n },
#define oo_WIDGET_SET(x,setType)  { #x, 22, (int) & ((ooSTRUCT) 0) -> x, sizeof (Widget), 0, 0, 3, 0, (char *) & enum_##setType },
#define oo_WIDGET_VECTOR_FROM(x,min,max)  { #x, 22, (int) & ((ooSTRUCT) 0) -> x, sizeof (Widget), 0, 0, 1, #min, #max },
#undef oo_WIDGET_VECTOR
#define oo_WIDGET_VECTOR(Type,x,n)  { #x, 22, (int) & ((ooSTRUCT) 0) -> x, sizeof (Widget), 0, 0, 1, 0, #n },

#define oo_OBJECT(Type,version,x)  { #x, 23, (int) & ((ooSTRUCT) 0) -> x, sizeof (Type), #Type, & theStruct##Type },
#define oo_COLLECTION(Type,x,ItemType,version)  { #x, 24, (int) & ((ooSTRUCT) 0) -> x, sizeof (struct struct##ItemType), #Type, & theStruct##Type, 0, (const char *) & theStruct##ItemType },
#define oo_FILE(x)
#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type)  static struct structData_Description Type##_description [] = {
#define oo_END_STRUCT(Type)  { 0 } };

#define oo_DEFINE_CLASS(Class,Parent)  static struct structData_Description class##Class##_description [] = { \
	{ #Class, 25, 0, sizeof (struct struct##Class), #Class, & theStruct##Parent. description },
#define oo_END_CLASS(Class)  { 0 } };

#define oo_IF(condition)
#define oo_ENDIF

#define oo_FROM(from)
#define oo_ENDFROM

#define oo_VERSION(version)

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
#define oo_WRITING  0
#define oo_WRITING_ASCII  0
#define oo_WRITING_BINARY  0
#define oo_WRITING_CACHE  0
#define oo_WRITING_LISP  0
#define oo_DESCRIBING  1

/* End of file oo_DESCRIPTION.h */
