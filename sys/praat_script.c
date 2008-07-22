/* praat_script.c
 *
 * Copyright (C) 1993-2007 Paul Boersma
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
 * pb 2002/10/02 system -> Melder_system
 * pb 2003/03/09 set UiInterpreter back to NULL
 * pb 2004/02/22 allow numeric expressions after select/plus/minus
 * pb 2004/10/27 warning off
 * pb 2004/12/04 support for multiple open script dialogs with Apply buttons, both from "Run script..." and from added buttons
 * pb 2005/02/10 corrected bug in nowarn
 * pb 2005/08/22 renamed Control menu to "Praat"
 * pb 2006/01/11 local variables
 * pb 2006/12/28 theCurrentPraat
 * pb 2007/02/17 corrected the messages about trailing spaces
 * pb 2007/06/11 wchar_t
 * pb 2007/10/04 removed swscanf
 */

#include <ctype.h>
#include "praatP.h"
#include "praat_script.h"
#include "sendpraat.h"
#include "sendsocket.h"

static int praat_findObjectFromString (Interpreter interpreter, const wchar_t *string) {
	int IOBJECT;
	while (*string == ' ') string ++;
	if (*string >= 'A' && *string <= 'Z') {
		static MelderString buffer = { 0 };
		MelderString_copy (& buffer, string);
		wchar_t *space = wcschr (buffer.string, ' ');
		if (space == NULL) goto end;
		*space = '\0';
		wchar_t *className = & buffer.string [0], *givenName = space + 1;
		WHERE_DOWN (1) {
			Data object = OBJECT;
			if (wcsequ (className, Thing_className (OBJECT)) && wcsequ (givenName, object -> name))
				return IOBJECT;
		}
	} else {
		double value;
		if (! Interpreter_numericExpression (interpreter, string, & value)) goto end;
		long id = (long) value;
		WHERE (ID == id) return IOBJECT;
		goto end;
	}
end:
	return Melder_error3 (L"Object \"", string, L"\" does not exist.");
}

Editor praat_findEditorFromString (const wchar_t *string) {
	int IOBJECT;
	while (*string == ' ') string ++;
	if (*string >= 'A' && *string <= 'Z') {
		WHERE_DOWN (1) {
			for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor editor = theCurrentPraat -> list [IOBJECT]. editors [ieditor];
				if (editor != NULL) {
					const wchar_t *name = wcschr (editor -> name, ' ') + 1;
					if (wcsequ (name, string)) return editor;
				}
			}
		}
	} else {
		WHERE_DOWN (1) {
			for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor editor = theCurrentPraat -> list [IOBJECT]. editors [ieditor];
				if (editor && wcsequ (editor -> name, string)) return editor;
			}
		}
	}
	return NULL;
}

int praat_executeCommand (Interpreter interpreter, const wchar_t *command) {
	if (command [0] == '\0' || command [0] == '#' || command [0] == '!' || command [0] == ';')
		/* Skip empty lines and comments. */;
	else if ((command [0] == '.' || command [0] == '+' || command [0] == '-') && isupper (command [1])) {   /* Selection? */
		int IOBJECT = praat_findObjectFromString (interpreter, command + 1);
		if (! IOBJECT) return 0;
		if (command [0] == '.') praat_deselectAll ();
		if (command [0] == '-') praat_deselect (IOBJECT); else praat_select (IOBJECT); 
		praat_show ();
	} else if (islower (command [0])) {   /* All directives start with a lower-case letter. */
		if (wcsnequ (command, L"select ", 7)) {
			if (wcsnequ (command + 7, L"all", 3)) {
				praat_selectAll ();
				praat_show ();
			} else {
				int IOBJECT = praat_findObjectFromString (interpreter, command + 7);
				if (! IOBJECT) return 0;
				praat_deselectAll ();
				praat_select (IOBJECT);
				praat_show ();
			}
		} else if (wcsnequ (command, L"plus ", 5)) {
			int IOBJECT = praat_findObjectFromString (interpreter, command + 5);
			if (! IOBJECT) return 0;
			praat_select (IOBJECT);
			praat_show ();
		} else if (wcsnequ (command, L"minus ", 6)) {
			int IOBJECT = praat_findObjectFromString (interpreter, command + 6);
			if (! IOBJECT) return 0;
			praat_deselect (IOBJECT);
			praat_show ();
		} else if (wcsnequ (command, L"echo ", 5)) {
			MelderInfo_open ();
			MelderInfo_write1 (command + 5);
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
			if (theCurrentPraat != & theForegroundPraat)
				return Melder_error1 (L"The script command \"fappendinfo\" is not available inside pictures.");
			structMelderFile file = { 0 };
			if (! Melder_relativePathToFile (command + 12, & file)) return 0;
			if (! MelderFile_appendText (& file, Melder_getInfo ())) return 0;
		} else if (wcsnequ (command, L"unix ", 5)) {
			if (theCurrentPraat != & theForegroundPraat)
				return Melder_error1 (L"The script command \"unix\" is not available inside pictures.");
			if (! Melder_system (command + 5))
				return Melder_error3 (L"Unix command \"", command + 5, L"\" returned error status;\n"
					"if you want to ignore this, use `unix_nocheck' instead of `unix'.");
		} else if (wcsnequ (command, L"unix_nocheck ", 13)) {
			if (theCurrentPraat != & theForegroundPraat)
				return Melder_error1 (L"The script command \"unix_nocheck\" is not available inside pictures.");
			(void) Melder_system (command + 13); Melder_clearError ();
		} else if (wcsnequ (command, L"system ", 7)) {
			if (theCurrentPraat != & theForegroundPraat)
				return Melder_error1 (L"The script command \"system\" is not available inside pictures.");
			if (! Melder_system (command + 7))
				return Melder_error3 (L"System command \"", command + 7, L"\" returned error status;\n"
					"if you want to ignore this, use `system_nocheck' instead of `system'.");
		} else if (wcsnequ (command, L"system_nocheck ", 15)) {
			if (theCurrentPraat != & theForegroundPraat)
				return Melder_error1 (L"The script command \"system_nocheck\" is not available inside pictures.");
			(void) Melder_system (command + 15); Melder_clearError ();
		} else if (wcsnequ (command, L"nowarn ", 7)) {
			int result;
			Melder_warningOff ();
			result = praat_executeCommand (interpreter, command + 7);
			Melder_warningOn ();
			return result;
		} else if (wcsnequ (command, L"noprogress ", 11)) {
			int result;
			Melder_progressOff ();
			result = praat_executeCommand (interpreter, command + 11);
			Melder_progressOn ();
			return result;
		} else if (wcsnequ (command, L"nocheck ", 8)) {
			(void) praat_executeCommand (interpreter, command + 8);
			Melder_clearError ();
		} else if (wcsnequ (command, L"pause", 5)) {
			int wasBackgrounding = Melder_backgrounding;
			structMelderDir dir = { { 0 } };
			Melder_getDefaultDir (& dir);
			if (theCurrentPraat -> batch) return 1;
			UiFile_hide ();
			if (wasBackgrounding) praat_foreground ();
			if (! Melder_pause (command + 5)) return Melder_error1 (L"You interrupted the script.");
			if (wasBackgrounding) praat_background ();
			Melder_setDefaultDir (& dir);
			/* BUG: should also restore praatP. editor. */
		} else if (wcsnequ (command, L"execute ", 8)) {
			praat_executeScriptFromFileNameWithArguments (command + 8);
		} else if (wcsnequ (command, L"editor", 6)) {
			if (theCurrentPraat != & theForegroundPraat)
				return Melder_error1 (L"The script command \"editor\" is not available inside pictures.");
			if (command [6] == ' ' && isalpha (command [7])) {
				praatP. editor = praat_findEditorFromString (command + 7);
				if (praatP. editor == NULL)
					return Melder_error3 (L"Editor \"", command + 7, L"\" does not exist.");
			} else if (interpreter && interpreter -> editorClass) {
				praatP. editor = praat_findEditorFromString (interpreter -> environmentName);
				if (praatP. editor == NULL)
					return Melder_error3 (L"Editor \"", interpreter -> environmentName, L"\" does not exist.");
			} else {
				return Melder_error1 (L"No editor specified.");
			}
		} else if (wcsnequ (command, L"endeditor", 9)) {
			if (theCurrentPraat != & theForegroundPraat)
				return Melder_error1 (L"The script command \"endeditor\" is not available inside pictures.");
			praatP. editor = NULL;
		} else if (wcsnequ (command, L"sendpraat ", 10)) {
			if (theCurrentPraat != & theForegroundPraat)
				return Melder_error1 (L"The script command \"sendpraat\" is not available inside pictures.");
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
				return Melder_error1 (L"Missing program name after `sendpraat'.");
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0')
				return Melder_error1 (L"Missing command after `sendpraat'.");
			#if motif
			char *result = sendpraat (XtDisplay (theCurrentPraat -> topShell), Melder_peekWcsToUtf8 (programName),
				SENDPRAAT_TIMEOUT, Melder_peekWcsToUtf8 (p));
			if (result)
				return Melder_error4 (Melder_peekUtf8ToWcs (result), L"\nMessage to ", programName, L" not completed.");
			#endif
		} else if (wcsnequ (command, L"sendsocket ", 11)) {
			if (theCurrentPraat != & theForegroundPraat)
				return Melder_error1 (L"The script command \"sendsocket\" is not available inside pictures.");
			wchar_t hostName [61], *q = & hostName [0];
			const wchar_t *p = command + 11;
			while (*p == ' ' || *p == '\t') p ++;
			while (*p != '\0' && *p != ' ' && *p != '\t' && q < hostName + 59) *q ++ = *p ++;
			*q = '\0';
			if (q == hostName)
				return Melder_error1 (L"Missing host name after `sendsocket'.");
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0')
				return Melder_error1 (L"Missing command after `sendsocket'.");
			char *result = sendsocket (Melder_peekWcsToUtf8 (hostName), Melder_peekWcsToUtf8 (p));
			if (result)
				return Melder_error4 (Melder_peekUtf8ToWcs (result), L"\nMessage to ", hostName, L" not completed.");
		} else if (wcsnequ (command, L"filedelete ", 11)) {
			if (theCurrentPraat != & theForegroundPraat)
				return Melder_error1 (L"The script command \"filedelete\" is not available inside pictures.");
			const wchar_t *p = command + 11;
			structMelderFile file = { 0 };
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0')
				return Melder_error1 (L"Missing file name after `filedelete'.");
			if (! Melder_relativePathToFile (p, & file)) return 0;
			MelderFile_delete (& file);
		} else if (wcsnequ (command, L"fileappend ", 11)) {
			if (theCurrentPraat != & theForegroundPraat)
				return Melder_error1 (L"The script command \"fileappend\" is not available inside pictures.");
			const wchar_t *p = command + 11;
			wchar_t path [256], *q = & path [0];
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0')
				return Melder_error1 (L"Missing file name after `fileappend'.");
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
				if (! Melder_relativePathToFile (path, & file)) return 0;
				if (! MelderFile_appendText (& file, p + 1)) return 0;
			}
		} else {
			if (wcsnequ (command, L"getinfostring ", 14))
				return Melder_error1 (L"Command \"getinfostring\" no longer supported. Consult scripting tutorial or the author.");
			if (wcsnequ (command, L"getinfonumber ", 14))
				return Melder_error1 (L"Command \"getinfonumber\" no longer supported. Consult scripting tutorial or the author.");
			if (wcsnequ (command, L"getnameofselected ", 18))
				return Melder_error1 (L"Command \"getnameofselected\" no longer supported. Instead use things like: name$ = selected$ (\"Sound\")");
			if (wcsnequ (command, L"getidofselected ", 16))
				return Melder_error1 (L"Command \"getidofselected\" no longer supported. Instead use things like: id = selected (\"Sound\")");
			if (wcsnequ (command, L"getnumberofselected ", 20))
				return Melder_error1 (L"Command \"getnumberofselected\" no longer supported. Instead use things like: n = numberOfSelected (\"Sound\")");
			if (wcsnequ (command, L"getnumber ", 10))
				return Melder_error1 (L"Command \"getnumber\" no longer supported. Instead use things like: \"variable = Menu command\".");
			if (wcsnequ (command, L"getstring ", 10))
				return Melder_error1 (L"Command \"getstring\" no longer supported. Instead use things like: \"variable$ = Menu command\".");
			if (wcsnequ (command, L"let ", 4))
				return Melder_error1 (L"Command \"let\" no longer supported. Instead use things like: \"variable = numeric-formula\" or \"variable$ = string-formula\".");
			if (wcsnequ (command, L"proc ", 5))
				return Melder_error1 (L"Command \"proc\" no longer supported. Instead use \"procedure\", and distinguish between numeric variables and string variables.");
			if (wcsnequ (command, L"copy ", 5))
				return Melder_error1 (L"Command \"copy x y\" no longer supported. Instead use normal assignments, i.e. y = x.");
			return Melder_error2 (L"Unknown command: ", command);
		}
	} else {   /* Simulate menu choice. */
		wchar_t *arguments;

 		/* Parse command line into command and arguments. */
		/* The separation is formed by the three dots. */

		if ((arguments = wcsstr (command, L"...")) == NULL || wcslen (arguments) < 4) {
			arguments = L"";
		} else {
			arguments += 4;
			if (arguments [-1] != ' ' && arguments [-1] != '0') {
				return Melder_error1 (L"There should be a space after the three dots.");
			}
			arguments [-1] = '\0'; /* New end of "command". */
		}

		/* See if command exists and is available; ignore separators. */
		/* First try loose commands, then fixed commands. */

		UiInterpreter_set (interpreter);
		if (theCurrentPraat == & theForegroundPraat && praatP. editor != NULL) {
			if (! Editor_doMenuCommand (praatP. editor, command, arguments)) {
				UiInterpreter_set (NULL);
				return 0;
			}
		} else if (theCurrentPraat != & theForegroundPraat &&
		    (wcsnequ (command, L"Write ", 6) ||
			 wcsnequ (command, L"Append ", 7) ||
			 wcsequ (command, L"Quit")))
		{
			return Melder_error1 (L"Commands that write files (including Quit) are not available inside pictures.");
		} else if (! praat_doAction (command, arguments)) {
			if (Melder_hasError ()) {
				UiInterpreter_set (NULL);
				if (arguments [0] != '\0' && arguments [wcslen (arguments) - 1] == ' ') {
					return Melder_error3 (L"It may be helpful to remove the trailing spaces in \"", arguments, L"\".");
				}
				return 0;
			}
			if (! praat_doMenuCommand (command, arguments)) {
				UiInterpreter_set (NULL);
				if (Melder_hasError ()) {
					if (arguments [0] != '\0' && arguments [wcslen (arguments) - 1] == ' ') {
						return Melder_error3 (L"It may be helpful to remove the trailing spaces in \"", arguments, L"\".");
					}
					return 0;
				} else if (wcsnequ (command, L"ARGS ", 5)) {
					return Melder_error1 (L"Command \"ARGS\" no longer supported. Instead use \"form\" and \"endform\".");
				} else if (wcschr (command, '=')) {
					return Melder_error3 (L"Command \"", command, L"\" not recognized.\n"
						"Probable cause: you are trying to use a variable name that starts with a capital.");
				} else if (command [0] != '\0' && command [wcslen (command) - 1] == ' ') {
					return Melder_error3 (L"Command \"", command, L"\" not available for current selection. "
						"It may be helpful to remove the trailing spaces.");
				} else {
					return Melder_error3 (L"Command \"", command, L"\" not available for current selection.");
				}
			}
		}
		UiInterpreter_set (NULL);
		praat_updateSelection ();
	}
	return 1;
}

int praat_executeCommandFromStandardInput (const char *programName) {
	char command [1000]; /* Can be recursive. */
	for (;;) {
		char *newLine;
		printf ("%s > ", programName);
		if (! fgets (command, 999, stdin)) return 0;
		newLine = strchr (command, '\n');
		if (newLine) *newLine = '\0';
		wchar_t *commandW = Melder_utf8ToWcs (command);
		if (! praat_executeCommand (NULL, commandW))
			Melder_flushError ("%s: command \"%s\" not executed.", programName, command);
		Melder_free (commandW);
	}
	return 1;
}

int praat_executeScriptFromFile (MelderFile file, const wchar_t *arguments) {
	wchar_t *text = NULL;
	Interpreter interpreter = NULL;
	structMelderDir saveDir = { { 0 } };
	Melder_getDefaultDir (& saveDir);   /* Before the first cherror! */

	text = MelderFile_readText (file); cherror
	MelderFile_setDefaultDir (file);   /* So that relative file names can be used inside the script. */
	Melder_includeIncludeFiles (& text); cherror
	interpreter = Interpreter_createFromEnvironment (praatP.editor); cherror
	if (arguments) {
		Interpreter_readParameters (interpreter, text); cherror
		Interpreter_getArgumentsFromString (interpreter, arguments); cherror
	}
	Interpreter_run (interpreter, text);
end:
	Melder_setDefaultDir (& saveDir);
	Melder_free (text);
	forget (interpreter);
	iferror return Melder_error3 (L"Script \"", MelderFile_messageNameW (file), L"\" not completed.");
	return 1;
}

int praat_executeScriptFromFileNameWithArguments (const wchar_t *nameAndArguments) {
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
	if (! Melder_relativePathToFile (path, & file)) return 0;
	return praat_executeScriptFromFile (& file, arguments);
}

int praat_executeScriptFromText (wchar_t *text) {
	Interpreter interpreter = Interpreter_create (NULL, NULL);
	Interpreter_run (interpreter, text);
	forget (interpreter);
	iferror return Melder_error1 (L"Script not completed.");
	return 1;
}

int praat_executeScriptFromDialog (Any dia) {
	Interpreter interpreter = NULL;
	structMelderFile file = { 0 };
	wchar_t *text = NULL, *path = UiForm_getString (dia, L"$file");
	structMelderDir saveDir = { { 0 } };
	Melder_getDefaultDir (& saveDir);

	Melder_pathToFile (path, & file); cherror
	text = MelderFile_readText (& file); cherror
	MelderFile_setDefaultDir (& file);
	Melder_includeIncludeFiles (& text); cherror
	interpreter = Interpreter_createFromEnvironment (praatP.editor); cherror
	Interpreter_readParameters (interpreter, text); cherror
	Interpreter_getArgumentsFromDialog (interpreter, dia); cherror
	praat_background ();
	Interpreter_run (interpreter, text);
	praat_foreground ();
end:
	Melder_setDefaultDir (& saveDir);
	Melder_free (text);
	forget (interpreter);
	iferror return 0;
	return 1;
}

static int secondPassThroughScript (Any dia, void *dummy) {
	(void) dummy;
	return praat_executeScriptFromDialog (dia);
}

static int firstPassThroughScript (MelderFile file) {
	wchar_t *text = NULL;
	Interpreter interpreter = NULL;
	structMelderDir saveDir = { { 0 } };
	Melder_getDefaultDir (& saveDir);

	text = MelderFile_readText (file); cherror
	MelderFile_setDefaultDir (file);
	Melder_includeIncludeFiles (& text); cherror
	Melder_setDefaultDir (& saveDir);
	interpreter = Interpreter_createFromEnvironment (praatP.editor);
	if (Interpreter_readParameters (interpreter, text) > 0) {
		Any form = Interpreter_createForm (interpreter,
			praatP.editor ? ((Editor) praatP.editor) -> dialog : theCurrentPraat -> topShell,
			Melder_fileToPath (file), secondPassThroughScript, NULL);
		UiForm_destroyWhenUnmanaged (form);
		UiForm_do (form, false);
	} else {
		praat_background ();
		praat_executeScriptFromFile (file, NULL);
		praat_foreground ();
	}
end:
	Melder_setDefaultDir (& saveDir);
	Melder_free (text);
	forget (interpreter);
	iferror return Melder_error3 (L"Script ", MelderFile_messageNameW (file), L" not completed.");
	return 1;
}

static int fileSelectorOkCallback (Any dia, void *dummy) {
	(void) dummy;
	return firstPassThroughScript (UiFile_getFile (dia));
}

/*
 * DO_praat_runScript () is the command callback for "Run script...", which is a bit obsolete command,
 * hidden in the Praat menu, and otherwise replaced by "execute".
 */
int DO_praat_runScript (Any sender, void *dummy) {
	(void) dummy;
	if (sender == NULL) {
		/*
		 * User clicked the "Run script..." button in the Praat menu.
		 */
		static Any file_dialog;
		if (! file_dialog)
			file_dialog = UiInfile_create (theCurrentPraat -> topShell, L"Praat: run script", fileSelectorOkCallback, NULL, 0);
		UiInfile_do (file_dialog);
	} else {
		/*
		 * A script called "Run script..."
		 */
		praat_executeScriptFromFileNameWithArguments (sender);
	}
	return 1;
}

int DO_RunTheScriptFromAnyAddedMenuCommand (Any scriptPath, void *dummy) {
	structMelderFile file = { 0 };
	(void) dummy;
	if (! Melder_relativePathToFile ((wchar_t *) scriptPath, & file)) return 0;
	return firstPassThroughScript (& file);
}

int DO_RunTheScriptFromAnyAddedEditorCommand (Any editor, const wchar_t *script) {
	praatP.editor = editor;
	DO_RunTheScriptFromAnyAddedMenuCommand ((Any) script, NULL);
	/*praatP.editor = NULL;*/
	iferror return 0;
	return 1;
}

/* End of file praat_script.c */
