/* oo_DESTROY.h
 *
 * Copyright (C) 1994-2007,2009-2020 Paul Boersma
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

#define oo_SIMPLE(type, storage, x)

#define oo_SET(type, storage, x, setType)

#define oo_ANYVEC(type, storage, x, sizeExpression)  \
	if (! _thisStructCanAutodestroyItsMembers_) { \
		our x. reset (); \
	}

#define oo_ANYMAT(type, storage, x, nrowExpression, ncolExpression)  \
	if (! _thisStructCanAutodestroyItsMembers_) { \
		our x. reset (); \
	}

#define oo_ANYTEN3(type, storage, x, ndim1Expression, ndim2Expression, ndim3Expression)  \
	if (! _thisStructCanAutodestroyItsMembers_) { \
		our x. reset (); \
	}

#define oo_ENUMx(kType, storage, x)

//#define oo_ENUMx_SET(kType, storage, x, setType)

#define oo_STRINGx(storage, x)  \
	if (! _thisStructCanAutodestroyItsMembers_) { \
		our x. reset (); \
	}

#define oo_STRINGx_SET(storage, x, setType)  \
	if (! _thisStructCanAutodestroyItsMembers_) { \
		for (int _i = 0; _i <= setType::MAX; _i ++) { \
			our x [_i]. reset (); \
		} \
	}

#define oo_STRINGx_VECTOR(storage, x, n)  \
	if (! _thisStructCanAutodestroyItsMembers_) { \
		our x. reset (); \
	}

#define oo_STRUCT(Type, x)  \
	our x. destroy ();

#define oo_STRUCT_SET(Type, x, setType)  \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		our x [_i]. destroy (); \
	}

#define oo_STRUCTVEC(Type, x, n)  \
{ \
	for (integer _i = 1; _i <= our x.size; _i ++) { \
		our x [_i]. destroy (); \
	} \
	if (! _thisStructCanAutodestroyItsMembers_) { \
		our x. reset (); \
	} \
}

#define oo_OBJECT(Class, version, x)  \
	if (! _thisStructCanAutodestroyItsMembers_) { \
		our x. reset (); \
	}

#define oo_COLLECTION_OF(Class, x, ItemClass, version)

#define oo_COLLECTION(Class, x, ItemClass, version)

#define oo_FILE(x)

#define oo_DIR(x)

#define oo_DEFINE_STRUCT(Struct)  \
	void struct##Struct :: destroy () { \
		constexpr bool _thisStructCanAutodestroyItsMembers_ = false; \
		(void) _thisStructCanAutodestroyItsMembers_;

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class, Parent)  \
	void struct##Class :: v_destroy () noexcept { \
		constexpr bool _thisStructCanAutodestroyItsMembers_ = true; \
		(void) _thisStructCanAutodestroyItsMembers_;

#define oo_END_CLASS(Class)  \
		Class##_Parent :: v_destroy (); \
	}

#define oo_FROM(from)

#define oo_ENDFROM

#define oo_DECLARING  0
#define oo_DESTROYING  1
#define oo_COPYING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo_DESTROY.h */
