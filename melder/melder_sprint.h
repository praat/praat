#ifndef _melder_sprint_h_
#define _melder_sprint_h_
/* melder_sprint.h
 *
 * Copyright (C) 1992-2018,2020 Paul Boersma
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

inline void _recursiveTemplate_Melder_sprint (char32 **inout_pointer, const MelderArg& arg) {
	if (arg._arg) {
		char32 *newEndOfStringLocation = stp32cpy (*inout_pointer, arg._arg);
		*inout_pointer = newEndOfStringLocation;
	}
}
template <typename... Args>
void _recursiveTemplate_Melder_sprint (char32 **inout_pointer, const MelderArg& first, Args... rest) {
	_recursiveTemplate_Melder_sprint (inout_pointer, first);
	_recursiveTemplate_Melder_sprint (inout_pointer, rest...);
}

template <typename... Args>
void Melder_sprint (mutablestring32 buffer, int64 bufferSize, const MelderArg& first, Args... rest) {
	integer length = MelderArg__length (first, rest...);
	if (length >= bufferSize) {
		for (int64 i = 0; i < bufferSize; i ++)
			buffer [i] = U'?';
		if (bufferSize > 0)
			buffer [bufferSize - 1] = U'\0';
		return;
	}
	char32 *p = & buffer [0];
	_recursiveTemplate_Melder_sprint (& p, first, rest...);
}

/* End of file melder_sprint.h */
#endif
