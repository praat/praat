/* site.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2007/08/12 wchar_t
 * pb 2007/12/09 preferences
 * pb 2011/05/15 C++
 */

#include "Preferences.h"
#include <string.h>
#include "site.h"

static wchar_t printCommand [Preferences_STRING_BUFFER_SIZE];

wchar_t * Site_getPrintCommand (void) { return printCommand; }

void Site_setPrintCommand (const wchar_t *text) { wcscpy (printCommand, text); }

void Site_prefs (void) {
	Preferences_addString (L"Site.printCommand", printCommand, L"lp -c %s");
}

/* End of file site.cpp */

