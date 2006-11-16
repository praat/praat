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
	pb 2006/11/14 extracted from melder.c
*/

#include "melder.h"
#include "longchar.h"

static void defaultErrorA (char *message) {
	fprintf (stderr, strstr (message, "You interrupted") ? "User interrupt: %s\n" : "Error: %s\n", message);
}

static void defaultErrorW (wchar_t *message) {
	fwprintf (stderr, wcsstr (message, L"You interrupted") ? L"User interrupt: %s\n" : L"Error: %s\n", message);
}

static void (*theErrorA) (char *) = defaultErrorA;   // initial setting after start-up; will stay if program is run from batch
static void (*theErrorW) (wchar_t *) = NULL;

void Melder_setErrorProc (void (*error) (char *)) {
	theErrorA = error ? error : defaultErrorA;
}
static void Melder_setErrorProcW (void (*error) (wchar_t *)) {
	theErrorW = error ? error : defaultErrorW;
}

static char errors [2001];   /* Safe in low-memory situations. */

static void appendError (const char *message) {
	int length = strlen (errors), messageLength = strlen (message);
	if (length + messageLength > 2000 - 1) return;   /* 1 == length of "\n" */
	strcpy (errors + length, message);
	strcpy (errors + length + messageLength, "\n");
}

int Melder_error (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vsprintf (Melder_buffer1, format, arg);
	Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
	appendError (Melder_buffer2);
	va_end (arg);
	return 0;
}

void * Melder_errorp (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vsprintf (Melder_buffer1, format, arg);
	Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
	appendError (Melder_buffer2);
	va_end (arg);
	return NULL;
}

int Melder_hasError (void) { return errors [0] != '\0'; }

void Melder_clearError (void) { errors [0] = '\0'; }

char * Melder_getError (void) { return & errors [0]; }

void Melder_flushError (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	if (format) {
		vsprintf (Melder_buffer1, format, arg);
		Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
		appendError (Melder_buffer2);
	}
	/*
		"errors" has to be cleared *before* the message is put on the screen.
		This is because on some platforms the message dialog is synchronous
		(Melder_flushError will wait until the message dialog is closed),
		and some operating systems may force an immediate redraw event as soon as
		the message dialog is closed. We want "errors" to be empty when redrawing!
	*/
	strcpy (Melder_buffer1, errors);
	Melder_clearError ();
	theErrorA (Melder_buffer1);
	va_end (arg);
}

/* End of file melder_error.c */
