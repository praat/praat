/* Script.cpp
 *
 * Copyright (C) 1997-2005,2009,2011,2015,2016,2020,2024,2025 Paul Boersma
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

#include "Script.h"

Thing_implement (Script, SimpleString, 0);
Thing_implement (ScriptSet, SortedSetOfString, 0);

static autoScriptSet theKnownScripts;

autoScript Script_createFromFile (MelderFile file) {
	autoScript me = Thing_new (Script);
	my string = Melder_dup (MelderFile_peekPath (file));
	return me;
}

void Script_rememberDuringThisAppSession_move (autoScript me) {
	if (! theKnownScripts)
		theKnownScripts = ScriptSet_create ();
	//TRACE
	trace (U"Adding script: \"", my string.get(), U"\"");
	theKnownScripts -> addItem_move (me.move());
}

Script Script_find (conststring32 filePath) {
	integer position = theKnownScripts -> lookUp (filePath);
	Melder_assert (position != 0);
	return theKnownScripts -> at [position];
}

/* End of file Script.cpp */
