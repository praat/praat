/* melder_console.cpp
 *
 * Copyright (C) 1992-2005,2007,2008,2011,2015-2017 Paul Boersma
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

#include "melder.h"

#if defined (_WIN32) && defined (CONSOLE_APPLICATION)
int main (int argc, char *argvA []);
extern "C" int wmain (int argc, wchar_t *argvW []);
extern "C" int wmain (int argc, wchar_t *argvW []) {
	char **argvA = nullptr;
	if (argc > 0) {
		argvA = NUMvector <char *> (0, argc - 1);
		for (int iarg = 0; iarg < argc; iarg ++) {
			argvA [iarg] = Melder_32to8 (Melder_peekWto32 ((argvW [iarg])));
		}
	}
	return main (argc, argvA);
}
#endif

/* End of file melder_console.cpp */
