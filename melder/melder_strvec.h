#ifndef _melder_strvec_h_
#define _melder_strvec_h_
/* melder_strvec.h
 *
 * Copyright (C) 1992-2020 Paul Boersma
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

template <typename T>
class _stringvector {
public:
	T** elements = nullptr;
	integer size = 0;
	_stringvector () { }
	_stringvector (T** givenElements, integer givenSize): elements (givenElements), size (givenSize) { }
	T* & operator[] (integer i) const {
		return our elements [i - 1];
	}
	_stringvector<T> part (integer firstPosition, integer lastPosition) {
		Melder_assert (firstPosition >= 1 && firstPosition <= our size);
		Melder_assert (lastPosition >= 1 && lastPosition <= our size);
		integer newSize = lastPosition - (firstPosition - 1);
		if (newSize <= 0)
			return _stringvector<T> ();
		return _stringvector (our elements + (firstPosition - 1), newSize);
	}
	T* *begin () const { return & our operator[] (1); }
	T* *end () const { return & our operator[] (our size + 1); }
};
using string32vector = _stringvector <char32>;
using string8vector  = _stringvector <char>;

template <typename T>
class _autostringvectorview;

template <typename T>
class _conststringvector {
public:
	const T* const * elements = nullptr;
	integer size = 0;
	_conststringvector () { }
	_conststringvector (const T* const * givenElements, integer givenSize): elements (givenElements), size (givenSize) { }
	_conststringvector (_stringvector<T> other): elements (other.elements), size (other.size) { }
	_conststringvector (_autostringvectorview<T> other): elements ((T**) other._ptr), size (other.size) { }
	const T* const & operator[] (integer i) const {
		return our elements [i - 1];
	}
	_conststringvector<T> part (integer firstPosition, integer lastPosition) {
		Melder_assert (firstPosition >= 1 && firstPosition <= our size);
		Melder_assert (lastPosition >= 1 && lastPosition <= our size);
		integer newSize = lastPosition - (firstPosition - 1);
		if (newSize <= 0)
			return _conststringvector<T> ();
		return _conststringvector (our elements + (firstPosition - 1), newSize);
	}
	T* *begin () const { return & our operator[] (1); }
	T* *end () const { return & our operator[] (our size + 1); }
};
using conststring32vector = _conststringvector <char32>;
using conststring8vector  = _conststringvector <char>;

template <typename T>
class _autostringvectorview {
public:
	_autostring <T> * elements = nullptr;
	integer size = 0;
	_autostringvectorview<T> () = default;
	_autostringvectorview<T> (_autostring <T> * givenElements, integer givenSize): elements (givenElements), size (givenSize) { }
	_autostring <T> & operator[] (integer i) const {
		return our elements [i - 1];
	}
};

template <typename T>
void operator<<= (_autostringvectorview <T> const& target, _autostringvectorview <T> const& source) {
	Melder_assert (target.size == source.size);
	for (integer i = 1; i <= target.size; i ++)
		target [i] = Melder_dup (source [i]);
}

template <typename T>
class _autostringautovector {
public:
	_autostring <T> * elements;
	integer size;
	_autostringautovector () {
		our elements = nullptr;
		our size = 0;
	}
	_autostringautovector<T> (integer initialSize) {
		our elements = MelderArray:: _alloc <_autostring <T>> (initialSize, MelderArray::kInitializationType :: ZERO);
		our size = initialSize;
	}
	_autostringautovector (const _autostringautovector &) = delete;
	_autostringautovector (_autostringautovector&& other) {
		our elements = other. elements;
		our size = other. size;
		other. elements = nullptr;
		other. size = 0;
	}
	_autostringautovector& operator= (const _autostringautovector &) = delete;   // disable copy assignment
	_autostringautovector& operator= (_autostringautovector&& other) noexcept {   // enable move assignment
		if (& other != this) {
			our reset ();
			our elements = other. elements;
			our size = other. size;
			other. elements = nullptr;
			other. size = 0;
		}
		return *this;
	}
	~ _autostringautovector<T> () {
		our reset ();
	}
	explicit operator bool () const noexcept { return !! our elements; }
	_autostring <T> & operator[] (integer i) {
		return our elements [i - 1];
	}
	_stringvector<T> get () const {
		return _stringvector<T> ((T**) our elements, our size);
	}
	_autostringvectorview<T> all () const {
		return _autostringvectorview<T> (our elements, our size);
	}
	T** peek2 () const {   // can be assigned to a [const] mutablestring32* and to a const conststring32*, but not to a conststring32*
		return (T**) our elements - 1;
	}
	void adoptFromAmbiguousOwner (_stringvector<T> const& given) {   // buy the payload from a non-autostring
		our reset();
		our elements = (_autostring <T> *) given.elements;
		our size = given.size;
	}
	_stringvector<T> releaseToAmbiguousOwner () {   // sell the payload to a non-autostring
		_autostring<T> * oldElements = our elements;
		our elements = nullptr;   // disown ourselves, preventing automatic destruction of the payload
		return _stringvector<T> ((T**) oldElements, our size);
	}
	void reset () {
		if (our elements) {
			for (integer i = 1; i <= our size; i ++)
				our elements [i - 1]. reset ();
			MelderArray:: _free (our elements, our size);
			our elements = nullptr;
			our size = 0;
		}
	}
	_autostringvectorview<T> part (integer firstPosition, integer lastPosition) {
		Melder_assert (firstPosition >= 1 && firstPosition <= our size);
		Melder_assert (lastPosition >= 1 && lastPosition <= our size);
		integer newSize = lastPosition - (firstPosition - 1);
		if (newSize <= 0)
			return _autostringvectorview<T> ();
		return _autostringvectorview<T> (our elements + (firstPosition - 1), newSize);
	}
};

using autostring32vector = _autostringautovector <char32>;
using autostring8vector  = _autostringautovector <char>;

using STRVEC = _stringvector <char32>;
using constSTRVEC = _conststringvector <char32>;
using autoSTRVEC = _autostringautovector <char32>;

inline static autoSTRVEC newSTRVECcopy (constSTRVEC strvec) {
	autoSTRVEC result (strvec.size);
	for (integer i = 1; i <= result.size; i ++)
		result [i] = Melder_dup (strvec [i]);
	return result;
}

conststring32 Melder_STRVEC (constSTRVEC const& value);

inline void operator<<= (_autostringvectorview <char32> const& target, _autostringvectorview <char32> const& source) {
	Melder_assert (target.size == source.size);
	for (integer i = 1; i <= target.size; i ++)
		target [i] = Melder_dup (source [i].get());
}
inline void operator<<= (_autostringvectorview <char32> const& target, _stringvector <char32> const& source) {
	Melder_assert (target.size == source.size);
	for (integer i = 1; i <= target.size; i ++)
		target [i] = Melder_dup (source [i]);
}

/* End of file melder_strvec.h */
#endif
