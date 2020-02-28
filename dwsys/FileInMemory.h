#ifndef _FileInMemory_h_
#define _FileInMemory_h_
/* FileInMemory.h
 *
 * Copyright (C) 2011-2020 David Weenink, 2015,2018 Paul Boersma
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

/*
	If the FileInMemory is created from static data we do not need to copy the data into the object but only
	create a link to the data. This implies that the data cannot be deleted if the FileInMemory object gets
	destroyed. However, if the FileInMemory object is copied, for example, then we also have to copy the data,
	and the copied object has to become owner of these copied data. This is automatically guaranteed by our
	implementation, because _dontOwnData is default-initialised as false.
	Only if we create a FileInMemory object from data we have to be explicit about ownership.
*/
autoFileInMemory FileInMemory_createWithData (integer numberOfBytes, const char *data, bool isStaticData, conststring32 path, conststring32 id);

void FileInMemory_setId (FileInMemory me, conststring32 newId);

void FileInMemory_showAsCode (FileInMemory me, conststring32 name, integer numberOfBytesPerLine);


#endif // _FileInMemory_h_
