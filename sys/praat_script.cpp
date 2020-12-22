/* praat_script.cpp
 *
 * Copyright (C) 1993-2020 Paul Boersma
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
#include "sendpraat.h"
#include "sendsocket.h"
#include "UiPause.h"
#include "DemoEditor.h"

static int praat_findObjectFromString (Interpreter interpreter, conststring32 string) {
	try {
		int IOBJECT;
		while (*string == U' ') string ++;
		if (*string >= U'A' && *string <= U'Z') {
			/*
				Find the object by its name.
			*/
			static MelderString buffer;
			MelderString_copy (& buffer, string);
			char32 *space = str32chr (buffer.string, U' ');
			if (! space)
				Melder_throw (U"Missing space in name.");
			*space = U'\0';
			char32 *className = & buffer.string [0], *givenName = space + 1;
			WHERE_DOWN (1) {
				Daata object = (Daata) OBJECT;
				if (str32equ (className, Thing_className (OBJECT)) && str32equ (givenName, object -> name.get()))
					return IOBJECT;
			}
			/*
				No object with that name. Perhaps the class name was wrong?
			*/
			ClassInfo klas = Thing_classFromClassName (className, nullptr);
			WHERE_DOWN (1) {
				Daata object = (Daata) OBJECT;
				if (str32equ (klas -> className, Thing_className (OBJECT)) && str32equ (givenName, object -> name.get()))
					return IOBJECT;
			}
			Melder_throw (U"No object with that name.");
		} else {
			/*
				Find the object by its ID.
			*/
			double value;
			Interpreter_numericExpression (interpreter, string, & value);
			integer id = (integer) value;
			WHERE (ID == id)
				return IOBJECT;
			Melder_throw (U"No object with number ", id, U".");
		}
	} catch (MelderError) {
		Melder_throw (U"Object \"", string, U"\" does not exist.");
	}
}

Editor praat_findEditorFromString (conststring32 string) {
	int IOBJECT;
	while (*string == U' ')
		string ++;
	if (*string >= U'A' && *string <= U'Z') {
		WHERE_DOWN (1) {
			for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor editor = theCurrentPraatObjects -> list [IOBJECT]. editors [ieditor];
				if (editor) {
					Melder_assert (editor -> name);
					const char32 *space = str32chr (editor -> name.get(), U' ');   // editors tend to be called like "3. Sound kanweg"
					if (space) {   // but not all
						conststring32 name = space + 1;
						if (str32equ (name, string))
							return editor;
					}
				}
			}
		}
	} else {
		WHERE_DOWN (1) {
			for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor editor = theCurrentPraatObjects -> list [IOBJECT]. editors [ieditor];
				if (editor && str32equ (editor -> name.get(), string))
					return editor;
			}
		}
	}
	Melder_throw (U"Editor \"", string, U"\" does not exist.");
}

Editor praat_findEditorById (integer id) {
	int IOBJECT;
	WHERE (1) {
		if (ID == id) {
			for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor editor = theCurrentPraatObjects -> list [IOBJECT]. editors [ieditor];
				if (editor)
					return editor;
			}
		}
	}
	Melder_throw (U"Editor ", id, U" does not exist.");
}

static int parseCommaSeparatedArguments (Interpreter interpreter, char32 *arguments, structStackel *args) {
	int narg = 0, depth = 0;
	for (char32 *p = arguments; ; p ++) {
		bool endOfArguments = *p == U'\0';
		if (endOfArguments || (*p == U',' && depth == 0)) {
			if (narg == MAXIMUM_NUMBER_OF_FIELDS)
				Melder_throw (U"Cannot have more than ", MAXIMUM_NUMBER_OF_FIELDS, U" arguments");
			*p = U'\0';
			Formula_Result result;
			Interpreter_anyExpression (interpreter, arguments, & result);
			narg ++;
			/*
				First remove the old contents.
			*/
			args [narg]. reset();
			#if STACKEL_VARIANTS_ARE_PACKED_IN_A_UNION
				memset (& args [narg], 0, sizeof (structStackel));
			#endif
			/*
				Then copy in the new contents.
			*/
			switch (result. expressionType) {
				case kFormula_EXPRESSION_TYPE_NUMERIC: {
					args [narg]. which = Stackel_NUMBER;
					args [narg]. number = result. numericResult;
				} break; case kFormula_EXPRESSION_TYPE_STRING: {
					args [narg]. setString (result. stringResult.move());
				} break; case kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR: {
					args [narg]. which = Stackel_NUMERIC_VECTOR;
					args [narg]. numericVector = result. numericVectorResult;
					args [narg]. owned = result. owned;
					result. owned = false;
				} break; case kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX: {
					args [narg]. which = Stackel_NUMERIC_MATRIX;
					args [narg]. numericMatrix = result. numericMatrixResult;
					args [narg]. owned = result. owned;
					result. owned = false;
				} break; case kFormula_EXPRESSION_TYPE_STRING_ARRAY: {
					args [narg]. which = Stackel_STRING_ARRAY;
					args [narg]. stringArray = result. stringArrayResult;
					args [narg]. owned = result. owned;
					result. owned = false;
				} break;
			}
			arguments = p + 1;
		} else if (*p == U'(' || *p == U'[' || *p == U'{') {
			depth ++;
		} else if (*p == U')' || *p == U']' || *p == U'}') {
			depth --;
		} else if (*p == U'\"') {
			for (;;) {
				p ++;
				if (*p == U'\"') {
					if (p [1] == U'\"')
						p ++;
					else
						break;
				}
			}
		}
		if (endOfArguments)
			break;
	}
	return narg;
}

int praat_executeCommand (Interpreter interpreter, char32 *command) {
	static struct structStackel args [1 + MAXIMUM_NUMBER_OF_FIELDS];
	//trace (U"praat_executeCommand: ", Melder_pointer (interpreter), U": ", command);
	if (command [0] == U'\0' || command [0] == U'#' || command [0] == U'!' || command [0] == U';')
		/* Skip empty lines and comments. */;
	else if ((command [0] == U'.' || command [0] == U'+' || command [0] == U'-') && Melder_isAsciiUpperCaseLetter (command [1])) {   // selection?
		int IOBJECT = praat_findObjectFromString (interpreter, command + 1);
		if (command [0] == '.')
			praat_deselectAll ();
		if (command [0] == '-')
			praat_deselect (IOBJECT);
		else
			praat_select (IOBJECT); 
		praat_show ();
	} else if (Melder_isLetter (command [0]) && ! Melder_isUpperCaseLetter (command [0])) {   // all directives start with an ASCII lower-case letter
		if (str32nequ (command, U"select ", 7)) {
			if (str32nequ (command + 7, U"all", 3) && (command [10] == U'\0' || command [10] == U' ' || command [10] == U'\t')) {
				praat_selectAll ();
				praat_show ();
			} else {
				int IOBJECT = praat_findObjectFromString (interpreter, command + 7);
				praat_deselectAll ();
				praat_select (IOBJECT);
				praat_show ();
			}
		} else if (str32nequ (command, U"plus ", 5)) {
			int IOBJECT = praat_findObjectFromString (interpreter, command + 5);
			praat_select (IOBJECT);
			praat_show ();
		} else if (str32nequ (command, U"minus ", 6)) {
			int IOBJECT = praat_findObjectFromString (interpreter, command + 6);
			praat_deselect (IOBJECT);
			praat_show ();
		} else if (str32nequ (command, U"echo ", 5)) {
			MelderInfo_open ();
			MelderInfo_write (command + 5);
			MelderInfo_close ();
		} else if (str32nequ (command, U"clearinfo", 9)) {
			Melder_clearInfo ();
		} else if (str32nequ (command, U"print ", 6)) {
			MelderInfo_write (command + 6);
			MelderInfo_drain ();
		} else if (str32nequ (command, U"printtab", 8)) {
			MelderInfo_write (U"\t");
			MelderInfo_drain ();
		} else if (str32nequ (command, U"printline", 9)) {
			if (command [9] == ' ') MelderInfo_write (command + 10);
			MelderInfo_write (U"\n");
			MelderInfo_drain ();
		} else if (str32nequ (command, U"fappendinfo ", 12)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (U"The script command \"fappendinfo\" is not available inside pictures.");
			structMelderFile file { };
			Melder_relativePathToFile (command + 12, & file);
			MelderFile_appendText (& file, Melder_getInfo ());
		} else if (str32nequ (command, U"unix ", 5)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (U"The script command \"unix\" is not available inside manuals.");
			try {
				Melder_system (command + 5);
			} catch (MelderError) {
				Melder_throw (U"Unix command \"", command + 5, U"\" returned error status;\n"
					U"if you want to ignore this, use `unix_nocheck' instead of `unix'.");
			}
		} else if (str32nequ (command, U"unix_nocheck ", 13)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (U"The script command \"unix_nocheck\" is not available inside manuals.");
			try {
				Melder_system (command + 13);
			} catch (MelderError) {
				Melder_clearError ();
			}
		} else if (str32nequ (command, U"system ", 7)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (U"The script command \"system\" is not available inside manuals.");
			try {
				Melder_system (command + 7);
			} catch (MelderError) {
				Melder_throw (U"System command \"", command + 7, U"\" returned error status;\n"
					U"if you want to ignore this, use `system_nocheck' instead of `system'.");
			}
		} else if (str32nequ (command, U"system_nocheck ", 15)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (U"The script command \"system_nocheck\" is not available inside manuals.");
			try {
				Melder_system (command + 15);
			} catch (MelderError) {
				Melder_clearError ();
			}
		} else if (str32nequ (command, U"nowarn ", 7)) {
			autoMelderWarningOff nowarn;
			praat_executeCommand (interpreter, command + 7);
		} else if (str32nequ (command, U"noprogress ", 11)) {
			autoMelderProgressOff noprogress;
			praat_executeCommand (interpreter, command + 11);
		} else if (str32nequ (command, U"nocheck ", 8)) {
			try {
				praat_executeCommand (interpreter, command + 8);
			} catch (MelderError) {
				Melder_clearError ();
				return 0;
			}
		} else if (str32nequ (command, U"demo ", 5)) {
			autoDemoOpen demo;
			praat_executeCommand (interpreter, command + 5);
		} else if (str32nequ (command, U"asynchronous ", 13)) {
			autoMelderAsynchronous asynchronous;
			praat_executeCommand (interpreter, command + 13);
		} else if (str32nequ (command, U"pause ", 6) || str32equ (command, U"pause")) {
			if (theCurrentPraatApplication -> batch)
				return 1;   // in batch we ignore pause statements
			UiPause_begin (theCurrentPraatApplication -> topShell, U"stop or continue", interpreter);
			UiPause_comment (str32equ (command, U"pause") ? U"..." : command + 6);
			UiPause_end (1, 1, 0, U"Continue", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, interpreter);
		} else if (str32nequ (command, U"execute ", 8)) {
			praat_executeScriptFromFileNameWithArguments (command + 8);
		} else if (str32nequ (command, U"editor", 6)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (U"The script command \"editor\" is not available inside manuals.");
			if (command [6] == U' ' && Melder_isLetter (command [7])) {
				praatP. editor = praat_findEditorFromString (command + 7);
			} else if (command [6] == U'\0') {
				if (interpreter && interpreter -> editorClass) {
					praatP. editor = praat_findEditorFromString (interpreter -> environmentName.get());
				} else {
					Melder_throw (U"The function \"editor\" requires an argument when called from outside an editor.");
				}
			} else {
				Interpreter_voidExpression (interpreter, command);
			}
		} else if (str32nequ (command, U"endeditor", 9)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (U"The script command \"endeditor\" is not available inside manuals.");
			praatP. editor = nullptr;
		} else if (str32nequ (command, U"sendpraat ", 10)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (U"The script command \"sendpraat\" is not available inside manuals.");
			char32 programName [41], *q = & programName [0];
			#ifdef macintosh
				#define SENDPRAAT_TIMEOUT  10
			#else
				#define SENDPRAAT_TIMEOUT  0
			#endif
			const char32 *p = command + 10;
			while (*p == U' ' || *p == U'\t') p ++;
			while (*p != U'\0' && *p != U' ' && *p != U'\t' && q < programName + 39)
				*q ++ = *p ++;
			*q = U'\0';
			if (q == & programName [0])
				Melder_throw (U"Missing program name after `sendpraat'.");
			while (*p == U' ' || *p == U'\t') p ++;
			if (*p == U'\0')
				Melder_throw (U"Missing command after `sendpraat'.");
			#if motif
			char *result = sendpraat (XtDisplay (theCurrentPraatApplication -> topShell), Melder_peek32to8 (programName),
				SENDPRAAT_TIMEOUT, Melder_peek32to8 (p));
			if (result)
				Melder_throw (Melder_peek8to32 (result), U"\nMessage to ", programName, U" not completed.");
			#endif
		} else if (str32nequ (command, U"sendsocket ", 11)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (U"The script command \"sendsocket\" is not available inside manuals.");
			char32 hostName [61], *q = & hostName [0];
			const char32 *p = command + 11;
			while (*p == U' ' || *p == U'\t')
				p ++;
			while (*p != U'\0' && *p != U' ' && *p != U'\t' && q < hostName + 59)
				*q ++ = *p ++;
			*q = U'\0';
			if (q == hostName)
				Melder_throw (U"Missing host name after `sendsocket'.");
			while (*p == U' ' || *p == U'\t') p ++;
			if (*p == U'\0')
				Melder_throw (U"Missing command after `sendsocket'.");
			char *result = sendsocket (Melder_peek32to8 (hostName), Melder_peek32to8 (p));
			if (result)
				Melder_throw (Melder_peek8to32 (result), U"\nMessage to ", hostName, U" not completed.");
		} else if (str32nequ (command, U"filedelete ", 11)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (U"The script command \"filedelete\" is not available inside manuals.");
			const char32 *p = command + 11;
			structMelderFile file { };
			while (*p == U' ' || *p == U'\t') p ++;
			if (*p == U'\0')
				Melder_throw (U"Missing file name after `filedelete'.");
			Melder_relativePathToFile (p, & file);
			MelderFile_delete (& file);
		} else if (str32nequ (command, U"fileappend ", 11)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (U"The script command \"fileappend\" is not available inside manuals.");
			const char32 *p = command + 11;
			char32 path [kMelder_MAXPATH+1], *q = & path [0];
			while (*p == U' ' || *p == U'\t') p ++;
			if (*p == U'\0')
				Melder_throw (U"Missing file name after `fileappend'.");
			if (*p == '\"') {
				for (;;) {
					char32 kar = * ++ p;
					if (kar == U'\"') if (* ++ p == U'\"') *q ++ = U'\"'; else break;
					else if (kar == U'\0') break;
					else *q ++ = kar;
				}
			} else {
				for (;;) {
					char32 kar = * p;
					if (kar == U'\0' || kar == U' ' || kar == U'\t') break;
					*q ++ = kar;
					p ++;
				}
			}
			*q = U'\0';
			if (*p == U' ' || *p == U'\t') {
				structMelderFile file { };
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

		char32 *arguments = & command [0];
		for (arguments = & command [0]; *arguments != U'\0'; arguments ++) {
			if (*arguments == U':') {
				hasColon = true;
				if (arguments [1] == U'\0') {
					arguments = & arguments [1];   // empty string
				} else {
					if (arguments [1] != U' ') {
						Melder_throw (U"There should be a space after the colon.");
					}
					arguments [1] = U'\0';   // new end of "command"
					arguments += 2;   // the arguments start after the space
				}
				break;
			}
			if (*arguments == U'.' && arguments [1] == U'.' && arguments [2] == U'.') {
				hasDots = true;
				arguments += 3;
				if (*arguments == U'\0') {
					// empty string
				} else {
					if (*arguments != U' ') {
						Melder_throw (U"There should be a space after the three dots.");
					}
					*arguments = U'\0';   // new end of "command"
					arguments ++;   // the arguments start after the space
				}
				break;
			}
		}

		/* See if command exists and is available; ignore separators. */
		/* First try loose commands, then fixed commands. */

		integer narg;
		char32 command2 [200];
		if (hasColon) {
			narg = parseCommaSeparatedArguments (interpreter, arguments, args);
			str32cpy (command2, command);
			char32 *colon = str32chr (command2, U':');
			colon [0] = colon [1] = colon [2] = U'.';
			colon [3] = U'\0';
		}
		if (theCurrentPraatObjects == & theForegroundPraatObjects && praatP. editor) {
			if (hasColon) {
				Editor_doMenuCommand (praatP. editor, command2, narg, args, nullptr, interpreter);
			} else {
				Editor_doMenuCommand (praatP. editor, command, 0, nullptr, arguments, interpreter);
			}
		} else if (theCurrentPraatObjects != & theForegroundPraatObjects &&
		    (str32nequ (command, U"Save ", 5) ||
			 str32nequ (command, U"Write ", 6) ||
			 str32nequ (command, U"Append ", 7) ||
			 str32equ (command, U"Quit")))
		{
			Melder_throw (U"Commands that write files (including Quit) are not available inside manuals.");
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
				if (hasDots && arguments [0] != U'\0' && arguments [str32len (arguments) - 1] == U' ') {
					Melder_throw (U"It may be helpful to remove the trailing spaces in \"", arguments, U"\".");
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
					if (hasDots && arguments [0] != U'\0' && arguments [str32len (arguments) - 1] == U' ') {
						Melder_throw (U"It may be helpful to remove the trailing spaces in \"", arguments, U"\".");
					} else {
						throw;
					}
				}
				if (! theCommandIsAnExistingMenuCommand) {
					const integer length = str32len (command);
					if (str32nequ (command, U"ARGS ", 5)) {
						Melder_throw (U"Command \"ARGS\" no longer supported. Instead use \"form\" and \"endform\".");
					} else if (str32chr (command, U'=')) {
						Melder_throw (U"Command \"", command, U"\" not recognized.\n"
							U"Probable cause: you are trying to use a variable name that starts with a capital.");
					} else if (length >= 1 && Melder_isHorizontalSpace (command [length - 1])) {
						Melder_throw (U"Command \"", command, U"\" not available for current selection. "
							U"It may be helpful to remove the trailing spaces.");
					} else if (length >= 2 && Melder_isHorizontalSpace (command [length - 2]) && command [length - 1] == U':') {
						Melder_throw (U"Command \"", command, U"\" not available for current selection. "
							U"It may be helpful to remove the space before the colon.");
					} else if (str32nequ (command, U"\"ooTextFile\"", 12)) {
						Melder_throw (U"Command \"", command, U"\" not available for current selection. "
							U"It is possible that this file is not a Praat script but a Praat data file that you can open with \"Read from file...\".");
					} else {
						Melder_throw (U"Command \"", command, U"\" not available for current selection.");
					}
				}
			}
		}
		praat_updateSelection ();
	}
	return 1;
}

void praat_executeCommandFromStandardInput (conststring32 programName) {
	char command8 [1000];   // can be recursive
	/*
		FIXME: implement for Windows.
	*/
	for (;;) {
		printf ("%s > ", Melder_peek32to8 (programName));
		if (! fgets (command8, 999, stdin))
			Melder_throw (U"Cannot read input.");
		char *newLine = strchr (command8, '\n');
		if (newLine)
			*newLine = '\0';
		autostring32 command32 = Melder_8to32 (command8);
		try {
			praat_executeCommand (nullptr, command32.get());
		} catch (MelderError) {
			Melder_flushError (programName, U": Command \"", Melder_peek8to32 (command8), U"\" not executed.");
		}
	}
}

void praat_executeScriptFromFile (MelderFile file, conststring32 arguments) {
	try {
		autostring32 text = MelderFile_readText (file);
		autoMelderFileSetDefaultDir dir (file);   // so that relative file names can be used inside the script
		Melder_includeIncludeFiles (& text);
		autoInterpreter interpreter = Interpreter_createFromEnvironment (praatP.editor);
		if (arguments) {
			Interpreter_readParameters (interpreter.get(), text.get());
			Interpreter_getArgumentsFromString (interpreter.get(), arguments);
		}
		Interpreter_run (interpreter.get(), text.get());
	} catch (MelderError) {
		Melder_throw (U"Script ", file, U" not completed.");
	}
}

void praat_executeScriptFromFileName (conststring32 fileName, integer narg, Stackel args) {
	/*
		The argument 'fileName' is unsafe. Duplicate its contents.
	*/
	structMelderFile file { };
	Melder_relativePathToFile (fileName, & file);
	try {
		autostring32 text = MelderFile_readText (& file);
		autoMelderFileSetDefaultDir dir (& file);   // so that relative file names can be used inside the script
		Melder_includeIncludeFiles (& text);
		autoInterpreter interpreter = Interpreter_createFromEnvironment (praatP.editor);
		Interpreter_readParameters (interpreter.get(), text.get());
		Interpreter_getArgumentsFromArgs (interpreter.get(), narg, args);
		Interpreter_run (interpreter.get(), text.get());
	} catch (MelderError) {
		Melder_throw (U"Script ", & file, U" not completed.");   // don't refer to 'fileName', because its contents may have changed
	}
}

void praat_executeScriptFromFileNameWithArguments (conststring32 nameAndArguments) {
	char32 path [256];
	const char32 *p, *arguments;
	structMelderFile file { };
	/*
		Split into file name and arguments.
	*/
	p = nameAndArguments;
	while (*p == U' ' || *p == U'\t')
		p ++;
	if (*p == U'\"') {
		char32 *q = path;
		p ++;   // skip quote
		while (*p != U'\"' && *p != U'\0')
			* q ++ = * p ++;
		*q = U'\0';
		arguments = p;
		if (*arguments == U'\"')
			arguments ++;
		if (*arguments == U' ')
			arguments ++;
	} else {
		char32 *q = path;
		while (*p != U' ' && *p != U'\0')
			* q ++ = * p ++;
		*q = U'\0';
		arguments = p;
		if (*arguments == U' ')
			arguments ++;
	}
	Melder_relativePathToFile (path, & file);
	praat_executeScriptFromFile (& file, arguments);
}

extern "C" void praatlib_executeScript (const char *text8) {
	try {
		autoInterpreter interpreter = Interpreter_create (nullptr, nullptr);
		autostring32 string = Melder_8to32 (text8);
		Interpreter_run (interpreter.get(), string.get());
	} catch (MelderError) {
		Melder_throw (U"Script not completed.");
	}
}

void praat_executeScriptFromText (conststring32 text) {
	try {
		autoInterpreter interpreter = Interpreter_create (nullptr, nullptr);
		autostring32 string = Melder_dup (text);   // copy, because Interpreter will change it (UGLY)
		Interpreter_run (interpreter.get(), string.get());
	} catch (MelderError) {
		Melder_throw (U"Script not completed.");
	}
}

void praat_executeScriptFromDialog (UiForm dia) {
	char32 *path = UiForm_getString (dia, U"$file");
	structMelderFile file { };
	Melder_pathToFile (path, & file);
	autostring32 text = MelderFile_readText (& file);
	autoMelderFileSetDefaultDir dir (& file);
	Melder_includeIncludeFiles (& text);
	autoInterpreter interpreter = Interpreter_createFromEnvironment (praatP.editor);
	Interpreter_readParameters (interpreter.get(), text.get());
	Interpreter_getArgumentsFromDialog (interpreter.get(), dia);
	autoPraatBackground background;
	Interpreter_run (interpreter.get(), text.get());
}

static void secondPassThroughScript (UiForm sendingForm, integer /* narg */, Stackel /* args */,
	conststring32 /* sendingString_dummy */, Interpreter /* interpreter_dummy */,
	conststring32 /* invokingButtonTitle */, bool /* modified */, void *)
{
	praat_executeScriptFromDialog (sendingForm);
}

static void firstPassThroughScript (MelderFile file) {
	try {
		autostring32 text = MelderFile_readText (file);
		{// scope
			autoMelderFileSetDefaultDir dir (file);
			Melder_includeIncludeFiles (& text);
		}
		autoInterpreter interpreter = Interpreter_createFromEnvironment (praatP.editor);
		if (Interpreter_readParameters (interpreter.get(), text.get()) > 0) {
			autoUiForm form = Interpreter_createForm (interpreter.get(),
				praatP.editor ? praatP.editor -> windowForm : theCurrentPraatApplication -> topShell,
				Melder_fileToPath (file), secondPassThroughScript, nullptr, false
			);
			UiForm_destroyWhenUnmanaged (form.get());
			UiForm_do (form.get(), false);
			form. releaseToUser();
		} else {
			autoPraatBackground background;
			praat_executeScriptFromFile (file, nullptr);
		}
	} catch (MelderError) {
		Melder_throw (U"Script ", file, U" not completed.");
	}
}

static void fileSelectorOkCallback (UiForm dia, integer /* narg */, Stackel /* args */,
	conststring32 /* sendingString_dummy */, Interpreter /* interpreter_dummy */,
	conststring32 /* invokingButtonTitle */, bool /* modified */, void *)
{
	firstPassThroughScript (UiFile_getFile (dia));
}

void DO_RunTheScriptFromAnyAddedMenuCommand (UiForm /* sendingForm_dummy */, integer /* narg */, Stackel /* args */,
	conststring32 scriptPath, Interpreter /* interpreter */,
	conststring32 /* invokingButtonTitle */, bool /* modified */, void *)
{
	structMelderFile file { };
	Melder_relativePathToFile (scriptPath, & file);
	firstPassThroughScript (& file);
}

void DO_RunTheScriptFromAnyAddedEditorCommand (Editor editor, conststring32 script) {
	praatP.editor = editor;
	DO_RunTheScriptFromAnyAddedMenuCommand (nullptr, 0, nullptr, script, nullptr, nullptr, false, nullptr);
	/*praatP.editor = nullptr;*/
}

/* End of file praat_script.cpp */
