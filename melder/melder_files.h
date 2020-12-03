#ifndef _melder_files_h_
#define _melder_files_h_
/* melder_files.h
 *
 * Copyright (C) 1992-2018,2020 Paul Boersma
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

#define kMelder_MAXPATH 1023   /* excluding the null byte */

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

struct structMelderDir {
	char32 path [kMelder_MAXPATH+1];
};
typedef struct structMelderDir *MelderDir;

conststring32 MelderFile_name (MelderFile file);
conststring32 MelderDir_name (MelderDir dir);
void Melder_pathToDir (conststring32 path, MelderDir dir);
void Melder_pathToFile (conststring32 path, MelderFile file);
void Melder_relativePathToFile (conststring32 path, MelderFile file);
conststring32 Melder_dirToPath (MelderDir dir);
	/* Returns a pointer internal to 'dir', like "/u/paul/praats" or "D:\Paul\Praats" */
conststring32 Melder_fileToPath (MelderFile file);
void MelderFile_copy (MelderFile file, MelderFile copy);
void MelderDir_copy (MelderDir dir, MelderDir copy);
bool MelderFile_equal (MelderFile file1, MelderFile file2);
bool MelderDir_equal (MelderDir dir1, MelderDir dir2);
void MelderFile_setToNull (MelderFile file);
bool MelderFile_isNull (MelderFile file);
void MelderDir_setToNull (MelderDir dir);
bool MelderDir_isNull (MelderDir dir);
void MelderDir_getFile (MelderDir parent, conststring32 fileName, MelderFile file);
void MelderDir_relativePathToFile (MelderDir dir, conststring32 path, MelderFile file);
void MelderFile_getParentDir (MelderFile file, MelderDir parent);
void MelderDir_getParentDir (MelderDir file, MelderDir parent);
bool MelderDir_isDesktop (MelderDir dir);
void MelderDir_getSubdir (MelderDir parent, conststring32 subdirName, MelderDir subdir);
void Melder_rememberShellDirectory ();
conststring32 Melder_getShellDirectory ();
void Melder_getHomeDir (MelderDir homeDir);
void Melder_getPrefDir (MelderDir prefDir);
void Melder_getTempDir (MelderDir tempDir);

bool MelderFile_exists (MelderFile file);
bool MelderFile_readable (MelderFile file);
bool Melder_tryToWriteFile (MelderFile file);
bool Melder_tryToAppendFile (MelderFile file);
integer MelderFile_length (MelderFile file);
void MelderFile_delete (MelderFile file);

/* The following two should be combined with each other and with Windows extension setting: */
FILE * Melder_fopen (MelderFile file, const char *type);
void Melder_fclose (MelderFile file, FILE *stream);
void Melder_files_cleanUp ();

/* Use the following functions to pass unchanged text or file names to Melder_* functions. */
/* Backslashes are replaced by "\bs". */
/* The trick is that they return one of 11 cyclically used static strings, */
/* so you can use up to 11 strings in a single Melder_* call. */
char32 * Melder_peekExpandBackslashes (conststring32 message);
conststring32 MelderFile_messageName (MelderFile file);   // calls Melder_peekExpandBackslashes ()

void Melder_createDirectory (MelderDir parent, conststring32 subdirName, int mode);

void Melder_getDefaultDir (MelderDir dir);
void Melder_setDefaultDir (MelderDir dir);
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
	void reset (FILE *f) {
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

class autoMelderSaveDefaultDir {
	structMelderDir _savedDir;
public:
	autoMelderSaveDefaultDir () {
		Melder_getDefaultDir (& our _savedDir);
	}
	~autoMelderSaveDefaultDir () {
		Melder_setDefaultDir (& our _savedDir);
	}
	/*
		Disable copying.
	*/
	autoMelderSaveDefaultDir (const autoMelderSaveDefaultDir&) = delete;   // disable copy constructor
	autoMelderSaveDefaultDir& operator= (const autoMelderSaveDefaultDir&) = delete;   // disable copy assignment
};

class autoMelderSetDefaultDir {
	structMelderDir _savedDir;
public:
	autoMelderSetDefaultDir (MelderDir dir) {
		Melder_getDefaultDir (& our _savedDir);
		Melder_setDefaultDir (dir);
	}
	~autoMelderSetDefaultDir () {
		Melder_setDefaultDir (& our _savedDir);
	}
	/*
		Disable copying.
	*/
	autoMelderSetDefaultDir (const autoMelderSetDefaultDir&) = delete;   // disable copy constructor
	autoMelderSetDefaultDir& operator= (const autoMelderSetDefaultDir&) = delete;   // disable copy assignment
};

class autoMelderFileSetDefaultDir {
	structMelderDir _savedDir;
public:
	autoMelderFileSetDefaultDir (MelderFile file) {
		Melder_getDefaultDir (& our _savedDir);
		MelderFile_setDefaultDir (file);
	}
	~autoMelderFileSetDefaultDir () {
		Melder_setDefaultDir (& our _savedDir);
	}
	/*
		Disable copying.
	*/
	autoMelderFileSetDefaultDir (const autoMelderFileSetDefaultDir&) = delete;   // disable copy constructor
	autoMelderFileSetDefaultDir& operator= (const autoMelderFileSetDefaultDir&) = delete;   // disable copy assignment
};

/* Read and write whole text files. */
autostring32 MelderFile_readText (MelderFile file, autostring8 *string8 = nullptr);
void Melder_fwrite32to8 (conststring32 string, FILE *f);
void MelderFile_writeText (MelderFile file, conststring32 text, kMelder_textOutputEncoding outputEncoding);
void MelderFile_appendText (MelderFile file, conststring32 text);

/* End of file melder_files.h */
#endif
