/* melder_fatal.cpp
 *
 * Copyright (C) 1992-2021 Paul Boersma
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
#include <mutex>

static std::mutex theMelder_fatal_mutex;

static void defaultFatal (conststring32 message) {
	MelderConsole::write (U"Fatal error: ", true);
	MelderConsole::write (message, true);
	MelderConsole::write (U"\n", true);
}

static void (*theFatalProc) (conststring32 message) = & defaultFatal;

static const conststring32 theCrashMessage { U"Praat will crash. "
	"Please notify the author (paul.boersma@uva.nl) with all of the following information, "
	"before closing this window (and please mention in your email precisely what you were doing when this crash occurred):\n\n" };

void Melder_fatal_ (const MelderArg& arg1,
	const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6, const MelderArg& arg7,
	const MelderArg& arg8, const MelderArg& arg9, const MelderArg& arg10,
	const MelderArg& arg11, const MelderArg& arg12, const MelderArg& arg13)
{
	std::lock_guard <std::mutex> lock (theMelder_fatal_mutex);
	MelderError::_append (theCrashMessage);
	MelderError::_append (arg1. _arg ? arg1. _arg : U"");
	MelderError::_append (arg2. _arg ? arg2. _arg : U"");
	MelderError::_append (arg3. _arg ? arg3. _arg : U"");
	MelderError::_append (arg4. _arg ? arg4. _arg : U"");
	MelderError::_append (arg5. _arg ? arg5. _arg : U"");
	MelderError::_append (arg6. _arg ? arg6. _arg : U"");
	MelderError::_append (arg7. _arg ? arg7. _arg : U"");
	MelderError::_append (arg8. _arg ? arg8. _arg : U"");
	MelderError::_append (arg9. _arg ? arg9. _arg : U"");
	MelderError::_append (arg10._arg ? arg10._arg : U"");
	MelderError::_append (arg11._arg ? arg11._arg : U"");
	MelderError::_append (arg12._arg ? arg12._arg : U"");
	MelderError::_append (arg13._arg ? arg13._arg : U"");
	trace (U"FATAL: ", Melder_getError ());
	throw MelderError ();
}

void Melder_assert_ (const char *pathName, int lineNumber, const char *condition) {
	/*
		This function tries to make sure that it allocates no heap memory.
		Hence, character conversion is done in place rather than with Melder_peek8to32(),
		and Melder_integer() is also avoided.
	*/
	std::lock_guard <std::mutex> lock (theMelder_fatal_mutex);
	static char32 pathNameBuffer [1000], conditionBuffer [1000], lineNumberBuffer [40];
	Melder_8to32_inplace (pathName, pathNameBuffer, kMelder_textInputEncoding::UTF8);
	const char32 *p_lastFolderSeparator = str32rchr (pathNameBuffer, U'/');
	if (! p_lastFolderSeparator)
		p_lastFolderSeparator = str32rchr (pathNameBuffer, U'\\');
	const conststring32 fileName = ( p_lastFolderSeparator ? p_lastFolderSeparator + 1 : pathNameBuffer );
	Melder_8to32_inplace (condition, conditionBuffer, kMelder_textInputEncoding::UTF8);
	static char lineNumberBuffer8 [40];
	sprintf (lineNumberBuffer8, "%d", lineNumber);
	Melder_8to32_inplace (lineNumberBuffer8, lineNumberBuffer, kMelder_textInputEncoding::UTF8);
	MelderError::_append (theCrashMessage);
	MelderError::_append (U"Assertion failed in file \"");
	MelderError::_append (fileName);
	MelderError::_append (U"\" at line ");
	MelderError::_append (lineNumberBuffer);
	MelderError::_append (U":\n   ");
	MelderError::_append (conditionBuffer);
	MelderError::_append (U"\n\n");
	trace (U"FATAL: ", Melder_getError ());
	throw MelderError ();
}

void Melder_setFatalProc (void (*fatal) (conststring32))
	{ theFatalProc = fatal ? fatal : & defaultFatal; }

void Melder_fatal__ () {
	(*theFatalProc) (Melder_getError ());
}

/* End of file melder_fatal.cpp */
