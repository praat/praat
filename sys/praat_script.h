#ifndef _praat_script_h_
#define _praat_script_h_
/* praat_script.h
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

#include "Interpreter.h"

void praat_executeCommand (Interpreter me, wchar *command);
void praat_executeCommandFromStandardInput (const char *programName);
void praat_executeScriptFromFile (MelderFile file, const wchar *arguments);
void praat_executeScriptFromFileNameWithArguments (const wchar *nameAndArguments);
void praat_executeScriptFromText (wchar *text);
void praat_executeScriptFromDialog (Any dia);
void DO_praat_runScript (UiForm sendingForm, const wchar *sendingString, Interpreter interpreter_dummy, const wchar *invokingButtonTitle, bool modified, void *dummy);
void DO_RunTheScriptFromAnyAddedMenuCommand (UiForm sendingForm_dummy, const wchar *scriptPath, Interpreter interpreter_dummy, const wchar *invokingButtonTitle, bool modified, void *dummy);
void DO_RunTheScriptFromAnyAddedEditorCommand (Editor editor, const wchar *script);

/* End of file praat_script.h */
#endif
