/* melder_warning.cpp
 *
 * Copyright (C) 1992-2012,2014-2016,2018,2020 Paul Boersma
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

int MelderWarning::_depth = 0;

void MelderWarning::_defaultProc (conststring32 message) {
	MelderConsole::write (U"Warning: ", true);
	MelderConsole::write (message, true);
	MelderConsole::write (U"\n", true);
}

MelderWarning::Proc MelderWarning::_p_currentProc = & MelderWarning::_defaultProc;

MelderString MelderWarning::_buffer;

void Melder_warningOff () { MelderWarning::_depth --; }
void Melder_warningOn () { MelderWarning::_depth ++; }

void Melder_setWarningProc (MelderWarning::Proc p_proc) {
	MelderWarning::_p_currentProc = ( p_proc ? p_proc : & MelderWarning::_defaultProc );
}

/* End of file melder_warning.cpp */
