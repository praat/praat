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
	integer _capacity = 0;
public:
	_autostring <T> * elements;
	integer size;
	_autostringautovector () {
		our elements = nullptr;
		our size = 0;
	}
	explicit _autostringautovector (integer givenSize) {
		our elements = MelderArray:: _alloc <_autostring <T>> (givenSize, MelderArray::kInitializationType :: ZERO);
		our size = givenSize;
		our _capacity = givenSize;
	}
	void reset () {
		if (our elements) {
			for (integer i = 1; i <= our size; i ++)
				our elements [i - 1]. reset ();
			MelderArray:: _free (our elements, our size);
			our elements = nullptr;
		}
		our size = 0;
		our _capacity = 0;
	}
	~ _autostringautovector<T> () {
		our reset ();
	}
	_stringvector<T> get () const {
		return _stringvector<T> (reinterpret_cast <T**> (our elements), our size);
	}
	_autostringvectorview<T> all () const {
		return _autostringvectorview<T> (our elements, our size);
	}
	void adoptFromAmbiguousOwner (_stringvector<T> const& given) {   // buy the payload from a non-autostring
		our reset();
		our elements = reinterpret_cast <_autostring <T> *> (given.elements);
		our size = given.size;
		our _capacity = given.size;
	}
	_stringvector<T> releaseToAmbiguousOwner () {   // sell the payload to a non-autostring
		_autostring<T> * oldElements = our elements;
		our elements = nullptr;   // disown ourselves, preventing automatic destruction of the payload
		integer oldSize = our size;
		our size = 0;
		our _capacity = 0;
		return _stringvector<T> (reinterpret_cast <T**> (oldElements), oldSize);
	}
	_autostringautovector (const _autostringautovector &) = delete;   // disable copy construction
	_autostringautovector& operator= (const _autostringautovector &) = delete;   // disable copy assignment
	_autostringautovector (_autostringautovector&& other) {   // enable move construction
		our elements = other.elements;
		our size = other.size;
		our _capacity = other._capacity;
		other.elements = nullptr;   // disown source
		other.size = 0;   // to keep the source in a valid state
		other._capacity = 0;
	}
	_autostringautovector& operator= (_autostringautovector&& other) noexcept {   // enable move assignment
		if (other.elements != our elements) {
			our reset ();
			our elements = other.elements;
			our size = other.size;
			our _capacity = other._capacity;
			other.elements = nullptr;   // disown source
			other.size = 0;   // to keep the source in a valid state
			other._capacity = 0;
		}
		return *this;
	}
	explicit operator bool () const noexcept { return !! our elements; }
	_autostring <T> & operator[] (integer i) {
		return our elements [i - 1];
	}
	T** peek2 () const {   // can be assigned to a [const] mutablestring32* and to a const conststring32*, but not to a conststring32*
		return (T**) our elements - 1;
	}
	_autostringvectorview<T> part (integer firstPosition, integer lastPosition) {
		Melder_assert (firstPosition >= 1 && firstPosition <= our size);
		Melder_assert (lastPosition >= 1 && lastPosition <= our size);
		integer newSize = lastPosition - (firstPosition - 1);
		if (newSize <= 0)
			return _autostringvectorview<T> ();
		return _autostringvectorview<T> (our elements + (firstPosition - 1), newSize);
	}
	_autostringautovector&& move () noexcept { return static_cast <_autostringautovector&&> (*this); }   // enable construction and assignment for l-values (variables) via explicit move()
	void initWithCapacity (integer capacity) {
		if (capacity > 0)
			our cells = MelderArray:: _alloc <_autostring <T>> (capacity, MelderArray::kInitializationType::ZERO);
		our size = 0;
		our _capacity = capacity;
	}
	void resize (integer newSize) {
		if (newSize > our _capacity) {
			/*
				The new capacity is at least twice the old capacity.
				When starting at a capacity of 0, and continually upsizing by one,
				the capacity sequence will be: 0, 11, 33, 77, 165, 341, 693, 1397,
				2805, 5621, 11253, 22517, 45045, 90101, 180213, 360437, 720885...
			*/
			integer newCapacity = newSize + our size + 10;
			/*
				Create without change.
			*/
			_autostring <T> * newElements = MelderArray:: _alloc <_autostring <T>> (newCapacity, MelderArray::kInitializationType::ZERO);
			/*
				Change without error.
			*/
			for (integer i = 1; i <= our size; i ++)
				newElements [i - 1] = our elements [i - 1]. move();
			if (our elements)
				MelderArray:: _free (our elements, our _capacity);
			our elements = newElements;
			our _capacity = newCapacity;
		}
		our size = newSize;
	}
	void insert (integer position, conststring32 value) {
		if (position == 0)
			position = our size + 1;
		/*
			Create without change.
		*/
		autostring32 newValue = Melder_dup (value);
		/*
			Pivot.
		*/
		our resize (our size + 1);
		/*
			Change without error.
		*/
		Melder_assert (position >= 1 && position <= our size);
		for (integer i = our size; i > position; i --)
			our elements [i - 1] = our elements [i - 2]. move();
		our elements [position - 1] = newValue. move();
	}
	void append (conststring32 value) {
		our insert (0, value);
	}
	void remove (integer position) noexcept {
		Melder_assert (position >= 1 && position <= our size);
		for (integer i = position; i < our size; i ++)
			our elements [i - 1] = std::move (our elements [i]);
		our resize (our size - 1);   // change without error
	}
};

using autostring32vector = _autostringautovector <char32>;
using autostring8vector  = _autostringautovector <char>;

using STRVEC = _stringvector <char32>;
using constSTRVEC = _conststringvector <char32>;
using autoSTRVEC = _autostringautovector <char32>;

inline autoSTRVEC newSTRVECcopy (constSTRVEC strvec) {
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
