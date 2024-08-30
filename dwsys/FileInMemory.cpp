/* FileInMemory.cpp
 *
 * Copyright (C) 2012-2021 David Weenink, 2017,2024 Paul Boersma
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
#include "Strings_.h"

#include "oo_DESTROY.h"
#include "FileInMemory_def.h"
#include "oo_COPY.h"
#include "FileInMemory_def.h"
#include "oo_EQUAL.h"
#include "FileInMemory_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FileInMemory_def.h"
#include "oo_WRITE_TEXT.h"
#include "FileInMemory_def.h"
#include "oo_READ_TEXT.h"
#include "FileInMemory_def.h"
#include "oo_WRITE_BINARY.h"
#include "FileInMemory_def.h"
#include "oo_READ_BINARY.h"
#include "FileInMemory_def.h"
#include "oo_DESCRIPTION.h"
#include "FileInMemory_def.h"

Thing_implement (FileInMemory, Daata, 0);
Thing_implement (FileInMemorySet, SortedSetOfString, 0);

void structFileInMemory :: v1_info () {
	our structDaata :: v1_info ();
	MelderInfo_writeLine (U"File name: ", our string.get());
	MelderInfo_writeLine (U"Number of bytes: ", our d_numberOfBytes);
}

autoFileInMemory FileInMemory_create (MelderFile file) {
	try {
		Melder_require (MelderFile_readable (file),
			U"File is not readable.");
		const integer length = MelderFile_length (file);
		Melder_require (length > 0,
			U"File should not be empty.");
		
		autoFileInMemory me = Thing_new (FileInMemory);
		my string = Melder_dup (file -> path);
		my d_numberOfBytes = length;
		my _dontOwnData = false;
		my d_data = newvectorzero <byte> (my d_numberOfBytes + 1);   // includes room for a final null byte in case the file happens to contain only text
		MelderFile_open (file);
		for (integer i = 1; i <= my d_numberOfBytes; i++) {
			const unsigned int number = bingetu8 (file -> filePointer);
			my d_data [i] = number;
		}
		my d_data [my d_numberOfBytes + 1] = 0;   // "extra" null byte
		MelderFile_close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemory not created from \"", Melder_fileToPath (file), U"\".");
	}
}

autoFileInMemory FileInMemory_createWithData (integer numberOfBytes, const char *data, bool isStaticData, conststring32 path, conststring32 id) {
	try {
		autoFileInMemory me = Thing_new (FileInMemory);
		my string = Melder_dup (path);
		my d_numberOfBytes = numberOfBytes;
		if (isStaticData) {
			my _dontOwnData = true; // we cannot dispose of the data!
			/*
				djmw 20200226:
				We changed d_data from type vector to autovector and cannot share the data anynmore.
				Therefore make an explicit copy until we find a solution.
			*/
			//my d_data.at = reinterpret_cast<unsigned char *> (const_cast<char *> (data))-1; // ... just a link
			//my d_data.size = numberOfBytes + 1;
			my _dontOwnData = false; // we can dispose of the data!
			my d_data = newvectorraw <unsigned char> (numberOfBytes + 1);
			memcpy (my d_data.asArgumentToFunctionThatExpectsZeroBasedArray(), data, (size_t) numberOfBytes + 1);
		} else {
			my _dontOwnData = false;
			my d_data = newvectorraw <unsigned char> (numberOfBytes + 1);
			memcpy (my d_data.asArgumentToFunctionThatExpectsZeroBasedArray(), data, (size_t) numberOfBytes + 1);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemory not created from data.");
	}
}

void FileInMemory_showAsCode (FileInMemory me, conststring32 name, integer numberOfBytesPerLine) {
	if (numberOfBytesPerLine < 1)
		numberOfBytesPerLine = 20;

	MelderInfo_write (U"\t\tstatic unsigned char ", name, U"_data[", my d_numberOfBytes+1, U"] = {");
	for (integer i = 1; i <= my d_numberOfBytes; i ++) {
		const unsigned char number = my d_data [i];
		MelderInfo_write (( i % numberOfBytesPerLine == 1 ? U"\n\t\t\t" : U"" ), number, U",");
	}
	MelderInfo_writeLine (U"0};");
	MelderInfo_write (U"\t\tautoFileInMemory ", name, U" = FileInMemory_createWithData (");
	MelderInfo_writeLine (my d_numberOfBytes, U", reinterpret_cast<const char *> (& ",
		name, U"_data), true, \n\t\t\tU\"", my string.get(), U"\");");
}

FileInMemory FileInMemorySet_fopen (FileInMemorySet me, const char *fileName, const char *mode) {
	Melder_assert (mode [0] == 'r');   // i.e. "r" or "rb"; only reading has been implemented; cannot write ('w') or append ('a')
	const integer index = my lookUp (Melder_peek8to32 (fileName));
	if (index == 0) {
		errno = ENOENT;   // "no such file or directory"
		return nullptr;
	}
	FileInMemory thee = my at [index];
	thy d_position = 0;   // after opening, start at the beginning of the file
	thy d_errno = 0;
	thy ungetChar = -1;
	thy isOpen = true;
	return thee;
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	void rewind ( FILE * stream );

	Set position of stream to the beginning
	Sets the position indicator associated with stream to the beginning of the file.

	The end-of-file and error internal indicators associated to the stream are cleared after a successful call to this function,
	and all effects from previous calls to ungetc on this stream are dropped.

	On streams open for update (read+write), a call to rewind allows to switch between reading and writing.

	Parameters

	stream
		Pointer to a FILE object that identifies the stream.


	Return Value
	none
*/
void FileInMemory_rewind (FileInMemory me) {
	Melder_assert (my isOpen);
	my d_position = 0;   // this was successful...
	my d_errno = 0;   // ...so we clear the end-of-file indicator and the error indicator
	my ungetChar = -1;   // we also drop any effects from previous calls to `ungetc` on this stream
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int fclose ( FILE * stream );

	Close file
	Closes the file associated with the stream and disassociates it.

	All internal buffers associated with the stream are disassociated from it and flushed:
	the content of any unwritten output buffer is written and the content of any unread input buffer is discarded.

	Even if the call fails, the stream passed as parameter will no longer be associated with the file nor its buffers.

	Parameters

	stream
		Pointer to a FILE object that specifies the stream to be closed. 


	Return Value
	If the stream is successfully closed, a zero value is returned.
	On failure, EOF is returned.
*/
int FileInMemory_fclose (FileInMemory me) {
	Melder_assert (my isOpen);
	my d_position = 0;
	my d_errno = 0;
	my ungetChar = -1;
	my isOpen = false;
	return 0;
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int feof ( FILE * stream );

	Check end-of-file indicator
	Checks whether the end-of-File indicator associated with stream is set, returning a value different from zero if it is.

	This indicator is generally set by a previous operation on the stream that attempted to read at or past the end-of-file.

	Notice that stream's internal position indicator may point to the end-of-file for the next operation, but still, the end-of-file indicator may not be set until an operation attempts to read at that point.

	This indicator is cleared by a call to clearerr, rewind, fseek, fsetpos or freopen. Although if the position indicator is not repositioned by such a call, the next i/o operation is likely to set the indicator again.

	Parameters

	stream
		Pointer to a FILE object that identifies the stream.


	Return Value
	A non-zero value is returned in the case that the end-of-file indicator associated with the stream is set.
	Otherwise, zero is returned.
*/
int FileInMemory_feof (FileInMemory me) {
	Melder_assert (my isOpen);
	return my d_position >= my d_numberOfBytes;
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int fseek ( FILE * stream, long int offset, int origin );

	Reposition stream position indicator
	Sets the position indicator associated with the stream to a new position.

	For streams open in binary mode, the new position is defined by adding offset to a reference position specified by origin.

	For streams open in text mode, offset shall either be zero or a value returned by a previous call to ftell,
	and origin shall necessarily be SEEK_SET.

	If the function is called with other values for these arguments,
	support depends on the particular system and library implementation (non-portable).

	The end-of-file internal indicator of the stream is cleared after a successful call to this function,
	and all effects from previous calls to ungetc on this stream are dropped.

	On streams open for update (read+write), a call to fseek allows to switch between reading and writing.

	Parameters

	stream
		Pointer to a FILE object that identifies the stream.
	offset
		Binary files: Number of bytes to offset from origin.
		Text files: Either zero, or a value returned by ftell.
	origin
		Position used as reference for the offset. It is specified by one of the following constants defined in <cstdio> exclusively to be used as arguments for this function:
		Constant	Reference position
		SEEK_SET	Beginning of file
		SEEK_CUR	Current position of the file pointer
		SEEK_END	End of file *
		* Library implementations are allowed to not meaningfully support SEEK_END (therefore, code using it has no real standard portability).


	Return Value
	If successful, the function returns zero.
	Otherwise, it returns non-zero value.
	If a read or write error occurs, the error indicator (ferror) is set.
*/
int FileInMemory_fseek (FileInMemory me, integer offset, int origin) {
	Melder_assert (my isOpen);
	my d_errno = 0;   // set the error indicator to nonzero only if there is an error
	integer newPosition = 0;
	if (origin == SEEK_SET)
		newPosition = offset;
	else if (origin == SEEK_CUR)
		newPosition = my d_position + offset;
	else if (origin == SEEK_END)
		newPosition = my d_numberOfBytes + offset;
	else
		return errno = my d_errno = EINVAL;   // FIXME: to boss? global? what about EBADF?

	if (newPosition < 0)   // > numberOfBytes is allowed
		newPosition = 0;

	my d_position = newPosition;   // FIXME: this may entail end-of-file (which is implementation-dependent)
	my ungetChar = -1;   // drop all effects of previous calls to ungetc on this stream
	return my d_errno;
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	long int ftell ( FILE * stream );

	Get current position in stream
	Returns the current value of the position indicator of the stream.

	For binary streams, this is the number of bytes from the beginning of the file.

	For text streams, the numerical value may not be meaningful but can still be used to restore the position to the same position later using fseek (if there are characters put back using ungetc still pending of being read, the behavior is undefined).

	Parameters

	stream
		Pointer to a FILE object that identifies the stream.


	Return Value
	On success, the current value of the position indicator is returned.
	On failure, -1L is returned, and errno is set to a system-specific positive value.
*/
integer FileInMemory_ftell (FileInMemory me) {
	Melder_assert (my isOpen);
	return my d_position;   // FIXME: what about EBDF?
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	char * fgets ( char * str, int num, FILE * stream );

	Get string from stream
	Reads characters from stream and stores them as a C string into str until (num-1) characters have been read or either a newline or the end-of-file is reached, whichever happens first.

	A newline character makes fgets stop reading, but it is considered a valid character by the function and included in the string copied to str.

	A terminating null character is automatically appended after the characters copied to str.

	Notice that fgets is quite different from gets: not only fgets accepts a stream argument,
	but also allows to specify the maximum size of str and includes in the string any ending newline character.

	Parameters

	str
		Pointer to an array of chars where the string read is copied.
	num
		Maximum number of characters to be copied into str (including the terminating null-character).
	stream
		Pointer to a FILE object that identifies an input stream.
		stdin can be used as argument to read from the standard input.


	Return Value
	On success, the function returns str.
	If the end-of-file is encountered while attempting to read a character, the eof indicator is set (feof). If this happens before any characters could be read, the pointer returned is a null pointer (and the contents of str remain unchanged).
	If a read error occurs, the error indicator (ferror) is set and a null pointer is also returned (but the contents pointed by str may have changed). 
 */
char *FileInMemory_fgets (char *str, int num, FileInMemory me) {
	Melder_assert (my isOpen);
	char *result = nullptr;

	integer startPos = my d_position;
	if (startPos < my d_numberOfBytes) {
		integer i = 0, endPos = startPos + num;
		endPos = endPos < my d_numberOfBytes ? endPos : my d_numberOfBytes;   // FIXME: use Melder_clip
		const unsigned char * p = my d_data.asArgumentToFunctionThatExpectsZeroBasedArray() + startPos;
		char *p_str = str;
		if (my ungetChar > 0) {
			/*
				copy the ungetChar and advance one position in stream
			*/
			*p_str ++ = my ungetChar;
			p ++;
			i ++;
			my ungetChar = -1;
		}
		while (i ++ < num && (*p_str ++ = *p) && *p ++ != '\n')
			;
		str [i] = '\0';
		my d_position += i;
		result = str;   // everything OK
	} else {
		my d_errno = EOF;
	}
	return result;
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int fgetc ( FILE * stream );

	Get character from stream
	Returns the character currently pointed by the internal file position indicator of the specified stream. The internal file position indicator is then advanced to the next character.

	If the stream is at the end-of-file when called, the function returns EOF and sets the end-of-file indicator for the stream (feof).

	If a read error occurs, the function returns EOF and sets the error indicator for the stream (ferror).

	fgetc and getc are equivalent, except that getc may be implemented as a macro in some libraries.

	Parameters

	stream
		Pointer to a FILE object that identifies an input stream.


	Return Value
	On success, the character read is returned (promoted to an int value).
	The return type is int to accommodate for the special value EOF, which indicates failure:
	If the position indicator was at the end-of-file, the function returns EOF and sets the eof indicator (feof) of stream.
	If some other reading error happens, the function also returns EOF, but sets its error indicator (ferror) instead.
*/
int FileInMemory_fgetc (FileInMemory me) {
	Melder_assert (my isOpen);
	char kar;
	(void) FileInMemory_fgets (& kar, 1, me);
	return FileInMemory_feof (me) ? EOF : (unsigned char) (kar);
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );

	Read block of data from stream
	Reads an array of count elements, each one with a size of size bytes, from the stream and stores them in the block of memory specified by ptr.

	The position indicator of the stream is advanced by the total amount of bytes read.

	The total amount of bytes read if successful is (size*count).

	Parameters

	ptr
		Pointer to a block of memory with a size of at least (size*count) bytes, converted to a void*.
	size
		Size, in bytes, of each element to be read.
		size_t is an unsigned integral type.
	count
		Number of elements, each one with a size of size bytes.
		size_t is an unsigned integral type.
	stream
		Pointer to a FILE object that specifies an input stream.

	Return Value
	The total number of elements successfully read is returned.
	If this number differs from the count parameter, either a reading error occurred or the end-of-file was reached while reading. In 	both cases, the proper indicator is set, which can be checked with ferror and feof, respectively.
	If either size or count is zero, the function returns zero and both the stream state and the content pointed by ptr remain unchanged.
	size_t is an unsigned integral type. 
*/
size_t FileInMemory_fread (void *ptr, size_t size, size_t count, FileInMemory me) {
	Melder_assert (my isOpen);
	size_t result = 0;
	integer startPos = my d_position;
	if (startPos < my d_numberOfBytes) {
		integer i = 0, endPos = startPos + count * size;
		
		if (endPos > my d_numberOfBytes) {
			count = (my d_numberOfBytes - startPos) / size;
			endPos = startPos + count * size;
			my d_errno = EOF;
		}
		const integer numberOfBytes = count * size;
		const unsigned char * p = my d_data.asArgumentToFunctionThatExpectsZeroBasedArray() + my d_position;
		char * str = static_cast<char *> (ptr);
		while (i < numberOfBytes)
			str [i ++] = *p ++;
		my d_position = endPos;
	}
	result = count;
	return result;
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int ungetc ( int character, FILE * stream );

	Unget character from stream
	A character is virtually put back into an input stream, decreasing its internal file position as if a previous getc operation was undone.

	This character may or may not be the one read from the stream in the preceding input operation. In any case, the next character retrieved from stream is the character passed to this function, independently of the original one.

	Notice though, that this only affects further input operations on that stream, and not the content of the physical file associated with it, which is not modified by any calls to this function.

	Some library implementations may support this function to be called multiple times, making the characters available in the reverse order in which they were put back. Although this behavior has no standard portability guarantees, and further calls may simply fail after any number of calls beyond the first.

	If successful, the function clears the end-of-file indicator of stream (if it was currently set), and decrements its internal file position indicator if it operates in binary mode; In text mode, the position indicator has unspecified value until all characters put back with ungetc have been read or discarded.

	A call to fseek, fsetpos or rewind on stream will discard any characters previously put back into it with this function.

	If the argument passed for the character parameter is EOF, the operation fails and the input stream remains unchanged.

	Parameters

	character
		The int promotion of the character to be put back.
		The value is internally converted to an unsigned char when put back.
	stream
		Pointer to a FILE object that identifies an input stream.


	Return Value
	On success, the character put back is returned.
	If the operation fails, EOF is returned.
*/

int FileInMemory_ungetc (int character, FileInMemory me) {
	Melder_assert (my isOpen);
	int result = EOF;
	if (character != EOF) {
		-- my d_position;
		result = my ungetChar = character;
	}
	return result;
}

void test_FileInMemorySet_io (void) {
	const conststring32 path1 = U"~/kanweg1.txt";
	const conststring32 path2 = U"~/kanweg2.txt";
	const conststring32 lines1 [3] = { U"abcd\n", U"ef\n",  U"ghijk\n" };
	const conststring32 lines2 [3] = { U"lmno\n", U"pqr\n",  U"stuvwxyz\n" };
	/*
		Create a test FileInMemorySet with two (text) files in it.
	*/
	MelderInfo_writeLine (U"test_FileInMemoryManager_io:");
	MelderInfo_writeLine (U"\tCreating two files: ", path1, U" and ", path2);
	structMelderFile s_file1 = {} , s_file2 = {};
	const MelderFile file1 = & s_file1, file2 = & s_file2;
	Melder_relativePathToFile (path1, file1);
	Melder_relativePathToFile (path2, file2);
	autoFileInMemorySet me = FileInMemorySet_create ();

	FILE *f = fopen (Melder_peek32to8_fileSystem (file1 -> path), "w");
	for (integer j = 0; j <= 2; j ++)
		fputs (Melder_peek32to8 (lines1 [j]), f);
	fclose (f);

	f = fopen (Melder_peek32to8_fileSystem (file2 -> path), "w");
	for (integer j = 0; j <= 2; j ++)
		fputs (Melder_peek32to8 (lines2 [j]), f);
	fclose (f);
	
	MelderInfo_writeLine (U"\tCreating FileInMemorySet from two files...");
	
	autoFileInMemory fim1 = FileInMemory_create (file1);
	my addItem_move (fim1.move());
	autoFileInMemory fim2 = FileInMemory_create (file2);
	my addItem_move (fim2.move());

	/*
		Test
	*/

	// fopen test
	MelderInfo_writeLine (U"\tOpen file ", file1 -> path);
	FileInMemory f1 = FileInMemorySet_fopen (me.get(), Melder_peek32to8_fileSystem (file1 -> path), "r");
	MelderInfo_writeLine (U"\t\t ...opened");
	
	MelderInfo_writeLine (U"\tOpen file ", file2 -> path);
	FileInMemory f2 = FileInMemorySet_fopen (me.get(), Melder_peek32to8_fileSystem (file2 -> path), "r");
	MelderInfo_writeLine (U"\t\t ...opened");
	
	FileInMemory_fclose (f2);
	MelderInfo_writeLine (U"\tClosed file ", file2 -> path);
	
	// read from open text file
	
	MelderInfo_writeLine (U"\tRead as text file in memory: ", file1 -> path);
	char buf0 [200], buf1 [200];
	const long nbuf = 200;
	
	FILE *file0 = fopen (Melder_peek32to8_fileSystem (file1 -> path), "r");
	for (integer i = 0; i <= 2; i ++) {
		char *p0 = fgets (buf0, nbuf, file0);
		const integer pos0 = ftell (file0);
		char *p1 = FileInMemory_fgets (buf1, nbuf, f1);
		const integer pos1 = FileInMemory_ftell (f1);
		Melder_assert (Melder_equ (Melder_peek8to32 (buf0), Melder_peek8to32 (buf1)));
		Melder_assert (pos0 == pos1);
		Melder_assert (p0 == buf0 && p1 == buf1);
		MelderInfo_writeLine (U"\t\tRead 1 line. Positions: ", pos0, U" and ", pos1);
	}

	MelderInfo_writeLine (U"\t\tRead while at EOF, returns nullptr");	
	char *shouldbenull = FileInMemory_fgets (buf1, nbuf, f1);
	Melder_assert (shouldbenull == nullptr);
	
	MelderInfo_writeLine (U"\tFinished reading... rewind ");
	
	// read as binary file
	
	rewind (file0);
	FileInMemory_rewind (f1);
	
	MelderInfo_writeLine (U"\tRead as binary file in memory: ", file1 -> path);
	
	//Melder_assert (fim -> d_position == 0);
	const integer count = 8;
	size_t nread0 = fread (buf0, 1, count, file0);
	size_t nread1 = FileInMemory_fread (buf1, 1, count, f1);
	MelderInfo_writeLine (U"\t\tRead ", nread0, U" and ", nread1, U" bytes");
	
	Melder_assert (nread0 == nread0);
	//Melder_assert (fim -> d_position == count);

	nread0 = fread (buf0, 1, count, file0);
	nread1 = FileInMemory_fread (buf1, 1, count, f1);
	MelderInfo_writeLine (U"\t\tRead ", nread0, U" and ", nread1, U" bytes");
	Melder_assert (nread0 == nread1);
	
	const int eof0 = feof (file0);
	const int eof1 = FileInMemory_feof (f1);
	MelderInfo_writeLine (U"\tEOF ? ", eof0, U" and ", eof1);
	
	Melder_assert (eof0 != 0 && eof1 != 0);
	
	//  clean up
	
	MelderFile_delete (file1);
	MelderFile_delete (file2);
	
	MelderInfo_writeLine (U"test_FileInMemoryManager_io: OK");
}

void structFileInMemorySet :: v1_info () {
	MelderInfo_writeLine (U"Number of files: ", size);
	MelderInfo_writeLine (U"Total number of bytes: ", FileInMemorySet_getTotalNumberOfBytes (this));
}

integer FileInMemorySet_getTotalNumberOfBytes (FileInMemorySet me) {
	integer numberOfBytes = 0;
	for (integer ifile = 1; ifile <= my size; ifile ++) {
		const FileInMemory fim = my at [ifile];
		numberOfBytes += fim -> d_numberOfBytes;
	}
	return numberOfBytes;
}

autoFileInMemorySet FileInMemorySets_merge (OrderedOf<structFileInMemorySet>& list) {
	try {
		autoFileInMemorySet thee = Data_copy (list.at [1]);
		for (integer iset = 1; iset <= list.size; iset ++)
			thy merge (list.at [iset]);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySets not merged.");
	}
}

autoFileInMemorySet FileInMemorySet_createFromDirectoryContents (conststring32 dirpath, conststring32 fileGlobber) {
	try {
		structMelderFolder parent { };
		Melder_pathToFolder (dirpath, & parent);
		autoStrings thee = Strings_createAsFileList (Melder_cat (dirpath, U"/", fileGlobber));
		Melder_require (thy numberOfStrings > 0,
			U"No files found.");

		autoFileInMemorySet me = FileInMemorySet_create ();
		for (integer i = 1; i <= thy numberOfStrings; i ++) {
			structMelderFile file { };
			MelderFolder_getFile (& parent, thy strings [i].get(), & file);
			my addItem_move (FileInMemory_create (& file));
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySet not created from directory \"", dirpath, U"\" for files that match \"", fileGlobber, U"\".");
	}
}

autoFileInMemorySet FilesInMemory_to_FileInMemorySet (OrderedOf<structFileInMemory>& list) {
	try {
		autoFileInMemorySet thee = FileInMemorySet_create ();
		for (integer ifile = 1; ifile <= list.size; ifile ++)
			thy addItem_move (Data_copy (list.at [ifile]));
		return thee;
	} catch (MelderError) {
		Melder_throw (U"FilesInMemory not collected in FileInMemorySet.");
	}
	
}

autoFileInMemorySet FileInMemorySet_extractFiles (FileInMemorySet me, kMelder_string which, conststring32 criterion) {
	try {
		autoFileInMemorySet thee = Thing_new (FileInMemorySet);
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = my at [ifile];
			if (Melder_stringMatchesCriterion (fim -> string.get(), which, criterion, true))
				thy addItem_move (Data_copy (fim));
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot extract files.");
	}
}

autoFileInMemorySet FileInMemorySet_removeFiles (FileInMemorySet me, kMelder_string which, conststring32 criterion) {
	try {
		autoFileInMemorySet thee = Thing_new (FileInMemorySet);
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = my at [ifile];
			if (Melder_stringMatchesCriterion (fim -> string.get(), which, criterion, true))
				thy addItem_move (my subtractItem_move (ifile));
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot extract files.");
	}
}

autoFileInMemorySet FileInMemorySet_listFiles (FileInMemorySet me, kMelder_string which, conststring32 criterion) {
	try {
		autoFileInMemorySet thee = Thing_new (FileInMemorySet);
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = my at [ifile];
			if (Melder_stringMatchesCriterion (fim -> string.get(), which, criterion, true))
				thy addItem_ref (fim);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot extract files.");
	}
}

void FileInMemorySet_showAsCode (FileInMemorySet me, conststring32 name, integer numberOfBytesPerLine) {
	autoMelderString one_fim;
	MelderInfo_writeLine (U"#include \"FileInMemory.h\"\n");
	MelderInfo_writeLine (U"autoFileInMemorySet create_", name, U" () {");
	MelderInfo_writeLine (U"\ttry {");
	MelderInfo_writeLine (U"\t\tautoFileInMemorySet me = FileInMemorySet_create ();");
	for (integer ifile = 1; ifile <= my size; ifile ++) {
		const FileInMemory fim = my at [ifile];
		MelderString_copy (& one_fim, name, ifile);
		FileInMemory_showAsCode (fim, one_fim.string, numberOfBytesPerLine);
		MelderInfo_writeLine (U"\t\tmy addItem_move (", one_fim.string, U".move());\n");
	}
	MelderInfo_writeLine (U"\t\treturn me;");
	MelderInfo_writeLine (U"\t} catch (MelderError) {");
	MelderInfo_writeLine (U"\t\tMelder_throw (U\"FileInMemorySet not created.\");");
	MelderInfo_writeLine (U"\t}");
	MelderInfo_writeLine (U"}\n");
}

void FileInMemorySet_showOneFileAsCode (FileInMemorySet me, integer index, conststring32 name, integer numberOfBytesPerLine) {
	if (index < 1 || index > my size)
		return;
	MelderInfo_writeLine (U"#include \"FileInMemory.h\"\n");
	MelderInfo_writeLine (U"static autoFileInMemory create_new_object () {");
	MelderInfo_writeLine (U"\ttry {");
	autoMelderString one_fim;
	const FileInMemory fim = my at [index];
	MelderString_append (& one_fim, name, index);
	FileInMemory_showAsCode (fim, U"me", numberOfBytesPerLine);
	MelderInfo_writeLine (U"\t\treturn me;");
	MelderInfo_writeLine (U"\t} catch (MelderError) {");
	MelderInfo_writeLine (U"\t\tMelder_throw (U\"FileInMemory not created.\");");
	MelderInfo_writeLine (U"\t}");
	MelderInfo_writeLine (U"}\n\n");
	MelderInfo_writeLine (U"autoFileInMemory ", name, U" = create_new_object ();");
}

integer FileInMemorySet_findNumberOfMatches_path (FileInMemorySet me, kMelder_string which, conststring32 criterion) {
	integer numberOfMatches = 0;
	for (integer ifile = 1; ifile <= my size; ifile ++) {
		const FileInMemory fim = my at [ifile];
		if (Melder_stringMatchesCriterion (fim -> string.get(), which, criterion, true))
			numberOfMatches ++;
	}
	return numberOfMatches;
}

bool FileInMemorySet_hasDirectory (FileInMemorySet me, conststring32 name) {
	autoMelderString searchString;
	MelderString_append (& searchString, U"/", name, U"/");
	for (integer i = 1; i <= my size; i ++) {
		const FileInMemory fim = my at [i];
		if (str32str (fim -> string.get(), searchString.string))
			return true;
	}
	return false;
}

autoStrings FileInMemorySet_to_Strings_path (FileInMemorySet me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = autoSTRVEC (my size);
		thy numberOfStrings = 0;
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = my at [ifile];
			thy strings [ifile] = Melder_dup_f (fim -> string.get());
			thy numberOfStrings ++;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No Strings created from FilesinMemory.");
	}
}

/* End of file FileInMemory.cpp */
