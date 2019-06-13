/* UiFile.cpp
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

static void UiFile_init (UiForm me, GuiWindow parent, conststring32 title) {
	my d_dialogParent = parent;
	Thing_setName (me, title);
}

MelderFile UiFile_getFile (UiForm me) {
	return & my file;
}

/********** READING A FILE **********/

autoUiForm UiInfile_create (GuiWindow parent, conststring32 title,
	UiCallback okCallback, void *okClosure,
	conststring32 invokingButtonTitle, conststring32 helpTitle, bool allowMultipleFiles)
{
	autoUiForm me = Thing_new (UiForm);
	my okCallback = okCallback;
	my buttonClosure = okClosure;
	my invokingButtonTitle = Melder_dup (invokingButtonTitle);
	my helpTitle = Melder_dup (helpTitle);
	my allowMultipleFiles = allowMultipleFiles;
	UiFile_init (me.get(), parent, title);
	return me;
}

void UiInfile_do (UiForm me) {
	try {
		autoStringSet infileNames = GuiFileSelect_getInfileNames (my d_dialogParent, my name.get(), my allowMultipleFiles);
		for (integer ifile = 1; ifile <= infileNames->size; ifile ++) {
			SimpleString infileName = infileNames->at [ifile];
			Melder_pathToFile (infileName -> string.get(), & my file);
			UiHistory_write (U"\n");
			UiHistory_write_colonize (my invokingButtonTitle.get());
			UiHistory_write (U" \"");
			UiHistory_write_expandQuotes (infileName -> string.get());
			UiHistory_write (U"\"");
			structMelderFile file { };
			MelderFile_copy (& my file, & file);
			try {
				my okCallback (me, 0, nullptr, nullptr, nullptr, my invokingButtonTitle.get(), false, my buttonClosure);
			} catch (MelderError) {
				Melder_throw (U"File ", & file, U" not finished.");
			}
		}
	} catch (MelderError) {
		Melder_flushError ();
	}
}

/********** WRITING A FILE **********/

autoUiForm UiOutfile_create (GuiWindow parent, conststring32 title,
	UiCallback okCallback, void *okClosure, conststring32 invokingButtonTitle, conststring32 helpTitle)
{
	autoUiForm me = Thing_new (UiForm);
	my okCallback = okCallback;
	my buttonClosure = okClosure;
	my invokingButtonTitle = Melder_dup (invokingButtonTitle);
	my helpTitle = Melder_dup (helpTitle);
	UiFile_init (me.get(), parent, title);
	my allowExecutionHook = theAllowExecutionHookHint;
	my allowExecutionClosure = theAllowExecutionClosureHint;
	return me;
}

static void commonOutfileCallback (UiForm sendingForm, integer narg, Stackel args, conststring32 sendingString,
	Interpreter interpreter, conststring32 /* invokingButtonTitle */, bool /* modified */, void *closure)
{
	EditorCommand command = (EditorCommand) closure;
	command -> commandCallback (command -> d_editor, command, sendingForm, narg, args, sendingString, interpreter);
}

autoUiForm UiOutfile_createE (EditorCommand cmd, conststring32 title, conststring32 invokingButtonTitle, conststring32 helpTitle) {
	Editor editor = cmd -> d_editor;
	autoUiForm dia = UiOutfile_create (editor -> windowForm, title, commonOutfileCallback, cmd, invokingButtonTitle, helpTitle);
	dia -> command = cmd;
	return dia;
}

autoUiForm UiInfile_createE (EditorCommand cmd, conststring32 title, conststring32 invokingButtonTitle, conststring32 helpTitle) {
	Editor editor = cmd -> d_editor;
	autoUiForm dia = UiInfile_create (editor -> windowForm, title, commonOutfileCallback, cmd, invokingButtonTitle, helpTitle, false);
	dia -> command = cmd;
	return dia;
}

void UiOutfile_do (UiForm me, conststring32 defaultName) {
	autostring32 outfileName = GuiFileSelect_getOutfileName (nullptr, my name.get(), defaultName);
	if (! outfileName) return;   // cancelled
	if (my allowExecutionHook && ! my allowExecutionHook (my allowExecutionClosure)) {
		Melder_flushError (U"Dialog \"", my name.get(), U"\" cancelled.");
		return;
	}
	Melder_pathToFile (outfileName.get(), & my file);
	structMelderFile file { };
	MelderFile_copy (& my file, & file);   // save, because okCallback could destroy me
	UiHistory_write (U"\n");
	UiHistory_write_colonize (my invokingButtonTitle.get());
	try {
		my okCallback (me, 0, nullptr, nullptr, nullptr, my invokingButtonTitle.get(), false, my buttonClosure);
	} catch (MelderError) {
		Melder_flushError (U"File ", & file, U" not finished.");
	}
	UiHistory_write (U" \"");
	UiHistory_write (outfileName.get());
	UiHistory_write (U"\"");
}

/* End of file UiFile.cpp */
