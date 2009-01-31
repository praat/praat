/* praat_objectMenus.c
 *
 * Copyright (C) 1992-2009 Paul Boersma
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
 * pb 2007/08/12 wchar_t
 * pb 2008/04/30 new Formula API
 * pb 2009/01/17 arguments to UiForm callbacks
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

FORM (Rename, L"Rename object", L"Rename...")
	LABEL (L"rename object", L"New name:")
	TEXTFIELD (L"newName", L"")
	OK
{ int IOBJECT; WHERE (SELECTED) SET_STRING (L"newName", NAMEW) }
DO
	wchar_t *string = GET_STRING (L"newName");
	if (theCurrentPraat -> totalSelection == 0)
		return Melder_error1 (L"Selection changed!\nNo object selected. Cannot rename.");
	if (theCurrentPraat -> totalSelection > 1)
		return Melder_error1 (L"Selection changed!\nCannot rename more than one object at a time.");
	WHERE (SELECTED) break;
	praat_cleanUpName (string);   /* This is allowed because "string" is local and dispensible. */
	static MelderString fullName = { 0 };
	MelderString_empty (& fullName);
	MelderString_append3 (& fullName, Thing_className (OBJECT), L" ", string);
	if (! wcsequ (fullName.string, FULL_NAME)) {
		Melder_free (FULL_NAME), FULL_NAME = Melder_wcsdup (fullName.string);
		MelderString listName = { 0 };
		MelderString_append3 (& listName, Melder_integer (ID), L". ", fullName.string);
		praat_list_renameAndSelect (IOBJECT, listName.string);
		MelderString_free (& listName);
		for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (EDITOR [ieditor]) Thing_setName (EDITOR [ieditor], fullName.string);
		Thing_setName (OBJECT, string);
	}
END

FORM (Copy, L"Copy object", L"Copy...")
	LABEL (L"copy object", L"Name of new object:")
	TEXTFIELD (L"newName", L"")
	OK
{ int IOBJECT; WHERE (SELECTED) SET_STRING (L"newName", NAMEW) }
DO
	if (theCurrentPraat -> totalSelection == 0)
		return Melder_error1 (L"Selection changed!\nNo object selected. Cannot copy.");
	if (theCurrentPraat -> totalSelection > 1)
		return Melder_error1 (L"Selection changed!\nCannot copy more than one object at a time.");
	WHERE (SELECTED) {
		wchar_t *name = GET_STRING (L"newName");
		if (! praat_new1 (Data_copy (OBJECT), name)) return 0;
	}
END

DIRECT (Info)
	if (theCurrentPraat -> totalSelection == 0)
		return Melder_error1 (L"Selection changed!\nNo object selected. Cannot query.");
	if (theCurrentPraat -> totalSelection > 1)
		return Melder_error1 (L"Selection changed!\nCannot query more than one object at a time.");
	WHERE (SELECTED) Thing_infoWithId (OBJECT, ID);
END

DIRECT (Inspect)
	if (theCurrentPraat -> totalSelection == 0)
		return Melder_error1 (L"Selection changed!\nNo object selected. Cannot inspect.");
	if (theCurrentPraat -> batch) {
		return Melder_error1 (L"Cannot inspect data from batch.");
	} else {
		WHERE (SELECTED)
			if (! praat_installEditor (DataEditor_create (theCurrentPraat -> topShell, ID_AND_FULL_NAME, OBJECT), IOBJECT)) return 0;
	}
END

/********** The fixed menus. **********/

static Widget praatMenu, newMenu, readMenu, goodiesMenu, preferencesMenu, applicationHelpMenu, helpMenu;

Widget praat_objects_resolveMenu (const wchar_t *menu) {
	return
		wcsequ (menu, L"Praat") || wcsequ (menu, L"Control") ? praatMenu :
		wcsequ (menu, L"New") || wcsequ (menu, L"Create") ? newMenu :
		wcsequ (menu, L"Read") ? readMenu :
		wcsequ (menu, L"Help") ? helpMenu :
		wcsequ (menu, L"Goodies") ? goodiesMenu :
		wcsequ (menu, L"Preferences") ? preferencesMenu :
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
	TextEditor_showOpen (ScriptEditor_as_TextEditor (editor));
END

static ButtonEditor theButtonEditor;

static void cb_ButtonEditor_destroy (Any editor, void *closure) {
	(void) editor;
	(void) closure;
	theButtonEditor = NULL;
}

FORM (praat_debug, L"Set debugging options", 0)
	LABEL (L"", L"Setting the following to anything other than zero")
	LABEL (L"", L"will alter the behaviour of this program")
	LABEL (L"", L"in inpredictable ways.")
	INTEGER (L"Debug option", L"0")
	OK
SET_INTEGER (L"Debug option", Melder_debug)
DO
	Melder_debug = GET_INTEGER (L"Debug option");
END

DIRECT (praat_editButtons)
	if (theButtonEditor) {
		Editor_raise (ButtonEditor_as_Editor (theButtonEditor));
	} else {
		theButtonEditor = ButtonEditor_create (theCurrentPraat -> topShell);
		Editor_setDestroyCallback (ButtonEditor_as_Editor (theButtonEditor), cb_ButtonEditor_destroy, NULL);
		if (! theButtonEditor) return 0;
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
	if (! praat_addMenuCommandScript (GET_STRING (L"Window"), GET_STRING (L"Menu"),
		GET_STRING (L"Command"), GET_STRING (L"After command"),
		GET_INTEGER (L"Depth"), GET_STRING (L"Script"))) return 0;
END

FORM (praat_hideMenuCommand, L"Hide menu command", L"Hide menu command...")
	WORD (L"Window", L"Objects")
	WORD (L"Menu", L"New")
	SENTENCE (L"Command", L"Hallo...")
	OK
DO
	if (! praat_hideMenuCommand (GET_STRING (L"Window"), GET_STRING (L"Menu"),
		GET_STRING (L"Command"))) return 0;
END

FORM (praat_showMenuCommand, L"Show menu command", L"Show menu command...")
	WORD (L"Window", L"Objects")
	WORD (L"Menu", L"New")
	SENTENCE (L"Command", L"Hallo...")
	OK
DO
	if (! praat_showMenuCommand (GET_STRING (L"Window"), GET_STRING (L"Menu"),
		GET_STRING (L"Command"))) return 0;
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
	if (! praat_addActionScript (GET_STRING (L"Class 1"), GET_INTEGER (L"Number 1"),
		GET_STRING (L"Class 2"), GET_INTEGER (L"Number 2"), GET_STRING (L"Class 3"),
		GET_INTEGER (L"Number 3"), GET_STRING (L"Command"), GET_STRING (L"After command"),
		GET_INTEGER (L"Depth"), GET_STRING (L"Script"))) return 0;
END

FORM (praat_hideAction, L"Hide action command", L"Hide action command...")
	WORD (L"Class 1", L"Sound")
	WORD (L"Class 2", L"")
	WORD (L"Class 3", L"")
	SENTENCE (L"Command", L"Play")
	OK
DO
	if (! praat_hideAction_classNames (GET_STRING (L"Class 1"),
		GET_STRING (L"Class 2"), GET_STRING (L"Class 3"), GET_STRING (L"Command"))) return 0;
END

FORM (praat_showAction, L"Show action command", L"Show action command...")
	WORD (L"Class 1", L"Sound")
	WORD (L"Class 2", L"")
	WORD (L"Class 3", L"")
	SENTENCE (L"Command", L"Play")
	OK
DO
	if (! praat_showAction_classNames (GET_STRING (L"Class 1"),
		GET_STRING (L"Class 2"), GET_STRING (L"Class 3"), GET_STRING (L"Command"))) return 0;
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
		int status = Interpreter_anyExpression (interpreter, GET_STRING (L"expression"), NULL);
		forget (interpreter);
		return status;
	} else {
		return Interpreter_anyExpression (interpreter, GET_STRING (L"expression"), NULL);
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
	MelderInfo_writeLine4 (L"Observed row 1 =    ", Melder_integer (a), L"    ", Melder_integer (b));
	MelderInfo_writeLine4 (L"Observed row 2 =    ", Melder_integer (c), L"    ", Melder_integer (d));
	aexp = (a + b) * (a + c) / n;
	bexp = (a + b) * (b + d) / n;
	cexp = (a + c) * (c + d) / n;
	dexp = (b + d) * (c + d) / n;
	MelderInfo_writeLine1 (L"");
	MelderInfo_writeLine4 (L"Expected row 1 =    ", Melder_double (aexp), L"    ", Melder_double (bexp));
	MelderInfo_writeLine4 (L"Expected row 2 =    ", Melder_double (cexp), L"    ", Melder_double (dexp));
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
	MelderInfo_writeLine1 (L"");
	MelderInfo_writeLine4 (L"Corrected observed row 1 =    ", Melder_double (a), L"    ", Melder_double (b));
	MelderInfo_writeLine4 (L"Corrected observed row 2 =    ", Melder_double (c), L"    ", Melder_double (d));
	
	n = a + b + c + d;
	crossDifference = a * d - b * c;
	x2 = n * crossDifference * crossDifference / (a + b) / (c + d) / (a + c) / (b + d);
	MelderInfo_writeLine1 (L"");
	MelderInfo_writeLine2 (L"Chi-square =    ", Melder_double (x2));
	MelderInfo_writeLine2 (L"Two-tailed p =    ", Melder_double (NUMchiSquareQ (x2, 1)));
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
			Melder_warning1 (L"These manual pages contain links to executable scripts.\n"
				"Only navigate these pages if you trust their author!");
		return 1;
	}
	if (object && Thing_member (object, classScript)) {
		ScriptEditor_createFromScript (theCurrentPraat -> topShell, NULL, (Script) object);
		forget (object);
		iferror return 0;
		return 1;
	}
	result = praat_new1 (object, MelderFile_name (file));
	praat_updateSelection ();
	return result;
}

FORM_READ (Data_readFromFile, L"Read Object(s) from file", 0)
	if (! readFromFile (file)) return 0;
END

/********** Callbacks of the Write menu. **********/

FORM_WRITE (Data_writeToTextFile, L"Write Object(s) to text file", 0, 0)
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

FORM_WRITE (Data_writeToShortTextFile, L"Write Object(s) to short text file", 0, 0)
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

FORM_WRITE (Data_writeToBinaryFile, L"Write Object(s) to binary file", 0, 0)
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

FORM_WRITE (Data_writeToLispFile, L"Write Object to LISP file", 0, L"lsp")
	return Data_writeToLispFile (ONLY_OBJECT, file);
END

/********** Callbacks of the Help menu. **********/

FORM (SearchManual, L"Search manual", L"Manual")
	LABEL (L"", L"Search for strings (separate with spaces):")
	TEXTFIELD (L"query", L"")
	OK
DO
	Manual manPage;
	if (theCurrentPraat -> batch)
		return Melder_error1 (L"Cannot view manual from batch.");
	manPage = Manual_create (theCurrentPraat -> topShell, L"Intro", theCurrentPraat -> manPages);
	Manual_search (manPage, GET_STRING (L"query"));
END

FORM (GoToManualPage, L"Go to manual page", 0)
	{long numberOfPages;
	const wchar_t **pages = ManPages_getTitles (theCurrentPraat -> manPages, & numberOfPages);
	LIST (L"Page", numberOfPages, pages, 1)}
	OK
DO
	Manual manPage;
	if (theCurrentPraat -> batch)
		return Melder_error1 (L"Cannot view manual from batch.");
	manPage = Manual_create (theCurrentPraat -> topShell, L"Intro", theCurrentPraat -> manPages);
	if (! HyperPage_goToPage_i (manPage, GET_INTEGER (L"Page"))) return 0;
END

FORM (WriteManualToHtmlDirectory, L"Write all pages as HTML files", 0)
	LABEL (L"", L"Type a directory name:")
	TEXTFIELD (L"directory", L"")
	OK
structMelderDir currentDirectory = { { 0 } };
Melder_getDefaultDir (& currentDirectory);
SET_STRING (L"directory", Melder_dirToPath (& currentDirectory))
DO
	wchar_t *directory = GET_STRING (L"directory");
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
	if (theCurrentPraat == & theForegroundPraat && theButtonEditor) Editor_dataChanged (ButtonEditor_as_Editor (theButtonEditor), NULL);
}

/********** Menu descriptions. **********/

void praat_addFixedButtons (Widget form) {
// Het is bagger, ik weet het, maar kom maar met een betere oplossing... bijvoorkeur zonder #defines
#if gtk
	Widget buttons1 = NULL, buttons2 = NULL, buttons3 = NULL;
	if (form) {
		buttons1 = gtk_hbutton_box_new ();
		buttons2 = gtk_hbutton_box_new ();
		buttons3 = gtk_hbutton_box_new ();
		gtk_button_box_set_layout (GTK_BUTTON_BOX (buttons1), GTK_BUTTONBOX_START);
		gtk_button_box_set_layout (GTK_BUTTON_BOX (buttons2), GTK_BUTTONBOX_START);
		gtk_button_box_set_layout (GTK_BUTTON_BOX (buttons3), GTK_BUTTONBOX_START);
		gtk_box_pack_end (GTK_BOX (form), buttons3, FALSE, FALSE, 0);
		gtk_box_pack_end (GTK_BOX (form), buttons2, FALSE, FALSE, 0);
		gtk_box_pack_end (GTK_BOX (form), buttons1, FALSE, FALSE, 0);
		gtk_widget_show (buttons1);
		gtk_widget_show (buttons2);
		gtk_widget_show (buttons3);
	}
	praat_addFixedButtonCommand (buttons1, L"Rename...", DO_Rename, 8, 70);
	praat_addFixedButtonCommand (buttons1, L"Copy...", DO_Copy, 92, 70);
	praat_addFixedButtonCommand (buttons2, L"Inspect", DO_Inspect, 8, 40);
	praat_addFixedButtonCommand (buttons2, L"Info", DO_Info, 92, 40);
	praat_addFixedButtonCommand (buttons3, L"Remove", DO_Remove, 8, 10);
#else
	praat_addFixedButtonCommand (form, L"Rename...", DO_Rename, 8, 70);
	praat_addFixedButtonCommand (form, L"Copy...", DO_Copy, 92, 70);
	praat_addFixedButtonCommand (form, L"Inspect", DO_Inspect, 8, 40);
	praat_addFixedButtonCommand (form, L"Info", DO_Info, 92, 40);
	praat_addFixedButtonCommand (form, L"Remove", DO_Remove, 8, 10);
#endif
}

static void searchProc (void) {
	DO_SearchManual (NULL, NULL, NULL, NULL);
}

static MelderString itemTitle_about = { 0 };

static Any scriptRecognizer (int nread, const char *header, MelderFile file) {
	wchar_t *name = MelderFile_name (file);
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
			praatMenu = GuiMenuBar_addMenu (bar ? praatP.topBar : NULL, L"\024", 0); /* Apple icon. */
		#else
			praatMenu = GuiMenuBar_addMenu (bar, L"Praat", 0);
		#endif
		newMenu = GuiMenuBar_addMenu (bar, L"New", 0);
		readMenu = GuiMenuBar_addMenu (bar, L"Read", 0);
		praat_actions_createWriteMenu (bar);
		#ifdef macintosh
			applicationHelpMenu = GuiMenuBar_addMenu (bar ? praatP.topBar : NULL, L"Help", 0);
		#endif
		helpMenu = GuiMenuBar_addMenu (bar, L"Help", 0);
	}

	MelderString_append3 (& itemTitle_about, L"About ", Melder_peekUtf8ToWcs (praatP.title), L"...");
	#ifdef macintosh
		praat_addMenuCommand (L"Objects", L"Praat", itemTitle_about.string, 0, praat_UNHIDABLE, DO_About);
	#endif
	#ifdef UNIX
		praat_addMenuCommand (L"Objects", L"Praat", itemTitle_about.string, 0, praat_UNHIDABLE, DO_About);
		praat_addMenuCommand (L"Objects", L"Praat", L"-- script --", 0, 0, 0);
	#endif
	praat_addMenuCommand (L"Objects", L"Praat", L"Debug...", 0, praat_HIDDEN, DO_praat_debug);
	praat_addMenuCommand (L"Objects", L"Praat", L"Statistics...", 0, 0, DO_Memory_info);
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
	button = praat_addMenuCommand (L"Objects", L"Praat", L"Goodies", 0, praat_UNHIDABLE, 0);

	#if gtk
		if (button) goodiesMenu = gtk_menu_item_get_submenu (GTK_MENU_ITEM (button));
	#elif motif
		if (button) XtVaGetValues (button, XmNsubMenuId, & goodiesMenu, NULL);
	#endif
	praat_addMenuCommand (L"Objects", L"Goodies", L"Calculator...", 0, 'U', DO_praat_calculator);
	praat_addMenuCommand (L"Objects", L"Goodies", L"Report difference of two proportions...", 0, 0, DO_praat_reportDifferenceOfTwoProportions);
	button = praat_addMenuCommand (L"Objects", L"Praat", L"Preferences", 0, praat_UNHIDABLE, 0);
	#if gtk
		if (button) preferencesMenu = gtk_menu_item_get_submenu (GTK_MENU_ITEM (button));
	#elif motif
		if (button) XtVaGetValues (button, XmNsubMenuId, & preferencesMenu, NULL);
	#endif
	praat_addMenuCommand (L"Objects", L"Preferences", L"Buttons...", 0, praat_UNHIDABLE, DO_praat_editButtons);   /* Cannot be hidden. */
	praat_addMenuCommand (L"Objects", L"Preferences", L"-- encoding prefs --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"Preferences", L"Text reading preferences...", 0, 0, DO_TextInputEncodingSettings);
	praat_addMenuCommand (L"Objects", L"Preferences", L"Text writing preferences...", 0, 0, DO_TextOutputEncodingSettings);

	praat_addMenuCommand (L"Objects", L"Read", L"Read from file...", 0, 'O', DO_Data_readFromFile);

	praat_addAction1 (classData, 0, L"Write to text file...", 0, 0, DO_Data_writeToTextFile);
	praat_addAction1 (classData, 0, L"Write to short text file...", 0, 0, DO_Data_writeToShortTextFile);
	praat_addAction1 (classData, 0, L"Write to binary file...", 0, 0, DO_Data_writeToBinaryFile);
}

void praat_addMenus2 (void) {
	static MelderString itemTitle_search = { 0 };
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"-- manual --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Go to manual page...", 0, 0, DO_GoToManualPage);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Write manual to HTML directory...", 0, praat_HIDDEN, DO_WriteManualToHtmlDirectory);
	MelderString_empty (& itemTitle_search);
	MelderString_append3 (& itemTitle_search, L"Search ", Melder_peekUtf8ToWcs (praatP.title), L" manual...");
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", itemTitle_search.string, 0, 'M', DO_SearchManual);
	#ifdef _WIN32
		praat_addMenuCommand (L"Objects", L"Help", L"-- about --", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"Help", itemTitle_about.string, 0, praat_UNHIDABLE, DO_About);
	#endif

	#if defined (macintosh) || defined (_WIN32)
		Gui_setOpenDocumentCallback (cb_openDocument);
	#endif
}

/* End of file praat_objectMenus.c */
