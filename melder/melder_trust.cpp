/* melder_trust.cpp
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

#include "Interpreter.h"
#include "Script.h"
#include "Notebook.h"

void MelderTrust::_defaultProc (void *void_interpreter, conststring32 message) {
	Interpreter interpreter = (Interpreter) void_interpreter;
	if (interpreter) {
		Script script = interpreter -> scriptReference;
		Notebook notebook = interpreter -> notebookReference;
		if (! script && ! notebook)
			Melder_throw (U"We expected a script or a notebook.");
		if (script && ! script -> trusted)
			Melder_throw (U"The following action was requested but is not allowed:\n", message);
		if (notebook && ! notebook -> trusted)
			Melder_throw (U"The following action was requested but is not allowed:\n", message);
	}
}

MelderTrust::Proc MelderTrust::_p_currentProc = & MelderTrust::_defaultProc;

MelderString MelderTrust::_buffer;

void Melder_setTrustProc (MelderTrust::Proc p_proc) {
	MelderTrust::_p_currentProc = ( p_proc ? p_proc : & MelderTrust::_defaultProc );
}

/* End of file melder_trust.cpp */
