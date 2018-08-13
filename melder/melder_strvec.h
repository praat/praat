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
	T** at;
	integer size;
	T* & operator[] (integer i) {
		return our at [i];
	}
};
typedef _stringvector <char32> string32vector;
typedef _stringvector <char> string8vector;

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
			for (integer i = 1; i <= our size; i ++) {
				our _ptr [i]. reset ();
			}
			NUMvector_free (our _ptr, 1);
			our _ptr = nullptr;
			our size = 0;
		}
	}
	void copyFrom (_autostringvector& other) {
		our reset ();
		our _ptr = NUMvector <_autostring <T>> (1, other. size, true);
		our size = other. size;
		for (integer i = 1; i <= our size; i ++) {
			our _ptr [i] = Melder_dup (other. _ptr [i].get());
		}
	}
	void copyElementsFrom (_autostringvector& other) {
		Melder_assert (other. size == our size);
		for (integer i = 1; i <= our size; i ++) {
			our _ptr [i] = Melder_dup (other. _ptr [i].get());
		}
	}
	void copyElementsFrom_upTo (_autostringvector& other, integer to) {
		Melder_assert (to <= other. size && to <= our size);
		for (integer i = 1; i <= to; i ++) {
			our _ptr [i] = Melder_dup (other. _ptr [i].get());
		}
	}
};

typedef _autostringvector <char32> autostring32vector;
typedef _autostringvector <char> autostring8vector;

/* End of file melder_strvec.h */
#endif
