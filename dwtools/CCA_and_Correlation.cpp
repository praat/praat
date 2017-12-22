/* CCA_and_Correlation.cpp
 *
 * Copyright (C) 1993-2017 David Weenink, 2017 Paul Boersma
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
 djmw 2001
 djmw 20020525 GPL header.
 djmw 20060323 Stewart-Love redundancy added.
 djmw 20071022 Melder_error<n>
 */

#include "CCA_and_Correlation.h"
#include "NUM2.h"

autoTableOfReal CCA_Correlation_factorLoadings (CCA me, Correlation thee) {
	try {
		integer ny = my y -> dimension, nx = my x -> dimension;

		if (ny + nx != thy numberOfColumns) {
			Melder_throw (U"The number of columns in the Correlation must equal the sum of the dimensions in the CCA object");
		}
		autoTableOfReal him = TableOfReal_create (2 * my numberOfCoefficients, thy numberOfColumns);

		NUMstrings_copyElements (thy columnLabels, his columnLabels, 1, thy numberOfColumns);
		TableOfReal_setSequentialRowLabels (him.get(), 1, my numberOfCoefficients, U"dv", 1, 1);
		TableOfReal_setSequentialRowLabels (him.get(), my numberOfCoefficients + 1, 2 * my numberOfCoefficients, U"iv", 1, 1);

		double **evecy = my y -> eigenvectors, **evecx = my x -> eigenvectors;
		for (integer i = 1; i <= thy numberOfRows; i ++) {
			for (integer j = 1; j <= my numberOfCoefficients; j ++) {
				double t = 0.0;
				for (integer k = 1; k <= ny; k ++) {
					t += thy data [i] [k] * evecy [j] [k];
				}
				his data[j][i] = t;
			}
			for (integer j = 1; j <= my numberOfCoefficients; j ++) {
				double t = 0.0;
				for (integer k = 1; k <= nx; k ++) {
					t += thy data [i] [ny + k] * evecx [j] [k];
				}
				his data [my numberOfCoefficients + j] [i] = t;
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not created from CCA & Correlation.");
	}
}

static void _CCA_Correlation_check (CCA me, Correlation thee, integer canonicalVariate_from, integer canonicalVariate_to) {
	Melder_require (my y -> dimension + my x -> dimension == thy numberOfColumns, 
		U"The number of columns in the Correlation object should equal the sum of the dimensions in the CCA object");
	Melder_require (canonicalVariate_to >= canonicalVariate_from,
		U"The second value in the \"Canonical variate range\" should be equal or larger than the first.");
	Melder_require (canonicalVariate_from > 0 && canonicalVariate_to <= my numberOfCoefficients, 
		U"The \"Canonical variate range\" should be within the interval [1, ", my numberOfCoefficients, U"].");
}

double CCA_Correlation_getVarianceFraction (CCA me, Correlation thee, int x_or_y, integer canonicalVariate_from, integer canonicalVariate_to) {
	_CCA_Correlation_check (me, thee, canonicalVariate_from, canonicalVariate_to);

	/* For the formulas see:
		William W. Cooley & Paul R. Lohnes (1971), Multivariate data Analysis, John Wiley & Sons, pag 170-...
		varianceFraction = s'.s / n,
		where e.g. for the independent set x:
			s = Rxx . c,
		and Rxx is the correlation matrix of x,
		c is the factor coefficient for x,
		nx is the dimension of x,
		The factor coefficient c is the eigenvector e for x scaled by the st.dev of the component,
		i.e. c = e / sqrt (e'.R.e) (pag 32-33).
		Therefore:
		varianceFraction = s'.s / n = c'Rxx' Rxx c/n = (e'.Rxx' Rxx.e) /(e'.Rxx.e) * 1/n
		(for one can. variate)
	*/

	integer n = my x -> dimension;
	double **evec = my x -> eigenvectors;
	integer ioffset = my y -> dimension;
	if (x_or_y == 1) { /* y: dependent set */
		n = my y -> dimension;
		evec = my y -> eigenvectors;
		ioffset = 0;
	}

	double varianceFraction = 0.0;
	for (integer icv = canonicalVariate_from; icv <= canonicalVariate_to; icv ++) {
		double variance = 0.0, varianceScaling = 0.0;

		for (integer i = 1; i <= n; i ++) {
			double si = 0.0;
			for (integer j = 1; j <= n; j++) {
				si += thy data [ioffset + i] [ioffset + j] * evec [icv] [j]; /* Rxx.e */
			}
			variance += si * si; /* (Rxx.e)'(Rxx.e) =  e'.Rxx'.Rxx.e */
			varianceScaling +=  evec [icv] [i] * si; /* e'.Rxx.e*/
		}
		varianceFraction += (variance / varianceScaling) / n;
	}

	return varianceFraction;
}

double CCA_Correlation_getRedundancy_sl (CCA me, Correlation thee, int x_or_y, integer canonicalVariate_from, integer canonicalVariate_to) {
	_CCA_Correlation_check (me, thee, canonicalVariate_from, canonicalVariate_to);

	double redundancy = 0.0;
	for (integer icv = canonicalVariate_from; icv <= canonicalVariate_to; icv ++) {
		double varianceFraction = CCA_Correlation_getVarianceFraction (me, thee, x_or_y, icv, icv);
		if (isundef (varianceFraction)) {
			return undefined;
		}
		redundancy += varianceFraction * my y -> eigenvalues [icv];
	}

	return redundancy;
}

/* End of file CCA_and_Correlation.cpp */
