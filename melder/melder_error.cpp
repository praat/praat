/* melder_error.cpp
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

static void defaultError (conststring32 message) {
	MelderConsole::write (str32str (message, U"You interrupted ") ? U"User interrupt: " : U"Error: ", true);
	MelderConsole::write (message, true);
	MelderConsole::write (U"\n", true);
}

static void (*p_theErrorProc) (conststring32) = & defaultError;   // initial setting after start-up; will stay if program is run from batch

void Melder_setErrorProc (void (*p_errorProc) (conststring32)) {
	p_theErrorProc = p_errorProc ? p_errorProc : & defaultError;
}

constexpr integer BUFFER_LENGTH = 2000;

static char32 buffer [BUFFER_LENGTH];   // safe in low-memory situations

void MelderError::_append (conststring32 message) {
	if (! message)
		return;
	integer length = str32len (buffer), messageLength = str32len (message);
	if (length + messageLength >= BUFFER_LENGTH)
		return;
	str32cpy (buffer + length, message);
}

bool Melder_hasError () {
	return buffer [0] != U'\0';
}

bool Melder_hasError (conststring32 partialError) {
	return !! str32str (buffer, partialError);
}

void Melder_clearError () {
	buffer [0] = U'\0';
}

conststring32 Melder_getError () {
	return & buffer [0];
}

void Melder_appendError_noLine (const MelderArg& arg) {
	MelderError::_append (arg._arg);
}

void Melder_flushError () {
	/*
		"errors" has to be cleared *before* the message is put on the screen.
		This is because on some platforms the message dialog is synchronous
		(Melder_flushError will wait until the message dialog is closed),
		and some operating systems may force an immediate redraw event as soon as
		the message dialog is closed. We want "errors" to be empty when redrawing!
	*/
	static char32 temp [BUFFER_LENGTH];
	str32cpy (temp, buffer);
	Melder_clearError ();
	(*p_theErrorProc) (temp);
}

/* End of file melder_error.cpp */
