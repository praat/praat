/* Interpreter.cpp
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

#include "Interpreter.h"
#include "praatP.h"
extern structMelderDir praatDir;
#include "praat_script.h"
#include "Formula.h"
#include "praat_version.h"
#include "../kar/UnicodeData.h"

#include "../fon/Vector.h"

#define Interpreter_WORD 1
#define Interpreter_REAL 2
#define Interpreter_POSITIVE 3
#define Interpreter_INTEGER 4
#define Interpreter_NATURAL 5
#define Interpreter_BOOLEAN 6
#define Interpreter_SENTENCE 7
#define Interpreter_TEXT 8
#define Interpreter_VECTOR 9
#define Interpreter_MATRIX 10
#define Interpreter_CHOICE 11
#define Interpreter_OPTIONMENU 12
#define Interpreter_BUTTON 13
#define Interpreter_OPTION 14
#define Interpreter_COMMENT 15

autoVEC theInterpreterNumvec;
autoMAT theInterpreterNummat;
autoSTRVEC theInterpreterStrvec;

Thing_implement (InterpreterVariable, SimpleString, 0);

static autoInterpreterVariable InterpreterVariable_create (conststring32 key) {
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

autoInterpreter Interpreter_create (conststring32 environmentName, ClassInfo editorClass) {
	try {
		autoInterpreter me = Thing_new (Interpreter);
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
	return Interpreter_create (editor -> name.get(), editor -> classInfo);
}

void Melder_includeIncludeFiles (autostring32 *inout_text) {
	for (int depth = 0; ; depth ++) {
		char32 *head = inout_text->get();
		integer numberOfIncludes = 0;
		if (depth > 10)
			Melder_throw (U"Include files nested too deep. Probably cyclic.");
		for (;;) {
			char32 *includeLocation, *includeFileName, *tail;
			integer headLength, includeTextLength, newLength;
			/*
				Look for an include statement. If not found, we have finished.
			 */
			includeLocation = ( str32nequ (head, U"include ", 8) ? head : str32str (head, U"\ninclude ") );
			if (! includeLocation)
				break;
			if (includeLocation != head)
				includeLocation += 1;
			numberOfIncludes += 1;
			/*
				Separate out the head.
			 */
			*includeLocation = U'\0';
			/*
				Separate out the name of the include file.
			 */
			includeFileName = includeLocation + 8;
			while (Melder_isHorizontalSpace (*includeFileName)) includeFileName ++;
			tail = includeFileName;
			while (Melder_staysWithinLine (*tail)) tail ++;
			if (*tail != U'\0') {
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
				includeText = MelderFile_readText (& includeFile);
			} catch (MelderError) {
				Melder_throw (U"Include file ", & includeFile, U" not read.");
			}
			/*
				Construct the new text.
			 */
			headLength = (head - inout_text->get()) + str32len (head);
			includeTextLength = str32len (includeText.get());
			newLength = headLength + includeTextLength + 1 + str32len (tail);
			autostring32 newText (newLength);
			str32cpy (newText.get(), inout_text->get());
			str32cpy (newText.get() + headLength, includeText.get());
			str32cpy (newText.get() + headLength + includeTextLength, U"\n");
			str32cpy (newText.get() + headLength + includeTextLength + 1, tail);
			/*
				Replace the old text with the new. This will work even within an autostring.
			 */
			*inout_text = newText.move();
			/*
				Cycle.
			 */
			head = inout_text->get() + headLength + includeTextLength + 1;
		}
		if (numberOfIncludes == 0) break;
	}
}

integer Interpreter_readParameters (Interpreter me, mutablestring32 text) {
	char32 *formLocation = nullptr;
	integer npar = 0;
	my dialogTitle [0] = U'\0';
	/*
		Look for a "form" line.
	*/
	{// scope
		char32 *p = & text [0];
		for (;;) {
			/*
				Invariant here: we are at the beginning of a line.
			*/
			Melder_skipHorizontalSpace (& p);
			if (str32nequ (p, U"form", 4) && Melder_isEndOfInk (p [4])) {
				formLocation = p;
				break;
			}
			Melder_skipToEndOfLine (& p);
			if (*p == U'\0')
				break;
			p ++;   // skip newline symbol
		}
	}
	/*
		If there is no "form" line, there are no parameters.
	*/
	if (formLocation) {
		char32 *dialogTitle = Melder_findEndOfHorizontalSpace (formLocation + 4);
		char32 *endOfLine = Melder_findEndOfLine (dialogTitle);
		if (*endOfLine == U'\0')
			Melder_throw (U"Unfinished form (only a \"form\" line).");
		*endOfLine = U'\0';   // destroy input temporarily in order to limit copying of dialog title
		str32ncpy (my dialogTitle, dialogTitle, Interpreter_MAX_DIALOG_TITLE_LENGTH);
		*endOfLine = U'\n';   // restore input
		my numberOfParameters = 0;
		while (true) {
			int type = 0;
			char32 *startOfLine = Melder_findEndOfHorizontalSpace (endOfLine + 1);
			/*
				Skip empty lines and lines with comments.
			*/
			while (*startOfLine == U'#' || *startOfLine == U';' || *startOfLine == U'!' || Melder_isEndOfLine (*startOfLine)) {
				endOfLine = Melder_findEndOfLine (startOfLine);
				if (Melder_isEndOfText (*endOfLine))
					Melder_throw (U"Unfinished form (missing \"endform\").");
				startOfLine = Melder_findEndOfHorizontalSpace (endOfLine + 1);
			}
			if (str32nequ (startOfLine, U"endform", 7) && Melder_isEndOfInk (startOfLine [7])) break;
			char32 *parameterLocation;
			if (str32nequ (startOfLine, U"word", 4) && Melder_isEndOfInk (startOfLine [4]))
				{ type = Interpreter_WORD; parameterLocation = startOfLine + 4; }
			else if (str32nequ (startOfLine, U"real", 4) && Melder_isEndOfInk (startOfLine [4]))
				{ type = Interpreter_REAL; parameterLocation = startOfLine + 4; }
			else if (str32nequ (startOfLine, U"positive", 8) && Melder_isEndOfInk (startOfLine [8]))
				{ type = Interpreter_POSITIVE; parameterLocation = startOfLine + 8; }
			else if (str32nequ (startOfLine, U"integer", 7) && Melder_isEndOfInk (startOfLine [7]))
				{ type = Interpreter_INTEGER; parameterLocation = startOfLine + 7; }
			else if (str32nequ (startOfLine, U"natural", 7) && Melder_isEndOfInk (startOfLine [7]))
				{ type = Interpreter_NATURAL; parameterLocation = startOfLine + 7; }
			else if (str32nequ (startOfLine, U"boolean", 7) && Melder_isEndOfInk (startOfLine [7]))
				{ type = Interpreter_BOOLEAN; parameterLocation = startOfLine + 7; }
			else if (str32nequ (startOfLine, U"sentence", 8) && Melder_isEndOfInk (startOfLine [8]))
				{ type = Interpreter_SENTENCE; parameterLocation = startOfLine + 8; }
			else if (str32nequ (startOfLine, U"text", 4) && Melder_isEndOfInk (startOfLine [4]))
				{ type = Interpreter_TEXT; parameterLocation = startOfLine + 4; }
			else if (str32nequ (startOfLine, U"vector", 6) && Melder_isEndOfInk (startOfLine [6]))
				{ type = Interpreter_VECTOR; parameterLocation = startOfLine + 6; }
			else if (str32nequ (startOfLine, U"matrix", 6) && Melder_isEndOfInk (startOfLine [6]))
				{ type = Interpreter_MATRIX; parameterLocation = startOfLine + 6; }
			else if (str32nequ (startOfLine, U"choice", 6) && Melder_isEndOfInk (startOfLine [6]))
				{ type = Interpreter_CHOICE; parameterLocation = startOfLine + 6; }
			else if (str32nequ (startOfLine, U"optionmenu", 10) && Melder_isEndOfInk (startOfLine [10]))
				{ type = Interpreter_OPTIONMENU; parameterLocation = startOfLine + 10; }
			else if (str32nequ (startOfLine, U"button", 6) && Melder_isEndOfInk (startOfLine [6]))
				{ type = Interpreter_BUTTON; parameterLocation = startOfLine + 6; }
			else if (str32nequ (startOfLine, U"option", 6) && Melder_isEndOfInk (startOfLine [6]))
				{ type = Interpreter_OPTION; parameterLocation = startOfLine + 6; }
			else if (str32nequ (startOfLine, U"comment", 7) && Melder_isEndOfInk (startOfLine [7]))
				{ type = Interpreter_COMMENT; parameterLocation = startOfLine + 7; }
			else {
				endOfLine = Melder_findEndOfLine (startOfLine);
				*endOfLine = U'\0';   // destroy input in order to limit printing of parameter type
				Melder_throw (U"Unknown parameter type:\n\"", startOfLine, U"\".");
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
				Melder_skipHorizontalSpace (& parameterLocation);
				if (Melder_isEndOfLine (*parameterLocation)) {
					*parameterLocation = U'\0';   // destroy input in order to limit printing of line
					Melder_throw (U"Missing parameter:\n\"", startOfLine, U"\".");
				}
				char32 *q = my parameters [++ my numberOfParameters];
				while (Melder_staysWithinInk (*parameterLocation))
					* (q ++) = * (parameterLocation ++);
				*q = U'\0';
				npar ++;
			} else {
				my parameters [++ my numberOfParameters] [0] = U'\0';
			}
			char32 *argumentLocation = Melder_findEndOfHorizontalSpace (parameterLocation);
			endOfLine = Melder_findEndOfLine (argumentLocation);
			if (Melder_isEndOfText (*endOfLine))
				Melder_throw (U"Unfinished form (missing \"endform\").");
			*endOfLine = U'\0';   // destroy input temporarily in order to limit copying of argument
			my arguments [my numberOfParameters] = Melder_dup_f (argumentLocation);
			*endOfLine = U'\n';   // restore input
			my types [my numberOfParameters] = type;
		}
	} else {
		npar = my numberOfParameters = 0;
	}
	return npar;
}

autoUiForm Interpreter_createForm (Interpreter me, GuiWindow parent, conststring32 path,
	void (*okCallback) (UiForm, integer, Stackel, conststring32, Interpreter, conststring32, bool, void *), void *okClosure,
	bool selectionOnly)
{
	autoUiForm form = UiForm_create (parent,
		Melder_cat (selectionOnly ? U"Run script (selection only): " : U"Run script: ", my dialogTitle),
		okCallback, okClosure, nullptr, nullptr);
	UiField radio = nullptr;
	if (path)
		UiForm_addText (form.get(), nullptr, nullptr, U"$file", path);
	for (int ipar = 1; ipar <= my numberOfParameters; ipar ++) {
		/*
			Convert underscores to spaces.
		*/
		char32 parameter [100], *p = & parameter [0];
		str32cpy (parameter, my parameters [ipar]);
		while (*p) {
			if (*p == U'_')
				*p = U' ';
			p ++;
		}
		switch (my types [ipar]) {
			case Interpreter_WORD:
				UiForm_addWord (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get()); break;
			case Interpreter_REAL:
				UiForm_addReal (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get()); break;   // TODO: an address of a real variable
			case Interpreter_POSITIVE:
				UiForm_addPositive (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get()); break;
			case Interpreter_INTEGER:
				UiForm_addInteger (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get()); break;
			case Interpreter_NATURAL:
				UiForm_addNatural (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get()); break;
			case Interpreter_BOOLEAN:
				UiForm_addBoolean (form.get(), nullptr, nullptr, parameter, my arguments [ipar] [0] == U'1' ||
					my arguments [ipar] [0] == U'y' || my arguments [ipar] [0] == U'Y' ||
					(my arguments [ipar] [0] == U'o' && my arguments [ipar] [1] == U'n')); break;
			case Interpreter_SENTENCE:
				UiForm_addSentence (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get()); break;
			case Interpreter_TEXT:
				UiForm_addText (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get()); break;
			case Interpreter_VECTOR:
				UiForm_addNumvec (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get()); break;
			case Interpreter_MATRIX:
				UiForm_addNummat (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get()); break;
			case Interpreter_CHOICE:
				radio = UiForm_addRadio (form.get(), nullptr, nullptr, nullptr, parameter, Melder_atoi (my arguments [ipar].get()), 1); break;
			case Interpreter_OPTIONMENU:
				radio = UiForm_addOptionMenu (form.get(), nullptr, nullptr, nullptr, parameter, Melder_atoi (my arguments [ipar].get()), 1); break;
			case Interpreter_BUTTON:
				if (radio) UiRadio_addButton (radio, my arguments [ipar].get()); break;
			case Interpreter_OPTION:
				if (radio) UiOptionMenu_addButton (radio, my arguments [ipar].get()); break;
			case Interpreter_COMMENT:
				UiForm_addLabel (form.get(), nullptr, my arguments [ipar].get()); break;
			default:
				UiForm_addWord (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get()); break;
		}
		/*
			Strip parentheses and colon off parameter name.
		*/
		if ((p = str32chr (my parameters [ipar], U'(')) != nullptr) {
			*p = U'\0';
			if (p - my parameters [ipar] > 0 && p [-1] == U'_')
				p [-1] = U'\0';
		}
		p = my parameters [ipar];
		if (*p != U'\0' && p [str32len (p) - 1] == U':')
			p [str32len (p) - 1] = U'\0';
	}
	UiForm_finish (form.get());
	return form;
}

void Interpreter_getArgumentsFromDialog (Interpreter me, UiForm dialog) {
	for (int ipar = 1; ipar <= my numberOfParameters; ipar ++) {
		char32 parameter [100], *p;
		/*
			Strip parentheses and colon off parameter name.
		*/
		if ((p = str32chr (my parameters [ipar], U'(')) != nullptr) {
			*p = U'\0';
			if (p - my parameters [ipar] > 0 && p [-1] == U'_')
				p [-1] = U'\0';
		}
		p = my parameters [ipar];
		if (*p != U'\0' && p [str32len (p) - 1] == U':')
			p [str32len (p) - 1] = U'\0';
		/*
			Convert underscores to spaces.
		*/
		str32cpy (parameter, my parameters [ipar]);
		p = & parameter [0];
		while (*p) {
			if (*p == U'_')
				*p = U' ';
			p ++;
		}
		switch (my types [ipar]) {
			case Interpreter_REAL:
			case Interpreter_POSITIVE: {
				double value = UiForm_getReal_check (dialog, parameter);
				my arguments [ipar] = autostring32 (40, true);
				Melder_sprint (my arguments [ipar].get(),40+1, value);
				break;
			}
			case Interpreter_INTEGER:
			case Interpreter_NATURAL:
			case Interpreter_BOOLEAN: {
				integer value = UiForm_getInteger (dialog, parameter);
				my arguments [ipar] = autostring32 (40, true);
				Melder_sprint (my arguments [ipar].get(),40+1, value);
				break;
			}
			case Interpreter_CHOICE:
			case Interpreter_OPTIONMENU: {
				integer integerValue = 0;
				integerValue = UiForm_getInteger (dialog, parameter);
				conststring32 stringValue = UiForm_getString (dialog, parameter);
				my arguments [ipar] = autostring32 (40, true);
				Melder_sprint (my arguments [ipar].get(),40+1, integerValue);
				Melder_sprint (my choiceArguments [ipar],100, stringValue);
				break;
			}
			case Interpreter_BUTTON:
			case Interpreter_OPTION:
			case Interpreter_COMMENT:
				break;
			default: {
				conststring32 value = UiForm_getString (dialog, parameter);
				my arguments [ipar] = Melder_dup_f (value);
				break;
			}
		}
	}
}

void Interpreter_getArgumentsFromString (Interpreter me, conststring32 arguments) {
	int size = my numberOfParameters;
	integer length = str32len (arguments);
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
		/*
			Erase the current values, probably the default values,
			and replace with the actual arguments.
		*/
		my arguments [ipar] = autostring32 (length);
		/*
			Skip spaces until next argument.
		 */
		while (Melder_isHorizontalSpace (*arguments)) arguments ++;
		/*
			The argument is everything up to the next space, or, if it starts with a double quote,
			everything between this quote and the matching double quote;
			in this case, the argument can represent a double quote by a sequence of two double quotes.
			Example: the string
				"I said ""hello"""
			will be passed to the dialog as a single argument containing the text
				I said "hello"
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
			while (Melder_staysWithinInk (*arguments))
				my arguments [ipar] [ichar ++] = *arguments ++;
		}
		my arguments [ipar] [ichar] = U'\0';   // trailing null byte
	}
	/*
		The last item is handled separately, because it consists of the rest of the line.
		Leading spaces are skipped, but trailing spaces are included.
	*/
	if (size > 0) {
		while (Melder_isHorizontalSpace (*arguments)) arguments ++;
		my arguments [size] = Melder_dup_f (arguments);
	}
	/*
		Convert booleans and choices to numbers.
	*/
	for (int ipar = 1; ipar <= size; ipar ++) {
		if (my types [ipar] == Interpreter_BOOLEAN) {
			mutablestring32 arg = & my arguments [ipar] [0];
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
			mutablestring32 arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_BUTTON && my types [jpar] != Interpreter_OPTION)
					Melder_throw (U"Unknown value \"", arg, U"\" for choice \"", my parameters [ipar], U"\".");
				if (str32equ (my arguments [jpar].get(), arg)) {   // the button labels are in the arguments; see Interpreter_readParameters
					str32cpy (arg, Melder_integer (jpar - ipar));
					str32cpy (my choiceArguments [ipar], my arguments [jpar].get());
					break;
				}
			}
			if (jpar > my numberOfParameters)
				Melder_throw (U"Unknown value \"", arg, U"\" for choice \"", my parameters [ipar], U"\".");
		} else if (my types [ipar] == Interpreter_OPTIONMENU) {
			int jpar;
			mutablestring32 arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_OPTION && my types [jpar] != Interpreter_BUTTON)
					Melder_throw (U"Unknown value \"", arg, U"\" for option menu \"", my parameters [ipar], U"\".");
				if (str32equ (my arguments [jpar].get(), arg)) {
					str32cpy (arg, Melder_integer (jpar - ipar));
					str32cpy (my choiceArguments [ipar], my arguments [jpar].get());
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
	while (size >= 1 && my parameters [size] [0] == U'\0')
		size --;   // ignore trailing fields without a variable name (button, comment)
	for (int ipar = 1; ipar <= size; ipar ++) {
		mutablestring32 p = my parameters [ipar];
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
		/*
			Erase the current values, probably the default values...
		*/
		my arguments [ipar]. reset();   //
		if (iarg == narg)
			Melder_throw (U"Found ", narg, U" arguments but expected more.");
		Stackel arg = & args [++ iarg];
		/*
			... and replace with the actual arguments.
		*/
		my arguments [ipar] =
			arg -> which == Stackel_NUMBER ? Melder_dup (Melder_double (arg -> number)) :
			arg -> which == Stackel_STRING ? Melder_dup (arg -> getString()) : autostring32();
		Melder_assert (my arguments [ipar]);
	}
	if (iarg < narg)
		Melder_throw (U"Found ", narg, U" arguments but expected only ", iarg, U".");
	/*
	 * Convert booleans and choices to numbers.
	 */
	for (int ipar = 1; ipar <= size; ipar ++) {
		if (my types [ipar] == Interpreter_BOOLEAN) {
			mutablestring32 arg = & my arguments [ipar] [0];
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
			mutablestring32 arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_BUTTON && my types [jpar] != Interpreter_OPTION)
					Melder_throw (U"Unknown value \"", arg, U"\" for choice \"", my parameters [ipar], U"\".");
				if (str32equ (my arguments [jpar].get(), arg)) {   // the button labels are in the arguments; see Interpreter_readParameters
					str32cpy (arg, Melder_integer (jpar - ipar));
					str32cpy (my choiceArguments [ipar], my arguments [jpar].get());
					break;
				}
			}
			if (jpar > my numberOfParameters)
				Melder_throw (U"Unknown value \"", arg, U"\" for choice \"", my parameters [ipar], U"\".");
		} else if (my types [ipar] == Interpreter_OPTIONMENU) {
			int jpar;
			mutablestring32 arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_OPTION && my types [jpar] != Interpreter_BUTTON)
					Melder_throw (U"Unknown value \"", arg, U"\" for option menu \"", my parameters [ipar], U"\".");
				if (str32equ (my arguments [jpar].get(), arg)) {
					str32cpy (arg, Melder_integer (jpar - ipar));
					str32cpy (my choiceArguments [ipar], my arguments [jpar].get());
					break;
				}
			}
			if (jpar > my numberOfParameters)
				Melder_throw (U"Unknown value \"", arg, U"\" for option menu \"", my parameters [ipar], U"\".");
		}
	}
}

static void Interpreter_addNumericVariable (Interpreter me, conststring32 key, double value) {
	autoInterpreterVariable variable = InterpreterVariable_create (key);
	variable -> numericValue = value;
	my variablesMap [key] = variable.move();
	variable.releaseToAmbiguousOwner();
}

static void Interpreter_addStringVariable (Interpreter me, conststring32 key, conststring32 value) {
	autoInterpreterVariable variable = InterpreterVariable_create (key);
	variable -> stringValue = Melder_dup (value);
	my variablesMap [key] = variable.move();
	variable.releaseToAmbiguousOwner();
}

InterpreterVariable Interpreter_hasVariable (Interpreter me, conststring32 key) {
	Melder_assert (key);
	auto it = my variablesMap. find (key [0] == U'.' ? Melder_cat (my procedureNames [my callDepth], key) : key);
	if (it != my variablesMap. end()) {
		return it -> second.get();
	} else {
		return nullptr;
	}
}

InterpreterVariable Interpreter_lookUpVariable (Interpreter me, conststring32 key) {
	Melder_assert (key);
	conststring32 variableNameIncludingProcedureName =
		key [0] == U'.' ? Melder_cat (my procedureNames [my callDepth], key) : key;
	auto it = my variablesMap. find (variableNameIncludingProcedureName);
	if (it != my variablesMap. end()) {
		return it -> second.get();
	}
	/*
	 * The variable doesn't yet exist: create a new one.
	 */
	autoInterpreterVariable variable = InterpreterVariable_create (variableNameIncludingProcedureName);
	InterpreterVariable variable_ref = variable.get();
	my variablesMap [variableNameIncludingProcedureName] = variable.move();
	return variable_ref;
}

static integer lookupLabel (Interpreter me, conststring32 labelName) {
	for (integer ilabel = 1; ilabel <= my numberOfLabels; ilabel ++)
		if (str32equ (labelName, my labelNames [ilabel]))
			return ilabel;
	Melder_throw (U"Unknown label \"", labelName, U"\".");
}

static bool isCommand (conststring32 string) {
	const char32 *p = & string [0];
	/*
	 * Things that start with "nowarn", "noprogress", or "nocheck" are commands.
	 */
	if (p [0] == U'n' && p [1] == U'o' &&
		(str32nequ (p + 2, U"warn ", 5) || str32nequ (p + 2, U"progress ", 9) || str32nequ (p + 2, U"check ", 6))) return true;
	if (str32nequ (p, U"demo ", 5)) return true;
	/*
	 * Otherwise, things that start with nonupper case are formulas.
	 */
	if (! Melder_isUpperCaseLetter (*p)) return false;
	/*
	 * The remaining possibility is things that start with upper case.
	 * If they contain an underscore, they are object names, hence we must have a formula.
	 * Otherwise, we have a command.
	 */
	while (Melder_isAlphanumeric (*p)) p ++;
	return *p != '_';
}

static void parameterToVariable (Interpreter me, int type, conststring32 in_parameter, int ipar) {
	char32 parameter [200];
	Melder_assert (type != 0);
	str32cpy (parameter, in_parameter);
	if (type >= Interpreter_REAL && type <= Interpreter_BOOLEAN) {
		Interpreter_addNumericVariable (me, parameter, Melder_atof (my arguments [ipar].get()));
	} else if (type == Interpreter_CHOICE || type == Interpreter_OPTIONMENU) {
		Interpreter_addNumericVariable (me, parameter, Melder_atof (my arguments [ipar].get()));
		str32cat (parameter, U"$");
		Interpreter_addStringVariable (me, parameter, my choiceArguments [ipar]);
	} else if (type == Interpreter_BUTTON || type == Interpreter_OPTION || type == Interpreter_COMMENT) {
		/* Do not add a variable. */
	} else {
		str32cat (parameter, U"$");
		Interpreter_addStringVariable (me, parameter, my arguments [ipar].get());
	}
}

inline static void NumericVectorVariable_move (InterpreterVariable variable, VEC movedVector, bool rightHandSideOwned) {
	if (rightHandSideOwned) {
		/*
			Statement like: a# = b# + c#
		*/
		variable -> numericVectorValue. adoptFromAmbiguousOwner (movedVector);
	} else if (variable -> numericVectorValue.size == movedVector.size) {
		if (variable -> numericVectorValue.cells == movedVector.cells) {
			/*
				Statement like: a# = a#
			*/
			(void) 0;   // assigning a variable to itself: do nothing
		} else {
			/*
				Statement like: a# = b#   // with matching sizes
			*/
			variable -> numericVectorValue.all()  <<=  movedVector;
		}
	} else {
		/*
			Statement like: a# = b#   // with non-matching sizes
		*/
		variable -> numericVectorValue = copy_VEC (movedVector);
	}
}

inline static void StringArrayVariable_move (InterpreterVariable variable, STRVEC movedVector, bool rightHandSideOwned) {
	if (rightHandSideOwned) {
		/*
			Statement like: a$# = b$# + c$#
		*/
		variable -> stringArrayValue. adoptFromAmbiguousOwner (movedVector);
	} else if (variable -> stringArrayValue.size == movedVector.size) {
		if ((char32 **) variable -> stringArrayValue.elements == movedVector.elements) {
			/*
				Statement like: a$# = a$#
			*/
			(void) 0;   // assigning a variable to itself: do nothing
		} else {
			/*
				Statement like: a$# = b$#   // with matching sizes
			*/
			variable -> stringArrayValue.all()  <<=  movedVector;
		}
	} else {
		/*
			Statement like: a$# = b$#   // with non-matching sizes
		*/
		variable -> stringArrayValue = newSTRVECcopy (movedVector);
	}
}

inline static void NumericMatrixVariable_move (InterpreterVariable variable, MAT movedMatrix, bool rightHandSideOwned) {
	if (rightHandSideOwned) {
		/*
			Statement like: a## = b## + c##
		*/
		variable -> numericMatrixValue. adoptFromAmbiguousOwner (movedMatrix);
	} else if (variable -> numericMatrixValue.nrow == movedMatrix.nrow && variable -> numericMatrixValue.ncol == movedMatrix.ncol) {
		if (variable -> numericMatrixValue.cells == movedMatrix.cells) {
			/*
				Statement like: a## = a##
			*/
			(void) 0;   // assigning a variable to itself: do nothing
		} else {
			/*
				Statement like: a## = b##   // with matching sizes
			*/
			variable -> numericMatrixValue.all()  <<=  movedMatrix;
		}
	} else {
		/*
			Statement like: a## = b##   // with non-matching sizes
		*/
		variable -> numericMatrixValue = copy_MAT (movedMatrix);
	}
}

inline static void NumericVectorVariable_add (InterpreterVariable variable, double scalar) {
	variable -> numericVectorValue.all()  +=  scalar;
}
inline static void NumericVectorVariable_add (InterpreterVariable variable, constVEC vector) {
	const VEC& variableVector = variable -> numericVectorValue.get();
	Melder_require (vector.size == variableVector.size,
		U"You cannot add a vector with size ", vector.size,
		U" to a vector with a different size (", variableVector.size, U")."
	);
	variableVector  +=  vector;
}
inline static void NumericVectorVariable_subtract (InterpreterVariable variable, double scalar) {
	variable -> numericVectorValue.all()  -=  scalar;
}
inline static void NumericVectorVariable_subtract (InterpreterVariable variable, constVEC vector) {
	const VEC& variableVector = variable -> numericVectorValue.get();
	Melder_require (vector.size == variable -> numericVectorValue.size,
		U"You cannot subtract a vector with size ", vector.size,
		U" from a vector with a different size (", variableVector.size, U")."
	);
	variableVector  -=  vector;
}
inline static void NumericVectorVariable_multiply (InterpreterVariable variable, double scalar) {
	variable -> numericVectorValue.all()  *=  scalar;
}
inline static void NumericVectorVariable_multiply (InterpreterVariable variable, constVEC vector) {
	const VEC& variableVector = variable -> numericVectorValue.get();
	Melder_require (vector.size != variableVector.size,
		U"You cannot multiply a vector with size ", variableVector.size,
		U" with a vector with a different size (", vector.size, U")."
	);
	variableVector  *=  vector;
}
inline static void NumericVectorVariable_divide (InterpreterVariable variable, double scalar) {
	variable -> numericVectorValue.all()  /=  scalar;
}
inline static void NumericVectorVariable_divide (InterpreterVariable variable, constVEC vector) {
	const VEC& variableVector = variable -> numericVectorValue.get();
	Melder_require (vector.size != variableVector.size,
		U"You cannot divide a vector with size ", variableVector.size,
		U" by a vector with a different size (", vector.size, U")."
	);
	variableVector  /=  vector;
}
inline static void NumericMatrixVariable_add (InterpreterVariable variable, double scalar) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] += scalar;
}
inline static void NumericMatrixVariable_add (InterpreterVariable variable, constMAT matrix) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	if (matrix.nrow != variableMatrix.nrow || matrix.ncol != variableMatrix.ncol)
		Melder_throw (U"You cannot add a matrix with size ", matrix.nrow, U"x", matrix.ncol,
		              U" to a matrix with a different size (", variableMatrix.nrow, U"x", variableMatrix.ncol, U").");
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] += matrix [irow] [icol];
}
inline static void NumericMatrixVariable_subtract (InterpreterVariable variable, double scalar) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] -= scalar;
}
inline static void NumericMatrixVariable_subtract (InterpreterVariable variable, constMAT matrix) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	if (matrix.nrow != variableMatrix.nrow || matrix.ncol != variableMatrix.ncol)
		Melder_throw (U"You cannot subtract a matrix with size ", matrix.nrow, U"x", matrix.ncol,
		              U" from a matrix with a different size (", variableMatrix.nrow, U"x", variableMatrix.ncol, U").");
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] -= matrix [irow] [icol];
}
inline static void NumericMatrixVariable_multiply (InterpreterVariable variable, double scalar) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] *= scalar;
}
inline static void NumericMatrixVariable_multiply (InterpreterVariable variable, constMAT matrix) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	if (matrix.nrow != variableMatrix.nrow || matrix.ncol != variableMatrix.ncol)
		Melder_throw (U"You cannot multiply a matrix with size ", variableMatrix.nrow, U"x", variableMatrix.ncol,
		              U" from a matrix with a different size (", matrix.nrow, U"x", matrix.ncol, U").");
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] *= matrix [irow] [icol];
}
inline static void NumericMatrixVariable_divide (InterpreterVariable variable, double scalar) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] /= scalar;
}
inline static void NumericMatrixVariable_divide (InterpreterVariable variable, constMAT matrix) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	if (matrix.nrow != variableMatrix.nrow || matrix.ncol != variableMatrix.ncol)
		Melder_throw (U"You cannot divide a matrix with size ", variableMatrix.nrow, U"x", variableMatrix.ncol,
		              U" by a matrix with a different size (", matrix.nrow, U"x", matrix.ncol, U").");
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] /= matrix [irow] [icol];
}

static void Interpreter_do_procedureCall (Interpreter me, char32 *command,
	constvector <mutablestring32> const& lines, integer& lineNumber, integer callStack [], int& callDepth)
{
	/*
		Modern type of procedure calls, with comma separation, quoted strings, and array support.

		We just passed the `@` sign, so we continue by looking for a procedure name at the call site.
	*/
	char32 *p = command;
	while (Melder_isHorizontalSpace (*p))
		p ++;   // skip whitespace
	char32 *callName = p;
	while (Melder_staysWithinInk (*p) && *p != U'(' && *p != U':')
		p ++;
	if (p == callName) Melder_throw (U"Missing procedure name after \"@\".");
	bool hasArguments = ( *p != U'\0' );
	if (hasArguments) {
		bool parenthesisOrColonFound = ( *p == U'(' || *p == U':' );
		*p = U'\0';   // close procedure name
		if (! parenthesisOrColonFound) {
			p ++;   // step over first white space
			while (Melder_isHorizontalSpace (*p))
				p ++;   // skip more whitespace
			hasArguments = ( *p != U'\0' );
			parenthesisOrColonFound = ( *p == U'(' || *p == U':' );
			if (hasArguments && ! parenthesisOrColonFound)
				Melder_throw (U"Missing parenthesis or colon after procedure name \"", callName, U"\".");
		}
		p ++;   // step over parenthesis or colon
	}
	integer callLength = str32len (callName);
	integer iline = 1;
	for (; iline <= lines.size; iline ++) {
		if (! str32nequ (lines [iline], U"procedure ", 10))
			continue;
		char32 *q = lines [iline] + 10;
		while (Melder_isHorizontalSpace (*q))
			q ++;   // skip whitespace before procedure name
		char32 *procName = q;
		while (Melder_staysWithinInk (*q) && *q != U'(' && *q != U':')
			q ++;
		if (q == procName) Melder_throw (U"Missing procedure name after 'procedure'.");
		if (q - procName == callLength && str32nequ (procName, callName, callLength)) {
			/*
				We found the procedure definition.
			*/
			if (++ my callDepth > Interpreter_MAX_CALL_DEPTH)
				Melder_throw (U"Call depth greater than ", Interpreter_MAX_CALL_DEPTH, U".");
			str32cpy (my procedureNames [my callDepth], callName);
			bool parenthesisOrColonFound = ( *q == U'(' || *q == U':' );
			if (*q)
				q ++;   // step over parenthesis or colon or first white space
			if (! parenthesisOrColonFound) {
				while (Melder_isHorizontalSpace (*q))
					q ++;   // skip more whitespace
				if (*q == U'(' || *q == U':')
					q ++;   // step over parenthesis or colon
			}
			while (*q && *q != U')' && *q != U';') {
				static MelderString argument;
				MelderString_empty (& argument);
				while (Melder_isHorizontalSpace (*p))
					p ++;
				while (Melder_isHorizontalSpace (*q))
					q ++;
				conststring32 parameterName = q;
				while (Melder_staysWithinInk (*q) && *q != U',' && *q != U')' && *q != U';')
					q ++;   // collect parameter name
				int expressionDepth = 0;
				for (; *p; p ++) {
					if (*p == U',') {
						if (expressionDepth == 0)
							break;   // depth-0 comma ends expression
						MelderString_appendCharacter (& argument, U',');
					} else if (*p == U')' || *p == U']' || *p == U'}') {
						if (expressionDepth == 0)
							break;   // depth-0 closing parenthesis ends expression
						expressionDepth --;
						MelderString_appendCharacter (& argument, *p);
					} else if (*p == U'(' || *p == U'[' || *p == U'{') {
						expressionDepth ++;
						MelderString_appendCharacter (& argument, *p);
					} else if (*p == U'\"') {
						/*
							Enter a string literal.
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
				if (q == parameterName)
					break;
				if (*p) {
					*p = U'\0';
					p ++;
				}
				if (q [-1] == U'$') {
					my callDepth --;
					autostring32 value = Interpreter_stringExpression (me, argument.string);
					my callDepth ++;
					char32 save = *q;
					*q = U'\0';
					InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName);
					*q = save;
					var -> stringValue = value.move();
				} else if (q [-1] == U'#') {
					if (q [-2] == U'#') {
						MAT value;
						bool owned;
						my callDepth --;
						Interpreter_numericMatrixExpression (me, argument.string, & value, & owned);
						my callDepth ++;
						char32 save = *q;
						*q = U'\0';
						InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName);
						*q = save;
						NumericMatrixVariable_move (var, value, owned);
					} else if (q [-2] == U'$') {
						STRVEC value;
						bool owned;
						my callDepth --;
						Interpreter_stringArrayExpression (me, argument.string, & value, & owned);
						my callDepth ++;
						char32 save = *q;
						*q = U'\0';
						InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName);
						*q = save;
						StringArrayVariable_move (var, value, owned);
					} else {
						VEC value;
						bool owned;
						my callDepth --;
						Interpreter_numericVectorExpression (me, argument.string, & value, & owned);
						my callDepth ++;
						char32 save = *q;
						*q = U'\0';
						InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName);
						*q = save;
						NumericVectorVariable_move (var, value, owned);
					}
				} else {
					double value;
					my callDepth --;
					Interpreter_numericExpression (me, argument.string, & value);
					my callDepth ++;
					char32 save = *q;
					*q = U'\0';
					InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName);
					*q = save;
					var -> numericValue = value;
				}
				if (*q)
					q ++;   // skip comma
			}
			if (callDepth == Interpreter_MAX_CALL_DEPTH)
				Melder_throw (U"Call depth greater than ", Interpreter_MAX_CALL_DEPTH, U".");
			callStack [++ callDepth] = lineNumber;
			lineNumber = iline;
			break;
		}
	}
	if (iline > lines.size)
		Melder_throw (U"Procedure \"", callName, U"\" not found.");
}
static void Interpreter_do_oldProcedureCall (Interpreter me, char32 *command,
	constvector <mutablestring32> const& lines, integer& lineNumber, integer callStack [], int& callDepth)
{
	/*
		Old type of procedure calls, with space separation, unquoted strings, and no array support.
	*/
	char32 *p = command;
	while (Melder_isHorizontalSpace (*p))
		p ++;   // skip whitespace
	char32 *callName = p;
	while (*p != U'\0' && ! Melder_isHorizontalSpace (*p) && *p != U'(' && *p != U':')
		p ++;
	if (p == callName)
		Melder_throw (U"Missing procedure name after 'call'.");
	bool hasArguments = ( *p != U'\0' );
	*p = U'\0';   // close procedure name
	integer callLength = str32len (callName);
	integer iline = 1;
	for (; iline <= lines.size; iline ++) {
		if (! str32nequ (lines [iline], U"procedure ", 10))
			continue;
		char32 *q = lines [iline] + 10;
		while (Melder_isHorizontalSpace (*q))
			q ++;
		char32 *procName = q;
		while (*q != U'\0' && ! Melder_isHorizontalSpace (*q) && *q != U'(' && *q != U':')
			q ++;
		if (q == procName)
			Melder_throw (U"Missing procedure name after 'procedure'.");
		bool hasParameters = ( *q != U'\0' );
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
					while (Melder_isHorizontalSpace (*q))
						q ++;   // skip more whitespace
					if (*q == U'(' || *q == U':')
						q ++;   // step over parenthesis or colon
				}
				++ p;   // first argument
				while (*q && *q != U')') {
					char32 *par, save;
					static MelderString arg;
					MelderString_empty (& arg);
					while (Melder_isHorizontalSpace (*p))
						p ++;
					while (Melder_isHorizontalSpace (*q) || *q == U',' || *q == U')')
						q ++;
					par = q;
					while (*q != U'\0' && ! Melder_isHorizontalSpace (*q) && *q != U',' && *q != U')')
						q ++;   // collect parameter name
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
							while (*p != U'\0' && ! Melder_isHorizontalSpace (*p))
								MelderString_appendCharacter (& arg, *p ++);   // white space separates
						}
						if (*p) {
							*p = U'\0';
							p ++;
						}
					} else {   // else rest of line
						while (*p != '\0')
							MelderString_appendCharacter (& arg, *p ++);
					}
					if (q [-1] == '$') {
						save = *q; *q = U'\0';
						InterpreterVariable var = Interpreter_lookUpVariable (me, par); *q = save;
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
	if (iline > lines.size)
		Melder_throw (U"Procedure \"", callName, U"\" not found.");
}

static void assignToNumericVectorElement (Interpreter me, char32 *& p, const char32* vectorName, MelderString& valueString) {
	integer indexValue = 0;
	static MelderString index;
	MelderString_empty (& index);
	int depth = 0;
	bool inString = false;
	while ((depth > 0 || *p != U']' || inString) && Melder_staysWithinLine (*p)) {
		MelderString_appendCharacter (& index, *p);
		if (*p == U'[') {
			if (! inString)
				depth ++;
		} else if (*p == U']') {
			if (! inString)
				depth --;
		}
		if (*p == U'"') inString = ! inString;
		p ++;
	}
	if (! Melder_staysWithinLine (*p))
		Melder_throw (U"Missing closing bracket (]) in array element.");
	Formula_Result result;
	Interpreter_anyExpression (me, index.string, & result);
	if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
		indexValue = Melder_iround (result. numericResult);
	} else {
		Melder_throw (U"Element index should be numeric.");
	}
	p ++;   // step over closing bracket
	while (Melder_isHorizontalSpace (*p))
		p ++;
	if (*p != U'=')
		Melder_throw (U"Missing '=' after vector element ", vectorName, U" [", index.string, U"].");
	p ++;   // step over equals sign
	while (Melder_isHorizontalSpace (*p))
		p ++;   // go to first token after assignment
	if (*p == U'\0')
		Melder_throw (U"Missing expression after vector element ", vectorName, U" [", index.string, U"].");
	double value;
	if (isCommand (p)) {
		/*
			Get the value of the query.
		*/
		MelderString_empty (& valueString);
		autoMelderDivertInfo divert (& valueString);
		MelderString_appendCharacter (& valueString, 1);   // will be overwritten by something totally different if any MelderInfo function is called...
		int status = praat_executeCommand (me, p);
		if (status == 0) {
			value = undefined;
		} else if (valueString.string [0] == 1) {   // ...not overwritten by any MelderInfo function? then the return value will be the selected object
			int IOBJECT, selectedObject = 0, numberOfSelectedObjects = 0;
			WHERE (SELECTED) { selectedObject = IOBJECT; numberOfSelectedObjects += 1; }
			if (numberOfSelectedObjects > 1)
				Melder_throw (U"Multiple objects selected. Cannot assign object ID to vector element.");
			if (numberOfSelectedObjects == 0)
				Melder_throw (U"No objects selected. Cannot assign object ID to vector element.");
			value = theCurrentPraatObjects -> list [selectedObject]. id;
		} else {
			value = Melder_atof (valueString.string);   // including --undefined--
		}
	} else {
		/*
			Get the value of the formula.
		*/
		Interpreter_numericExpression (me, p, & value);
	}
	InterpreterVariable var = Interpreter_hasVariable (me, vectorName);
	if (! var)
		Melder_throw (U"Vector ", vectorName, U" does not exist.");
	if (indexValue < 1)
		Melder_throw (U"A vector index cannot be less than 1 (the index you supplied is ", indexValue, U").");
	if (indexValue > var -> numericVectorValue.size)
		Melder_throw (U"A vector index cannot be greater than the number of elements (here ",
			var -> numericVectorValue.size, U"). The index you supplied is ", indexValue, U".");
	var -> numericVectorValue [indexValue] = value;
}

static void assignToNumericMatrixElement (Interpreter me, char32 *& p, const char32* matrixName, MelderString& valueString) {
	integer rowNumber = 0, columnNumber = 0;
	/*
		Get the row number.
	*/
	static MelderString rowFormula;
	MelderString_empty (& rowFormula);
	int depth = 0;
	bool inString = false;
	while ((depth > 0 || *p != U',' || inString) && Melder_staysWithinLine (*p)) {
		MelderString_appendCharacter (& rowFormula, *p);
		if (*p == U'[' || *p == U'(') {
			if (! inString)
				depth ++;
		} else if (*p == U']' || *p == U')') {
			if (! inString)
				depth --;
		}
		if (*p == U'"') inString = ! inString;
		p ++;
	}
	if (! Melder_staysWithinLine (*p))
		Melder_throw (U"Missing comma in matrix indexing.");
	Formula_Result result;
	Interpreter_anyExpression (me, rowFormula.string, & result);
	if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
		rowNumber = Melder_iround (result. numericResult);
	} else {
		Melder_throw (U"Row number should be numeric.");
	}
	p ++;   // step over comma
	/*
		Get the column number.
	*/
	static MelderString columnFormula;
	MelderString_empty (& columnFormula);
	depth = 0;
	inString = false;
	while ((depth > 0 || *p != U']' || inString) && Melder_staysWithinLine (*p)) {
		MelderString_appendCharacter (& columnFormula, *p);
		if (*p == U'[') {
			if (! inString)
				depth ++;
		} else if (*p == U']') {
			if (! inString)
				depth --;
		}
		if (*p == U'"')
			inString = ! inString;
		p ++;
	}
	if (! Melder_staysWithinLine (*p))
		Melder_throw (U"Missing closing bracket (]) in matrix indexing.");
	Interpreter_anyExpression (me, columnFormula.string, & result);
	if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
		columnNumber = Melder_iround (result. numericResult);
	} else {
		Melder_throw (U"Column number should be numeric.");
	}
	p ++;   // step over closing bracket
	while (Melder_isHorizontalSpace (*p)) p ++;
	if (*p != U'=')
		Melder_throw (U"Missing '=' after matrix element ", matrixName, U" [",
			rowFormula.string, U",", columnFormula.string, U"].");
	p ++;   // step over equals sign
	while (Melder_isHorizontalSpace (*p)) p ++;   // go to first token after assignment
	if (*p == U'\0')
		Melder_throw (U"Missing expression after matrix element ", matrixName, U" [",
				rowFormula.string, U",", columnFormula.string, U"].");
	double value;
	if (isCommand (p)) {
		/*
			Get the value of the query.
		*/
		MelderString_empty (& valueString);
		autoMelderDivertInfo divert (& valueString);
		MelderString_appendCharacter (& valueString, 1);   // will be overwritten by something totally different if any MelderInfo function is called...
		int status = praat_executeCommand (me, p);
		if (status == 0) {
			value = undefined;
		} else if (valueString.string [0] == 1) {   // ...not overwritten by any MelderInfo function? then the return value will be the selected object
			int IOBJECT, selectedObject = 0, numberOfSelectedObjects = 0;
			WHERE (SELECTED) { selectedObject = IOBJECT; numberOfSelectedObjects += 1; }
			if (numberOfSelectedObjects > 1) {
				Melder_throw (U"Multiple objects selected. Cannot assign object ID to matrix element.");
			} else if (numberOfSelectedObjects == 0) {
				Melder_throw (U"No objects selected. Cannot assign object ID to matrix element.");
			} else {
				value = theCurrentPraatObjects -> list [selectedObject]. id;
			}
		} else {
			value = Melder_atof (valueString.string);   // including --undefined--
		}
	} else {
		Interpreter_numericExpression (me, p, & value);
	}
	InterpreterVariable var = Interpreter_hasVariable (me, matrixName);
	if (! var)
		Melder_throw (U"Matrix ", matrixName, U" does not exist.");
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
	var -> numericMatrixValue [rowNumber] [columnNumber] = value;
}

static void assignToStringArrayElement (Interpreter me, char32 *& p, const char32* vectorName, MelderString& valueString) {
	integer indexValue = 0;
	static MelderString index;
	MelderString_empty (& index);
	int depth = 0;
	bool inString = false;
	while ((depth > 0 || *p != U']' || inString) && Melder_staysWithinLine (*p)) {
		MelderString_appendCharacter (& index, *p);
		if (*p == U'[') {
			if (! inString)
				depth ++;
		} else if (*p == U']') {
			if (! inString)
				depth --;
		}
		if (*p == U'"') inString = ! inString;
		p ++;
	}
	if (! Melder_staysWithinLine (*p))
		Melder_throw (U"Missing closing bracket (]) in array element.");
	Formula_Result result;
	Interpreter_anyExpression (me, index.string, & result);
	if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
		indexValue = Melder_iround (result. numericResult);
	} else {
		Melder_throw (U"Element index should be numeric.");
	}
	p ++;   // step over closing bracket
	while (Melder_isHorizontalSpace (*p))
		p ++;
	if (*p != U'=')
		Melder_throw (U"Missing '=' after string vector element ", vectorName, U" [", index.string, U"].");
	p ++;   // step over equals sign
	while (Melder_isHorizontalSpace (*p))
		p ++;   // go to first token after assignment
	if (*p == U'\0')
		Melder_throw (U"Missing expression after string vector element ", vectorName, U" [", index.string, U"].");
	autostring32 value;
	if (isCommand (p)) {
		/*
			Get the value of the query.
		*/
		MelderString_empty (& valueString);
		autoMelderDivertInfo divert (& valueString);
		int status = praat_executeCommand (me, p);
		value = ( status == 0 ? autostring32 () : Melder_dup (valueString.string) );
	} else {
		/*
			Get the value of the formula.
		*/
		value = Interpreter_stringExpression (me, p);
	}
	InterpreterVariable var = Interpreter_hasVariable (me, vectorName);
	if (! var)
		Melder_throw (U"String vector ", vectorName, U" does not exist.");
	if (indexValue < 1)
		Melder_throw (U"A vector index cannot be less than 1 (the index you supplied is ", indexValue, U").");
	if (indexValue > var -> stringArrayValue.size)
		Melder_throw (U"A vector index cannot be greater than the number of elements (here ",
			var -> stringArrayValue.size, U"). The index you supplied is ", indexValue, U".");
	var -> stringArrayValue [indexValue] = value. move();
}

void Interpreter_run (Interpreter me, char32 *text) {
	autovector <mutablestring32> lines;   // not autostringvector, because the elements are reference copies
	integer lineNumber = 0;
	bool assertionFailed = false;
	try {
		static MelderString valueString;   // to divert the info
		static MelderString assertErrorString;
		char32 *command = text;
		autoMelderString command2;
		autoMelderString buffer;
		integer numberOfLines = 0, assertErrorLineNumber = 0, callStack [1 + Interpreter_MAX_CALL_DEPTH];
		bool atLastLine = false, fromif = false, fromendfor = false;
		int callDepth = 0, chopped = 0, ipar;
		my callDepth = 0;
		/*
			The "environment" is null if we are in the Praat shell, or an editor otherwise.
		*/
		if (my editorClass)
			praatP. editor = praat_findEditorFromString (my environmentName.get());
		else
			praatP. editor = nullptr;
		/*
			Start.
		*/
		my running = true;
		/*
			Count lines and set the newlines to zero.
		*/
		while (! atLastLine) {
			char32 *endOfLine = command;
			while (Melder_staysWithinLine (*endOfLine))
				endOfLine ++;
			if (*endOfLine == U'\0')
				atLastLine = true;
			*endOfLine = U'\0';
			numberOfLines ++;
			command = endOfLine + 1;
		}
		/*
			Remember line starts and labels.
		*/
		lines. resize (numberOfLines);
		for (lineNumber = 1, command = text; lineNumber <= numberOfLines; lineNumber ++, command += str32len (command) + 1 + chopped) {
			while (Melder_isHorizontalSpace (*command))
				command ++;   // nbsp can occur for scripts copied from the manual
			/*
				Chop trailing spaces?
			*/
			#if 0
				chopped = 0;
				int length = str32len (command);
				while (length > 0) {
					char kar = command [-- length];
					if (! Melder_isHorizontalSpace (kar))
						break;
					command [length] = U'\0';
					chopped ++;
				}
			#endif
			lines [lineNumber] = command;
			if (str32nequ (command, U"label ", 6)) {
				for (integer ilabel = 1; ilabel <= my numberOfLabels; ilabel ++)
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
			Connect continuation lines.
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
			Copy the parameter names and argument values into the array of variables.
		*/
		my variablesMap. clear ();
		for (ipar = 1; ipar <= my numberOfParameters; ipar ++) {
			char32 parameter [200];
			/*
				Create variable names as-are and variable names without capitals.
			*/
			str32cpy (parameter, my parameters [ipar]);
			parameterToVariable (me, my types [ipar], parameter, ipar);
			if (parameter [0] >= U'A' && parameter [0] <= U'Z') {
				parameter [0] = Melder_toLowerCase (parameter [0]);
				parameterToVariable (me, my types [ipar], parameter, ipar);
			}
		}
		/*
			Initialize some variables.
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
			Execute commands.
		*/
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
				if (c0 == U'\0')
					continue;
				/*
					Substitute variables.
				*/
				trace (U"substituting variables");
				for (char32 *p = & command2. string [0]; *p != U'\0'; p ++) if (*p == U'\'') {
					/*
						Found a left quote. Search for a matching right quote.
					*/
					char32 *q = p + 1, varName [300], *r, *s;
					integer precision = -1;
					bool percent = false;
					while (*q != U'\0' && *q != U'\'' && q - p < 299)
						q ++;
					if (*q == U'\0')
						break;   // no matching right quote? done with this line!
					if (q - p == 1 || q - p >= 299)
						continue;   // ignore empty and too long variable names
					trace (U"found ", q - p - 1);
					/*
						Found a right quote. Get potential variable name.
					*/
					for (r = p + 1, s = varName; q - r > 0; r ++, s ++)
						*s = *r;
					*s = U'\0';   // trailing null byte
					char32 *colon = str32chr (varName, U':');
					if (colon) {
						precision = Melder_atoi (colon + 1);
						if (str32chr (colon + 1, U'%'))
							percent = true;
						*colon = U'\0';
					}
					InterpreterVariable var = Interpreter_hasVariable (me, varName);
					if (var) {
						/*
							Found a variable (p points to the left quote, q to the right quote). Substitute.
						*/
						integer headlen = p - command2.string;
						conststring32 string = ( var -> stringValue ? var -> stringValue.get() :
								percent ? Melder_percent (var -> numericValue, precision) :
								precision >= 0 ?  Melder_fixed (var -> numericValue, precision) :
								Melder_double (var -> numericValue) );
						integer arglen = str32len (string);
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
				if ((! Melder_isLetter (c0) || Melder_isUpperCaseLetter (c0)) && c0 != U'@' &&
						! (c0 == U'.' && Melder_isLetter (command2.string [1]) && ! Melder_isUpperCaseLetter (command2.string [1]))) {
					praat_executeCommand (me, command2.string);
				/*
				 * Interpret control flow and variables.
				 */
				} else switch (c0) {
					case U'.':
						fail = true;
						break;
					case U'@':
						Interpreter_do_procedureCall (me, command2.string + 1, lines.get(), lineNumber, callStack, callDepth);
						break;
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
						} else
							fail = true;
						break;
					case U'b':
						fail = true;
						break;
					case U'c':
						if (str32nequ (command2.string, U"call ", 5)) {
							Interpreter_do_oldProcedureCall (me, command2.string + 5, lines.get(), lineNumber, callStack, callDepth);
						} else
							fail = true;
						break;
					case U'd':
						if (str32nequ (command2.string, U"dec ", 4)) {
							InterpreterVariable var = Interpreter_lookUpVariable (me, command2.string + 4);
							var -> numericValue -= 1.0;
						} else
							fail = true;
						break;
					case U'e':
						if (command2.string [1] == U'n' && command2.string [2] == U'd') {
							if (str32nequ (command2.string, U"endif", 5) &&
									(! Melder_staysWithinInk (command2.string [5]) || command2.string [5] == U';'))
							{
								const char32 *startOfInk = Melder_findInk (command2.string + 5);
								if (startOfInk && *startOfInk != U';')
									Melder_throw (U"Stray text after 'endif'.");
								/* Ignore. */
							} else if (str32nequ (command2.string, U"endfor", 6) &&
									(! Melder_staysWithinInk (command2.string [6]) || command2.string [6] == U';'))
							{
								const char32 *startOfInk = Melder_findInk (command2.string + 6);
								if (startOfInk && *startOfInk != U';')
									Melder_throw (U"Stray text after 'endfor'.");
								int depth = 0;
								integer iline;
								for (iline = lineNumber - 1; iline > 0; iline --) {
									char32 *line = lines [iline];
									if (line [0] == U'f' && line [1] == U'o' && line [2] == U'r' && line [3] == U' ') {
										if (depth == 0) { lineNumber = iline - 1; fromendfor = true; break; }   // go before 'for'
										else depth --;
									} else if (str32nequ (lines [iline], U"endfor", 6) &&
											(! Melder_staysWithinInk (lines [iline] [6]) || lines [iline] [6] == U';'))
									{
										depth ++;
									}
								}
								if (iline <= 0) Melder_throw (U"Unmatched 'endfor'.");
							} else if (str32nequ (command2.string, U"endwhile", 8) &&
									(! Melder_staysWithinInk (command2.string [8]) || command2.string [8] == U';'))
							{
								const char32 *startOfInk = Melder_findInk (command2.string + 8);
								if (startOfInk && *startOfInk != U';')
									Melder_throw (U"Stray text after 'endwhile'.");
								int depth = 0;
								integer iline;
								for (iline = lineNumber - 1; iline > 0; iline --) {
									if (str32nequ (lines [iline], U"while ", 6)) {
										if (depth == 0) {
											lineNumber = iline - 1;
											break;   // go before 'while'
										} else
											depth --;
									} else if (str32nequ (lines [iline], U"endwhile", 8) &&
											(! Melder_staysWithinInk (lines [iline] [8]) || lines [iline] [8] == U';'))
									{
										depth ++;
									}
								}
								if (iline <= 0) Melder_throw (U"Unmatched 'endwhile'.");
							} else if (str32nequ (command2.string, U"endproc", 7) &&
									(! Melder_staysWithinInk (command2.string [7]) || command2.string [7] == U';'))
							{
								const char32 *startOfInk = Melder_findInk (command2.string + 7);
								if (startOfInk && *startOfInk != U';')
									Melder_throw (U"Stray text after 'endproc'.");
								if (callDepth == 0)
									Melder_throw (U"Unmatched 'endproc'.");
								lineNumber = callStack [callDepth --];
								-- my callDepth;
							} else fail = true;
						} else if (str32nequ (command2.string, U"else", 4) &&
								(! Melder_staysWithinInk (command2.string [4]) || command2.string [4] == U';'))
						{
							const char32 *startOfInk = Melder_findInk (command2.string + 4);
							if (startOfInk && *startOfInk != U';')
								Melder_throw (U"Stray text after 'else'.");
							int depth = 0;
							integer iline;
							for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
								if (str32nequ (lines [iline], U"endif", 5) &&
										(! Melder_staysWithinInk (lines [iline] [5]) || lines [iline] [5] == U';'))
								{
									startOfInk = Melder_findInk (lines [iline] + 5);
									if (startOfInk && *startOfInk != U';') {
										lineNumber = iline;   // on behalf of the error message
										Melder_throw (U"Stray text after 'endif'.");
									}
									if (depth == 0) { lineNumber = iline; break; }   // go after `endif`
									else depth --;
								} else if (str32nequ (lines [iline], U"if ", 3)) {
									depth ++;
								}
							}
							if (iline > numberOfLines)
								Melder_throw (U"Unmatched 'else'.");
						} else if (str32nequ (command2.string, U"elsif ", 6) || str32nequ (command2.string, U"elif ", 5)) {
							if (fromif) {
								double value;
								fromif = false;
								Interpreter_numericExpression (me, command2.string + 5, & value);
								if (value == 0.0) {
									int depth = 0;
									integer iline;
									for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
										if (str32nequ (lines [iline], U"endif", 5) &&
												(! Melder_staysWithinInk (lines [iline] [5]) || lines [iline] [5] == U';'))
										{
											const char32 *startOfInk = Melder_findInk (lines [iline] + 5);
											if (startOfInk && *startOfInk != U';') {
												lineNumber = iline;   // on behalf of error message
												Melder_throw (U"Stray text after 'endif'.");
											}
											if (depth == 0) {
												lineNumber = iline;
												break;   // go after `endif`
											} else
												depth --;
										} else if (str32nequ (lines [iline], U"else", 4) &&
												(! Melder_staysWithinInk (lines [iline] [4]) || lines [iline] [4] == U';'))
										{
											const char32 *startOfInk = Melder_findInk (lines [iline] + 4);
											if (startOfInk && *startOfInk != U';') {
												lineNumber = iline;   // on behalf of error message
												Melder_throw (U"Stray text after 'else'.");
											}
											if (depth == 0) {
												lineNumber = iline;
												break;   // go after `else`
											}
										} else if ((str32nequ (lines [iline], U"elsif", 5) && ! Melder_staysWithinInk (lines [iline] [5]))
											|| (str32nequ (lines [iline], U"elif", 4) && ! Melder_staysWithinInk (lines [iline] [4]))) {
											if (depth == 0) {
												lineNumber = iline - 1;
												fromif = true;
												break;   // go at next 'elsif' or 'elif'
											}
										} else if (str32nequ (lines [iline], U"if ", 3)) {
											depth ++;
										}
									}
									if (iline > numberOfLines)
										Melder_throw (U"Unmatched 'elsif'.");
								}
							} else {
								int depth = 0;
								integer iline;
								for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (str32nequ (lines [iline], U"endif", 5) &&
											(! Melder_staysWithinInk (lines [iline] [5]) || lines [iline] [5] == U';'))
									{
										const char32 *startOfInk = Melder_findInk (lines [iline] + 5);
										if (startOfInk && *startOfInk != U';') {
											lineNumber = iline;   // on behalf of error message
											Melder_throw (U"Stray text after 'endif'.");
										}
										if (depth == 0) {
											lineNumber = iline;
											break;   // go after `endif`
										} else
											depth --;
									} else if (str32nequ (lines [iline], U"if ", 3)) {
										depth ++;
									}
								}
								if (iline > numberOfLines)
									Melder_throw (U"'elsif' not matched with 'endif'.");
							}
						} else if (str32nequ (command2.string, U"exit", 4)) {
							if (command2.string [4] == U'\0') {
								lineNumber = numberOfLines;   // go after end
							} else if (command2.string [4] == U' ') {
								Melder_throw (command2.string + 5);
							} else
								fail = true;
						} else if (str32nequ (command2.string, U"echo ", 5)) {
							/*
								Make sure that lines like "echo = 3" will not be regarded as assignments.
							*/
							praat_executeCommand (me, command2.string);
						} else
							fail = true;
						break;
					case U'f':
						if (command2.string [1] == U'o' && command2.string [2] == U'r' && command2.string [3] == U' ') {   // for_
							double toValue, loopVariable;
							char32 *frompos = str32str (command2.string, U" from "), *topos = str32str (command2.string, U" to ");
							char32 *varpos = command2.string + 4, *endvar = frompos;
							if (! topos)
								Melder_throw (U"Missing \'to\' in \'for\' loop.");
							if (! endvar)
								endvar = topos;
							while (*endvar == U' ') {
								*endvar = U'\0';
								endvar --;
							}
							while (*varpos == U' ')
								varpos ++;
							if (endvar - varpos < 0)
								Melder_throw (U"Missing loop variable after \'for\'.");
							InterpreterVariable var = Interpreter_lookUpVariable (me, varpos);
							Interpreter_numericExpression (me, topos + 4, & toValue);
							if (fromendfor) {
								fromendfor = false;
								loopVariable = var -> numericValue + 1.0;
							} else if (frompos) {
								*topos = U'\0';
								Interpreter_numericExpression (me, frompos + 6, & loopVariable);
							} else {
								loopVariable = 1.0;
							}
							var -> numericValue = loopVariable;
							if (loopVariable > toValue) {
								int depth = 0;
								integer iline;
								for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (str32nequ (lines [iline], U"endfor", 6) &&
											(! Melder_staysWithinInk (lines [iline] [6]) || lines [iline] [6] == U';'))
									{
										const char32 *startOfInk = Melder_findInk (lines [iline] + 6);
										if (startOfInk && *startOfInk != U';') {
											lineNumber = iline;   // on behalf of error message
											Melder_throw (U"Stray text after 'endfor'.");
										}
										if (depth == 0) {
											lineNumber = iline;
											break;   // go after 'endfor'
										} else
											depth --;
									} else if (str32nequ (lines [iline], U"for ", 4)) {
										depth ++;
									}
								}
								if (iline > numberOfLines)
									Melder_throw (U"Unmatched 'for'.");
							}
						} else if (str32nequ (command2.string, U"form", 4) && Melder_isEndOfInk (command2.string [4])) {
							integer iline;
							for (iline = lineNumber + 1; iline <= numberOfLines; iline ++)
								if (str32nequ (lines [iline], U"endform", 7) && Melder_isEndOfInk (lines [iline] [7])) {
									lineNumber = iline;
									break;   // go after 'endform'
								}
							if (iline > numberOfLines)
								Melder_throw (U"Unmatched 'form'.");
						} else
							fail = true;
						break;
					case U'g':
						if (str32nequ (command2.string, U"goto ", 5)) {
							char32 labelName [1+Interpreter_MAX_LABEL_LENGTH];
							str32ncpy (labelName, command2.string + 5, 1+Interpreter_MAX_LABEL_LENGTH);
							labelName [Interpreter_MAX_LABEL_LENGTH] = U'\0';
							char32 *space = str32chr (labelName, U' ');
							if (space == labelName)
								Melder_throw (U"Missing label name after 'goto'.");
							bool dojump = true;
							if (space) {
								double value;
								*space = '\0';
								Interpreter_numericExpression (me, command2.string + 6 + str32len (labelName), & value);
								if (value == 0.0)
									dojump = false;
							}
							if (dojump) {
								integer ilabel = lookupLabel (me, labelName);
								lineNumber = my labelLines [ilabel];   // loop will add 1
							}
						} else
							fail = true;
						break;
					case U'h':
						fail = true;
						break;
					case U'i':
						if (command2.string [1] == U'f' && Melder_isHorizontalSpace (command2.string [2])) {   // if_
							double value;
							Interpreter_numericExpression (me, command2.string + 3, & value);
							if (value == 0.0) {
								int depth = 0;
								integer iline;
								for (iline = lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (str32nequ (lines [iline], U"endif", 5) &&
											(! Melder_staysWithinInk (lines [iline] [5]) || lines [iline] [5] == U';'))
									{
										const char32 *startOfInk = Melder_findInk (lines [iline] + 5);
										if (startOfInk && *startOfInk != U';') {
											lineNumber = iline;   // on behalf of error message
											Melder_throw (U"Stray text after 'endif'.");
										}
										if (depth == 0) {
											lineNumber = iline;
											break;   // go after 'endif'
										} else
											depth --;
									} else if (str32nequ (lines [iline], U"else", 4) &&
											(! Melder_staysWithinInk (lines [iline] [4]) || lines [iline] [4] == U';'))
									{
										const char32 *startOfInk = Melder_findInk (lines [iline] + 4);
										if (startOfInk && *startOfInk != U';') {
											lineNumber = iline;   // on behalf of error message
											Melder_throw (U"Stray text after 'else'.");
										}
										if (depth == 0) {
											lineNumber = iline;
											break;   // go after 'else'
										}
									} else if (str32nequ (lines [iline], U"elsif ", 6) || str32nequ (lines [iline], U"elif ", 5)) {
										if (depth == 0) {
											lineNumber = iline - 1;
											fromif = true;
											break;   // go at 'elsif'
										}
									} else if (str32nequ (lines [iline], U"if ", 3)) {
										depth ++;
									}
								}
								if (iline > numberOfLines)
									Melder_throw (U"Unmatched 'if'.");
							} else if (isundef (value)) {
								Melder_throw (U"The value of the 'if' condition is undefined.");
							}
						} else if (str32nequ (command2.string, U"inc ", 4)) {
							InterpreterVariable var = Interpreter_lookUpVariable (me, command2.string + 4);
							var -> numericValue += 1.0;
						} else
							fail = true;
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
						} else
							fail = true;
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
							integer iline = lineNumber + 1;
							for (; iline <= numberOfLines; iline ++) {
								if (str32nequ (lines [iline], U"endproc", 7) &&
										(! Melder_staysWithinInk (lines [iline] [7]) || lines [iline] [7] == U';'))
								{
									const char32 *startOfInk = Melder_findInk (lines [iline] + 7);
									if (startOfInk && *startOfInk != U';') {
										lineNumber = iline;   // on behalf of error message
										Melder_throw (U"Stray text after 'endproc'.");
									}
									lineNumber = iline;
									break;
								}   // go after `endproc`
							}
							if (iline > numberOfLines) Melder_throw (U"Unmatched 'procedure'.");
						} else if (str32nequ (command2.string, U"print", 5)) {
							/*
							 * Make sure that lines like "print = 3" will not be regarded as assignments.
							 */
							if (command2.string [5] == U' ' || (str32nequ (command2.string + 5, U"line", 4) && (command2.string [9] == U' ' || command2.string [9] == U'\0'))) {
								praat_executeCommand (me, command2.string);
							} else
								fail = true;
						} else
							fail = true;
						break;
					case U'q':
						fail = true;
						break;
					case U'r':
						if (str32nequ (command2.string, U"repeat", 6) &&
								(! Melder_staysWithinInk (command2.string [6]) || command2.string [6] == U';'))
						{
							const char32 *startOfInk = Melder_findInk (command2.string + 6);
							if (startOfInk && *startOfInk != U';')
								Melder_throw (U"Stray text after 'repeat'.");
							/* Ignore. */
						} else
							fail = true;
						break;
					case U's':
						if (str32nequ (command2.string, U"stopwatch", 9) && ! Melder_staysWithinInk (command2.string [9])) {
							(void) Melder_stopwatch ();   // reset stopwatch
						} else
							fail = true;
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
								integer iline = lineNumber - 1;
								for (; iline > 0; iline --) {
									if (str32nequ (lines [iline], U"repeat", 6) &&
											(! Melder_staysWithinInk (lines [iline] [6]) || lines [iline] [6] == U';'))
									{
										if (depth == 0) {
											lineNumber = iline;
											break;   // go after `repeat`
										} else
											depth --;
									} else if (str32nequ (lines [iline], U"until ", 6)) {
										depth ++;
									}
								}
								if (iline <= 0)
									Melder_throw (U"Unmatched 'until'.");
							}
						} else
							fail = true;
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
								integer iline = lineNumber + 1;
								for (; iline <= numberOfLines; iline ++) {
									if (str32nequ (lines [iline], U"endwhile", 8) &&
											(! Melder_staysWithinInk (lines [iline] [8]) || lines [iline] [8] == U';'))
									{
										const char32 *startOfInk = Melder_findInk (lines [iline] + 8);
										if (startOfInk && *startOfInk != U';') {
											lineNumber = iline;
											Melder_throw (U"Stray text after 'endwhile'.");
										}
										if (depth == 0) {
											lineNumber = iline;
											break;   // go after `endwhile`
										} else
											depth --;
									} else if (str32nequ (lines [iline], U"while ", 6)) {
										depth ++;
									}
								}
								if (iline > numberOfLines)
									Melder_throw (U"Unmatched 'while'.");
							}
						} else
							fail = true;
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
					default:
						fail = true;
						break;
				}
				if (fail) {
					/*
						Found an unknown word starting with a nonupper-case letter, optionally preceded by a period.
						See whether the word is a variable name.
					*/
					trace (U"found an unknown word starting with a nonupper-case letter, optionally preceded by a period");
					char32 *p = & command2.string [0];
					/*
						Variable names consist of a sequence of letters, digits, and underscores,
						optionally preceded by a period and optionally followed by a $ and/or #.
					*/
					if (*p == U'.')
						p ++;
					while (Melder_isWordCharacter (*p) || *p == U'.')
						p ++;
					if (*p == U'$') {
						if (p [1] == U'#') {
							/*
								Assign to a string vector variable or a string vector element.
							*/
							static MelderString vectorName;
							p ++;
							*p = U'\0';   // erase the number sign temporarily
							MelderString_copy (& vectorName, command2.string, U"#");
							*p = U'#';   // put the number sign back
							p ++;   // step over number sign
							while (Melder_isHorizontalSpace (*p))
								p ++;   // go to first token after array name
							if (*p == U'=') {
								/*
									This must be an assignment to a string vector variable.
								*/
								p ++;   // step over equals sign
								while (Melder_isHorizontalSpace (*p))
									p ++;   // go to first token after assignment
								if (*p == U'\0')
									Melder_throw (U"Missing right-hand expression in assignment to string vector ", vectorName.string, U".");
								if (isCommand (p)) {
									/*
										Statement like: lines$# = Get all strings
									*/
									praat_executeCommand (me, p);
									InterpreterVariable var = Interpreter_lookUpVariable (me, vectorName.string);
									var -> stringArrayValue = std::move (theInterpreterStrvec);
								} else {
									STRVEC value;
									bool owned;
									Interpreter_stringArrayExpression (me, p, & value, & owned);
									InterpreterVariable var = Interpreter_lookUpVariable (me, vectorName.string);
									StringArrayVariable_move (var, value, owned);
								}
							} else if (*p == U'[') {
								assignToStringArrayElement (me, ++ p, vectorName.string, valueString);
							} else Melder_throw (U"Missing '=' or '[' after string vector variable ", vectorName.string, U".");
						} else {
							/*
								Assign to a string variable.
							*/
							trace (U"detected an assignment to a string variable");
							char32 *endOfVariable = ++ p;
							char32 *variableName = command2.string;
							while (Melder_isHorizontalSpace (*p))
								p ++;   // go to first token after variable name
							if (*p == U'[') {
								/*
									This must be an assignment to an indexed string variable.
								*/
								*endOfVariable = U'\0';
								static MelderString indexedVariableName;
								MelderString_copy (& indexedVariableName, command2.string, U"[");
								for (;;) {
									p ++;   // skip opening bracket or comma
									static MelderString index;
									MelderString_empty (& index);
									int depth = 0;
									bool inString = false;
									while ((depth > 0 || (*p != U',' && *p != U']') || inString) && Melder_staysWithinLine (*p)) {
										MelderString_appendCharacter (& index, *p);
										if (*p == U'[') {
											if (! inString)
												depth ++;
										} else if (*p == U']') {
											if (! inString)
												depth --;
										}
										if (*p == U'"')
											inString = ! inString;
										p ++;
									}
									if (! Melder_staysWithinLine (*p))
										Melder_throw (U"Missing closing bracket (]) in indexed variable.");
									Formula_Result result;
									Interpreter_anyExpression (me, index.string, & result);
									if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
										double numericIndexValue = result. numericResult;
										MelderString_append (& indexedVariableName, numericIndexValue);
									} else if (result.expressionType == kFormula_EXPRESSION_TYPE_STRING) {
										MelderString_append (& indexedVariableName, U"\"", result. stringResult.get(), U"\"");
									}
									MelderString_appendCharacter (& indexedVariableName, *p);
									if (*p == U']') {
										break;
									}
								}
								variableName = indexedVariableName.string;
								p ++;   // skip closing bracket
							}
							while (Melder_isHorizontalSpace (*p)) p ++;   // go to first token after (perhaps indexed) variable name
							int typeOfAssignment;   // 0, 1, 2, 3 or 4
							if (*p == U'=') {
								typeOfAssignment = 0;   // assignment
							} else if (*p == U'+') {
								if (p [1] == U'=') {
									typeOfAssignment = 1;   // adding assignment
									p ++;
								} else {
									Melder_throw (U"Missing \"=\", \"+=\", \"<\", or \">\" after variable ", variableName, U".");
								}
							} else if (*p == U'<') {
								typeOfAssignment = 2;   // read from file
							} else if (*p == U'>') {
								if (p [1] == U'>') {
									typeOfAssignment = 3;   // append to file
									p ++;
								} else {
									typeOfAssignment = 4;   // save to file
								}
							} else Melder_throw (U"Missing \"=\", \"+=\", \"<\", or \">\" after variable ", variableName, U".");
							*endOfVariable = U'\0';
							p ++;
							while (Melder_isHorizontalSpace (*p)) p ++;   // go to first token after assignment or I/O symbol
							if (*p == U'\0') {
								if (typeOfAssignment >= 2)
									Melder_throw (U"Missing file name after variable ", variableName, U".");
								else
									Melder_throw (U"Missing expression after variable ", variableName, U".");
							}
							if (typeOfAssignment >= 2) {
								structMelderFile file { };
								Melder_relativePathToFile (p, & file);
								if (typeOfAssignment == 2) {
									autostring32 stringValue = MelderFile_readText (& file);
									InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
									var -> stringValue = stringValue.move();
								} else if (typeOfAssignment == 3) {
									if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw (U"Commands that write to a file are not available inside pictures.");
									InterpreterVariable var = Interpreter_hasVariable (me, variableName);
									if (! var) Melder_throw (U"Variable ", variableName, U" undefined.");
									MelderFile_appendText (& file, var -> stringValue.get());
								} else {
									if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw (U"Commands that write to a file are not available inside pictures.");
									InterpreterVariable var = Interpreter_hasVariable (me, variableName);
									if (! var) Melder_throw (U"Variable ", variableName, U" undefined.");
									MelderFile_writeText (& file, var -> stringValue.get(), Melder_getOutputEncoding ());
								}
							} else if (isCommand (p)) {
								/*
									Statement like: name$ = Get name
								*/
								MelderString_empty (& valueString);   // empty because command may print nothing; also makes sure that valueString.string exists
								autoMelderDivertInfo divert (& valueString);
								int status = praat_executeCommand (me, p);
								InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
								var -> stringValue = Melder_dup (status ? valueString.string : U"");
							} else {
								/*
									Evaluate a string expression and assign the result to the variable.
									Statements like:
										sentence$ = subject$ + verb$ + object$
										extension$ = if index (file$, ".") <> 0
										... then right$ (file$, length (file$) - rindex (file$, "."))
										... else "" fi
								*/
								trace (U"evaluating string expression");
								autostring32 stringValue = Interpreter_stringExpression (me, p);
								trace (U"assigning to string variable ", variableName);
								if (typeOfAssignment == 1) {
									InterpreterVariable var = Interpreter_hasVariable (me, variableName);
									if (! var)
										Melder_throw (U"The string ", variableName, U" does not exist.\n"
													  U"You can increment (+=) only existing strings.");
									integer oldLength = str32len (var -> stringValue.get()), extraLength = str32len (stringValue.get());
									autostring32 newString = autostring32 (oldLength + extraLength, false);
									str32cpy (newString.get(), var -> stringValue.get());
									str32cpy (newString.get() + oldLength, stringValue.get());
									var -> stringValue = newString.move();
								} else {
									InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
									var -> stringValue = stringValue.move();
								}
							}
						}
					} else if (*p == U'#') {
						if (p [1] == U'#') {
							/*
								Assign to a numeric matrix variable or to a matrix element.
							*/
							static MelderString matrixName;
							p ++;   // go to second '#'
							*p = U'\0';   // erase the last number sign temporarily
							MelderString_copy (& matrixName, command2.string, U'#');
							*p = U'#';   // put the number sign back
							p ++;   // step over last number sign
							while (Melder_isHorizontalSpace (*p)) p ++;   // go to first token after matrix name
							if (*p == U'=') {
								/*
									This must be an assignment to a matrix variable.
								*/
								p ++;   // step over equals sign
								while (Melder_isHorizontalSpace (*p)) p ++;   // go to first token after assignment
								if (*p == U'\0')
									Melder_throw (U"Missing right-hand expression in assignment to matrix ", matrixName.string, U".");
								if (isCommand (p)) {
									/*
										Statement like: values## = Get all values
									*/
									praat_executeCommand (me, p);
									InterpreterVariable var = Interpreter_lookUpVariable (me, matrixName.string);
									var -> numericMatrixValue = theInterpreterNummat.move();
								} else {
									MAT value;
									bool owned;
									Interpreter_numericMatrixExpression (me, p, & value, & owned);
									InterpreterVariable var = Interpreter_lookUpVariable (me, matrixName.string);
									NumericMatrixVariable_move (var, value, owned);
								}
							} else if (*p == U'[') {
								assignToNumericMatrixElement (me, ++ p, matrixName.string, valueString);
							} else if (*p == U'+' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, matrixName.string);
								if (! var)
									Melder_throw (U"The matrix ", matrixName.string, U" does not exist.\n"
									              U"You can increment (+=) only existing matrices.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX) {
									NumericMatrixVariable_add (var, result. numericMatrixResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericMatrixVariable_add (var, result. numericResult);
								} else {
									Melder_throw (U"You can increment (+=) a numeric matrix only with a number or another numeric matrix.");
								}
							} else if (*p == U'-' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, matrixName.string);
								if (! var)
									Melder_throw (U"The matrix ", matrixName.string, U" does not exist.\n"
									              U"You can decrement (-=) only existing matrices.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX) {
									NumericMatrixVariable_subtract (var, result. numericMatrixResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericMatrixVariable_subtract (var, result. numericResult);
								} else {
									Melder_throw (U"You can decrement (-=) a numeric matrix only with a number or another numeric matrix.");
								}
							} else if (*p == U'*' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, matrixName.string);
								if (! var)
									Melder_throw (U"The matrix ", matrixName.string, U" does not exist.\n"
									              U"You can multiply (*=) only existing matrices.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX) {
									NumericMatrixVariable_multiply (var, result. numericMatrixResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericMatrixVariable_multiply (var, result. numericResult);
								} else {
									Melder_throw (U"You can multiply (*=) a numeric matrix only with a number or another numeric matrix.");
								}
							} else if (*p == U'/' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, matrixName.string);
								if (! var)
									Melder_throw (U"The matrix ", matrixName.string, U" does not exist.\n"
									              U"You can divide (/=) only existing matrices.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX) {
									NumericMatrixVariable_divide (var, result. numericMatrixResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericMatrixVariable_divide (var, result. numericResult);
								} else {
									Melder_throw (U"You can divide (/=) a numeric matrix only with a number or another numeric matrix.");
								}
							} else if (*p == U'~') {
								/*
									This must be a formula assignment to a matrix variable.
								*/
								p ++;   // step over tilde
								while (Melder_isHorizontalSpace (*p)) p ++;   // go to first token after assignment
								if (*p == U'\0')
									Melder_throw (U"Missing formula expression for matrix ", matrixName.string, U".");
								InterpreterVariable var = Interpreter_hasVariable (me, matrixName.string);
								if (! var)
									Melder_throw (U"The matrix ", matrixName.string, U" does not exist.\n"
										"You can assign a formula only to an existing matrix.");
								static Matrix matrixObject;
								if (! matrixObject)
									matrixObject = Matrix_createSimple (1, 1). releaseToAmbiguousOwner();   // prevent exit-time destruction
								MAT mat = var -> numericMatrixValue.get();
								matrixObject -> xmax = mat.ncol + 0.5;
								matrixObject -> nx = mat.ncol;
								matrixObject -> ymax = mat.nrow + 0.5;
								matrixObject -> ny = mat.nrow;
								matrixObject -> z.cells = mat.cells;   // just a reference (YUCK)
								matrixObject -> z.nrow = mat.nrow;
								matrixObject -> z.ncol = mat.ncol;
								Matrix_formula (matrixObject, p, me, nullptr);
							} else Melder_throw (U"Missing '=' after matrix variable ", matrixName.string, U".");
						} else {
							/*
								Assign to a numeric vector variable or to a vector element.
							*/
							static MelderString vectorName;
							*p = U'\0';   // erase the number sign temporarily
							MelderString_copy (& vectorName, command2.string, U"#");
							*p = U'#';   // put the number sign back
							p ++;   // step over number sign
							while (Melder_isHorizontalSpace (*p))
								p ++;   // go to first token after array name
							if (*p == U'=') {
								/*
									This must be an assignment to a vector variable.
								*/
								p ++;   // step over equals sign
								while (Melder_isHorizontalSpace (*p))
									p ++;   // go to first token after assignment
								if (*p == U'\0')
									Melder_throw (U"Missing right-hand expression in assignment to vector ", vectorName.string, U".");
								if (isCommand (p)) {
									/*
										Statement like: times# = Get all times
									*/
									praat_executeCommand (me, p);
									InterpreterVariable var = Interpreter_lookUpVariable (me, vectorName.string);
									var -> numericVectorValue = theInterpreterNumvec.move();
								} else {
									VEC value;
									bool owned;
									Interpreter_numericVectorExpression (me, p, & value, & owned);
									InterpreterVariable var = Interpreter_lookUpVariable (me, vectorName.string);
									NumericVectorVariable_move (var, value, owned);
								}
							} else if (*p == U'[') {
								assignToNumericVectorElement (me, ++ p, vectorName.string, valueString);
							} else if (*p == U'+' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, vectorName.string);
								if (! var)
									Melder_throw (U"The vector ", vectorName.string, U" does not exist.\n"
									              U"You can increment (+=) only existing vectors.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR) {
									NumericVectorVariable_add (var, result. numericVectorResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericVectorVariable_add (var, result. numericResult);
								} else {
									Melder_throw (U"You can increment (+=) a numeric vector only with a number or another numeric vector.");
								}
							} else if (*p == U'-' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, vectorName.string);
								if (! var)
									Melder_throw (U"The vector ", vectorName.string, U" does not exist.\n"
									              U"You can decrement (-=) only existing vectors.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR) {
									NumericVectorVariable_subtract (var, result. numericVectorResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericVectorVariable_subtract (var, result. numericResult);
								} else {
									Melder_throw (U"You can decrement (-=) a numeric vector only with a number or another numeric vector.");
								}
							} else if (*p == U'*' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, vectorName.string);
								if (! var)
									Melder_throw (U"The vector ", vectorName.string, U" does not exist.\n"
									              U"You can multiply (*=) only existing vectors.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR) {
									NumericVectorVariable_multiply (var, result. numericVectorResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericVectorVariable_multiply (var, result. numericResult);
								} else {
									Melder_throw (U"You can multiply (*=) a numeric vector only with a number or another numeric vector.");
								}
							} else if (*p == U'/' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, vectorName.string);
								if (! var)
									Melder_throw (U"The vector ", vectorName.string, U" does not exist.\n"
									              U"You can divide (/=) only existing vectors.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR) {
									NumericVectorVariable_divide (var, result. numericVectorResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericVectorVariable_divide (var, result. numericResult);
								} else {
									Melder_throw (U"You can divide (/=) a numeric vector only with a number or another numeric vector.");
								}
							} else if (*p == U'~') {
								/*
									This must be a formula assignment to a vector variable.
								*/
								p ++;   // step over tilde
								while (Melder_isHorizontalSpace (*p))
									p ++;   // go to first token after assignment
								if (*p == U'\0')
									Melder_throw (U"Missing formula expression for vector ", vectorName.string, U".");
								InterpreterVariable var = Interpreter_hasVariable (me, vectorName.string);
								if (! var)
									Melder_throw (U"The vector ", vectorName.string, U" does not exist.\n"
										"You can assign a formula only to an existing vector.");
								static Matrix vectorObject;
								if (! vectorObject)
									vectorObject = Matrix_createSimple (1, 1). releaseToAmbiguousOwner();   // prevent destruction when program ends
								VEC vec = var -> numericVectorValue.get();
								//vectorObject -> xmin = 0.5;
								vectorObject -> xmax = vec.size + 0.5;
								vectorObject -> nx = vec.size;
								vectorObject -> z.cells = & vec [1];
								//vectorObject -> z.nrow = 1;
								vectorObject -> z.ncol = vec.size;
								Matrix_formula (vectorObject, p, me, nullptr);
							} else Melder_throw (U"Missing '=' or '+=' or '[' or '~' after vector variable ", vectorName.string, U".");
						}
					} else {
						/*
							Try to assign to a numeric variable.
						*/
						double value;
						char32 *variableName = command2.string;
						int typeOfAssignment = 0;   // plain assignment
						if (*p == U'\0') {
							/*
								Command ends here: it may be a PraatShell command.
							*/
							praat_executeCommand (me, command2.string);
							continue;   // next line
						}
						char32 *endOfVariable = p;
						while (Melder_isHorizontalSpace (*p)) p ++;
						if (*p == U'=' || ((*p == U'+' || *p == U'-' || *p == U'*' || *p == U'/') && p [1] == U'=')) {
							/*
								This must be an assignment (though: "echo = ..." ???)
							*/
							typeOfAssignment = ( *p == U'+' ? 1 : *p == U'-' ? 2 : *p == U'*' ? 3 : *p == U'/' ? 4 : 0 );
							*endOfVariable = U'\0';   // close variable name; FIXME: this can be any weird character, e.g. hallo&
						} else if (*p == U'[') {
							/*
								This must be an assignment to an indexed numeric variable.
							*/
							*endOfVariable = U'\0';
							static MelderString indexedVariableName;
							MelderString_copy (& indexedVariableName, command2.string, U"[");
							for (;;) {
								p ++;   // skip opening bracket or comma
								static MelderString index;
								MelderString_empty (& index);
								int depth = 0;
								bool inString = false;
								while ((depth > 0 || (*p != U',' && *p != U']') || inString) && Melder_staysWithinLine (*p)) {
									MelderString_appendCharacter (& index, *p);
									if (*p == U'[') {
										if (! inString)
											depth ++;
									} else if (*p == U']') {
										if (! inString)
											depth --;
									}
									if (*p == U'"')
										inString = ! inString;
									p ++;
								}
								if (! Melder_staysWithinLine (*p))
									Melder_throw (U"Missing closing bracket (]) in indexed variable.");
								Formula_Result result;
								Melder_assert (! result. stringResult);
								Interpreter_anyExpression (me, index.string, & result);
								if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									double numericIndexValue = result. numericResult;
									MelderString_append (& indexedVariableName, numericIndexValue);
								} else if (result.expressionType == kFormula_EXPRESSION_TYPE_STRING) {
									MelderString_append (& indexedVariableName, U"\"", result. stringResult.get(), U"\"");
								}
								MelderString_appendCharacter (& indexedVariableName, *p);
								if (*p == U']')
									break;
							}
							variableName = indexedVariableName.string;
							p ++;   // skip closing bracket
							while (Melder_isHorizontalSpace (*p))
								p ++;
							if (*p == U'=' || ((*p == U'+' || *p == U'-' || *p == U'*' || *p == U'/') && p [1] == U'=')) {
								typeOfAssignment = ( *p == U'+' ? 1 : *p == U'-' ? 2 : *p == U'*' ? 3 : *p == U'/' ? 4 : 0 );
							}
						} else {
							/*
								Not an assignment: perhaps a PraatShell command (select, echo, execute, pause ...).
							*/
							praat_executeCommand (me, variableName);
							continue;   // next line
						}
						p += ( typeOfAssignment == 0 ? 1 : 2 );
						while (Melder_isHorizontalSpace (*p))
							p ++;
						if (*p == U'\0')
							Melder_throw (U"Missing expression after variable ", variableName, U".");
						/*
							Three classes of assignments:
								var = formula
								var = Query
								var = Object creation
						*/
						if (isCommand (p)) {
							/*
								Get the value of the query.
							*/
							MelderString_empty (& valueString);
							autoMelderDivertInfo divert (& valueString);
							MelderString_appendCharacter (& valueString, 1);   // will be overwritten by something totally different if any MelderInfo function is called...
							int status = praat_executeCommand (me, p);
							if (status == 0) {
								value = undefined;
							} else if (valueString.string [0] == 1) {   // ...not overwritten by any MelderInfo function? then the return value will be the selected object
								int IOBJECT, selectedObject = 0, numberOfSelectedObjects = 0;
								WHERE (SELECTED) { selectedObject = IOBJECT; numberOfSelectedObjects += 1; }
								if (numberOfSelectedObjects > 1) {
									Melder_throw (U"Multiple objects selected. Cannot assign object ID to variable.");
								} else if (numberOfSelectedObjects == 0) {
									Melder_throw (U"No objects selected. Cannot assign object ID to variable.");
								} else {
									value = theCurrentPraatObjects -> list [selectedObject]. id;
								}
							} else {
								value = Melder_atof (valueString.string);   // including --undefined--
							}
						} else {
							/*
								Get the value of the formula.
							*/
							Interpreter_numericExpression (me, p, & value);
						}
						/*
							Assign the value to a variable.
						*/
						if (typeOfAssignment == 0) {
							/*
								Use an existing variable, or create a new one.
							*/
							//Melder_casual (U"looking up variable ", variableName);
							InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
							var -> numericValue = value;
						} else {
							/*
								Modify an existing variable.
							*/
							InterpreterVariable var = Interpreter_hasVariable (me, variableName);
							if (! var)
								Melder_throw (U"The variable ", variableName, U" does not exist. You can modify only existing variables.");
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
					integer save_assertErrorLineNumber = assertErrorLineNumber;
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
						autostring32 errorCopy_nothrow = Melder_dup_f (Melder_getError ());
						Melder_clearError ();
						Melder_throw (U"Script assertion fails in line ", assertErrorLineNumber,
							U": error « ", assertErrorString.string, U" » not raised. Instead:\n",
							errorCopy_nothrow.get());
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

void Interpreter_voidExpression (Interpreter me, conststring32 expression) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_NUMERIC, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
}

void Interpreter_numericExpression (Interpreter me, conststring32 expression, double *out_value) {
	Melder_assert (out_value);
	if (str32str (expression, U"(=")) {
		*out_value = Melder_atof (expression);
	} else {
		Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_NUMERIC, false);
		Formula_Result result;
		Formula_run (0, 0, & result);
		*out_value = result. numericResult;
	}
}

void Interpreter_numericVectorExpression (Interpreter me, conststring32 expression, VEC *out_value, bool *out_owned) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
	*out_value = result. numericVectorResult;
	*out_owned = result. owned;
	result. owned = false;
}

void Interpreter_numericMatrixExpression (Interpreter me, conststring32 expression, MAT *out_value, bool *out_owned) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
	*out_value = result. numericMatrixResult;
	*out_owned = result. owned;
	result. owned = false;
}

autostring32 Interpreter_stringExpression (Interpreter me, conststring32 expression) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_STRING, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
	return result. stringResult.move();
}

void Interpreter_stringArrayExpression (Interpreter me, conststring32 expression, STRVEC *out_value, bool *out_owned) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_STRING_ARRAY, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
	*out_value = result. stringArrayResult;
	*out_owned = result. owned;
	result. owned = false;
}

void Interpreter_anyExpression (Interpreter me, conststring32 expression, Formula_Result *out_result) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_UNKNOWN, false);
	Formula_run (0, 0, out_result);
}

/* End of file Interpreter.cpp */
