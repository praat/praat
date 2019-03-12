/* NMF.cpp
 *
 * Copyright (C) 2019 David Weenink
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

#include "NMF.h"
#include "NUMmachar.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "NMF_def.h"
#include "oo_COPY.h"
#include "NMF_def.h"
#include "oo_EQUAL.h"
#include "NMF_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "NMF_def.h"
#include "oo_WRITE_TEXT.h"
#include "NMF_def.h"
#include "oo_WRITE_BINARY.h"
#include "NMF_def.h"
#include "oo_READ_TEXT.h"
#include "NMF_def.h"
#include "oo_READ_BINARY.h"
#include "NMF_def.h"
#include "oo_DESCRIPTION.h"
#include "NMF_def.h"

void structNMF :: v_info () {
	MelderInfo_writeLine (U"Number of rows: ", numberOfRows);
	MelderInfo_writeLine (U"Number of columns: ", numberOfColumns);
	MelderInfo_writeLine (U"Dimension of approximation", dimensionOfApproximation);
}

Thing_implement (NMF, Daata, 0);


autoNMF NMF_create (integer numberOfRows, integer numberOfColumns, integer dimensionOfApproximation) {
	try {
		autoNMF me = Thing_new (NMF);
		my numberOfRows = numberOfRows;
		my numberOfColumns = numberOfColumns;
		my dimensionOfApproximation = dimensionOfApproximation;
		my w = newMATzero (numberOfRows,  dimensionOfApproximation);
		my h = newMATzero (dimensionOfApproximation, numberOfColumns);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NMF not created.");
	}
}

void NMF_initialize (NMF me, constMAT m, int initialisationMethod) {
	if (initialisationMethod != 0) {
		double rmin = 0.0, rmax = 1.0;
		VEC h = asvector (my h.get()), w = asvector (my w.get());
		for (long i = 1; i <= my numberOfRows * my dimensionOfApproximation; i ++)
			w [i] = NUMrandomUniform (rmin, rmax);
		for (long i = 1; i <= my dimensionOfApproximation * my numberOfColumns; i ++)
			h [i] = NUMrandomUniform (rmin, rmax);
	} else {
	}
}

autoNMF NMF_createFromGeneralMatrix_mu (constMAT m, integer dimensionOfApproximation) {
	try {
		Melder_require (NUMcheckNonNegativity (asvector (m)) == 0, U"The matrix elements should not be negative.");
		Melder_require (dimensionOfApproximation <= m.ncol, U"The dimension of approximation should not exceed the number of columns.");
		autoNMF me = NMF_create (m.nrow, m.ncol, dimensionOfApproximation);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NMF cannot be created.");
	}
}

static double getNorm (MAT d, constMAT a, constMAT w, constMAT h) {
	Melder_assert (a.nrow == d.nrow && a.ncol == d.ncol);
	Melder_assert (w.nrow == a.nrow && w.ncol == h.nrow);
	Melder_assert (a.ncol == h.ncol);
	MATVUmul_fast (d, w, h);
	d *= -1.0;
	d += a;
	double dnorm = NUMnorm (asvector (d), 2.0) / sqrt(d.nrow * d.ncol);
	return dnorm;
}

static double getMaximumChange (constMAT m, MAT m0, const double sqrteps) {
	double min = NUMmin (asvector (m0));
	double max = NUMmax (asvector (m0));
	double extremum1 = std::max (fabs (min), fabs (max));
	m0 -= m;
	min = NUMmin (asvector (m0));
	max = NUMmax (asvector (m0));
	double extremum2 = std::max (fabs (min), fabs (max));
	double dmat =  extremum2 / (sqrteps + extremum1);
	return dmat;
}

/*
	Calculating elementwise matrix multiplication, division and addition m = m0 .* (numerm ./(work2 + eps))
	Set elements < zero_threshold to zero
*/
const void MATupdate (MAT m, constMAT m0, constMAT numer, constMAT work, double eps) {
	Melder_assert (m.nrow == m0.nrow && m.ncol == m0.ncol);
	Melder_assert (m.nrow == numer.nrow && m.ncol == numer.ncol);
	Melder_assert (m.nrow == work.nrow && m.ncol == work.ncol);
	const double DIV_BY_ZERO_AVOIDANCE = 1E-09;
	const double ZERO_THRESHOLD = eps;
	for (integer irow = 1; irow <= m.nrow; irow ++) 
		for (integer icol = 1; icol <= m.ncol; icol++) {
			if ( m0 [irow] [icol] == 0.0 || numer [irow] [icol]  == 0.0)
				m [irow] [icol] = 0.0;
			else {
				double tmp = m0 [irow] [icol] * (numer [irow] [icol] / (work [irow] [icol] + DIV_BY_ZERO_AVOIDANCE));
				m [irow] [icol] = ( tmp < ZERO_THRESHOLD ? 0.0 : tmp );
			}
		}
}

/*
	Algorithm for Non-negative Matrix Factorization by multiplicative updates.
	The algoritm is inspired by the nmf_mu.c algorithm in libNMF by 
	A. Janecek, S. Schulze Grotthoff & W.N. Gangsterer (2011): 
		"LIBNMF - A library for nonnegative matrix factorization."
		Computing and informatics% #30: 205--224.

*/
void NMF_improveApproximation_mu (NMF me, constMAT a, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance) {
	try {
		Melder_require (my numberOfColumns == a.ncol, U"The number of columns should be equal.");
		Melder_require (my numberOfRows == a.nrow, U"The number of rows should be equal.");
		
		autoMAT work1 = newMATzero (my dimensionOfApproximation, my dimensionOfApproximation); // used for calculation of h & w
		
		autoMAT numerh = newMATzero (my dimensionOfApproximation, my numberOfColumns); // k x n
		autoMAT work2h = newMATzero (my dimensionOfApproximation, my numberOfColumns);
		autoMAT h0 = newMATzero (my dimensionOfApproximation, my numberOfColumns);
		
		autoMAT numerw = newMATzero (my numberOfRows, my dimensionOfApproximation); // m*k
		autoMAT work2w = newMATzero (my numberOfRows, my dimensionOfApproximation);
		autoMAT w0 = newMATzero (my numberOfRows, my dimensionOfApproximation);
		
		autoMAT d = newMATzero (my numberOfRows, my numberOfColumns); // d = a - w*h
		
		w0.get() <<= my w.get();
		h0.get() <<= my h.get();
		
		if (! NUMfpp) NUMmachar ();
		const double eps = NUMfpp -> eps;
		const double sqrteps = sqrt(eps);
		double dnorm0 = 0.0;
		bool convergence = false;		
		integer iter = 1;
		while (iter <= maximumNumberOfIterations && not convergence) {
			MATVUmul (numerh.get(), w0.transpose(), a); // numerh = w0'*a
			MATVUmul (work1.get(), w0.transpose(), w0.get()); // work1 = w0'*w0
			MATVUmul  (work2h.get(), work1.get(), h0.get()); // work2 = work1 * h0

			MATupdate (my h.get(), h0.get(), numerh.get(), work2h.get(), eps);

			MATVUmul  (numerw.get(), a, my h.transpose()); // numerw = a*h'
			MATVUmul (work1.get(), my h.get(), my h.transpose()); // work1 = h*h'
			MATVUmul  (work2w.get(), w0.get(), work1.get()); // work2w = w0 * work1
			
			MATupdate (my w.get(), w0.get(), numerw.get(), work2w.get(), eps);
		
			double dnorm = getNorm (d.get(), a, my w.get(), my h.get());
			double dw = getMaximumChange (my w.get(), w0.get(), sqrteps);
			double dh = getMaximumChange (my h.get(), h0.get(), sqrteps);
			double delta = std::max (dw, dh);
			
			convergence = ( iter > 1 && (delta < changeTolerance || dnorm < dnorm0 * approximationTolerance) );
			
			dnorm0 = dnorm;
			
			w0.get() <<= my w.get();
			h0.get() <<= my h.get();

			++ iter;
		}

	} catch (MelderError) {
		Melder_throw (me, U" cannot be improved.");
	}
}

/* End of file NMF.cpp */
