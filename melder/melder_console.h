#ifndef _melder_console_h_
#define _melder_console_h_

/* melder_console.h
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

namespace MelderConsole {
	enum class Encoding {
		UTF8 = 0,
		UTF16 = 1,
		ANSI = 2
	};
	extern void write (conststring32 message, bool useStderr);
	extern void setEncoding (Encoding encoding);
}

/* End of file melder_console.h */
#endif
