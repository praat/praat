#ifndef _melder_string_h_
#define _melder_string_h_
/* MelderString.h
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

/*
	Strings that:
		- are null-terminated
		- have O(1) access to their length
		- grow as needed
		- can be appended to without scanning for the final null character
		- automatically convert numbers, objects, file names, vectors, and matrices to strings
*/

struct MelderString16 {
	int64 length = 0;
	int64 bufferSize = 0;
	char16 *string = nullptr;   // a growing buffer, rarely shrunk (can only be freed by MelderString16_free)
};
struct MelderString {
	int64 length = 0;
	int64 bufferSize = 0;
	char32 *string = nullptr;   // a growing buffer, rarely shrunk (can only be freed by MelderString_free)
};

void MelderString16_free (MelderString16 *me);   // frees the buffer (and sets other attributes to zero)
void MelderString_free (MelderString *me);   // frees the buffer (and sets other attributes to zero)
void MelderString16_empty (MelderString16 *me);   // sets to empty string (buffer shrunk if very large)
void MelderString_empty (MelderString *me);   // sets to empty string (buffer shrunk if very large)
void MelderString_expand (MelderString *me, int64 sizeNeeded);   // increases the buffer size; there's normally no need to call this
void MelderString_ncopy (MelderString *me, conststring32 source, int64 n);

inline void _recursiveTemplate_MelderString_append (MelderString *me, const MelderArg& arg) {
	if (arg._arg) {
		const char32 *newEndOfStringLocation = stp32cpy (& my string [my length], arg._arg);   // this will append a null character
		my length = newEndOfStringLocation - & my string [0];
	} else {
		/*
			Append a null string: do nothing.
			The result will be null-terminated if `me` was null-terminated to start with,
			which is a required invariant.
		*/
	}
}
template <typename... Args>
void _recursiveTemplate_MelderString_append (MelderString *me, const MelderArg& first, Args... rest) {
	_recursiveTemplate_MelderString_append (me, first);
	_recursiveTemplate_MelderString_append (me, rest...);
}

template <typename... Args>
void MelderString_append (MelderString *me, const MelderArg& first, Args... rest) {
	integer extraLength = MelderArg__length (first, rest...);
	integer sizeNeeded = my length + extraLength + 1;
	if (sizeNeeded > my bufferSize)
		MelderString_expand (me, sizeNeeded);
	_recursiveTemplate_MelderString_append (me, first, rest...);
}

template <typename... Args>
void MelderString_copy (MelderString *me, const MelderArg& first, Args... rest) {
	constexpr int64 FREE_THRESHOLD_BYTES = 10'000;
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES)
		MelderString_free (me);
	integer length = MelderArg__length (first, rest...);
	integer sizeNeeded = length + 1;
	if (sizeNeeded > my bufferSize)
		MelderString_expand (me, sizeNeeded);
	my length = 0;
	my string [0] = U'\0';   // maintain invariant
	_recursiveTemplate_MelderString_append (me, first, rest...);
}

void MelderString16_appendCharacter (MelderString16 *me, char32 character);
void MelderString_appendCharacter (MelderString *me, char32 character);
void MelderString_get (MelderString *me, char32 *destination);   // performs no boundary checking
int64 MelderString_allocationCount ();
int64 MelderString_deallocationCount ();
int64 MelderString_allocationSize ();
int64 MelderString_deallocationSize ();

struct autoMelderString : MelderString {
	autoMelderString () {
		// inherited zero initialization suffices
	}
	~autoMelderString () {
		MelderString_free (this);
	}
};

/* End of file MelderString.h */
#endif
