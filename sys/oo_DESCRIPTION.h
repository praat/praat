/* oo_DESCRIPTION.h
 *
 * Copyright (C) 1994-2012,2013,2015,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "oo_undef.h"

#undef oo_BYTE
#define oo_BYTE(x)  { U"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_INT16
#define oo_INT16(x)  { U"" #x, int16wa, Melder_offsetof (ooSTRUCT, x), sizeof (int16), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_INT
#define oo_INT(x)  { U"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_INT32
#define oo_INT32(x)  { U"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int32), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_LONG
#define oo_LONG(x)  { U"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_UBYTE
#define oo_UBYTE(x)  { U"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_UINT
#define oo_UINT(x)  { U"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_ULONG
#define oo_ULONG(x)  { U"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_BOOL
#define oo_BOOL(x)  { U"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_FLOAT
#define oo_FLOAT(x)  { U"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_DOUBLE
#define oo_DOUBLE(x)  { U"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_FCOMPLEX
#define oo_FCOMPLEX(x)  { U"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_DCOMPLEX
#define oo_DCOMPLEX(x)  { U"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, 0, nullptr, nullptr, nullptr, nullptr },

#undef oo_BYTE_ARRAY
#define oo_BYTE_ARRAY(x,cap,n)  { U"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, - cap, nullptr, U"" #n, nullptr, nullptr },
#undef oo_INT_ARRAY
#define oo_INT_ARRAY(x,cap,n)  { U"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, - cap, nullptr, U"" #n, nullptr, nullptr },
#undef oo_LONG_ARRAY
#define oo_LONG_ARRAY(x,cap,n)  { U"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, - cap, nullptr, U"" #n, nullptr, nullptr },
#undef oo_UBYTE_ARRAY
#define oo_UBYTE_ARRAY(x,cap,n)  { U"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, - cap, nullptr, U"" #n, nullptr, nullptr },
#undef oo_UINT_ARRAY
#define oo_UINT_ARRAY(x,cap,n)  { U"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, - cap, nullptr, U"" #n, nullptr, nullptr },
#undef oo_ULONG_ARRAY
#define oo_ULONG_ARRAY(x,cap,n)  { U"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, - cap, nullptr, U"" #n, nullptr, nullptr },
#undef oo_BOOL_ARRAY
#define oo_BOOL_ARRAY(x,cap,n)  { U"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, - cap, nullptr, U"" #n, nullptr, nullptr },
#undef oo_FLOAT_ARRAY
#define oo_FLOAT_ARRAY(x,cap,n)  { U"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, - cap, nullptr, U"" #n, nullptr, nullptr },
#undef oo_DOUBLE_ARRAY
#define oo_DOUBLE_ARRAY(x,cap,n)  { U"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, - cap, nullptr, U"" #n, nullptr, nullptr },
#undef oo_FCOMPLEX_ARRAY
#define oo_FCOMPLEX_ARRAY(x,cap,n)  { U"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, - cap, nullptr, U"" #n, nullptr, nullptr },
#undef oo_DCOMPLEX_ARRAY
#define oo_DCOMPLEX_ARRAY(x,cap,n)  { U"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, - cap, nullptr, U"" #n, nullptr, nullptr },

#undef oo_BYTE_SET
#define oo_BYTE_SET(x,setType)  { U"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr }, /* BUG function pointer to pointer */
#undef oo_INT_SET
#define oo_INT_SET(x,setType)  { U"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_LONG_SET
#define oo_LONG_SET(x,setType)  { U"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_UBYTE_SET
#define oo_UBYTE_SET(x,setType)  { U"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_UINT_SET
#define oo_UINT_SET(x,setType)  { U"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_ULONG_SET
#define oo_ULONG_SET(x,setType)  { U"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_BOOL_SET
#define oo_BOOL_SET(x,setType)  { U"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_FLOAT_SET
#define oo_FLOAT_SET(x,setType)  { U"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_DOUBLE_SET
#define oo_DOUBLE_SET(x,setType)  { U"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_FCOMPLEX_SET
#define oo_FCOMPLEX_SET(x,setType)  { U"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_DCOMPLEX_SET
#define oo_DCOMPLEX_SET(x,setType)  { U"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },

#undef oo_BYTE_VECTOR_FROM
#define oo_BYTE_VECTOR_FROM(x,min,max)  { U"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_INT_VECTOR_FROM
#define oo_INT_VECTOR_FROM(x,min,max)  { U"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_LONG_VECTOR_FROM
#define oo_LONG_VECTOR_FROM(x,min,max)  { U"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_UBYTE_VECTOR_FROM
#define oo_UBYTE_VECTOR_FROM(x,min,max)  { U"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_UINT_VECTOR_FROM
#define oo_UINT_VECTOR_FROM(x,min,max)  { U"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_ULONG_VECTOR_FROM
#define oo_ULONG_VECTOR_FROM(x,min,max)  { U"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_BOOL_VECTOR_FROM
#define oo_BOOL_VECTOR_FROM(x,min,max)  { U"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_FLOAT_VECTOR_FROM
#define oo_FLOAT_VECTOR_FROM(x,min,max)  { U"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_DOUBLE_VECTOR_FROM
#define oo_DOUBLE_VECTOR_FROM(x,min,max)  { U"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_FCOMPLEX_VECTOR_FROM
#define oo_FCOMPLEX_VECTOR_FROM(x,min,max)  { U"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_DCOMPLEX_VECTOR_FROM
#define oo_DCOMPLEX_VECTOR_FROM(x,min,max)  { U"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },

#undef oo_BYTE_MATRIX_FROM
#define oo_BYTE_MATRIX_FROM(x,r1,r2,c1,c2)  { U"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 2, U"" #r1, U"" #r2, U"" #c1, U"" #c2 },
#undef oo_INT_MATRIX_FROM
#define oo_INT_MATRIX_FROM(x,r1,r2,c1,c2)  { U"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, 2, U"" #r1, U"" #r2, U"" #c1, U"" #c2 },
#undef oo_LONG_MATRIX_FROM
#define oo_LONG_MATRIX_FROM(x,r1,r2,c1,c2)  { U"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, 2, U"" #r1, U"" #r2, U"" #c1, U"" #c2 },
#undef oo_UBYTE_MATRIX_FROM
#define oo_UBYTE_MATRIX_FROM(x,r1,r2,c1,c2)  { U"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, 2, U"" #r1, U"" #r2, U"" #c1, U"" #c2 },
#undef oo_UINT_MATRIX_FROM
#define oo_UINT_MATRIX_FROM(x,r1,r2,c1,c2)  { U"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, 2, U"" #r1, U"" #r2, U"" #c1, U"" #c2 },
#undef oo_ULONG_MATRIX_FROM
#define oo_ULONG_MATRIX_FROM(x,r1,r2,c1,c2)  { U"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, 2, U"" #r1, U"" #r2, U"" #c1, U"" #c2 },
#undef oo_BOOL_MATRIX_FROM
#define oo_BOOL_MATRIX_FROM(x,r1,r2,c1,c2)  { U"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 2, U"" #r1, U"" #r2, U"" #c1, U"" #c2 },
#undef oo_FLOAT_MATRIX_FROM
#define oo_FLOAT_MATRIX_FROM(x,r1,r2,c1,c2)  { U"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 2, U"" #r1, U"" #r2, U"" #c1, U"" #c2 },
#undef oo_DOUBLE_MATRIX_FROM
#define oo_DOUBLE_MATRIX_FROM(x,r1,r2,c1,c2)  { U"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 2, U"" #r1, U"" #r2, U"" #c1, U"" #c2 },
#undef oo_FCOMPLEX_MATRIX_FROM
#define oo_FCOMPLEX_MATRIX_FROM(x,r1,r2,c1,c2)  { U"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, 2, U"" #r1, U"" #r2, U"" #c1, U"" #c2 },
#undef oo_DCOMPLEX_MATRIX_FROM
#define oo_DCOMPLEX_MATRIX_FROM(x,r1,r2,c1,c2)  { U"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, 2, U"" #r1, U"" #r2, U"" #c1, U"" #c2 },

#undef oo_BYTE_VECTOR
#define oo_BYTE_VECTOR(x,n)  { U"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_INT_VECTOR
#define oo_INT_VECTOR(x,n)  { U"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_LONG_VECTOR
#define oo_LONG_VECTOR(x,n)  { U"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_UBYTE_VECTOR
#define oo_UBYTE_VECTOR(x,n)  { U"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_UINT_VECTOR
#define oo_UINT_VECTOR(x,n)  { U"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_ULONG_VECTOR
#define oo_ULONG_VECTOR(x,n)  { U"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_BOOL_VECTOR
#define oo_BOOL_VECTOR(x,n)  { U"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_FLOAT_VECTOR
#define oo_FLOAT_VECTOR(x,n)  { U"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_DOUBLE_VECTOR
#define oo_DOUBLE_VECTOR(x,n)  { U"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_FCOMPLEX_VECTOR
#define oo_FCOMPLEX_VECTOR(x,n)  { U"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_DCOMPLEX_VECTOR
#define oo_DCOMPLEX_VECTOR(x,n)  { U"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, 1, nullptr, U"" #n, nullptr, nullptr },

#undef oo_BYTE_MATRIX
#define oo_BYTE_MATRIX(x,nrow,ncol)  { U"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), 0, 0, 2, nullptr, U"" #nrow, (const char32 *) 0, U"" #ncol },
#undef oo_INT_MATRIX
#define oo_INT_MATRIX(x,nrow,ncol)  { U"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), 0, 0, 2, nullptr, U"" #nrow, (const char32 *) 0, U"" #ncol },
#undef oo_LONG_MATRIX
#define oo_LONG_MATRIX(x,nrow,ncol)  { U"" #x, longwa, Melder_offsetof (ooSTRUCT, x), sizeof (long), 0, 0, 2, nullptr, U"" #nrow, (const char32 *) 0, U"" #ncol },
#undef oo_UBYTE_MATRIX
#define oo_UBYTE_MATRIX(x,nrow,ncol)  { U"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), 0, 0, 2, nullptr, U"" #nrow, (const char32 *) 0, U"" #ncol },
#undef oo_UINT_MATRIX
#define oo_UINT_MATRIX(x,nrow,ncol)  { U"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), 0, 0, 2, nullptr, U"" #nrow, (const char32 *) 0, U"" #ncol },
#undef oo_ULONG_MATRIX
#define oo_ULONG_MATRIX(x,nrow,ncol)  { U"" #x, ulongwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned long), 0, 0, 2, nullptr, U"" #nrow, (const char32 *) 0, U"" #ncol },
#undef oo_BOOL_MATRIX
#define oo_BOOL_MATRIX(x,nrow,ncol)  { U"" #x, boolwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 2, nullptr, U"" #nrow, (const char32 *) 0, U"" #ncol },
#undef oo_FLOAT_MATRIX
#define oo_FLOAT_MATRIX(x,nrow,ncol)  { U"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 2, nullptr, U"" #nrow, (const char32 *) 0, U"" #ncol },
#undef oo_DOUBLE_MATRIX
#define oo_DOUBLE_MATRIX(x,nrow,ncol)  { U"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), 0, 0, 2, nullptr, U"" #nrow, (const char32 *) 0, U"" #ncol },
#undef oo_FCOMPLEX_MATRIX
#define oo_FCOMPLEX_MATRIX(x,nrow,ncol)  { U"" #x, fcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (fcomplex), 0, 0, 2, nullptr, U"" #nrow, (const char32 *) 0, U"" #ncol },
#undef oo_DCOMPLEX_MATRIX
#define oo_DCOMPLEX_MATRIX(x,nrow,ncol)  { U"" #x, dcomplexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), 0, 0, 2, nullptr, U"" #nrow, (const char32 *) 0, U"" #ncol },

#undef oo_ENUM
#define oo_ENUM(Type,x)  { U"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), U"" #Type, (void *) Type##_getText, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_LENUM
#define oo_LENUM(Type,x)  { U"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), U"" #Type, (void *) Type##_getText, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_ENUM_ARRAY
#define oo_ENUM_ARRAY(Type,x,cap,n)  { U"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), U"" #Type, (void *) Type##_getText, - cap, (const char32 *) 0, U"" #n, nullptr, nullptr },
#undef oo_LENUM_ARRAY
#define oo_LENUM_ARRAY(Type,x,cap,n)  { U"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), U"" #Type, (void *) Type##_getText, - cap, (const char32 *) 0, U"" #n, nullptr, nullptr },
#undef oo_ENUM_SET
#define oo_ENUM_SET(Type,x,setType)  { U"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), U"" #Type, (void *) Type##_getText, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_LENUM_SET
#define oo_LENUM_SET(Type,x,setType)  { U"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), U"" #Type, (void *) Type##_getText, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_ENUM_VECTOR_FROM
#define oo_ENUM_VECTOR_FROM(Type,x,min,max)  { U"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), U"" #Type, (void *) Type##_getText, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_LENUM_VECTOR_FROM
#define oo_LENUM_VECTOR_FROM(Type,x,min,max)  { U"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), U"" #Type, (void *) Type##_getText, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_ENUM_VECTOR
#define oo_ENUM_VECTOR(Type,x,n)  { U"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), U"" #Type, (void *) Type##_getText, 1, (const char32 *) 0, U"" #n, nullptr, nullptr },
#undef oo_LENUM_VECTOR
#define oo_LENUM_VECTOR(Type,x,n)  { U"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), U"" #Type, (void *) Type##_getText, 1, (const char32 *) 0, U"" #n, nullptr, nullptr },
#undef oo_BOOLEAN
#define oo_BOOLEAN(x)  { U"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_BOOLEAN_ARRAY
#define oo_BOOLEAN_ARRAY(x,cap,n)  { U"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, - cap, (const char32 *) 0, U"" #n, nullptr, nullptr },
#undef oo_BOOLEAN_SET
#define oo_BOOLEAN_SET(x,setType)  { U"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_BOOLEAN_VECTOR_FROM
#define oo_BOOLEAN_VECTOR_FROM(x,min,max)  { U"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, (const char32 *) 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_BOOLEAN_VECTOR
#define oo_BOOLEAN_VECTOR(x,n)  { U"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 1, (const char32 *) 0, U"" #n, nullptr, nullptr },
#undef oo_QUESTION
#define oo_QUESTION(x)  { U"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_QUESTION_ARRAY
#define oo_QUESTION_ARRAY(x,cap,n)  { U"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, - cap, (const char32 *) 0, U"" #n, nullptr, nullptr },
#undef oo_QUESTION_SET
#define oo_QUESTION_SET(x,setType)  { U"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_QUESTION_VECTOR_FROM
#define oo_QUESTION_VECTOR_FROM(x,min,max)  { U"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_QUESTION_VECTOR
#define oo_QUESTION_VECTOR(x,n)  { U"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), 0, 0, 1, (const char32 *) 0, U"" #n, nullptr, nullptr },

#undef oo_STRING
#define oo_STRING(x)  { U"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_LSTRING
#define oo_LSTRING(x)  { U"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },

#undef oo_STRING_ARRAY
#define oo_STRING_ARRAY(x,cap,n)  { U"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), 0, 0, - cap, (const char32 *) 0, U"" #n, nullptr, nullptr },
#undef oo_LSTRING_ARRAY
#define oo_LSTRING_ARRAY(x,cap,n)  { U"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), 0, 0, - cap, (const char32 *) 0, U"" #n, nullptr, nullptr },

#undef oo_STRING_SET
#define oo_STRING_SET(x,setType)  { U"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#undef oo_LSTRING_SET
#define oo_LSTRING_SET(x,setType)  { U"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), 0, 0, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },

#undef oo_STRING_VECTOR_FROM
#define oo_STRING_VECTOR_FROM(x,min,max)  { U"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_LSTRING_VECTOR_FROM
#define oo_LSTRING_VECTOR_FROM(x,min,max)  { U"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), 0, 0, 1, U"" #min, U"" #max, nullptr, nullptr },

#undef oo_STRING_VECTOR
#define oo_STRING_VECTOR(x,n)  { U"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), 0, 0, 1, (const char32 *) 0, U"" #n, nullptr, nullptr },
#undef oo_LSTRING_VECTOR
#define oo_LSTRING_VECTOR(x,n)  { U"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), 0, 0, 1, (const char32 *) 0, U"" #n, nullptr, nullptr },

#define oo_STRUCT(Type,x)  { U"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), U"" #Type, & struct##Type :: s_description, 0, nullptr, nullptr, nullptr, nullptr },
#define oo_STRUCT_ARRAY(Type,x,cap,n)  { U"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), U"" #Type, & struct##Type :: s_description, - cap, (const char32 *) 0, U"" #n, nullptr, nullptr },
#define oo_STRUCT_SET(Type,x,setType)  { U"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), U"" #Type, & struct##Type :: s_description, 3, (const char32 *) setType##_getText, (const char32 *) setType##_getValue, nullptr, nullptr },
#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  { U"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), U"" #Type, & struct##Type :: s_description, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_STRUCT_VECTOR
#define oo_STRUCT_VECTOR(Type,x,n)  { U"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), U"" #Type, & struct##Type :: s_description, 1, (const char32 *) 0, U"" #n, nullptr, nullptr },
#define oo_STRUCT_MATRIX_FROM(Type,x,r1,r2,c1,c2)  { U"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), U"" #Type, & struct##Type :: s_description, 2, U"" #r1, U"" #r2, U"" #c1, U"" #c2 },
#undef oo_STRUCT_MATRIX
#define oo_STRUCT_MATRIX(Type,x,nrow,ncol)  { U"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), U"" #Type, & struct##Type :: s_description, 2, nullptr, U"" #nrow, nullptr, U"" #ncol },

#define oo_AUTO_OBJECT(Type,version,x)  { U"" #x, autoobjectwa, Melder_offsetof (ooSTRUCT, x), sizeof (Type), U"" #Type, & theClassInfo_##Type, 0, nullptr, nullptr, nullptr, nullptr },
#define oo_COLLECTION_OF(Type,x,ItemType,version)  { U"" #x, collectionofwa, Melder_offsetof (ooSTRUCT, x), sizeof (class struct##ItemType), U"" #Type, & theClassInfo_Collection, 0, (const char32 *) & theClassInfo_##ItemType, nullptr, nullptr, nullptr },
#define oo_AUTO_COLLECTION(Type,x,ItemType,version)  { U"" #x, autocollectionwa, Melder_offsetof (ooSTRUCT, x), sizeof (class struct##ItemType), U"" #Type, & theClassInfo_##Type, 0, (const char32 *) & theClassInfo_##ItemType, nullptr, nullptr, nullptr },
#define oo_FILE(x)
#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type) \
	static struct structData_Description the##Type##_description [] = {
#define oo_END_STRUCT(Type) \
		{ } \
	}; \
	Data_Description struct##Type :: s_description = & the##Type##_description [0];

#define oo_DEFINE_CLASS(Class,Parent) \
	static struct structData_Description the##Class##_description [] = { \
		{ U"" #Class, inheritwa, 0, sizeof (class struct##Class), U"" #Class, & theClassInfo_##Parent, 0, nullptr, nullptr, nullptr, nullptr },
#define oo_END_CLASS(Class) \
		{ } \
	}; \
	Data_Description struct##Class :: s_description = & the##Class##_description [0];

#define oo_IF(condition)
#define oo_ENDIF

#define oo_FROM(from)
#define oo_ENDFROM

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  1

/* End of file oo_DESCRIPTION.h */
