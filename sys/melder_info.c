/* melder_info.c
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

/* pb 2002/03/07 GPL
 * pb 2006/12/08 separated from melder.c
 * pb 2006/12/17 removed Melder_info and made Melder_print safe
 * pb 2006/12/19 all functions number 1 through 9
 * pb 2007/06/11 wchar_t
 * pb 2007/08/14 faster
 */

#include "melder.h"
#include "NUM.h"

static void defaultInformation (wchar_t *message) {
	if (message) {
		static MelderStringA messageA = { 0 };
		MelderStringA_copyW (& messageA, message);
		printf ("%s", messageA.string);
	}
}

static void (*theInformation) (wchar_t *) = defaultInformation;

void Melder_setInformationProc (void (*information) (wchar_t *)) {
	theInformation = information ? information : defaultInformation;
}

static MelderStringW theForegroundBuffer = { 0 }, *theInfos = & theForegroundBuffer;

void MelderInfo_open (void) {
	MelderStringW_empty (theInfos);
}

void MelderInfo_write1 (const wchar_t *s1) {
	MelderStringW_append1 (theInfos, s1);
}
void MelderInfo_write2 (const wchar_t *s1, const wchar_t *s2) {
	MelderStringW_append2 (theInfos, s1, s2);
}
void MelderInfo_write3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	MelderStringW_append3 (theInfos, s1, s2, s3);
}
void MelderInfo_write4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	MelderStringW_append4 (theInfos, s1, s2, s3, s4);
}
void MelderInfo_write5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	MelderStringW_append5 (theInfos, s1, s2, s3, s4, s5);
}
void MelderInfo_write6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	MelderStringW_append6 (theInfos, s1, s2, s3, s4, s5, s6);
}
void MelderInfo_write7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	MelderStringW_append7 (theInfos, s1, s2, s3, s4, s5, s6, s7);
}
void MelderInfo_write8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	MelderStringW_append8 (theInfos, s1, s2, s3, s4, s5, s6, s7, s8);
}
void MelderInfo_write9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	MelderStringW_append9 (theInfos, s1, s2, s3, s4, s5, s6, s7, s8, s9);
}

void MelderInfo_writeLine1 (const wchar_t *s1) {
	MelderStringW_append1 (theInfos, s1);
	MelderStringW_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine2 (const wchar_t *s1, const wchar_t *s2) {
	MelderStringW_append2 (theInfos, s1, s2);
	MelderStringW_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	MelderStringW_append3 (theInfos, s1, s2, s3);
	MelderStringW_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	MelderStringW_append4 (theInfos, s1, s2, s3, s4);
	MelderStringW_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	MelderStringW_append5 (theInfos, s1, s2, s3, s4, s5);
	MelderStringW_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	MelderStringW_append6 (theInfos, s1, s2, s3, s4, s5, s6);
	MelderStringW_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	MelderStringW_append7 (theInfos, s1, s2, s3, s4, s5, s6, s7);
	MelderStringW_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	MelderStringW_append8 (theInfos, s1, s2, s3, s4, s5, s6, s7, s8);
	MelderStringW_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	MelderStringW_append9 (theInfos, s1, s2, s3, s4, s5, s6, s7, s8, s9);
	MelderStringW_appendCharacter (theInfos, '\n');
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
			MelderStringW_appendCharacter (theInfos, '\n');
		}
		theInformation (theInfos -> string ? theInfos -> string : L"");
	}
}

void Melder_informationReal (double value, const wchar_t *units) {
	MelderInfo_open ();
	if (value == NUMundefined)
		MelderInfo_write1 (L"--undefined--");
	else if (units == NULL)
		MelderInfo_write1 (Melder_double (value));
	else
		MelderInfo_write3 (Melder_double (value), L" ", units);
	MelderInfo_close ();
}

void Melder_divertInfo (MelderStringW *buffer) {
	theInfos = buffer == NULL ? & theForegroundBuffer : buffer;
}

void Melder_clearInfo (void) {
	if (theInfos == & theForegroundBuffer) {
		MelderStringW_empty (theInfos);
		theInformation (L"");
	}
}

wchar_t * Melder_getInfo (void) {
	return theInfos -> string ? theInfos -> string : L"";
}

void Melder_print (const wchar_t *s) {
	//Melder_assert (theInfos == & theForegroundBuffer);   // Never diverted.
	/*
	 * This procedure is always called from a script; therefore, this is unlikely to be called when the info is diverted.
	 * Unlikely, but not impossible!
	 * The small possibility occurs when the script, having diverted the info through an assignment command,
	 * causes the progress bar to move. If the user chooses Run while the progress bar moves,
	 * the script will start to run and may call print:
	 *    pitch = To Pitch... 0 75 600
	 * Therefore, we write into the Info window explicitly. The results will still be strange,
	 * and a better solution would be to disallow the script from running (BUG: accept fewer events in waitWhileProgress).
	 */
	if (theInformation == defaultInformation) {
		printf ("%s", Melder_peekWcsToAscii (s));   // Do not print the previous lines again.
	} else {
		MelderStringW_appendW (& theForegroundBuffer, s);
		theInformation (theForegroundBuffer. string);
	}
}

void Melder_information1 (const wchar_t *s1) {
	MelderStringW_empty (theInfos);
	MelderStringW_append1 (theInfos, s1);
	MelderInfo_close ();
}

void Melder_information2 (const wchar_t *s1, const wchar_t *s2) {
	MelderStringW_empty (theInfos);
	MelderStringW_append2 (theInfos, s1, s2);
	MelderInfo_close ();
}

void Melder_information3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	MelderStringW_empty (theInfos);
	MelderStringW_append3 (theInfos, s1, s2, s3);
	MelderInfo_close ();
}

void Melder_information4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	MelderStringW_empty (theInfos);
	MelderStringW_append4 (theInfos, s1, s2, s3, s4);
	MelderInfo_close ();
}

void Melder_information5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	MelderStringW_empty (theInfos);
	MelderStringW_append5 (theInfos, s1, s2, s3, s4, s5);
	MelderInfo_close ();
}

void Melder_information6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	MelderStringW_empty (theInfos);
	MelderStringW_append6 (theInfos, s1, s2, s3, s4, s5, s6);
	MelderInfo_close ();
}

void Melder_information7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	MelderStringW_empty (theInfos);
	MelderStringW_append7 (theInfos, s1, s2, s3, s4, s5, s6, s7);
	MelderInfo_close ();
}

void Melder_information8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	MelderStringW_empty (theInfos);
	MelderStringW_append8 (theInfos, s1, s2, s3, s4, s5, s6, s7, s8);
	MelderInfo_close ();
}

void Melder_information9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	MelderStringW_empty (theInfos);
	MelderStringW_append9 (theInfos, s1, s2, s3, s4, s5, s6, s7, s8, s9);
	MelderInfo_close ();
}

/* End of file melder_info.c */
