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
	bool owned;   // relevant only to numeric vector/matrix/tensor3/tensor4
	/*
		The following member of structStackel can either be a struct or a union.
		The struct option is the easy option: no special care will have to be taken when assigning to its members.
		The union option attempts to optimize for space and perhaps speed by putting the variants into a smaller space,
		but it requires special logic for initialization, deletion, and assignment.
		The special logic is partly implemented by encapsulating _string as a "private" member.
	*/
	#define STACKEL_VARIANTS_ARE_PACKED_IN_A_UNION  1
	#if STACKEL_VARIANTS_ARE_PACKED_IN_A_UNION
		union
	#else
		struct
	#endif
	{
		double number;
		autostring32 _string;
		Daata object;
		numvec numericVector;
		nummat numericMatrix;
		InterpreterVariable variable;
	};
	structStackel () {
		memset (this, 0, sizeof (structStackel));   // union-safe zeroing of all members of structStackel
		Melder_assert (our which == Stackel_NUMBER);   // check that on this computer, 0 is represented with zero bits only
		Melder_assert (our number == 0.0);   // check that on this computer, 0.0 is represented with zero bits only
		Melder_assert (! our _string);   // check that on this computer, a plain-old-data null pointer is represented with zero bits only
		Melder_assert (! our object);   // check that on this computer, a class null pointer is represented with zero bits only
		Melder_assert (! our owned);   // check that on this computer, false is represented with zero bits only
	}
	void reset () {   // union-safe destruction: test which variant we have
		if (our which <= Stackel_NUMBER)
			return;
		if (our which == Stackel_STRING) {
			our _string. reset();
		} else if (our which == Stackel_NUMERIC_VECTOR) {
			if (our owned)
				our numericVector. reset();   // ambiguous owner happens to own; this is the reason why a numvec has a reset() method
		} else if (our which == Stackel_NUMERIC_MATRIX) {
			if (our owned)
				our numericMatrix. reset();   // ambiguous owner happens to own; this is the reason why a nummat has a reset() method
		}
	}
	~structStackel () {   // union-safe destruction: test which variant we have
		our reset();
	}
	structStackel& operator= (structStackel&& other) noexcept {   // generalized move assignment
		if (& other != this) {
			our reset();
			memmove (this, & other, sizeof (structStackel));   // union-safe: even our biggest variant is bit-copied entirely
			memset (& other, 0, sizeof (structStackel));   // union-safe: even the biggest variant in `other` is erased
		}
		return *this;
	}
	conststring32 getString () {
		return our _string.get();
	}
	autostring32 moveString () {
		return our _string.move();
	}
	void setString (autostring32 newString) {
		our reset();
		#if STACKEL_VARIANTS_ARE_PACKED_IN_A_UNION
			our _string. _zero_asInUnion();
		#endif
		our which = Stackel_STRING;
		our _string = newString.move();
	}
	conststring32 whichText ();
} *Stackel;

struct Formula_Result {
	int expressionType;
	double numericResult;
	autostring32 stringResult;
	numvec numericVectorResult;
	nummat numericMatrixResult;
	bool owned;
	Formula_Result () {
		our expressionType = kFormula_EXPRESSION_TYPE_NUMERIC;
		our numericResult = 0.0;
		our stringResult = autostring32();
		our numericVectorResult = empty_numvec;
		our numericMatrixResult = empty_nummat;
		our owned = false;
	}
	void reset () {
		our stringResult. reset();
		if (our owned) {
			our numericVectorResult. reset();
			our numericMatrixResult. reset();
		}
	}
	~ Formula_Result () {
		our reset();
	}
};

Thing_declare (Interpreter);

void Formula_compile (Interpreter interpreter, Daata data, conststring32 expression, int expressionType, bool optimize);

void Formula_run (integer row, integer col, Formula_Result *result);

/* End of file Formula.h */
#endif
