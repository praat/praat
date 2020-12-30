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

#include "Graphics.h"
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


double MATgetDivergence_ItakuraSaito (constMATVU const& ref, constMATVU const& x) {
	Melder_assert (ref.nrow == x.nrow);
	Melder_assert (ref.ncol == x.ncol);
	double divergence = 0.0;
	for (integer irow = 1; irow <= ref.nrow; irow ++)
		for (integer icol = 1; icol <= ref.ncol; icol ++) {
			const double refval = ref [irow] [icol];
			if (refval == 0.0)
				return undefined;
			divergence += x [irow] [icol] / refval - log (x [irow] [icol] / refval) - 1.0;
		}
	return divergence;
}

void NMF_paintFeatures (NMF me, Graphics g, integer fromFeature, integer toFeature, integer fromRow, integer toRow, double minimum, double maximum, int amplitudeScale, int scaling, bool garnish) {
	fixUnspecifiedColumnRange (& fromFeature, & toFeature, my features.get());
	fixUnspecifiedRowRange (& fromRow, & toRow, my features.get());
	
	autoMAT part = copy_MAT (my features.part (fromRow, toRow, fromFeature, toFeature));
	
	if (minimum == 0.0 && maximum == 0.0) {
		minimum = NUMmin (part.get());
		maximum = NUMmax (part.get());
	}
	
	Graphics_setInner (g);
	Graphics_setWindow (g, fromFeature, toFeature, fromRow, toRow);
	

	Graphics_cellArray (g, my features.part (fromRow, toRow, fromFeature, toFeature), 
	fromFeature, toFeature, fromRow, toRow, minimum, maximum);
	Graphics_unsetInner (g);
	if (garnish)
		Graphics_drawInnerBox (g);
}

void NMF_paintWeights (NMF me, Graphics g, integer fromWeight, integer toWeight, integer fromRow, integer toRow, double minimum, double maximum, int amplitudeScale, int scaling, bool garnish) {
	fixUnspecifiedColumnRange (& fromWeight, & toWeight, my weights.get());
	fixUnspecifiedRowRange (& fromRow, & toRow, my weights.get());
	
	autoMAT part = copy_MAT (my weights.part (fromRow, toRow, fromWeight, toWeight));
	
	if (minimum == 0.0 && maximum == 0.0) {
		minimum = NUMmin (part.get());
		maximum = NUMmax (part.get());
	}
	
	Graphics_setInner (g);
	Graphics_setWindow (g, fromWeight, toWeight, fromRow, toRow);
	

	Graphics_cellArray (g, my weights.part (fromRow, toRow, fromWeight, toWeight), 
	fromWeight, toWeight, fromRow, toRow, minimum, maximum);
	Graphics_unsetInner (g);
	if (garnish)
		Graphics_drawInnerBox (g);
}

autoNMF NMF_create (integer numberOfRows, integer numberOfColumns, integer numberOfFeatures) {
	try {
		autoNMF me = Thing_new (NMF);
		my numberOfRows = numberOfRows;
		my numberOfColumns = numberOfColumns;
		my numberOfFeatures = numberOfFeatures;
		my features = zero_MAT (numberOfRows, numberOfFeatures);
		my weights = zero_MAT (numberOfFeatures, numberOfColumns);
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
		my features.all()  <<=  thy u.verticalBand (1, my numberOfFeatures);
		for (integer irow = 1; irow <= my numberOfFeatures; irow ++)
			my weights.row (irow) <<= thy d [irow]  *  thy v.row (irow);
		
	} catch (MelderError) {
		Melder_throw (me, U": cpuld not initialize by svd method.");
	}	
}

void NMF_initializeFactorization (NMF me, constMATVU const& data, kNMF_Initialization initializationMethod) {
	if (initializationMethod == kNMF_Initialization::RANDOM_UNIFORM) {
		const double rmin = 0.0, rmax = 1.0;
		randomUniform_MAT_out (my features.all(), rmin, rmax);
		randomUniform_MAT_out (my weights.all(), rmin, rmax);
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
	return NUMnorm (synthesis.get(), 2.0);
}

double NMF_getItakuraSaitoDivergence (NMF me, constMATVU const& data) {
	Melder_require (data.nrow == my numberOfRows && data.ncol == my numberOfColumns,
		U"Dimensions should match.");
	autoMAT synthesis = NMF_synthesize (me);
	return MATgetDivergence_ItakuraSaito (data, synthesis.get());
}

static double getMaximumChange (constMATVU const& m, MATVU const& m0, const double sqrteps) {
	double min = NUMmin (m0);
	double max = NUMmax (m0);
	const double extremum1 = std::max (fabs (min), fabs (max));
	m0  -=  m;
	min = NUMmin (m0);
	max = NUMmax (m0);
	const double extremum2 = std::max (fabs (min), fabs (max));
	const double dmat = extremum2 / (sqrteps + extremum1);
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
		Melder_require (my numberOfColumns == data.ncol,
			U"The number of columns should be equal.");
		Melder_require (my numberOfRows == data.nrow,
			U"The number of rows should be equal.");
		
		autoMAT productFtD = zero_MAT (my numberOfFeatures, my numberOfColumns); // calculations of F'D
		autoMAT productFtFW = zero_MAT (my numberOfFeatures, my numberOfColumns); // calculations of F'F W
		autoMAT weights0 = zero_MAT (my numberOfFeatures, my numberOfColumns);
		
		autoMAT productDWt = zero_MAT (my numberOfRows, my numberOfFeatures); // calculations of DW'
		autoMAT productFWWt = zero_MAT (my numberOfRows, my numberOfFeatures); // calculations of FWW'
		autoMAT features0 = zero_MAT (my numberOfRows, my numberOfFeatures);
		
		autoMAT productWWt = zero_MAT (my numberOfFeatures, my numberOfFeatures); // calculations of WW'
		autoMAT productFtF = zero_MAT (my numberOfFeatures, my numberOfFeatures); // calculations of F'F
		
		const double traceDtD = NUMtrace2 (data.transpose(), data); // for distance calculation
		features0.all()  <<=  my features.all();
		weights0.all() <<= my weights.all();
		
		if (! NUMfpp)
			NUMmachar ();
		
		const double eps = NUMfpp -> eps;
		const double sqrteps = sqrt (eps);
		const double maximum = NUMmax (data);
		double dnorm0 = 0.0;
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
			features0.all()  <<=  my features.all();
			weights0.all()  <<=  my weights.all();
			mul_MAT_out (productFtD.get(), features0.transpose(), data);
			mul_MAT_out (productFtF.get(), features0.transpose(), features0.get());
			mul_MAT_out (productFtFW.get(), productFtF.get(), weights0.get());
			update (my weights.get(), weights0.get(), productFtD.get(), productFtFW.get(), eps, maximum);

			// 2. Update F matrix
			mul_MAT_out (productDWt.get(), data, my weights.transpose()); // productDWt = data*weights'
			mul_MAT_out (productWWt.get(), my weights.get(), my weights.transpose()); // work1 = weights*weights'
			mul_MAT_out (productFWWt.get(), features0.get(), productWWt.get()); // productFWWt = features0 * work1
			update (my features.get(), features0.get(), productDWt.get(), productFWWt.get(), eps, maximum);
			
			/* 3. Convergence test:
				The Frobenius norm ||D-FW|| of a matrix can be written as
				||D-FW||=trace(D'D) − 2trace(W'F'D) + trace(W'F'FW)
						=trace(D'D) - 2trace(W'(F'D))+trace((F'F)(WW'))
				This saves us from explicitly calculating the reconstruction FW because we already have performed most of
				the needed matrix multiplications in the update step.
			*/
			
			const double traceWtFtD  = NUMtrace2 (my weights.transpose(), productFtD.get());
			const double traceWtFtFW = NUMtrace2 (productFtF.get(), productWWt.get());
			const double distance = sqrt (std::max (traceDtD - 2.0 * traceWtFtD + traceWtFtFW, 0.0)); // just in case
			const double dnorm = distance / (my numberOfRows * my numberOfColumns);
			const double df = getMaximumChange (my features.get(), features0.get(), sqrteps);
			const double dw = getMaximumChange (my weights.get(), weights0.get(), sqrteps);
			const double delta = std::max (df, dw);
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
		
		autoMAT productFtD = zero_MAT (my numberOfFeatures, my numberOfColumns); // calculations of F'D
		autoMAT productWDt = zero_MAT (my numberOfFeatures, my numberOfRows); // calculations of WD'
		
		autoMAT weights0 = zero_MAT (my numberOfFeatures, my numberOfColumns);
		autoMAT features0 = zero_MAT (my numberOfRows, my numberOfFeatures);

		autoMAT productFtF = zero_MAT (my numberOfFeatures, my numberOfFeatures); // calculations of F'F
		autoMAT productWWt = zero_MAT (my numberOfFeatures, my numberOfFeatures); // calculations of WW'
		
		autoSVD svd_WWt = SVD_create (my numberOfFeatures, my numberOfFeatures); // solving W*W'*F' = W*D'
		autoSVD svd_FtF = SVD_create (my numberOfFeatures, my numberOfFeatures); // solving F´*F*W = F'*D
				
		const double traceDtD = NUMtrace2 (data.transpose(), data); // for distance calculation
		
		if (! NUMfpp)
			NUMmachar ();
		const double eps = NUMfpp -> eps;
		const double sqrteps = sqrt (eps);
		double dnorm0 = 0.0;
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
			mul_MAT_out (productFtD.get(), my features.transpose(), data);
			mul_MAT_out (productFtF.get(), my features.transpose(), my features.get());

			svd_FtF -> u.all()  <<=  productFtF.all();
			SVD_compute (svd_FtF.get());
			SVD_solve_preallocated (svd_FtF.get(), productFtD.get(), my weights.get());
			MATmakeElementsNonNegative (my weights.get(), 0);
			
			// 2. Solve equations for new F:  W*W'*F' = W*D'
			features0.all()  <<=  my features.all(); // save previous features for convergence test
			mul_MAT_out  (productWDt.get(), my weights.get(), data.transpose());
			mul_MAT_out (productWWt.get(), my weights.get(), my weights.transpose());

			svd_WWt -> u.all()  <<=  productWWt.all();
			SVD_compute (svd_WWt.get());
			SVD_solve_preallocated (svd_WWt.get(), productWDt.get(), my features.transpose());

			// 3. Convergence test
			const double traceWtFtD  = NUMtrace2 (my weights.transpose(), productFtD.get());
			const double traceWtFtFW = NUMtrace2 (productFtF.get(), productWWt.get());
			const double distance = sqrt (std::max (traceDtD - 2.0 * traceWtFtD + traceWtFtFW, 0.0)); // just in case
			const double dnorm = distance / (my numberOfRows * my numberOfColumns);
			const double df = getMaximumChange (my features.get(), features0.get(), sqrteps);
			const double dw = getMaximumChange (my weights.get(), weights0.get(), sqrteps);
			const double delta = std::max (df, dw);
			
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

static void VECinvertAndScale (VECVU const& target, constVECVU const& source, double scaleFactor) {
	Melder_assert (target.size == source.size);
	for (integer i = 1; i <= target.size; i ++)
		target [i] = scaleFactor / source [i];
}

void NMF_improveFactorization_is (NMF me, constMATVU const& data, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, bool info) {
	try {
		Melder_require (my numberOfColumns == data.ncol, U"The number of columns should be equal.");
		Melder_require (my numberOfRows == data.nrow, U"The number of rows should be equal.");
		Melder_require (NUMhasZeroElement (data) == false,
			U"The data matrix should not have cells that are zero.");
		autoMAT vk = raw_MAT (data.nrow, data.ncol);
		autoMAT fw = raw_MAT (data.nrow, data.ncol);
		autoMAT fcol_x_wrow = raw_MAT (data.nrow, data.ncol);
		autoVEC fcolumn_inv = raw_VEC (data.nrow); // feature column
		autoVEC wrow_inv = raw_VEC (data.ncol); // weight row
		mul_MAT_out (fw.get(), my features.get(), my weights.get());
		double divergence = MATgetDivergence_ItakuraSaito (data, fw.get());
		const double divergence0 = divergence;
		if (info)
			MelderInfo_writeLine (U"Iteration: 0", U" divergence: ", divergence, U" delta: ", divergence);
		integer iter = 1;
		bool convergence = false;
		while (iter <= maximumNumberOfIterations && not convergence) {
			/*
				C. Févotte, N. Berin & J.-L. Durrieu (2009), Nonnegative matrix facorization with the Itakura-Saito divergene: with
					applications to music analysis, Neural Computation 21, 793--830.
				algorithm 2, page 806
				until convergence {
					for k to numberOfFeateres {
						G(k) = fcol(k) x wrow(k) / F.H                           (1)
						V(k) = G(k)^(.2).V+(1-G(k)).(fcol(k) x wrow(k))          (2)
						wrow(k) <-- (1/fcol(k))' . V(k) / numberOfRows           (3)
						fcol(k) <-- V(k).(1/wrow(k))' / numberOfColumns          (4)
						Normalize fcol(k) and wrow(k)                            (5)
						F.H - old(fcol(k) x wrow(k)) + new(fcol(k) x wrow(k))    (6)
					}
				}
				There is no need to calculate G(k) explicitly as in (1).
				We can calculate the elements of G(k) while we are doing (2).
			*/
			for (integer kf = 1; kf <= my numberOfFeatures; kf ++) {
				// (1) and (2)
				outer_MAT_out (fcol_x_wrow.get(), my features.column (kf), my weights.row (kf));
				for (integer irow = 1; irow <= data.nrow; irow ++)
					for (integer icol = 1; icol <= data.ncol; icol ++) {
						double gk = fcol_x_wrow [irow] [icol] / fw [irow] [icol];
						vk [irow] [icol] = gk * gk * data [irow] [icol] + (1.0 - gk) * fcol_x_wrow [irow] [icol];
					}
				// (3)
				VECinvertAndScale (fcolumn_inv.get(), my features.column (kf), 1.0 / my numberOfRows);	
				mul_VEC_out (my weights.row (kf), fcolumn_inv.get(), vk.get());
				// (4)
				VECinvertAndScale (wrow_inv.get(), my weights.row (kf), 1.0 / my numberOfColumns);
				mul_VEC_out (my features.column (kf), vk.get(), wrow_inv.get());
				// (5)
				double fcolumn_norm = NUMnorm (my features.column (kf), 2.0);
				my features.column (kf)  /=  fcolumn_norm;
				my weights.row (kf)  *=  fcolumn_norm;
				// (6)
				fw.get()  -=  fcol_x_wrow.get();
				outer_MAT_out (fcol_x_wrow.get(), my features.column (kf), my weights.row (kf));
				fw.get()  +=  fcol_x_wrow.get();
			}
			const double divergence_update = MATgetDivergence_ItakuraSaito (data, fw.get());
			const double delta = divergence - divergence_update;
			convergence = ( iter > 1 && (fabs (delta) < changeTolerance || divergence_update < divergence0 * approximationTolerance) );
			if (info)
				MelderInfo_writeLine (U"Iteration: ", iter, U" divergence: ", divergence_update, U" delta: ", delta);
			++ iter;
			divergence = divergence_update;
		}
		if (info)
			MelderInfo_drain();
		
	} catch (MelderError) {
		Melder_throw (me, U" IS factorization cannot be improved.");
	}
}

autoMAT NMF_synthesize (NMF me) {
	try {
		autoMAT result = mul_MAT (my features.get(), my weights.get());
		return result;
	} catch (MelderError) {
		Melder_throw (me, U": No matrix created.");
	}
}

/* End of file NMF.cpp */
