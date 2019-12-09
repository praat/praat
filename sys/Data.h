#ifndef _Data_h_
#define _Data_h_
/* Data.h
 *
 * Copyright (C) 1992-2012,2013-2018 Paul Boersma
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

/* Data inherits from Thing. */
/* It adds the functionality of reproduction, comparison, reading, and writing. */
#include "Thing.h"

typedef struct structData_Description {
	conststring32 name;   // the name of this field
	int type;   // bytewa..inheritwa, see below
	int offset;   // the offset of this field in the enveloping struct
	int size;   // the size of this field if it is in an array
	conststring32 tagName;   // for structs: tag; for classes: class name; for enums: type name
	void *tagType;   // for structs: offset table; for classes: class pointer; for enums: enum pointer
	int rank;   // 0 = single, 1 = vector, 2 = matrix, 3 = set, -1 = array
	conststring32 min1, max1;   // for vectors and matrices
	conststring32 min2, max2;   // for matrices
} *Data_Description;

/*
 * We call the class Daata instead of Data,
 * because of a conflict with the use of "Data" in assembler strings in intrin-impl.h on Windows.
 */

Thing_define (Daata, Thing) {
	virtual void v_copy (Daata data_to);
	virtual bool v_equal (Daata otherData);
	virtual Data_Description v_description () { return nullptr; }
	virtual bool v_writable () { return true; }
	virtual bool v_canWriteAsEncoding (int outputEncoding);
	virtual void v_writeText (MelderFile openFile);
	virtual void v_readText (MelderReadText text, int formatVersion);
	virtual void v_writeBinary (FILE *f);
	virtual void v_readBinary (FILE *f, int formatVersion);
	virtual void v_repair () { }   // after reading Praat data files created by others
	// methods for scripting:
	virtual bool v_hasGetNrow      () { return false; }   virtual double        v_getNrow      ()                                       { return undefined; }
	virtual bool v_hasGetNcol      () { return false; }   virtual double        v_getNcol      ()                                       { return undefined; }
	virtual bool v_hasGetXmin      () { return false; }   virtual double        v_getXmin      ()                                       { return undefined; }
	virtual bool v_hasGetXmax      () { return false; }   virtual double        v_getXmax      ()                                       { return undefined; }
	virtual bool v_hasGetYmin      () { return false; }   virtual double        v_getYmin      ()                                       { return undefined; }
	virtual bool v_hasGetYmax      () { return false; }   virtual double        v_getYmax      ()                                       { return undefined; }
	virtual bool v_hasGetNx        () { return false; }   virtual double        v_getNx        ()                                       { return undefined; }
	virtual bool v_hasGetNy        () { return false; }   virtual double        v_getNy        ()                                       { return undefined; }
	virtual bool v_hasGetDx        () { return false; }   virtual double        v_getDx        ()                                       { return undefined; }
	virtual bool v_hasGetDy        () { return false; }   virtual double        v_getDy        ()                                       { return undefined; }
	virtual bool v_hasGetX         () { return false; }   virtual double        v_getX         (integer /* ix */)                       { return undefined; }
	virtual bool v_hasGetY         () { return false; }   virtual double        v_getY         (integer /* iy */)                       { return undefined; }
	virtual bool v_hasGetRowStr    () { return false; }   virtual conststring32 v_getRowStr    (integer /* irow */)                     { return nullptr;   }
	virtual bool v_hasGetColStr    () { return false; }   virtual conststring32 v_getColStr    (integer /* icol */)                     { return nullptr;   }
	virtual bool v_hasGetCell      () { return false; }   virtual double        v_getCell      ()                                       { return undefined; }
	virtual bool v_hasGetCellStr   () { return false; }   virtual conststring32 v_getCellStr   ()                                       { return nullptr;   }
	virtual bool v_hasGetVector    () { return false; }   virtual double        v_getVector    (integer /* irow */, integer /* icol */) { return undefined; }
	virtual bool v_hasGetVectorStr () { return false; }   virtual conststring32 v_getVectorStr (integer /* icol */)                     { return nullptr;   }
	virtual bool v_hasGetMatrix    () { return false; }   virtual double        v_getMatrix    (integer /* irow */, integer /* icol */) { return undefined; }
	virtual bool v_hasGetMatrixStr () { return false; }   virtual conststring32 v_getMatrixStr (integer /* irow */, integer /* icol */) { return nullptr;   }
	virtual bool v_hasGetFunction0 () { return false; }   virtual double        v_getFunction0 ()                                       { return undefined; }
	virtual bool v_hasGetFunction1 () { return false; }   virtual double        v_getFunction1 (integer /* irow */, double /* x */)     { return undefined; }
	virtual bool v_hasGetFunction2 () { return false; }   virtual double        v_getFunction2 (double /* x */, double /* y */)         { return undefined; }
	virtual bool v_hasGetRowIndex  () { return false; }   virtual double        v_getRowIndex  (conststring32 /* rowLabel */)           { return undefined; }
	virtual bool v_hasGetColIndex  () { return false; }   virtual double        v_getColIndex  (conststring32 /* colLabel */)           { return undefined; }
};

template <class T> autoSomeThing<T> Data_copy (T* data) {
	return _Data_copy (data).template static_cast_move <T> ();
}
autoDaata _Data_copy (Daata me);
/*
	Message:
		"return a deep copy of yourself."
	Postconditions:
		result -> name == nullptr;	  // the only attribute NOT copied
*/

bool Data_equal (Daata data1, Daata data2);
/*
	Message:
		"return true if the shallow or deep attributes of 'data1' and 'data2' are equal;
		 otherwise, return false."
	Comment:
		Data_equal (data, Data_copy (data)) should always return 1; the names are not compared.
*/

typedef MelderCompareHook<structDaata> Data_CompareHook;

bool Data_canWriteAsEncoding (Daata me, int outputEncoding);
/*
	Message:
		"Can you write yourself in that encoding?"
	The answer depends on whether all members can be written in that encoding.
*/

bool Data_canWriteText (Daata me);
/*
	Message:
		"Can you write yourself as text?"
	The answer depends on whether the subclass defines the 'writeText' method.
*/

MelderFile Data_createTextFile (
	Daata me,
	MelderFile file,
	bool verbose
);   // returns the input MelderFile in order to be caught by an autoMelderFile

void Data_writeText (Daata me, MelderFile openFile);
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

void Data_writeToTextFile (Daata me, MelderFile file);
/*
	Message:
		"try to write yourself as text to a file".
	Description:
		The first line is 'File type = "ooTextFile"'.
		Your class name is written in the second line,
		e.g., if you are a Person, the second line will be 'Object class = "Person"'.
		The format of the lines after the second line is the same as in Data_writeText.
*/

void Data_writeToShortTextFile (Daata me, MelderFile file);
/*
	Message:
		"try to write yourself as text to a file".
	Description:
		The first line is 'File type = "ooTextFile short"'.
		Your class name is written in the second line,
		e.g., if you are a Person, the second line will be '"Person"'.
		The format of the lines after the second line is the same as in Data_writeText.
*/

bool Data_canWriteBinary (Daata me);
/*
	Message:
		"Can you write yourself as binary data?"
	The answer depends on whether the subclass defines the 'writeBinary' method.
*/

void Data_writeBinary (Daata me, FILE *f);
/*
	Message:
		"try to write yourself as binary data to an open file."
	Failures:
		I/O error.
		Disk full.
	Description:
		The format depends on the 'writeBinary' method defined by the subclass,
		but is machine independent because it always uses 'most significant byte first'
		and IEEE floating-point format.
*/

void Data_writeToBinaryFile (Daata me, MelderFile file);
/*
	Message:
		"try to write yourself as binary data to a file".
	Description:
		First, your class name is written in the file,
		e.g., if you are a Person, the file will start with "PersonBinaryFile".
		The format of the file after this is the same as in Data_writeBinary.
*/

bool Data_canReadText (Daata me);
/*
	Message:
		"Can you read yourself as text?"
	The answer depends on whether the subclass defines a 'readText' method,
	but is preferably the same as the answer from Data_canWriteText.
*/

void Data_readText (Daata me, MelderReadText text, int formatVersion);
/*
	Message:
		"try to read yourself as text from a string."
	Failures:
		The 'readText' method of the subclass failed.
		I/O error.
		Early end of file detected.
	Description:
		The format depends on the 'readText' method defined by the subclass,
		but is preferably the same as the format produced by the 'writeText' method.
*/

autoDaata Data_readFromTextFile (MelderFile file);
/*
	Message:
		"try to read a Data as text from a file".
	Description:
		The Data's class name is read from the first line,
		e.g., if the first line is "PersonTextFile", the Data will be a Person.
		The format of the lines after the first line is the same as in Data_readText.
	Return value:
		the new object.
	Failures:
		Error opening file <fileName>.
		The file <fileName> does not contain an object.
		(plus those from Data_readText)
*/

bool Data_canReadBinary (Daata me);
/*
	Message:
		"Can you read yourself as binary data?"
	The answer depends on whether the subclass defines a 'readBinary' method,
	but is preferably the same as the answer from Data_canWriteBinary.
*/

void Data_readBinary (Daata me, FILE *f, int formatVersion);
/*
	Message:
		"try to read yourself as binary data from the stream <f>."
	Failures:
		The 'readBinary' method of the subclass throws an error.
		I/O error.
		Early end of file detected.
	Description:
		The format depends on the 'readBinary' method defined by the subclass,
		but is preferably the same as the format produced by the 'writeBinary' method.
*/

autoDaata Data_readFromBinaryFile (MelderFile file);
/*
	Message:
		"try to read a Data as binary data from a file".
	Description:
		The Data's class name is read from the start of the file,
		e.g., if the file starts with is "PersonBinaryFile", the Data will be a Person.
		The format of the file after this is the same as in Data_readBinary.
	Return value:
		the new object.
	Failures:
		Error opening file <fileName>.
		The file <fileName> does not contain an object.
		(plus those from Data_readBinary)
*/

using Data_FileTypeRecognizer = autoDaata (*) (integer numberOfBytesRead, const char *header, MelderFile file);

void Data_recognizeFileType (Data_FileTypeRecognizer recognizer);
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
	autoDaata Sound_sunAudioFileRecognizer (int nread, const char *header, MelderFile file) {
		if (nread >= 24 && strnequ (& header [0], ".snd", 4))
			return Sound_readFromSunAudioFile (file);
		else
			return autoDaata ();
	}
	From this example, we see that if the file is recognized, it should be read immediately,
	and the resulting object (always a descendant of class Data) should be returned.
	We also see that the return value nullptr is used for notifying Data_readFromFile
	of the fact that the file is not a Sun audio file.
Registering a file-type recognizer:
	You would put a statement like the following in the initialization section of your program:
	Data_recognizeFileType (Sound_sunAudioFileRecognizer);
	After this, Data_readFromFile is able to read Sun audio files.
*/

autoDaata Data_readFromFile (MelderFile file);
/*
Purpose:
	to read a file with data of any kind.
Return value:
	the object read from `file`.
Failure:
	the file was not recognized.
Behaviour:
	Data_readFromFile first checks whether the file is a text file
	that is readable by Data_readFromTextFile, or a binary file
	written by Data_writeToBinaryFile, or a file as written by Data_writeToLispFile.
	If one of these succeeds, the file is read and the resulting object is returned.
	If not, the recognizers installed with Data_recognizeFileType are tried.
*/

extern structMelderDir Data_directoryBeingRead;

int Data_publish (autoDaata me);

void Data_setPublishProc (int (*publish) (autoDaata));

/* The values of 'type' in struct descriptions. */

#define bytewa  1
#define int16wa  2
#define intwa  3
#define integerwa  4
#define ubytewa  5
#define uintwa  6
#define uintegerwa  7
#define floatwa  8
#define doublewa  9
#define complexwa  10
#define enumwa  11
#define lenumwa  12
#define booleanwa  13
#define questionwa  14
#define stringwa  15
#define lstringwa  16
#define maxsingletypewa lstringwa
#define structwa  17
#define widgetwa  18
#define objectwa  19
#define collectionofwa  20
#define collectionwa  21
#define inheritwa  22

/* Recursive routines for working with struct members. */

int Data_Description_countMembers (Data_Description structDescription);
/* Including inherited members. */

Data_Description Data_Description_findMatch (Data_Description structDescription, conststring32 member);
/* Find the location of member 'member' in a struct. */
/* If 'structDescription' describes a class, the ancestor classes are also searched. */

Data_Description Data_Description_findNumberUse (Data_Description structDescription, conststring32 string);
/* Find the first member that uses member 'string' in its size description (max1 or max2 fields). */

/* Retrieving data from object + description. */

int64 Data_Description_integer (void *structAddress, Data_Description description);
/* Convert data found at a certain offset from 'address' to an integer, according to the given 'description'. */

int Data_Description_evaluateInteger (void *structAddress, Data_Description structDescription,
	conststring32 formula, integer *result);
/*
 * Translates a string like '100' or 'numberOfHorses' or 'numberOfCows - 1' to an integer.
 * The 'algorithm' does some wild guesses as to the meanings of the 'min1' and 'max1' strings.
 * A full-fledged interpretation is preferable...
 * Returns 0 if 'formula' cannot be parsed to a number.
 */

/* End of file Data.h */
#endif
