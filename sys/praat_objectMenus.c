/* praat_objectMenus.c
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
 * pb 2002/03/07 GPL
 * pb 2002/12/01 allow string expressions in calculator
 * pb 2003/03/09 simplified calculator
 * pb 2004/11/16 Win: more room for fixed buttons
 * pb 2004/12/05 renamed script running procedures
 * pb 2005/07/06 repaired a memory leak in creating a script editor from a double click
 * pb 2005/08/22 renamed the Control menu to "Praat" on all systems (like on the Mac)
 * pb 2005/11/18 HTML files are considered scripts (this is just for testing)
 * pb 2006/08/12 allowed renaming with European characters
 * pb 2006/10/20 embedded scripts
 * pb 2006/12/26 theCurrentPraat
 * pb 2007/01/26 layout objects window
 * pb 2007/06/10 wchar_t
 */

#include <ctype.h>
#include "praatP.h"
#include "praat_script.h"
#include "ScriptEditor.h"
#include "ButtonEditor.h"
#include "DataEditor.h"
#include "site.h"

#define EDITOR  theCurrentPraat -> list [IOBJECT]. editors

/********** Callbacks of the fixed buttons. **********/

DIRECT (Remove)
	WHERE_DOWN (SELECTED)
		praat_removeObject (IOBJECT);
	praat_show ();
END

FORM (Rename, "Rename object", "Rename...")
	LABEL ("rename object", "New name:")
	TEXTFIELD ("newName", "")
	OK
{ int IOBJECT; WHERE (SELECTED) SET_STRING ("newName", NAME) }
DO
	wchar_t fullName [200], *string = GET_STRINGW (L"newName");
	int ieditor;
	if (theCurrentPraat -> totalSelection == 0)
		return Melder_error ("Selection changed!\nNo object selected. Cannot rename.");
	if (theCurrentPraat -> totalSelection > 1)
		return Melder_error ("Selection changed!\nCannot rename more than one object at a time.");
	WHERE (SELECTED) break;
	praat_cleanUpName (string);   /* This is allowed because "string" is local and dispensible. */
	swprintf (fullName, 200, L"%ls %ls", Thing_classNameW (OBJECT), string);
	if (! wcsequ (fullName, FULL_NAMEW)) {
		Melder_free (FULL_NAMEW), FULL_NAMEW = Melder_wcsdup (fullName);
		praat_list_renameAndSelect (IOBJECT, fullName);
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (EDITOR [ieditor]) Thing_setNameW (EDITOR [ieditor], fullName);
		Thing_setNameW (OBJECT, string);
	}
END

FORM (Copy, "Copy object", "Copy...")
	LABEL ("copy object", "Name of new object:")
	TEXTFIELD ("newName", "")
	OK
{ int IOBJECT; WHERE (SELECTED) SET_STRING ("newName", NAME) }
DO
	if (theCurrentPraat -> totalSelection == 0)
		return Melder_error ("Selection changed!\nNo object selected. Cannot copy.");
	if (theCurrentPraat -> totalSelection > 1)
		return Melder_error ("Selection changed!\nCannot copy more than one object at a time.");
	WHERE (SELECTED) {
		char *name = GET_STRING ("newName");
		if (! praat_new (Data_copy (OBJECT), name)) return 0;
	}
END

DIRECT (Info)
	if (theCurrentPraat -> totalSelection == 0)
		return Melder_error ("Selection changed!\nNo object selected. Cannot query.");
	if (theCurrentPraat -> totalSelection > 1)
		return Melder_error ("Selection changed!\nCannot query more than one object at a time.");
	WHERE (SELECTED) Thing_info (OBJECT);
END

DIRECT (Inspect)
	if (theCurrentPraat -> totalSelection == 0)
		return Melder_error ("Selection changed!\nNo object selected. Cannot inspect.");
	if (theCurrentPraat -> batch) {
		return Melder_error ("Cannot inspect data from batch.");
	} else {
		WHERE (SELECTED)
			if (! praat_installEditor (DataEditor_create (theCurrentPraat -> topShell, FULL_NAMEW, OBJECT), IOBJECT)) return 0;
	}
END

/********** The fixed menus. **********/

static Widget appleMenu, praatMenu, newMenu, readMenu, goodiesMenu, preferencesMenu, applicationHelpMenu, helpMenu;

Widget praat_objects_resolveMenu (const wchar_t *menu) {
	return
		#ifdef macintosh
		wcsequ (menu, L"Praat") || wcsequ (menu, L"Control") ? ( Melder_systemVersion >= 0x0A00 ? appleMenu : praatMenu ) :
		#else
		wcsequ (menu, L"Praat") || wcsequ (menu, L"Control") ? praatMenu :
		#endif
		wcsequ (menu, L"New") || wcsequ (menu, L"Create") ? newMenu :
		wcsequ (menu, L"Read") ? readMenu :
		wcsequ (menu, L"Help") ? helpMenu :
		wcsequ (menu, L"Goodies") ? goodiesMenu :
		wcsequ (menu, L"Preferences") ? preferencesMenu :
		#ifdef macintosh
		wcsequ (menu, L"Apple") ? appleMenu :
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

DIRECT (Memory_info)
	praat_memoryInfo ();
END

DIRECT (praat_newScript)
	ScriptEditor editor = ScriptEditor_createFromText (theCurrentPraat -> topShell, NULL, NULL);
	if (! editor) return 0;
END

DIRECT (praat_openScript)
	ScriptEditor editor = ScriptEditor_createFromText (theCurrentPraat -> topShell, NULL, NULL);
	if (! editor) return 0;
	TextEditor_showOpen (editor);
END

static ButtonEditor theButtonEditor;

static void cb_ButtonEditor_destroy (Any editor, void *closure) {
	(void) editor;
	(void) closure;
	theButtonEditor = NULL;
}

FORM (praat_debug, "Set debugging options", 0)
	LABEL ("", "Setting the following to anything other than zero")
	LABEL ("", "will alter the behaviour of this program")
	LABEL ("", "in inpredictable ways.")
	INTEGER ("Debug option", "0")
	OK
SET_INTEGER ("Debug option", Melder_debug)
DO
	Melder_debug = GET_INTEGER ("Debug option");
END

DIRECT (praat_editButtons)
	if (theButtonEditor) {
		Editor_raise (theButtonEditor);
	} else {
		theButtonEditor = ButtonEditor_create (theCurrentPraat -> topShell);
		Editor_setDestroyCallback (theButtonEditor, cb_ButtonEditor_destroy, NULL);
		if (! theButtonEditor) return 0;
	}
END

FORM (praat_addMenuCommand, "Add menu command", "Add menu command...")
	WORD ("Window", "Objects")
	WORD ("Menu", "New")
	SENTENCE ("Command", "Hallo...")
	SENTENCE ("After command", "")
	INTEGER ("Depth", "0")
	LABEL ("", "Script file:")
	TEXTFIELD ("Script", "/u/miep/hallo.praat")
	OK
DO
	if (! praat_addMenuCommandScript (GET_STRINGW (L"Window"), GET_STRINGW (L"Menu"),
		GET_STRINGW (L"Command"), GET_STRINGW (L"After command"),
		GET_INTEGER ("Depth"), GET_STRINGW (L"Script"))) return 0;
END

FORM (praat_hideMenuCommand, "Hide menu command", "Hide menu command...")
	WORD ("Window", "Objects")
	WORD ("Menu", "New")
	SENTENCE ("Command", "Hallo...")
	OK
DO
	if (! praat_hideMenuCommand (GET_STRINGW (L"Window"), GET_STRINGW (L"Menu"),
		GET_STRINGW (L"Command"))) return 0;
END

FORM (praat_showMenuCommand, "Show menu command", "Show menu command...")
	WORD ("Window", "Objects")
	WORD ("Menu", "New")
	SENTENCE ("Command", "Hallo...")
	OK
DO
	if (! praat_showMenuCommand (GET_STRINGW (L"Window"), GET_STRINGW (L"Menu"),
		GET_STRINGW (L"Command"))) return 0;
END

FORM (praat_addAction, "Add action command", "Add action command...")
	WORD ("Class 1", "Sound")
	INTEGER ("Number 1", "0")
	WORD ("Class 2", "")
	INTEGER ("Number 2", "0")
	WORD ("Class 3", "")
	INTEGER ("Number 3", "0")
	SENTENCE ("Command", "Play reverse")
	SENTENCE ("After command", "Play")
	INTEGER ("Depth", "0")
	LABEL ("", "Script file:")
	TEXTFIELD ("Script", "/u/miep/playReverse.praat")
	OK
DO
	if (! praat_addActionScript (GET_STRINGW (L"Class 1"), GET_INTEGER ("Number 1"),
		GET_STRINGW (L"Class 2"), GET_INTEGER ("Number 2"), GET_STRINGW (L"Class 3"),
		GET_INTEGER ("Number 3"), GET_STRINGW (L"Command"), GET_STRINGW (L"After command"),
		GET_INTEGER ("Depth"), GET_STRINGW (L"Script"))) return 0;
END

FORM (praat_hideAction, "Hide action command", "Hide action command...")
	WORD ("Class 1", "Sound")
	WORD ("Class 2", "")
	WORD ("Class 3", "")
	SENTENCE ("Command", "Play")
	OK
DO
	if (! praat_hideAction_classNames (GET_STRINGW (L"Class 1"),
		GET_STRINGW (L"Class 2"), GET_STRINGW (L"Class 3"), GET_STRINGW (L"Command"))) return 0;
END

FORM (praat_showAction, "Show action command", "Show action command...")
	WORD ("Class 1", "Sound")
	WORD ("Class 2", "")
	WORD ("Class 3", "")
	SENTENCE ("Command", "Play")
	OK
DO
	if (! praat_showAction_classNames (GET_STRINGW (L"Class 1"),
		GET_STRINGW (L"Class 2"), GET_STRINGW (L"Class 3"), GET_STRINGW (L"Command"))) return 0;
END

/********** Callbacks of the Preferences menu. **********/

FORM (TextInputEncodingSettings, "Text reading preferences", "Text reading preferences")
	#if defined (macintosh)
	RADIO ("Input encoding", 6)
	#elif defined (_WIN32)
	RADIO ("Input encoding", 4)
	#else
	RADIO ("Input encoding", 2)
	#endif
		OPTION ("UTF-8")
		OPTION ("Try UTF-8, then ISO Latin-1")
		OPTION ("ISO Latin-1")
		OPTION ("Try UTF-8, then Windows Latin-1")
		OPTION ("Windows Latin-1")
		OPTION ("Try UTF-8, then MacRoman")
		OPTION ("MacRoman")
	OK
SET_INTEGER ("Input encoding", Melder_getInputEncoding ())
DO
	Melder_setInputEncoding (GET_INTEGER ("Input encoding"));
END

FORM (TextOutputEncodingSettings, "Text writing preferences", "Text writing preferences")
	RADIO ("Output encoding", 3)
		OPTION ("UTF-8")
		OPTION ("UTF-16")
		OPTION ("Try ASCII, then UTF-16")
	OK
SET_INTEGER ("Output encoding", Melder_getOutputEncoding ())
DO
	Melder_setOutputEncoding (GET_INTEGER ("Output encoding"));
END

/********** Callbacks of the Goodies menu. **********/

FORMW (praat_calculator, L"Calculator", L"Calculator")
	LABELW (L"", L"Type any numeric formula or string formula:")
	TEXTFIELDW (L"expression", L"5*5")
	LABELW (L"", L"Note that you can include many special functions in your formula,")
	LABELW (L"", L"including statistical functions and acoustics-auditory conversions.")
	LABELW (L"", L"For details, click Help.")
	OK
DO
	return Interpreter_numericOrStringExpression (NULL, GET_STRINGW (L"expression"), NULL, NULL);
END

FORM (praat_reportDifferenceOfTwoProportions, "Report difference of two proportions", "Difference of two proportions")
	INTEGER ("left Row 1", "71")
	INTEGER ("right Row 1", "39")
	INTEGER ("left Row 2", "93")
	INTEGER ("right Row 2", "27")
	OK
DO
	double a = GET_INTEGER ("left Row 1"), b = GET_INTEGER ("right Row 1");
	double c = GET_INTEGER ("left Row 2"), d = GET_INTEGER ("right Row 2");
	double n = a + b + c + d;
	double aexp, bexp, cexp, dexp, crossDifference, x2;
	REQUIRE (a >= 0 && b >= 0 && c >= 0 && d >= 0, "Numbers must not be negative.")
	REQUIRE ((a > 0 || b > 0) && (c > 0 || d > 0), "Row totals must be positive.")
	REQUIRE ((a > 0 || c > 0) && (b > 0 || d > 0), "Column totals must be positive.")
	MelderInfo_open ();
	MelderInfo_writeLine4 ("Observed row 1 =    ", Melder_integer (a), "    ", Melder_integer (b));
	MelderInfo_writeLine4 ("Observed row 2 =    ", Melder_integer (c), "    ", Melder_integer (d));
	aexp = (a + b) * (a + c) / n;
	bexp = (a + b) * (b + d) / n;
	cexp = (a + c) * (c + d) / n;
	dexp = (b + d) * (c + d) / n;
	MelderInfo_writeLine1 ("");
	MelderInfo_writeLine4 ("Expected row 1 =    ", Melder_double (aexp), "    ", Melder_double (bexp));
	MelderInfo_writeLine4 ("Expected row 2 =    ", Melder_double (cexp), "    ", Melder_double (dexp));
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
	MelderInfo_writeLine1 ("");
	MelderInfo_writeLine4 ("Corrected observed row 1 =    ", Melder_double (a), "    ", Melder_double (b));
	MelderInfo_writeLine4 ("Corrected observed row 2 =    ", Melder_double (c), "    ", Melder_double (d));
	
	n = a + b + c + d;
	crossDifference = a * d - b * c;
	x2 = n * crossDifference * crossDifference / (a + b) / (c + d) / (a + c) / (b + d);
	MelderInfo_writeLine1 ("");
	MelderInfo_writeLine2 ("Chi-square =    ", Melder_double (x2));
	MelderInfo_writeLine2 ("Two-tailed p =    ", Melder_double (NUMchiSquareQ (x2, 1)));
	MelderInfo_close ();
END

/********** Callbacks of the Read menu. **********/

static int readFromFile (MelderFile file) {
	Data object = Data_readFromFile (file);
	int result;
	if (object && Thing_member (object, classManPages)) {
		ManPages pages = (ManPages) object;
		ManPage firstPage = pages -> pages -> item [1];
		if (! Manual_create (theCurrentPraat -> topShell, firstPage -> title, object)) return 0;
		if (pages -> executable)
			Melder_warning ("These manual pages contain links to executable scripts.\n"
				"Only navigate these pages if you trust their author!");
		return 1;
	}
	if (object && Thing_member (object, classScript)) {
		ScriptEditor_createFromScript (theCurrentPraat -> topShell, NULL, (Script) object);
		forget (object);
		iferror return 0;
		return 1;
	}
	result = praat_new (object, MelderFile_name (file));
	praat_updateSelection ();
	return result;
}

FORM_READ (Data_readFromFile, "Read Object(s) from file", 0)
	if (! readFromFile (file)) return 0;
END

/********** Callbacks of the Write menu. **********/

DIRECT (Data_writeToConsole) return Data_writeToConsole (ONLY_OBJECT); END

FORM_WRITE (Data_writeToTextFile, "Write Object(s) to text file", 0, 0)
	if (theCurrentPraat -> totalSelection == 1) {
		return Data_writeToTextFile (ONLY_OBJECT, file);
	} else {
		int result, IOBJECT;
		Collection list = Collection_create (classData, theCurrentPraat -> n);
		WHERE (SELECTED) Collection_addItem (list, OBJECT);
		result = Data_writeToTextFile (list, file);
		list -> size = 0;   /* Disown. */
		forget (list);
		return result;
	}
END

FORM_WRITE (Data_writeToShortTextFile, "Write Object(s) to short text file", 0, 0)
	if (theCurrentPraat -> totalSelection == 1)
		return Data_writeToShortTextFile (ONLY_OBJECT, file);
	else {
		int result, IOBJECT;
		Collection list = Collection_create (classData, theCurrentPraat -> n);
		WHERE (SELECTED) Collection_addItem (list, OBJECT);
		result = Data_writeToShortTextFile (list, file);
		list -> size = 0;   /* Disown. */
		forget (list);
		return result;
	}
END

FORM_WRITE (Data_writeToBinaryFile, "Write Object(s) to binary file", 0, 0)
	if (theCurrentPraat -> totalSelection == 1)
		return Data_writeToBinaryFile (ONLY_OBJECT, file);
	else {
		int result, IOBJECT;
		Collection list = Collection_create (classData, theCurrentPraat -> n);
		WHERE (SELECTED) Collection_addItem (list, OBJECT);
		result = Data_writeToBinaryFile (list, file);
		list -> size = 0;   /* Disown. */
		forget (list);
		return result;
	}
END

FORM_WRITE (Data_writeToLispFile, "Write Object to LISP file", 0, "lsp")
	return Data_writeToLispFile (ONLY_OBJECT, file);
END

/********** Callbacks of the Help menu. **********/

FORM (SearchManual, "Search manual", "Manual")
	LABEL ("", "Search for strings (separate with spaces):")
	TEXTFIELD ("query", "")
	OK
DO
	Manual manPage;
	if (theCurrentPraat -> batch)
		return Melder_error ("Cannot view manual from batch.");
	manPage = Manual_create (theCurrentPraat -> topShell, "Intro", theCurrentPraat -> manPages);
	Manual_search (manPage, GET_STRING ("query"));
END

FORM (GoToManualPage, "Go to manual page", 0)
	{long numberOfPages;
	const wchar_t **pages = ManPages_getTitles (theCurrentPraat -> manPages, & numberOfPages);
	LIST ("Page", numberOfPages, pages, 1)}
	OK
DO
	Manual manPage;
	if (theCurrentPraat -> batch)
		return Melder_error ("Cannot view manual from batch.");
	manPage = Manual_create (theCurrentPraat -> topShell, "Intro", theCurrentPraat -> manPages);
	if (! HyperPage_goToPage_i (manPage, GET_INTEGER ("Page"))) return 0;
END

FORM (WriteManualToHtmlDirectory, "Write all pages as HTML files", 0)
	LABEL ("", "Type a directory name:")
	TEXTFIELD ("directory", "")
	OK
structMelderDir currentDirectory = { { 0 } };
Melder_getDefaultDir (& currentDirectory);
SET_STRING ("directory", Melder_dirToPath (& currentDirectory))
DO
	wchar_t *directory = GET_STRINGW (L"directory");
	if (! ManPages_writeAllToHtmlDir (theCurrentPraat -> manPages, directory)) return 0;
END

/********** Menu descriptions. **********/

void praat_show (void) {
	/*
	 * (De)sensitivize the fixed buttons as appropriate for the current selection.
	 */
	praat_sensitivizeFixedButtonCommand (L"Remove", theCurrentPraat -> totalSelection != 0);
	praat_sensitivizeFixedButtonCommand (L"Rename...", theCurrentPraat -> totalSelection == 1);
	praat_sensitivizeFixedButtonCommand (L"Copy...", theCurrentPraat -> totalSelection == 1);
	praat_sensitivizeFixedButtonCommand (L"Info", theCurrentPraat -> totalSelection == 1);
	praat_sensitivizeFixedButtonCommand (L"Inspect", theCurrentPraat -> totalSelection != 0);
	praat_actions_show ();
	if (theCurrentPraat == & theForegroundPraat && theButtonEditor) Editor_dataChanged (theButtonEditor, NULL);
}

/********** Menu descriptions. **********/

void praat_addFixedButtons (Widget form) {
	praat_addFixedButtonCommand (form, L"Rename...", DO_Rename, 8, 70);
	praat_addFixedButtonCommand (form, L"Copy...", DO_Copy, 92, 70);
	praat_addFixedButtonCommand (form, L"Inspect", DO_Inspect, 8, 40);
	praat_addFixedButtonCommand (form, L"Info", DO_Info, 92, 40);
	praat_addFixedButtonCommand (form, L"Remove", DO_Remove, 8, 10);
}

static void searchProc (void) {
	DO_SearchManual (NULL, NULL);
}

static char itemTitle_about [100];

static Any scriptRecognizer (int nread, const char *header, MelderFile file) {
	wchar_t *name = MelderFile_nameW (file);
	if (nread < 2) return NULL;
	if ((header [0] == '#' && header [1] == '!') || wcsstr (name, L".praat") == name + wcslen (name) - 6
	    || wcsstr (name, L".html") == name + wcslen (name) - 5)
	{
		return Script_createFromFile (file);
	}
	return NULL;
}

static int cb_openDocument (MelderFile file) {
	if (! readFromFile (file))
		Melder_flushError (NULL);
	return 0;
}

void praat_addMenus (Widget bar) {
	Widget button;

	Melder_setSearchProc (searchProc);

	Data_recognizeFileType (scriptRecognizer);

	/*
	 * Create the menu titles in the bar.
	 */
	if (! theCurrentPraat -> batch) {
		#ifdef macintosh
			appleMenu = motif_addMenu (bar ? praatP.topBar : NULL, L"\024", 0); /* Apple icon. */
		#endif
		#ifdef macintosh
			if (Melder_systemVersion < 0x0A00) praatMenu = motif_addMenu (bar ? praatP.topBar : NULL, Melder_peekAsciiToWcs (praatP.title), 0);
		#else
			praatMenu = motif_addMenu (bar, L"Praat", 0);
		#endif
		newMenu = motif_addMenu (bar, L"New", 0);
		readMenu = motif_addMenu (bar, L"Read", 0);
		praat_actions_createWriteMenu (bar);
		#ifdef macintosh
			applicationHelpMenu = motif_addMenu (bar ? praatP.topBar : NULL, L"Help", 0);
		#endif
		helpMenu = motif_addMenu (bar, L"Help", 0);
	}
		
	sprintf (itemTitle_about, "About %s...", praatP.title);
	#ifdef macintosh
		praat_addMenuCommand ("Objects", "Apple", itemTitle_about, 0, praat_UNHIDABLE, DO_About);
	#endif
	#ifdef UNIX
		praat_addMenuCommand ("Objects", "Praat", itemTitle_about, 0, praat_UNHIDABLE, DO_About);
		praat_addMenuCommand ("Objects", "Praat", "-- script --", 0, 0, 0);
	#endif
	praat_addMenuCommand ("Objects", "Praat", "Debug...", 0, praat_HIDDEN, DO_praat_debug);
	praat_addMenuCommand ("Objects", "Praat", "Statistics...", 0, 0, DO_Memory_info);
	praat_addMenuCommand ("Objects", "Praat", "-- script --", 0, 0, 0);
	praat_addMenuCommand ("Objects", "Praat", "Run script...", 0, praat_HIDDEN, DO_praat_runScript);
	praat_addMenuCommand ("Objects", "Praat", "New Praat script", 0, 0, DO_praat_newScript);
	praat_addMenuCommand ("Objects", "Praat", "Open Praat script...", 0, 0, DO_praat_openScript);
	praat_addMenuCommand ("Objects", "Praat", "-- buttons --", 0, 0, 0);
	praat_addMenuCommand ("Objects", "Praat", "Add menu command...", 0, praat_HIDDEN, DO_praat_addMenuCommand);
	praat_addMenuCommand ("Objects", "Praat", "Hide menu command...", 0, praat_HIDDEN, DO_praat_hideMenuCommand);
	praat_addMenuCommand ("Objects", "Praat", "Show menu command...", 0, praat_HIDDEN, DO_praat_showMenuCommand);
	praat_addMenuCommand ("Objects", "Praat", "Add action command...", 0, praat_HIDDEN, DO_praat_addAction);
	praat_addMenuCommand ("Objects", "Praat", "Hide action command...", 0, praat_HIDDEN, DO_praat_hideAction);
	praat_addMenuCommand ("Objects", "Praat", "Show action command...", 0, praat_HIDDEN, DO_praat_showAction);
	button = praat_addMenuCommand ("Objects", "Praat", "Goodies", 0, praat_UNHIDABLE, 0);
	if (button) XtVaGetValues (button, XmNsubMenuId, & goodiesMenu, NULL);
	praat_addMenuCommand ("Objects", "Goodies", "Calculator...", 0, 'U', DO_praat_calculator);
	praat_addMenuCommand ("Objects", "Goodies", "Report difference of two proportions...", 0, 0, DO_praat_reportDifferenceOfTwoProportions);
	button = praat_addMenuCommand ("Objects", "Praat", "Preferences", 0, praat_UNHIDABLE, 0);
	if (button) XtVaGetValues (button, XmNsubMenuId, & preferencesMenu, NULL);
	praat_addMenuCommand ("Objects", "Preferences", "Buttons...", 0, praat_UNHIDABLE, DO_praat_editButtons);   /* Cannot be hidden. */
	praat_addMenuCommand ("Objects", "Preferences", "-- encoding prefs --", 0, 0, 0);
	praat_addMenuCommand ("Objects", "Preferences", "Text reading preferences...", 0, 0, DO_TextInputEncodingSettings);
	praat_addMenuCommand ("Objects", "Preferences", "Text writing preferences...", 0, 0, DO_TextOutputEncodingSettings);

	praat_addMenuCommand ("Objects", "Read", "Read from file...", 0, 'O', DO_Data_readFromFile);

	praat_addAction1 (classData, 1, "Write to console", 0,
	#ifdef UNIX
		0,
	#else
		praat_HIDDEN,
	#endif
		DO_Data_writeToConsole);
	praat_addAction1 (classData, 0, "Write to text file...", 0, 0, DO_Data_writeToTextFile);
	praat_addAction1 (classData, 0, "Write to short text file...", 0, 0, DO_Data_writeToShortTextFile);
	praat_addAction1 (classData, 0, "Write to binary file...", 0, 0, DO_Data_writeToBinaryFile);
}

void praat_addMenus2 (void) {
	static char itemTitle_search [100];
	praat_addMenuCommand ("Objects", "ApplicationHelp", "-- manual --", 0, 0, 0);
	praat_addMenuCommand ("Objects", "ApplicationHelp", "Go to manual page...", 0, 0, DO_GoToManualPage);
	praat_addMenuCommand ("Objects", "ApplicationHelp", "Write manual to HTML directory...", 0, praat_HIDDEN, DO_WriteManualToHtmlDirectory);
	sprintf (itemTitle_search, "Search %s manual...", praatP.title);
	praat_addMenuCommand ("Objects", "ApplicationHelp", itemTitle_search, 0, 'M', DO_SearchManual);
	#ifdef _WIN32
		praat_addMenuCommand ("Objects", "Help", "-- about --", 0, 0, 0);
		praat_addMenuCommand ("Objects", "Help", itemTitle_about, 0, praat_UNHIDABLE, DO_About);
	#endif

	#if defined (macintosh) || defined (_WIN32)
		motif_setOpenDocumentCallback (cb_openDocument);
	#endif
}

/* End of file praat_objectMenus.c */
