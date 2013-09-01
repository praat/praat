/* praat_objectMenus.cpp
 *
 * Copyright (C) 1992-2012,2013 Paul Boersma
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

#include <ctype.h>
#include "praatP.h"
#include "praat_script.h"
#include "ScriptEditor.h"
#include "ButtonEditor.h"
#include "DataEditor.h"
#include "site.h"

#undef iam
#define iam iam_LOOP

#define EDITOR  theCurrentPraatObjects -> list [IOBJECT]. editors

/********** Callbacks of the fixed buttons. **********/

DIRECT (Remove)
	WHERE_DOWN (SELECTED)
		praat_removeObject (IOBJECT);
	praat_show ();
END

FORM (Rename, L"Rename object", L"Rename...")
	LABEL (L"rename object", L"New name:")
	TEXTFIELD (L"newName", L"")
	OK
{ int IOBJECT; WHERE (SELECTED) SET_STRING (L"newName", NAME) }
DO
	wchar_t *string = GET_STRING (L"newName");
	if (theCurrentPraatObjects -> totalSelection == 0)
		Melder_throw ("Selection changed!\nNo object selected. Cannot rename.");
	if (theCurrentPraatObjects -> totalSelection > 1)
		Melder_throw (L"Selection changed!\nCannot rename more than one object at a time.");
	WHERE (SELECTED) break;
	praat_cleanUpName (string);   /* This is allowed because "string" is local and dispensible. */
	static MelderString fullName = { 0 };
	MelderString_empty (& fullName);
	MelderString_append (& fullName, Thing_className ((Thing) OBJECT), L" ", string);
	if (! wcsequ (fullName.string, FULL_NAME)) {
		Melder_free (FULL_NAME), FULL_NAME = Melder_wcsdup_f (fullName.string);
		MelderString listName = { 0 };
		MelderString_append (& listName, Melder_integer (ID), L". ", fullName.string);
		praat_list_renameAndSelect (IOBJECT, listName.string);
		MelderString_free (& listName);
		for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (EDITOR [ieditor]) Thing_setName ((Thing) EDITOR [ieditor], fullName.string);
		Thing_setName ((Thing) OBJECT, string);
	}
END

FORM (Copy, L"Copy object", L"Copy...")
	LABEL (L"copy object", L"Name of new object:")
	TEXTFIELD (L"newName", L"")
	OK
{ int IOBJECT; WHERE (SELECTED) SET_STRING (L"newName", NAME) }
DO
	if (theCurrentPraatObjects -> totalSelection == 0)
		Melder_throw ("Selection changed!\nNo object selected. Cannot copy.");
	if (theCurrentPraatObjects -> totalSelection > 1)
		Melder_throw ("Selection changed!\nCannot copy more than one object at a time.");
	WHERE (SELECTED) {
		wchar_t *name = GET_STRING (L"newName");
		praat_new (Data_copy ((Data) OBJECT), name);
	}
END

DIRECT (Info)
	if (theCurrentPraatObjects -> totalSelection == 0)
		Melder_throw ("Selection changed!\nNo object selected. Cannot query.");
	if (theCurrentPraatObjects -> totalSelection > 1)
		Melder_throw ("Selection changed!\nCannot query more than one object at a time.");
	WHERE (SELECTED) Thing_infoWithId ((Thing) OBJECT, ID);
END

DIRECT (Inspect)
	if (theCurrentPraatObjects -> totalSelection == 0)
		Melder_throw ("Selection changed!\nNo object selected. Cannot inspect.");
	if (theCurrentPraatApplication -> batch) {
		Melder_throw ("Cannot inspect data from batch.");
	} else {
		WHERE (SELECTED) {
			praat_installEditor (DataEditor_create (ID_AND_FULL_NAME, OBJECT), IOBJECT);
		}
	}
END

/********** The fixed menus. **********/

static GuiMenu praatMenu, editMenu, newMenu, readMenu, goodiesMenu, preferencesMenu, technicalMenu, applicationHelpMenu, helpMenu;

GuiMenu praat_objects_resolveMenu (const wchar_t *menu) {
	return
		wcsequ (menu, L"Praat") || wcsequ (menu, L"Control") ? praatMenu :
		#if cocoa
			wcsequ (menu, L"Edit") ? editMenu :
		#endif
		wcsequ (menu, L"New") || wcsequ (menu, L"Create") ? newMenu :
		wcsequ (menu, L"Open") || wcsequ (menu, L"Read") ? readMenu :
		wcsequ (menu, L"Help") ? helpMenu :
		wcsequ (menu, L"Goodies") ? goodiesMenu :
		wcsequ (menu, L"Preferences") ? preferencesMenu :
		wcsequ (menu, L"Technical") ? technicalMenu :
		#ifdef macintosh
			wcsequ (menu, L"ApplicationHelp") ? applicationHelpMenu :
		#else
			wcsequ (menu, L"ApplicationHelp") ? helpMenu :
		#endif
		newMenu;   /* Default. */
}

/********** Callbacks of the Praat menu. **********/

DIRECT (About)
	praat_showLogo (FALSE);
END

DIRECT (praat_newScript)
	autoScriptEditor editor = ScriptEditor_createFromText (NULL, NULL);
	editor.transfer();   // the user becomes the owner
END

DIRECT (praat_openScript)
	autoScriptEditor editor = ScriptEditor_createFromText (NULL, NULL);
	TextEditor_showOpen (editor.peek());
	editor.transfer();   // the user becomes the owner
END

static ButtonEditor theButtonEditor;

static void cb_ButtonEditor_destruction (Editor editor, void *closure) {
	(void) editor;
	(void) closure;
	theButtonEditor = NULL;
}

DIRECT (praat_editButtons)
	if (theButtonEditor) {
		theButtonEditor -> raise ();
	} else {
		theButtonEditor = ButtonEditor_create ();
		theButtonEditor -> setDestructionCallback (cb_ButtonEditor_destruction, NULL);
	}
END

FORM (praat_addMenuCommand, L"Add menu command", L"Add menu command...")
	WORD (L"Window", L"Objects")
	WORD (L"Menu", L"New")
	SENTENCE (L"Command", L"Hallo...")
	SENTENCE (L"After command", L"")
	INTEGER (L"Depth", L"0")
	LABEL (L"", L"Script file:")
	TEXTFIELD (L"Script", L"/u/miep/hallo.praat")
	OK
DO
	praat_addMenuCommandScript (GET_STRING (L"Window"), GET_STRING (L"Menu"),
		GET_STRING (L"Command"), GET_STRING (L"After command"),
		GET_INTEGER (L"Depth"), GET_STRING (L"Script"));
END

FORM (praat_hideMenuCommand, L"Hide menu command", L"Hide menu command...")
	WORD (L"Window", L"Objects")
	WORD (L"Menu", L"New")
	SENTENCE (L"Command", L"Hallo...")
	OK
DO
	praat_hideMenuCommand (GET_STRING (L"Window"), GET_STRING (L"Menu"), GET_STRING (L"Command"));
END

FORM (praat_showMenuCommand, L"Show menu command", L"Show menu command...")
	WORD (L"Window", L"Objects")
	WORD (L"Menu", L"New")
	SENTENCE (L"Command", L"Hallo...")
	OK
DO
	praat_showMenuCommand (GET_STRING (L"Window"), GET_STRING (L"Menu"), GET_STRING (L"Command"));
END

FORM (praat_addAction, L"Add action command", L"Add action command...")
	WORD (L"Class 1", L"Sound")
	INTEGER (L"Number 1", L"0")
	WORD (L"Class 2", L"")
	INTEGER (L"Number 2", L"0")
	WORD (L"Class 3", L"")
	INTEGER (L"Number 3", L"0")
	SENTENCE (L"Command", L"Play reverse")
	SENTENCE (L"After command", L"Play")
	INTEGER (L"Depth", L"0")
	LABEL (L"", L"Script file:")
	TEXTFIELD (L"Script", L"/u/miep/playReverse.praat")
	OK
DO
	praat_addActionScript (GET_STRING (L"Class 1"), GET_INTEGER (L"Number 1"),
		GET_STRING (L"Class 2"), GET_INTEGER (L"Number 2"), GET_STRING (L"Class 3"),
		GET_INTEGER (L"Number 3"), GET_STRING (L"Command"), GET_STRING (L"After command"),
		GET_INTEGER (L"Depth"), GET_STRING (L"Script"));
END

FORM (praat_hideAction, L"Hide action command", L"Hide action command...")
	WORD (L"Class 1", L"Sound")
	WORD (L"Class 2", L"")
	WORD (L"Class 3", L"")
	SENTENCE (L"Command", L"Play")
	OK
DO
	praat_hideAction_classNames (GET_STRING (L"Class 1"), GET_STRING (L"Class 2"), GET_STRING (L"Class 3"), GET_STRING (L"Command"));
END

FORM (praat_showAction, L"Show action command", L"Show action command...")
	WORD (L"Class 1", L"Sound")
	WORD (L"Class 2", L"")
	WORD (L"Class 3", L"")
	SENTENCE (L"Command", L"Play")
	OK
DO
	praat_showAction_classNames (GET_STRING (L"Class 1"), GET_STRING (L"Class 2"), GET_STRING (L"Class 3"), GET_STRING (L"Command"));
END

/********** Callbacks of the Preferences menu. **********/

FORM (TextInputEncodingSettings, L"Text reading preferences", L"Unicode")
	RADIO_ENUM (L"Encoding of 8-bit text files", kMelder_textInputEncoding, DEFAULT)
	OK
SET_ENUM (L"Encoding of 8-bit text files", kMelder_textInputEncoding, Melder_getInputEncoding ())
DO
	Melder_setInputEncoding (GET_ENUM (kMelder_textInputEncoding, L"Encoding of 8-bit text files"));
END

FORM (TextOutputEncodingSettings, L"Text writing preferences", L"Unicode")
	RADIO_ENUM (L"Output encoding", kMelder_textOutputEncoding, DEFAULT)
	OK
SET_ENUM (L"Output encoding", kMelder_textOutputEncoding, Melder_getOutputEncoding ())
DO
	Melder_setOutputEncoding (GET_ENUM (kMelder_textOutputEncoding, L"Output encoding"));
END

/********** Callbacks of the Goodies menu. **********/

FORM (praat_calculator, L"Calculator", L"Calculator")
	LABEL (L"", L"Type any numeric formula or string formula:")
	TEXTFIELD (L"expression", L"5*5")
	LABEL (L"", L"Note that you can include many special functions in your formula,")
	LABEL (L"", L"including statistical functions and acoustics-auditory conversions.")
	LABEL (L"", L"For details, click Help.")
	OK
DO
	if (interpreter == NULL) {
		interpreter = Interpreter_create (NULL, NULL);
		try {
			Interpreter_anyExpression (interpreter, GET_STRING (L"expression"), NULL);
			forget (interpreter);
		} catch (MelderError) {
			forget (interpreter);
			throw;
		}
	} else {
		Interpreter_anyExpression (interpreter, GET_STRING (L"expression"), NULL);
	}
END

FORM (praat_reportDifferenceOfTwoProportions, L"Report difference of two proportions", L"Difference of two proportions")
	INTEGER (L"left Row 1", L"71")
	INTEGER (L"right Row 1", L"39")
	INTEGER (L"left Row 2", L"93")
	INTEGER (L"right Row 2", L"27")
	OK
DO
	double a = GET_INTEGER (L"left Row 1"), b = GET_INTEGER (L"right Row 1");
	double c = GET_INTEGER (L"left Row 2"), d = GET_INTEGER (L"right Row 2");
	double n = a + b + c + d;
	double aexp, bexp, cexp, dexp, crossDifference, x2;
	REQUIRE (a >= 0 && b >= 0 && c >= 0 && d >= 0, L"Numbers must not be negative.")
	REQUIRE ((a > 0 || b > 0) && (c > 0 || d > 0), L"Row totals must be positive.")
	REQUIRE ((a > 0 || c > 0) && (b > 0 || d > 0), L"Column totals must be positive.")
	MelderInfo_open ();
	MelderInfo_writeLine (L"Observed row 1 =    ", Melder_integer (a), L"    ", Melder_integer (b));
	MelderInfo_writeLine (L"Observed row 2 =    ", Melder_integer (c), L"    ", Melder_integer (d));
	aexp = (a + b) * (a + c) / n;
	bexp = (a + b) * (b + d) / n;
	cexp = (a + c) * (c + d) / n;
	dexp = (b + d) * (c + d) / n;
	MelderInfo_writeLine (L"");
	MelderInfo_writeLine (L"Expected row 1 =    ", Melder_double (aexp), L"    ", Melder_double (bexp));
	MelderInfo_writeLine (L"Expected row 2 =    ", Melder_double (cexp), L"    ", Melder_double (dexp));
	/*
	 * Continuity correction:
	 * bring the observed numbers closer to the expected numbers by 0.5 (if possible).
	 */
	if (a < aexp) { a += 0.5; if (a > aexp) a = aexp; }
	else if (a > aexp) { a -= 0.5; if (a < aexp) a = aexp; }
	if (b < bexp) { b += 0.5; if (b > bexp) b = bexp; }
	else if (b > bexp) { b -= 0.5; if (b < bexp) b = bexp; }
	if (c < cexp) { c += 0.5; if (c > cexp) c = cexp; }
	else if (c > cexp) { c -= 0.5; if (c < cexp) c = cexp; }
	if (d < dexp) { d += 0.5; if (d > dexp) d = dexp; }
	else if (d > dexp) { d -= 0.5; if (d < dexp) d = dexp; }
	MelderInfo_writeLine (L"");
	MelderInfo_writeLine (L"Corrected observed row 1 =    ", Melder_double (a), L"    ", Melder_double (b));
	MelderInfo_writeLine (L"Corrected observed row 2 =    ", Melder_double (c), L"    ", Melder_double (d));
	
	n = a + b + c + d;
	crossDifference = a * d - b * c;
	x2 = n * crossDifference * crossDifference / (a + b) / (c + d) / (a + c) / (b + d);
	MelderInfo_writeLine (L"");
	MelderInfo_writeLine (L"Chi-square =    ", Melder_double (x2));
	MelderInfo_writeLine (L"Two-tailed p =    ", Melder_double (NUMchiSquareQ (x2, 1)));
	MelderInfo_close ();
END

/********** Callbacks of the Technical menu. **********/

FORM (praat_debug, L"Set debugging options", 0)
	LABEL (L"", L"If you switch Tracing on, Praat will write lots of detailed ")
	LABEL (L"", L"information about what goes on in Praat")
	structMelderDir dir;
	Melder_getPrefDir (& dir);
	structMelderFile file;
	#ifdef UNIX
		MelderDir_getFile (& dir, L"tracing", & file);
	#else
		MelderDir_getFile (& dir, L"Tracing.txt", & file);
	#endif
	LABEL (L"", Melder_wcscat (L"to ", Melder_fileToPath (& file), L"."))
	BOOLEAN (L"Tracing", 0)
	LABEL (L"", L"Setting the following to anything other than zero")
	LABEL (L"", L"will alter the behaviour of Praat")
	LABEL (L"", L"in unpredictable ways.")
	INTEGER (L"Debug option", L"0")
	OK
SET_INTEGER (L"Tracing", Melder_getTracing ())
SET_INTEGER (L"Debug option", Melder_debug)
DO
	Melder_setTracing (GET_INTEGER (L"Tracing"));
	Melder_debug = GET_INTEGER (L"Debug option");
END

DIRECT (praat_listReadableTypesOfObjects)
	Thing_listReadableClasses ();
END

DIRECT (praat_reportGraphicalProperties)
	praat_reportGraphicalProperties ();
END

DIRECT (praat_reportIntegerProperties)
	praat_reportIntegerProperties ();
END

DIRECT (praat_reportMemoryUse)
	praat_reportMemoryUse ();
END

DIRECT (praat_reportTextProperties)
	praat_reportTextProperties ();
END

/********** Callbacks of the Open menu. **********/

static void readFromFile (MelderFile file) {
	autoData object = (Data) Data_readFromFile (file);
	if (object.peek() == NULL) return;
	if (Thing_member (object.peek(), classManPages) && ! Melder_batch) {
		ManPages pages = (ManPages) object.peek();
		ManPage firstPage = static_cast<ManPage> (pages -> pages -> item [1]);
		Manual_create (firstPage -> title, object.transfer(), true);
		if (pages -> executable)
			Melder_warning (L"These manual pages contain links to executable scripts.\n"
				"Only navigate these pages if you trust their author!");
		return;
	}
	if (Thing_member (object.peek(), classScript) && ! Melder_batch) {
		ScriptEditor_createFromScript (NULL, (Script) object.peek());
		return;
	}
	praat_new (object.transfer(), MelderFile_name (file));
	praat_updateSelection ();
}

FORM_READ (Data_readFromFile, L"Read Object(s) from file", 0, true)
	readFromFile (file);
END

/********** Callbacks of the Save menu. **********/

FORM_WRITE (Data_writeToTextFile, L"Save Object(s) as one text file", 0, 0)
	if (theCurrentPraatObjects -> totalSelection == 1) {
		LOOP {
			iam (Data);
			Data_writeToTextFile (me, file);
		}
	} else {
		autoCollection set = praat_getSelectedObjects ();
		Data_writeToTextFile (set.peek(), file);
	}
END

FORM_WRITE (Data_writeToShortTextFile, L"Save Object(s) as one short text file", 0, 0)
	if (theCurrentPraatObjects -> totalSelection == 1) {
		LOOP {
			iam (Data);
			Data_writeToShortTextFile (me, file);
		}
	} else {
		autoCollection set = praat_getSelectedObjects ();
		Data_writeToShortTextFile (set.peek(), file);
	}
END

FORM_WRITE (Data_writeToBinaryFile, L"Save Object(s) as one binary file", 0, 0)
	if (theCurrentPraatObjects -> totalSelection == 1) {
		LOOP {
			iam (Data);
			Data_writeToBinaryFile (me, file);
		}
	} else {
		autoCollection set = praat_getSelectedObjects ();
		Data_writeToBinaryFile (set.peek(), file);
	}
END

FORM (ManPages_saveToHtmlDirectory, L"Save all pages as HTML files", 0)
	LABEL (L"", L"Type a directory name:")
	TEXTFIELD (L"directory", L"")
	OK
structMelderDir currentDirectory = { { 0 } };
Melder_getDefaultDir (& currentDirectory);
SET_STRING (L"directory", Melder_dirToPath (& currentDirectory))
DO
	wchar_t *directory = GET_STRING (L"directory");
	LOOP {
		iam (ManPages);
		ManPages_writeAllToHtmlDir (me, directory);
	}
END

DIRECT (ManPages_view)
	LOOP {
		iam (ManPages);
		ManPage firstPage = static_cast<ManPage> (my pages -> item [1]);
		autoManual manual = Manual_create (firstPage -> title, me, false);
		if (my executable)
			Melder_warning (L"These manual pages contain links to executable scripts.\n"
				"Only navigate these pages if you trust their author!");
		praat_installEditor (manual.transfer(), IOBJECT);
	}
END

/********** Callbacks of the Help menu. **********/

FORM (SearchManual, L"Search manual", L"Manual")
	LABEL (L"", L"Search for strings (separate with spaces):")
	TEXTFIELD (L"query", L"")
	OK
DO
	if (theCurrentPraatApplication -> batch)
		Melder_throw (L"Cannot view a manual from batch.");
	Manual manPage = Manual_create (L"Intro", theCurrentPraatApplication -> manPages, false);
	Manual_search (manPage, GET_STRING (L"query"));
END

FORM (GoToManualPage, L"Go to manual page", 0)
	{long numberOfPages;
	const wchar_t **pages = ManPages_getTitles (theCurrentPraatApplication -> manPages, & numberOfPages);
	LIST (L"Page", numberOfPages, pages, 1)}
	OK
DO
	if (theCurrentPraatApplication -> batch)
		Melder_throw (L"Cannot view a manual from batch.");
	Manual manPage = Manual_create (L"Intro", theCurrentPraatApplication -> manPages, false);
	HyperPage_goToPage_i (manPage, GET_INTEGER (L"Page"));
END

FORM (WriteManualToHtmlDirectory, L"Save all pages as HTML files", 0)
	LABEL (L"", L"Type a directory name:")
	TEXTFIELD (L"directory", L"")
	OK
structMelderDir currentDirectory = { { 0 } };
Melder_getDefaultDir (& currentDirectory);
SET_STRING (L"directory", Melder_dirToPath (& currentDirectory))
DO
	wchar_t *directory = GET_STRING (L"directory");
	ManPages_writeAllToHtmlDir (theCurrentPraatApplication -> manPages, directory);
END

/********** Menu descriptions. **********/

void praat_show (void) {
	/*
	 * (De)sensitivize the fixed buttons as appropriate for the current selection.
	 */
	praat_sensitivizeFixedButtonCommand (L"Remove", theCurrentPraatObjects -> totalSelection != 0);
	praat_sensitivizeFixedButtonCommand (L"Rename...", theCurrentPraatObjects -> totalSelection == 1);
	praat_sensitivizeFixedButtonCommand (L"Copy...", theCurrentPraatObjects -> totalSelection == 1);
	praat_sensitivizeFixedButtonCommand (L"Info", theCurrentPraatObjects -> totalSelection == 1);
	praat_sensitivizeFixedButtonCommand (L"Inspect", theCurrentPraatObjects -> totalSelection != 0);
	praat_actions_show ();
	if (theCurrentPraatApplication == & theForegroundPraatApplication && theButtonEditor) theButtonEditor -> dataChanged ();
}

/********** Menu descriptions. **********/

void praat_addFixedButtons (GuiWindow window) {
	praat_addFixedButtonCommand (window, L"Rename...", DO_Rename, 8, 70);
	praat_addFixedButtonCommand (window, L"Copy...", DO_Copy, 98, 70);
	praat_addFixedButtonCommand (window, L"Inspect", DO_Inspect, 8, 40);
	praat_addFixedButtonCommand (window, L"Info", DO_Info, 98, 40);
	praat_addFixedButtonCommand (window, L"Remove", DO_Remove, 8, 10);
}

static void searchProc (void) {
	DO_SearchManual (NULL, 0, NULL, NULL, NULL, NULL, false, NULL);
}

static MelderString itemTitle_about = { 0 };

static Any scriptRecognizer (int nread, const char *header, MelderFile file) {
	const wchar_t *name = MelderFile_name (file);
	if (nread < 2) return NULL;
	if ((header [0] == '#' && header [1] == '!') || wcsstr (name, L".praat") == name + wcslen (name) - 6
	    || wcsstr (name, L".html") == name + wcslen (name) - 5)
	{
		return Script_createFromFile (file);
	}
	return NULL;
}

static void cb_openDocument (MelderFile file) {
	try {
		readFromFile (file);
	} catch (MelderError) {
		Melder_flushError (NULL);
	}
}

#if cocoa
DIRECT (praat_cut)
	[[[NSApp keyWindow] fieldEditor: YES forObject: nil] cut: nil];
END
DIRECT (praat_copy)
	[[[NSApp keyWindow] fieldEditor: YES forObject: nil] copy: nil];
END
DIRECT (praat_paste)
	[[[NSApp keyWindow] fieldEditor: YES forObject: nil] pasteAsPlainText: nil];
END
#endif

void praat_addMenus (GuiWindow window) {
	Melder_setSearchProc (searchProc);

	Data_recognizeFileType (scriptRecognizer);

	/*
	 * Create the menu titles in the bar.
	 */
	if (! theCurrentPraatApplication -> batch) {
		#ifdef macintosh
			praatMenu = GuiMenu_createInWindow (NULL, L"\024", 0);
			#if cocoa
				editMenu = GuiMenu_createInWindow (NULL, L"Edit", 0);
			#endif
		#else
			praatMenu = GuiMenu_createInWindow (window, L"Praat", 0);
		#endif
		newMenu = GuiMenu_createInWindow (window, L"New", 0);
		readMenu = GuiMenu_createInWindow (window, L"Open", 0);
		praat_actions_createWriteMenu (window);
		#ifdef macintosh
			applicationHelpMenu = GuiMenu_createInWindow (NULL, L"Help", 0);
		#endif
		helpMenu = GuiMenu_createInWindow (window, L"Help", 0);
	}
	
	MelderString_append (& itemTitle_about, L"About ", Melder_peekUtf8ToWcs (praatP.title), L"...");
	#ifdef macintosh
		praat_addMenuCommand (L"Objects", L"Praat", itemTitle_about.string, 0, praat_UNHIDABLE, DO_About);
		#if cocoa
			praat_addMenuCommand (L"Objects", L"Edit", L"Cut", 0, 'X', DO_praat_cut);
			praat_addMenuCommand (L"Objects", L"Edit", L"Copy", 0, 'C', DO_praat_copy);
			praat_addMenuCommand (L"Objects", L"Edit", L"Paste", 0, 'V', DO_praat_paste);
		#endif
	#endif
	#ifdef UNIX
		praat_addMenuCommand (L"Objects", L"Praat", itemTitle_about.string, 0, praat_UNHIDABLE, DO_About);
	#endif
	praat_addMenuCommand (L"Objects", L"Praat", L"-- script --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"Praat", L"Run script...", 0, praat_HIDDEN, DO_praat_runScript);
	praat_addMenuCommand (L"Objects", L"Praat", L"New Praat script", 0, 0, DO_praat_newScript);
	praat_addMenuCommand (L"Objects", L"Praat", L"Open Praat script...", 0, 0, DO_praat_openScript);
	praat_addMenuCommand (L"Objects", L"Praat", L"-- buttons --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"Praat", L"Add menu command...", 0, praat_HIDDEN, DO_praat_addMenuCommand);
	praat_addMenuCommand (L"Objects", L"Praat", L"Hide menu command...", 0, praat_HIDDEN, DO_praat_hideMenuCommand);
	praat_addMenuCommand (L"Objects", L"Praat", L"Show menu command...", 0, praat_HIDDEN, DO_praat_showMenuCommand);
	praat_addMenuCommand (L"Objects", L"Praat", L"Add action command...", 0, praat_HIDDEN, DO_praat_addAction);
	praat_addMenuCommand (L"Objects", L"Praat", L"Hide action command...", 0, praat_HIDDEN, DO_praat_hideAction);
	praat_addMenuCommand (L"Objects", L"Praat", L"Show action command...", 0, praat_HIDDEN, DO_praat_showAction);

	GuiMenuItem menuItem = praat_addMenuCommand (L"Objects", L"Praat", L"Goodies", 0, praat_UNHIDABLE, 0);
	goodiesMenu = menuItem ? menuItem -> d_menu : NULL;
	praat_addMenuCommand (L"Objects", L"Goodies", L"Calculator...", 0, 'U', DO_praat_calculator);
	praat_addMenuCommand (L"Objects", L"Goodies", L"Report difference of two proportions...", 0, 0, DO_praat_reportDifferenceOfTwoProportions);

	menuItem = praat_addMenuCommand (L"Objects", L"Praat", L"Preferences", 0, praat_UNHIDABLE, 0);
	preferencesMenu = menuItem ? menuItem -> d_menu : NULL;
	praat_addMenuCommand (L"Objects", L"Preferences", L"Buttons...", 0, praat_UNHIDABLE, DO_praat_editButtons);   /* Cannot be hidden. */
	praat_addMenuCommand (L"Objects", L"Preferences", L"-- encoding prefs --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"Preferences", L"Text reading preferences...", 0, 0, DO_TextInputEncodingSettings);
	praat_addMenuCommand (L"Objects", L"Preferences", L"Text writing preferences...", 0, 0, DO_TextOutputEncodingSettings);

	menuItem = praat_addMenuCommand (L"Objects", L"Praat", L"Technical", 0, praat_UNHIDABLE, 0);
	technicalMenu = menuItem ? menuItem -> d_menu : NULL;
	praat_addMenuCommand (L"Objects", L"Technical", L"List readable types of objects", 0, 0, DO_praat_listReadableTypesOfObjects);
	praat_addMenuCommand (L"Objects", L"Technical", L"Report memory use", 0, 0, DO_praat_reportMemoryUse);
	praat_addMenuCommand (L"Objects", L"Technical", L"Report integer properties", 0, 0, DO_praat_reportIntegerProperties);
	praat_addMenuCommand (L"Objects", L"Technical", L"Report text properties", 0, 0, DO_praat_reportTextProperties);
	praat_addMenuCommand (L"Objects", L"Technical", L"Report graphical properties", 0, 0, DO_praat_reportGraphicalProperties);
	praat_addMenuCommand (L"Objects", L"Technical", L"Debug...", 0, 0, DO_praat_debug);

	praat_addMenuCommand (L"Objects", L"Open", L"Read from file...", 0, praat_ATTRACTIVE + 'O', DO_Data_readFromFile);

	praat_addAction1 (classData, 0, L"Save as text file...", 0, 0, DO_Data_writeToTextFile);
	praat_addAction1 (classData, 0, L"Write to text file...", 0, praat_HIDDEN, DO_Data_writeToTextFile);
	praat_addAction1 (classData, 0, L"Save as short text file...", 0, 0, DO_Data_writeToShortTextFile);
	praat_addAction1 (classData, 0, L"Write to short text file...", 0, praat_HIDDEN, DO_Data_writeToShortTextFile);
	praat_addAction1 (classData, 0, L"Save as binary file...", 0, 0, DO_Data_writeToBinaryFile);
	praat_addAction1 (classData, 0, L"Write to binary file...", 0, praat_HIDDEN, DO_Data_writeToBinaryFile);

	praat_addAction1 (classManPages, 1, L"Save to HTML directory...", 0, 0, DO_ManPages_saveToHtmlDirectory);
	praat_addAction1 (classManPages, 1, L"View", 0, 0, DO_ManPages_view);
}

void praat_addMenus2 (void) {
	static MelderString itemTitle_search = { 0 };
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"-- manual --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Go to manual page...", 0, 0, DO_GoToManualPage);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Write manual to HTML directory...", 0, praat_HIDDEN, DO_WriteManualToHtmlDirectory);
	MelderString_empty (& itemTitle_search);
	MelderString_append (& itemTitle_search, L"Search ", Melder_peekUtf8ToWcs (praatP.title), L" manual...");
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", itemTitle_search.string, 0, 'M', DO_SearchManual);
	#ifdef _WIN32
		praat_addMenuCommand (L"Objects", L"Help", L"-- about --", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"Help", itemTitle_about.string, 0, praat_UNHIDABLE, DO_About);
	#endif

	#if defined (macintosh) || defined (_WIN32)
		Gui_setOpenDocumentCallback (cb_openDocument);
	#endif
}

/* End of file praat_objectMenus.cpp */
