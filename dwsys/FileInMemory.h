#ifndef _FileInMemory_h_
#define _FileInMemory_h_
/* FileInMemory.h
 *
 * Copyright (C) 2011-2017 David Weenink, 2015 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Collection.h"
#include "Strings_.h"


#include "FileInMemory_def.h"

autoFileInMemory FileInMemory_create (MelderFile file);

autoFileInMemory FileInMemory_createWithData (integer numberOfBytes, const char *data, const char32 *path, const char32 *id);

void FileInMemory_dontOwnData (FileInMemory me);

void FileInMemory_setId (FileInMemory me, const char32 *newId);

void FileInMemory_showAsCode (FileInMemory me, const char32 *name, integer numberOfBytesPerLine);


#endif // _FileInMemory_h_
