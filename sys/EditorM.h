#ifndef _EditorM_h_
#define _EditorM_h_
/* EditorM.h
 *
 * Copyright (C) 1992-2011,2013,2015,2016,2017 Paul Boersma
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
#undef TEXTFIELD
#undef RADIO
#undef RADIOBUTTON
#undef OPTIONMENU
#undef OPTION
#undef RADIO_ENUM
#undef OPTIONMENU_ENUM
#undef LIST
#undef SET_REAL
#undef SET_INTEGER
#undef SET_STRING
#undef SET_ENUM
#undef GET_REAL
#undef GET_INTEGER
#undef GET_STRING
#undef GET_FILE

#define EDITOR_ARGS_FORM  EditorCommand cmd, UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter
#define EDITOR_ARGS_CMD  EditorCommand cmd, UiForm, int, Stackel, const char32 *, Interpreter
#define EDITOR_ARGS_DIRECT  EditorCommand, UiForm, int, Stackel, const char32 *, Interpreter
#define EDITOR_FORM(title,helpTitle)  if (! cmd -> d_uiform) { UiField _radio_ = nullptr; (void) _radio_; \
	cmd -> d_uiform = UiForm_createE (cmd, title, cmd -> itemTitle, helpTitle);
#define EDITOR_OK  UiForm_finish (cmd -> d_uiform.get()); } if (! sendingForm && ! args && ! sendingString) {
#define EDITOR_DO  UiForm_do (cmd -> d_uiform.get(), false); } else if (! sendingForm) { \
	UiForm_parseStringE (cmd, narg, args, sendingString, interpreter); } else {
#define EDITOR_END  }

/*
	Functions to define the fields in a form on the basis of the label text
	(or an invisible name) and factory default value.
	They are to be called between EDITOR_FORM and EDITOR_OK.
*/

#define REAL(fieldLabel, defaultValue) \
	UiForm_addReal (cmd -> d_uiform.get(), fieldLabel, defaultValue);

#define REAL_OR_UNDEFINED(fieldLabel, defaultValue) \
	UiForm_addRealOrUndefined (cmd -> d_uiform.get(), fieldLabel, defaultValue);

#define POSITIVE(fieldLabel, defaultValue) \
	UiForm_addPositive (cmd -> d_uiform.get(), fieldLabel, defaultValue);

#define INTEGER(fieldLabel, defaultValue) \
	UiForm_addInteger (cmd -> d_uiform.get(), fieldLabel, defaultValue);

#define NATURAL(fieldLabel, defaultValue) \
	UiForm_addNatural (cmd -> d_uiform.get(), fieldLabel, defaultValue);

#define WORD(fieldLabel, defaultValue) \
	UiForm_addWord (cmd -> d_uiform.get(), fieldLabel, defaultValue);

#define SENTENCE(fieldLabel, defaultValue) \
	UiForm_addSentence (cmd -> d_uiform.get(), fieldLabel, defaultValue);

#define COLOUR(fieldLabel, defaultValue)	\
	UiForm_addColour (cmd -> d_uiform.get(), fieldLabel, defaultValue);

#define CHANNEL(fieldLabel, defaultValue) \
	UiForm_addChannel (cmd -> d_uiform.get(), fieldLabel, defaultValue);

#define BOOLEAN(fieldLabel, defaultValue) \
	UiForm_addBoolean (cmd -> d_uiform.get(), fieldLabel, defaultValue);

#define LABEL(invisibleName, labelText) \
	UiForm_addLabel (cmd -> d_uiform.get(), invisibleName, labelText);

#define TEXTFIELD(invisibleName, defaultValue) \
	UiForm_addText (cmd -> d_uiform.get(), invisibleName, defaultValue);

#define RADIO(fieldLabel, defaultValue)\
	_radio_ = UiForm_addRadio (cmd -> d_uiform.get(), fieldLabel, defaultValue);

#define RADIOBUTTON(fieldLabel)	\
	UiRadio_addButton (_radio_, fieldLabel);

#define OPTIONMENU(fieldLabel, defaultValue)	\
	_radio_ = UiForm_addOptionMenu (cmd -> d_uiform.get(), fieldLabel, defaultValue);

#define OPTION(fieldLabel)\
	UiOptionMenu_addButton (_radio_, fieldLabel);

#define RADIO_ENUM(fieldLabel, kType, defaultValue) \
	{ kType _compilerTypeCheckDummy = defaultValue; (void) _compilerTypeCheckDummy; } \
	RADIO (fieldLabel, (int) defaultValue - (int) kType::MIN + 1) \
	for (int _ienum = (int) kType::MIN; _ienum <= (int) kType::MAX; _ienum ++) \
		OPTION (kType##_getText ((kType) _ienum))

#define OPTIONMENU_ENUM(fieldLabel, kType, defaultValue) \
	{ kType _compilerTypeCheckDummy = defaultValue; (void) _compilerTypeCheckDummy; } \
	OPTIONMENU (fieldLabel, (int) defaultValue - (int) kType::MIN + 1) \
	for (int _ienum = (int) kType::MIN; _ienum <= (int) kType::MAX; _ienum ++) \
		OPTION (kType##_getText ((kType) _ienum))

#define LIST(fieldLabel, numberOfStrings, strings, defaultValue)	\
	UiForm_addList (cmd -> d_uiform.get(), fieldLabel, numberOfStrings, strings, defaultValue);

/*
	Four optional functions to change the content of a field on the basis of the current
	editor setting rather than on the basis of the factory default.
	They are to be called between EDITOR_OK and EDITOR_DO.
*/

#define SET_REAL(fieldLabel, newValue) \
	UiForm_setReal (cmd -> d_uiform.get(), fieldLabel, newValue);

#define SET_INTEGER(fieldLabel, newValue) \
	UiForm_setInteger (cmd -> d_uiform.get(), fieldLabel, newValue);

#define SET_STRING(fieldLabelOrInvisibleName, newValue) \
	UiForm_setString (cmd -> d_uiform.get(), fieldLabelOrInvisibleName, newValue);

#define SET_ENUM(fieldLabel, kType, newValue) \
	{ kType _compilerTypeCheckDummy = newValue; (void) _compilerTypeCheckDummy; } \
	UiForm_setString (cmd -> d_uiform.get(), fieldLabel, kType##_getText (newValue));


#define DIALOG  cmd -> d_uiform

#define EDITOR_FORM_SAVE(title,helpTitle) \
	if (! cmd -> d_uiform) { \
		cmd -> d_uiform = autoUiForm (UiOutfile_createE (cmd, title, cmd -> itemTitle, helpTitle)); \
		} if (! sendingForm && ! args && ! sendingString) { char32 defaultName [300]; defaultName [0] = U'\0';
#define EDITOR_DO_SAVE \
	UiOutfile_do (cmd -> d_uiform.get(), defaultName); } else { MelderFile file; structMelderFile file2 { }; \
		if (! args && ! sendingString) file = UiFile_getFile (sendingForm); \
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }

#define EDITOR_FORM_READ(title,helpTitle) \
	if (! cmd -> d_uiform) { \
		cmd -> d_uiform = autoUiForm (UiInfile_createE (cmd, title, cmd -> itemTitle, helpTitle)); \
		} if (! sendingForm && ! args && ! sendingString) {
#define EDITOR_DO_READ \
	UiInfile_do (cmd -> d_uiform.get()); } else { MelderFile file; structMelderFile file2 { }; \
		if (! args && ! sendingString) file = UiFile_getFile (sendingForm); \
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }

#define GET_REAL(name)  UiForm_getReal (cmd -> d_uiform.get(), name)
#define GET_INTEGER(name)  UiForm_getInteger (cmd -> d_uiform.get(), name)
#define GET_STRING(name)  UiForm_getString (cmd -> d_uiform.get(), name)
#define GET_ENUM(enum,name)  (enum) enum##_getValue (GET_STRING (name))
#define GET_FILE  UiForm_getFile (cmd -> d_uiform.get())

/* End of file EditorM.h */
#endif
