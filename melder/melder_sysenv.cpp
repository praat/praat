/* melder_sysenv.cpp
 *
 * Copyright (C) 1992-2007,2011,2012,2015-2019,2023 Paul Boersma
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
	#include <fcntl.h>
	#include <windows.h>
	#include <errno.h>
	#include <stdlib.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/wait.h>
#endif
#include "melder.h"

conststring32 Melder_getenv (conststring32 variableName) {
	#if defined (macintosh) || defined (UNIX) || defined (__MINGW32__) || defined (__CYGWIN__)
		return Melder_peek8to32 (getenv (Melder_peek32to8 (variableName)));
	#elif defined (_WIN32)
		static char32 buffer [11] [255];
		static int ibuffer = 0;
		if (++ ibuffer == 11)
			ibuffer = 0;
		DWORD n = GetEnvironmentVariableW (variableName, buffer [ibuffer], 255);   BUG
		if (n == ERROR_ENVVAR_NOT_FOUND) return nullptr;
		return & buffer [ibuffer] [0];
	#else
		return nullptr;
	#endif
}

static void ensureThatStdoutAndStderrAreInitialized () {
	#if defined (_WIN32)
		/*
			Stdout and stderr are initialized automatically if we are redirected to a pipe or file.
			Stdout and stderr are not initialized, however, if Praat is started from the console,
			neither in GUI mode nor in console mode; in these latter cases,
			we manually attach stdout and stderr to the calling console.
		*/
		auto ensureThatStreamIsInitialized = [] (FILE *stream, int handle) {
			const bool streamHasBeenInitialized = ( _fileno (stream) >= 0 );
			if (! streamHasBeenInitialized) {
				/*
					Don't change the following four lines into
						freopen ("CONOUT$", "w", stream);
					because if you did that, the distinction between stdout and stderr would be lost.
				*/
				HANDLE osfHandle = GetStdHandle (handle);
				if (osfHandle) {
					const int fileDescriptor = _open_osfhandle ((intptr_t) osfHandle, _O_TEXT);
					Melder_assert (fileDescriptor != 0);
					FILE *f = _fdopen (fileDescriptor, "w");
					if (! f)
						return;   // this can happen under Cygwin
					*stream = *f;
				}
			}
		};
		ensureThatStreamIsInitialized (stdout, STD_OUTPUT_HANDLE);
		ensureThatStreamIsInitialized (stderr, STD_ERROR_HANDLE);
	#endif
}
autostring32 runSystem_STR (conststring32 command) {
	autostring32 result;
	if (! command)
		command = U"";
	autostring8 command8 = Melder_32to8 (command);
	#if defined (macintosh) || defined (UNIX)
		int stdoutPipe [2], stderrPipe [2];
		if (pipe (stdoutPipe) == -1 || pipe (stderrPipe) == -1)
			Melder_throw (U"Cannot start system command <<", command, U">> (“pipe error”).");
		pid_t childProcess = fork ();
		if (childProcess == -1)
			Melder_throw (U"Cannot start system command <<", command, U">> (“fork error”).");
		if (childProcess == 0) {
			/*
				We are in the child process.
			*/
			while ((dup2 (stdoutPipe [1], STDOUT_FILENO) == -1) && (errno == EINTR)) {
			}
			while ((dup2 (stderrPipe [1], STDERR_FILENO) == -1) && (errno == EINTR)) {
			}
			close (stdoutPipe [1]);
			close (stderrPipe [1]);
			close (stdoutPipe [0]);
			close (stderrPipe [0]);
			//
			//	From the execl man page:
			//		int execl(const char *path, const char *arg0, ..., /*, (char *)0, */);
			//	With more quotes from the execl man page:
			//
			execl (
				"/bin/sh",   // "The initial argument for these functions is the pathname of a file which is to be executed."
				// "The const char *arg0 and subsequent ellipses in the execl(), execlp(), and execle() functions"
				// "can be thought of as arg0, arg1, ..., argn.  Together they describe a list of one or more pointers"
				// "to null-terminated strings that represent the argument list available to the executed program."
				"sh",   // "The first argument, by convention, should point to the file name associated with the file being executed."
					// (that is, this is arg0, which should to sh become argv[0], which should generally be the app name)
				"-c",   // (from the bash man page: "If the -c option is present, then commands are read from `string`."; this is argv[1])
				command8.get(),   // (the `string`, combining the complete space-separated command that sh should execute)
				nullptr   // "The list of arguments *must* be terminated by a NULL pointer."
			);   // if all goes right, this implicity closes the child process
			/*
				If we arrive here, then execl must have returned,
				which is an error condition.
			*/
			_exit (1);   // close the child process explicitly
		}
		/*
			We are in the parent process.
		*/
		close (stdoutPipe [1]);
		close (stderrPipe [1]);
		/*
			Collect the output of the child process.
		*/
		autoMelderString stdout_string, stderr_string;
		char buffer8 [1+4096];
		for (;;) {
			ssize_t count = read (stdoutPipe [0], buffer8, 4096);
			if (count == -1) {
				if (errno == EINTR)
					continue;
				Melder_throw (U"Error while handling child process output.");
			}
			if (count == 0)
				break;
			buffer8 [count] = '\0';
			MelderString_append (& stdout_string, Melder_peek8to32 (buffer8));
		}
		for (;;) {
			ssize_t count = read (stderrPipe [0], buffer8, 4096);
			if (count == -1) {
				if (errno == EINTR)
					continue;
				Melder_throw (U"Error while handling child process error output.");
			}
			if (count == 0)
				break;
			buffer8 [count] = '\0';
			MelderString_append (& stderr_string, Melder_peek8to32 (buffer8));
		}
		close (stdoutPipe [0]);
		close (stderrPipe [0]);
		wait (0);
		if (stderr_string.length > 0)
			Melder_throw (U"runSystem$: error:\n", stderr_string.string);
		result = Melder_dup (stdout_string. string);
	#elif defined (_WIN32)
	#endif
	return result;
}
void Melder_system (conststring32 command) {
	if (! command)
		command = U"";
	#if defined (macintosh) || defined (UNIX)
		if (system (Melder_peek32to8 (command)) != 0)
			Melder_throw (U"System command failed.");
	#elif defined (_WIN32)
		STARTUPINFO siStartInfo;
		PROCESS_INFORMATION piProcInfo;
		conststring32 comspec = Melder_getenv (U"COMSPEC");   // e.g. "C:\WINDOWS\COMMAND.COM" or "C:\WINNT\windows32\cmd.exe"
		if (! comspec)
			comspec = Melder_getenv (U"ComSpec");
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
		constexpr bool tryStdPassThrough = false;
		int savedModeStdout, savedModeStderr;
		if (tryStdPassThrough) {
			ensureThatStdoutAndStderrAreInitialized ();
			Melder_assert (_fileno (stdout) >= 0);
			Melder_assert (_fileno (stderr) >= 0);
			fflush (stdout);
			fflush (stderr);
			savedModeStdout = _setmode (_fileno (stdout), _O_U16TEXT);   // without line-break translation
			savedModeStderr = _setmode (_fileno (stderr), _O_U16TEXT);   // without line-break translation
		}
		Melder_assert (! str32chr (buffer.string, ' '));
		MelderString_append (& buffer, U" /c ", command);
		memset (& siStartInfo, 0, sizeof (siStartInfo));
		siStartInfo. cb = sizeof (siStartInfo);
		siStartInfo. dwFlags = STARTF_USESTDHANDLES;
		//siStartInfo. hStdInput = GetStdHandle (STD_INPUT_HANDLE);
		siStartInfo. hStdOutput = GetStdHandle (STD_OUTPUT_HANDLE);
		siStartInfo. hStdError = GetStdHandle (STD_ERROR_HANDLE);
		memset (& piProcInfo, 0, sizeof (piProcInfo));
		autostringW bufferW = Melder_32toW_fileSystem (buffer.string);
		if (! CreateProcess (nullptr, bufferW.get(), nullptr, nullptr, true, CREATE_NO_WINDOW, nullptr, nullptr, & siStartInfo, & piProcInfo))
			Melder_throw (U"Cannot start system command <<", command, U">>.");
		if (0) {
			HANDLE stdoutDup, stderrDup;
			DuplicateHandle (GetCurrentProcess(),
				GetStdHandle (STD_OUTPUT_HANDLE),
				piProcInfo. hProcess,
				& stdoutDup,
				0,
				TRUE,
				DUPLICATE_SAME_ACCESS
			);
			DuplicateHandle (GetCurrentProcess(),
				GetStdHandle (STD_ERROR_HANDLE),
				piProcInfo. hProcess,
				& stderrDup,
				0,
				TRUE,
				DUPLICATE_SAME_ACCESS
			);
		}
		if (WaitForSingleObject (piProcInfo. hProcess, INFINITE) != 0)
			Melder_throw (U"Cannot finish system command <<", command, U">>.");
		DWORD exitCode;
		if (! GetExitCodeProcess (piProcInfo. hProcess, & exitCode))
			Melder_throw (U"Cannot evaluate system command <<", command, U">>.");
		if (exitCode != 0)
			Melder_throw (U"Error in system command <<", command, U">> (exit code ", exitCode, U").");
		CloseHandle (piProcInfo. hProcess);
		CloseHandle (piProcInfo. hThread);
		if (tryStdPassThrough) {
			fflush (stdout);
			fflush (stderr);
			_setmode (_fileno (stdout), savedModeStdout);
			_setmode (_fileno (stderr), savedModeStderr);
		}
	#endif
}

void Melder_execv (conststring32 executableFileName, integer narg, char32 ** args) {
	#if defined (macintosh) || defined (UNIX)
		Melder_casual (U"Command: <<", executableFileName, U">>");
		autostring8vector args8 (narg + 2);
		args8 [1] = Melder_32to8 (executableFileName);
		for (integer i = 1; i <= narg; i ++) {
			Melder_casual (U"Argument ", i, U": <<", args [i], U">>");
			args8 [1 + i] = Melder_32to8 (args [i]);
		}
		args8 [narg + 2] = autostring8();
		pid_t processID = fork ();
		if (processID == 0) {   // we are in the child process
			execvp (Melder_peek32to8 (executableFileName), & args8.peek2() [1]);
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
