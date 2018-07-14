#ifndef _praat_script_h_
#define _praat_script_h_
/* praat_script.h
 *
 * Copyright (C) 1992-2005,2007,2009-2016,2018 Paul Boersma
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

#include "Interpreter.h"

int praat_executeCommand (Interpreter me, char32 *command);
void praat_executeCommandFromStandardInput (conststring32 programName);
void praat_executeScriptFromFile (MelderFile file, conststring32 arguments);
void praat_executeScriptFromFileName (conststring32 fileName, integer narg, Stackel args);
void praat_executeScriptFromFileNameWithArguments (conststring32 nameAndArguments);
void praat_executeScriptFromText (conststring32 text);
void praat_executeScriptFromDialog (UiForm dia);
extern "C" void praatlib_executeScript (const char *text8);
void DO_praat_runScript (UiForm sendingForm, integer narg, Stackel args, conststring32 sendingString, Interpreter interpreter_dummy, conststring32 invokingButtonTitle, bool modified, void *dummy);
void DO_RunTheScriptFromAnyAddedMenuCommand (UiForm sendingForm_dummy, integer narg, Stackel args, conststring32 scriptPath, Interpreter interpreter_dummy, conststring32 invokingButtonTitle, bool modified, void *dummy);
void DO_RunTheScriptFromAnyAddedEditorCommand (Editor editor, conststring32 script);

/* End of file praat_script.h */
#endif
