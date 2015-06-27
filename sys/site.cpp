/* site.cpp
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

/*
 * pb 2002/03/07 GPL
 * pb 2004/10/21 simplified print command
 * pb 2005/03/02 pref string buffer 260 bytes long
 * pb 2007/08/12 wchar
 * pb 2007/12/09 preferences
 * pb 2011/05/15 C++
 */

#include "Preferences.h"
#include <string.h>
#include "site.h"

static char32 printCommand [Preferences_STRING_BUFFER_SIZE];

char32 * Site_getPrintCommand (void) { return printCommand; }

void Site_setPrintCommand (const char32 *text) { str32cpy (printCommand, text); }

void Site_prefs (void) {
	Preferences_addString (U"Site.printCommand", printCommand, U"lp -c %s");
}

/* End of file site.cpp */

