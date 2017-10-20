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

template <typename T   Melder_ENABLE_IF_ISA (T, structFileInMemory)>
struct SortedSetOfFileInMemoryOf : SortedSetOf <T> {
	SortedSetOfFileInMemoryOf () {
	}
	SortedSetOfFileInMemoryOf<T>&& move () noexcept { return static_cast <SortedSetOfFileInMemoryOf<T>&&> (*this); }
	static int s_compareHook (FileInMemory me, FileInMemory thee) noexcept {
		return Melder_cmp (my d_path, thy d_path);
	}
	typename SortedOf<T>::CompareHook v_getCompareHook ()
		override { return (typename SortedOf<T>::CompareHook) our s_compareHook; }
};
		
autoFileInMemorySet FileInMemorySet_create (); 

/*
Collection_define (FileInMemorySet, SortedSetOf, FileInMemory) {
	static int s_compare_name (FileInMemory f1, FileInMemory f2) {
		return Melder_cmp (f1 -> d_path, f2 -> d_path);
	}
	CompareHook v_getCompareHook ()	override { 
		return s_compare_name;
	}
};*/

//autoFileInMemorySet FileInMemorySet_create ();

autoFileInMemorySet FileInMemorySet_createFromDirectoryContents (const char32 *dirpath, const char32 *file);

autoFileInMemorySet FileInMemorySets_merge (OrderedOf<structFileInMemorySet>& list);

void FileInMemorySet_showAsCode (FileInMemorySet me, const char32 *name, integer numberOfBytesPerLine);

void FileInMemorySet_showOneFileAsCode (FileInMemorySet me, integer index, const char32 *name, integer numberOfBytesPerLine);

char * FileInMemorySet_getCopyOfData (FileInMemorySet me, const char32 *id, integer *numberOfBytes);
const char * FileInMemorySet_getData (FileInMemorySet me, const char32 *id, integer *numberOfBytes);

integer FileInMemorySet_getIndexFromId (FileInMemorySet me, const char32 *id);

integer FileInMemorySet_getIndexFromPathName (FileInMemorySet me, const char32 *path);

bool FileInMemorySet_hasDirectory (FileInMemorySet me, const char32 *name);

autoStrings FileInMemorySet_to_Strings_id (FileInMemorySet me);

#endif // _FileInMemorySet_h_
