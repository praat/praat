/* melder_info.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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
#include "NUM.h"   // NUMundefined

static void defaultInformation (const wchar_t *message) {
	Melder_writeToConsole (message, false);
}

static void (*theInformation) (const wchar_t *) = defaultInformation;

void Melder_setInformationProc (void (*information) (const wchar_t *)) {
	theInformation = information ? information : defaultInformation;
}

static MelderString theForegroundBuffer = { 0 }, *theInfos = & theForegroundBuffer;

void MelderInfo_open (void) {
	MelderString_empty (theInfos);
}

void MelderInfo_write (const wchar_t *s1) {
	MelderString_append (theInfos, s1);
}
void MelderInfo_write (const wchar_t *s1, const wchar_t *s2) {
	MelderString_append (theInfos, s1, s2);
}
void MelderInfo_write (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	MelderString_append (theInfos, s1, s2, s3);
}
void MelderInfo_write (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	MelderString_append (theInfos, s1, s2, s3, s4);
}
void MelderInfo_write (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	MelderString_append (theInfos, s1, s2, s3, s4, s5);
}
void MelderInfo_write (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	MelderString_append (theInfos, s1, s2, s3, s4, s5, s6);
}
void MelderInfo_write (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	MelderString_append (theInfos, s1, s2, s3, s4, s5, s6, s7);
}
void MelderInfo_write (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	MelderString_append (theInfos, s1, s2, s3, s4, s5, s6, s7, s8);
}
void MelderInfo_write (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	MelderString_append (theInfos, s1, s2, s3, s4, s5, s6, s7, s8, s9);
}

void MelderInfo_writeLine (const wchar_t *s1) {
	MelderString_append (theInfos, s1);
	MelderString_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine (const wchar_t *s1, const wchar_t *s2) {
	MelderString_append (theInfos, s1, s2);
	MelderString_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	MelderString_append (theInfos, s1, s2, s3);
	MelderString_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	MelderString_append (theInfos, s1, s2, s3, s4);
	MelderString_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	MelderString_append (theInfos, s1, s2, s3, s4, s5);
	MelderString_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	MelderString_append (theInfos, s1, s2, s3, s4, s5, s6);
	MelderString_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	MelderString_append (theInfos, s1, s2, s3, s4, s5, s6, s7);
	MelderString_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	MelderString_append (theInfos, s1, s2, s3, s4, s5, s6, s7, s8);
	MelderString_appendCharacter (theInfos, '\n');
}
void MelderInfo_writeLine (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	MelderString_append (theInfos, s1, s2, s3, s4, s5, s6, s7, s8, s9);
	MelderString_appendCharacter (theInfos, '\n');
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
			MelderString_appendCharacter (theInfos, '\n');
		}
		theInformation (theInfos -> string ? theInfos -> string : L"");
	}
}

void MelderInfo_drain (void) {
	if (theInfos == & theForegroundBuffer) {
		theInformation (theInfos -> string ? theInfos -> string : L"");
	}
}

void Melder_informationReal (double value, const wchar_t *units) {
	MelderInfo_open ();
	if (value == NUMundefined)
		MelderInfo_write (L"--undefined--");
	else if (units == NULL)
		MelderInfo_write (Melder_double (value));
	else
		MelderInfo_write (Melder_double (value), L" ", units);
	MelderInfo_close ();
}

void Melder_divertInfo (MelderString *buffer) {
	theInfos = buffer == NULL ? & theForegroundBuffer : buffer;
}

void Melder_clearInfo (void) {
	if (theInfos == & theForegroundBuffer) {
		MelderString_empty (theInfos);
		theInformation (L"");
	}
}

const wchar_t * Melder_getInfo (void) {
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
		theInformation ((wchar_t *) s);   // Do not print the previous lines again.
	} else {
		MelderString_append (& theForegroundBuffer, s);
		theInformation (theForegroundBuffer. string);
	}
}

void Melder_information (const wchar_t *s1) {
	MelderString_empty (theInfos);
	MelderString_append (theInfos, s1);
	MelderInfo_close ();
}

void Melder_information (const wchar_t *s1, const wchar_t *s2) {
	MelderString_empty (theInfos);
	MelderString_append (theInfos, s1, s2);
	MelderInfo_close ();
}

void Melder_information (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	MelderString_empty (theInfos);
	MelderString_append (theInfos, s1, s2, s3);
	MelderInfo_close ();
}

void Melder_information (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	MelderString_empty (theInfos);
	MelderString_append (theInfos, s1, s2, s3, s4);
	MelderInfo_close ();
}

void Melder_information (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	MelderString_empty (theInfos);
	MelderString_append (theInfos, s1, s2, s3, s4, s5);
	MelderInfo_close ();
}

void Melder_information (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	MelderString_empty (theInfos);
	MelderString_append (theInfos, s1, s2, s3, s4, s5, s6);
	MelderInfo_close ();
}

void Melder_information (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	MelderString_empty (theInfos);
	MelderString_append (theInfos, s1, s2, s3, s4, s5, s6, s7);
	MelderInfo_close ();
}

void Melder_information (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	MelderString_empty (theInfos);
	MelderString_append (theInfos, s1, s2, s3, s4, s5, s6, s7, s8);
	MelderInfo_close ();
}

void Melder_information (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	MelderString_empty (theInfos);
	MelderString_append (theInfos, s1, s2, s3, s4, s5, s6, s7, s8, s9);
	MelderInfo_close ();
}

/* End of file melder_info.cpp */
