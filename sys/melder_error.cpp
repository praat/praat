/* melder_error.cpp
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

static void defaultError (const char32 *message) {
	Melder_writeToConsole (str32str (message, U"You interrupted ") ? U"User interrupt: " : U"Error: ", true);
	Melder_writeToConsole (message, true);
	Melder_writeToConsole (U"\n", true);
}

static void (*theError) (const char32 *) = defaultError;   // initial setting after start-up; will stay if program is run from batch

void Melder_setErrorProc (void (*error) (const char32 *)) {
	theError = error ? error : defaultError;
}

static char32 errors [2000+1];   // safe in low-memory situations

static void appendError (const char32 *message) {
	if (! message) return;
	int length = str32len (errors), messageLength = str32len (message);
	if (length + messageLength > 2000) return;
	str32cpy (errors + length, message);
}

bool Melder_hasError () { return errors [0] != U'\0'; }

bool Melder_hasError (const char32 *partialError) { return !! str32str (errors, partialError); }

void Melder_clearError () { errors [0] = U'\0'; }

char32 * Melder_getError () { return & errors [0]; }

void Melder_appendError (Melder_1_ARG) {
	appendError (arg1. _arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_2_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_3_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (arg3. _arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_4_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (arg3. _arg);
	appendError (arg4. _arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_5_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (arg3. _arg);
	appendError (arg4. _arg);
	appendError (arg5. _arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_6_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (arg3. _arg);
	appendError (arg4. _arg);
	appendError (arg5. _arg);
	appendError (arg6. _arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_7_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (arg3. _arg);
	appendError (arg4. _arg);
	appendError (arg5. _arg);
	appendError (arg6. _arg);
	appendError (arg7. _arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_8_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (arg3. _arg);
	appendError (arg4. _arg);
	appendError (arg5. _arg);
	appendError (arg6. _arg);
	appendError (arg7. _arg);
	appendError (arg8. _arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_9_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (arg3. _arg);
	appendError (arg4. _arg);
	appendError (arg5. _arg);
	appendError (arg6. _arg);
	appendError (arg7. _arg);
	appendError (arg8. _arg);
	appendError (arg9. _arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_10_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (arg3. _arg);
	appendError (arg4. _arg);
	appendError (arg5. _arg);
	appendError (arg6. _arg);
	appendError (arg7. _arg);
	appendError (arg8. _arg);
	appendError (arg9. _arg);
	appendError (arg10._arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_11_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (arg3. _arg);
	appendError (arg4. _arg);
	appendError (arg5. _arg);
	appendError (arg6. _arg);
	appendError (arg7. _arg);
	appendError (arg8. _arg);
	appendError (arg9. _arg);
	appendError (arg10._arg);
	appendError (arg11._arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_13_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (arg3. _arg);
	appendError (arg4. _arg);
	appendError (arg5. _arg);
	appendError (arg6. _arg);
	appendError (arg7. _arg);
	appendError (arg8. _arg);
	appendError (arg9. _arg);
	appendError (arg10._arg);
	appendError (arg11._arg);
	appendError (arg12._arg);
	appendError (arg13._arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_15_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (arg3. _arg);
	appendError (arg4. _arg);
	appendError (arg5. _arg);
	appendError (arg6. _arg);
	appendError (arg7. _arg);
	appendError (arg8. _arg);
	appendError (arg9. _arg);
	appendError (arg10._arg);
	appendError (arg11._arg);
	appendError (arg12._arg);
	appendError (arg13._arg);
	appendError (arg14._arg);
	appendError (arg15._arg);
	appendError (U"\n");
}
void Melder_appendError (Melder_19_ARGS) {
	appendError (arg1. _arg);
	appendError (arg2. _arg);
	appendError (arg3. _arg);
	appendError (arg4. _arg);
	appendError (arg5. _arg);
	appendError (arg6. _arg);
	appendError (arg7. _arg);
	appendError (arg8. _arg);
	appendError (arg9. _arg);
	appendError (arg10._arg);
	appendError (arg11._arg);
	appendError (arg12._arg);
	appendError (arg13._arg);
	appendError (arg14._arg);
	appendError (arg15._arg);
	appendError (arg16._arg);
	appendError (arg17._arg);
	appendError (arg18._arg);
	appendError (arg19._arg);
	appendError (U"\n");
}

void Melder_appendError_noLine (Melder_1_ARG) {
	appendError (arg1._arg);
}

void Melder_flushError () {
	/*
		"errors" has to be cleared *before* the message is put on the screen.
		This is because on some platforms the message dialog is synchronous
		(Melder_flushError will wait until the message dialog is closed),
		and some operating systems may force an immediate redraw event as soon as
		the message dialog is closed. We want "errors" to be empty when redrawing!
	*/
	static char32 temp [2000+1];
	str32cpy (temp, errors);
	Melder_clearError ();
	theError (temp);
}

void Melder_flushError (Melder_1_ARG)   { Melder_appendError (Melder_1_ARG_CALL);   Melder_flushError (); }
void Melder_flushError (Melder_2_ARGS)  { Melder_appendError (Melder_2_ARGS_CALL);  Melder_flushError (); }
void Melder_flushError (Melder_3_ARGS)  { Melder_appendError (Melder_3_ARGS_CALL);  Melder_flushError (); }
void Melder_flushError (Melder_4_ARGS)  { Melder_appendError (Melder_4_ARGS_CALL);  Melder_flushError (); }
void Melder_flushError (Melder_5_ARGS)  { Melder_appendError (Melder_5_ARGS_CALL);  Melder_flushError (); }
void Melder_flushError (Melder_6_ARGS)  { Melder_appendError (Melder_6_ARGS_CALL);  Melder_flushError (); }
void Melder_flushError (Melder_7_ARGS)  { Melder_appendError (Melder_7_ARGS_CALL);  Melder_flushError (); }
void Melder_flushError (Melder_8_ARGS)  { Melder_appendError (Melder_8_ARGS_CALL);  Melder_flushError (); }
void Melder_flushError (Melder_9_ARGS)  { Melder_appendError (Melder_9_ARGS_CALL);  Melder_flushError (); }
void Melder_flushError (Melder_10_ARGS) { Melder_appendError (Melder_10_ARGS_CALL); Melder_flushError (); }
void Melder_flushError (Melder_11_ARGS) { Melder_appendError (Melder_11_ARGS_CALL); Melder_flushError (); }
void Melder_flushError (Melder_13_ARGS) { Melder_appendError (Melder_13_ARGS_CALL); Melder_flushError (); }
void Melder_flushError (Melder_15_ARGS) { Melder_appendError (Melder_15_ARGS_CALL); Melder_flushError (); }
void Melder_flushError (Melder_19_ARGS) { Melder_appendError (Melder_19_ARGS_CALL); Melder_flushError (); }

/* End of file melder_error.cpp */
