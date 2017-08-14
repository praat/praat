/* Interpreter.cpp
 *
 * Copyright (C) 1993-2011,2013,2014,2015,2016,2017 Paul Boersma
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

void structInterpreterVariable :: v_destroy () noexcept {
	Melder_free (our string);
	Melder_free (our stringValue);
	NUMvector_free (our numericVectorValue.at, 1);
	NUMmatrix_free (our numericMatrixValue.at, 1, 1);
	InterpreterVariable_Parent :: v_destroy ();
}

static autoInterpreterVariable InterpreterVariable_create (const char32 *key) {
	try {
		if (key [0] == U'e' && key [1] == U'\0')
			Melder_throw (U"You cannot use 'e' as the name of a variable (e is the constant 2.71...).");
		if (key [0] == U'p' && key [1] == U'i' && key [2] == U'\0')
			Melder_throw (U"You cannot use 'pi' as the name of a variable (pi is the constant 3.14...).");
		if (key [0] == U'u' && key [1] == U'n' && key [2] == U'd' && key [3] == U'e' && key [4] == U'f' && key [5] == U'i' &&
			key [6] == U'n' && key [7] == U'e' && key [8] == U'd' && key [9] == U'\0')
			Melder_throw (U"You cannot use 'undefined' as the name of a variable.");
		autoInterpreterVariable me = Thing_new (InterpreterVariable);
		my string = Melder_dup (key);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Interpreter variable not created.");
	}
}

Thing_implement (Interpreter, Thing, 0);

void structInterpreter :: v_destroy () noexcept {
	Melder_free (our environmentName);
	for (int ipar = 1; ipar <= Interpreter_MAXNUM_PARAMETERS; ipar ++)
		Melder_free (our arguments [ipar]);
	//if (our variablesMap) {
		for (auto it = our variablesMap. begin(); it != our variablesMap. end(); it ++) {
			InterpreterVariable var = it -> second;
			forget (var);
		}
	//	delete (our variablesMap);
	//}
	Interpreter_Parent :: v_destroy ();
}

autoInterpreter Interpreter_create (char32 *environmentName, ClassInfo editorClass) {
	try {
		autoInterpreter me = Thing_new (Interpreter);
		//my variablesMap = new std::unordered_map <std::u32string, InterpreterVariable>;
		my variablesMap. max_load_factor (0.65f);
		my environmentName = Melder_dup (environmentName);
		my editorClass = editorClass;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Interpreter not created.");
	}
}

autoInterpreter Interpreter_createFromEnvironment (Editor editor) {
	if (! editor) return Interpreter_create (nullptr, nullptr);
	return Interpreter_create (editor -> name, editor -> classInfo);
}

void Melder_includeIncludeFiles (char32 **text) {
	for (int depth = 0; ; depth ++) {
		char32 *head = *text;
		long numberOfIncludes = 0;
		if (depth > 10)
			Melder_throw (U"Include files nested too deep. Probably cyclic.");
		for (;;) {
			char32 *includeLocation, *includeFileName, *tail, *newText;
			long headLength, includeTextLength, newLength;
			/*
				Look for an include statement. If not found, we have finished.
			 */
			includeLocation = str32nequ (head, U"include ", 8) ? head : str32str (head, U"\ninclude ");
			if (! includeLocation) break;
			if (includeLocation != head) includeLocation += 1;
			numberOfIncludes += 1;
			/*
				Separate out the head.
			 */
			*includeLocation = U'\0';
			/*
				Separate out the name of the include file.
			 */
			includeFileName = includeLocation + 8;
			while (*includeFileName == U' ' || *includeFileName == U'\t') includeFileName ++;
			tail = includeFileName;
			while (*tail != U'\n' && *tail != U'\0') tail ++;
			if (*tail == U'\n') {
				*tail = U'\0';
				tail += 1;
			}
			/*
				Get the contents of the include file.
			 */
			structMelderFile includeFile { };
			Melder_relativePathToFile (includeFileName, & includeFile);
			autostring32 includeText;
			try {
				includeText.reset (MelderFile_readText (& includeFile));
			} catch (MelderError) {
				Melder_throw (U"Include file ", & includeFile, U" not read.");
			}
			/*
				Construct the new text.
			 */
			headLength = (head - *text) + str32len (head);
			includeTextLength = str32len (includeText.peek());
			newLength = headLength + includeTextLength + 1 + str32len (tail);
			newText = Melder_malloc (char32, newLength + 1);
			str32cpy (newText, *text);
			str32cpy (newText + headLength, includeText.peek());
			str32cpy (newText + headLength + includeTextLength, U"\n");
			str32cpy (newText + headLength + includeTextLength + 1, tail);
			/*
				Replace the old text with the new. This will work even within an autostring.
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

inline static bool Melder_isblank (char32 kar) {
	return kar == U' ' || kar == U'\t';
}

long Interpreter_readParameters (Interpreter me, char32 *text) {
	char32 *formLocation = nullptr;
	long npar = 0;
	my dialogTitle [0] = U'\0';
	/*
	 * Look for a "form" line.
	 */
	{// scope
		char32 *p = text;
		for (;;) {
			while (Melder_isblank (*p)) p ++;
			if (str32nequ (p, U"form ", 5)) {
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
		char32 *dialogTitle = formLocation + 5, *newLine;
		while (*dialogTitle == U' ' || *dialogTitle == U'\t') dialogTitle ++;
		newLine = str32chr (dialogTitle, U'\n');
		if (newLine) *newLine = U'\0';
		str32cpy (my dialogTitle, dialogTitle);
		if (newLine) *newLine = U'\n';
		my numberOfParameters = 0;
		while (newLine) {
			char32 *line = newLine + 1, *p;
			int type = 0;
			while (Melder_isblank (*line)) line ++;
			while (*line == U'#' || *line == U';' || *line == U'!' || *line == U'\n') {
				newLine = str32chr (line, U'\n');
				if (! newLine)
					Melder_throw (U"Unfinished form.");
				line = newLine + 1;
				while (Melder_isblank (*line)) line ++;
			}
			if (str32nequ (line, U"endform", 7)) break;
			if (str32nequ (line, U"word ", 5)) { type = Interpreter_WORD; p = line + 5; }
			else if (str32nequ (line, U"real ", 5)) { type = Interpreter_REAL; p = line + 5; }
			else if (str32nequ (line, U"positive ", 9)) { type = Interpreter_POSITIVE; p = line + 9; }
			else if (str32nequ (line, U"integer ", 8)) { type = Interpreter_INTEGER; p = line + 8; }
			else if (str32nequ (line, U"natural ", 8)) { type = Interpreter_NATURAL; p = line + 8; }
			else if (str32nequ (line, U"boolean ", 8)) { type = Interpreter_BOOLEAN; p = line + 8; }
			else if (str32nequ (line, U"sentence ", 9)) { type = Interpreter_SENTENCE; p = line + 9; }
			else if (str32nequ (line, U"text ", 5)) { type = Interpreter_TEXT; p = line + 5; }
			else if (str32nequ (line, U"choice ", 7)) { type = Interpreter_CHOICE; p = line + 7; }
			else if (str32nequ (line, U"optionmenu ", 11)) { type = Interpreter_OPTIONMENU; p = line + 11; }
			else if (str32nequ (line, U"button ", 7)) { type = Interpreter_BUTTON; p = line + 7; }
			else if (str32nequ (line, U"option ", 7)) { type = Interpreter_OPTION; p = line + 7; }
			else if (str32nequ (line, U"comment ", 8)) { type = Interpreter_COMMENT; p = line + 8; }
			else {
				newLine = str32chr (line, U'\n');
				if (newLine) *newLine = U'\0';
				Melder_appendError (U"Unknown parameter type:\n\"", line, U"\".");
				if (newLine) *newLine = U'\n';
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
				while (Melder_isblank (*p)) p ++;
				if (*p == U'\n' || *p == U'\0')
					Melder_throw (U"Missing parameter:\n\"", line, U"\".");
				char32 *q = my parameters [++ my numberOfParameters];
				while (*p != U' ' && *p != U'\t' && *p != U'\n' && *p != U'\0') * (q ++) = * (p ++);
				*q = U'\0';
				npar ++;
			} else {
				my parameters [++ my numberOfParameters] [0] = U'\0';
			}
			while (Melder_isblank (*p)) p ++;
			newLine = str32chr (p, U'\n');
			if (newLine) *newLine = U'\0';
			Melder_free (my arguments [my numberOfParameters]);
			my arguments [my numberOfParameters] = Melder_dup_f (p);
			if (newLine) *newLine = U'\n';
			my types [my numberOfParameters] = type;
		}
	} else {
		npar = my numberOfParameters = 0;
	}
	return npar;
}

UiForm Interpreter_createForm (Interpreter me, GuiWindow parent, const char32 *path,
	void (*okCallback) (UiForm, int, Stackel, const char32 *, Interpreter, const char32 *, bool, void *), void *okClosure,
	bool selectionOnly)
{
	UiForm form = UiForm_create (parent,
		Melder_cat (selectionOnly ? U"Run script (selection only): " : U"Run script: ", my dialogTitle),
		okCallback, okClosure, nullptr, nullptr);
	UiField radio = nullptr;
	if (path) UiForm_addText (form, U"$file", path);
	for (int ipar = 1; ipar <= my numberOfParameters; ipar ++) {
		/*
		 * Convert underscores to spaces.
		 */
		char32 parameter [100], *p = & parameter [0];
		str32cpy (parameter, my parameters [ipar]);
		while (*p) { if (*p == U'_') *p = U' '; p ++; }
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
				UiForm_addBoolean (form, parameter, my arguments [ipar] [0] == U'1' ||
					my arguments [ipar] [0] == U'y' || my arguments [ipar] [0] == U'Y' ||
					(my arguments [ipar] [0] == U'o' && my arguments [ipar] [1] == U'n')); break;
			case Interpreter_SENTENCE:
				UiForm_addSentence (form, parameter, my arguments [ipar]); break;
			case Interpreter_TEXT:
				UiForm_addText (form, parameter, my arguments [ipar]); break;
			case Interpreter_CHOICE:
				radio = UiForm_addRadio (form, parameter, a32tol (my arguments [ipar])); break;
			case Interpreter_OPTIONMENU:
				radio = UiForm_addOptionMenu (form, parameter, a32tol (my arguments [ipar])); break;
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
		if ((p = str32chr (my parameters [ipar], U'(')) != nullptr) {
			*p = U'\0';
			if (p - my parameters [ipar] > 0 && p [-1] == U'_') p [-1] = U'\0';
		}
		p = my parameters [ipar];
		if (*p != U'\0' && p [str32len (p) - 1] == U':') p [str32len (p) - 1] = U'\0';
	}
	UiForm_finish (form);
	return form;
}

void Interpreter_getArgumentsFromDialog (Interpreter me, UiForm dialog) {
	for (int ipar = 1; ipar <= my numberOfParameters; ipar ++) {
		char32 parameter [100], *p;
		/*
		 * Strip parentheses and colon off parameter name.
		 */
		if ((p = str32chr (my parameters [ipar], U'(')) != nullptr) {
			*p = U'\0';
			if (p - my parameters [ipar] > 0 && p [-1] == U'_') p [-1] = U'\0';
		}
		p = my parameters [ipar];
		if (*p != U'\0' && p [str32len (p) - 1] == U':') p [str32len (p) - 1] = U'\0';
		/*
		 * Convert underscores to spaces.
		 */
		str32cpy (parameter, my parameters [ipar]);
		p = & parameter [0]; while (*p) { if (*p == U'_') *p = U' '; p ++; }
		switch (my types [ipar]) {
			case Interpreter_REAL:
			case Interpreter_POSITIVE: {
				double value = UiForm_getReal_check (dialog, parameter);
				Melder_free (my arguments [ipar]);
				my arguments [ipar] = Melder_calloc_f (char32, 40);
				Melder_sprint (my arguments [ipar],40, value);
				break;
			}
			case Interpreter_INTEGER:
			case Interpreter_NATURAL:
			case Interpreter_BOOLEAN: {
				long value = UiForm_getInteger (dialog, parameter);
				Melder_free (my arguments [ipar]);
				my arguments [ipar] = Melder_calloc_f (char32, 40);
				Melder_sprint (my arguments [ipar],40, value);
				break;
			}
			case Interpreter_CHOICE:
			case Interpreter_OPTIONMENU: {
				long integerValue = 0;
				char32 *stringValue = nullptr;
				integerValue = UiForm_getInteger (dialog, parameter);
				stringValue = UiForm_getString (dialog, parameter);
				Melder_free (my arguments [ipar]);
				my arguments [ipar] = Melder_calloc_f (char32, 40);
				Melder_sprint (my arguments [ipar],40, integerValue);
				Melder_sprint (my choiceArguments [ipar],100, stringValue);
				break;
			}
			case Interpreter_BUTTON:
			case Interpreter_OPTION:
			case Interpreter_COMMENT:
				break;
			default: {
				char32 *value = UiForm_getString (dialog, parameter);
				Melder_free (my arguments [ipar]);
				my arguments [ipar] = Melder_dup_f (value);
				break;
			}
		}
	}
}

void Interpreter_getArgumentsFromString (Interpreter me, const char32 *arguments) {
	int size = my numberOfParameters;
	long length = str32len (arguments);
	while (size >= 1 && my parameters [size] [0] == U'\0')
		size --;   /* Ignore fields without a variable name (button, comment). */
	for (int ipar = 1; ipar <= size; ipar ++) {
		char32 *p = my parameters [ipar];
		/*
		 * Ignore buttons and comments again.
		 */
		if (! *p) continue;
		/*
		 * Strip parentheses and colon off parameter name.
		 */
		if ((p = str32chr (p, U'(')) != nullptr) {
			*p = U'\0';
			if (p - my parameters [ipar] > 0 && p [-1] == U'_') p [-1] = U'\0';
		}
		p = my parameters [ipar];
		if (*p != U'\0' && p [str32len (p) - 1] == U':') p [str32len (p) - 1] = U'\0';
	}
	for (int ipar = 1; ipar < size; ipar ++) {
		int ichar = 0;
		/*
		 * Ignore buttons and comments again. The buttons will keep their labels as "arguments".
		 */
		if (my parameters [ipar] [0] == U'\0') continue;
		Melder_free (my arguments [ipar]);   // erase the current values, probably the default values
		my arguments [ipar] = Melder_calloc_f (char32, length + 1);   // replace with the actual arguments
		/*
		 * Skip spaces until next argument.
		 */
		while (*arguments == U' ' || *arguments == U'\t') arguments ++;
		/*
		 * The argument is everything up to the next space, or, if that starts with a double quote,
		 * everything between this quote and the matching double quote;
		 * in this case, the argument can represent a double quote by a sequence of two double quotes.
		 * Example: the string
		 *     "I said ""hello"""
		 * will be passed to the dialog as a single argument containing the text
		 *     I said "hello"
		 */
		if (*arguments == U'\"') {
			arguments ++;   // do not include leading double quote
			for (;;) {
				if (*arguments == U'\0')
					Melder_throw (U"Missing matching quote.");
				if (*arguments == U'\"' && * ++ arguments != U'\"') break;   // remember second quote
				my arguments [ipar] [ichar ++] = *arguments ++;
			}
		} else {
			while (*arguments != U' ' && *arguments != U'\t' && *arguments != U'\0')
				my arguments [ipar] [ichar ++] = *arguments ++;
		}
		my arguments [ipar] [ichar] = U'\0';   // trailing null byte
	}
	/* The last item is handled separately, because it consists of the rest of the line.
	 * Leading spaces are skipped, but trailing spaces are included.
	 */
	if (size > 0) {
		while (Melder_isblank (*arguments)) arguments ++;
		Melder_free (my arguments [size]);
		my arguments [size] = Melder_dup_f (arguments);
	}
	/*
	 * Convert booleans and choices to numbers.
	 */
	for (int ipar = 1; ipar <= size; ipar ++) {
		if (my types [ipar] == Interpreter_BOOLEAN) {
			char32 *arg = & my arguments [ipar] [0];
			if (str32equ (arg, U"1") || str32equ (arg, U"yes") || str32equ (arg, U"on") ||
			    str32equ (arg, U"Yes") || str32equ (arg, U"On") || str32equ (arg, U"YES") || str32equ (arg, U"ON"))
			{
				str32cpy (arg, U"1");
			} else if (str32equ (arg, U"0") || str32equ (arg, U"no") || str32equ (arg, U"off") ||
			    str32equ (arg, U"No") || str32equ (arg, U"Off") || str32equ (arg, U"NO") || str32equ (arg, U"OFF"))
			{
				str32cpy (arg, U"0");
			} else {
				Melder_throw (U"Unknown value \"", arg, U"\" for boolean \"", my parameters [ipar], U"\".");
			}
		} else if (my types [ipar] == Interpreter_CHOICE) {
			int jpar;
			char32 *arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_BUTTON && my types [jpar] != Interpreter_OPTION)
					Melder_throw (U"Unknown value \"", arg, U"\" for choice \"", my parameters [ipar], U"\".");
				if (str32equ (my arguments [jpar], arg)) {   // the button labels are in the arguments; see Interpreter_readParameters
					str32cpy (arg, Melder_integer (jpar - ipar));
					str32cpy (my choiceArguments [ipar], my arguments [jpar]);
					break;
				}
			}
			if (jpar > my numberOfParameters)
				Melder_throw (U"Unknown value \"", arg, U"\" for choice \"", my parameters [ipar], U"\".");
		} else if (my types [ipar] == Interpreter_OPTIONMENU) {
			int jpar;
			char32 *arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_OPTION && my types [jpar] != Interpreter_BUTTON)
					Melder_throw (U"Unknown value \"", arg, U"\" for option menu \"", my parameters [ipar], U"\".");
				if (str32equ (my arguments [jpar], arg)) {
					str32cpy (arg, Melder_integer (jpar - ipar));
					str32cpy (my choiceArguments [ipar], my arguments [jpar]);
					break;
				}
			}
			if (jpar > my numberOfParameters)
				Melder_throw (U"Unknown value \"", arg, U"\" for option menu \"", my parameters [ipar], U"\".");
		}
	}
}

void Interpreter_getArgumentsFromArgs (Interpreter me, int narg, Stackel args) {
	trace (narg, U" arguments");
	int size = my numberOfParameters;
	while (size >= 1 && my parameters [size] [0] == '\0')
		size --;   // ignore trailing fields without a variable name (button, comment)
	for (int ipar = 1; ipar <= size; ipar ++) {
		char32 *p = my parameters [ipar];
		/*
		 * Ignore buttons and comments again.
		 */
		if (! *p) continue;
		/*
		 * Strip parentheses and colon off parameter name.
		 */
		if ((p = str32chr (p, U'(')) != nullptr) {
			*p = U'\0';
			if (p - my parameters [ipar] > 0 && p [-1] == U'_') p [-1] = U'\0';
		}
		p = my parameters [ipar];
		if (*p != U'\0' && p [str32len (p) - 1] == U':') p [str32len (p) - 1] = U'\0';
	}
	int iarg = 0;
	for (int ipar = 1; ipar <= size; ipar ++) {
		/*
		 * Ignore buttons and comments again. The buttons will keep their labels as "arguments".
		 */
		if (my parameters [ipar] [0] == U'\0') continue;
		Melder_free (my arguments [ipar]);   // erase the current values, probably the default values
		if (iarg == narg)
			Melder_throw (U"Found ", narg, U" arguments but expected more.");
		Stackel arg = & args [++ iarg];
		my arguments [ipar] =
			arg -> which == Stackel_NUMBER ? Melder_dup (Melder_double (arg -> number)) :
			arg -> which == Stackel_STRING ? Melder_dup (arg -> string) : nullptr;   // replace with the actual arguments
		Melder_assert (my arguments [ipar]);
	}
	if (iarg < narg)
		Melder_throw (U"Found ", narg, U" arguments but expected only ", iarg, U".");
	/*
	 * Convert booleans and choices to numbers.
	 */
	for (int ipar = 1; ipar <= size; ipar ++) {
		if (my types [ipar] == Interpreter_BOOLEAN) {
			char32 *arg = & my arguments [ipar] [0];
			if (str32equ (arg, U"1") || str32equ (arg, U"yes") || str32equ (arg, U"on") ||
			    str32equ (arg, U"Yes") || str32equ (arg, U"On") || str32equ (arg, U"YES") || str32equ (arg, U"ON"))
			{
				str32cpy (arg, U"1");
			} else if (str32equ (arg, U"0") || str32equ (arg, U"no") || str32equ (arg, U"off") ||
			    str32equ (arg, U"No") || str32equ (arg, U"Off") || str32equ (arg, U"NO") || str32equ (arg, U"OFF"))
			{
				str32cpy (arg, U"0");
			} else {
				Melder_throw (U"Unknown value \"", arg, U"\" for boolean \"", my parameters [ipar], U"\".");
			}
		} else if (my types [ipar] == Interpreter_CHOICE) {
			int jpar;
			char32 *arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_BUTTON && my types [jpar] != Interpreter_OPTION)
					Melder_throw (U"Unknown value \"", arg, U"\" for choice \"", my parameters [ipar], U"\".");
				if (str32equ (my arguments [jpar], arg)) {   // the button labels are in the arguments; see Interpreter_readParameters
					str32cpy (arg, Melder_integer (jpar - ipar));
					str32cpy (my choiceArguments [ipar], my arguments [jpar]);
					break;
				}
			}
			if (jpar > my numberOfParameters)
				Melder_throw (U"Unknown value \"", arg, U"\" for choice \"", my parameters [ipar], U"\".");
		} else if (my types [ipar] == Interpreter_OPTIONMENU) {
			int jpar;
			char32 *arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_OPTION && my types [jpar] != Interpreter_BUTTON)
					Melder_throw (U"Unknown value \"", arg, U"\" for option menu \"", my parameters [ipar], U"\".");
				if (str32equ (my arguments [jpar], arg)) {
					str32cpy (arg, Melder_integer (jpar - ipar));
					str32cpy (my choiceArguments [ipar], my arguments [jpar]);
					break;
				}
			}
			if (jpar > my numberOfParameters)
				Melder_throw (U"Unknown value \"", arg, U"\" for option menu \"", my parameters [ipar], U"\".");
		}
	}
}

static void Interpreter_addNumericVariable (Interpreter me, const char32 *key, double value) {
	autoInterpreterVariable variable = InterpreterVariable_create (key);
	variable -> numericValue = value;
	my variablesMap [key] = variable.get();   // YUCK
	variable.releaseToAmbiguousOwner();
}

static void Interpreter_addStringVariable (Interpreter me, const char32 *key, const char32 *value) {
	autoInterpreterVariable variable = InterpreterVariable_create (key);
	variable -> stringValue = Melder_dup (value);
	my variablesMap [key] = variable.get();   // YUCK
	variable.releaseToAmbiguousOwner();
}

InterpreterVariable Interpreter_hasVariable (Interpreter me, const char32 *key) {
	Melder_assert (key);
	auto it = my variablesMap. find (key [0] == U'.' ? Melder_cat (my procedureNames [my callDepth], key) : key);
	if (it != my variablesMap. end()) {
		return it -> second;
	} else {
		return nullptr;
	}
}

InterpreterVariable Interpreter_lookUpVariable (Interpreter me, const char32 *key) {
	Melder_assert (key);
	const char32 *variableNameIncludingProcedureName =
		key [0] == U'.' ? Melder_cat (my procedureNames [my callDepth], key) : key;
	auto it = my variablesMap. find (variableNameIncludingProcedureName);
	if (it != my variablesMap. end()) {
		return it -> second;
	}
	/*
	 * The variable doesn't yet exist: create a new one.
	 */
	autoInterpreterVariable variable = InterpreterVariable_create (variableNameIncludingProcedureName);
	InterpreterVariable variable_ref = variable.get();
	variable.releaseToAmbiguousOwner();   // YUCK
	my variablesMap [variableNameIncludingProcedureName] = variable_ref;
	return variable_ref;
}

static long lookupLabel (Interpreter me, const char32 *labelName) {
	for (long ilabel = 1; ilabel <= my numberOfLabels; ilabel ++)
		if (str32equ (labelName, my labelNames [ilabel]))
			return ilabel;
	Melder_throw (U"Unknown label \"", labelName, U"\".");
}

static bool isCommand (const char32 *p) {
	/*
	 * Things that start with "nowarn", "noprogress", or "nocheck" are commands.
	 */
	if (p [0] == U'n' && p [1] == U'o' &&
		(str32nequ (p + 2, U"warn ", 5) || str32nequ (p + 2, U"progress ", 9) || str32nequ (p + 2, U"check ", 6))) return true;
	if (str32nequ (p, U"demo ", 5)) return true;
	/*
	 * Otherwise, things that start with lower case are formulas.
	 */
	if (! isupper ((int) *p)) return false;
	/*
	 * The remaining possibility is things that start with upper case.
	 * If they contain an underscore, they are object names, hence we must have a formula.
	 * Otherwise, we have a command.
	 */
	while (isalnum ((int) *p)) p ++;
	return *p != '_';
}

static void parameterToVariable (Interpreter me, int type, const char32 *in_parameter, int ipar) {
	char32 parameter [200];
	Melder_assert (type != 0);
	str32cpy (parameter, in_parameter);
	if (type >= Interpreter_REAL && type <= Interpreter_BOOLEAN) {
		Interpreter_addNumericVariable (me, parameter, Melder_atof (my arguments [ipar]));
	} else if (type == Interpreter_CHOICE || type == Interpreter_OPTIONMENU) {
		Interpreter_addNumericVariable (me, parameter, Melder_atof (my arguments [ipar]));
		str32cpy (parameter + str32len (parameter), U"$");
		Interpreter_addStringVariable (me, parameter, my choiceArguments [ipar]);
	} else if (type == Interpreter_BUTTON || type == Interpreter_OPTION || type == Interpreter_COMMENT) {
		/* Do not add a variable. */
	} else {
		str32cpy (parameter + str32len (parameter), U"$");
		Interpreter_addStringVariable (me, parameter, my arguments [ipar]);
	}
}

void Interpreter_run (Interpreter me, char32 *text) {
	autoNUMvector <char32 *> lines;   // not autostringvector, because the elements are reference copies
	long lineNumber = 0;
	bool assertionFailed = false;
	try {
		static MelderString valueString { };   // to divert the info
		static MelderString assertErrorString { };
		char32 *command = text;
		autoMelderString command2;
		autoMelderString buffer;
		long numberOfLines = 0, assertErrorLineNumber = 0, callStack [1 + Interpreter_MAX_CALL_DEPTH];
		bool atLastLine = false, fromif = false, fromendfor = false;
		int callDepth = 0, chopped = 0, ipar;
		my callDepth = 0;
		/*
		 * The "environment" is null if we are in the Praat shell, or an editor otherwise.
		 */
		if (my editorClass) {
			praatP. editor = praat_findEditorFromString (my environmentName);
		} else {
			praatP. editor = nullptr;
		}
		/*
		 * Start.
		 */
		my running = true;
		/*
		 * Count lines and set the newlines to zero.
		 */
		while (! atLastLine) {
			char32 *endOfLine = command;
			while (*endOfLine != U'\n' && *endOfLine != U'\0') endOfLine ++;
			if (*endOfLine == U'\0') atLastLine = true;
			*endOfLine = U'\0';
			numberOfLines ++;
			command = endOfLine + 1;
		}
		/*
		 * Remember line starts and labels.
		 */
		lines.reset (1, numberOfLines);
		for (lineNumber = 1, command = text; lineNumber <= numberOfLines; lineNumber ++, command += str32len (command) + 1 + chopped) {
			int length;
			while (Melder_isblank (*command) || *command == UNICODE_NO_BREAK_SPACE) command ++;   // nbsp can occur for scripts copied from the manual
			length = str32len (command);
			/*
			 * Chop trailing spaces?
			 */
			/*chopped = 0;
			while (length > 0) { char kar = command [-- length]; if (kar != ' ' && kar != '\t') break; command [length] = '\0'; chopped ++; }*/
			lines [lineNumber] = command;
			if (str32nequ (command, U"label ", 6)) {
				int ilabel;
				for (ilabel = 1; ilabel <= my numberOfLabels; ilabel ++)
					if (str32equ (command + 6, my labelNames [ilabel]))
						Melder_throw (U"Duplicate label \"", command + 6, U"\".");
				if (my numberOfLabels >= Interpreter_MAXNUM_LABELS)
					Melder_throw (U"Too many labels.");
				str32ncpy (my labelNames [++ my numberOfLabels], command + 6, 1+Interpreter_MAX_LABEL_LENGTH);
				my labelNames [my numberOfLabels] [Interpreter_MAX_LABEL_LENGTH] = U'\0';
				my labelLines [my numberOfLabels] = lineNumber;
			}
		}
		/*
		 * Connect continuation lines.
		 */
		trace (U"connect continuation lines");
		for (lineNumber = numberOfLines; lineNumber >= 2; lineNumber --) {
			char32 *line = lines [lineNumber];
			if (line [0] == U'.' && line [1] == U'.' && line [2] == U'.') {
				char32 *previous = lines [lineNumber - 1];
				MelderString_copy (& command2, line + 3);
				MelderString_get (& command2, previous + str32len (previous));
				static char32 emptyLine [] = { U'\0' };
				lines [lineNumber] = emptyLine;
			}
		}
		/*
		 * Copy the parameter names and argument values into the array of variables.
		 */
		for (auto it = my variablesMap. begin(); it != my variablesMap. end(); it ++) {
			InterpreterVariable var = it -> second;
			forget (var);
		}
		my variablesMap. clear ();
		for (ipar = 1; ipar <= my numberOfParameters; ipar ++) {
			char32 parameter [200];
			/*
			 * Create variable names as-are and variable names without capitals.
			 */
			str32cpy (parameter, my parameters [ipar]);
			parameterToVariable (me, my types [ipar], parameter, ipar);
			if (parameter [0] >= U'A' && parameter [0] <= U'Z') {
				parameter [0] = (char32) tolower ((int) parameter [0]);
				parameterToVariable (me, my types [ipar], parameter, ipar);
			}
		}
		/*
		 * Initialize some variables.
		 */
		Interpreter_addStringVariable (me, U"newline$", U"\n");
		Interpreter_addStringVariable (me, U"tab$", U"\t");
		Interpreter_addStringVariable (me, U"shellDirectory$", Melder_getShellDirectory ());
		structMelderDir dir { }; Melder_getDefaultDir (& dir);
		Interpreter_addStringVariable (me, U"defaultDirectory$", Melder_dirToPath (& dir));
		Interpreter_addStringVariable (me, U"preferencesDirectory$", Melder_dirToPath (& praatDir));
		Melder_getHomeDir (& dir);
		Interpreter_addStringVariable (me, U"homeDirectory$", Melder_dirToPath (& dir));
		Melder_getTempDir (& dir);
		Interpreter_addStringVariable (me, U"temporaryDirectory$", Melder_dirToPath (& dir));
		#if defined (macintosh)
			Interpreter_addNumericVariable (me, U"macintosh", 1);
			Interpreter_addNumericVariable (me, U"windows", 0);
			Interpreter_addNumericVariable (me, U"unix", 0);
		#elif defined (_WIN32)
			Interpreter_addNumericVariable (me, U"macintosh", 0);
			Interpreter_addNumericVariable (me, U"windows", 1);
			Interpreter_addNumericVariable (me, U"unix", 0);
		#elif defined (UNIX)
			Interpreter_addNumericVariable (me, U"macintosh", 0);
			Interpreter_addNumericVariable (me, U"windows", 0);
			Interpreter_addNumericVariable (me, U"unix", 1);
		#else
			Interpreter_addNumericVariable (me, U"macintosh", 0);
			Interpreter_addNumericVariable (me, U"windows", 0);
			Interpreter_addNumericVariable (me, U"unix", 0);
		#endif
		Interpreter_addNumericVariable (me, U"left", 1);   // to accommodate scripts from before Praat 5.2.06
		Interpreter_addNumericVariable (me, U"right", 2);   // to accommodate scripts from before Praat 5.2.06
		Interpreter_addNumericVariable (me, U"mono", 1);   // to accommodate scripts from before Praat 5.2.06
		Interpreter_addNumericVariable (me, U"stereo", 2);   // to accommodate scripts from before Praat 5.2.06
		Interpreter_addNumericVariable (me, U"all", 0);   // to accommodate scripts from before Praat 5.2.06
		Interpreter_addNumericVariable (me, U"average", 0);   // to accommodate scripts from before Praat 5.2.06
		#define xstr(s) str(s)
		#define str(s) #s
		Interpreter_addStringVariable (me, U"praatVersion$", U"" xstr(PRAAT_VERSION_STR));
		Interpreter_addNumericVariable (me, U"praatVersion", PRAAT_VERSION_NUM);
		/*
		 * Execute commands.
		 */
		#define wordEnd(c)  (c == U'\0' || c == U' ' || c == U'\t')
		trace (U"going to handle ", numberOfLines, U" lines");
		//for (lineNumber = 1; lineNumber <= numberOfLines; lineNumber ++) {
			//trace (U"line ", lineNumber, U": ", lines [lineNumber]);
		//}
		for (lineNumber = 1; lineNumber <= numberOfLines; lineNumber ++) {
			if (my stopped) break;
			//trace (U"now at line ", lineNumber, U": ", lines [lineNumber]);
			//for (int lineNumber2 = 1; lineNumber2 <= numberOfLines; lineNumber2 ++) {
				//trace (U"  line ", lineNumber2, U": ", lines [lineNumber2]);
			//}
			try {
				char32 c0;
				bool fail = false;
				MelderString_copy (& command2, lines [lineNumber]);
				c0 = command2. string [0];
				if (c0 == U'\0') continue;
				/*
				 * Substitute variables.
				 */
				trace (U"substituting variables");
				for (char32 *p = & command2. string [0]; *p != U'\0'; p ++) if (*p == U'\'') {
					/*
					 * Found a left quote. Search for a matching right quote.
					 */
					char32 *q = p + 1, varName [300], *r, *s, *colon;
					int precision = -1;
					bool percent = false;
					while (*q != U'\0' && *q != U'\'' && q - p < 299) q ++;
					if (*q == U'\0') break;   // no matching right quote? done with this line!
					if (q - p == 1 || q - p >= 299) continue;   // ignore empty and too long variable names
					trace (U"found ", q - p - 1);
					/*
					 * Found a right quote. Get potential variable name.
					 */
					for (r = p + 1, s = varName; q - r > 0; r ++, s ++) *s = *r;
					*s = U'\0';   // trailing null byte
					colon = str32chr (varName, U':');
					if (colon) {
						precision = a32tol (colon + 1);
						if (str32chr (colon + 1, U'%')) percent = true;
						*colon = '\0';
					}
					InterpreterVariable var = Interpreter_hasVariable (me, varName);
					if (var) {
						/*
						 * Found a variable (p points to the left quote, q to the right quote). Substitute.
						 */
						int headlen = p - command2.string;
						const char32 *string = var -> stringValue ? var -> stringValue :
							percent ? Melder_percent (var -> numericValue, precision) :
							precision >= 0 ?  Melder_fixed (var -> numericValue, precision) :
							Melder_double (var -> numericValue);
						int arglen = str32len (string);
						MelderString_ncopy (& buffer, command2.string, headlen);
						MelderString_append (& buffer, string, q + 1);
						MelderString_copy (& command2, buffer.string);   // This invalidates p!! (really bad bug 20070203)
						p = command2.string + headlen + arglen - 1;
					} else {
						p = q - 1;   // go to before next quote
					}
				}
				trace (U"resume");
				c0 = command2.string [0];   // resume in order to allow things like 'c$' = 5
				if ((c0 < U'a' || c0 > U'z') && c0 != U'@' && ! (c0 == U'.' && command2.string [1] >= U'a' && command2.string [1] <= U'z')) {
					praat_executeCommand (me, command2.string);
				/*
				 * Interpret control flow and variables.
				 */
				} else switch (c0) {
					case U'.':
						fail = true;
						break;
					case U'@':
					{
						/*
						 * This is a function call.
						 * Look for a function name.
						 */
						char32 *p = command2.string + 1;
						while (Melder_isblank (*p)) p ++;   // skip whitespace
						char32 *callName = p;
						while (*p != U'\0' && *p != U' ' && *p != U'\t' && *p != U'(' && *p != U':') p ++;
						if (p == callName) Melder_throw (U"Missing procedure name after \"@\".");
						bool hasArguments = ( *p != U'\0' );
						if (hasArguments) {
							bool parenthesisOrColonFound = ( *p == U'(' || *p == U':' );
							*p = U'\0';   // close procedure name
							if (! parenthesisOrColonFound) {
								p ++;   // step over first white space
								while (Melder_isblank (*p)) p ++;   // skip more whitespace
								hasArguments = ( *p != U'\0' );
								parenthesisOrColonFound = ( *p == U'(' || *p == U':' );
								if (hasArguments && ! parenthesisOrColonFound)
									Melder_throw (U"Missing parenthesis or colon after procedure name \"", callName, U"\".");
							}
							p ++;   // step over parenthesis or colon
						}
						int64 callLength = str32len (callName);
						long iline = 1;
						for (; iline <= numberOfLines; iline ++) {
							char32 *linei = lines [iline], *q;
							if (linei [0] != U'p' || linei [1] != U'r' || linei [2] != U'o' || linei [3] != U'c' ||
								linei [4] != U'e' || linei [5] != U'd' || linei [6] != U'u' || linei [7] != U'r' ||
								linei [8] != U'e' || linei [9] != U' ') continue;
							q = lines [iline] + 10;
							while (Melder_isblank (*q)) q ++;   // skip whitespace before procedure name
							char32 *procName = q;
							while (*q != U'\0' && ! Melder_isblank (*q) && *q != U'(' && *q != U':') q ++;
							if (q == procName) Melder_throw (U"Missing procedure name after 'procedure'.");
							if (q - procName == callLength && str32nequ (procName, callName, callLength)) {
								/*
								 * We found the procedure definition.
								 */
								if (++ my callDepth > Interpreter_MAX_CALL_DEPTH)
									Melder_throw (U"Call depth greater than ", Interpreter_MAX_CALL_DEPTH, U".");
								str32cpy (my procedureNames [my callDepth], callName);
								bool parenthesisOrColonFound = ( *q == U'(' || *q == U':' );
								if (*q) q ++;   // step over parenthesis or colon or first white space
								if (! parenthesisOrColonFound) {
									while (Melder_isblank (*q)) q ++;   // skip more whitespace
									if (*q == U'(' || *q == U':') q ++;   // step over parenthesis or colon
								}
								while (*q && *q != U')') {
									static MelderString argument { };
									MelderString_empty (& argument);
									while (Melder_isblank (*p)) p ++;
									while (Melder_isblank (*q)) q ++;
									char32 *parameterName = q;
									while (*q != U'\0' && ! Melder_isblank (*q) && *q != U',' && *q != U')') q ++;   // collect parameter name
									int expressionDepth = 0;
									for (; *p; p ++) {
										if (*p == U',') {
											if (expressionDepth == 0) break;   // depth-0 comma ends expression
											MelderString_appendCharacter (& argument, U',');
										} else if (*p == U')') {
											if (expressionDepth == 0) break;   // depth-0 closing parenthesis ends expression
											expressionDepth --;
											MelderString_appendCharacter (& argument, U')');
										} else if (*p == U'(') {
											expressionDepth ++;
											MelderString_appendCharacter (& argument, U'(');
										} else if (*p == U'\"') {
											/*
											 * Enter a string literal.
											 */
											MelderString_appendCharacter (& argument, U'\"');
											p ++;
											for (;; p ++) {
												if (*p == U'\0') {
													Melder_throw (U"Incomplete string literal: the quotes don't match.");
												} else if (*p == U'\"') {
													MelderString_appendCharacter (& argument, U'\"');
													if (p [1] == '\"') {
														p ++;   // stay in the string literal
														MelderString_appendCharacter (& argument, U'\"');
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
									if (*p) { *p = U'\0'; p ++; }
									if (q [-1] == U'$') {
										char32 *value;
										my callDepth --;
										Interpreter_stringExpression (me, argument.string, & value);
										my callDepth ++;
										char32 save = *q; *q = U'\0';
										InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName); *q = save;
										Melder_free (var -> stringValue);
										var -> stringValue = value;
									} else if (q [-1] == U'#') {
										if (q [-2] == U'#') {
											nummat value;
											my callDepth --;
											Interpreter_numericMatrixExpression (me, argument.string, & value);
											my callDepth ++;
											char32 save = *q; *q = U'\0';
											InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName); *q = save;
											var -> numericMatrixValue. reset();
											var -> numericMatrixValue = value;
										} else {
											numvec value;
											my callDepth --;
											Interpreter_numericVectorExpression (me, argument.string, & value);
											my callDepth ++;
											char32 save = *q; *q = U'\0';
											InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName); *q = save;
											var -> numericVectorValue. reset();
											var -> numericVectorValue = value;
										}
									} else {
										double value;
										my callDepth --;
										Interpreter_numericExpression (me, argument.string, & value);
										my callDepth ++;
										char32 save = *q; *q = U'\0';
										InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName); *q = save;
										var -> numericValue = value;
									}
									if (*q) q ++;   // skip comma
								}
								if (callDepth == Interpreter_MAX_CALL_DEPTH)
									Melder_throw (U"Call depth greater than ", Interpreter_MAX_CALL_DEPTH, U".");
								callStack [++ callDepth] = lineNumber;
								lineNumber = iline;
								break;
							}
						}
						if (iline > numberOfLines) Melder_throw (U"Procedure \"", callName, U"\" not found.");
						break;
					}
					case U'a':
						if (str32nequ (command2.string, U"assert ", 7)) {
							double value;
							Interpreter_numericExpression (me, command2.string + 7, & value);
							if (value == 0.0 || isundef (value)) {
								assertionFailed = true;
								Melder_throw (U"Script assertion fails in line ", lineNumber,
									U" (", value == 0.0 ? U"false" : U"undefined", U"):\n   ", command2.string + 7);
							}
						} else if (str32nequ (command2.string, U"asserterror ", 12)) {
							MelderString_copy (& assertErrorString, command2.string + 12);
							assertErrorLineNumber = lineNumber;
						} else fail = true;
						break;
					case U'b':
						fail = true;
						break;
					case U'c':
						if (str32nequ (command2.string, U"call ", 5)) {
							char32 *p = command2.string + 5, *callName, *procName;
							long iline;
							bool hasArguments;
							int64 callLength;
							while (Melder_isblank (*p)) p ++;   // skip whitespace
							callName = p;
							while (*p != U'\0' && *p != U' ' && *p != U'\t' && *p != U'(' && *p != U':') p ++;
							if (p == callName) Melder_throw (U"Missing procedure name after 'call'.");
							hasArguments = *p != U'\0';
							*p = U'\0';   // close procedure name
							callLength = str32len (callName);
							for (iline = 1; iline <= numberOfLines; iline ++) {
								char32 *linei = lines [iline], *q;
								int hasParameters;
								if (linei [0] != U'p' || linei [1] != U'r' || linei [2] != U'o' || linei [3] != U'c' ||
									linei [4] != U'e' || linei [5] != U'd' || linei [6] != U'u' || linei [7] != U'r' ||
									linei [8] != U'e' || linei [9] != U' ') continue;
								q = lines [iline] + 10;
								while (Melder_isblank (*q)) q ++;
								procName = q;
								while (*q != U'\0' && *q != U' ' && *q != U'\t' && *q != U'(' && *q != U':') q ++;
								if (q == procName) Melder_throw (U"Missing procedure name after 'procedure'.");
								hasParameters = *q != U'\0';
								if (q - procName == callLength && str32nequ (procName, callName, callLength)) {
									if (hasArguments && ! hasParameters)
										Melder_throw (U"Call to procedure \"", callName, U"\" has too many arguments.");
									if (hasParameters && ! hasArguments)
										Melder_throw (U"Call to procedure \"", callName, U"\" has too few arguments.");
									if (++ my callDepth > Interpreter_MAX_CALL_DEPTH)
										Melder_throw (U"Call depth greater than ", Interpreter_MAX_CALL_DEPTH, U".");
									str32cpy (my procedureNames [my callDepth], callName);
									if (hasParameters) {
										bool parenthesisOrColonFound = ( *q == U'(' || *q == U':' );
										q ++;   // step over parenthesis or colon or first white space
										if (! parenthesisOrColonFound) {
											while (Melder_isblank (*q)) q ++;   // skip more whitespace
											if (*q == U'(' || *q == U':') q ++;   // step over parenthesis or colon
										}
										++ p;   // first argument
										while (*q && *q != ')') {
											char32 *par, save;
											static MelderString arg { };
											MelderString_empty (& arg);
											while (Melder_isblank (*p)) p ++;
											while (*q == U' ' || *q == U'\t' || *q == U',' || *q == U')') q ++;
											par = q;
											while (*q != U'\0' && *q != U' ' && *q != U'\t' && *q != U',' && *q != U')') q ++;   // collect parameter name
											if (*q) {   // does anything follow the parameter name?
												if (*p == U'\"') {
													p ++;   // skip initial quote
													while (*p != U'\0') {
														if (*p == U'\"') {   // quote signals end-of-string or string-internal quote
															if (p [1] == U'\"') {   // double quote signals string-internal quote
																MelderString_appendCharacter (& arg, U'\"');
																p += 2;   // skip second quote
															} else {   // single quote signals end-of-string
																break;
															}
														} else {
															MelderString_appendCharacter (& arg, *p ++);
														}
													}
												} else {
													while (*p != U'\0' && *p != U' ' && *p != U'\t')
														MelderString_appendCharacter (& arg, *p ++);   // white space separates
												}
												if (*p) { *p = U'\0'; p ++; }
											} else {   // else rest of line
												while (*p != '\0')
													MelderString_appendCharacter (& arg, *p ++);
											}
											if (q [-1] == '$') {
												save = *q; *q = U'\0';
												InterpreterVariable var = Interpreter_lookUpVariable (me, par); *q = save;
												Melder_free (var -> stringValue);
												var -> stringValue = Melder_dup_f (arg.string);
											} else {
												double value;
												my callDepth --;
												Interpreter_numericExpression (me, arg.string, & value);
												my callDepth ++;
												save = *q; *q = U'\0';
												InterpreterVariable var = Interpreter_lookUpVariable (me, par); *q = save;
												var -> numericValue = value;
											}
										}
									}
									if (callDepth == Interpreter_MAX_CALL_DEPTH)
										Melder_throw (U"Call depth greater than ", Interpreter_MAX_CALL_DEPTH, U".");
									callStack [++ callDepth] = lineNumber;
									lineNumber = iline;
									break;
								}
							}
							if (iline > numberOfLines) Melder_throw (U"Procedure \"", callName, U"\" not found.");
						} else fail = true;
						break;
					case U'd':
						if (str32nequ (command2.string, U"dec ", 4)) {
							InterpreterVariable var = Interpreter_lookUpVariable (me, command2.string + 4);
							var -> numericValue -= 1.0;
						} else fail = true;
						break;
					case U'e':
						if (command2.string [1] == 'n' && command2.string [2] == 'd') {
							if (str32nequ (command2.string, U"endif", 5) && wordEnd (command2.string [5])) {
								/* Ignore. */
							} else if (str32nequ (command2.string, U"endfor", 6) && wordEnd (command2.string [6])) {
								int depth = 0;
								long iline;
								for (iline = lineNumber - 1; iline > 0; iline --) {
									char32 *line = lines [iline];
									if (line [0] == U'f' && line [1] == U'o' && line [2] == U'r' && line [3] == U' ') {
										if (depth == 0) { lineNumber = iline - 1; fromendfor = true; break; }   // go before 'for'
										else depth --;
									} else if (str32nequ (lines [iline], U"endfor", 6) && wordEnd (lines [iline] [6])) {
										depth ++;
									}
								}
								if (iline <= 0) Melder_throw (U"Unmatched 'endfor'.");
							} else if (str32nequ (command2.string, U"endwhile", 8) && wordEnd (command2.string [8])) {
								int depth = 0;
								long iline;
								for (iline = lineNumber - 1; iline > 0; iline --) {
									if (str32nequ (lines [iline], U"while ", 6)) {
										if (depth == 0) { lineNumber = iline - 1; break; }   // go before 'while'
										else depth --;
									} else if (str32nequ (lines [iline], U"endwhile", 8) && wordEnd (lines [iline] [8])) {
										depth ++;
									}
								}
								if (iline <= 0) Melder_throw (U"Unmatched 'endwhile'.");
							} else if (str32nequ (command2.string, U"endproc", 7) && wordEnd (command2.string [7])) {
								if (callDepth == 0) Melder_throw (U"Unmatched 'endproc'.");
								lineNumber = callStack [callDepth --];
								-- my callDepth;
							} else fail = true;
						} else if (str32nequ (command2.string, U"else", 4) && wordEnd (command2.string [4])) {
							int depth = 0;
							long iline;
							for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
								if (str32nequ (lines [iline], U"endif", 5) && wordEnd (lines [iline] [5])) {
									if (depth == 0) { lineNumber = iline; break; }   // go after 'endif'
									else depth --;
								} else if (str32nequ (lines [iline], U"if ", 3)) {
									depth ++;
								}
							}
							if (iline > numberOfLines) Melder_throw (U"Unmatched 'else'.");
						} else if (str32nequ (command2.string, U"elsif ", 6) || str32nequ (command2.string, U"elif ", 5)) {
							if (fromif) {
								double value;
								fromif = false;
								Interpreter_numericExpression (me, command2.string + 5, & value);
								if (value == 0.0) {
									int depth = 0;
									long iline;
									for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
										if (str32nequ (lines [iline], U"endif", 5) && wordEnd (lines [iline] [5])) {
											if (depth == 0) { lineNumber = iline; break; }   // go after 'endif'
											else depth --;
										} else if (str32nequ (lines [iline], U"else", 4) && wordEnd (lines [iline] [4])) {
											if (depth == 0) { lineNumber = iline; break; }   // go after 'else'
										} else if ((str32nequ (lines [iline], U"elsif", 5) && wordEnd (lines [iline] [5]))
											|| (str32nequ (lines [iline], U"elif", 4) && wordEnd (lines [iline] [4]))) {
											if (depth == 0) { lineNumber = iline - 1; fromif = true; break; }   // go at next 'elsif' or 'elif'
										} else if (str32nequ (lines [iline], U"if ", 3)) {
											depth ++;
										}
									}
									if (iline > numberOfLines) Melder_throw (U"Unmatched 'elsif'.");
								}
							} else {
								int depth = 0;
								long iline;
								for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (str32nequ (lines [iline], U"endif", 5) && wordEnd (lines [iline] [5])) {
										if (depth == 0) { lineNumber = iline; break; }   /* Go after 'endif'. */
										else depth --;
									} else if (str32nequ (lines [iline], U"if ", 3)) {
										depth ++;
									}
								}
								if (iline > numberOfLines) Melder_throw (U"'elsif' not matched with 'endif'.");
							}
						} else if (str32nequ (command2.string, U"exit", 4)) {
							if (command2.string [4] == U'\0') {
								lineNumber = numberOfLines;   /* Go after end. */
							} else if (command2.string [4] == U' ') {
								Melder_throw (command2.string + 5);
							} else fail = true;
						} else if (str32nequ (command2.string, U"echo ", 5)) {
							/*
							 * Make sure that lines like "echo = 3" will not be regarded as assignments.
							 */
							praat_executeCommand (me, command2.string);
						} else fail = true;
						break;
					case U'f':
						if (command2.string [1] == U'o' && command2.string [2] == U'r' && command2.string [3] == U' ') {   // for_
							double toValue, loopVariable;
							char32 *frompos = str32str (command2.string, U" from "), *topos = str32str (command2.string, U" to ");
							char32 *varpos = command2.string + 4, *endvar = frompos;
							if (! topos) Melder_throw (U"Missing \'to\' in \'for\' loop.");
							if (! endvar) endvar = topos;
							while (*endvar == U' ') { *endvar = '\0'; endvar --; }
							while (*varpos == U' ') varpos ++;
							if (endvar - varpos < 0) Melder_throw (U"Missing loop variable after \'for\'.");
							InterpreterVariable var = Interpreter_lookUpVariable (me, varpos);
							Interpreter_numericExpression (me, topos + 4, & toValue);
							if (fromendfor) {
								fromendfor = false;
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
									if (str32nequ (lines [iline], U"endfor", 6)) {
										if (depth == 0) { lineNumber = iline; break; }   // go after 'endfor'
										else depth --;
									} else if (str32nequ (lines [iline], U"for ", 4)) {
										depth ++;
									}
								}
								if (iline > numberOfLines) Melder_throw (U"Unmatched 'for'.");
							}
						} else if (str32nequ (command2.string, U"form ", 5)) {
							long iline;
							for (iline = lineNumber + 1; iline <= numberOfLines; iline ++)
								if (str32nequ (lines [iline], U"endform", 7))
									{ lineNumber = iline; break; }   // go after 'endform'
							if (iline > numberOfLines) Melder_throw (U"Unmatched 'form'.");
						} else fail = true;
						break;
					case U'g':
						if (str32nequ (command2.string, U"goto ", 5)) {
							char32 labelName [1+Interpreter_MAX_LABEL_LENGTH];
							str32ncpy (labelName, command2.string + 5, 1+Interpreter_MAX_LABEL_LENGTH);
							labelName [Interpreter_MAX_LABEL_LENGTH] = U'\0';
							char32 *space = str32chr (labelName, U' ');
							if (space == labelName) Melder_throw (U"Missing label name after 'goto'.");
							bool dojump = true;
							if (space) {
								double value;
								*space = '\0';
								Interpreter_numericExpression (me, command2.string + 6 + str32len (labelName), & value);
								if (value == 0.0) dojump = false;
							}
							if (dojump) {
								int ilabel = lookupLabel (me, labelName);
								lineNumber = my labelLines [ilabel];   // loop will add 1
							}
						} else fail = true;
						break;
					case U'h':
						fail = true;
						break;
					case U'i':
						if (command2.string [1] == U'f' && command2.string [2] == U' ') {   // if_
							double value;
							Interpreter_numericExpression (me, command2.string + 3, & value);
							if (value == 0.0) {
								int depth = 0;
								long iline;
								for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (str32nequ (lines [iline], U"endif", 5)) {
										if (depth == 0) { lineNumber = iline; break; }   // go after 'endif'
										else depth --;
									} else if (str32nequ (lines [iline], U"else", 4)) {
										if (depth == 0) { lineNumber = iline; break; }   // go after 'else'
									} else if (str32nequ (lines [iline], U"elsif ", 6) || str32nequ (lines [iline], U"elif ", 5)) {
										if (depth == 0) { lineNumber = iline - 1; fromif = true; break; }   // go at 'elsif'
									} else if (str32nequ (lines [iline], U"if ", 3)) {
										depth ++;
									}
								}
								if (iline > numberOfLines) Melder_throw (U"Unmatched 'if'.");
							} else if (isundef (value)) {
								Melder_throw (U"The value of the 'if' condition is undefined.");
							}
						} else if (str32nequ (command2.string, U"inc ", 4)) {
							InterpreterVariable var = Interpreter_lookUpVariable (me, command2.string + 4);
							var -> numericValue += 1.0;
						} else fail = true;
						break;
					case U'j':
						fail = true;
						break;
					case U'k':
						fail = true;
						break;
					case U'l':
						if (str32nequ (command2.string, U"label ", 6)) {
							;   // ignore labels
						} else fail = true;
						break;
					case U'm':
						fail = true;
						break;
					case U'n':
						fail = true;
						break;
					case U'o':
						fail = true;
						break;
					case U'p':
						if (str32nequ (command2.string, U"procedure ", 10)) {
							long iline = lineNumber + 1;
							for (; iline <= numberOfLines; iline ++) {
								if (str32nequ (lines [iline], U"endproc", 7) && wordEnd (lines [iline] [7])) {
									lineNumber = iline;
									break;
								}   // go after 'endproc'
							}
							if (iline > numberOfLines) Melder_throw (U"Unmatched 'proc'.");
						} else if (str32nequ (command2.string, U"print", 5)) {
							/*
							 * Make sure that lines like "print = 3" will not be regarded as assignments.
							 */
							if (command2.string [5] == U' ' || (str32nequ (command2.string + 5, U"line", 4) && (command2.string [9] == U' ' || command2.string [9] == U'\0'))) {
								praat_executeCommand (me, command2.string);
							} else fail = true;
						} else fail = true;
						break;
					case U'q':
						fail = true;
						break;
					case U'r':
						if (str32nequ (command2.string, U"repeat", 6) && wordEnd (command2.string [6])) {
							/* Ignore. */
						} else fail = true;
						break;
					case U's':
						if (str32nequ (command2.string, U"stopwatch", 9) && wordEnd (command2.string [9])) {
							(void) Melder_stopwatch ();   /* Reset stopwatch. */
						} else fail = true;
						break;
					case U't':
						fail = true;
						break;
					case U'u':
						if (str32nequ (command2.string, U"until ", 6)) {
							double value;
							Interpreter_numericExpression (me, command2.string + 6, & value);
							if (value == 0.0) {
								int depth = 0;
								long iline;
								for (iline = lineNumber - 1; iline > 0; iline --) {
									if (str32nequ (lines [iline], U"repeat", 6) && wordEnd (lines [iline] [6])) {
										if (depth == 0) { lineNumber = iline; break; }   // go after 'repeat'
										else depth --;
									} else if (str32nequ (lines [iline], U"until ", 6)) {
										depth ++;
									}
								}
								if (iline <= 0) Melder_throw (U"Unmatched 'until'.");
							}
						} else fail = true;
						break;
					case U'v':
						fail = true;
						break;
					case U'w':
						if (str32nequ (command2.string, U"while ", 6)) {
							double value;
							Interpreter_numericExpression (me, command2.string + 6, & value);
							if (value == 0.0) {
								int depth = 0;
								long iline;
								for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (str32nequ (lines [iline], U"endwhile", 8) && wordEnd (lines [iline] [8])) {
										if (depth == 0) { lineNumber = iline; break; }   // go after 'endwhile'
										else depth --;
									} else if (str32nequ (lines [iline], U"while ", 6)) {
										depth ++;
									}
								}
								if (iline > numberOfLines) Melder_throw (U"Unmatched 'while'.");
							}
						} else fail = true;
						break;
					case U'x':
						fail = true;
						break;
					case U'y':
						fail = true;
						break;
					case U'z':
						fail = true;
						break;
					default: break;
				}
				if (fail) {
					/*
					 * Found an unknown word starting with a lower-case letter, optionally preceded by a period.
					 * See whether the word is a variable name.
					 */
					trace (U"found an unknown word starting with a lower-case letter, optionally preceded by a period");
					char32 *p = & command2.string [0];
					/*
					 * Variable names consist of a sequence of letters, digits, and underscores,
					 * optionally preceded by a period and optionally followed by a $ and/or #.
					 */
					if (*p == U'.') p ++;
					while (isalnum ((int) *p) || *p == U'_' || *p == U'.')  p ++;
					if (*p == U'$') {
						/*
						 * Assign to a string variable.
						 */
						trace (U"detected an assignment to a string variable");
						char32 *endOfVariable = ++ p;
						char32 *variableName = command2.string;
						int withFile;
						while (Melder_isblank (*p)) p ++;   // go to first token after variable name
						if (*p == U'[') {
							/*
							 * This must be an assignment to an indexed string variable.
							 */
							*endOfVariable = U'\0';
							static MelderString indexedVariableName { };
							MelderString_copy (& indexedVariableName, command2.string, U"[");
							for (;;) {
								p ++;   // skip opening bracket or comma
								static MelderString index { };
								MelderString_empty (& index);
								int depth = 0;
								bool inString = false;
								while ((depth > 0 || (*p != U',' && *p != U']') || inString) && *p != U'\n' && *p != U'\0') {
									MelderString_appendCharacter (& index, *p);
									if (*p == U'[') {
										if (! inString) depth ++;
									} else if (*p == U']') {
										if (! inString) depth --;
									}
									if (*p == U'"') inString = ! inString;
									p ++;
								}
								if (*p == U'\n' || *p == U'\0')
									Melder_throw (U"Missing closing bracket (]) in indexed variable.");
								Formula_Result result;
								Interpreter_anyExpression (me, index.string, & result);
								if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									double numericIndexValue = result.result.numericResult;
									MelderString_append (& indexedVariableName, numericIndexValue);
								} else if (result.expressionType == kFormula_EXPRESSION_TYPE_STRING) {
									MelderString_append (& indexedVariableName, U"\"", result.result.stringResult, U"\"");
									Melder_free (result.result.stringResult);
								}
								MelderString_appendCharacter (& indexedVariableName, *p);
								if (*p == U']') {
									break;
								}
							}
							variableName = indexedVariableName.string;
							p ++;   // skip closing bracket
						}
						while (Melder_isblank (*p)) p ++;   // go to first token after (perhaps indexed) variable name
						if (*p == U'=') {
							withFile = 0;   // assignment
						} else if (*p == U'<') {
							withFile = 1;   // read from file
						} else if (*p == U'>') {
							if (p [1] == U'>')
								withFile = 2, p ++;   // append to file
							else
								withFile = 3;   // save to file
						} else Melder_throw (U"Missing '=', '<', or '>' after variable ", variableName, U".");
						*endOfVariable = U'\0';
						p ++;
						while (Melder_isblank (*p)) p ++;   // go to first token after assignment or I/O symbol
						if (*p == U'\0') {
							if (withFile != 0)
								Melder_throw (U"Missing file name after variable ", variableName, U".");
							else
								Melder_throw (U"Missing expression after variable ", variableName, U".");
						}
						if (withFile) {
							structMelderFile file { };
							Melder_relativePathToFile (p, & file);
							if (withFile == 1) {
								char32 *stringValue = MelderFile_readText (& file);
								InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
								Melder_free (var -> stringValue);
								var -> stringValue = stringValue;   /* var becomes owner */
							} else if (withFile == 2) {
								if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw (U"Commands that write to a file are not available inside pictures.");
								InterpreterVariable var = Interpreter_hasVariable (me, variableName);
								if (! var) Melder_throw (U"Variable ", variableName, U" undefined.");
								MelderFile_appendText (& file, var -> stringValue);
							} else {
								if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw (U"Commands that write to a file are not available inside pictures.");
								InterpreterVariable var = Interpreter_hasVariable (me, variableName);
								if (! var) Melder_throw (U"Variable ", variableName, U" undefined.");
								MelderFile_writeText (& file, var -> stringValue, Melder_getOutputEncoding ());
							}
						} else if (isCommand (p)) {
							/*
							 * Example: name$ = Get name
							 */
							MelderString_empty (& valueString);   // empty because command may print nothing; also makes sure that valueString.string exists
							autoMelderDivertInfo divert (& valueString);
							int status = praat_executeCommand (me, p);
							InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
							Melder_free (var -> stringValue);
							var -> stringValue = Melder_dup (status ? valueString.string : U"");
						} else {
							/*
							 * Evaluate a string expression and assign the result to the variable.
							 * Examples:
							 *    sentence$ = subject$ + verb$ + object$
							 *    extension$ = if index (file$, ".") <> 0
							 *       ... then right$ (file$, length (file$) - rindex (file$, "."))
							 *       ... else "" fi
							 */
							char32 *stringValue;
							trace (U"evaluating string expression");
							Interpreter_stringExpression (me, p, & stringValue);
							trace (U"assigning to string variable ", variableName);
							InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
							Melder_free (var -> stringValue);
							var -> stringValue = stringValue;   // var becomes owner
						}
					} else if (*p == U'#') {
						if (p [1] == U'#') {
							/*
								Assign to a numeric matrix variable or to a matrix element.
							*/
							static MelderString matrixName { };
							p ++;   // go to second '#'
							*p = U'\0';   // erase the last number sign temporarily
							MelderString_copy (& matrixName, command2.string, U'#');
							*p = U'#';   // put the number sign back
							p ++;   // step over last number sign
							while (Melder_isblank (*p)) p ++;   // go to first token after matrix name
							if (*p == U'=') {
								/*
									This must be an assignment to a matrix variable.
								*/
								p ++;   // step over equals sign
								while (Melder_isblank (*p)) p ++;   // go to first token after assignment
								if (*p == U'\0')
									Melder_throw (U"Missing right-hand expression in assignment to matrix ", matrixName.string, U".");
								nummat value;
								Interpreter_numericMatrixExpression (me, p, & value);
								InterpreterVariable var = Interpreter_lookUpVariable (me, matrixName.string);
								NUMmatrix_free (var -> numericMatrixValue.at, 1, 1);
								var -> numericMatrixValue = value;
							} else if (*p == U'[') {
								/*
								 * This must be an assignment to an element of the matrix variable.
								 */
								long rowNumber = 0, columnNumber = 0;
								p ++;   // step over opening bracket
								/*
									Get the row number.
								*/
								static MelderString rowFormula { };
								MelderString_empty (& rowFormula);
								int depth = 0;
								bool inString = false;
								while ((depth > 0 || *p != U',' || inString) && *p != U'\n' && *p != U'\0') {
									MelderString_appendCharacter (& rowFormula, *p);
									if (*p == U'[' || *p == U'(') {
										if (! inString) depth ++;
									} else if (*p == U']' || *p == U')') {
										if (! inString) depth --;
									}
									if (*p == U'"') inString = ! inString;
									p ++;
								}
								if (*p == U'\n' || *p == U'\0')
									Melder_throw (U"Missing comma in matrix indexing.");
								Formula_Result result;
								Interpreter_anyExpression (me, rowFormula.string, & result);
								if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									rowNumber = lround (result.result.numericResult);
								} else {
									Melder_throw (U"Row number should be numeric.");
								}

								p ++;   // step over comma
								/*
									Get the column number.
								*/
								static MelderString columnFormula { };
								MelderString_empty (& columnFormula);
								depth = 0;
								inString = false;
								while ((depth > 0 || *p != U']' || inString) && *p != U'\n' && *p != U'\0') {
									MelderString_appendCharacter (& columnFormula, *p);
									if (*p == U'[') {
										if (! inString) depth ++;
									} else if (*p == U']') {
										if (! inString) depth --;
									}
									if (*p == U'"') inString = ! inString;
									p ++;
								}
								if (*p == U'\n' || *p == U'\0')
									Melder_throw (U"Missing closing bracket (]) in matrix indexing.");
								Interpreter_anyExpression (me, columnFormula.string, & result);
								if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									columnNumber = lround (result.result.numericResult);
								} else {
									Melder_throw (U"Column number should be numeric.");
								}
								p ++;   // step over closing bracket
								while (Melder_isblank (*p)) p ++;
								if (*p != U'=')
									Melder_throw (U"Missing '=' after matrix element ", matrixName.string, U" [",
										rowFormula.string, U",", columnFormula.string, U"].");
								p ++;   // step over equals sign
								while (Melder_isblank (*p)) p ++;   // go to first token after assignment
								if (*p == U'\0') {
									Melder_throw (U"Missing expression after matrix element ", matrixName.string, U" [",
										rowFormula.string, U",", columnFormula.string, U"].");
								}
								double value;
								Interpreter_numericExpression (me, p, & value);
								InterpreterVariable var = Interpreter_hasVariable (me, matrixName.string);
								if (! var)
									Melder_throw (U"Matrix ", matrixName.string, U" does not exist.");
								if (rowNumber < 1)
									Melder_throw (U"A row number cannot be less than 1 (the row number you supplied is ", rowNumber, U").");
								if (rowNumber > var -> numericMatrixValue. nrow)
									Melder_throw (U"A row number cannot be greater than the number of rows (here ",
										var -> numericMatrixValue. nrow, U"). The row number you supplied is ", rowNumber, U".");
								if (columnNumber < 1)
									Melder_throw (U"A column number cannot be less than 1 (the column number you supplied is ", columnNumber, U").");
								if (columnNumber > var -> numericMatrixValue. ncol)
									Melder_throw (U"A column number cannot be greater than the number of columns (here ",
										var -> numericMatrixValue. ncol, U"). The column number you supplied is ", columnNumber, U".");
								var -> numericMatrixValue.at [rowNumber] [columnNumber] = value;
							} else Melder_throw (U"Missing '=' after matrix variable ", matrixName.string, U".");
						} else {
							/*
								Assign to a numeric vector variable or to a vector element.
							*/
							static MelderString vectorName { };
							*p = U'\0';   // erase the number sign temporarily
							MelderString_copy (& vectorName, command2.string, U"#");
							*p = U'#';   // put the number sign back
							p ++;   // step over number sign
							while (Melder_isblank (*p)) p ++;   // go to first token after array name
							if (*p == U'=') {
								/*
									This must be an assignment to a vector variable.
								*/
								p ++;   // step over equals sign
								while (Melder_isblank (*p)) p ++;   // go to first token after assignment
								if (*p == U'\0')
									Melder_throw (U"Missing right-hand expression in assignment to vector ", vectorName.string, U".");
								numvec value;
								Interpreter_numericVectorExpression (me, p, & value);
								InterpreterVariable var = Interpreter_lookUpVariable (me, vectorName.string);
								NUMvector_free (var -> numericVectorValue.at, 1);
								var -> numericVectorValue = value;
							} else if (*p == U'[') {
								/*
								 * This must be an assignment to an element of the vector variable.
								 */
								long indexValue = 0;
								p ++;   // step over opening bracket
								static MelderString index { };
								MelderString_empty (& index);
								int depth = 0;
								bool inString = false;
								while ((depth > 0 || *p != U']' || inString) && *p != U'\n' && *p != U'\0') {
									MelderString_appendCharacter (& index, *p);
									if (*p == U'[') {
										if (! inString) depth ++;
									} else if (*p == U']') {
										if (! inString) depth --;
									}
									if (*p == U'"') inString = ! inString;
									p ++;
								}
								if (*p == U'\n' || *p == U'\0')
									Melder_throw (U"Missing closing bracket (]) in array element.");
								Formula_Result result;
								Interpreter_anyExpression (me, index.string, & result);
								if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									indexValue = lround (result.result.numericResult);
								} else {
									Melder_throw (U"Element index should be numeric.");
								}
								p ++;   // step over closing bracket
								while (Melder_isblank (*p)) p ++;
								if (*p != U'=')
									Melder_throw (U"Missing '=' after vector element ", vectorName.string, U" [", index.string, U"].");
								p ++;   // step over equals sign
								while (Melder_isblank (*p)) p ++;   // go to first token after assignment
								if (*p == U'\0') {
									Melder_throw (U"Missing expression after vector element ", vectorName.string, U" [", index.string, U"].");
								}
								double value;
								Interpreter_numericExpression (me, p, & value);
								InterpreterVariable var = Interpreter_hasVariable (me, vectorName.string);
								if (! var)
									Melder_throw (U"Vector ", vectorName.string, U" does not exist.");
								if (indexValue < 1)
									Melder_throw (U"A vector index cannot be less than 1 (the index you supplied is ", indexValue, U").");
								if (indexValue > var -> numericVectorValue.size)
									Melder_throw (U"A vector index cannot be greater than the number of elements (here ",
										var -> numericVectorValue.size, U"). The index you supplied is ", indexValue, U".");
								var -> numericVectorValue.at [indexValue] = value;
							} else Melder_throw (U"Missing '=' after vector variable ", vectorName.string, U".");
						}
					} else {
						/*
						 * Try to assign to a numeric variable.
						 */
						double value;
						char32 *variableName = command2.string;
						int typeOfAssignment = 0;   // plain assignment
						if (*p == U'\0') {
							/*
							 * Command ends here: it may be a PraatShell command.
							 */
							praat_executeCommand (me, command2.string);
							continue;   // next line
						}
						char32 *endOfVariable = p;
						while (Melder_isblank (*p)) p ++;
						if (*p == U'=' || ((*p == U'+' || *p == U'-' || *p == U'*' || *p == U'/') && p [1] == U'=')) {
							/*
							 * This must be an assignment (though: "echo = ..." ???)
							 */
							typeOfAssignment = *p == U'+' ? 1 : *p == U'-' ? 2 : *p == U'*' ? 3 : *p == U'/' ? 4 : 0;
							*endOfVariable = U'\0';   // close variable name; FIXME: this can be any weird character, e.g. hallo&
						} else if (*p == U'[') {
							/*
							 * This must be an assignment to an indexed numeric variable.
							 */
							*endOfVariable = U'\0';
							static MelderString indexedVariableName { };
							MelderString_copy (& indexedVariableName, command2.string, U"[");
							for (;;) {
								p ++;   // skip opening bracket or comma
								static MelderString index { };
								MelderString_empty (& index);
								int depth = 0;
								bool inString = false;
								while ((depth > 0 || (*p != U',' && *p != U']') || inString) && *p != U'\n' && *p != U'\0') {
									MelderString_appendCharacter (& index, *p);
									if (*p == U'[') {
										if (! inString) depth ++;
									} else if (*p == U']') {
										if (! inString) depth --;
									}
									if (*p == U'"') inString = ! inString;
									p ++;
								}
								if (*p == U'\n' || *p == U'\0')
									Melder_throw (U"Missing closing bracket (]) in indexed variable.");
								Formula_Result result;
								Interpreter_anyExpression (me, index.string, & result);
								if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									double numericIndexValue = result.result.numericResult;
									MelderString_append (& indexedVariableName, numericIndexValue);
								} else if (result.expressionType == kFormula_EXPRESSION_TYPE_STRING) {
									MelderString_append (& indexedVariableName, U"\"", result.result.stringResult, U"\"");
									Melder_free (result.result.stringResult);
								}
								MelderString_appendCharacter (& indexedVariableName, *p);
								if (*p == U']') {
									break;
								}
							}
							variableName = indexedVariableName.string;
							p ++;   // skip closing bracket
							while (Melder_isblank (*p)) p ++;
							if (*p == U'=' || ((*p == U'+' || *p == U'-' || *p == U'*' || *p == U'/') && p [1] == U'=')) {
								typeOfAssignment = *p == U'+' ? 1 : *p == U'-' ? 2 : *p == U'*' ? 3 : *p == U'/' ? 4 : 0;
							}
						} else {
							/*
							 * Not an assignment: perhaps a PraatShell command (select, echo, execute, pause ...).
							 */
							praat_executeCommand (me, variableName);
							continue;   // next line
						}
						p += typeOfAssignment == 0 ? 1 : 2;
						while (*p == U' ' || *p == U'\t') p ++;
						if (*p == U'\0') Melder_throw (U"Missing expression after variable ", variableName, U".");
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
							MelderString_appendCharacter (& valueString, 1);   // will be overwritten by something totally different if any MelderInfo function is called...
							int status = praat_executeCommand (me, p);
							if (status == 0) {
								value = undefined;
							} else if (valueString.string [0] == 1) {   // ...not overwritten by any MelderInfo function? then the return value will be the selected object
								int IOBJECT, result = 0, found = 0;
								WHERE (SELECTED) { result = IOBJECT; found += 1; }
								if (found > 1) {
									Melder_throw (U"Multiple objects selected. Cannot assign ID to variable.");
								} else if (found == 0) {
									Melder_throw (U"No objects selected. Cannot assign ID to variable.");
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
							//Melder_casual (U"looking up variable ", variableName);
							InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
							var -> numericValue = value;
						} else {
							/*
							 * Modify an existing variable.
							 */
							InterpreterVariable var = Interpreter_hasVariable (me, variableName);
							if (! var) Melder_throw (U"Unknown variable ", variableName, U".");
							if (isundef (var -> numericValue)) {
								/* Keep it that way. */
							} else {
								if (typeOfAssignment == 1) {
									var -> numericValue += value;
								} else if (typeOfAssignment == 2) {
									var -> numericValue -= value;
								} else if (typeOfAssignment == 3) {
									var -> numericValue *= value;
								} else if (value == 0) {
									var -> numericValue = undefined;
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
					Melder_throw (U"Script assertion fails in line ", save_assertErrorLineNumber,
							U": error « ", assertErrorString.string, U" » not raised. Instead: no error.");
					
				}
			} catch (MelderError) {
				//	Melder_casual (U"Error: << ", Melder_getError(),
				//		U" >>\nassertErrorLineNumber: ", assertErrorLineNumber,
				//		U"\nlineNumber: ", lineNumber,
				//		U"\nAssert error string: << ", assertErrorString.string,
				//		U" >>\n"
				//	);
				if (assertErrorLineNumber == 0) {
					throw;
				} else if (assertErrorLineNumber != lineNumber) {
					if (str32str (Melder_getError (), assertErrorString.string)) {
						Melder_clearError ();
						assertErrorLineNumber = 0;
					} else {
						char32 *errorCopy_nothrow = Melder_dup_f (Melder_getError ());   // UGLY but necessary (1)
						Melder_clearError ();
						autostring32 errorCopy = errorCopy_nothrow;   // UGLY but necessary (2)
						Melder_throw (U"Script assertion fails in line ", assertErrorLineNumber,
							U": error « ", assertErrorString.string, U" » not raised. Instead:\n",
							errorCopy.peek());
					}
				}
			}
		} // endfor lineNumber
		my numberOfLabels = 0;
		my running = false;
		my stopped = false;
	} catch (MelderError) {
		if (lineNumber > 0) {
			bool normalExplicitExit = str32nequ (lines [lineNumber], U"exit ", 5) || Melder_hasError (U"Script exited.");
			if (! normalExplicitExit && ! assertionFailed) {   // don't show the message twice!
				while (lines [lineNumber] [0] == U'\0') {   // did this use to be a continuation line?
					lineNumber --;
					Melder_assert (lineNumber > 0);   // originally empty lines that stayed empty should not generate errors
				}
				Melder_appendError (U"Script line ", lineNumber, U" not performed or completed:\n« ", lines [lineNumber], U" »");
			}
		}
		my numberOfLabels = 0;
		my running = false;
		my stopped = false;
		if (str32equ (Melder_getError (), U"\nScript exited.\n")) {
			Melder_clearError ();
		} else {
			throw;
		}
	}
}

void Interpreter_stop (Interpreter me) {
//Melder_casual (U"Interpreter_stop in: ", Melder_pointer (me));
	my stopped = true;
//Melder_casual (U"Interpreter_stop out: ", Melder_pointer (me));
}

void Interpreter_voidExpression (Interpreter me, const char32 *expression) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_NUMERIC, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
}

void Interpreter_numericExpression (Interpreter me, const char32 *expression, double *value) {
	Melder_assert (value);
	if (str32str (expression, U"(=")) {
		*value = Melder_atof (expression);
	} else {
		Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_NUMERIC, false);
		Formula_Result result;
		Formula_run (0, 0, & result);
		*value = result. result.numericResult;
	}
}

void Interpreter_numericVectorExpression (Interpreter me, const char32 *expression, numvec *value) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
	*value = result. result.numericVectorResult;
}

void Interpreter_numericMatrixExpression (Interpreter me, const char32 *expression, nummat *value) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
	*value = result. result.numericMatrixResult;
}

void Interpreter_stringExpression (Interpreter me, const char32 *expression, char32 **value) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_STRING, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
	*value = result. result.stringResult;
}

void Interpreter_anyExpression (Interpreter me, const char32 *expression, Formula_Result *result) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_UNKNOWN, false);
	Formula_run (0, 0, result);
}

/* End of file Interpreter.cpp */
