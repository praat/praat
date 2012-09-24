#ifndef _Strings_h_
#define _Strings_h_
/* Strings.h
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

#include "Data.h"

#include "Strings_def.h"
oo_CLASS_CREATE (Strings, Data);

Strings Strings_createAsFileList (const wchar_t *path);
Strings Strings_createAsDirectoryList (const wchar_t *path);
Strings Strings_readFromRawTextFile (MelderFile file);
void Strings_writeToRawTextFile (Strings me, MelderFile file);

void Strings_randomize (Strings me);
void Strings_genericize (Strings me);
void Strings_nativize (Strings me);
void Strings_sort (Strings me);

void Strings_remove (Strings me, long position);
void Strings_replace (Strings me, long position, const wchar_t *text);
void Strings_insert (Strings me, long position, const wchar_t *text);

/* End of file Strings.h */
#endif
