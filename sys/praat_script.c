/* praat_script.c
 *
 * Copyright (C) 1993-2006 Paul Boersma
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
 */

#include <ctype.h>
#include "praatP.h"
#include "praat_script.h"
#include "sendpraat.h"
#include "sendsocket.h"

static int praat_findObjectFromString (Interpreter interpreter, const char *string) {
	int IOBJECT;
	while (*string == ' ') string ++;
	if (*string >= 'A' && *string <= 'Z') {
		char className [30], givenName [200];
		if (sscanf (string, "%s %s", className, givenName) < 2) goto end;
		WHERE_DOWN (1) {
			Data object = OBJECT;
			if (strequ (className, Thing_className (OBJECT)) && strequ (givenName, object -> name))
				return IOBJECT;
		}
	} else {
		double value;
		long id;
		if (! Interpreter_numericExpression (interpreter, string, & value)) goto end;
		id = (long) value;
		WHERE (ID == id) return IOBJECT;
		goto end;
	}
end:
	return Melder_error ("Object \"%s\" does not exist.", string);
}

int praat_executeCommand (Interpreter interpreter, const char *command) {
	if (command [0] == '\0' || command [0] == '#' || command [0] == '!' || command [0] == ';')
		/* Skip empty lines and comments. */;
	else if ((command [0] == '.' || command [0] == '+' || command [0] == '-') && isupper (command [1])) {   /* Selection? */
		int IOBJECT = praat_findObjectFromString (interpreter, command + 1);
		if (! IOBJECT) return 0;
		if (command [0] == '.') praat_deselectAll ();
		if (command [0] == '-') praat_deselect (IOBJECT); else praat_select (IOBJECT); 
		praat_show ();
	} else if (islower (command [0])) {   /* All directives start with a lower-case letter. */
		if (strnequ (command, "select ", 7)) {
			if (strnequ (command + 7, "all", 3)) {
				praat_selectAll ();
				praat_show ();
			} else {
				int IOBJECT = praat_findObjectFromString (interpreter, command + 7);
				if (! IOBJECT) return 0;
				praat_deselectAll ();
				praat_select (IOBJECT);
				praat_show ();
			}
		} else if (strnequ (command, "plus ", 5)) {
			int IOBJECT = praat_findObjectFromString (interpreter, command + 5);
			if (! IOBJECT) return 0;
			praat_select (IOBJECT);
			praat_show ();
		} else if (strnequ (command, "minus ", 6)) {
			int IOBJECT = praat_findObjectFromString (interpreter, command + 6);
			if (! IOBJECT) return 0;
			praat_deselect (IOBJECT);
			praat_show ();
		} else if (strnequ (command, "echo ", 5)) {
			MelderInfo_open ();
			MelderInfo_write1 (command + 5);
			MelderInfo_close ();
		} else if (strnequ (command, "clearinfo", 9)) {
			Melder_clearInfo ();
		} else if (strnequ (command, "print ", 6)) {
			Melder_print ("%s", command + 6);
		} else if (strnequ (command, "printtab", 8)) {
			Melder_print ("\t");
		} else if (strnequ (command, "printline", 9)) {
			if (command [9] == ' ') Melder_print ("%s\n", command + 10);
			else Melder_print ("\n");
		} else if (strnequ (command, "fappendinfo ", 12)) {
			FILE *f;
			structMelderFile file;
			if (! Melder_relativePathToFile (command + 12, & file)) return 0;
			f = Melder_fopen (& file, "a");
			if (! f) return 0;
			fprintf (f, "%s", Melder_getInfo ());
			fclose (f);
		} else if (strnequ (command, "unix ", 5)) {
			if (! Melder_system (command + 5))
				return Melder_error ("Unix command \"%s\" returned error status;\n"
					"if you want to ignore this, use `unix_nocheck' instead of `unix'.", command + 5);
		} else if (strnequ (command, "unix_nocheck ", 13)) {
			(void) Melder_system (command + 13); Melder_clearError ();
		} else if (strnequ (command, "system ", 7)) {
			if (! Melder_system (command + 7))
				return Melder_error ("System command \"%s\" returned error status;\n"
					"if you want to ignore this, use `system_nocheck' instead of `system'.", command + 7);
		} else if (strnequ (command, "system_nocheck ", 15)) {
			(void) Melder_system (command + 15); Melder_clearError ();
		} else if (strnequ (command, "nowarn ", 7)) {
			int result;
			Melder_warningOff ();
			result = praat_executeCommand (interpreter, command + 7);
			Melder_warningOn ();
			return result;
		} else if (strnequ (command, "noprogress ", 11)) {
			int result;
			Melder_progressOff ();
			result = praat_executeCommand (interpreter, command + 11);
			Melder_progressOn ();
			return result;
		} else if (strnequ (command, "nocheck ", 8)) {
			(void) praat_executeCommand (interpreter, command + 8);
			Melder_clearError ();
		} else if (strnequ (command, "pause", 5)) {
			int wasBackgrounding = Melder_backgrounding;
			structMelderDir dir;
			Melder_getDefaultDir (& dir);
			if (praat.batch) return 1;
			UiFile_hide ();
			if (wasBackgrounding) praat_foreground ();
			if (! Melder_pause (command + 5)) return Melder_error ("You interrupted the script.");
			if (wasBackgrounding) praat_background ();
			Melder_setDefaultDir (& dir);
			/* BUG: should also restore praatP. editor. */
		} else if (strnequ (command, "execute ", 8)) {
			praat_executeScriptFromFileNameWithArguments (command + 8);
		} else if (strnequ (command, "editor", 6)) {
			if (command [6] == ' ' && isalpha (command [7])) {
				praatP. editor = praat_findEditorFromString (command + 7);
				if (praatP. editor == NULL)
					return Melder_error ("Editor \"%s\" does not exist.", command + 7);
			} else if (interpreter && interpreter -> editorClass) {
				praatP. editor = praat_findEditorFromString (interpreter -> environmentName);
				if (praatP. editor == NULL)
					return Melder_error ("Editor \"%s\" does not exist.", interpreter -> environmentName);
			} else {
				return Melder_error ("No editor specified.");
			}
		} else if (strnequ (command, "endeditor", 9)) {
			praatP. editor = NULL;
		} else if (strnequ (command, "sendpraat ", 10)) {
			char programName [41], *q = & programName [0], *result;
			#ifdef macintosh
				#define SENDPRAAT_TIMEOUT  10
			#else
				#define SENDPRAAT_TIMEOUT  0
			#endif
			const char *p = command + 10;
			while (*p == ' ' || *p == '\t') p ++;
			while (*p != '\0' && *p != ' ' && *p != '\t' && q < programName + 39) *q ++ = *p ++;
			*q = '\0';
			if (q == programName)
				return Melder_error ("Missing program name after `sendpraat'.");
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0')
				return Melder_error ("Missing command after `sendpraat'.");
			result = sendpraat (XtDisplay (praat.topShell), programName, SENDPRAAT_TIMEOUT, p);
			if (result)
				return Melder_error ("%s\nMessage to %s not completed.", result, programName);
		} else if (strnequ (command, "sendsocket ", 11)) {
			char hostName [61], *q = & hostName [0], *result;
			const char *p = command + 11;
			while (*p == ' ' || *p == '\t') p ++;
			while (*p != '\0' && *p != ' ' && *p != '\t' && q < hostName + 59) *q ++ = *p ++;
			*q = '\0';
			if (q == hostName)
				return Melder_error ("Missing host name after `sendsocket'.");
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0')
				return Melder_error ("Missing command after `sendsocket'.");
			result = sendsocket (hostName, p);
			if (result)
				return Melder_error ("%s\nMessage to %s not completed.", result, hostName);
		} else if (strnequ (command, "filedelete ", 11)) {
			const char *p = command + 11;
			structMelderFile file;
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0')
				return Melder_error ("Missing file name after `filedelete'.");
			if (! Melder_relativePathToFile (p, & file)) return 0;
			MelderFile_delete (& file);
		} else if (strnequ (command, "fileappend ", 11)) {
			const char *p = command + 11;
			char path [256], *q = & path [0];
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0')
				return Melder_error ("Missing file name after `fileappend'.");
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
				structMelderFile file;
				if (! Melder_relativePathToFile (path, & file)) return 0;
				if (! MelderFile_appendText (& file, p + 1)) return 0;
			}
		} else {
			if (strnequ (command, "getinfostring ", 14))
				return Melder_error ("Command \"getinfostring\" no longer supported. Consult scripting tutorial or the author.");
			if (strnequ (command, "getinfonumber ", 14))
				return Melder_error ("Command \"getinfonumber\" no longer supported. Consult scripting tutorial or the author.");
			if (strnequ (command, "getnameofselected ", 18))
				return Melder_error ("Command \"getnameofselected\" no longer supported. Instead use things like: name$ = selected$ (\"Sound\")");
			if (strnequ (command, "getidofselected ", 16))
				return Melder_error ("Command \"getidofselected\" no longer supported. Instead use things like: id = selected (\"Sound\")");
			if (strnequ (command, "getnumberofselected ", 20))
				return Melder_error ("Command \"getnumberofselected\" no longer supported. Instead use things like: n = numberOfSelected (\"Sound\")");
			if (strnequ (command, "getnumber ", 10))
				return Melder_error ("Command \"getnumber\" no longer supported. Instead use things like: \"variable = Menu command\".");
			if (strnequ (command, "getstring ", 10))
				return Melder_error ("Command \"getstring\" no longer supported. Instead use things like: \"variable$ = Menu command\".");
			if (strnequ (command, "let ", 4))
				return Melder_error ("Command \"let\" no longer supported. Instead use things like: \"variable = numeric-formula\" or \"variable$ = string-formula\".");
			if (strnequ (command, "proc ", 5))
				return Melder_error ("Command \"proc\" no longer supported. Instead use \"procedure\", and distinguish between numeric variables and string variables.");
			if (strnequ (command, "copy ", 5))
				return Melder_error ("Command \"copy x y\" no longer supported. Instead use normal assignments, i.e. y = x.");
			return Melder_error ("Unknown command: %s", command);
		}
	} else {   /* Simulate menu choice. */
		char *arguments;

 		/* Parse command line into command and arguments. */
		/* The separation is formed by the three dots. */

		if ((arguments = strstr (command, "...")) == NULL || strlen (arguments) < 4) {
			arguments = "";
		} else {
			arguments += 4;
			if (arguments [-1] != ' ' && arguments [-1] != '0') {
				return Melder_error ("There should be a space after the three dots.");
			}
			arguments [-1] = '\0'; /* New end of "command". */
		}

		/* See if command exists and is available; ignore separators. */
		/* First try loose commands, then fixed commands. */

		UiInterpreter_set (interpreter);
		if (praatP. editor) {
			if (! Editor_doMenuCommand (praatP. editor, command, arguments)) {
				UiInterpreter_set (NULL);
				return 0;
			}
		} else if (! praat_doAction (command, arguments)) {
			if (Melder_hasError ()) {
				UiInterpreter_set (NULL);
				if (arguments [0] != '\0' && command [strlen (arguments) - 1] == ' ') {
					return Melder_error ("It may be helpful to remove the trailing spaces.");
				}
				return 0;
			}
			if (! praat_doMenuCommand (command, arguments)) {
				UiInterpreter_set (NULL);
				if (Melder_hasError ()) {
					if (arguments [0] != '\0' && command [strlen (arguments) - 1] == ' ') {
						return Melder_error ("It may be helpful to remove the trailing spaces.");
					}
					return 0;
				} else if (strnequ (command, "ARGS ", 5)) {
					return Melder_error ("Command \"ARGS\" no longer supported. Instead use \"form\" and \"endform\".");
				} else if (strchr (command, '=')) {
					return Melder_error ("Command \"%s\" not recognized.\n"
						"Probable cause: you are trying to use a variable name that starts with a capital.", command);
				} else if (command [0] != '\0' && command [strlen (command) - 1] == ' ') {
					return Melder_error ("Command \"%s\" not available for current selection. "
						"It may be helpful to remove the trailing spaces.", command);
				} else {
					return Melder_error ("Command \"%s\" not available for current selection.", command);
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
		if (! praat_executeCommand (NULL, command))
			Melder_flushError ("%s: command \"%s\" not executed.", programName, command);
	}
	return 1;
}

int praat_executeScriptFromFile (MelderFile file, const char *arguments) {
	char *text = NULL;
	Interpreter interpreter = NULL;
	structMelderDir saveDir;
	Melder_getDefaultDir (& saveDir);   /* Before the first cherror! */

	text = MelderFile_readText (file); cherror
	MelderFile_setDefaultDir (file);   /* So that relative file names can be used inside the script. */
	Melder_includeIncludeFiles (& text, 1); cherror
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
	iferror return Melder_error ("Script \"%s\" not completed.", MelderFile_messageName (file));
	return 1;
}

int praat_executeScriptFromFileNameWithArguments (const char *nameAndArguments) {
	char path [256];
	const char *p, *arguments;
	structMelderFile file;
	/*
	 * Split into file name and arguments.
	 */
	p = nameAndArguments;
	while (*p == ' ' || *p == '\t') p ++;
	if (*p == '\"') {
		char *q = path;
		p ++;
		while (*p != '\"' && *p != '\0') * q ++ = * p ++;
		*q = '\0';
		arguments = p; if (*p) arguments ++;
	} else {
		sscanf (p, "%s", path);
		arguments = p + strlen (path);
	}
	if (! Melder_relativePathToFile (path, & file)) return 0;
	return praat_executeScriptFromFile (& file, arguments);
}

int praat_executeScriptFromText (char *text) {
	Interpreter interpreter = Interpreter_create (NULL, NULL);
	Interpreter_run (interpreter, text);
	forget (interpreter);
	iferror return Melder_error ("Script not completed.");
	return 1;
}

int praat_executeScriptFromDialog (Any dia) {
	Interpreter interpreter = NULL;
	structMelderFile file;
	char *text = NULL, *path = UiForm_getString (dia, "$file");
	structMelderDir saveDir;
	Melder_getDefaultDir (& saveDir);

	Melder_pathToFile (path, & file); cherror
	text = MelderFile_readText (& file); cherror
	MelderFile_setDefaultDir (& file);
	Melder_includeIncludeFiles (& text, 1); cherror
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
	char *text = NULL;
	Interpreter interpreter = NULL;
	structMelderDir saveDir;
	Melder_getDefaultDir (& saveDir);

	text = MelderFile_readText (file); cherror
	MelderFile_setDefaultDir (file);
	Melder_includeIncludeFiles (& text, 1); cherror
	Melder_setDefaultDir (& saveDir);
	interpreter = Interpreter_createFromEnvironment (praatP.editor);
	if (Interpreter_readParameters (interpreter, text) > 0) {
		Any form = Interpreter_createForm (interpreter, praat.topShell, Melder_fileToPath (file), secondPassThroughScript, NULL);
		UiForm_destroyWhenUnmanaged (form);
		UiForm_do (form, 0);
	} else {
		praat_background ();
		praat_executeScriptFromFile (file, NULL);
		praat_foreground ();
	}
end:
	Melder_setDefaultDir (& saveDir);
	Melder_free (text);
	forget (interpreter);
	iferror return Melder_error ("Script \"%s\" not completed.", MelderFile_messageName (file));
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
			file_dialog = UiInfile_create (praat.topShell, "Praat: run script", fileSelectorOkCallback, NULL, 0);
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
	structMelderFile file;
	(void) dummy;
	if (! Melder_relativePathToFile ((char *) scriptPath, & file)) return 0;
	return firstPassThroughScript (& file);
}

int DO_RunTheScriptFromAnyAddedEditorCommand (Any editor, const char *script) {
	praatP.editor = editor;
	DO_RunTheScriptFromAnyAddedMenuCommand ((Any) script, NULL);
	/*praatP.editor = NULL;*/
	iferror return 0;
	return 1;
}

/* End of file praat_script.c */
