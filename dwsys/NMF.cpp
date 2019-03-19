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

void NMF_initialize (NMF me, constMAT data, kNMF_Initialization initializationMethod) {
	if (initializationMethod == kNMF_Initialization::RandomUniform) {
		double rmin = 0.0, rmax = 1.0;
		VEC weights = asvector (my weights.get()), features = asvector (my features.get());
		for (long i = 1; i <= my numberOfRows * my numberOfFeatures; i ++)
			features [i] = NUMrandomUniform (rmin, rmax);
		for (long i = 1; i <= my numberOfFeatures * my numberOfColumns; i ++)
			weights [i] = NUMrandomUniform (rmin, rmax);
	} else {
	}
}

autoNMF NMF_createFromGeneralMatrix (constMAT m, integer numberOfFeatures) {
	try {
		Melder_require (NUMcheckNonNegativity (asvector (m)) == 0, U"The matrix elements should not be negative.");
		Melder_require (numberOfFeatures <= m.ncol, U"The number of features should not exceed the number of columns.");
		autoNMF me = NMF_create (m.nrow, m.ncol, numberOfFeatures);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NMF cannot be created.");
	}
}

double NMF_getEuclideanDistance (NMF me, constMATVU data) {
	Melder_require (data.nrow == my numberOfRows && data.ncol == my numberOfColumns, U"Dimensions should match.");
	autoMAT synthesis = NMF_synthesize (me);
	synthesis.get()  -=  data;
	double dist = NUMnorm (synthesis.get(), 2.0);
	return dist;
}

static double getMaximumChange (constMAT m, MAT m0, const double sqrteps) {
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

static const void update (MAT m, constMAT m0, constMAT numer, constMAT denom, double zeroThreshold, double maximum) {
	Melder_assert (m.nrow == m0.nrow && m.ncol == m0.ncol);
	Melder_assert (m.nrow == numer.nrow && m.ncol == numer.ncol);
	Melder_assert (m.nrow == denom.nrow && m.ncol == denom.ncol);
	/*
		The value 1e-9 is OK for matrices with values that are larger than 1.
		For matrices with very small values we have to scale the divByZeroAvoidance value
		otherwise the precision would suffer. 
		A scaling with the maximum value seems reasonable.
	*/
	const double divByZeroAvoidance = 1e-09 * (maximum < 1 ? maximum : 1.0);
	for (integer irow = 1; irow <= m.nrow; irow ++) 
		for (integer icol = 1; icol <= m.ncol; icol++) {
			if ( m0 [irow] [icol] == 0.0 || numer [irow] [icol]  == 0.0)
				m [irow] [icol] = 0.0;
			else {
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
void NMF_improveFactorization_mu (NMF me, constMAT data, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance) {
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
				Lee & Seung (2001) update formulas (4)
				W[i,j] <- W[i,j] ((F'D)[i,j] / ((F'FW)[i,j])
				F[i,j] <- F[i,j] ((DW')[i,j]) / (FWW')[i,j]),
				where W, F, D are the weight matrix, the feature matrix and the data matrix, respectively.
			*/
			MATVUmul (productFtD.get(), features0.transpose(), data); // productFtD = features0'*data
			MATVUmul (productFtF.get(), features0.transpose(), features0.get()); // work1 = features0'*features0
			MATVUmul  (productFtFW.get(), productFtF.get(), weights0.get()); // productFtFW = work1 * weights0

			update (my weights.get(), weights0.get(), productFtD.get(), productFtFW.get(), eps, maximum);

			MATVUmul  (productDWt.get(), data, my weights.transpose()); // productDWt = data*weights'
			MATVUmul (productWWt.get(), my weights.get(), my weights.transpose()); // work1 = weights*weights'
			MATVUmul  (productFWWt.get(), features0.get(), productWWt.get()); // productFWWt = features0 * work1
			
			update (my features.get(), features0.get(), productDWt.get(), productFWWt.get(), eps, maximum);
			
			/*
				The Frobenius norm ||D-FW|| of a matrix can be written as
				||D-FW||=trace(D'D) − 2trace(W'F'D) + trace(W'F'FW)
						=trace(D'D) - 2trace(W'(F'D))+trace((F'F)(WW'))
				This saves us from explicitly calculating the reconstruction FW because we already have performed most of
				the needed matrix multiplications in the update step.
			*/
			
			double traceWtFtD  = NUMtrace2 (my weights.transpose(), productFtD.get());
			double traceWtFtFW = NUMtrace2 (productFtF.get(), productWWt.get());
			double distance = traceDtD -2.0 * traceWtFtD + traceWtFtFW;
			double dnorm = distance / (my numberOfRows * my numberOfColumns);
			double df = getMaximumChange (my features.get(), features0.get(), sqrteps);
			double dw = getMaximumChange (my weights.get(), weights0.get(), sqrteps);
			double delta = std::max (df, dw);
			
			convergence = ( iter > 1 && (delta < changeTolerance || dnorm < dnorm0 * approximationTolerance) );
			
			dnorm0 = dnorm;
			
			features0.get() <<= my features.get();
			weights0.get() <<= my weights.get();

			++ iter;
		}
	} catch (MelderError) {
		Melder_throw (me, U" factorization cannot be improved.");
	}
}

void NMF_makeWeightsNonnegative (NMF me, int /* strategy */) {
		for (integer irow = 1; irow <= my numberOfFeatures; irow++)
			for (integer icol = 1; icol <= my numberOfColumns; icol ++)
				if (my weights [irow] [icol] < 0.0)
					my weights [irow] [icol] = 0.0;
}

void NMF_makeFeaturesNonnegative (NMF me, int /* strategy */) {
		for (integer irow = 1; irow <= my numberOfRows; irow++)
			for (integer icol = 1; icol <= my numberOfFeatures; icol ++)
				if (my features [irow] [icol] < 0.0)
					my features [irow] [icol] = 0.0;
}

void NMF_improveFactorization_als (NMF me, constMAT data, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance) {
	try {
		Melder_require (my numberOfColumns == data.ncol, U"The number of columns should be equal.");
		Melder_require (my numberOfRows == data.nrow, U"The number of rows should be equal.");
		
		autoMAT productFtD = newMATzero (my numberOfFeatures, my numberOfColumns); // calculations of F'D
		autoMAT productWDt = newMATzero (my numberOfFeatures, my numberOfRows); // calculations of WD'
		
		autoMAT weights0 = newMATzero (my numberOfFeatures, my numberOfColumns);
		autoMAT features0 = newMATzero (my numberOfRows, my numberOfFeatures);

		
		autoMAT productFtF = newMATzero (my numberOfFeatures, my numberOfFeatures); // calculations of F'F
		autoMAT productWWt = newMATzero (my numberOfFeatures, my numberOfFeatures); // calculations of WW'
				
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
					W is solution of F´*F*W = F'*D.
					Set all negative elements in W to 0.
					F is solution of W*W'*F' = W*D' .
					Set all negative elements in F to 0.
				endfor
			*/
			weights0.get() <<= my weights.get(); // save previous weigts
			MATVUmul (productFtD.get(), my features.transpose(), data); // productFtD = features'*data
			MATVUmul (productFtF.get(), my features.transpose(), my features.get()); // work1 = features0'*features0
			// solve equations for new W
			// TODO more efficient
			autoMAT newweights = NUMsolveEquations (productFtF.get(), productFtD.get(), eps);
			my weights.get() <<= newweights.get();
			NMF_makeWeightsNonnegative (me, 0);
			
			features0.get() <<= my features.get(); // save previous features
			MATVUmul  (productWDt.get(), my weights.get(), data.transpose()); // productWDt = weights*data'
			MATVUmul (productWWt.get(), my weights.get(), my weights.transpose()); // work1 = weights*weights'
			// solve equations for new F 
			autoMAT newfeatures = NUMsolveEquations (productWWt.get(), productWDt.get(), eps);
			my features.get() <<= newfeatures.transpose();
			
			double traceWtFtD  = NUMtrace2 (my weights.transpose(), productFtD.get());
			double traceWtFtFW = NUMtrace2 (productFtF.get(), productWWt.get());
			double distance = traceDtD -2.0 * traceWtFtD + traceWtFtFW;
			double dnorm = distance / (my numberOfRows * my numberOfColumns);
			double df = getMaximumChange (my features.get(), features0.get(), sqrteps);
			double dw = getMaximumChange (my weights.get(), weights0.get(), sqrteps);
			double delta = std::max (df, dw);
			
			convergence = ( iter > 1 && (delta < changeTolerance || dnorm < dnorm0 * approximationTolerance) );
			
			dnorm0 = dnorm;
			++ iter;
		}
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
