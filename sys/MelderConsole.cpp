/* MelderConsole.cpp
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

#include "melder.h"

namespace MelderConsole {// reopen
	static bool _isAnsi = false;
}

void MelderConsole::setIsAnsi (bool isAnsi) {
	MelderConsole :: _isAnsi = isAnsi;
}

void MelderConsole::write (conststring32 message, bool useStderr) {
	if (! message)
		return;
	#if defined (_WIN32)
		(void) useStderr;
		static HANDLE console = nullptr;
		if (! console)
			console = CreateFileW (L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, 0);
		if (MelderConsole :: _isAnsi) {
			size_t n = str32len (message);
			for (integer i = 0; i < n; i ++) {
				unsigned int kar = (unsigned short) message [i];
				fputc (kar, stdout);
			}
			//CHAR* messageA = (CHAR*) MelderTrace::_peek32to8 (message);
			//WriteConsoleA (console, messageA, strlen (messageA), nullptr, nullptr);
		//} else if (Melder_consoleIsUtf8) {
			//char *messageA = MelderTrace::_peek32to8 (message);
			//fprintf (stdout, "%s", messageA);
		} else {
			WCHAR* messageW = (WCHAR*) MelderTrace::_peek32to16 (message);
			WriteConsoleW (console, messageW, wcslen (messageW), nullptr, nullptr);
		}
	#else
		FILE *f = ( useStderr ? stderr : stdout );
		for (const char32 *p = & message [0]; *p != U'\0'; p ++) {
			char32 kar = *p;
			if (kar <= 0x00007F) {
				fputc ((int) kar, f);   // because fputc wants an int instead of a uint8 (guarded conversion)
			} else if (kar <= 0x0007FF) {
				fputc (0xC0 | (kar >> 6), f);
				fputc (0x80 | (kar & 0x00003F), f);
			} else if (kar <= 0x00FFFF) {
				fputc (0xE0 | (kar >> 12), f);
				fputc (0x80 | ((kar >> 6) & 0x00003F), f);
				fputc (0x80 | (kar & 0x00003F), f);
			} else {
				fputc (0xF0 | (kar >> 18), f);
				fputc (0x80 | ((kar >> 12) & 0x00003F), f);
				fputc (0x80 | ((kar >> 6) & 0x00003F), f);
				fputc (0x80 | (kar & 0x00003F), f);
			}
		}
	#endif
}

/* End of file MelderConsole.cpp */
