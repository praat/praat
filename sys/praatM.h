#ifndef _praatM_h_
#define _praatM_h_
/* praatM.h
 *
 * Copyright (C) 1992-2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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

#ifdef _EditorM_h_
	#error Include either praatM.h or EditorM.h, but not both.
#endif

#include "praat.h"

/* Macros for description of forms (dialog boxes, setting windows).
	FORM prompts the user for arguments to proc.
	Macros for FORM:
	FORM (proc, title, helpString)
		`proc` is the `cb` argument of the corresponding command call.
		`title` is the title of the form, shown in its title bar.
		`helpString` may be null.
	INTEGER (variable, labelText, defaultStringValue)
	NATURAL (variable, labelText, defaultStringValue)
	REAL (variable, labelText, defaultStringValue)
	REAL_OR_UNDEFINED (variable, labelText, defaultStringValue)
	POSITIVE (variable, labelText, defaultStringValue)
	WORD (variable, labelText, defaultStringValue)
	SENTENCE (variable, labelText, defaultStringValue)
	COLOUR (variable, labelText, defaultStringValue)
	CHANNEL (variable, labelText, defaultStringValue)
	BOOLEAN (variable, labelText, defaultBooleanValue)
		the value is 0 (off) or 1 (on).
	COMMENT (labelText)
	TEXTFIELD (variable, labelText, defaultStringValue, numberOfLines)
	REALVECTOR (variable, labelText, WHITESPACE_SEPARATED_, defaultStringValue)
	REALMATRIX (variable, labelText, ONE_ROW_PER_LINE_, defaultStringValue)
	CHOICE (variable, labelText, defaultOptionNumber, base)
		this should be followed by two or more OPTIONs;
		the initial value is between base and the number of choice buttons plus base-1.
		OPTION (labelText)
	OPTIONMENU (variable, labelText, defaultOptionNumber)
		this should be followed by two or more OPTIONs;
		the initial value is between 1 and the number of options.
		OPTION (labelText)
	LIST (variable, labelText, strings, defaultOptionNumber)
	OK
		this statement is obligatory.
	SET_XXXXXX (name, value)
		sets the value of REAL..LIST.
	DO
	The order of the macros should be:
		FORM {
			one or more from REAL..LIST
		OK
			zero or more SETs
		DO
			...
		}
	See Ui.h for more information.
	Between DO and `}`, you can throw an exception if anything is wrong;
	if everything is all right, you just trickle down to END.
	Never do "return", because END will update the selection if an object is created.
 */

#define FORM(proc,name,helpTitle)  \
	extern "C" void proc (UiForm sendingForm, integer narg, Stackel args, conststring32 sendingString, \
			Interpreter interpreter, conststring32 invokingButtonTitle, bool isModified, void *buttonClosure, Editor optionalEditor); \
	void proc (UiForm _sendingForm_, integer _narg_, Stackel _args_, conststring32 _sendingString_, \
			Interpreter interpreter, conststring32 _invokingButtonTitle_, bool _isModified_, void *_buttonClosure_, Editor _optionalEditor_) \
	{ \
		[[maybe_unused]] integer IOBJECT = 0; \
		static autoUiForm _dia_; \
		if (_dia_) \
			goto _dia_inited_; \
		_dia_ = UiForm_create (theCurrentPraatApplication -> topShell, _optionalEditor_, name, \
				proc, _buttonClosure_, _invokingButtonTitle_, helpTitle);

#define REAL(realVariable, labelText, defaultStringValue)  \
		static double realVariable; \
		UiForm_addReal (_dia_.get(), & realVariable, U"" #realVariable, labelText, defaultStringValue);

#define REAL_OR_UNDEFINED(realVariable, labelText, defaultStringValue)  \
		static double realVariable; \
		UiForm_addRealOrUndefined (_dia_.get(), & realVariable, U"" #realVariable, labelText, defaultStringValue);

#define POSITIVE(realVariable, labelText, defaultStringValue)  \
		static double realVariable; \
		UiForm_addPositive (_dia_.get(), & realVariable, U"" #realVariable, labelText, defaultStringValue);

#define INTEGER(integerVariable, labelText, defaultStringValue)  \
		static integer integerVariable; \
		UiForm_addInteger (_dia_.get(), & integerVariable, U"" #integerVariable, labelText, defaultStringValue);

#define NATURAL(integerVariable, labelText, defaultStringValue)  \
		static integer integerVariable; \
		UiForm_addNatural (_dia_.get(), & integerVariable, U"" #integerVariable, labelText, defaultStringValue);

#define WORD(stringVariable, labelText, defaultStringValue)  \
		static conststring32 stringVariable; \
		UiForm_addWord (_dia_.get(), & stringVariable, U"" #stringVariable, labelText, defaultStringValue);

#define SENTENCE(stringVariable, labelText, defaultStringValue)  \
		static conststring32 stringVariable; \
		UiForm_addSentence (_dia_.get(), & stringVariable, U"" #stringVariable, labelText, defaultStringValue);

#define BOOLEAN(booleanVariable, labelText, defaultBooleanValue)  \
		static bool booleanVariable; \
		UiForm_addBoolean (_dia_.get(), & booleanVariable, U"" #booleanVariable, labelText, defaultBooleanValue);

#define HEADING(labelText)  UiForm_addHeading (_dia_.get(), nullptr, labelText);

#define COMMENT(labelText)  UiForm_addComment (_dia_.get(), nullptr, labelText);

#define CAPTION(labelText)  UiForm_addCaption (_dia_.get(), nullptr, labelText);

#define MUTABLE_COMMENT(stringVariable, labelText)  \
		static conststring32 stringVariable; \
		UiForm_addComment (_dia_.get(), & stringVariable, labelText);

#define TEXTFIELD(stringVariable, labelText, defaultStringValue, numberOfLines)  \
		static conststring32 stringVariable; \
		UiForm_addText (_dia_.get(), & stringVariable, U"" #stringVariable, labelText, defaultStringValue, numberOfLines);

#define FORMULA(stringVariable, labelText, defaultStringValue)  \
		static conststring32 stringVariable; \
		UiForm_addFormula (_dia_.get(), & stringVariable, U"" #stringVariable, labelText, defaultStringValue);

#define INFILE(stringVariable, labelText, defaultStringValue)  \
		static conststring32 stringVariable; \
		UiForm_addInfile (_dia_.get(), & stringVariable, U"" #stringVariable, labelText, defaultStringValue);

#define OUTFILE(stringVariable, labelText, defaultStringValue)  \
		static conststring32 stringVariable; \
		UiForm_addOutfile (_dia_.get(), & stringVariable, U"" #stringVariable, labelText, defaultStringValue);

#define FOLDER(stringVariable, labelText, defaultStringValue)  \
		static conststring32 stringVariable; \
		UiForm_addFolder (_dia_.get(), & stringVariable, U"" #stringVariable, labelText, defaultStringValue);

#define REALVECTOR(realVectorVariable, labelText, defaultFormat, defaultStringValue)  \
		static constVEC realVectorVariable; \
		UiForm_addRealVector (_dia_.get(), & realVectorVariable, U"" #realVectorVariable, \
				labelText, kUi_realVectorFormat::defaultFormat, defaultStringValue);

#define POSITIVEVECTOR(realVectorVariable, labelText, defaultFormat, defaultStringValue)  \
		static constVEC realVectorVariable; \
		UiForm_addPositiveVector (_dia_.get(), & realVectorVariable, U"" #realVectorVariable, \
				labelText, kUi_realVectorFormat::defaultFormat, defaultStringValue);

#define INTEGERVECTOR(integerVectorVariable, labelText, defaultFormat, defaultStringValue)  \
		static constINTVEC integerVectorVariable; \
		UiForm_addIntegerVector (_dia_.get(), & integerVectorVariable, U"" #integerVectorVariable, \
				labelText, kUi_integerVectorFormat::defaultFormat, defaultStringValue);

#define NATURALVECTOR(integerVectorVariable, labelText, defaultFormat, defaultStringValue)  \
		static constINTVEC integerVectorVariable; \
		UiForm_addNaturalVector (_dia_.get(), & integerVectorVariable, U"" #integerVectorVariable, \
				labelText, kUi_integerVectorFormat::defaultFormat, defaultStringValue);

#define REALMATRIX(numericMatrixVariable, labelText, defaultNumericMatrixValue)  \
		static constMAT numericMatrixVariable; \
		UiForm_addRealMatrix (_dia_.get(), & numericMatrixVariable, U"" #numericMatrixVariable, \
				labelText, defaultNumericMatrixValue.get());

#define STRINGARRAY(stringArrayVariable, labelText, ...)  \
		static constSTRVEC stringArrayVariable; \
		{ \
			static const conststring32 _defaultStringArrayValue [] = __VA_ARGS__; \
			UiForm_addStringArray (_dia_.get(), & stringArrayVariable, U"" #stringArrayVariable, \
					labelText, ARRAY_TO_STRVEC (_defaultStringArrayValue)); \
		}

#define STRINGARRAY_LINES(numberOfLines, stringArrayVariable, labelText, ...)  \
		static constSTRVEC stringArrayVariable; \
		{ \
			static const conststring32 _defaultStringArrayValue [] = __VA_ARGS__; \
			UiForm_addStringArray (_dia_.get(), & stringArrayVariable, U"" #stringArrayVariable, \
					labelText, ARRAY_TO_STRVEC (_defaultStringArrayValue), numberOfLines); \
		}

#define CHOICE(intVariable, labelText, defaultOptionNumber)  \
		static int intVariable; \
		UiForm_addChoice (_dia_.get(), & intVariable, nullptr, U"" #intVariable, \
				labelText, defaultOptionNumber, 1);

#define CHOICEx(intVariable, labelText, defaultOptionNumber, base)  \
		static int intVariable; \
		UiForm_addChoice (_dia_.get(), & intVariable, nullptr, U"" #intVariable, \
				labelText, defaultOptionNumber, base);

#define CHOICESTR(stringVariable, labelText, defaultOptionNumber)  \
		static conststring32 stringVariable; \
		UiForm_addChoice (_dia_.get(), nullptr, & stringVariable, U"" #stringVariable, \
				labelText, defaultOptionNumber, 1);

#define OPTIONMENU(intVariable, labelText, defaultOptionNumber)  \
		static int intVariable; \
		UiForm_addOptionMenu (_dia_.get(), & intVariable, nullptr, U"" #intVariable, \
				labelText, defaultOptionNumber, 1);

#define OPTIONMENUx(intVariable, labelText, defaultOptionNumber, base)  \
		static int intVariable; \
		UiForm_addOptionMenu (_dia_.get(), & intVariable, nullptr, U"" #intVariable, \
				labelText, defaultOptionNumber, base);

#define OPTIONMENUSTR(stringVariable, labelText, defaultOptionNumber)  \
		static conststring32 stringVariable; \
		UiForm_addOptionMenu (_dia_.get(), nullptr, & stringVariable, U"" #stringVariable, \
				labelText, defaultOptionNumber, 1);

#define OPTION(optionText)  \
		UiForm_addOption (_dia_.get(), optionText);

#define CHOICE_ENUM(EnumeratedType, enumeratedVariable, labelText, defaultValue)  \
		static enum EnumeratedType enumeratedVariable; \
		{/* type checks */ \
			[[maybe_unused]] enum EnumeratedType _compilerTypeCheckDummy = defaultValue; \
			_compilerTypeCheckDummy = enumeratedVariable; \
		} \
		UiForm_addChoiceEnum (_dia_.get(), (int *) & enumeratedVariable, nullptr, U"" #enumeratedVariable, \
			labelText, (int) defaultValue - (int) EnumeratedType::MIN + 1, (int) EnumeratedType::MIN, \
			(enum_generic_getValue) EnumeratedType##_getValue \
		); \
		for (int ienum = (int) EnumeratedType::MIN; ienum <= (int) EnumeratedType::MAX; ienum ++) \
			UiForm_addOption (_dia_.get(), EnumeratedType##_getText ((enum EnumeratedType) ienum));

#define OPTIONMENU_ENUM(EnumeratedType, enumeratedVariable, labelText, defaultValue)  \
		static EnumeratedType enumeratedVariable; \
		{/* type checks */ \
			[[maybe_unused]] enum EnumeratedType _compilerTypeCheckDummy = defaultValue; \
			_compilerTypeCheckDummy = enumeratedVariable; \
		} \
		UiForm_addOptionMenuEnum (_dia_.get(), (int *) & enumeratedVariable, nullptr, U"" #enumeratedVariable, \
			labelText, (int) defaultValue - (int) EnumeratedType::MIN + 1, (int) EnumeratedType::MIN, \
			(enum_generic_getValue) EnumeratedType##_getValue \
		); \
		for (int ienum = (int) EnumeratedType::MIN; ienum <= (int) EnumeratedType::MAX; ienum ++) \
			UiForm_addOption (_dia_.get(), EnumeratedType##_getText ((enum EnumeratedType) ienum));

#define OPTIONMENU_ENUMSTR(EnumeratedType, enumeratedVariableAsString, labelText, defaultValue)  \
		static char32 *enumeratedVariableAsString; \
		{/* type checks */ \
			[[maybe_unused]] enum EnumeratedType _compilerTypeCheckDummy = defaultValue; \
			_compilerTypeCheckDummy = enumeratedVariable; \
		} \
		UiForm_addOptionMenuEnum (_dia_.get(), nullptr, & enumeratedVariableAsString, U"" #enumeratedVariableAsString, \
			labelText, (int) defaultValue - (int) EnumeratedType::MIN + 1, (int) EnumeratedType::MIN, \
			(enum_generic_getValue) EnumeratedType##_getValue \
		); \
		for (int ienum = (int) EnumeratedType::MIN; ienum <= (int) EnumeratedType::MAX; ienum ++) \
			UiForm_addOption (_dia_.get(), EnumeratedType##_getText ((enum EnumeratedType) ienum));

/*
	LIST will throw if the supplied string is not in the list.
*/
#define LIST(integerVariable, labelText, strings, defaultOptionNumber)  \
		static integer integerVariable; \
		UiForm_addList (_dia_.get(), & integerVariable, nullptr, U"" #integerVariable, \
				labelText, strings, defaultOptionNumber);

/*
	LISTSTR will throw if the supplied string is not in the list.
*/
#define LISTSTR(stringVariable, labelText, strings, defaultOptionNumber)  \
		static conststring32 stringVariable; \
		UiForm_addList (_dia_.get(), nullptr, & stringVariable, U"" #stringVariable, \
				labelText, strings, defaultOptionNumber);

/*
	If the supplied string is not in the list, LISTNUMSTR will return 0 as well as the supplied string, for later handling.
*/
#define LISTNUMSTR(integerVariable, stringVariable, labelText, strings, defaultOptionNumber)  \
		static integer integerVariable; \
		static conststring32 stringVariable; \
		UiForm_addList (_dia_.get(), & integerVariable, & stringVariable, U"" #stringVariable, \
				labelText, strings, defaultOptionNumber);

#define FILE_IN(labelText)  \
		UiForm_addFileIn (_dia_.get(), labelText);

#define FILE_OUT(labelText, defaultStringValue)  \
		UiForm_addFileOut (_dia_.get(), labelText, defaultStringValue);

#define COLOUR(colourVariable, labelText, defaultStringValue)  \
		static MelderColour colourVariable; \
		UiForm_addColour (_dia_.get(), & colourVariable, U"" #colourVariable, labelText, defaultStringValue);

#define CHANNEL(integerVariable, labelText, defaultStringValue)  \
		static integer integerVariable; \
		UiForm_addChannel (_dia_.get(), & integerVariable, U"" #integerVariable, labelText, defaultStringValue);

#define OK  \
		UiForm_finish (_dia_.get()); \
	_dia_inited_: \
		if (_narg_ < 0) UiForm_info (_dia_.get(), _narg_); else if (! _args_ && ! _sendingForm_ && ! _sendingString_) {

#define SET_REAL(realVariable, realValue)  \
			UiForm_setReal (_dia_.get(), & realVariable, realValue);

#define SET_INTEGER(integerVariable, integerValue)  \
			UiForm_setInteger (_dia_.get(), & integerVariable, integerValue);

#define SET_BOOLEAN(booleanVariable, booleanValue)  \
			UiForm_setBoolean (_dia_.get(), & booleanVariable, booleanValue);

#define SET_OPTION(optionVariable, optionValue)  \
			UiForm_setOption (_dia_.get(), & optionVariable, optionValue);

#define SET_STRING(stringVariable, stringValue)  \
			UiForm_setString (_dia_.get(), & stringVariable, stringValue);

#define SET_ENUM(enumeratedVariable, EnumeratedType, enumeratedValue)  \
			enumeratedVariable = enumeratedValue; /* just for typechecking */ \
			UiForm_setOption (_dia_.get(), (int *) & enumeratedVariable, (int) enumeratedValue - (int) EnumeratedType::MIN + 1);

#define DO  \
			UiForm_do (_dia_.get(), _isModified_); \
		} else if (! _sendingForm_) { \
			trace (U"args ", Melder_pointer (_args_)); \
			if (_args_) { \
				UiForm_call (_dia_.get(), _narg_, _args_, interpreter); \
			} else { \
				UiForm_parseString (_dia_.get(), _sendingString_, interpreter); \
			} \
		} else { \
			try { \
				{

#define DO_ALTERNATIVE(alternative)  \
			UiForm_do (_dia_.get(), _isModified_); \
		} else if (! _sendingForm_) { \
			trace (U"alternative args ", Melder_pointer (_args_)); \
			try { \
				if (_args_) { \
					UiForm_call (_dia_.get(), _narg_, _args_, interpreter); \
				} else { \
					UiForm_parseString (_dia_.get(), _sendingString_, interpreter); \
				} \
			} catch (MelderError) { \
				if (Melder_hasCrash ()) \
					throw; \
				autostring32 _parkedError = Melder_dup_f (Melder_getError ()); \
				Melder_clearError (); \
				try { \
					alternative (nullptr, _narg_, _args_, _sendingString_, interpreter, _invokingButtonTitle_, _isModified_, _buttonClosure_, _optionalEditor_); \
				} catch (MelderError) { \
					Melder_clearError (); \
					Melder_appendError (_parkedError.get()); \
					throw; \
				} \
			} \
		} else { \
			try { \
				{

#define END_WITH_NEW_DATA  \
				} \
			} catch (MelderError) { \
				praat_updateSelection (); \
				throw; \
			} \
			praat_updateSelection (); \
		} \
	}

#define END_NO_NEW_DATA  \
				} \
			} catch (MelderError) { \
				throw; \
			} \
		} \
	}

#define DIRECT(proc)  \
	extern "C" void proc (UiForm, integer, Stackel, conststring32, Interpreter interpreter, conststring32, bool, void *, Editor); \
	void proc (UiForm, integer, Stackel, conststring32, [[maybe_unused]] Interpreter interpreter, conststring32, bool, void *, Editor) { \
		[[maybe_unused]] integer IOBJECT = 0; \
		{ { \
			try {

#define FORM_READ(proc,title,help,allowMult)  \
	extern "C" void proc (UiForm sendingForm, integer, structStackel args [], conststring32 sendingString, \
			Interpreter interpreter, conststring32 invokingButtonTitle, bool, void *okClosure, Editor optionalEditor); \
	void proc (UiForm _sendingForm_, integer _narg_, structStackel _args_ [], conststring32 _sendingString_, \
			Interpreter interpreter, conststring32 _invokingButtonTitle_, bool, void *_okClosure_, Editor _optionalEditor_) \
	{ \
		{ static autoUiForm _dia_; \
		if (! _dia_) \
			_dia_ = UiInfile_create (theCurrentPraatApplication -> topShell, _optionalEditor_, title, proc, _okClosure_, _invokingButtonTitle_, help, allowMult); \
		if (_narg_ < 0) UiForm_info (_dia_.get(), _narg_); else if (! _args_ && ! _sendingForm_ && ! _sendingString_) { \
			UiInfile_do (_dia_.get()); \
		} else { \
			try { \
				MelderFile file; \
				[[maybe_unused]] integer IOBJECT = 0; \
				structMelderFile _file2 { };  /* don't move this into an inner scope, because the contents of a local variable don't persist into the outer scope */ \
				if (_args_) { \
					Melder_require (_narg_ == 1, \
						U"Command requires exactly 1 argument, the name of the file to read, instead of the given ", _narg_, U" arguments."); \
					Melder_require (_args_ [1]. which == Stackel_STRING, \
						U"The file name argument should be a string, not ", _args_ [1]. whichText(), U"."); \
					Melder_relativePathToFile (_args_ [1]. getString(), & _file2); \
					file = & _file2; \
				} else if (_sendingString_) { \
					Melder_relativePathToFile (_sendingString_, & _file2); \
					file = & _file2; \
				} else { \
					file = UiFile_getFile (_dia_.get()); \
				}

#define FORM_SAVE(proc,title,help,ext)  \
	extern "C" void proc (UiForm sendingForm, integer, structStackel args [], conststring32 sendingString, \
			Interpreter interpreter, conststring32 invokingButtonTitle, bool, void *okClosure, Editor optionalEditor); \
	void proc (UiForm _sendingForm_, integer _narg_, Stackel _args_, conststring32 _sendingString_, \
			[[maybe_unused]] Interpreter _interpreter_, conststring32 _invokingButtonTitle_, bool, void *_okClosure_, Editor _optionalEditor_) \
	{ \
		{ static autoUiForm _dia_; \
		if (! _dia_) \
			_dia_ = UiOutfile_create (theCurrentPraatApplication -> topShell, _optionalEditor_, title, proc, _okClosure_, _invokingButtonTitle_, help); \
		if (_narg_ < 0) UiForm_info (_dia_.get(), _narg_); else if (! _args_ && ! _sendingForm_ && ! _sendingString_) { \
			praat_write_do (_dia_.get(), ext); \
		} else { \
			try { \
				MelderFile file; \
				[[maybe_unused]] integer IOBJECT = 0; \
				structMelderFile _file2 { };  /* don't move this into an inner scope, because the contents of a local variable don't persist into the outer scope */ \
				if (_args_) { \
					Melder_require (_narg_ == 1, \
						U"Command requires exactly 1 argument, the name of the file to write, instead of the given ", _narg_, U" arguments."); \
					Melder_require (_args_ [1]. which == Stackel_STRING, \
						U"The file name argument should be a string, not ", _args_ [1]. whichText(), U"."); \
					Melder_relativePathToFile (_args_ [1]. getString(), & _file2); \
					file = & _file2; \
				} else if (_sendingString_) { \
					Melder_relativePathToFile (_sendingString_, & _file2); \
					file = & _file2; \
				} else { \
					file = UiFile_getFile (_dia_.get()); \
				}

#define CREATE_MULTIPLE
#define CREATE_MULTIPLE_END  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::OBJECT_; \
	END_WITH_NEW_DATA

#define CREATE_ONE
#define CREATE_ONE_END(...)  \
	praat_new (result.move(), __VA_ARGS__); \
	CREATE_MULTIPLE_END

#define FIND_ONE(klas)  \
	klas me = nullptr; \
	LOOP { if (CLASS == class##klas || Thing_isSubclass (CLASS, class##klas)) me = (klas) OBJECT; break; }

#define FIND_ONE_WITH_IOBJECT(klas)  \
	klas me = nullptr; integer _klas_position = 0; \
	LOOP { if (CLASS == class##klas) me = (klas) OBJECT, _klas_position = IOBJECT; break; } \
	IOBJECT = _klas_position;

#define FIND_ONE_AND_ONE(klas1,klas2)  \
	klas1 me = nullptr; klas2 you = nullptr; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT; else if (CLASS == class##klas2) you = (klas2) OBJECT; \
	if (me && you) break; }

#define FIND_ONE_AND_ONE_WITH_IOBJECT(klas1,klas2)  \
	klas1 me = nullptr; klas2 you = nullptr; integer _klas1_position = 0; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT, _klas1_position = IOBJECT; \
		else if (CLASS == class##klas2) you = (klas2) OBJECT; \
		if (me && you) break; } \
	IOBJECT = _klas1_position;

#define FIND_TWO(klas)  \
	klas me = nullptr, you = nullptr; \
	LOOP if (CLASS == class##klas || Thing_isSubclass (CLASS, class##klas)) (me ? you : me) = (klas) OBJECT;

#define FIND_TWO_AND_ONE(klas1,klas2)  \
	klas1 me = nullptr, you = nullptr; klas2 him = nullptr; \
	LOOP { if (CLASS == class##klas1) (me ? you : me) = (klas1) OBJECT; else if (CLASS == class##klas2) him = (klas2) OBJECT; \
	if (me && you && him) break; }

#define FIND_ONE_AND_TWO(klas1,klas2)  \
	klas1 me = nullptr; klas2 you = nullptr, him = nullptr; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT; else if (CLASS == class##klas2) (you ? him : you) = (klas2) OBJECT; \
	if (me && you && him) break; }

#define FIND_ONE_AND_GENERIC(klas1,klas2)  \
	klas1 me = nullptr; klas2 you = nullptr; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT; \
	else if (Thing_isSubclass (CLASS, class##klas2)) { you = (klas2) OBJECT; } } \
	Melder_assert (me && you);

#define FIND_ONE_AND_ONE_AND_ONE(klas1,klas2,klas3)  \
	klas1 me = nullptr; klas2 you = nullptr; klas3 him = nullptr; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT; else if (CLASS == class##klas2) you = (klas2) OBJECT; \
	else if (CLASS == class##klas3) him = (klas3) OBJECT; \
	if (me && you && him) break; }
	
#define FIND_ONE_AND_ONE_AND_ONE_WITH_IOBJECT(klas1,klas2,klas3)  \
	klas1 me = nullptr; klas2 you = nullptr; klas3 him = nullptr; integer _klas1_position = 0;\
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT, _klas1_position = IOBJECT; \
		else if (CLASS == class##klas2) you = (klas2) OBJECT; \
		else if (CLASS == class##klas3) him = (klas3) OBJECT; \
		if (me && you && him) break; } \
	IOBJECT = _klas1_position;

#define FIND_1_1_1_1(klas1,klas2,klas3,klas4)  \
	klas1 me = nullptr; klas2 you = nullptr; klas3 him = nullptr; klas4 she = nullptr; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT; else if (CLASS == class##klas2) you = (klas2) OBJECT; \
	else if (CLASS == class##klas3) him = (klas3) OBJECT; else if (CLASS == class##klas4) she = (klas4) OBJECT; \
	if (me && you && him && she) break; }

#define FIND_1_1_1_1_WITH_IOBJECT(klas1,klas2,klas3,klas4)  \
	klas1 me = nullptr; klas2 you = nullptr; klas3 him = nullptr; klas4 she = nullptr;  integer _klas1_position = 0; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT, _klas1_position = IOBJECT; else if (CLASS == class##klas2) you = (klas2) OBJECT; \
	else if (CLASS == class##klas3) him = (klas3) OBJECT; else if (CLASS == class##klas4) she = (klas4) OBJECT; \
	if (me && you && him && she) break; } \
	IOBJECT = _klas1_position;

#define FIND_ALL(klas)  \
	OrderedOf<struct##klas> list; \
	LOOP { iam_LOOP (klas); list. addItem_ref (me); }
	
#define FIND_ALL_LISTED(klas,listClass)  \
	auto##listClass list = listClass##_create (); \
	LOOP { iam_LOOP (klas); list -> addItem_ref (me); }
	
#define FIND_ONE_AND_ALL(klas1,klas2)  \
	OrderedOf<struct##klas2> list; klas1 me = nullptr; \
	LOOP { if (CLASS == class##klas2) list. addItem_ref ((klas2) OBJECT); else if (CLASS == class##klas1) me = (klas1) OBJECT; }

#define FIND_ONE_AND_ALL_LISTED(klas1,klas2,listClass)  \
	auto##listClass list = listClass##_create (); klas1 me = nullptr; \
	LOOP { if (CLASS == class##klas2) list -> addItem_ref ((klas2) OBJECT); else if (CLASS == class##klas1) me = (klas1) OBJECT; }

#define FIND_ONE_AND_ONE_AND_ALL(klas1,klas2,klas3)  \
	OrderedOf<struct##klas3> list; klas1 me = nullptr; klas2 you = nullptr; \
	LOOP { if (CLASS == class##klas3) list. addItem_ref ((klas3) OBJECT); else if (CLASS == class##klas1) me = (klas1) OBJECT; \
	else if (CLASS == class##klas2) you = (klas2) OBJECT; }

/*
	INFO
*/

#define INFO_NONE
#define INFO_NONE_END  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::STRING_; \
	END_NO_NEW_DATA

#define INFO_ONE(klas)  FIND_ONE (klas)
#define INFO_ONE_END  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::STRING_; \
	END_NO_NEW_DATA

#define INFO_ONE_AND_ONE(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2)
#define INFO_ONE_AND_ONE_END  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::STRING_; \
	END_NO_NEW_DATA

#define INFO_TWO(klas)  \
	FIND_TWO (klas)
#define INFO_TWO_END  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::STRING_; \
	END_NO_NEW_DATA

#define INFO_ONE_AND_ONE_AND_ONE(klas1,klas2,klas3)  \
	FIND_ONE_AND_ONE_AND_ONE (klas1, klas2, klas3)
#define INFO_ONE_AND_ONE_AND_ONE_END  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::STRING_; \
	END_NO_NEW_DATA

#define HELP(page)  \
	Melder_help (page); \
	END_NO_NEW_DATA

/*
	Parts.
*/

#define EACH_BEGIN__(klas)  \
	LOOP { \
		iam_LOOP (klas);
#define EACH_END__  \
	} \

#define FIRST_WEAK_BEGIN  \
	try {
#define FIRST_WEAK_END  \
	} catch (MelderError) { \
		praat_dataChanged (me); \
		throw; \
	} \
	praat_dataChanged (me);

/*
	PLAY
*/

#define PLAY_END__  \
	END_NO_NEW_DATA

#define PLAY_NONE
#define PLAY_NONE_END  \
	PLAY_END__

#define PLAY_EACH(klas)  \
	int _numberOfSelectedSounds = 0; \
	LOOP { \
		_numberOfSelectedSounds ++; \
	} \
	if (_numberOfSelectedSounds > 1) \
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel::INTERRUPTABLE); \
	EACH_BEGIN__ (klas)
#define PLAY_EACH_END  \
	EACH_END__ \
	if (_numberOfSelectedSounds > 1) \
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel::ASYNCHRONOUS); \
	PLAY_END__

#define PLAY_ONE(klas)  \
	FIND_ONE (klas)
#define PLAY_ONE_END  \
	PLAY_END__

#define PLAY_ONE_AND_ONE(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2)
#define PLAY_ONE_AND_ONE_END  \
	PLAY_END__

/*
	RECORD
*/

#define RECORD_ONE
#define RECORD_ONE_END(...)  \
	praat_new (result.move(), __VA_ARGS__); \
	CREATE_MULTIPLE_END

/*
	DRAW
*/

#define GRAPHICS_BEGIN__  \
	autoPraatPictureOpen picture;
#define GRAPHICS_END__  \
	END_NO_NEW_DATA

#define GRAPHICS_NONE  \
	GRAPHICS_BEGIN__
#define GRAPHICS_NONE_END  \
	GRAPHICS_END__

#define GRAPHICS_EACH(klas)  \
	GRAPHICS_BEGIN__ \
	EACH_BEGIN__ (klas)
#define GRAPHICS_EACH_END  \
	EACH_END__ \
	GRAPHICS_END__

#define GRAPHICS_ONE_AND_ONE(klas1,klas2)  \
	GRAPHICS_BEGIN__ \
	FIND_ONE_AND_ONE (klas1, klas2)
#define GRAPHICS_ONE_AND_ONE_END  \
	GRAPHICS_END__

#define GRAPHICS_TWO(klas)  \
	GRAPHICS_BEGIN__ \
	FIND_TWO (klas)
#define GRAPHICS_TWO_END  \
	GRAPHICS_END__

#define GRAPHICS_TWO_AND_ONE(klas1,klas2)  \
	GRAPHICS_BEGIN__ \
	FIND_TWO_AND_ONE (klas1, klas2)
#define GRAPHICS_TWO_AND_ONE_END  \
	GRAPHICS_END__

#define MOVIE_ONE(klas,title,width,height)  \
	Graphics graphics = Movie_create (title, width, height); \
	FIND_ONE (klas)
#define MOVIE_ONE_END  END_NO_NEW_DATA

#define MOVIE_ONE_AND_ONE(klas1,klas2,title,width,height)  \
	Graphics graphics = Movie_create (title, width, height); \
	FIND_ONE_AND_ONE (klas1, klas2)
#define MOVIE_ONE_AND_ONE_END  END_NO_NEW_DATA

#define MOVIE_ONE_AND_ONE_AND_ONE(klas1,klas2,klas3,title,width,height)  \
	Graphics graphics = Movie_create (title, width, height); \
	FIND_ONE_AND_ONE_AND_ONE (klas1, klas2, klas3)
#define MOVIE_ONE_AND_ONE_AND_ONE_END  END_NO_NEW_DATA

/*
	QUERY
*/

#define FOR_REAL__(...)  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::REAL_; \
	Melder_information (result, __VA_ARGS__);

#define FOR_INTEGER__(...)  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::INTEGER_; \
	Melder_information (double (result), __VA_ARGS__);

#define FOR_BOOLEAN__(...)  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::BOOLEAN_; \
	Melder_information (double (result), __VA_ARGS__);

#define FOR_COMPLEX__(...)  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::STRING_; /* TODO: make true complex types in script */ \
	Melder_information (result, __VA_ARGS__);

#define FOR_STRING__  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::STRING_; \
	Melder_information (result);

#define FOR_AUTOSTRING__  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::STRING_; \
	Melder_information (result.get());

#define QUERY_END__  \
	END_NO_NEW_DATA

#define QUERY_FOR_REAL_END__(...)  \
	FOR_REAL__ (__VA_ARGS__) \
	QUERY_END__

#define QUERY_WEAK_FOR_REAL_END__(...)  \
	FOR_REAL__ (__VA_ARGS__) \
	FIRST_WEAK_END \
	QUERY_END__

#define QUERY_FOR_INTEGER_END__(...)  \
	FOR_INTEGER__ (__VA_ARGS__) \
	QUERY_END__

#define QUERY_WEAK_FOR_INTEGER_END__(...)  \
	FOR_INTEGER__ (__VA_ARGS__) \
	FIRST_WEAK_END \
	QUERY_END__

#define QUERY_FOR_BOOLEAN_END__(...)  \
	FOR_BOOLEAN__ (__VA_ARGS__) \
	QUERY_END__

#define QUERY_WEAK_FOR_BOOLEAN_END__(...)  \
	FOR_BOOLEAN__ (__VA_ARGS__) \
	FIRST_WEAK_END \
	QUERY_END__

#define QUERY_FOR_COMPLEX_END__(...)  \
	FOR_COMPLEX__ (__VA_ARGS__) \
	QUERY_END__

#define QUERY_WEAK_FOR_COMPLEX_END__(...)  \
	FOR_COMPLEX__ (__VA_ARGS__) \
	FIRST_WEAK_END \
	QUERY_END__

#define QUERY_FOR_STRING_END__  \
	FOR_STRING__ \
	QUERY_END__

#define QUERY_FOR_AUTOSTRING_END__  \
	FOR_AUTOSTRING__ \
	QUERY_END__

#define QUERY_WEAK_FOR_STRING_END__  \
	FOR_STRING__ \
	FIRST_WEAK_END \
	QUERY_END__

#define QUERY_GRAPHICS_FOR_REAL
#define QUERY_GRAPHICS_FOR_REAL_END(...)  \
	QUERY_FOR_REAL_END__ (__VA_ARGS__)

#define QUERY_ONE_FOR_REAL(klas)  \
	FIND_ONE (klas)
#define QUERY_ONE_FOR_REAL_END(...)  \
	QUERY_FOR_REAL_END__ (__VA_ARGS__)

#define QUERY_ONE_WEAK_FOR_REAL(klas)  \
	FIND_ONE (klas) \
	FIRST_WEAK_BEGIN
#define QUERY_ONE_WEAK_FOR_REAL_END(...)  \
	QUERY_WEAK_FOR_REAL_END__ (__VA_ARGS__)

#define QUERY_ONE_FOR_INTEGER(klas)  \
	FIND_ONE (klas)
#define QUERY_ONE_FOR_INTEGER_END(...)  \
	QUERY_FOR_INTEGER_END__ (__VA_ARGS__)

#define QUERY_ONE_WEAK_FOR_INTEGER(klas)  \
	FIND_ONE (klas) \
	FIRST_WEAK_BEGIN
#define QUERY_ONE_WEAK_FOR_INTEGER_END(...)  \
	QUERY_WEAK_FOR_INTEGER_END__ (__VA_ARGS__)

#define QUERY_ONE_FOR_BOOLEAN(klas)  \
	FIND_ONE (klas)
#define QUERY_ONE_FOR_BOOLEAN_END(...)  \
	QUERY_FOR_BOOLEAN_END__ (__VA_ARGS__)

#define QUERY_ONE_WEAK_FOR_BOOLEAN(klas)  \
	FIND_ONE (klas) \
	FIRST_WEAK_BEGIN
#define QUERY_ONE_WEAK_FOR_BOOLEAN_END(...)  \
	QUERY_WEAK_FOR_BOOLEAN_END__ (__VA_ARGS__)

#define QUERY_ONE_AND_ONE_FOR_REAL(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2)
#define QUERY_ONE_AND_ONE_FOR_REAL_END(...)  \
	QUERY_FOR_REAL_END__ (__VA_ARGS__)

#define QUERY_ONE_WEAK_AND_ONE_FOR_REAL(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2) \
	FIRST_WEAK_BEGIN
#define QUERY_ONE_WEAK_AND_ONE_FOR_REAL_END(...)  \
	QUERY_WEAK_FOR_REAL_END__ (__VA_ARGS__)

#define QUERY_ONE_AND_ONE_FOR_INTEGER(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2)
#define QUERY_ONE_AND_ONE_FOR_INTEGER_END(...)  \
	QUERY_FOR_INTEGER_END__ (__VA_ARGS__)

#define QUERY_ONE_WEAK_AND_ONE_FOR_INTEGER(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2) \
	FIRST_WEAK_BEGIN
#define QUERY_ONE_WEAK_AND_ONE_FOR_INTEGER_END(...)  \
	QUERY_WEAK_FOR_INTEGER_END__ (__VA_ARGS__)

#define QUERY_ONE_AND_ONE_FOR_BOOLEAN(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2)
#define QUERY_ONE_AND_ONE_FOR_BOOLEAN_END(...)  \
	QUERY_FOR_BOOLEAN_END__ (__VA_ARGS__)

#define QUERY_ONE_WEAK_AND_ONE_FOR_BOOLEAN(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2) \
	FIRST_WEAK_BEGIN
#define QUERY_ONE_WEAK_AND_ONE_FOR_BOOLEAN_END(...)  \
	QUERY_WEAK_FOR_BOOLEAN_END__ (__VA_ARGS__)

#define QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL(klas1,klas2,klas3)  \
	FIND_ONE_AND_ONE_AND_ONE (klas1, klas2, klas3)
#define QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL_END(...)  \
	QUERY_FOR_REAL_END__ (__VA_ARGS__)

#define QUERY_TWO_FOR_REAL(klas)  \
	FIND_TWO (klas)
#define QUERY_TWO_FOR_REAL_END(...)  \
	QUERY_FOR_REAL_END__ (__VA_ARGS__)

#define QUERY_TWO_AND_ONE_FOR_REAL(klas1,klas2)  \
	FIND_TWO_AND_ONE (klas1, klas2)
#define QUERY_TWO_AND_ONE_FOR_REAL_END(...)  \
	QUERY_FOR_REAL_END__ (__VA_ARGS__)

#define QUERY_ONE_AND_ALL_FOR_REAL(klas1,klas2)  \
	FIND_ONE_AND_ALL (klas1, klas2)
#define QUERY_ONE_AND_ALL_FOR_REAL_END(...)  \
	QUERY_FOR_REAL_END__ (__VA_ARGS__)

#define QUERY_ONE_AND_ONE_AND_ALL_FOR_REAL(klas1,klas2,klas3)  \
	FIND_ONE_AND_ONE_AND_ALL (klas1, klas2, klas3)
#define QUERY_ONE_AND_ONE_AND_ALL_FOR_REAL_END(...)  \
	QUERY_FOR_REAL_END__ (__VA_ARGS__)

#define QUERY_NONE_FOR_COMPLEX
#define QUERY_NONE_FOR_COMPLEX_END(...)  \
	QUERY_FOR_COMPLEX_END__ (__VA_ARGS__)

#define QUERY_NONE_FOR_REAL
#define QUERY_NONE_FOR_REAL_END(...)  \
	QUERY_FOR_REAL_END__ (__VA_ARGS__)

#define QUERY_ONE_FOR_COMPLEX(klas)  \
	FIND_ONE (klas)
#define QUERY_ONE_FOR_COMPLEX_END(...)  \
	QUERY_FOR_COMPLEX_END__ (__VA_ARGS__)

#define QUERY_ONE_FOR_STRING(klas)  \
	FIND_ONE (klas)
#define QUERY_ONE_FOR_STRING_END  \
	QUERY_FOR_STRING_END__

#define QUERY_ONE_FOR_AUTOSTRING(klas)  \
	FIND_ONE (klas)
#define QUERY_ONE_FOR_AUTOSTRING_END  \
	QUERY_FOR_AUTOSTRING_END__

#define QUERY_ONE_WEAK_FOR_STRING(klas)  \
	FIND_ONE (klas) \
	FIRST_WEAK_BEGIN
#define QUERY_ONE_WEAK_FOR_STRING_END  \
	QUERY_WEAK_FOR_STRING_END__

#define QUERY_ONE_FOR_REAL_VECTOR(klas)  \
	FIND_ONE (klas)
#define QUERY_ONE_FOR_REAL_VECTOR_END  \
	if (interpreter) { \
		interpreter -> returnType = kInterpreter_ReturnType::REALVECTOR_; \
		interpreter -> returnedRealVector = result.move(); \
	} else \
		Melder_information (constVECVU (result.all())); \
	QUERY_END__

#define QUERY_ONE_FOR_MATRIX(klas)  \
	FIND_ONE (klas)
#define QUERY_ONE_FOR_MATRIX_END  \
	if (interpreter) { \
		interpreter -> returnType = kInterpreter_ReturnType::REALMATRIX_; \
		interpreter -> returnedRealMatrix = result.move(); \
	} else \
		Melder_information (constMATVU (result.all())); \
	QUERY_END__

#define QUERY_ONE_FOR_STRING_ARRAY(klas)  \
	FIND_ONE (klas)
#define QUERY_ONE_FOR_STRING_ARRAY_END  \
	if (interpreter) { \
		interpreter -> returnType = kInterpreter_ReturnType::STRINGARRAY_; \
		interpreter -> returnedStringArray = result.move(); \
	} else \
		Melder_information (constSTRVEC (result.get())); \
	QUERY_END__

/*
	MODIFY
*/

#define MODIFY_END__  \
	END_NO_NEW_DATA

/*
	If there is no exception, the object will change fully.
	STRONG exception guarantee: if there is an exception, the object will not change at all.
	This includes cases in which there cannot be an exception.
	User interface optimization: if there is an exception,
	no praat_dataChanged() message will be sent to the editors.
*/
#define FIRST_STRONG_BEGIN__
#define FIRST_STRONG_END__  \
	praat_dataChanged (me);   /* To be jumped over if there is an exception. */

/*
	If there is no exception, the object will change fully.
	WEAK exception guarantee: if there is an exception, the object may change,
	although it will end up in a sort-of reasonable (at least inspectable) state.
	To stay on the safe side, a praat_dataChanged() message is sent to the editors,
	independently of whether there is an exception.
*/
#define FIRST_WEAK_BEGIN__  \
	try {
#define FIRST_WEAK_END__  \
	} catch (MelderError) { \
		praat_dataChanged (me);   /* Exception: data perhaps partially changed. */ \
		throw; \
	} \
	praat_dataChanged (me);   /* No exception: data fully changed. */

#define MODIFY_EACH(klas)  \
	EACH_BEGIN__ (klas) \
	FIRST_STRONG_BEGIN__
#define MODIFY_EACH_END  \
	FIRST_STRONG_END__ \
	EACH_END__ \
	MODIFY_END__

#define MODIFY_EACH_WEAK(klas)  \
	EACH_BEGIN__ (klas) \
	FIRST_WEAK_BEGIN__
#define MODIFY_EACH_WEAK_END  \
	FIRST_WEAK_END__ \
	EACH_END__ \
	MODIFY_END__

#define MODIFY_ALL(klas)  \
	FIND_ALL (klas) \
	FIRST_STRONG_BEGIN__
#define MODIFY_ALL_END  \
	for (integer i = 1; i <= list.size; i ++) \
		praat_dataChanged (list.at [i]); \
	MODIFY_END__

#define MODIFY_FIRST_OF_ONE_AND_ONE(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2) \
	FIRST_STRONG_BEGIN__
#define MODIFY_FIRST_OF_ONE_AND_ONE_END  \
	FIRST_STRONG_END__ \
	MODIFY_END__

#define MODIFY_FIRST_OF_ONE_WEAK_AND_ONE(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2) \
	FIRST_WEAK_BEGIN__
#define MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_END  \
	FIRST_WEAK_END__ \
	MODIFY_END__

#define MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY(klas1,klas2,historyKlas)  \
	FIND_ONE_AND_ONE (klas1, klas2) \
	auto##historyKlas history; \
	try {
#define MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY_END  \
	} catch (MelderError) { \
		praat_dataChanged (me);   /* e.g. in case of partial learning */ \
		Melder_flushError ();   /* trickle down to save history */ \
	} \
	if (history) \
		praat_new (history.move(), my name.get()); \
	praat_dataChanged (me);   /* No exception: data fully changed. */ \
	MODIFY_END__

#define MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE(klas1,klas2,klas3)  \
	FIND_ONE_AND_ONE_AND_ONE (klas1, klas2, klas3) \
	FIRST_STRONG_BEGIN__
#define MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE_END  \
	FIRST_STRONG_END__ \
	MODIFY_END__

#define MODIFY_FIRST_OF_ONE_AND_TWO(klas1,klas2)  \
	FIND_ONE_AND_TWO (klas1, klas2) \
	FIRST_STRONG_BEGIN__
#define MODIFY_FIRST_OF_ONE_AND_TWO_END  \
	FIRST_STRONG_END__ \
	MODIFY_END__

#define MODIFY_FIRST_OF_ONE_WEAK_AND_TWO(klas1,klas2)  \
	FIND_ONE_AND_TWO (klas1, klas2) \
	FIRST_WEAK_BEGIN__
#define MODIFY_FIRST_OF_ONE_WEAK_AND_TWO_END  \
	FIRST_WEAK_END__ \
	MODIFY_END__

#define MODIFY_FIRST_OF_ONE_AND_ALL(klas1,klas2)  \
	FIND_ONE_AND_ALL (klas1, klas2) \
	FIRST_STRONG_BEGIN__
#define MODIFY_FIRST_OF_ONE_AND_ALL_END  \
	FIRST_STRONG_END__ \
	MODIFY_END__

/*
	CONVERT
*/

#define TO_MULTIPLE__  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::OBJECT_; \

#define TO_ONE__(...)  \
	praat_new (result.move(), __VA_ARGS__); \
	TO_MULTIPLE__

#define CONVERT_END__  \
	END_WITH_NEW_DATA

#define CONVERT_EACH_TO_MULTIPLE(klas)  \
	EACH_BEGIN__ (klas)
#define CONVERT_EACH_TO_MULTIPLE_END  \
	TO_MULTIPLE__ \
	EACH_END__ \
	CONVERT_END__

#define CONVERT_EACH_TO_ONE(klas)  \
	EACH_BEGIN__ (klas)
#define CONVERT_EACH_TO_ONE_END(...)  \
	TO_ONE__ (__VA_ARGS__) \
	EACH_END__ \
	CONVERT_END__

#define CONVERT_EACH_WEAK_TO_ONE(klas)  \
	EACH_BEGIN__ (klas) \
	FIRST_WEAK_BEGIN
#define CONVERT_EACH_WEAK_TO_ONE_END(...)  \
	TO_ONE__ (__VA_ARGS__) \
	FIRST_WEAK_END \
	EACH_END__ \
	CONVERT_END__

#define CONVERT_ONE_TO_MULTIPLE(klas)  \
	FIND_ONE (klas)
#define CONVERT_ONE_TO_MULTIPLE_END  \
	TO_MULTIPLE__ \
	CONVERT_END__

#define COMBINE_ALL_TO_ONE(klas)  \
	FIND_ALL (klas)
#define COMBINE_ALL_TO_ONE_END(...)  \
	TO_ONE__ (__VA_ARGS__) \
	CONVERT_END__

#define COMBINE_ALL_LISTED_TO_ONE(klas,listClass)  \
	FIND_ALL_LISTED (klas,listClass)
#define COMBINE_ALL_LISTED_TO_ONE_END(...)  \
	TO_ONE__ (__VA_ARGS__) \
	CONVERT_END__

#define CONVERT_ALL_TO_MULTIPLE(klas)  \
	FIND_ALL(klas)
#define CONVERT_ALL_TO_MULTIPLE_END \
	TO_MULTIPLE__ \
	CONVERT_END__

#define CONVERT_ALL_LISTED_TO_MULTIPLE(klas,listClass)  \
	FIND_ALL_LISTED (klas,listClass)
#define CONVERT_ALL_LISTED_TO_MULTIPLE_END \
	TO_MULTIPLE__ \
	CONVERT_END__

#define CONVERT_ONE_AND_ONE_TO_ONE(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2)
#define CONVERT_ONE_AND_ONE_TO_ONE_END(...)  \
	TO_ONE__ (__VA_ARGS__) \
	CONVERT_END__

#define CONVERT_ONE_AND_ONE_TO_MULTIPLE(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2)
#define CONVERT_ONE_AND_ONE_TO_MULTIPLE_END  \
	TO_MULTIPLE__ \
	CONVERT_END__

#define CONVERT_ONE_WEAK_AND_ONE_TO_ONE(klas1,klas2)  \
	FIND_ONE_AND_ONE (klas1, klas2) \
	FIRST_WEAK_BEGIN
#define CONVERT_ONE_WEAK_AND_ONE_TO_ONE_END(...)  \
	TO_ONE__ (__VA_ARGS__) \
	FIRST_WEAK_END \
	CONVERT_END__

#define CONVERT_TWO_TO_MULTIPLE(klas)  \
	FIND_TWO (klas)
#define CONVERT_TWO_TO_MULTIPLE_END  \
	TO_MULTIPLE__ \
	CONVERT_END__

#define CONVERT_TWO_TO_ONE(klas)  \
	FIND_TWO (klas)
#define CONVERT_TWO_TO_ONE_END(...)  \
	TO_ONE__ (__VA_ARGS__) \
	CONVERT_END__

#define CONVERT_TWO_AND_ONE_TO_ONE(klas1,klas2)  \
	FIND_TWO_AND_ONE (klas1,klas2)
#define CONVERT_TWO_AND_ONE_TO_ONE_END(...)  \
	TO_ONE__ (__VA_ARGS__) \
	CONVERT_END__

#define CONVERT_ONE_AND_TWO_TO_ONE(klas1,klas2)  \
	FIND_ONE_AND_TWO (klas1,klas2)
#define CONVERT_ONE_AND_TWO_TO_ONE_END(...)  \
	TO_ONE__ (__VA_ARGS__) \
	CONVERT_END__

#define CONVERT_ONE_AND_ONE_AND_ONE_TO_ONE(klas1,klas2,klas3)  \
	FIND_ONE_AND_ONE_AND_ONE (klas1, klas2, klas3)
#define CONVERT_ONE_AND_ONE_AND_ONE_TO_ONE_END(...)  \
	TO_ONE__ (__VA_ARGS__) \
	CONVERT_END__

#define CONVERT_ONE_AND_ONE_AND_ONE_TO_MULTIPLE(klas1,klas2,klas3)  \
	FIND_ONE_AND_ONE_AND_ONE (klas1, klas2, klas3)
#define CONVERT_ONE_AND_ONE_AND_ONE_TO_MULTIPLE_END  \
	TO_MULTIPLE__ \
	CONVERT_END__

#define CONVERT_ONE_AND_ONE_AND_ALL_TO_MULTIPLE(klas1,klas2,klas3)  \
	FIND_ONE_AND_ONE_AND_ALL (klas1, klas2, klas3)
#define CONVERT_ONE_AND_ONE_AND_ALL_TO_MULTIPLE_END  \
	TO_MULTIPLE__ \
	CONVERT_END__

#define CONVERT_ONE_AND_ONE_AND_ONE_AND_ONE_TO_ONE(klas1,klas2,klas3,klas4)  \
	FIND_1_1_1_1 (klas1, klas2, klas3, klas4)
#define CONVERT_ONE_AND_ONE_AND_ONE_AND_ONE_TO_ONE_END(...)  \
	TO_ONE__ (__VA_ARGS__) \
	CONVERT_END__

#define CONVERT_ONE_AND_ALL_TO_ONE(klas1,klas2)  \
	FIND_ONE_AND_ALL (klas1, klas2)
#define CONVERT_ONE_AND_ALL_TO_ONE_END(...)  \
	TO_ONE__ (__VA_ARGS__) \
	CONVERT_END__

#define CONVERT_ONE_AND_ALL_TO_MULTIPLE(klas1,klas2)  \
	FIND_ONE_AND_ALL (klas1, klas2)
#define CONVERT_ONE_AND_ALL_TO_MULTIPLE_END  \
	TO_MULTIPLE__ \
	CONVERT_END__

#define CONVERT_ONE_AND_ALL_LISTED_TO_ONE(klas1,klas2,listClass)  \
	FIND_ONE_AND_ALL_LISTED (klas1, klas2, listClass)
#define CONVERT_ONE_AND_ALL_LISTED_TO_ONE_END(...) \
	TO_ONE__ (__VA_ARGS__) \
	CONVERT_END__

#define CONVERT_ONE_AND_ONE_GENERIC_TO_ONE(klas1,klas2)  \
	FIND_ONE_AND_GENERIC(klas1,klas2)
#define CONVERT_ONE_AND_ONE_GENERIC_TO_ONE_END(...) \
	TO_ONE__ (__VA_ARGS__) \
	CONVERT_END__

/*
	READ
*/

#define READ_MULTIPLE
#define READ_MULTIPLE_END  \
	if (interpreter) \
		interpreter -> returnType = kInterpreter_ReturnType::OBJECT_; \
	END_WITH_NEW_DATA

#define READ_ONE
#define READ_ONE_END  \
	praat_newWithFile (result.move(), file, MelderFile_name (file)); \
	READ_MULTIPLE_END

/*
	SAVE
*/

#define SAVE_ONE(klas)  \
	FIND_ONE (klas)
#define SAVE_ONE_END  \
	END_NO_NEW_DATA

#define SAVE_TWO(klas)  \
	FIND_TWO (klas)
#define SAVE_TWO_END  \
	END_NO_NEW_DATA

#define SAVE_ALL(klas)  \
	FIND_ALL (klas)
#define SAVE_ALL_END  \
	END_NO_NEW_DATA

#define SAVE_ALL_LISTED(klas,listClass)  \
	FIND_ALL_LISTED (klas, listClass)
#define SAVE_ALL_LISTED_END  \
	END_NO_NEW_DATA

/*
	APPEND
*/

#define APPEND_ALL(klas)  \
	FIND_ALL (klas)
#define APPEND_ALL_END  \
	END_NO_NEW_DATA

/*
	EDITOR
*/

#define EDITOR_END__  \
	praat_installEditor (editor.get(), IOBJECT); \
	editor.releaseToUser(); \
	END_NO_NEW_DATA

#define EDITOR_ONE(indefiniteArticle,klas)  \
	if (theCurrentPraatApplication -> batch) \
		Melder_throw (U"Cannot edit " #indefiniteArticle " " #klas " from batch."); \
	FIND_ONE_WITH_IOBJECT (klas)
#define EDITOR_ONE_END  \
	EDITOR_END__

#define EDITOR_ONE_WITH_ONE(indefiniteArticle,klas1,klas2)  \
	if (theCurrentPraatApplication -> batch) \
		Melder_throw (U"Cannot edit " #indefiniteArticle " " #klas1 " from batch."); \
	FIND_ONE_AND_ONE_WITH_IOBJECT (klas1, klas2)
#define EDITOR_ONE_WITH_ONE_END  \
	EDITOR_END__

#define EDITOR_ONE_WITH_ONE_AND_ONE(indefiniteArticle,klas1,klas2,klas3)  \
	if (theCurrentPraatApplication -> batch) \
		Melder_throw (U"Cannot edit " #indefiniteArticle " " #klas1 " from batch."); \
	FIND_ONE_AND_ONE_AND_ONE_WITH_IOBJECT (klas1, klas2, klas3)
#define EDITOR_ONE_WITH_ONE_AND_ONE_END  \
	EDITOR_END__

/*
	CREATION_WINDOW
*/

#define SINGLETON_CREATION_WINDOW(indefiniteArticle,klas)  \
	if (theCurrentPraatApplication -> batch) \
		Melder_throw (U"Cannot create " #indefiniteArticle " " #klas " in a window from batch.");
#define SINGLETON_CREATION_WINDOW_END  \
	END_NO_NEW_DATA   // no new data *yet*, because the window is asynchronous

#define CREATION_WINDOW(indefiniteArticle,klas)  \
	if (theCurrentPraatApplication -> batch) \
		Melder_throw (U"Cannot create " #indefiniteArticle " " #klas " in a window from batch.");
#define CREATION_WINDOW_END  \
	creationWindow.releaseToUser(); \
	END_NO_NEW_DATA   // no new data *yet*, because the window is asynchronous

/*
	WARNING
*/

#define WARNING
#define WARNING_END  \
	END_NO_NEW_DATA

/*
	PREFS
*/

#define PREFS
#define PREFS_END  \
	END_NO_NEW_DATA

/*
	PRAAT
*/

#define PRAAT
#define PRAAT_END  \
	END_NO_NEW_DATA

/* End of file praatM.h */
#endif
