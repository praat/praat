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

Thing_define (FileInMemory, Daata) {
	char32 *d_path;
	char32 *d_id;
	integer d_numberOfBytes;
	char *d_data;
	bool ownData;

	void v_copy (Daata data_to)
		override;
	void v_destroy () noexcept
		override;
	void v_info ()
		override;
};

autoFileInMemory FileInMemory_create (MelderFile file);

autoFileInMemory FileInMemory_createWithData (integer numberOfBytes, const char *data, const char32 *path, const char32 *id);

void FileInMemory_dontOwnData (FileInMemory me);

void FileInMemory_setId (FileInMemory me, const char32 *newId);

void FileInMemory_showAsCode (FileInMemory me, const char32 *name, integer numberOfBytesPerLine);

Collection_define (FileInMemorySet, SortedSetOf, FileInMemory) {
	int d_sortKey;

	static int s_compare_name (FileInMemory data1, FileInMemory data2);
	static int s_compare_id (FileInMemory data1, FileInMemory data2);
	CompareHook v_getCompareHook ()
		override { return d_sortKey == 0 ? s_compare_name : s_compare_id; }
};

autoFileInMemorySet FileInMemorySet_createFromDirectoryContents (const char32 *dirpath, const char32 *file);

void FileInMemorySet_showAsCode (FileInMemorySet me, const char32 *name, integer numberOfBytesPerLine);

void FileInMemorySet_showOneFileAsCode (FileInMemorySet me, integer index, const char32 *name, integer numberOfBytesPerLine);

integer FileInMemorySet_getIndexFromId (FileInMemorySet me, const char32 *id);

autoStrings FileInMemorySet_to_Strings_id (FileInMemorySet me);

char * FileInMemorySet_getCopyOfData (FileInMemorySet me, const char32 *id, integer *numberOfBytes);

const char * FileInMemorySet_getData (FileInMemorySet me, const char32 *id, integer *numberOfBytes);

#endif // _FileInMemory_h_
