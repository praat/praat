/* Data.c
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
 */

#include "Collection.h"

structMelderDir Data_directoryBeingRead = { { 0 } };

static int copy (Any data1, Any data2) {
	(void) data1;
	(void) data2;
	return 1;
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

static int writeText (Any data, MelderFile openFile) {
	(void) data;
	(void) openFile;
	return 1;
}

static int readText (Any data, MelderReadString *text) {
	(void) data;
	(void) text;
	return 1;
}

static int writeBinary (Any data, FILE *f) {
	(void) data;
	(void) f;
	return 1;
}

static int readBinary (Any data, FILE *f) {
	(void) data;
	(void) f;
	return 1;
}

static int writeCache (Any data, CACHE *f) {
	(void) data;
	(void) f;
	return 1;
}

static int readCache (Any data, CACHE *f) {
	(void) data;
	(void) f;
	return 1;
}

static int writeLisp (Any data, FILE *f) {
	(void) data;
	(void) f;
	return 1;
}

static int readLisp (Any data, FILE *f) {
	(void) data;
	(void) f;
	return 1;
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
	Data thee;
	if (me == NULL) return NULL;
	if (our copy == classData -> copy) return Melder_errorp ("(Data_copy:) Class %s cannot be copied.", our _className);
	thee = Thing_new (my methods);
	if (! thee) return NULL;
	if (! our copy (me, thee)) {
		forget (thee);
		return Melder_errorp ("(Data_copy:) Object of class %s not copied.", our _className);
	}
	Thing_setName (thee, my name);
	return thee;
}

bool Data_equal (I, thou) {
	iam (Data); thouart (Data);
	int offset;
	if (my methods != thy methods) return 0;   /* Different class: not equal. */
	offset = sizeof (struct structData);   /* We already compared the methods, and are going to skip the names. */
	if (! memcmp ((char *) me + offset, (char *) thee + offset, our _size - offset))
		return 1;   /* No shallow differences. */
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

int Data_writeText (I, MelderFile openFile) {
	iam (Data);
	if (! our writeText (me, openFile)) return 0;
	if (ferror (openFile -> filePointer)) return Melder_error1 (L"(Data_writeText:) I/O error.");
	return 1;
}

int Data_createTextFile (I, MelderFile file, bool verbose) {
	iam (Data);
	MelderFile_create (file, L"TEXT", 0, L"txt"); cherror
	#if defined (_WIN32)
		file -> requiresCRLF = true;
	#endif
	file -> verbose = verbose;
	file -> outputEncoding = Melder_getOutputEncoding ();
	if (file -> outputEncoding == kMelder_textOutputEncoding_ASCII_THEN_UTF16)
		file -> outputEncoding = Data_canWriteAsEncoding (me, kMelder_textOutputEncoding_ASCII) ? kMelder_textOutputEncoding_ASCII : kMelder_textOutputEncoding_UTF16;
	else if (file -> outputEncoding == kMelder_textOutputEncoding_ISO_LATIN1_THEN_UTF16)
		file -> outputEncoding = Data_canWriteAsEncoding (me, kMelder_textOutputEncoding_ISO_LATIN1) ? kMelder_textOutputEncoding_ISO_LATIN1 : kMelder_textOutputEncoding_UTF16;
	if (file -> outputEncoding == kMelder_textOutputEncoding_UTF16)
		binputu2 (0xfeff, file -> filePointer);
end:
	iferror return 0;
	return 1;
}

static int _Data_writeToTextFile (I, MelderFile file, bool verbose) {
	iam (Data);
	if (! Data_canWriteText (me)) error3 (L"(Data_writeToTextFile:) Objects of class ", our _classNameW, L" cannot be written to a text file.")
	Data_createTextFile (me, file, verbose); cherror
	MelderFile_write2 (file, L"File type = \"ooTextFile\"\nObject class = \"", our _classNameW);
	if (our version > 0) MelderFile_write2 (file, L" ", Melder_integer (our version));
	MelderFile_write1 (file, L"\"\n");
	Data_writeText (me, file); cherror
	MelderFile_writeCharacter (file, '\n');
end:
	MelderFile_close (file);
	iferror return Melder_error5 (L"Cannot write ", our _classNameW, L"to file \"", MelderFile_messageNameW (file), L"\".");
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	return 1;
}

int Data_writeToTextFile (I, MelderFile file) {
	return _Data_writeToTextFile (void_me, file, true);
}

int Data_writeToShortTextFile (I, MelderFile file) {
	return _Data_writeToTextFile (void_me, file, false);
}

bool Data_canWriteBinary (I) {
	iam (Data);
	return our writeBinary != classData -> writeBinary;
}

int Data_writeBinary (I, FILE *f) {
	iam (Data);
	if (! our writeBinary (me, f)) return 0;
	if (ferror (f)) return Melder_error1 (L"I/O error.");
	return 1;
}

int Data_writeToBinaryFile (I, MelderFile file) {
	iam (Data);
	if (! Data_canWriteBinary (me)) error3 (L"(Data_writeToBinaryFile:) Objects of class ", our _classNameW, L" cannot be written to a generic binary file.")
	MelderFile_create (file, 0, 0, 0); cherror
	if (fprintf (file -> filePointer, "ooBinaryFile") < 0)
		error1 (L"Cannot write first bytes of file.")
	char className [100];
	if (our version)
		sprintf (className, "%s %ld", our _className, our version);
	else
		strcpy (className, our _className);
	binputs1 (className, file -> filePointer);
	if (! Data_writeBinary (me, file -> filePointer)) goto end;
end:
	MelderFile_close (file);
	iferror return Melder_error3 (L"(Data_writeToBinaryFile:) Cannot write file \"", MelderFile_messageNameW (file), L"\".");
	MelderFile_setMacTypeAndCreator (file, 'BINA', 0);
	return 1;
}

bool Data_canWriteLisp (I) {
	iam (Data);
	return our writeLisp != classData -> writeLisp;
}

int Data_writeLisp (I, FILE *f) {
	iam (Data);
	if (! our writeLisp (me, f)) return 0;
	if (ferror (f)) return Melder_error1 (L"(Data_writeLisp:) I/O error.");
	return 1;
}

int Data_writeLispToConsole (I) {
	iam (Data);
	if (! Data_canWriteLisp (me)) return Melder_error ("(Data_writeLispToConsole:) Class %s cannot be written as LISP.", our _className);
	wprintf (L"Write as LISP sequence to console: class %ls,  name \"%ls\".\n", Thing_classNameW (me), my name ? my name : L"<none>");
	return Data_writeLisp (me, stdout);
}

int Data_writeToLispFile (I, MelderFile fs) {
	iam (Data);
	FILE *f;
	if (! Data_canWriteLisp (me)) return Melder_error ("(Data_writeToLispFile:) Class %s cannot be written as LISP.", our _className);
	if ((f = Melder_fopen (fs, "w")) == NULL) return 0;
	if (fprintf (f, "%sLispFile\n", our _className) == EOF || ! Data_writeLisp (me, f)) {
		fclose (f);
		return Melder_error ("(Data_writeToLispFile:) Error while writing file \"%.200s\". Disk probably full.", MelderFile_messageName (fs));
	}
	fclose (f);
	MelderFile_setMacTypeAndCreator (fs, 'TEXT', 0);
	return 1;
}

bool Data_canReadText (I) {
	iam (Data);
	return our readText != classData -> readText;
}

int Data_readText (I, MelderReadString *text) {
	iam (Data);
	if (! our readText (me, text) || Melder_hasError ())
		return Melder_error2 (Thing_classNameW (me), L" not read.");
	if (text -> readPointer == NULL)
		return Melder_error3 (L"Early end of file. ", Thing_classNameW (me), L" not read.");
	return 1;
}

Any Data_readFromTextFile (MelderFile file) {
	Data me = NULL;
	wchar_t *klas = NULL, *string = NULL;
	string = MelderFile_readText (file); cherror
	MelderReadString text = { string, string };
	wchar_t *line = MelderReadString_readLine (& text);
	wchar_t *end = wcsstr (line, L"ooTextFile");   /* oo format? */
	if (end) {
		klas = texgetw2 (& text); cherror
		me = Thing_newFromClassNameW (klas); cherror
	} else {
		end = wcsstr (line, L"TextFile");
		if (end == NULL) error1 (L"Not an old-type text file; should not occur.")
		*end = '\0';
		me = Thing_newFromClassNameW (line); cherror
		Thing_version = -1;   /* Old version: override version number, which was set to 0 by newFromClassName. */
	}
	MelderFile_getParentDir (file, & Data_directoryBeingRead);
	Data_readText (me, & text); cherror
end:
	Melder_free (klas);
	Melder_free (string);
	iferror forget (me);
	return me;
}

bool Data_canReadBinary (I) {
	iam (Data);
	return our readBinary != classData -> readBinary;
}

int Data_readBinary (I, FILE *f) {
	iam (Data);
	if (! our readBinary (me, f)) return Melder_error ("(Data_readBinary:) %s not read.", Thing_className (me));
	if (feof (f))
		return Melder_error ("(Data_readBinary:) Early end of file. %s not read.", Thing_className (me));
	if (ferror (f)) return Melder_error ("(Data_readBinary:) I/O error. %s not read.", Thing_className (me));
	return 1;
}

Any Data_readFromBinaryFile (MelderFile file) {
	Data me = NULL;
	int n;
	FILE *f;
	char line [200], *end;
	if ((f = Melder_fopen (file, "rb")) == NULL) return NULL;
	n = fread (line, 1, 199, f); line [n] = '\0';
	end = strstr (line, "ooBinaryFile");
	if (end) {
		char *klas;
		fseek (f, strlen ("ooBinaryFile"), 0);
		klas = bingets1 (f);
		if (! klas || ! (me = Thing_newFromClassName (klas))) { fclose (f); return 0; }
		Melder_free (klas);
	} else {
		end = strstr (line, "BinaryFile");
		if (! end || ! (*end = '\0', me = Thing_newFromClassName (line))) {
			fclose (f);
			return Melder_errorp ("(Data_readFromBinaryFile:) File \"%.200s\" is not a Data binary file.", MelderFile_messageName (file));
		}
		Thing_version = -1;   /* Old version: override version number, which was set to 0 by newFromClassName. */
		rewind (f);
		fread (line, 1, end - line + strlen ("BinaryFile"), f);
	}
	MelderFile_getParentDir (file, & Data_directoryBeingRead);
	if (! Data_readBinary (me, f)) forget (me);
	fclose (f);
	return me;
}

bool Data_canReadLisp (I) {
	iam (Data);
	return our readLisp != classData -> readLisp;
}

int Data_readLisp (I, FILE *f) {
	iam (Data);
	if (! our readLisp (me, f)) return Melder_error ("(Data_readLisp:) %s not read.", Thing_className (me));
	/* (Do not check for end-of-file) */
	if (ferror (f)) return Melder_error ("(Data_readLisp:) I/O error. %s not read.", Thing_className (me));
	return 1;
}

Any Data_readFromLispFile (MelderFile file) {
	Data me = NULL;
	FILE *f;
	char line [200], *end;
	if ((f = Melder_fopen (file, "r")) == NULL) return NULL;
	fgets (line, 199, f);
	end = strstr (line, "LispFile");
	if (! end || ! (*end = '\0', me = Thing_newFromClassName (line))) {
		fclose (f);
		return Melder_errorp ("(Data_readFromLispFile:) File \"%.200s\" is not a Data LISP file.", MelderFile_messageName (file));
	}
	MelderFile_getParentDir (file, & Data_directoryBeingRead);
	if (! Data_readLisp (me, f)) forget (me);
	fclose (f);
	return me;
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
	FILE *f = Melder_fopen (file, "rb");
	if (! f) return NULL;
	nread = fread (& header [0], 1, 512, f);
	fclose (f);
	header [nread] = 0;

	/***** 1. Is this file a text file as defined in Data.c? *****/

	if (nread > 11) {
		char *p = strstr (header, "TextFile");
		if (p != NULL && p - header < nread - 8 && p - header < 40)
			return Data_readFromTextFile (file);
	}
	if (nread > 22) {
		char headerCopy [513];
		memcpy (headerCopy, header, 100);
		for (i = 0; i < 512; i ++)
			if (header [i] == '\0') header [i] = '\001';
		char *p = strstr (header, "T\001e\001x\001t\001F\001i\001l\001e");
		if (p != NULL && p - header < nread - 15 && p - header < 80)
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
		Data object = fileTypeRecognizers [i] (nread, header, file);
		if (Melder_hasError ()) return NULL;
		if (object) return object;
	}

	/***** 5. Is this a common text file? *****/

	for (i = 0; i < nread; i ++)
		if (header [i] < 32 || header [i] > 126)   /* Not ASCII? */
			break;
	if (i >= nread) return Data_readFromTextFile (file);

	return Melder_errorp ("(Data_readFromFile:) File %.200s not recognized.", MelderFile_messageName (file));
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
		case charwa: return * (char *) ((char *) address + description -> offset);
		case collectionwa: return (* (Collection *) ((char *) address + description -> offset)) -> size;
		case objectwa: return (* (Collection *) ((char *) address + description -> offset)) -> size;
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

/* End of file Data.c */
