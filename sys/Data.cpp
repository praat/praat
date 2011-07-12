/* Data.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/03/07 GPL
 * pb 2003/07/02 Data_copy returns NULL if argument is NULL
 * pb 2003/09/14 old ClassTextFile formant readable across systems
 * pb 2004/10/16 C++ compatible struct tags
 * pb 2007/06/21 wchar_t
 * pb 2007/07/21 Data_canWriteAsEncoding
 * pb 2008/01/18 guarded against some crashes (-> Data me = NULL)
 * pb 2008/07/20 wchar_t
 * pb 2008/08/13 prevented overriding of header in file recognition
 * pb 2011/03/29 C++
 */

#include "Collection.h"

#undef our
#define our ((Data_Table) my methods) ->

structMelderDir Data_directoryBeingRead = { { 0 } };

static void copy (Any data1, Any data2) {
	(void) data1;
	(void) data2;
}

static bool equal (Any data1, Any data2) {
	(void) data1;
	(void) data2;
	return 1;
}   /* Names may be different. */

static bool canWriteAsEncoding (Any data, int encoding) {
	(void) data;
	(void) encoding;
	return 1;
}

static void writeText (Any data, MelderFile openFile) {
	(void) data;
	(void) openFile;
}

static void readText (Any data, MelderReadText text) {
	(void) data;
	(void) text;
}

static void writeBinary (Any data, FILE *f) {
	(void) data;
	(void) f;
}

static void readBinary (Any data, FILE *f) {
	(void) data;
	(void) f;
}

static void writeCache (Any data, CACHE *f) {
	(void) data;
	(void) f;
}

static void readCache (Any data, CACHE *f) {
	(void) data;
	(void) f;
}

static void writeLisp (Any data, FILE *f) {
	(void) data;
	(void) f;
}

static void readLisp (Any data, FILE *f) {
	(void) data;
	(void) f;
}

class_methods (Data, Thing) {
	class_method (copy)
	class_method (equal)
	class_method (canWriteAsEncoding)
	class_method (writeText)
	class_method (readText)
	class_method (writeBinary)
	class_method (readBinary)
	class_method (writeCache)
	class_method (readCache)
	class_method (writeLisp)
	class_method (readLisp)
	class_methods_end
}

Any Data_copy (I) {
	iam (Data);
	try {
		if (me == NULL) return NULL;
		if (our copy == classData -> copy) Melder_throw ("Objects of type ", our _className, " can never be copied.");
		autoData thee = (Data) _Thing_new (my methods);
		our copy (me, thee.peek()); therror
		Thing_setName (thee.peek(), my name);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not copied.");
	}
}

bool Data_equal (I, thou) {
	iam (Data); thouart (Data);
	if (my methods != thy methods) return false;   // different class: not equal
	int offset = sizeof (struct structData);   // we already compared the methods, and are going to skip the names
	if (! memcmp ((char *) me + offset, (char *) thee + offset, our _size - offset))
		return true;   // no shallow differences
	return our equal (me, thee);
}

bool Data_canWriteAsEncoding (I, int encoding) {
	iam (Data);
	return our canWriteAsEncoding (me, encoding);
}

bool Data_canWriteText (I) {
	iam (Data);
	return our writeText != classData -> writeText;
}

void Data_writeText (I, MelderFile openFile) {
	iam (Data);
	our writeText (me, openFile); therror
	if (ferror (openFile -> filePointer))
		Melder_throw ("I/O error.");
}

MelderFile Data_createTextFile (I, MelderFile file, bool verbose) {
	iam (Data);
	autoMelderFile mfile = MelderFile_create (file, L"TEXT", 0, L"txt");
	#if defined (_WIN32)
		file -> requiresCRLF = true;
	#endif
	file -> verbose = verbose;
	file -> outputEncoding = Melder_getOutputEncoding ();
	if (file -> outputEncoding == kMelder_textOutputEncoding_ASCII_THEN_UTF16)
		file -> outputEncoding = Data_canWriteAsEncoding (me, kMelder_textOutputEncoding_ASCII) ? kMelder_textOutputEncoding_ASCII : kMelder_textOutputEncoding_UTF16;
	else if (file -> outputEncoding == kMelder_textOutputEncoding_ISO_LATIN1_THEN_UTF16)
		file -> outputEncoding = Data_canWriteAsEncoding (me, kMelder_textOutputEncoding_ISO_LATIN1) ? kMelder_textOutputEncoding_ISO_LATIN1 : kMelder_textOutputEncoding_UTF16;
	if (file -> outputEncoding == kMelder_textOutputEncoding_UTF16) {
		binputu2 (0xfeff, file -> filePointer);
	}
	return mfile.transfer();
}


static void _Data_writeToTextFile (I, MelderFile file, bool verbose) {
	iam (Data);
	try {
		if (! Data_canWriteText (me))
			Melder_throw ("Objects of class ", our _className, " cannot be written to a text file.");
		autoMelderFile mfile = Data_createTextFile (me, file, verbose);
		#ifndef _WIN32
			flockfile (file -> filePointer);   // BUG
		#endif
		MelderFile_write2 (file, L"File type = \"ooTextFile\"\nObject class = \"", our _className);
		if (our version > 0)
			MelderFile_write2 (file, L" ", Melder_integer (our version));
		MelderFile_write1 (file, L"\"\n");
		Data_writeText (me, file);
		MelderFile_writeCharacter (file, '\n');
		#ifndef _WIN32
			if (file -> filePointer) funlockfile (file -> filePointer);
		#endif
		mfile.close ();
		MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	} catch (MelderError) {
		#ifndef _WIN32
			if (file -> filePointer) funlockfile (file -> filePointer);   // the file pointer is NULL before Data_createTextFile() and after mfile.close()
		#endif
		throw;
	}
}

void Data_writeToTextFile (I, MelderFile file) {
	iam (Data);
	try {
		_Data_writeToTextFile (me, file, true);
	} catch (MelderError) {
		Melder_throw (me, ": not written to text file ", MelderFile_messageName (file), ".");
	}
}

void Data_writeToShortTextFile (I, MelderFile file) {
	iam (Data);
	try {
		_Data_writeToTextFile (me, file, false);
	} catch (MelderError) {
		Melder_throw (me, ": not written to short text file ", MelderFile_messageName (file), ".");
	}
}

bool Data_canWriteBinary (I) {
	iam (Data);
	return our writeBinary != classData -> writeBinary;
}

void Data_writeBinary (I, FILE *f) {
	iam (Data);
	our writeBinary (me, f); therror
	if (ferror (f))
		Melder_throw ("I/O error.");
}

void Data_writeToBinaryFile (I, MelderFile file) {
	iam (Data);
	try {
		if (! Data_canWriteBinary (me))
			Melder_throw ("Objects of class ", our _className, L" cannot be written to a generic binary file.");
		autoMelderFile mfile = MelderFile_create (file, 0, 0, 0);
		if (fprintf (file -> filePointer, "ooBinaryFile") < 0)
			Melder_throw ("Cannot write first bytes of file.");
		binputw1 (our version > 0 ? Melder_wcscat3 (our _className, L" ", Melder_integer (our version)) : our _className, file -> filePointer);
		Data_writeBinary (me, file -> filePointer);
		mfile.close ();
		MelderFile_setMacTypeAndCreator (file, 'BINA', 0);
	} catch (MelderError) {
		Melder_throw (me, ": not written to binary file ", MelderFile_messageName (file), ".");
	}
}

bool Data_canWriteLisp (I) {
	iam (Data);
	return our writeLisp != classData -> writeLisp;
}

void Data_writeLisp (I, FILE *f) {
	iam (Data);
	our writeLisp (me, f); therror
	if (ferror (f))
		Melder_throw ("I/O error.");
}

void Data_writeLispToConsole (I) {
	iam (Data);
	try {
		if (! Data_canWriteLisp (me))
			Melder_throw ("Class ", our _className, " cannot be written as LISP.");
		wprintf (L"Write as LISP sequence to console: class %ls,  name \"%ls\".\n", Thing_className (me), my name ? my name : L"<none>");
		Data_writeLisp (me, stdout);
	} catch (MelderError) {
		Melder_throw (me, ": not written as LISP text to console.");
	}
}

void Data_writeToLispFile (I, MelderFile file) {
	iam (Data);
	try {
		if (! Data_canWriteLisp (me))
			Melder_throw ("Class ", our _className, L" cannot be written as LISP.");
		autofile f = Melder_fopen (file, "w");
		if (fprintf (f, "%sLispFile\n", Melder_peekWcsToUtf8 (our _className)) == EOF)
			Melder_throw ("Error while writing file. Disk probably full.");
		Data_writeLisp (me, f); therror
		f.close (file);
		MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	} catch (MelderError) {
		Melder_throw (me, ": not written to LISP file ", Thing_messageName (me), ".");
	}
}

bool Data_canReadText (I) {
	iam (Data);
	return our readText != classData -> readText;
}

void Data_readText (I, MelderReadText text) {
	iam (Data);
	try {
		our readText (me, text); therror
	} catch (MelderError) {
		Melder_throw (Thing_className (me), " not read.");
	}
}

Any Data_readFromTextFile (MelderFile file) {
	try {
		autoMelderReadText text = MelderReadText_createFromFile (file);
		wchar *line = MelderReadText_readLine (text.peek()); therror
		if (line == NULL)
			Melder_throw ("No lines.");
		wchar *end = wcsstr (line, L"ooTextFile");   // oo format?
		autoData me = NULL;
		if (end) {
			autostring klas = texgetw2 (text.peek());
			me.reset ((Data) Thing_newFromClassName (klas.peek()));
		} else {
			end = wcsstr (line, L"TextFile");
			if (end == NULL)
				Melder_throw ("Not an old-type text file; should not occur.");
			*end = '\0';
			me.reset ((Data) Thing_newFromClassName (line));
			Thing_version = -1;   // old version: override version number, which was set to 0 by newFromClassName
		}
		MelderFile_getParentDir (file, & Data_directoryBeingRead);
		Data_readText (me.peek(), text.peek()); therror
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Data not read from text file ", MelderFile_messageName (file), ".");
	}
}

bool Data_canReadBinary (I) {
	iam (Data);
	return our readBinary != classData -> readBinary;
}

void Data_readBinary (I, FILE *f) {
	iam (Data);
	try {
		our readBinary (me, f); therror
		if (feof (f))
			Melder_throw ("Early end of file.");
		if (ferror (f))
			Melder_throw ("I/O error.");
	} catch (MelderError) {
		Melder_throw (Thing_className (me), " not read.");
	}
}

Any Data_readFromBinaryFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");
		char line [200];
		int n = fread (line, 1, 199, f); line [n] = '\0';
		char *end = strstr (line, "ooBinaryFile");
		autoData me = NULL;
		if (end) {
			fseek (f, strlen ("ooBinaryFile"), 0);
			autostring8 klas = bingets1 (f);
			me.reset ((Data) Thing_newFromClassNameA (klas.peek()));
		} else {
			end = strstr (line, "BinaryFile");
			if (! end) {
				Melder_throw ("File ", MelderFile_messageName (file), " is not a Data binary file.");
			}
			*end = '\0';
			me.reset ((Data) Thing_newFromClassNameA (line));
			Thing_version = -1;   // old version: override version number, which was set to 0 by newFromClassName
			rewind (f);
			fread (line, 1, end - line + strlen ("BinaryFile"), f);
		}
		MelderFile_getParentDir (file, & Data_directoryBeingRead);
		Data_readBinary (me.peek(), f); therror
		f.close (file);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Data not read from binary file ", MelderFile_messageName (file), ".");
	}
}

bool Data_canReadLisp (I) {
	iam (Data);
	return our readLisp != classData -> readLisp;
}

void Data_readLisp (I, FILE *f) {
	iam (Data);
	try {
		our readLisp (me, f);
	} catch (MelderError) {
		Melder_throw (Thing_className (me), " not read.");
	}
}

Any Data_readFromLispFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "r");
		char line [200];
		fgets (line, 199, f);
		char *end = strstr (line, "LispFile");
		if (! end) {
			Melder_throw ("File ", MelderFile_messageName (file), " is not a Data LISP file.");
		}
		*end = '\0';
		autoData me = (Data) Thing_newFromClassNameA (line);
		MelderFile_getParentDir (file, & Data_directoryBeingRead);
		Data_readLisp (me.peek(), f); therror
		f.close (file);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Data not read from LISP file ", MelderFile_messageName (file), ".");
	}
}

/* Generic reading. */

static int numFileTypeRecognizers = 0;
static Any (*fileTypeRecognizers [100]) (int nread, const char *header, MelderFile fs);
void Data_recognizeFileType (Any (*recognizer) (int nread, const char *header, MelderFile fs)) {
	Melder_assert (numFileTypeRecognizers < 100);
	fileTypeRecognizers [++ numFileTypeRecognizers] = recognizer;
}

Any Data_readFromFile (MelderFile file) {
	int nread, i;
	char header [513];
	autofile f = Melder_fopen (file, "rb");
	nread = fread (& header [0], 1, 512, f);
	f.close (file);
	header [nread] = 0;

	/***** 1. Is this file a text file as defined in Data.c? *****/

	if (nread > 11) {
		char *p = strstr (header, "TextFile");
		if (p != NULL && p - header < nread - 8 && p - header < 40)
			return Data_readFromTextFile (file);
	}
	if (nread > 22) {
		char headerCopy [101];
		memcpy (headerCopy, header, 100);
		headerCopy [100] = '\0';
		for (i = 0; i < 100; i ++)
			if (headerCopy [i] == '\0') headerCopy [i] = '\001';
		char *p = strstr (headerCopy, "T\001e\001x\001t\001F\001i\001l\001e");
		if (p != NULL && p - headerCopy < nread - 15 && p - headerCopy < 80)
			return Data_readFromTextFile (file);
	}

	/***** 2. Is this file a binary file as defined in Data.c? *****/

	if (nread > 13) {
		char *p = strstr (header, "BinaryFile");
		if (p != NULL && p - header < nread - 10 && p - header < 40)
			return Data_readFromBinaryFile (file);
	}

	/***** 3. Is this file a LISP file as defined in Data.c? *****/

	if (nread > 11) {
		char *p = strstr (header, "LispFile");
		if (p != NULL && p - header < nread - 8 && p - header < 40)
			return Data_readFromLispFile (file);
	}

	/***** 4. Is this file of a type for which a recognizer has been installed? *****/

	MelderFile_getParentDir (file, & Data_directoryBeingRead);
	for (i = 1; i <= numFileTypeRecognizers; i ++) {
		Data object = (Data) fileTypeRecognizers [i] (nread, header, file);
		if (Melder_hasError ()) return NULL;
		if (object) return object;
	}

	/***** 5. Is this a common text file? *****/

	for (i = 0; i < nread; i ++)
		if (header [i] < 32 || header [i] > 126)   /* Not ASCII? */
			break;
	if (i >= nread) return Data_readFromTextFile (file);

	Melder_throw ("File ", MelderFile_messageName (file), " not recognized.");
}

/* Recursive routines for working with struct members. */

int Data_Description_countMembers (Data_Description structDescription) {
	Data_Description desc;
	int count = 0;
	for (desc = structDescription; desc -> name; desc ++)
		count ++;
	if (structDescription [0]. type == inheritwa) {
		Data_Description parentDescription = * (Data_Description *) structDescription [0]. tagType;
		if (parentDescription)
			return count + Data_Description_countMembers (parentDescription);
	}
	return count;
}

Data_Description Data_Description_findMatch (Data_Description structDescription, const wchar_t *name) {
	Data_Description desc;
	for (desc = structDescription; desc -> name; desc ++)
		if (wcsequ (name, desc -> name)) return desc;
	if (structDescription [0]. type == inheritwa) {
		Data_Description parentDescription = * (Data_Description *) structDescription [0]. tagType;
		if (parentDescription)
			return Data_Description_findMatch (parentDescription, name);
	}
	return NULL;   /* Not found. */
}

Data_Description Data_Description_findNumberUse (Data_Description structDescription, const wchar_t *string) {
	Data_Description desc;
	for (desc = structDescription; desc -> name; desc ++) {
		if (desc -> max1 && wcsequ (desc -> max1, string)) return desc;
		if (desc -> max2 && wcsequ (desc -> max2, string)) return desc;
	}
	if (structDescription [0]. type == inheritwa) {
		Data_Description parentDescription = * (Data_Description *) structDescription [0]. tagType;
		if (parentDescription)
			return Data_Description_findNumberUse (parentDescription, string);
	}
	return NULL;
}

/* Retrieving data from object + description. */

long Data_Description_integer (void *address, Data_Description description) {
	switch (description -> type) {
		case bytewa: return * (signed char *) ((char *) address + description -> offset);
		case shortwa: return * (short *) ((char *) address + description -> offset);
		case intwa: return * (int *) ((char *) address + description -> offset);
		case longwa: return * (long *) ((char *) address + description -> offset);
		case ubytewa: return * (unsigned char *) ((char *) address + description -> offset);
		case ushortwa: return * (unsigned short *) ((char *) address + description -> offset);
		case uintwa: return * (unsigned int *) ((char *) address + description -> offset);
		case ulongwa: return * (unsigned long *) ((char *) address + description -> offset);
		case boolwa: return * (bool *) ((char *) address + description -> offset);
		case objectwa: return (* (Collection *) ((char *) address + description -> offset)) -> size;
		case collectionwa: return (* (Collection *) ((char *) address + description -> offset)) -> size;
		default: return 0;
	}
}

int Data_Description_evaluateInteger (void *structAddress, Data_Description structDescription,
	const wchar_t *formula, long *result)
{
	if (formula == NULL) {   /* This was a VECTOR_FROM array. */
		*result = 1;
		return 1;
	}
	if (wcsnequ (formula, L"my ", 3)) {
		wchar_t buffer [100], *minus1, *psize;
		Data_Description sizeDescription;
		wcscpy (buffer, formula + 3);   /* Skip leading "my ". */
		if ((minus1 = wcsstr (buffer, L" - 1")) != NULL)
			*minus1 = '\0';   /* Strip trailing " - 1", but remember. */
		if ((psize = wcsstr (buffer, L" -> size")) != NULL)
			*psize = '\0';   /* Strip trailing " -> size". */
		if (! (sizeDescription = Data_Description_findMatch (structDescription, buffer))) {
			*result = 0;
			return 0 /*Melder_error ("Cannot find member \"%ls\".", buffer)*/;
		}
		*result = Data_Description_integer (structAddress, sizeDescription);
		if (minus1) *result -= 1;
	} else {
		*result = wcstol (formula, NULL, 10);
	}
	return 1;
}

/* End of file Data.cpp */
