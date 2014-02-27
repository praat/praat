/* praat_script.cpp
 *
 * Copyright (C) 1993-2012,2013,2014 Paul Boersma
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
#include "sendpraat.h"
#include "sendsocket.h"
#include "UiPause.h"
#include "DemoEditor.h"

static int praat_findObjectFromString (Interpreter interpreter, const wchar_t *string) {
	try {
		int IOBJECT;
		while (*string == ' ') string ++;
		if (*string >= 'A' && *string <= 'Z') {
			/*
			 * Find the object by its name.
			 */
			static MelderString buffer = { 0 };
			MelderString_copy (& buffer, string);
			wchar_t *space = wcschr (buffer.string, ' ');
			if (space == NULL)
				Melder_throw ("Missing space in name.");
			*space = '\0';
			wchar_t *className = & buffer.string [0], *givenName = space + 1;
			WHERE_DOWN (1) {
				Data object = (Data) OBJECT;
				if (wcsequ (className, Thing_className ((Thing) OBJECT)) && wcsequ (givenName, object -> name))
					return IOBJECT;
			}
			Melder_throw ("No object with that name.");
		} else {
			/*
			 * Find the object by its ID.
			 */
			double value;
			Interpreter_numericExpression (interpreter, string, & value);
			long id = (long) value;
			WHERE (ID == id)
				return IOBJECT;
			Melder_throw ("No object with number ", id, ".");
		}
	} catch (MelderError) {
		Melder_throw ("Object \"", string, L"\" does not exist.");
	}
}

Editor praat_findEditorFromString (const wchar_t *string) {
	int IOBJECT;
	while (*string == ' ') string ++;
	if (*string >= 'A' && *string <= 'Z') {
		WHERE_DOWN (1) {
			for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor editor = (Editor) theCurrentPraatObjects -> list [IOBJECT]. editors [ieditor];
				if (editor != NULL) {
					const wchar_t *name = wcschr (editor -> name, ' ') + 1;
					if (wcsequ (name, string)) return editor;
				}
			}
		}
	} else {
		WHERE_DOWN (1) {
			for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor editor = (Editor) theCurrentPraatObjects -> list [IOBJECT]. editors [ieditor];
				if (editor && wcsequ (editor -> name, string)) return editor;
			}
		}
	}
	Melder_throw ("Editor \"", string, "\" does not exist.");
}

static int parseCommaSeparatedArguments (Interpreter interpreter, wchar_t *arguments, structStackel args []) {
	int narg = 0, depth = 0;
	for (wchar_t *p = arguments; ; p ++) {
		bool endOfArguments = *p == '\0';
		if (endOfArguments || (*p == ',' && depth == 0)) {
			if (narg == MAXIMUM_NUMBER_OF_FIELDS)
				Melder_throw ("Cannot have more than ", MAXIMUM_NUMBER_OF_FIELDS, " arguments");
			*p = '\0';
			struct Formula_Result result;
			Interpreter_anyExpression (interpreter, arguments, & result);
			narg ++;
			/*
			 * First remove the old contents.
			 */
			switch (args [narg]. which) {
				case Stackel_NUMBER: {
					// do nothing
				} break;
				case Stackel_STRING: {
					Melder_free (args [narg].string);
				} break;
			}
			/*
			 * Then copy in the new contents.
			 */
			switch (result. expressionType) {
				case kFormula_EXPRESSION_TYPE_NUMERIC: {
					args [narg]. which = Stackel_NUMBER;
					args [narg]. number = result. result. numericResult;
				} break;
				case kFormula_EXPRESSION_TYPE_STRING: {
					args [narg]. which = Stackel_STRING;
					args [narg]. string = result. result. stringResult;
				} break;
			}
			arguments = p + 1;
		} else if (*p == '(' || *p == '[' || *p == '{') {
			depth ++;
		} else if (*p == ')' || *p == ']' || *p == '}') {
			depth --;
		} else if (*p == '\"') {
			for (;;) {
				p ++;
				if (*p == '\"') {
					if (p [1] == '\"') p ++;
					else break;
				}
			}
		}
		if (endOfArguments) break;
	}
	return narg;
}

int praat_executeCommand (Interpreter interpreter, wchar_t *command) {
	static struct structStackel args [1 + MAXIMUM_NUMBER_OF_FIELDS];
	//Melder_casual ("praat_executeCommand: %ld: %ls", interpreter, command);
	if (command [0] == '\0' || command [0] == '#' || command [0] == '!' || command [0] == ';')
		/* Skip empty lines and comments. */;
	else if ((command [0] == '.' || command [0] == '+' || command [0] == '-') && isupper (command [1])) {   // selection?
		int IOBJECT = praat_findObjectFromString (interpreter, command + 1);
		if (command [0] == '.') praat_deselectAll ();
		if (command [0] == '-') praat_deselect (IOBJECT); else praat_select (IOBJECT); 
		praat_show ();
	} else if (islower (command [0])) {   // all directives start with a lower-case letter
		if (wcsnequ (command, L"select ", 7)) {
			if (wcsnequ (command + 7, L"all", 3) && (command [10] == '\0' || command [10] == ' ' || command [10] == '\t')) {
				praat_selectAll ();
				praat_show ();
			} else {
				int IOBJECT = praat_findObjectFromString (interpreter, command + 7);
				praat_deselectAll ();
				praat_select (IOBJECT);
				praat_show ();
			}
		} else if (wcsnequ (command, L"plus ", 5)) {
			int IOBJECT = praat_findObjectFromString (interpreter, command + 5);
			praat_select (IOBJECT);
			praat_show ();
		} else if (wcsnequ (command, L"minus ", 6)) {
			int IOBJECT = praat_findObjectFromString (interpreter, command + 6);
			praat_deselect (IOBJECT);
			praat_show ();
		} else if (wcsnequ (command, L"echo ", 5)) {
			MelderInfo_open ();
			MelderInfo_write (command + 5);
			MelderInfo_close ();
		} else if (wcsnequ (command, L"clearinfo", 9)) {
			Melder_clearInfo ();
		} else if (wcsnequ (command, L"print ", 6)) {
			Melder_print (command + 6);
		} else if (wcsnequ (command, L"printtab", 8)) {
			Melder_print (L"\t");
		} else if (wcsnequ (command, L"printline", 9)) {
			if (command [9] == ' ') Melder_print (command + 10);
			Melder_print (L"\n");
		} else if (wcsnequ (command, L"fappendinfo ", 12)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw ("The script command \"fappendinfo\" is not available inside pictures.");
			structMelderFile file = { 0 };
			Melder_relativePathToFile (command + 12, & file);
			MelderFile_appendText (& file, Melder_getInfo ());
		} else if (wcsnequ (command, L"unix ", 5)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw ("The script command \"unix\" is not available inside manuals.");
			try {
				Melder_system (command + 5);
			} catch (MelderError) {
				Melder_throw ("Unix command \"", command + 5, "\" returned error status;\n"
					"if you want to ignore this, use `unix_nocheck' instead of `unix'.");
			}
		} else if (wcsnequ (command, L"unix_nocheck ", 13)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (L"The script command \"unix_nocheck\" is not available inside manuals.");
			try {
				Melder_system (command + 13);
			} catch (MelderError) {
				Melder_clearError ();
			}
		} else if (wcsnequ (command, L"system ", 7)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (L"The script command \"system\" is not available inside manuals.");
			try {
				Melder_system (command + 7);
			} catch (MelderError) {
				Melder_throw ("System command \"", command + 7, "\" returned error status;\n"
					"if you want to ignore this, use `system_nocheck' instead of `system'.");
			}
		} else if (wcsnequ (command, L"system_nocheck ", 15)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw ("The script command \"system_nocheck\" is not available inside manuals.");
			try {
				Melder_system (command + 15);
			} catch (MelderError) {
				Melder_clearError ();
			}
		} else if (wcsnequ (command, L"nowarn ", 7)) {
			autoMelderWarningOff nowarn;
			praat_executeCommand (interpreter, command + 7);
		} else if (wcsnequ (command, L"noprogress ", 11)) {
			autoMelderProgressOff noprogress;
			praat_executeCommand (interpreter, command + 11);
		} else if (wcsnequ (command, L"nocheck ", 8)) {
			try {
				praat_executeCommand (interpreter, command + 8);
			} catch (MelderError) {
				Melder_clearError ();
				return 0;
			}
		} else if (wcsnequ (command, L"demo ", 5)) {
			autoDemoOpen demo;
			praat_executeCommand (interpreter, command + 5);
		} else if (wcsnequ (command, L"pause ", 6) || wcsequ (command, L"pause")) {
			if (theCurrentPraatApplication -> batch)
				return 1;   // in batch we ignore pause statements
			UiPause_begin (theCurrentPraatApplication -> topShell, L"stop or continue", interpreter);
			UiPause_comment (wcsequ (command, L"pause") ? L"..." : command + 6);
			UiPause_end (1, 1, 0, L"Continue", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, interpreter);
		} else if (wcsnequ (command, L"execute ", 8)) {
			praat_executeScriptFromFileNameWithArguments (command + 8);
		} else if (wcsnequ (command, L"editor", 6)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw ("The script command \"editor\" is not available inside manuals.");
			if (command [6] == ' ' && isalpha (command [7])) {
				praatP. editor = praat_findEditorFromString (command + 7);
			} else if (command [6] == '\0') {
				if (interpreter && interpreter -> editorClass) {
					praatP. editor = praat_findEditorFromString (interpreter -> environmentName);
				} else {
					Melder_throw ("The function \"editor\" requires an argument when called from outside an editor.");
				}
			} else {
				Interpreter_voidExpression (interpreter, command);
			}
		} else if (wcsnequ (command, L"endeditor", 9)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw ("The script command \"endeditor\" is not available inside manuals.");
			praatP. editor = NULL;
		} else if (wcsnequ (command, L"sendpraat ", 10)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw ("The script command \"sendpraat\" is not available inside manuals.");
			wchar_t programName [41], *q = & programName [0];
			#ifdef macintosh
				#define SENDPRAAT_TIMEOUT  10
			#else
				#define SENDPRAAT_TIMEOUT  0
			#endif
			const wchar_t *p = command + 10;
			while (*p == ' ' || *p == '\t') p ++;
			while (*p != '\0' && *p != ' ' && *p != '\t' && q < programName + 39) *q ++ = *p ++;
			*q = '\0';
			if (q == programName)
				Melder_throw ("Missing program name after `sendpraat'.");
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0')
				Melder_throw ("Missing command after `sendpraat'.");
			#if motif
			char *result = sendpraat (XtDisplay (theCurrentPraatApplication -> topShell), Melder_peekWcsToUtf8 (programName),
				SENDPRAAT_TIMEOUT, Melder_peekWcsToUtf8 (p));
			if (result)
				Melder_throw (result, "\nMessage to ", programName, " not completed.");
			#endif
		} else if (wcsnequ (command, L"sendsocket ", 11)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw ("The script command \"sendsocket\" is not available inside manuals.");
			wchar_t hostName [61], *q = & hostName [0];
			const wchar_t *p = command + 11;
			while (*p == ' ' || *p == '\t') p ++;
			while (*p != '\0' && *p != ' ' && *p != '\t' && q < hostName + 59) *q ++ = *p ++;
			*q = '\0';
			if (q == hostName)
				Melder_throw ("Missing host name after `sendsocket'.");
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0')
				Melder_throw ("Missing command after `sendsocket'.");
			char *result = sendsocket (Melder_peekWcsToUtf8 (hostName), Melder_peekWcsToUtf8 (p));
			if (result)
				Melder_throw (result, "\nMessage to ", hostName, " not completed.");
		} else if (wcsnequ (command, L"filedelete ", 11)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw ("The script command \"filedelete\" is not available inside manuals.");
			const wchar_t *p = command + 11;
			structMelderFile file = { 0 };
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0')
				Melder_throw ("Missing file name after `filedelete'.");
			Melder_relativePathToFile (p, & file);
			MelderFile_delete (& file);
		} else if (wcsnequ (command, L"fileappend ", 11)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw ("The script command \"fileappend\" is not available inside manuals.");
			const wchar_t *p = command + 11;
			wchar_t path [256], *q = & path [0];
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0')
				Melder_throw (L"Missing file name after `fileappend'.");
			if (*p == '\"') {
				for (;;) {
					int kar = * ++ p;
					if (kar == '\"') if (* ++ p == '\"') *q ++ = '\"'; else break;
					else if (kar == '\0') break;
					else *q ++ = kar;
				}
			} else {
				for (;;) {
					int kar = * p;
					if (kar == '\0' || kar == ' ' || kar == '\t') break;
					*q ++ = kar;
					p ++;
				}
			}
			*q = '\0';
			if (*p == ' ' || *p == '\t') {
				structMelderFile file = { 0 };
				Melder_relativePathToFile (path, & file);
				MelderFile_appendText (& file, p + 1);
			}
		} else {
			/*
			 * This must be a formula command:
			 *    proc (args)
			 */
			Interpreter_voidExpression (interpreter, command);
		}
	} else {   /* Simulate menu choice. */
		bool hasDots = false, hasColon = false;

 		/* Parse command line into command and arguments. */
		/* The separation is formed by the three dots or a colon. */

		wchar_t *arguments = & command [0];
		for (arguments = & command [0]; *arguments != '\0'; arguments ++) {
			if (*arguments == ':') {
				hasColon = true;
				if (arguments [1] == '\0') {
					arguments = & arguments [1];   // empty string
				} else {
					if (arguments [1] != ' ') {
						Melder_throw ("There should be a space after the colon.");
					}
					arguments [1] = '\0';   // new end of "command"
					arguments += 2;   // the arguments start after the space
				}
				break;
			}
			if (*arguments == '.' && arguments [1] == '.' && arguments [2] == '.') {
				hasDots = true;
				arguments += 3;
				if (*arguments == '\0') {
					// empty string
				} else {
					if (*arguments != ' ') {
						Melder_throw ("There should be a space after the three dots.");
					}
					*arguments = '\0';   // new end of "command"
					arguments ++;   // the arguments start after the space
				}
				break;
			}
		}

		/* See if command exists and is available; ignore separators. */
		/* First try loose commands, then fixed commands. */

		int narg;
		wchar_t command2 [200];
		if (hasColon) {
			narg = parseCommaSeparatedArguments (interpreter, arguments, args);
			wcscpy (command2, command);
			wchar_t *colon = wcschr (command2, ':');
			colon [0] = colon [1] = colon [2] = '.';
			colon [3] = '\0';
		}
		if (theCurrentPraatObjects == & theForegroundPraatObjects && praatP. editor != NULL) {
			if (hasColon) {
				Editor_doMenuCommand ((Editor) praatP. editor, command2, narg, args, NULL, interpreter);
			} else {
				Editor_doMenuCommand ((Editor) praatP. editor, command, 0, NULL, arguments, interpreter);
			}
		} else if (theCurrentPraatObjects != & theForegroundPraatObjects &&
		    (wcsnequ (command, L"Save ", 5) ||
			 wcsnequ (command, L"Write ", 6) ||
			 wcsnequ (command, L"Append ", 7) ||
			 wcsequ (command, L"Quit")))
		{
			Melder_throw ("Commands that write files (including Quit) are not available inside manuals.");
		} else {
			bool theCommandIsAnExistingAction = false;
			try {
				if (hasColon) {
					theCommandIsAnExistingAction = praat_doAction (command2, narg, args, interpreter);
				} else {
					theCommandIsAnExistingAction = praat_doAction (command, arguments, interpreter);
				}
			} catch (MelderError) {
				/*
				 * We only get here if the command *was* an existing action.
				 * Anything could have gone wrong in its execution,
				 * but one invisible problem can be checked here.
				 */
				if (hasDots && arguments [0] != '\0' && arguments [wcslen (arguments) - 1] == ' ') {
					Melder_throw ("It may be helpful to remove the trailing spaces in \"", arguments, "\".");
				} else {
					throw;
				}
			}
			if (! theCommandIsAnExistingAction) {
				bool theCommandIsAnExistingMenuCommand = false;
				try {
					if (hasColon) {
						theCommandIsAnExistingMenuCommand = praat_doMenuCommand (command2, narg, args, interpreter);
					} else {
						theCommandIsAnExistingMenuCommand = praat_doMenuCommand (command, arguments, interpreter);
					}
				} catch (MelderError) {
					/*
					 * We only get here if the command *was* an existing menu command.
					 * Anything could have gone wrong in its execution,
					 * but one invisible problem can be checked here.
					 */
					if (hasDots && arguments [0] != '\0' && arguments [wcslen (arguments) - 1] == ' ') {
						Melder_throw ("It may be helpful to remove the trailing spaces in \"", arguments, L"\".");
					} else {
						throw;
					}
				}
				if (! theCommandIsAnExistingMenuCommand) {
					if (wcsnequ (command, L"ARGS ", 5)) {
						Melder_throw ("Command \"ARGS\" no longer supported. Instead use \"form\" and \"endform\".");
					} else if (wcschr (command, '=')) {
						Melder_throw ("Command \"", command, "\" not recognized.\n"
							"Probable cause: you are trying to use a variable name that starts with a capital.");
					} else if (command [0] != '\0' && command [wcslen (command) - 1] == ' ') {
						Melder_throw ("Command \"", command, "\" not available for current selection. "
							"It may be helpful to remove the trailing spaces.");
					} else {
						Melder_throw ("Command \"", command, "\" not available for current selection.");
					}
				}
			}
		}
		praat_updateSelection ();
	}
	return 1;
}

void praat_executeCommandFromStandardInput (const char *programName) {
	char command [1000]; // can be recursive
	for (;;) {
		char *newLine;
		printf ("%s > ", programName);
		if (! fgets (command, 999, stdin))
			Melder_throw ("Cannot read input.");
		newLine = strchr (command, '\n');
		if (newLine) *newLine = '\0';
		autostring commandW = Melder_utf8ToWcs (command);
		try {
			praat_executeCommand (NULL, commandW.peek());
		} catch (MelderError) {
			Melder_error_ (programName, ": command \"", command, "\" not executed.");
			Melder_flushError (NULL);
		}
	}
}

void praat_executeScriptFromFile (MelderFile file, const wchar_t *arguments) {
	try {
		autostring text = MelderFile_readText (file);
		autoMelderFileSetDefaultDir dir (file);   // so that relative file names can be used inside the script
		Melder_includeIncludeFiles (& text);
		autoInterpreter interpreter = Interpreter_createFromEnvironment (praatP.editor);
		if (arguments) {
			Interpreter_readParameters (interpreter.peek(), text.peek());
			Interpreter_getArgumentsFromString (interpreter.peek(), arguments);
		}
		Interpreter_run (interpreter.peek(), text.peek());
	} catch (MelderError) {
		Melder_throw (L"Script ", file, L" not completed.");
	}
}

void praat_executeScriptFromFileName (const wchar_t *fileName, int narg, Stackel args) {
	/*
	 * The argument 'fileName' is unsafe. Duplicate its contents.
	 */
	structMelderFile file = { 0 };
	Melder_relativePathToFile (fileName, & file);
	try {
		autostring text = MelderFile_readText (& file);
		autoMelderFileSetDefaultDir dir (& file);   // so that relative file names can be used inside the script
		Melder_includeIncludeFiles (& text);
		autoInterpreter interpreter = Interpreter_createFromEnvironment (praatP.editor);
		Interpreter_readParameters (interpreter.peek(), text.peek());
		Interpreter_getArgumentsFromArgs (interpreter.peek(), narg, args);
		Interpreter_run (interpreter.peek(), text.peek());
	} catch (MelderError) {
		Melder_throw (L"Script ", & file, L" not completed.");   // don't refer to 'fileName', because its contents may have changed
	}
}

void praat_executeScriptFromFileNameWithArguments (const wchar_t *nameAndArguments) {
	wchar_t path [256];
	const wchar_t *p, *arguments;
	structMelderFile file = { 0 };
	/*
	 * Split into file name and arguments.
	 */
	p = nameAndArguments;
	while (*p == ' ' || *p == '\t') p ++;
	if (*p == '\"') {
		wchar_t *q = path;
		p ++;   // skip quote
		while (*p != '\"' && *p != '\0') * q ++ = * p ++;
		*q = '\0';
		arguments = p;
		if (*arguments == '\"') arguments ++;
		if (*arguments == ' ') arguments ++;
	} else {
		wchar_t *q = path;
		while (*p != ' ' && *p != '\0') * q ++ = * p ++;
		*q = '\0';
		arguments = p;
		if (*arguments == ' ') arguments ++;
	}
	Melder_relativePathToFile (path, & file);
	praat_executeScriptFromFile (& file, arguments);
}

void praat_executeScriptFromText (wchar_t *text) {
	try {
		autoInterpreter interpreter = Interpreter_create (NULL, NULL);
		Interpreter_run (interpreter.peek(), text);
	} catch (MelderError) {
		Melder_throw (L"Script not completed.");
	}
}

void praat_executeScriptFromDialog (Any dia) {
	wchar_t *path = UiForm_getString (dia, L"$file");
	structMelderFile file = { 0 };
	Melder_pathToFile (path, & file);
	autostring text = MelderFile_readText (& file);
	autoMelderFileSetDefaultDir dir (& file);
	Melder_includeIncludeFiles (& text);
	autoInterpreter interpreter = Interpreter_createFromEnvironment (praatP.editor);
	Interpreter_readParameters (interpreter.peek(), text.peek());
	Interpreter_getArgumentsFromDialog (interpreter.peek(), dia);
	autoPraatBackground background;
	Interpreter_run (interpreter.peek(), text.peek());
}

static void secondPassThroughScript (UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString_dummy, Interpreter interpreter_dummy, const wchar_t *invokingButtonTitle, bool modified, void *dummy) {
	(void) sendingString_dummy;
	(void) interpreter_dummy;
	(void) invokingButtonTitle;
	(void) modified;
	(void) dummy;
	praat_executeScriptFromDialog (sendingForm);
}

static void firstPassThroughScript (MelderFile file) {
	try {
		autostring text = MelderFile_readText (file);
		{// scope
			autoMelderFileSetDefaultDir dir (file);
			Melder_includeIncludeFiles (& text);
		}
		autoInterpreter interpreter = Interpreter_createFromEnvironment (praatP.editor);
		if (Interpreter_readParameters (interpreter.peek(), text.peek()) > 0) {
			Any form = Interpreter_createForm (interpreter.peek(),
				praatP.editor ? ((Editor) praatP.editor) -> d_windowForm : theCurrentPraatApplication -> topShell,
				Melder_fileToPath (file), secondPassThroughScript, NULL, false);
			UiForm_destroyWhenUnmanaged (form);
			UiForm_do (form, false);
		} else {
			autoPraatBackground background;
			praat_executeScriptFromFile (file, NULL);
		}
	} catch (MelderError) {
		Melder_throw ("Script ", file, " not completed.");
	}
}

static void fileSelectorOkCallback (UiForm dia, int narg, Stackel args, const wchar_t *sendingString_dummy, Interpreter interpreter_dummy, const wchar_t *invokingButtonTitle, bool modified, void *dummy) {
	(void) sendingString_dummy;
	(void) interpreter_dummy;
	(void) invokingButtonTitle;
	(void) modified;
	(void) dummy;
	firstPassThroughScript (UiFile_getFile (dia));
}

void DO_RunTheScriptFromAnyAddedMenuCommand (UiForm sendingForm_dummy, int narg, Stackel args, const wchar_t *scriptPath, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *dummy) {
	structMelderFile file = { 0 };
	(void) sendingForm_dummy;
	(void) interpreter;
	(void) invokingButtonTitle;
	(void) modified;
	(void) dummy;
	Melder_relativePathToFile ((wchar_t *) scriptPath, & file);
	firstPassThroughScript (& file);
}

void DO_RunTheScriptFromAnyAddedEditorCommand (Editor editor, const wchar_t *script) {
	praatP.editor = editor;
	DO_RunTheScriptFromAnyAddedMenuCommand (NULL, 0, NULL, script, NULL, NULL, false, NULL);
	/*praatP.editor = NULL;*/
}

/* End of file praat_script.cpp */
