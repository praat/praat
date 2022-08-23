/* site.cpp
 *
 * Copyright (C) 1992-2011,2015,2016,2018,2022 Paul Boersma
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

#include "Preferences.h"
#include <string.h>
#include "site.h"

static PrefsString printCommand;

char32 * Site_getPrintCommand () { return printCommand; }

void Site_setPrintCommand (conststring32 text) { Pref_copyString (text, printCommand); }

void Site_prefs () {
	Preferences_addString (U"Site.printCommand", printCommand, U"lp -c %s");
}

/* End of file site.cpp */

