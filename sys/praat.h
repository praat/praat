#ifndef _praat_h_
#define _praat_h_
/* praat.h
 *
 * Copyright (C) 1992-2023 Paul Boersma
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

#include "ScriptEditor.h"
#include "Manual.h"
#include "Preferences.h"

/*
	The explanations in this header file assume
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
void praat_init (conststring32 title, int argc, char **argv);
void praat_run ();
void praat_setStandAloneScriptText (conststring32 text);   // call before praat_init if you want to create a stand-alone application without Objects and Picture window
extern "C" void praatlib_init ();   // for use in an application that uses Praatlib

#define praat_addAction1(c1,n1,t,a,f,c)  praat_addAction1_ (c1, n1, t, a, f, c, U"" #c)
#define praat_addAction2(c1,n1,c2,n2,t,a,f,c)  praat_addAction2_ (c1, n1, c2, n2, t, a, f, c, U"" #c)
#define praat_addAction3(c1,n1,c2,n2,c3,n3,t,a,f,c)  praat_addAction3_ (c1, n1, c2, n2, c3, n3, t, a, f, c, U"" #c)
#define praat_addAction4(c1,n1,c2,n2,c3,n3,c4,n4,t,a,f,c)  praat_addAction4_ (c1, n1, c2, n2, c3, n3, c4, n4, t, a, f, c, U"" #c)

void praat_addAction1_ (ClassInfo class1, integer n1,
	conststring32 title, conststring32 after, uint32 flags, UiCallback callback, conststring32 nameOfCallback);
void praat_addAction2_ (ClassInfo class1, integer n1, ClassInfo class2, integer n2,
	conststring32 title, conststring32 after, uint32 flags, UiCallback callback, conststring32 nameOfCallback);
void praat_addAction3_ (ClassInfo class1, integer n1, ClassInfo class2, integer n2, ClassInfo class3, integer n3,
	conststring32 title, conststring32 after, uint32 flags, UiCallback callback, conststring32 nameOfCallback);
void praat_addAction4_ (ClassInfo class1, integer n1, ClassInfo class2, integer n2, ClassInfo class3, integer n3, ClassInfo class4, integer n4,
	conststring32 title, conststring32 after, uint32 flags, UiCallback callback, conststring32 nameOfCallback);
/*
	'title' is the name that will appear in the dynamic menu,
		and also the command that is used in command files;
		this title is deep-copied.
	'callback' refers to a function prototyped like this:
		static void DO_Class_action (UiForm sendingForm, int narg, Stackel args, conststring32 sendingString,
				Interpreter interpreter, conststring32 invokingButtonTitle, bool modified, void *closure);
		this function should throw an exception if the command failed;
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

void praat_removeAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, conststring32 title);
	/* 'class2' and 'class3' may be null. */
	/* 'title' may be null; reference-copied. */

#define praat_addMenuCommand(w,m,t,a,f,c)  praat_addMenuCommand_ (w, m, t, a, f, c, U"" #c)
GuiMenuItem praat_addMenuCommand_ (conststring32 window, conststring32 menu, conststring32 title /* cattable */,
	conststring32 after, uint32 flags, UiCallback callback, conststring32 nameOfCallback);
/* All strings are reference-copied; 'title', 'after', and 'callback' may be null. */

#define praat_MAXNUM_EDITORS 5
#include "Ui.h"
typedef struct structPraat_Object {
	ClassInfo klas;   // the class
	Daata object;   // the instance
	autostring32 name;   // the name of the object as it appears in the List
	structMelderFile file;   // is this Object associated with a file?
	integer id;   // the unique number of the object
	bool isSelected;   // is the name of the object inverted in the list?
	Editor editors [praat_MAXNUM_EDITORS];   // are there editors open with this Object in it?
	bool isBeingCreated;
} *praat_Object;

#define praat_MAXNUM_OBJECTS 10000   /* Maximum number of objects in the list. */
typedef struct structPraatApplication {   // read-only (for interface files)
	MelderString batchName;   // the name of the command file when called from batch
	bool batch;   // was the program called from the command line?
	GuiWindow topShell;   // the application shell: parent of standard dialogs
	ManPages manPages;
} *PraatApplication;
typedef struct structPraatObjects {   // read-only (for interface files)
	integer n;   // the current number of objects in the list
	structPraat_Object list [1 + praat_MAXNUM_OBJECTS];   // the list of objects: list [1..n]
	integer totalSelection;   // the total number of selected objects, <= n
	static constexpr integer MAXNUM_DATA_CLASSES = 1000;
	integer numberOfSelected [1 + MAXNUM_DATA_CLASSES];   // for each (readable) class
	integer totalBeingCreated;
	integer uniqueId;
	void reset () {
		for (integer iobject = our n; iobject >= 1; iobject --) {
			our list [iobject]. name. reset();
			forget (our list [iobject]. object);
		}
		#if 0
		memset (this, 0, sizeof (structPraatObjects));
		#else
		our n = 0;
		our totalSelection = 0;
		for (integer i = 1; i <= MAXNUM_DATA_CLASSES; i ++)
			our numberOfSelected [i] = 0;
		#endif
	}
} *PraatObjects;
typedef struct structPraatPicture {   // read-only (for interface files)
	Graphics graphics;   // the Graphics associated with the Picture window
	kGraphics_font font;
	int lineType;
	double fontSize;
	MelderColour colour;
	double lineWidth, arrowSize, speckleSize, x1NDC, x2NDC, y1NDC, y2NDC;
} *PraatPicture;
/*
	The following six cannot be "inline",
	because that would cost 42 MB (per architecture)
	on the Mac or on Windows (though nothing on Linux),
	i.e. a full copy of all three structures.
	(ppgb 2021-04-15)
*/
extern structPraatApplication theForegroundPraatApplication;
extern PraatApplication theCurrentPraatApplication;
extern structPraatObjects theForegroundPraatObjects;
extern PraatObjects theCurrentPraatObjects;
extern structPraatPicture theForegroundPraatPicture;
extern PraatPicture theCurrentPraatPicture;
	/* The global objects containing the state of the application; only reachable from interface files. */
bool praat_commandsWithExternalSideEffectsAreAllowed ();

char32 *praat_name (integer iobject);
void praat_write_do (UiForm dia, conststring32 extension);
void praat_new (autoDaata me);
void praat_new (autoDaata me, const MelderArg& arg);
void praat_new (autoDaata me, const MelderArg& arg1, const MelderArg& arg2,
	const MelderArg& arg3 = U"", const MelderArg& arg4 = U"", const MelderArg& arg5 = U"");
void praat_newWithFile (autoDaata me, MelderFile file, conststring32 name);
void praat_name2 (char32 *name, ClassInfo klas1, ClassInfo klas2);

#define iam_LOOP(klas)  klas me = static_cast<klas> (OBJECT)
#define WHERE(condition)  for (IOBJECT = 1; IOBJECT <= theCurrentPraatObjects -> n; IOBJECT ++) if (condition)
#define WHERE_DOWN(condition)  for (IOBJECT = theCurrentPraatObjects -> n; IOBJECT > 0; IOBJECT --) if (condition)
#define SELECTED  (theCurrentPraatObjects -> list [IOBJECT]. isSelected)
#define LOOP  for (IOBJECT = 1; IOBJECT <= theCurrentPraatObjects -> n; IOBJECT ++) if (SELECTED)
#define CLASS  (theCurrentPraatObjects -> list [IOBJECT]. klas)
#define OBJECT  (theCurrentPraatObjects -> list [IOBJECT]. object)
#define GRAPHICS  theCurrentPraatPicture -> graphics
#define FULL_NAME  (theCurrentPraatObjects -> list [IOBJECT]. name.get())
#define ID  (theCurrentPraatObjects -> list [IOBJECT]. id)
#define ID_AND_FULL_NAME  Melder_cat (ID, U". ", FULL_NAME)
#define NAME  praat_name (IOBJECT)

/* Used by praat_Sybil.cpp, if you put an Editor on the screen: */
void praat_installEditor (Editor editor, integer iobject);
/* This routine adds a reference to a new editor (unless it is null) to the screen object
   which is in the list at position 'iobject'.
   It sets the destroyCallback and dataChangedCallback as appropriate for Praat:
   the destroyCallback will set the now dangling reference to nullptr,
   so that a subsequent click on the "View & Edit" button will create a new editor;
   the dataChangedCallback will notify an open DataEditor with the same data,
   after that data will have changed.
*/
void praat_installEditor2 (Editor editor, integer iobject1, integer iobject2);
void praat_installEditor3 (Editor editor, integer iobject1, integer iobject2, integer iobject3);
void praat_installEditorN (Editor editor, DaataList objects);

void praat_dataChanged (Daata object);
/* Call this after changing a screen object. */
/* Associated editors and data editors will be notified (with Editor_dataChanged). */

/* Used by praat.cpp, praat_Basic.cpp, and praat_Sybil.cpp; defined in praat_picture.cpp.
*/
void praat_picture_open ();
void praat_picture_close ();
/* These two routines should bracket drawing commands. */
/* However, they usually do so RAII-wise by being packed into autoPraatPictureOpen (see GRAPHICS_EACH). */

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
void praat_removeObject (integer i);   // i = 1..praat.n
void praat_show ();   // forces an update of the dynamic menu
void praat_updateSelection ();
	/* If you require the correct selection immediately after calling praat_new. */

void praat_addCommandsToEditor (Editor me);

autoCollection praat_getSelectedObjects ();

struct autoPraatPictureOpen {
	autoPraatPictureOpen () { praat_picture_open (); }
	~autoPraatPictureOpen () { praat_picture_close (); }
};

#if defined (_WIN32)
	#define main wingwmain
#endif

/* End of file praat.h */
#endif
