#ifndef _oo_h_
#define _oo_h_
/* oo.h
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

/*** Single types. ***/

/* The possible storage types give these binary formats: */
/*    i1: store as signed big-endian integer in 1 byte (-128..127). */
/*    i2: store as signed big-endian integer in 2 bytes (-32768..32767). */
/*    i4: store as signed big-endian integer in 4 bytes. */
/*    u1: store as unsigned big-endian integer integer in 1 byte (0..255). */
/*    u2: store as unsigned big-endian integer in 2 bytes (0..65535). */
/*    u4: store as unsigned big-endian integer in 4 bytes. */
/*    i1LE ... u4LE: store as little-endian integers. */
/*    r4: store as 4-byte IEEE MSB-first floating point format. */
/*    r8: store as 8-byte IEEE MSB-first floating point format. */
/*    r10: store as 10-byte IEEE/Apple MSB-first floating point format. */
/*    c8: store real and imaginary part as r4. */
/*    c16: store real and imaginary part as r4. */
/* For text format, reading imposes the same restrictions for the integer types, */
/* and the real numbers are written with a precision of 8, 17, or 20 characters. */

/* Single types. Declarations like: int x; */

#define oo_BYTE(x)  oo_SIMPLE (signed char, i1, x)
#define oo_INT(x)  oo_SIMPLE (int, i2, x)
#define oo_LONG(x)  oo_SIMPLE (long, i4, x)
#define oo_UBYTE(x)  oo_SIMPLE (unsigned char, u1, x)
#define oo_UINT(x)  oo_SIMPLE (unsigned int, u2, x)
#define oo_ULONG(x)  oo_SIMPLE (unsigned long, u4, x)
#define oo_BOOL(x)  oo_SIMPLE (unsigned char, u1, x)
#define oo_FLOAT(x)  oo_SIMPLE (double, r4, x)
#define oo_DOUBLE(x)  oo_SIMPLE (double, r8, x)
#define oo_FCOMPLEX(x)  oo_SIMPLE (fcomplex, c8, x)
#define oo_DCOMPLEX(x)  oo_SIMPLE (dcomplex, c16, x)
#define oo_POINTER(x)  oo_SIMPLE (void *, dummy, x)

/* Arrays with compile-time allocation of capacity. Declarations like: int x [cap]; */
/* 'cap' is a compile-time expression evaluating to a positive integer (e.g., 3). */
/* First index is always 0, last index is 'n' - 1. */
/* Actual number of elements 'n' may vary during run-time and while structure exists, */
/* but must never be greater than 'cap'. */

#define oo_BYTE_ARRAY(x,cap,n)  oo_ARRAY (signed char, i1, x, cap, n)
#define oo_INT_ARRAY(x,cap,n)  oo_ARRAY (int, i2, x, cap, n)
#define oo_LONG_ARRAY(x,cap,n)  oo_ARRAY (long, i4, x, cap, n)
#define oo_UBYTE_ARRAY(x,cap,n)  oo_ARRAY (unsigned char, u1, x, cap, n)
#define oo_UINT_ARRAY(x,cap,n)  oo_ARRAY (unsigned int, u2, x, cap, n)
#define oo_ULONG_ARRAY(x,cap,n)  oo_ARRAY (unsigned long, u4, x, cap, n)
#define oo_BOOL_ARRAY(x,cap,n)  oo_ARRAY (unsigned char, u1, x, cap, n)
#define oo_FLOAT_ARRAY(x,cap,n)  oo_ARRAY (double, r4, x, cap, n)
#define oo_DOUBLE_ARRAY(x,cap,n)  oo_ARRAY (double, r8, x, cap, n)
#define oo_FCOMPLEX_ARRAY(x,cap,n)  oo_ARRAY (fcomplex, c8, x, cap, n)
#define oo_DCOMPLEX_ARRAY(x,cap,n)  oo_ARRAY (dcomplex, c16, x, cap, n)
#define oo_POINTER_ARRAY(x,cap,n)  oo_ARRAY (void *, dummy, x, cap, n)

/* Sets with compile-time allocation of capacity. Declarations like: int x [1 + setType_MAX]; */
/* First index is always 0, last index is setType_MAX. */

#define oo_BYTE_SET(x,setType)  oo_SET (signed char, i1, x, setType)
#define oo_INT_SET(x,setType)  oo_SET (int, i2, x, setType)
#define oo_LONG_SET(x,setType)  oo_SET (long, i4, x, setType)
#define oo_UBYTE_SET(x,setType)  oo_SET (unsigned char, u1, x, setType)
#define oo_UINT_SET(x,setType)  oo_SET (unsigned int, u2, x, setType)
#define oo_ULONG_SET(x,setType)  oo_SET (unsigned long, u4, x, setType)
#define oo_BOOL_SET(x,setType)  oo_SET (unsigned char, u1, x, setType)
#define oo_FLOAT_SET(x,setType)  oo_SET (double, r4, x, setType)
#define oo_DOUBLE_SET(x,setType)  oo_SET (double, r8, x, setType)
#define oo_FCOMPLEX_SET(x,setType)  oo_SET (fcomplex, c8, x, setType)
#define oo_DCOMPLEX_SET(x,setType)  oo_SET (dcomplex, c16, x, setType)
#define oo_POINTER_SET(x,setType)  oo_SET (void *, dummy, x, setType)

/* Arrays with run-time allocation of size. Declarations like: int *x; */
/* First index is 'min', last index is 'max'. */
/* While the structure exists, 'max' may become less than the value it had at the time of allocation. */

#define oo_BYTE_VECTOR_FROM(x,min,max)  oo_VECTOR (signed char, i1, x, min, max)
#define oo_INT_VECTOR_FROM(x,min,max)  oo_VECTOR (int, i2, x, min, max)
#define oo_LONG_VECTOR_FROM(x,min,max)  oo_VECTOR (long, i4, x, min, max)
#define oo_UBYTE_VECTOR_FROM(x,min,max)  oo_VECTOR (unsigned char, u1, x, min, max)
#define oo_UINT_VECTOR_FROM(x,min,max)  oo_VECTOR (unsigned int, u2, x, min, max)
#define oo_ULONG_VECTOR_FROM(x,min,max)  oo_VECTOR (unsigned long, u4, x, min, max)
#define oo_BOOL_VECTOR_FROM(x,min,max)  oo_VECTOR (unsigned char, u1, x, min, max)
#define oo_FLOAT_VECTOR_FROM(x,min,max)  oo_VECTOR (double, r4, x, min, max)
#define oo_DOUBLE_VECTOR_FROM(x,min,max)  oo_VECTOR (double, r8, x, min, max)
#define oo_FCOMPLEX_VECTOR_FROM(x,min,max)  oo_VECTOR (fcomplex, c8, x, min, max)
#define oo_DCOMPLEX_VECTOR_FROM(x,min,max)  oo_VECTOR (dcomplex, c16, x, min, max)
#define oo_POINTER_VECTOR_FROM(x,min,max)  oo_VECTOR (void *, dummy, x, min, max)

#define oo_BYTE_MATRIX_FROM(x,row1,row2,col1,col2)  oo_MATRIX (signed char, i1, x, row1, row2, col1, col2)
#define oo_INT_MATRIX_FROM(x,row1,row2,col1,col2)  oo_MATRIX (int, i2, x, row1, row2, col1, col2)
#define oo_LONG_MATRIX_FROM(x,row1,row2,col1,col2)  oo_MATRIX (long, i4, x, row1, row2, col1, col2)
#define oo_UBYTE_MATRIX_FROM(x,row1,row2,col1,col2)  oo_MATRIX (unsigned char, u1, x, row1, row2, col1, col2)
#define oo_UINT_MATRIX_FROM(x,row1,row2,col1,col2)  oo_MATRIX (unsigned int, u2, x, row1, row2, col1, col2)
#define oo_ULONG_MATRIX_FROM(x,row1,row2,col1,col2)  oo_MATRIX (unsigned long, u4, x, row1, row2, col1, col2)
#define oo_BOOL_MATRIX_FROM(x,row1,row2,col1,col2)  oo_MATRIX (unsigned char, u1, x, row1, row2, col1, col2)
#define oo_FLOAT_MATRIX_FROM(x,row1,row2,col1,col2)  oo_MATRIX (double, r4, x, row1, row2, col1, col2)
#define oo_DOUBLE_MATRIX_FROM(x,row1,row2,col1,col2)  oo_MATRIX (double, r8, x, row1, row2, col1, col2)
#define oo_FCOMPLEX_MATRIX_FROM(x,row1,row2,col1,col2)  oo_MATRIX (fcomplex, c8, x, row1, row2, col1, col2)
#define oo_DCOMPLEX_MATRIX_FROM(x,row1,row2,col1,col2)  oo_MATRIX (dcomplex, c16, x, row1, row2, col1, col2)
#define oo_POINTER_MATRIX_FROM(x,row1,row2,col1,col2)  oo_MATRIX (void *, dummy, x, row1, row2, col1, col2)

/* The same arrays, with the first index fixed at 1. */

#define oo_BYTE_VECTOR(x,n)  oo_VECTOR (signed char, i1, x, 1, n)
#define oo_INT_VECTOR(x,n)  oo_VECTOR (int, i2, x, 1, n)
#define oo_LONG_VECTOR(x,n)  oo_VECTOR (long, i4, x, 1, n)
#define oo_UBYTE_VECTOR(x,n)  oo_VECTOR (unsigned char, u1, x, 1, n)
#define oo_UINT_VECTOR(x,n)  oo_VECTOR (unsigned int, u2, x, 1, n)
#define oo_ULONG_VECTOR(x,n)  oo_VECTOR (unsigned long, u4, x, 1, n)
#define oo_BOOL_VECTOR(x,n)  oo_VECTOR (unsigned char, u1, x, 1, n)
#define oo_FLOAT_VECTOR(x,n)  oo_VECTOR (double, r4, x, 1, n)
#define oo_DOUBLE_VECTOR(x,n)  oo_VECTOR (double, r8, x, 1, n)
#define oo_FCOMPLEX_VECTOR(x,n)  oo_VECTOR (fcomplex, c8, x, 1, n)
#define oo_DCOMPLEX_VECTOR(x,n)  oo_VECTOR (dcomplex, c16, x, 1, n)
#define oo_POINTER_VECTOR(x,n)  oo_VECTOR (void *, dummy, x, 1, n)

#define oo_BYTE_MATRIX(x,nrow,ncol)  oo_MATRIX (signed char, i1, x, 1, nrow, 1, ncol)
#define oo_INT_MATRIX(x,nrow,ncol)  oo_MATRIX (int, i2, x, 1, nrow, 1, ncol)
#define oo_LONG_MATRIX(x,nrow,ncol)  oo_MATRIX (long, i4, x, 1, nrow, 1, ncol)
#define oo_UBYTE_MATRIX(x,nrow,ncol)  oo_MATRIX (unsigned char, u1, x, 1, nrow, 1, ncol)
#define oo_UINT_MATRIX(x,nrow,ncol)  oo_MATRIX (unsigned int, u2, x, 1, nrow, 1, ncol)
#define oo_ULONG_MATRIX(x,nrow,ncol)  oo_MATRIX (unsigned long, u4, x, 1, nrow, 1, ncol)
#define oo_BOOL_MATRIX(x,nrow,ncol)  oo_MATRIX (unsigned char, u1, x, 1, nrow, 1, ncol)
#define oo_FLOAT_MATRIX(x,nrow,ncol)  oo_MATRIX (double, r4, x, 1, nrow, 1, ncol)
#define oo_DOUBLE_MATRIX(x,nrow,ncol)  oo_MATRIX (double, r8, x, 1, nrow, 1, ncol)
#define oo_FCOMPLEX_MATRIX(x,nrow,ncol)  oo_MATRIX (fcomplex, c8, x, 1, nrow, 1, ncol)
#define oo_DCOMPLEX_MATRIX(x,nrow,ncol)  oo_MATRIX (dcomplex, c16, x, 1, nrow, 1, ncol)
#define oo_POINTER_MATRIX(x,nrow,ncol)  oo_MATRIX (void *, dummy, x, 1, nrow, 1, ncol)


/*** Enumerated types. ***/

/* The possible storage types give these binary formats: */
/*    e1: store as signed integer in 1 byte. */
/*    e2: store in signed big-endian integer in 2 bytes. */
/*    eb: store as byte 170 (false) or byte 185 (true). */
/*    eq: store as byte 150 (no) or byte 165 (yes). */
/*    ex: store as byte 226 (absent) or byte 241 (present). */
/* For text format, the value is written as a string between '<' and '>'. */

#define oo_ENUM(Type,x)  oo_ENUMx (signed char, e1, Type, x)
#define oo_LENUM(Type,x)  oo_ENUMx (short, e2, Type, x)
#define oo_ENUM_ARRAY(Type,x,cap,n)  oo_ENUMx_ARRAY (signed char, e1, Type, x, cap, n)
#define oo_LENUM_ARRAY(Type,x,cap,n)  oo_ENUMx_ARRAY (short, e2, Type, x, cap, n)
#define oo_ENUM_SET(Type,x,setType)  oo_ENUMx_SET (signed char, e1, Type, x, setType)
#define oo_LENUM_SET(Type,x,setType)  oo_ENUMx_SET (short, e2, Type, x, setType)
#define oo_ENUM_VECTOR_FROM(Type,x,min,max)  oo_ENUMx_VECTOR (signed char, e1, Type, x, min, max)
#define oo_LENUM_VECTOR_FROM(Type,x,min,max)  oo_ENUMx_VECTOR (short, e2, Type, x, min, max)
#define oo_ENUM_VECTOR(Type,x,n)  oo_ENUMx_VECTOR (signed char, e1, Type, x, 1, n)
#define oo_LENUM_VECTOR(Type,x,n)  oo_ENUMx_VECTOR (short, e2, Type, x, 1, n)
#define oo_BOOLEAN(x)  oo_SIMPLE (signed char, eb, x)
#define oo_BOOLEAN_ARRAY(x,cap,n)  oo_ARRAY (signed char, eb, x, cap, n)
#define oo_BOOLEAN_SET(x,setType)  oo_ARRAY (signed char, eb, x, setType)
#define oo_BOOLEAN_VECTOR_FROM(x,min,max)  oo_VECTOR (signed char, eb, x, min, max)
#define oo_BOOLEAN_VECTOR(x,n)  oo_VECTOR (signed char, eb, x, 1, n)
#define oo_QUESTION(x)  oo_SIMPLE (signed char, eq, x)
#define oo_QUESTION_ARRAY(x,cap,n)  oo_ARRAY (signed char, eq, x, cap, n)
#define oo_QUESTION_SET(x,setType)  oo_ARRAY (signed char, eq, x, setType)
#define oo_QUESTION_VECTOR_FROM(x,min,max)  oo_VECTOR (signed char, eq, x, min, max)
#define oo_QUESTION_VECTOR(x,n)  oo_VECTOR (signed char, eq, x, 1, n)

/*** Strings. ***/

/* The possible storage types give these binary formats: */
/*    s2: store as sequence of bytes, preceded with 2 bytes (u2) to denote length. */
/*    w2: store as sequence of characters (u2), preceded with 2 bytes (u2) to denote length. */
/*    s4: store as sequence of bytes, preceded with 4 bytes (u4) to denote length. */
/*    w4: store as sequence of characters (u2), preceded with 4 bytes (u4) to denote length. */

#define oo_STRING(x)  oo_STRINGx (w2, x)
#define oo_LSTRING(x)  oo_STRINGx (w4, x)

#define oo_STRING_ARRAY(x,cap,n)  oo_STRINGx_ARRAY (w2, x, cap, n)
#define oo_LSTRING_ARRAY(x,cap,n)  oo_STRINGx_ARRAY (w4, x, cap, n)

#define oo_STRING_SET(x,setType)  oo_STRINGx_SET (w2, x, setType)
#define oo_LSTRING_SET(x,setType)  oo_STRINGx_SET (w4, x, setType)

#define oo_STRING_VECTOR_FROM(x,min,max)  oo_STRINGx_VECTOR (w2, x, min, max)
#define oo_LSTRING_VECTOR_FROM(x,min,max)  oo_STRINGx_VECTOR (w4, x, min, max)

#define oo_STRING_VECTOR(x,n)  oo_STRINGx_VECTOR (w2, x, 1, n)
#define oo_LSTRING_VECTOR(x,n)  oo_STRINGx_VECTOR (w4, x, 1, n)

/*** Structs. ***/

#define oo_STRUCT_VECTOR(Type,x,n)  oo_STRUCT_VECTOR_FROM (Type, x, 1, n)
#define oo_STRUCT_MATRIX(Type,x,nrow,ncol)  oo_STRUCT_MATRIX_FROM (Type, x, 1, nrow, 1, ncol)

/*** Class declaration in header file. ***/

#define oo_CLASS_CREATE(klas,parent) \
	typedef _Thing_auto <struct##klas> auto##klas; \
	extern struct structClassInfo theClassInfo_##klas; \
	extern ClassInfo class##klas



/********** Definitions for header files only. **********/
/* These are undef'ed and redefined in the header files that implement methods, */
/* like oo_DESTROY.h, oo_COPY.h, oo_EQUAL.h, oo_WRITE_TEXT.h, etc. */

/* Types. */

#define oo_SIMPLE(type,storage,x)  type x;
#define oo_ARRAY(type,storage,x,cap,n)  type x [cap];
#define oo_SET(type,storage,x,setType)  type x [1 + setType##_MAX];
#define oo_VECTOR(type,storage,x,min,max)  type *x;
#define oo_MATRIX(type,storage,x,row1,row2,col1,col2)  type **x;

#define oo_ENUMx(type,storage,Type,x)  type x;
#define oo_ENUMx_ARRAY(type,storage,Type,x,cap,n)  type x [cap];
#define oo_ENUMx_SET(type,storage,Type,x,setType)  type x [1 + setType##_MAX];
#define oo_ENUMx_VECTOR(type,storage,Type,x,min,max)  type *x;

#define oo_STRINGx(storage,x)  wchar_t *x;
#define oo_STRINGx_ARRAY(storage,x,cap,n)  wchar_t *x [cap];
#define oo_STRINGx_SET(storage,x,setType)  wchar_t *x [1 + setType##_MAX];
#define oo_STRINGx_VECTOR(storage,x,min,max)  wchar_t **x;

#define oo_STRUCT(Type,x)  struct struct##Type x;
#define oo_STRUCT_ARRAY(Type,x,cap,n)  struct struct##Type x [cap];
#define oo_STRUCT_SET(Type,x,setType)  struct struct##Type x [1 + setType##_MAX];
#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  Type x;
#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  struct struct##Type **x;

#define oo_OBJECT(Class,version,x)  Class x;
#define oo_COLLECTION(Class,x,ItemClass,version)  Class x;
#define oo_FILE(x)  structMelderFile x;
#define oo_DIR(x)  structMelderDir x;

/* Struct and class definitions. */

#define oo_DEFINE_STRUCT(T) \
	typedef struct struct##T *T; \
	struct struct##T {
#define oo_END_STRUCT(T) \
		void destroy (); \
		void copy (T data_to); \
		bool equal (T otherData); \
		Data_Description description (); \
		static Data_Description s_description; \
		bool canWriteAsEncoding (int outputEncoding); \
		void writeText (MelderFile openFile); \
		void readText (MelderReadText text); \
		void writeBinary (FILE *f); \
		void readBinary (FILE *f); \
	};

#define oo_DEFINE_CLASS(klas,parent) \
	typedef class struct##klas *klas; \
	typedef struct##parent klas##_Parent; \
	class struct##klas : public struct##parent { \
		public:
#define oo_END_CLASS(Class) \
		virtual void v_destroy (); \
		virtual void v_copy (Any data_to); \
		virtual bool v_equal (Any otherData); \
		static Data_Description s_description; \
		virtual Data_Description v_description () { return s_description; } \
		virtual bool v_canWriteAsEncoding (int outputEncoding); \
		virtual void v_writeText (MelderFile openFile); \
		virtual void v_readText (MelderReadText text); \
		virtual void v_writeBinary (FILE *f); \
		virtual void v_readBinary (FILE *f); \
	};

/*** Miscellaneous. ***/

/* For fields that should only be destroyed, copied, compared, read, or written if 'condition': */
#define oo_IF(condition)
#define oo_ENDIF

/* For fields that should not be read in older versions: */
#define oo_FROM(from)
#define oo_ENDFROM

/* For OBJECT fields that have version numbers: */
#define oo_VERSION(version)

/* Examples of the usage of the following macros:
	For code that should only appear in reading methods (consistency checks etc.),
		use:  #if oo_READING
	For code that should only appear in writing methods (e.g., debugging statements),
		use:  #if oo_WRITING
	For fields that are not destroyed, copied, or compared, either (i.e., only declarations),
		use:  #if oo_DECLARING
	For fields that do not take part in I/O (e.g. redundant or temporary fields),
		use:  #if ! oo_READING && ! oo_WRITING
	For fields that should not be read (e.g., when expanding a database file),
		use:  #if ! oo_READING
	For fields that should not be written (e.g., when shrinking a database file),
		use:  #if ! oo_WRITING
	The last two should be temporary situations,
		because the output files do not match the input files!
*/
#define oo_DECLARING  1
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
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_WRITING_CACHE  0
#define oo_WRITING_LISP  0
#define oo_DESCRIBING  0

/* End of file oo.h */
#endif
