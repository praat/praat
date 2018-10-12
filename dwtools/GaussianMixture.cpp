/* GaussianMixture.cpp
 *
 * Copyright (C) 2011-2018 David Weenink
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
  djmw 20101021 Initial version
*/
#include "Distributions_and_Strings.h"
#include "GaussianMixture.h"
#include "NUMmachar.h"
#include "NUM2.h"
#include "Strings_extensions.h"

#include "oo_DESTROY.h"
#include "GaussianMixture_def.h"
#include "oo_COPY.h"
#include "GaussianMixture_def.h"
#include "oo_EQUAL.h"
#include "GaussianMixture_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "GaussianMixture_def.h"
#include "oo_WRITE_TEXT.h"
#include "GaussianMixture_def.h"
#include "oo_READ_TEXT.h"
#include "GaussianMixture_def.h"
#include "oo_WRITE_BINARY.h"
#include "GaussianMixture_def.h"
#include "oo_READ_BINARY.h"
#include "GaussianMixture_def.h"
#include "oo_DESCRIPTION.h"
#include "GaussianMixture_def.h"

Thing_implement (GaussianMixture, Daata, 0);

conststring32 GaussianMixture_criterionText (int criterion) {
	conststring32 criterionText [6] =  { U"(1/n)*LLH", U"(1/n)*MML", U"(1/n)*BIC", U"(1/n)*AIC", U"(1/n)*AICc", U"(1/n)*CD_LLH" };
	return criterion >= 0 && criterion < 7 ? criterionText [criterion] : U"(1/n)*ln(p)";
}

void GaussianMixture_removeComponent (GaussianMixture me, integer component);
autoMAT GaussianMixture_removeComponent_bookkeeping (GaussianMixture me, integer component, constMAT p);
void GaussianMixture_TableOfReal_getProbabilities (GaussianMixture me, TableOfReal thee, integer component, MAT p);
autoMAT GaussianMixture_TableOfReal_getGammas (GaussianMixture me, TableOfReal thee, double *out_lnp);
double GaussianMixture_getLikelihoodValue (GaussianMixture me, constMAT p, int onlyLikelyhood);
void GaussianMixture_updateProbabilityMarginals (GaussianMixture me, MAT p);
integer GaussianMixture_getNumberOfParametersInComponent (GaussianMixture me);

static void GaussianMixture_updateCovariance (GaussianMixture me, integer component, constMAT data, constMAT p) {
	Melder_assert (p.nrow == data.nrow + 1);
	Melder_assert (p.ncol == my numberOfComponents + 1);
	Melder_assert (data.ncol == my dimension);
	Melder_assert (component > 0 && component <= my numberOfComponents);
	
	Covariance thee = my covariances->at [component];

	// update the means
	
	autoVEC gamma = VECraw (data.nrow);
	for (integer irow = 1; irow <= data.nrow; irow ++)
		gamma [irow] = my mixingProbabilities [component] * p [irow] [component] / p [irow] [p.ncol + 1];
	autoVEC column = VECraw (data.nrow);
	for (integer icol = 1; icol <= data.ncol; icol ++) {
		VECcopy_preallocated (column.get(), data, icol);
		thy centroid [icol] = NUMinner (column.get (), gamma.get()) / p [p.nrow] [component];
	}

	// update covariance with the new mean
	MATset (thy data.get(), 0.0);
	autoVEC row = VECraw (data.ncol);
	autoMAT outer = MATraw (data.ncol,data.ncol);
	for (integer irow = 1; irow <= data.nrow; irow ++) {
		VECcopy_preallocated (row.get(), data.row (irow));
		VECsubtract_inplace (row.get(), thy centroid.get());
		if (thy numberOfRows == 1) {
			VECmul_elementwise_inplace (row.get(), row.get());
			VECsaxpy (thy data.row (1), row.get(), gamma [irow]);
		} else {
			MATouter_preallocated (outer, row.get(), row.get());
			MATsaxpy (thy data.get(), outer.get(), gamma [irow]);
		}
	}
	thy numberOfObservations = my mixingProbabilities [component] * data.nrow;
}

static void GaussianMixture_addCovarianceFraction (GaussianMixture me, integer im, Covariance him, double fraction) {
	if (im < 1 || im > my numberOfComponents || fraction == 0) {
		return;
	}

	Covariance thee = my covariances->at [im];

	// prevent instability: add lambda fraction of global covariances

	if (thy numberOfRows == 1) {
		for (integer j = 1; j <= thy numberOfColumns; j ++) {
			thy data [1] [j] += fraction * his data [j] [j];
		}
	} else
		MATsaxpy (thy data.get(), his data.get(), fraction);
}

void structGaussianMixture :: v_info () {
	our structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of components: ", our numberOfComponents);
	MelderInfo_writeLine (U"Dimension of component: ", our dimension);
	MelderInfo_writeLine (U"Mixing probabilities:");
	for (integer im = 1; im <= numberOfComponents; im ++) {
		MelderInfo_writeLine (U"  ", im, U": p = ", our mixingProbabilities [im], U"  Name =  \"", Thing_getName (our covariances->at [im]), U"\"");
	}
}

static void GaussianMixture_setLabelsFromTableOfReal (GaussianMixture me, TableOfReal thee) {
	for (integer im = 1; im <= my numberOfComponents; im ++) {
		Covariance cov = my covariances->at [im];
		for (integer j = 1; j <= my dimension; j ++) {
			TableOfReal_setColumnLabel (cov, j, thy columnLabels [j].get());
		}
	}
}

// only from big to reduced or same
static void Covariance_into_Covariance (Covariance me, Covariance thee) {
	Melder_require (my numberOfColumns == thy numberOfColumns, U"Dimensions should be equal.");
	
	SSCP_unExpand (thee); // to its original state

	thy numberOfObservations = my numberOfObservations;
	// copy centroid & column labels
	for (integer ic = 1; ic <= my numberOfColumns; ic ++) {
		thy centroid [ic] = my centroid [ic];
	}
	thy columnLabels. copyElementsFrom (my columnLabels.get());
	// Are the matrix sizes equal
	if (my numberOfRows == thy numberOfRows) {
		thy rowLabels. copyElementsFrom (my rowLabels.get());
		matrixcopy_preallocated (thy data.get(), my data.get());
		return;
	} else {
		for (integer ir = 1; ir <= my numberOfRows; ir ++) {
			for (integer ic = ir; ic <= my numberOfColumns; ic ++) {
				integer dij = ic - ir;
				if (dij < thy numberOfRows)
					thy data [dij + 1] [ic] = my data [ir] [ic];
			}
		}
	}
}

static void GaussianMixture_setDefaultMixtureNames (GaussianMixture me) {
	for (integer im = 1; im <= my numberOfComponents; im ++) {
		Covariance cov = my covariances->at [im];
		Thing_setName (cov, Melder_cat (U"m", im));
	}
}

autoGaussianMixture GaussianMixture_create (integer numberOfComponents, integer dimension, integer storage) {
	try {
		autoGaussianMixture me = Thing_new (GaussianMixture);
		my numberOfComponents = numberOfComponents;
		my dimension = dimension;
		my mixingProbabilities = VECzero (numberOfComponents);
		my covariances = CovarianceList_create ();
		for (integer im = 1; im <= numberOfComponents; im ++) {
			autoCovariance cov = Covariance_create_reduceStorage (dimension, storage);
			my covariances -> addItemAtPosition_move (cov.move(), im);
		}
		VECset (my mixingProbabilities.get(), 1.0 / numberOfComponents);
		GaussianMixture_setDefaultMixtureNames (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"GaussianMixture not created.");
	}
}

int GaussianMixture_generateOneVector_inline (GaussianMixture me, VEC c, char32 **covname, VEC buf) {
	try {
		double p = NUMrandomUniform (0.0, 1.0);
		integer im = NUMgetIndexFromProbability (my mixingProbabilities.at, my numberOfComponents, p);
		Covariance thee = my covariances->at [im];
		*covname = thy name.get();   // BUG dangle
		if (thy numberOfRows == 1) { // 1xn reduced form
			for (integer i = 1; i <= my dimension; i ++) {
				c [i] = NUMrandomGauss (thy centroid [i], sqrt (thy data [1] [i]));
			}
		} else { // nxn
			if (! thy pca) {
				SSCP_expandPCA (thee);    // on demand expanding
			}
			Covariance_PCA_generateOneVector_inline (thee, thy pca.get(), c, buf);
		}
		return 1;
	} catch (MelderError) {
		Melder_throw (me, U": vector not generated.");
	}
}

autoGaussianMixture TableOfReal_to_GaussianMixture_fromRowLabels (TableOfReal me, integer storage) {
	try {
		autoStrings rowLabels = TableOfReal_extractRowLabels (me);
		autoDistributions dist = Strings_to_Distributions (rowLabels.get());
		integer numberOfComponents = dist -> numberOfRows;

		autoGaussianMixture thee = GaussianMixture_create (numberOfComponents, my numberOfColumns, storage);

		GaussianMixture_setLabelsFromTableOfReal (thee.get(), me);

		for (integer i = 1; i <= numberOfComponents; i ++) {
			autoTableOfReal tab = TableOfReal_extractRowsWhereLabel (me, kMelder_string::EQUAL_TO, dist -> rowLabels [i].get());
			autoCovariance cov = TableOfReal_to_Covariance (tab.get());
			Covariance_into_Covariance (cov.get(), thy covariances->at [i]);
			Thing_setName (thy covariances->at [i], dist -> rowLabels [i].get());
		}
		for (integer im = 1; im <= numberOfComponents; im ++) {
			thy mixingProbabilities [im] = dist -> data [im] [1] / my numberOfRows;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no GaussianMixture created.");
	}
}

autoCovariance GaussianMixture_to_Covariance_between (GaussianMixture me) {
	return CovarianceList_to_Covariance_between (my covariances.get());
}
autoCovariance GaussianMixture_to_Covariance_within (GaussianMixture me) {
	return CovarianceList_to_Covariance_within (my covariances.get());
}

autoCovariance GaussianMixture_to_Covariance_total (GaussianMixture me) {
	try {
		autoCovariance thee = GaussianMixture_to_Covariance_between (me);
		autoCovariance within = GaussianMixture_to_Covariance_within (me);

		
		MATadd_inplace (thy data.get(), within -> data.get());
		
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Covariance (total) created.");
	}
}

autoCovariance GaussianMixture_extractComponent (GaussianMixture me, integer component) {
	try {
		Melder_require (component > 0 && component <= my numberOfComponents, U"The component should be in [1, ", my numberOfComponents, U".");

		autoCovariance thee = Data_copy (my covariances->at [component]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no component extracted.");
	}
}

autoTableOfReal GaussianMixture_extractMixingProbabilities (GaussianMixture me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfComponents, 2);
		TableOfReal_setColumnLabel (thee.get(), 1, U"p");
		TableOfReal_setColumnLabel (thee.get(), 2, U"n");
		for (integer im = 1; im <= my numberOfComponents; im ++) {
			Covariance cov = my covariances->at [im];
			thy data [im] [1] = my mixingProbabilities [im];
			thy data [im] [2] = cov -> numberOfObservations;
			TableOfReal_setRowLabel (thee.get(), im, Thing_getName (cov));
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no mixing probabilities extracted.");
	}
}

autoTableOfReal GaussianMixture_extractCentroids (GaussianMixture me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfComponents, my dimension);

		for (integer im = 1; im <= my numberOfComponents; im ++) {
			Covariance cov = my covariances->at [im];
			if (im == 1) {
				for (integer j = 1; j <= my dimension; j ++) {
					TableOfReal_setColumnLabel (thee.get(), j, cov -> columnLabels [j].get());
				}
			}
			TableOfReal_setRowLabel (thee.get(), im, Thing_getName (cov));
			for (integer j = 1; j <= my dimension; j ++) {
				thy data [im] [j] = cov -> centroid [j];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no centroid extracted.");
	}
}

autoPCA GaussianMixture_to_PCA (GaussianMixture me) {
	try {
		autoCovariance him = GaussianMixture_to_Covariance_total (me);
		autoPCA thee = SSCP_to_PCA (him.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PCA calculated.");
	}
}

void GaussianMixture_getIntervalsAlongDirections (GaussianMixture me, integer d1, integer d2, double nsigmas, double *xmin, double *xmax, double *ymin, double *ymax) {
	*xmin = *xmax = *ymin = *ymax = undefined;
	Melder_require (d1 > 0 && d1 <= my dimension && d2 > 0 && d2 <= my dimension, U"Incorrect directions.");
	
	autoSSCPList sscps = SSCPList_extractTwoDimensions (my covariances->asSSCPList(), d1, d2);
	SSCPList_getEllipsesBoundingBoxCoordinates (sscps.get(), -nsigmas, 0, xmin, xmax, ymin, ymax);
}

void GaussianMixture_getIntervalAlongDirection (GaussianMixture me, integer d, double nsigmas, double *xmin, double *xmax) {
	double ymin, ymax;
	GaussianMixture_getIntervalsAlongDirections (me, d, d, nsigmas, xmin, xmax, &ymin, &ymax);
}

void GaussianMixture_PCA_getIntervalsAlongDirections (GaussianMixture me, PCA thee, integer d1, integer d2, double nsigmas, double *xmin, double *xmax, double *ymin, double *ymax) {
	Melder_require (my dimension == thy dimension, 
		U"Dimensions should be equal.");
	Melder_require (d1 > 0 && d1 <= my dimension && d2 > 0 && d2 <= my dimension, 
		U"Incorrect directions.");
	
	autoSSCPList sscps = SSCPList_toTwoDimensions (my covariances->asSSCPList(), thy eigenvectors.row (d1), thy eigenvectors.row (d2));
	SSCPList_getEllipsesBoundingBoxCoordinates (sscps.get(), -nsigmas, 0, xmin, xmax, ymin, ymax);
}


void GaussianMixture_PCA_getIntervalAlongDirection (GaussianMixture me, PCA thee, integer d, double nsigmas, double *xmin, double *xmax) {
	GaussianMixture_PCA_getIntervalsAlongDirections (me, thee, d, d, nsigmas, xmin, xmax, nullptr, nullptr);
}

void GaussianMixture_PCA_drawMarginalPdf (GaussianMixture me, PCA thee, Graphics g, integer d, double xmin, double xmax, double ymin, double ymax, integer npoints, integer nbins, int garnish) {
	if (my dimension != thy dimension || d < 1 || d > my dimension) {
		Melder_warning (U"Dimensions don't agree.");
		return;
	}
	if (npoints <= 1)
		npoints = 1000;
	double nsigmas = 2;
	if (xmax <= xmin)
		GaussianMixture_PCA_getIntervalAlongDirection (me, thee, d, nsigmas, & xmin, & xmax);
	double pmax = 0.0, dx = (xmax - xmin) / npoints, x1 = xmin + 0.5 * dx;
	double scalef = ( nbins <= 0 ? 1.0 : 1.0 ); // TODO
	autoVEC p = VECraw (npoints);
	for (integer i = 1; i <= npoints; i ++) {
		double x = x1 + (i - 1) * dx;
		Melder_assert (thy eigenvectors.ncol == thy dimension);
		p [i] = scalef * GaussianMixture_getMarginalProbabilityAtPosition (me, thy eigenvectors.row (d), x);
		if (p [i] > pmax)
			pmax = p [i];
	}
	if (ymin >= ymax) {
		ymin = 0.0;
		ymax = pmax;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_function (g, p.at, 1, npoints, x1, xmax - 0.5 * dx);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_markBottom (g, xmin, true, true, false, nullptr);
		Graphics_markBottom (g, xmax, true, true, false, nullptr);
		Graphics_markLeft (g, ymin, true, true, false, nullptr);
		Graphics_markLeft (g, ymax, true, true, false, nullptr);
	}
}

void GaussianMixture_drawMarginalPdf (GaussianMixture me, Graphics g, integer d, double xmin, double xmax, double ymin, double ymax, integer npoints, integer nbins, int garnish) {
	Melder_require (d > 0 && d <= my dimension, U"The chosen dimension needs to be larger than 0 and smaller than ", my dimension, U".");
	
	if (npoints <= 1) {
		npoints = 1000;
	}

	double nsigmas = 2.0;
	if (xmax <= xmin) 
		GaussianMixture_getIntervalAlongDirection (me, d, nsigmas, & xmin, & xmax);

	double pmax = 0.0, dx = (xmax - xmin) / (npoints - 1);
	double scalef = 1.0; // TODO
	autoVEC p = VECraw (npoints);
	autoVEC v = VECraw (my dimension);
	
	for (integer i = 1; i <= npoints; i++) {
		double x = xmin + (i - 1) * dx;
		for (integer k = 1; k <= my dimension; k++) {
			v [k] = k == d ? 1.0 : 0.0;
		}
		p [i] = scalef * GaussianMixture_getMarginalProbabilityAtPosition (me, v.get(), x);
		if (p [i] > pmax)
			pmax = p [i];
	}
	if (ymin >= ymax) {
		ymin = 0;
		ymax = pmax;
	}

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_function (g, p.at, 1, npoints, xmin, xmax);
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_markBottom (g, xmin, true, true, false, nullptr);
		Graphics_markBottom (g, xmax, true, true, false, nullptr);
		Graphics_markLeft (g, ymin, true, true, false, nullptr);
		Graphics_markLeft (g, ymax, true, true, false, nullptr);
	}
}

void GaussianMixture_PCA_drawConcentrationEllipses (GaussianMixture me, PCA him, Graphics g, double scale, int confidence, char32 *label, integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, int fontSize, int garnish) {
	if (my dimension != his dimension) {
		Melder_warning (U"Dimensions don't agree.");
		return;
	}
	bool d1_inverted = d1 < 0, d2_inverted = d2 < 0;
	d1 = labs (d1);
	d2 = labs (d2);

	if (d1_inverted) Eigen_invertEigenvector (him, d1);
	if (d2_inverted) Eigen_invertEigenvector (him, d2);

	autoSSCPList thee = SSCPList_toTwoDimensions (my covariances->asSSCPList(), his eigenvectors.row(d1), his eigenvectors.row (d2));

	// Restore eigenvectors
	
	if (d1_inverted) Eigen_invertEigenvector (him, d1);
	if (d2_inverted) Eigen_invertEigenvector (him, d2);

	SSCPList_drawConcentrationEllipses (thee.get(), g, -scale, confidence, label, 1, 2, xmin, xmax, ymin, ymax, fontSize, 0);

	if (garnish) {
		char32 llabel [40];
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Melder_sprint (llabel,40, U"pc ", d2);
		Graphics_textLeft (g, true, llabel);
		Graphics_marksBottom (g, 2, true, true, false);
		Melder_sprint (llabel,40, U"pc ", d1);
		Graphics_textBottom (g, true, llabel);
	}
}

void GaussianMixture_drawConcentrationEllipses (GaussianMixture me, Graphics g, double scale, int confidence, char32 *label, int pcaDirections, integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, int fontSize, int garnish) {
	if (d1 == 0 && d2 == 0) {
		d1 = 1;
		d2 = 2;
	}
	if (labs (d1) > my dimension || labs (d2) > my dimension)
		return;

	if (! pcaDirections) {
		SSCPList_drawConcentrationEllipses (my covariances->asSSCPList(), g, -scale, confidence, label,
			labs (d1), labs (d2), xmin, xmax, ymin, ymax, fontSize, garnish);
		return;
	}

	autoPCA him = GaussianMixture_to_PCA (me);

	GaussianMixture_PCA_drawConcentrationEllipses (me, him.get(), g, scale, confidence, label, d1, d2,
	        xmin, xmax, ymin, ymax, fontSize, garnish);
}

void GaussianMixture_initialGuess (GaussianMixture me, TableOfReal thee, double nSigmas, double ru_range) {
	try {
		autoCovariance cov_t = TableOfReal_to_Covariance (thee);

		// assume equal probabilities for mixture
		// assume equal covariance matrices
		// spread centroids on an ellips in pc1-pc2 plane?

		if (my dimension == 1) {
			double dm = 2.0 * sqrt (cov_t -> data [1] [1]) / my numberOfComponents;
			double m1 = cov_t -> centroid [1] - dm;
			for (integer im = 1; im <= my numberOfComponents; im ++) {
				Covariance covi = my covariances->at [im];
				covi -> centroid [1] = m1;
				covi -> data [1] [1] = dm * dm;
				m1 += dm;
				covi -> numberOfObservations = thy numberOfRows / my numberOfComponents;
			}
		} else {
			autoPCA pca = SSCP_to_PCA (cov_t.get());
			autoSSCP s2d = SSCP_toTwoDimensions (cov_t.get(), pca -> eigenvectors.row (1), pca -> eigenvectors.row (2));
			autoMAT means2d = MATraw (my numberOfComponents, 2);

			double a, b, cs, sn;
			NUMeigencmp22 (s2d -> data [1] [1], s2d -> data [1] [2], s2d -> data [2] [2], & a, & b, & cs, & sn);

			a = nSigmas * sqrt (a);
			b = nSigmas * sqrt (b);
			double angle = 0.0, angle_inc = NUM2pi / my numberOfComponents;
			for (integer im = 1; im <= my numberOfComponents; im++, angle += angle_inc) {
				double xc = a * (1.0 + NUMrandomUniform (-ru_range, ru_range)) * cos (angle);
				double yc = b * (1.0 + NUMrandomUniform (-ru_range, ru_range)) * sin (angle);
				means2d [im] [1] = s2d -> centroid [1] + xc * cs - yc * sn;
				means2d [im] [2] = s2d -> centroid [2] + xc * sn + yc * cs;
			}

			// reconstruct the n-dimensional means from the 2-d pc's and the 2 eigenvectors
			autoMAT means = MATmul (means2d.get(), pca -> eigenvectors.horizontalBand (1, 2));

			for (integer im = 1; im <= my numberOfComponents; im ++) {
				Covariance covi = my covariances->at [im];
				VECcopy_preallocated (covi -> centroid.get(), means.row (im));
				covi -> numberOfObservations = thy numberOfRows / my numberOfComponents;
			}

			// Trick: use the new means to get the between SSCP,
			// if there is only one component the cov_b will be zero!

			autoCovariance cov_b = GaussianMixture_to_Covariance_between (me);
			double var_t = SSCP_getTotalVariance (cov_t.get());
			double var_b = SSCP_getTotalVariance (cov_b.get());
			if (var_b >= var_t) { // we have chosen our initial values too far out
				double scale = 0.9 * sqrt (var_t / var_b);
				for (integer im = 1; im <= my numberOfComponents; im ++) {
					Covariance covi = my covariances->at [im];
					for (integer ic = 1; ic <= my dimension; ic ++)
						covi -> centroid [ic] -= (1.0 - scale) * (covi -> centroid [ic] - cov_b -> centroid [ic]);
				}
				cov_b = GaussianMixture_to_Covariance_between (me);
			}

			// Within variances are now (total - between) / numberOfComponents;

			if (my numberOfComponents > 1)
				MATmultiply_inplace (cov_t -> data.get(), (var_b / var_t) / my numberOfComponents);

			// Copy them

			for (integer im = 1; im <= my numberOfComponents; im ++) {
				Covariance cov = my covariances->at [im];
				if (cov -> numberOfRows == 1) {
					for (integer ic = 1; ic <= my dimension; ic ++) {
						cov -> data [1] [ic] = cov_t -> data [ic] [ic];
					}
				} else {
					MATcopy_preallocated (cov -> data.get(), cov_t -> data.get());
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no initial guess possible.");
	}
}


autoClassificationTable GaussianMixture_TableOfReal_to_ClassificationTable (GaussianMixture me, TableOfReal thee) {
	try {
		autoClassificationTable him = ClassificationTable_create (thy numberOfRows, my numberOfComponents);
		for (integer ic = 1; ic <= my numberOfComponents; ic ++) {
			Covariance cov = my covariances->at [ic];
			SSCP_expandLowerCholeskyInverse (cov);
			TableOfReal_setColumnLabel (him.get(), ic, Thing_getName (cov));
		}

		double ln2pid = -0.5 * my dimension * log (NUM2pi);
		autoVEC lnN = VECraw (my numberOfComponents);
		for (integer irow = 1; irow <=  thy numberOfRows; irow ++) {
			longdouble psum = 0.0;
			for (integer ic = 1; ic <= my numberOfComponents; ic ++) {
				Covariance cov = my covariances->at [ic];
				double dsq = NUMmahalanobisDistance (cov -> lowerCholeskyInverse.get(), thy data.row (irow), cov -> centroid.get());
				lnN [ic] = ln2pid - 0.5 * (cov -> lnd + dsq);
				psum += his data [irow] [ic] = my mixingProbabilities [ic] * exp (lnN [ic]);
			}
			if (psum == 0.0) { // p's might be too small (underflow), make the largest equal to sfmin
				double lnmax = -1e308;
				integer imm = 1;
				for (integer ic = 1; ic <= my numberOfComponents; ic ++) {
					if (lnN [ic] > lnmax) {
						lnmax = lnN [ic];
					} 
					imm = ic;
				}
				his data [irow] [imm] = NUMfpp -> sfmin;
			}

			// for (integer im = 1; im <= my numberOfComponents; im++) { his data [i] [im] /= psum; }

			TableOfReal_setRowLabel (him.get(), irow, thy rowLabels [irow].get());
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"No ClassificationTable created from GaussianMixture & TableOfReal.");
	}
}

autoMAT GaussianMixture_TableOfReal_getGammas (GaussianMixture me, TableOfReal thee, double *out_lnp) {
	try {
		autoMAT gamma = MATzero (thy numberOfRows + 1, my numberOfComponents);
		for (integer im = 1; im <= my numberOfComponents; im ++) {
			Covariance cov = my covariances->at [im];
			SSCP_expandLowerCholeskyInverse (cov); 
		}
		longdouble lnp = 0.0;
		double ln2pid = - 0.5 * my dimension * log (NUM2pi);
		autoVEC lnN = VECraw (my numberOfComponents);
		for (integer i = 1; i <=  thy numberOfRows; i ++) {
			for (integer im = 1; im <= my numberOfComponents; im ++) {
				Covariance cov = my covariances->at [im];
				double dsq = NUMmahalanobisDistance (cov -> lowerCholeskyInverse.get(), thy data.row(i), cov -> centroid.get());
				lnN [im] = ln2pid - 0.5 * (cov -> lnd + dsq);
				gamma [i] [im] = my mixingProbabilities [im] * exp (lnN [im]); // eq. Bishop 9.16
			}
			
			double rowsum = NUMsum (gamma.row (i));
			if (rowsum == 0.0) continue;    // This is ok because gamma [i]'s will all be zero

			// scale gamma and get log(likehood) (Bishop eq. 9.40)
			VECmultiply_inplace (gamma.row (i), 1.0 / rowsum);
			gamma.row (gamma.nrow) += gamma.row (i);  // eq. Bishop 9.18
			for (integer im = 1; im <= my numberOfComponents; im ++)
				lnp += gamma [i] [im] * (log (my mixingProbabilities [im])  + lnN [im]); // eq. Bishop 9.40
		}
		if (out_lnp) *out_lnp = lnp;
		return gamma;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no gammas.");
	}
}

void GaussianMixture_splitComponent (GaussianMixture me, integer component) {
	try {
		Melder_require (component > 0 && component <= my numberOfComponents, U"The component should be in [1, ", my numberOfComponents, U"].");
		
		Covariance thee = my covariances->at [component];
		// Always new PCA because we cannot be sure of data unchanged.
		SSCP_expandPCA (thee);
		autoCovariance cov1 = Data_copy (thee);
		autoCovariance cov2 = Data_copy (thee);
		SSCP_unExpandPCA (cov1.get());
		SSCP_unExpandPCA (cov2.get());

		// Eventually cov1 replaces component, cov2 at end

		autoVEC mixingProbabilities = VECraw (my numberOfComponents + 1);
		for (integer i = 1; i <= my numberOfComponents; i ++) {
			mixingProbabilities [i] = my mixingProbabilities [i];
		}
		double gamma = 0.5, lambda = 0.5, eta = 0.5, mu = 0.5;
		mixingProbabilities [component] = gamma * my mixingProbabilities [component];
		mixingProbabilities [my numberOfComponents + 1] = (1.0 - gamma) * my mixingProbabilities [component];
		double mp12 =  mixingProbabilities [component] / mixingProbabilities [my numberOfComponents + 1];
		double factor1 = (eta - eta * lambda * lambda - 1.0) / gamma + 1.0;
		double factor2 = (eta * lambda * lambda - eta - lambda * lambda) / (1.0 - gamma) + 1.0;
		constVEC ev = thy pca -> eigenvectors.row (1);
		double d2 = thy pca -> eigenvalues [1];

		for (integer i = 1; i <= my dimension; i ++) {
			cov1 -> centroid [i] -= (1.0 / sqrt (mp12)) * sqrt (d2) * mu * ev [i];
			cov2 -> centroid [i] +=        sqrt (mp12)  * sqrt (d2) * mu * ev [i];

			if (thy numberOfRows == 1) { // diagonal
				cov1 -> data [1] [i] = cov1 -> data [1] [i] / mp12 + factor1 * d2;
				cov1 -> data [1] [i] = cov2 -> data [i] [i] * mp12 + factor2 * d2;
			} else {
				for (integer j = i; j <= my dimension; j++) {
					cov1 -> data [j] [i] = cov1 -> data [i] [j] = cov1 -> data [i] [j] / mp12 + factor1 * d2 * ev [i] * ev [j];
					cov2 -> data [j] [i] = cov2 -> data [i] [j] = cov2 -> data [i] [j] * mp12 + factor2 * d2 * ev [i] * ev [j];
				}
			}
		}
		cov1 -> numberOfObservations *= gamma;
		cov2 -> numberOfObservations *= 1.0 - gamma;

		// Replace cov1 at component + add cov2. If something goes wrong we should be able to restore original!
		try {
			Thing_setName (cov2.get(), Melder_cat (Thing_getName (cov2.get()), U"-", my numberOfComponents + 1));
			my covariances -> addItem_move (cov2.move());
		} catch (MelderError) {
			Melder_throw (me, U" cannot add new component.");
		}

		my covariances -> replaceItem_move (cov1.move(), component);

		my numberOfComponents ++;
		my mixingProbabilities = mixingProbabilities.move();
	} catch (MelderError) {
		Melder_throw (me, U": component ", component, U" cannot be split.");
	}
}

void GaussianMixture_TableOfReal_getProbabilities (GaussianMixture me, TableOfReal thee, integer component, MAT p) {
	try {
		Melder_assert (p.nrow == thy numberOfRows + 1);
		Melder_assert (p.ncol == my numberOfComponents + 1);
		Melder_assert (my dimension == thy numberOfColumns);
		
		double ln2pid = my dimension * log (NUM2pi);

		integer icb = 1, ice = my numberOfComponents;
		if (component > 0 && component <= my numberOfComponents) // if component == 0 update all probabilities
			icb = ice = component;
		
		for (integer ic = icb; ic <= ice; ic ++) {
			Covariance him = my covariances->at [ic];
			SSCP_expandLowerCholeskyInverse (him);

			for (integer i = 1; i <= thy numberOfRows; i++) {
				double dsq = NUMmahalanobisDistance (his lowerCholeskyInverse.get(), thy data.row (i), his centroid.get());
				p [i] [ic] = std::max (1e-300, exp (- 0.5 * (ln2pid + his lnd + dsq))); // prevent p from being zero
			}
		}

		GaussianMixture_updateProbabilityMarginals (me, p);
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no probabilies could be calculated.");
	}
}

void GaussianMixture_expandPCA (GaussianMixture me) {
	for (integer im = 1; im <= my numberOfComponents; im ++) {
		Covariance him = my covariances->at [im];
		Melder_require (his numberOfRows > 1, U"Nothing to expand.");
		his pca = SSCP_to_PCA (him);
	}
}

void GaussianMixture_unExpandPCA (GaussianMixture me) {
	for (integer im = 1; im <= my numberOfComponents; im ++) {
		SSCP_unExpandPCA (my covariances->at [im]);
	}
}

void GaussianMixture_TableOfReal_improveLikelihood (GaussianMixture me, TableOfReal thee, double delta_lnp, integer maxNumberOfIterations, double lambda, int criterion) {
	try {
		conststring32 criterionText = GaussianMixture_criterionText (criterion);

		// The global covariance matrix is added with scaling coefficient lambda during updating the
		// mixture covariances to prevent numerical instabilities.

		autoCovariance covg = TableOfReal_to_Covariance (thee);
		// p's last row has the column marginals n(k)
		autoMAT p = MATraw (thy numberOfRows + 1, my numberOfComponents + 1);
		
		GaussianMixture_TableOfReal_getProbabilities (me, thee, 0, p.get()); // get initial p's

		double lnp = GaussianMixture_getLikelihoodValue (me, p.get(), criterion);
		integer iter = 0;
		autoMelderProgress progress (U"Improve likelihood...");
		try {
			double lnp_prev, lnp_start = lnp / thy numberOfRows;
			do {

				// E-step: get responsabilities (gamma) with current parameters
				// See C. Bishop (2006), Pattern reconition and machine learning, Springer, page 439...

				lnp_prev = lnp;
				iter ++;
				// M-step: 1. new means & covariances

				for (integer im = 1; im <= my numberOfComponents; im ++) {
					GaussianMixture_updateCovariance (me, im, thy data.get(), p.get());
					GaussianMixture_addCovarianceFraction (me, im, covg.get(), lambda);
				}

				// M-step: 2. new mixingProbabilities
				VECcopy_preallocated (my mixingProbabilities.get(), p.row(p.nrow).subview (1, p.ncol - 1));
				VECmultiply_inplace (my mixingProbabilities.get(), 1.0 / thy numberOfRows);
				
				GaussianMixture_TableOfReal_getProbabilities (me, thee, 0, p.get());
				
				lnp = GaussianMixture_getLikelihoodValue (me, p.get(), criterion);
				Melder_progress ((double) iter / (double) maxNumberOfIterations, criterionText, U": ", lnp / thy numberOfRows, U", L0: ", lnp_start);
			} while (fabs ((lnp - lnp_prev) / lnp_prev) > delta_lnp && iter < maxNumberOfIterations);
		} catch (MelderError) {
			Melder_clearError ();
		}

		// During EM, covariances were underestimated by a factor of (n-1)/n. Correction now.

		for (integer im = 1; im <= my numberOfComponents; im ++) {
			Covariance cov = my covariances->at [im];
			if (cov -> numberOfObservations > 1.5) {
				if (cov -> numberOfRows == 1) {
					for (integer j = 1; j <= thy numberOfColumns; j ++) {
						cov -> data [1] [j] *= cov -> numberOfObservations / (cov -> numberOfObservations - 1);
					}
				} else {
					for (integer j = 1; j <= thy numberOfColumns; j ++)
						for (integer k = j; k <= thy numberOfColumns; k ++) {
							cov -> data [j] [k] = cov -> data [k] [j] *= cov -> numberOfObservations / (cov -> numberOfObservations - 1.0);
						}
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": likelihood cannot be improved.");
	}
}

integer GaussianMixture_getNumberOfParametersInComponent (GaussianMixture me) {
	Melder_assert (my covariances->size > 0);
	Covariance thee = my covariances->at [1];
	// if diagonal) d (means) + d (stdev)
	// else  n + n(n+1)/2
	return thy numberOfRows == 1 ? 2 * thy numberOfColumns : thy numberOfColumns * (thy numberOfColumns + 3) / 2;
}

/*
	The marginals may be numbers larger than 1.0
	Each column is weighted with its mixingProbability
*/
void GaussianMixture_updateProbabilityMarginals (GaussianMixture me, MAT p) {
	Melder_assert (p.ncol == my numberOfComponents + 1);
	Melder_assert (p.nrow > 1);
	
	VECset (p.row (p.nrow), 0.0);
	for (integer irow = 1; irow <= p.nrow - 1; irow ++) {
		p [irow] [p.ncol] = NUMinner (my mixingProbabilities.get(), p.row (irow).subview (1, p.ncol - 1));
		for (integer icol = 1; icol <= my numberOfComponents; icol ++)
			p [p.nrow] [icol] += my mixingProbabilities [icol] * p [irow] [icol] / p [irow] [p.ncol];
	}
}

autoMAT GaussianMixture_removeComponent_bookkeeping (GaussianMixture me, integer component, constMAT p) {
	Melder_assert (my numberOfComponents == p.ncol - 1);
	Melder_assert (p.nrow > 1);
	// p is (numberOfRows+1) by (numberOfComponents+1)
	autoMAT pc = MATcopy (p, 1, p.nrow, 1, p.ncol - 1);
	for (integer i = 1; i <= p.nrow; i ++)
		for (integer ic = component; ic <= pc.ncol; ic ++)
			pc [i] [ic] = p [i] [ic + 1];
		
	// First we have to remove the component before we can update probabilities!
	GaussianMixture_removeComponent (me, component);
	GaussianMixture_updateProbabilityMarginals (me, pc.get());
	return pc;
}

double GaussianMixture_TableOfReal_getLikelihoodValue (GaussianMixture me, TableOfReal thee, int criterion) {
	autoMAT p = MATraw (thy numberOfRows + 1, my numberOfComponents + 1);
	GaussianMixture_TableOfReal_getProbabilities (me, thee, 0, p.get());
	return GaussianMixture_getLikelihoodValue (me, p.get(), criterion);;
}

double GaussianMixture_getLikelihoodValue (GaussianMixture me, constMAT p, int criterion) {
	Melder_assert (p.ncol == my numberOfComponents + 1);
	// Because we try to _maximize_ a criterion, all criteria are negative numbers.

	if (criterion == GaussianMixture_CD_LIKELIHOOD) {
		longdouble lnpcd = 0.0;
		for (integer irow = 1; irow <= p.nrow - 1; irow ++) {
			longdouble psum = 0.0, lnsum = 0.0;
			for (integer icol = 1; icol <= my numberOfComponents; icol ++) {
				longdouble pp = my mixingProbabilities [icol] * p [irow] [icol];
				psum += pp;
				lnsum += pp * log (pp);
			}
			if (psum > 0)
				lnpcd += lnsum / psum;
		}
		return lnpcd;
	}

	// The common factor for all other criteria is the log(likelihood)

	longdouble lnp = 0.0;
	for (integer irow = 1; irow <= p.nrow - 1; irow ++) {
		double psum = NUMinner (my mixingProbabilities.get(), p.row (irow).subview (1, p.ncol - 1));
		if (psum > 0.0)
			lnp += (longdouble) log (psum);
	}

	if (criterion == GaussianMixture_LIKELIHOOD)
		return lnp;

	double npars = GaussianMixture_getNumberOfParametersInComponent (me), np = npars * my numberOfComponents;
	if (criterion == GaussianMixture_MML) {
		/* Equation (15) in
			Mario A.T. Figueiredo, and Anil K. Jain, Unsupervised Learning of Finite Mixture Models :
			IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, VOL. 24, NO. 3, MARCH 2002

			L(theta,Y)= N/2*sum(m=1..k, log(n*alpha [k]/12)) +k/2*ln(n/12) +k(N+1)/2
				- log (sum(i=1..n, sum(m=1..k, alpha [k]*p(k))))
		*/
		longdouble logmpn = 0.0;
		for (integer ic = 1; ic <= my numberOfComponents; ic ++)
			logmpn += log (my mixingProbabilities [ic]);

		// a rewritten L(theta,Y) is

		return lnp - 0.5 * my numberOfComponents * (npars + 1) * (log ((p.nrow - 1) / 12.0) + 1.0)
		       + 0.5 * npars * logmpn;
	} else if (criterion == GaussianMixture_BIC)
		return 2.0 * lnp - np * log (p.nrow - 1.0);
	else if (criterion == GaussianMixture_AIC)
		return 2.0 * (lnp - np);
	else if (criterion == GaussianMixture_AICC) {
		np = npars * my numberOfComponents;
		return 2.0 * (lnp - np * ((p.nrow - 1.0) / (p.nrow - 1.0 - np - 1.0)));
	}
	return lnp;
}

autoGaussianMixture GaussianMixture_TableOfReal_to_GaussianMixture_CEMM (GaussianMixture gm, TableOfReal thee, integer minNumberOfComponents, double delta_l, integer maxNumberOfIterations, double lambda, int criterion) {
	try {
		conststring32 criterionText = GaussianMixture_criterionText (criterion);
		bool deleteWeakComponents = minNumberOfComponents > 0;
		autoGaussianMixture me = Data_copy (gm);
		autoMAT p = MATzero (thy numberOfRows + 1, my numberOfComponents + 1);

		autoCovariance covg = TableOfReal_to_Covariance (thee);

		double npars = GaussianMixture_getNumberOfParametersInComponent (me.get());
		double nparsd2 = ( deleteWeakComponents ? npars / 2.0 : 0.0 );

		// Initial E-step: Update all p's.

		GaussianMixture_TableOfReal_getProbabilities (me.get(), thee, 0, p.get());

		double lnew = GaussianMixture_getLikelihoodValue (me.get(), p.get(), criterion);

		autoMelderProgress progress (U"Gaussian mixture...");
		autoGaussianMixture best;
		try {
			double lstart = lnew / thy numberOfRows;
			integer iter = 0, component;
			double lmax = -1e308, lprev;
			while (my numberOfComponents >= minNumberOfComponents) {
				do {
					iter ++;
					component = 1;
					lprev = lnew;
					while (component <= my numberOfComponents) {
						// M-step for means and covariances
						GaussianMixture_updateProbabilityMarginals (me.get(), p.get());
						GaussianMixture_updateCovariance (me.get(), component, thy data.get(), p.get());
						if (lambda > 0)
							GaussianMixture_addCovarianceFraction (me.get(), component, covg.get(), lambda);

						// Now check if enough support for a component exists
						
						double support_im = p [p.nrow] [component] - nparsd2, support = 0.0;
						for (integer ic = 1; ic <= my numberOfComponents; ic ++) {
							double support_ic = p [p.nrow] [ic] - nparsd2;
							if (support_ic > 0.0)
								support += support_ic;
						}
						my mixingProbabilities [component] = support_im > 0.0 ? support_im : 0.0;
						if (support > 0.0)
							my mixingProbabilities [component] /= support;

						VECnormalize_inplace (my mixingProbabilities.get(), 1.0, 1.0);

						if (my mixingProbabilities [component] > 0.0) { // update p for component
							GaussianMixture_TableOfReal_getProbabilities (me.get(), thee, component, p.get());
							component ++;
						} else {
							// "Remove" the component column from p by shifting column values

							autoMAT pc = GaussianMixture_removeComponent_bookkeeping (me.get(), component, p.get());
							p = pc.move();
							// Now numberOfComponents is one less!
							// MelderInfo_writeLine (U"Removed component ", component);
						}
					}

					// L(theta,Y)=N/2 sum(m=1..k, log(n*mixingP [m]/12))+k/2log(n/12)+k/2(N+1)-loglikelihood reduces to:
					// k/2 (N+1){log(n/12)+1}+N/2sum(m=1..k,mixingP [m]) - loglikelihood

					lnew = GaussianMixture_getLikelihoodValue (me.get(), p.get(), criterion);

					Melder_progress ((double) iter / (double) maxNumberOfIterations, U", ", criterionText, U": ",
						lnew / thy numberOfRows, U"\nComponents: ", my numberOfComponents, U"\nL0: ", lstart);
				} while (lnew > lprev && fabs ((lprev - lnew) / lnew) > delta_l && iter < maxNumberOfIterations);
				if (lnew > lmax) {
					best = Data_copy (me.get());
					lmax = lnew;
					if (! deleteWeakComponents) {
						break;    // TODO was got end; is dat hetzelfde?
					}
				}
				if (my numberOfComponents > 1) { // remove smallest component
					component = 1;
					double mpmin = my mixingProbabilities [component];
					for (integer ic = 2; ic <= my numberOfComponents; ic ++) {
						if (my mixingProbabilities [ic] < mpmin) {
							mpmin = my mixingProbabilities [ic];
							component = ic;
						}
					}

					autoMAT pc = GaussianMixture_removeComponent_bookkeeping (me.get(), component, p.get());
					p = pc.move();
				} else {
					break;
				}
			}
		} catch (MelderError) {
			Melder_clearError ();
		}
		return best;
	} catch (MelderError) {
		Melder_throw (U"GaussianMixture not improved.");
	}
}

// The numberOfElements per covariance needs to be updated later
void GaussianMixture_removeComponent (GaussianMixture me, integer component) {
	if (component < 1 || component > my numberOfComponents || my numberOfComponents == 1) {
		return;
	}

	my covariances -> removeItem (component);
	my numberOfComponents --;

	for (integer ic = component; ic <= my numberOfComponents; ic ++) {
		my mixingProbabilities [ic] = my mixingProbabilities [ic + 1];
	}
	
	my mixingProbabilities.resize (my numberOfComponents);
	
	VECnormalize_inplace (my mixingProbabilities.get(), 1.0, 1.0);
}

autoGaussianMixture TableOfReal_to_GaussianMixture (TableOfReal me, integer numberOfComponents, double delta_lnp, integer maxNumberOfIterations, double lambda, int storage, int criterion) {
	try {
		Melder_require (my numberOfRows >= 2 * numberOfComponents,
			U"The number of rows should at least be twice the number of components.");
		autoGaussianMixture thee = GaussianMixture_create (numberOfComponents, my numberOfColumns, storage);
		GaussianMixture_setLabelsFromTableOfReal (thee.get(), me);
		GaussianMixture_initialGuess (thee.get(), me, 1.0, 0.05);
		if (maxNumberOfIterations <= 0)
			return thee;
		GaussianMixture_TableOfReal_improveLikelihood (thee.get(), me, delta_lnp, maxNumberOfIterations, lambda, criterion);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no GaussianMixture created.");
	}
}

autoCorrelation GaussianMixture_TableOfReal_to_Correlation (GaussianMixture me, TableOfReal thee) {
	try {
		Melder_require (my dimension == thy numberOfColumns,
			U"Dimensions should be equal.");
		autoClassificationTable ct = GaussianMixture_TableOfReal_to_ClassificationTable (me, thee);
		autoCorrelation him = ClassificationTable_to_Correlation_columns (ct.get());
		return him;
	} catch (MelderError) {
		Melder_throw (U"Correlation not created from GaussianMixture & TableOfReal.");
	}
}

double GaussianMixture_getProbabilityAtPosition_string (GaussianMixture me, conststring32 vector_string) {
	autostring32vector vector = STRVECtokenize (vector_string);
	autoVEC pos = VECzero (my dimension);
	for (integer i = 1; i <= vector.size; i ++) {
		pos [i] = Melder_atof (vector [i].get());
		if (i == my dimension)
			break;
	}
	double p = GaussianMixture_getProbabilityAtPosition (me, pos.get());
	return p;
}

double GaussianMixture_getMarginalProbabilityAtPosition (GaussianMixture me, constVEC pos, double x) {
	longdouble p = 0.0;
	for (integer im = 1; im <= my numberOfComponents; im ++) {
		double pim = Covariance_getMarginalProbabilityAtPosition (my covariances->at [im], pos, x);
		p += my mixingProbabilities [im] * pim;
	}
	return (double) p;
}

double GaussianMixture_getProbabilityAtPosition (GaussianMixture me, constVEC xpos) {
	longdouble p = 0.0;
	for (integer im = 1; im <= my numberOfComponents; im ++) {
		double pim = Covariance_getProbabilityAtPosition (my covariances->at [im], xpos);
		p += my mixingProbabilities [im] * pim;
	}
	return (double) p;
}

autoMatrix GaussianMixture_PCA_to_Matrix_density (GaussianMixture me, PCA thee, integer d1, integer d2, double xmin, double xmax, integer nx, double ymin, double ymax, integer ny) {
	try {
		Melder_require (my dimension == thy dimension, U"Dimensions should be equal.");
		Melder_require (d1 <= thy numberOfEigenvalues && d2 <= thy numberOfEigenvalues, U"Direction index too high.");
		
		autoVEC v (my dimension, kTensorInitializationType::ZERO);
		if (xmax == xmin || ymax == ymin) {
			double xmind, xmaxd, ymind, ymaxd, nsigmas = 2.0;

			GaussianMixture_PCA_getIntervalsAlongDirections (me, thee, d1, d2, nsigmas, &xmind, &xmaxd, &ymind, &ymaxd);
			if (xmax == xmin) {
				xmin = xmind;
				xmax = xmaxd;
			}
			if (ymax == ymin) {
				ymin = ymind;
				ymax = ymaxd;
			}
		}

		// xmin,xmax and ymin,ymax are coordinates in the pc1 vs pc2 plane

		double dx = fabs (xmax - xmin) / nx, dy = fabs (ymax - ymin) / ny;
		double x1 = xmin + 0.5 * dx, y1 = ymin + 0.5 * dy;
		autoMatrix him = Matrix_create (xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);

		for (integer i = 1; i <= ny; i ++) {
			double y = y1 + (i - 1) * dy;
			for (integer j = 1; j <= nx; j ++) {
				double x = x1 + (j - 1) * dx;
				for (integer k = 1; k <= my dimension; k ++) {
					v [k] = x * thy eigenvectors [d1] [k] + y * thy eigenvectors [d2] [k];
				}
				his z [i] [j] = GaussianMixture_getProbabilityAtPosition (me, v.get());
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no Matrix density created.");
	}
}

autoTableOfReal GaussianMixture_to_TableOfReal_randomSampling (GaussianMixture me, integer numberOfPoints) {
	try {
		Covariance cov = my covariances->at [1];
		autoTableOfReal thee = TableOfReal_create (numberOfPoints, my dimension);
		autoVEC buf (my dimension, kTensorInitializationType::RAW);
		thy columnLabels. copyElementsFrom_upTo (cov -> columnLabels.get(), my dimension);
			// ppgb FIXME: is the number of column labels in the covariance equal to the number of dimensions? If so, document or assert.
		for (integer i = 1; i <= numberOfPoints; i ++) {
			char32 *covname;
			GaussianMixture_generateOneVector_inline (me, thy data.row (i), & covname, buf.get());
			TableOfReal_setRowLabel (thee.get(), i, covname);
		}
		GaussianMixture_unExpandPCA (me);
		return thee;
	} catch (MelderError) {
		GaussianMixture_unExpandPCA (me); 
		Melder_throw (U"TableOfReal with random sampling not created.");
	}
}

autoTableOfReal GaussianMixture_TableOfReal_to_TableOfReal_BHEPNormalityTests (GaussianMixture me, TableOfReal thee, double h) {
	try {
		integer n = thy numberOfRows, d = thy numberOfColumns, nocp1 = my numberOfComponents + 1;
		
		Melder_require (d == my dimension, U"Dimensions should agree.");
		
		// We cannot use a classification table because this could weigh a far-off data point with high probability

		autoMAT p = MATzero (thy numberOfRows + 1, my numberOfComponents + 1);

		GaussianMixture_TableOfReal_getProbabilities (me, thee, 0, p.get());

		// prob, beta, tnbo, lnmu, lnvar, ndata, ncol
		autoTableOfReal him = TableOfReal_create (my numberOfComponents, 7);

		// labels

		integer iprob = 1, ih = 2, itnb = 3, ilnmu = 4, ilnvar = 5, indata = 6, id = 7;
		conststring32 label [8] = { U"", U"p", U"h", U"tnb", U"lnmu", U"lnvar", U"ndata", U"d" };
		for (integer icol = 1; icol <= 7; icol ++) {
			TableOfReal_setColumnLabel (him.get(), icol, label [icol]);
		}
		for (integer irow = 1; irow <= my numberOfComponents; irow ++) {
			Covariance cov = my covariances->at [irow];
			TableOfReal_setRowLabel (him.get(), irow, Thing_getName (cov));
		}

		for (integer icol = 1 ; icol <= my numberOfComponents; icol ++) {
			his data [icol] [indata] = p [n + 1] [icol];
		}

		for (integer im = 1; im <= my numberOfComponents; im ++) {
			Covariance cov = my covariances->at [im];
			double mixingP = my mixingProbabilities [im];
			double nd = his data [im] [indata], d2 = d / 2.0;
			double beta = h > 0.0 ? NUMsqrt1_2 / h : NUMsqrt1_2 * pow ( (1.0 + 2.0 * d) / 4.0, 1.0 / (d + 4.0)) * pow (nd, 1.0 / (d + 4.0));
			double beta2 = beta * beta, beta4 = beta2 * beta2, beta8 = beta4 * beta4;
			double gamma = 1.0 + 2.0 * beta2, gamma2 = gamma * gamma, gamma4 = gamma2 * gamma2;
			double delta = 1.0 + beta2 * (4.0 + 3.0 * beta2), delta2 = delta * delta;
			double mu = 1.0 - pow (gamma, -d2) * (1.0 + d * beta2 / gamma + d * (d + 2.0) * beta4 / (2.0 * gamma2));
			double var = 2.0 * pow (1.0 + 4.0 * beta2, -d2)
			             + 2.0 * pow (gamma,  -d) * (1.0 + 2.0 * d * beta4 / gamma2  + 3.0 * d * (d + 2.0) * beta8 / (4.0 * gamma4))
			             - 4.0 * pow (delta, -d2) * (1.0 + 3.0 * d * beta4 / (2.0 * delta) + d * (d + 2.0) * beta8 / (2.0 * delta2));
			double mu2 = mu * mu;

			double prob = undefined, tnb = undefined, lnmu = undefined, lnvar = undefined;

			try {
				SSCP_expandLowerCholeskyInverse (cov);
			} catch (MelderError) {
				tnb = 4.0 * nd;
			}

			double djk, djj, sumjk = 0.0, sumj = 0.0;
			double b1 = beta2 / 2.0, b2 = b1 / (1.0 + beta2);

			/* Heinze & Wagner (1997), page 3
				We use d [j] [k] = ||Y [j]-Y [k]||^2 = (Y [j]-Y [k])'S^(-1)(Y [j]-Y [k])
				So d [j] [k]= d [k] [j] and d [j] [j] = 0
			*/
			for (integer j = 1; j <= n; j ++) {
				double wj = p [j] [nocp1] > 0.0 ? mixingP * p [j] [im] / p [j] [nocp1] : 0.0;
				for (integer k = 1; k < j; k ++) {
					djk = NUMmahalanobisDistance (cov -> lowerCholeskyInverse.get(), thy data.row (j), thy data.row (k));
					double w = p [k] [nocp1] > 0.0 ? wj * mixingP * p [k] [im] / p [k] [nocp1] : 0.0;
					sumjk += 2.0 * w * exp (-b1 * djk); // factor 2 because d [j] [k] == d [k] [j]
				}
				sumjk += wj * wj; // for k == j. Is this ok now for probability weighing ????
				djj = NUMmahalanobisDistance (cov -> lowerCholeskyInverse.get(), thy data.row(j), cov -> centroid.get());
				sumj += wj * exp (-b2 * djj);
			}
			tnb = (1.0 / nd) * sumjk - 2.0 * pow (1.0 + beta2, - d2) * sumj + nd * pow (gamma, - d2); // n *

			his data [im] [ilnmu] = lnmu = 0.5 * log (mu2 * mu2 / (mu2 + var)); //log (sqrt (mu2 * mu2 /(mu2 + var)));
			his data [im] [ilnvar] = lnvar = sqrt (log ( (mu2 + var) / mu2));
			his data [im] [iprob] = prob = NUMlogNormalQ (tnb, lnmu, lnvar);
			his data [im] [ih] = NUMsqrt1_2 / beta;
			his data [im] [id] = d;
			his data [im] [itnb] = tnb;
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal for BHEP not created.");
	}
}

/* End of file GaussianMixture.cpp 1555*/
