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

/*
 * pb 2002/03/07 GPL
 * pb 2003/10/02 Melder_flushError: empty "errors" before showing the message
 * pb 2006/11/14 separated from melder.c
 * pb 2006/12/08 turned wchar
 * pb 2007/05/28 Melder_error1-9
 * pb 2007/06/14 more wchar
 * pb 2007/12/13 Melder_writeToConsole
 * pb 2011/03/05 C++
 */

#include "melder.h"
#include "longchar.h"

static void defaultError (const wchar *message) {
	Melder_writeToConsole (wcsstr (message, L"You interrupted ") ? L"User interrupt: " : L"Error: ", true);
	Melder_writeToConsole (message, true);
	Melder_writeToConsole (L"\n", true);
}

static void (*theError) (const wchar *) = defaultError;   // initial setting after start-up; will stay if program is run from batch

void Melder_setErrorProc (void (*error) (const wchar *)) {
	theError = error ? error : defaultError;
}

static wchar errors [2000+1];   /* Safe in low-memory situations. */

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
	appendErrorALine (Melder_buffer1);
	va_end (arg);
	return 0;
}

static void appendErrorW (const wchar *message) {
	int length = wcslen (errors), messageLength = wcslen (message);
	if (length + messageLength > 2000) return;
	wcscpy (errors + length, message);
}

bool Melder_error1 (const wchar *s1) {
	if (s1) appendErrorW (s1);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error2 (const wchar *s1, const wchar *s2) {
	if (s1) appendErrorW (s1);
	if (s2) appendErrorW (s2);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error3 (const wchar *s1, const wchar *s2, const wchar *s3) {
	if (s1) appendErrorW (s1);
	if (s2) appendErrorW (s2);
	if (s3) appendErrorW (s3);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error4 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4) {
	if (s1) appendErrorW (s1);
	if (s2) appendErrorW (s2);
	if (s3) appendErrorW (s3);
	if (s4) appendErrorW (s4);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error5 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5) {
	if (s1) appendErrorW (s1);
	if (s2) appendErrorW (s2);
	if (s3) appendErrorW (s3);
	if (s4) appendErrorW (s4);
	if (s5) appendErrorW (s5);
	appendErrorW (L"\n");
	return false;
}
bool Melder_error6 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6)
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
bool Melder_error7 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7)
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
bool Melder_error8 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7, const wchar *s8)
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
bool Melder_error9 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9)
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
bool Melder_error10 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10)
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
bool Melder_error11 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11)
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
bool Melder_error12 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11,
	const wchar *s12)
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
bool Melder_error13 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11,
	const wchar *s12, const wchar *s13)
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
bool Melder_error14 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11,
	const wchar *s12, const wchar *s13, const wchar *s14)
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
bool Melder_error15 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11,
	const wchar *s12, const wchar *s13, const wchar *s14, const wchar *s15)
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
bool Melder_error16 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11,
	const wchar *s12, const wchar *s13, const wchar *s14, const wchar *s15, const wchar *s16)
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
bool Melder_error17 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11,
	const wchar *s12, const wchar *s13, const wchar *s14, const wchar *s15, const wchar *s16, const wchar *s17)
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
bool Melder_error18 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11,
	const wchar *s12, const wchar *s13, const wchar *s14, const wchar *s15, const wchar *s16, const wchar *s17,
	const wchar *s18)
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
bool Melder_error19 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5,
	const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11,
	const wchar *s12, const wchar *s13, const wchar *s14, const wchar *s15, const wchar *s16, const wchar *s17,
	const wchar *s18, const wchar *s19)
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
void * Melder_errorp1 (const wchar *s1) { Melder_error1 (s1); return NULL; }
void * Melder_errorp2 (const wchar *s1, const wchar *s2) { Melder_error2 (s1,s2); return NULL; }
void * Melder_errorp3 (const wchar *s1, const wchar *s2, const wchar *s3) { Melder_error3 (s1,s2,s3); return NULL; }
void * Melder_errorp4 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4) { Melder_error4 (s1,s2,s3,s4); return NULL; }
void * Melder_errorp5 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5) { Melder_error5 (s1,s2,s3,s4,s5); return NULL; }
void * Melder_errorp6 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6) { Melder_error6 (s1,s2,s3,s4,s5,s6); return NULL; }
void * Melder_errorp7 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7) { Melder_error7 (s1,s2,s3,s4,s5,s6,s7); return NULL; }
void * Melder_errorp8 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7, const wchar *s8) { Melder_error8 (s1,s2,s3,s4,s5,s6,s7,s8); return NULL; }
void * Melder_errorp9 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9) { Melder_error9 (s1,s2,s3,s4,s5,s6,s7,s8,s9); return NULL; }
void * Melder_errorp10 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10) { Melder_error10 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10); return NULL; }
void * Melder_errorp11 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11) { Melder_error11 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11); return NULL; }
void * Melder_errorp12 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11, const wchar *s12) { Melder_error12 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12); return NULL; }
void * Melder_errorp13 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11, const wchar *s12, const wchar *s13) { Melder_error13 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13); return NULL; }
void * Melder_errorp14 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11, const wchar *s12, const wchar *s13, const wchar *s14) { Melder_error14 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14); return NULL; }
void * Melder_errorp15 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11, const wchar *s12, const wchar *s13, const wchar *s14, const wchar *s15) { Melder_error15 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15); return NULL; }
void * Melder_errorp16 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11, const wchar *s12, const wchar *s13, const wchar *s14, const wchar *s15, const wchar *s16) { Melder_error16 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16); return NULL; }
void * Melder_errorp17 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11, const wchar *s12, const wchar *s13, const wchar *s14, const wchar *s15, const wchar *s16, const wchar *s17) { Melder_error17 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,s17); return NULL; }
void * Melder_errorp18 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11, const wchar *s12, const wchar *s13, const wchar *s14, const wchar *s15, const wchar *s16, const wchar *s17, const wchar *s18) { Melder_error18 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,s17,s18); return NULL; }
void * Melder_errorp19 (const wchar *s1, const wchar *s2, const wchar *s3, const wchar *s4, const wchar *s5, const wchar *s6, const wchar *s7, const wchar *s8, const wchar *s9, const wchar *s10, const wchar *s11, const wchar *s12, const wchar *s13, const wchar *s14, const wchar *s15, const wchar *s16, const wchar *s17, const wchar *s18, const wchar *s19) { Melder_error19 (s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,s17,s18,s19); return NULL; }

void * Melder_errorp (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vsprintf (Melder_buffer1, format, arg);
	appendErrorALine (Melder_buffer1);
	va_end (arg);
	return NULL;
}

int Melder_hasError (void) { return errors [0] != L'\0'; }

void Melder_clearError (void) { errors [0] = L'\0'; }

wchar * Melder_getError (void) { return & errors [0]; }

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
	static wchar temp [2000+1];
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
