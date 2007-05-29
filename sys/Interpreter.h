#ifndef _Interpreter_h_
#define _Interpreter_h_
/* Interpreter.h
 *
 * Copyright (C) 1993-2007 Paul Boersma
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
 * pb 2007/05/26
 */

#ifndef _Collection_h_
	#include "Collection.h"
#endif
#ifndef _Gui_h_
	#include "Gui.h"
#endif

#define InterpreterVariable_members Thing_members \
	char *key, *stringValue; \
	double numericValue;
#define InterpreterVariable_methods Thing_methods
class_create (InterpreterVariable, Thing);

#define Interpreter_MAXNUM_PARAMETERS  400
#define Interpreter_MAXNUM_LABELS  30
#define Interpreter_MAX_CALL_DEPTH  50

#define Interpreter_members Thing_members \
	char *environmentName; \
	Any editorClass; \
	int numberOfParameters, numberOfLabels, callDepth; \
	char parameters [1+Interpreter_MAXNUM_PARAMETERS] [50]; \
	unsigned char types [1+Interpreter_MAXNUM_PARAMETERS]; \
	char *arguments [1+Interpreter_MAXNUM_PARAMETERS]; \
	char choiceArguments [1+Interpreter_MAXNUM_PARAMETERS] [100]; \
	char labelNames [1+Interpreter_MAXNUM_LABELS] [50]; \
	long labelLines [1+Interpreter_MAXNUM_LABELS]; \
	char dialogTitle [1+100], procedureNames [1+Interpreter_MAX_CALL_DEPTH] [100]; \
	SortedSetOfString variables;
#define Interpreter_methods Thing_methods
class_create (Interpreter, Thing);

Interpreter Interpreter_create (char *environmentName, Any editorClass);
Interpreter Interpreter_createFromEnvironment (Any editor);

int Melder_includeIncludeFiles (char **text, int allocationMethod);
	/* text can be reallocated; methods: 1 = Melder_alloc, 2 = XtAlloc */
int Interpreter_readParameters (Interpreter me, char *text);
Any Interpreter_createForm (Interpreter me, Widget parent, const char *fileName,
	int (*okCallback) (Any dia, void *closure), void *okClosure);
int Interpreter_getArgumentsFromDialog (Interpreter me, Any dialog);
int Interpreter_getArgumentsFromString (Interpreter me, const char *arguments);
int Interpreter_run (Interpreter me, char *text);   /* Destroys 'text'. */
int Interpreter_numericExpression (Interpreter me, const char *expression, double *result);
int Interpreter_stringExpression (Interpreter me, const char *expression, char **result);
int Interpreter_numericOrStringExpression (Interpreter me, const char *expression, double *numericResult, char **stringResult);

InterpreterVariable Interpreter_hasVariable (Interpreter me, const char *key);
InterpreterVariable Interpreter_lookUpVariable (Interpreter me, const char *key);

/* End of file Interpreter.h */
#endif
