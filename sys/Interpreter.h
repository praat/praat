#ifndef _Interpreter_h_
#define _Interpreter_h_
/* Interpreter.h
 *
 * Copyright (C) 1993-2018,2020 Paul Boersma
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

#include "Collection.h"
#include "Gui.h"
#include "Formula.h"

#include <string>
#include <unordered_map>

Thing_define (InterpreterVariable, SimpleString) {
	autostring32 stringValue;
	double numericValue;
	autoVEC numericVectorValue;
	autoMAT numericMatrixValue;
	autoSTRVEC stringArrayValue;
};

#define Interpreter_MAXNUM_PARAMETERS  400
#define Interpreter_MAXNUM_LABELS  1000
#define Interpreter_MAX_CALL_DEPTH  50
#define Interpreter_MAX_DIALOG_TITLE_LENGTH  100

#define Interpreter_MAX_LABEL_LENGTH  99

Thing_declare (UiForm);
Thing_declare (Editor);

Thing_define (Interpreter, Thing) {
	autostring32 environmentName;
	ClassInfo editorClass;
	int numberOfParameters, numberOfLabels, callDepth;
	char32 parameters [1+Interpreter_MAXNUM_PARAMETERS] [100];
	int types [1+Interpreter_MAXNUM_PARAMETERS];
	autostring32 arguments [1+Interpreter_MAXNUM_PARAMETERS];
	char32 choiceArguments [1+Interpreter_MAXNUM_PARAMETERS] [100];
	char32 labelNames [1+Interpreter_MAXNUM_LABELS] [1+Interpreter_MAX_LABEL_LENGTH];
	integer labelLines [1+Interpreter_MAXNUM_LABELS];
	char32 dialogTitle [1+Interpreter_MAX_DIALOG_TITLE_LENGTH], procedureNames [1+Interpreter_MAX_CALL_DEPTH] [100];
	std::unordered_map <std::u32string, autoInterpreterVariable> variablesMap;
	bool running, stopped;
};

autoInterpreter Interpreter_create (conststring32 environmentName, ClassInfo editorClass);
autoInterpreter Interpreter_createFromEnvironment (Editor editor);

void Melder_includeIncludeFiles (autostring32 *text);
integer Interpreter_readParameters (Interpreter me, mutablestring32 text);
Thing_declare (UiForm);
autoUiForm Interpreter_createForm (Interpreter me, GuiWindow parent, conststring32 fileName,
	void (*okCallback) (UiForm sendingForm, integer narg, Stackel args, conststring32 sendingString, Interpreter interpreter, conststring32 invokingButtonTitle, bool modified, void *closure), void *okClosure,
	bool selectionOnly);
void Interpreter_getArgumentsFromDialog (Interpreter me, UiForm dialog);
void Interpreter_getArgumentsFromString (Interpreter me, conststring32 arguments);
void Interpreter_getArgumentsFromArgs (Interpreter me, int nargs, Stackel args);
void Interpreter_run (Interpreter me, char32 *text);   // destroys 'text'
void Interpreter_stop (Interpreter me);   // can be called from any procedure called deep-down by the interpreter; will stop before next line

void Interpreter_voidExpression (Interpreter me, conststring32 expression);
void Interpreter_numericExpression (Interpreter me, conststring32 expression, double *p_value);
void Interpreter_numericVectorExpression (Interpreter me, conststring32 expression, VEC *p_value, bool *p_owned);
void Interpreter_numericMatrixExpression (Interpreter me, conststring32 expression, MAT *p_value, bool *p_owned);
autostring32 Interpreter_stringExpression (Interpreter me, conststring32 expression);
void Interpreter_stringArrayExpression (Interpreter me, conststring32 expression, STRVEC *out_value, bool *out_owned);
void Interpreter_anyExpression (Interpreter me, conststring32 expression, Formula_Result *p_result);

InterpreterVariable Interpreter_hasVariable (Interpreter me, conststring32 key);
InterpreterVariable Interpreter_lookUpVariable (Interpreter me, conststring32 key);

extern autoVEC theInterpreterNumvec;
extern autoMAT theInterpreterNummat;

/* End of file Interpreter.h */
#endif
