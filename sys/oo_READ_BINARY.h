/* oo_READ_BINARY.h
 *
 * Copyright (C) 1994-2009,2011-2019 Paul Boersma
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

#define oo_SIMPLE(type, storage, x)  \
	our x = binget##storage (_filePointer_);

#define oo_SET(type, storage, x, setType)  \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		our x [_i] = binget##storage (_filePointer_); \
	}

#define oo_VECTOR(type, storage, x, min, max)  \
	{ \
		integer _min = (min), _max = (max); \
		if (_max >= _min) { \
			our x = NUMvector_readBinary_##storage (_min, _max, _filePointer_); \
		} \
	}

#define oo_ANYVEC(type, storage, x, sizeExpression)  \
	{ \
		integer _size = (sizeExpression); \
		our x = vector_readBinary_##storage (_size, _filePointer_); \
	}

#define oo_ANYMAT(type, storage, x, nrowExpression, ncolExpression)  \
	{ \
		integer _nrow = (nrowExpression), _ncol = (ncolExpression); \
		our x = matrix_readBinary_##storage (_nrow, _ncol, _filePointer_); \
	}

#define oo_ANYTEN3(type, storage, x, ndim1Expression, ndim2Expression, ndim3Expression)  \
	{ \
		integer _ndim1 = (ndim1Expression), _ndim2 = (ndim2Expression), _ndim3 = (ndim3Expression); \
		our x = tensor3_readBinary_##storage (_ndim1, _ndim2, _ndim3, _filePointer_); \
	}

#define oo_ENUMx(kType, storage, x)  \
	our x = (kType) binget##storage (_filePointer_, (int) kType::MIN, (int) kType::MAX, U"" #kType);

//#define oo_ENUMx_SET(kType, storage, x, setType)  \
//	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
//		our x [_i] = (kType) binget##storage (_filePointer_, (int) kType::MIN, (int) kType::MAX, U"" #kType); \
//	}

//#define oo_ENUMx_VECTOR(kType, storage, x, min, max)  \
//	{ \
//		integer _min = (min), _max = (max); \
//		if (_max >= _min) { \
//			our x = NUMvector <kType> (_min, _max); \
//			for (integer _i = _min; _i <= _max; _i ++) { \
//				our x [_i] = (kType) binget##storage (_filePointer_, (int) kType::MIN, (int) kType::MAX, U"" #kType); \
//		} \
//	}

#define oo_STRINGx(storage, x)  \
	our x = binget##storage (_filePointer_);

#define oo_STRINGx_SET(storage, x, setType)  \
	for (int _i = 0; _i <= setType::MAX; _i ++) { \
		our x [_i] = binget##storage (_filePointer_); \
	}

#define oo_STRINGx_VECTOR(storage, x, n)  \
	{ \
		integer _size = (n); \
		if (_size >= 1) { \
			our x = autostring32vector (_size); \
			for (integer _i = 1; _i <= _size; _i ++) { \
				our x [_i] = binget##storage (_filePointer_); \
			} \
		} \
	}

#define oo_STRUCT(Type,x)  \
	our x. readBinary (_filePointer_, _formatVersion_);

#define oo_STRUCT_SET(Type, x, setType) \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		our x [_i]. readBinary (_filePointer_, _formatVersion_); \
	}

#define oo_STRUCT_VECTOR(Type, x, n)  \
	{ \
		integer _size = (n); \
		if (_size >= 1) { \
			our x = NUMvector <struct##Type> (1, _size); \
			for (integer _i = 1; _i <= _size; _i ++) { \
				our x [_i]. readBinary (_filePointer_, _formatVersion_); \
			} \
		} \
	}

#define oo_OBJECT(Class, formatVersion, x)  \
	{ \
		int _formatVersion = (formatVersion); \
		if (bingetex (_filePointer_)) { \
			our x = Thing_new (Class); \
			our x -> v_readBinary (_filePointer_, _formatVersion); \
		} \
	}

#define oo_COLLECTION_OF(Class, x, ItemClass, formatVersion)  \
	{ \
		int _formatVersion = (formatVersion); \
		integer _n = bingetinteger32BE (_filePointer_); \
		for (integer _i = 1; _i <= _n; _i ++) { \
			auto##ItemClass _item = Thing_new (ItemClass); \
			_item -> v_readBinary (_filePointer_, _formatVersion); \
			our x.addItem_move (_item.move()); \
		} \
	}

#define oo_COLLECTION(Class, x, ItemClass, formatVersion)  \
	{ \
		int _formatVersion = (formatVersion); \
		integer _n = bingetinteger32BE (_filePointer_); \
		our x = Class##_create (); \
		for (integer _i = 1; _i <= _n; _i ++) { \
			auto##ItemClass _item = Thing_new (ItemClass); \
			_item -> v_readBinary (_filePointer_, _formatVersion); \
			our x -> addItem_move (_item.move()); \
		} \
	}

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Type)  \
	void struct##Type :: readBinary (FILE *_filePointer_, int _formatVersion_) { \
		(void) _formatVersion_;

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class, Parent)  \
	void struct##Class :: v_readBinary (FILE *_filePointer_, int _formatVersion_) { \
		Melder_require (_formatVersion_ <= our classInfo -> version, \
			U"The format of this file is too new. Download a newer version of Praat."); \
		Class##_Parent :: v_readBinary (_filePointer_, _formatVersion_);

#define oo_END_CLASS(Class)  \
	}

#define oo_FROM(from)  \
	{ \
		int _from = (from); \
		if (_formatVersion_ >= _from) {

#define oo_ENDFROM  \
		} \
	}

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  1
#define oo_READING_TEXT  0
#define oo_READING_BINARY  1
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo_READ_BINARY.h */
