/* praat_objectMenus.cpp
 *
 * Copyright (C) 1992-2021 Paul Boersma
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

#include "praatP.h"
#include "praat_script.h"
#include "ScriptEditor.h"
#include "ButtonEditor.h"
#include "DataEditor.h"
#include "site.h"
#include "GraphicsP.h"

#define EDITOR  theCurrentPraatObjects -> list [IOBJECT]. editors

/********** Callbacks of the fixed buttons. **********/

DIRECT (PRAAT_Remove) {
	WHERE_DOWN (SELECTED)
		praat_removeObject (IOBJECT);
	praat_show ();
	END_NO_NEW_DATA
}

FORM (MODIFY_Rename, U"Rename object", U"Rename...") {
	TEXTFIELD (newName, U"New name:", U"", 3)
OK
	WHERE (SELECTED) SET_STRING (newName, NAME)
DO
	if (theCurrentPraatObjects -> totalSelection == 0)
		Melder_throw (U"Selection changed!\nNo object selected. Cannot rename.");
	if (theCurrentPraatObjects -> totalSelection > 1)
		Melder_throw (U"Selection changed!\nCannot rename more than one object at a time.");
	WHERE (SELECTED) break;
	static MelderString string;
	MelderString_copy (& string, newName);
	praat_cleanUpName (string.string);
	static MelderString fullName;
	MelderString_copy (& fullName, Thing_className (OBJECT), U" ", string.string);
	if (! str32equ (fullName.string, FULL_NAME)) {
		theCurrentPraatObjects -> list [IOBJECT]. name = Melder_dup_f (fullName.string);
		autoMelderString listName;
		MelderString_append (& listName, ID, U". ", fullName.string);
		praat_list_renameAndSelect (IOBJECT, listName.string);
		for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (EDITOR [ieditor]) Thing_setName (EDITOR [ieditor], fullName.string);
		Thing_setName (OBJECT, string.string);
	}
	END_NO_NEW_DATA
}

FORM (NEW1_Copy, U"Copy object", U"Copy...") {
	TEXTFIELD (newName, U"Name of new object:", U"", 3)
OK
	WHERE (SELECTED) SET_STRING (newName, NAME)
DO
	if (theCurrentPraatObjects -> totalSelection == 0)
		Melder_throw (U"Selection changed!\nNo object selected. Cannot copy.");
	if (theCurrentPraatObjects -> totalSelection > 1)
		Melder_throw (U"Selection changed!\nCannot copy more than one object at a time.");
	if (interpreter)
		interpreter -> returnType = kInterpreter_ReturnType::OBJECT_;
	WHERE (SELECTED)
		praat_new (Data_copy ((Daata) OBJECT), newName);
	END_WITH_NEW_DATA
}

DIRECT (INFO_Info) {
	INFO_NONE
		if (theCurrentPraatObjects -> totalSelection == 0)
			Melder_throw (U"Selection changed!\nNo object selected. Cannot query.");
		if (theCurrentPraatObjects -> totalSelection > 1)
			Melder_throw (U"Selection changed!\nCannot query more than one object at a time.");
		WHERE (SELECTED) Thing_infoWithIdAndFile (OBJECT, ID, & theCurrentPraatObjects -> list [IOBJECT]. file);
	INFO_NONE_END
}

DIRECT (WINDOW_Inspect) {
	if (theCurrentPraatObjects -> totalSelection == 0)
		Melder_throw (U"Selection changed!\nNo object selected. Cannot inspect.");
	if (theCurrentPraatApplication -> batch) {
		Melder_throw (U"Cannot inspect data from batch.");
	} else {
		WHERE (SELECTED) {
			autoDataEditor editor = DataEditor_create (ID_AND_FULL_NAME, OBJECT);
			praat_installEditor (editor.get(), IOBJECT);
			editor.releaseToUser();
		}
	}
	END_NO_NEW_DATA
}

/********** The fixed menus. **********/

static GuiMenu praatMenu, editMenu, windowMenu, newMenu, readMenu, goodiesMenu, preferencesMenu, technicalMenu, applicationHelpMenu, helpMenu;

GuiMenu praat_objects_resolveMenu (conststring32 menu) {
	return
		str32equ (menu, U"Praat") || str32equ (menu, U"Control") ? praatMenu :
		#if cocoa
			str32equ (menu, U"Edit") ? editMenu :
			str32equ (menu, U"Window") ? windowMenu :
		#endif
		str32equ (menu, U"New") || str32equ (menu, U"Create") ? newMenu :
		str32equ (menu, U"Open") || str32equ (menu, U"Read") ? readMenu :
		str32equ (menu, U"Help") ? helpMenu :
		str32equ (menu, U"Goodies") ? goodiesMenu :
		str32equ (menu, U"Preferences") ? preferencesMenu :
		str32equ (menu, U"Technical") ? technicalMenu :
		#ifdef macintosh
			str32equ (menu, U"ApplicationHelp") ? applicationHelpMenu :
		#else
			str32equ (menu, U"ApplicationHelp") ? helpMenu :
		#endif
		newMenu;   // default
}

/********** Callbacks of the Praat menu. **********/

DIRECT (WINDOW_About) {
	praat_showLogo ();
	END_NO_NEW_DATA
}

DIRECT (WINDOW_praat_newScript) {
	autoScriptEditor editor = ScriptEditor_createFromText (nullptr, nullptr);
	editor.releaseToUser();
	END_NO_NEW_DATA
}

DIRECT (WINDOW_praat_openScript) {
	autoScriptEditor editor = ScriptEditor_createFromText (nullptr, nullptr);
	TextEditor_showOpen (editor.get());
	editor.releaseToUser();
	END_NO_NEW_DATA
}

static ButtonEditor theReferenceToTheOnlyButtonEditor;

static void cb_ButtonEditor_destruction (Editor /* editor */) {
	theReferenceToTheOnlyButtonEditor = nullptr;
}

DIRECT (WINDOW_praat_editButtons) {
	if (theReferenceToTheOnlyButtonEditor) {
		Editor_raise (theReferenceToTheOnlyButtonEditor);
	} else {
		autoButtonEditor editor = ButtonEditor_create ();
		Editor_setDestructionCallback (editor.get(), cb_ButtonEditor_destruction);
		theReferenceToTheOnlyButtonEditor = editor.get();
		editor.releaseToUser();
	}
	END_NO_NEW_DATA
}

FORM (PRAAT_addMenuCommand, U"Add menu command", U"Add menu command...") {
	WORD (window, U"Window", U"Objects")
	WORD (menu, U"Menu", U"New")
	SENTENCE (command, U"Command", U"Hallo...")
	SENTENCE (afterCommand, U"After command", U"")
	INTEGER (depth, U"Depth", U"0")
	INFILE (script, U"Script file:", U"/u/miep/hallo.praat")
	OK
DO
	praat_addMenuCommandScript (window, menu, command, afterCommand, depth, script);
	END_NO_NEW_DATA
}

FORM (PRAAT_hideMenuCommand, U"Hide menu command", U"Hide menu command...") {
	WORD (window, U"Window", U"Objects")
	WORD (menu, U"Menu", U"New")
	SENTENCE (command, U"Command", U"Hallo...")
	OK
DO
	praat_hideMenuCommand (window, menu, command);
	END_NO_NEW_DATA
}

FORM (PRAAT_showMenuCommand, U"Show menu command", U"Show menu command...") {
	WORD (window, U"Window", U"Objects")
	WORD (menu, U"Menu", U"New")
	SENTENCE (command, U"Command", U"Hallo...")
	OK
DO
	praat_showMenuCommand (window, menu, command);
	END_NO_NEW_DATA
}

FORM (PRAAT_addAction, U"Add action command", U"Add action command...") {
	WORD (class1, U"Class 1", U"Sound")
	INTEGER (number1, U"Number 1", U"0")
	WORD (class2, U"Class 2", U"")
	INTEGER (number2, U"Number 2", U"0")
	WORD (class3, U"Class 3", U"")
	INTEGER (number3, U"Number 3", U"0")
	SENTENCE (command, U"Command", U"Play reverse")
	SENTENCE (afterCommand, U"After command", U"Play")
	INTEGER (depth, U"Depth", U"0")
	INFILE (script, U"Script file:", U"/u/miep/playReverse.praat")
	OK
DO
	praat_addActionScript (class1, number1, class2, number2, class3, number3, command, afterCommand, depth, script);
	END_NO_NEW_DATA
}

FORM (PRAAT_hideAction, U"Hide action command", U"Hide action command...") {
	WORD (class1, U"Class 1", U"Sound")
	WORD (class2, U"Class 2", U"")
	WORD (class3, U"Class 3", U"")
	SENTENCE (command, U"Command", U"Play")
	OK
DO
	praat_hideAction_classNames (class1, class2, class3, command);
	END_NO_NEW_DATA
}

FORM (PRAAT_showAction, U"Show action command", U"Show action command...") {
	WORD (class1, U"Class 1", U"Sound")
	WORD (class2, U"Class 2", U"")
	WORD (class3, U"Class 3", U"")
	SENTENCE (command, U"Command", U"Play")
	OK
DO
	praat_showAction_classNames (class1, class2, class3, command);
	END_NO_NEW_DATA
}

/********** Callbacks of the Preferences menu. **********/

FORM (PREFS_TextInputEncodingSettings, U"Text reading preferences", U"Unicode") {
	RADIO_ENUM (kMelder_textInputEncoding, encodingOf8BitTextFiles,
			U"Encoding of 8-bit text files", kMelder_textInputEncoding::DEFAULT)
OK
	SET_ENUM (encodingOf8BitTextFiles, kMelder_textInputEncoding, Melder_getInputEncoding ())
DO
	Melder_setInputEncoding (encodingOf8BitTextFiles);
	END_NO_NEW_DATA
}

FORM (PREFS_TextOutputEncodingSettings, U"Text writing preferences", U"Unicode") {
	RADIO_ENUM (kMelder_textOutputEncoding, outputEncoding,
			U"Output encoding", kMelder_textOutputEncoding::DEFAULT)
OK
	SET_ENUM (outputEncoding, kMelder_textOutputEncoding, Melder_getOutputEncoding ())
DO
	Melder_setOutputEncoding (outputEncoding);
	END_NO_NEW_DATA
}

FORM (PREFS_GraphicsCjkFontStyleSettings, U"CJK font style preferences", nullptr) {
	OPTIONMENU_ENUM (kGraphics_cjkFontStyle, cjkFontStyle,
			U"CJK font style", kGraphics_cjkFontStyle::DEFAULT)
OK
	SET_ENUM (cjkFontStyle, kGraphics_cjkFontStyle, theGraphicsCjkFontStyle)
DO
	theGraphicsCjkFontStyle = cjkFontStyle;
	END_NO_NEW_DATA
}

/********** Callbacks of the Goodies menu. **********/

FORM (STRING_praat_calculator, U"Calculator", U"Calculator") {
	LABEL (U"Type any numeric formula or string formula:")
	TEXTFIELD (expression, nullptr, U"5*5", 5)
	LABEL (U"Note that you can include many special functions in your formula,")
	LABEL (U"including statistical functions and acoustics-auditory conversions.")
	LABEL (U"For details, click Help.")
	OK
DO
	INFO_NONE
		Formula_Result result;
		if (! interpreter) {
			autoInterpreter tempInterpreter = Interpreter_create (nullptr, nullptr);
			Interpreter_anyExpression (tempInterpreter.get(), expression, & result);
		} else {
			Interpreter_anyExpression (interpreter, expression, & result);
		}
		switch (result. expressionType) {
			case kFormula_EXPRESSION_TYPE_NUMERIC:
				Melder_information (result. numericResult);
			break; case kFormula_EXPRESSION_TYPE_STRING:
				Melder_information (result. stringResult.get());
			break; case kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR:
				Melder_information (constVECVU (result. numericVectorResult));
			break; case kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX:
				Melder_information (constMATVU (result. numericMatrixResult));
			break; case kFormula_EXPRESSION_TYPE_STRING_ARRAY:
				Melder_information (result. stringArrayResult);
		}
	INFO_NONE_END
}

FORM (INFO_reportDifferenceOfTwoProportions, U"Report difference of two proportions", U"Difference of two proportions") {
	INTEGER (a_int, U"left Row 1", U"71")
	INTEGER (b_int, U"right Row 1", U"39")
	INTEGER (c_int, U"left Row 2", U"93")
	INTEGER (d_int, U"right Row 2", U"27")
	OK
DO
	INFO_NONE
		double a = a_int, b = b_int, c = c_int, d = d_int;
		double n = a + b + c + d;
		if (a < 0 || b < 0 || c < 0 || d < 0)
			Melder_throw (U"The numbers should not be negative.");
		if (a + b <= 0 || c + d <= 0)
			Melder_throw (U"The row totals should be positive.");
		if (a + c <= 0 || b + d <= 0)
			Melder_throw (U"The column totals should be positive.");
		MelderInfo_open ();
		MelderInfo_writeLine (U"Observed row 1 =    ", Melder_iround (a), U"    ", Melder_iround (b));
		MelderInfo_writeLine (U"Observed row 2 =    ", Melder_iround (c), U"    ", Melder_iround (d));
		double aexp = (a + b) * (a + c) / n;
		double bexp = (a + b) * (b + d) / n;
		double cexp = (a + c) * (c + d) / n;
		double dexp = (b + d) * (c + d) / n;
		MelderInfo_writeLine (U"");
		MelderInfo_writeLine (U"Expected row 1 =    ", aexp, U"    ", bexp);
		MelderInfo_writeLine (U"Expected row 2 =    ", cexp, U"    ", dexp);
		/*
			Continuity correction:
			bring the observed numbers closer to the expected numbers by 0.5 (if possible).
		*/
		Melder_moveCloserToBy (& a, aexp, 0.5);
		Melder_moveCloserToBy (& b, bexp, 0.5);
		Melder_moveCloserToBy (& c, cexp, 0.5);
		Melder_moveCloserToBy (& d, dexp, 0.5);
		MelderInfo_writeLine (U"");
		MelderInfo_writeLine (U"Corrected observed row 1 =    ", a, U"    ", b);
		MelderInfo_writeLine (U"Corrected observed row 2 =    ", c, U"    ", d);

		n = a + b + c + d;
		double crossDifference = a * d - b * c;
		double x2 = n * crossDifference * crossDifference / (a + b) / (c + d) / (a + c) / (b + d);
		MelderInfo_writeLine (U"");
		MelderInfo_writeLine (U"Chi-square =    ", x2);
		MelderInfo_writeLine (U"Two-tailed p =    ", NUMchiSquareQ (x2, 1));
		MelderInfo_close ();
	INFO_NONE_END
}

/********** Callbacks of the Technical menu. **********/

FORM (PRAAT_debug, U"Set debugging options", nullptr) {
	LABEL (U"If you switch Tracing on, Praat will write lots of detailed ")
	LABEL (U"information about what goes on in Praat")
	structMelderDir dir;
	Melder_getPrefDir (& dir);
	structMelderFile file;
	#ifdef UNIX
		MelderDir_getFile (& dir, U"tracing", & file);
	#else
		MelderDir_getFile (& dir, U"Tracing.txt", & file);
	#endif
	LABEL (Melder_cat (U"to ", Melder_fileToPath (& file), U"."))
	BOOLEAN (tracing, U"Tracing", false)
	LABEL (U"Setting the following to anything other than zero")
	LABEL (U"will alter the behaviour of Praat")
	LABEL (U"in unpredictable ways.")
	INTEGER (debugOption, U"Debug option", U"0")
OK
	SET_BOOLEAN (tracing, Melder_isTracing)
	SET_INTEGER (debugOption, Melder_debug)
DO
	Melder_setTracing (tracing);
	Melder_debug = debugOption;
	END_NO_NEW_DATA
}

DIRECT (INFO_listReadableTypesOfObjects) {
	INFO_NONE
		Thing_listReadableClasses ();
	INFO_NONE_END
}

FORM (INFO_praat_library_createC, U"PraatLib: Create C header or file", nullptr) {
	BOOLEAN (isInHeader, U"Is in header", true)
	BOOLEAN (includeCreateAPI, U"Include \"Create\" API", true)
	BOOLEAN (includeReadAPI, U"Include \"Read\" API", true)
	BOOLEAN (includeSaveAPI, U"Include \"Save\" API", true)
	BOOLEAN (includeQueryAPI, U"Include \"Query\" API", true)
	BOOLEAN (includeModifyAPI, U"Include \"Modify\" API", true)
	BOOLEAN (includeToAPI, U"Include \"To\" API", true)
	BOOLEAN (includeRecordAPI, U"Include \"Record\" API", true)
	BOOLEAN (includePlayAPI, U"Include \"Play\" API", true)
	BOOLEAN (includeDrawAPI, U"Include \"Draw\" API", true)
	BOOLEAN (includeHelpAPI, U"Include \"Help\" API", false)
	BOOLEAN (includeWindowAPI, U"Include \"Window\" API", false)
	BOOLEAN (includeDemoAPI, U"Include \"Demo\" API", false)
	OK
DO
	praat_library_createC (isInHeader, includeCreateAPI, includeReadAPI, includeSaveAPI,
		includeQueryAPI, includeModifyAPI, includeToAPI, includeRecordAPI, includePlayAPI,
		includeDrawAPI, includeHelpAPI, includeWindowAPI, includeDemoAPI);
	END_NO_NEW_DATA
}

DIRECT (INFO_reportSystemProperties) {
	INFO_NONE
		praat_reportSystemProperties ();
	INFO_NONE_END
}

DIRECT (INFO_reportGraphicalProperties) {
	INFO_NONE
		praat_reportGraphicalProperties ();
	INFO_NONE_END
}

DIRECT (INFO_reportIntegerProperties) {
	INFO_NONE
		praat_reportIntegerProperties ();
	INFO_NONE_END
}

DIRECT (INFO_reportMemoryUse) {
	INFO_NONE
		praat_reportMemoryUse ();
	INFO_NONE_END
}

DIRECT (INFO_reportTextProperties) {
	INFO_NONE
		praat_reportTextProperties ();
	INFO_NONE_END
}

DIRECT (INFO_reportFontProperties) {
	INFO_NONE
		praat_reportFontProperties ();
	INFO_NONE_END
}

/********** Callbacks of the Open menu. **********/

/*
	Note: readFromFile should not call praat_updateSelection(),
	because praat_updateSelection() should be called after all files have been read,
	not just the current one.
*/
static void readFromFile (MelderFile file) {
	autoDaata object = Data_readFromFile (file);
	Melder_assert (object);   // if the object was not created, there should have been an exception
	if (Thing_isa (object.get(), classManPages) && ! Melder_batch) {
		ManPages manPages = (ManPages) object.get();
		ManPage firstPage = manPages -> pages.at [1];
		autoManual manual = Manual_create (firstPage -> title.get(), object.releaseToAmbiguousOwner(), true);
		if (manPages -> executable)
			Melder_warning (U"These manual pages contain links to executable scripts.\n"
				"Only navigate these pages if you trust their author!");
		manual.releaseToUser();
		return;
	}
	if (Thing_isa (object.get(), classScript) && ! Melder_batch) {
		autoScriptEditor editor = ScriptEditor_createFromScript_canBeNull (nullptr, (Script) object.get());
		if (! editor) {
			(void) 0;   // the script was already open, and the user has been notified of that
		} else {
			editor.releaseToUser();
		}
		return;
	}
	praat_newWithFile (object.move(), file, MelderFile_name (file));
}

FORM_READ (READMANY_Data_readFromFile, U"Read Object(s) from file", 0, true) {
	readFromFile (file);
	if (interpreter)
		interpreter -> returnType = kInterpreter_ReturnType::OBJECT_;
	END_WITH_NEW_DATA   // this calls praat_updateSelection(), after reading a single file; see also cb_openDocument
}

/********** Callbacks of the Save menu. **********/

FORM_SAVE (SAVE_Data_writeToTextFile, U"Save Object(s) as one text file", nullptr, nullptr) {
	if (theCurrentPraatObjects -> totalSelection == 1) {
		LOOP {
			iam_LOOP (Daata);
			Data_writeToTextFile (me, file);
		}
	} else {
		autoCollection set = praat_getSelectedObjects ();
		Data_writeToTextFile (set.get(), file);
	}
	END_NO_NEW_DATA
}

FORM_SAVE (SAVE_Data_writeToShortTextFile, U"Save Object(s) as one short text file", nullptr, nullptr) {
	if (theCurrentPraatObjects -> totalSelection == 1) {
		LOOP {
			iam_LOOP (Daata);
			Data_writeToShortTextFile (me, file);
		}
	} else {
		autoCollection set = praat_getSelectedObjects ();
		Data_writeToShortTextFile (set.get(), file);
	}
	END_NO_NEW_DATA
}

FORM_SAVE (SAVE_Data_writeToBinaryFile, U"Save Object(s) as one binary file", nullptr, nullptr) {
	if (theCurrentPraatObjects -> totalSelection == 1) {
		LOOP {
			iam_LOOP (Daata);
			Data_writeToBinaryFile (me, file);
		}
	} else {
		autoCollection set = praat_getSelectedObjects ();
		Data_writeToBinaryFile (set.get(), file);
	}
	END_NO_NEW_DATA
}

FORM (PRAAT_ManPages_saveToHtmlFolder, U"Save all pages as HTML files", nullptr) {
	FOLDER (folder, U"Folder:", U"")
OK
	LOOP {
		iam_LOOP (ManPages);
		SET_STRING (folder, Melder_dirToPath (& my rootDirectory))
	}
DO
	LOOP {
		iam_LOOP (ManPages);
		ManPages_writeAllToHtmlDir (me, folder);
	}
	END_NO_NEW_DATA
}


DIRECT (WINDOW_ManPages_view) {
	LOOP {
		iam_LOOP (ManPages);
		ManPage firstPage = my pages.at [1];
		autoManual manual = Manual_create (firstPage -> title.get(), me, false);
		if (my executable)
			Melder_warning (U"These manual pages contain links to executable scripts.\n"
				"Only navigate these pages if you trust their author!");
		praat_installEditor (manual.get(), IOBJECT);
		manual.releaseToUser();
	}
	END_NO_NEW_DATA
}

/********** Callbacks of the Help menu. **********/

FORM (HELP_SearchManual, U"Search manual", U"Manual") {
	TEXTFIELD (query, U"Search for strings (separate with spaces):", U"", 3)
	OK
DO
	if (theCurrentPraatApplication -> batch)
		Melder_throw (U"Cannot view a manual from batch.");
	autoManual manual = Manual_create (U"Intro", theCurrentPraatApplication -> manPages, false);
	Manual_search (manual.get(), query);
	manual.releaseToUser();
	END_NO_NEW_DATA
}

FORM (HELP_GoToManualPage, U"Go to manual page", nullptr) {
	static constSTRVEC pages;
	pages = ManPages_getTitles (theCurrentPraatApplication -> manPages);
	LIST (goToPageNumber, U"Page", pages, 1)
	OK
DO
	if (theCurrentPraatApplication -> batch)
		Melder_throw (U"Cannot view a manual from batch.");
	autoManual manual = Manual_create (U"Intro", theCurrentPraatApplication -> manPages, false);
	HyperPage_goToPage_number (manual.get(), goToPageNumber);
	manual.releaseToUser();
	END_NO_NEW_DATA
}

FORM (HELP_SaveManualToHtmlFolder, U"Save all pages as HTML files", nullptr) {
	FOLDER (folder, U"Folder:", U"")
OK
	structMelderDir currentDirectory { };
	Melder_getDefaultDir (& currentDirectory);
	SET_STRING (folder, Melder_dirToPath (& currentDirectory))
DO
	ManPages_writeAllToHtmlDir (theCurrentPraatApplication -> manPages, folder);
	END_NO_NEW_DATA
}

/********** Menu descriptions. **********/

void praat_show () {
	/*
		(De)sensitivize the fixed buttons as appropriate for the current selection.
	*/
	praat_sensitivizeFixedButtonCommand (U"Remove", theCurrentPraatObjects -> totalSelection != 0);
	praat_sensitivizeFixedButtonCommand (U"Rename...", theCurrentPraatObjects -> totalSelection == 1);
	praat_sensitivizeFixedButtonCommand (U"Copy...", theCurrentPraatObjects -> totalSelection == 1);
	praat_sensitivizeFixedButtonCommand (U"Info", theCurrentPraatObjects -> totalSelection == 1);
	praat_sensitivizeFixedButtonCommand (U"Inspect", theCurrentPraatObjects -> totalSelection != 0);
	praat_actions_show ();
	if (theCurrentPraatApplication == & theForegroundPraatApplication && theReferenceToTheOnlyButtonEditor)
		Editor_dataChanged (theReferenceToTheOnlyButtonEditor);
}

/********** Menu descriptions. **********/

void praat_addFixedButtons (GuiWindow window) {
	praat_addFixedButtonCommand (window, U"Rename...", MODIFY_Rename, 8, 70);
	praat_addFixedButtonCommand (window, U"Copy...", NEW1_Copy, 98, 70);
	praat_addFixedButtonCommand (window, U"Inspect", WINDOW_Inspect, 8, 40);
	praat_addFixedButtonCommand (window, U"Info", INFO_Info, 98, 40);
	praat_addFixedButtonCommand (window, U"Remove", PRAAT_Remove, 8, 10);
}

static void searchProc () {
	HELP_SearchManual (nullptr, 0, nullptr, nullptr, nullptr, nullptr, false, nullptr);
}

static MelderString itemTitle_about;

static autoDaata scriptRecognizer (integer nread, const char *header, MelderFile file) {
	conststring32 name = MelderFile_name (file);
	if (nread < 2)
		return autoDaata ();
	if ((header [0] == '#' && header [1] == '!')
		|| Melder_stringMatchesCriterion (name, kMelder_string::ENDS_WITH, U".praat", false)
	    || Melder_stringMatchesCriterion (name, kMelder_string::ENDS_WITH, U".html", false)
	) {
		return Script_createFromFile (file);
	}
	return autoDaata ();
}

static void cb_openDocument (MelderFile file) {
	try {
		readFromFile (file);   // read a single file without calling praat_updateSelection()
	} catch (MelderError) {
		Melder_flushError ();
	}
}
static void cb_finishedOpeningDocuments () {
	try {
		praat_updateSelection ();   // this finally calls praat_updateSelection(), after each separate file has been read
	} catch (MelderError) {
		Melder_flushError ();
	}
}

#if cocoa
DIRECT (PRAAT_cut) {
	[[[NSApp keyWindow] firstResponder] cut: nil];
	END_NO_NEW_DATA
}
DIRECT (PRAAT_copy) {
	[[[NSApp keyWindow] firstResponder] copy: nil];
	END_NO_NEW_DATA
}
DIRECT (PRAAT_paste) {
	[[[NSApp keyWindow] firstResponder] pasteAsPlainText: nil];
	END_NO_NEW_DATA
}
DIRECT (PRAAT_minimize) {
	[[NSApp keyWindow] performMiniaturize: nil];
	END_NO_NEW_DATA
}
DIRECT (PRAAT_zoom) {
	[[NSApp keyWindow] performZoom: nil];
	END_NO_NEW_DATA
}
DIRECT (PRAAT_close) {
	[[NSApp keyWindow] performClose: nil];
	END_NO_NEW_DATA
}
#endif

void praat_addMenus (GuiWindow window) {
	Melder_setSearchProc (searchProc);

	Data_recognizeFileType (scriptRecognizer);

	/*
		Create the menu titles in the bar.
	*/
	if (! theCurrentPraatApplication -> batch) {
		#ifdef macintosh
			praatMenu = GuiMenu_createInWindow (nullptr, U"\024", 0);
			#if cocoa
				editMenu = GuiMenu_createInWindow (nullptr, U"Edit", 0);
				windowMenu = GuiMenu_createInWindow (nullptr, U"Window", 0);
			#endif
		#else
			praatMenu = GuiMenu_createInWindow (window, U"Praat", 0);
		#endif
		newMenu = GuiMenu_createInWindow (window, U"New", 0);
		readMenu = GuiMenu_createInWindow (window, U"Open", 0);
		praat_actions_createWriteMenu (window);
		#ifdef macintosh
			applicationHelpMenu = GuiMenu_createInWindow (nullptr, U"Help", 0);
		#endif
		helpMenu = GuiMenu_createInWindow (window, U"Help", 0);
	}
	
	MelderString_append (& itemTitle_about, U"About ", praatP.title.get());
	praat_addMenuCommand (U"Objects", U"Praat", itemTitle_about.string, nullptr, praat_UNHIDABLE, WINDOW_About);
	#ifdef macintosh
		#if cocoa
			/*
				HACK: give the following commands weird names,
				because otherwise they may be called from a script.
				(we add three alt-spaces)
			*/
			praat_addMenuCommand (U"Objects", U"Edit", U"Cut   ", nullptr, praat_UNHIDABLE | 'X' | praat_NO_API, PRAAT_cut);
			praat_addMenuCommand (U"Objects", U"Edit", U"Copy   ", nullptr, praat_UNHIDABLE | 'C' | praat_NO_API, PRAAT_copy);
			praat_addMenuCommand (U"Objects", U"Edit", U"Paste   ", nullptr, praat_UNHIDABLE | 'V' | praat_NO_API, PRAAT_paste);
			praat_addMenuCommand (U"Objects", U"Window", U"Minimize   ", nullptr, praat_UNHIDABLE | praat_NO_API, PRAAT_minimize);
			praat_addMenuCommand (U"Objects", U"Window", U"Zoom   ", nullptr, praat_UNHIDABLE | praat_NO_API, PRAAT_zoom);
			praat_addMenuCommand (U"Objects", U"Window", U"Close   ", nullptr, 'W' | praat_NO_API, PRAAT_close);
		#endif
	#endif
	praat_addMenuCommand (U"Objects", U"Praat", U"-- script --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Praat", U"New Praat script", nullptr, praat_NO_API, WINDOW_praat_newScript);
	praat_addMenuCommand (U"Objects", U"Praat", U"Open Praat script...", nullptr, praat_NO_API, WINDOW_praat_openScript);
	praat_addMenuCommand (U"Objects", U"Praat", U"-- buttons --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Praat", U"Add menu command...", nullptr, praat_HIDDEN | praat_NO_API, PRAAT_addMenuCommand);
	praat_addMenuCommand (U"Objects", U"Praat", U"Hide menu command...", nullptr, praat_HIDDEN | praat_NO_API, PRAAT_hideMenuCommand);
	praat_addMenuCommand (U"Objects", U"Praat", U"Show menu command...", nullptr, praat_HIDDEN | praat_NO_API, PRAAT_showMenuCommand);
	praat_addMenuCommand (U"Objects", U"Praat", U"Add action command...", nullptr, praat_HIDDEN | praat_NO_API, PRAAT_addAction);
	praat_addMenuCommand (U"Objects", U"Praat", U"Hide action command...", nullptr, praat_HIDDEN | praat_NO_API, PRAAT_hideAction);
	praat_addMenuCommand (U"Objects", U"Praat", U"Show action command...", nullptr, praat_HIDDEN | praat_NO_API, PRAAT_showAction);

	GuiMenuItem menuItem = praat_addMenuCommand (U"Objects", U"Praat", U"Goodies", nullptr, praat_UNHIDABLE, nullptr);
	goodiesMenu = menuItem ? menuItem -> d_menu : nullptr;
	praat_addMenuCommand (U"Objects", U"Goodies", U"Calculator...", nullptr, 'U', STRING_praat_calculator);
	praat_addMenuCommand (U"Objects", U"Goodies", U"Report difference of two proportions...", nullptr, 0, INFO_reportDifferenceOfTwoProportions);

	menuItem = praat_addMenuCommand (U"Objects", U"Praat", U"Preferences", nullptr, praat_UNHIDABLE, nullptr);
	preferencesMenu = menuItem ? menuItem -> d_menu : nullptr;
	praat_addMenuCommand (U"Objects", U"Preferences", U"Buttons...", nullptr, praat_UNHIDABLE, WINDOW_praat_editButtons);
	praat_addMenuCommand (U"Objects", U"Preferences", U"-- encoding prefs --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Preferences", U"Text reading preferences...", nullptr, 0, PREFS_TextInputEncodingSettings);
	praat_addMenuCommand (U"Objects", U"Preferences", U"Text writing preferences...", nullptr, 0, PREFS_TextOutputEncodingSettings);
	praat_addMenuCommand (U"Objects", U"Preferences", U"CJK font style preferences...", nullptr, 0, PREFS_GraphicsCjkFontStyleSettings);

	menuItem = praat_addMenuCommand (U"Objects", U"Praat", U"Technical", nullptr, praat_UNHIDABLE, nullptr);
	technicalMenu = menuItem ? menuItem -> d_menu : nullptr;
	praat_addMenuCommand (U"Objects", U"Technical", U"Report memory use", nullptr, 0, INFO_reportMemoryUse);
	praat_addMenuCommand (U"Objects", U"Technical", U"Report integer properties", nullptr, 0, INFO_reportIntegerProperties);
	praat_addMenuCommand (U"Objects", U"Technical", U"Report system properties", nullptr, 0, INFO_reportSystemProperties);
	praat_addMenuCommand (U"Objects", U"Technical", U"Report graphical properties", nullptr, 0, INFO_reportGraphicalProperties);
	praat_addMenuCommand (U"Objects", U"Technical", U"Report text properties", nullptr, 0, INFO_reportTextProperties);
	praat_addMenuCommand (U"Objects", U"Technical", U"Report font properties", nullptr, 0, INFO_reportFontProperties);
	praat_addMenuCommand (U"Objects", U"Technical", U"Debug...", nullptr, 0, PRAAT_debug);
	praat_addMenuCommand (U"Objects", U"Technical", U"-- api --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Technical", U"List readable types of objects", nullptr, 0, INFO_listReadableTypesOfObjects);
	praat_addMenuCommand (U"Objects", U"Technical", U"Create C interface...", nullptr, 0, INFO_praat_library_createC);

	praat_addMenuCommand (U"Objects", U"Open", U"Read from file...", nullptr, praat_ATTRACTIVE | 'O', READMANY_Data_readFromFile);

	praat_addAction1 (classDaata, 0, U"Save as text file...", nullptr, 0, SAVE_Data_writeToTextFile);
	praat_addAction1 (classDaata, 0,   U"Write to text file...", nullptr, praat_DEPRECATED_2011, SAVE_Data_writeToTextFile);
	praat_addAction1 (classDaata, 0, U"Save as short text file...", nullptr, 0, SAVE_Data_writeToShortTextFile);
	praat_addAction1 (classDaata, 0,   U"Write to short text file...", nullptr, praat_DEPRECATED_2011, SAVE_Data_writeToShortTextFile);
	praat_addAction1 (classDaata, 0, U"Save as binary file...", nullptr, 0, SAVE_Data_writeToBinaryFile);
	praat_addAction1 (classDaata, 0,   U"Write to binary file...", nullptr, praat_DEPRECATED_2011, SAVE_Data_writeToBinaryFile);

	praat_addAction1 (classManPages, 1, U"Save to HTML folder...", nullptr, 0, PRAAT_ManPages_saveToHtmlFolder);
	praat_addAction1 (classManPages, 1, U"Save to HTML directory...", nullptr, praat_DEPRECATED_2020, PRAAT_ManPages_saveToHtmlFolder);
	praat_addAction1 (classManPages, 1, U"View", nullptr, 0, WINDOW_ManPages_view);
}

void praat_addMenus2 () {
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"-- manual --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Go to manual page...", nullptr, 0, HELP_GoToManualPage);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Save manual to HTML folder...", nullptr, praat_HIDDEN, HELP_SaveManualToHtmlFolder);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp",
		Melder_cat (U"Search ", praatP.title.get(), U" manual..."),
		nullptr, 'M' | praat_NO_API, HELP_SearchManual);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"-- about --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", itemTitle_about.string, nullptr, praat_UNHIDABLE, WINDOW_About);

	#if defined (macintosh) || defined (_WIN32)
		Gui_setOpenDocumentCallback (cb_openDocument, cb_finishedOpeningDocuments);
	#endif
}

/* End of file praat_objectMenus.cpp */
