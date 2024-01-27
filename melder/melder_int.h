#ifndef _melder_int_h_
#define _melder_int_h_
/* melder_int.h
 *
 * Copyright (C) 1992-2021,2023,2024 Paul Boersma
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

/*
	The following two lines are for obsolete (i.e. C99) versions of stdint.h
*/
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <stdint.h>
using byte = unsigned char;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using integer = intptr_t;   // the default size of an integer (a "long" is only 32 bits on 64-bit Windows)
using long_not_integer = long;   // for cases where we explicitly need the type "long", such as when printfing to %ld
using int_not_integer = int;   // for cases where we explicitly need the type "int", such as when scanfing to %n
using uinteger = uintptr_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
#ifndef INT12_MAX
	#define INT12_MAX   2047
	#define INT12_MIN  -2048
#endif
#ifndef UINT12_MAX
	#define UINT12_MAX   4096
#endif
#ifndef INT24_MAX
	#define INT24_MAX   8388607
	#define INT24_MIN  -8388608
#endif
#ifndef UINT24_MAX
	#define UINT24_MAX   16777216
#endif
#define INTEGER_MAX  ( sizeof (integer) == 4 ? INT32_MAX : INT64_MAX )
#define INTEGER_MIN  ( sizeof (integer) == 4 ? INT32_MIN : INT64_MIN )
/*
	The bounds of the contiguous set of integers that in a "double" can represent only themselves.
*/
#ifndef INT54_MAX
	#define INT54_MAX   9007199254740991LL
	#define INT54_MIN  -9007199254740991LL
#endif

inline bool Melder_integersAreBigEndian () {
	int32_t dummy = 1;
	return * (char *) & dummy == '\0';
}

inline integer operator"" _integer (unsigned long long value) { return integer (value); }

/*
	We assume that the types "integer" and "uinteger" are both large enough to contain
	any possible value that Praat wants to assign to them.
	This entails that we assume that these types can be converted to each other without bounds checking.
	We therefore crash Praat if this second assumption is not met.
*/
inline uinteger integer_to_uinteger (integer n) {
	Melder_assert (n >= 0);
	return (uinteger) n;
}
inline integer uinteger_to_integer (uinteger n) {
	Melder_assert (n <= INTEGER_MAX);
	return (integer) n;
}
inline int32 integer_to_int32 (integer n) {
	Melder_assert (n >= INT32_MIN && n <= INT32_MAX);
	return (int32) n;
}
inline integer Melder_iroundUpToPowerOfTwo (integer n) {
	if (n <= 0)
		return 1;
	if (n > INTEGER_MAX / 2 + 1)
		return 0;   // 0 signals overflow; note that signed integer overflow is UB in C++, so this test cannot be removed by relying on n becoming negative
	n -= 1;
	n |= n >> 1;   // copy the highest 1-bit to its right
	n |= n >> 2;   // copy the two highest 1-bits to their right
	n |= n >> 4;   // copy the four highest 1-bits to their right
	n |= n >> 8;   // copy the eight highest 1-bits to their right
	n |= n >> 16;   // copy the 16 highest 1-bits to their right
	if (sizeof (integer) > 4)
		n |= (n >> 16) >> 16;   // copy the 32 highest 1-bits to their right ("n >> 32" would give a compiler warning on 32-bit platforms)
	n += 1;
	return n;
}

inline integer integer_abs (integer n) {
	Melder_assert (sizeof (integer) == sizeof (long) || sizeof (integer) == sizeof (long long));
	if (sizeof (integer) == sizeof (long))
		return labs ((long_not_integer) n);
	else // sizeof (integer) == sizeof (long long)
		return llabs (n);
}

struct MelderIntegerRange {
	integer first, last;
	bool isEmpty () const { return ( last < first ); }
	integer size () const {
		const integer result = last - first + 1;
		return std::max (result, 0_integer);
	}
};

template <typename T>
void Melder_clipLeft (T minimum, T *var) {   // no action if either undefined
	if (*var < minimum)
		*var = minimum;
}

template <typename T>
T Melder_clippedLeft (T minimum, T var) {
	return var < minimum ? minimum : var;   // if minimum undefined, then var
}

template <typename T>
void Melder_clipRight (T *var, T maximum) {   // no action if either undefined
	if (*var > maximum)
		*var = maximum;
}

template <typename T>
T Melder_clippedRight (T var, T maximum) {
	return var > maximum ? maximum : var;   // if maximum undefined, then var
}

template <typename T>
void Melder_clip (T minimum, T *var, T maximum) {
	Melder_assert (! (maximum < minimum));   // NaN-safe
	if (*var < minimum)
		*var = minimum;
	else if (*var > maximum)
		*var = maximum;
}

template <typename T>
T Melder_clipped (T minimum, T var, T maximum) {
	Melder_assert (! (maximum < minimum));   // NaN-safe
	return var < minimum ? minimum : var > maximum ? maximum : var;   // if minimum and maximum undefined, then var
}

template <typename T>
void Melder_moveCloserToBy (T *x, T to, T by) {
	if (*x < to)
		*x = Melder_clippedRight (*x + by, to);
	else if (*x > to)
		*x = Melder_clippedLeft (to, *x - by);
}

template <typename T>
void Melder_sort (T *p1, T *p2) {
	if (*p2 < *p1)
		std::swap (*p1, *p2);
}

class kleenean {
	int _intValue;
public:
	constexpr static int UNKNOWN = -1;
	constexpr static int NO_ = 0;
	constexpr static int YES_ = 1;
	explicit constexpr kleenean (int initialValue): _intValue (initialValue) { }
	bool isTrue () const noexcept {
		return our _intValue > 0;
	}
	bool isFalse () const noexcept {
		return our _intValue == 0;
	}
	bool isUnknown () const noexcept {
		return our _intValue < 0;
	}
	bool isKnown () const noexcept {
		return our _intValue >= 0;
	}
	explicit operator bool () const noexcept {
		return our isTrue();
	}
	bool operator! () const noexcept {
		return our isFalse();
	}
	kleenean operator&& (const kleenean other) const noexcept {
		return our isFalse() || other. isFalse() ? kleenean (our NO_) :
				our isTrue() && other. isTrue() ? kleenean (our YES_) :
				kleenean (our UNKNOWN);
	}
	kleenean operator|| (const kleenean other) const noexcept {
		return our isTrue() || other. isTrue() ? kleenean (our YES_) :
				our isFalse() && other. isFalse() ? kleenean (our NO_) :
				kleenean (our UNKNOWN);
	}
	kleenean operator== (const kleenean other) const noexcept {   // logical equivalence
		return our isUnknown() || other. isUnknown() ? kleenean (our UNKNOWN) :
				kleenean (our isTrue() == other. isTrue());
	}
	kleenean operator!= (const kleenean other) const noexcept {
		return our isUnknown() || other. isUnknown() ? kleenean (our UNKNOWN) :
				kleenean (our isTrue() != other. isTrue());
		// this is the same as ! ( *this == other )
	}
};
constexpr kleenean kleenean_UNKNOWN = kleenean (-1);
constexpr kleenean kleenean_NO = kleenean (0);
constexpr kleenean kleenean_YES = kleenean (1);

/* End of file melder_int.h */
#endif
