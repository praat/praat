/* praat_script.h
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2004/12/05 separated from praatP.h because of conflict between praat.h and EditorM.h
 */

#ifndef _Interpreter_h_
	#include "Interpreter.h"
#endif

int praat_executeCommand (Interpreter me, const char *command);
int praat_executeCommandFromStandardInput (const char *programName);
int praat_executeScriptFromFile (MelderFile file, const char *arguments);
int praat_executeScriptFromFileNameWithArguments (const char *nameAndArguments);
int praat_executeScriptFromText (char *text);
int praat_executeScriptFromDialog (Any dia);
int DO_praat_runScript (Any sender, void *dummy);
int DO_RunTheScriptFromAnyAddedMenuCommand (Any sender, void *dummy);
int DO_RunTheScriptFromAnyAddedEditorCommand (Any editor, const char *script);

/* End of file praat_script.h */
