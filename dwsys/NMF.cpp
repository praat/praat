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
		my features = newMATzero (numberOfRows,  numberOfFeatures);
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

autoNMF NMF_createFromGeneralMatrix_mu (constMAT m, integer numberOfFeatures) {
	try {
		Melder_require (NUMcheckNonNegativity (asvector (m)) == 0, U"The matrix elements should not be negative.");
		Melder_require (numberOfFeatures <= m.ncol, U"The number of features should not exceed the number of columns.");
		autoNMF me = NMF_create (m.nrow, m.ncol, numberOfFeatures);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NMF cannot be created.");
	}
}

static double NUMdistance (constMATVU m1, constMATVU m2) {
	Melder_assert (m1.nrow == m2.nrow && m1.ncol == m2.ncol);
	longdouble sumsq = 0.0;
	for (long irow = 1; irow <= m1.nrow; irow ++)
		for (long icol = 1; icol <= m1.ncol; icol ++) {
			double dif = m1 [irow][icol] - m2 [irow][icol];
			sumsq += dif * dif;
		}
	return sqrt (sumsq);
}

static double _NMF_getEuclideanDistance_preallocated (NMF me, constMATVU data, MAT buffer) {
	Melder_require (data.nrow == my numberOfRows && data.ncol == my numberOfColumns, U"Dimension of NMF and data should match.");
	Melder_require (data.nrow == buffer.nrow && data.ncol == buffer.ncol, U"Buffer has wrong dimensions.");
	MATVUmul (buffer, my features.get(), my weights.get());
	double dist = NUMdistance (buffer, data);
	return dist;
}

double NMF_getEuclideanDistance (NMF me, constMATVU data) {
	Melder_require (data.nrow == my numberOfRows && data.ncol == my numberOfColumns, U"Dimensions should match.");
	autoMAT buffer = newMATraw (my numberOfRows, my numberOfColumns);
	return _NMF_getEuclideanDistance_preallocated (me, data, buffer.get());
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
	Calculating elementwise matrix multiplication, division and addition m = m0 .* (numerm ./(denom + eps))
	Set elements < zero_threshold to zero
*/
const void MATupdate (MAT m, constMAT m0, constMAT numer, constMAT denom, double eps) {
	Melder_assert (m.nrow == m0.nrow && m.ncol == m0.ncol);
	Melder_assert (m.nrow == numer.nrow && m.ncol == numer.ncol);
	Melder_assert (m.nrow == denom.nrow && m.ncol == denom.ncol);
	const double DIV_BY_ZERO_AVOIDANCE = 1E-09;
	const double ZERO_THRESHOLD = eps;
	for (integer irow = 1; irow <= m.nrow; irow ++) 
		for (integer icol = 1; icol <= m.ncol; icol++) {
			if ( m0 [irow] [icol] == 0.0 || numer [irow] [icol]  == 0.0)
				m [irow] [icol] = 0.0;
			else {
				double update = m0 [irow] [icol] * (numer [irow] [icol] / (denom [irow] [icol] + DIV_BY_ZERO_AVOIDANCE));
				m [irow] [icol] = ( update < ZERO_THRESHOLD ? 0.0 : update );
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
		
		autoMAT numerw = newMATzero (my numberOfFeatures, my numberOfColumns);
		autoMAT denomw = newMATzero (my numberOfFeatures, my numberOfColumns);
		autoMAT weights0 = newMATzero (my numberOfFeatures, my numberOfColumns);
		
		autoMAT numerf = newMATzero (my numberOfRows, my numberOfFeatures);
		autoMAT denomf = newMATzero (my numberOfRows, my numberOfFeatures);
		autoMAT features0 = newMATzero (my numberOfRows, my numberOfFeatures);
		
		autoMAT work1 = newMATzero (my numberOfFeatures, my numberOfFeatures); // used for intermediate calculations
		autoMAT approximation = newMATzero (my numberOfRows, my numberOfColumns); // approximation = features * weights
		
		features0.get() <<= my features.get();
		weights0.get() <<= my weights.get();
		
		if (! NUMfpp) NUMmachar ();
		const double eps = NUMfpp -> eps;
		const double sqrteps = sqrt(eps);
		double dnorm0 = 0.0;
		bool convergence = false;		
		integer iter = 1;
		while (iter <= maximumNumberOfIterations && not convergence) {
			/*
				Lee & Seung (2001) update formulas (4)
				W[i,j] <- W[i,j] ((F'D)[i,j] / ((F'FW)[i,j])
				F[i,j] <- F[i,j] ((DW')[i,j]) / (FWW')[i,j]),
				where W, F, D are the weight matrix, the feature matrix and the data matrix, respectively.
			*/
			MATVUmul (numerw.get(), features0.transpose(), data); // numerw = features0'*data
			MATVUmul (work1.get(), features0.transpose(), features0.get()); // work1 = features0'*features0
			MATVUmul  (denomw.get(), work1.get(), weights0.get()); // denomw = work1 * weights0

			MATupdate (my weights.get(), weights0.get(), numerw.get(), denomw.get(), eps);

			MATVUmul  (numerf.get(), data, my weights.transpose()); // numerf = data*weights'
			MATVUmul (work1.get(), my weights.get(), my weights.transpose()); // work1 = weights*weights'
			MATVUmul  (denomf.get(), features0.get(), work1.get()); // denomf = features0 * work1
			
			MATupdate (my features.get(), features0.get(), numerf.get(), denomf.get(), eps);
			
			double distance = _NMF_getEuclideanDistance_preallocated (me, data, approximation.get());
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
		Melder_throw (me, U" cannot be improved.");
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
