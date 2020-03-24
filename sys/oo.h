#ifndef _oo_h_
#define _oo_h_
/* oo.h
 *
 * Copyright (C) 1994-2013,2015-2020 Paul Boersma
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

/*** Single types. ***/

/*
	The possible storage types give these binary formats:
		i8: store as signed big-endian integer in 8 bits (-128..+127).
		i16: store as signed big-endian integer in 16 bits (-32768..+32767).
		integer: store as signed big-endian integer in 32 bits (-2147483648..+2147483647). This will be changed to allow 64 bits.
		u8: store as unsigned big-endian integer in 8 bits (0..255).
		u16: store as unsigned big-endian integer in 16 bits (0..65535).
		u32: store as unsigned big-endian integer in 32 bits (0..4294967295).
		i8LE ... u32LE: store as little-endian integers.
		r32: store as 32-bits IEEE MSB-first floating point format.
		r64: store as 64-bits IEEE MSB-first floating point format.
		r80: store as 80-bits IEEE/Apple MSB-first floating point format.
		c64: store real and imaginary part as r32.
		c128: store real and imaginary part as r64.
	For text format, reading imposes the same restrictions for the integer types,
	and the real numbers are written with a precision of 8, 17 (or 16 or 15), or 20 characters.
*/

/* Single types. Declarations like: int x; */

#define oo_BYTE(x)  oo_SIMPLE (signed char, i8, x)
#define oo_INT(x)  oo_SIMPLE (int, i16, x)
#define oo_INTEGER(x)  oo_SIMPLE (integer, integer32BE, x)
#define oo_INTEGER16(x)  oo_SIMPLE (integer, integer16BE, x)
#define oo_UBYTE(x)  oo_SIMPLE (unsigned char, u8, x)
#define oo_UINT(x)  oo_SIMPLE (unsigned int, u16, x)
#define oo_UINTEGER(x)  oo_SIMPLE (uinteger, u32, x)
#define oo_INT8(x)  oo_SIMPLE (int8, i8, x)
#define oo_INT16(x)  oo_SIMPLE (int16, i16, x)
#define oo_INT32(x)  oo_SIMPLE (int32, i32, x)
#define oo_INT64(x)  oo_SIMPLE (int64, i64, x)
#define oo_UINT8(x)  oo_SIMPLE (uint8, u8, x)
#define oo_UINT16(x)  oo_SIMPLE (uint16, u16, x)
#define oo_UINT32(x)  oo_SIMPLE (uint32, u32, x)
#define oo_FLOAT(x)  oo_SIMPLE (double, r32, x)
#define oo_DOUBLE(x)  oo_SIMPLE (double, r64, x)
//#define oo_COMPLEX(x)  oo_SIMPLE (dcomplex, c128, x)

/* Sets with compile-time allocation of capacity. Declarations like: int x [1 + setType::MAX]; */
/* The first index is always 0, the last index is setType::MAX. */

//#define oo_BYTE_SET(x,setType)  oo_SET (signed char, i8, x, setType)
//#define oo_INT_SET(x,setType)  oo_SET (int, i16, x, setType)
//#define oo_INTEGER_SET(x,setType)  oo_SET (integer, integer32BE, x, setType)
//#define oo_UBYTE_SET(x,setType)  oo_SET (unsigned char, u8, x, setType)
//#define oo_UINT_SET(x,setType)  oo_SET (unsigned int, u16, x, setType)
//#define oo_UINTEGER_SET(x,setType)  oo_SET (uinteger, u32, x, setType)
//#define oo_FLOAT_SET(x,setType)  oo_SET (double, r32, x, setType)
#define oo_DOUBLE_SET(x,setType)  oo_SET (double, r64, x, setType)
//#define oo_COMPLEX_SET(x,setType)  oo_SET (dcomplex, c128, x, setType)

#define oo_VEC(x,size)  oo_ANYVEC (double, r64, x, size)
#define oo_obsoleteVEC32(x,size)  oo_ANYVEC (double, r32, x, size)
#define oo_INTVEC(x,size)  oo_ANYVEC (integer, integer32BE, x, size)
#define oo_INTVEC16(x,size)  oo_ANYVEC (integer, integer16BE, x, size)
#define oo_BOOLVEC(x,size)  oo_ANYVEC (bool, eb, x, size)
#define oo_COMPVEC(x,size) oo_ANYVEC (dcomplex, c128, x, size)
#define oo_BYTEVEC(x,size) oo_ANYVEC (byte, u8, x, size)

#define oo_MAT(x,nrow,ncol)  oo_ANYMAT (double, r64, x, nrow, ncol)
#define oo_INTMAT(x,nrow,ncol)  oo_ANYMAT (integer, integer32BE, x, nrow, ncol)
#define oo_obsoleteMAT32(x,nrow,ncol)  oo_ANYMAT (double, r32, x, nrow, ncol)
#define oo_BYTEMAT(x,nrow,ncol)  oo_ANYMAT (byte, u8, x, nrow, ncol)

#define oo_TEN3(x,ndi1,ndim2,ndim3)  oo_ANYTEN3 (double, r64, x, ndim1, ndim2, ndim3)

/*** Enumerated types. ***/

/* The possible storage types give these binary formats: */
/*    e8: store as signed integer in 8 bits. */
/*    e16: store in signed big-endian integer in 16 bits. */
/*    eb: store as byte 170 (false) or byte 185 (true). */
/*    eq: store as byte 150 (no) or byte 165 (yes). */
/*    ex: store as byte 226 (absent) or byte 241 (present). */
/* For text format, the value is written as a string between '<' and '>'. */

#define oo_ENUM(kType,x)  oo_ENUMx (kType, e8, x)
//#define oo_LENUM(Type,x)  oo_ENUMx (short, e16, Type, x)
//#define oo_ENUM_SET(Type,x,setType)  oo_ENUMx_SET (signed char, e8, Type, x, setType)
//#define oo_LENUM_SET(Type,x,setType)  oo_ENUMx_SET (short, e16, Type, x, setType)
#define oo_BOOLEAN(x)  oo_SIMPLE (bool, eb, x)
#define oo_QUESTION(x)  oo_SIMPLE (bool, eq, x)

/*** Strings. ***/

/* The possible storage types give these binary formats: */
/*    s16: store as sequence of bytes, preceded with 16 bits (u16) to denote length. */
/*    w16: store as sequence of characters (u16), preceded with 16 bits (u16) to denote length. */
/*    s32: store as sequence of bytes, preceded with 32 bits (u32) to denote length. */
/*    w32: store as sequence of characters (u16), preceded with 32 bits (u32) to denote length. */

#define oo_STRING(x)  oo_STRINGx (w16, x)
#define oo_LSTRING(x)  oo_STRINGx (w32, x)

//#define oo_STRING_SET(x,setType)  oo_STRINGx_SET (w16, x, setType)
//#define oo_LSTRING_SET(x,setType)  oo_STRINGx_SET (w32, x, setType)

#define oo_STRING_VECTOR(x,n)  oo_STRINGx_VECTOR (w16, x, n)
//#define oo_LSTRING_VECTOR(x,n)  oo_STRINGx_VECTOR (w32, x, n)

/********** Definitions for header files only. **********/
/* These are undef'ed and redefined in the header files that implement methods, */
/* such as oo_DESTROY.h, oo_COPY.h, oo_EQUAL.h, oo_WRITE_TEXT.h, etc. */

/* Types. */

#define oo_SIMPLE(type,storage,x)  type x;
#define oo_SET(type,storage,x,setType)  type x [1 + (int) setType::MAX];
#define oo_ANYVEC(type,storage,x,size)  autovector <type> x;
#define oo_ANYMAT(type,storage,x,nrow,ncol)  automatrix <type> x;
#define oo_ANYTEN3(type,storage,x,ndim1,ndim2,ndim3)  autotensor3 <type> x;

#define oo_ENUMx(kType,storage,x)  kType x;
//#define oo_ENUMx_SET(kType,storage,x,setType)  kType x [1 + (int) setType::MAX];

#define oo_STRINGx(storage,x)  autostring32 x;
#define oo_STRINGx_SET(storage,x,setType)  autostring32 x [1 + setType::MAX];
#define oo_STRINGx_VECTOR(storage,x,size)  autoSTRVEC x;

#define oo_STRUCT(Type,x)  struct struct##Type x;
#define oo_STRUCT_SET(Type,x,setType)  struct struct##Type x [1 + (int) setType::MAX];
#define oo_STRUCTVEC(Type,x,size)   autovector <struct##Type> x;

#define oo_OBJECT(Class,version,x)  auto##Class x;
#define oo_COLLECTION_OF(Class,x,ItemClass,version)  Class<struct##ItemClass> x;
#define oo_COLLECTION(Class,x,ItemClass,version)  auto##Class x;
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
		void readText (MelderReadText text, int formatVersion); \
		void writeBinary (FILE *f); \
		void readBinary (FILE *f, int formatVersion); \
	};

#define oo_DEFINE_CLASS(klas,parent) \
	typedef struct struct##klas *klas; \
	typedef autoSomeThing <struct##klas> auto##klas; \
	typedef struct##parent klas##_Parent; \
	extern struct structClassInfo theClassInfo_##klas; \
	extern ClassInfo class##klas; \
	struct struct##klas : public struct##parent {

#define oo_END_CLASS(Class) \
		void v_destroy () noexcept override; \
		void v_copy (Daata data_to) override; \
		bool v_equal (Daata otherData) override; \
		static Data_Description s_description; \
		Data_Description v_description () override { return s_description; } \
		bool v_canWriteAsEncoding (int outputEncoding) override; \
		void v_writeText (MelderFile openFile) override; \
		void v_readText (MelderReadText text, int formatVersion) override; \
		void v_writeBinary (FILE *f) override; \
		void v_readBinary (FILE *f, int formatVersion) override; \
	};

/*** Miscellaneous. ***/

/* For fields that should not be read in older versions: */
#define oo_FROM(from)
#define oo_ENDFROM

#define oo_VERSION_UNTIL(version)  \
	if (_formatVersion_ < version) {
#define oo_VERSION_ELSE_UNTIL(version)  \
	} else if (_formatVersion_ < version) {
#define oo_VERSION_ELSE  \
	} else {
#define oo_VERSION_END  \
	}

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
#define oo_COMPARING  0
#define oo_VALIDATING_ASCII  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo.h */
#endif
