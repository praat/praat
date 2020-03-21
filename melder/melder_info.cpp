/* melder_info.cpp
 *
 * Copyright (C) 1992-2007,2011-2018,2020 Paul Boersma
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

void MelderInfo::_defaultProc (conststring32 message) {
	MelderConsole::write (message, false);
}

MelderInfo::Proc MelderInfo::_p_currentProc = & MelderInfo::_defaultProc;

void Melder_setInformationProc (MelderInfo::Proc proc) {
	MelderInfo::_p_currentProc = ( proc ? proc : MelderInfo::_defaultProc );
}

MelderString MelderInfo::_foregroundBuffer;
MelderString *MelderInfo::_p_currentBuffer = & MelderInfo::_foregroundBuffer;

void MelderInfo_open () {
	MelderString_empty (MelderInfo::_p_currentBuffer);
}

void MelderInfo_close () {
	if (MelderInfo::_p_currentBuffer == & MelderInfo::_foregroundBuffer) {
		/*
			When writing to the Info window or the console, we must add a newline symbol,
			because a subsequent MelderInfo_write call has to start on the next line.
			When writing to a diverted string, we must *not* add a newline symbol,
			because scripts expect returned strings without appended newlines!
		*/
		if (MelderInfo::_p_currentBuffer -> length == 0 ||
		    MelderInfo::_p_currentBuffer -> string [MelderInfo::_p_currentBuffer -> length - 1] != U'\n')   // only if no newline there yet
		{
			MelderString_appendCharacter (MelderInfo::_p_currentBuffer, U'\n');
			if (MelderInfo::_p_currentProc == MelderInfo::_defaultProc)
				MelderConsole::write (U"\n", false);
		}
		if (MelderInfo::_p_currentProc != & MelderInfo::_defaultProc)
			MelderInfo::_p_currentProc (MelderInfo::_p_currentBuffer -> string ? MelderInfo::_p_currentBuffer -> string : U"");
	}
}

void MelderInfo_drain () {
	if (MelderInfo::_p_currentBuffer == & MelderInfo::_foregroundBuffer) {
		if (MelderInfo::_p_currentProc != & MelderInfo::_defaultProc)
			MelderInfo::_p_currentProc (MelderInfo::_p_currentBuffer -> string ? MelderInfo::_p_currentBuffer -> string : U"");
	}
}

void Melder_informationReal (double value, conststring32 units) {
	MelderInfo_open ();
	if (! units) {
		MelderInfo_write (value);
	} else {
		MelderInfo_write (value, U" ", units);
	}
	MelderInfo_close ();
}

void Melder_divertInfo (MelderString *p_buffer) {
	MelderInfo::_p_currentBuffer = ( p_buffer ? p_buffer : & MelderInfo::_foregroundBuffer );
}

void Melder_clearInfo () {
	if (MelderInfo::_p_currentBuffer == & MelderInfo::_foregroundBuffer) {
		MelderString_empty (MelderInfo::_p_currentBuffer);
		if (MelderInfo::_p_currentProc != & MelderInfo::_defaultProc)
			MelderInfo::_p_currentProc (U"");
	}
}

conststring32 Melder_getInfo () {
	return MelderInfo::_p_currentBuffer -> string ? MelderInfo::_p_currentBuffer -> string : U"";
}

/* End of file melder_info.cpp */
