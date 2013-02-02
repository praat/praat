/* oo_DESCRIPTION.h
 *
 * Copyright (C) 1994-2012,2013 Paul Boersma
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

#undef oo_BYTE
#define oo_BYTE(x)  { L"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char) },
#undef oo_INT
#define oo_INT(x)  { L"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int) },
#undef oo_LONG
#define oo_LONG(x)  { L"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long) },
#undef oo_UBYTE
#define oo_UBYTE(x)  { L"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char) },
#undef oo_UINT
#define oo_UINT(x)  { L"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int) },
#undef oo_ULONG
#define oo_ULONG(x)  { L"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long) },
#undef oo_BOOL
#define oo_BOOL(x)  { L"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool) },
#undef oo_FLOAT
#define oo_FLOAT(x)  { L"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double) },
#undef oo_DOUBLE
#define oo_DOUBLE(x)  { L"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double) },
#undef oo_FCOMPLEX
#define oo_FCOMPLEX(x)  { L"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex) },
#undef oo_DCOMPLEX
#define oo_DCOMPLEX(x)  { L"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex) },

#undef oo_BYTE_ARRAY
#define oo_BYTE_ARRAY(x,cap,n)  { L"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_INT_ARRAY
#define oo_INT_ARRAY(x,cap,n)  { L"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_LONG_ARRAY
#define oo_LONG_ARRAY(x,cap,n)  { L"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_UBYTE_ARRAY
#define oo_UBYTE_ARRAY(x,cap,n)  { L"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_UINT_ARRAY
#define oo_UINT_ARRAY(x,cap,n)  { L"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_ULONG_ARRAY
#define oo_ULONG_ARRAY(x,cap,n)  { L"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_BOOL_ARRAY
#define oo_BOOL_ARRAY(x,cap,n)  { L"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_FLOAT_ARRAY
#define oo_FLOAT_ARRAY(x,cap,n)  { L"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_DOUBLE_ARRAY
#define oo_DOUBLE_ARRAY(x,cap,n)  { L"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_FCOMPLEX_ARRAY
#define oo_FCOMPLEX_ARRAY(x,cap,n)  { L"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_DCOMPLEX_ARRAY
#define oo_DCOMPLEX_ARRAY(x,cap,n)  { L"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, - cap, (const wchar_t *) 0, L"" #n },

#undef oo_BYTE_SET
#define oo_BYTE_SET(x,setType)  { L"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue }, /* BUG function pointer to pointer */
#undef oo_INT_SET
#define oo_INT_SET(x,setType)  { L"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_LONG_SET
#define oo_LONG_SET(x,setType)  { L"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_UBYTE_SET
#define oo_UBYTE_SET(x,setType)  { L"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_UINT_SET
#define oo_UINT_SET(x,setType)  { L"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_ULONG_SET
#define oo_ULONG_SET(x,setType)  { L"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_BOOL_SET
#define oo_BOOL_SET(x,setType)  { L"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_FLOAT_SET
#define oo_FLOAT_SET(x,setType)  { L"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_DOUBLE_SET
#define oo_DOUBLE_SET(x,setType)  { L"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_FCOMPLEX_SET
#define oo_FCOMPLEX_SET(x,setType)  { L"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_DCOMPLEX_SET
#define oo_DCOMPLEX_SET(x,setType)  { L"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },

#undef oo_BYTE_VECTOR_FROM
#define oo_BYTE_VECTOR_FROM(x,min,max)  { L"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 1, L"" #min, L"" #max },
#undef oo_INT_VECTOR_FROM
#define oo_INT_VECTOR_FROM(x,min,max)  { L"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, 1, L"" #min, L"" #max },
#undef oo_LONG_VECTOR_FROM
#define oo_LONG_VECTOR_FROM(x,min,max)  { L"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, 1, L"" #min, L"" #max },
#undef oo_UBYTE_VECTOR_FROM
#define oo_UBYTE_VECTOR_FROM(x,min,max)  { L"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, 1, L"" #min, L"" #max },
#undef oo_UINT_VECTOR_FROM
#define oo_UINT_VECTOR_FROM(x,min,max)  { L"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, 1, L"" #min, L"" #max },
#undef oo_ULONG_VECTOR_FROM
#define oo_ULONG_VECTOR_FROM(x,min,max)  { L"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, 1, L"" #min, L"" #max },
#undef oo_BOOL_VECTOR_FROM
#define oo_BOOL_VECTOR_FROM(x,min,max)  { L"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 1, L"" #min, L"" #max },
#undef oo_FLOAT_VECTOR_FROM
#define oo_FLOAT_VECTOR_FROM(x,min,max)  { L"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 1, L"" #min, L"" #max },
#undef oo_DOUBLE_VECTOR_FROM
#define oo_DOUBLE_VECTOR_FROM(x,min,max)  { L"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 1, L"" #min, L"" #max },
#undef oo_FCOMPLEX_VECTOR_FROM
#define oo_FCOMPLEX_VECTOR_FROM(x,min,max)  { L"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, 1, L"" #min, L"" #max },
#undef oo_DCOMPLEX_VECTOR_FROM
#define oo_DCOMPLEX_VECTOR_FROM(x,min,max)  { L"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, 1, L"" #min, L"" #max },

#undef oo_BYTE_MATRIX_FROM
#define oo_BYTE_MATRIX_FROM(x,r1,r2,c1,c2)  { L"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 2, L"" #r1, L"" #r2, L"" #c1, L"" #c2 },
#undef oo_INT_MATRIX_FROM
#define oo_INT_MATRIX_FROM(x,r1,r2,c1,c2)  { L"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, 2, L"" #r1, L"" #r2, L"" #c1, L"" #c2 },
#undef oo_LONG_MATRIX_FROM
#define oo_LONG_MATRIX_FROM(x,r1,r2,c1,c2)  { L"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, 2, L"" #r1, L"" #r2, L"" #c1, L"" #c2 },
#undef oo_UBYTE_MATRIX_FROM
#define oo_UBYTE_MATRIX_FROM(x,r1,r2,c1,c2)  { L"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, 2, L"" #r1, L"" #r2, L"" #c1, L"" #c2 },
#undef oo_UINT_MATRIX_FROM
#define oo_UINT_MATRIX_FROM(x,r1,r2,c1,c2)  { L"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, 2, L"" #r1, L"" #r2, L"" #c1, L"" #c2 },
#undef oo_ULONG_MATRIX_FROM
#define oo_ULONG_MATRIX_FROM(x,r1,r2,c1,c2)  { L"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, 2, L"" #r1, L"" #r2, L"" #c1, L"" #c2 },
#undef oo_BOOL_MATRIX_FROM
#define oo_BOOL_MATRIX_FROM(x,r1,r2,c1,c2)  { L"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 2, L"" #r1, L"" #r2, L"" #c1, L"" #c2 },
#undef oo_FLOAT_MATRIX_FROM
#define oo_FLOAT_MATRIX_FROM(x,r1,r2,c1,c2)  { L"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 2, L"" #r1, L"" #r2, L"" #c1, L"" #c2 },
#undef oo_DOUBLE_MATRIX_FROM
#define oo_DOUBLE_MATRIX_FROM(x,r1,r2,c1,c2)  { L"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 2, L"" #r1, L"" #r2, L"" #c1, L"" #c2 },
#undef oo_FCOMPLEX_MATRIX_FROM
#define oo_FCOMPLEX_MATRIX_FROM(x,r1,r2,c1,c2)  { L"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, 2, L"" #r1, L"" #r2, L"" #c1, L"" #c2 },
#undef oo_DCOMPLEX_MATRIX_FROM
#define oo_DCOMPLEX_MATRIX_FROM(x,r1,r2,c1,c2)  { L"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, 2, L"" #r1, L"" #r2, L"" #c1, L"" #c2 },

#undef oo_BYTE_VECTOR
#define oo_BYTE_VECTOR(x,n)  { L"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 1, (const wchar_t *) 0, L"" #n },
#undef oo_INT_VECTOR
#define oo_INT_VECTOR(x,n)  { L"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, 1, (const wchar_t *) 0, L"" #n },
#undef oo_LONG_VECTOR
#define oo_LONG_VECTOR(x,n)  { L"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, 1, (const wchar_t *) 0, L"" #n },
#undef oo_UBYTE_VECTOR
#define oo_UBYTE_VECTOR(x,n)  { L"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, 1, (const wchar_t *) 0, L"" #n },
#undef oo_UINT_VECTOR
#define oo_UINT_VECTOR(x,n)  { L"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, 1, (const wchar_t *) 0, L"" #n },
#undef oo_ULONG_VECTOR
#define oo_ULONG_VECTOR(x,n)  { L"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, 1, (const wchar_t *) 0, L"" #n },
#undef oo_BOOL_VECTOR
#define oo_BOOL_VECTOR(x,n)  { L"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 1, (const wchar_t *) 0, L"" #n },
#undef oo_FLOAT_VECTOR
#define oo_FLOAT_VECTOR(x,n)  { L"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 1, (const wchar_t *) 0, L"" #n },
#undef oo_DOUBLE_VECTOR
#define oo_DOUBLE_VECTOR(x,n)  { L"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 1, (const wchar_t *) 0, L"" #n },
#undef oo_FCOMPLEX_VECTOR
#define oo_FCOMPLEX_VECTOR(x,n)  { L"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, 1, (const wchar_t *) 0, L"" #n },
#undef oo_DCOMPLEX_VECTOR
#define oo_DCOMPLEX_VECTOR(x,n)  { L"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, 1, (const wchar_t *) 0, L"" #n },

#undef oo_BYTE_MATRIX
#define oo_BYTE_MATRIX(x,nrow,ncol)  { L"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 2, (const wchar_t *) 0, L"" #nrow, (const wchar_t *) 0, L"" #ncol },
#undef oo_INT_MATRIX
#define oo_INT_MATRIX(x,nrow,ncol)  { L"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, 2, (const wchar_t *) 0, L"" #nrow, (const wchar_t *) 0, L"" #ncol },
#undef oo_LONG_MATRIX
#define oo_LONG_MATRIX(x,nrow,ncol)  { L"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, 2, (const wchar_t *) 0, L"" #nrow, (const wchar_t *) 0, L"" #ncol },
#undef oo_UBYTE_MATRIX
#define oo_UBYTE_MATRIX(x,nrow,ncol)  { L"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, 2, (const wchar_t *) 0, L"" #nrow, (const wchar_t *) 0, L"" #ncol },
#undef oo_UINT_MATRIX
#define oo_UINT_MATRIX(x,nrow,ncol)  { L"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, 2, (const wchar_t *) 0, L"" #nrow, (const wchar_t *) 0, L"" #ncol },
#undef oo_ULONG_MATRIX
#define oo_ULONG_MATRIX(x,nrow,ncol)  { L"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, 2, (const wchar_t *) 0, L"" #nrow, (const wchar_t *) 0, L"" #ncol },
#undef oo_BOOL_MATRIX
#define oo_BOOL_MATRIX(x,nrow,ncol)  { L"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 2, (const wchar_t *) 0, L"" #nrow, (const wchar_t *) 0, L"" #ncol },
#undef oo_FLOAT_MATRIX
#define oo_FLOAT_MATRIX(x,nrow,ncol)  { L"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 2, (const wchar_t *) 0, L"" #nrow, (const wchar_t *) 0, L"" #ncol },
#undef oo_DOUBLE_MATRIX
#define oo_DOUBLE_MATRIX(x,nrow,ncol)  { L"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 2, (const wchar_t *) 0, L"" #nrow, (const wchar_t *) 0, L"" #ncol },
#undef oo_FCOMPLEX_MATRIX
#define oo_FCOMPLEX_MATRIX(x,nrow,ncol)  { L"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, 2, (const wchar_t *) 0, L"" #nrow, (const wchar_t *) 0, L"" #ncol },
#undef oo_DCOMPLEX_MATRIX
#define oo_DCOMPLEX_MATRIX(x,nrow,ncol)  { L"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, 2, (const wchar_t *) 0, L"" #nrow, (const wchar_t *) 0, L"" #ncol },

#undef oo_ENUM
#define oo_ENUM(Type,x)  { L"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), L"" #Type, (void *) Type##_getText },
#undef oo_LENUM
#define oo_LENUM(Type,x)  { L"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), L"" #Type, (void *) Type##_getText },
#undef oo_ENUM_ARRAY
#define oo_ENUM_ARRAY(Type,x,cap,n)  { L"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), L"" #Type, (void *) Type##_getText, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_LENUM_ARRAY
#define oo_LENUM_ARRAY(Type,x,cap,n)  { L"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), L"" #Type, (void *) Type##_getText, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_ENUM_SET
#define oo_ENUM_SET(Type,x,setType)  { L"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), L"" #Type, (void *) Type##_getText, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_LENUM_SET
#define oo_LENUM_SET(Type,x,setType)  { L"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), L"" #Type, (void *) Type##_getText, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_ENUM_VECTOR_FROM
#define oo_ENUM_VECTOR_FROM(Type,x,min,max)  { L"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), L"" #Type, (void *) Type##_getText, 1, L"" #min, L"" #max },
#undef oo_LENUM_VECTOR_FROM
#define oo_LENUM_VECTOR_FROM(Type,x,min,max)  { L"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), L"" #Type, (void *) Type##_getText, 1, L"" #min, L"" #max },
#undef oo_ENUM_VECTOR
#define oo_ENUM_VECTOR(Type,x,n)  { L"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), L"" #Type, (void *) Type##_getText, 1, (const wchar_t *) 0, L"" #n },
#undef oo_LENUM_VECTOR
#define oo_LENUM_VECTOR(Type,x,n)  { L"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), L"" #Type, (void *) Type##_getText, 1, (const wchar_t *) 0, L"" #n },
#undef oo_BOOLEAN
#define oo_BOOLEAN(x)  { L"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char) },
#undef oo_BOOLEAN_ARRAY
#define oo_BOOLEAN_ARRAY(x,cap,n)  { L"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_BOOLEAN_SET
#define oo_BOOLEAN_SET(x,setType)  { L"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_BOOLEAN_VECTOR_FROM
#define oo_BOOLEAN_VECTOR_FROM(x,min,max)  { L"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, (const wchar_t *) 1, L"" #min, L"" #max },
#undef oo_BOOLEAN_VECTOR
#define oo_BOOLEAN_VECTOR(x,n)  { L"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 1, (const wchar_t *) 0, L"" #n },
#undef oo_QUESTION
#define oo_QUESTION(x)  { L"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char) },
#undef oo_QUESTION_ARRAY
#define oo_QUESTION_ARRAY(x,cap,n)  { L"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_QUESTION_SET
#define oo_QUESTION_SET(x,setType)  { L"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_QUESTION_VECTOR_FROM
#define oo_QUESTION_VECTOR_FROM(x,min,max)  { L"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 1, L"" #min, L"" #max },
#undef oo_QUESTION_VECTOR
#define oo_QUESTION_VECTOR(x,n)  { L"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 1, (const wchar_t *) 0, L"" #n },

#undef oo_STRING
#define oo_STRING(x)  { L"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (wchar_t *) },
#undef oo_LSTRING
#define oo_LSTRING(x)  { L"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (wchar_t *) },

#undef oo_STRING_ARRAY
#define oo_STRING_ARRAY(x,cap,n)  { L"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (wchar_t *), 0, 0, - cap, (const wchar_t *) 0, L"" #n },
#undef oo_LSTRING_ARRAY
#define oo_LSTRING_ARRAY(x,cap,n)  { L"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (wchar_t *), 0, 0, - cap, (const wchar_t *) 0, L"" #n },

#undef oo_STRING_SET
#define oo_STRING_SET(x,setType)  { L"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (wchar_t *), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#undef oo_LSTRING_SET
#define oo_LSTRING_SET(x,setType)  { L"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (wchar_t *), 0, 0, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },

#undef oo_STRING_VECTOR_FROM
#define oo_STRING_VECTOR_FROM(x,min,max)  { L"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (wchar_t *), 0, 0, 1, L"" #min, L"" #max },
#undef oo_LSTRING_VECTOR_FROM
#define oo_LSTRING_VECTOR_FROM(x,min,max)  { L"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (wchar_t *), 0, 0, 1, L"" #min, L"" #max },

#undef oo_STRING_VECTOR
#define oo_STRING_VECTOR(x,n)  { L"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (wchar_t *), 0, 0, 1, (const wchar_t *) 0, L"" #n },
#undef oo_LSTRING_VECTOR
#define oo_LSTRING_VECTOR(x,n)  { L"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (wchar_t *), 0, 0, 1, (const wchar_t *) 0, L"" #n },

#define oo_STRUCT(Type,x)  { L"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), L"" #Type, & struct##Type :: s_description },
#define oo_STRUCT_ARRAY(Type,x,cap,n)  { L"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), L"" #Type, & struct##Type :: s_description, - cap, (const wchar_t *) 0, L"" #n },
#define oo_STRUCT_SET(Type,x,setType)  { L"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), L"" #Type, & struct##Type :: s_description, 3, (const wchar_t *) setType##_getText, (const wchar_t *) setType##_getValue },
#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  { L"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), L"" #Type, & struct##Type :: s_description, 1, L"" #min, L"" #max },
#undef oo_STRUCT_VECTOR
#define oo_STRUCT_VECTOR(Type,x,n)  { L"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), L"" #Type, & struct##Type :: s_description, 1, (const wchar_t *) 0, L"" #n },

#define oo_OBJECT(Type,version,x)  { L"" #x, objectwa, Melder_offsetof (ooSTRUCT, x), sizeof (Type), L"" #Type, & theClassInfo_##Type },
#define oo_COLLECTION(Type,x,ItemType,version)  { L"" #x, collectionwa, Melder_offsetof (ooSTRUCT, x), sizeof (class struct##ItemType), L"" #Type, & theClassInfo_##Type, 0, (const wchar_t *) & theClassInfo_##ItemType },
#define oo_FILE(x)
#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type) \
	static struct structData_Description the##Type##_description [] = {
#define oo_END_STRUCT(Type) \
		{ 0 } \
	}; \
	Data_Description struct##Type :: s_description = & the##Type##_description [0];

#define oo_DEFINE_CLASS(Class,Parent) \
	static struct structData_Description the##Class##_description [] = { \
		{ L"" #Class, inheritwa, 0, sizeof (class struct##Class), L"" #Class, & theClassInfo_##Parent },
#define oo_END_CLASS(Class) \
		{ 0 } \
	}; \
	Data_Description struct##Class :: s_description = & the##Class##_description [0];

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
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_READING_CACHE  0
#define oo_READING_LISP  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_WRITING_CACHE  0
#define oo_WRITING_LISP  0
#define oo_DESCRIBING  1

/* End of file oo_DESCRIPTION.h */
