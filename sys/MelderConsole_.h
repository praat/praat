#ifdef _header_MelderConsole_h_
#error This file is only for inclusion in melder.h
#else
#define _header_MelderConsole_h_

/* MelderConsole_.h
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
	extern void write (conststring32 message, bool useStderr);
	extern void setIsAnsi (bool isAnsi);
}

/* End of file MelderConsole_.h */
#endif
