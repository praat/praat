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

/*
 * pb 2011/03/24
 */

#include "Interpreter.h"

#ifdef __cplusplus
	extern "C" {
#endif

int praat_executeCommand (Interpreter me, wchar_t *command);
int praat_executeCommandFromStandardInput (const char *programName);
int praat_executeScriptFromFile (MelderFile file, const wchar_t *arguments);
int praat_executeScriptFromFileNameWithArguments (const wchar_t *nameAndArguments);
int praat_executeScriptFromText (wchar_t *text);
int praat_executeScriptFromDialog (Any dia);
int DO_praat_runScript (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter_dummy, const wchar_t *invokingButtonTitle, bool modified, void *dummy);
int DO_RunTheScriptFromAnyAddedMenuCommand (UiForm sendingForm_dummy, const wchar_t *scriptPath, Interpreter interpreter_dummy, const wchar_t *invokingButtonTitle, bool modified, void *dummy);
int DO_RunTheScriptFromAnyAddedEditorCommand (Any editor, const wchar_t *script);

#ifdef __cplusplus
	}
#endif

/* End of file praat_script.h */
#endif
