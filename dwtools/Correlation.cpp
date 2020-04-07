/* Correlation.cpp
 *
 * Copyright (C) 1993-2020 David Weenink
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

#include "Correlation.h"
#include "Eigen.h"
#include "NUMlapack.h"
#include "NUM2.h"
#include "SVD.h"


Thing_implement (Correlation, SSCP, 0);


autoCorrelation TableOfReal_to_Correlation (TableOfReal me) {
	try {
		autoSSCP sscp = TableOfReal_to_SSCP (me, 0, 0, 0, 0);
		autoCorrelation thee = SSCP_to_Correlation (sscp.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": correlations not created.");
	}
}

autoCorrelation TableOfReal_to_Correlation_rank (TableOfReal me) {
	try {
		autoTableOfReal t = TableOfReal_rankColumns (me, 1, my numberOfColumns);
		autoCorrelation thee = TableOfReal_to_Correlation (t.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": rank correlations not created.");
	}
}

autoCorrelation Correlation_createSimple (conststring32 s_correlations, conststring32 s_centroid, integer numberOfObservations) {
	try {
		autoVEC centroids = newVECfromString (s_centroid);
		autoVEC correlations = newVECfromString (s_correlations);
		integer numberOfCorrelations_wanted = centroids.size * (centroids.size + 1) / 2;
		Melder_require (correlations.size == numberOfCorrelations_wanted,
			U"The number of correlation matrix elements and the number of centroid elements should agree. "
			"There should be d(d+1)/2 correlation values and d centroid values.");

		autoCorrelation me = Correlation_create (centroids.size);
		/*
			Construct the full correlation matrix from the upper-diagonal elements
		*/
		integer rowNumber = 1;
		for (integer inum = 1; inum <= correlations.size; inum ++) {
			const integer nmissing = (rowNumber - 1) * rowNumber / 2;
			const integer inumc = inum + nmissing;
			rowNumber = (inumc - 1) / centroids.size + 1;
			const integer icol = ( (inumc - 1) % centroids.size) + 1;
			my data [rowNumber] [icol] = my data [icol] [rowNumber] = correlations [inum];
			if (icol == centroids.size)
				rowNumber ++;
		}
		/*
			Check if a valid correlations, first check diagonal then off-diagonals
		*/
		for (integer irow = 1; irow <= centroids.size; irow ++)
			Melder_require (my data [irow] [irow] == 1.0,
				U"The diagonal matrix elements should all equal 1.0.");
		for (integer irow = 1; irow <= centroids.size; irow ++)
			for (integer icol = irow + 1; icol <= centroids.size; icol ++)
				Melder_require (fabs (my data [irow] [icol]) <= 1.0,
					U"The correlation in cell [", irow, U",", icol, U"], i.e. input item ",
					(irow - 1) * centroids.size + icol - (irow - 1) * irow / 2, U" should not exceed 1.0.");
		for (integer inum = 1; inum <= centroids.size; inum ++)
			my centroid [inum] = centroids [inum];
		my numberOfObservations = numberOfObservations;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Simple Correlation not created.");
	}
}

autoCorrelation Correlation_create (integer dimension) {
	try {
		autoCorrelation me = Thing_new (Correlation);
		SSCP_init (me.get(), dimension, kSSCPstorage::COMPLETE);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Correlation not created.");
	}
}

autoCorrelation SSCP_to_Correlation (SSCP me) {
	try {
		autoCorrelation thee = Thing_new (Correlation);
		my structSSCP :: v_copy (thee.get());
		for (integer i = 1; i <= my numberOfRows; i ++)
			for (integer j = i; j <= my numberOfColumns; j ++)
				thy data [j] [i] = thy data [i] [j] /= sqrt (my data [i] [i] * my data [j] [j]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Correlation not created.");
	}
}

autoTableOfReal Correlation_confidenceIntervals (Correlation me, double confidenceLevel, integer numberOfTests, int method) {
	try {
		const integer m_bonferroni = my numberOfRows * (my numberOfRows - 1) / 2;
		Melder_require (confidenceLevel > 0 && confidenceLevel <= 1.0,
			U"Confidence level should be in interval (0-1).");
		Melder_require (my numberOfObservations > 4,
			U"The number of observations should be greater than 4.");
		Melder_require (numberOfTests >= 0,
			U"The \"number of tests\" should not be less than zero.");

		if (numberOfTests == 0)
			numberOfTests = m_bonferroni;
		if (numberOfTests > m_bonferroni)
			Melder_warning (U"The \"number of tests\" should not exceed the number of elements in the Correlation object.");

		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfRows);

		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		/*
			Obtain large-sample conservative multiple tests and intervals by the
			Bonferroni inequality and the Fisher z transformation.
			Put upper value of confidence intervals in upper part and lower
			values of confidence intervals in lower part of resulting table.
		*/
		const double z = NUMinvGaussQ ( (1 - confidenceLevel) / (2.0 * numberOfTests));
		const double zf = z / sqrt (my numberOfObservations - 3.0);
		const double two_n = 2.0 * my numberOfObservations;

		for (integer i = 1; i <= my numberOfRows; i ++) {
			for (integer j = i + 1; j <= my numberOfRows; j ++) {
				const double rij = my data [i] [j];
				double rmin, rmax;
				if (method == 2) {
					/*
						Fisher's approximation
					*/
					const double zij = 0.5 * log ( (1 + rij) / (1 - rij));
					rmax = tanh (zij + zf);
					rmin = tanh (zij - zf);
				} else if (method == 1) {
					/*
						Ruben's approximation
					*/
					const double rs = rij / sqrt (1.0 - rij * rij);
					const double a = two_n - 3.0 - z * z;
					const double b = rs * sqrt ( (two_n - 3.0) * (two_n - 5.0));
					const double c = (a - 2.0) * rs * rs - 2.0 * z * z;
					/*
						Solve:  a y^2 - 2b y + c = 0
						q = -0.5((-2b) + sgn(-2b) sqrt((-2b)^2 - 4ac))
						y1 = q/a; y2 = c/q;
					*/
					double d = sqrt (b * b - a * c);
					if (b > 0)
						d = - d;
					double q = b - d;
					rmin = q / a;
					rmin /= sqrt (1.0 + rmin * rmin);
					rmax = c / q; 
					rmax /= sqrt (1.0 + rmax * rmax);
					if (rmin > rmax) {
						double t = rmin;
						rmin = rmax;
						rmax = t;
					}
				} else {
					rmax = rmin = 0;
				}
				thy data [i] [j] = rmax;
				thy data [j] [i] = rmin;
			}
			thy data [i] [i] = 1;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": confidence intervals not created.");
	}
}

/* Morrison, page 118 */
void Correlation_testDiagonality_bartlett (Correlation me, integer numberOfContraints, double *out_chisq, double *out_prob, double *out_df) {
	const integer p = my numberOfRows;
	double chisq = undefined, prob = undefined;
	const double df = p * (p -1) / 2.0;

	if (numberOfContraints <= 0)
		numberOfContraints = 1;

	if (numberOfContraints > my numberOfObservations) {
		Melder_warning (U"Correlation_testDiagonality_bartlett: number of constraints cannot exceed the number of observations.");
		return;
	}
	if (my numberOfObservations >= numberOfContraints) {
		const double ln_determinant = NUMdeterminant_fromSymmetricMatrix (my data.get());
		chisq = - ln_determinant * (my numberOfObservations - numberOfContraints - (2.0 * p + 5.0) / 6.0);
		if (out_prob)
			prob = NUMchiSquareQ (chisq, df);
	}
	if (out_chisq)
		*out_chisq = chisq;
	if (out_prob)
		*out_prob = prob;
	if (out_df)
		*out_df = df;
}

/* End of file Correlation.cpp */
