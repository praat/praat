#ifndef _EditorM_h_
#define _EditorM_h_
/* EditorM.h
 *
 * Copyright (C) 1992-2013,2015-2020 Paul Boersma
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

#undef REAL
#undef REAL_OR_UNDEFINED
#undef POSITIVE
#undef INTEGER
#undef NATURAL
#undef WORD
#undef SENTENCE
#undef COLOUR
#undef CHANNEL
#undef BOOLEAN
#undef LABEL
#undef MUTABLE_LABEL
#undef TEXTFIELD
#undef RADIO
#undef RADIOSTR
#undef RADIOBUTTON
#undef OPTIONMENU
#undef OPTIONMENUSTR
#undef OPTION
#undef RADIO_ENUM
#undef OPTIONMENU_ENUM
#undef LIST
#undef SET_REAL
#undef SET_INTEGER
#undef SET_BOOLEAN
#undef SET_STRING
#undef SET_ENUM

#define EDITOR_ARGS_FORM  EditorCommand cmd, UiForm _sendingForm_, integer _narg_, Stackel _args_, conststring32 _sendingString_, Interpreter interpreter
#define EDITOR_ARGS_FORM_FORWARD  cmd, _sendingForm_, _narg_, _args_, _sendingString_, interpreter
#define EDITOR_ARGS_CMD  EditorCommand cmd, UiForm, integer, Stackel, conststring32, Interpreter
#define EDITOR_ARGS_CMD_FORWARD  cmd, nullptr, 0, nullptr, nullptr, nullptr
#define EDITOR_ARGS_DIRECT  EditorCommand, UiForm, integer, Stackel, conststring32, Interpreter
#define EDITOR_ARGS_DIRECT_FORWARD  nullptr, nullptr, 0, nullptr, nullptr, nullptr

#define EDITOR_FORM(title, helpTitle)  \
	UiField _radio_ = nullptr; \
	(void) _radio_; \
	if (cmd -> d_uiform) goto _form_inited_; \
	cmd -> d_uiform = UiForm_createE (cmd, title, cmd -> itemTitle.get(), helpTitle);

#define EDITOR_OK  \
	UiForm_finish (cmd -> d_uiform.get()); \
_form_inited_: \
	if (! _args_ && ! _sendingForm_ && ! _sendingString_) {

#define EDITOR_DO  \
		UiForm_do (cmd -> d_uiform.get(), false); \
	} else if (! _sendingForm_) { \
		UiForm_parseStringE (cmd, _narg_, _args_, _sendingString_, interpreter); \
	} else {

#define EDITOR_END  \
	}

/*
	Functions to define the fields in a form on the basis of the label text
	and factory default value.
	They are to be called between EDITOR_FORM and EDITOR_OK.
*/

#define REAL_VARIABLE(realVariable) \
	static double realVariable;

#define REAL_FIELD(realVariable, labelText, defaultStringValue) \
	UiForm_addReal (cmd -> d_uiform.get(), & realVariable, nullptr, labelText, defaultStringValue);

#define REAL(realVariable, labelText, defaultStringValue) \
	REAL_VARIABLE (realVariable) \
	REAL_FIELD (realVariable, labelText, defaultStringValue)


#define REAL_OR_UNDEFINED_VARIABLE(realVariable) \
	static double realVariable;

#define REAL_OR_UNDEFINED_FIELD(realVariable, labelText, defaultStringValue) \
	UiForm_addRealOrUndefined (cmd -> d_uiform.get(), & realVariable, nullptr, labelText, defaultStringValue);

#define REAL_OR_UNDEFINED(realVariable, labelText, defaultStringValue) \
	REAL_OR_UNDEFINED_VARIABLE (realVariable) \
	REAL_OR_UNDEFINED_FIELD (realVariable, labelText, defaultStringValue)


#define POSITIVE_VARIABLE(realVariable) \
	static double realVariable;

#define POSITIVE_FIELD(realVariable, labelText, defaultStringValue) \
	UiForm_addPositive (cmd -> d_uiform.get(), & realVariable, nullptr, labelText, defaultStringValue);

#define POSITIVE(realVariable, labelText, defaultStringValue) \
	POSITIVE_VARIABLE (realVariable) \
	POSITIVE_FIELD (realVariable, labelText, defaultStringValue)


#define INTEGER_VARIABLE(integerVariable) \
	static integer integerVariable;

#define INTEGER_FIELD(integerVariable, labelText, defaultStringValue) \
	UiForm_addInteger (cmd -> d_uiform.get(), & integerVariable, nullptr, labelText, defaultStringValue);

#define INTEGER(integerVariable, labelText, defaultStringValue) \
	INTEGER_VARIABLE (integerVariable) \
	INTEGER_FIELD (integerVariable, labelText, defaultStringValue)


#define NATURAL_VARIABLE(integerVariable) \
	static integer integerVariable;

#define NATURAL_FIELD(integerVariable, labelText, defaultStringValue) \
	UiForm_addNatural (cmd -> d_uiform.get(), & integerVariable, nullptr, labelText, defaultStringValue);

#define NATURAL(integerVariable, labelText, defaultStringValue) \
	NATURAL_VARIABLE (integerVariable) \
	NATURAL_FIELD (integerVariable, labelText, defaultStringValue)


#define WORD_VARIABLE(stringVariable) \
	static conststring32 stringVariable;

#define WORD_FIELD(stringVariable, labelText, defaultStringValue) \
	UiForm_addWord (cmd -> d_uiform.get(), & stringVariable, nullptr, labelText, defaultStringValue);

#define WORD(stringVariable, labelText, defaultStringValue) \
	WORD_VARIABLE (stringVariable) \
	WORD_FIELD (stringVariable, labelText, defaultStringValue)


#define SENTENCE_VARIABLE(stringVariable) \
	static conststring32 stringVariable;

#define SENTENCE_FIELD(stringVariable, labelText, defaultStringValue) \
	UiForm_addSentence (cmd -> d_uiform.get(), & stringVariable, nullptr, labelText, defaultStringValue);

#define SENTENCE(stringVariable, labelText, defaultStringValue) \
	SENTENCE_VARIABLE (stringVariable) \
	SENTENCE_FIELD (stringVariable, labelText, defaultStringValue)


#define COLOUR_VARIABLE(colourVariable) \
	static MelderColour colourVariable;

#define COLOUR_FIELD(colourVariable, labelText, defaultStringValue) \
	UiForm_addColour (cmd -> d_uiform.get(), & colourVariable, nullptr, labelText, defaultStringValue);

#define COLOUR(colourVariable, labelText, defaultStringValue) \
	COLOUR_VARIABLE (colourVariable) \
	COLOUR_FIELD (colourVariable, labelText, defaultStringValue)


#define CHANNEL_VARIABLE(integerVariable) \
	static integer integerVariable;

#define CHANNEL_FIELD(integerVariable, labelText, defaultStringValue) \
	UiForm_addChannel (cmd -> d_uiform.get(), & integerVariable, nullptr, labelText, defaultStringValue);

#define CHANNEL(integerVariable, labelText, defaultStringValue) \
	CHANNEL_VARIABLE (integerVariable) \
	CHANNEL_FIELD (integerVariable, labelText, defaultStringValue)


#define BOOLEAN_VARIABLE(booleanVariable) \
	static bool booleanVariable;

#define BOOLEAN_FIELD(booleanVariable, labelText, defaultBooleanValue) \
	UiForm_addBoolean (cmd -> d_uiform.get(), & booleanVariable, nullptr, labelText, defaultBooleanValue);

#define BOOLEAN(booleanVariable, labelText, defaultBooleanValue) \
	BOOLEAN_VARIABLE (booleanVariable) \
	BOOLEAN_FIELD (booleanVariable, labelText, defaultBooleanValue)


#define LABEL(labelText) \
	UiForm_addLabel (cmd -> d_uiform.get(), nullptr, labelText);


#define MUTABLE_LABEL_VARIABLE(stringVariable) \
	static conststring32 stringVariable;

#define MUTABLE_LABEL_FIELD(stringVariable, labelText) \
	UiForm_addLabel (cmd -> d_uiform.get(), & stringVariable, labelText);

#define MUTABLE_LABEL(stringVariable, labelText) \
	MUTABLE_LABEL_VARIABLE (stringVariable) \
	MUTABLE_LABEL_FIELD (stringVariable, labelText)


#define TEXTFIELD_VARIABLE(stringVariable) \
	static conststring32 stringVariable;

#define TEXTFIELD_FIELD(stringVariable, labelText, defaultValue) \
	if (labelText != nullptr) UiForm_addLabel (cmd -> d_uiform.get(), nullptr, labelText); \
	UiForm_addText (cmd -> d_uiform.get(), & stringVariable, nullptr, U"", defaultValue);

#define TEXTFIELD(stringVariable, labelText, defaultValue) \
	TEXTFIELD_VARIABLE (stringVariable) \
	TEXTFIELD_FIELD (stringVariable, labelText, defaultValue)


#define RADIO_VARIABLE(optionVariable) \
	static int optionVariable;

#define RADIO_FIELD(optionVariable, labelText, defaultValue) \
	_radio_ = UiForm_addRadio (cmd -> d_uiform.get(), & optionVariable, nullptr, nullptr, labelText, defaultValue, 1);

#define RADIO(optionVariable, labelText, defaultValue) \
	RADIO_VARIABLE (optionVariable) \
	RADIO_FIELD (optionVariable, labelText, defaultValue)


#define RADIOSTR(stringVariable, labelText, defaultValue) \
	static conststring32 stringVariable; \
	_radio_ = UiForm_addRadio (cmd -> d_uiform.get(), nullptr, & stringVariable, nullptr, labelText, defaultValue, 1);


#define RADIOBUTTON(labelText) \
	UiRadio_addButton (_radio_, labelText);


#define OPTIONMENU_VARIABLE(optionVariable) \
	static int optionVariable;

#define OPTIONMENU_FIELD(optionVariable, labelText, defaultValue) \
	_radio_ = UiForm_addOptionMenu (cmd -> d_uiform.get(), & optionVariable, nullptr, nullptr, labelText, defaultValue, 1);

#define OPTIONMENU(optionVariable, labelText, defaultValue) \
	OPTIONMENU_VARIABLE (optionVariable) \
	OPTIONMENU_FIELD(optionVariable, labelText, defaultValue)


#define OPTIONMENUSTR(stringVariable, labelText, defaultValue) \
	static conststring32 stringVariable; \
	_radio_ = UiForm_addOptionMenu (cmd -> d_uiform.get(), nullptr, & stringVariable, nullptr, labelText, defaultValue, 1);


#define OPTION(labelText) \
	UiOptionMenu_addButton (_radio_, labelText);


#define RADIO_ENUM_VARIABLE(EnumeratedType, enumeratedVariable) \
	static enum EnumeratedType enumeratedVariable; \

#define RADIO_ENUM_FIELD(EnumeratedType, enumeratedVariable, labelText, defaultValue) \
	{/* type checks */ \
		enum EnumeratedType _compilerTypeCheckDummy = defaultValue; \
		_compilerTypeCheckDummy = enumeratedVariable; \
		(void) _compilerTypeCheckDummy; \
	} \
	{/* scope */ \
		UiField _radio = UiForm_addRadio (cmd -> d_uiform.get(), (int *) & enumeratedVariable, nullptr, nullptr, labelText, \
			(int) defaultValue - (int) EnumeratedType::MIN + 1, (int) EnumeratedType::MIN); \
		for (int _ienum = (int) EnumeratedType::MIN; _ienum <= (int) EnumeratedType::MAX; _ienum ++) \
			UiRadio_addButton (_radio, EnumeratedType##_getText ((enum EnumeratedType) _ienum)); \
	}

#define RADIO_ENUM(EnumeratedType, enumeratedVariable, labelText, defaultValue) \
	RADIO_ENUM_VARIABLE (EnumeratedType, enumeratedVariable) \
	RADIO_ENUM_FIELD (EnumeratedType, enumeratedVariable, labelText, defaultValue)


#define OPTIONMENU_ENUM_VARIABLE(EnumeratedType, enumeratedVariable) \
	static enum EnumeratedType enumeratedVariable; \

#define OPTIONMENU_ENUM_FIELD(EnumeratedType, enumeratedVariable, labelText, defaultValue) \
	{/* type checks */ \
		enum EnumeratedType _compilerTypeCheckDummy = defaultValue; \
		_compilerTypeCheckDummy = enumeratedVariable; \
		(void) _compilerTypeCheckDummy; \
	} \
	{/* scope */ \
		UiField _radio = UiForm_addOptionMenu (cmd -> d_uiform.get(), (int *) & enumeratedVariable, nullptr, nullptr, labelText, \
			(int) defaultValue - (int) EnumeratedType::MIN + 1, (int) EnumeratedType::MIN); \
		for (int _ienum = (int) EnumeratedType::MIN; _ienum <= (int) EnumeratedType::MAX; _ienum ++) \
			UiOptionMenu_addButton (_radio, EnumeratedType##_getText ((enum EnumeratedType) _ienum)); \
	}

#define OPTIONMENU_ENUM(EnumeratedType, enumeratedVariable, labelText, defaultValue) \
	OPTIONMENU_ENUM_VARIABLE (EnumeratedType, enumeratedVariable) \
	OPTIONMENU_ENUM_FIELD (EnumeratedType, enumeratedVariable, labelText, defaultValue)


#define LIST(integerVariable, labelText, strings, defaultValue) \
	static integer integerVariable; \
	UiForm_addList (cmd -> d_uiform.get(), & integerVariable, nullptr, nullptr, labelText, strings, defaultValue);

/*
	Seven optional functions to change the content of a field on the basis of the current
	editor setting rather than on the basis of the factory default.
	They are to be called between EDITOR_OK and EDITOR_DO.
*/

#define SET_REAL(realVariable, realValue) \
	UiForm_setReal (cmd -> d_uiform.get(), & realVariable, realValue);

#define SET_INTEGER(integerVariable, integerValue)  \
	UiForm_setInteger (cmd -> d_uiform.get(), & integerVariable, integerValue);

#define SET_INTEGER_AS_STRING(integerVariable, stringValue)  \
	UiForm_setIntegerAsString (cmd -> d_uiform.get(), & integerVariable, stringValue);

#define SET_BOOLEAN(booleanVariable, booleanValue)  \
	UiForm_setBoolean (cmd -> d_uiform.get(), & booleanVariable, booleanValue);

#define SET_OPTION(optionVariable, optionValue)  \
	UiForm_setOption (cmd -> d_uiform.get(), & optionVariable, optionValue);

#define SET_STRING(stringVariable, stringValue) \
	UiForm_setString (cmd -> d_uiform.get(), & stringVariable, stringValue);

#define SET_ENUM(enumeratedVariable, EnumeratedType, enumeratedValue) \
	enumeratedVariable = enumeratedValue /* type check */; \
	UiForm_setOption (cmd -> d_uiform.get(), (int *) & enumeratedVariable, (int) enumeratedValue - (int) EnumeratedType::MIN + 1);


#define DIALOG  cmd -> d_uiform

#define EDITOR_FORM_SAVE(title, helpTitle) \
	if (! cmd -> d_uiform) { \
		cmd -> d_uiform = UiOutfile_createE (cmd, title, cmd -> itemTitle.get(), helpTitle); \
		} if (! _args_ && ! _sendingForm_ && ! _sendingString_) { char32 defaultName [300]; defaultName [0] = U'\0';
#define EDITOR_DO_SAVE \
	(void) interpreter; \
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

#define EDITOR_FORM_READ(title, helpTitle) \
	if (! cmd -> d_uiform) { \
		cmd -> d_uiform = UiInfile_createE (cmd, title, cmd -> itemTitle.get(), helpTitle); \
		} if (! _args_ && ! _sendingForm_ && ! _sendingString_) {
#define EDITOR_DO_READ \
	(void) interpreter; \
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

/* End of file EditorM.h */
#endif
