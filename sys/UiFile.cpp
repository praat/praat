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

/*
 * pb 2002/03/07 GPL
 * pb 2006/08/10 Windows: turned file selector into a modal dialog box
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2007/02/12 worked around a bug in Windows XP that caused Praat to crash
                 when the user moved the mouse pointer over a file in the Desktop of the second file selector
                 that was raised in Praat. The workaround is to temporarily disable file info tips.
 * pb 2007/03/23 new Editor API
 * pb 2007/05/30 wchar_t
 * pb 2009/01/18 arguments to UiForm callbacks
 * pb 2009/12/22 invokingButtonTitle
 * pb 2010/07/21 erased Motif stuff
 * pb 2010/07/26 split off GuiFileSelect.c
 * pb 2011/05/15 C++
 * pb 2011/07/11 C++
 */

#include "UiP.h"
#include "Editor.h"

Thing_declare1cpp (UiFile);
struct structUiFile : public structThing {
	EditorCommand command;
	GuiObject parent;
	structMelderFile file;
	const wchar *invokingButtonTitle, *helpTitle;
	void (*okCallback) (UiForm sendingForm, const wchar *sendingString, Interpreter interpreter, const wchar *invokingButtonTitle, bool modified, void *closure);
	void *okClosure;
	int shiftKeyPressed;
};
#define UiFile__methods(klas) Thing__methods(klas)
Thing_declare2cpp (UiFile, Thing);

static void classUiFile_destroy (I) {
	iam (UiFile);
	inherited (UiFile) destroy (me);
}

class_methods (UiFile, Thing) {
	class_method_local (UiFile, destroy)
	class_methods_end
}

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

Thing_declare1cpp (UiInfile);
struct structUiInfile : public structUiFile {
	bool allowMultipleFiles;
};
#define UiInfile__methods(klas) UiFile__methods(klas)
Thing_declare2cpp (UiInfile, UiFile);

class_methods (UiInfile, UiFile) {
	class_methods_end
}

UiForm UiInfile_create (GuiObject parent, const wchar_t *title,
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
	SortedSetOfString infileNames = GuiFileSelect_getInfileNames (my parent, my name, my allowMultipleFiles);
	if (infileNames == NULL) {
		Melder_flushError (NULL);
		return;
	}
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
			Melder_error_ ("File ", & file, " not finished.");
			Melder_flushError (NULL);
		}
	}
	forget (infileNames);
}

/********** WRITING A FILE **********/

Thing_declare1cpp (UiOutfile);
struct structUiOutfile : public structUiFile {
	bool (*allowExecutionHook) (void *closure);
	void *allowExecutionClosure;   // I am owner (see destroy)
};
#define UiOutfile__methods(klas) UiFile__methods(klas)
Thing_declare2cpp (UiOutfile, UiFile);

class_methods (UiOutfile, UiFile) {
	class_methods_end
}

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
	command -> commandCallback (command -> editor, command, sendingForm, sendingString, interpreter); therror
}

UiForm UiOutfile_createE (EditorCommand cmd, const wchar_t *title, const wchar_t *invokingButtonTitle, const wchar_t *helpTitle) {
	Editor editor = (Editor) cmd -> editor;
	UiOutfile dia = (UiOutfile) UiOutfile_create (editor -> dialog, title, commonOutfileCallback, cmd, invokingButtonTitle, helpTitle);
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
