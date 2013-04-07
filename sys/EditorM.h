#ifndef _EditorM_h_
#define _EditorM_h_
/* EditorM.h
 *
 * Copyright (C) 1992-2011,2013 Paul Boersma
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

#undef FORM
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
#undef RADIOBUTTONS_ENUM
#undef OPTIONS_ENUM
#undef RADIO_ENUM
#undef OPTIONMENU_ENUM
#undef LIST
#undef OK
#undef SET_REAL
#undef SET_INTEGER
#undef SET_STRING
#undef DO
#undef END
#undef DIRECT
#undef FORM_WRITE
#undef DO_WRITE
#undef GET_REAL
#undef GET_INTEGER
#undef GET_STRING
#undef GET_FILE

#define REAL(label,def)		UiForm_addReal (cmd -> d_uiform, label, def);
#define REAL_OR_UNDEFINED(label,def)  UiForm_addRealOrUndefined (cmd -> d_uiform, label, def);
#define POSITIVE(label,def)	UiForm_addPositive (cmd -> d_uiform, label, def);
#define INTEGER(label,def)	UiForm_addInteger (cmd -> d_uiform, label, def);
#define NATURAL(label,def)	UiForm_addNatural (cmd -> d_uiform, label, def);
#define WORD(label,def)		UiForm_addWord (cmd -> d_uiform, label, def);
#define SENTENCE(label,def)	UiForm_addSentence (cmd -> d_uiform, label, def);
#define COLOUR(label,def)	UiForm_addColour (cmd -> d_uiform, label, def);
#define CHANNEL(label,def)	UiForm_addChannel (cmd -> d_uiform, label, def);
#define BOOLEAN(label,def)	UiForm_addBoolean (cmd -> d_uiform, label, def);
#define LABEL(name,label)	UiForm_addLabel (cmd -> d_uiform, name, label);
#define TEXTFIELD(name,def)	UiForm_addText (cmd -> d_uiform, name, def);
#define RADIO(label,def)	radio = UiForm_addRadio (cmd -> d_uiform, label, def);
#define RADIOBUTTON(label)	UiRadio_addButton (radio, label);
#define OPTIONMENU(label,def)	radio = UiForm_addOptionMenu (cmd -> d_uiform, label, def);
#define OPTION(label)	UiOptionMenu_addButton (radio, label);
#define RADIOBUTTONS_ENUM(labelProc,min,max) { for (int itext = min; itext <= max; itext ++) RADIOBUTTON (labelProc) }
#define OPTIONS_ENUM(labelProc,min,max) { for (int itext = min; itext <= max; itext ++) OPTION (labelProc) }
#define RADIO_ENUM(label,enum,def) \
	RADIO (label, def - enum##_MIN + 1) \
	for (int ienum = enum##_MIN; ienum <= enum##_MAX; ienum ++) \
		OPTION (enum##_getText (ienum))
#define OPTIONMENU_ENUM(label,enum,def) \
	OPTIONMENU (label, def - enum##_MIN + 1) \
	for (int ienum = enum##_MIN; ienum <= enum##_MAX; ienum ++) \
		OPTION (enum##_getText (ienum))
#define LIST(label,n,str,def)	UiForm_addList (cmd -> d_uiform, label, n, str, def);
#define SET_REAL(name,value)	UiForm_setReal (cmd -> d_uiform, name, value);
#define SET_INTEGER(name,value)	UiForm_setInteger (cmd -> d_uiform, name, value);
#define SET_STRING(name,value)	UiForm_setString (cmd -> d_uiform, name, value);
#define SET_ENUM(name,enum,value)  SET_STRING (name, enum##_getText (value))

#define DIALOG  cmd -> d_uiform

#define EDITOR_ARGS  Editor void_me, EditorCommand cmd, UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter
#define EDITOR_IAM(klas)  iam (klas); (void) me; (void) cmd; (void) sendingForm; (void) narg; (void) args; (void) sendingString; (void) interpreter
#define EDITOR_FORM(title,helpTitle)  if (cmd -> d_uiform == NULL) { Any radio = 0; (void) radio; \
	cmd -> d_uiform = UiForm_createE (cmd, title, cmd -> itemTitle, helpTitle);
#define EDITOR_OK  UiForm_finish (cmd -> d_uiform); } if (sendingForm == NULL && args == NULL && sendingString == NULL) {
#define EDITOR_DO  UiForm_do (cmd -> d_uiform, false); } else if (sendingForm == NULL) { \
	UiForm_parseStringE (cmd, narg, args, sendingString, interpreter); } else {
#define EDITOR_END  }

#define EDITOR_FORM_WRITE(title,helpTitle) \
	if (cmd -> d_uiform == NULL) { \
		cmd -> d_uiform = UiOutfile_createE (cmd, title, cmd -> itemTitle, helpTitle); \
		} if (sendingForm == NULL && args == NULL && sendingString == NULL) { wchar_t defaultName [300]; defaultName [0] = '\0';
#define EDITOR_DO_WRITE \
	UiOutfile_do (cmd -> d_uiform, defaultName); } else { MelderFile file; structMelderFile file2 = { 0 }; \
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (sendingForm); \
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }

#define EDITOR_FORM_READ(title,helpTitle) \
	if (cmd -> d_uiform == NULL) { \
		cmd -> d_uiform = UiInfile_createE (cmd, title, cmd -> itemTitle, helpTitle); \
		} if (sendingForm == NULL && args == NULL && sendingString == NULL) {
#define EDITOR_DO_READ \
	UiInfile_do (cmd -> d_uiform); } else { MelderFile file; structMelderFile file2 = { 0 }; \
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (sendingForm); \
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }

#define GET_REAL(name)  UiForm_getReal (cmd -> d_uiform, name)
#define GET_INTEGER(name)  UiForm_getInteger (cmd -> d_uiform, name)
#define GET_STRING(name)  UiForm_getString (cmd -> d_uiform, name)
#define GET_ENUM(enum,name)  (enum) enum##_getValue (GET_STRING (name))
#define GET_FILE  UiForm_getFile (cmd -> d_uiform)

/* End of file EditorM.h */
#endif
