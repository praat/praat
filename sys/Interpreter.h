#ifndef _Interpreter_h_
#define _Interpreter_h_
/* Interpreter.h
 *
 * Copyright (C) 1993-2011,2013 Paul Boersma
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

#include "Collection.h"
#include "Gui.h"
#include "Formula.h"

Thing_define (InterpreterVariable, SimpleString) {
	// new data:
	public:
		wchar_t *stringValue;
		double numericValue;
		struct Formula_NumericArray numericArrayValue;
	// overridden methods:
	protected:
		virtual void v_destroy ();
};

#define Interpreter_MAXNUM_PARAMETERS  400
#define Interpreter_MAXNUM_LABELS  1000
#define Interpreter_MAX_CALL_DEPTH  50

Thing_declare (UiForm);
Thing_declare (Editor);

Thing_define (Interpreter, Thing) {
	// new data:
	public:
		wchar_t *environmentName;
		ClassInfo editorClass;
		int numberOfParameters, numberOfLabels, callDepth;
		wchar_t parameters [1+Interpreter_MAXNUM_PARAMETERS] [100];
		unsigned char types [1+Interpreter_MAXNUM_PARAMETERS];
		wchar_t *arguments [1+Interpreter_MAXNUM_PARAMETERS];
		wchar_t choiceArguments [1+Interpreter_MAXNUM_PARAMETERS] [100];
		wchar_t labelNames [1+Interpreter_MAXNUM_LABELS] [100];
		long labelLines [1+Interpreter_MAXNUM_LABELS];
		wchar_t dialogTitle [1+100], procedureNames [1+Interpreter_MAX_CALL_DEPTH] [100];
		SortedSetOfString variables;
		bool running, stopped;
	// overridden methods:
	protected:
		virtual void v_destroy ();
};

Interpreter Interpreter_create (wchar_t *environmentName, ClassInfo editorClass);
Interpreter Interpreter_createFromEnvironment (Editor editor);

void Melder_includeIncludeFiles (wchar_t **text);
long Interpreter_readParameters (Interpreter me, wchar_t *text);
Thing_declare (UiForm);
UiForm Interpreter_createForm (Interpreter me, GuiWindow parent, const wchar_t *fileName,
	void (*okCallback) (UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *closure), void *okClosure);
void Interpreter_getArgumentsFromDialog (Interpreter me, Any dialog);
void Interpreter_getArgumentsFromString (Interpreter me, const wchar_t *arguments);
void Interpreter_run (Interpreter me, wchar_t *text);   // destroys 'text'
void Interpreter_stop (Interpreter me);   // can be called from any procedure called deep-down by the interpreter; will stop before next line
void Interpreter_voidExpression (Interpreter me, const wchar_t *expression);
void Interpreter_numericExpression (Interpreter me, const wchar_t *expression, double *value);
void Interpreter_stringExpression (Interpreter me, const wchar_t *expression, wchar_t **value);
void Interpreter_numericArrayExpression (Interpreter me, const wchar_t *expression, struct Formula_NumericArray *value);
void Interpreter_anyExpression (Interpreter me, const wchar_t *expression, struct Formula_Result *result);

InterpreterVariable Interpreter_hasVariable (Interpreter me, const wchar_t *key);
InterpreterVariable Interpreter_lookUpVariable (Interpreter me, const wchar_t *key);

/* End of file Interpreter.h */
#endif
