#ifndef _melder_casual_h_
#define _melder_casual_h_
/* melder_casual.h
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

/*
	Function:
		Sends a message without user interference.
	Behaviour:
		Writes to stderr on Unix, otherwise to a special window.
*/

inline static void _recursiveTemplate_Melder_casual (const MelderArg& arg) {
	MelderConsole::write (arg._arg, true);
}
template <typename... Args>
void _recursiveTemplate_Melder_casual (const MelderArg& first, Args... rest) {
	_recursiveTemplate_Melder_casual (first);
	_recursiveTemplate_Melder_casual (rest...);
}

template <typename... Args>
void Melder_casual (const MelderArg& first, Args... rest) {
	_recursiveTemplate_Melder_casual (first, rest...);
	MelderConsole::write (U"\n", true);
}

void MelderCasual_memoryUse (integer message = 0);

/* End of file melder_casual.h */
#endif
