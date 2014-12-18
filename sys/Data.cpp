/* Data.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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

#include "Collection.h"

Thing_implement (Data, Thing, 0);

structMelderDir Data_directoryBeingRead = { { 0 } };

void structData :: v_copy (thou) {
	thouart (Data);
	(void) thee;
}

bool structData :: v_equal (thou) {
	thouart (Data);
	(void) thee;
	return true;
}   // names of "identical" objects are allowed to be different

bool structData :: v_canWriteAsEncoding (int encoding) {
	(void) encoding;
	return true;
}

void structData :: v_writeText (MelderFile openFile) {
	(void) openFile;
}

void structData :: v_readText (MelderReadText text) {
	(void) text;
}

void structData :: v_writeBinary (FILE *f) {
	(void) f;
}

void structData :: v_readBinary (FILE *f) {
	(void) f;
}

Any _Data_copy (Data me) {
	try {
		if (me == NULL) return NULL;
		autoData thee = (Data) _Thing_new (my classInfo);
		my v_copy (thee.peek());
		Thing_setName (thee.peek(), my name);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not copied.");
	}
}

bool Data_equal (Data me, Data thee) {
	if (my classInfo != thy classInfo) return false;   // different class: not equal
	int offset = sizeof (struct structData);   // we already compared the methods, and are going to skip the names
	if (! memcmp ((char *) me + offset, (char *) thee + offset, my classInfo -> size - offset))
		return true;   // no shallow differences
	return my v_equal (thee);
}

bool Data_canWriteAsEncoding (Data me, int encoding) {
	return my v_canWriteAsEncoding (encoding);
}

bool Data_canWriteText (Data me) {
	return my v_writable ();
}

void Data_writeText (Data me, MelderFile openFile) {
	my v_writeText (openFile);
	if (ferror (openFile -> filePointer))
		Melder_throw ("I/O error.");
}

MelderFile Data_createTextFile (Data me, MelderFile file, bool verbose) {
	autoMelderFile mfile = MelderFile_create (file);
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

static void _Data_writeToTextFile (Data me, MelderFile file, bool verbose) {
	try {
		if (! Data_canWriteText (me))
			Melder_throw ("Objects of class ", my classInfo -> className, " cannot be written to a text file.");
		autoMelderFile mfile = Data_createTextFile (me, file, verbose);
		#ifndef _WIN32
			flockfile (file -> filePointer);   // BUG
		#endif
		MelderFile_write (file, L"File type = \"ooTextFile\"\nObject class = \"", my classInfo -> className);
		if (my classInfo -> version > 0)
			MelderFile_write (file, L" ", Melder_integer (my classInfo -> version));
		MelderFile_write (file, L"\"\n");
		Data_writeText (me, file);
		MelderFile_writeCharacter (file, '\n');
		#ifndef _WIN32
			if (file -> filePointer) funlockfile (file -> filePointer);
		#endif
		mfile.close ();
	} catch (MelderError) {
		#ifndef _WIN32
			if (file -> filePointer) funlockfile (file -> filePointer);   // the file pointer is NULL before Data_createTextFile() and after mfile.close()
		#endif
		throw;
	}
}

void Data_writeToTextFile (Data me, MelderFile file) {
	try {
		_Data_writeToTextFile (me, file, true);
	} catch (MelderError) {
		Melder_throw (me, ": not written to text file ", file, ".");
	}
}

void Data_writeToShortTextFile (Data me, MelderFile file) {
	try {
		_Data_writeToTextFile (me, file, false);
	} catch (MelderError) {
		Melder_throw (me, ": not written to short text file ", file, ".");
	}
}

bool Data_canWriteBinary (Data me) {
	return my v_writable ();
}

void Data_writeBinary (Data me, FILE *f) {
	my v_writeBinary (f);
	if (ferror (f))
		Melder_throw ("I/O error.");
}

void Data_writeToBinaryFile (Data me, MelderFile file) {
	try {
		if (! Data_canWriteBinary (me))
			Melder_throw ("Objects of class ", my classInfo -> className, L" cannot be written to a generic binary file.");
		autoMelderFile mfile = MelderFile_create (file);
		if (fprintf (file -> filePointer, "ooBinaryFile") < 0)
			Melder_throw ("Cannot write first bytes of file.");
		binputw1 (
			my classInfo -> version > 0 ?
				Melder_wcscat (my classInfo -> className, L" ", Melder_integer (my classInfo -> version)) :
				my classInfo -> className,
			file -> filePointer);
		Data_writeBinary (me, file -> filePointer);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, ": not written to binary file ", file, ".");
	}
}

bool Data_canReadText (Data me) {
	return my v_writable ();
}

void Data_readText (Data me, MelderReadText text) {
	try {
		my v_readText (text);
		my v_repair ();
	} catch (MelderError) {
		Melder_throw (Thing_className (me), " not read.");
	}
}

Any Data_readFromTextFile (MelderFile file) {
	try {
		autoMelderReadText text = MelderReadText_createFromFile (file);
		wchar_t *line = MelderReadText_readLine (text.peek());
		if (line == NULL)
			Melder_throw ("No lines.");
		wchar_t *end = wcsstr (line, L"ooTextFile");   // oo format?
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
		Data_readText (me.peek(), text.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Data not read from text file ", file, ".");
	}
}

bool Data_canReadBinary (Data me) {
	return my v_writable ();
}

void Data_readBinary (Data me, FILE *f) {
	try {
		my v_readBinary (f);
		if (feof (f))
			Melder_throw ("Early end of file.");
		if (ferror (f))
			Melder_throw ("I/O error.");
		my v_repair ();
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
				Melder_throw ("File ", file, " is not a Data binary file.");
			}
			*end = '\0';
			me.reset ((Data) Thing_newFromClassNameA (line));
			Thing_version = -1;   // old version: override version number, which was set to 0 by newFromClassName
			rewind (f);
			fread (line, 1, end - line + strlen ("BinaryFile"), f);
		}
		MelderFile_getParentDir (file, & Data_directoryBeingRead);
		Data_readBinary (me.peek(), f);
		f.close (file);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Data not read from binary file ", file, ".");
	}
}

/* Generic reading. */

static int numFileTypeRecognizers = 0;
static Any (*fileTypeRecognizers [100]) (int nread, const char *header, MelderFile file);
void Data_recognizeFileType (Any (*recognizer) (int nread, const char *header, MelderFile file)) {
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

	/***** 1. Is this file a text file as defined in Data.cpp? *****/

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

	/***** 2. Is this file a binary file as defined in Data.cpp? *****/

	if (nread > 13) {
		char *p = strstr (header, "BinaryFile");
		if (p != NULL && p - header < nread - 10 && p - header < 40)
			return Data_readFromBinaryFile (file);
	}

	/***** 3. Is this file of a type for which a recognizer has been installed? *****/

	MelderFile_getParentDir (file, & Data_directoryBeingRead);
	for (i = 1; i <= numFileTypeRecognizers; i ++) {
		Data object = (Data) fileTypeRecognizers [i] (nread, header, file);
		if (object == (Data) 1) return NULL;
		if (object) return object;
	}

	/***** 4. Is this a common text file? *****/

	for (i = 0; i < nread; i ++)
		if (header [i] < 32 || header [i] > 126)   /* Not ASCII? */
			break;
	if (i >= nread) return Data_readFromTextFile (file);

	Melder_throw ("File ", file, " not recognized.");
}

/* Recursive routines for working with struct members. */

int Data_Description_countMembers (Data_Description structDescription) {
	int count = 0;
	for (Data_Description desc = structDescription; desc -> name; desc ++)
		count ++;
	if (structDescription [0]. type == inheritwa) {
		Data_Description parentDescription = ((Data) _Thing_dummyObject ((ClassInfo) structDescription [0]. tagType)) -> v_description ();
		if (parentDescription)
			return count + Data_Description_countMembers (parentDescription);
	}
	return count;
}

Data_Description Data_Description_findMatch (Data_Description structDescription, const wchar_t *name) {
	for (Data_Description desc = structDescription; desc -> name; desc ++)
		if (wcsequ (name, desc -> name)) return desc;
	if (structDescription [0]. type == inheritwa) {
		Data_Description parentDescription = ((Data) _Thing_dummyObject ((ClassInfo) structDescription [0]. tagType)) -> v_description ();
		if (parentDescription)
			return Data_Description_findMatch (parentDescription, name);
	}
	return NULL;   /* Not found. */
}

Data_Description Data_Description_findNumberUse (Data_Description structDescription, const wchar_t *string) {
	for (Data_Description desc = structDescription; desc -> name; desc ++) {
		if (desc -> max1 && wcsequ (desc -> max1, string)) return desc;
		if (desc -> max2 && wcsequ (desc -> max2, string)) return desc;
	}
	if (structDescription [0]. type == inheritwa) {
		Data_Description parentDescription = ((Data) _Thing_dummyObject ((ClassInfo) structDescription [0]. tagType)) -> v_description ();
		if (parentDescription)
			return Data_Description_findNumberUse (parentDescription, string);
	}
	return NULL;
}

/* Retrieving data from object + description. */

long Data_Description_integer (void *address, Data_Description description) {
	switch (description -> type) {
		case bytewa:       return * (signed char *)    ((char *) address + description -> offset);
		case intwa:        return * (int *)            ((char *) address + description -> offset);
		case longwa:       return * (long *)           ((char *) address + description -> offset);
		case ubytewa:      return * (unsigned char *)  ((char *) address + description -> offset);
		case uintwa:       return * (unsigned int *)   ((char *) address + description -> offset);
		case ulongwa:      return * (unsigned long *)  ((char *) address + description -> offset);
		case boolwa:       return * (bool *)           ((char *) address + description -> offset);
		case objectwa:     return (* (Collection *)    ((char *) address + description -> offset)) -> size;
		case collectionwa: return (* (Collection *)    ((char *) address + description -> offset)) -> size;
		default: return 0;
	}
}

int Data_Description_evaluateInteger (void *structAddress, Data_Description structDescription,
	const wchar_t *formula, long *result)
{
	if (formula == NULL) {   // this was a VECTOR_FROM array
		*result = 1;
		return 1;
	}
	if (formula [0] >= 'a' && formula [0] <= 'z') {
		wchar_t buffer [100], *minus1, *psize;
		Data_Description sizeDescription;
		wcscpy (buffer, formula);
		if ((minus1 = wcsstr (buffer, L" - 1")) != NULL)
			*minus1 = '\0';   // strip trailing " - 1", but remember
		if ((psize = wcsstr (buffer, L" -> size")) != NULL)
			*psize = '\0';   // strip trailing " -> size"
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
