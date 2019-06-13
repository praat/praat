#ifndef _melder_fatal_h_
#define _melder_fatal_h_
/* melder_fatal.h
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
	SYNOPSIS (2018-08-08)

	Melder_fatal (args...);

		Gives an error message and aborts the program.
		Should only be caused by programming errors.

	See also Melder_assert ().
*/

void Melder_fatal (const MelderArg&,
	const MelderArg& = U"", const MelderArg& = U"", const MelderArg& = U"",
	const MelderArg& = U"", const MelderArg& = U"", const MelderArg& = U"",
	const MelderArg& = U"", const MelderArg& = U"", const MelderArg& = U""
);

void Melder_setFatalProc (void (*p_fatalProc) (conststring32));

/* End of file melder_fatal.h */
#endif
