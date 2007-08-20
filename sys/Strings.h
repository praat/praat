#ifndef _Strings_h_
#define _Strings_h_
/* Strings.h
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/08/10
 */

#ifndef _Data_h_
	#include "Data.h"
#endif
#include "Strings_def.h"

#define Strings_methods Data_methods
oo_CLASS_CREATE (Strings, Data);

Strings Strings_createAsFileList (const wchar_t *path);
Strings Strings_createAsDirectoryList (const wchar_t *path);
Strings Strings_readFromRawTextFile (MelderFile file);
int Strings_writeToRawTextFile (Strings me, MelderFile file);

void Strings_randomize (Strings me);
int Strings_genericize (Strings me);
int Strings_nativize (Strings me);
void Strings_sort (Strings me);

/* End of file Strings.h */
#endif
