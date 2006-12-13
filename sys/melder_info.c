/* melder_info.c
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

/* pb 2002/03/07 GPL
 * pb 2006/12/08 separated from melder.c
 */

#include "melder.h"
#include "NUM.h"

static void defaultInformation (char *message) {
	if (message) printf ("%s", message);
}

static void (*theInformation) (char *) = defaultInformation;

void Melder_setInformationProc (void (*information) (char *)) {
	theInformation = information ? information : defaultInformation;
}

static MelderStringA theForegroundBuffer, *theInfos = & theForegroundBuffer;

void MelderInfo_open (void) {
	MelderStringA_empty (theInfos);
}

void MelderInfo_write1 (const char *s1) {
	MelderStringA_appendA (theInfos, s1);
}
void MelderInfo_write2 (const char *s1, const char *s2) {
	MelderInfo_write1 (s1);
	MelderInfo_write1 (s2);
}
void MelderInfo_write3 (const char *s1, const char *s2, const char *s3) {
	MelderInfo_write2 (s1, s2);
	MelderInfo_write1 (s3);
}
void MelderInfo_write4 (const char *s1, const char *s2, const char *s3, const char *s4) {
	MelderInfo_write2 (s1, s2);
	MelderInfo_write2 (s3, s4);
}
void MelderInfo_write5 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5) {
	MelderInfo_write3 (s1, s2, s3);
	MelderInfo_write2 (s4, s5);
}

void MelderInfo_writeLine1 (const char *s1) {
	MelderInfo_write2 (s1, "\n");
}
void MelderInfo_writeLine2 (const char *s1, const char *s2) {
	MelderInfo_write3 (s1, s2, "\n");
}
void MelderInfo_writeLine3 (const char *s1, const char *s2, const char *s3) {
	MelderInfo_write4 (s1, s2, s3, "\n");
}
void MelderInfo_writeLine4 (const char *s1, const char *s2, const char *s3, const char *s4) {
	MelderInfo_write5 (s1, s2, s3, s4, "\n");
}
void MelderInfo_writeLine5 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5) {
	MelderInfo_write4 (s1, s2, s3, s4);
	MelderInfo_write2 (s5, "\n");
}

void MelderInfo_close (void) {
	if (theInfos == & theForegroundBuffer) {
		/*
			When writing to the Info window or the console, we must add a newline symbol,
			because a subsequent Melder_print call has to start on the next line.
			When writing to a diverted string, we must *not* add a newline symbol,
			because scripts expect returned strings without appended newlines!
		*/
		if (theInfos -> length == 0 || theInfos -> string [theInfos -> length - 1] != '\n') {
			MelderStringA_appendCharacter (theInfos, '\n');
		}
		theInformation (theInfos -> string ? theInfos -> string : "");
	}
}

void Melder_informationReal (double value, const char *units) {
	MelderInfo_open ();
	if (value == NUMundefined)
		MelderInfo_write1 ("--undefined--");
	else if (units == NULL)
		MelderInfo_write1 (Melder_double (value));
	else
		MelderInfo_write3 (Melder_double (value), " ", units);
	MelderInfo_close ();
}

void Melder_divertInfo (MelderStringA *buffer) {
	theInfos = buffer == NULL ? & theForegroundBuffer : buffer;
}

void Melder_clearInfo (void) {
	if (theInfos == & theForegroundBuffer) {
		MelderStringA_empty (theInfos);
		theInformation ("");
	}
}

char * Melder_getInfo (void) {
	return theInfos -> string ? theInfos -> string : "";
}

/********** UNSAFE **********/

static char theUnsafeBuffer [30000];

void Melder_information (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	if (strequ (format, "%s")) {
		MelderStringA_copyA (theInfos, va_arg (arg, char *));
	} else {
		vsprintf (theUnsafeBuffer, format, arg);
		MelderStringA_copyA (theInfos, theUnsafeBuffer);
	}
	MelderInfo_close ();
	va_end (arg);
}

void Melder_info (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vsprintf (theUnsafeBuffer, format, arg);
	if (theInfos == & theForegroundBuffer) {
		if (theInformation == defaultInformation) {
			printf ("%s\n", theUnsafeBuffer);   // Do not show the previous lines again.
		} else {
			MelderStringA_appendA (theInfos, theUnsafeBuffer);   // Append...
			MelderStringA_appendCharacter (theInfos, '\n');
			theInformation (theInfos -> string);   // ... and show, including the previous lines.
		}
	} else {
		/*
		 * Diverted.
		 */
		MelderStringA_copyA (theInfos, theUnsafeBuffer);   // Erase, and show, without newline!
	}
	va_end (arg);
}

void Melder_print (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	if (theInformation == defaultInformation) {
		vprintf (format, arg);   /* Do not print the previous lines again. */
	} else {
		if (strequ (format, "%s")) {
			MelderStringA_appendA (theInfos, va_arg (arg, char *));
		} else if (strequ (format, "%s\n")) {
			MelderStringA_appendA (theInfos, va_arg (arg, char *));
			MelderStringA_appendCharacter (theInfos, '\n');
		} else {
			vsprintf (theUnsafeBuffer, format, arg);
			MelderStringA_appendA (theInfos, theUnsafeBuffer);
		}
		theInformation (theInfos -> string);
	}
	va_end (arg);
}

/* End of file melder_info.c */
