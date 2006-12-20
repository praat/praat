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
 * pb 2006/12/17 removed Melder_info and made Melder_print safe
 * pb 2006/12/19 all functions number 1 through 9
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
	MelderStringA_appendA (theInfos, s1);
	MelderStringA_appendA (theInfos, s2);
}
void MelderInfo_write3 (const char *s1, const char *s2, const char *s3) {
	MelderStringA_appendA (theInfos, s1);
	MelderStringA_appendA (theInfos, s2);
	MelderStringA_appendA (theInfos, s3);
}
void MelderInfo_write4 (const char *s1, const char *s2, const char *s3, const char *s4) {
	MelderStringA_appendA (theInfos, s1);
	MelderStringA_appendA (theInfos, s2);
	MelderStringA_appendA (theInfos, s3);
	MelderStringA_appendA (theInfos, s4);
}
void MelderInfo_write5 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5) {
	MelderStringA_appendA (theInfos, s1);
	MelderStringA_appendA (theInfos, s2);
	MelderStringA_appendA (theInfos, s3);
	MelderStringA_appendA (theInfos, s4);
	MelderStringA_appendA (theInfos, s5);
}
void MelderInfo_write6 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5, const char *s6) {
	MelderInfo_write9 (s1, s2, s3, s4, s5, s6, NULL, NULL, NULL);
}
void MelderInfo_write7 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5, const char *s6, const char *s7) {
	MelderInfo_write9 (s1, s2, s3, s4, s5, s6, s7, NULL, NULL);
}
void MelderInfo_write8 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5, const char *s6, const char *s7, const char *s8) {
	MelderInfo_write9 (s1, s2, s3, s4, s5, s6, s7, s8, NULL);
}
void MelderInfo_write9 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5,
	const char *s6, const char *s7, const char *s8, const char *s9)
{
	MelderStringA_appendA (theInfos, s1);
	MelderStringA_appendA (theInfos, s2);
	MelderStringA_appendA (theInfos, s3);
	MelderStringA_appendA (theInfos, s4);
	MelderStringA_appendA (theInfos, s5);
	MelderStringA_appendA (theInfos, s6);
	MelderStringA_appendA (theInfos, s7);
	MelderStringA_appendA (theInfos, s8);
	MelderStringA_appendA (theInfos, s9);
}

void MelderInfo_writeLine1 (const char *s1) {
	MelderStringA_appendA (theInfos, s1);
	MelderStringA_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine2 (const char *s1, const char *s2) {
	MelderStringA_appendA (theInfos, s1);
	MelderStringA_appendA (theInfos, s2);
	MelderStringA_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine3 (const char *s1, const char *s2, const char *s3) {
	MelderStringA_appendA (theInfos, s1);
	MelderStringA_appendA (theInfos, s2);
	MelderStringA_appendA (theInfos, s3);
	MelderStringA_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine4 (const char *s1, const char *s2, const char *s3, const char *s4) {
	MelderStringA_appendA (theInfos, s1);
	MelderStringA_appendA (theInfos, s2);
	MelderStringA_appendA (theInfos, s3);
	MelderStringA_appendA (theInfos, s4);
	MelderStringA_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine5 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5) {
	MelderStringA_appendA (theInfos, s1);
	MelderStringA_appendA (theInfos, s2);
	MelderStringA_appendA (theInfos, s3);
	MelderStringA_appendA (theInfos, s4);
	MelderStringA_appendA (theInfos, s5);
	MelderStringA_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine6 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5, const char *s6) {
	MelderInfo_writeLine9 (s1, s2, s3, s4, s5, s6, NULL, NULL, NULL);
}
void MelderInfo_writeLine7 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5, const char *s6, const char *s7) {
	MelderInfo_writeLine9 (s1, s2, s3, s4, s5, s6, s7, NULL, NULL);
}
void MelderInfo_writeLine8 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5, const char *s6, const char *s7, const char *s8) {
	MelderInfo_writeLine9 (s1, s2, s3, s4, s5, s6, s7, s8, NULL);
}
void MelderInfo_writeLine9 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5,
	const char *s6, const char *s7, const char *s8, const char *s9)
{
	MelderStringA_appendA (theInfos, s1);
	MelderStringA_appendA (theInfos, s2);
	MelderStringA_appendA (theInfos, s3);
	MelderStringA_appendA (theInfos, s4);
	MelderStringA_appendA (theInfos, s5);
	MelderStringA_appendA (theInfos, s6);
	MelderStringA_appendA (theInfos, s7);
	MelderStringA_appendA (theInfos, s8);
	MelderStringA_appendA (theInfos, s9);
	MelderStringA_appendCharacter (theInfos, '\n');
}

void MelderInfo_close (void) {
	if (theInfos == & theForegroundBuffer) {
		/*
			When writing to the Info window or the console, we must add a newline symbol,
			because a subsequent Melder_print call has to start on the next line.
			When writing to a diverted string, we must *not* add a newline symbol,
			because scripts expect returned strings without appended newlines!
		*/
		if (theInfos -> length == 0 || theInfos -> string [theInfos -> length - 1] != '\n') {   // Only if no newline there yet.
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

void Melder_print (const char *s) {
	Melder_assert (theInfos == & theForegroundBuffer);   // Never diverted.
	if (theInformation == defaultInformation) {
		printf ("%s", s);   // Do not print the previous lines again.
	} else {
		MelderStringA_appendA (theInfos, s);
		theInformation (theInfos -> string);
	}
}

void Melder_information1 (const char *s1) {
	MelderStringA_empty (theInfos);
	MelderStringA_appendA (theInfos, s1);
	MelderInfo_close ();
}

void Melder_information2 (const char *s1, const char *s2) {
	MelderStringA_empty (theInfos);
	MelderStringA_appendA (theInfos, s1);
	MelderStringA_appendA (theInfos, s2);
	MelderInfo_close ();
}

void Melder_information3 (const char *s1, const char *s2, const char *s3) {
	Melder_information9 (s1, s2, s3, NULL, NULL, NULL, NULL, NULL, NULL);
}

void Melder_information4 (const char *s1, const char *s2, const char *s3, const char *s4) {
	Melder_information9 (s1, s2, s3, s4, NULL, NULL, NULL, NULL, NULL);
}

void Melder_information5 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5) {
	Melder_information9 (s1, s2, s3, s4, s5, NULL, NULL, NULL, NULL);
}

void Melder_information6 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5, const char *s6) {
	Melder_information9 (s1, s2, s3, s4, s5, s6, NULL, NULL, NULL);
}

void Melder_information7 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5, const char *s6, const char *s7) {
	Melder_information9 (s1, s2, s3, s4, s5, s6, s7, NULL, NULL);
}

void Melder_information8 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5, const char *s6, const char *s7, const char *s8) {
	Melder_information9 (s1, s2, s3, s4, s5, s6, s7, s8, NULL);
}

void Melder_information9 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5,
	const char *s6, const char *s7, const char *s8, const char *s9)
{
	MelderStringA_empty (theInfos);
	if (s1) MelderStringA_appendA (theInfos, s1);   // Checking for a null s1 is an optimization.
	if (s2) MelderStringA_appendA (theInfos, s2);
	if (s3) MelderStringA_appendA (theInfos, s3);
	if (s4) MelderStringA_appendA (theInfos, s4);
	if (s5) MelderStringA_appendA (theInfos, s5);
	if (s6) MelderStringA_appendA (theInfos, s6);
	if (s7) MelderStringA_appendA (theInfos, s7);
	if (s8) MelderStringA_appendA (theInfos, s8);
	if (s9) MelderStringA_appendA (theInfos, s9);
	MelderInfo_close ();
}

/* End of file melder_info.c */
