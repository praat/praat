#ifndef _FileInMemorySet_h_
#define _FileInMemorySet_h_
/* FileInMemorySet.h
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

#include "FileInMemory.h"

#include "Collection.h"
#include "Strings_.h"

#include "FileInMemorySet_def.h"

autoFileInMemorySet FileInMemorySet_create (); 

autoFileInMemorySet FileInMemorySet_createFromDirectoryContents (const char32 *dirpath, const char32 *file);

autoFileInMemorySet FilesInMemory_to_FileInMemorySet (OrderedOf<structFileInMemory>& list);

autoFileInMemorySet FileInMemorySet_extractFiles (FileInMemorySet me, kMelder_string which, const char32 *criterion);

/* Only creates references to files in memory. Once me is deleted, the references are not valid any more!! */
autoFileInMemorySet FileInMemorySet_listFiles (FileInMemorySet me, kMelder_string which, const char32 *criterion);

integer FileInMemorySet_getTotalNumberOfBytes (FileInMemorySet me);

autoFileInMemorySet FileInMemorySets_merge (OrderedOf<structFileInMemorySet>& list);

void FileInMemorySet_showAsCode (FileInMemorySet me, const char32 *name, integer numberOfBytesPerLine);

void FileInMemorySet_showOneFileAsCode (FileInMemorySet me, integer index, const char32 *name, integer numberOfBytesPerLine);

char * FileInMemorySet_getCopyOfData (FileInMemorySet me, const char32 *id, integer *numberOfBytes);
const char * FileInMemorySet_getData (FileInMemorySet me, const char32 *id, integer *numberOfBytes);

integer FileInMemorySet_findNumberOfMatches_path (FileInMemorySet me, kMelder_string which, const char32 *criterion);

integer FileInMemorySet_getIndexFromId (FileInMemorySet me, const char32 *id);

integer FileInMemorySet_lookUp (FileInMemorySet me, const char32 *path);

bool FileInMemorySet_hasDirectory (FileInMemorySet me, const char32 *name);

autoStrings FileInMemorySet_to_Strings_id (FileInMemorySet me);

#endif // _FileInMemorySet_h_
