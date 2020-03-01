/* oo_COPY.h
 *
 * Copyright (C) 1994-2007,2009,2011-2020 Paul Boersma
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
	thy x = our x;

#define oo_SET(type, storage, x, setType)  \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		thy x [_i] = our x [_i]; \
	}

#define oo_ANYVEC(type, storage, x, sizeExpression)  \
	thy x = newvectorcopy (our x.all());

#define oo_ANYMAT(type, storage, x, nrowExpression, ncolExpression)  \
	thy x = newmatrixcopy (our x.all());

#define oo_ANYTEN3(type, storage, x, ndim1Expression, ndim2Expression, ndim3Expression)  \
	thy x = newtensor3copy (our x.get());

#define oo_ENUMx(kType, storage, x)  \
	thy x = our x;

//#define oo_ENUMx_SET(kType, storage, x, setType)  \
//	for (int _i = 0; _i <= (int) setType::MAX; _i ++) thy x [_i] = our x [_i];

#define oo_STRINGx(storage, x)  \
	if (our x) thy x = Melder_dup (our x.get());

#define oo_STRINGx_SET(storage, x, setType)  \
	for (int _i = 0; _i <= setType::MAX; _i ++) { \
		if (our x [_i]) thy x [_i] = Melder_dup (our x [_i].get()); \
	}

#define oo_STRINGx_VECTOR(storage, x, n)  \
	{ \
		integer _size = (n); \
		Melder_assert (_size == our x.size); \
		if (our x) { \
			thy x = autoSTRVEC (_size); \
			for (integer _i = 1; _i <= _size; _i ++) { \
				if (our x [_i]) thy x [_i] = Melder_dup (our x [_i].get()); \
			} \
		} \
	}

#define oo_STRUCT(Type, x)  \
	our x. copy (& thy x);

#define oo_STRUCT_SET(Type, x, setType)  \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		our x [_i]. copy (& thy x [_i]); \
	}

#define oo_STRUCTVEC(Type, x, n)  \
{ \
	integer _size = (n); \
	Melder_assert (_size == our x.size); \
	if (_size > 0) { \
		thy x = newvectorzero <struct##Type> (_size); \
		for (integer _i = 1; _i <= _size; _i ++) { \
			our x [_i]. copy (& thy x [_i]); \
		} \
	} \
}

#define oo_OBJECT(Class, version, x)  \
	if (our x) thy x = Data_copy (our x.get());

#define oo_COLLECTION_OF(Class, x, ItemClass, version)  \
	if (our x._capacity > 0) { \
		thy x.at._elements = Melder_calloc (ItemClass, our x._capacity); \
		thy x.at._elements --; \
	} \
	thy x.size = our x.size; \
	thy x._capacity = our x._capacity; \
	thy x._ownItems = our x._ownItems; \
	thy x._ownershipInitialized = our x._ownershipInitialized; \
	for (integer _i = 1; _i <= our x.size; _i ++) { \
		if (our x.at [_i]) thy x.at [_i] = Data_copy (our x.at [_i]).releaseToAmbiguousOwner(); \
	}

#define oo_COLLECTION(Class, x, ItemClass, version)  \
	if (our x) thy x = Data_copy (our x.get());

#define oo_FILE(x)  \
	MelderFile_copy (& our x, & thy x);

#define oo_DIR(x)  \
	MelderDir_copy (& our x, & thy x);

#define oo_DEFINE_STRUCT(Type)  \
	void struct##Type :: copy (Type thee) {

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class, Parent)  \
	void struct##Class :: v_copy (Daata _thee_Daata) { \
		Class thee = static_cast <Class> (_thee_Daata); \
		Class##_Parent :: v_copy (thee);

#define oo_END_CLASS(Class)  \
	}

#define oo_FROM(from)

#define oo_ENDFROM

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  1
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo_COPY.h */
