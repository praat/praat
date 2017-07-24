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
#undef GET_REAL
#undef GET_INTEGER
#undef GET_STRING
#undef GET_FILE

#define REAL(label,def)		UiForm_addReal (cmd -> d_uiform.get(), label, def);
#define REAL_OR_UNDEFINED(label,def)  UiForm_addRealOrUndefined (cmd -> d_uiform.get(), label, def);
#define POSITIVE(label,def)	UiForm_addPositive (cmd -> d_uiform.get(), label, def);
#define INTEGER(label,def)	UiForm_addInteger (cmd -> d_uiform.get(), label, def);
#define NATURAL(label,def)	UiForm_addNatural (cmd -> d_uiform.get(), label, def);
#define WORD(label,def)		UiForm_addWord (cmd -> d_uiform.get(), label, def);
#define SENTENCE(label,def)	UiForm_addSentence (cmd -> d_uiform.get(), label, def);
#define COLOUR(label,def)	UiForm_addColour (cmd -> d_uiform.get(), label, def);
#define CHANNEL(label,def)	UiForm_addChannel (cmd -> d_uiform.get(), label, def);
#define BOOLEAN(label,def)	UiForm_addBoolean (cmd -> d_uiform.get(), label, def);
#define LABEL(name,label)	UiForm_addLabel (cmd -> d_uiform.get(), name, label);
#define TEXTFIELD(name,def)	UiForm_addText (cmd -> d_uiform.get(), name, def);
#define RADIO(label,def)	radio = UiForm_addRadio (cmd -> d_uiform.get(), label, def);
#define RADIOBUTTON(label)	UiRadio_addButton (radio, label);
#define OPTIONMENU(label,def)	radio = UiForm_addOptionMenu (cmd -> d_uiform.get(), label, def);
#define OPTION(label)	UiOptionMenu_addButton (radio, label);
#define RADIO_ENUM(label,enum,def) \
	RADIO (label, def - enum##_MIN + 1) \
	for (int ienum = enum##_MIN; ienum <= enum##_MAX; ienum ++) \
		OPTION (enum##_getText (ienum))
#define OPTIONMENU_ENUM(label,enum,def) \
	OPTIONMENU (label, def - enum##_MIN + 1) \
	for (int ienum = enum##_MIN; ienum <= enum##_MAX; ienum ++) \
		OPTION (enum##_getText (ienum))
#define LIST(label,n,str,def)	UiForm_addList (cmd -> d_uiform.get(), label, n, str, def);
#define SET_REAL(name,value)	UiForm_setReal (cmd -> d_uiform.get(), name, value);
#define SET_INTEGER(name,value)	UiForm_setInteger (cmd -> d_uiform.get(), name, value);
#define SET_STRING(name,value)	UiForm_setString (cmd -> d_uiform.get(), name, value);
#define SET_ENUM(name,enum,value)  SET_STRING (name, enum##_getText (value))

#define DIALOG  cmd -> d_uiform

#define EDITOR_ARGS_FORM  EditorCommand cmd, UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter
#define EDITOR_ARGS_CMD  EditorCommand cmd, UiForm, int, Stackel, const char32 *, Interpreter
#define EDITOR_ARGS_DIRECT  EditorCommand, UiForm, int, Stackel, const char32 *, Interpreter
#define EDITOR_FORM(title,helpTitle)  if (! cmd -> d_uiform) { UiField radio = nullptr; (void) radio; \
	cmd -> d_uiform = UiForm_createE (cmd, title, cmd -> itemTitle, helpTitle);
#define EDITOR_OK  UiForm_finish (cmd -> d_uiform.get()); } if (! sendingForm && ! args && ! sendingString) {
#define EDITOR_DO  UiForm_do (cmd -> d_uiform.get(), false); } else if (! sendingForm) { \
	UiForm_parseStringE (cmd, narg, args, sendingString, interpreter); } else {
#define EDITOR_END  }

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
