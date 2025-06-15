/* praat.h
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

#include "Editor.h"
#include "Manual.h"
#include "Preferences.h"

#ifdef __cplusplus
	extern "C" {
#endif

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
void praat_init (const char *title, unsigned int argc, char **argv);
void praat_run (void);
void praat_setStandAloneScriptText (wchar_t *text);   // call before praat_init if you want to create a stand-alone application without Objects and Picture window

void praat_addAction (void *class1, int n1, void *class2, int n2, void *class3, int n3,
	const wchar_t *title, const wchar_t *after, unsigned long flags,
	int (*callback) (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *closure));
/* 'class2', 'class3', 'title', 'after', and 'callback' may be NULL; 'title' is reference-copied. */
void praat_addAction1 (void *class1, int n1,
	const wchar_t *title, const wchar_t *after, unsigned long flags,
	int (*callback) (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *closure));
void praat_addAction2 (void *class1, int n1, void *class2, int n2,
	const wchar_t *title, const wchar_t *after, unsigned long flags,
	int (*callback) (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *closure));
void praat_addAction3 (void *class1, int n1, void *class2, int n2, void *class3, int n3,
	const wchar_t *title, const wchar_t *after, unsigned long flags,
	int (*callback) (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *closure));
void praat_addAction4 (void *class1, int n1, void *class2, int n2, void *class3, int n3, void *class4, int n4,
	const wchar_t *title, const wchar_t *after, unsigned long flags,
	int (*callback) (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *closure));
/*
	'title' is the name that will appear in the dynamic menu,
		and also the command that is used in command files.
	'callback' refers to a function prototyped like this:
		static int DO_Class_action (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, void *closure);
		this function should return 0 if the command failed,
		and 1 if the command was executed successfully;
		this function will be called by 'praat' when the user clicks a menu command,
		in which case 'sendingForm', 'sendingString' and 'closure' will be NULL;
		it is also called by scripts,
		in which case 'sendingString' is the argument list (after the dots).
		When called by Ui (after UiForm_create), 'sendingForm' is the UiForm, and 'closure'
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
#define praat_CTRL  0x00200000
int praat_removeAction (void *class1, void *class2, void *class3, const wchar_t *title);
	/* 'class2' and 'class3' may be NULL. */
	/* 'title' may be NULL; reference-copied. */

GuiObject praat_addMenuCommand (const wchar_t *window, const wchar_t *menu, const wchar_t *title,
	const wchar_t *after, unsigned long flags, int (*callback) (UiForm, const wchar_t *, Interpreter, const wchar_t *, bool, void *));
/* All strings are reference-copied; 'title', 'after', and 'callback' may be NULL. */

#define praat_MAXNUM_EDITORS 5
#include "Ui.h"
typedef struct {
	void *klas;   /* The class. */
	Any object;   /* The instance. */
	wchar_t *name;   /* The name of the object as it appears in the List. */
	long id;   /* The unique number of the object. */
	int selected;   /* Is the name of the object inverted in the list? */
	Any editors [praat_MAXNUM_EDITORS];   /* Are there editors open with this Object in it? */
	structMelderFile file;   /* Is this (very large) Object (partially) in a file? */
	int _beingCreated;
} structPraat_Object, *praat_Object;

#define praat_MAXNUM_OBJECTS 10000   /* Maximum number of objects in the list. */
typedef struct {   /* Readonly */
	MelderString batchName;   /* The name of the command file when called from batch. */
	int batch;   /* Was the program called from the command line? */
	#if gtk
		GMainContext *context;
	#elif motif
		XtAppContext context;   /* If you want to install an Xt WorkProc (rare). */
	#endif
	GuiObject topShell;   /* The application shell: parent of editors and standard dialogs. */
	ManPages manPages;
} structPraatApplication, *PraatApplication;
typedef struct {   /* Readonly */
	int n;	 /* The current number of objects in the list. */
	structPraat_Object list [1 + praat_MAXNUM_OBJECTS];   /* The list of objects: list [1..n]. */
	int totalSelection;   /* The total number of selected objects, <= n. */
	int numberOfSelected [1 + 1000];   /* For each (readable) class. */
	int totalBeingCreated;
	long uniqueId;
} structPraatObjects, *PraatObjects;
typedef struct {   /* Readonly */
	Graphics graphics;   /* The Graphics associated with the Picture window or HyperPage window or Demo window. */
	int font, fontSize, lineType;
	Graphics_Colour colour;
	double lineWidth, arrowSize, x1NDC, x2NDC, y1NDC, y2NDC;
} structPraatPicture, *PraatPicture;
extern structPraatApplication theForegroundPraatApplication;
extern PraatApplication theCurrentPraatApplication;
extern structPraatObjects theForegroundPraatObjects;
extern PraatObjects theCurrentPraatObjects;
extern structPraatPicture theForegroundPraatPicture;
extern PraatPicture theCurrentPraatPicture;
	/* The global objects containing the state of the application; only reachable from interface files. */

Any praat_onlyObject (void *klas);
	/* Returns a selected Data of class 'klas'. */
Any praat_onlyObject_generic (void *klas);
	/* Returns a selected Data of class 'klas' or a subclass. */
praat_Object praat_onlyScreenObject (void);
wchar_t *praat_name (int iobject);
void praat_write_do (Any dia, const wchar_t *extension);
bool praat_new1 (I, const wchar_t *s1);
bool praat_new2 (I, const wchar_t *s1, const wchar_t *s2);
bool praat_new3 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
bool praat_new4 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
bool praat_new5 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
bool praat_new6 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
bool praat_new7 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
bool praat_new8 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
bool praat_new9 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
bool praat_newWithFile1 (I, const wchar_t *s1, MelderFile file);
bool praat_newWithFile2 (I, const wchar_t *s1, const wchar_t *s2, MelderFile file);
bool praat_newWithFile3 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, MelderFile file);
bool praat_newWithFile4 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, MelderFile file);
bool praat_newWithFile5 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, MelderFile file);
void praat_name2 (wchar_t *name, void *klas1, void *klas2);

/* Macros for description of forms (dialog boxes).
	FORM prompts the user for arguments to DO_proc.
	Macros for FORM:
	FORM (proc, title, helpString)
		'proc' is the 'cb' argument of the corresponding VERB macro.
		'title' is the title of the standard dialog, shown in its title bar.
		'helpString' may be NULL.
	INTEGER (name, initialString)
	NATURAL (name, initialString)
	REAL (name, initialString)
	REAL_OR_UNDEFINED (name, initialString)
	POSITIVE (name, initialString)
	WORD (name, initialString)
	SENTENCE (name, initialString)
	COLOUR (name, initialString)
	CHANNEL (name, initialString)
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
	E.g. NOT return praat_new1 (...); BUT if (! praat_new1 (...)) return 0;
	This is because END updates the selection if an object was created.
 */

#define FORM(proc,name,helpTitle) \
	static int DO_##proc (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *buttonClosure) \
	{ static UiForm dia; if (dia == NULL) { Any radio = 0; (void) radio; \
	dia = UiForm_create (theCurrentPraatApplication -> topShell, name, DO_##proc, buttonClosure, invokingButtonTitle, helpTitle);
#define REAL(label,def)		UiForm_addReal (dia, label, def);
#define REAL_OR_UNDEFINED(label,def)  UiForm_addRealOrUndefined (dia, label, def);
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
#define RADIOBUTTONS_ENUM(labelProc,min,max) { for (int itext = min; itext <= max; itext ++) RADIOBUTTON (labelProc) }
#define OPTIONS_ENUM(labelProc,min,max) { for (int itext = min; itext <= max; itext ++) OPTION (labelProc) }
#define RADIO_ENUM(label,enum,def) \
	RADIO (label, enum##_##def - enum##_MIN + 1) \
	for (int ienum = enum##_MIN; ienum <= enum##_MAX; ienum ++) \
		OPTION (enum##_getText (ienum))
#define OPTIONMENU_ENUM(label,enum,def) \
	OPTIONMENU (label, enum##_##def - enum##_MIN + 1) \
	for (int ienum = enum##_MIN; ienum <= enum##_MAX; ienum ++) \
		OPTION (enum##_getText (ienum))
#define LIST(label,n,str,def)	UiForm_addList (dia, label, n, str, def);
#define FILE_IN(label)		UiForm_addFileIn (dia, label);
#define FILE_OUT(label,def)	UiForm_addFileOut (dia, label, def);
#define COLOUR(label,def)	UiForm_addColour (dia, label, def);
#define CHANNEL(label,def)	UiForm_addChannel (dia, label, def);
#define OK UiForm_finish (dia); } if (sendingForm == NULL && sendingString == NULL) {
#define SET_REAL(name,value)	UiForm_setReal (dia, name, value);
#define SET_INTEGER(name,value)	UiForm_setInteger (dia, name, value);
#define SET_STRING(name,value)	UiForm_setString (dia, name, value);
#define SET_ENUM(name,enum,value)  SET_STRING (name, enum##_getText (value))
#define DO  UiForm_do (dia, modified); } else if (sendingForm == NULL) { \
	if (! UiForm_parseString (dia, sendingString, interpreter)) return 0; } else { int IOBJECT = 0; (void) IOBJECT; {
#define DO_ALTERNATIVE(alternative)  UiForm_do (dia, modified); } else if (sendingForm == NULL) { \
	if (! UiForm_parseString (dia, sendingString, interpreter)) { wchar_t *parkedError = Melder_wcsdup_f (Melder_getError ()); Melder_clearError (); \
	int result = DO_##alternative (NULL, sendingString, interpreter, invokingButtonTitle, modified, buttonClosure); \
	if (result == 0 && parkedError) { Melder_clearError (); Melder_error1 (parkedError); } Melder_free (parkedError); return result; \
	} } else { int IOBJECT = 0; (void) IOBJECT; {
#define END  (void) 0; } } iferror return 0; praat_updateSelection (); return 1; }
#define DIRECT(proc)  static int DO_##proc (UiForm dummy1, const wchar_t *dummy2, Interpreter dummy3, const wchar_t *dummy4, bool dummy5, void *dummy6) { \
	(void) dummy1; (void) dummy2; (void) dummy3; (void) dummy4; (void) dummy5; (void) dummy6; { int IOBJECT = 0; (void) IOBJECT; {

#ifdef UNIX_newFileSelector
	#define FORM_READ(proc,title,help) \
		FORM (proc, title, help) \
			FILE_IN (L"infile") \
			OK \
		DO \
			MelderFile file = GET_FILE (L"infile");
	#define FORM_WRITE(proc,title,help,ext) \
		FORM (proc, title, help) \
			FILE_OUT (L"outfile", ext) \
			OK \
		DO \
			MelderFile file = GET_FILE (L"outfile");
#else
	#define FORM_READ(proc,title,help,allowMult) \
	static int DO_##proc (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *okClosure) { \
		static UiForm dia; (void) interpreter; (void) modified; (void) okClosure; \
		if (dia == NULL) dia = UiInfile_create (theCurrentPraatApplication -> topShell, title, DO_##proc, okClosure, invokingButtonTitle, help, allowMult); \
		if (sendingForm == NULL && sendingString == NULL) UiInfile_do (dia); else { MelderFile file; int IOBJECT = 0; structMelderFile file2 = { 0 }; (void) IOBJECT; \
		if (sendingString == NULL) file = UiFile_getFile (dia); \
		else { if (! Melder_relativePathToFile (sendingString, & file2)) return 0; file = & file2; } {
	#define FORM_WRITE(proc,title,help,ext) \
	static int DO_##proc (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *okClosure) { \
		static Any dia; (void) interpreter; (void) modified; (void) okClosure; \
		if (dia == NULL) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, title, DO_##proc, okClosure, invokingButtonTitle, help); \
		if (sendingForm == NULL && sendingString == NULL) praat_write_do (dia, ext); else { MelderFile file; int IOBJECT = 0; structMelderFile file2 = { 0 }; (void) IOBJECT; \
		if (sendingString == NULL) file = UiFile_getFile (dia); \
		else { if (! Melder_relativePathToFile (sendingString, & file2)) return 0; file = & file2; } {
#endif
  
/* Callbacks should return 1 if OK, and 0 if failure.
	Macros for DO_proc:
	GET_REAL (name)
	GET_INTEGER (name)
	GET_STRING (name)
	GET_COLOUR (name)
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
#define GET_ENUM(enum,name)  enum##_getValue (GET_STRING (name))
#define GET_COLOUR(name)  UiForm_getColour (dia, name)
#define GET_FILE(name)  UiForm_getFile (dia, name)
#define REQUIRE(c,t)  if (! (c)) return Melder_error1 (t);
#define NEW(proc)  if (! praat_new1 (proc, NULL)) return 0;

#define WHERE(condition)  for (IOBJECT = 1; IOBJECT <= theCurrentPraatObjects -> n; IOBJECT ++) if (condition)
#define WHERE_DOWN(condition)  for (IOBJECT = theCurrentPraatObjects -> n; IOBJECT > 0; IOBJECT --) if (condition)
#define SELECTED  (theCurrentPraatObjects -> list [IOBJECT]. selected)
#define CLASS  (theCurrentPraatObjects -> list [IOBJECT]. klas)
#define OBJECT  (theCurrentPraatObjects -> list [IOBJECT]. object)
#define GRAPHICS  theCurrentPraatPicture -> graphics
#define FULL_NAME  (theCurrentPraatObjects -> list [IOBJECT]. name)
#define ID  (theCurrentPraatObjects -> list [IOBJECT]. id)
#define ID_AND_FULL_NAME  Melder_wcscat3 (Melder_integer (ID), L". ", FULL_NAME)
#define NAME  praat_name (IOBJECT)
#define EVERY(proc)  WHERE (SELECTED) proc;
#define EVERY_CHECK(proc)  EVERY (if (! proc) return 0)
#define EVERY_TO(proc)  EVERY_CHECK (praat_new1 (proc, NAME))
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
		if (praat.batch) return Melder_error1 (L"Cannot edit a Spectrogram from batch.");
		else WHERE (SELECTED)
			if (! praat_installEditor
				(SpectrogramEditor_create (praat.topShell, ID_AND_FULL_NAME, OBJECT), IOBJECT)) return 0;
	END
*/
int praat_installEditor2 (Any editor, int iobject1, int iobject2);
int praat_installEditor3 (Any editor, int iobject1, int iobject2, int iobject3);
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
   { extern void manual_xxx_init (ManPages me); manual_xxx_init (theCurrentPraatApplication -> manPages); }

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

#ifdef __cplusplus
	}
#endif

/* End of file praat.h */
