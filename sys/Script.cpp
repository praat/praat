/* Script.cpp
 *
 * Copyright (C) 1997-2005,2009,2011,2015,2016,2020 Paul Boersma
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

Thing_implement (Script, Daata, 0);

autoScript Script_createFromFile (MelderFile file) {
	autoScript me = Thing_new (Script);
	MelderFile_copy (file, & my file);
	return me;
}

/* End of file Script.cpp */
