#ifndef _Interpreter_h_
#define _Interpreter_h_
/* Interpreter.h
 *
 * Copyright (C) 1993-2011 Paul Boersma
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
		wchar *stringValue;
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
		wchar *environmentName;
		ClassInfo editorClass;
		int numberOfParameters, numberOfLabels, callDepth;
		wchar parameters [1+Interpreter_MAXNUM_PARAMETERS] [100];
		unsigned char types [1+Interpreter_MAXNUM_PARAMETERS];
		wchar *arguments [1+Interpreter_MAXNUM_PARAMETERS];
		wchar choiceArguments [1+Interpreter_MAXNUM_PARAMETERS] [100];
		wchar labelNames [1+Interpreter_MAXNUM_LABELS] [100];
		long labelLines [1+Interpreter_MAXNUM_LABELS];
		wchar dialogTitle [1+100], procedureNames [1+Interpreter_MAX_CALL_DEPTH] [100];
		SortedSetOfString variables;
		bool running, stopped;
	// overridden methods:
	protected:
		virtual void v_destroy ();
};

Interpreter Interpreter_create (wchar *environmentName, ClassInfo editorClass);
Interpreter Interpreter_createFromEnvironment (Editor editor);

void Melder_includeIncludeFiles (wchar **text);
long Interpreter_readParameters (Interpreter me, wchar *text);
Thing_declare (UiForm);
UiForm Interpreter_createForm (Interpreter me, GuiObject parent, const wchar_t *fileName,
	void (*okCallback) (UiForm sendingForm, const wchar *sendingString, Interpreter interpreter, const wchar *invokingButtonTitle, bool modified, void *closure), void *okClosure);
void Interpreter_getArgumentsFromDialog (Interpreter me, Any dialog);
void Interpreter_getArgumentsFromString (Interpreter me, const wchar *arguments);
void Interpreter_run (Interpreter me, wchar *text);   // destroys 'text'
void Interpreter_stop (Interpreter me);   // can be called from any procedure called deep-down by the interpreter; will stop before next line
void Interpreter_voidExpression (Interpreter me, const wchar *expression);
void Interpreter_numericExpression (Interpreter me, const wchar *expression, double *value);
void Interpreter_stringExpression (Interpreter me, const wchar *expression, wchar **value);
void Interpreter_numericArrayExpression (Interpreter me, const wchar *expression, struct Formula_NumericArray *value);
void Interpreter_anyExpression (Interpreter me, const wchar *expression, struct Formula_Result *result);

InterpreterVariable Interpreter_hasVariable (Interpreter me, const wchar *key);
InterpreterVariable Interpreter_lookUpVariable (Interpreter me, const wchar *key);

/* End of file Interpreter.h */
#endif
