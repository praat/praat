/* UiFile.cpp
 *
 * Copyright (C) 1992-2011,2013,2014,2015,2017 Paul Boersma
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

#include "UiP.h"
#include "Editor.h"

static void UiFile_init (UiForm me, GuiWindow parent, const char32 *title) {
	my d_dialogParent = parent;
	Thing_setName (me, title);
}

MelderFile UiFile_getFile (UiForm me) {
	return & my file;
}

/********** READING A FILE **********/

UiForm UiInfile_create (GuiWindow parent, const char32 *title,
	UiCallback okCallback, void *okClosure,
	const char32 *invokingButtonTitle, const char32 *helpTitle, bool allowMultipleFiles)
{
	autoUiForm me = Thing_new (UiForm);
	my okCallback = okCallback;
	my buttonClosure = okClosure;
	my invokingButtonTitle = Melder_dup (invokingButtonTitle);
	my helpTitle = helpTitle;
	my allowMultipleFiles = allowMultipleFiles;
	UiFile_init (me.get(), parent, title);
	return me.releaseToAmbiguousOwner();
}

void UiInfile_do (UiForm me) {
	try {
		autoStringSet infileNames = GuiFileSelect_getInfileNames (my d_dialogParent, my name, my allowMultipleFiles);
		for (long ifile = 1; ifile <= infileNames->size; ifile ++) {
			SimpleString infileName = infileNames->at [ifile];
			Melder_pathToFile (infileName -> string, & my file);
			UiHistory_write (U"\n");
			UiHistory_write_colonize (my invokingButtonTitle);
			UiHistory_write (U" \"");
			UiHistory_write_expandQuotes (infileName -> string);
			UiHistory_write (U"\"");
			structMelderFile file { };
			MelderFile_copy (& my file, & file);
			try {
				my okCallback (me, 0, nullptr, nullptr, nullptr, my invokingButtonTitle, false, my buttonClosure);
			} catch (MelderError) {
				Melder_throw (U"File ", & file, U" not finished.");
			}
		}
	} catch (MelderError) {
		Melder_flushError ();
	}
}

/********** WRITING A FILE **********/

UiForm UiOutfile_create (GuiWindow parent, const char32 *title,
	UiCallback okCallback, void *okClosure, const char32 *invokingButtonTitle, const char32 *helpTitle)
{
	autoUiForm me = Thing_new (UiForm);
	my okCallback = okCallback;
	my buttonClosure = okClosure;
	my invokingButtonTitle = Melder_dup (invokingButtonTitle);
	my helpTitle = helpTitle;
	UiFile_init (me.get(), parent, title);
	my allowExecutionHook = theAllowExecutionHookHint;
	my allowExecutionClosure = theAllowExecutionClosureHint;
	return me.releaseToAmbiguousOwner();
}

static void commonOutfileCallback (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString,
	Interpreter interpreter, const char32 * /* invokingButtonTitle */, bool /* modified */, void *closure)
{
	EditorCommand command = (EditorCommand) closure;
	command -> commandCallback (command -> d_editor, command, sendingForm, narg, args, sendingString, interpreter);
}

UiForm UiOutfile_createE (EditorCommand cmd, const char32 *title, const char32 *invokingButtonTitle, const char32 *helpTitle) {
	Editor editor = cmd -> d_editor;
	UiForm dia = UiOutfile_create (editor -> windowForm, title, commonOutfileCallback, cmd, invokingButtonTitle, helpTitle);
	dia -> command = cmd;
	return dia;
}

UiForm UiInfile_createE (EditorCommand cmd, const char32 *title, const char32 *invokingButtonTitle, const char32 *helpTitle) {
	Editor editor = cmd -> d_editor;
	UiForm dia = UiInfile_create (editor -> windowForm, title, commonOutfileCallback, cmd, invokingButtonTitle, helpTitle, false);
	dia -> command = cmd;
	return dia;
}

void UiOutfile_do (UiForm me, const char32 *defaultName) {
	char32 *outfileName = GuiFileSelect_getOutfileName (nullptr, my name, defaultName);
	if (! outfileName) return;   // cancelled
	if (my allowExecutionHook && ! my allowExecutionHook (my allowExecutionClosure)) {
		Melder_flushError (U"Dialog \"", my name, U"\" cancelled.");
		return;
	}
	Melder_pathToFile (outfileName, & my file);
	structMelderFile file { };
	MelderFile_copy (& my file, & file);   // save, because okCallback could destroy me
	UiHistory_write (U"\n");
	UiHistory_write_colonize (my invokingButtonTitle);
	try {
		my okCallback (me, 0, nullptr, nullptr, nullptr, my invokingButtonTitle, false, my buttonClosure);
	} catch (MelderError) {
		Melder_flushError (U"File ", & file, U" not finished.");
	}
	UiHistory_write (U" \"");
	UiHistory_write (outfileName);
	UiHistory_write (U"\"");
	Melder_free (outfileName);
}

/* End of file UiFile.cpp */
