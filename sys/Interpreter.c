/* Interpreter.c
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
 * pb 2002/03/25 option menus
 * pb 2002/06/04 include the script compiler
 * pb 2002/09/26 removed bug: crashed if a line in a form contained only the word "comment"
 * pb 2002/11/25 Melder_double
 * pb 2002/12/10 include files
 * pb 2002/12/14 more informative error messages
 * pb 2003/05/19 Melder_atof
 * pb 2003/07/15 assert
 * pb 2003/07/19 if undefined fails
 * pb 2004/10/16 C++ compatible structs
 * pb 2004/12/06 made Interpreter_getArgumentsFromDialog resistant to changes in the script while the dialog is up
 * pb 2005/01/01 there can be spaces before the "form" statement
 * pb 2005/11/26 allow mixing of "option" and "button", as in Ui.c
 * pb 2006/01/11 local variables
 */

#include <ctype.h>
#include "Interpreter.h"
#include "praatP.h"
#include "praat_script.h"
#include "Formula.h"

#define Interpreter_WORD 1
#define Interpreter_REAL 2
#define Interpreter_POSITIVE 3
#define Interpreter_INTEGER 4
#define Interpreter_NATURAL 5
#define Interpreter_BOOLEAN 6
#define Interpreter_SENTENCE 7
#define Interpreter_TEXT 8
#define Interpreter_CHOICE 9
#define Interpreter_OPTIONMENU 10
#define Interpreter_BUTTON 11
#define Interpreter_OPTION 12
#define Interpreter_COMMENT 13

static void classInterpreterVariable_destroy (I) {
	iam (InterpreterVariable);
	Melder_free (my key);
	Melder_free (my stringValue);
	inherited (InterpreterVariable) destroy (me);
}

class_methods (InterpreterVariable, Thing)
	class_method_local (InterpreterVariable, destroy)
class_methods_end

static InterpreterVariable InterpreterVariable_create (const char *key) {
	if (key [0] == 'e' && key [1] == '\0')
		return Melder_errorp ("You cannot use 'e' as the name of a variable (e is the constant 2.71...).");
	if (key [0] == 'p' && key [1] == 'i' && key [2] == '\0')
		return Melder_errorp ("You cannot use 'pi' as the name of a variable (pi is the constant 3.14...).");
	if (key [0] == 'u' && key [1] == 'n' && key [2] == 'd' && key [3] == 'e' && key [4] == 'f' && key [5] == 'i' &&
		key [6] == 'n' && key [7] == 'e' && key [8] == 'd' && key [9] == '\0')
		return Melder_errorp ("You cannot use 'undefined' as the name of a variable.");
	{
		InterpreterVariable me = new (InterpreterVariable);
		if (! me || ! (my key = Melder_strdup (key))) { forget (me); return NULL; }
		return me;
	}
}

static void classInterpreter_destroy (I) {
	iam (Interpreter);
	int ipar;
	Melder_free (my environmentName);
	for (ipar = 1; ipar <= Interpreter_MAXNUM_PARAMETERS; ipar ++)
		Melder_free (my arguments [ipar]);
	forget (my variables);
	inherited (Interpreter) destroy (me);
}

class_methods (Interpreter, Thing)
	class_method_local (Interpreter, destroy)
class_methods_end

Interpreter Interpreter_create (char *environmentName, Any editorClass) {
	Interpreter me = new (Interpreter);
	if (! me || ! (my variables = SortedSetOfString_create ())) { forget (me); return NULL; }
	my environmentName = Melder_strdup (environmentName);
	my editorClass = editorClass;
	return me;
}

Interpreter Interpreter_createFromEnvironment (Any editor) {
	if (editor == NULL) return Interpreter_create (NULL, NULL);
	return Interpreter_create (((Editor) editor) -> name, ((Editor) editor) -> methods);
}

int Melder_includeIncludeFiles (char **text, int allocationMethod) {
	int depth;
	for (depth = 0; ; depth ++) {
		char *head = *text;
		long numberOfIncludes = 0;
		if (depth > 10)
			return Melder_error ("Include files nested too deep. Probably cyclic.");
		for (;;) {
			structMelderFile includeFile;
			char *includeLocation, *includeFileName, *includeText, *tail, *newText;
			long headLength, includeTextLength, newLength;
			/*
				Look for an include statement. If not found, we have finished.
			 */
			includeLocation = strnequ (head, "include ", 8) ? head : strstr (head, "\ninclude ");
			if (includeLocation == NULL) break;
			if (includeLocation != head) includeLocation += 1;
			numberOfIncludes += 1;
			/*
				Separate out the head.
			 */
			*includeLocation = '\0';
			/*
				Separate out the name of the include file.
			 */
			includeFileName = includeLocation + 8;
			while (*includeFileName == ' ' || *includeFileName == '\t') includeFileName ++;
			tail = includeFileName;
			while (*tail != '\n' && *tail != '\0') tail ++;
			if (*tail == '\n') {
				*tail = '\0';
				tail += 1;
			}
			/*
				Get the contents of the include file.
			 */
			if (! Melder_relativePathToFile (includeFileName, & includeFile)) return 0;
			includeText = MelderFile_readText (& includeFile);
			if (! includeText) {
				Melder_error ("Include file \"%s\" not read.", MelderFile_messageName (& includeFile));
				goto end;
			}
			/*
				Construct the new text.
			 */
			headLength = (head - *text) + strlen (head);
			includeTextLength = strlen (includeText);
			newLength = headLength + includeTextLength + 1 + strlen (tail);
			newText = allocationMethod == 1 ? Melder_malloc (newLength + 1) : XtCalloc (1, newLength + 1);
			if (! newText) { Melder_free (includeText); cherror }
			strcpy (newText, *text);
			strcpy (newText + headLength, includeText);
			strcpy (newText + headLength + includeTextLength, "\n");
			strcpy (newText + headLength + includeTextLength + 1, tail);
			/*
				Replace the old text with the new.
			 */
			if (allocationMethod == 1) Melder_free (*text); else XtFree (*text);
			*text = newText;
			/*
				Clean up.
			 */
			Melder_free (includeText);
			/*
				Cycle.
			 */
			head = *text + headLength + includeTextLength + 1;
		}
		if (numberOfIncludes == 0) break;
	}
end:
	iferror return 0;
	return 1;
}

int Interpreter_readParameters (Interpreter me, char *text) {
	char *formLocation = NULL;
	int npar = 0;
	my dialogTitle [0] = '\0';
	/*
	 * Look for a "form" line.
	 */
	{
		char *p = text;
		for (;;) {
			while (*p == ' ' || *p == '\t') p ++;
			if (strnequ (p, "form ", 5)) {
				formLocation = p;
				break;
			}
			while (*p != '\0' && *p != '\n') p ++;
			if (*p == '\0') break;
			p ++;   /* Skip newline symbol. */
		}
	}
	/*
	 * If there is no "form" line, there are no parameters.
	 */
	if (formLocation) {
		char *dialogTitle = formLocation + 5, *newLine;
		while (*dialogTitle == ' ' || *dialogTitle == '\t') dialogTitle ++;
		newLine = strchr (dialogTitle, '\n');
		if (newLine) *newLine = '\0';
		strcpy (my dialogTitle, dialogTitle);
		if (newLine) *newLine = '\n';
		my numberOfParameters = 0;
		while (newLine) {
			char *line = newLine + 1, *p;
			int type = 0;
			while (*line == ' ' || *line == '\t') line ++;
			if (strnequ (line, "endform", 7)) break;
			if (strnequ (line, "word ", 5)) { type = Interpreter_WORD; p = line + 5; }
			else if (strnequ (line, "real ", 5)) { type = Interpreter_REAL; p = line + 5; }
			else if (strnequ (line, "positive ", 9)) { type = Interpreter_POSITIVE; p = line + 9; }
			else if (strnequ (line, "integer ", 8)) { type = Interpreter_INTEGER; p = line + 8; }
			else if (strnequ (line, "natural ", 8)) { type = Interpreter_NATURAL; p = line + 8; }
			else if (strnequ (line, "boolean ", 8)) { type = Interpreter_BOOLEAN; p = line + 8; }
			else if (strnequ (line, "sentence ", 9)) { type = Interpreter_SENTENCE; p = line + 9; }
			else if (strnequ (line, "text ", 5)) { type = Interpreter_TEXT; p = line + 5; }
			else if (strnequ (line, "choice ", 7)) { type = Interpreter_CHOICE; p = line + 7; }
			else if (strnequ (line, "optionmenu ", 11)) { type = Interpreter_OPTIONMENU; p = line + 11; }
			else if (strnequ (line, "button ", 7)) { type = Interpreter_BUTTON; p = line + 7; }
			else if (strnequ (line, "option ", 7)) { type = Interpreter_OPTION; p = line + 7; }
			else if (strnequ (line, "comment ", 8)) { type = Interpreter_COMMENT; p = line + 8; }
			else {
				newLine = strchr (line, '\n');
				if (newLine) *newLine = '\0';
				Melder_error ("Unknown parameter type:\n\"%s\".", line);
				if (newLine) *newLine = '\n';
				return 0;
			}
			/*
				Example:
					form Something
						real Time_(s) 3.14 (= pi)
						choice Colour 2
							button Red
							button Green
							button Blue
					endform
				my parameters [1] := "Time_(s)"
				my parameters [2] := "Colour"
				my parameters [3] := ""
				my parameters [4] := ""
				my parameters [5] := ""
				my arguments [1] := "3.14 (= pi)"
				my arguments [2] := "2"
				my arguments [3] := "Red"   (funny, but needed in Interpreter_getArgumentsFromString)
				my arguments [4] := "Green"
				my arguments [5] := "Blue"
			*/
			if (type <= Interpreter_OPTIONMENU) {
				while (*p == ' ' || *p == '\t') p ++;
				if (*p == '\n' || *p == '\0') return Melder_error ("Missing parameter:\n\"%s\".", line);
				sscanf (p, "%s", my parameters [++ my numberOfParameters]);
				p += strlen (my parameters [my numberOfParameters]);
				npar ++;
			} else {
				my parameters [++ my numberOfParameters] [0] = '\0';
			}
			while (*p == ' ' || *p == '\t') p ++;
			newLine = strchr (p, '\n');
			if (newLine) *newLine = '\0';
			Melder_free (my arguments [my numberOfParameters]);
			my arguments [my numberOfParameters] = Melder_strdup (p);
			if (newLine) *newLine = '\n';
			my types [my numberOfParameters] = type;
		}
	} else {
		npar = my numberOfParameters = 0;
	}
	return npar;
}

Any Interpreter_createForm (Interpreter me, Widget parent, const char *path, int (*okCallback) (Any dia, void *closure), void *okClosure) {
	Any form = UiForm_create (parent, my dialogTitle [0] ? my dialogTitle : "Script arguments", okCallback, okClosure, NULL);
	int ipar;
	Any radio = NULL;
	if (path) UiForm_addText (form, "$file", path);
	for (ipar = 1; ipar <= my numberOfParameters; ipar ++) {
		/*
		 * Convert underscores to spaces.
		 */
		char parameter [100], *p = & parameter [0];
		strcpy (parameter, my parameters [ipar]);
		while (*p) { if (*p == '_') *p = ' '; p ++; }
		switch (my types [ipar]) {
			case Interpreter_WORD:
				UiForm_addWord (form, parameter, my arguments [ipar]); break;
			case Interpreter_REAL:
				UiForm_addReal (form, parameter, my arguments [ipar]); break;
			case Interpreter_POSITIVE:
				UiForm_addPositive (form, parameter, my arguments [ipar]); break;
			case Interpreter_INTEGER:
				UiForm_addInteger (form, parameter, my arguments [ipar]); break;
			case Interpreter_NATURAL:
				UiForm_addNatural (form, parameter, my arguments [ipar]); break;
			case Interpreter_BOOLEAN:
				UiForm_addBoolean (form, parameter, my arguments [ipar] [0] == '1' ||
					my arguments [ipar] [0] == 'y' || my arguments [ipar] [0] == 'Y' ||
					my arguments [ipar] [0] == 'o' && my arguments [ipar] [1] == 'n'); break;
			case Interpreter_SENTENCE:
				UiForm_addSentence (form, parameter, my arguments [ipar]); break;
			case Interpreter_TEXT:
				UiForm_addText (form, parameter, my arguments [ipar]); break;
			case Interpreter_CHOICE:
				radio = UiForm_addRadio (form, parameter, atoi (my arguments [ipar])); break;
			case Interpreter_OPTIONMENU:
				radio = UiForm_addOptionMenu (form, parameter, atoi (my arguments [ipar])); break;
			case Interpreter_BUTTON:
				if (radio) UiRadio_addButton (radio, my arguments [ipar]); break;
			case Interpreter_OPTION:
				if (radio) UiOptionMenu_addButton (radio, my arguments [ipar]); break;
			case Interpreter_COMMENT:
				UiForm_addLabel (form, parameter, my arguments [ipar]); break;
			default:
				UiForm_addWord (form, parameter, my arguments [ipar]); break;
		}
		/*
		 * Strip parentheses and colon off parameter name.
		 */
		if ((p = strchr (my parameters [ipar], '(')) != NULL) {
			*p = '\0';
			if (p - my parameters [ipar] > 0 && p [-1] == '_') p [-1] = '\0';
		}
		p = my parameters [ipar];
		if (*p != '\0' && p [strlen (p) - 1] == ':') p [strlen (p) - 1] = '\0';
	}
	UiForm_finish (form);
	return form;
}

int Interpreter_getArgumentsFromDialog (Interpreter me, Any dialog) {
	int ipar;
	for (ipar = 1; ipar <= my numberOfParameters; ipar ++) {
		char parameter [100], *p;
		/*
		 * Strip parentheses and colon off parameter name.
		 */
		if ((p = strchr (my parameters [ipar], '(')) != NULL) {
			*p = '\0';
			if (p - my parameters [ipar] > 0 && p [-1] == '_') p [-1] = '\0';
		}
		p = my parameters [ipar];
		if (*p != '\0' && p [strlen (p) - 1] == ':') p [strlen (p) - 1] = '\0';
		/*
		 * Convert underscores to spaces.
		 */
		strcpy (parameter, my parameters [ipar]);
		p = & parameter [0]; while (*p) { if (*p == '_') *p = ' '; p ++; }
		switch (my types [ipar]) {
			case Interpreter_REAL:
			case Interpreter_POSITIVE: {
				double value = UiForm_getReal_check (dialog, parameter); cherror
				Melder_free (my arguments [ipar]);
				my arguments [ipar] = Melder_calloc (1, 40);
				strcpy (my arguments [ipar], Melder_double (value));
				break;
			}
			case Interpreter_INTEGER:
			case Interpreter_NATURAL:
			case Interpreter_BOOLEAN: {
				long value = UiForm_getInteger (dialog, parameter); cherror
				Melder_free (my arguments [ipar]);
				my arguments [ipar] = Melder_calloc (1, 40);
				sprintf (my arguments [ipar], "%ld", value);
				break;
			}
			case Interpreter_CHOICE:
			case Interpreter_OPTIONMENU: {
				long integerValue = 0;
				char *stringValue = NULL;
				integerValue = UiForm_getInteger (dialog, parameter); cherror
				stringValue = UiForm_getString (dialog, parameter); cherror
				Melder_free (my arguments [ipar]);
				my arguments [ipar] = Melder_calloc (1, 40);
				sprintf (my arguments [ipar], "%ld", integerValue);
				strcpy (my choiceArguments [ipar], stringValue);
				break;
			}
			case Interpreter_BUTTON:
			case Interpreter_OPTION:
			case Interpreter_COMMENT:
				break;
			default: {
				char *value = UiForm_getString (dialog, parameter); cherror
				Melder_free (my arguments [ipar]);
				my arguments [ipar] = Melder_strdup (value);
				break;
			}
		}
	}
end:
	iferror return 0;
	return 1;
}

int Interpreter_getArgumentsFromString (Interpreter me, const char *arguments) {
	int ipar, size = my numberOfParameters;
	long length = strlen (arguments);
	while (size >= 1 && my parameters [size] [0] == '\0')
		size --;   /* Ignore fields without a variable name (button, comment). */
	for (ipar = 1; ipar <= size; ipar ++) {
		char *p = my parameters [ipar];
		/*
		 * Ignore buttons and comments again.
		 */
		if (! *p) continue;
		/*
		 * Strip parentheses and colon off parameter name.
		 */
		if ((p = strchr (p, '(')) != NULL) {
			*p = '\0';
			if (p - my parameters [ipar] > 0 && p [-1] == '_') p [-1] = '\0';
		}
		p = my parameters [ipar];
		if (*p != '\0' && p [strlen (p) - 1] == ':') p [strlen (p) - 1] = '\0';
	}
	for (ipar = 1; ipar < size; ipar ++) {
		int ichar = 0;
		/*
		 * Ignore buttons and comments again. The buttons will keep their labels as "arguments".
		 */
		if (my parameters [ipar] [0] == '\0') continue;
		Melder_free (my arguments [ipar]);   /* Erase the current values, probably the default values. */
		my arguments [ipar] = Melder_calloc (1, length + 1);   /* Replace with the actual arguments. */
		/*
		 * Skip spaces until next argument.
		 */
		while (*arguments == ' ' || *arguments == '\t') arguments ++;
		/*
		 * The argument is everything up to the next space, or, if that starts with a double quote,
		 * everything between this quote and the matching double quote;
		 * in this case, the argument can represent a double quote by a sequence of two double quotes.
		 * Example: the string
		 *     "I said ""hello"""
		 * will be passed to the dialog as a single argument containing the text
		 *     I said "hello"
		 */
		if (*arguments == '\"') {
			arguments ++;   /* Do not include leading double quote. */
			for (;;) {
				if (*arguments == '\0') return Melder_error ("Missing matching quote.");
				if (*arguments == '\"' && * ++ arguments != '\"') break;   /* Remember second quote. */
				my arguments [ipar] [ichar ++] = *arguments ++;
			}
		} else {
			while (*arguments != ' ' && *arguments != '\t' && *arguments != '\0')
				my arguments [ipar] [ichar ++] = *arguments ++;
		}
		my arguments [ipar] [ichar] = '\0';   /* Trailing null byte. */
	}
	/* The last item is handled separately, because it consists of the rest of the line.
	 * Leading spaces are skipped, but trailing spaces are included.
	 */
	if (size > 0) {
		while (*arguments == ' ' || *arguments == '\t') arguments ++;
		Melder_free (my arguments [size]);
		my arguments [size] = Melder_strdup (arguments);
	}
	/*
	 * Convert booleans and choices to numbers.
	 */
	for (ipar = 1; ipar <= size; ipar ++) {
		if (my types [ipar] == Interpreter_BOOLEAN) {
			char *arg = & my arguments [ipar] [0];
			if (strequ (arg, "1") || strequ (arg, "yes") || strequ (arg, "on") ||
			    strequ (arg, "Yes") || strequ (arg, "On") || strequ (arg, "YES") || strequ (arg, "ON"))
			{
				strcpy (arg, "1");
			} else if (strequ (arg, "0") || strequ (arg, "no") || strequ (arg, "off") ||
			    strequ (arg, "No") || strequ (arg, "Off") || strequ (arg, "NO") || strequ (arg, "OFF"))
			{
				strcpy (arg, "0");
			} else {
				return Melder_error ("Unknown value \"%s\" for boolean \"%s\".", arg, my parameters [ipar]);
			}
		} else if (my types [ipar] == Interpreter_CHOICE) {
			int jpar;
			char *arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_BUTTON && my types [jpar] != Interpreter_OPTION)
					return Melder_error ("Unknown value \"%s\" for choice \"%s\".", arg, my parameters [ipar]);
				if (strequ (my arguments [jpar], arg)) {   /* The button labels are in the arguments, see Interpreter_readParameters */
					sprintf (arg, "%d", jpar - ipar);
					strcpy (my choiceArguments [ipar], my arguments [jpar]);
					break;
				}
			}
			if (jpar > my numberOfParameters)
				return Melder_error ("Unknown value \"%s\" for choice \"%s\".", arg, my parameters [ipar]);
		} else if (my types [ipar] == Interpreter_OPTIONMENU) {
			int jpar;
			char *arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_OPTION && my types [jpar] != Interpreter_BUTTON)
					return Melder_error ("Unknown value \"%s\" for option menu \"%s\".", arg, my parameters [ipar]);
				if (strequ (my arguments [jpar], arg)) {
					sprintf (arg, "%d", jpar - ipar);
					strcpy (my choiceArguments [ipar], my arguments [jpar]);
					break;
				}
			}
			if (jpar > my numberOfParameters)
				return Melder_error ("Unknown value \"%s\" for option menu \"%s\".", arg, my parameters [ipar]);
		}
	}
	return 1;
}

static int Interpreter_addNumericVariable (Interpreter me, const char *key, double value) {
	InterpreterVariable variable = InterpreterVariable_create (key);
	if (! variable || ! Collection_addItem (my variables, variable)) return 0;
	variable -> numericValue = value;
	return 1;
}

static InterpreterVariable Interpreter_addStringVariable (Interpreter me, const char *key, const char *value) {
	InterpreterVariable variable = InterpreterVariable_create (key);
	if (! variable || ! Collection_addItem (my variables, variable)) return NULL;
	variable -> stringValue = Melder_strdup (value);
	return variable;
}

InterpreterVariable Interpreter_hasVariable (Interpreter me, const char *key) {
	long ivar = 0;
	char variableNameIncludingProcedureName [1+200];
	Melder_assert (key != NULL);
	if (key [0] == '.') {
		strcpy (variableNameIncludingProcedureName, my procedureNames [my callDepth]);
		strcat (variableNameIncludingProcedureName, key);
	} else {
		strcpy (variableNameIncludingProcedureName, key);
	}
	ivar = SortedSetOfString_lookUp (my variables, variableNameIncludingProcedureName);
	return ivar ? my variables -> item [ivar] : NULL;
}

InterpreterVariable Interpreter_lookUpVariable (Interpreter me, const char *key) {
	InterpreterVariable var = NULL;
	char variableNameIncludingProcedureName [1+200];
	Melder_assert (key != NULL);
	if (key [0] == '.') {
		strcpy (variableNameIncludingProcedureName, my procedureNames [my callDepth]);
		strcat (variableNameIncludingProcedureName, key);
	} else {
		strcpy (variableNameIncludingProcedureName, key);
	}
	var = Interpreter_hasVariable (me, variableNameIncludingProcedureName);
	if (var) return var;
	var = InterpreterVariable_create (variableNameIncludingProcedureName);
	if (! var || ! Collection_addItem (my variables, var)) return NULL;
	return Interpreter_hasVariable (me, variableNameIncludingProcedureName);
}

static long lookupLabel (Interpreter me, const char *labelName) {
	int ilabel;
	for (ilabel = 1; ilabel <= my numberOfLabels; ilabel ++)
		if (strequ (labelName, my labelNames [ilabel]))
			return ilabel;
	return Melder_error ("Unknown label \"%s\".", labelName);
}

static int isAnObjectName (const char *p) {
	while (isalnum (*p)) p ++;
	return *p == '_';
}

static void parameterToVariable (Interpreter me, int type, const char *in_parameter, int ipar) {
	char parameter [200];
	Melder_assert (type != 0);
	strcpy (parameter, in_parameter);
	if (type >= Interpreter_REAL && type <= Interpreter_BOOLEAN) {
		Interpreter_addNumericVariable (me, parameter, atof (my arguments [ipar]));
	} else if (type == Interpreter_CHOICE || type == Interpreter_OPTIONMENU) {
		Interpreter_addNumericVariable (me, parameter, atof (my arguments [ipar]));
		strcat (parameter, "$");
		Interpreter_addStringVariable (me, parameter, my choiceArguments [ipar]);
	} else if (type == Interpreter_BUTTON || type == Interpreter_OPTION || type == Interpreter_COMMENT) {
		/* Do not add a variable. */
	} else {
		strcat (parameter, "$");
		Interpreter_addStringVariable (me, parameter, my arguments [ipar]);
	}
}

int Interpreter_run (Interpreter me, char *text) {
	static char valueString [30000];   /* To divert the info. */
	char *command = text, command2 [28000];
	char **lines = NULL;
	long lineNumber = 0, numberOfLines = 0, callStack [1 + Interpreter_MAX_CALL_DEPTH];
	int atLastLine = FALSE, fromif = FALSE, fromendfor = FALSE, callDepth = 0, chopped = 0, ipar, assertionFailed = FALSE;
	InterpreterVariable var;
	my callDepth = 0;
	/*
	 * The "environment" is NULL if we are in the Praat shell, or an editor otherwise.
	 */
	if (my editorClass) {
		praatP. editor = praat_findEditorFromString (my environmentName);
		if (praatP. editor == NULL)
			return Melder_error ("Editor \"%s\" does not exist.", my environmentName);
	} else {
		praatP. editor = NULL;
	}
	/*
	 * Count lines and set the newlines to zero.
	 */
	while (! atLastLine) {
		char *endOfLine = command;
		while (*endOfLine != '\n' && *endOfLine != '\0') endOfLine ++;
		if (*endOfLine == '\0') atLastLine = TRUE;
		*endOfLine = '\0';
		numberOfLines ++;
		command = endOfLine + 1;
	}
	/*
	 * Remember line starts and labels.
	 */
	lines = NUMpvector (1, numberOfLines); cherror
	for (lineNumber = 1, command = text; lineNumber <= numberOfLines; lineNumber ++, command += strlen (command) + 1 + chopped) {
		int length;
		while (*command == ' ' || *command == '\t') command ++;
		length = strlen (command);
		/*
		 * Chop trailing spaces?
		 */
		/*chopped = 0;
		while (length > 0) { char kar = command [-- length]; if (kar != ' ' && kar != '\t') break; command [length] = '\0'; chopped ++; }*/
		lines [lineNumber] = command;
		if (strnequ (command, "label ", 6)) {
			int ilabel;
			for (ilabel = 1; ilabel <= my numberOfLabels; ilabel ++)
				if (strequ (command + 6, my labelNames [ilabel]))
					{ Melder_error ("Duplicate label \"%s\".", command + 6); goto end; }
			if (my numberOfLabels >= Interpreter_MAXNUM_LABELS)
				{ Melder_error ("Too many labels."); goto end; }
			sprintf (my labelNames [++ my numberOfLabels], "%.47s", command + 6);
			my labelLines [my numberOfLabels] = lineNumber;
		}
	}
	/*
	 * Connect continuation lines.
	 */
	for (lineNumber = numberOfLines; lineNumber >= 2; lineNumber --) {
		char *line = lines [lineNumber];
		if (line [0] == '.' && line [1] == '.' && line [2] == '.') {
			char *previous = lines [lineNumber - 1];
			strcpy (command2, line + 3);
			strcat (previous, command2);
			lines [lineNumber] = "";
		}
	}
	/*
	 * Copy the parameter names and argument values into the array of variables.
	 */
	forget (my variables);
	my variables = SortedSetOfString_create ();
	for (ipar = 1; ipar <= my numberOfParameters; ipar ++) {
		char parameter [200];
		/*
		 * Create variable names as-are and variable names without capitals.
		 */
		strcpy (parameter, my parameters [ipar]);
		parameterToVariable (me, my types [ipar], parameter, ipar); cherror
		if (parameter [0] >= 'A' && parameter [0] <= 'Z') {
			parameter [0] = tolower (parameter [0]);
			parameterToVariable (me, my types [ipar], parameter, ipar); cherror
		}
	}
	/*
	 * Initialize some variables.
	 */
	Interpreter_addStringVariable (me, "newline$", "\n");
	Interpreter_addStringVariable (me, "tab$", "\t");
	Interpreter_addStringVariable (me, "shellDirectory$", Melder_getShellDirectory ());
	{
		structMelderDir dir;
		Melder_getDefaultDir (& dir);
		var = Interpreter_addStringVariable (me, "defaultDirectory$", Melder_dirToPath (& dir));
	}
	/*
	 * Execute commands.
	 */
	#define wordEnd(c)  (c == '\0' || c == ' ' || c == '\t')
	for (lineNumber = 1; lineNumber <= numberOfLines; lineNumber ++) {
		int c0, fail = FALSE;
		char *p;
		strcpy (command2, lines [lineNumber]);
		c0 = command2 [0];
		/*
		 * Substitute variables.
		 */
		for (p = command2; *p !='\0'; p ++) if (*p == '\'') {
			/*
			 * Found a left quote. Search for a matching right quote.
			 */
			char *q = p + 1, varName [300], *r, *s, *colon;
			int precision = -1, percent = FALSE;
			while (*q != '\0' && *q != '\'') q ++;
			if (*q == '\0') break;   /* No matching right quote: done with this line. */
			if (q - p == 1) continue;   /* Ignore empty variable names. */
			/*
			 * Found a right quote. Get potential variable name.
			 */
			for (r = p + 1, s = varName; q - r > 0; r ++, s ++) *s = *r;
			*s = '\0';   /* Trailing null byte. */
			colon = strchr (varName, ':');
			if (colon) {
				precision = atoi (colon + 1);
				if (strchr (colon + 1, '%')) percent = TRUE;
				*colon = '\0';
			}
			var = Interpreter_hasVariable (me, varName);
			if (var) {
				/*
				 * Found a variable. Substitute.
				 */
				int varlen = (q - p) - 1, headlen = p - command2;
				int arglen;
				const char *string = var -> stringValue ? var -> stringValue :
					percent ? Melder_percent (var -> numericValue, precision) :
					precision >= 0 ?  Melder_fixed (var -> numericValue, precision) :
					Melder_double (var -> numericValue);
				arglen = strlen (string);
				strncpy (Melder_buffer1, command2, headlen);
				strcpy (Melder_buffer1 + headlen, string);
				strcpy (Melder_buffer1 + headlen + arglen, p + varlen + 2);
				strcpy (command2, Melder_buffer1);
				p += arglen - 1;
			} else {
				p = q - 1;   /* Go to before next quote. */
			}
		}
		c0 = command2 [0];   /* Resume in order to allow things like 'c$' = 5 */
		if ((c0 < 'a' || c0 > 'z') && ! (c0 == '.' && command2 [1] >= 'a' && command2 [1] <= 'z')) {
			praat_executeCommand (me, command2); cherror
		/*
		 * Interpret control flow and variables.
		 */
		} else switch (c0) {
			case '.':
				fail = TRUE;
				break;
			case 'a':
				if (strnequ (command2, "assert ", 7)) {
					double value;
					Interpreter_numericExpression (me, command2 + 7, & value); cherror
					if (value == 0.0 || value == NUMundefined) {
						assertionFailed = TRUE;
						Melder_error ("Script assertion fails in line %ld (%s):\n   %s", lineNumber, value ? "undefined" : "false", command2 + 7);
						goto end;
					}
				} else fail = TRUE;
				break;
			case 'b':
				fail = TRUE;
				break;
			case 'c':
				if (strnequ (command2, "call ", 5)) {
					char *p = command2 + 5, *callName, *procName;
					long iline;
					int hasArguments, callLength;
					while (*p == ' ' || *p == '\t') p ++;
					callName = p;
					while (*p != '\0' && *p != ' ' && *p != '\t') p ++;
					if (p == callName) { Melder_error ("Missing procedure name after 'call'."); goto end; }
					hasArguments = *p != '\0';
					*p = '\0';   /* Close procedure name. */
					callLength = strlen (callName);
					for (iline = 1; iline <= numberOfLines; iline ++) {
						char *linei = lines [iline], *q;
						int hasParameters;
						if (linei [0] != 'p' || linei [1] != 'r' || linei [2] != 'o' || linei [3] != 'c' ||
						    linei [4] != 'e' || linei [5] != 'd' || linei [6] != 'u' || linei [7] != 'r' ||
						    linei [8] != 'e' || linei [9] != ' ') continue;
						q = lines [iline] + 10;
						while (*q == ' ' || *q == '\t') q ++;
						procName = q;
						while (*q != '\0' && *q != ' ' && *q != '\t') q ++;
						if (q == procName) { Melder_error ("Missing procedure name after 'procedure'."); goto end; }
						hasParameters = *q != '\0';
						if (q - procName == callLength && strnequ (procName, callName, callLength)) {
							if (hasArguments && ! hasParameters)
									{ Melder_error ("Call to procedure \"%s\" has too many arguments.", callName); goto end; }
							else if (hasParameters && ! hasArguments)
									{ Melder_error ("Call to procedure \"%s\" has too few arguments.", callName); goto end; }
							if (++ my callDepth > Interpreter_MAX_CALL_DEPTH) {
								Melder_error ("Call depth greater than %d.", Interpreter_MAX_CALL_DEPTH);
								goto end;
							}
							strcpy (my procedureNames [my callDepth], callName);
							if (hasParameters) {
								++ p;   /* First argument. */
								++ q;   /* First parameter. */
								while (*q) {
									char *par, save, arg [1000], *to = & arg [0];
									while (*p == ' ' || *p == '\t') p ++;
									while (*q == ' ' || *q == '\t') q ++;
									par = q;
									while (*q != '\0' && *q != ' ' && *q != '\t') q ++;   /* Collect parameter name. */
									if (*q) {   /* Does anything follow the parameter name? */
										if (*p == '\"') {
											p ++;   /* Skip initial quote. */
											while (*p != '\0') {
												if (*p == '\"') {   /* Quote signals end-of-string or string-internal quote. */
													if (p [1] == '\"') {   /* Double quote signals string-internal quote. */
														*to ++ = '\"';
														p += 2;   /* Skip second quote. */
													} else {   /* Single quote signals end-of-string. */
														break;
													}
												} else {
													*to ++ = *p ++;
												}
											}
										} else {
											while (*p != '\0' && *p != ' ' && *p != '\t') *to ++ = *p ++;   /* White space separates. */
										}
										if (*p) { *p = '\0'; p ++; }
									} else {   /* Else rest of line. */
										while (*p != '\0') *to ++ = *p ++;
									}
									*to = '\0';
									if (q [-1] == '$') {
										save = *q; *q = '\0'; var = Interpreter_lookUpVariable (me, par); *q = save; cherror
										Melder_free (var -> stringValue);
										var -> stringValue = Melder_strdup (arg);
									} else {
										double value;
										my callDepth --;
										Interpreter_numericExpression (me, arg, & value);
										my callDepth ++;
										save = *q; *q = '\0'; var = Interpreter_lookUpVariable (me, par); *q = save; cherror
										var -> numericValue = value;
									}
								}
							}
							if (callDepth == Interpreter_MAX_CALL_DEPTH)
								{ Melder_error ("Call depth greater than %d.", Interpreter_MAX_CALL_DEPTH); goto end; }
							callStack [++ callDepth] = lineNumber;
							lineNumber = iline;
							break;
						}
					}
					if (iline > numberOfLines) { Melder_error ("Procedure \"%s\" not found.", callName); goto end; }
				} else fail = TRUE;
				break;
			case 'd':
				if (strnequ (command2, "dec ", 4)) {
					var = Interpreter_lookUpVariable (me, command2 + 4); cherror
					var -> numericValue -= 1.0;
				} else fail = TRUE;
				break;
			case 'e':
				if (command2 [1] == 'n' && command2 [2] == 'd') {
					if (strnequ (command2, "endif", 5) && wordEnd (command2 [5])) {
						/* Ignore. */
					} else if (strnequ (command2, "endfor", 6) && wordEnd (command2 [6])) {
						int depth = 0;
						long iline;
						for (iline = lineNumber - 1; iline > 0; iline --) {
							char *line = lines [iline];
							if (line [0] == 'f' && line [1] == 'o' && line [2] == 'r' && line [3] == ' ') {
								if (depth == 0) { lineNumber = iline - 1; fromendfor = TRUE; break; }   /* Go before 'for'. */
								else depth --;
							} else if (strnequ (lines [iline], "endfor", 6) && wordEnd (lines [iline] [6])) {
								depth ++;
							}
						}
						if (iline <= 0) { Melder_error ("Unmatched 'endfor'."); goto end; }
					} else if (strnequ (command2, "endwhile", 8) && wordEnd (command2 [8])) {
						int depth = 0;
						long iline;
						for (iline = lineNumber - 1; iline > 0; iline --) {
							if (strnequ (lines [iline], "while ", 6)) {
								if (depth == 0) { lineNumber = iline - 1; break; }   /* Go before 'while'. */
								else depth --;
							} else if (strnequ (lines [iline], "endwhile", 8) && wordEnd (lines [iline] [8])) {
								depth ++;
							}
						}
						if (iline <= 0) { Melder_error ("Unmatched 'endwhile'."); goto end; }
					} else if (strnequ (command2, "endproc", 7) && wordEnd (command2 [7])) {
						if (callDepth == 0) { Melder_error ("Unmatched 'endproc'."); goto end; }
						lineNumber = callStack [callDepth --];
						-- my callDepth;
					} else fail = TRUE;
				} else if (strnequ (command2, "else", 4) && wordEnd (command2 [4])) {
					int depth = 0;
					long iline;
					for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
						if (strnequ (lines [iline], "endif", 5) && wordEnd (lines [iline] [5])) {
							if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endif'. */
							else depth --;
						} else if (strnequ (lines [iline], "if ", 3)) {
							depth ++;
						}
					}
					if (iline > numberOfLines) { Melder_error ("Unmatched 'else'."); goto end; }
				} else if (strnequ (command2, "elsif ", 6) || strnequ (command2, "elif ", 5)) {
					if (fromif) {
						double value;
						fromif = FALSE;
						Interpreter_numericExpression (me, command2 + 5, & value); cherror
						if (value == 0.0) {
							int depth = 0;
							long iline;
							for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
								if (strnequ (lines [iline], "endif", 5) && wordEnd (lines [iline] [5])) {
									if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endif'. */
									else depth --;
								} else if (strnequ (lines [iline], "else", 4) && wordEnd (lines [iline] [4])) {
									if (depth == 0) { lineNumber = iline; break; }   /* Go after 'else'. */
								} else if (strnequ (lines [iline], "elsif", 5) && wordEnd (lines [iline] [5])
									|| strnequ (lines [iline], "elif", 4) && wordEnd (lines [iline] [4])) {
									if (depth == 0) { lineNumber = iline - 1; fromif = TRUE; break; }   /* Go at next 'elsif' or 'elif'. */
								} else if (strnequ (lines [iline], "if ", 3)) {
									depth ++;
								}
							}
							if (iline > numberOfLines) { Melder_error ("Unmatched 'elsif'."); goto end; }
						}
					} else {
						int depth = 0;
						long iline;
						for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
							if (strnequ (lines [iline], "endif", 5) && wordEnd (lines [iline] [5])) {
								if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endif'. */
								else depth --;
							} else if (strnequ (lines [iline], "if ", 3)) {
								depth ++;
							}
						}
						if (iline > numberOfLines) { Melder_error ("'elsif' not matched with 'endif'."); goto end; }
					}
				} else if (strnequ (command2, "exit", 4)) {
					if (command2 [4] == '\0') {
						lineNumber = numberOfLines;   /* Go after end. */
					} else {
						Melder_error ("%s", command2 + 5);
						goto end;
					}
				} else fail = TRUE;
				break;
			case 'f':
				if (command2 [1] == 'o' && command2 [2] == 'r' && command2 [3] == ' ') {   /* for_ */
					double toValue, loopVariable;
					char *frompos = strstr (command2, " from "), *topos = strstr (command2, " to ");
					char *varpos = command2 + 4, *endvar = frompos;
					if (! topos) { Melder_error ("Missing \'to\' in \'for\' loop."); goto end; }
					if (! endvar) endvar = topos;
					while (*endvar == ' ') { *endvar = '\0'; endvar --; }
					while (*varpos == ' ') varpos ++;
					if (endvar - varpos < 0) { Melder_error ("Missing loop variable after \'for\'."); goto end; }
					var = Interpreter_lookUpVariable (me, varpos);
					Interpreter_numericExpression (me, topos + 4, & toValue); cherror
					if (fromendfor) {
						fromendfor = FALSE;
						loopVariable = var -> numericValue + 1.0;
					} else if (frompos) {
						*topos = '\0';
						Interpreter_numericExpression (me, frompos + 6, & loopVariable); cherror
					} else {
						loopVariable = 1.0;
					}
					var -> numericValue = loopVariable;
					if (loopVariable > toValue) {
						int depth = 0;
						long iline;
						for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
							if (strnequ (lines [iline], "endfor", 6)) {
								if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endfor'. */
								else depth --;
							} else if (strnequ (lines [iline], "for ", 4)) {
								depth ++;
							}
						}
						if (iline > numberOfLines) { Melder_error ("Unmatched 'for'."); goto end; }
					}
				} else if (strnequ (command2, "form ", 5)) {
					long iline;
					for (iline = lineNumber + 1; iline <= numberOfLines; iline ++)
						if (strnequ (lines [iline], "endform", 7))
							{ lineNumber = iline; break; }   /* Go after 'endform'. */
					if (iline > numberOfLines) { Melder_error ("Unmatched 'form'."); goto end; }
				} else fail = TRUE;
				break;
			case 'g':
				if (strnequ (command2, "goto ", 5)) {
					char labelName [50], *space;
					int dojump = TRUE, ilabel;
					sprintf (labelName, "%.47s", command2 + 5);
					space = strchr (labelName, ' ');
					if (space == labelName) { Melder_error ("Missing label name after 'goto'."); goto end; }
					if (space) {
						double value;
						*space = '\0';
						Interpreter_numericExpression (me, command2 + 6 + strlen (labelName), & value); cherror
						if (value == 0.0) dojump = FALSE;
					}
					ilabel = lookupLabel (me, labelName); cherror
					if (dojump) lineNumber = my labelLines [ilabel];   /* Loop will add 1. */
				} else fail = TRUE;
				break;
			case 'h':
				fail = TRUE;
				break;
			case 'i':
				if (command2 [1] == 'f' && command2 [2] == ' ') {   /* if_ */
					double value;
					Interpreter_numericExpression (me, command2 + 3, & value); cherror
					if (value == 0.0) {
						int depth = 0;
						long iline;
						for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
							if (strnequ (lines [iline], "endif", 5)) {
								if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endif'. */
								else depth --;
							} else if (strnequ (lines [iline], "else", 4)) {
								if (depth == 0) { lineNumber = iline; break; }   /* Go after 'else'. */
							} else if (strnequ (lines [iline], "elsif ", 6) || strnequ (lines [iline], "elif ", 5)) {
								if (depth == 0) { lineNumber = iline - 1; fromif = TRUE; break; }   /* Go at 'elsif'. */
							} else if (strnequ (lines [iline], "if ", 3)) {
								depth ++;
							}
						}
						if (iline > numberOfLines) { Melder_error ("Unmatched 'if'."); goto end; }
					} else if (value == NUMundefined) {
						Melder_error ("The value of the 'if' condition is undefined."); goto end;
					}
				} else if (strnequ (command2, "inc ", 4)) {
					var = Interpreter_lookUpVariable (me, command2 + 4); cherror
					var -> numericValue += 1.0;
				} else fail = TRUE;
				break;
			case 'j':
				fail = TRUE;
				break;
			case 'k':
				fail = TRUE;
				break;
			case 'l':
				if (strnequ (command2, "label ", 6)) {
					;   /* Ignore labels. */
				} else fail = TRUE;
				break;
			case 'm':
				fail = TRUE;
				break;
			case 'n':
				fail = TRUE;
				break;
			case 'o':
				fail = TRUE;
				break;
			case 'p':
				if (strnequ (command2, "procedure ", 10)) {
					long iline;
					for (iline = lineNumber + 1; iline <= numberOfLines; iline ++)
						if (strnequ (lines [iline], "endproc", 7) && wordEnd (lines [iline] [7]))
							{ lineNumber = iline; break; }   /* Go after 'endproc'. */
					if (iline > numberOfLines) { Melder_error ("Unmatched 'proc'."); goto end; }
				} else fail = TRUE;
				break;
			case 'q':
				fail = TRUE;
				break;
			case 'r':
				if (strnequ (command2, "repeat", 6) && wordEnd (command2 [6])) {
					/* Ignore. */
				} else fail = TRUE;
				break;
			case 's':
				if (strnequ (command2, "stopwatch", 9) && wordEnd (command2 [9])) {
					(void) Melder_stopwatch ();   /* Reset stopwatch. */
				} else fail = TRUE;
				break;
			case 't':
				fail = TRUE;
				break;
			case 'u':
				if (strnequ (command2, "until ", 6)) {
					double value;
					Interpreter_numericExpression (me, command2 + 6, & value); cherror
					if (value == 0.0) {
						int depth = 0;
						long iline;
						for (iline = lineNumber - 1; iline > 0; iline --) {
							if (strnequ (lines [iline], "repeat", 6) && wordEnd (lines [iline] [6])) {
								if (depth == 0) { lineNumber = iline; break; }   /* Go after 'repeat'. */
								else depth --;
							} else if (strnequ (lines [iline], "until ", 6)) {
								depth ++;
							}
						}
						if (iline <= 0) { Melder_error ("Unmatched 'until'."); goto end; }
					}
				} else fail = TRUE;
				break;
			case 'v':
				fail = TRUE;
				break;
			case 'w':
				if (strnequ (command2, "while ", 6)) {
					double value;
					Interpreter_numericExpression (me, command2 + 6, & value); cherror
					if (value == 0.0) {
						int depth = 0;
						long iline;
						for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
							if (strnequ (lines [iline], "endwhile", 8) && wordEnd (lines [iline] [8])) {
								if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endwhile'. */
								else depth --;
							} else if (strnequ (lines [iline], "while ", 6)) {
								depth ++;
							}
						}
						if (iline > numberOfLines) { Melder_error ("Unmatched 'while'."); goto end; }
					}
				} else fail = TRUE;
				break;
			case 'x':
				fail = TRUE;
				break;
			case 'y':
				fail = TRUE;
				break;
			case 'z':
				fail = TRUE;
				break;
			default: break;
		}
		if (fail) {
			/*
			 * Found an unknown word starting with a lower-case letter, optionally preced by a period.
			 * See whether the word is a variable name.
			 */
			char *p = & command2 [0];
			/*
			 * Variable names consist of a sequence of letters, digits, and underscores,
			 * optionally precede by a period and optionally followed by a $.
			 */
			if (*p == '.') p ++;
			while (isalnum (*p) || *p == '_' || *p == '.')  p ++;
			if (*p == '$') {
				/*
				 * Assign to a string variable.
				 */
				char *value, *endOfVariable = ++ p;
				int withFile;
				while (*p == ' ' || *p == '\t') p ++;   /* Go to first token after variable name. */
				if (*p == '=') {
					withFile = 0;   /* Assignment. */
				} else if (*p == '<') {
					withFile = 1;   /* Read from file. */
				} else if (*p == '>') {
					if (p [1] == '>')
						withFile = 2, p ++;   /* Append to file. */
					else
						withFile = 3;   /* Write to file. */
				} else { Melder_error ("Missing '=', '<', or '>' after variable %s.", command2); goto end; }
				*endOfVariable = '\0';
				p ++;
				while (*p == ' ' || *p == '\t') p ++;   /* Go to first token after assignment or I/O symbol. */		
				if (*p == '\0') {
					if (withFile)
						{ Melder_error ("Missing file name after variable %s.", command2); goto end; }
					else
						{ Melder_error ("Missing expression after variable %s.", command2); goto end; }
				}
				if (withFile) {
					structMelderFile file;
					Melder_relativePathToFile (p, & file); cherror
					if (withFile == 1) {
						value = MelderFile_readText (& file); cherror
						var = Interpreter_lookUpVariable (me, command2); cherror
						Melder_free (var -> stringValue);
						var -> stringValue = value;   /* var becomes owner */
					} else if (withFile == 2) {
						var = Interpreter_hasVariable (me, command2); cherror
						if (! var) { Melder_error ("Variable %s undefined.", command2); goto end; }
						MelderFile_appendText (& file, var -> stringValue); cherror
					} else {
						var = Interpreter_hasVariable (me, command2); cherror
						if (! var) { Melder_error ("Variable %s undefined.", command2); goto end; }
						MelderFile_writeText (& file, var -> stringValue); cherror
					}
				} else if (isupper (*p) && ! isAnObjectName (p)) {
					/*
					 * Example: name$ = Get name
					 */
					Melder_divertInfo (valueString);
					praat_executeCommand (me, p);
					Melder_divertInfo (NULL); cherror
					var = Interpreter_lookUpVariable (me, command2); cherror
					Melder_free (var -> stringValue);
					var -> stringValue = Melder_strdup (valueString); cherror   /* var becomes owner */
				} else {
					/*
					 * Evaluate a string expression and assign the result to the variable.
					 * Examples:
					 *    sentence$ = subject$ + verb$ + object$
					 *    extension$ = if index (file$, ".") <> 0
					 *       ... then right$ (file$, length (file$) - rindex (file$, "."))
					 *       ... else "" fi
					 */
					Interpreter_stringExpression (me, p, & value); cherror
					var = Interpreter_lookUpVariable (me, command2); cherror
					Melder_free (var -> stringValue);
					var -> stringValue = value;   /* var becomes owner */
				}
			} else {
				/*
				 * Try to assign to a numeric variable.
				 */
				double value;
				int typeOfAssignment = 0;   /* Plain assignment. */
				if (*p == '\0') {
					/*
					 * Command ends here: it may be a PraatShell command.
					 */
					praat_executeCommand (me, command2); cherror
					continue;
				}
				if (*p == '=' || (*p == '+' || *p == '-' || *p == '*' || *p == '/') && p [1] == '=') {
					/*
					 * This must be an assignment (though: "echo= ..." ???)
					 */
					typeOfAssignment = *p == '+' ? 1 : *p == '-' ? 2 : *p == '*' ? 3 : *p == '/' ? 4 : 0;
					*p = '\0';   /* Close variable name. */
				} else {
					char *endOfVariable = p;
					p ++;
					while (*p == ' ' || *p == '\t') p ++;
					if (*p == '=' || (*p == '+' || *p == '-' || *p == '*' || *p == '/') && p [1] == '=') {
						/*
						 * This must be an assignment (though: "echo = ..." ???)
						 */
						typeOfAssignment = *p == '+' ? 1 : *p == '-' ? 2 : *p == '*' ? 3 : *p == '/' ? 4 : 0;
						*endOfVariable = '\0';   /* Close variable name. */
					} else {
						/*
						 * Not an assignment: perhaps a PraatShell command (select, echo, execute, pause ...).
						 */
						praat_executeCommand (me, command2); cherror
						continue;
					}
				}
				p += typeOfAssignment == 0 ? 1 : 2;
				while (*p == ' ' || *p == '\t') p ++;			
				if (*p == '\0') { Melder_error ("Missing expression after variable %s.", command2); goto end; }
				/*
				 * Two classes of assignments:
				 *    var = formula
				 *    var = Query
				 */
				if (isupper (*p) && ! isAnObjectName (p)) {
					/*
					 * Get the value of the query.
					 */
					Melder_divertInfo (valueString);
					praat_executeCommand (me, p);
					Melder_divertInfo (NULL); cherror
					value = strnequ (valueString, "--undefined--", 13) ? NUMundefined : Melder_atof (valueString);
				} else {
					/*
					 * Get the value of the formula.
					 */
					Interpreter_numericExpression (me, p, & value); cherror
				}
				/*
				 * Assign the value to a variable.
				 */
				if (typeOfAssignment == 0) {
					/*
					 * Use an existing variable, or create a new one.
					 */
					var = Interpreter_lookUpVariable (me, command2); cherror
					var -> numericValue = value;
				} else {
					/*
					 * Modify an existing variable.
					 */
					var = Interpreter_hasVariable (me, command2); cherror
					if (var == NULL)
						{ Melder_error ("Unknown variable %s.", command2); goto end; }
					if (var -> numericValue == NUMundefined) {
						/* Keep it that way. */
					} else {
						if (typeOfAssignment == 1) {
							var -> numericValue += value;
						} else if (typeOfAssignment == 2) {
							var -> numericValue -= value;
						} else if (typeOfAssignment == 3) {
							var -> numericValue *= value;
						} else if (value == 0) {
							var -> numericValue = NUMundefined;
						} else {
							var -> numericValue /= value;
						}
					}
				}
			}
		}
	}
end:
	iferror {
		if (! strnequ (lines [lineNumber], "exit ", 5) && ! assertionFailed) {   /* Don't show the message twice! */
			while (lines [lineNumber] [0] == '\0') {   /* Did this use to be a continuation line? */
				lineNumber --;
				Melder_assert (lineNumber > 0);   /* Originally empty lines that stayed empty should not generate errors. */
			}
			Melder_error ("Script line %ld not performed or completed:\n\\<< %s \\>>", lineNumber, lines [lineNumber]);
		}
	}
	NUMpvector_free (lines, 1);
	my numberOfLabels = 0;
	iferror return 0;
	return 1;
}

int Interpreter_numericExpression (Interpreter me, const char *expression, double *result) {
	Melder_assert (result != NULL);
	if (strstr (expression, "(=")) { *result = Melder_atof (expression); return 1; }
	if (! Formula_compile (me, NULL, expression, 0, FALSE)) return 0;
	return Formula_run (0, 0, result, NULL);
}

int Interpreter_stringExpression (Interpreter me, const char *expression, char **result) {
	if (! Formula_compile (me, NULL, expression, 1, FALSE)) return 0;
	return Formula_run (0, 0, NULL, result);
}

int Interpreter_numericOrStringExpression (Interpreter me, const char *expression, double *numericResult, char **stringResult) {
	if (! Formula_compile (me, NULL, expression, 2, FALSE)) return 0;
	return Formula_run (0, 0, numericResult, stringResult);
}

/* End of file Interpreter.c */
