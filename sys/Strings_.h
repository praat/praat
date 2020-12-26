#ifndef _Strings_h_
#define _Strings_h_
/* Strings.h
 *
 * Copyright (C) 1992-2007,2011,2012,2015-2018,2020 Paul Boersma
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

#include "Data.h"

#include "Strings_def.h"

autoStrings Strings_createAsFileList (conststring32 path /* cattable */);
autoStrings Strings_createAsFolderList (conststring32 path /* cattable */);
autoStrings Strings_readFromRawTextFile (MelderFile file);
void Strings_writeToRawTextFile (Strings me, MelderFile file);

void Strings_randomize (Strings me);
void Strings_genericize (Strings me);
void Strings_nativize (Strings me);
void Strings_sort (Strings me);

void Strings_remove (Strings me, integer position);
void Strings_replace (Strings me, integer position, conststring32 text);
void Strings_insert (Strings me, integer position, conststring32 text);

/* End of file Strings.h */
#endif
