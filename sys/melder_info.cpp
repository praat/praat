/* melder_info.cpp
 *
 * Copyright (C) 1992-2012,2014,2015,2016,2017 Paul Boersma
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

static void defaultInformation (const char32 *message) {
	Melder_writeToConsole (message, false);
}

static void (*theInformation) (const char32 *) = defaultInformation;

void Melder_setInformationProc (void (*information) (const char32 *)) {
	theInformation = information ? information : defaultInformation;
}

static MelderString theForegroundBuffer = { 0 }, *theInfos = & theForegroundBuffer;

void MelderInfo_open () {
	MelderString_empty (theInfos);
}

void MelderInfo_write (Melder_1_ARG) {
	MelderString_append (theInfos, Melder_1_ARG_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
	}
}
void MelderInfo_write (Melder_2_ARGS) {
	MelderString_append (theInfos, Melder_2_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
	}
}
void MelderInfo_write (Melder_3_ARGS) {
	MelderString_append (theInfos, Melder_3_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
	}
}
void MelderInfo_write (Melder_4_ARGS) {
	MelderString_append (theInfos, Melder_4_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
	}
}
void MelderInfo_write (Melder_5_ARGS) {
	MelderString_append (theInfos, Melder_5_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
	}
}
void MelderInfo_write (Melder_6_ARGS) {
	MelderString_append (theInfos, Melder_6_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
	}
}
void MelderInfo_write (Melder_7_ARGS) {
	MelderString_append (theInfos, Melder_7_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
	}
}
void MelderInfo_write (Melder_8_ARGS) {
	MelderString_append (theInfos, Melder_8_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
	}
}
void MelderInfo_write (Melder_9_ARGS) {
	MelderString_append (theInfos, Melder_9_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
	}
}
void MelderInfo_write (Melder_10_ARGS) {
	MelderString_append (theInfos, Melder_10_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
	}
}
void MelderInfo_write (Melder_11_ARGS) {
	MelderString_append (theInfos, Melder_11_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (arg11._arg, false);
	}
}
void MelderInfo_write (Melder_13_ARGS) {
	MelderString_append (theInfos, Melder_13_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (arg11._arg, false);
		Melder_writeToConsole (arg12._arg, false);
		Melder_writeToConsole (arg13._arg, false);
	}
}
void MelderInfo_write (Melder_15_ARGS) {
	MelderString_append (theInfos, Melder_15_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (arg11._arg, false);
		Melder_writeToConsole (arg12._arg, false);
		Melder_writeToConsole (arg13._arg, false);
		Melder_writeToConsole (arg14._arg, false);
		Melder_writeToConsole (arg15._arg, false);
	}
}
void MelderInfo_write (Melder_19_ARGS) {
	MelderString_append (theInfos, Melder_19_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (arg11._arg, false);
		Melder_writeToConsole (arg12._arg, false);
		Melder_writeToConsole (arg13._arg, false);
		Melder_writeToConsole (arg14._arg, false);
		Melder_writeToConsole (arg15._arg, false);
		Melder_writeToConsole (arg16._arg, false);
		Melder_writeToConsole (arg17._arg, false);
		Melder_writeToConsole (arg18._arg, false);
		Melder_writeToConsole (arg19._arg, false);
	}
}

void MelderInfo_writeLine (Melder_1_ARG) {
	MelderString_append (theInfos, Melder_1_ARG_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_2_ARGS) {
	MelderString_append (theInfos, Melder_2_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_3_ARGS) {
	MelderString_append (theInfos, Melder_3_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_4_ARGS) {
	MelderString_append (theInfos, Melder_4_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_5_ARGS) {
	MelderString_append (theInfos, Melder_5_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_6_ARGS) {
	MelderString_append (theInfos, Melder_6_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_7_ARGS) {
	MelderString_append (theInfos, Melder_7_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_8_ARGS) {
	MelderString_append (theInfos, Melder_8_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_9_ARGS) {
	MelderString_append (theInfos, Melder_9_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_10_ARGS) {
	MelderString_append (theInfos, Melder_10_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_11_ARGS) {
	MelderString_append (theInfos, Melder_11_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (arg11._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_13_ARGS) {
	MelderString_append (theInfos, Melder_13_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (arg11._arg, false);
		Melder_writeToConsole (arg12._arg, false);
		Melder_writeToConsole (arg13._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_15_ARGS) {
	MelderString_append (theInfos, Melder_15_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (arg11._arg, false);
		Melder_writeToConsole (arg12._arg, false);
		Melder_writeToConsole (arg13._arg, false);
		Melder_writeToConsole (arg14._arg, false);
		Melder_writeToConsole (arg15._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}
void MelderInfo_writeLine (Melder_19_ARGS) {
	MelderString_append (theInfos, Melder_19_ARGS_CALL);
	MelderString_appendCharacter (theInfos, U'\n');
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (arg11._arg, false);
		Melder_writeToConsole (arg12._arg, false);
		Melder_writeToConsole (arg13._arg, false);
		Melder_writeToConsole (arg14._arg, false);
		Melder_writeToConsole (arg15._arg, false);
		Melder_writeToConsole (arg16._arg, false);
		Melder_writeToConsole (arg17._arg, false);
		Melder_writeToConsole (arg18._arg, false);
		Melder_writeToConsole (arg19._arg, false);
		Melder_writeToConsole (U"\n", false);
	}
}

void MelderInfo_close () {
	if (theInfos == & theForegroundBuffer) {
		/*
			When writing to the Info window or the console, we must add a newline symbol,
			because a subsequent MelderInfo_write call has to start on the next line.
			When writing to a diverted string, we must *not* add a newline symbol,
			because scripts expect returned strings without appended newlines!
		*/
		if (theInfos -> length == 0 || theInfos -> string [theInfos -> length - 1] != U'\n') {   // only if no newline there yet
			MelderString_appendCharacter (theInfos, U'\n');
			if (theInformation == defaultInformation) {
				Melder_writeToConsole (U"\n", false);
			}
		}
		if (theInformation != defaultInformation) {
			theInformation (theInfos -> string ? theInfos -> string : U"");
		}
	}
}

void MelderInfo_drain () {
	if (theInfos == & theForegroundBuffer) {
		if (theInformation != defaultInformation) {
			theInformation (theInfos -> string ? theInfos -> string : U"");
		}
	}
}

void Melder_informationReal (double value, const char32 *units) {
	MelderInfo_open ();
	if (! units) {
		MelderInfo_write (value);
	} else {
		MelderInfo_write (value, U" ", units);
	}
	MelderInfo_close ();
}

void Melder_divertInfo (MelderString *buffer) {
	theInfos = ( buffer ? buffer : & theForegroundBuffer );
}

void Melder_clearInfo () {
	if (theInfos == & theForegroundBuffer) {
		MelderString_empty (theInfos);
		if (theInformation != defaultInformation) {
			theInformation (U"");
		}
	}
}

const char32 * Melder_getInfo () {
	return theInfos -> string ? theInfos -> string : U"";
}

void Melder_information (Melder_1_ARG) {
	MelderString_copy (theInfos, Melder_1_ARG_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_2_ARGS) {
	MelderString_copy (theInfos, Melder_2_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_3_ARGS) {
	MelderString_copy (theInfos, Melder_3_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_4_ARGS) {
	MelderString_copy (theInfos, Melder_4_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_5_ARGS) {
	MelderString_copy (theInfos, Melder_5_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_6_ARGS) {
	MelderString_copy (theInfos, Melder_6_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_7_ARGS) {
	MelderString_copy (theInfos, Melder_7_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_8_ARGS) {
	MelderString_copy (theInfos, Melder_8_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_9_ARGS) {
	MelderString_copy (theInfos, Melder_9_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_10_ARGS) {
	MelderString_copy (theInfos, Melder_10_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_11_ARGS) {
	MelderString_copy (theInfos, Melder_11_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (arg11._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_13_ARGS) {
	MelderString_copy (theInfos, Melder_13_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (arg11._arg, false);
		Melder_writeToConsole (arg12._arg, false);
		Melder_writeToConsole (arg13._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_15_ARGS) {
	MelderString_copy (theInfos, Melder_15_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (arg11._arg, false);
		Melder_writeToConsole (arg12._arg, false);
		Melder_writeToConsole (arg13._arg, false);
		Melder_writeToConsole (arg14._arg, false);
		Melder_writeToConsole (arg15._arg, false);
	}
	MelderInfo_close ();
}
void Melder_information (Melder_19_ARGS) {
	MelderString_copy (theInfos, Melder_19_ARGS_CALL);
	if (theInformation == defaultInformation && theInfos == & theForegroundBuffer) {
		Melder_writeToConsole (arg1 ._arg, false);
		Melder_writeToConsole (arg2 ._arg, false);
		Melder_writeToConsole (arg3 ._arg, false);
		Melder_writeToConsole (arg4 ._arg, false);
		Melder_writeToConsole (arg5 ._arg, false);
		Melder_writeToConsole (arg6 ._arg, false);
		Melder_writeToConsole (arg7 ._arg, false);
		Melder_writeToConsole (arg8 ._arg, false);
		Melder_writeToConsole (arg9 ._arg, false);
		Melder_writeToConsole (arg10._arg, false);
		Melder_writeToConsole (arg11._arg, false);
		Melder_writeToConsole (arg12._arg, false);
		Melder_writeToConsole (arg13._arg, false);
		Melder_writeToConsole (arg14._arg, false);
		Melder_writeToConsole (arg15._arg, false);
		Melder_writeToConsole (arg16._arg, false);
		Melder_writeToConsole (arg17._arg, false);
		Melder_writeToConsole (arg18._arg, false);
		Melder_writeToConsole (arg19._arg, false);
	}
	MelderInfo_close ();
}

/* End of file melder_info.cpp */
