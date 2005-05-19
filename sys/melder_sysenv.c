/* melder_sysenv.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
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

/*
 * pb 2004/10/14 made Cygwin-compatible
 * Eric Carlson & Paul Boersma 2005/05/19 made MinGW-compatible
 */

/*
 * This is a replacement for the CodeWarrior routines getenv and system,
 * into which many bugs were introduced in the year 2000.
 */

#if defined (_WIN32)
	#if ! defined (__CYGWIN__) && ! defined (MINGW)
		#include <crtl.h>
	#endif
	#include <windows.h>
	#include <errno.h>
	#include <stdlib.h>
#endif
#if defined (__MACH__)
	#undef macintosh
	#define UNIX
#endif
#include "melder.h"

char * Melder_getenv (const char *variableName) {
	#if defined (UNIX) || defined (MINGW)
		return getenv (variableName);
	#elif defined (macintosh)
		(void) variableName;
		return NULL;
	#elif defined (_WIN32)
		char *env;
		int length = strlen (variableName);
		int i = 0;
		for (i = 0; (env = _Environ [i]) != NULL; i ++)
			if (strnequ (env, variableName, length))
				return env [length] == '=' ? env + length + 1 : NULL;
		return NULL;
	#endif
}

int Melder_system (const char *command) {
	#if defined (UNIX)
		if (system (command) != 0) return Melder_error ("System command failed.");
		return 1;
	#elif defined (macintosh)
		(void) command;
		return Melder_error ("System command not implemented on classic MacOS. Upgrade to MacOS X.");
	#elif defined (_WIN32)
		STARTUPINFO siStartInfo;
		PROCESS_INFORMATION piProcInfo;
		char *comspec = Melder_getenv ("COMSPEC");   /* E.g. "C:\WINDOWS\COMMAND.COM" or "C:\WINNT\windows32\cmd.exe" */
		if (comspec != NULL) {
			strcpy (Melder_buffer1, comspec);
		} else {
			OSVERSIONINFOEX osVersionInfo;
			memset (& osVersionInfo, 0, sizeof (OSVERSIONINFOEX));
			osVersionInfo. dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);
			if (! GetVersionEx ((OSVERSIONINFO *) & osVersionInfo)) {
				osVersionInfo. dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
				if (! GetVersionEx ((OSVERSIONINFO *) & osVersionInfo))
					return Melder_error ("System command cannot find system version.");
			}
			switch (osVersionInfo. dwPlatformId) {
				case VER_PLATFORM_WIN32_NT: {
					strcpy (Melder_buffer1, "cmd.exe");
				} break; case VER_PLATFORM_WIN32_WINDOWS: {
					strcpy (Melder_buffer1, "command.com");
				} break; default: {
					strcpy (Melder_buffer1, "command.com");
				}
			}
		}
		strcat (Melder_buffer1, " /c ");
		strcat (Melder_buffer1, command == NULL ? "" : command);
        memset (& siStartInfo, 0, sizeof (siStartInfo));
        siStartInfo. cb = sizeof (siStartInfo);
		if (! CreateProcess (NULL, Melder_buffer1, NULL, NULL, TRUE, 0, NULL, NULL, & siStartInfo, & piProcInfo))
			return 0;
		WaitForSingleObject (piProcInfo. hProcess, -1);
		CloseHandle (piProcInfo. hProcess);
		CloseHandle (piProcInfo. hThread);
		return 1;
	#endif
}

/* End of file melder_sysenv.c */
