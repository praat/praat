/* SVD.cpp
 *
 * Copyright (C) 1994-2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20010719
 djmw 20020408 GPL + cosmetic changes.
 djmw 20020415 +SVD_synthesize.
 djmw 20030624 Removed NRC svd calls.
 djmw 20030825 Removed praat_USE_LAPACK external variable.
 djmw 20031018 Removed  bug in SVD_solve that caused incorrect output when nrow > ncol
 djmw 20031101 Changed documentation in SVD_compute + bug correction in SVD_synthesize.
 djmw 20031111 Added GSVD_create_d.
 djmw 20051201 Adapt for numberOfRows < numberOfColumns
 djmw 20060810 Removed #include praat.h
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20070102 Removed the #include "TableOfReal.h"
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20110304 Thing_new
*/

#include "SVD.h"
#include <algorithm>
#include "NUMmachar.h"
#include "Collection.h"
#include "../melder/melder.h"
#include "NUMlapack.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "SVD_def.h"
#include "oo_COPY.h"
#include "SVD_def.h"
#include "oo_EQUAL.h"
#include "SVD_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SVD_def.h"
#include "oo_WRITE_TEXT.h"
#include "SVD_def.h"
#include "oo_WRITE_BINARY.h"
#include "SVD_def.h"
#include "oo_READ_TEXT.h"
#include "SVD_def.h"
#include "oo_READ_BINARY.h"
#include "SVD_def.h"
#include "oo_DESCRIPTION.h"
#include "SVD_def.h"

void structSVD :: v_info () {
	MelderInfo_writeLine (U"Number of rows: ", numberOfRows);
	MelderInfo_writeLine (U"Number of columns: ", numberOfColumns);
	MelderInfo_writeLine (U"This matrix is", (isTransposed ? U"" : U" not "), U" transposed.");
}

Thing_implement (SVD, Daata, 1);

/*
	m >=n, mxn matrix A has svd UDV', where u is mxn, D is n and V is nxn.
	m < n, then transpose A. Consider A' with svd (UDV')'= VDU', where v is mxm, D is m and U' is mxn
*/
void SVD_init (SVD me, integer numberOfRows, integer numberOfColumns) {
	if (numberOfRows < numberOfColumns) {
		my isTransposed = true;
		std::swap (numberOfRows, numberOfColumns);
	}
	my numberOfRows = numberOfRows;
	my numberOfColumns = numberOfColumns;
	if (! NUMfpp)
		NUMmachar ();
	my tolerance = NUMfpp -> eps * numberOfRows;
	my u = zero_MAT (numberOfRows,  numberOfColumns);
	my v = zero_MAT (numberOfColumns, numberOfColumns);
	my d = zero_VEC (numberOfColumns);
}

autoSVD SVD_create (integer numberOfRows, integer numberOfColumns) {
	try {
		autoSVD me = Thing_new (SVD);
		SVD_init (me.get(), numberOfRows, numberOfColumns);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SVD not created.");
	}
}

autoSVD SVD_createFromGeneralMatrix (constMATVU const& m) {
	try {
		autoSVD me = SVD_create (m.nrow, m.ncol);
		my u.all()  <<=  ( my isTransposed ? m.transpose() : m );
		SVD_compute (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"SVD not created from general matrix.");
	}
}


void SVD_update (SVD me, constMATVU const& m) {
	Melder_assert ((! my isTransposed && my numberOfRows == m.nrow && my numberOfColumns == m.ncol) ||
		(my isTransposed && my numberOfRows == m.ncol && my numberOfColumns == m.nrow));
	my u.all()  <<=  ( my isTransposed ? m.transpose() : m );
	SVD_compute (me);
}

void SVD_setTolerance (SVD me, double tolerance) {
	my tolerance = tolerance;
}

double SVD_getTolerance (SVD me) {
	return my tolerance;
}

void SVD_compute (SVD me) {
	try {
		autoMAT a = copy_MAT (my u.get());
		integer m = my numberOfColumns; // number of rows of input matrix
		integer n = my numberOfRows; // number of columns of input matrix
		double wtmp;
		integer lwork = -1, info;
		NUMlapack_dgesvd_ ("S", "O", m, n, & my u [1] [1], m, & my d [1], & my v [1] [1], m, nullptr, m, & wtmp, lwork, & info);
		Melder_require (info == 0,
			U"NUMlapack_dgesvd_ query returns error ", info, U".");
		
		lwork =  Melder_roundUp (wtmp);
		autoVEC work = raw_VEC (lwork);
		NUMlapack_dgesvd_ ("S", "O", m, n, & my u [1] [1], m, & my d [1], & my v [1] [1], m, nullptr, m, & work [1], lwork, & info);		
		Melder_require (info == 0,
			U"NUMlapack_dgesvd_ returns error ", info, U".");
		/*
			Because we store the eigenvectors row-wise, they must be transposed
		*/
		transpose_mustBeSquare_MAT_inout (my v.get());
	} catch (MelderError) {
		Melder_throw (me, U": SVD could not be computed.");
	}
}

// V D^2 V'or V D^-2 V
void SVD_getSquared_preallocated (SVD me, bool inverse, MAT const& m) {
	Melder_assert (m.nrow == m.ncol && m.ncol == my numberOfColumns);
	for (integer i = 1; i <= my numberOfColumns; i ++) {
		for (integer j = i; j <= my numberOfColumns; j ++) {
			longdouble val = 0.0;
			for (integer k = 1; k <= my numberOfColumns; k ++) {
				if (my d [k] > 0.0) {
					const longdouble dsq = my d [k] * my d [k];
					const longdouble factor = ( inverse ? 1.0 / dsq : dsq );
					val += my v [i] [k] * my v [j] [k] * factor;
				}
			}
			m [i] [j] = m [j] [i] = double (val);
		}
	}
}

autoMAT SVD_getSquared (SVD me, bool inverse) {
	autoMAT result = raw_MAT (my numberOfColumns, my numberOfColumns);
	SVD_getSquared_preallocated (me, inverse, result.get());
	return result;
}

void SVD_solve_preallocated (SVD me, constVECVU const& b, VECVU const& result) {
	try {
		/*
			Solve UDV' x = b.
			Solution: x = V D^-1 U' b
		*/
		autoVEC t = zero_VEC (my numberOfColumns);
		if (! my isTransposed) {
			Melder_assert (my numberOfRows == b.size);
			Melder_assert (result.size == my numberOfColumns);
			for (integer j = 1; j <= my numberOfColumns; j ++)
				if (my d [j] > 0.0)
					t [j] = NUMinner (my u.column (j), b) / my d [j];
			mul_VEC_out (result, my v.get(), t.get());
		} else {
			/*
				Solve (UDV')' x = b or VDU' x = b.
				Solution: x = UD^(-1)V'b
			*/
			Melder_assert (my numberOfColumns == b.size);
			Melder_assert (result.size == my numberOfRows);
			for (integer i = 1; i <= my numberOfColumns; i ++)
				if (my d [i] > 0.0)
					t [i] = NUMinner (my v.column (i), b) / my d [i];
			for (integer i = 1; i <= my numberOfColumns; i ++)
				result [i] = NUMinner (my u.row (i), t.get());
		}
	} catch (MelderError) {
		Melder_throw (me, U": not solved.");
	}
}

autoVEC SVD_solve (SVD me, constVECVU const& b) {
	integer numberOfRows = my numberOfRows, numberOfColumns = my numberOfColumns;
	if (my isTransposed)
		std::swap (numberOfRows, numberOfColumns);
	Melder_assert (numberOfRows == b.size);
	autoVEC result = zero_VEC (numberOfColumns);
	SVD_solve_preallocated (me, b, result.get());
	return result;
}

/*
	Solve UDV' X = B.
*/
void SVD_solve_preallocated (SVD me, constMATVU const& b, MATVU const& result) {
	Melder_assert (b.nrow == my numberOfRows && b.ncol == result.ncol);
	Melder_assert (result.nrow == my numberOfColumns);
	autoVEC bcol = raw_VEC (b.nrow);
	autoVEC resultcol = raw_VEC (result.nrow);
	for (integer icol = 1; icol <= b.ncol; icol ++) {
		bcol.all()  <<=  b.column (icol);
		SVD_solve_preallocated (me, bcol.get(), resultcol.get());
		result.column (icol) <<= resultcol.get();
	}
}

integer SVD_getMinimumNumberOfSingularValues (SVD me, double fractionOfSumOfSingularValues) {
	const double sumOfSingularValues = SVD_getSumOfSingularValues (me, 1, my numberOfColumns);
	const double criterion = sumOfSingularValues * fractionOfSumOfSingularValues;
	integer j = 1;
	longdouble sum = my d [1];
	while (sum < criterion && j < my numberOfColumns) {
		sum += my d [++ j];
	}
	return j;
}

void SVD_sort (SVD me) { // Superfluous??, SVD is always sorted
	try {
		autoSVD thee = Data_copy (me);
		autoINTVEC index = newINTVECindex (my d.get());

		for (integer j = 1; j <= my numberOfColumns; j ++) {
			const integer from = index [my numberOfColumns - j + 1];
			my d [j] = thy d [from];
			for (integer i = 1; i <= my numberOfRows; i ++)
				my u [i] [j] = thy u [i] [from];
			for (integer i = 1; i <= my numberOfColumns; i ++)
				my v [i] [j] = thy v [i] [from];
		}
	} catch (MelderError) {
		Melder_throw (me, U": not sorted.");
	}
}

double SVD_getConditionNumber (SVD me) {
	return my d[my numberOfColumns] > 0.0 ? my d[1] / my d[my numberOfColumns] : undefined;
}

double SVD_getSumOfSingularValuesAsFractionOfTotal (SVD me, integer from, integer to) {
	const double part = SVD_getSumOfSingularValues (me, from, to);
	const double total = SVD_getSumOfSingularValues (me, 1, my numberOfColumns);
	return part / total;
}

double SVD_getSumOfSingularValues (SVD me, integer from, integer to) {
	from = ( from == 0 ? 1 : from );
	to = ( to == 0 ? my numberOfColumns : to );
	Melder_require (from > 0 && from <= to && to <= my numberOfColumns,
		U"The range should be within [1,", my numberOfColumns, U"].");
	return NUMsum (my d.part (from, to));
}

integer SVD_zeroSmallSingularValues (SVD me, double tolerance) {
	if (tolerance == 0.0)
		tolerance = my tolerance;

	const double dmax = NUMmax (my d.all());

	integer numberOfZeroed = 0;
	for (integer i = 1; i <= my numberOfColumns; i ++)
		if (my d [i] < dmax * tolerance) {
			my d [i] = 0.0;
			numberOfZeroed ++;
		}
	return numberOfZeroed;
}


integer SVD_getRank (SVD me) {
	integer rank = 0;
	for (integer i = 1; i <= my numberOfColumns; i ++)
		if (my d [i] > 0.0)
			rank ++;
	return rank;
}

/*
	SVD of A = U D V'.
	This can be written as A = sum_{r=1}^n d[i] u[i]v[i]', where u[i] and [v[i] are columnvectors
	(Golub & van Loan, 3rd ed, p 71).
	If (internally) the matrix was transposed we can rewrite this as A=sum_{r=1}^n d[i] u[i]'v[i].
*/
autoMAT SVD_synthesize (SVD me, integer sv_from, integer sv_to) {
	if (sv_to == 0)
		sv_to = my numberOfColumns;
	try {
		Melder_require (sv_from > 0 && sv_from <= sv_to && sv_to <= my numberOfColumns,
			U"Indices must be in range [1, ", my numberOfColumns, U"].");
		long nrow = my numberOfRows;
		long ncol = my numberOfColumns;
		if (my isTransposed)
			std::swap (nrow, ncol);
		autoMAT result = zero_MAT (nrow, ncol);
		autoMAT outer = zero_MAT (nrow, ncol);

		for (integer k = sv_from; k <= sv_to; k ++) {
			if (my isTransposed)
				outer_MAT_out (outer.get(), my v.column(k), my u.column(k));
			else
				outer_MAT_out (outer.get(), my u.column(k), my v.row(k)); // because the transposed of v is in the svd!
			result.get()  +=  outer.get()  *  my d [k];
		}
		return result;
	} catch (MelderError) {
		Melder_throw (me, U": no synthesis.");
	}
}

Thing_implement (GSVD, Daata, 0);

void structGSVD :: v_info () {
	MelderInfo_writeLine (U"Number of columns: ", numberOfColumns);
}

autoGSVD GSVD_create (integer numberOfColumns) {
	try {
		autoGSVD me = Thing_new (GSVD);
		my numberOfColumns = numberOfColumns;

		my q = zero_MAT (numberOfColumns, numberOfColumns);
		my r = zero_MAT (numberOfColumns, numberOfColumns);
		my d1 = zero_VEC (numberOfColumns);
		my d2 = zero_VEC (numberOfColumns);
		return me;
	} catch (MelderError) {
		Melder_throw (U"GSVD not created.");
	}
}

autoGSVD GSVD_create (constMATVU const& m1, constMATVU const& m2) {
	try {
		const integer m = m1.nrow, n = m1.ncol, p = m2.nrow;

		// Store the matrices a and b as column major!
		autoMAT a = transpose_MAT (m1);
		autoMAT b = transpose_MAT (m2);
		autoMAT q = raw_MAT (n, n);
		autoVEC alpha = raw_VEC (n);
		autoVEC beta = raw_VEC (n);
		integer lwork = std::max (std::max (3 * n, m), p) + n;		
		autoVEC work = raw_VEC (lwork);
		autoINTVEC iwork = raw_INTVEC (n);

		integer k, l, info;
		NUMlapack_dggsvd_ ("N", "N", "Q", m, n, p, & k, & l,
		    & a [1] [1], m, & b [1] [1], p, & alpha [1], & beta [1], nullptr, m,
		    nullptr, p, & q [1] [1], n, & work [1], & iwork [1], & info);
		Melder_require (info == 0,
			U"dggsvd failed with error = ", info);

		const integer kl = k + l;
		autoGSVD me = GSVD_create (kl);

		for (integer i = 1; i <= kl; i ++) {
			my d1 [i] = alpha [i];
			my d2 [i] = beta [i];
		}

		transpose_MAT_out (my q.get(), q.get());
		/*
			Get R from a(1:k+l,n-k-l+1:n)
		*/
		const double *pr = & a [1] [1];
		for (integer i = 1; i <= kl; i ++) {
			for (integer j = i; j <= kl; j ++) {
				my r [i] [j] = pr [i - 1 + (n - kl + j - 1) * m]; /* from col-major */
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"GSVD not created.");
	}
}

void GSVD_setTolerance (GSVD me, double tolerance) {
	my tolerance = tolerance;
}

double GSVD_getTolerance (GSVD me) {
	return my tolerance;
}

/* End of file SVD.c */
