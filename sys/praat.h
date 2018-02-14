#ifndef _praat_h_
#define _praat_h_
/* praat.h
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

#include "Editor.h"
#include "Manual.h"
#include "Preferences.h"

/* The explanations in this header file assume
	that you put your extra commands in praat_Sybil.cpp
	and the main() function in main_Sybil.cpp,
	but these files may have different names if you are not Sybil.
	Linking with the rest of Praat will create an executable
	that has all the functionality of the basic Praat,
	plus everything that you made available in praat_Sybil.cpp.
*/

/* Program example:
// File main_Sybil.cpp: //
int main (int argc, char **argv)
{
	praat_init (U"Praat_Sybil", argc, argv);   // Obligatory.
	INCLUDE_LIBRARY (praat_Fon_init)   // Optional: inherit phonetic stuff.
	INCLUDE_LIBRARY (praat_Sybil_init)   // Optional: add Sybil's things.
	INCLUDE_MANPAGES (manual_Sybil)
	praat_run ();   // Obligatory.
}
// File praat_Sybil.cpp: //
void praat_Sybil (void)
{
	...
	...
}
The dots consist of, in any order:
	Thing_recognizeClassesByName (...);
	Data_recognizeFileType (...);
	praat_addMenuCommand (...);
	praat_addAction1 (...);
All of these statements are optional and may occur more than once.
To make any class string-readable, use Thing_recognizeClassesByName ().
String-readable classes are known by Thing_newFromClassName () and can therefore
be read by Data_readFromTextFile () and Data_readFromBinaryFile ().
*/
void praat_init (const char32 *title, int argc, char **argv);
void praat_run ();
void praat_setStandAloneScriptText (const char32 *text);   // call before praat_init if you want to create a stand-alone application without Objects and Picture window
extern "C" void praatlib_init ();   // for use in an application that uses Praatlib

#define praat_addAction1(c1,n1,t,a,f,c)  praat_addAction1_ (c1, n1, t, a, f, c, U"" #c)
#define praat_addAction2(c1,n1,c2,n2,t,a,f,c)  praat_addAction2_ (c1, n1, c2, n2, t, a, f, c, U"" #c)
#define praat_addAction3(c1,n1,c2,n2,c3,n3,t,a,f,c)  praat_addAction3_ (c1, n1, c2, n2, c3, n3, t, a, f, c, U"" #c)
#define praat_addAction4(c1,n1,c2,n2,c3,n3,c4,n4,t,a,f,c)  praat_addAction4_ (c1, n1, c2, n2, c3, n3, c4, n4, t, a, f, c, U"" #c)

void praat_addAction1_ (ClassInfo class1, integer n1,
	const char32 *title, const char32 *after, uint32 flags, UiCallback callback, const char32 *nameOfCallback);
void praat_addAction2_ (ClassInfo class1, integer n1, ClassInfo class2, integer n2,
	const char32 *title, const char32 *after, uint32 flags, UiCallback callback, const char32 *nameOfCallback);
void praat_addAction3_ (ClassInfo class1, integer n1, ClassInfo class2, integer n2, ClassInfo class3, integer n3,
	const char32 *title, const char32 *after, uint32 flags, UiCallback callback, const char32 *nameOfCallback);
void praat_addAction4_ (ClassInfo class1, integer n1, ClassInfo class2, integer n2, ClassInfo class3, integer n3, ClassInfo class4, integer n4,
	const char32 *title, const char32 *after, uint32 flags, UiCallback callback, const char32 *nameOfCallback);
/*
	'title' is the name that will appear in the dynamic menu,
		and also the command that is used in command files;
		this title is reference-copied.
	'callback' refers to a function prototyped like this:
		static int DO_Class_action (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter, void *closure);
		this function should throw an exception if the command failed,
		and return 1 if the command was executed successfully;
		this function will be called by 'praat' when the user clicks a menu command,
		in which case 'sendingForm', 'args' and 'sendingString' and 'closure' will be null;
		it is also called by scripts,
		in which case 'args[1..n]' or 'sendingString' is the argument list (after the dots).
		When called by Ui (after UiForm_create), 'sendingForm' is the UiForm, and 'closure'
		is the closure you passed to UiForm_create (which may be an editor).

	The availability of the dynamic commands depends on
	the current selection: e.g., if the user has selected three objects
	of type Matrix and nothing else, the commands registered with
	praat_addAction1 (classMatrix, n, "xxx", "xxxx", x, DO_xxx) are visible,
	and those with n=0 or n=3 are executable (the buttons are sensitive)
	and, if chosen, performed on each of these three objects;
	if the user has selected one object of type Artword and one of type
	Speaker, the commands from praat_addAction2 (classArtword, 1, classSpeaker, 1, ...) are executable.
	You may want to restrict the availability to one object for commands that write objects to file,
	commands that present information in a dialog, or the View & Edit command.
*/
#define praat_INSENSITIVE  GuiMenu_INSENSITIVE
#define praat_CHECKBUTTON  GuiMenu_CHECKBUTTON
#define praat_TOGGLE_ON  GuiMenu_TOGGLE_ON
#define praat_ATTRACTIVE  GuiMenu_ATTRACTIVE
#define praat_RADIO_FIRST  GuiMenu_RADIO_FIRST
#define praat_RADIO_NEXT  GuiMenu_RADIO_NEXT
#define praat_HIDDEN  0x00004000
#define praat_UNHIDABLE  0x00008000
#define praat_DEPTH_1  0x00010000
#define praat_DEPTH_2  0x00020000
#define praat_DEPTH_3  0x00030000
#define praat_DEPTH_4  0x00040000
#define praat_DEPTH_5  0x00050000
#define praat_DEPTH_6  0x00060000
#define praat_DEPTH_7  0x00070000
#define praat_NO_API  0x00080000
#define praat_FORCE_API  0x00100000
#define praat_DEPRECATED  (0x00200000 | praat_HIDDEN)
#define praat_DEPRECATED_2004  (0x04200000 | praat_HIDDEN)
#define praat_DEPRECATED_2005  (0x05200000 | praat_HIDDEN)
#define praat_DEPRECATED_2006  (0x06200000 | praat_HIDDEN)
#define praat_DEPRECATED_2007  (0x07200000 | praat_HIDDEN)
#define praat_DEPRECATED_2008  (0x08200000 | praat_HIDDEN)
#define praat_DEPRECATED_2009  (0x09200000 | praat_HIDDEN)
#define praat_DEPRECATED_2010  (0x0A200000 | praat_HIDDEN)
#define praat_DEPRECATED_2011  (0x0B200000 | praat_HIDDEN)
#define praat_DEPRECATED_2012  (0x0C200000 | praat_HIDDEN)
#define praat_DEPRECATED_2013  (0x0D200000 | praat_HIDDEN)
#define praat_DEPRECATED_2014  (0x0E200000 | praat_HIDDEN)
#define praat_DEPRECATED_2015  (0x0F200000 | praat_HIDDEN)
#define praat_DEPRECATED_2016  (0x10200000 | praat_HIDDEN)
#define praat_DEPRECATED_2017  (0x11200000 | praat_HIDDEN)
/*
	The following three can also be used, but not for deprecated commands.
*/
//#define GuiMenu_OPTION  (1 << 24)
//#define GuiMenu_SHIFT  (1 << 25)
//#define GuiMenu_COMMAND  (1 << 26)
void praat_removeAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, const char32 *title);
	/* 'class2' and 'class3' may be null. */
	/* 'title' may be null; reference-copied. */

#define praat_addMenuCommand(w,m,t,a,f,c)  praat_addMenuCommand_ (w, m, t, a, f, c, U"" #c)
GuiMenuItem praat_addMenuCommand_ (const char32 *window, const char32 *menu, const char32 *title /* cattable */,
	const char32 *after, uint32 flags, UiCallback callback, const char32 *nameOfCallback);
/* All strings are reference-copied; 'title', 'after', and 'callback' may be null. */

#define praat_MAXNUM_EDITORS 5
#include "Ui.h"
typedef struct {
	ClassInfo klas;   // the class
	Daata object;   // the instance
	char32 *name;   // the name of the object as it appears in the List
	structMelderFile file;   // is this Object associated with a file?
	integer id;   // the unique number of the object
	bool isSelected;   // is the name of the object inverted in the list?
	Editor editors [praat_MAXNUM_EDITORS];   // are there editors open with this Object in it?
	bool isBeingCreated;
} structPraat_Object, *praat_Object;

#define praat_MAXNUM_OBJECTS 10000   /* Maximum number of objects in the list. */
typedef struct {   /* Readonly */
	MelderString batchName;   /* The name of the command file when called from batch. */
	int batch;   /* Was the program called from the command line? */
	GuiWindow topShell;   /* The application shell: parent of standard dialogs. */
	ManPages manPages;
} structPraatApplication, *PraatApplication;
typedef struct {   /* Readonly */
	int n;	 /* The current number of objects in the list. */
	structPraat_Object list [1 + praat_MAXNUM_OBJECTS];   /* The list of objects: list [1..n]. */
	int totalSelection;   /* The total number of selected objects, <= n. */
	int numberOfSelected [1 + 1000];   /* For each (readable) class. */
	int totalBeingCreated;
	integer uniqueId;
} structPraatObjects, *PraatObjects;
typedef struct {   // readonly
	Graphics graphics;   /* The Graphics associated with the Picture window or HyperPage window or Demo window. */
	int font, fontSize, lineType;
	Graphics_Colour colour;
	double lineWidth, arrowSize, speckleSize, x1NDC, x2NDC, y1NDC, y2NDC;
} structPraatPicture, *PraatPicture;
extern structPraatApplication theForegroundPraatApplication;
extern PraatApplication theCurrentPraatApplication;
extern structPraatObjects theForegroundPraatObjects;
extern PraatObjects theCurrentPraatObjects;
extern structPraatPicture theForegroundPraatPicture;
extern PraatPicture theCurrentPraatPicture;
	/* The global objects containing the state of the application; only reachable from interface files. */

char32 *praat_name (int iobject);
void praat_write_do (UiForm dia, const char32 *extension);
void praat_new (autoDaata me);
void praat_new (autoDaata me, Melder_1_ARG);
void praat_new (autoDaata me, Melder_2_ARGS);
void praat_new (autoDaata me, Melder_3_ARGS);
void praat_new (autoDaata me, Melder_4_ARGS);
void praat_new (autoDaata me, Melder_5_ARGS);
void praat_new (autoDaata me, Melder_6_ARGS);
void praat_new (autoDaata me, Melder_7_ARGS);
void praat_new (autoDaata me, Melder_8_ARGS);
void praat_new (autoDaata me, Melder_9_ARGS);
void praat_newWithFile (autoDaata me, MelderFile file, const char32 *name);
void praat_name2 (char32 *name, ClassInfo klas1, ClassInfo klas2);

/* Macros for description of forms (dialog boxes, setting windows).
	FORM prompts the user for arguments to proc.
	Macros for FORM:
	FORM (proc, title, helpString)
		`proc` is the `cb` argument of the corresponding command call.
		`title` is the title of the form, shown in its title bar.
		`helpString` may be null.
	INTEGER (variable, labelText, defaultStringValue)
	NATURAL (variable, labelText, defaultStringValue)
	REAL (variable, labelText, defaultStringValue)
	REAL_OR_UNDEFINED (variable, labelText, defaultStringValue)
	POSITIVE (variable, labelText, defaultStringValue)
	WORD (variable, labelText, defaultStringValue)
	SENTENCE (variable, labelText, defaultStringValue)
	COLOUR (variable, labelText, defaultStringValue)
	CHANNEL (variable, labelText, defaultStringValue)
	BOOLEAN (variable, labelText, defaultBooleanValue)
		the value is 0 (off) or 1 (on).
	LABEL (labelText)
	TEXTFIELD (variable, labelText, defaultStringValue)
	NUMVEC (variable, labelText, defaultStringValue)
	NUMMAT (variable, labelText, defaultStringValue)
	RADIO (variable, labelText, defaultOptionNumber, base)
		this should be followed by two or more RADIOBUTTONs;
		the initial value is between base and the number of radio buttons plus base-1.
	RADIOBUTTON (labelText)
	OPTIONMENU (variable, labelText, defaultOptionNumber)
		this should be followed by two or more OPTIONs;
		the initial value is between 1 and the number of options.
	OPTION (labelText)
	LIST (variable, labelText, numberOfStrings, strings, defaultOptionNumber)
	OK
		this statement is obligatory.
	SET_XXXXXX (name, value)
		sets the value of REAL..LIST.
	DO
	The order of the macros should be:
		FORM {
			one or more from REAL..LIST
		OK
			zero or more SETs
		DO
			...
		}
	See Ui.h for more information.
	Between DO and `}`, you can throw an exception if anything is wrong;
	if everything is all right, you just trickle down to END.
	Never do "return", because END will update the selection if an object is created.
 */

#ifndef _EditorM_h_

#define FORM(proc,name,helpTitle)  \
	extern "C" void proc (UiForm sendingForm, integer narg, Stackel args, const char32 *sendingString, Interpreter interpreter, const char32 *invokingButtonTitle, bool modified, void *buttonClosure); \
	void proc (UiForm sendingForm, integer narg, Stackel args, const char32 *sendingString, Interpreter interpreter, const char32 *invokingButtonTitle, bool modified, void *buttonClosure) { \
		int IOBJECT = 0; \
		(void) IOBJECT; \
		UiField radio = nullptr; \
		(void) radio; \
		static UiForm dia; \
		if (dia) goto dia_inited; \
		dia = UiForm_create (theCurrentPraatApplication -> topShell, name, proc, buttonClosure, invokingButtonTitle, helpTitle);

#define REAL(realVariable, labelText, defaultStringValue)  \
		static double realVariable; \
		UiForm_addReal (dia, & realVariable, U"" #realVariable, labelText, defaultStringValue);

#define REAL_OR_UNDEFINED(realVariable, labelText, defaultStringValue)  \
		static double realVariable; \
		UiForm_addRealOrUndefined (dia, & realVariable, U"" #realVariable, labelText, defaultStringValue);

#define POSITIVE(realVariable, labelText, defaultStringValue)  \
		static double realVariable; \
		UiForm_addPositive (dia, & realVariable, U"" #realVariable, labelText, defaultStringValue);

#define INTEGER(integerVariable, labelText, defaultStringValue)  \
		static integer integerVariable; \
		UiForm_addInteger (dia, & integerVariable, U"" #integerVariable, labelText, defaultStringValue);

#define NATURAL(integerVariable, labelText, defaultStringValue)  \
		static integer integerVariable; \
		UiForm_addNatural (dia, & integerVariable, U"" #integerVariable, labelText, defaultStringValue);

#define WORD(stringVariable, labelText, defaultStringValue)  \
		static char32 *stringVariable; \
		UiForm_addWord (dia, & stringVariable, U"" #stringVariable, labelText, defaultStringValue);

#define SENTENCE(stringVariable, labelText, defaultStringValue)  \
		static char32 *stringVariable; \
		UiForm_addSentence (dia, & stringVariable, U"" #stringVariable, labelText, defaultStringValue);

#define BOOLEAN(booleanVariable, labelText, defaultBooleanValue)  \
		static bool booleanVariable; \
		UiForm_addBoolean (dia, & booleanVariable, U"" #booleanVariable, labelText, defaultBooleanValue);

#define LABEL(labelText)  UiForm_addLabel (dia, nullptr, labelText);

#define MUTABLE_LABEL(stringVariable, labelText) \
		static char32 *stringVariable; \
		UiForm_addLabel (dia, & stringVariable, labelText);

#define TEXTFIELD(stringVariable, labelText, defaultStringValue)  \
		if (labelText != nullptr) /* an explicit nullptr comparison, because string literals don't convert well to bools */ \
			UiForm_addLabel (dia, nullptr, labelText); \
		static char32 *stringVariable; \
		UiForm_addText (dia, & stringVariable, U"" #stringVariable, U"", defaultStringValue);

#define NUMVEC(numericVectorVariable, labelText, defaultStringValue)  \
		if (labelText != nullptr) /* an explicit nullptr comparison, because string literals don't convert well to bools */ \
			UiForm_addLabel (dia, nullptr, labelText); \
		static numvec numericVectorVariable; \
		UiForm_addNumvec (dia, & numericVectorVariable, U"" #numericVectorVariable, U"", defaultStringValue);

#define NUMMAT(numericMatrixVariable, labelText, defaultStringValue)  \
		if (labelText != nullptr) /* an explicit nullptr comparison, because string literals don't convert well to bools */ \
			UiForm_addLabel (dia, nullptr, labelText); \
		static nummat numericMatrixVariable; \
		UiForm_addNummat (dia, & numericMatrixVariable, U"" #numericMatrixVariable, U"", defaultStringValue);

#define RADIO(intVariable, labelText, defaultOptionNumber)  \
		static int intVariable; \
		radio = UiForm_addRadio (dia, & intVariable, nullptr, U"" #intVariable, labelText, defaultOptionNumber, 1);

#define RADIOx(intVariable, labelText, defaultOptionNumber, base)  \
		static int intVariable; \
		radio = UiForm_addRadio (dia, & intVariable, nullptr, U"" #intVariable, labelText, defaultOptionNumber, base);

#define RADIOSTR(stringVariable, labelText, defaultOptionNumber)  \
		static char32 *stringVariable; \
		radio = UiForm_addRadio (dia, nullptr, & stringVariable, U"" #stringVariable, labelText, defaultOptionNumber, 1);

#define RADIOBUTTON(labelText)  \
		UiRadio_addButton (radio, labelText);

#define OPTIONMENU(intVariable, labelText, defaultOptionNumber)  \
		static int intVariable; \
		radio = UiForm_addOptionMenu (dia, & intVariable, nullptr, U"" #intVariable, labelText, defaultOptionNumber, 1);

#define OPTIONMENUx(intVariable, labelText, defaultOptionNumber, base)  \
		static int intVariable; \
		radio = UiForm_addOptionMenu (dia, & intVariable, nullptr, U"" #intVariable, labelText, defaultOptionNumber, base);

#define OPTIONMENUSTR(stringVariable, labelText, defaultOptionNumber)  \
		static char32 *stringVariable; \
		radio = UiForm_addOptionMenu (dia, nullptr, & stringVariable, U"" #stringVariable, labelText, defaultOptionNumber, 1);

#define OPTION(labelText)  \
		UiOptionMenu_addButton (radio, labelText);

#define RADIO_ENUM(enumeratedVariable, labelText, EnumeratedType, defaultEnumeratedSubvalue)  \
		static enum EnumeratedType enumeratedVariable; \
		radio = UiForm_addRadio (dia, (int *) & enumeratedVariable, nullptr, U"" #enumeratedVariable, labelText, \
			(int) EnumeratedType::defaultEnumeratedSubvalue - (int) EnumeratedType::MIN + 1, (int) EnumeratedType::MIN); \
		for (int ienum = (int) EnumeratedType::MIN; ienum <= (int) EnumeratedType::MAX; ienum ++) \
			UiRadio_addButton (radio, EnumeratedType##_getText ((enum EnumeratedType) ienum));

#define OPTIONMENU_ENUM(enumeratedVariable, labelText, EnumeratedType, defaultEnumeratedSubvalue)  \
		static EnumeratedType enumeratedVariable; \
		radio = UiForm_addOptionMenu (dia, (int *) & enumeratedVariable, nullptr, U"" #enumeratedVariable, labelText, \
			(int) EnumeratedType::defaultEnumeratedSubvalue - (int) EnumeratedType::MIN + 1, (int) EnumeratedType::MIN); \
		for (int ienum = (int) EnumeratedType::MIN; ienum <= (int) EnumeratedType::MAX; ienum ++) \
			UiOptionMenu_addButton (radio, EnumeratedType##_getText ((enum EnumeratedType) ienum));

#define OPTIONMENU_ENUMSTR(enumeratedVariableAsString, labelText, EnumeratedType, defaultEnumeratedSubvalue)  \
		static char32 *enumeratedVariableAsString; \
		radio = UiForm_addOptionMenu (dia, nullptr, & enumeratedVariableAsString, U"" #enumeratedVariableAsString, labelText, \
			(int) EnumeratedType::defaultEnumeratedSubvalue - (int) EnumeratedType::MIN + 1, (int) EnumeratedType::MIN); \
		for (int ienum = (int) EnumeratedType::MIN; ienum <= (int) EnumeratedType::MAX; ienum ++) \
			UiOptionMenu_addButton (radio, EnumeratedType##_getText ((enum EnumeratedType) ienum));

#define LIST(integerVariable, labelText, numberOfStrings, strings, defaultOptionNumber)  \
		static integer integerVariable; \
		UiForm_addList (dia, & integerVariable, nullptr, U"" #integerVariable, labelText, numberOfStrings, strings, defaultOptionNumber);

#define LISTSTR(stringVariable, labelText, numberOfStrings, strings, defaultOptionNumber)  \
		static char32 *stringVariable; \
		UiForm_addList (dia, nullptr, & stringVariable, U"" #stringVariable, labelText, numberOfStrings, strings, defaultOptionNumber);

#define FILE_IN(labelText)  \
		UiForm_addFileIn (dia, labelText);

#define FILE_OUT(labelText, defaultStringValue)  \
		UiForm_addFileOut (dia, labelText, defaultStringValue);

#define COLOUR(colourVariable, labelText, defaultStringValue)  \
		static Graphics_Colour colourVariable; \
		UiForm_addColour (dia, & colourVariable, U"" #colourVariable, labelText, defaultStringValue);

#define CHANNEL(integerVariable, labelText, defaultStringValue)  \
		static integer integerVariable; \
		UiForm_addChannel (dia, & integerVariable, U"" #integerVariable, labelText, defaultStringValue);

#define OK  \
		UiForm_finish (dia); \
	dia_inited: \
		if (narg < 0) UiForm_info (dia, narg); else if (! args && ! sendingForm && ! sendingString) {

#define SET_REAL(realVariable, realValue)  \
			UiForm_setReal (dia, & realVariable, realValue);

#define SET_INTEGER(integerVariable, integerValue)  \
			UiForm_setInteger (dia, & integerVariable, integerValue);

#define SET_BOOLEAN(booleanVariable, booleanValue)  \
			UiForm_setBoolean (dia, & booleanVariable, booleanValue);

#define SET_STRING(stringVariable, stringValue)  \
			UiForm_setString (dia, & stringVariable, stringValue);

#define SET_ENUM(enumeratedVariable, EnumeratedType, enumeratedValue)  \
			enumeratedVariable = enumeratedValue; /* just for typechecking */ \
			UiForm_setOption (dia, (int *) & enumeratedVariable, (int) enumeratedValue - (int) EnumeratedType::MIN + 1);

#define DO  \
			UiForm_do (dia, modified); \
		} else if (! sendingForm) { \
			trace (U"args ", Melder_pointer (args)); \
			if (args) { \
				UiForm_call (dia, narg, args, interpreter); \
			} else { \
				UiForm_parseString (dia, sendingString, interpreter); \
			} \
		} else { \
			try { \
				{

#define DO_ALTERNATIVE(alternative)  \
			UiForm_do (dia, modified); \
		} else if (! sendingForm) { \
			trace (U"alternative args ", Melder_pointer (args)); \
			try { \
				if (args) { \
					UiForm_call (dia, narg, args, interpreter); \
				} else { \
					UiForm_parseString (dia, sendingString, interpreter); \
				} \
			} catch (MelderError) { \
				char32 *parkedError = Melder_dup_f (Melder_getError ()); \
				Melder_clearError (); \
				try { \
					alternative (nullptr, narg, args, sendingString, interpreter, invokingButtonTitle, modified, buttonClosure); \
				} catch (MelderError) { \
					Melder_clearError (); \
					Melder_appendError (parkedError); \
					Melder_free (parkedError); \
					throw; \
				} \
				Melder_free (parkedError); \
			} \
		} else { \
			try { \
				{

#define END  \
				} \
			} catch (MelderError) { \
				praat_updateSelection (); \
				throw; \
			} \
			praat_updateSelection (); \
		} \
	}

#define END_NO_NEW_DATA  \
				} \
			} catch (MelderError) { \
				throw; \
			} \
		} \
	}

#define DIRECT(proc)  \
	extern "C" void proc (UiForm dummy1, integer narg, Stackel args, const char32 *dummy2, Interpreter interpreter, const char32 *dummy4, bool dummy5, void *dummy6); \
	void proc (UiForm dummy1, integer narg, Stackel args, const char32 *dummy2, Interpreter interpreter, const char32 *dummy4, bool dummy5, void *dummy6) { \
		(void) dummy1; (void) narg; (void) args; (void) dummy2; (void) interpreter; (void) dummy4; (void) dummy5; (void) dummy6; \
		int IOBJECT = 0; \
		(void) IOBJECT; \
		{ { \
			try {

#define FORM_READ(proc,title,help,allowMult)  \
	extern "C" void proc (UiForm sendingForm, integer, Stackel args, const char32 *sendingString, Interpreter, const char32 *invokingButtonTitle, bool, void *okClosure); \
	void proc (UiForm sendingForm, integer narg, Stackel args, const char32 *sendingString, Interpreter, const char32 *invokingButtonTitle, bool, void *okClosure) { \
		{ static UiForm dia; \
		if (! dia) \
			dia = UiInfile_create (theCurrentPraatApplication -> topShell, title, proc, okClosure, invokingButtonTitle, help, allowMult); \
		if (narg < 0) UiForm_info (dia, narg); else if (! args && ! sendingForm && ! sendingString) { \
			UiInfile_do (dia); \
		} else { \
			try { \
				MelderFile file; \
				int IOBJECT = 0; \
				(void) IOBJECT; \
				structMelderFile _file2 { };  /* don't move this into an inner scope, because the contents of a local variable don't persist into the outer scope */ \
				if (! args && ! sendingString) { \
					file = UiFile_getFile (dia); \
				} else { \
					Melder_require (! args || narg == 1, U"Command requires exactly 1 argument, the name of the file to read, instead of the given ", narg, U" arguments."); \
					Melder_require (! args || args [1]. which == Stackel_STRING, U"The file name argument should be a string, not ", Stackel_whichText (& args [1]), U"."); \
					Melder_relativePathToFile (args ? args [1]. string : sendingString, & _file2); \
					file = & _file2; \
				}

#define FORM_SAVE(proc,title,help,ext)  \
	extern "C" void proc (UiForm sendingForm, integer, Stackel args, const char32 *sendingString, Interpreter, const char32 *invokingButtonTitle, bool, void *okClosure); \
	void proc (UiForm sendingForm, integer narg, Stackel args, const char32 *sendingString, Interpreter, const char32 *invokingButtonTitle, bool, void *okClosure) { \
		{ static UiForm dia; \
		if (! dia) \
			dia = UiOutfile_create (theCurrentPraatApplication -> topShell, title, proc, okClosure, invokingButtonTitle, help); \
		if (narg < 0) UiForm_info (dia, narg); else if (! args && ! sendingForm && ! sendingString) { \
			praat_write_do (dia, ext); \
		} else { \
			try { \
				MelderFile file; \
				int IOBJECT = 0; \
				(void) IOBJECT; \
				structMelderFile _file2 { };  /* don't move this into an inner scope, because the contents of a local variable don't persist into the outer scope */ \
				if (! args && ! sendingString) { \
					file = UiFile_getFile (dia); \
				} else { \
					Melder_require (! args || narg == 1, U"Command requires exactly 1 argument, the name of the file to write, instead of the given ", narg, U" arguments."); \
					Melder_require (! args || args [1]. which == Stackel_STRING, U"The file name argument should be a string, not ", Stackel_whichText (& args [1]), U"."); \
					Melder_relativePathToFile (args ? args [1]. string : sendingString, & _file2); \
					file = & _file2; \
				}

#define GET_FILE(name)  UiForm_getFile (dia, name)

#endif // _EditorM_h_

#define iam_LOOP(klas)  klas me = static_cast<klas> (OBJECT)
#define WHERE(condition)  for (IOBJECT = 1; IOBJECT <= theCurrentPraatObjects -> n; IOBJECT ++) if (condition)
#define WHERE_DOWN(condition)  for (IOBJECT = theCurrentPraatObjects -> n; IOBJECT > 0; IOBJECT --) if (condition)
#define SELECTED  (theCurrentPraatObjects -> list [IOBJECT]. isSelected)
#define LOOP  for (IOBJECT = 1; IOBJECT <= theCurrentPraatObjects -> n; IOBJECT ++) if (SELECTED)
#define CLASS  (theCurrentPraatObjects -> list [IOBJECT]. klas)
#define OBJECT  (theCurrentPraatObjects -> list [IOBJECT]. object)
#define GRAPHICS  theCurrentPraatPicture -> graphics
#define FULL_NAME  (theCurrentPraatObjects -> list [IOBJECT]. name)
#define ID  (theCurrentPraatObjects -> list [IOBJECT]. id)
#define ID_AND_FULL_NAME  Melder_cat (ID, U". ", FULL_NAME)
#define NAME  praat_name (IOBJECT)

#define CREATE_ONE
#define CREATE_ONE_END(...)  praat_new (result.move(), __VA_ARGS__); END

#define FIND_ONE(klas)  \
	klas me = nullptr; \
	LOOP { if (CLASS == class##klas || Thing_isSubclass (CLASS, class##klas)) me = (klas) OBJECT; break; }

#define FIND_ONE_WITH_IOBJECT(klas)  \
	klas me = nullptr; int _klas_position = 0; \
	LOOP { if (CLASS == class##klas) me = (klas) OBJECT, _klas_position = IOBJECT; break; } \
	IOBJECT = _klas_position;

#define FIND_TWO(klas1,klas2)  \
	klas1 me = nullptr; klas2 you = nullptr; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT; else if (CLASS == class##klas2) you = (klas2) OBJECT; \
	if (me && you) break; }

#define FIND_TWO_WITH_IOBJECT(klas1,klas2)  \
	klas1 me = nullptr; klas2 you = nullptr; int _klas1_position = 0; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT, _klas1_position = IOBJECT; \
		else if (CLASS == class##klas2) you = (klas2) OBJECT; if (me && you) break; } \
	IOBJECT = _klas1_position;

#define FIND_COUPLE(klas)  \
	klas me = nullptr, you = nullptr; \
	LOOP if (CLASS == class##klas || Thing_isSubclass (CLASS, class##klas)) (me ? you : me) = (klas) OBJECT;

#define FIND_COUPLE_AND_ONE(klas1,klas2)  \
	klas1 me = nullptr, you = nullptr; klas2 him = nullptr; \
	LOOP { if (CLASS == class##klas1) (me ? you : me) = (klas1) OBJECT; else if (CLASS == class##klas2) him = (klas2) OBJECT; \
	if (me && you && him) break; }

#define FIND_ONE_AND_COUPLE(klas1,klas2)  \
	klas1 me = nullptr; klas2 you = nullptr, him = nullptr; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT; else if (CLASS == class##klas2) (you ? him : you) = (klas2) OBJECT; \
	if (me && you && him) break; }

#define FIND_THREE(klas1,klas2,klas3)  \
	klas1 me = nullptr; klas2 you = nullptr; klas3 him = nullptr; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT; else if (CLASS == class##klas2) you = (klas2) OBJECT; \
	else if (CLASS == class##klas3) him = (klas3) OBJECT; if (me && you && him) break; }

#define FIND_FOUR(klas1,klas2,klas3,klas4)  \
	klas1 me = nullptr; klas2 you = nullptr; klas3 him = nullptr; klas4 she = nullptr; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT; else if (CLASS == class##klas2) you = (klas2) OBJECT; \
	else if (CLASS == class##klas3) him = (klas3) OBJECT; else if (CLASS == class##klas4) she = (klas4) OBJECT; \
	if (me && you && him && she) break; }

#define FIND_LIST(klas)  \
	OrderedOf<struct##klas> list; \
	LOOP { iam_LOOP (klas); list. addItem_ref (me); }
	
#define FIND_TYPED_LIST(klas,listClass)  \
	auto##listClass list = listClass##_create (); \
	LOOP { iam_LOOP (klas); list -> addItem_ref (me); }
	
#define FIND_ONE_AND_LIST(klas1,klas2)  \
	OrderedOf<struct##klas2> list; klas1 me = nullptr; \
	LOOP { if (CLASS == class##klas2) list. addItem_ref ((klas2) OBJECT); else if (CLASS == class##klas1) me = (klas1) OBJECT; }

#define FIND_ONE_AND_TYPED_LIST(klas1,klas2,listClass)  \
	auto##listClass list = listClass##_create (); klas1 me = nullptr; \
	LOOP { if (CLASS == class##klas2) list -> addItem_ref ((klas2) OBJECT); else if (CLASS == class##klas1) me = (klas1) OBJECT; }

#define FIND_TWO_AND_LIST(klas1,klas2,klas3)  \
	OrderedOf<struct##klas3> list; klas1 me = nullptr; klas2 you = nullptr; \
	LOOP { if (CLASS == class##klas3) list. addItem_ref ((klas3) OBJECT); else if (CLASS == class##klas1) me = (klas1) OBJECT; \
	else if (CLASS == class##klas2) you = (klas2) OBJECT; }

#define INFO_NONE
#define INFO_NONE_END  END_NO_NEW_DATA

#define INFO_ONE(klas)  FIND_ONE (klas)
#define INFO_ONE_END  END_NO_NEW_DATA

#define INFO_TWO(klas1,klas2)  FIND_TWO (klas1, klas2)
#define INFO_TWO_END  END_NO_NEW_DATA

#define INFO_COUPLE(klas)  FIND_COUPLE (klas)
#define INFO_COUPLE_END  END_NO_NEW_DATA

#define INFO_THREE(klas1,klas2,klas3)  FIND_THREE (klas1, klas2, klas3)
#define INFO_THREE_END  END_NO_NEW_DATA

#define HELP(page)  Melder_help (page); END_NO_NEW_DATA

#define PLAY_EACH(klas)  LOOP { iam_LOOP (klas);
#define PLAY_EACH_END  } END_NO_NEW_DATA

#define GRAPHICS_NONE  autoPraatPicture picture;
#define GRAPHICS_NONE_END  END_NO_NEW_DATA

#define GRAPHICS_EACH(klas)  autoPraatPicture picture; LOOP { iam_LOOP (klas);
#define GRAPHICS_EACH_END  } END_NO_NEW_DATA

#define GRAPHICS_TWO(klas1,klas2)  autoPraatPicture picture; FIND_TWO (klas1, klas2)
#define GRAPHICS_TWO_END  END_NO_NEW_DATA

#define GRAPHICS_COUPLE(klas)  autoPraatPicture picture; FIND_COUPLE (klas)
#define GRAPHICS_COUPLE_END  END_NO_NEW_DATA

#define GRAPHICS_COUPLE_AND_ONE(klas1,klas2)  autoPraatPicture picture; FIND_COUPLE_AND_ONE (klas1, klas2)
#define GRAPHICS_COUPLE_AND_ONE_END  END_NO_NEW_DATA

#define MOVIE_ONE(klas,title,width,height)  \
	Graphics graphics = Movie_create (title, width, height); \
	FIND_ONE (klas)
#define MOVIE_ONE_END  END_NO_NEW_DATA

#define MOVIE_TWO(klas1,klas2,title,width,height)  \
	Graphics graphics = Movie_create (title, width, height); \
	FIND_TWO (klas1, klas2)
#define MOVIE_TWO_END  END_NO_NEW_DATA

#define MOVIE_THREE(klas1,klas2,klas3,title,width,height)  \
	Graphics graphics = Movie_create (title, width, height); \
	FIND_THREE (klas1, klas2, klas3)
#define MOVIE_THREE_END  END_NO_NEW_DATA

#define NUMBER_ONE(klas)  FIND_ONE (klas)
#define NUMBER_ONE_END(...)  Melder_information (result, __VA_ARGS__); END_NO_NEW_DATA

#define NUMBER_TWO(klas1,klas2)  FIND_TWO (klas1, klas2)
#define NUMBER_TWO_END(...)  Melder_information (result, __VA_ARGS__); END_NO_NEW_DATA

#define NUMBER_THREE(klas1,klas2,klas3)  FIND_THREE (klas1, klas2, klas3)
#define NUMBER_THREE_END(...)  Melder_information (result, __VA_ARGS__); END_NO_NEW_DATA

#define NUMBER_COUPLE(klas)  FIND_COUPLE (klas)
#define NUMBER_COUPLE_END(...)  Melder_information (result, __VA_ARGS__); END_NO_NEW_DATA

#define NUMBER_COUPLE_AND_ONE(klas1,klas2)  FIND_COUPLE_AND_ONE (klas1, klas2)
#define NUMBER_COUPLE_AND_ONE_END(...)  Melder_information (result, __VA_ARGS__); END_NO_NEW_DATA

#define NUMBER_ONE_AND_LIST(klas1,klas2)  FIND_ONE_AND_LIST (klas1, klas2)
#define NUMBER_ONE_AND_LIST_END(...)  Melder_information (result, __VA_ARGS__); END_NO_NEW_DATA

#define NUMBER_TWO_AND_LIST(klas1,klas2,klas3)  FIND_TWO_AND_LIST (klas1, klas2, klas3)
#define NUMBER_TWO_AND_LIST_END(...)  Melder_information (result, __VA_ARGS__); END_NO_NEW_DATA

#define INTEGER_ONE(klas)  FIND_ONE (klas)
#define INTEGER_ONE_END(...)  Melder_information (result, __VA_ARGS__); END_NO_NEW_DATA

#define STRING_ONE(klas)  FIND_ONE (klas)
#define STRING_ONE_END  Melder_information (result); END_NO_NEW_DATA

#define NUMVEC_ONE(klas)  FIND_ONE (klas)
#define NUMVEC_ONE_END  if (interpreter) theInterpreterNumvec = result.move(); else Melder_information (result.get()); END_NO_NEW_DATA

#define NUMMAT_ONE(klas)  FIND_ONE (klas)
#define NUMMAT_ONE_END  if (interpreter) theInterpreterNummat = result.move(); else Melder_information (result.get()); END_NO_NEW_DATA

#define MODIFY_EACH(klas)  LOOP { iam_LOOP (klas);
#define MODIFY_EACH_END  praat_dataChanged (me); } END_NO_NEW_DATA

#define MODIFY_EACH_WEAK(klas)  LOOP { iam_LOOP (klas); try {
#define MODIFY_EACH_WEAK_END  praat_dataChanged (me); } catch (MelderError) { praat_dataChanged (me); throw; } } END_NO_NEW_DATA

#define MODIFY_FIRST_OF_TWO(klas1,klas2)  FIND_TWO (klas1, klas2)
#define MODIFY_FIRST_OF_TWO_END  praat_dataChanged (me); END_NO_NEW_DATA

#define MODIFY_FIRST_OF_TWO_WEAK(klas1,klas2)  FIND_TWO (klas1, klas2) try {
#define MODIFY_FIRST_OF_TWO_WEAK_END  praat_dataChanged (me); } catch (MelderError) { praat_dataChanged (me); throw; } END_NO_NEW_DATA

#define MODIFY_FIRST_OF_THREE(klas1,klas2,klas3)  FIND_THREE (klas1, klas2, klas3)
#define MODIFY_FIRST_OF_THREE_END  praat_dataChanged (me); END_NO_NEW_DATA

#define MODIFY_FIRST_OF_ONE_AND_COUPLE(klas1,klas2)  FIND_ONE_AND_COUPLE (klas1, klas2)
#define MODIFY_FIRST_OF_ONE_AND_COUPLE_END  praat_dataChanged (me); END_NO_NEW_DATA

#define MODIFY_FIRST_OF_ONE_AND_COUPLE_WEAK(klas1,klas2)  FIND_ONE_AND_COUPLE (klas1, klas2) try {
#define MODIFY_FIRST_OF_ONE_AND_COUPLE_WEAK_END  praat_dataChanged (me); } catch (MelderError) { praat_dataChanged (me); throw; } END_NO_NEW_DATA

#define MODIFY_FIRST_OF_ONE_AND_LIST(klas1,klas2)  FIND_ONE_AND_LIST (klas1, klas2)
#define MODIFY_FIRST_OF_ONE_AND_LIST_END  praat_dataChanged (me); END_NO_NEW_DATA

#define CONVERT_EACH(klas)  LOOP { iam_LOOP (klas);
#define CONVERT_EACH_END(...)  praat_new (result.move(), __VA_ARGS__); } END

#define CONVERT_TWO(klas1,klas2)  FIND_TWO (klas1, klas2)
#define CONVERT_TWO_END(...)  praat_new (result.move(), __VA_ARGS__); END

#define CONVERT_COUPLE(klas)  FIND_COUPLE (klas)
#define CONVERT_COUPLE_END(...)  praat_new (result.move(), __VA_ARGS__); END

#define CONVERT_COUPLE_AND_ONE(klas1,klas2)  FIND_COUPLE_AND_ONE (klas1,klas2)
#define CONVERT_COUPLE_AND_ONE_END(...)  praat_new (result.move(), __VA_ARGS__); END

#define CONVERT_THREE(klas1,klas2,klas3)  FIND_THREE (klas1, klas2, klas3)
#define CONVERT_THREE_END(...)  praat_new (result.move(), __VA_ARGS__); END

#define CONVERT_FOUR(klas1,klas2,klas3,klas4)  FIND_FOUR (klas1, klas2, klas3, klas4)
#define CONVERT_FOUR_END(...)  praat_new (result.move(), __VA_ARGS__); END

#define CONVERT_LIST(klas)  FIND_LIST (klas)
#define CONVERT_LIST_END(...)  praat_new (result.move(), __VA_ARGS__); END

#define CONVERT_TYPED_LIST(klas,listClass)  FIND_TYPED_LIST (klas,listClass)
#define CONVERT_TYPED_LIST_END(...)  praat_new (result.move(), __VA_ARGS__); END

#define CONVERT_ONE_AND_LIST(klas1,klas2) FIND_ONE_AND_LIST (klas1, klas2)
#define CONVERT_ONE_AND_LIST_END(...) praat_new (result.move(), __VA_ARGS__); END

#define CONVERT_ONE_AND_TYPED_LIST(klas1,klas2,listClass) FIND_ONE_AND_TYPED_LIST (klas1, klas2, listClass)
#define CONVERT_ONE_AND_TYPED_LIST_END(...) praat_new (result.move(), __VA_ARGS__); END

#define READ_ONE
#define READ_ONE_END  praat_newWithFile (result.move(), file, MelderFile_name (file)); END

#define SAVE_ONE(klas)  FIND_ONE (klas)
#define SAVE_ONE_END  END_NO_NEW_DATA

#define SAVE_LIST(klas)  FIND_LIST (klas)
#define SAVE_LIST_END  END_NO_NEW_DATA

#define SAVE_TYPED_LIST(klas,listClass)  FIND_TYPED_LIST (klas, listClass)
#define SAVE_TYPED_LIST_END  END_NO_NEW_DATA

/* Used by praat_Sybil.cpp, if you put an Editor on the screen: */
int praat_installEditor (Editor editor, int iobject);
/* This routine adds a reference to a new editor (unless it is null) to the screen object
   which is in the list at position 'iobject'.
   It sets the destroyCallback and dataChangedCallback as appropriate for Praat:
   the destroyCallback will set the now dangling reference to nullptr,
   so that a subsequent click on the "View & Edit" button will create a new editor;
   the dataChangedCallback will notify an open DataEditor with the same data,
   after that data will have changed.
      Return value: normally 1, but 0 if 'editor' is null.
   A typical calling sequence is:
	DIRECT (WINDOW_Spectrogram_viewAndEdit) {
		if (praat.batch) Melder_throw (U"Cannot view or edit a Spectrogram from batch.");
		else WHERE (SELECTED)
			praat_installEditor
				(SpectrogramEditor_create (praat.topShell, ID_AND_FULL_NAME, OBJECT), IOBJECT);
	END }
*/
int praat_installEditor2 (Editor editor, int iobject1, int iobject2);
int praat_installEditor3 (Editor editor, int iobject1, int iobject2, int iobject3);
int praat_installEditorN (Editor editor, DaataList objects);

void praat_dataChanged (Daata object);
/* Call this after changing a screen object. */
/* Associated editors and data editors will be notified (with Editor_dataChanged). */

/* Used by praat.cpp, praat_Basic.cpp, and praat_Sybil.cpp; defined in praat_picture.cpp.
*/
void praat_picture_open ();
void praat_picture_close ();
/* These two routines should bracket drawing commands. */
/* However, they usually do so RAII-wise by being packed into autoPraatPicture (see GRAPHICS_EACH). */

/* For main.cpp */

#define INCLUDE_LIBRARY(praat_xxx_init)  \
   { extern void praat_xxx_init (); praat_xxx_init (); }
#define INCLUDE_MANPAGES(manual_xxx_init)  \
   { extern void manual_xxx_init (ManPages me); manual_xxx_init (theCurrentPraatApplication -> manPages); }

/* For text-only applications that do not want to see that irritating Picture window. */
/* Works only if called before praat_init. */
/* The program will crash if you still try to use drawing routines. */
void praat_dontUsePictureWindow ();

/* Before praat_init: */
void praat_setLogo (double width_mm, double height_mm, void (*draw) (Graphics g));

/* Removing objects from the list. */
/* To remove the selected objects of class Klas from the list: */
/*
	for (i = praat.n; i >= 1; i --)   // Down!
		if (praat.list[i].selected && Thing_isa (praat.list[i].object, classKlas)
			praat_removeObject (i);
	praat_show ();   // Needed because the selection has changed.
*/
void praat_removeObject (int i);   // i = 1..praat.n
void praat_show ();   // forces an update of the dynamic menu
void praat_updateSelection ();
	/* If you require the correct selection immediately after calling praat_new. */

void praat_addCommandsToEditor (Editor me);

autoCollection praat_getSelectedObjects ();

struct autoPraatPicture {
	autoPraatPicture () { praat_picture_open (); }
	~autoPraatPicture () { praat_picture_close (); }
};

#if defined (_WIN32)
	#define main wingwmain
#endif

/* End of file praat.h */
#endif
