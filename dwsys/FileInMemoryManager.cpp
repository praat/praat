/* FileInMemoryManager.cpp
 *
 * Copyright (C) 2017 David Weenink
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

#include "FileInMemoryManager.h"
#include "Collection.h"

#include "oo_DESTROY.h"
#include "FileInMemoryManager_def.h"
#include "oo_COPY.h"
#include "FileInMemoryManager_def.h"
#include "oo_EQUAL.h"
#include "FileInMemoryManager_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FileInMemoryManager_def.h"
#include "oo_WRITE_TEXT.h"
#include "FileInMemoryManager_def.h"
#include "oo_READ_TEXT.h"
#include "FileInMemoryManager_def.h"
#include "oo_WRITE_BINARY.h"
#include "FileInMemoryManager_def.h"
#include "oo_READ_BINARY.h"
#include "FileInMemoryManager_def.h"
#include "oo_DESCRIPTION.h"
#include "FileInMemoryManager_def.h"

/*
	File open and read emulations. The FILE * is internally used as a pointer to the index of the file in the Set.
	List of open files has to contain per file: index, position, length (bytes), pointer to data
*/

Thing_implement (FileInMemoryManager, Daata, 0);

bool FileInMemoryManager_hasDirectory (FileInMemoryManager me, const char32 *name) {
		return FileInMemorySet_hasDirectory (my files.get(), name);
}

autoFileInMemoryManager FileInMemoryManager_create (FileInMemorySet files) {
	try {
		autoFileInMemoryManager me = Thing_new (FileInMemoryManager);
		my files = Data_copy (files);
		my openFiles = FileInMemorySet_create ();
		my openFiles -> _initializeOwnership (false);
		return me;
	} catch (MelderError) {
		Melder_throw (U"");
	}
}

/*
integer SortedSetOfLong_Lookup (SortedSetOfLong me, integer number) {
	if (my size == 0) return 0;   // empty set 
	integer where = number - my at [my size] -> number;   // compare with last item
	if (where > 0) return 0;   // not at end
	if (where == 0) return my size;
	where = number - my at [1] -> number;   // compare with first item
	if (where < 0) return 0;   // not at start
	if (where == 0) return 1;
	integer left = 1, right = my size;
	while (left < right - 1) {
		integer mid = (left + right) / 2;
		where = number - my at [mid] -> number;
		if (where == 0) { // found
			return mid;
		} else if (where > 0) {
			left = mid;
		} else {
			right = mid;
		}
	}
	Melder_assert (right == left + 1);
	if ((number - my at [left] -> number) == 0) {
		return left;
	} else if ((number - my at [right] -> number) == 0) {
		return right;
	} else {
		return 0;
	}
}
*/

autoTable FileInMemoryManager_downto_Table (FileInMemoryManager me, bool openFilesOnly) {
	try {
		long numberOfRows = openFilesOnly ? my openFiles -> size : my files -> size;
		autoTable thee = Table_createWithColumnNames (numberOfRows, U"path id size position");
		for (integer irow = 1; irow <= numberOfRows; irow ++) {
			FileInMemory fim = static_cast <FileInMemory> (openFilesOnly ? my openFiles -> at [irow] : my files -> at [irow]);
			Table_setStringValue (thee.get(), irow, 1, fim -> d_path);
			Table_setStringValue (thee.get(), irow, 2, fim -> d_id);
			Table_setNumericValue (thee.get(), irow, 3, fim -> d_numberOfBytes);
			Table_setNumericValue (thee.get(), irow, 4, fim -> d_position);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Table created.");
	}
}

autoFileInMemory FileInMemoryManager_createFile (FileInMemoryManager me, MelderFile file) {
	try {
		autoFileInMemory thee = FileInMemory_create (file);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"Cannot create a FileInMemory object.");
	}
}

autoFileInMemorySet FileInMemoryManager_extractFiles (FileInMemoryManager me, kMelder_string which, const char32 *criterion) {
	try {
		autoFileInMemorySet thee = Thing_new (FileInMemorySet);
		for (long ifile = 1; ifile <= my files -> size; ifile ++) {
			FileInMemory fim = static_cast <FileInMemory> (my files -> at [ifile]);
			if (Melder_stringMatchesCriterion (fim -> d_path, which, criterion)) {
				autoFileInMemory item = Data_copy (fim);
				thy addItem_move (item.move());
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot extract files.");
	}
}

FILE *FileInMemoryManager_fopen (FileInMemoryManager me, const char *filename, const char *mode) {
	try {
		integer index = 0;
		if (*mode == 'r') {
			index = FileInMemorySet_lookUp (my files.get(), Melder_peek8to32(filename));
			if (index > 0) {
				FileInMemory fim = (FileInMemory) my files -> at [index];
				if (fim -> d_position == 0) { // not open
					my openFiles -> addItem_ref (fim);
				} else { // reset position
					fim -> d_position = 0;
				}
			} else {
				// file does not exist, set error condition?
			}
		} 
		return reinterpret_cast<FILE *> (index);
	} catch (MelderError) {
		Melder_throw (U"File ", Melder_peek8to32(filename), U" cannot be opended.");
	}
}

static integer _FileInMemoryManager_getIndexInOpenFiles (FileInMemoryManager me, FILE *stream) {
	integer filesIndex = reinterpret_cast<integer> (stream);
	FileInMemory fim = static_cast<FileInMemory> (my files -> at [filesIndex]);
	integer openFilesIndex = FileInMemorySet_lookUp (my openFiles.get(), fim -> d_path);
	return openFilesIndex;
}

void FileInMemoryManager_rewind (FileInMemoryManager me, FILE *stream) {
	integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	if (openFilesIndex > 0) {
		FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
		fim -> d_position = 0; fim -> d_errno = 0;  fim -> ungetBuffer = -1;
	}
}

int FileInMemoryManager_fclose (FileInMemoryManager me, FILE *stream) {
	integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	if (openFilesIndex > 0) {
		FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
		fim -> d_position = 0; fim -> d_errno = 0;  fim -> ungetBuffer = -1;
		my openFiles -> removeItem (openFilesIndex);
	}
	return my errorNumber = 0; // always ok
}

int FileInMemoryManager_feof (FileInMemoryManager me, FILE *stream) {
	integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	int eof = 0;
	if (openFilesIndex > 0) {
		FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
		if (fim -> d_position >= fim -> d_numberOfBytes) {
			eof = 1;
		}
	}
	return eof;
}

int FileInMemoryManager_fseek (FileInMemoryManager me, FILE *stream, integer offset, int origin) {
	integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	int errval = EBADF;
	if (openFilesIndex > 0) {
		FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
		integer newPosition = 0;
		if (origin == SEEK_SET) {
			newPosition = offset;
		} else if (origin == SEEK_CUR) {
			newPosition = fim -> d_position + offset;
		} else if (origin == SEEK_END) {
			newPosition = fim -> d_numberOfBytes + offset;
		} else {
			return my errorNumber = EINVAL;
		}
		if (newPosition < 0) { // > numberOfBytes is allowed
			newPosition = 0;
		}
		fim -> d_position = newPosition;
		fim -> ungetBuffer = -1;
		errval = 0;
	}
	return my errorNumber = errval;
}

char *FileInMemoryManager_fgets (FileInMemoryManager me, char *str, int num, FILE *stream) {
	integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	if (openFilesIndex > 0) {
		FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
		integer startPos = fim -> d_position;
		if (startPos < fim -> d_numberOfBytes) {
			integer i = 0;
			integer endPos = startPos + num;
			endPos = endPos < fim -> d_numberOfBytes ? endPos : fim -> d_numberOfBytes;
			const unsigned char * p = fim -> d_data + startPos;
			if (fim -> ungetBuffer > 0) {
				str [i ++] = fim -> ungetBuffer;
				p ++;
				fim -> ungetBuffer = -1;
			}
			while (i < num) {
				str [i ++] = *p;
				if (*p ++ == '\n') {
					break;
				}
			}
			str [i] = '\0';
			fim -> d_position = endPos;
		} else {
			fim -> d_errno = EOF;
		}
		return str;
	} else {
		Melder_throw (me, U": File is not open.");
	}
}

int FileInMemoryManager_fgetc (FileInMemoryManager me, FILE *stream) {
	char str[4];
	FileInMemoryManager_fgets (me, str, 1, stream);
	return FileInMemoryManager_feof (me, stream) ? EOF : static_cast<int> (*str);
}

/* size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );*/
size_t FileInMemoryManager_fread (FileInMemoryManager me, void *ptr, size_t size, size_t nmemb, FILE *stream) {
	integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	if (openFilesIndex > 0) {
		FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
		integer startPos = fim -> d_position;
		if (startPos < fim -> d_numberOfBytes) {
			integer i = 0, numberOfBytes = nmemb * size; // 
			integer endPos = startPos + numberOfBytes;
			
			if (endPos >= fim -> d_numberOfBytes) {
				nmemb = (endPos - startPos) / size;
				endPos = startPos + nmemb * size;
				fim -> d_errno = EOF;
			}
			numberOfBytes = nmemb * size;
			const unsigned char * p = fim -> d_data + fim -> d_position;
			char * str = static_cast<char *> (ptr);
			while (i < numberOfBytes) {
				str [i ++] = *p ++;
			}
			fim -> d_position = endPos;
		}
		return nmemb;
	} else {
		Melder_throw (me, U": File is not open.");
	}
}

/* No pushback buffer */
int FileInMemoryManager_ungetc (FileInMemoryManager me, int character, FILE * stream) {
	if (character == EOF) {
		return -1;
	}
	integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	if (openFilesIndex > 0) {
		FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
		-- (fim -> d_position);
		fim -> ungetBuffer = character;
		return character;
	}
}

/* This mimics GetFileLength of espeak-ng */
int FileInMemoryManager_GetFileLength (FileInMemoryManager me, const char *filename) {
		integer index = FileInMemorySet_lookUp (my files.get(), Melder_peek8to32(filename));
		if (index > 0) {
			FileInMemory fim = static_cast<FileInMemory> (my files -> at [index]);
			return fim -> d_numberOfBytes;
		}
		// Directory ??
		if (FileInMemorySet_hasDirectory (my files.get(), Melder_peek8to32(filename))) {
			return -EISDIR;
		}
		return -1;
}

/* long int ftell ( FILE * stream ); 
 * int fgetpos ( FILE * stream, fpos_t * pos );
 * int fsetpos ( FILE * stream, const fpos_t * pos );
 * void rewind ( FILE * stream );
 */

/* End of file FileInMemoryManager.cpp */
