#ifndef _melder_warning_h_
#define _melder_warning_h_
/* melder_warning.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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
	SYNOPSIS (2018-08-08)

	Melder_warning (args...);

		Gives a warning to stderr (batch) or to a "Warning" dialog.
		Use sparingly, because it interrupts the user's workflow.
*/

namespace MelderWarning {
	extern int _depth;
	extern MelderString _buffer;
	using Proc = void (*) (conststring32 message);
	void _defaultProc (conststring32 message);
	extern Proc _p_currentProc;
}

template <typename... Args>
void Melder_warning (const MelderArg& first, Args... rest);

template <typename... Args>
void Melder_warning (const MelderArg& first, Args... rest) {
	if (MelderWarning::_depth < 0)
		return;
	MelderString_copy (& MelderWarning::_buffer, first, rest...);
	(*MelderWarning::_p_currentProc) (MelderWarning::_buffer.string);
}

void Melder_warningOff ();
void Melder_warningOn ();

class autoMelderWarningOff {
public:
	autoMelderWarningOff () { Melder_warningOff (); }
	~autoMelderWarningOff () { Melder_warningOn (); }
};

void Melder_setWarningProc (MelderWarning::Proc p_proc);

/* End of file melder_warning.h */
#endif
