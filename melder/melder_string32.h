#ifndef _melder_string32_h_
#define _melder_string32_h_
/* melder_string32.h
 *
 * Copyright (C) 1992-2019 Paul Boersma
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

using char8 = unsigned char;
using char16 = char16_t;
using char32 = char32_t;

/* USAGE: Constant strings.

	For whole null-terminated strings whose contents will not be changed, use conststring32:
		void writeString (conststring32 text);

	For a pointer to a character in a conststring32, use "const char32 *".
	This is appropriate if you need to cycle later:
		const char32 *p = & text [0];
		...
		p ++;
	This is also appropriate for character searches:
		const char32 *semicolonLocation = str32chr (text, U';');

	For an array of characters, use "const char32 []":
		void displayCharacters (const char32 characters []);
	Such an array may or may not be null-terminated.
*/
using conststring8 = const char *;
using conststring16 = const char16 *;
using conststringW = const wchar_t *;
using conststring32 = const char32 *;

/* USAGE: Mutable strings.

	For whole null-terminated strings whose contents will be changed, use mutablestring32:
		void changeCase (mutablestring32 string);

	For a pointer to a character in a mutablestring32, use "char32 *".
	This is appropriate if you need to cycle later:
		char32 *p = & string [0];
		...
		p ++;
	This is also appropriate for character searches:
		char32 *semicolonLocation = str32chr (string, U';');

	For an array of characters that will be changed, use "char32 []":
		void modifyCodes (char32 codes []);
	Such an array may or may not be null-terminated.
*/
using mutablestring8 = char *;
using mutablestring16 = char16 *;
using mutablestringW = wchar_t *;
using mutablestring32 = char32 *;

#define strequ  ! strcmp
#define strnequ  ! strncmp

template <class T>
class _autostring {
	T *ptr;
public:
	#if 1
	_autostring () : ptr (nullptr) {
		//if (Melder_debug == 39) Melder_casual (U"autostring: zero constructor");
	}
	#else
	_autostring () = default;   // explicit default, so that it can be used in a union
	#endif
	explicit _autostring (integer length, bool f = false) {
		our ptr = ( f ? Melder_malloc_f (T, length + 1) : Melder_malloc (T, length + 1) );
		our ptr [0] = '\0';
		our ptr [length] = '\0';
	}
	//_autostring (T *string) : ptr (string) {
		//if (Melder_debug == 39) Melder_casual (U"autostring: constructor from C-string ", Melder_pointer (ptr));
	//}
	~_autostring () {
		//if (Melder_debug == 39) Melder_casual (U"autostring: entering destructor ptr = ", Melder_pointer (ptr));
		if (our ptr) Melder_free (our ptr);
		//if (Melder_debug == 39) Melder_casual (U"autostring: leaving destructor");
	}
	template <class U> T& operator[] (U i) {
		return our ptr [i];
	}
	T * get () const {
		return our ptr;
	}
	#if 0
	operator T* () const {
		return our ptr;
	}
	#endif
	/*T ** operator& () {
		return & our ptr;
	}*/
	T * transfer () {
		T *tmp = our ptr;
		our ptr = nullptr;
		return tmp;
	}
	void reset () {
		if (our ptr) Melder_free (our ptr);
	}
	void resize (int64 newLength) {
		T *tmp = (T *) Melder_realloc (our ptr, (newLength + 1) * (int64) sizeof (T));
		our ptr = tmp;
		our ptr [newLength] = '\0';
	}
	_autostring& operator= (const _autostring&) = delete;   // disable copy assignment
	_autostring (_autostring &) = delete;   // disable copy constructor
	template <class Y> _autostring (_autostring<Y> &) = delete;   // disable copy constructor
	explicit operator bool () const noexcept { return !! our ptr; }
	/*
		Enable moving.
	*/
	_autostring (_autostring&& other) noexcept {   // enable move constructor
		our ptr = other.ptr;
		other.ptr = nullptr;
	}
	_autostring& operator= (_autostring&& other) noexcept {   // enable move assignment
		if (& other != this) {
			if (our ptr) Melder_free (our ptr);
			our ptr = other.ptr;
			other.ptr = nullptr;
		}
		return *this;
	}
	_autostring&& move () noexcept {
		return static_cast <_autostring&&> (*this);
	}
	void _zero_asInUnion () {
		our ptr = nullptr;
	}
};

typedef _autostring <char> autostring8;
typedef _autostring <char16> autostring16;
typedef _autostring <wchar_t> autostringW;
typedef _autostring <char32> autostring32;

autostring32 Melder_dup (conststring32 string /* cattable */);
autostring32 Melder_dup_f (conststring32 string /* cattable */);

/* End of file melder_string32.h */
#endif
