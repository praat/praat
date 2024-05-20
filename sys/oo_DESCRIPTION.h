/* oo_DESCRIPTION.h
 *
 * Copyright (C) 1994-2013,2015-2020,2023,2024 Paul Boersma
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
#define oo_BYTE(x)  { U"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_INT16
#define oo_INT16(x)  { U"" #x, int16wa, Melder_offsetof (ooSTRUCT, x), sizeof (int16), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_INT
#define oo_INT(x)  { U"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_INT32
#define oo_INT32(x)  { U"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int32), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_INTEGER
#define oo_INTEGER(x)  { U"" #x, integerwa, Melder_offsetof (ooSTRUCT, x), sizeof (integer), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_INTEGER16
#define oo_INTEGER16(x)  { U"" #x, integerwa, Melder_offsetof (ooSTRUCT, x), sizeof (integer), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_UBYTE
#define oo_UBYTE(x)  { U"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_UINT
#define oo_UINT(x)  { U"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_UINTEGER
#define oo_UINTEGER(x)  { U"" #x, uintegerwa, Melder_offsetof (ooSTRUCT, x), sizeof (uinteger), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_FLOAT
#define oo_FLOAT(x)  { U"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_DOUBLE
#define oo_DOUBLE(x)  { U"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_COMPLEX
#define oo_COMPLEX(x)  { U"" #x, complexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },

#undef oo_BYTE_SET
#define oo_BYTE_SET(x,setType)  { U"" #x, bytewa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr }, /* BUG function pointer to pointer */
#undef oo_INT_SET
#define oo_INT_SET(x,setType)  { U"" #x, intwa, Melder_offsetof (ooSTRUCT, x), sizeof (int), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#undef oo_INTEGER_SET
#define oo_INTEGER_SET(x,setType)  { U"" #x, integerwa, Melder_offsetof (ooSTRUCT, x), sizeof (integer), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#undef oo_UBYTE_SET
#define oo_UBYTE_SET(x,setType)  { U"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned char), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#undef oo_UINT_SET
#define oo_UINT_SET(x,setType)  { U"" #x, uintwa, Melder_offsetof (ooSTRUCT, x), sizeof (unsigned int), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#undef oo_UINTEGER_SET
#define oo_UINTEGER_SET(x,setType)  { U"" #x, uintegerwa, Melder_offsetof (ooSTRUCT, x), sizeof (uinteger), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#undef oo_FLOAT_SET
#define oo_FLOAT_SET(x,setType)  { U"" #x, floatwa, Melder_offsetof (ooSTRUCT, x), sizeof (double), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#undef oo_DOUBLE_SET
#define oo_DOUBLE_SET(x,setType)  { U"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#undef oo_COMPLEX_SET
#define oo_COMPLEX_SET(x,setType)  { U"" #x, complexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },

#undef oo_VEC
#define oo_VEC(x,n)  { U"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), nullptr, nullptr, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_INTVEC
#define oo_INTVEC(x,n)  { U"" #x, integerwa, Melder_offsetof (ooSTRUCT, x), sizeof (integer), nullptr, nullptr, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_INTVEC16
#define oo_INTVEC16(x,n)  { U"" #x, integerwa, Melder_offsetof (ooSTRUCT, x), sizeof (integer), nullptr, nullptr, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_BOOLVEC
#define oo_BOOLVEC(x,n)  { U"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), nullptr, nullptr, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_COMPVEC
#define oo_COMPVEC(x,n)  { U"" #x, complexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), nullptr, nullptr, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_BYTEVEC
#define oo_BYTEVEC(x,n)  { U"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (byte), nullptr, nullptr, 1, nullptr, U"" #n, nullptr, nullptr },

#undef oo_MAT
#define oo_MAT(x,nrow,ncol)  { U"" #x, doublewa, Melder_offsetof (ooSTRUCT, x), sizeof (double), nullptr, nullptr, 2, nullptr, U"" #nrow, nullptr, U"" #ncol },
#undef oo_INTMAT
#define oo_INTMAT(x,nrow,ncol)  { U"" #x, integerwa, Melder_offsetof (ooSTRUCT, x), sizeof (integer), nullptr, nullptr, 2, nullptr, U"" #nrow, nullptr, U"" #ncol },
#undef oo_BOOLMAT
#define oo_BOOLMAT(x,nrow,ncol)  { U"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), nullptr, nullptr, 2, nullptr, U"" #nrow, nullptr, U"" #ncol },
#undef oo_COMPMAT
#define oo_COMPMAT(x,nrow,ncol)  { U"" #x, complexwa, Melder_offsetof (ooSTRUCT, x), sizeof (dcomplex), nullptr, nullptr, 2, nullptr, U"" #nrow, nullptr, U"" #ncol },
#undef oo_BYTEMAT
#define oo_BYTEMAT(x,nrow,ncol)  { U"" #x, ubytewa, Melder_offsetof (ooSTRUCT, x), sizeof (byte), nullptr, nullptr, 2, nullptr, U"" #nrow, nullptr, U"" #ncol },

#undef oo_ENUM
#define oo_ENUM(Type,x)  { U"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), U"" #Type, (void *) Type##_getText, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_LENUM
#define oo_LENUM(Type,x)  { U"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), U"" #Type, (void *) Type##_getText, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_ENUM_SET
#define oo_ENUM_SET(Type,x,setType)  { U"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), U"" #Type, (void *) Type##_getText, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#undef oo_LENUM_SET
#define oo_LENUM_SET(Type,x,setType)  { U"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), U"" #Type, (void *) Type##_getText, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#undef oo_ENUM_VECTOR_FROM
#define oo_ENUM_VECTOR_FROM(Type,x,min,max)  { U"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), U"" #Type, (void *) Type##_getText, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_LENUM_VECTOR_FROM
#define oo_LENUM_VECTOR_FROM(Type,x,min,max)  { U"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), U"" #Type, (void *) Type##_getText, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_ENUM_VECTOR
#define oo_ENUM_VECTOR(Type,x,n)  { U"" #x, enumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed char), U"" #Type, (void *) Type##_getText, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_LENUM_VECTOR
#define oo_LENUM_VECTOR(Type,x,n)  { U"" #x, lenumwa, Melder_offsetof (ooSTRUCT, x), sizeof (signed short), U"" #Type, (void *) Type##_getText, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_BOOLEAN
#define oo_BOOLEAN(x)  { U"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_BOOLEAN_SET
#define oo_BOOLEAN_SET(x,setType)  { U"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#undef oo_BOOLEAN_VECTOR_FROM
#define oo_BOOLEAN_VECTOR_FROM(x,min,max)  { U"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), nullptr, nullptr, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_BOOLEAN_VECTOR
#define oo_BOOLEAN_VECTOR(x,n)  { U"" #x, booleanwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), nullptr, nullptr, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_QUESTION
#define oo_QUESTION(x)  { U"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_QUESTION_SET
#define oo_QUESTION_SET(x,setType)  { U"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#undef oo_QUESTION_VECTOR_FROM
#define oo_QUESTION_VECTOR_FROM(x,min,max)  { U"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), nullptr, nullptr, 1, U"" #min, U"" #max, nullptr, nullptr },
#undef oo_QUESTION_VECTOR
#define oo_QUESTION_VECTOR(x,n)  { U"" #x, questionwa, Melder_offsetof (ooSTRUCT, x), sizeof (bool), nullptr, nullptr, 1, nullptr, U"" #n, nullptr, nullptr },

#undef oo_STRING
#define oo_STRING(x)  { U"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
#undef oo_LSTRING
#define oo_LSTRING(x)  { U"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },

#undef oo_STRING_SET
#define oo_STRING_SET(x,setType)  { U"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#undef oo_LSTRING_SET
#define oo_LSTRING_SET(x,setType)  { U"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), nullptr, nullptr, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },

#undef oo_STRING_VECTOR
#define oo_STRING_VECTOR(x,n)  { U"" #x, stringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), nullptr, nullptr, 1, nullptr, U"" #n, nullptr, nullptr },
#undef oo_LSTRING_VECTOR
#define oo_LSTRING_VECTOR(x,n)  { U"" #x, lstringwa, Melder_offsetof (ooSTRUCT, x), sizeof (char32 *), nullptr, nullptr, 1, nullptr, U"" #n, nullptr, nullptr },

#define oo_STRUCT(Type,x)  { U"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), U"" #Type, & struct##Type :: s_description, 0, nullptr, nullptr, nullptr, nullptr },
#define oo_STRUCT_SET(Type,x,setType)  { U"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), U"" #Type, & struct##Type :: s_description, 3, (conststring32) setType##_getText, (conststring32) setType##_getValue, nullptr, nullptr },
#define oo_STRUCTVEC(Type,x,n)  { U"" #x, structwa, Melder_offsetof (ooSTRUCT, x), sizeof (struct struct##Type), U"" #Type, & struct##Type :: s_description, 1, nullptr, U"" #n, nullptr, nullptr },

#define oo_OBJECT(Type,version,x)  { U"" #x, objectwa, Melder_offsetof (ooSTRUCT, x), sizeof (Type), U"" #Type, & theClassInfo_##Type, 0, nullptr, nullptr, nullptr, nullptr },
#define oo_COLLECTION_OF(Type,x,ItemType,version)  { U"" #x, collectionofwa, Melder_offsetof (ooSTRUCT, x), sizeof (class struct##ItemType), U"" #Type, & theClassInfo_Collection, 0, (conststring32) & theClassInfo_##ItemType, nullptr, nullptr, nullptr },
#define oo_COLLECTION(Type,x,ItemType,version)  { U"" #x, collectionwa, Melder_offsetof (ooSTRUCT, x), sizeof (class struct##ItemType), U"" #Type, & theClassInfo_##Type, 0, (conststring32) & theClassInfo_##ItemType, nullptr, nullptr, nullptr },
#define oo_TRANSIENT_FILE(x)
#define oo_TRANSIENT_FOLDER(x)
#define oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE(Class, x)
#define oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE(Class, x)

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
