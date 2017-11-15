#ifndef _Formula_h_
#define _Formula_h_
/* Formula.h
 *
 * Copyright (C) 1990-2011,2013,2014,2015,2016,2017 Paul Boersma
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

#include "Data.h"
#include "tensor.h"

#define kFormula_EXPRESSION_TYPE_NUMERIC  0
#define kFormula_EXPRESSION_TYPE_STRING  1
#define kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR  2
#define kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX  3
#define kFormula_EXPRESSION_TYPE_NUMERIC_TENSOR3  4
#define kFormula_EXPRESSION_TYPE_NUMERIC_TENSOR4  5
#define kFormula_EXPRESSION_TYPE_STRING_ARRAY  6
#define kFormula_EXPRESSION_TYPE_UNKNOWN  7

Thing_declare (InterpreterVariable);

typedef struct structStackel {
	#define Stackel_NUMBER  0
	#define Stackel_STRING  1
	#define Stackel_NUMERIC_VECTOR  2
	#define Stackel_NUMERIC_MATRIX  3
	#define Stackel_NUMERIC_TENSOR3  4
	#define Stackel_NUMERIC_TENSOR4  5
	#define Stackel_STRING_ARRAY  6
	#define Stackel_VARIABLE  -1
	#define Stackel_OBJECT  -2
	int which;   // 0 or negative = no clean-up required, positive = requires clean-up
	union {
		double number;
		char32 *string;
		Daata object;
		numvec numericVector;
		nummat numericMatrix;
		InterpreterVariable variable;
	};
	bool owned;
} *Stackel;
const char32 *Stackel_whichText (Stackel me);

struct Formula_Result {
	int expressionType;
	union {
		double numericResult;
		char32 *stringResult;
		numvec numericVectorResult;
		nummat numericMatrixResult;
	};
	bool owned;
};

Thing_declare (Interpreter);

void Formula_compile (Interpreter interpreter, Daata data, const char32 *expression, int expressionType, bool optimize);

void Formula_run (integer row, integer col, Formula_Result *result);

/* End of file Formula.h */
#endif
