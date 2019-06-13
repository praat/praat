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
#include "SVD.h"

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

#include "enums_getText.h"
#include "NMF_enums.h"
#include "enums_getValue.h"
#include "NMF_enums.h"

void structNMF :: v_info () {
	MelderInfo_writeLine (U"Number of rows: ", numberOfRows);
	MelderInfo_writeLine (U"Number of columns: ", numberOfColumns);
	MelderInfo_writeLine (U"Number of features: ", numberOfFeatures);
}

Thing_implement (NMF, Daata, 0);


autoNMF NMF_create (integer numberOfRows, integer numberOfColumns, integer numberOfFeatures) {
	try {
		autoNMF me = Thing_new (NMF);
		my numberOfRows = numberOfRows;
		my numberOfColumns = numberOfColumns;
		my numberOfFeatures = numberOfFeatures;
		my features = newMATzero (numberOfRows, numberOfFeatures);
		my weights = newMATzero (numberOfFeatures, numberOfColumns);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NMF not created.");
	}
}

static void MATmakeElementsNonNegative (MATVU const& m, int strategy) {
	for (integer irow = 1; irow <= m.nrow; irow ++)
		for (integer icol = 1; icol <= m.ncol; icol ++)
			if (m [irow][icol] < 0.0)
				m [irow] [icol] = strategy == 0 ? 0.0 : fabs (m [irow] [icol]);	
}

static void NMF_initializeFactorization_svd (NMF me, constMATVU const& data, kNMF_Initialization initializationMethod) {
	try {
		autoSVD thee = SVD_createFromGeneralMatrix (data);
		MATmakeElementsNonNegative (thy u.get(), 1);
		MATmakeElementsNonNegative (thy v.get(), 1);
		my features.get() <<= thy u.verticalBand (1, my numberOfFeatures);
		for (integer irow = 1; irow <= my numberOfFeatures; irow ++)
			my weights.row (irow) <<= thy d [irow]  *  thy v.row (irow);
		
	} catch (MelderError) {
		Melder_throw (me, U": cpuld not initialize by svd method.");
	}	
}

void NMF_initializeFactorization (NMF me, constMATVU const& data, kNMF_Initialization initializationMethod) {
	if (initializationMethod == kNMF_Initialization::RandomUniform) {
		double rmin = 0.0, rmax = 1.0;
		MATrandomUniform (my features.all(), rmin, rmax);
		MATrandomUniform (my weights.all(), rmin, rmax);
	} else {
		NMF_initializeFactorization_svd (me, data, initializationMethod);
	}
}

autoNMF NMF_createFromGeneralMatrix (constMATVU const& m, integer numberOfFeatures) {
	try {
		Melder_require (NUMisNonNegative (m),
			U"No matrix elements should be negative.");
		Melder_require (numberOfFeatures <= m.ncol,
			U"The number of features should not exceed the number of columns.");
		autoNMF me = NMF_create (m.nrow, m.ncol, numberOfFeatures);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NMF cannot be created.");
	}
}

double NMF_getEuclideanDistance (NMF me, constMATVU const& data) {
	Melder_require (data.nrow == my numberOfRows && data.ncol == my numberOfColumns,
		U"Dimensions should match.");
	autoMAT synthesis = NMF_synthesize (me);
	synthesis.get()  -=  data;
	double dist = NUMnorm (synthesis.get(), 2.0);
	return dist;
}

static double getMaximumChange (constMATVU const& m, MATVU const& m0, const double sqrteps) {
	double min = NUMmin (m0);
	double max = NUMmax (m0);
	double extremum1 = std::max (fabs (min), fabs (max));
	m0  -=  m;
	min = NUMmin (m0);
	max = NUMmax (m0);
	double extremum2 = std::max (fabs (min), fabs (max));
	double dmat = extremum2 / (sqrteps + extremum1);
	return dmat;
}

/*
	Calculating elementwise matrix multiplication, division and addition m = m0 .* (numerm ./(denom + eps))
	Set elements < zero_threshold to zero
*/

static const void update (MATVU const& m, constMATVU const& m0, constMATVU const& numer, constMATVU const& denom, double zeroThreshold, double maximum) {
	Melder_assert (m.nrow == m0.nrow && m.ncol == m0.ncol);
	Melder_assert (m.nrow == numer.nrow && m.ncol == numer.ncol);
	Melder_assert (m.nrow == denom.nrow && m.ncol == denom.ncol);
	/*
		The value 1e-9 is OK for matrices with values that are larger than 1.
		For matrices with very small values we have to scale the divByZeroAvoidance value
		otherwise the precision would suffer. 
		A scaling with the maximum value seems reasonable.
	*/
	const double divByZeroAvoidance = 1e-09 * ( maximum < 1.0 ? maximum : 1.0 );
	for (integer irow = 1; irow <= m.nrow; irow ++) 
		for (integer icol = 1; icol <= m.ncol; icol++) {
			if (m0 [irow] [icol] == 0.0 || numer [irow] [icol]  == 0.0) {
				m [irow] [icol] = 0.0;
			} else {
				double update = m0 [irow] [icol] * (numer [irow] [icol] / (denom [irow] [icol] + divByZeroAvoidance));
				m [irow] [icol] = ( update < zeroThreshold ? 0.0 : update );
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
void NMF_improveFactorization_mu (NMF me, constMATVU const& data, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, bool info) {
	try {
		Melder_require (my numberOfColumns == data.ncol, U"The number of columns should be equal.");
		Melder_require (my numberOfRows == data.nrow, U"The number of rows should be equal.");
		
		autoMAT productFtD = newMATzero (my numberOfFeatures, my numberOfColumns); // calculations of F'D
		autoMAT productFtFW = newMATzero (my numberOfFeatures, my numberOfColumns); // calculations of F'F W
		autoMAT weights0 = newMATzero (my numberOfFeatures, my numberOfColumns);
		
		autoMAT productDWt = newMATzero (my numberOfRows, my numberOfFeatures); // calculations of DW'
		autoMAT productFWWt = newMATzero (my numberOfRows, my numberOfFeatures); // calculations of FWW'
		autoMAT features0 = newMATzero (my numberOfRows, my numberOfFeatures);
		
		autoMAT productWWt = newMATzero (my numberOfFeatures, my numberOfFeatures); // calculations of WW'
		autoMAT productFtF = newMATzero (my numberOfFeatures, my numberOfFeatures); // calculations of F'F
		
		double traceDtD = NUMtrace2 (data.transpose(), data); // for distance calculation
		features0.get() <<= my features.get();
		weights0.get() <<= my weights.get();
		
		if (! NUMfpp) NUMmachar ();
		const double eps = NUMfpp -> eps;
		const double sqrteps = sqrt (eps);
		double dnorm0 = 0.0;
		double maximum = NUMmax (data);
		integer iter = 1;
		bool convergence = false;		
		
		while (iter <= maximumNumberOfIterations && not convergence) {
			/*
				while iter < maxinter and not convergence
					(1) W = W .* (F'*D) ./ (F'*F*W + 10^^−9^)
					(2) F = F .* (D*W') ./ (F*W*W' + 10^^−9^)
					(3) test for convergence
				endwhile
			*/
			
			// 1. Update W matrix
			features0.get() <<= my features.get();
			weights0.get() <<= my weights.get();
			MATmul (productFtD.get(), features0.transpose(), data);
			MATmul (productFtF.get(), features0.transpose(), features0.get());
			MATmul (productFtFW.get(), productFtF.get(), weights0.get());
			update (my weights.get(), weights0.get(), productFtD.get(), productFtFW.get(), eps, maximum);

			// 2. Update F matrix
			MATmul (productDWt.get(), data, my weights.transpose()); // productDWt = data*weights'
			MATmul (productWWt.get(), my weights.get(), my weights.transpose()); // work1 = weights*weights'
			MATmul (productFWWt.get(), features0.get(), productWWt.get()); // productFWWt = features0 * work1
			update (my features.get(), features0.get(), productDWt.get(), productFWWt.get(), eps, maximum);
			
			/* 3. Convergence test:
				The Frobenius norm ||D-FW|| of a matrix can be written as
				||D-FW||=trace(D'D) − 2trace(W'F'D) + trace(W'F'FW)
						=trace(D'D) - 2trace(W'(F'D))+trace((F'F)(WW'))
				This saves us from explicitly calculating the reconstruction FW because we already have performed most of
				the needed matrix multiplications in the update step.
			*/
			
			double traceWtFtD  = NUMtrace2 (my weights.transpose(), productFtD.get());
			double traceWtFtFW = NUMtrace2 (productFtF.get(), productWWt.get());
			double distance = sqrt (std::max (traceDtD - 2.0 * traceWtFtD + traceWtFtFW, 0.0)); // just in case
			double dnorm = distance / (my numberOfRows * my numberOfColumns);
			double df = getMaximumChange (my features.get(), features0.get(), sqrteps);
			double dw = getMaximumChange (my weights.get(), weights0.get(), sqrteps);
			double delta = std::max (df, dw);
			
			convergence = ( iter > 1 && (delta < changeTolerance || dnorm < dnorm0 * approximationTolerance) );
			if (info)
				MelderInfo_writeLine (U"Iteration: ", iter, U", dnorm: ", dnorm, U", delta: ", delta);
			
			dnorm0 = dnorm;
			++ iter;
		}
		if (info)
			MelderInfo_drain();
	} catch (MelderError) {
		Melder_throw (me, U" factorization cannot be improved.");
	}
}

void NMF_improveFactorization_als (NMF me, constMATVU const& data, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, bool info) {
	try {
		Melder_require (my numberOfColumns == data.ncol, U"The number of columns should be equal.");
		Melder_require (my numberOfRows == data.nrow, U"The number of rows should be equal.");
		
		autoMAT productFtD = newMATzero (my numberOfFeatures, my numberOfColumns); // calculations of F'D
		autoMAT productWDt = newMATzero (my numberOfFeatures, my numberOfRows); // calculations of WD'
		
		autoMAT weights0 = newMATzero (my numberOfFeatures, my numberOfColumns);
		autoMAT features0 = newMATzero (my numberOfRows, my numberOfFeatures);

		autoMAT productFtF = newMATzero (my numberOfFeatures, my numberOfFeatures); // calculations of F'F
		autoMAT productWWt = newMATzero (my numberOfFeatures, my numberOfFeatures); // calculations of WW'
		
		autoSVD svd_WWt = SVD_create (my numberOfFeatures, my numberOfFeatures); // solving W*W'*F' = W*D'
		autoSVD svd_FtF = SVD_create (my numberOfFeatures, my numberOfFeatures); // solving F´*F*W = F'*D
				
		double traceDtD = NUMtrace2 (data.transpose(), data); // for distance calculation
		
		if (! NUMfpp) NUMmachar ();
		const double eps = NUMfpp -> eps;
		const double sqrteps = sqrt (eps);
		double dnorm0 = 0.0;
		double maximum = NUMmax (data);
		integer iter = 1;
		bool convergence = false;
		
		while (iter <= maximumNumberOfIterations && not convergence) {
			/*
				for iter to maxiter
					(1) W is solution of F´*F*W = F'*D.
					    Set all negative elements in W to 0.
					(2) F is solution of W*W'*F' = W*D' .
					    Set all negative elements in F to 0.
					(3) test for convergence
				endfor
			*/
			
			// 1. Solve equations for new W:  F´*F*W = F'*D.
			weights0.get() <<= my weights.get(); // save previous weigts for convergence test
			MATmul (productFtD.get(), my features.transpose(), data);
			MATmul (productFtF.get(), my features.transpose(), my features.get());

			svd_FtF -> u.get() <<= productFtF.get();
			SVD_compute (svd_FtF.get());
			SVD_solve_preallocated (svd_FtF.get(), productFtD.get(), my weights.get());
			MATmakeElementsNonNegative (my weights.get(), 0);
			
			// 2. Solve equations for new F:  W*W'*F' = W*D'
			features0.get() <<= my features.get(); // save previous features for convergence test
			MATmul  (productWDt.get(), my weights.get(), data.transpose());
			MATmul (productWWt.get(), my weights.get(), my weights.transpose());

			svd_WWt -> u.get() <<= productWWt.get();
			SVD_compute (svd_WWt.get());
			SVD_solve_preallocated (svd_WWt.get(), productWDt.get(), my features.transpose());

			// 3. Convergence test
			double traceWtFtD  = NUMtrace2 (my weights.transpose(), productFtD.get());
			double traceWtFtFW = NUMtrace2 (productFtF.get(), productWWt.get());
			double distance = sqrt (std::max (traceDtD - 2.0 * traceWtFtD + traceWtFtFW, 0.0)); // just in case
			double dnorm = distance / (my numberOfRows * my numberOfColumns);
			double df = getMaximumChange (my features.get(), features0.get(), sqrteps);
			double dw = getMaximumChange (my weights.get(), weights0.get(), sqrteps);
			double delta = std::max (df, dw);
			
			convergence = ( iter > 1 && (delta < changeTolerance || dnorm < dnorm0 * approximationTolerance) );
			if (info)
				MelderInfo_writeLine (U"Iteration: ", iter, U", dnorm: ", dnorm, U", delta: ", delta);
			dnorm0 = dnorm;
			++ iter;
		}
		if (info)
			MelderInfo_drain();
	} catch (MelderError) {
		Melder_throw (me, U" ALS factorization cannot be improved.");
	}
}

autoMAT NMF_synthesize (NMF me) {
	try {
		autoMAT result = newMATmul (my features.get(), my weights.get());
		return result;
	} catch (MelderError) {
		Melder_throw (me, U": No matrix created.");
	}
}

/* End of file NMF.cpp */
