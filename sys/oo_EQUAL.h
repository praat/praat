/* oo_EQUAL.h
 *
 * Copyright (C) 1994-2007,2009,2011-2019 Paul Boersma
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
	if (our x != thy x) return false;

#define oo_SET(type, storage, x, setType)  \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		if (our x [_i] != thy x [_i]) return false; \
	}

#define oo_VECTOR(type, storage, x, min, max)  \
	{ \
		integer _min = (min), _max = (max); \
		if (! our x != ! thy x || \
			(our x && ! NUMvector_equal <type> (our x, thy x, _min, _max))) return false; \
	}

#define oo_ANYVEC(type, storage, x, sizeExpression)  \
	{ \
		integer _size = (sizeExpression); \
		Melder_assert (our x.size == _size); \
		if (! NUMequal (our x.get(), thy x.get())) return false; \
	}

#define oo_ANYMAT(type, storage, x, nrowExpression, ncolExpression)  \
	{ \
		integer _nrow = (nrowExpression); \
		integer _ncol = (ncolExpression); \
		if (_nrow != 0 && _ncol != 0) { \
			/*Melder_assert (our x.nrow == _nrow && our x.ncol == _ncol); BUG routinely violated in SSCP for lowerCholesky */ \
			if (! NUMequal (our x.get(), thy x.get())) return false; \
		} \
	}

#define oo_ANYTEN3(type, storage, x, ndim1Expression, ndim2Expression, ndim3Expression)  \
	{ \
		integer _ndim1 = (ndim1Expression); \
		integer _ndim2 = (ndim2Expression); \
		integer _ndim3 = (ndim3Expression); \
		if (_ndim1 != 0 && _ndim2 != 0 && _ndim3 != 0) { \
			if (! NUMequal (our x.get(), thy x.get())) return false; \
		} \
	}

#define oo_ENUMx(kType, storage, x)  \
	if (our x != thy x) return false;

//#define oo_ENUMx_SET(kType, storage, x, setType)  \
//	for (int _i = 0; _i <= (int) setType::MAX; _i ++) if (our x [_i] != thy x [_i]) return false;

//#define oo_ENUMx_VECTOR(kType, storage, x, min, max)  \
//	{ \
//		integer _min = (min), _max = (max); \
//		if (! our x != ! thy x || \
//			(our x && ! NUMvector_equal <type> (our x, thy x, _min, _max))) return false; \
//	}

#define oo_STRINGx(storage, x)  \
	if (! Melder_equ (our x.get(), thy x.get())) return false;

#define oo_STRINGx_SET(storage, x, setType)  \
	for (int _i = 0; _i <= setType::MAX; _i ++) \
		if (! Melder_equ (our x [_i].get(), thy x [_i].get())) return false;

#define oo_STRINGx_VECTOR(storage, x, n)  \
	{ \
		integer _size = (n); \
		Melder_assert (_size == our x.size); \
		if (! our x != ! thy x) return false; \
		if (our x) { \
			for (integer _i = 1; _i <= _size; _i ++) { \
				if (! Melder_equ (our x [_i].get(), thy x [_i].get())) return false; \
			} \
		} \
	}

#define oo_STRUCT(Type, x)  \
	if (! our x. equal (& thy x)) return false;

#define oo_STRUCT_SET(Type, x, setType)  \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		if (! our x [_i]. equal (& thy x [_i])) return false; \
	}

#define oo_STRUCT_VECTOR(Type, x, n)  \
	{ \
		integer _size = (n); \
		if (! our x != ! thy x) return false; \
		if (our x) { \
			for (integer _i = 1; _i <= _size; _i ++) { \
				if (! our x [_i]. equal (& thy x [_i])) return false; \
			} \
		} \
	}

#define oo_OBJECT(Class, version, x)  \
	if (! our x != ! thy x || (our x && ! Data_equal (our x.get(), thy x.get()))) return false;

#define oo_COLLECTION_OF(Class, x, ItemClass, version)  \
	for (integer _i = 1; _i <= our x.size; _i ++) { \
		if (! our x.at [_i] != ! thy x.at [_i] || (our x.at [_i] && ! Data_equal (our x.at [_i], thy x.at [_i]))) return false; \
	}

#define oo_COLLECTION(Class, x, ItemClass, version)  \
	if (! our x != ! thy x || (our x && ! Data_equal (our x.get(), thy x.get()))) return false;

#define oo_FILE(x)  \
	if (! MelderFile_equal (& our x, & thy x)) return false;

#define oo_DIR(x)  \
	if (! MelderDir_equal (& our x, & thy x)) return false;

#define oo_DEFINE_STRUCT(Type)  \
	bool struct##Type :: equal (Type thee) {

#define oo_END_STRUCT(Type)  \
		return true; \
	}

#define oo_DEFINE_CLASS(Class, Parent)  \
	bool struct##Class :: v_equal (Daata _thee_Daata) { \
		Class thee = static_cast <Class> (_thee_Daata); \
		if (! Class##_Parent :: v_equal (thee)) return false;

#define oo_END_CLASS(Class)  \
		return true; \
	}

#define oo_FROM(from)

#define oo_ENDFROM

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_COMPARING  1
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo_EQUAL.h */
