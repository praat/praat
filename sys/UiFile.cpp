/* UiFile.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "UiP.h"
#include "Editor.h"

Thing_define (UiFile, Thing) {
	// new data:
	public:
		EditorCommand command;
		GuiObject parent;
		structMelderFile file;
		const wchar *invokingButtonTitle, *helpTitle;
		void (*okCallback) (UiForm sendingForm, const wchar *sendingString, Interpreter interpreter, const wchar *invokingButtonTitle, bool modified, void *closure);
		void *okClosure;
		int shiftKeyPressed;
	// overridden methods:
		virtual void v_destroy ();
};

void structUiFile :: v_destroy () {
	UiFile_Parent :: v_destroy ();
}

Thing_implement (UiFile, Thing, 0);

static void UiFile_init (I, GuiObject parent, const wchar_t *title) {
	iam (UiFile);
	my parent = parent;
	Thing_setName (me, title);
}

MelderFile UiFile_getFile (I) {
	iam (UiFile);
	return & my file;
}

/********** READING A FILE **********/

Thing_define (UiInfile, UiFile) {
	// new data:
	public:
		bool allowMultipleFiles;
};

Thing_implement (UiInfile, UiFile, 0);

UiForm UiInfile_create (GuiObject parent, const wchar *title,
	void (*okCallback) (UiForm, const wchar *, Interpreter, const wchar *, bool, void *), void *okClosure,
	const wchar *invokingButtonTitle, const wchar *helpTitle, bool allowMultipleFiles)
{
	UiInfile me = Thing_new (UiInfile);
	my okCallback = okCallback;
	my okClosure = okClosure;
	my invokingButtonTitle = invokingButtonTitle;
	my helpTitle = helpTitle;
	my allowMultipleFiles = allowMultipleFiles;
	UiFile_init (me, parent, title);
	return (UiForm) me;
}

void UiInfile_do (I) {
	iam (UiInfile);
	try {
		autoSortedSetOfString infileNames = GuiFileSelect_getInfileNames (my parent, my name, my allowMultipleFiles);
		for (long ifile = 1; ifile <= infileNames -> size; ifile ++) {
			SimpleString infileName = (SimpleString) infileNames -> item [ifile];
			Melder_pathToFile (infileName -> string, & my file);
			UiHistory_write (L"\n");
			UiHistory_write (my invokingButtonTitle);
			UiHistory_write (L" ");
			UiHistory_write (infileName -> string);
			structMelderFile file;
			MelderFile_copy (& my file, & file);
			try {
				my okCallback ((UiForm) me, NULL, NULL, my invokingButtonTitle, false, my okClosure);
			} catch (MelderError) {
				Melder_throw ("File ", & file, " not finished.");
			}
		}
	} catch (MelderError) {
		Melder_flushError (NULL);
	}
}

/********** WRITING A FILE **********/

Thing_define (UiOutfile, UiFile) {
	// new data:
	public:
		bool (*allowExecutionHook) (void *closure);
		void *allowExecutionClosure;   // I am owner (see destroy)
};

Thing_implement (UiOutfile, UiFile, 0);

UiForm UiOutfile_create (GuiObject parent, const wchar_t *title,
	void (*okCallback) (UiForm, const wchar *, Interpreter, const wchar *, bool, void *), void *okClosure, const wchar *invokingButtonTitle, const wchar_t *helpTitle)
{
	UiOutfile me = Thing_new (UiOutfile);
	my okCallback = okCallback;
	my okClosure = okClosure;
	my invokingButtonTitle = invokingButtonTitle;
	my helpTitle = helpTitle;
	UiFile_init (me, parent, title);
	my allowExecutionHook = theAllowExecutionHookHint;
	my allowExecutionClosure = theAllowExecutionClosureHint;
	return (UiForm) me;
}

static void commonOutfileCallback (UiForm sendingForm, const wchar *sendingString, Interpreter interpreter, const wchar *invokingButtonTitle, bool modified, void *closure) {
	EditorCommand command = (EditorCommand) closure;
	(void) invokingButtonTitle;
	(void) modified;
	command -> commandCallback (command -> d_editor, command, sendingForm, sendingString, interpreter); therror
}

UiForm UiOutfile_createE (EditorCommand cmd, const wchar_t *title, const wchar_t *invokingButtonTitle, const wchar_t *helpTitle) {
	Editor editor = (Editor) cmd -> d_editor;
	UiOutfile dia = (UiOutfile) UiOutfile_create (editor -> d_windowForm, title, commonOutfileCallback, cmd, invokingButtonTitle, helpTitle);
	dia -> command = cmd;
	return (UiForm) dia;   // BUG
}

void UiOutfile_do (I, const wchar_t *defaultName) {
	iam (UiOutfile);
	wchar_t *outfileName = GuiFileSelect_getOutfileName (NULL, my name, defaultName);
	if (outfileName == NULL) return;   // cancelled
	if (my allowExecutionHook && ! my allowExecutionHook (my allowExecutionClosure)) {
		Melder_flushError ("Dialog `%s' cancelled.", my name);
		return;
	}
	Melder_pathToFile (outfileName, & my file);
	structMelderFile file;
	MelderFile_copy (& my file, & file);   // save, because okCallback could destroy me
	UiHistory_write (L"\n");
	UiHistory_write (my invokingButtonTitle);
	try {
		my okCallback ((UiForm) me, NULL, NULL, my invokingButtonTitle, false, my okClosure);
	} catch (MelderError) {
		Melder_error_ ("File ", & file, " not finished.");
		Melder_flushError (NULL);
	}
	UiHistory_write (L" ");
	UiHistory_write (outfileName);
	Melder_free (outfileName);
}

/* End of file UiFile.cpp */
