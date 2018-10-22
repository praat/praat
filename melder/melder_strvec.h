#ifndef _melder_strvec_h_
#define _melder_strvec_h_
/* melder_strvec.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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
	T** at = nullptr;
	integer size = 0;
	_stringvector () { }
	_stringvector (T** givenAt, integer givenSize): at (givenAt), size (givenSize) { }
	T* & operator[] (integer i) {
		return our at [i];
	}
	_stringvector<T> part (integer firstPosition, integer lastPosition) {
		Melder_assert (firstPosition >= 1 && firstPosition <= our size);
		Melder_assert (lastPosition >= 1 && lastPosition <= our size);
		integer newSize = lastPosition - (firstPosition - 1);
		if (newSize <= 0) return _stringvector<T> ();
		return _stringvector (at + (firstPosition -1), newSize);
	}
	void copyElementsFrom (_stringvector<T> other) {
		Melder_assert (other. size == our size);
		for (integer i = 1; i <= our size; i ++) {
			Melder_free (our at [i]);   // YUCK
			our at [i] = Melder_dup (other [i]). transfer();
		}
	}
};
typedef _stringvector <char32> string32vector;
typedef _stringvector <char> string8vector;

template <typename T>
class _conststringvector {
public:
	const T* const * at = nullptr;
	integer size = 0;
	_conststringvector () { }
	_conststringvector (const T* const * givenAt, integer givenSize): at (givenAt), size (givenSize) { }
	_conststringvector (_stringvector<T> other): at (other.at), size (other.size) { }
	const T* const & operator[] (integer i) {
		return our at [i];
	}
	_conststringvector<T> part (integer firstPosition, integer lastPosition) {
		Melder_assert (firstPosition >= 1 && firstPosition <= our size);
		Melder_assert (lastPosition >= 1 && lastPosition <= our size);
		integer newSize = lastPosition - (firstPosition - 1);
		if (newSize <= 0) return _stringvector<T> ();
		return _conststringvector (at + (firstPosition -1), newSize);
	}
};
typedef _conststringvector <char32> conststring32vector;
typedef _conststringvector <char> conststring8vector;

template <class T>
class _autostringvector {
	_autostring <T> * _ptr;
public:
	integer size;
	_autostringvector () {
		our _ptr = nullptr;
		our size = 0;
	}
	_autostringvector<T> (integer initialSize) {
		our _ptr = NUMvector <_autostring <T>> (1, initialSize, true);
		our size = initialSize;
	}
	_autostringvector (const _autostringvector &) = delete;
	_autostringvector (_autostringvector&& other) {
		our _ptr = other. _ptr;
		our size = other. size;
		other. _ptr = nullptr;
		other. size = 0;
	}
	_autostringvector& operator= (const _autostringvector &) = delete;   // disable copy assignment
	_autostringvector& operator= (_autostringvector&& other) noexcept {   // enable move assignment
		if (& other != this) {
			our reset ();
			our _ptr = other. _ptr;
			our size = other. size;
			other. _ptr = nullptr;
			other. size = 0;
		}
		return *this;
	}
	~ _autostringvector<T> () {
		our reset ();
	}
	explicit operator bool () const { return !! our _ptr; }
	_autostring <T> & operator[] (integer i) {
		return our _ptr [i];
	}
	_stringvector<T> get () const {
		return _stringvector<T> { (T**) our _ptr, our size };
	}
	T** peek2 () const {   // can be assigned to a [const] mutablestring32* and to a const conststring32*, but not to a conststring32*
		return (T**) our _ptr;
	}
	void reset () {
		if (our _ptr) {
			for (integer i = 1; i <= our size; i ++)
				our _ptr [i]. reset ();
			NUMvector_free (our _ptr, 1);
			our _ptr = nullptr;
			our size = 0;
		}
	}
	void copyElementsFrom (_conststringvector<T> other) {
		Melder_assert (other. size == our size);
		for (integer i = 1; i <= our size; i ++)
			our _ptr [i] = Melder_dup (other [i]);
	}
	_stringvector<T> part (integer firstPosition, integer lastPosition) {
		Melder_assert (firstPosition >= 1 && firstPosition <= our size);
		Melder_assert (lastPosition >= 1 && lastPosition <= our size);
		integer newSize = lastPosition - (firstPosition - 1);
		if (newSize <= 0) return _stringvector<T> ();
		return _stringvector ((T **) our _ptr + (firstPosition - 1), newSize);
	}
};

typedef _autostringvector <char32> autostring32vector;
typedef _autostringvector <char> autostring8vector;

using STRVEC = _stringvector <char32>;
using constSTRVEC = _conststringvector <char32>;
using autoSTRVEC = _autostringvector <char32>;

inline static autoSTRVEC STRVECclone (constSTRVEC strvec) {
	autoSTRVEC result (strvec.size);
	for (integer i = 1; i <= result.size; i ++)
		result [i] = Melder_dup (strvec [i]);
	return result;
}

/* End of file melder_strvec.h */
#endif
