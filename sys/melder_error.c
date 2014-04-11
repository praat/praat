/* melder_error.c
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2003/10/02 Melder_flushError: empty "errors" before showing the message
 * pb 2006/11/14 separated from melder.c
 * pb 2006/12/08 turned wchar
 */

#include "melder.h"
#include "longchar.h"

#define CONSOLE_SUPPORTS_WCHAR  0

static void defaultError (wchar_t *message) {
	#if CONSOLE_SUPPORTS_WCHAR
		fwprintf (stderr, wcsstr (message, L"You interrupted") ? L"User interrupt: %s\n" : L"Error: %s\n", message);
	#else
		static char messageA [12000+1];   /* Safe in low-memory situations. */
		int messageLength = wcslen (message);
		for (int i = 0; i <= messageLength; i ++) {
			messageA [i] = message [i];   // BUG: should convert to UTF8
		}
		fprintf (stderr, strstr (messageA, "You interrupted") ? "User interrupt: %s\n" : "Error: %s\n", messageA);
	#endif
}

static void (*theError) (wchar_t *) = defaultError;   // initial setting after start-up; will stay if program is run from batch

void Melder_setErrorProc (void (*error) (wchar_t *)) {
	theError = error ? error : defaultError;
}

static wchar_t errors [2000+1];   /* Safe in low-memory situations. */

static void appendErrorA (const char *message) {
	int length = wcslen (errors), messageLength = strlen (message);
	if (length + messageLength > 2000 - 1) return;   /* 1 == length of "\n" */
	for (long i = 0; i < messageLength; i ++) {
		errors [length + i] = (unsigned char) message [i];
	}
	errors [length + messageLength] = L'\n';
	errors [length + messageLength + 1] = L'\0';
}

int Melder_error (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vsprintf (Melder_buffer1, format, arg);
	appendErrorA (Melder_buffer1);
	va_end (arg);
	return 0;
}

static void appendErrorW (const wchar_t *message) {
	int length = wcslen (errors), messageLength = wcslen (message);
	if (length + messageLength > 2000 - 1) return;   /* 1 == length of "\n" */
	wcscpy (errors + length, message);
	wcscpy (errors + length + messageLength, L"\n");
}

bool Melder_errorW9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9)
{
	if (s1) appendErrorW (s1);
	if (s2) appendErrorW (s2);
	if (s3) appendErrorW (s3);
	if (s4) appendErrorW (s4);
	if (s5) appendErrorW (s5);
	if (s6) appendErrorW (s6);
	if (s7) appendErrorW (s7);
	if (s8) appendErrorW (s8);
	if (s9) appendErrorW (s9);
	return false;
}

void * Melder_errorp (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vsprintf (Melder_buffer1, format, arg);
	appendErrorA (Melder_buffer1);
	va_end (arg);
	return NULL;
}

int Melder_hasError (void) { return errors [0] != L'\0'; }

void Melder_clearError (void) { errors [0] = L'\0'; }

wchar_t * Melder_getErrorW (void) { return & errors [0]; }

char * Melder_getError (void) {
	unsigned long messageLength = wcslen (errors);
	static char errorsA [2000+1];
	for (unsigned long i = 0; i <= messageLength; i ++) {
		errorsA [i] = errors [i];   // no data range checking
	}
	return & errorsA [0];
}

void Melder_flushError (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	if (format) {
		vsprintf (Melder_buffer1, format, arg);
		appendErrorA (Melder_buffer1);
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

/* End of file melder_error.c */
