/* melder_fatal.cpp
 *
 * Copyright (C) 1992-2019 Paul Boersma
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

constexpr int Melder_FATAL_BUFFER_SIZE { 2000 };
static char32 theFatalBuffer [Melder_FATAL_BUFFER_SIZE];
static const conststring32 theCrashMessage { U"Praat will crash. Notify the author (paul.boersma@uva.nl) with the following information:\n" };

void Melder_fatal (const MelderArg& arg1,
	const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6, const MelderArg& arg7,
	const MelderArg& arg8, const MelderArg& arg9, const MelderArg& arg10)
{
	std::lock_guard <std::mutex> lock (theMelder_fatal_mutex);
	conststring32 s1  = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	conststring32 s2  = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	conststring32 s3  = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	conststring32 s4  = arg4. _arg ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	conststring32 s5  = arg5. _arg ? arg5. _arg : U"";   int64 length5  = str32len (s5);
	conststring32 s6  = arg6. _arg ? arg6. _arg : U"";   int64 length6  = str32len (s6);
	conststring32 s7  = arg7. _arg ? arg7. _arg : U"";   int64 length7  = str32len (s7);
	conststring32 s8  = arg8. _arg ? arg8. _arg : U"";   int64 length8  = str32len (s8);
	conststring32 s9  = arg9. _arg ? arg9. _arg : U"";   int64 length9  = str32len (s9);
	conststring32 s10 = arg10._arg ? arg10._arg : U"";   int64 length10 = str32len (s10);
	str32cpy (theFatalBuffer, theCrashMessage);
	int64 length = str32len (theFatalBuffer);
	if (length + length1  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s1);  length += length1;  }
	if (length + length2  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s2);  length += length2;  }
	if (length + length3  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s3);  length += length3;  }
	if (length + length4  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s4);  length += length4;  }
	if (length + length5  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s5);  length += length5;  }
	if (length + length6  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s6);  length += length6;  }
	if (length + length7  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s7);  length += length7;  }
	if (length + length8  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s8);  length += length8;  }
	if (length + length9  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s9);  length += length9;  }
	if (length + length10 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s10); length += length10; }
	trace (U"FATAL: ", theFatalBuffer);
	(*theFatalProc) (theFatalBuffer);
	abort ();
}

void Melder_assert_ (const char *fileName, int lineNumber, const char *condition) {
	/*
		This function tries to make sure that it allocates no heap memory.
		Hence, character conversion is done in place rather than with Melder_peek8to32(),
		and Melder_integer() is also avoided.
	*/
	std::lock_guard <std::mutex> lock (theMelder_fatal_mutex);
	static char32 fileNameBuffer [1000], conditionBuffer [1000], lineNumberBuffer [40];
	Melder_8to32_inplace (fileName, fileNameBuffer, kMelder_textInputEncoding::UTF8);
	Melder_8to32_inplace (condition, conditionBuffer, kMelder_textInputEncoding::UTF8);
	static char lineNumberBuffer8 [40];
	sprintf (lineNumberBuffer8, "%d", lineNumber);
	Melder_8to32_inplace (lineNumberBuffer8, lineNumberBuffer, kMelder_textInputEncoding::UTF8);
	str32cpy (theFatalBuffer, theCrashMessage);
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), U"Assertion failed in file \"");
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), fileNameBuffer);
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), U"\" at line ");
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), lineNumberBuffer);
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), U":\n   ");
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), conditionBuffer);
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), U"\n");
	trace (U"FATAL: ", theFatalBuffer);
	//Melder_throw (theFatalBuffer);   // un-comment-out if you want to trace an assert
	(*theFatalProc) (theFatalBuffer);   // ...but this call will use heap memory...
	abort ();
}

void Melder_setFatalProc (void (*fatal) (conststring32))
	{ theFatalProc = fatal ? fatal : & defaultFatal; }

/* End of file melder_fatal.cpp */
