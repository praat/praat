/* praat.h
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
 * pb 2006/12/26
 */

#include "Editor.h"
#include "Manual.h"
#include "Preferences.h"

/* The explanations in this header file assume
	that you put your extra commands in praat_Sybil.c
	and the main() function in main_Sybil.c,
	but these files may have different names if you are not Sybil.
	Linking with the rest of Praat will create an executable
	that has all the functionality of the basic Praat,
	plus everything that you made available in praat_Sybil.c.
*/

/* Program example:
// File main_Sybil.c: //
int main (unsigned int argc, char **argv)
{
	praat_init ("Praat_Sybil", argc, argv);   // Obligatory.
	INCLUDE_LIBRARY (praat_Fon_init)   // Optional: inherit phonetic stuff.
	INCLUDE_LIBRARY (praat_Sybil_init)   // Optional: add Sybil's things.
	INCLUDE_MANPAGES (manual_Sybil)
	praat_run ();   // Obligatory.
}
// File praat_Sybil.c: //
void praat_Sybil (void)
{
	...
	...
}
The dots consist of, in any order:
	Thing_recognizeClassesByName (...);
	Data_recognizeFileType (...);
	praat_addMenuCommand (...);
	praat_addAction (...);
All of these statements are optional and may occur more than once.
To make any class string-readable, use Thing_recognizeClassesByName ().
String-readable classes are known by Thing_newFromClassName () and can therefore
be read by Data_readFromTextFile () and Data_readFromBinaryFile ().
*/
extern void praat_init (const char *title, unsigned int argc, char **argv);
extern void praat_run (void);

void praat_addAction (void *class1, int n1, void *class2, int n2, void *class3, int n3,
	const char *title, const char *after, unsigned long flags, int (*callback) (Any sender, void *closure));
/* 'class2', 'class3', 'title', 'after', and 'callback' may be NULL; 'title' is reference-copied. */
void praat_addAction1 (void *class1, int n1,
	const char *title, const char *after, unsigned long flags, int (*callback) (Any sender, void *closure));
void praat_addAction2 (void *class1, int n1, void *class2, int n2,
	const char *title, const char *after, unsigned long flags, int (*callback) (Any sender, void *closure));
void praat_addAction3 (void *class1, int n1, void *class2, int n2, void *class3, int n3,
	const char *title, const char *after, unsigned long flags, int (*callback) (Any sender, void *closure));
void praat_addAction4 (void *class1, int n1, void *class2, int n2, void *class3, int n3, void *class4, int n4,
	const char *title, const char *after, unsigned long flags, int (*callback) (Any sender, void *closure));
/*
	'title' is the name that will appear in the dynamic menu,
		and also the command that is used in command files.
		If the command presents a dialog, include "...";
		note that 'Run script...' expects the three dots.
	'callback' refers to a function prototyped like this:
		static int DO_Class_action (Any sender, void *closure);
		this function should return 0 if the command failed,
		and 1 if the command was executed successfully;
		this function will be called by 'praat' when the user clicks a menu command,
		in which case 'sender' and 'closure' will be NULL;
		it is also called by 'Run script...',
		in which case 'sender' is the argument string  read from the file (after the dots).
		When called by Ui (after UiForm_create), 'sender' is the UiForm, and 'closure'
		is the closure you passed to UiForm_create (which may be an editor).

	The availability of the dynamic commands depends on
	the current selection: e.g., if the user has selected three objects
	of type Matrix and nothing else, the commands registered with
	praat_addAction (classMatrix, n, 0, 0, 0, 0, "xxx", "xxxx", x, DO_xxx) are visible,
	and those with n=0 or n=3 are executable (the buttons are sensitive)
	and, if chosen, performed on each of these three objects;
	if the user has selected one object of type Artword and one of type
	Speaker, the commands from praat_addAction (classArtword, 1, classSpeaker, 1, ...) are executable.
	You may want to restrict the availability to one object for commands that write objects to file,
	commands that present information in a dialog, or the Edit command.
*/
#define praat_INSENSITIVE  0x00000100
#define praat_CHECKABLE  0x00000200
#define praat_CHECKED  0x00000400
#define praat_HIDDEN  0x00000800
#define praat_UNHIDABLE  0x00001000
#define praat_DEPTH_1  0x00010000
#define praat_DEPTH_2  0x00020000
#define praat_DEPTH_3  0x00030000
#define praat_DEPTH_4  0x00040000
#define praat_DEPTH_5  0x00050000
#define praat_DEPTH_6  0x00060000
#define praat_DEPTH_7  0x00070000
#define praat_CTRL  0x00200000
int praat_removeAction (void *class1, void *class2, void *class3, const char *title);
	/* 'class2' and 'class3' may be NULL. */
	/* 'title' may be NULL; reference-copied. */

Widget praat_addMenuCommand (const char *window, const char *menu, const char *title,
	const char *after, unsigned long flags, int (*callback) (Any, void *));
/* All strings are reference-copied; 'title', 'after', and 'callback' may be NULL. */

#define praat_MAXNUM_EDITORS 5
#include "Ui.h"
typedef struct {
	void *klas;   /* The class. */
	Any object;   /* The instance. */
	char *name;   /* The name of the object as it appears in the List. */
	long id;   /* The unique number of the object. */
	int selected;   /* Is the name of the object inverted in the list? */
	Any editors [praat_MAXNUM_EDITORS];   /* Are there editors open with this Object in it? */
	structMelderFile file;   /* Is this (very large) Object (partially) in a file? */
	int _beingCreated;
} structPraat_Object, *praat_Object;

#define praat_MAXNUM_OBJECTS 1000   /* Maximum number of objects in the list. */
typedef struct {   /* Readonly */
	int n;	 /* The current number of objects in the list. */
	structPraat_Object list [1 + praat_MAXNUM_OBJECTS];   /* The list of objects: list [1..n]. */
	char *batchName;   /* The name of the command file when called from batch. */
	int batch;   /* Was the program called from the command line? */
	int totalSelection;   /* The total number of selected objects, <= n. */
	int totalBeingCreated;
	XtAppContext context;   /* If you want to install an Xt WorkProc (rare). */
	Widget topShell;   /* The application shell: parent of editors and standard dialogs. */
	Graphics graphics;   /* The Graphics associated with the Picture window. */
	ManPages manPages;
} structPraat, *Praat;
extern structPraat theForegroundPraat;
extern Praat theCurrentPraat;
	/* The global object containing the state of the application; only reachable from interface files. */

Any praat_onlyObject (void *klas);
	/* Returns a selected Data of class 'klas'. */
Any praat_onlyObject_generic (void *klas);
	/* Returns a selected Data of class 'klas' or a subclass. */
praat_Object praat_onlyScreenObject (void);
char *praat_name (int iobject);
void praat_write_do (Any dia, const char *extension);
int praat_new (Any data, const char *myName, ...);
void praat_name2 (char *name, void *klas1, void *klas2);

/* Macros for description of forms (dialog boxes).
	FORM prompts the user for arguments to DO_proc.
	Macros for FORM:
	FORM (proc, title, helpString)
		'proc' is the 'cb' argument of the corresponding VERB macro.
		'title' is the title of the standard dialog, shown in its title bar.
		'helpString' may be NULL.
	INTEGER (name, initialString)
	NATURAL (name, initialString)
	DOUBLE (name, initialString)
	POSITIVE (name, initialString)
	WORD (name, initialString)
	SENTENCE (name, initialString)
		the name is also the string displayed in the label.
	BOOLEAN (name, initialValue)
		the name is also the title of the check box;
		the initial value is 0 (off) or 1 (on).
	LABEL (name, initialLabelString)
		'name' is not shown but can still be used by the SETs
	TEXTFIELD (name, initialString)
		'name' is not shown but can still be used by the SETs
	RADIO (name, initialValue)
		the name is also the string displayed in the label;
		this should be followed by two or more RADIOBUTTONs.
		The initial value is between 1 and the number of radio buttons.
	RADIOBUTTON (name)
		the name is also the title of the button.
	OPTIONMENU (name, initialValue)
		the name is also the string displayed in the label;
		this should be followed by two or more OPTIONs.
		The initial value is between 1 and the number of options.
	OPTION (name)
		the name is also the title of the button.
	ENUM (name, enumtype, initialValue)
		the name is also the string displayed in the label.
		The initial value is between 0 and enumlength(enumtype).
	OK
		this statement is obligatory.
	SET_XXXXXX (name, value)
		sets the value of REAL..LIST.
	DO
	The order of the macros should be:
		FORM
		one or more from REAL..LIST
		OK
		zero or more SETs
		DO
		...
		END
	See Ui.h for more information.
	Between DO and END, you should return 0 if something is wrong;
	if everything is alright, you should trickle down to END; never return 1!
	E.g. NOT return praat_new (...); BUT if (! praat_new (...)) return 0;
	This is because END updates the selection if an object was created.
 */

#define FORM(proc,name,helpTitle) \
	static int DO_##proc (Any sender, void *modified) \
	{ static Any dia; if (dia == NULL) { Any radio = 0; (void) radio; \
	dia = UiForm_create (theCurrentPraat -> topShell, name, DO_##proc, NULL, helpTitle);
#define REAL(label,def)		UiForm_addReal (dia, label, def);
#define POSITIVE(label,def)	UiForm_addPositive (dia, label, def);
#define INTEGER(label,def)	UiForm_addInteger (dia, label, def);
#define NATURAL(label,def)	UiForm_addNatural (dia, label, def);
#define WORD(label,def)		UiForm_addWord (dia, label, def);
#define SENTENCE(label,def)	UiForm_addSentence (dia, label, def);
#define BOOLEAN(label,def)	UiForm_addBoolean (dia, label, def);
#define LABEL(name,label)	UiForm_addLabel (dia, name, label);
#define TEXTFIELD(name,def)	UiForm_addText (dia, name, def);
#define RADIO(label,def)	radio = UiForm_addRadio (dia, label, def);
#define RADIOBUTTON(label)	UiRadio_addButton (radio, label);
#define OPTIONMENU(label,def)	radio = UiForm_addOptionMenu (dia, label, def);
#define OPTION(label)	UiOptionMenu_addButton (radio, label);
#define ENUM(label,type,def)	UiForm_addEnum (dia, label, & enum_##type, def);
#define RADIOBUTTONS_ENUM(labelProc,min,max) { int itext; for (itext = min; itext <= max; itext ++) RADIOBUTTON (labelProc) }
#define OPTIONS_ENUM(labelProc,min,max) { int itext; for (itext = min; itext <= max; itext ++) OPTION (labelProc) }
#define LIST(label,n,str,def)	UiForm_addList (dia, label, n, str, def);
#define FILE_IN(label)		UiForm_addFileIn (dia, label);
#define FILE_OUT(label,def)	UiForm_addFileOut (dia, label, def);
#define COLOUR(label,def)	UiForm_addColour (dia, label, def);
#define OK UiForm_finish (dia); } if (sender == NULL) {
#define SET_REAL(name,value)	UiForm_setReal (dia, name, value);
#define SET_INTEGER(name,value)	UiForm_setInteger (dia, name, value);
#define SET_STRING(name,value)	UiForm_setString (dia, name, value);
#define DO  UiForm_do (dia, (int) modified); } else if (sender != dia) { \
	if (! UiForm_parseString (dia, (char *) sender)) return 0; } else { int IOBJECT = 0; (void) IOBJECT; {
#define DO_ALTERNATIVE(alternative)  UiForm_do (dia, (int) modified); } else if (sender != dia) { \
	if (! UiForm_parseString (dia, (char *) sender)) { Melder_clearError (); \
	return DO_##alternative (sender, modified); } } else { int IOBJECT = 0; (void) IOBJECT; {
#define END  } } praat_updateSelection (); return 1; }
#define DIRECT(proc)  static int DO_##proc (Any dummy1, void *dummy2) { \
	(void) dummy1; (void) dummy2; { int IOBJECT = 0; (void) IOBJECT; {

#ifdef UNIX_newFileSelector
	#define FORM_READ(proc,title,help) \
		FORM (proc, title, help) \
			FILE_IN ("infile") \
			OK \
		DO \
			MelderFile file = GET_FILE ("infile");
	#define FORM_WRITE(proc,title,help,ext) \
		FORM (proc, title, help) \
			FILE_OUT ("outfile", ext) \
			OK \
		DO \
			MelderFile file = GET_FILE ("outfile");
#else
	#define FORM_READ(proc,title,help) \
	static int DO_##proc (Any sender, void *dummy) { \
		static Any dia; (void) dummy; \
		if (! dia) dia = UiInfile_create (theCurrentPraat -> topShell, title, DO_##proc, NULL, help); \
		if (! sender) UiInfile_do (dia); else { MelderFile file; int IOBJECT = 0; structMelderFile file2; (void) IOBJECT; \
		if (sender == dia) file = UiFile_getFile (sender); \
		else { if (! Melder_relativePathToFile (sender, & file2)) return 0; file = & file2; } {
	#define FORM_WRITE(proc,title,help,ext) \
	static int DO_##proc (Any sender, void *dummy) { \
		static Any dia; (void) dummy; \
		if (! dia) dia = UiOutfile_create (theCurrentPraat -> topShell, title, DO_##proc, NULL, help); \
		if (! sender) praat_write_do (dia, ext); else { MelderFile file; int IOBJECT = 0; structMelderFile file2; (void) IOBJECT; \
		if (sender == dia) file = UiFile_getFile (sender); \
		else { if (! Melder_relativePathToFile (sender, & file2)) return 0; file = & file2; } {
#endif
  
/* Callbacks should return 1 if OK, and 0 if failure.
	Macros for DO_proc:
	GET_REAL (name)
	GET_INTEGER (name)
	GET_STRING (name)
	GET_FILE (name)
	REQUIRE (condition, errorMessage)
		returns 0 if condition false.
	NEW (functionCall)
		returns 1 if function returned a new object;
		returns 0 if function returned a NULL object.
*/
#define GET_REAL(name)  UiForm_getReal (dia, name)
#define GET_INTEGER(name)  UiForm_getInteger (dia, name)
#define GET_STRING(name)  UiForm_getString (dia, name)
#define GET_FILE(name)  UiForm_getFile (dia, name)
#define REQUIRE(c,t)  if (! (c)) return Melder_error ("%s", t);
#define NEW(proc)  if (! praat_new (proc, NULL)) return 0;

#define WHERE(condition)  for (IOBJECT = 1; IOBJECT <= theCurrentPraat -> n; IOBJECT ++) if (condition)
#define WHERE_DOWN(condition)  for (IOBJECT = theCurrentPraat -> n; IOBJECT > 0; IOBJECT --) if (condition)
#define SELECTED  (theCurrentPraat -> list [IOBJECT]. selected)
#define CLASS  (theCurrentPraat -> list [IOBJECT]. klas)
#define OBJECT  (theCurrentPraat -> list [IOBJECT]. object)
#define GRAPHICS  theCurrentPraat -> graphics
#define FULL_NAME  (theCurrentPraat -> list [IOBJECT]. name)
#define ID  (theCurrentPraat -> list [IOBJECT]. id)
#define NAME  praat_name (IOBJECT)
#define FILENAME  (theCurrentPraat -> list [IOBJECT]. file)
#define EVERY(proc)  WHERE (SELECTED) proc;
#define EVERY_CHECK(proc)  EVERY (if (! proc) return 0)
#define EVERY_TO(proc)  EVERY_CHECK (praat_new (proc, NAME))
#define ONLY(klas)  praat_onlyObject (klas)
#define ONLY_GENERIC(klas)  praat_onlyObject_generic (klas)
#define ONLY_OBJECT  (praat_onlyScreenObject () -> object)
#define EVERY_DRAW(proc) \
	praat_picture_open (); WHERE (SELECTED) proc; praat_picture_close (); return 1;

/* Used by praat_Sybil.c, if you put an Editor on the screen: */
int praat_installEditor (Any editor, int iobject);
/* This routine adds a reference to a new editor (unless it is NULL) to the screen object
   which is in the list at position 'iobject'.
   It sets the destroyCallback and dataChangedCallback as appropriate for Praat:
   the destroyCallback will set the now dangling reference to NULL,
   so that a subsequent click on the "Edit" button will create a new editor;
   the dataChangedCallback will notify an open DataEditor with the same data,
   after that data will have changed.
      Return value: normally 1, but 0 if 'editor' is NULL.
   A typical calling sequence is:
	DIRECT (Spectrogram_edit)
		if (praat.batch) return Melder_error ("Cannot edit a Spectrogram from batch.");
		else WHERE (SELECTED)
			if (! praat_installEditor
				(SpectrogramEditor_create (praat.topShell, FULL_NAME, OBJECT), IOBJECT)) return 0;
	END
*/
int praat_installEditor2 (Any editor, int iobject1, int iobject2);
int praat_installEditor3 (Any editor, int iobject1, int iobject2, int iobject3);
/* The same for two objects in one editor. Example:
DIRECT (SiftLdbDisambiguate)
	if (praat.batch) {
		return Melder_error ("Cannot edit from batch.");
	} else {
		int ildb, idict;
		WHERE (SELECTED) {
			if (CLASS == classLdb) ildb = IOBJECT;
			else if (CLASS == classDictSenses) idict = IOBJECT;
		}
		{
			char title [200];
			Ldb ldb = praat.list [ildb]. object;
			DictSenses senses = praat.list [idict]. object;
			sprintf (title, "Disambiguating genus word \"%s\" in SIFT Ldb \"%s\"", senses -> name, ldb -> name);
			if (! praat_installEditor2 (SiftLdbDisambiguator_create (praat.topShell, title, ldb, senses),
				ildb, idict)) return 0;
		}
	}
END
*/
int praat_installEditorN (Any editor, Ordered objects);

void praat_dataChanged (Any object);
/* Call this after changing a screen object. */
/* Associated editors and data editors will be notified (with Editor_dataChanged). */

void praat_clipboardChanged (void *closure, Any clipboard);
/* Make this the callback for all changed clipboards, with 'closure' == NULL: */
/* e.g. Sound_setClipboardCallback (praat_clipboardChanged, NULL); */
/* This routine sends an Editor_clipboardChanged message to all editors. */

/* Used by praat.c, praat_Basic.c, and praat_Sybil.c; defined in praat_picture.c.
*/
void praat_picture_open (void);
void praat_picture_close (void);
/* These two routines should bracket drawing commands. */
/* See also the EVERY_DRAW macro. */

/* For main.c */

#define INCLUDE_LIBRARY(praat_xxx_init) \
   { extern void praat_xxx_init (void); praat_xxx_init (); }
#define INCLUDE_MANPAGES(manual_xxx_init) \
   { extern void manual_xxx_init (ManPages me); manual_xxx_init (theCurrentPraat -> manPages); }

/* For text-only applications that do not want to see that irritating Picture window. */
/* Works only if called before praat_init. */
/* The program will crash if you still try to use drawing routines. */
void praat_dontUsePictureWindow (void);

/* Before praat_init: */
void praat_setLogo (double width_mm, double height_mm, void (*draw) (Graphics g));

/* Removing objects from the list. */
/* To remove the selected objects of class Klas from the list: */
/*
	for (i = praat.n; i >= 1; i --)   // Down!
		if (praat.list[i].selected && Thing_member (praat.list[i].object, classKlas)
			praat_removeObject (i);
	praat_show ();   // Needed because the selection has changed.
*/
void praat_removeObject (int i);   /* i = 1..praat.n */
void praat_show (void);   /* Forces an update of the dynamic menu. */
void praat_updateSelection (void);
	/* If you require the correct selection immediately after calling praat_new. */

void praat_addCommandsToEditor (Editor me);

/* End of file praat.h */
