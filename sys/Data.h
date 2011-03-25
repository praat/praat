#ifndef _Data_h_
#define _Data_h_
/* Data.h
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
 * pb 2011/03/02
 */

/* Data inherits from Thing. */
/* It adds the functionality of reproduction, comparison, reading, and writing. */
#ifndef _Thing_h_
	#include "Thing.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

Any Data_copy (I);
/*
	Message:
		"return a deep copy of yourself, or NULL if out of memory."
	Postconditions:
		result -> name == NULL;	// The only attribute NOT copied.
*/

bool Data_equal (Any data1, Any data2);
/*
	Message:
		"return 1 if the shallow or deep attributes of 'data1' and 'data2' are equal;
		 otherwise, return 0."
	Comment:
		Data_equal (data, Data_copy (data)) should always return 1; the names are not compared.
*/

bool Data_canWriteAsEncoding (I, int outputEncoding);
/*
	Message:
		"Can you write yourself in that encoding?"
	The answer depends on whether all members can be written in that encoding.
*/

bool Data_canWriteText (I);
/*
	Message:
		"Can you write yourself as text?"
	The answer depends on whether the subclass defines the 'writeText' method.
*/

int Data_createTextFile (I, MelderFile file, bool verbose);

int Data_writeText (I, MelderFile openFile);
/*
	Message:
		"try to write yourself as text to an open file."
	Return value:
		1 if OK, 0 in case of failure.
	Failures:
		I/O error.
		Disk full.
	Description:
		The format depends on the 'writeText' method defined by the subclass.
*/

int Data_writeToTextFile (I, MelderFile file);
/*
	Message:
		"try to write yourself as text to a file".
	Description:
		The first line is 'File type = "ooTextFile"'.
		Your class name is written in the second line,
		e.g., if you are a Person, the second line will be 'Object class = "Person"'.
		The format of the lines after the second line is the same as in Data_writeText.
*/

int Data_writeToShortTextFile (I, MelderFile file);
/*
	Message:
		"try to write yourself as text to a file".
	Description:
		The first line is 'File type = "ooTextFile short"'.
		Your class name is written in the second line,
		e.g., if you are a Person, the second line will be '"Person"'.
		The format of the lines after the second line is the same as in Data_writeText.
*/

bool Data_canWriteBinary (I);
/*
	Message:
		"Can you write yourself as binary data?"
	The answer depends on whether the subclass defines the 'writeBinary' method.
*/

int Data_writeBinary (I, FILE *f);
/*
	Message:
		"try to write yourself as binary data to an open file."
	Return value:
		1 if OK, 0 in case of failure.
	Failures:
		I/O error.
		Disk full.
	Description:
		The format depends on the 'writeBinary' method defined by the subclass,
		but is machine independent because it always uses 'most significant byte first'
		and IEEE floating-point format.
*/

int Data_writeToBinaryFile (I, MelderFile file);
/*
	Message:
		"try to write yourself as binary data to a file".
	Description:
		First, your class name is written in the file,
		e.g., if you are a Person, the file will start with "PersonBinaryFile".
		The format of the file after this is the same as in Data_writeBinary.
*/

bool Data_canWriteLisp (I);
/*
	Message:
		"Can you write yourself as a sequece of LISP objects?"
	The answer depends on whether the subclass defines a 'writeLisp' method.
*/

int Data_writeLisp (I, FILE *f);
/*
	Message:
		"try to write yourself as a sequence of LISP objects to the stream <f>."
	Return value:
		1 if OK, 0 in case of failure.
	Failures:
		I/O error.
		Disk full.
	Description:
		The format depends on the 'writeLisp' method defined by the subclass.
*/

int Data_writeLispToConsole (I);
/*
	Message:
		"try to write yourself as a sequence of LISP objects to the standard output."
	Return value:
		1 if OK, 0 in case of failure.
	Description:
		The format is the same as in Data_writeLisp.
		The standard output will most often be a window named "Console".
*/

int Data_writeToLispFile (I, MelderFile file);
/*
	Message:
		"try to write yourself as a sequence of LISP objects to a file".
	Description:
		Your class name is written in the first line,
		e.g., if you are a Person, the first line will be "PersonLispFile".
		The format of the lines after the first line is the same as in Data_writeLisp.
*/

/*
	The routines Data_readXXX assume that a class can be read from its name (a string).
	You should have called Thing_recognizeClassesByName () for all the classes
	that you want to read by name. This call is best placed in the beginning of main ().
*/

bool Data_canReadText (I);
/*
	Message:
		"Can you read yourself as text?"
	The answer depends on whether the subclass defines a 'readText' method,
	but is preferably the same as the answer from Data_canWriteText.
*/

int Data_readText (I, MelderReadText text);
/*
	Message:
		"try to read yourself as text from a string."
	Return value:
		1 if OK, 0 in case of failure.
	Failures:
		The 'readText' method of the subclass failed (returned 0).
		I/O error.
		Early end of file detected.
	Description:
		The format depends on the 'readText' method defined by the subclass,
		but is preferably the same as the format produced by the 'writeText' method.
*/

Any Data_readFromTextFile (MelderFile file);
/*
	Message:
		"try to read a Data as text from a file".
	Description:
		The Data's class name is read from the first line,
		e.g., if the first line is "PersonTextFile", the Data will be a Person.
		The format of the lines after the first line is the same as in Data_readText.
	Return value:
		the new object if OK, or NULL in case of failure.
	Failures:
		Error opening file <fileName>.
		The file <fileName> does not contain an object.
		(plus those from Data_readText)
*/

bool Data_canReadBinary (I);
/*
	Message:
		"Can you read yourself as binary data?"
	The answer depends on whether the subclass defines a 'readBinary' method,
	but is preferably the same as the answer from Data_canWriteBinary.
*/

int Data_readBinary (I, FILE *f);
/*
	Message:
		"try to read yourself as binary data from the stream <f>."
	Return value:
		1 if OK, 0 in case of failure.
	Failures:
		The 'readBinary' method of the subclass returns an error (0).
		I/O error.
		Early end of file detected.
	Description:
		The format depends on the 'readBinary' method defined by the subclass,
		but is preferably the same as the format produced by the 'writeBinary' method.
*/

Any Data_readFromBinaryFile (MelderFile file);
/*
	Message:
		"try to read a Data as binary data from a file".
	Description:
		The Data's class name is read from the start of the file,
		e.g., if the file starts with is "PersonBinaryFile", the Data will be a Person.
		The format of the file after this is the same as in Data_readBinary.
	Return value:
		the new object if OK, or NULL in case of failure.
	Failures:
		Error opening file <fileName>.
		The file <fileName> does not contain an object.
		(plus those from Data_readBinary)
*/

bool Data_canReadLisp (I);
/*
	Message:
		"Can you read yourself from a sequence of LISP objects?"
	The answer depends on whether the subclass defines a 'readLisp' method,
	but is preferably the same as the answer from Data_canWriteLisp.
*/

int Data_readLisp (I, FILE *f);
/*
	Message:
		"try to read yourself from a sequence of LISP objects in the stream <f>."
	Return value:
		1 if OK, 0 in case of failure.
	Failures:
		The 'readLisp' method of the subclass failed (returned 0).
		I/O error.
		Early end of file detected.
	Description:
		The format depends on the 'readLisp' method defined by the subclass,
		but is preferably the same as the format produced by the 'writeLisp' method.
*/

Any Data_readFromLispFile (MelderFile fs);
/*
	Message:
		"try to read a Data from a sequence of LISP objets in a file".
	Description:
		The Data's class name is read from the first line,
		e.g., if the first line is "PersonLispFile", the Data will be a Person.
		The format of the lines after the first line is the same as in Data_readLisp.
	Return value:
		the new object if OK, or NULL in case of failure.
	Failures:
		Error opening file <fileName>.
		The file <fileName> does not contain an object.
		(plus those from Data_readLisp)
*/

typedef struct structData_Description {
	const wchar_t *name;   /* The name of this field. */
	int type;   /* bytewa..inheritwa, see below */
	int offset;   /* The offset of this field in the enveloping struct. */
	int size;   /* The size of this field if it is in an array. */
	const wchar_t *tagName;   /* For structs: tag; for classes: class name; for enums: type name. */
	void *tagType;   /* For structs: offset table; for classes: class pointer; for enums: enum pointer. */
	int rank;   /* 0 = single, 1 = vector, 2 = matrix, 3 = set, -1 = array. */
	const wchar_t *min1, *max1;   /* For vectors and matrices. */
	const wchar_t *min2, *max2;   /* For matrices. */
} *Data_Description;

#define Data_members Thing_members
#define Data_methods Thing_methods \
	struct structData_Description *description; \
	int (*copy) (Any data_from, Any data_to); \
	bool (*equal) (Any data1, Any data2); \
	bool (*canWriteAsEncoding) (I, int outputEncoding); \
	int (*writeText) (I, MelderFile openFile); \
	int (*readText) (I, MelderReadText text); \
	int (*writeBinary) (I, FILE *f); \
	int (*readBinary) (I, FILE *f); \
	int (*writeCache) (I, CACHE *f); \
	int (*readCache) (I, CACHE *f); \
	int (*writeLisp) (I, FILE *f); \
	int (*readLisp) (I, FILE *f); \
	/* Messages for scripting. */ \
	double (*getNrow) (I); \
	double (*getNcol) (I); \
	double (*getXmin) (I); \
	double (*getXmax) (I); \
	double (*getYmin) (I); \
	double (*getYmax) (I); \
	double (*getNx) (I); \
	double (*getNy) (I); \
	double (*getDx) (I); \
	double (*getDy) (I); \
	double (*getX) (I, long ix); \
	double (*getY) (I, long iy); \
	const wchar * (*getRowStr) (I, long irow); \
	const wchar * (*getColStr) (I, long icol); \
	double (*getCell) (I); \
	const wchar * (*getCellStr) (I); \
	double (*getVector) (I, long irow, long icol); \
	const wchar * (*getVectorStr) (I, long icol); \
	double (*getMatrix) (I, long irow, long icol); \
	const wchar * (*getMatrixStr) (I, long irow, long icol); \
	double (*getFunction0) (I); \
	double (*getFunction1) (I, long irow, double x); \
	double (*getFunction2) (I, double x, double y); \
	double (*getRowIndex) (I, const wchar_t *rowLabel); \
	double (*getColumnIndex) (I, const wchar_t *columnLabel);
class_create (Data, Thing);

/* The values of 'type' in struct descriptions. */

#define bytewa  1
#define shortwa  2
#define intwa  3
#define longwa  4
#define ubytewa  5
#define ushortwa  6
#define uintwa  7
#define ulongwa  8
#define boolwa 9
#define floatwa  10
#define doublewa  11
#define fcomplexwa  12
#define dcomplexwa  13
#define charwa  14
#define wcharwa  15
#define enumwa  16
#define lenumwa  17
#define booleanwa  18
#define questionwa  19
#define stringwa  20
#define lstringwa  21
#define maxsingletypewa lstringwa
#define structwa  22
#define widgetwa  23
#define objectwa  24
#define collectionwa  25
#define inheritwa  26

/* Recursive routines for working with struct members. */

int Data_Description_countMembers (Data_Description structDescription);
/* Including inherited members. */

Data_Description Data_Description_findMatch (Data_Description structDescription, const wchar_t *member);
/* Find the location of member 'member' in a struct. */
/* If 'structDescription' describes a class, the ancestor classes are also searched. */

Data_Description Data_Description_findNumberUse (Data_Description structDescription, const wchar_t *string);
/* Find the first member that uses member 'string' in its size description (max1 or max2 fields). */

/* Retrieving data from object + description. */

long Data_Description_integer (void *structAddress, Data_Description description);
/* Convert data found at a certain offset from 'address' to an integer, according to the given 'description'. */

int Data_Description_evaluateInteger (void *structAddress, Data_Description structDescription,
	const wchar_t *formula, long *result);
/*
 * Translates a string like '100' or 'my numberOfHorses' or 'my numberOfCows - 1' to an integer.
 * The 'algorithm' does some wild guesses as to the meanings of the 'min1' and 'max1' strings.
 * A full-fledged interpretation is preferable...
 * Returns 0 if 'formula' cannot be parsed to a number.
 */

void Data_recognizeFileType (Any (*recognizer) (int nread, const char *header, MelderFile fs));
/*
Purpose:
	to make sure that a file can be read by Data_readFromFile.
Arguments:
	recognizer
		a routine that tries to identify a file as being of a certain type,
		from the information in the header of the file and/or from the file name.
		If the identification succeeds, the recognizer routine should call the routine
		that actually reads the file.
	nread
		the length of the header of the file. The maximum value of nread is 512,
		but it is smaller if the file is shorter than 512 bytes.
	header
		a buffer that contains the first nread bytes of the file;
		the data were put there by Data_readFromFile before calling the recognizers.
		The first byte is in header [0].
	fileName
		the name of the file that is to be recognized. The recognizer routine uses this name
		to actually read the file if it positively identifies the file. The recognizer routine
		may also use %fileName in the recognition process; e.g. some files with raw sound data
		have names that make them recognizable as files with raw sound data.
Defining a file-type recognizer:
	You define a file-type recognizer as in the following example,
	which tries to identify and read a Sun audio file.
	A Sun audio file should contain at least 24 bytes and start with the string ".snd":
	Any Sound_sunAudioFileRecognizer (int nread, const char *header, const char *fileName) {
		if (nread >= 24 && strnequ (& header [0], ".snd", 4))
			return Sound_readFromSunAudioFile (fileName);
		else
			return NULL;
	}
	From this example, we see that if the file is recognized, it should be read immediately,
	and the resulting object (always a descendant of class Data) should be returned.
	We also see that the return value NULL is used either for notifying Data_readFromFile
	of the fact that the file is not a Sun audio file,
	or for returning an error message (because Sound_readFromSunAudioFile may return NULL).
	Data_readFromFile will distinguish between these two possibilities by checking the error queue
	(with Melder_hasError). This means that Sound_readFromSunAudioFile is obliged
	to queue an error (with Melder_error) in case of a failure.
Registering a file-type recognizer:
	You would put a statement like the following in the initialization section of your program:
	Data_recognizeFileType (Sound_sunAudioFileRecognizer);
	After this, Data_readFromFile is able to read Sun audio files.
*/

Any Data_readFromFile (MelderFile file);
/*
Purpose:
	to read a file with data of any kind.
Return value:
	the object read from the file fs,
	or NULL if the file was not recognized (an error message is queued in that case).
Behaviour:
	Data_readFromFile first checks whether the file is a text file
	that is readable by Data_readFromTextFile, or a binary file
	written by Data_writeToBinaryFile, or a file as written by Data_writeToLispFile.
	If one of these succeeds, the file is read and the resulting object is returned.
	If not, the recognizers installed with Data_recognizeFileType are tried.
	If this also fails, Data_readFromFile returns NULL.
*/

extern structMelderDir Data_directoryBeingRead;

#ifdef __cplusplus
	}
#endif

/* End of file Data.h */
#endif
