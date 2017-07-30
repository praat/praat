/* melder_sysenv.cpp
 *
 * Copyright (C) 1992-2011,2015,2016,2017 Paul Boersma
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
 * pb 2004/10/14 made Cygwin-compatible
 * Eric Carlson & Paul Boersma 2005/05/19 made MinGW-compatible
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2011/04/05 C++
 */

#if defined (_WIN32)
	#if ! defined (__CYGWIN__) && ! defined (__MINGW32__)
		#include <crtl.h>
	#endif
	#include <windows.h>
	#include <errno.h>
	#include <stdlib.h>
#else
	#if defined (linux)
		#include  <sys/wait.h>
	#endif
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/wait.h>
#endif
#include "melder.h"

char32 * Melder_getenv (const char32 *variableName) {
	#if defined (macintosh) || defined (UNIX) || defined (__MINGW32__) || defined (__CYGWIN__)
		return Melder_peek8to32 (getenv (Melder_peek32to8 (variableName)));
	#elif defined (_WIN32)
		static char32 buffer [11] [255];
		static int ibuffer = 0;
		if (++ ibuffer == 11) ibuffer = 0;
		long n = GetEnvironmentVariableW (variableName, buffer [ibuffer], 255);   BUG
		if (n == ERROR_ENVVAR_NOT_FOUND) return nullptr;
		return & buffer [ibuffer] [0];
	#else
		return nullptr;
	#endif
}

void Melder_system (const char32 *command) {
	if (! command) command = U"";
	#if defined (macintosh) || defined (UNIX)
		if (system (Melder_peek32to8 (command)) != 0)
			Melder_throw (U"System command failed.");
	#elif defined (_WIN32)
		STARTUPINFO siStartInfo;
		PROCESS_INFORMATION piProcInfo;
		char32 *comspec = Melder_getenv (U"COMSPEC");   // e.g. "C:\WINDOWS\COMMAND.COM" or "C:\WINNT\windows32\cmd.exe"
		if (! comspec) {
			comspec = Melder_getenv (U"ComSpec");
		}
		autoMelderString buffer;
		if (comspec) {
			MelderString_copy (& buffer, comspec);
		} else {
			OSVERSIONINFOEX osVersionInfo;
			memset (& osVersionInfo, 0, sizeof (OSVERSIONINFOEX));
			osVersionInfo. dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);
			if (! GetVersionEx ((OSVERSIONINFO *) & osVersionInfo)) {
				osVersionInfo. dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
				if (! GetVersionEx ((OSVERSIONINFO *) & osVersionInfo))
					Melder_throw (U"System command cannot find system version.");
			}
			switch (osVersionInfo. dwPlatformId) {
				case VER_PLATFORM_WIN32_NT: {
					MelderString_copy (& buffer, U"cmd.exe");
				} break; case VER_PLATFORM_WIN32_WINDOWS: {
					MelderString_copy (& buffer, U"command.com");
				} break; default: {
					MelderString_copy (& buffer, U"command.com");
				}
			}
		}
		MelderString_append (& buffer, U" /c ", command);
        memset (& siStartInfo, 0, sizeof (siStartInfo));
        siStartInfo. cb = sizeof (siStartInfo);
		if (! CreateProcess (nullptr, Melder_peek32toW (buffer.string), nullptr, nullptr, true, CREATE_NO_WINDOW, nullptr, nullptr, & siStartInfo, & piProcInfo))
			Melder_throw (U"Cannot create subprocess.");
		WaitForSingleObject (piProcInfo. hProcess, -1);
		CloseHandle (piProcInfo. hProcess);
		CloseHandle (piProcInfo. hThread);
	#endif
}

void Melder_execv (const char32 *executableFileName, int narg, char32 ** args) {
	#if defined (macintosh) || defined (UNIX)
		Melder_casual (U"Command: <<", executableFileName, U">>");
		autostring8vector args8 (0, narg + 1);
		args8 [0] = Melder_32to8 (executableFileName);
		for (int i = 1; i <= narg; i ++) {
			Melder_casual (U"Argument ", i, U": <<", args [i], U">>");
			args8 [i] = Melder_32to8 (args [i]);
		}
		args8 [narg + 1] = nullptr;
		pid_t processID = fork ();
		if (processID == 0) {   // we are in the child process
			execvp (Melder_peek32to8 (executableFileName), args8.peek());
			/* if we arrive here, some error occurred */
			fprintf (stderr, "Some error occurred");
			_exit (EXIT_FAILURE);
		} else if (processID > 0) {   // we are still in the calling Praat
			waitpid (processID, nullptr, 0);
		} else {
			Melder_throw (U"Could not fork.");
		}
	#elif defined (_WIN32)
	#endif
}

/* End of file melder_sysenv.cpp */
