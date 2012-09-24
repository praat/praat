/* melder_error.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "melder.h"
#include "longchar.h"

static void defaultError (const wchar_t *message) {
	Melder_writeToConsole (wcsstr (message, L"You interrupted ") ? L"User interrupt: " : L"Error: ", true);
	Melder_writeToConsole (message, true);
	Melder_writeToConsole (L"\n", true);
}

static void (*theError) (const wchar_t *) = defaultError;   // initial setting after start-up; will stay if program is run from batch

void Melder_setErrorProc (void (*error) (const wchar_t *)) {
	theError = error ? error : defaultError;
}

static wchar_t errors [2000+1];   // safe in low-memory situations

static void appendErrorA (const char *message) {
	int length = wcslen (errors), messageLength = strlen (message);
	if (length + messageLength > 2000) return;
	for (long i = 0; i < messageLength; i ++) {
		errors [length + i] = (unsigned char) message [i];
	}
	errors [length + messageLength] = L'\0';
}

static void appendErrorALine (const char *message) {
	int length = wcslen (errors), messageLength = strlen (message);
	if (length + messageLength > 2000 - 1) return;   // 1 == length of "\n"
	for (long i = 0; i < messageLength; i ++) {
		errors [length + i] = (unsigned char) message [i];
	}
	errors [length + messageLength] = L'\n';
	errors [length + messageLength + 1] = L'\0';
}

static void appendErrorW (const wchar_t *message) {
	int length = wcslen (errors), messageLength = wcslen (message);
	if (length + messageLength > 2000) return;
	wcscpy (errors + length, message);
}

bool Melder_hasError () { return errors [0] != L'\0'; }

bool Melder_hasError (const wchar_t *partialError) { return wcsstr (errors, partialError) != NULL; }

void Melder_clearError (void) { errors [0] = L'\0'; }

wchar_t * Melder_getError (void) { return & errors [0]; }

void Melder_flushError (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	if (format) {
		vsprintf (Melder_buffer1, format, arg);
		appendErrorALine (Melder_buffer1);
	}
	/*
		"errors" has to be cleared *before* the message is put on the screen.
		This is because on some platforms the message dialog is synchronous
		(Melder_flushError will wait until the message dialog is closed),
		and some operating systems may force an immediate redraw event as soon as
		the message dialog is closed. We want "errors" to be empty when redrawing!
	*/
	static wchar_t temp [2000+1];
	wcscpy (temp, errors);
	Melder_clearError ();
	theError (temp);
	va_end (arg);
}

void Melder_error_ (const MelderArg& arg1)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	if (arg3.argW) { if (arg3.type == 1) appendErrorW (arg3.argW); else appendErrorA (arg3.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	if (arg3.argW) { if (arg3.type == 1) appendErrorW (arg3.argW); else appendErrorA (arg3.arg8); }
	if (arg4.argW) { if (arg4.type == 1) appendErrorW (arg4.argW); else appendErrorA (arg4.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4, const MelderArg& arg5)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	if (arg3.argW) { if (arg3.type == 1) appendErrorW (arg3.argW); else appendErrorA (arg3.arg8); }
	if (arg4.argW) { if (arg4.type == 1) appendErrorW (arg4.argW); else appendErrorA (arg4.arg8); }
	if (arg5.argW) { if (arg5.type == 1) appendErrorW (arg5.argW); else appendErrorA (arg5.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	if (arg3.argW) { if (arg3.type == 1) appendErrorW (arg3.argW); else appendErrorA (arg3.arg8); }
	if (arg4.argW) { if (arg4.type == 1) appendErrorW (arg4.argW); else appendErrorA (arg4.arg8); }
	if (arg5.argW) { if (arg5.type == 1) appendErrorW (arg5.argW); else appendErrorA (arg5.arg8); }
	if (arg6.argW) { if (arg6.type == 1) appendErrorW (arg6.argW); else appendErrorA (arg6.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6, const MelderArg& arg7)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	if (arg3.argW) { if (arg3.type == 1) appendErrorW (arg3.argW); else appendErrorA (arg3.arg8); }
	if (arg4.argW) { if (arg4.type == 1) appendErrorW (arg4.argW); else appendErrorA (arg4.arg8); }
	if (arg5.argW) { if (arg5.type == 1) appendErrorW (arg5.argW); else appendErrorA (arg5.arg8); }
	if (arg6.argW) { if (arg6.type == 1) appendErrorW (arg6.argW); else appendErrorA (arg6.arg8); }
	if (arg7.argW) { if (arg7.type == 1) appendErrorW (arg7.argW); else appendErrorA (arg7.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	if (arg3.argW) { if (arg3.type == 1) appendErrorW (arg3.argW); else appendErrorA (arg3.arg8); }
	if (arg4.argW) { if (arg4.type == 1) appendErrorW (arg4.argW); else appendErrorA (arg4.arg8); }
	if (arg5.argW) { if (arg5.type == 1) appendErrorW (arg5.argW); else appendErrorA (arg5.arg8); }
	if (arg6.argW) { if (arg6.type == 1) appendErrorW (arg6.argW); else appendErrorA (arg6.arg8); }
	if (arg7.argW) { if (arg7.type == 1) appendErrorW (arg7.argW); else appendErrorA (arg7.arg8); }
	if (arg8.argW) { if (arg8.type == 1) appendErrorW (arg8.argW); else appendErrorA (arg8.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8, const MelderArg& arg9)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	if (arg3.argW) { if (arg3.type == 1) appendErrorW (arg3.argW); else appendErrorA (arg3.arg8); }
	if (arg4.argW) { if (arg4.type == 1) appendErrorW (arg4.argW); else appendErrorA (arg4.arg8); }
	if (arg5.argW) { if (arg5.type == 1) appendErrorW (arg5.argW); else appendErrorA (arg5.arg8); }
	if (arg6.argW) { if (arg6.type == 1) appendErrorW (arg6.argW); else appendErrorA (arg6.arg8); }
	if (arg7.argW) { if (arg7.type == 1) appendErrorW (arg7.argW); else appendErrorA (arg7.arg8); }
	if (arg8.argW) { if (arg8.type == 1) appendErrorW (arg8.argW); else appendErrorA (arg8.arg8); }
	if (arg9.argW) { if (arg9.type == 1) appendErrorW (arg9.argW); else appendErrorA (arg9.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8,
	const MelderArg& arg9, const MelderArg& arg10)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	if (arg3.argW) { if (arg3.type == 1) appendErrorW (arg3.argW); else appendErrorA (arg3.arg8); }
	if (arg4.argW) { if (arg4.type == 1) appendErrorW (arg4.argW); else appendErrorA (arg4.arg8); }
	if (arg5.argW) { if (arg5.type == 1) appendErrorW (arg5.argW); else appendErrorA (arg5.arg8); }
	if (arg6.argW) { if (arg6.type == 1) appendErrorW (arg6.argW); else appendErrorA (arg6.arg8); }
	if (arg7.argW) { if (arg7.type == 1) appendErrorW (arg7.argW); else appendErrorA (arg7.arg8); }
	if (arg8.argW) { if (arg8.type == 1) appendErrorW (arg8.argW); else appendErrorA (arg8.arg8); }
	if (arg9.argW) { if (arg9.type == 1) appendErrorW (arg9.argW); else appendErrorA (arg9.arg8); }
	if (arg10.argW) { if (arg10.type == 1) appendErrorW (arg10.argW); else appendErrorA (arg10.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8,
	const MelderArg& arg9, const MelderArg& arg10, const MelderArg& arg11)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	if (arg3.argW) { if (arg3.type == 1) appendErrorW (arg3.argW); else appendErrorA (arg3.arg8); }
	if (arg4.argW) { if (arg4.type == 1) appendErrorW (arg4.argW); else appendErrorA (arg4.arg8); }
	if (arg5.argW) { if (arg5.type == 1) appendErrorW (arg5.argW); else appendErrorA (arg5.arg8); }
	if (arg6.argW) { if (arg6.type == 1) appendErrorW (arg6.argW); else appendErrorA (arg6.arg8); }
	if (arg7.argW) { if (arg7.type == 1) appendErrorW (arg7.argW); else appendErrorA (arg7.arg8); }
	if (arg8.argW) { if (arg8.type == 1) appendErrorW (arg8.argW); else appendErrorA (arg8.arg8); }
	if (arg9.argW) { if (arg9.type == 1) appendErrorW (arg9.argW); else appendErrorA (arg9.arg8); }
	if (arg10.argW) { if (arg10.type == 1) appendErrorW (arg10.argW); else appendErrorA (arg10.arg8); }
	if (arg11.argW) { if (arg11.type == 1) appendErrorW (arg11.argW); else appendErrorA (arg11.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8,
	const MelderArg& arg9, const MelderArg& arg10, const MelderArg& arg11, const MelderArg& arg12, const MelderArg& arg13)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	if (arg3.argW) { if (arg3.type == 1) appendErrorW (arg3.argW); else appendErrorA (arg3.arg8); }
	if (arg4.argW) { if (arg4.type == 1) appendErrorW (arg4.argW); else appendErrorA (arg4.arg8); }
	if (arg5.argW) { if (arg5.type == 1) appendErrorW (arg5.argW); else appendErrorA (arg5.arg8); }
	if (arg6.argW) { if (arg6.type == 1) appendErrorW (arg6.argW); else appendErrorA (arg6.arg8); }
	if (arg7.argW) { if (arg7.type == 1) appendErrorW (arg7.argW); else appendErrorA (arg7.arg8); }
	if (arg8.argW) { if (arg8.type == 1) appendErrorW (arg8.argW); else appendErrorA (arg8.arg8); }
	if (arg9.argW) { if (arg9.type == 1) appendErrorW (arg9.argW); else appendErrorA (arg9.arg8); }
	if (arg10.argW) { if (arg10.type == 1) appendErrorW (arg10.argW); else appendErrorA (arg10.arg8); }
	if (arg11.argW) { if (arg11.type == 1) appendErrorW (arg11.argW); else appendErrorA (arg11.arg8); }
	if (arg12.argW) { if (arg12.type == 1) appendErrorW (arg12.argW); else appendErrorA (arg12.arg8); }
	if (arg13.argW) { if (arg13.type == 1) appendErrorW (arg13.argW); else appendErrorA (arg13.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8,
	const MelderArg& arg9, const MelderArg& arg10, const MelderArg& arg11, const MelderArg& arg12,
	const MelderArg& arg13, const MelderArg& arg14, const MelderArg& arg15)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	if (arg3.argW) { if (arg3.type == 1) appendErrorW (arg3.argW); else appendErrorA (arg3.arg8); }
	if (arg4.argW) { if (arg4.type == 1) appendErrorW (arg4.argW); else appendErrorA (arg4.arg8); }
	if (arg5.argW) { if (arg5.type == 1) appendErrorW (arg5.argW); else appendErrorA (arg5.arg8); }
	if (arg6.argW) { if (arg6.type == 1) appendErrorW (arg6.argW); else appendErrorA (arg6.arg8); }
	if (arg7.argW) { if (arg7.type == 1) appendErrorW (arg7.argW); else appendErrorA (arg7.arg8); }
	if (arg8.argW) { if (arg8.type == 1) appendErrorW (arg8.argW); else appendErrorA (arg8.arg8); }
	if (arg9.argW) { if (arg9.type == 1) appendErrorW (arg9.argW); else appendErrorA (arg9.arg8); }
	if (arg10.argW) { if (arg10.type == 1) appendErrorW (arg10.argW); else appendErrorA (arg10.arg8); }
	if (arg11.argW) { if (arg11.type == 1) appendErrorW (arg11.argW); else appendErrorA (arg11.arg8); }
	if (arg12.argW) { if (arg12.type == 1) appendErrorW (arg12.argW); else appendErrorA (arg12.arg8); }
	if (arg13.argW) { if (arg13.type == 1) appendErrorW (arg13.argW); else appendErrorA (arg13.arg8); }
	if (arg14.argW) { if (arg14.type == 1) appendErrorW (arg14.argW); else appendErrorA (arg14.arg8); }
	if (arg15.argW) { if (arg15.type == 1) appendErrorW (arg15.argW); else appendErrorA (arg15.arg8); }
	appendErrorW (L"\n");
}

void Melder_error_ (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8,
	const MelderArg& arg9, const MelderArg& arg10, const MelderArg& arg11, const MelderArg& arg12,
	const MelderArg& arg13, const MelderArg& arg14, const MelderArg& arg15, const MelderArg& arg16,
	const MelderArg& arg17, const MelderArg& arg18, const MelderArg& arg19, const MelderArg& arg20)
{
	if (arg1.argW) { if (arg1.type == 1) appendErrorW (arg1.argW); else appendErrorA (arg1.arg8); }
	if (arg2.argW) { if (arg2.type == 1) appendErrorW (arg2.argW); else appendErrorA (arg2.arg8); }
	if (arg3.argW) { if (arg3.type == 1) appendErrorW (arg3.argW); else appendErrorA (arg3.arg8); }
	if (arg4.argW) { if (arg4.type == 1) appendErrorW (arg4.argW); else appendErrorA (arg4.arg8); }
	if (arg5.argW) { if (arg5.type == 1) appendErrorW (arg5.argW); else appendErrorA (arg5.arg8); }
	if (arg6.argW) { if (arg6.type == 1) appendErrorW (arg6.argW); else appendErrorA (arg6.arg8); }
	if (arg7.argW) { if (arg7.type == 1) appendErrorW (arg7.argW); else appendErrorA (arg7.arg8); }
	if (arg8.argW) { if (arg8.type == 1) appendErrorW (arg8.argW); else appendErrorA (arg8.arg8); }
	if (arg9.argW) { if (arg9.type == 1) appendErrorW (arg9.argW); else appendErrorA (arg9.arg8); }
	if (arg10.argW) { if (arg10.type == 1) appendErrorW (arg10.argW); else appendErrorA (arg10.arg8); }
	if (arg11.argW) { if (arg11.type == 1) appendErrorW (arg11.argW); else appendErrorA (arg11.arg8); }
	if (arg12.argW) { if (arg12.type == 1) appendErrorW (arg12.argW); else appendErrorA (arg12.arg8); }
	if (arg13.argW) { if (arg13.type == 1) appendErrorW (arg13.argW); else appendErrorA (arg13.arg8); }
	if (arg14.argW) { if (arg14.type == 1) appendErrorW (arg14.argW); else appendErrorA (arg14.arg8); }
	if (arg15.argW) { if (arg15.type == 1) appendErrorW (arg15.argW); else appendErrorA (arg15.arg8); }
	if (arg16.argW) { if (arg16.type == 1) appendErrorW (arg16.argW); else appendErrorA (arg16.arg8); }
	if (arg17.argW) { if (arg17.type == 1) appendErrorW (arg17.argW); else appendErrorA (arg17.arg8); }
	if (arg18.argW) { if (arg18.type == 1) appendErrorW (arg18.argW); else appendErrorA (arg18.arg8); }
	if (arg19.argW) { if (arg19.type == 1) appendErrorW (arg19.argW); else appendErrorA (arg19.arg8); }
	if (arg20.argW) { if (arg20.type == 1) appendErrorW (arg20.argW); else appendErrorA (arg20.arg8); }
	appendErrorW (L"\n");
}

/* End of file melder_error.cpp */
