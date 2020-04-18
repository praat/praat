/* FileInMemoryManager.cpp
 *
 * Copyright (C) 2017-2020 David Weenink
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

#include <errno.h>

/*
	File open and read emulations. The FILE * is internally used as a pointer to the index of the file in the Set.
	List of open files has to contain per file: index, position, length (bytes), pointer to data
*/

Thing_implement (FileInMemoryManager, Daata, 0);

void structFileInMemoryManager :: v_info () {
	FileInMemoryManager_Parent :: v_info ();
	MelderInfo_writeLine (U"Number of files: ", files -> size);
	MelderInfo_writeLine (U"Total number of bytes: ", FileInMemorySet_getTotalNumberOfBytes (files.get()));
}

bool FileInMemoryManager_hasDirectory (FileInMemoryManager me, conststring32 name) {
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

autoFileInMemory FileInMemoryManager_createFile (FileInMemoryManager me, MelderFile file) {
	try {
		autoFileInMemory thee = FileInMemory_create (file);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"Cannot create a FileInMemory object.");
	}
}

autoFileInMemorySet FileInMemoryManager_extractFiles (FileInMemoryManager me, kMelder_string which, conststring32 criterion) {
	return FileInMemorySet_extractFiles (my files.get(), which, criterion);
}

static integer _FileInMemoryManager_getIndexInOpenFiles (FileInMemoryManager me, FILE *stream) {
	const integer filesIndex = reinterpret_cast<integer> (stream);
	Melder_require (filesIndex > 0 && filesIndex <= my files -> size,
		U": Invalid file index: ", filesIndex);

	const FileInMemory fim = static_cast<FileInMemory> (my files -> at [filesIndex]);
	const integer openFilesIndex = FileInMemorySet_lookUp (my openFiles.get(), fim -> d_path.get());
	return openFilesIndex;
}

/* 
	From http://www.cplusplus.com/reference/cstdio
	FILE * fopen ( const char * filename, const char * mode );

	Open file
	Opens the file whose name is specified in the parameter filename and associates it with a stream that can be identified in future operations by the FILE pointer returned.

	The operations that are allowed on the stream and how these are performed are defined by the mode parameter.

	The returned stream is fully buffered by default if it is known to not refer to an interactive device (see setbuf).

	The returned pointer can be disassociated from the file by calling fclose or freopen. All opened files are automatically closed on normal program termination.

	The running environment supports at least FOPEN_MAX files open simultaneously.

	Parameters

	filename
		C string containing the name of the file to be opened.
		Its value shall follow the file name specifications of the running environment and can include a path (if supported by the system).
	mode
		C string containing a file access mode. It can be:
		"r"	read: Open file for input operations. The file must exist.
		"w"	write: Create an empty file for output operations. If a file with the same name already exists, its contents are discarded and the file is treated as a new empty file.
		"a"	append: Open file for output at the end of a file. Output operations always write data at the end of the file, expanding it. Repositioning operations (fseek, fsetpos, rewind) are ignored. The file is created if it does not exist.
		"r+"	read/update: Open a file for update (both for input and output). The file must exist.
		"w+"	write/update: Create an empty file and open it for update (both for input and output). If a file with the same name already exists its contents are discarded and the file is treated as a new empty file.
		"a+"	append/update: Open a file for update (both for input and output) with all output operations writing data at the end of the file. Repositioning operations (fseek, fsetpos, rewind) affects the next input operations, but output operations move the position back to the end of file. The file is created if it does not exist.
		With the mode specifiers above the file is open as a text file. In order to open a file as a binary file, a "b" character has to be included in the mode string. This additional "b" character can either be appended at the end of the string (thus making the following compound modes: "rb", "wb", "ab", "r+b", "w+b", "a+b") or be inserted between the letter and the "+" sign for the mixed modes ("rb+", "wb+", "ab+").

		The new C standard (C2011, which is not part of C++) adds a new standard subspecifier ("x"), that can be appended to any "w" specifier (to form "wx", "wbx", "w+x" or "w+bx"/"wb+x"). This subspecifier forces the function to fail if the file exists, instead of overwriting it.

		If additional characters follow the sequence, the behavior depends on the library implementation: some implementations may ignore additional characters so that for example an additional "t" (sometimes used to explicitly state a text file) is accepted.

		On some library implementations, opening or creating a text file with update mode may treat the stream instead as a binary file.


	Text files are files containing sequences of lines of text. Depending on the environment where the application runs, some special character conversion may occur in input/output operations in text mode to adapt them to a system-specific text file format. Although on some environments no conversions occur and both text files and binary files are treated the same way, using the appropriate mode improves portability.

	For files open for update (those which include a "+" sign), on which both input and output operations are allowed, the stream shall be flushed (fflush) or repositioned (fseek, fsetpos, rewind) before a reading operation that follows a writing operation. The stream shall be repositioned (fseek, fsetpos, rewind) before a writing operation that follows a reading operation (whenever that operation did not reach the end-of-file).

	Return Value
	If the file is successfully opened, the function returns a pointer to a FILE object that can be used to identify the stream on future operations.
	Otherwise, a null pointer is returned.
	On most library implementations, the errno variable is also set to a system-specific error code on failure.
*/
FILE *FileInMemoryManager_fopen (FileInMemoryManager me, const char *filename, const char *mode) {
	try {
		integer index = 0;
		if (*mode == 'r') { // also covers mode == 'rb'
			index = FileInMemorySet_lookUp (my files.get(), Melder_peek8to32(filename));
			if (index > 0) {
				const FileInMemory fim = (FileInMemory) my files -> at [index];
				if (fim -> d_position == 0) // not open
					my openFiles -> addItem_ref (fim);
				else // reset position
					fim -> d_position = 0;
			} else {
				// file does not exist, set error condition?
			}
		} else if (*mode == 'w') {
			
		}
		return reinterpret_cast<FILE *> (index);
	} catch (MelderError) {
		Melder_throw (U"File ", Melder_peek8to32(filename), U" cannot be opended.");
	}
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	void rewind ( FILE * stream );

	Set position of stream to the beginning
	Sets the position indicator associated with stream to the beginning of the file.

	The end-of-file and error internal indicators associated to the stream are cleared after a successful call to this function, and all effects from previous calls to ungetc on this stream are dropped.

	On streams open for update (read+write), a call to rewind allows to switch between reading and writing.

	Parameters

	stream
		Pointer to a FILE object that identifies the stream.


	Return Value
	none
*/
void FileInMemoryManager_rewind (FileInMemoryManager me, FILE *stream) {
	const integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	if (openFilesIndex > 0) {
		const FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
		fim -> d_position = 0;
		fim -> d_errno = 0;
		fim -> ungetChar = -1;
	}
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int fclose ( FILE * stream );

	Close file
	Closes the file associated with the stream and disassociates it.

	All internal buffers associated with the stream are disassociated from it and flushed: the content of any unwritten output buffer is written and the content of any unread input buffer is discarded.

	Even if the call fails, the stream passed as parameter will no longer be associated with the file nor its buffers.

	Parameters

	stream
		Pointer to a FILE object that specifies the stream to be closed. 


	Return Value
	If the stream is successfully closed, a zero value is returned.
	On failure, EOF is returned.
*/
int FileInMemoryManager_fclose (FileInMemoryManager me, FILE *stream) {
	const integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	if (openFilesIndex > 0) {
		const FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
		fim -> d_position = 0;
		fim -> d_errno = 0;
		fim -> ungetChar = -1;
		my openFiles -> removeItem (openFilesIndex);
	}
	return my errorNumber = 0; // always ok
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
int FileInMemoryManager_feof (FileInMemoryManager me, FILE *stream) {
	const integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	int eof = 0;
	if (openFilesIndex > 0) {
		const FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
		if (fim -> d_position >= fim -> d_numberOfBytes)
			eof = 1;
	}
	return eof;
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int fseek ( FILE * stream, long int offset, int origin );

	Reposition stream position indicator
	Sets the position indicator associated with the stream to a new position.

	For streams open in binary mode, the new position is defined by adding offset to a reference position specified by origin.

	For streams open in text mode, offset shall either be zero or a value returned by a previous call to ftell, and origin shall necessarily be SEEK_SET.

	If the function is called with other values for these arguments, support depends on the particular system and library implementation (non-portable).

	The end-of-file internal indicator of the stream is cleared after a successful call to this function, and all effects from previous calls to ungetc on this stream are dropped.

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
int FileInMemoryManager_fseek (FileInMemoryManager me, FILE *stream, integer offset, int origin) {
	const integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	int errval = EBADF;
	if (openFilesIndex > 0) {
		const FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
		integer newPosition = 0;
		if (origin == SEEK_SET)
			newPosition = offset;
		else if (origin == SEEK_CUR)
			newPosition = fim -> d_position + offset;
		else if (origin == SEEK_END)
			newPosition = fim -> d_numberOfBytes + offset;
		else
			return my errorNumber = EINVAL;

		if (newPosition < 0) // > numberOfBytes is allowed
			newPosition = 0;

		fim -> d_position = newPosition;
		fim -> ungetChar = -1;
		errval = 0;
	}
	return my errorNumber = errval;
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
integer FileInMemoryManager_ftell (FileInMemoryManager me, FILE *stream) {
	const integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	/* int errval = EBADF; */
	integer currentPosition = -1L;
	if (openFilesIndex > 0) {
		const FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
		currentPosition = fim -> d_position;
	}
	return currentPosition;
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	char * fgets ( char * str, int num, FILE * stream );

	Get string from stream
	Reads characters from stream and stores them as a C string into str until (num-1) characters have been read or either a newline or the end-of-file is reached, whichever happens first.

	A newline character makes fgets stop reading, but it is considered a valid character by the function and included in the string copied to str.

	A terminating null character is automatically appended after the characters copied to str.

	Notice that fgets is quite different from gets: not only fgets accepts a stream argument, but also allows to specify the maximum size of str and includes in the string any ending newline character.

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
char *FileInMemoryManager_fgets (FileInMemoryManager me, char *str, int num, FILE *stream) {
	const integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	char *result = nullptr;
	
	Melder_require (openFilesIndex > 0,
		U": File should be open.");

	FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
	integer startPos = fim -> d_position;
	if (startPos < fim -> d_numberOfBytes) {
		integer i = 0, endPos = startPos + num;
		endPos = endPos < fim -> d_numberOfBytes ? endPos : fim -> d_numberOfBytes;
		const unsigned char * p = fim -> d_data.asArgumentToFunctionThatExpectsZeroBasedArray () + startPos;
		char *p_str = str;
		if (fim -> ungetChar > 0) {
			/*
				copy the ungetChar and advance one position in stream
			*/
			*p_str ++ = fim -> ungetChar;
			p ++;
			i ++;
			fim -> ungetChar = -1;
		}
		while (i ++ < num && (*p_str ++ = *p) && *p ++ != '\n');
		str [i] = '\0';
		fim -> d_position += i;
		result = str; // everything ok, return the str pointer
	} else {
		fim -> d_errno = EOF;
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
int FileInMemoryManager_fgetc (FileInMemoryManager me, FILE *stream) {
	char str[4];
	(void) FileInMemoryManager_fgets (me, str, 1, stream);
	return FileInMemoryManager_feof (me, stream) ? EOF : static_cast<int> (*str);
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
size_t FileInMemoryManager_fread (FileInMemoryManager me, void *ptr, size_t size, size_t count, FILE *stream) {
	const integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
	
	Melder_require (openFilesIndex > 0 && size > 0 && count > 0,
		U": File should be open.");
	
	const FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
	size_t result = 0;
	integer startPos = fim -> d_position;
	if (startPos < fim -> d_numberOfBytes) {
		integer i = 0, endPos = startPos + count * size;
		
		if (endPos > fim -> d_numberOfBytes) {
			count = (fim -> d_numberOfBytes - startPos) / size;
			endPos = startPos + count * size;
			fim -> d_errno = EOF;
		}
		const integer numberOfBytes = count * size;
		const unsigned char * p = fim -> d_data.asArgumentToFunctionThatExpectsZeroBasedArray () + fim -> d_position;
		char * str = static_cast<char *> (ptr);
		while (i < numberOfBytes)
			str [i ++] = *p ++;
		fim -> d_position = endPos;
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

int FileInMemoryManager_ungetc (FileInMemoryManager me, int character, FILE * stream) {
	int result = EOF;
	if (character != EOF) {
		const integer openFilesIndex = _FileInMemoryManager_getIndexInOpenFiles (me, stream);
		if (openFilesIndex > 0) {
			const FileInMemory fim = static_cast<FileInMemory> (my openFiles -> at [openFilesIndex]);
			-- (fim -> d_position);
			result = fim -> ungetChar = character;
		}
	}
	return result;
}


/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int fprintf ( FILE * stream, const char * format, ... );

	Write formatted data to stream
	Writes the C string pointed by format to the stream. If format includes format specifiers (subsequences beginning with %), the additional arguments following format are formatted and inserted in the resulting string replacing their respective specifiers.

	After the format parameter, the function expects at least as many additional arguments as specified by format.
	
	Parameters

	stream
		Pointer to a FILE object that identifies an output stream.
	format
		C string that contains the text to be written to the stream.
		It can optionally contain embedded format specifiers that are replaced by the values specified in subsequent additional arguments and formatted as requested.

		A format specifier follows this prototype:

		%[flags][width][.precision][length]specifier

		Where the specifier character at the end is the most significant component, since it defines the type and the interpretation of its corresponding argument:
		specifier	Output	Example
		d or i	Signed decimal integer	392
		u	Unsigned decimal integer	7235
		o	Unsigned octal	610
		x	Unsigned hexadecimal integer	7fa
		X	Unsigned hexadecimal integer (uppercase)	7FA
		f	Decimal floating point, lowercase	392.65
		F	Decimal floating point, uppercase	392.65
		e	Scientific notation (mantissa/exponent), lowercase	3.9265e+2
		E	Scientific notation (mantissa/exponent), uppercase	3.9265E+2
		g	Use the shortest representation: %e or %f	392.65
		G	Use the shortest representation: %E or %F	392.65
		a	Hexadecimal floating point, lowercase	-0xc.90fep-2
		A	Hexadecimal floating point, uppercase	-0XC.90FEP-2
		c	Character	a
		s	String of characters	sample
		p	Pointer address	b8000000
		n	Nothing printed.
		The corresponding argument must be a pointer to a signed int.
		The number of characters written so far is stored in the pointed location.	
		%	A % followed by another % character will write a single % to the stream.	%

		The format specifier can also contain sub-specifiers: flags, width, .precision and modifiers (in that order), which are optional and follow these specifications:

		flags	description
		-	Left-justify within the given field width; Right justification is the default (see width sub-specifier).
		+	Forces to preceed the result with a plus or minus sign (+ or -) even for positive numbers. By default, only negative numbers are preceded with a - sign.
		(space)	If no sign is going to be written, a blank space is inserted before the value.
		#	Used with o, x or X specifiers the value is preceeded with 0, 0x or 0X respectively for values different than zero.
		Used with a, A, e, E, f, F, g or G it forces the written output to contain a decimal point even if no more digits follow. By default, if no digits follow, no decimal point is written.
		0	Left-pads the number with zeroes (0) instead of spaces when padding is specified (see width sub-specifier).

		width	description
		(number)	Minimum number of characters to be printed. If the value to be printed is shorter than this number, the result is padded with blank spaces. The value is not truncated even if the result is larger.
		*	The width is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted.

		.precision	description
		.number	For integer specifiers (d, i, o, u, x, X): precision specifies the minimum number of digits to be written. If the value to be written is shorter than this number, the result is padded with leading zeros. The value is not truncated even if the result is longer. A precision of 0 means that no character is written for the value 0.
		For a, A, e, E, f and F specifiers: this is the number of digits to be printed after the decimal point (by default, this is 6).
		For g and G specifiers: This is the maximum number of significant digits to be printed.
		For s: this is the maximum number of characters to be printed. By default all characters are printed until the ending null character is encountered.
		If the period is specified without an explicit value for precision, 0 is assumed.
		.*	The precision is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted.

		The length sub-specifier modifies the length of the data type. This is a chart showing the types used to interpret the corresponding arguments with and without length specifier (if a different type is used, the proper type promotion or conversion is performed, if allowed):
			specifiers
		length	d i	u o x X	f F e E g G a A	c	s	p	n
		(none)	int	unsigned int	double	int	char*	void*	int*
		hh	signed char	unsigned char					signed char*
		h	short int	unsigned short int					short int*
		l	long int	unsigned long int		wint_t	wchar_t*		long int*
		ll	long long int	unsigned long long int					long long int*
		j	intmax_t	uintmax_t					intmax_t*
		z	size_t	size_t					size_t*
		t	ptrdiff_t	ptrdiff_t					ptrdiff_t*
		L			long double				
		Note that the c specifier takes an int (or wint_t) as argument, but performs the proper conversion to a char value (or a wchar_t) before formatting it for output.

		Note: Yellow rows indicate specifiers and sub-specifiers introduced by C99. See <cinttypes> for the specifiers for extended types.
	... (additional arguments)
		Depending on the format string, the function may expect a sequence of additional arguments, each containing a value to be used to replace a format specifier in the format string (or a pointer to a storage location, for n).
		There should be at least as many of these arguments as the number of values specified in the format specifiers. Additional arguments are ignored by the function.


	Return Value
		On success, the total number of characters written is returned.

		If a writing error occurs, the error indicator (ferror) is set and a negative number is returned.

	If a multibyte character encoding error occurs while writing wide characters, errno is set to EILSEQ and a negative number is returned.
*/
int FileInMemoryManager_fprintf (FileInMemoryManager me, FILE * stream, const char *format, ... ) {
	(void) me;
	va_list args;
	if (stream == stderr) {
		va_start (args, format);
		int sizeNeeded = vsnprintf (nullptr, 0, format, args); // find size of needed buffer (without final null byte)
		const size_t bufferSize = sizeNeeded + 1;
		va_end (args);
		return bufferSize;
	}
	return -1;
}

void test_FileInMemoryManager_io (void) {
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
	autoFileInMemorySet fims = FileInMemorySet_create ();

	FILE *f = fopen (Melder_peek32to8 (file1 -> path), "w");
	for (integer j = 0; j <= 2; j ++)
		fputs (Melder_peek32to8 (lines1 [j]), f);
	
	fclose (f);

	f = fopen (Melder_peek32to8 (file2 -> path), "w");
	for (integer j = 0; j <= 2; j ++)
		fputs (Melder_peek32to8 (lines2 [j]), f);
	
	fclose (f);
	
	MelderInfo_writeLine (U"\tCreating FileInMemorySet from two files...");
	
	autoFileInMemory fim1 = FileInMemory_create (file1);
	fims -> addItem_move (fim1.move());
	autoFileInMemory fim2 = FileInMemory_create (file2);
	fims -> addItem_move (fim2.move());
	
	/*
		Create the FileInMemoryManager and test
	*/

	autoFileInMemoryManager me = FileInMemoryManager_create (fims.get());
	
	// fopen test
	MelderInfo_writeLine (U"\tOpen file ", file1 -> path);
	FILE * f1 = FileInMemoryManager_fopen (me.get(), Melder_peek32to8 (file1 -> path), "r");
	const integer openFilesIndex1 = _FileInMemoryManager_getIndexInOpenFiles (me.get(), f1);
	Melder_assert (openFilesIndex1 == 1);
	MelderInfo_writeLine (U"\t\t ...opened");
	
	MelderInfo_writeLine (U"\tOpen file ", file2 -> path);
	FILE * f2 = FileInMemoryManager_fopen (me.get(), Melder_peek32to8 (file2 -> path), "r");
	const integer openFilesIndex2 = _FileInMemoryManager_getIndexInOpenFiles (me.get(), f2);
	Melder_assert (openFilesIndex2 == 2);
	MelderInfo_writeLine (U"\t\t ...opened");
	
	FileInMemoryManager_fclose (me.get(), f2);
	Melder_assert (my openFiles -> size == 1);
	MelderInfo_writeLine (U"\tClosed file ", file2 -> path);
	
	// read from open text file
	
	MelderInfo_writeLine (U"\tRead as text file in memory: ", file1 -> path);
	char buf0 [200], buf1 [200];
	const long nbuf = 200;
	
	FileInMemory fim = (FileInMemory) my files -> at [openFilesIndex1];
	FILE *file0 = fopen (Melder_peek32to8 (file1 -> path), "r");
	for (integer i = 0; i <= 2; i ++) {
		char *p0 = fgets (buf0, nbuf, file0);
		const integer pos0 = ftell (file0);
		char *p1 = FileInMemoryManager_fgets (me.get(), buf1, nbuf, f1);
		const integer pos1 = FileInMemoryManager_ftell (me.get(), f1);
		Melder_assert (Melder_equ (Melder_peek8to32 (buf0), Melder_peek8to32 (buf1)));
		Melder_assert (pos0 == pos1);
		Melder_assert (p0 == buf0 && p1 == buf1);
		MelderInfo_writeLine (U"\t\tRead 1 line. Positions: ", pos0, U" and ", pos1);
	}

	MelderInfo_writeLine (U"\t\tRead while at EOF, returns nullptr");	
	char *shouldbenull = FileInMemoryManager_fgets (me.get(), buf1, nbuf, f1);
	Melder_assert (shouldbenull == nullptr);
	
	MelderInfo_writeLine (U"\tFinished reading... rewind ");
	
	// read as binary file
	
	rewind (file0);
	FileInMemoryManager_rewind (me.get(), f1);
	
	MelderInfo_writeLine (U"\tRead as binary file in memory: ", file1 -> path);
	
	Melder_assert (fim -> d_position == 0);
	const integer count = 8;
	size_t nread0 = fread (buf0, 1, count, file0);
	size_t nread1 =  FileInMemoryManager_fread (me.get(), buf1, 1, count, f1);
	MelderInfo_writeLine (U"\t\tRead ", nread0, U" and ", nread1, U" bytes");
	
	Melder_assert (nread0 == nread0);
	Melder_assert (fim -> d_position == count);
	
	nread0 = fread (buf0, 1, count, file0);
	nread1 = FileInMemoryManager_fread (me.get(), buf1, 1, count, f1);
	MelderInfo_writeLine (U"\t\tRead ", nread0, U" and ", nread1, U" bytes");
	Melder_assert (nread0 == nread1);
	
	const int eof0 = feof (file0);
	const int eof1 = FileInMemoryManager_feof (me.get(), f1);
	MelderInfo_writeLine (U"\tEOF ? ", eof0, U" and ", eof1);
	
	Melder_assert (eof0 != 0 && eof1 != 0);
	
	//  clean up
	
	MelderFile_delete (file1);
	MelderFile_delete (file2);
	
	MelderInfo_writeLine (U"test_FileInMemoryManager_io: OK");
}

/* End of file FileInMemoryManager.cpp */
