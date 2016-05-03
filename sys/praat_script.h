#ifndef _praat_script_h_
#define _praat_script_h_
/* praat_script.h
 *
 * Copyright (C) 1992-2011,2013,2014,2015,2016 Paul Boersma
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
void praat_executeCommandFromStandardInput (const char32 *programName);
void praat_executeScriptFromFile (MelderFile file, const char32 *arguments);
void praat_executeScriptFromFileName (const char32 *fileName, int narg, Stackel args);
void praat_executeScriptFromFileNameWithArguments (const char32 *nameAndArguments);
void praat_executeScriptFromText (const char32 *text);
void praat_executeScriptFromDialog (UiForm dia);
extern "C" void praatlib_executeScript (const char *text8);
void DO_praat_runScript (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter_dummy, const char32 *invokingButtonTitle, bool modified, void *dummy);
void DO_RunTheScriptFromAnyAddedMenuCommand (UiForm sendingForm_dummy, int narg, Stackel args, const char32 *scriptPath, Interpreter interpreter_dummy, const char32 *invokingButtonTitle, bool modified, void *dummy);
void DO_RunTheScriptFromAnyAddedEditorCommand (Editor editor, const char32 *script);

/* End of file praat_script.h */
#endif
