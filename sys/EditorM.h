/* EditorM.h
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
 * pb 2006/12/05
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

#define FORM(klas,proc,title,helpTitle) \
	static int proc (EditorCommand cmd, Any sender) \
	{ klas me = (klas) cmd -> editor; (void) sender; (void) me; if (cmd -> dialog == NULL) { Any radio = 0; (void) radio; \
	cmd -> dialog = UiForm_createE (cmd, title, helpTitle);

#define REAL(label,def)		UiForm_addReal (cmd -> dialog, label, def);
#define POSITIVE(label,def)	UiForm_addPositive (cmd -> dialog, label, def);
#define INTEGER(label,def)	UiForm_addInteger (cmd -> dialog, label, def);
#define NATURAL(label,def)	UiForm_addNatural (cmd -> dialog, label, def);
#define WORD(label,def)		UiForm_addWord (cmd -> dialog, label, def);
#define SENTENCE(label,def)	UiForm_addSentence (cmd -> dialog, label, def);
#define BOOLEAN(label,def)	UiForm_addBoolean (cmd -> dialog, label, def);
#define LABEL(name,label)	UiForm_addLabel (cmd -> dialog, name, label);
#define TEXTFIELD(name,def)	UiForm_addText (cmd -> dialog, name, def);
#define RADIO(label,def)	radio = UiForm_addRadio (cmd -> dialog, label, def);
#define RADIOBUTTON(label)	UiRadio_addButton (radio, label);
#define OPTIONMENU(label,def)	radio = UiForm_addOptionMenu (cmd -> dialog, label, def);
#define OPTION(label)	UiOptionMenu_addButton (radio, label);
#define ENUM(label,type,def)	UiForm_addEnum (cmd -> dialog, label, & enum_##type, def);
#define RADIOBUTTONS_ENUM(labelProc,min,max) { int itext; for (itext = min; itext <= max; itext ++) RADIOBUTTON (labelProc) }
#define OPTIONS_ENUM(labelProc,min,max) { int itext; for (itext = min; itext <= max; itext ++) OPTION (labelProc) }
#define LIST(label,n,str,def)	UiForm_addList (cmd -> dialog, label, n, str, def);
#define OK UiForm_finish (cmd -> dialog); } if (sender == NULL) {
#define SET_REAL(name,value)	UiForm_setReal (cmd -> dialog, name, value);
#define SET_INTEGER(name,value)	UiForm_setInteger (cmd -> dialog, name, value);
#define SET_STRING(name,value)	UiForm_setString (cmd -> dialog, name, value);
#define DO  UiForm_do (cmd -> dialog, 0); } else if (sender != cmd -> dialog) { \
	if (! UiForm_parseStringE (cmd, (char *) sender)) return 0; } else { {
#define END  } } return 1; }
#define DIRECT(klas,proc)  static int proc (EditorCommand cmd, Any sender) { klas me = (klas) cmd -> editor; \
	(void) sender; (void) me; { {

#define FORM_WRITE(klas,proc,title,helpTitle) \
	static int proc (EditorCommand cmd, Any sender) \
	{ klas me = (klas) cmd -> editor; (void) me; if (cmd -> dialog == NULL) { \
	cmd -> dialog = UiOutfile_createE (cmd, title, helpTitle); \
	} if (sender == NULL) { char defaultName [300]; defaultName [0] = '\0'; {
#define DO_WRITE \
	} UiOutfile_do (cmd -> dialog, defaultName); } else { MelderFile file; structMelderFile file2; \
		if (sender == cmd -> dialog) file = UiFile_getFile (sender); \
		else { if (! Melder_relativePathToFile (sender, & file2)) return 0; file = & file2; } {

#define GET_REAL(name)  UiForm_getReal (cmd -> dialog, name)
#define GET_INTEGER(name)  UiForm_getInteger (cmd -> dialog, name)
#define GET_STRING(name)  UiForm_getString (cmd -> dialog, name)
#define GET_FILE  UiForm_getFile (cmd -> dialog)

/* End of file EditorM.h */
