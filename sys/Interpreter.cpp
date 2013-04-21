/* Interpreter.cpp
 *
 * Copyright (C) 1993-2011,2013 Paul Boersma
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
 * pb 2007/02/05 preferencesDirectory$, homeDirectory$, temporaryDirectory$
 * pb 2007/04/02 allow comments (with '#' or ';' or empty lines) in forms
 * pb 2007/04/19 allow comments with '!' in forms
 * pb 2007/05/24 some wchar_t
 * pb 2007/06/09 wchar_t
 * pb 2007/08/12 more wchar_t
 * pb 2007/11/30 removed bug: allowed long arguments to the "call" statement (thanks to Ingmar Steiner)
 * pb 2007/12/10 predefined numeric variables macintosh/windows/unix
 * pb 2008/04/30 new Formula API
 * pb 2008/05/01 arrays
 * pb 2008/05/15 praatVersion, praatVersion$
 * pb 2009/01/04 Interpreter_voidExpression
 * pb 2009/01/17 arguments to UiForm callbacks
 * pb 2009/01/20 pause forms
 * pb 2009/03/17 split up structPraat
 * pb 2009/12/22 invokingButtonTitle
 * pb 2010/04/30 guard against leading nonbreaking spaces
 * pb 2011/05/14 C++
 */

#include <ctype.h>
#include "Interpreter.h"
#include "praatP.h"
extern structMelderDir praatDir;
#include "praat_script.h"
#include "Formula.h"
#include "praat_version.h"
#include "UnicodeData.h"

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

Thing_implement (InterpreterVariable, SimpleString, 0);

void structInterpreterVariable :: v_destroy () {
	Melder_free (string);
	Melder_free (stringValue);
	NUMmatrix_free (numericArrayValue. data, 1, 1);
	InterpreterVariable_Parent :: v_destroy ();
}

static InterpreterVariable InterpreterVariable_create (const wchar_t *key) {
	try {
		if (key [0] == 'e' && key [1] == '\0')
			Melder_throw ("You cannot use 'e' as the name of a variable (e is the constant 2.71...).");
		if (key [0] == 'p' && key [1] == 'i' && key [2] == '\0')
			Melder_throw ("You cannot use 'pi' as the name of a variable (pi is the constant 3.14...).");
		if (key [0] == 'u' && key [1] == 'n' && key [2] == 'd' && key [3] == 'e' && key [4] == 'f' && key [5] == 'i' &&
			key [6] == 'n' && key [7] == 'e' && key [8] == 'd' && key [9] == '\0')
			Melder_throw ("You cannot use 'undefined' as the name of a variable.");
		autoInterpreterVariable me = Thing_new (InterpreterVariable);
		my string = Melder_wcsdup (key);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Interpreter variable not created.");
	}
}

Thing_implement (Interpreter, Thing, 0);

void structInterpreter :: v_destroy () {
	Melder_free (environmentName);
	for (int ipar = 1; ipar <= Interpreter_MAXNUM_PARAMETERS; ipar ++)
		Melder_free (arguments [ipar]);
	forget (variables);
	Interpreter_Parent :: v_destroy ();
}

Interpreter Interpreter_create (wchar_t *environmentName, ClassInfo editorClass) {
	try {
		autoInterpreter me = Thing_new (Interpreter);
		my variables = SortedSetOfString_create ();
		my environmentName = Melder_wcsdup (environmentName);
		my editorClass = editorClass;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Interpreter not created.");
	}
}

Interpreter Interpreter_createFromEnvironment (Editor editor) {
	if (editor == NULL) return Interpreter_create (NULL, NULL);
	return Interpreter_create (editor -> name, editor -> classInfo);
}

void Melder_includeIncludeFiles (wchar_t **text) {
	for (int depth = 0; ; depth ++) {
		wchar_t *head = *text;
		long numberOfIncludes = 0;
		if (depth > 10)
			Melder_throw ("Include files nested too deep. Probably cyclic.");
		for (;;) {
			wchar_t *includeLocation, *includeFileName, *tail, *newText;
			long headLength, includeTextLength, newLength;
			/*
				Look for an include statement. If not found, we have finished.
			 */
			includeLocation = wcsnequ (head, L"include ", 8) ? head : wcsstr (head, L"\ninclude ");
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
			structMelderFile includeFile = { 0 };
			Melder_relativePathToFile (includeFileName, & includeFile);
			autostring includeText;
			try {
				includeText.reset (MelderFile_readText (& includeFile));
			} catch (MelderError) {
				Melder_throw ("Include file ", & includeFile, " not read.");
			}
			/*
				Construct the new text.
			 */
			headLength = (head - *text) + wcslen (head);
			includeTextLength = wcslen (includeText.peek());
			newLength = headLength + includeTextLength + 1 + wcslen (tail);
			newText = Melder_malloc (wchar_t, newLength + 1);
			wcscpy (newText, *text);
			wcscpy (newText + headLength, includeText.peek());
			wcscpy (newText + headLength + includeTextLength, L"\n");
			wcscpy (newText + headLength + includeTextLength + 1, tail);
			/*
				Replace the old text with the new.
			 */
			Melder_free (*text);
			*text = newText;
			/*
				Cycle.
			 */
			head = *text + headLength + includeTextLength + 1;
		}
		if (numberOfIncludes == 0) break;
	}
}

long Interpreter_readParameters (Interpreter me, wchar_t *text) {
	wchar_t *formLocation = NULL;
	long npar = 0;
	my dialogTitle [0] = '\0';
	/*
	 * Look for a "form" line.
	 */
	{// scope
		wchar_t *p = text;
		for (;;) {
			while (*p == ' ' || *p == '\t') p ++;
			if (wcsnequ (p, L"form ", 5)) {
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
		wchar_t *dialogTitle = formLocation + 5, *newLine;
		while (*dialogTitle == ' ' || *dialogTitle == '\t') dialogTitle ++;
		newLine = wcschr (dialogTitle, '\n');
		if (newLine) *newLine = '\0';
		wcscpy (my dialogTitle, dialogTitle);
		if (newLine) *newLine = '\n';
		my numberOfParameters = 0;
		while (newLine) {
			wchar_t *line = newLine + 1, *p;
			int type = 0;
			while (*line == ' ' || *line == '\t') line ++;
			while (*line == '#' || *line == ';' || *line == '!' || *line == '\n') {
				newLine = wcschr (line, '\n');
				if (newLine == NULL)
					Melder_throw ("Unfinished form.");
				line = newLine + 1;
				while (*line == ' ' || *line == '\t') line ++;
			}
			if (wcsnequ (line, L"endform", 7)) break;
			if (wcsnequ (line, L"word ", 5)) { type = Interpreter_WORD; p = line + 5; }
			else if (wcsnequ (line, L"real ", 5)) { type = Interpreter_REAL; p = line + 5; }
			else if (wcsnequ (line, L"positive ", 9)) { type = Interpreter_POSITIVE; p = line + 9; }
			else if (wcsnequ (line, L"integer ", 8)) { type = Interpreter_INTEGER; p = line + 8; }
			else if (wcsnequ (line, L"natural ", 8)) { type = Interpreter_NATURAL; p = line + 8; }
			else if (wcsnequ (line, L"boolean ", 8)) { type = Interpreter_BOOLEAN; p = line + 8; }
			else if (wcsnequ (line, L"sentence ", 9)) { type = Interpreter_SENTENCE; p = line + 9; }
			else if (wcsnequ (line, L"text ", 5)) { type = Interpreter_TEXT; p = line + 5; }
			else if (wcsnequ (line, L"choice ", 7)) { type = Interpreter_CHOICE; p = line + 7; }
			else if (wcsnequ (line, L"optionmenu ", 11)) { type = Interpreter_OPTIONMENU; p = line + 11; }
			else if (wcsnequ (line, L"button ", 7)) { type = Interpreter_BUTTON; p = line + 7; }
			else if (wcsnequ (line, L"option ", 7)) { type = Interpreter_OPTION; p = line + 7; }
			else if (wcsnequ (line, L"comment ", 8)) { type = Interpreter_COMMENT; p = line + 8; }
			else {
				newLine = wcschr (line, '\n');
				if (newLine) *newLine = '\0';
				Melder_error_ ("Unknown parameter type:\n\"", line, "\".");
				if (newLine) *newLine = '\n';
				throw MelderError ();
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
				if (*p == '\n' || *p == '\0')
					Melder_throw ("Missing parameter:\n\"", line, "\".");
				wchar_t *q = my parameters [++ my numberOfParameters];
				while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0') * (q ++) = * (p ++);
				*q = '\0';
				npar ++;
			} else {
				my parameters [++ my numberOfParameters] [0] = '\0';
			}
			while (*p == ' ' || *p == '\t') p ++;
			newLine = wcschr (p, '\n');
			if (newLine) *newLine = '\0';
			Melder_free (my arguments [my numberOfParameters]);
			my arguments [my numberOfParameters] = Melder_wcsdup_f (p);
			if (newLine) *newLine = '\n';
			my types [my numberOfParameters] = type;
		}
	} else {
		npar = my numberOfParameters = 0;
	}
	return npar;
}

UiForm Interpreter_createForm (Interpreter me, GuiWindow parent, const wchar_t *path,
	void (*okCallback) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *), void *okClosure)
{
	UiForm form = UiForm_create (parent, my dialogTitle [0] ? my dialogTitle : L"Script arguments", okCallback, okClosure, NULL, NULL);
	Any radio = NULL;
	if (path) UiForm_addText (form, L"$file", path);
	for (int ipar = 1; ipar <= my numberOfParameters; ipar ++) {
		/*
		 * Convert underscores to spaces.
		 */
		wchar_t parameter [100], *p = & parameter [0];
		wcscpy (parameter, my parameters [ipar]);
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
					(my arguments [ipar] [0] == 'o' && my arguments [ipar] [1] == 'n')); break;
			case Interpreter_SENTENCE:
				UiForm_addSentence (form, parameter, my arguments [ipar]); break;
			case Interpreter_TEXT:
				UiForm_addText (form, parameter, my arguments [ipar]); break;
			case Interpreter_CHOICE:
				radio = UiForm_addRadio (form, parameter, wcstol (my arguments [ipar], NULL, 10)); break;
			case Interpreter_OPTIONMENU:
				radio = UiForm_addOptionMenu (form, parameter, wcstol (my arguments [ipar], NULL, 10)); break;
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
		if ((p = wcschr (my parameters [ipar], '(')) != NULL) {
			*p = '\0';
			if (p - my parameters [ipar] > 0 && p [-1] == '_') p [-1] = '\0';
		}
		p = my parameters [ipar];
		if (*p != '\0' && p [wcslen (p) - 1] == ':') p [wcslen (p) - 1] = '\0';
	}
	UiForm_finish (form);
	return form;
}

void Interpreter_getArgumentsFromDialog (Interpreter me, Any dialog) {
	for (int ipar = 1; ipar <= my numberOfParameters; ipar ++) {
		wchar_t parameter [100], *p;
		/*
		 * Strip parentheses and colon off parameter name.
		 */
		if ((p = wcschr (my parameters [ipar], '(')) != NULL) {
			*p = '\0';
			if (p - my parameters [ipar] > 0 && p [-1] == '_') p [-1] = '\0';
		}
		p = my parameters [ipar];
		if (*p != '\0' && p [wcslen (p) - 1] == ':') p [wcslen (p) - 1] = '\0';
		/*
		 * Convert underscores to spaces.
		 */
		wcscpy (parameter, my parameters [ipar]);
		p = & parameter [0]; while (*p) { if (*p == '_') *p = ' '; p ++; }
		switch (my types [ipar]) {
			case Interpreter_REAL:
			case Interpreter_POSITIVE: {
				double value = UiForm_getReal_check (dialog, parameter);
				Melder_free (my arguments [ipar]);
				my arguments [ipar] = Melder_calloc_f (wchar_t, 40);
				wcscpy (my arguments [ipar], Melder_double (value));
				break;
			}
			case Interpreter_INTEGER:
			case Interpreter_NATURAL:
			case Interpreter_BOOLEAN: {
				long value = UiForm_getInteger (dialog, parameter);
				Melder_free (my arguments [ipar]);
				my arguments [ipar] = Melder_calloc_f (wchar_t, 40);
				swprintf (my arguments [ipar], 40, L"%ld", value);
				break;
			}
			case Interpreter_CHOICE:
			case Interpreter_OPTIONMENU: {
				long integerValue = 0;
				wchar_t *stringValue = NULL;
				integerValue = UiForm_getInteger (dialog, parameter);
				stringValue = UiForm_getString (dialog, parameter);
				Melder_free (my arguments [ipar]);
				my arguments [ipar] = Melder_calloc_f (wchar_t, 40);
				swprintf (my arguments [ipar], 40, L"%ld", integerValue);
				wcscpy (my choiceArguments [ipar], stringValue);
				break;
			}
			case Interpreter_BUTTON:
			case Interpreter_OPTION:
			case Interpreter_COMMENT:
				break;
			default: {
				wchar_t *value = UiForm_getString (dialog, parameter);
				Melder_free (my arguments [ipar]);
				my arguments [ipar] = Melder_wcsdup_f (value);
				break;
			}
		}
	}
}

void Interpreter_getArgumentsFromString (Interpreter me, const wchar_t *arguments) {
	int size = my numberOfParameters;
	long length = wcslen (arguments);
	while (size >= 1 && my parameters [size] [0] == '\0')
		size --;   /* Ignore fields without a variable name (button, comment). */
	for (int ipar = 1; ipar <= size; ipar ++) {
		wchar_t *p = my parameters [ipar];
		/*
		 * Ignore buttons and comments again.
		 */
		if (! *p) continue;
		/*
		 * Strip parentheses and colon off parameter name.
		 */
		if ((p = wcschr (p, '(')) != NULL) {
			*p = '\0';
			if (p - my parameters [ipar] > 0 && p [-1] == '_') p [-1] = '\0';
		}
		p = my parameters [ipar];
		if (*p != '\0' && p [wcslen (p) - 1] == ':') p [wcslen (p) - 1] = '\0';
	}
	for (int ipar = 1; ipar < size; ipar ++) {
		int ichar = 0;
		/*
		 * Ignore buttons and comments again. The buttons will keep their labels as "arguments".
		 */
		if (my parameters [ipar] [0] == '\0') continue;
		Melder_free (my arguments [ipar]);   // erase the current values, probably the default values
		my arguments [ipar] = Melder_calloc_f (wchar_t, length + 1);   // replace with the actual arguments
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
			arguments ++;   // do not include leading double quote
			for (;;) {
				if (*arguments == '\0')
					Melder_throw ("Missing matching quote.");
				if (*arguments == '\"' && * ++ arguments != '\"') break;   // remember second quote
				my arguments [ipar] [ichar ++] = *arguments ++;
			}
		} else {
			while (*arguments != ' ' && *arguments != '\t' && *arguments != '\0')
				my arguments [ipar] [ichar ++] = *arguments ++;
		}
		my arguments [ipar] [ichar] = '\0';   // trailing null byte
	}
	/* The last item is handled separately, because it consists of the rest of the line.
	 * Leading spaces are skipped, but trailing spaces are included.
	 */
	if (size > 0) {
		while (*arguments == ' ' || *arguments == '\t') arguments ++;
		Melder_free (my arguments [size]);
		my arguments [size] = Melder_wcsdup_f (arguments);
	}
	/*
	 * Convert booleans and choices to numbers.
	 */
	for (int ipar = 1; ipar <= size; ipar ++) {
		if (my types [ipar] == Interpreter_BOOLEAN) {
			wchar_t *arg = & my arguments [ipar] [0];
			if (wcsequ (arg, L"1") || wcsequ (arg, L"yes") || wcsequ (arg, L"on") ||
			    wcsequ (arg, L"Yes") || wcsequ (arg, L"On") || wcsequ (arg, L"YES") || wcsequ (arg, L"ON"))
			{
				wcscpy (arg, L"1");
			} else if (wcsequ (arg, L"0") || wcsequ (arg, L"no") || wcsequ (arg, L"off") ||
			    wcsequ (arg, L"No") || wcsequ (arg, L"Off") || wcsequ (arg, L"NO") || wcsequ (arg, L"OFF"))
			{
				wcscpy (arg, L"0");
			} else {
				Melder_throw ("Unknown value \"", arg, "\" for boolean \"", my parameters [ipar], "\".");
			}
		} else if (my types [ipar] == Interpreter_CHOICE) {
			int jpar;
			wchar_t *arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_BUTTON && my types [jpar] != Interpreter_OPTION)
					Melder_throw ("Unknown value \"", arg, "\" for choice \"", my parameters [ipar], "\".");
				if (wcsequ (my arguments [jpar], arg)) {   // the button labels are in the arguments; see Interpreter_readParameters
					swprintf (arg, 40, L"%d", jpar - ipar);
					wcscpy (my choiceArguments [ipar], my arguments [jpar]);
					break;
				}
			}
			if (jpar > my numberOfParameters)
				Melder_throw ("Unknown value \"", arg, "\" for choice \"", my parameters [ipar], "\".");
		} else if (my types [ipar] == Interpreter_OPTIONMENU) {
			int jpar;
			wchar_t *arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_OPTION && my types [jpar] != Interpreter_BUTTON)
					Melder_throw ("Unknown value \"", arg, "\" for option menu \"", my parameters [ipar], "\".");
				if (wcsequ (my arguments [jpar], arg)) {
					swprintf (arg, 40, L"%d", jpar - ipar);
					wcscpy (my choiceArguments [ipar], my arguments [jpar]);
					break;
				}
			}
			if (jpar > my numberOfParameters)
				Melder_throw ("Unknown value \"", arg, "\" for option menu \"", my parameters [ipar], "\".");
		}
	}
}

static int Interpreter_addNumericVariable (Interpreter me, const wchar_t *key, double value) {
	InterpreterVariable variable = InterpreterVariable_create (key);
	variable -> numericValue = value;
	Collection_addItem (my variables, variable);
	return 1;
}

static InterpreterVariable Interpreter_addStringVariable (Interpreter me, const wchar_t *key, const wchar_t *value) {
	InterpreterVariable variable = InterpreterVariable_create (key);
	variable -> stringValue = Melder_wcsdup (value);
	Collection_addItem (my variables, variable);
	return variable;
}

InterpreterVariable Interpreter_hasVariable (Interpreter me, const wchar_t *key) {
	long ivar = 0;
	wchar_t variableNameIncludingProcedureName [1+200];
	Melder_assert (key != NULL);
	if (key [0] == '.') {
		wcscpy (variableNameIncludingProcedureName, my procedureNames [my callDepth]);
		wcscat (variableNameIncludingProcedureName, key);
	} else {
		wcscpy (variableNameIncludingProcedureName, key);
	}
	ivar = SortedSetOfString_lookUp (my variables, variableNameIncludingProcedureName);
	return ivar ? (InterpreterVariable) my variables -> item [ivar] : NULL;
}

InterpreterVariable Interpreter_lookUpVariable (Interpreter me, const wchar_t *key) {
	InterpreterVariable var = NULL;
	wchar_t variableNameIncludingProcedureName [1+200];
	Melder_assert (key != NULL);
	if (key [0] == '.') {
		wcscpy (variableNameIncludingProcedureName, my procedureNames [my callDepth]);
		wcscat (variableNameIncludingProcedureName, key);
	} else {
		wcscpy (variableNameIncludingProcedureName, key);
	}
	var = Interpreter_hasVariable (me, variableNameIncludingProcedureName);
	if (var) return var;
	var = InterpreterVariable_create (variableNameIncludingProcedureName);
	Collection_addItem (my variables, var);
	return Interpreter_hasVariable (me, variableNameIncludingProcedureName);
}

static long lookupLabel (Interpreter me, const wchar_t *labelName) {
	for (long ilabel = 1; ilabel <= my numberOfLabels; ilabel ++)
		if (wcsequ (labelName, my labelNames [ilabel]))
			return ilabel;
	Melder_throw ("Unknown label \"", labelName, "\".");
}

static bool isCommand (const wchar_t *p) {
	/*
	 * Things that start with "nowarn", "noprogress", or "nocheck" are commands.
	 */
	if (p [0] == 'n' && p [1] == 'o' &&
		(wcsnequ (p + 2, L"warn ", 5) || wcsnequ (p + 2, L"progress ", 9) || wcsnequ (p + 2, L"check ", 6))) return true;
	if (wcsnequ (p, L"demo ", 5)) return true;
	/*
	 * Otherwise, things that start with lower case are formulas.
	 */
	if (! isupper (*p)) return false;
	/*
	 * The remaining possibility is things that start with upper case.
	 * If they contain an underscore, they are object names, hence we must have a formula.
	 * Otherwise, we have a command.
	 */
	while (isalnum (*p)) p ++;
	return *p != '_';
}

static void parameterToVariable (Interpreter me, int type, const wchar_t *in_parameter, int ipar) {
	wchar_t parameter [200];
	Melder_assert (type != 0);
	wcscpy (parameter, in_parameter);
	if (type >= Interpreter_REAL && type <= Interpreter_BOOLEAN) {
		Interpreter_addNumericVariable (me, parameter, Melder_atof (my arguments [ipar]));
	} else if (type == Interpreter_CHOICE || type == Interpreter_OPTIONMENU) {
		Interpreter_addNumericVariable (me, parameter, Melder_atof (my arguments [ipar]));
		wcscat (parameter, L"$");
		Interpreter_addStringVariable (me, parameter, my choiceArguments [ipar]);
	} else if (type == Interpreter_BUTTON || type == Interpreter_OPTION || type == Interpreter_COMMENT) {
		/* Do not add a variable. */
	} else {
		wcscat (parameter, L"$");
		Interpreter_addStringVariable (me, parameter, my arguments [ipar]);
	}
}

void Interpreter_run (Interpreter me, wchar_t *text) {
	autoNUMvector <wchar_t *> lines;   // not autostringvector, because the elements are reference copies
	long lineNumber = 0;
	bool assertionFailed = false;
	try {
		static MelderString valueString = { 0 };   // to divert the info
		static MelderString assertErrorString = { 0 };
		wchar_t *command = text;
		autoMelderString command2;
		autoMelderString buffer;
		long numberOfLines = 0, assertErrorLineNumber = 0, callStack [1 + Interpreter_MAX_CALL_DEPTH];
		int atLastLine = FALSE, fromif = FALSE, fromendfor = FALSE, callDepth = 0, chopped = 0, ipar;
		my callDepth = 0;
		/*
		 * The "environment" is NULL if we are in the Praat shell, or an editor otherwise.
		 */
		if (my editorClass) {
			praatP. editor = praat_findEditorFromString (my environmentName);
		} else {
			praatP. editor = NULL;
		}
		/*
		 * Start.
		 */
		my running = true;
		/*
		 * Count lines and set the newlines to zero.
		 */
		while (! atLastLine) {
			wchar_t *endOfLine = command;
			while (*endOfLine != '\n' && *endOfLine != '\0') endOfLine ++;
			if (*endOfLine == '\0') atLastLine = TRUE;
			*endOfLine = '\0';
			numberOfLines ++;
			command = endOfLine + 1;
		}
		/*
		 * Remember line starts and labels.
		 */
		lines.reset (1, numberOfLines);
		for (lineNumber = 1, command = text; lineNumber <= numberOfLines; lineNumber ++, command += wcslen (command) + 1 + chopped) {
			int length;
			while (*command == ' ' || *command == '\t' || *command == UNICODE_NO_BREAK_SPACE) command ++;   // nbsp can occur for scripts copied from the manual
			length = wcslen (command);
			/*
			 * Chop trailing spaces?
			 */
			/*chopped = 0;
			while (length > 0) { char kar = command [-- length]; if (kar != ' ' && kar != '\t') break; command [length] = '\0'; chopped ++; }*/
			lines [lineNumber] = command;
			if (wcsnequ (command, L"label ", 6)) {
				int ilabel;
				for (ilabel = 1; ilabel <= my numberOfLabels; ilabel ++)
					if (wcsequ (command + 6, my labelNames [ilabel]))
						Melder_throw ("Duplicate label \"", command + 6, "\".");
				if (my numberOfLabels >= Interpreter_MAXNUM_LABELS)
					Melder_throw ("Too many labels.");
				swprintf (my labelNames [++ my numberOfLabels], 50, L"%.47ls", command + 6);
				my labelLines [my numberOfLabels] = lineNumber;
			}
		}
		/*
		 * Connect continuation lines.
		 */
		for (lineNumber = numberOfLines; lineNumber >= 2; lineNumber --) {
			wchar_t *line = lines [lineNumber];
			if (line [0] == '.' && line [1] == '.' && line [2] == '.') {
				wchar_t *previous = lines [lineNumber - 1];
				MelderString_copy (& command2, line + 3);
				MelderString_get (& command2, previous + wcslen (previous));
				static wchar_t emptyLine [] = { '\0' };
				lines [lineNumber] = emptyLine;
			}
		}
		/*
		 * Copy the parameter names and argument values into the array of variables.
		 */
		forget (my variables);
		my variables = SortedSetOfString_create ();
		for (ipar = 1; ipar <= my numberOfParameters; ipar ++) {
			wchar_t parameter [200];
			/*
			 * Create variable names as-are and variable names without capitals.
			 */
			wcscpy (parameter, my parameters [ipar]);
			parameterToVariable (me, my types [ipar], parameter, ipar);
			if (parameter [0] >= 'A' && parameter [0] <= 'Z') {
				parameter [0] = tolower (parameter [0]);
				parameterToVariable (me, my types [ipar], parameter, ipar);
			}
		}
		/*
		 * Initialize some variables.
		 */
		Interpreter_addStringVariable (me, L"newline$", L"\n");
		Interpreter_addStringVariable (me, L"tab$", L"\t");
		Interpreter_addStringVariable (me, L"shellDirectory$", Melder_getShellDirectory ());
		structMelderDir dir = { { 0 } }; Melder_getDefaultDir (& dir);
		Interpreter_addStringVariable (me, L"defaultDirectory$", Melder_dirToPath (& dir));
		Interpreter_addStringVariable (me, L"preferencesDirectory$", Melder_dirToPath (& praatDir));
		Melder_getHomeDir (& dir);
		Interpreter_addStringVariable (me, L"homeDirectory$", Melder_dirToPath (& dir));
		Melder_getTempDir (& dir);
		Interpreter_addStringVariable (me, L"temporaryDirectory$", Melder_dirToPath (& dir));
		#if defined (macintosh)
			Interpreter_addNumericVariable (me, L"macintosh", 1);
			Interpreter_addNumericVariable (me, L"windows", 0);
			Interpreter_addNumericVariable (me, L"unix", 0);
		#elif defined (_WIN32)
			Interpreter_addNumericVariable (me, L"macintosh", 0);
			Interpreter_addNumericVariable (me, L"windows", 1);
			Interpreter_addNumericVariable (me, L"unix", 0);
		#elif defined (UNIX)
			Interpreter_addNumericVariable (me, L"macintosh", 0);
			Interpreter_addNumericVariable (me, L"windows", 0);
			Interpreter_addNumericVariable (me, L"unix", 1);
		#else
			Interpreter_addNumericVariable (me, L"macintosh", 0);
			Interpreter_addNumericVariable (me, L"windows", 0);
			Interpreter_addNumericVariable (me, L"unix", 0);
		#endif
		Interpreter_addNumericVariable (me, L"left", 1);   // to accommodate scripts from before Praat 5.2.06
		Interpreter_addNumericVariable (me, L"right", 2);   // to accommodate scripts from before Praat 5.2.06
		Interpreter_addNumericVariable (me, L"mono", 1);   // to accommodate scripts from before Praat 5.2.06
		Interpreter_addNumericVariable (me, L"stereo", 2);   // to accommodate scripts from before Praat 5.2.06
		Interpreter_addNumericVariable (me, L"all", 0);   // to accommodate scripts from before Praat 5.2.06
		Interpreter_addNumericVariable (me, L"average", 0);   // to accommodate scripts from before Praat 5.2.06
		#define xstr(s) str(s)
		#define str(s) #s
		Interpreter_addStringVariable (me, L"praatVersion$", L"" xstr(PRAAT_VERSION_STR));
		Interpreter_addNumericVariable (me, L"praatVersion", PRAAT_VERSION_NUM);
		/*
		 * Execute commands.
		 */
		#define wordEnd(c)  (c == '\0' || c == ' ' || c == '\t')
		for (lineNumber = 1; lineNumber <= numberOfLines; lineNumber ++) {
			if (my stopped) break;
			try {
				int c0, fail = FALSE;
				wchar_t *p;
				MelderString_copy (& command2, lines [lineNumber]);
				c0 = command2. string [0];
				if (c0 == '\0') continue;
				/*
				 * Substitute variables.
				 */
				for (p = & command2. string [0]; *p !='\0'; p ++) if (*p == '\'') {
					/*
					 * Found a left quote. Search for a matching right quote.
					 */
					wchar_t *q = p + 1, varName [300], *r, *s, *colon;
					int precision = -1, percent = FALSE;
					while (*q != '\0' && *q != '\'' && q - p < 299) q ++;
					if (*q == '\0') break;   /* No matching right quote: done with this line. */
					if (q - p == 1 || q - p >= 299) continue;   /* Ignore empty variable names. */
					/*
					 * Found a right quote. Get potential variable name.
					 */
					for (r = p + 1, s = varName; q - r > 0; r ++, s ++) *s = *r;
					*s = '\0';   /* Trailing null byte. */
					colon = wcschr (varName, ':');
					if (colon) {
						precision = wcstol (colon + 1, NULL, 10);
						if (wcschr (colon + 1, '%')) percent = TRUE;
						*colon = '\0';
					}
					InterpreterVariable var = Interpreter_hasVariable (me, varName);
					if (var) {
						/*
						 * Found a variable (p points to the left quote, q to the right quote). Substitute.
						 */
						int headlen = p - command2.string;
						const wchar_t *string = var -> stringValue ? var -> stringValue :
							percent ? Melder_percent (var -> numericValue, precision) :
							precision >= 0 ?  Melder_fixed (var -> numericValue, precision) :
							Melder_double (var -> numericValue);
						int arglen = wcslen (string);
						MelderString_ncopy (& buffer, command2.string, headlen);
						MelderString_append (& buffer, string, q + 1);
						MelderString_copy (& command2, buffer.string);   // This invalidates p!! (really bad bug 20070203)
						p = command2.string + headlen + arglen - 1;
					} else {
						p = q - 1;   /* Go to before next quote. */
					}
				}
				c0 = command2.string [0];   /* Resume in order to allow things like 'c$' = 5 */
				if ((c0 < 'a' || c0 > 'z') && c0 != '@' && ! (c0 == '.' && command2.string [1] >= 'a' && command2.string [1] <= 'z')) {
					praat_executeCommand (me, command2.string);
				/*
				 * Interpret control flow and variables.
				 */
				} else switch (c0) {
					case '.':
						fail = TRUE;
						break;
					case '@':
					{
						/*
						 * This is a function call.
						 * Look for a function name.
						 */
						wchar_t *p = command2.string + 1;
						while (*p == ' ' || *p == '\t') p ++;   // skip whitespace
						wchar_t *callName = p;
						while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '(') p ++;
						if (p == callName) Melder_throw ("Missing procedure name after \"@\".");
						if (*p == '\0') Melder_throw ("Missing parenthesis after procedure name.");
						bool parenthesisFound = *p == '(';
						*p = '\0';   // close procedure name
						if (! parenthesisFound) {
							p ++;   // step over first white space
							while (*p != '\0' && (*p == ' ' || *p == '\t') && *p != '(') p ++;   // skip more whitespace
							if (*p != '(') Melder_throw ("Missing parenthesis after procedure name \"", callName, "\".");
						}
						p ++;   // step over parenthesis
						int callLength = wcslen (callName);
						long iline = 1;
						for (; iline <= numberOfLines; iline ++) {
							wchar_t *linei = lines [iline], *q;
							if (linei [0] != 'p' || linei [1] != 'r' || linei [2] != 'o' || linei [3] != 'c' ||
								linei [4] != 'e' || linei [5] != 'd' || linei [6] != 'u' || linei [7] != 'r' ||
								linei [8] != 'e' || linei [9] != ' ') continue;
							q = lines [iline] + 10;
							while (*q == ' ' || *q == '\t') q ++;   // skip whitespace before procedure name
							wchar_t *procName = q;
							while (*q != '\0' && *q != ' ' && *q != '\t' && *q != '(') q ++;
							if (q == procName) Melder_throw ("Missing procedure name after 'procedure'.");
							if (q - procName == callLength && wcsnequ (procName, callName, callLength)) {
								/*
								 * We found the procedure definition.
								 */
								if (++ my callDepth > Interpreter_MAX_CALL_DEPTH)
									Melder_throw ("Call depth greater than ", Interpreter_MAX_CALL_DEPTH, ".");
								wcscpy (my procedureNames [my callDepth], callName);
								parenthesisFound = *q == '(';
								if (! parenthesisFound) {
									q ++;   // step over first white space
									while (*q != '\0' && (*q == ' ' || *q == '\t') && *q != '(') q ++;   // skip more whitespace
									if (*q != '(') Melder_throw ("Missing parenthesis after procedure name \"", callName, "\".");
								}
								++ q;   // step over parenthesis
								while (*q && *q != ')') {
									static MelderString argument = { 0 };
									MelderString_empty (& argument);
									while (*p == ' ' || *p == '\t') p ++;
									while (*q == ' ' || *q == '\t') q ++;
									wchar_t *parameterName = q;
									while (*q != '\0' && *q != ' ' && *q != '\t' && *q != ',' && *q != ')') q ++;   // collect parameter name
									int expressionDepth = 0;
									for (; *p; p ++) {
										if (*p == ',') {
											if (expressionDepth == 0) break;   // depth-0 comma ends expression
											MelderString_appendCharacter (& argument, ',');
										} else if (*p == ')') {
											if (expressionDepth == 0) break;   // depth-0 closing parenthesis ends expression
											expressionDepth --;
											MelderString_appendCharacter (& argument, ')');
										} else if (*p == '(') {
											expressionDepth ++;
											MelderString_appendCharacter (& argument, '(');
										} else if (*p == '\"') {
											/*
											 * Enter a string literal.
											 */
											MelderString_appendCharacter (& argument, '\"');
											p ++;
											for (;; p ++) {
												if (*p == '\0') {
													Melder_throw (L"Incomplete string literal: the quotes don't match.");
												} else if (*p == '\"') {
													MelderString_appendCharacter (& argument, '\"');
													if (p [1] == '\"') {
														p ++;   // stay in the string literal
														MelderString_appendCharacter (& argument, '\"');
													} else {
														break;
													}
												} else {
													MelderString_appendCharacter (& argument, *p);
												}
											}
										} else {
											MelderString_appendCharacter (& argument, *p);
										}
									}
									if (q == parameterName) break;
									if (*p) { *p = '\0'; p ++; }
									if (q [-1] == '$') {
										wchar_t *value;
										my callDepth --;
										Interpreter_stringExpression (me, argument.string, & value);
										my callDepth ++;
										wchar_t save = *q; *q = '\0';
										InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName); *q = save;
										Melder_free (var -> stringValue);
										var -> stringValue = value;
									} else {
										double value;
										my callDepth --;
										Interpreter_numericExpression (me, argument.string, & value);
										my callDepth ++;
										wchar_t save = *q; *q = '\0';
										InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName); *q = save;
										var -> numericValue = value;
									}
									if (*q) q ++;   // skip comma
								}
								if (callDepth == Interpreter_MAX_CALL_DEPTH)
									Melder_throw ("Call depth greater than ", Interpreter_MAX_CALL_DEPTH, ".");
								callStack [++ callDepth] = lineNumber;
								lineNumber = iline;
								break;
							}
						}
						if (iline > numberOfLines) Melder_throw ("Procedure \"", callName, "\" not found.");
						break;
					}
					case 'a':
						if (wcsnequ (command2.string, L"assert ", 7)) {
							double value;
							Interpreter_numericExpression (me, command2.string + 7, & value);
							if (value == 0.0 || value == NUMundefined) {
								assertionFailed = TRUE;
								Melder_throw ("Script assertion fails in line ", lineNumber,
									" (", value ? "undefined" : "false", "):\n   ", command2.string + 7);
							}
						} else if (wcsnequ (command2.string, L"asserterror ", 12)) {
							MelderString_copy (& assertErrorString, command2.string + 12);
							assertErrorLineNumber = lineNumber;
						} else fail = TRUE;
						break;
					case 'b':
						fail = TRUE;
						break;
					case 'c':
						if (wcsnequ (command2.string, L"call ", 5)) {
							wchar_t *p = command2.string + 5, *callName, *procName;
							long iline;
							int hasArguments, callLength;
							while (*p == ' ' || *p == '\t') p ++;
							callName = p;
							while (*p != '\0' && *p != ' ' && *p != '\t') p ++;
							if (p == callName) Melder_throw ("Missing procedure name after 'call'.");
							hasArguments = *p != '\0';
							*p = '\0';   /* Close procedure name. */
							callLength = wcslen (callName);
							for (iline = 1; iline <= numberOfLines; iline ++) {
								wchar_t *linei = lines [iline], *q;
								int hasParameters;
								if (linei [0] != 'p' || linei [1] != 'r' || linei [2] != 'o' || linei [3] != 'c' ||
									linei [4] != 'e' || linei [5] != 'd' || linei [6] != 'u' || linei [7] != 'r' ||
									linei [8] != 'e' || linei [9] != ' ') continue;
								q = lines [iline] + 10;
								while (*q == ' ' || *q == '\t') q ++;
								procName = q;
								while (*q != '\0' && *q != ' ' && *q != '\t') q ++;
								if (q == procName) Melder_throw ("Missing procedure name after 'procedure'.");
								hasParameters = *q != '\0';
								if (q - procName == callLength && wcsnequ (procName, callName, callLength)) {
									if (hasArguments && ! hasParameters)
										Melder_throw ("Call to procedure \"", callName, "\" has too many arguments.");
									if (hasParameters && ! hasArguments)
										Melder_throw ("Call to procedure \"", callName, "\" has too few arguments.");
									if (++ my callDepth > Interpreter_MAX_CALL_DEPTH)
										Melder_throw ("Call depth greater than ", Interpreter_MAX_CALL_DEPTH, ".");
									wcscpy (my procedureNames [my callDepth], callName);
									if (hasParameters) {
										++ p;   /* First argument. */
										++ q;   /* First parameter. */
										while (*q) {
											wchar_t *par, save;
											static MelderString arg = { 0 };
											MelderString_empty (& arg);
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
																MelderString_appendCharacter (& arg, '\"');
																p += 2;   /* Skip second quote. */
															} else {   /* Single quote signals end-of-string. */
																break;
															}
														} else {
															MelderString_appendCharacter (& arg, *p ++);
														}
													}
												} else {
													while (*p != '\0' && *p != ' ' && *p != '\t')
														MelderString_appendCharacter (& arg, *p ++);   /* White space separates. */
												}
												if (*p) { *p = '\0'; p ++; }
											} else {   /* Else rest of line. */
												while (*p != '\0')
													MelderString_appendCharacter (& arg, *p ++);
											}
											if (q [-1] == '$') {
												save = *q; *q = '\0';
												InterpreterVariable var = Interpreter_lookUpVariable (me, par); *q = save;
												Melder_free (var -> stringValue);
												var -> stringValue = Melder_wcsdup_f (arg.string);
											} else {
												double value;
												my callDepth --;
												Interpreter_numericExpression (me, arg.string, & value);
												my callDepth ++;
												save = *q; *q = '\0'; 
												InterpreterVariable var = Interpreter_lookUpVariable (me, par); *q = save;
												var -> numericValue = value;
											}
										}
									}
									if (callDepth == Interpreter_MAX_CALL_DEPTH)
										Melder_throw ("Call depth greater than ", Interpreter_MAX_CALL_DEPTH, ".");
									callStack [++ callDepth] = lineNumber;
									lineNumber = iline;
									break;
								}
							}
							if (iline > numberOfLines) Melder_throw ("Procedure \"", callName, "\" not found.");
						} else fail = TRUE;
						break;
					case 'd':
						if (wcsnequ (command2.string, L"dec ", 4)) {
							InterpreterVariable var = Interpreter_lookUpVariable (me, command2.string + 4);
							var -> numericValue -= 1.0;
						} else fail = TRUE;
						break;
					case 'e':
						if (command2.string [1] == 'n' && command2.string [2] == 'd') {
							if (wcsnequ (command2.string, L"endif", 5) && wordEnd (command2.string [5])) {
								/* Ignore. */
							} else if (wcsnequ (command2.string, L"endfor", 6) && wordEnd (command2.string [6])) {
								int depth = 0;
								long iline;
								for (iline = lineNumber - 1; iline > 0; iline --) {
									wchar_t *line = lines [iline];
									if (line [0] == 'f' && line [1] == 'o' && line [2] == 'r' && line [3] == ' ') {
										if (depth == 0) { lineNumber = iline - 1; fromendfor = TRUE; break; }   /* Go before 'for'. */
										else depth --;
									} else if (wcsnequ (lines [iline], L"endfor", 6) && wordEnd (lines [iline] [6])) {
										depth ++;
									}
								}
								if (iline <= 0) Melder_throw ("Unmatched 'endfor'.");
							} else if (wcsnequ (command2.string, L"endwhile", 8) && wordEnd (command2.string [8])) {
								int depth = 0;
								long iline;
								for (iline = lineNumber - 1; iline > 0; iline --) {
									if (wcsnequ (lines [iline], L"while ", 6)) {
										if (depth == 0) { lineNumber = iline - 1; break; }   /* Go before 'while'. */
										else depth --;
									} else if (wcsnequ (lines [iline], L"endwhile", 8) && wordEnd (lines [iline] [8])) {
										depth ++;
									}
								}
								if (iline <= 0) Melder_throw ("Unmatched 'endwhile'.");
							} else if (wcsnequ (command2.string, L"endproc", 7) && wordEnd (command2.string [7])) {
								if (callDepth == 0) Melder_throw ("Unmatched 'endproc'.");
								lineNumber = callStack [callDepth --];
								-- my callDepth;
							} else fail = TRUE;
						} else if (wcsnequ (command2.string, L"else", 4) && wordEnd (command2.string [4])) {
							int depth = 0;
							long iline;
							for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
								if (wcsnequ (lines [iline], L"endif", 5) && wordEnd (lines [iline] [5])) {
									if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endif'. */
									else depth --;
								} else if (wcsnequ (lines [iline], L"if ", 3)) {
									depth ++;
								}
							}
							if (iline > numberOfLines) Melder_throw ("Unmatched 'else'.");
						} else if (wcsnequ (command2.string, L"elsif ", 6) || wcsnequ (command2.string, L"elif ", 5)) {
							if (fromif) {
								double value;
								fromif = FALSE;
								Interpreter_numericExpression (me, command2.string + 5, & value);
								if (value == 0.0) {
									int depth = 0;
									long iline;
									for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
										if (wcsnequ (lines [iline], L"endif", 5) && wordEnd (lines [iline] [5])) {
											if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endif'. */
											else depth --;
										} else if (wcsnequ (lines [iline], L"else", 4) && wordEnd (lines [iline] [4])) {
											if (depth == 0) { lineNumber = iline; break; }   /* Go after 'else'. */
										} else if ((wcsnequ (lines [iline], L"elsif", 5) && wordEnd (lines [iline] [5]))
											|| (wcsnequ (lines [iline], L"elif", 4) && wordEnd (lines [iline] [4]))) {
											if (depth == 0) { lineNumber = iline - 1; fromif = TRUE; break; }   /* Go at next 'elsif' or 'elif'. */
										} else if (wcsnequ (lines [iline], L"if ", 3)) {
											depth ++;
										}
									}
									if (iline > numberOfLines) Melder_throw ("Unmatched 'elsif'.");
								}
							} else {
								int depth = 0;
								long iline;
								for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (wcsnequ (lines [iline], L"endif", 5) && wordEnd (lines [iline] [5])) {
										if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endif'. */
										else depth --;
									} else if (wcsnequ (lines [iline], L"if ", 3)) {
										depth ++;
									}
								}
								if (iline > numberOfLines) Melder_throw ("'elsif' not matched with 'endif'.");
							}
						} else if (wcsnequ (command2.string, L"exit", 4)) {
							if (command2.string [4] == '\0') {
								lineNumber = numberOfLines;   /* Go after end. */
							} else {
								Melder_throw (command2.string + 5);
							}
						} else if (wcsnequ (command2.string, L"echo ", 5)) {
							/*
							 * Make sure that lines like "echo = 3" will not be regarded as assignments.
							 */
							praat_executeCommand (me, command2.string);
						} else fail = TRUE;
						break;
					case 'f':
						if (command2.string [1] == 'o' && command2.string [2] == 'r' && command2.string [3] == ' ') {   /* for_ */
							double toValue, loopVariable;
							wchar_t *frompos = wcsstr (command2.string, L" from "), *topos = wcsstr (command2.string, L" to ");
							wchar_t *varpos = command2.string + 4, *endvar = frompos;
							if (! topos) Melder_throw ("Missing \'to\' in \'for\' loop.");
							if (! endvar) endvar = topos;
							while (*endvar == ' ') { *endvar = '\0'; endvar --; }
							while (*varpos == ' ') varpos ++;
							if (endvar - varpos < 0) Melder_throw ("Missing loop variable after \'for\'.");
							InterpreterVariable var = Interpreter_lookUpVariable (me, varpos);
							Interpreter_numericExpression (me, topos + 4, & toValue);
							if (fromendfor) {
								fromendfor = FALSE;
								loopVariable = var -> numericValue + 1.0;
							} else if (frompos) {
								*topos = '\0';
								Interpreter_numericExpression (me, frompos + 6, & loopVariable);
							} else {
								loopVariable = 1.0;
							}
							var -> numericValue = loopVariable;
							if (loopVariable > toValue) {
								int depth = 0;
								long iline;
								for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (wcsnequ (lines [iline], L"endfor", 6)) {
										if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endfor'. */
										else depth --;
									} else if (wcsnequ (lines [iline], L"for ", 4)) {
										depth ++;
									}
								}
								if (iline > numberOfLines) Melder_throw ("Unmatched 'for'.");
							}
						} else if (wcsnequ (command2.string, L"form ", 5)) {
							long iline;
							for (iline = lineNumber + 1; iline <= numberOfLines; iline ++)
								if (wcsnequ (lines [iline], L"endform", 7))
									{ lineNumber = iline; break; }   /* Go after 'endform'. */
							if (iline > numberOfLines) Melder_throw ("Unmatched 'form'.");
						} else fail = TRUE;
						break;
					case 'g':
						if (wcsnequ (command2.string, L"goto ", 5)) {
							wchar_t labelName [50], *space;
							int dojump = TRUE, ilabel;
							swprintf (labelName, 50, L"%.47ls", command2.string + 5);
							space = wcschr (labelName, ' ');
							if (space == labelName) Melder_throw ("Missing label name after 'goto'.");
							if (space) {
								double value;
								*space = '\0';
								Interpreter_numericExpression (me, command2.string + 6 + wcslen (labelName), & value);
								if (value == 0.0) dojump = FALSE;
							}
							if (dojump) {
								ilabel = lookupLabel (me, labelName);
								lineNumber = my labelLines [ilabel];   // loop will add 1
							}
						} else fail = TRUE;
						break;
					case 'h':
						fail = TRUE;
						break;
					case 'i':
						if (command2.string [1] == 'f' && command2.string [2] == ' ') {   /* if_ */
							double value;
							Interpreter_numericExpression (me, command2.string + 3, & value);
							if (value == 0.0) {
								int depth = 0;
								long iline;
								for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (wcsnequ (lines [iline], L"endif", 5)) {
										if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endif'. */
										else depth --;
									} else if (wcsnequ (lines [iline], L"else", 4)) {
										if (depth == 0) { lineNumber = iline; break; }   /* Go after 'else'. */
									} else if (wcsnequ (lines [iline], L"elsif ", 6) || wcsnequ (lines [iline], L"elif ", 5)) {
										if (depth == 0) { lineNumber = iline - 1; fromif = TRUE; break; }   /* Go at 'elsif'. */
									} else if (wcsnequ (lines [iline], L"if ", 3)) {
										depth ++;
									}
								}
								if (iline > numberOfLines) Melder_throw ("Unmatched 'if'.");
							} else if (value == NUMundefined) {
								Melder_throw ("The value of the 'if' condition is undefined.");
							}
						} else if (wcsnequ (command2.string, L"inc ", 4)) {
							InterpreterVariable var = Interpreter_lookUpVariable (me, command2.string + 4);
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
						if (wcsnequ (command2.string, L"label ", 6)) {
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
						if (wcsnequ (command2.string, L"procedure ", 10)) {
							long iline = lineNumber + 1;
							for (; iline <= numberOfLines; iline ++) {
								if (wcsnequ (lines [iline], L"endproc", 7) && wordEnd (lines [iline] [7])) {
									lineNumber = iline;
									break;
								}   /* Go after 'endproc'. */
							}
							if (iline > numberOfLines) Melder_throw ("Unmatched 'proc'.");
						} else if (wcsnequ (command2.string, L"print", 5)) {
							/*
							 * Make sure that lines like "print = 3" will not be regarded as assingments.
							 */
							if (command2.string [5] == ' ' || (wcsnequ (command2.string + 5, L"line", 4) && (command2.string [9] == ' ' || command2.string [9] == '\0'))) {
								praat_executeCommand (me, command2.string);
							} else fail = TRUE;
						} else fail = TRUE;
						break;
					case 'q':
						fail = TRUE;
						break;
					case 'r':
						if (wcsnequ (command2.string, L"repeat", 6) && wordEnd (command2.string [6])) {
							/* Ignore. */
						} else fail = TRUE;
						break;
					case 's':
						if (wcsnequ (command2.string, L"stopwatch", 9) && wordEnd (command2.string [9])) {
							(void) Melder_stopwatch ();   /* Reset stopwatch. */
						} else fail = TRUE;
						break;
					case 't':
						fail = TRUE;
						break;
					case 'u':
						if (wcsnequ (command2.string, L"until ", 6)) {
							double value;
							Interpreter_numericExpression (me, command2.string + 6, & value);
							if (value == 0.0) {
								int depth = 0;
								long iline;
								for (iline = lineNumber - 1; iline > 0; iline --) {
									if (wcsnequ (lines [iline], L"repeat", 6) && wordEnd (lines [iline] [6])) {
										if (depth == 0) { lineNumber = iline; break; }   /* Go after 'repeat'. */
										else depth --;
									} else if (wcsnequ (lines [iline], L"until ", 6)) {
										depth ++;
									}
								}
								if (iline <= 0) Melder_throw ("Unmatched 'until'.");
							}
						} else fail = TRUE;
						break;
					case 'v':
						fail = TRUE;
						break;
					case 'w':
						if (wcsnequ (command2.string, L"while ", 6)) {
							double value;
							Interpreter_numericExpression (me, command2.string + 6, & value);
							if (value == 0.0) {
								int depth = 0;
								long iline;
								for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (wcsnequ (lines [iline], L"endwhile", 8) && wordEnd (lines [iline] [8])) {
										if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endwhile'. */
										else depth --;
									} else if (wcsnequ (lines [iline], L"while ", 6)) {
										depth ++;
									}
								}
								if (iline > numberOfLines) Melder_throw ("Unmatched 'while'.");
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
					 * Found an unknown word starting with a lower-case letter, optionally preceded by a period.
					 * See whether the word is a variable name.
					 */
					wchar_t *p = & command2.string [0];
					/*
					 * Variable names consist of a sequence of letters, digits, and underscores,
					 * optionally preceded by a period and optionally followed by a $ and/or #.
					 */
					if (*p == '.') p ++;
					while (isalnum (*p) || *p == '_' || *p == '.')  p ++;
					if (*p == '$') {
						/*
						 * Assign to a string variable.
						 */
						wchar_t *endOfVariable = ++ p;
						wchar_t *variableName = command2.string;
						int withFile;
						while (*p == ' ' || *p == '\t') p ++;   /* Go to first token after variable name. */
						if (*p == '[') {
							/*
							 * This must be an assignment to an indexed string variable.
							 */
							*endOfVariable = '\0';
							static MelderString indexedVariableName = { 0 };
							MelderString_copy (& indexedVariableName, command2.string);
							MelderString_appendCharacter (& indexedVariableName, '[');
							for (;;) {
								p ++;   // skip opening bracket or comma
								static MelderString index = { 0 };
								MelderString_empty (& index);
								int depth = 0;
								while ((depth > 0 || (*p != ',' && *p != ']')) && *p != '\n' && *p != '\0') {
									MelderString_appendCharacter (& index, *p);
									if (*p == '[') depth ++;
									else if (*p == ']') depth --;
									p ++;
								}
								if (*p == '\n' || *p == '\0')
									Melder_throw ("Missing closing bracket (]) in indexed variable.");
								double numericIndexValue;
								Interpreter_numericExpression (me, index.string, & numericIndexValue);
								MelderString_append (& indexedVariableName, Melder_double (numericIndexValue));
								MelderString_appendCharacter (& indexedVariableName, *p);
								if (*p == ']') {
									break;
								}
							}
							variableName = indexedVariableName.string;
							p ++;   // skip closing bracket
						}
						while (*p == ' ' || *p == '\t') p ++;   /* Go to first token after (perhaps indexed) variable name. */
						if (*p == '=') {
							withFile = 0;   /* Assignment. */
						} else if (*p == '<') {
							withFile = 1;   /* Read from file. */
						} else if (*p == '>') {
							if (p [1] == '>')
								withFile = 2, p ++;   /* Append to file. */
							else
								withFile = 3;   /* Save to file. */
						} else Melder_throw ("Missing '=', '<', or '>' after variable ", variableName, ".");
						*endOfVariable = '\0';
						p ++;
						while (*p == ' ' || *p == '\t') p ++;   /* Go to first token after assignment or I/O symbol. */
						if (*p == '\0') {
							if (withFile != 0)
								Melder_throw ("Missing file name after variable ", variableName, ".");
							else
								Melder_throw ("Missing expression after variable ", variableName, ".");
						}
						if (withFile) {
							structMelderFile file = { 0 };
							Melder_relativePathToFile (p, & file);
							if (withFile == 1) {
								wchar_t *stringValue = MelderFile_readText (& file);
								InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
								Melder_free (var -> stringValue);
								var -> stringValue = stringValue;   /* var becomes owner */
							} else if (withFile == 2) {
								if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw ("Commands that write to a file are not available inside pictures.");
								InterpreterVariable var = Interpreter_hasVariable (me, variableName);
								if (! var) Melder_throw ("Variable ", variableName, " undefined.");
								MelderFile_appendText (& file, var -> stringValue);
							} else {
								if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw ("Commands that write to a file are not available inside pictures.");
								InterpreterVariable var = Interpreter_hasVariable (me, variableName);
								if (! var) Melder_throw ("Variable ", variableName, " undefined.");
								MelderFile_writeText (& file, var -> stringValue, Melder_getOutputEncoding ());
							}
						} else if (isCommand (p)) {
							/*
							 * Example: name$ = Get name
							 */
							MelderString_empty (& valueString);   // empty because command may print nothing; also makes sure that valueString.string exists
							autoMelderDivertInfo divert (& valueString);
							praat_executeCommand (me, p);
							InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
							Melder_free (var -> stringValue);
							var -> stringValue = Melder_wcsdup (valueString.string);
						} else {
							/*
							 * Evaluate a string expression and assign the result to the variable.
							 * Examples:
							 *    sentence$ = subject$ + verb$ + object$
							 *    extension$ = if index (file$, ".") <> 0
							 *       ... then right$ (file$, length (file$) - rindex (file$, "."))
							 *       ... else "" fi
							 */
							wchar_t *stringValue;
							Interpreter_stringExpression (me, p, & stringValue);
							InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
							Melder_free (var -> stringValue);
							var -> stringValue = stringValue;   /* var becomes owner */
						}
					} else if (*p == '#') {
						/*
						 * Assign to a numeric array variable.
						 */
						wchar_t *endOfVariable = ++ p;
						while (*p == ' ' || *p == '\t') p ++;   // Go to first token after variable name.
						if (*p == '=') {
							;
						} else Melder_throw ("Missing '=' after variable ", command2.string, ".");
						*endOfVariable = '\0';
						p ++;
						while (*p == ' ' || *p == '\t') p ++;   // Go to first token after assignment or I/O symbol.
						if (*p == '\0') {
							Melder_throw ("Missing expression after variable ", command2.string, ".");
						}
						struct Formula_NumericArray value;
						Interpreter_numericArrayExpression (me, p, & value);
						InterpreterVariable var = Interpreter_lookUpVariable (me, command2.string);
						NUMmatrix_free (var -> numericArrayValue. data, 1, 1);
						var -> numericArrayValue = value;
					} else {
						/*
						 * Try to assign to a numeric variable.
						 */
						double value;
						wchar_t *variableName = command2.string;
						int typeOfAssignment = 0;   /* Plain assignment. */
						if (*p == '\0') {
							/*
							 * Command ends here: it may be a PraatShell command.
							 */
							praat_executeCommand (me, command2.string);
							continue;   // next line
						}
						wchar_t *endOfVariable = p;
						while (*p == ' ' || *p == '\t') p ++;
						if (*p == '=' || ((*p == '+' || *p == '-' || *p == '*' || *p == '/') && p [1] == '=')) {
							/*
							 * This must be an assignment (though: "echo = ..." ???)
							 */
							typeOfAssignment = *p == '+' ? 1 : *p == '-' ? 2 : *p == '*' ? 3 : *p == '/' ? 4 : 0;
							*endOfVariable = '\0';   // Close variable name. FIXME: this can be any weird character, e.g. hallo&
						} else if (*p == '[') {
							/*
							 * This must be an assignment to an indexed numeric variable.
							 */
							*endOfVariable = '\0';
							static MelderString indexedVariableName = { 0 };
							MelderString_copy (& indexedVariableName, command2.string);
							MelderString_appendCharacter (& indexedVariableName, '[');
							for (;;) {
								p ++;   // skip opening bracket or comma
								static MelderString index = { 0 };
								MelderString_empty (& index);
								int depth = 0;
								while ((depth > 0 || (*p != ',' && *p != ']')) && *p != '\n' && *p != '\0') {
									MelderString_appendCharacter (& index, *p);
									if (*p == '[') depth ++;
									else if (*p == ']') depth --;
									p ++;
								}
								if (*p == '\n' || *p == '\0')
									Melder_throw ("Missing closing bracket (]) in indexed variable.");
								Interpreter_numericExpression (me, index.string, & value);
								MelderString_append (& indexedVariableName, Melder_double (value));
								MelderString_appendCharacter (& indexedVariableName, *p);
								if (*p == ']') {
									break;
								}
							}
							variableName = indexedVariableName.string;
							p ++;   // skip closing bracket
							while (*p == ' ' || *p == '\t') p ++;
							if (*p == '=' || ((*p == '+' || *p == '-' || *p == '*' || *p == '/') && p [1] == '=')) {
								typeOfAssignment = *p == '+' ? 1 : *p == '-' ? 2 : *p == '*' ? 3 : *p == '/' ? 4 : 0;
							}
						} else {
							/*
							 * Not an assignment: perhaps a PraatShell command (select, echo, execute, pause ...).
							 */
							praat_executeCommand (me, variableName);
							continue;   // next line
						}
						p += typeOfAssignment == 0 ? 1 : 2;
						while (*p == ' ' || *p == '\t') p ++;			
						if (*p == '\0') Melder_throw ("Missing expression after variable ", variableName, ".");
						/*
						 * Three classes of assignments:
						 *    var = formula
						 *    var = Query
						 *    var = Object creation
						 */
						if (isCommand (p)) {
							/*
							 * Get the value of the query.
							 */
							MelderString_empty (& valueString);
							autoMelderDivertInfo divert (& valueString);
							MelderString_appendCharacter (& valueString, 1);
							praat_executeCommand (me, p);
							if (valueString.string [0] == 1) {
								int IOBJECT, result = 0, found = 0;
								WHERE (SELECTED) { result = IOBJECT; found += 1; }
								if (found > 1) {
									Melder_throw ("Multiple objects selected. Cannot assign ID to variable.");
								} else if (found == 0) {
									Melder_throw ("No objects selected. Cannot assign ID to variable.");
								} else {
									value = theCurrentPraatObjects -> list [result]. id;
								}
							} else {
								value = Melder_atof (valueString.string);   // including --undefined--
							}
						} else {
							/*
							 * Get the value of the formula.
							 */
							Interpreter_numericExpression (me, p, & value);
						}
						/*
						 * Assign the value to a variable.
						 */
						if (typeOfAssignment == 0) {
							/*
							 * Use an existing variable, or create a new one.
							 */
							//Melder_casual ("looking up variable %ls", variableName);
							InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
							var -> numericValue = value;
						} else {
							/*
							 * Modify an existing variable.
							 */
							InterpreterVariable var = Interpreter_hasVariable (me, variableName);
							if (var == NULL) Melder_throw ("Unknown variable ", variableName, ".");
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
				} // endif fail
				if (assertErrorLineNumber != 0 && assertErrorLineNumber != lineNumber) {
					long save_assertErrorLineNumber = assertErrorLineNumber;
					assertErrorLineNumber = 0;
					Melder_throw ("Script assertion fails in line ", save_assertErrorLineNumber,
							": error " L_LEFT_GUILLEMET " ", assertErrorString.string, " " L_RIGHT_GUILLEMET " not raised. Instead: no error.");
					
				}
			} catch (MelderError) {
				//Melder_casual ("Error: << %ls >>\nassertErrorLineNumber: %ld\nlineNumber: %ld\nAssert error string: << %ls >>\n",
				//	Melder_getError(), assertErrorLineNumber, lineNumber, assertErrorString.string);
				if (assertErrorLineNumber == 0) {
					throw;
				} else if (assertErrorLineNumber != lineNumber) {
					if (wcsstr (Melder_getError (), assertErrorString.string)) {
						Melder_clearError ();
						assertErrorLineNumber = 0;
					} else {
						wchar_t *errorCopy_nothrow = Melder_wcsdup_f (Melder_getError ());   // UGLY but necessary (1)
						Melder_clearError ();
						autostring errorCopy = errorCopy_nothrow;   // UGLY but necessary (2)
						Melder_throw ("Script assertion fails in line ", assertErrorLineNumber,
							": error " L_LEFT_GUILLEMET " ", assertErrorString.string, " " L_RIGHT_GUILLEMET " not raised. Instead:\n",
							errorCopy.peek());
					}
				}
			}
		} // endfor lineNumber
		my numberOfLabels = 0;
		my running = false;
		my stopped = false;
	} catch (MelderError) {
		if (! wcsnequ (lines [lineNumber], L"exit ", 5) && ! assertionFailed) {   // don't show the message twice!
			while (lines [lineNumber] [0] == '\0') {   // did this use to be a continuation line?
				lineNumber --;
				Melder_assert (lineNumber > 0);   // originally empty lines that stayed empty should not generate errors
			}
			Melder_error_ ("Script line ", lineNumber, " not performed or completed:\n" L_LEFT_GUILLEMET " ", lines [lineNumber], " " L_RIGHT_GUILLEMET);
		}
		my numberOfLabels = 0;
		my running = false;
		my stopped = false;
		throw;
	}
}

void Interpreter_stop (Interpreter me) {
//Melder_casual ("Interpreter_stop in: %ld", me);
	my stopped = true;
//Melder_casual ("Interpreter_stop out: %ld", me);
}

void Interpreter_voidExpression (Interpreter me, const wchar_t *expression) {
	Formula_compile (me, NULL, expression, kFormula_EXPRESSION_TYPE_NUMERIC, FALSE);
	struct Formula_Result result;
	Formula_run (0, 0, & result);
}

void Interpreter_numericExpression (Interpreter me, const wchar_t *expression, double *value) {
	Melder_assert (value != NULL);
	if (wcsstr (expression, L"(=")) {
		*value = Melder_atof (expression);
	} else {
		Formula_compile (me, NULL, expression, kFormula_EXPRESSION_TYPE_NUMERIC, FALSE);
		struct Formula_Result result;
		Formula_run (0, 0, & result);
		*value = result. result.numericResult;
	}
}

void Interpreter_stringExpression (Interpreter me, const wchar_t *expression, wchar_t **value) {
	Formula_compile (me, NULL, expression, kFormula_EXPRESSION_TYPE_STRING, FALSE);
	struct Formula_Result result;
	Formula_run (0, 0, & result);
	*value = result. result.stringResult;
}

void Interpreter_numericArrayExpression (Interpreter me, const wchar_t *expression, struct Formula_NumericArray *value) {
	Formula_compile (me, NULL, expression, kFormula_EXPRESSION_TYPE_NUMERIC_ARRAY, FALSE);
	struct Formula_Result result;
	Formula_run (0, 0, & result);
	*value = result. result.numericArrayResult;
}

void Interpreter_anyExpression (Interpreter me, const wchar_t *expression, struct Formula_Result *result) {
	Formula_compile (me, NULL, expression, kFormula_EXPRESSION_TYPE_UNKNOWN, FALSE);
	Formula_run (0, 0, result);
}

/* End of file Interpreter.cpp */
