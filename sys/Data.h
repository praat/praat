#ifndef _Data_h_
#define _Data_h_
/* Data.h
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/06/21
 */

/* Data inherits from Thing. */
/* It adds the functionality of reproduction, comparison, reading, and writing. */
#ifndef _Thing_h_
	#include "Thing.h"
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

bool Data_canWriteAsAscii (I);
/*
	Message:
		"Can you write yourself as ASCII text?"
	The answer depends on whether all members can be written as ASCII text.
*/

bool Data_canWriteText (I);
/*
	Message:
		"Can you write yourself as text?"
	The answer depends on whether the subclass defines the 'writeText' method.
*/

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

int Data_writeToLispFile (I, MelderFile fs);
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

int Data_readText (I, MelderFile openFile);
/*
	Message:
		"try to read yourself as text from an open file."
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

Any Data_readFromBinaryFile (MelderFile fs);
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
	const char *name;   /* The name of this field. */
	int type;   /* bytewa..inheritwa, see below */
	int offset;   /* The offset of this field in the enveloping struct. */
	int size;   /* The size of this field if it is in an array. */
	const char *tagName;   /* For structs: tag; for classes: class name; for enums: type name. */
	void *tagType;   /* For structs: offset table; for classes: class pointer; for enums: enum pointer. */
	int rank;   /* 0 = single, 1 = vector, 2 = matrix, -1 = array. */
	const char *min1, *max1;   /* For vectors and matrices. */
	const char *min2, *max2;   /* For matrices. */
} *Data_Description;

#define Data_members Thing_members
#define Data_methods Thing_methods \
	struct structData_Description *description; \
	int (*copy) (Any data_from, Any data_to); \
	bool (*equal) (Any data1, Any data2); \
	bool (*canWriteAsAscii) (I); \
	int (*writeText) (I, MelderFile openFile); \
	int (*readText) (I, MelderFile openFile); \
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
	char * (*getRowStr) (I, long irow); \
	char * (*getColStr) (I, long icol); \
	double (*getCell) (I); \
	double (*getVector) (I, long irow, long icol); \
	char * (*getVectorStr) (I, long icol); \
	double (*getMatrix) (I, long irow, long icol); \
	char * (*getMatrixStr) (I, long irow, long icol); \
	double (*getFunction0) (I); \
	double (*getFunction1) (I, long irow, double x); \
	double (*getFunction2) (I, double x, double y); \
	double (*getRowIndex) (I, const char *rowLabel); \
	double (*getColumnIndex) (I, const char *columnLabel);
class_create (Data, Thing);

/*
	Methods. For the examples, we assume the following class definition:
		#define Miep_members Olie_members \
			double xmin, xmax; \
			long length; \
			float *array; \
			Theo object;
	We also assume the following class invariants:
		xmin < xmax;
		length >= 1;
		array is indexed as array [1..length];

	YOU WOULD NORMALLY PUT THE TYPE DESCRIPTION IN Miep_def.h:
		#define ooSTRUCT Miep
		oo_DEFINE_CLASS (Miep, Theo)
			DOUBLE (xmin)
			DOUBLE (xmax)
			LONG (length)
			FLOAT_VECTOR (array, my length)
			OBJECT (Theo, object)
		oo_END_CLASS (Miep)
		#undef ooSTRUCT
	See oo.h for more information.
	However, here are the descriptions of the methods:

	int copy (Any data_from, Any data_to)
		Message sent by Data_copy:
			deepen shallow copy, i.e.,
			perform a deep copy of all of my members who are arrays or objects.
		On entry:
			a shallow copy of all of my members to thee has been performed.
		Inheritor:
			1. Set all the arrays and objects that this class introduces to NULL
				(this removes the second references created by the shallow copy).
			2. Call the inherited 'copy' for copying the inherited arrays and objects.
				Use NUMxvector_copy and NUMxmatrix_copy for copying arrays.
				Use Data_copy for copying objects.
			3. If all went OK, return 1.
			As soon as anything goes wrong, return 0 without destroying any members.
		Example:
			Miep me = data_from, thee = data_to;   // xmin, xmax, and length already copied.
			thy array = NULL; thy object = NULL;
			if (! inherited (Miep) copy (me, thee)) return 0;   // i.e., classOlie -> copy
			if (! (thy array = NUMfvector_copy (my array, 1, my length))) return 0;
			if (! (thy object = Data_copy (my object))) return 0;
			return 1;
		Data::copy only sets thy name to NULL.
		After exit:
			if you returned 0, the 'thee' object will be destroyed.
		YOU WOULD NORMALLY USE oo_COPY.h INSTEAD OF IMPLEMENTING THIS ROUTINE BY YOURSELF:
			#include "oo_COPY.h"
			#include "Miep_def.h"
	
	int equal (Any data1, Any data2)
		Message sent by Data_equal:
			are your attributes deeply equal?
		On entry:
			'me' and 'thee' are members of the same class, but
			a shallow comparison of my and thy members has yielded a difference.
		Inheritor:
			compare all the members and return 0 soon as you find a difference.
			Call the inherited 'equal' for comparing the inherited members.
			Use NUMxvector_equal and NUMxmatrix_equal for comparing arrays.
			Use Data_equal for comparing objects.
			Return 1 if you find no difference.
		Example:
			Miep me = data1, thee = data2;
			return inherited (Miep) equal (me, thee) &&   // i.e., classOlie -> equal
				my xmin == thy xmin && my xmax == thy xmax &&
				my length == thy length &&
				NUMfvector_equal (my array, thy array, 1, my length) &&
				Data_equal (my object, thy object);
		Data::equal does nothing; it just returns 1.
		YOU WOULD NORMALLY USE oo_EQUAL.h INSTEAD OF IMPLEMENTING THIS ROUTINE BY YOURSELF:
			#include "oo_EQUAL.h"
			#include "Miep_def.h"

	int writeText (I, FILE *f)
		Message sent by Data_writeText:
			write all of my members as text to the stream f.
		Inheritor:
			You can either entirely override this method,
			or call the inherited method to write the inherited members.
			Return 1 if everything went OK.
			Return 0 as soon as you encounter an error; as you do not have to check
			for I/O errors, this will be rare.
		Example:
			iam (Miep);
			inherited (Miep) writeText (me, f);   // Olie's members.
			ascputr8 (my xmin, f, "xmin");
			ascputr8 (my xmax, f, "xmax");
			ascputi4 (my length, f, "length");
			NUMfvector_writeText (my array, 1, my length, f, "array");
			Data_writeText (my object, f);
			return 1;
		After exit:
			if you returned 0, or if there is an error on the stream (like disk full),
			Data_writeText queues an error message and returns 0.
		YOU WOULD NORMALLY USE oo_WRITE_TEXT.h INSTEAD OF IMPLEMENTING THIS ROUTINE BY YOURSELF:
			#include "oo_WRITE_TEXT.h"
			#include "Miep_def.h"

	int readText (I, FILE *f)
		Message sent by Data_readText:
			read all of my members as text from the stream f.
		Inheritor:
			You can either entirely override this method,
			or call the inherited method to read the inherited members.
			Return 1 if everything went OK.
			Return 0 as soon as you encounter a failure,
			like early end of file or a violation of a class invariant.
		Example:
			iam (Miep);
			char line [200];
			if (! inherited (Miep) readText (me, f)) return 0;
			my xmin = ascgetr8 (f);
			my xmax = ascgetr8 (f);
			if (my xmax <= my xmin)
				return Melder_error ("Miep::readText: xmax must be greater than xmin.");
			my length = ascgeti4 (f);
			if (my length < 1)
				return Melder_error ("Miep::readText: length must be positive, not %ld.", my length);
			if (! (my array = NUMfvector_readText (1, my length, f))) return 0;
			if (! (my object = new (Theo))) return 0;   // Out of memory.
			if (! Data_readText (my object, f))   // Recursion.
				return 0;
			return 1;
		After exit:
			if you returned 0, Data_readText relays your error message and returns 0.
		YOU WOULD NORMALLY USE oo_READ_TEXT.h INSTEAD OF IMPLEMENTING THIS ROUTINE BY YOURSELF:
			#include "oo_READ_TEXT.h"
			#include "Miep_def.h"

	int writeBinary (I, FILE *f)
		Message sent by Data_writeBinary:
			write all of my members as binary data to the stream f.
		Inheritor:
			1. Call the inherited method to write the inherited members.
			2. Use machine-independent IO (see binario.h),
				Data_writeBinary, and NUMxxxxxx_writeBinary,
				to write the members that this class introduces.
			3. Return 1 if everything went OK.
			Return 0 as soon as you encounter an error;
			you may ignore IO errors and end-of-file.
		Example:
			iam (Miep);
			inherited (Miep) writeBinary (me, f);
			binputr8 (my xmin, f);
			binputr8 (my xmax, f);
			binputi4 (my length, f);
			NUMfvector_writeBinary (my array, 1, my length, f);
			Data_writeBinary (my object, f);
			return 1;
		After exit:
			if you returned 0, or if there is an error on the stream (like disk full),
			Data_writeBinary queues an error message and returns 0.
		YOU WOULD NORMALLY USE oo_WRITE_BINARY.h INSTEAD OF IMPLEMENTING THIS ROUTINE BY YOURSELF:
			#include "oo_WRITE_BINARY.h"
			#include "Miep_def.h"

	int readBinary (I, FILE *f)
		Message sent by Data_readBinary:
			read all of my members as binary data from the stream f.
		Inheritor:
			1. Call the inherited method to read the inherited members.
			2. Use machine-independent IO (see binario.h),
				Data_readBinary, and NUMxxxxxx_readBinary,
				to read the members that this class introduces.
			3. Return 1 if everything went OK.
			Return 0 as soon as you encounter a failure,
			like a violation of a class invariant.
		Example:
			iam (Miep);
			if (! inherited (Miep) readBinary (me, f)) return 0;
			my xmin = bingetr8 (f);
			my xmax = bingetr8 (f);
			if (my xmax <= my xmin) return 0;
			if ((my length = bingeti4 (f)) < 1) return 0;
			if (! (my array = NUMfvector_readBinary (1, my length, f))) return 0;
			if (! (my object = new (Theo))) return 0;
			if (! Data_readBinary (my object, f)) return 0;
			return 1;
		After exit:
			if you returned 0, or if there is an error on the stream (like early end-of-file),
			Data_readBinary queues an error message and returns 0.
		YOU WOULD NORMALLY USE oo_READ_BINARY.h INSTEAD OF IMPLEMENTING THIS ROUTINE BY YOURSELF:
			#include "oo_READ_BINARY.h"
			#include "Miep_def.h"
*/

/* The values of 'type' in struct descriptions. */

#define bytewa  1
#define shortwa  2
#define intwa  3
#define longwa  4
#define ubytewa  5
#define ushortwa  6
#define uintwa  7
#define ulongwa  8
#define floatwa  9
#define doublewa  10
#define fcomplexwa  11
#define dcomplexwa  12
#define charwa  13
#define wcharwa  14
#define enumwa  15
#define lenumwa  16
#define booleanwa  17
#define questionwa  18
#define stringwa  19
#define stringwwa  20
#define lstringwa  21
#define lstringwwa  22
#define structwa  23
#define widgetwa  24
#define objectwa  25
#define collectionwa  26
#define inheritwa  27

/* Recursive routines for working with struct members. */

int Data_Description_countMembers (Data_Description structDescription);
/* Including inherited members. */

Data_Description Data_Description_findMatch (Data_Description structDescription, const char *member);
/* Find the location of member 'member' in a struct. */
/* If 'structDescription' describes a class, the ancestor classes are also searched. */

Data_Description Data_Description_findNumberUse (Data_Description structDescription, const char *string);
/* Find the first member that uses member 'string' in its size description (max1 or max2 fields). */

/* Retrieving data from object + description. */

long Data_Description_integer (void *structAddress, Data_Description description);
/* Convert data found at a certain offset from 'address' to an integer, according to the given 'description'. */

int Data_Description_evaluateInteger (void *structAddress, Data_Description structDescription,
	const char *formula, long *result);
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

Any Data_readFromFile (MelderFile fs);
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

/* End of file Data.h */
#endif
