/* melder_help.cpp
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

#include "melder.h"

static void defaultHelp (conststring32 query) {
	Melder_flushError (U"Don't know how to find help on \"", query, U"\".");
}

static void defaultSearch () {
	Melder_flushError (U"Do not know how to search.");
}

static void (*p_theHelpProc) (conststring32 query) = & defaultHelp;
static void (*p_theSearchProc) () = & defaultSearch;

void Melder_help (conststring32 query) {
	(*p_theHelpProc) (query);
}

void Melder_search () {
	(*p_theSearchProc) ();
}

void Melder_setHelpProc (void (*p_helpProc) (conststring32 query))
	{ p_theHelpProc = p_helpProc ? p_helpProc : & defaultHelp; }

void Melder_setSearchProc (void (*p_searchProc) (void))
	{ p_theSearchProc = p_searchProc ? p_searchProc : & defaultSearch; }

/* End of file melder_help.cpp */
