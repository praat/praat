#ifndef _melder_info_h_
#define _melder_info_h_
/* melder_info.h
 *
 * Copyright (C) 1992-2018,2020 Paul Boersma
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

/*
	Give information to stdout (batch), or to an "Info" window (interactive), or to a diverted string.
*/

namespace MelderInfo {
	using Proc = void (*) (conststring32 message);
	void _defaultProc (conststring32 message);
	extern Proc _p_currentProc;
	extern MelderString _foregroundBuffer, *_p_currentBuffer;
}

void MelderInfo_open ();   // clear the Info window in the background
void MelderInfo_close ();   // drain the background info to the Info window, making sure there is a line break
void MelderInfo_drain ();   // drain the background info to the Info window, without adding any extra line break

inline static void _recursiveTemplate_MelderInfo_write (const MelderArg& arg) {
	MelderConsole::write (arg._arg, false);
}
template <typename... Args>
void _recursiveTemplate_MelderInfo_write (const MelderArg& first, Args... rest) {
	_recursiveTemplate_MelderInfo_write (first);
	_recursiveTemplate_MelderInfo_write (rest...);
}

template <typename... Args>
void MelderInfo_write (const MelderArg& first, Args... rest) {
	MelderString_append (MelderInfo::_p_currentBuffer, first, rest...);
	if (MelderInfo::_p_currentProc == & MelderInfo::_defaultProc && MelderInfo::_p_currentBuffer == & MelderInfo::_foregroundBuffer)
		_recursiveTemplate_MelderInfo_write (first, rest...);
}

template <typename... Args>
void MelderInfo_writeLine (const MelderArg& first, Args... rest) {
	MelderString_append (MelderInfo::_p_currentBuffer, first, rest...);
	MelderString_appendCharacter (MelderInfo::_p_currentBuffer, U'\n');
	if (MelderInfo::_p_currentProc == & MelderInfo::_defaultProc && MelderInfo::_p_currentBuffer == & MelderInfo::_foregroundBuffer) {
		_recursiveTemplate_MelderInfo_write (first, rest...);
		MelderConsole::write (U"\n", false);
	}
}

template <typename... Args>
void Melder_information (const MelderArg& first, Args... rest) {
	MelderString_copy (MelderInfo::_p_currentBuffer, first, rest...);
	if (MelderInfo::_p_currentProc == & MelderInfo::_defaultProc && MelderInfo::_p_currentBuffer == & MelderInfo::_foregroundBuffer)
		_recursiveTemplate_MelderInfo_write (first, rest...);
	MelderInfo_close ();
}

void Melder_informationReal (double value, conststring32 units);   // %.17g or --undefined--; units may be null

void Melder_divertInfo (MelderString *p_buffer);   // nullptr = back to normal

class autoMelderDivertInfo {
	public:
		autoMelderDivertInfo (MelderString *p_buffer) { Melder_divertInfo (p_buffer); }
		~autoMelderDivertInfo () { Melder_divertInfo (nullptr); }
};

void Melder_clearInfo ();   // clear the Info window
conststring32 Melder_getInfo ();

void Melder_setInformationProc (MelderInfo::Proc proc);

/* End of file melder_info.h */
#endif
