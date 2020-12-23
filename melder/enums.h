#ifndef _enums_h_
#define _enums_h_
/* enums.h
 *
 * Copyright (C) 2005,2007,2013,2015-2018,2020 Paul Boersma
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

typedef int (*enum_generic_getValue) (conststring32 text);

#define enums_begin(kType,minimum)  enum class kType { UNDEFINED = -1, MIN = minimum,
#define enums_add(kType,value,which,text)  which = value,
#define enums_alt(kType,which,text)
#define enums_end(kType,maximum,default) MAX = maximum, \
	DEFAULT = default }; \
	conststring32 kType##_getText (kType value); \
	kType kType##_getValue (conststring32 text); \
	inline kType& operator++ (kType& value) { \
		value = static_cast <kType> (static_cast <int> (value) + 1); \
		return value; \
	}


/* End of file enums.h */
#endif
