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

#define DIV_BY_ZERO_AVOIDANCE 1E-09

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

void NMF_initializeApproximation (NMF me, constMAT m, int initialisationMethod) {
	
}

autoNMF NMF_createFromGeneralMatrix_mu (constMAT m, integer dimensionOfApproximation, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, int initialisationMethod) {
	try {
		autoNMF me = Thing_new (NMF);
		NMF_initializeApproximation (me.get(), m, initialisationMethod);
		NMF_improveApproximation_mu (me.get(), m, maximumNumberOfIterations, changeTolerance, approximationTolerance);
	} catch (MelderError) {
		Melder_throw (U"NMF cannot be created.");
	}
}

static double getNorm (constMAT a, constMAT w, constMAT h, MAT d) {
	Melder_assert (a.nrow == d.nrow && a.ncol == d.ncol);
	MATVUmul_fast (d, w, h);
	d *= -1.0;
	d += a;
	double dnorm = NUMnorm (asvector (d), 2.0) / (d.nrow * d.ncol);
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
	double dmat =  extremum2/ (sqrteps + extremum1);
	return dmat;
}


void NMF_improveApproximation_mu (NMF me, constMAT a, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance) {
	try {
		autoMAT numerh = newMATzero (my dimensionOfApproximation, my numberOfColumns); // k x n
		autoMAT work1 = newMATzero (my dimensionOfApproximation, my dimensionOfApproximation); // used for calculation of h & w
		autoMAT work2 = newMATzero (my dimensionOfApproximation, my numberOfColumns);
		autoMAT h0, h = newMATzero (my dimensionOfApproximation, my numberOfColumns); // k*n;

		autoMAT numerw = newMATzero (my numberOfRows, my dimensionOfApproximation); // m*k
		autoMAT work2w = newMATzero (my numberOfRows, my dimensionOfApproximation); // m*k;
		autoMAT w0, w = newMATzero (my numberOfRows, my dimensionOfApproximation); // m*k;
		
		autoMAT d = newMATzero (my numberOfRows, my numberOfColumns);	//d = a - w*h
		
		integer k = my dimensionOfApproximation, m = my numberOfRows, n = my numberOfColumns;
		
		if (! NUMfpp) NUMmachar ();
		const double eps = NUMfpp -> eps;
		const double sqrteps = sqrt(eps);
		double dnorm0 = 0.0;
		const double ZERO_THRESHOLD = eps;
		long iter = 1;
		bool convergence = false;
		
		w0.get() <<= my w.get();
		h0.get() <<= my h.get();
		
		while (iter <= maximumNumberOfIterations && not convergence) {
			MATVUmul_fast (numerh.get(), w0.transpose(), a); // numerh = w0'*a
			MATmtm_preallocated (work1.get(), w0.transpose()); // work1 = w0'*w0
			MATVUmul_fast  (work2.get(), work1.get(), h0.get()); // work2 = work1 * h0


			//calculating elementwise matrixmultiplication, Division and addition h = h0 .* (numerh ./(work2 + eps))
			//set elements < zero_threshold to zero
			VEC h0_v = asvector (h0.get()), h_v = asvector (h.get());
			VEC numerh_v = asvector (numerh.get()), work2_v = asvector (work2); 
			for (long i = 1; i <= k * n; i ++) {
				if ( h0_v [i] == 0.0 || numerh_v [i]  == 0.0)
					h_v [i] = 0.0;
				else {
					double tmp = h0_v [i] * (numerh_v [i] / (work2_v[i] + DIV_BY_ZERO_AVOIDANCE));
					h_v [i] = ( tmp < ZERO_THRESHOLD ? 0.0 : tmp );
				}
			}

			MATVUmul_fast  (numerw.get(), a, h.transpose()); // numerw = a*h'
			MATmtm_preallocated (work1.get(), h.transpose()); // work1 = h*h'
			MATVUmul_fast  (work2w.get(), w0.get(), work1.get()); // work2w = w0 * work1
			
			VEC w0_v = asvector (w0.get()), w_v = asvector (w.get());
			VEC numerw_v = asvector (numerw.get()), work2w_v = asvector (work2w.get());

			//calculating elementwise matrixmultiplication, Division and addition w = w0 .* (numerw ./ (work2w + eps))
			//set elements < zero_threshold to zero
			
			for(long i = 1; i <= m * k; i ++) {
				if ( w0_v [i] == 0.0 || numerw_v [i] == 0.0)
					w_v [i] = 0.0;
				else {
					double tmp = w0_v [i] * (numerw_v [i] / (work2w_v [i] + DIV_BY_ZERO_AVOIDANCE));
					w_v [i] = ( tmp < ZERO_THRESHOLD ? 0.0 : tmp );
				}
			}
			
			double dnorm = getNorm (d.get(), a, w.get(), h.get());
			
			double dw = getMaximumChange (w.get(), w0.get(), sqrteps);
			
			double dh = getMaximumChange (h.get(), h0.get(), sqrteps);
			
			double delta = std::max (dw, dh);
			
			convergence = ( iter > 1 && (delta < changeTolerance || dnorm < dnorm0 * approximationTolerance) );
			
			dnorm0 = dnorm;
			
			// swap w <-> w0 and h <-> h0
			
			std::swap (w.at_deprecated, w0.at_deprecated);
			std::swap (h.at_deprecated, h0.at_deprecated);

			++ iter;
		}
		
		my h.get() <<= h.get();
		my w.get() <<= w.get();

	} catch (MelderError) {
		Melder_throw (me, U" cannot be improved.");
	}
}

/* End of file NMF.cpp */
