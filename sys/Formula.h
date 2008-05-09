#ifndef _Formula_h_
#define _Formula_h_
/* Formula.h
 *
 * Copyright (C) 1990-2008 Paul Boersma
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
 * pb 2008/05/01
 */

#ifndef _Thing_h_
	#include "Thing.h"
#endif

#define kFormula_EXPRESSION_TYPE_NUMERIC  0
#define kFormula_EXPRESSION_TYPE_STRING  1
#define kFormula_EXPRESSION_TYPE_NUMERIC_ARRAY  2
#define kFormula_EXPRESSION_TYPE_STRING_ARRAY  3
#define kFormula_EXPRESSION_TYPE_UNKNOWN  4

struct Formula_NumericArray {
	long numberOfRows, numberOfColumns;
	double **data;
};

struct Formula_Result {
	int expressionType;
	union {
		double numericResult;
		wchar_t *stringResult;
		struct Formula_NumericArray numericArrayResult;
	} result;
};

int Formula_compile (Any interpreter, Any data, const wchar_t *expression, int expressionType, int optimize);

int Formula_run (long row, long col, struct Formula_Result *result);

/* End of file Formula.h */
#endif
