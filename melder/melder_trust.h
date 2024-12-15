#ifndef _melder_trust_h_
#define _melder_trust_h_
/* melder_trust.h
 *
 * Copyright (C) 2024 Paul Boersma
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
	SYNOPSIS (2024-12-14)

	Melder_checkTrust (interpreter, args...);
*/

namespace MelderTrust {
	extern MelderString _buffer;
	using Proc = void (*) (void *void_interpreter, conststring32 message);
	void _defaultProc (void *void_interpreter, conststring32 message);
	extern Proc _p_currentProc;
}

template <typename... Args>
void Melder_checkTrust (void *void_interpreter, const MelderArg& first, Args... rest) {
	MelderString_copy (& MelderTrust::_buffer, first, rest...);
	(*MelderTrust::_p_currentProc) (void_interpreter, MelderTrust::_buffer.string);
}

void Melder_setTrustProc (MelderTrust::Proc p_proc);

/* End of file melder_trust.h */
#endif
