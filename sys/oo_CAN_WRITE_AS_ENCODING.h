/* oo_CAN_WRITE_AS_ENCODING.h
 *
 * Copyright (C) 2007,2009,2011-2018 Paul Boersma
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

#define oo_SIMPLE(type,storage,x)

#define oo_SET(type,storage,x,setType)

#define oo_VECTOR(type,storage,x,min,max)

#define oo_MATRIX(type,storage,x,row1,row2,col1,col2)

#define oo_ENUMx(kType,storage,x)

//#define oo_ENUMx_SET(kType,storage,x,setType)

//#define oo_ENUMx_VECTOR(kType,storage,x,min,max)

#define oo_STRINGx(storage,x)  \
	if (our x && ! Melder_isEncodable (our x.get(), encoding)) return false;

#define oo_STRINGx_SET(storage,x,setType)  \
	for (int _i = 0; _i <= setType::MAX; _i ++) \
		if (our x [_i] && ! Melder_isEncodable (our x [_i].get(), encoding)) return false;

#define oo_STRINGx_VECTOR(storage,x,size)  \
	{ \
		integer _size = (size); \
		if (our x) { \
			for (integer _i = 1; _i <= _size; _i ++) { \
				if (our x [_i] && ! Melder_isEncodable (our x [_i].get(), encoding)) return false; \
			} \
		} \
	}

#define oo_STRUCT(Type,x)  \
	if (! our x. canWriteAsEncoding (encoding)) return false;

#define oo_STRUCT_SET(Type,x,setType)  \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		if (! our x [_i]. canWriteAsEncoding (encoding)) return false; \
	}

#define oo_STRUCT_VECTOR_FROM(Type,x,min,max)  \
	{ \
		integer _min = (min), _max = (max); \
		if (our x) { \
			for (integer _i = _min; _i <= _max; _i ++) { \
				if (! our x [_i]. canWriteAsEncoding (encoding)) return false; \
			} \
		} \
	}

#define oo_STRUCT_MATRIX_FROM(Type,x,row1,row2,col1,col2)  \
	{ \
		integer _row1 = (row1), _row2 = (row2), _col1 = (col1), _col2 = (col2); \
		if (our x) { \
			for (integer _irow = _row1; _irow <= _row2; _irow ++) { \
				for (integer _icol = _col1; _icol <= _col2; _icol ++) { \
					if (! our x [_irow] [_icol]. canWriteAsEncoding (encoding)) return false; \
				} \
			} \
		} \
	}

#define oo_OBJECT(Class,version,x)  \
	if (our x && ! Data_canWriteAsEncoding (our x.get(), encoding)) return false;

#define oo_COLLECTION_OF(Class,x,ItemClass,version)  \
	for (integer _i = 1; _i <= x.size; _i ++) { \
		if (our x.at [_i] && ! Data_canWriteAsEncoding (our x.at [_i], encoding)) return false; \
	}

#define oo_COLLECTION(Class,x,ItemClass,version)  \
	if (our x && ! Data_canWriteAsEncoding (our x.get(), encoding)) return false;

#define oo_FILE(x)  \
	if (! Melder_isEncodable (our x. path, encoding)) return false;

#define oo_DIR(x)  \
	if (! Melder_isEncodable (our x. path, encoding)) return false;

#define oo_DEFINE_STRUCT(Type)  \
	bool struct##Type :: canWriteAsEncoding (int encoding) { \
		(void) encoding;

#define oo_END_STRUCT(Type)  \
		return true; \
	}

#define oo_DEFINE_CLASS(Class,Parent)  \
	bool struct##Class :: v_canWriteAsEncoding (int encoding) { \
		if (! Class##_Parent :: v_canWriteAsEncoding (encoding)) return false;

#define oo_END_CLASS(Class)  \
		return true; \
	}

#define oo_FROM(from)

#define oo_ENDFROM

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  1
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo_CAN_WRITE_AS_ENCODING.h */
