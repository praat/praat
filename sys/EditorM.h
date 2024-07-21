#ifndef _EditorM_h_
#define _EditorM_h_
/* EditorM.h
 *
 * Copyright (C) 1992-2013,2015-2024 Paul Boersma
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

#ifdef _praatM_h_
	#error Include either praatM.h or EditorM.h, but not both.
#endif

#define EDITOR_ARGS  \
	[[maybe_unused]] EditorCommand cmd, \
	[[maybe_unused]] UiForm _sendingForm_, \
	[[maybe_unused]] integer _narg_, \
	[[maybe_unused]] Stackel _args_, \
	[[maybe_unused]] conststring32 _sendingString_, \
	[[maybe_unused]] Interpreter optionalInterpreter

#define EDITOR_FORM(title, helpTitle)  \
	if (cmd -> d_uiform) \
		goto _form_inited_; \
	cmd -> d_uiform = UiForm_createE (cmd, title, cmd -> itemTitle.get(), helpTitle);

#define EDITOR_OK  \
	UiForm_finish (cmd -> d_uiform.get()); \
_form_inited_: \
	if (! _args_ && ! _sendingForm_ && ! _sendingString_) {

#define EDITOR_DO  \
		UiForm_do (cmd -> d_uiform.get(), false); \
	} else if (! _sendingForm_) { \
		UiForm_parseStringE (cmd, _narg_, _args_, _sendingString_, optionalInterpreter); \
	} else {

#define EDITOR_DO_ALTERNATIVE(alternative)  \
		UiForm_do (cmd -> d_uiform.get(), false); \
	} else if (! _sendingForm_) { \
		try { \
			UiForm_parseStringE (cmd, _narg_, _args_, _sendingString_, optionalInterpreter); \
		} catch (MelderError) { \
			if (Melder_hasCrash ()) \
				throw; \
			autostring32 _parkedError = Melder_dup_f (Melder_getError ()); \
			Melder_clearError (); \
			try { \
				static autoEditorCommand _alternativeCmd; \
				if (! _alternativeCmd) \
					_alternativeCmd = Thing_new (EditorCommand); \
				_alternativeCmd -> d_editor = cmd -> d_editor; \
				_alternativeCmd -> sender = cmd -> sender; \
				_alternativeCmd -> menu = cmd -> menu; \
				_alternativeCmd -> itemTitle = Melder_dup (cmd -> itemTitle.get()); \
				_alternativeCmd -> itemWidget = nullptr; \
				_alternativeCmd -> commandCallback = cmd -> commandCallback; \
				_alternativeCmd -> script = Melder_dup (cmd -> script.get()); \
				_alternativeCmd -> d_uiform = autoUiForm (); \
				alternative (me, _alternativeCmd.get(), _sendingForm_, _narg_, _args_, _sendingString_, optionalInterpreter); \
			} catch (MelderError) { \
				Melder_clearError (); \
				Melder_appendError (_parkedError.get()); \
				throw; \
			} \
		} \
	} else {

#define EDITOR_END  \
	}

/*
	Functions to define the fields in a form on the basis of the label text
	and factory default value.
	They are to be called between EDITOR_FORM and EDITOR_OK.
*/

#define REAL_VARIABLE(realVariable)  \
	static double realVariable;

#define REAL_FIELD(realVariable, labelText, defaultStringValue)  \
	UiForm_addReal (cmd -> d_uiform.get(), & realVariable, nullptr, labelText, defaultStringValue);

#define REAL(realVariable, labelText, defaultStringValue)  \
	REAL_VARIABLE (realVariable) \
	REAL_FIELD (realVariable, labelText, defaultStringValue)


#define REAL_OR_UNDEFINED_VARIABLE(realVariable)  \
	static double realVariable;

#define REAL_OR_UNDEFINED_FIELD(realVariable, labelText, defaultStringValue)  \
	UiForm_addRealOrUndefined (cmd -> d_uiform.get(), & realVariable, nullptr, labelText, defaultStringValue);

#define REAL_OR_UNDEFINED(realVariable, labelText, defaultStringValue)  \
	REAL_OR_UNDEFINED_VARIABLE (realVariable) \
	REAL_OR_UNDEFINED_FIELD (realVariable, labelText, defaultStringValue)


#define POSITIVE_VARIABLE(realVariable)  \
	static double realVariable;

#define POSITIVE_FIELD(realVariable, labelText, defaultStringValue)  \
	UiForm_addPositive (cmd -> d_uiform.get(), & realVariable, nullptr, labelText, defaultStringValue);

#define POSITIVE(realVariable, labelText, defaultStringValue)  \
	POSITIVE_VARIABLE (realVariable) \
	POSITIVE_FIELD (realVariable, labelText, defaultStringValue)


#define INTEGER_VARIABLE(integerVariable)  \
	static integer integerVariable;

#define INTEGER_FIELD(integerVariable, labelText, defaultStringValue)  \
	UiForm_addInteger (cmd -> d_uiform.get(), & integerVariable, nullptr, labelText, defaultStringValue);

#define INTEGER(integerVariable, labelText, defaultStringValue)  \
	INTEGER_VARIABLE (integerVariable) \
	INTEGER_FIELD (integerVariable, labelText, defaultStringValue)


#define NATURAL_VARIABLE(integerVariable)  \
	static integer integerVariable;

#define NATURAL_FIELD(integerVariable, labelText, defaultStringValue)  \
	UiForm_addNatural (cmd -> d_uiform.get(), & integerVariable, nullptr, labelText, defaultStringValue);

#define NATURAL(integerVariable, labelText, defaultStringValue)  \
	NATURAL_VARIABLE (integerVariable) \
	NATURAL_FIELD (integerVariable, labelText, defaultStringValue)


#define WORD_VARIABLE(stringVariable)  \
	static conststring32 stringVariable;

#define WORD_FIELD(stringVariable, labelText, defaultStringValue)  \
	UiForm_addWord (cmd -> d_uiform.get(), & stringVariable, nullptr, labelText, defaultStringValue);

#define WORD(stringVariable, labelText, defaultStringValue)  \
	WORD_VARIABLE (stringVariable) \
	WORD_FIELD (stringVariable, labelText, defaultStringValue)


#define SENTENCE_VARIABLE(stringVariable)  \
	static conststring32 stringVariable;

#define SENTENCE_FIELD(stringVariable, labelText, defaultStringValue)  \
	UiForm_addSentence (cmd -> d_uiform.get(), & stringVariable, nullptr, labelText, defaultStringValue);

#define SENTENCE(stringVariable, labelText, defaultStringValue)  \
	SENTENCE_VARIABLE (stringVariable) \
	SENTENCE_FIELD (stringVariable, labelText, defaultStringValue)


#define COLOUR_VARIABLE(colourVariable)  \
	static MelderColour colourVariable;

#define COLOUR_FIELD(colourVariable, labelText, defaultStringValue)  \
	UiForm_addColour (cmd -> d_uiform.get(), & colourVariable, nullptr, labelText, defaultStringValue);

#define COLOUR(colourVariable, labelText, defaultStringValue)  \
	COLOUR_VARIABLE (colourVariable) \
	COLOUR_FIELD (colourVariable, labelText, defaultStringValue)


#define CHANNEL_VARIABLE(integerVariable)  \
	static integer integerVariable;

#define CHANNEL_FIELD(integerVariable, labelText, defaultStringValue)  \
	UiForm_addChannel (cmd -> d_uiform.get(), & integerVariable, nullptr, labelText, defaultStringValue);

#define CHANNEL(integerVariable, labelText, defaultStringValue)  \
	CHANNEL_VARIABLE (integerVariable) \
	CHANNEL_FIELD (integerVariable, labelText, defaultStringValue)


#define BOOLEAN_VARIABLE(booleanVariable)  \
	static bool booleanVariable;

#define BOOLEAN_FIELD(booleanVariable, labelText, defaultBooleanValue)  \
	UiForm_addBoolean (cmd -> d_uiform.get(), & booleanVariable, nullptr, labelText, defaultBooleanValue);

#define BOOLEAN(booleanVariable, labelText, defaultBooleanValue)  \
	BOOLEAN_VARIABLE (booleanVariable) \
	BOOLEAN_FIELD (booleanVariable, labelText, defaultBooleanValue)


#define HEADING(labelText)  \
	UiForm_addHeading (cmd -> d_uiform.get(), nullptr, labelText);


#define COMMENT(labelText)  \
	UiForm_addComment (cmd -> d_uiform.get(), nullptr, labelText);


#define MUTABLE_COMMENT_VARIABLE(stringVariable)  \
	static conststring32 stringVariable;

#define MUTABLE_COMMENT_FIELD(stringVariable, labelText)  \
	UiForm_addComment (cmd -> d_uiform.get(), & stringVariable, labelText);

#define MUTABLE_COMMENT(stringVariable, labelText)  \
	MUTABLE_COMMENT_VARIABLE (stringVariable) \
	MUTABLE_COMMENT_FIELD (stringVariable, labelText)


#define CAPTION(labelText)  \
	UiForm_addCaption (cmd -> d_uiform.get(), nullptr, labelText);


#define MUTABLE_CAPTION_VARIABLE(stringVariable)  \
	static conststring32 stringVariable;

#define MUTABLE_CAPTION_FIELD(stringVariable, labelText)  \
	UiForm_addCaption (cmd -> d_uiform.get(), & stringVariable, labelText);

#define MUTABLE_CAPTION(stringVariable, labelText)  \
	MUTABLE_CAPTION_VARIABLE (stringVariable) \
	MUTABLE_CAPTION_FIELD (stringVariable, labelText)


#define TEXTFIELD_VARIABLE(stringVariable)  \
	static conststring32 stringVariable;

#define TEXTFIELD_FIELD(stringVariable, labelText, defaultValue, numberOfLines)  \
	UiForm_addText (cmd -> d_uiform.get(), & stringVariable, nullptr, labelText, defaultValue, numberOfLines);

#define TEXTFIELD(stringVariable, labelText, defaultValue, numberOfLines)  \
	TEXTFIELD_VARIABLE (stringVariable) \
	TEXTFIELD_FIELD (stringVariable, labelText, defaultValue, numberOfLines)


#define FORMULA_VARIABLE(stringVariable)  \
	static conststring32 stringVariable;

#define FORMULA_FIELD(stringVariable, labelText, defaultValue)  \
	UiForm_addFormula (cmd -> d_uiform.get(), & stringVariable, nullptr, labelText, defaultValue);

#define FORMULA(stringVariable, labelText, defaultValue)  \
	FORMULA_VARIABLE (stringVariable) \
	FORMULA_FIELD (stringVariable, labelText, defaultValue)


#define INFILE_VARIABLE(stringVariable)  \
	static conststring32 stringVariable;

#define INFILE_FIELD(stringVariable, labelText, defaultValue)  \
	UiForm_addInfile (cmd -> d_uiform.get(), & stringVariable, nullptr, labelText, defaultValue);

#define INFILE(stringVariable, labelText, defaultValue)  \
	INFILE_VARIABLE (stringVariable) \
	INFILE_FIELD (stringVariable, labelText, defaultValue)


#define OUTFILE_VARIABLE(stringVariable)  \
	static conststring32 stringVariable;

#define OUTFILE_FIELD(stringVariable, labelText, defaultValue)  \
	UiForm_addOutfile (cmd -> d_uiform.get(), & stringVariable, nullptr, labelText, defaultValue);

#define OUTFILE(stringVariable, labelText, defaultValue)  \
	OUTFILE_VARIABLE (stringVariable) \
	OUTFILE_FIELD (stringVariable, labelText, defaultValue)


#define FOLDER_VARIABLE(stringVariable)  \
	static conststring32 stringVariable;

#define FOLDER_FIELD(stringVariable, labelText, defaultValue)  \
	UiForm_addFolder (cmd -> d_uiform.get(), & stringVariable, nullptr, labelText, defaultValue);

#define FOLDER(stringVariable, labelText, defaultValue)  \
	FOLDER_VARIABLE (stringVariable) \
	FOLDER_FIELD (stringVariable, labelText, defaultValue)


#define REALVECTOR_VARIABLE(realVectorVariable)  \
	static constVEC realVectorVariable;

#define REALVECTOR_FIELD(realVectorVariable, labelText, defaultFormat, defaultStringValue)  \
	UiForm_addRealVector (cmd -> d_uiform.get(), & realVectorVariable, nullptr, labelText, kUi_realVectorFormat::defaultFormat, defaultStringValue);

#define REALVECTOR(realVectorVariable, labelText, defaultFormat, defaultStringValue)  \
	REALVECTOR_VARIABLE (realVectorVariable) \
	REALVECTOR_FIELD (realVectorVariable, labelText, defaultFormat, defaultStringValue)


#define NATURALVECTOR_VARIABLE(integerVectorVariable)  \
	static constINTVEC integerVectorVariable;

#define NATURALVECTOR_FIELD(integerVectorVariable, labelText, defaultFormat, defaultStringValue)  \
	UiForm_addNaturalVector (cmd -> d_uiform.get(), & integerVectorVariable, nullptr, labelText, kUi_integerVectorFormat::defaultFormat, defaultStringValue);

#define NATURALVECTOR(integerVectorVariable, labelText, defaultFormat, defaultStringValue)  \
	NATURALVECTOR_VARIABLE (integerVectorVariable) \
	NATURALVECTOR_FIELD (integerVectorVariable, labelText, defaultFormat, defaultStringValue)


#define CHOICE_VARIABLE(optionVariable)  \
	static int optionVariable;

#define CHOICE_FIELD(optionVariable, labelText, defaultValue)  \
	UiForm_addChoice (cmd -> d_uiform.get(), & optionVariable, nullptr, nullptr, labelText, defaultValue, 1);

#define CHOICE(optionVariable, labelText, defaultValue)  \
	CHOICE_VARIABLE (optionVariable) \
	CHOICE_FIELD (optionVariable, labelText, defaultValue)


#define CHOICESTR(stringVariable, labelText, defaultValue)  \
	static conststring32 stringVariable; \
	UiForm_addChoice (cmd -> d_uiform.get(), nullptr, & stringVariable, nullptr, labelText, defaultValue, 1);


#define OPTIONMENU_VARIABLE(optionVariable)  \
	static int optionVariable;

#define OPTIONMENU_FIELD(optionVariable, labelText, defaultValue)  \
	UiForm_addOptionMenu (cmd -> d_uiform.get(), & optionVariable, nullptr, nullptr, labelText, defaultValue, 1);

#define OPTIONMENU(optionVariable, labelText, defaultValue)  \
	OPTIONMENU_VARIABLE (optionVariable) \
	OPTIONMENU_FIELD(optionVariable, labelText, defaultValue)


#define OPTIONMENUSTR(stringVariable, labelText, defaultValue)  \
	static conststring32 stringVariable; \
	UiForm_addOptionMenu (cmd -> d_uiform.get(), nullptr, & stringVariable, nullptr, labelText, defaultValue, 1);


#define OPTION(labelText)  \
	UiForm_addOption (cmd -> d_uiform.get(), labelText);


#define CHOICE_ENUM_VARIABLE(EnumeratedType, enumeratedVariable)  \
	static enum EnumeratedType enumeratedVariable; \

#define CHOICE_ENUM_FIELD(EnumeratedType, enumeratedVariable, labelText, defaultValue)  \
	{/* type checks */ \
		[[maybe_unused]] enum EnumeratedType _compilerTypeCheckDummy = defaultValue; \
		_compilerTypeCheckDummy = enumeratedVariable; \
	} \
	UiForm_addChoiceEnum (cmd -> d_uiform.get(), (int *) & enumeratedVariable, nullptr, nullptr, labelText, \
		(int) defaultValue - (int) EnumeratedType::MIN + 1, (int) EnumeratedType::MIN, \
		(enum_generic_getValue) EnumeratedType##_getValue \
	); \
	for (int _ienum = (int) EnumeratedType::MIN; _ienum <= (int) EnumeratedType::MAX; _ienum ++) \
		UiForm_addOption (cmd -> d_uiform.get(), EnumeratedType##_getText ((enum EnumeratedType) _ienum)); \

#define CHOICE_ENUM(EnumeratedType, enumeratedVariable, labelText, defaultValue)  \
	CHOICE_ENUM_VARIABLE (EnumeratedType, enumeratedVariable) \
	CHOICE_ENUM_FIELD (EnumeratedType, enumeratedVariable, labelText, defaultValue)


#define OPTIONMENU_ENUM_VARIABLE(EnumeratedType, enumeratedVariable)  \
	static enum EnumeratedType enumeratedVariable; \

#define OPTIONMENU_ENUM_FIELD(EnumeratedType, enumeratedVariable, labelText, defaultValue)  \
	{/* type checks */ \
		[[maybe_unused]] enum EnumeratedType _compilerTypeCheckDummy = defaultValue; \
		_compilerTypeCheckDummy = enumeratedVariable; \
	} \
	UiForm_addOptionMenuEnum (cmd -> d_uiform.get(), (int *) & enumeratedVariable, nullptr, nullptr, labelText, \
		(int) defaultValue - (int) EnumeratedType::MIN + 1, (int) EnumeratedType::MIN, \
		(enum_generic_getValue) EnumeratedType##_getValue \
	); \
	for (int _ienum = (int) EnumeratedType::MIN; _ienum <= (int) EnumeratedType::MAX; _ienum ++) \
		UiForm_addOption (cmd -> d_uiform.get(), EnumeratedType##_getText ((enum EnumeratedType) _ienum)); \

#define OPTIONMENU_ENUM(EnumeratedType, enumeratedVariable, labelText, defaultValue)  \
	OPTIONMENU_ENUM_VARIABLE (EnumeratedType, enumeratedVariable) \
	OPTIONMENU_ENUM_FIELD (EnumeratedType, enumeratedVariable, labelText, defaultValue)


#define LIST(integerVariable, labelText, strings, defaultValue)  \
	static integer integerVariable; \
	UiForm_addList (cmd -> d_uiform.get(), & integerVariable, nullptr, nullptr, labelText, strings, defaultValue);

/*
	Seven optional functions to change the content of a field on the basis of the current
	editor setting rather than on the basis of the factory default.
	They are to be called between EDITOR_OK and EDITOR_DO.
*/

#define SET_REAL(realVariable, realValue)  \
	UiForm_setReal (cmd -> d_uiform.get(), & realVariable, realValue);

#define SET_INTEGER(integerVariable, integerValue)  \
	UiForm_setInteger (cmd -> d_uiform.get(), & integerVariable, integerValue);

#define SET_INTEGER_AS_STRING(integerVariable, stringValue)  \
	UiForm_setIntegerAsString (cmd -> d_uiform.get(), & integerVariable, stringValue);

#define SET_BOOLEAN(booleanVariable, booleanValue)  \
	UiForm_setBoolean (cmd -> d_uiform.get(), & booleanVariable, booleanValue);

#define SET_OPTION(optionVariable, optionValue)  \
	UiForm_setOption (cmd -> d_uiform.get(), & optionVariable, optionValue);

#define SET_STRING(stringVariable, stringValue)  \
	UiForm_setString (cmd -> d_uiform.get(), & stringVariable, stringValue);

#define SET_ENUM(enumeratedVariable, EnumeratedType, enumeratedValue)  \
	enumeratedVariable = enumeratedValue /* type check */; \
	UiForm_setOption (cmd -> d_uiform.get(), (int *) & enumeratedVariable, (int) enumeratedValue - (int) EnumeratedType::MIN + 1);


#define DIALOG  cmd -> d_uiform

#define EDITOR_FORM_SAVE(title, helpTitle)  \
	if (! cmd -> d_uiform) { \
		cmd -> d_uiform = UiOutfile_createE (cmd, title, cmd -> itemTitle.get(), helpTitle); \
		} if (! _args_ && ! _sendingForm_ && ! _sendingString_) { char32 defaultName [300]; defaultName [0] = U'\0';
#define EDITOR_DO_SAVE  \
	UiOutfile_do (cmd -> d_uiform.get(), defaultName); } else { MelderFile file; structMelderFile _file2 { }; \
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
		file = UiFile_getFile (cmd -> d_uiform.get()); \
	}

#define EDITOR_FORM_READ(title, helpTitle)  \
	if (! cmd -> d_uiform) { \
		cmd -> d_uiform = UiInfile_createE (cmd, title, cmd -> itemTitle.get(), helpTitle); \
		} if (! _args_ && ! _sendingForm_ && ! _sendingString_) {
#define EDITOR_DO_READ  \
	UiInfile_do (cmd -> d_uiform.get()); } else { MelderFile file; structMelderFile _file2 { }; \
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
		file = UiFile_getFile (cmd -> d_uiform.get()); \
	}

#define VOID_EDITOR
#define VOID_EDITOR_END

#define DATA_BEGIN__  \
	Melder_assert (my data());

#define PLAY_DATA  \
	DATA_BEGIN__
#define PLAY_DATA_END

#define INFO_EDITOR
#define INFO_EDITOR_END  \
	if (optionalInterpreter) \
		optionalInterpreter -> returnType = kInterpreter_ReturnType::STRING_;

#define INFO_DATA  \
	DATA_BEGIN__
#define INFO_DATA_END  \
	if (optionalInterpreter) \
		optionalInterpreter -> returnType = kInterpreter_ReturnType::STRING_;

#define FOR_REAL__(...)  \
	if (optionalInterpreter) \
		optionalInterpreter -> returnType = kInterpreter_ReturnType::REAL_; \
	Melder_information (result, __VA_ARGS__);

#define FOR_INTEGER__(...)  \
	if (optionalInterpreter) \
		optionalInterpreter -> returnType = kInterpreter_ReturnType::INTEGER_; \
	Melder_information (double (result), __VA_ARGS__);

#define FOR_BOOLEAN__(...)  \
	if (optionalInterpreter) \
		optionalInterpreter -> returnType = kInterpreter_ReturnType::BOOLEAN_; \
	Melder_information (double (result), __VA_ARGS__);

#define FOR_COMPLEX__(...)  \
	if (optionalInterpreter) \
		optionalInterpreter -> returnType = kInterpreter_ReturnType::STRING_; /* TODO: make true complex types in script */ \
	Melder_information (result, __VA_ARGS__);

#define FOR_STRING__  \
	if (optionalInterpreter) \
		optionalInterpreter -> returnType = kInterpreter_ReturnType::STRING_; \
	Melder_information (result);

#define QUERY_EDITOR_FOR_REAL
#define QUERY_EDITOR_FOR_REAL_END(...)  \
	FOR_REAL__ (__VA_ARGS__)

#define QUERY_EDITOR_FOR_INTEGER
#define QUERY_EDITOR_FOR_INTEGER_END(...)  \
	FOR_INTEGER__ (__VA_ARGS__)

#define QUERY_EDITOR_FOR_BOOLEAN
#define QUERY_EDITOR_FOR_BOOLEAN_END(...)  \
	FOR_BOOLEAN__ (__VA_ARGS__)

#define QUERY_EDITOR_FOR_STRING
#define QUERY_EDITOR_FOR_STRING_END  \
	FOR_STRING__

#define QUERY_DATA_FOR_REAL  \
	DATA_BEGIN__
#define QUERY_DATA_FOR_REAL_END(...)  \
	FOR_REAL__ (__VA_ARGS__)

#define QUERY_DATA_FOR_INTEGER  \
	DATA_BEGIN__
#define QUERY_DATA_FOR_INTEGER_END(...)  \
	FOR_INTEGER__ (__VA_ARGS__)

#define QUERY_DATA_FOR_BOOLEAN  \
	DATA_BEGIN__
#define QUERY_DATA_FOR_BOOLEAN_END(...)  \
	FOR_BOOLEAN__ (__VA_ARGS__)

#define QUERY_DATA_FOR_STRING  \
	DATA_BEGIN__
#define QUERY_DATA_FOR_STRING_END  \
	FOR_STRING__

#define MODIFY_DATA(undoTitle)  \
	Editor_save (my boss(), undoTitle);
#define MODIFY_DATA_END  \
	Editor_broadcastDataChanged (my boss());

#define CONVERT_DATA_TO_ONE  \
	DATA_BEGIN__
#define CONVERT_DATA_TO_ONE_END(...)  \
	if (optionalInterpreter) \
		optionalInterpreter -> returnType = kInterpreter_ReturnType::OBJECT_; \
	Thing_setName (result.get(), __VA_ARGS__); \
	Editor_broadcastPublication (my boss(), result.move());

#define CREATE_ONE
#define CREATE_ONE_END(...)  \
	if (optionalInterpreter) \
		optionalInterpreter -> returnType = kInterpreter_ReturnType::OBJECT_; \
	Thing_setName (result.get(), __VA_ARGS__); \
	Editor_broadcastPublication (my boss(), result.move());

#define HELP(title)  \
	Melder_help (title);

/* End of file EditorM.h */
#endif
