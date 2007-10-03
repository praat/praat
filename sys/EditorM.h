/* EditorM.h
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/09/02
 */

#undef FORM
#undef REAL
#undef POSITIVE
#undef INTEGER
#undef NATURAL
#undef WORD
#undef SENTENCE
#undef BOOLEAN
#undef LABEL
#undef TEXTFIELD
#undef RADIO
#undef RADIOBUTTON
#undef OPTIONMENU
#undef OPTION
#undef ENUM
#undef RADIOBUTTONS_ENUM
#undef OPTIONS_ENUM
#undef LIST
#undef FORMW
#undef REALW
#undef POSITIVEW
#undef INTEGERW
#undef NATURALW
#undef WORDW
#undef SENTENCEW
#undef BOOLEANW
#undef LABELW
#undef TEXTFIELDW
#undef RADIOW
#undef RADIOBUTTONW
#undef OPTIONMENUW
#undef OPTIONW
#undef ENUMW
#undef RADIOBUTTONS_ENUMW
#undef OPTIONS_ENUMW
#undef LISTW
#undef OK
#undef SET_REAL
#undef SET_INTEGER
#undef SET_STRING
#undef SET_REALW
#undef SET_INTEGERW
#undef SET_STRINGW
#undef DO
#undef END
#undef DIRECT
#undef FORM_WRITE
#undef FORM_WRITEW
#undef DO_WRITE
#undef GET_REAL
#undef GET_INTEGER
#undef GET_STRING
#undef GET_REALW
#undef GET_INTEGERW
#undef GET_STRINGW
#undef GET_FILE

#define FORM(klas,proc,title,helpTitle) \
	static int proc (I, EditorCommand cmd, Any sender) \
	{ iam (klas); (void) me; if (cmd -> dialog == NULL) { Any radio = 0; (void) radio; \
	cmd -> dialog = UiForm_createE (cmd, Melder_peekUtf8ToWcs (title), Melder_peekUtf8ToWcs (helpTitle));
#define FORMW(klas,proc,title,helpTitle) \
	static int proc (I, EditorCommand cmd, Any sender) \
	{ iam (klas); (void) me; if (cmd -> dialog == NULL) { Any radio = 0; (void) radio; \
	cmd -> dialog = UiForm_createE (cmd, title, helpTitle);

#define REAL(label,def)		UiForm_addReal (cmd -> dialog, Melder_peekUtf8ToWcs (label), Melder_peekUtf8ToWcs (def));
#define POSITIVE(label,def)	UiForm_addPositive (cmd -> dialog, Melder_peekUtf8ToWcs (label), Melder_peekUtf8ToWcs (def));
#define INTEGER(label,def)	UiForm_addInteger (cmd -> dialog, Melder_peekUtf8ToWcs (label), Melder_peekUtf8ToWcs (def));
#define NATURAL(label,def)	UiForm_addNatural (cmd -> dialog, Melder_peekUtf8ToWcs (label), Melder_peekUtf8ToWcs (def));
#define WORD(label,def)		UiForm_addWord (cmd -> dialog, Melder_peekUtf8ToWcs (label), Melder_peekUtf8ToWcs (def));
#define SENTENCE(label,def)	UiForm_addSentence (cmd -> dialog, Melder_peekUtf8ToWcs (label), Melder_peekUtf8ToWcs (def));
#define BOOLEAN(label,def)	UiForm_addBoolean (cmd -> dialog, Melder_peekUtf8ToWcs (label), def);
#define LABEL(name,label)	UiForm_addLabel (cmd -> dialog, Melder_peekUtf8ToWcs (name), Melder_peekUtf8ToWcs (label));
#define TEXTFIELD(name,def)	UiForm_addText (cmd -> dialog, Melder_peekUtf8ToWcs (name), Melder_peekUtf8ToWcs (def));
#define RADIO(label,def)	radio = UiForm_addRadio (cmd -> dialog, Melder_peekUtf8ToWcs (label), def);
#define RADIOBUTTON(label)	UiRadio_addButton (radio, Melder_peekUtf8ToWcs (label));
#define OPTIONMENU(label,def)	radio = UiForm_addOptionMenu (cmd -> dialog, Melder_peekUtf8ToWcs (label), def);
#define OPTION(label)	UiOptionMenu_addButton (radio, Melder_peekUtf8ToWcs (label));
#define ENUM(label,type,def)	UiForm_addEnum (cmd -> dialog, Melder_peekUtf8ToWcs (label), & enum_##type, def);
#define RADIOBUTTONS_ENUM(labelProc,min,max) { int itext; for (itext = min; itext <= max; itext ++) RADIOBUTTON (labelProc) }
#define OPTIONS_ENUM(labelProc,min,max) { int itext; for (itext = min; itext <= max; itext ++) OPTION (labelProc) }
#define LIST(label,n,str,def)	UiForm_addList (cmd -> dialog, Melder_peekUtf8ToWcs (label), n, str, def);
#define REALW(label,def)		UiForm_addReal (cmd -> dialog, label, def);
#define POSITIVEW(label,def)	UiForm_addPositive (cmd -> dialog, label, def);
#define INTEGERW(label,def)	UiForm_addInteger (cmd -> dialog, label, def);
#define NATURALW(label,def)	UiForm_addNatural (cmd -> dialog, label, def);
#define WORDW(label,def)		UiForm_addWord (cmd -> dialog, label, def);
#define SENTENCEW(label,def)	UiForm_addSentence (cmd -> dialog, label, def);
#define BOOLEANW(label,def)	UiForm_addBoolean (cmd -> dialog, label, def);
#define LABELW(name,label)	UiForm_addLabel (cmd -> dialog, name, label);
#define TEXTFIELDW(name,def)	UiForm_addText (cmd -> dialog, name, def);
#define RADIOW(label,def)	radio = UiForm_addRadio (cmd -> dialog, label, def);
#define RADIOBUTTONW(label)	UiRadio_addButton (radio, label);
#define OPTIONMENUW(label,def)	radio = UiForm_addOptionMenu (cmd -> dialog, label, def);
#define OPTIONW(label)	UiOptionMenu_addButton (radio, label);
#define ENUMW(label,type,def)	UiForm_addEnum (cmd -> dialog, label, & enum_##type, def);
#define RADIOBUTTONS_ENUMW(labelProc,min,max) { int itext; for (itext = min; itext <= max; itext ++) RADIOBUTTONW (labelProc) }
#define OPTIONS_ENUMW(labelProc,min,max) { int itext; for (itext = min; itext <= max; itext ++) OPTIONW (labelProc) }
#define LISTW(label,n,str,def)	UiForm_addList (cmd -> dialog, label, n, str, def);
#define OK UiForm_finish (cmd -> dialog); } if (sender == NULL) {
#define SET_REAL(name,value)	UiForm_setReal (cmd -> dialog, Melder_peekUtf8ToWcs (name), value);
#define SET_INTEGER(name,value)	UiForm_setInteger (cmd -> dialog, Melder_peekUtf8ToWcs (name), value);
#define SET_STRING(name,value)	UiForm_setString (cmd -> dialog, Melder_peekUtf8ToWcs (name), Melder_peekUtf8ToWcs (value));
#define SET_REALW(name,value)	UiForm_setReal (cmd -> dialog, name, value);
#define SET_INTEGERW(name,value)	UiForm_setInteger (cmd -> dialog, name, value);
#define SET_STRINGW(name,value)	UiForm_setString (cmd -> dialog, name, value);
#define DO  UiForm_do (cmd -> dialog, 0); } else if (sender != cmd -> dialog) { \
	if (! UiForm_parseStringE (cmd, (wchar_t *) sender)) return 0; } else { {
#define END  } } return 1; }
#define DIRECT(klas,proc)  static int proc (I, EditorCommand cmd, Any sender) { iam (klas); (void) me; (void) cmd; (void) sender; { {

#define DIALOG  cmd -> dialog

#define EDITOR_ARGS  Any void_me, EditorCommand cmd, Any sender
#define EDITOR_IAM(klas)  iam (klas); (void) me; (void) cmd; (void) sender
#define EDITOR_FORM(title,helpTitle)  if (cmd -> dialog == NULL) { Any radio = 0; (void) radio; \
	cmd -> dialog = UiForm_createE (cmd, Melder_peekUtf8ToWcs (title), Melder_peekUtf8ToWcs (helpTitle));
#define EDITOR_FORMW(title,helpTitle)  if (cmd -> dialog == NULL) { Any radio = 0; (void) radio; \
	cmd -> dialog = UiForm_createE (cmd, title, helpTitle);
#define EDITOR_OK  UiForm_finish (cmd -> dialog); } if (sender == NULL) {
#define EDITOR_DO  UiForm_do (cmd -> dialog, 0); } else if (sender != cmd -> dialog) { \
	if (! UiForm_parseStringE (cmd, (wchar_t *) sender)) return 0; } else {
#define EDITOR_END  } return 1;

#define FORM_WRITE(klas,proc,title,helpTitle) \
	static int proc (I, EditorCommand cmd, Any sender) \
	{ iam (klas); (void) me; if (cmd -> dialog == NULL) { \
	cmd -> dialog = UiOutfile_createE (cmd, Melder_peekUtf8ToWcs (title), Melder_peekUtf8ToWcs (helpTitle)); \
	} if (sender == NULL) { wchar_t defaultName [300]; defaultName [0] = '\0'; {
#define FORM_WRITEW(klas,proc,title,helpTitle) \
	static int proc (I, EditorCommand cmd, Any sender) \
	{ iam (klas); (void) me; if (cmd -> dialog == NULL) { \
	cmd -> dialog = UiOutfile_createE (cmd, title, helpTitle); \
	} if (sender == NULL) { wchar_t defaultName [300]; defaultName [0] = '\0'; {
#define DO_WRITE \
	} UiOutfile_do (cmd -> dialog, defaultName); } else { MelderFile file; structMelderFile file2 = { 0 }; \
		if (sender == cmd -> dialog) file = UiFile_getFile (sender); \
		else { if (! Melder_relativePathToFile (sender, & file2)) return 0; file = & file2; } {

#define EDITOR_FORM_WRITE(title,helpTitle) \
	if (cmd -> dialog == NULL) { \
		cmd -> dialog = UiOutfile_createE (cmd, title, helpTitle); \
		} if (sender == NULL) { wchar_t defaultName [300]; defaultName [0] = '\0';
#define EDITOR_DO_WRITE \
	UiOutfile_do (cmd -> dialog, defaultName); } else { MelderFile file; structMelderFile file2 = { 0 }; \
		if (sender == cmd -> dialog) file = UiFile_getFile (sender); \
		else { if (! Melder_relativePathToFile (sender, & file2)) return 0; file = & file2; }

#define GET_REAL(name)  UiForm_getReal (cmd -> dialog, Melder_peekUtf8ToWcs (name))
#define GET_INTEGER(name)  UiForm_getInteger (cmd -> dialog, Melder_peekUtf8ToWcs (name))
#define GET_STRING(name)  UiForm_getStringA (cmd -> dialog, name)
#define GET_REALW(name)  UiForm_getReal (cmd -> dialog, name)
#define GET_INTEGERW(name)  UiForm_getInteger (cmd -> dialog, name)
#define GET_STRINGW(name)  UiForm_getString (cmd -> dialog, name)
#define GET_FILE  UiForm_getFile (cmd -> dialog)

/* End of file EditorM.h */
