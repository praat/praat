/* melder_error.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2006/12/08 turned wchar_t
 * pb 2007/05/28 Melder_error1-9
 * pb 2007/06/14 more wchar_t
 */

#include "melder.h"
#include "longchar.h"

#define CONSOLE_SUPPORTS_WCHAR  0

static void defaultError (wchar_t *message) {
	#if CONSOLE_SUPPORTS_WCHAR
		fwprintf (stderr, wcsstr (message, L"You interrupted") ? L"User interrupt: %ls\n" : L"Error: %ls\n", message);
	#else
		fprintf (stderr, wcsstr (message, L"You interrupted") ? "User interrupt: " : "Error: ");
		Melder_fwriteWcsAsUtf8 (message, wcslen (message), stderr);
		fprintf (stderr, "\n");
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
	if (length + messageLength > 2000) return;
	wcscpy (errors + length, message);
	//wcscpy (errors + length + messageLength, L"\n");
}

bool Melder_error1 (const wchar_t *s1) {
	if (s1) appendErrorW (s1);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error2 (const wchar_t *s1, const wchar_t *s2) {
	if (s1) appendErrorW (s1);
	if (s2) appendErrorW (s2);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	if (s1) appendErrorW (s1);
	if (s2) appendErrorW (s2);
	if (s3) appendErrorW (s3);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	if (s1) appendErrorW (s1);
	if (s2) appendErrorW (s2);
	if (s3) appendErrorW (s3);
	if (s4) appendErrorW (s4);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	if (s1) appendErrorW (s1);
	if (s2) appendErrorW (s2);
	if (s3) appendErrorW (s3);
	if (s4) appendErrorW (s4);
	if (s5) appendErrorW (s5);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6)
{
	if (s1) appendErrorW (s1);
	if (s2) appendErrorW (s2);
	if (s3) appendErrorW (s3);
	if (s4) appendErrorW (s4);
	if (s5) appendErrorW (s5);
	if (s6) appendErrorW (s6);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7)
{
	if (s1) appendErrorW (s1);
	if (s2) appendErrorW (s2);
	if (s3) appendErrorW (s3);
	if (s4) appendErrorW (s4);
	if (s5) appendErrorW (s5);
	if (s6) appendErrorW (s6);
	if (s7) appendErrorW (s7);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8)
{
	if (s1) appendErrorW (s1);
	if (s2) appendErrorW (s2);
	if (s3) appendErrorW (s3);
	if (s4) appendErrorW (s4);
	if (s5) appendErrorW (s5);
	if (s6) appendErrorW (s6);
	if (s7) appendErrorW (s7);
	if (s8) appendErrorW (s8);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
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
	appendErrorW (L"\n");
	return false;
}
bool Melder_error10 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10)
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
	if (s10) appendErrorW (s10);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error11 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11)
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
	if (s10) appendErrorW (s10);
	if (s11) appendErrorW (s11);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error12 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12)
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
	if (s10) appendErrorW (s10);
	if (s11) appendErrorW (s11);
	if (s12) appendErrorW (s12);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error13 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13)
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
	if (s10) appendErrorW (s10);
	if (s11) appendErrorW (s11);
	if (s12) appendErrorW (s12);
	if (s13) appendErrorW (s13);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error14 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14)
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
	if (s10) appendErrorW (s10);
	if (s11) appendErrorW (s11);
	if (s12) appendErrorW (s12);
	if (s13) appendErrorW (s13);
	if (s14) appendErrorW (s14);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error15 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15)
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
	if (s10) appendErrorW (s10);
	if (s11) appendErrorW (s11);
	if (s12) appendErrorW (s12);
	if (s13) appendErrorW (s13);
	if (s14) appendErrorW (s14);
	if (s15) appendErrorW (s15);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error16 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16)
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
	if (s10) appendErrorW (s10);
	if (s11) appendErrorW (s11);
	if (s12) appendErrorW (s12);
	if (s13) appendErrorW (s13);
	if (s14) appendErrorW (s14);
	if (s15) appendErrorW (s15);
	if (s16) appendErrorW (s16);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error17 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17)
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
	if (s10) appendErrorW (s10);
	if (s11) appendErrorW (s11);
	if (s12) appendErrorW (s12);
	if (s13) appendErrorW (s13);
	if (s14) appendErrorW (s14);
	if (s15) appendErrorW (s15);
	if (s16) appendErrorW (s16);
	if (s17) appendErrorW (s17);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error18 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17,
	const wchar_t *s18)
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
	if (s10) appendErrorW (s10);
	if (s11) appendErrorW (s11);
	if (s12) appendErrorW (s12);
	if (s13) appendErrorW (s13);
	if (s14) appendErrorW (s14);
	if (s15) appendErrorW (s15);
	if (s16) appendErrorW (s16);
	if (s17) appendErrorW (s17);
	if (s18) appendErrorW (s18);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error19 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
	const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17,
	const wchar_t *s18, const wchar_t *s19)
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
	if (s10) appendErrorW (s10);
	if (s11) appendErrorW (s11);
	if (s12) appendErrorW (s12);
	if (s13) appendErrorW (s13);
	if (s14) appendErrorW (s14);
	if (s15) appendErrorW (s15);
	if (s16) appendErrorW (s16);
	if (s17) appendErrorW (s17);
	if (s18) appendErrorW (s18);
	if (s19) appendErrorW (s19);
	appendErrorW (L"\n");
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
