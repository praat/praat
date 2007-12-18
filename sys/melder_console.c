/* melder_console.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* pb 2007/12/13
 */

#include "melder.h"
#ifdef _WIN32
	#include <windows.h>
#endif

void Melder_writeToConsole (wchar_t *message, bool useStderr) {
	if (message == NULL) return;
	#if defined (_WIN32)
		(void) stderr;
		static HANDLE console = NULL;
		if (console == NULL) {
			console = CreateFile (L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
		}
		WriteConsole (console, message, wcslen (message), NULL, NULL);   // BUG: does not support redirection.
	#else
		Melder_fwriteWcsAsUtf8 (message, wcslen (message), useStderr ? stderr : stdout);
	#endif
}

/* End of file melder_console.c */
