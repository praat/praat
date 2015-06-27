#ifndef _FileInMemory_h_
#define _FileInMemory_h_
/* FileInMemory.h
 *
 * Copyright (C) 2011-2012 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
	djmw 20120125
*/
#include "Collection.h"

#include "Strings_.h"
#include "melder.h"
#include "Strings_.h"

Thing_define (FileInMemory, Data) {
	char32 *d_path;
	char32 *d_id;
	long d_numberOfBytes;
	char *d_data;

	void v_copy (Any data_to)
		override;
	void v_destroy ()
		override;
	void v_info ()
		override;
};

FileInMemory FileInMemory_create (MelderFile file);
FileInMemory FileInMemory_createWithData (long numberOfBytes, const char *data, const char32 *path, const char32 *id);

void FileInMemory_dontOwnData (FileInMemory me);
void FileInMemory_setId (FileInMemory me, const char32 *newId);

void FileInMemory_showAsCode (FileInMemory me, const char32 *name, long numberOfBytesPerLine);

Thing_define (FilesInMemory, SortedSet) {
	int d_sortKey;

	static int s_compare_name (Any data1, Any data2);
	static int s_compare_id (Any data1, Any data2);
	Data_CompareFunction v_getCompareFunction ()
		override { return d_sortKey == 0 ? s_compare_name : s_compare_id; }
};

FilesInMemory FilesInMemory_create ();
FilesInMemory FilesInMemory_createFromDirectoryContents (const char32 *dirpath, const char32 *file);

void FilesInMemory_showAsCode (FilesInMemory me, const char32 *name, long numberOfBytesPerLine);
void FilesInMemory_showOneFileAsCode (FilesInMemory me, long index, const char32 *name, long numberOfBytesPerLine);

long FilesInMemory_getIndexFromId (FilesInMemory me, const char32 *id);
Strings FilesInMemory_to_Strings_id (FilesInMemory me);
char * FilesInMemory_getCopyOfData (FilesInMemory me, const char32 *id, long *numberOfBytes);
const char * FilesInMemory_getData (FilesInMemory me, const char32 *id, long *numberOfBytes);

#endif // _FileInMemory_h_
