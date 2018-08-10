#ifndef _melder_file_h_
#define _melder_file_h_
/* MelderFile.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/* These will be the future replacements for Melder_fopen, as soon as we rid of text files: */
MelderFile MelderFile_open (MelderFile file);
MelderFile MelderFile_append (MelderFile file);
MelderFile MelderFile_create (MelderFile file);
void * MelderFile_read (MelderFile file, integer nbytes);
char * MelderFile_readLine (MelderFile file);
void MelderFile_writeCharacter (MelderFile file, char32 kar);

void _MelderFile_write (MelderFile file, conststring32 string);

inline static void _recursiveTemplate_MelderFile_write (MelderFile file, const MelderArg& arg) {
	_MelderFile_write (file, arg. _arg);
}
template <typename... Args>
void _recursiveTemplate_MelderFile_write (MelderFile file, const MelderArg& first, Args... rest) {
	_recursiveTemplate_MelderFile_write (file, first);
	_recursiveTemplate_MelderFile_write (file, rest...);
}

template <typename... Args>
void MelderFile_write (MelderFile file, const MelderArg& first, Args... rest) {
	if (! file -> filePointer)
		return;
	_recursiveTemplate_MelderFile_write (file, first, rest...);
}

void MelderFile_rewind (MelderFile file);
void MelderFile_seek (MelderFile file, integer position, int direction);
integer MelderFile_tell (MelderFile file);
void MelderFile_close (MelderFile file);
void MelderFile_close_nothrow (MelderFile file);

/* Read and write whole text files. */
autostring32 MelderFile_readText (MelderFile file);
void MelderFile_writeText (MelderFile file, conststring32 text, kMelder_textOutputEncoding outputEncoding);
void MelderFile_appendText (MelderFile file, conststring32 text);

class autoMelderFile {
	MelderFile _file;
public:
	autoMelderFile (MelderFile file) : _file (file) {
	}
	~autoMelderFile () {
		if (_file)
			MelderFile_close_nothrow (_file);
	}
	void close () {
		if (_file && _file -> filePointer) {
			MelderFile tmp = _file;
			_file = nullptr;
			MelderFile_close (tmp);
		}
	}
	MelderFile transfer () {
		MelderFile tmp = _file;
		_file = nullptr;
		return tmp;
	}
};

/* End of file MelderFile.h */
#endif
