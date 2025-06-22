#ifndef _melder_files_h_
#define _melder_files_h_
/* melder_files.h
 *
 * Copyright (C) 1992-2018,2020-2024 Paul Boersma
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

#if defined (_WIN32)
	#define Melder_DIRECTORY_SEPARATOR  '\\'
#else
	#define Melder_DIRECTORY_SEPARATOR  '/'
#endif

struct FLAC__StreamDecoder;
struct FLAC__StreamEncoder;

#define kMelder_MAXPATH  1023   /* excluding the null character */

struct structMelderFile {
	FILE *filePointer;
	char32 path [kMelder_MAXPATH+1];
	enum class Format { none = 0, binary = 1, text = 2 } format;
	bool openForReading, openForWriting, verbose, requiresCRLF;
	uint32 outputEncoding;
	int indent;
	struct FLAC__StreamEncoder *flacEncoder;
};
typedef struct structMelderFile *MelderFile;
typedef const struct structMelderFile *constMelderFile;

struct structMelderFolder {
	char32 path [kMelder_MAXPATH+1];
};
typedef struct structMelderFolder *MelderFolder;
typedef const struct structMelderFolder *constMelderFolder;

conststring32 MelderFile_name (MelderFile file);
conststring32 MelderFolder_name (MelderFolder folder);
void Melder_pathToFile (conststring32 path, MelderFile file);
void Melder_pathToFolder (conststring32 path, MelderFolder folder);
void Melder_relativePathToFile (conststring32 path, MelderFile file);
void Melder_relativePathToFolder (conststring32 path, MelderFolder folder);
conststring32 MelderFolder_peekPath (MelderFolder folder);
	/* Returns a pointer internal to 'folder', like "/u/paul/praats" or "D:\Paul\Praats" */
conststring8 MelderFolder_peekPath8 (MelderFolder folder);
conststringW MelderFolder_peekPathW (MelderFolder folder);
conststring32 MelderFile_peekPath (MelderFile file);
conststring8 MelderFile_peekPath8 (MelderFile file);
conststringW MelderFile_peekPathW (MelderFile file);
const void * MelderFile_peekPathCfstring (MelderFile file);
const void * MelderFolder_peekPathCfstring (MelderFolder folder);
void MelderFile_copy (constMelderFile file, MelderFile copy);
void MelderFolder_copy (constMelderFolder folder, MelderFolder copy);
bool MelderFile_equal (MelderFile file1, MelderFile file2);
bool MelderFolder_equal (MelderFolder folder1, MelderFolder folder2);
void MelderFile_setToNull (MelderFile file);
bool MelderFile_isNull (MelderFile file);
void MelderFolder_setToNull (MelderFolder folder);
bool MelderFolder_isNull (MelderFolder folder);
void MelderFolder_getFile (MelderFolder parent, conststring32 fileName, MelderFile file);
void MelderFolder_relativePathToFile (MelderFolder folder, conststring32 path, MelderFile file);
void MelderFile_getParentFolder (MelderFile file, MelderFolder parent);
void MelderFolder_getParentFolder (MelderFolder file, MelderFolder parent);
bool MelderFolder_isDesktop (MelderFolder folder);
void MelderFolder_getSubfolder (MelderFolder parentFolder, conststring32 subfolderName, MelderFolder subfolder);
void Melder_rememberShellDirectory ();
conststring32 Melder_getShellDirectory ();
void Melder_getHomeDir (MelderFolder homeDir);
MelderFolder Melder_preferencesFolder5 (), Melder_preferencesFolder7 (), Melder_preferencesFolder ();
void Melder_setPreferencesFolder (conststring32 path);
void Melder_getTempDir (MelderFolder tempDir);

bool MelderFile_exists (MelderFile file);
bool MelderFolder_exists (MelderFolder folder);
bool MelderFile_readable (MelderFile file);
bool Melder_tryToWriteFile (MelderFile file);
bool Melder_tryToAppendFile (MelderFile file);
integer MelderFile_length (MelderFile file);
void MelderFile_delete (MelderFile file);
void MelderFile_moveAndOrRename (MelderFile from, MelderFile to);

/* The following two should be combined with each other and with Windows extension setting: */
FILE * Melder_fopen (MelderFile file, const char *type);
void Melder_fclose (MelderFile file, FILE *stream);

char32 * Melder_peekExpandBackslashes (conststring32 message);   // replace backslashes with "\bs"

/*
	Use the following functions to pass unchanged text or file names to Melder_* functions.
	The trick is that they return one of 11 cyclically used static strings,
	so you can use up to 11 strings in a single Melder_* call.
*/
conststring32 MelderFile_messageName (MelderFile file);   // calls Melder_peekExpandBackslashes ()
conststring32 MelderFolder_messageName (MelderFolder folder);   // calls Melder_peekExpandBackslashes ()

void Melder_createDirectory (MelderFolder parent, conststring32 subdirName, int mode);
void MelderFolder_create (MelderFolder folder);

void Melder_getCurrentFolder (MelderFolder folder);
void Melder_setCurrentFolder (MelderFolder folder);
void MelderFile_setDefaultDir (MelderFile file);

class autofile {
	FILE *ptr;
public:
	autofile (FILE *f) : ptr (f) {
	}
	autofile () : ptr (nullptr) {
	}
	~autofile () {
		if (ptr)
			fclose (ptr);   // no error checking, because this is a destructor, only called after a throw, because otherwise you'd use f.close(file)
	}
	operator FILE * () {
		return ptr;
	}
	void operator= (FILE *f) {
		if (ptr)
			fclose (ptr);   // BUG: not a normal closure
		ptr = f;
	}
	void close (MelderFile file) {
		if (ptr) {
			FILE *tmp = ptr;
			ptr = nullptr;
			Melder_fclose (file, tmp);
		}
	}
};

class autoMelderSaveCurrentFolder {
	structMelderFolder _savedFolder;
public:
	autoMelderSaveCurrentFolder () {
		Melder_getCurrentFolder (& our _savedFolder);
	}
	~autoMelderSaveCurrentFolder () {
		Melder_setCurrentFolder (& our _savedFolder);
	}
	/*
		Disable copying.
	*/
	autoMelderSaveCurrentFolder (const autoMelderSaveCurrentFolder&) = delete;   // disable copy constructor
	autoMelderSaveCurrentFolder& operator= (const autoMelderSaveCurrentFolder&) = delete;   // disable copy assignment
};

class autoMelderSetCurrentFolder {
	structMelderFolder _savedFolder;
public:
	autoMelderSetCurrentFolder (MelderFolder folder) {
		Melder_getCurrentFolder (& our _savedFolder);
		Melder_setCurrentFolder (folder);
	}
	~autoMelderSetCurrentFolder () {
		Melder_setCurrentFolder (& our _savedFolder);
	}
	/*
		Disable copying.
	*/
	autoMelderSetCurrentFolder (const autoMelderSetCurrentFolder&) = delete;   // disable copy constructor
	autoMelderSetCurrentFolder& operator= (const autoMelderSetCurrentFolder&) = delete;   // disable copy assignment
};

class autoMelderFileSetCurrentFolder {
	structMelderFolder _savedFolder;
public:
	autoMelderFileSetCurrentFolder (MelderFile file) {
		Melder_getCurrentFolder (& our _savedFolder);
		MelderFile_setDefaultDir (file);
	}
	~autoMelderFileSetCurrentFolder () {
		Melder_setCurrentFolder (& our _savedFolder);
	}
	/*
		Disable copying.
	*/
	autoMelderFileSetCurrentFolder (const autoMelderFileSetCurrentFolder&) = delete;   // disable copy constructor
	autoMelderFileSetCurrentFolder& operator= (const autoMelderFileSetCurrentFolder&) = delete;   // disable copy assignment
};

/* Read and write whole text files. */
autostring32 MelderFile_readText (MelderFile file, autostring8 *string8 = nullptr);
void Melder_fwrite32to8 (conststring32 string, FILE *f);
void MelderFile_writeText (MelderFile file, conststring32 text, kMelder_textOutputEncoding outputEncoding);
void MelderFile_appendText (MelderFile file, conststring32 text);

/* End of file melder_files.h */
#endif
