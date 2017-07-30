/* GaussianMixture.cpp
 *
 * Copyright (C) 2011-2014, 2015-2016 David Weenink
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
#include "NUMlapack.h"
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

const char32 *GaussianMixture_criterionText (int criterion) {
	const char32 *criterionText[6] =  { U"(1/n)*LLH", U"(1/n)*MML", U"(1/n)*BIC", U"(1/n)*AIC", U"(1/n)*AICc", U"(1/n)*CD_LLH" };
	return criterion >= 0 && criterion < 7 ? criterionText[criterion] : U"(1/n)*ln(p)";
}

void GaussianMixture_removeComponent (GaussianMixture me, long component);
void GaussianMixture_removeComponent_bookkeeping (GaussianMixture me, long component, double **p, long numberOfRows);
int GaussianMixture_and_TableOfReal_getProbabilities (GaussianMixture me, TableOfReal thee, long component, double **p);
void GaussianMixture_and_TableOfReal_getGammas (GaussianMixture me, TableOfReal thee, double **gamma, double *lnp);
double GaussianMixture_getLikelihoodValue (GaussianMixture me, double **p, long numberOfRows, int onlyLikelyhood);
void GaussianMixture_updateProbabilityMarginals (GaussianMixture me, double **p, long numberOfRows);
long GaussianMixture_getNumberOfParametersInComponent (GaussianMixture me);

static void NUMdvector_scaleAsProbabilities (double *v, long n) {
	double sum = 0;
	for (long i = 1; i <= n; i++) {
		sum += v[i];
	}
	if (sum > 0) for (long i = 1; i <= n; i++) {
			v[i] /= sum;
		}
}

static void GaussianMixture_updateCovariance (GaussianMixture me, long component, double **data, long numberOfRows, double **p) {
	if (component < 1 || component > my numberOfComponents) {
		return;
	}
	Covariance thee = my covariances->at [component];

	double mixprob = my mixingProbabilities [component];
	double gsum = p [numberOfRows + 1] [component];
	// update the means

	for (long j = 1; j <= thy numberOfColumns; j ++) {
		thy centroid [j] = 0;
		for (long i = 1; i <= numberOfRows; i ++) {
			double gamma = mixprob * p [i] [component] / p [i] [my numberOfComponents + 1];
			thy centroid [j] += gamma * data [i] [j] ; // eq. Bishop 9.17
		}
		thy centroid [j] /= gsum;
	}

	// update covariance with the new mean

	if (thy numberOfRows == 1) { // 1xn covariance
		for (long j = 1; j <= thy numberOfColumns; j ++) {
			thy data[1][j] = 0;
		}
		for (long i = 1; i <= numberOfRows; i ++) {
			double gamma = mixprob * p [i] [component] / p [i] [my numberOfComponents + 1];
			double gdn = gamma / gsum;
			for (long j = 1; j <= thy numberOfColumns; j ++) {
				double xj = thy centroid [j] - data [i] [j];
				thy data [1] [j] += gdn * xj * xj;
			}
		}
	} else { // nxn covariance
		for (long j = 1; j <= thy numberOfRows; j ++)
			for (long k = j; k <= thy numberOfColumns; k ++) {
				thy data [k] [j] = thy data [j] [k] = 0;
			}
		for (long i = 1; i <= numberOfRows; i ++) {
			double gamma = mixprob * p [i] [component] / p [i] [my numberOfComponents + 1];
			double gdn = gamma / gsum; // we cannot divide by nk - 1, this could cause instability
			for (long j = 1; j <= thy numberOfColumns; j ++) {
				double xj = thy centroid [j] - data [i] [j];
				for (long k = j; k <= thy numberOfColumns; k ++) {
					thy data [j] [k] = thy data [k] [j] += gdn * xj * (thy centroid [k] - data [i] [k]);
				}
			}
		}
	}
	thy numberOfObservations = my mixingProbabilities[component] * numberOfRows;
}

static void GaussianMixture_addCovarianceFraction (GaussianMixture me, long im, Covariance him, double fraction) {
	if (im < 1 || im > my numberOfComponents || fraction == 0) {
		return;
	}

	Covariance thee = my covariances->at [im];

	// prevent instability: add lambda fraction of global covariances

	if (thy numberOfRows == 1) {
		for (long j = 1; j <= thy numberOfColumns; j ++) {
			thy data [1] [j] += fraction * his data [j] [j];
		}
	} else {
		for (long j = 1; j <= thy numberOfColumns; j++) {
			for (long k = j; k <= thy numberOfColumns; k++) {
				thy data [k] [j] = thy data [j] [k] += fraction * his data [j] [k];
			}
		}
	}
}

void structGaussianMixture :: v_info () {
	our structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of components: ", our numberOfComponents);
	MelderInfo_writeLine (U"Dimension of component: ", our dimension);
	MelderInfo_writeLine (U"Mixing probabilities:");
	for (long im = 1; im <= numberOfComponents; im ++) {
		MelderInfo_writeLine (U"  ", im, U": p = ", our mixingProbabilities [im],
		                       U"  Name =  \"", Thing_getName (our covariances->at [im]), U"\"");
	}
}

static void GaussianMixture_setLabelsFromTableOfReal (GaussianMixture me, TableOfReal thee) {
	for (long im = 1; im <= my numberOfComponents; im ++) {
		Covariance cov = my covariances->at [im];
		for (long j = 1; j <= my dimension; j ++) {
			TableOfReal_setColumnLabel (cov, j, thy columnLabels [j]);
		}
	}
}

// only from big to reduced or same
static void Covariance_into_Covariance (Covariance me, Covariance thee) {
	if (my numberOfColumns != thy numberOfColumns) {
		Melder_throw (U"Dimensions must be equal.");
	}

	SSCP_unExpand (thee); // to its original state

	thy numberOfObservations = my numberOfObservations;
	// copy centroid & column labels
	for (long ic = 1; ic <= my numberOfColumns; ic ++) {
		thy centroid[ic] = my centroid[ic];
	}
	NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, thy numberOfColumns);
	// Are the matrix sizes equal
	if (my numberOfRows == thy numberOfRows) {
		NUMstrings_copyElements (my rowLabels, thy rowLabels, 1, thy numberOfRows);
		NUMmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);
		return;
	} else {
		for (long ir = 1; ir <= my numberOfRows; ir ++) {
			for (long ic = ir; ic <= my numberOfColumns; ic ++) {
				long dij = ic - ir;
				if (dij < thy numberOfRows) {
					thy data [dij + 1] [ic] = my data [ir] [ic];
				}
			}
		}
	}
}

static void GaussianMixture_setDefaultMixtureNames (GaussianMixture me) {
	for (long im = 1; im <= my numberOfComponents; im ++) {
		Covariance cov = my covariances->at [im];
		Thing_setName (cov, Melder_cat (U"m", im));
	}
}

autoGaussianMixture GaussianMixture_create (long numberOfComponents, long dimension, long storage) {
	try {
		autoGaussianMixture me = Thing_new (GaussianMixture);
		my numberOfComponents = numberOfComponents;
		my dimension = dimension;
		my mixingProbabilities = NUMvector<double> (1, numberOfComponents);
		my covariances = CovarianceList_create ();
		for (long im = 1; im <= numberOfComponents; im ++) {
			autoCovariance cov = Covariance_create_reduceStorage (dimension, storage);
			my covariances -> addItemAtPosition_move (cov.move(), im);
		}
		for (long im = 1; im <= numberOfComponents; im ++) {
			my mixingProbabilities[im] = 1.0 / numberOfComponents;
		}
		GaussianMixture_setDefaultMixtureNames (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"GaussianMixture not created.");
	}
}

/* c is double vector 1..dimension !!!!!! */
int GaussianMixture_generateOneVector (GaussianMixture me, double *c, char32 **covname, double *buf) {
	try {
		double p = NUMrandomUniform (0.0, 1.0);
		long im = NUMgetIndexFromProbability (my mixingProbabilities, my numberOfComponents, p);
		Covariance thee = my covariances->at [im];
		*covname = thy name;
		if (thy numberOfRows == 1) { // 1xn reduced form
			for (long i = 1; i <= my dimension; i ++) {
				c[i] = NUMrandomGauss (thy centroid [i], sqrt (thy data [1] [i]));
			}
		} else { // nxn
			if (! thy pca) {
				SSCP_expandPCA (thee);    // on demand expanding
			}
			Covariance_and_PCA_generateOneVector (thee, thy pca.get(), c, buf);
		}
		return 1;
	} catch (MelderError) {
		Melder_throw (me, U": vector not generated.");
	}
}

autoGaussianMixture TableOfReal_to_GaussianMixture_fromRowLabels (TableOfReal me, long storage) {
	try {
		autoStrings rowLabels = TableOfReal_extractRowLabels (me);
		autoDistributions dist = Strings_to_Distributions (rowLabels.get());
		long numberOfComponents = dist -> numberOfRows;

		autoGaussianMixture thee = GaussianMixture_create (numberOfComponents, my numberOfColumns, storage);

		GaussianMixture_setLabelsFromTableOfReal (thee.get(), me);

		for (long i = 1; i <= numberOfComponents; i ++) {
			autoTableOfReal tab = TableOfReal_extractRowsWhereLabel (me, kMelder_string_EQUAL_TO, dist -> rowLabels [i]);
			autoCovariance cov = TableOfReal_to_Covariance (tab.get());
			Covariance_into_Covariance (cov.get(), thy covariances->at [i]);
			Thing_setName (thy covariances->at [i], dist -> rowLabels [i]);
		}
		for (long im = 1; im <= numberOfComponents; im ++) {
			thy mixingProbabilities [im] = dist -> data [im] [1] / my numberOfRows;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no GaussianMixture created.");
	}
}

autoCovariance GaussianMixture_to_Covariance_between (GaussianMixture me) {
	try {
		autoCovariance thee = Covariance_create (my dimension);

		//	First the new centroid, based on the mixture centroids

		double nobs_total = 0;
		for (long i = 1; i <= my numberOfComponents; i ++) {
			Covariance him = my covariances->at [i];
			double nobs = his numberOfObservations; // the weighting factor
			for (long ic = 1; ic <= my dimension; ic ++) {
				thy centroid [ic] += nobs * his centroid [ic];
			}
			nobs_total += nobs;
		}
		for (long ic = 1; ic <= my dimension; ic ++) {
			thy centroid[ic] /= nobs_total;
		}

		Covariance cov = my covariances->at [1];
		for (long i = 1; i <= thy numberOfColumns; i ++) {
			if (cov -> columnLabels [i]) {
				TableOfReal_setColumnLabel (thee.get(), i, cov -> columnLabels [i]);
				TableOfReal_setRowLabel (thee.get(), i, cov -> columnLabels [i]); // if diagonal !
			}
		}

		// Between covariance, scale by the number of observations

		for (long i = 1; i <= my numberOfComponents; i ++) {
			Covariance him = my covariances->at [i];
			double nobs = his numberOfObservations - 1; // we loose 1 degree of freedom
			for (long ir = 1; ir <= my dimension; ir ++) {
				double dir = his centroid [ir] - thy centroid [ir];
				for (long ic = ir; ic <= my dimension; ic ++) {
					double dic = his centroid[ic] - thy centroid[ic];
					thy data [ir] [ic] = thy data [ic] [ir] += nobs * dir * dic;
				}
			}
		}

		// Scale back

		for (long ir = 1; ir <= my dimension; ir ++) {
			for (long ic = ir; ic <= my dimension; ic ++) {
				thy data [ir] [ic] = thy data [ic] [ir] /= nobs_total;
			}
		}

		thy numberOfObservations = nobs_total;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Covariance (between) created.");
	}
}

autoCovariance GaussianMixture_to_Covariance_within (GaussianMixture me) {
	try {
		autoCovariance thee = Covariance_create (my dimension);

		for (long i = 1; i <= my numberOfComponents; i ++) {
			double p = my mixingProbabilities [i];
			Covariance him = my covariances->at [i];
			if (his numberOfRows == 1) {
				for (long ic = 1; ic <= my dimension; ic ++) {
					thy data [ic] [ic] += p * his data [1] [ic];
				}
			} else {
				for (long ir = 1; ir <= my dimension; ir ++) {
					for (long ic = ir; ic <= my dimension; ic ++) {
						thy data [ir] [ic] = thy data [ic] [ir] += p * his data [ir] [ic];
					}
				}
			}
			thy numberOfObservations += his numberOfObservations - 1; // we loose a degree of freedom?
		}

		// Leave centroid at 0 so we can add the within and between covariance nicely
		// Copy row labels from columns, because covar might be diagonal
		TableOfReal_copyLabels (my covariances->at [1], thee.get(), -1, 1);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Covariance (within) created.");
	}
}

autoCovariance GaussianMixture_to_Covariance_total (GaussianMixture me) {
	try {
		autoCovariance thee = GaussianMixture_to_Covariance_between (me);
		autoCovariance within = GaussianMixture_to_Covariance_within (me);

		for (long ir = 1; ir <= my dimension; ir++) {
			for (long ic = ir; ic <= my dimension; ic++) {
				thy data[ir][ic] = thy data[ic][ir] += within -> data[ir][ic];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Covariance (total) created.");
	}
}

autoCovariance GaussianMixture_extractComponent (GaussianMixture me, long component) {
	try {
		if (component < 1 || component > my numberOfComponents) {
			Melder_throw (U"Illegal component.");
		}
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
		for (long im = 1; im <= my numberOfComponents; im ++) {
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

		for (long im = 1; im <= my numberOfComponents; im ++) {
			Covariance cov = my covariances->at [im];
			if (im == 1) {
				for (long j = 1; j <= my dimension; j ++) {
					TableOfReal_setColumnLabel (thee.get(), j, cov -> columnLabels [j]);
				}
			}
			TableOfReal_setRowLabel (thee.get(), im, Thing_getName (cov));
			for (long j = 1; j <= my dimension; j ++) {
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

void GaussianMixture_getIntervalsAlongDirections (GaussianMixture me, long d1, long d2, double nsigmas, double *xmin, double *xmax, double *ymin, double *ymax) {
	*xmin = *xmax = *ymin = *ymax = undefined;
	if (d1 < 1 || d1 > my dimension || d2 < 1 || d2 > my dimension) {
		Melder_throw (U"Incorrect directions.");
	}
	autoSSCPList sscps = SSCPList_extractTwoDimensions (my covariances->asSSCPList(), d1, d2);
	SSCPList_getEllipsesBoundingBoxCoordinates (sscps.get(), -nsigmas, 0, xmin, xmax, ymin, ymax);
}

void GaussianMixture_getIntervalAlongDirection (GaussianMixture me, long d, double nsigmas, double *xmin, double *xmax) {
	double ymin, ymax;
	GaussianMixture_getIntervalsAlongDirections (me, d, d, nsigmas, xmin, xmax, &ymin, &ymax);
}

void GaussianMixture_and_PCA_getIntervalsAlongDirections (GaussianMixture me, PCA thee, long d1, long d2, double nsigmas, double *xmin, double *xmax, double *ymin, double *ymax) {
	if (my dimension != thy dimension || d1 < 1 || d1 > my dimension || d2 < 1 || d2 > my dimension) {
		Melder_throw (U"Incorrect directions.");
	}
	autoSSCPList sscps = SSCPList_toTwoDimensions (my covariances->asSSCPList(), thy eigenvectors [d1], thy eigenvectors [d2]);
	SSCPList_getEllipsesBoundingBoxCoordinates (sscps.get(), -nsigmas, 0, xmin, xmax, ymin, ymax);
}


void GaussianMixture_and_PCA_getIntervalAlongDirection (GaussianMixture me, PCA thee, long d, double nsigmas, double *xmin, double *xmax) {
	GaussianMixture_and_PCA_getIntervalsAlongDirections (me, thee, d, d, nsigmas, xmin, xmax, nullptr, nullptr);
}

void GaussianMixture_and_PCA_drawMarginalPdf (GaussianMixture me, PCA thee, Graphics g, long d, double xmin, double xmax, double ymin, double ymax, long npoints, long nbins, int garnish) {
	if (my dimension != thy dimension || d < 1 || d > my dimension) {
		Melder_warning (U"Dimensions don't agree.");
		return;
	}

	if (npoints <= 1) {
		npoints = 1000;
	}
	autoNUMvector<double> p (1, npoints);
	double nsigmas = 2;

	if (xmax <= xmin) {
		GaussianMixture_and_PCA_getIntervalAlongDirection (me, thee, d, nsigmas, &xmin, &xmax);
	}

	double pmax = 0.0, dx = (xmax - xmin) / npoints, x1 = xmin + 0.5 * dx;
	double scalef = nbins <= 0 ? 1 : 1; // TODO
	for (long i = 1; i <= npoints; i++) {
		double x = x1 + (i - 1) * dx;
		p[i] = scalef * GaussianMixture_getMarginalProbabilityAtPosition (me, thy eigenvectors[d], x);
		if (p[i] > pmax) {
			pmax = p[i];
		}
	}
	if (ymin >= ymax) {
		ymin = 0.0;
		ymax = pmax;
	}

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_function (g, p.peek(), 1, npoints, x1, xmax - 0.5 * dx);
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_markBottom (g, xmin, true, true, false, nullptr);
		Graphics_markBottom (g, xmax, true, true, false, nullptr);
		Graphics_markLeft (g, ymin, true, true, false, nullptr);
		Graphics_markLeft (g, ymax, true, true, false, nullptr);
	}
}

void GaussianMixture_drawMarginalPdf (GaussianMixture me, Graphics g, long d, double xmin, double xmax, double ymin, double ymax, long npoints, long nbins, int garnish) {
	if (d < 1 || d > my dimension) {
		Melder_warning (U"Dimension doesn't agree.");
		return;
	}
	if (npoints <= 1) {
		npoints = 1000;
	}
	autoNUMvector<double> p (1, npoints);
	autoNUMvector<double> v (1, my dimension);

	double nsigmas = 2;
	if (xmax <= xmin) {
		GaussianMixture_getIntervalAlongDirection (me, d, nsigmas, &xmin, &xmax);
	}

	double pmax = 0, dx = (xmax - xmin) / (npoints - 1);
	double scalef = nbins <= 0 ? 1 : 1; // TODO
	for (long i = 1; i <= npoints; i++) {
		double x = xmin + (i - 1) * dx;
		for (long k = 1; k <= my dimension; k++) {
			v[k] = k == d ? 1 : 0;
		}
		p[i] = scalef * GaussianMixture_getMarginalProbabilityAtPosition (me, v.peek(), x);
		if (p[i] > pmax) {
			pmax = p[i];
		}
	}
	if (ymin >= ymax) {
		ymin = 0;
		ymax = pmax;
	}

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_function (g, p.peek(), 1, npoints, xmin, xmax);
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_markBottom (g, xmin, true, true, false, nullptr);
		Graphics_markBottom (g, xmax, true, true, false, nullptr);
		Graphics_markLeft (g, ymin, true, true, false, nullptr);
		Graphics_markLeft (g, ymax, true, true, false, nullptr);
	}
}

void GaussianMixture_and_PCA_drawConcentrationEllipses (GaussianMixture me, PCA him, Graphics g, double scale, int confidence, char32 *label, long d1, long d2, double xmin, double xmax, double ymin, double ymax, int fontSize, int garnish) {
	if (my dimension != his dimension) {
		Melder_warning (U"Dimensions don't agree.");
		return;
	}
	int d1_inverted = 0, d2_inverted = 0;

	if (d1 < 0) {
		d1 = labs (d1);
		Eigen_invertEigenvector (him, d1);
		d1_inverted = 1;
	}
	if (d2 < 0) {
		d2 = labs (d2);
		Eigen_invertEigenvector (him, d2);
		d2_inverted = 1;
	}

	autoSSCPList thee = SSCPList_toTwoDimensions (my covariances->asSSCPList(), his eigenvectors [d1], his eigenvectors [d2]);   // FIXME

	if (d1_inverted) {
		Eigen_invertEigenvector (him, d1);
	}
	if (d2_inverted) {
		Eigen_invertEigenvector (him, d2);
	}

	SSCPList_drawConcentrationEllipses (thee.get(), g, -scale, confidence, label, 1, 2, xmin, xmax, ymin, ymax, fontSize, 0);

	if (garnish) {
		char32 llabel[40];
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Melder_sprint (llabel,40, U"pc ", d2);
		Graphics_textLeft (g, true, llabel);
		Graphics_marksBottom (g, 2, true, true, false);
		Melder_sprint (llabel,40, U"pc ", d1);
		Graphics_textBottom (g, true, llabel);
	}
}

void GaussianMixture_drawConcentrationEllipses (GaussianMixture me, Graphics g, double scale, int confidence, char32 *label, int pcaDirections, long d1, long d2, double xmin, double xmax, double ymin, double ymax, int fontSize, int garnish) {
	if (d1 == 0 && d2 == 0) {
		d1 = 1;
		d2 = 2;
	}
	if (labs (d1) > my dimension || labs (d2) > my dimension) {
		return;
	}

	if (! pcaDirections) {
		SSCPList_drawConcentrationEllipses (my covariances->asSSCPList(), g, -scale, confidence, label,
		                                  labs (d1), labs (d2), xmin, xmax, ymin, ymax, fontSize, garnish);
		return;
	}

	autoPCA him = GaussianMixture_to_PCA (me);

	GaussianMixture_and_PCA_drawConcentrationEllipses (me, him.get(), g, scale, confidence, label, d1, d2,
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
			for (long im = 1; im <= my numberOfComponents; im ++) {
				Covariance covi = my covariances->at [im];
				covi -> centroid [1] = m1;
				covi -> data [1] [1] = dm * dm;
				m1 += dm;
				covi -> numberOfObservations = thy numberOfRows / my numberOfComponents;
			}
		} else {
			autoPCA pca = SSCP_to_PCA (cov_t.get());
			autoSSCP s2d = SSCP_toTwoDimensions (cov_t.get(), pca -> eigenvectors [1], pca -> eigenvectors [2]);
			autoConfiguration means2d = Configuration_create (my numberOfComponents, 2);

			double a, b, cs, sn;
			NUMeigencmp22 (s2d -> data [1] [1], s2d -> data [1] [2], s2d -> data[2][2], &a, &b, &cs, &sn);

			a = nSigmas * sqrt (a);
			b = nSigmas * sqrt (b);
			double angle = 0.0, angle_inc = NUM2pi / my numberOfComponents;
			for (long im = 1; im <= my numberOfComponents; im++, angle += angle_inc) {
				double xc = a * (1.0 + NUMrandomUniform (-ru_range, ru_range)) * cos (angle);
				double yc = b * (1.0 + NUMrandomUniform (-ru_range, ru_range)) * sin (angle);
				means2d -> data [im] [1] = s2d -> centroid [1] + xc * cs - yc * sn;
				means2d -> data [im] [2] = s2d -> centroid [2] + xc * sn + yc * cs;
			}

			// reconstruct the n-dimensional means from the 2-d from the eigenvectors

			autoTableOfReal means = PCA_and_Configuration_to_TableOfReal_reconstruct (pca.get(), means2d.get());

			for (long im = 1; im <= my numberOfComponents; im ++) {
				Covariance covi = my covariances->at [im];
				for (long ic = 1; ic <= my dimension; ic ++) {
					covi -> centroid [ic] = means -> data [im] [ic];
				}
				covi -> numberOfObservations = thy numberOfRows / my numberOfComponents;
			}

			// Trick: use the new means to get the between SSCP,
			// if there is only one component the cov_b will be zero!

			autoCovariance cov_b = GaussianMixture_to_Covariance_between (me);
			double var_t = SSCP_getTotalVariance (cov_t.get());
			double var_b = SSCP_getTotalVariance (cov_b.get());
			if (var_b >= var_t) { // we have chosen our initial values too far out
				double scale = 0.9 * sqrt (var_t / var_b);
				for (long im = 1; im <= my numberOfComponents; im ++) {
					Covariance covi = my covariances->at [im];
					for (long ic = 1; ic <= my dimension; ic ++) {
						covi -> centroid [ic] -= (1.0 - scale) * (covi -> centroid [ic] - cov_b -> centroid[ic]);
					}
				}
				cov_b = GaussianMixture_to_Covariance_between (me);
			}

			// Within variances are now (total - between) / numberOfComponents;

			for (long ir = 1; ir <= my dimension; ir ++) {
				for (long ic = ir; ic <= my dimension; ic ++) {
					double scalef = my numberOfComponents == 1 ? 1.0 : (var_b / var_t) / my numberOfComponents;
					cov_t -> data [ic] [ir] = cov_t -> data [ir] [ic] *= scalef;
				}
			}

			// Copy them

			for (long im = 1; im <= my numberOfComponents; im ++) {
				Covariance cov = my covariances->at [im];
				if (cov -> numberOfRows == 1) {
					for (long ic = 1; ic <= my dimension; ic ++) {
						cov -> data [1] [ic] = cov_t -> data [ic] [ic];
					}
				} else {
					for (long ir = 1; ir <= my dimension; ir ++) {
						for (long ic = ir; ic <= my dimension; ic ++) {
							cov -> data [ir] [ic] = cov -> data [ic] [ir] = cov_t -> data [ir] [ic];
						}
					}
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no initial guess possible.");
	}
}


autoClassificationTable GaussianMixture_and_TableOfReal_to_ClassificationTable (GaussianMixture me, TableOfReal thee) {
	try {
		autoClassificationTable him = ClassificationTable_create (thy numberOfRows, my numberOfComponents);
		for (long im = 1; im <= my numberOfComponents; im ++) {
			Covariance cov = my covariances->at [im];
			SSCP_expandLowerCholesky (cov);
			TableOfReal_setColumnLabel (him.get(), im, Thing_getName (cov));
		}

		double ln2pid = -0.5 * my dimension * log (NUM2pi);
		autoNUMvector<double> lnN (1, my numberOfComponents);
		for (long i = 1; i <=  thy numberOfRows; i ++) {
			double psum = 0.0;
			for (long im = 1; im <= my numberOfComponents; im ++) {
				Covariance cov = my covariances->at [im];
				double dsq = NUMmahalanobisDistance_chi (cov -> lowerCholesky, thy data [i], cov -> centroid, cov -> numberOfRows, my dimension);
				lnN[im] = ln2pid - 0.5 * (cov -> lnd + dsq);
				psum += his data[i][im] = my mixingProbabilities[im] * exp (lnN [im]);
			}
			if (psum == 0) { // p's might be too small (underflow), make the largest equal to sfmin
				double lnmax = -1e308;
				long imm = 1;
				for (long im = 1; im <= my numberOfComponents; im ++) {
					if (lnN [im] > lnmax) {
						lnmax = lnN [im];
					} imm = im;
				}
				his data [i] [imm] = NUMfpp -> sfmin;
			}

			// for (long im = 1; im <= my numberOfComponents; im++) { his data[i][im] /= psum; }

			TableOfReal_setRowLabel (him.get(), i, thy rowLabels [i]);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"No ClassificationTable created from GaussianMixture & TableOfReal.");
	}
}

void GaussianMixture_and_TableOfReal_getGammas (GaussianMixture me, TableOfReal thee, double **gamma, double *p_lnp) {
	try {
		for (long im = 1; im <= my numberOfComponents; im ++) {
			Covariance cov = my covariances->at [im];
			SSCP_expandLowerCholesky (cov);
		}

		double *nk = gamma[thy numberOfRows + 1];
		for (long im = 1; im <= my numberOfComponents; im ++) {
			nk[im] = 0.0;
		}

		double lnp = 0.0;
		double ln2pid = - 0.5 * my dimension * log (NUM2pi);
		autoNUMvector<double> lnN (1, my numberOfComponents);
		for (long i = 1; i <=  thy numberOfRows; i ++) {
			double rowsum = 0.0;
			for (long im = 1; im <= my numberOfComponents; im ++) {
				Covariance cov = my covariances->at [im];
				double dsq = NUMmahalanobisDistance_chi (cov -> lowerCholesky, thy data [i], cov -> centroid, cov -> numberOfRows, my dimension);
				lnN [im] = ln2pid - 0.5 * (cov -> lnd + dsq);
				gamma [i] [im] = my mixingProbabilities[im] * exp (lnN [im]); // eq. Bishop 9.16
				rowsum += gamma [i] [im];
			}

			// If the gamma[i]'s are too small, their sum will be zero and the scaling will overflow

			if (rowsum == 0.0) {
				continue;    // This is ok because gamma[i]'s will all be zero
			}

			// scale gamma and get log(likehood) (Bishop eq. 9.40)

			for (long im = 1; im <= my numberOfComponents; im ++) {
				gamma [i] [im] /= rowsum; // eq. Bishop 9.16
				nk [im] += gamma [i] [im]; // eq. Bishop 9.18
				lnp += gamma [i] [im] * (log (my mixingProbabilities [im])  + lnN [im]); // eq. Bishop 9.40
			}
		}
		if (p_lnp) {
			*p_lnp = lnp;
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no gammas.");
	}
}

void GaussianMixture_splitComponent (GaussianMixture me, long component) {
	try {
		if (component < 1 || component > my numberOfComponents) {
			Melder_throw (U"Illegal component.");
		}
		Covariance thee = my covariances->at [component];
		// Always new PCA because we cannot be sure of data unchanged.
		SSCP_expandPCA (thee);
		autoCovariance cov1 = Data_copy (thee);
		autoCovariance cov2 = Data_copy (thee);
		SSCP_unExpandPCA (cov1.get());
		SSCP_unExpandPCA (cov2.get());

		// Eventually cov1 replaces component, cov2 at end

		autoNUMvector<double> mixingProbabilities (1, my numberOfComponents + 1);
		for (long i = 1; i <= my numberOfComponents; i ++) {
			mixingProbabilities [i] = my mixingProbabilities [i];
		}
		double gamma = 0.5, lambda = 0.5, eta = 0.5, mu = 0.5;
		mixingProbabilities [component] = gamma * my mixingProbabilities [component];
		mixingProbabilities [my numberOfComponents + 1] = (1.0 - gamma) * my mixingProbabilities [component];
		double mp12 =  mixingProbabilities [component] / mixingProbabilities[my numberOfComponents + 1];
		double factor1 = (eta - eta * lambda * lambda - 1.0) / gamma + 1.0;
		double factor2 = (eta * lambda * lambda - eta - lambda * lambda) / (1.0 - gamma) + 1.0;
		double *ev = thy pca -> eigenvectors [1];
		double d2 = thy pca -> eigenvalues [1];

		for (long i = 1; i <= my dimension; i ++) {
			cov1 -> centroid [i] -= (1.0 / sqrt (mp12)) * sqrt (d2) * mu * ev [i];
			cov2 -> centroid [i] +=        sqrt (mp12)  * sqrt (d2) * mu * ev [i];

			if (thy numberOfRows == 1) { // diagonal
				cov1 -> data [1] [i] = cov1 -> data [1] [i] / mp12 + factor1 * d2;
				cov1 -> data [1] [i] = cov2 -> data [i] [i] * mp12 + factor2 * d2;
			} else {
				for (long j = i; j <= my dimension; j++) {
					cov1 -> data [j] [i] = cov1 -> data [i] [j] = cov1 -> data [i] [j] / mp12 + factor1 * d2 * ev [i] * ev [j];
					cov2 -> data [j] [i] = cov2 -> data [i] [j] = cov2 -> data [i] [j] * mp12 + factor2 * d2 * ev [i] * ev [j];
				}
			}
		}
		cov1 -> numberOfObservations *= gamma;
		cov2 -> numberOfObservations *= 1.0 - gamma;

		// Replace cov1 at component + add cov2. If something goes wrong we must be able to restore original!
		try {
			Thing_setName (cov2.get(), Melder_cat (Thing_getName (cov2.get()), U"-", my numberOfComponents + 1));
			my covariances -> addItem_move (cov2.move());
		} catch (MelderError) {
			Melder_throw (me, U" cannot add new component.");
		}

		my covariances -> replaceItem_move (cov1.move(), component);

		my numberOfComponents ++;
		NUMvector_free<double> (my mixingProbabilities, 1);
		my mixingProbabilities = mixingProbabilities.transfer();
	} catch (MelderError) {
		Melder_throw (me, U": component ", component, U" cannot be split.");
	}
}

int GaussianMixture_and_TableOfReal_getProbabilities (GaussianMixture me, TableOfReal thee, long component, double **p) {
	try {
		double ln2pid = my dimension * log (NUM2pi);

		// Update only one component or all?

		long icb = 1, ice = my numberOfComponents;
		if (component > 0 && component <= my numberOfComponents) {
			icb = ice = component;
		}

		for (long ic = icb; ic <= ice; ic ++) {
			Covariance him = my covariances->at [ic];
			SSCP_expandLowerCholesky (him);

			for (long i = 1; i <= thy numberOfRows; i++) {
				double dsq = NUMmahalanobisDistance_chi (his lowerCholesky, thy data [i], his centroid, his numberOfRows, my dimension);
				double prob = exp (- 0.5 * (ln2pid + his lnd + dsq));
				prob = prob < 1e-300 ? 1e-300 : prob; // prevent p from being zero
				p[i][ic] = prob;
			}
		}

		GaussianMixture_updateProbabilityMarginals (me, p, thy numberOfRows);
		return 1;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no probabilies could be calculated.");
	}
}

void GaussianMixture_expandPCA (GaussianMixture me) {
	for (long im = 1; im <= my numberOfComponents; im ++) {
		Covariance him = my covariances->at [im];
		if (his numberOfRows == 1) {
			Melder_throw (U"Nothing to expand.");
		}
		his pca = SSCP_to_PCA (him);
	}
}

void GaussianMixture_unExpandPCA (GaussianMixture me) {
	for (long im = 1; im <= my numberOfComponents; im ++) {
		SSCP_unExpandPCA (my covariances->at [im]);
	}
}

void GaussianMixture_and_TableOfReal_improveLikelihood (GaussianMixture me, TableOfReal thee, double delta_lnp, long maxNumberOfIterations, double lambda, int criterion) {
	try {
		const char32 *criterionText = GaussianMixture_criterionText (criterion);

		// The global covariance matrix is added with scaling coefficient lambda during updating the
		// mixture covariances to prevent numerical instabilities.

		autoCovariance covg = TableOfReal_to_Covariance (thee);
		autoNUMmatrix<double> pp (1, thy numberOfRows + 1, 1, my numberOfComponents + 1);
		double *nk = pp[thy numberOfRows + 1]; // last row has the column marginals n(k)
		if (! GaussianMixture_and_TableOfReal_getProbabilities (me, thee, 0, pp.peek())) {
			Melder_throw (U"Iteration not started. Too many components?");
		}
		double lnp = GaussianMixture_getLikelihoodValue (me, pp.peek(), thy numberOfRows, criterion);
		long iter = 0;
		autoMelderProgress progress (U"Improve likelihood...");
		try {
			double lnp_prev, lnp_start = lnp / thy numberOfRows;
			do {

				// E-step: get responsabilities (gamma) with current parameters
				// See C. Bishop (2006), Pattern reconition and machine learning, Springer, page 439...

				lnp_prev = lnp;
				iter ++;
				// M-step: 1. new means & covariances

				for (long im = 1; im <= my numberOfComponents; im ++) {
					GaussianMixture_updateCovariance (me, im, thy data, thy numberOfRows, pp.peek());
					GaussianMixture_addCovarianceFraction (me, im, covg.get(), lambda);
				}

				// M-step: 2. new mixingProbabilities

				for (long im = 1; im <= my numberOfComponents; im ++) {
					my mixingProbabilities[im] = nk[im] / thy numberOfRows;
				}
				if (! GaussianMixture_and_TableOfReal_getProbabilities (me, thee, 0, pp.peek())) {
					break;
				}
				lnp = GaussianMixture_getLikelihoodValue (me, pp.peek(), thy numberOfRows, criterion);
				Melder_progress ((double) iter / (double) maxNumberOfIterations, criterionText, U": ", lnp / thy numberOfRows, U", L0: ", lnp_start);
			} while (fabs ((lnp - lnp_prev) / lnp_prev) > delta_lnp && iter < maxNumberOfIterations);
		} catch (MelderError) {
			Melder_clearError ();
		}

		// During EM, covariances were underestimated by a factor of (n-1)/n. Correction now.

		for (long im = 1; im <= my numberOfComponents; im ++) {
			Covariance cov = my covariances->at [im];
			if (cov -> numberOfObservations > 1.5) {
				if (cov -> numberOfRows == 1) {
					for (long j = 1; j <= thy numberOfColumns; j ++) {
						cov -> data [1] [j] *= cov -> numberOfObservations / (cov -> numberOfObservations - 1);
					}
				} else {
					for (long j = 1; j <= thy numberOfColumns; j ++)
						for (long k = j; k <= thy numberOfColumns; k ++) {
							cov -> data [j] [k] = cov -> data [k] [j] *= cov -> numberOfObservations / (cov -> numberOfObservations - 1.0);
						}
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": likelihood cannot be improved.");
	}
}

long GaussianMixture_getNumberOfParametersInComponent (GaussianMixture me) {
	Melder_assert (my covariances->size > 0);
	Covariance thee = my covariances->at [1];
	// if diagonal) d (means) + d (stdev)
	// else  n + n(n+1)/2
	return thy numberOfRows == 1 ? 2 * thy numberOfColumns : thy numberOfColumns * (thy numberOfColumns + 3) / 2;
}

void GaussianMixture_updateProbabilityMarginals (GaussianMixture me, double **p, long numberOfRows) {
	long nocp1 = my numberOfComponents + 1, norp1 = numberOfRows + 1;

	for (long ic = 1; ic <= my numberOfComponents; ic ++) {
		p [norp1] [ic] = 0.0;
	}
	for (long i = 1; i <= numberOfRows; i ++) {
		double rowsum = 0.0;
		for (long ic = 1; ic <= my numberOfComponents; ic ++) {
			rowsum += my mixingProbabilities [ic] * p [i] [ic];
		}
		p [i] [nocp1] = rowsum;
		for (long ic = 1; ic <= my numberOfComponents; ic ++) {
			p [norp1] [ic] += my mixingProbabilities[ic] * p [i] [ic] / p [i] [nocp1];
		}
	}
}

void GaussianMixture_removeComponent_bookkeeping (GaussianMixture me, long component, double **p, long numberOfRows) {
	// p is (NumberOfRows+1) by (numberOfComponents+1) !

	for (long i = 1; i <= numberOfRows + 1; i ++) {
		for (long ic = component; ic <= my numberOfComponents; ic ++) {
			p [i] [ic] = p [i] [ic + 1];
		}
	}
	GaussianMixture_updateProbabilityMarginals (me, p, numberOfRows);
	GaussianMixture_removeComponent (me, component);
}

double GaussianMixture_and_TableOfReal_getLikelihoodValue (GaussianMixture me, TableOfReal thee, int criterion) {
	double value = undefined;
	autoNUMmatrix<double> pp (1, thy numberOfRows + 1, 1, my numberOfComponents + 1);
	if (GaussianMixture_and_TableOfReal_getProbabilities (me, thee, 0, pp.peek())) {
		value = GaussianMixture_getLikelihoodValue (me, pp.peek(), thy numberOfRows, criterion);
	}
	return value;
}

double GaussianMixture_getLikelihoodValue (GaussianMixture me, double **p, long numberOfRows, int criterion) {
	// Because we try to _maximize_ a criterion, all criteria are negative numbers.

	if (criterion == GaussianMixture_CD_LIKELIHOOD) {
		double lnpcd = 0.0;
		for (long i = 1; i <= numberOfRows; i ++) {
			double psum = 0, lnsum = 0;
			for (long ic = 1; ic <= my numberOfComponents; ic ++) {
				double pp = my mixingProbabilities[ic] * p [i] [ic];
				psum += pp;
				lnsum += pp * log (pp);
			}
			if (psum > 0) {
				lnpcd += lnsum / psum;
			}
		}
		return lnpcd;
	}

	// The common factor for all other criteria is the log(likelihood)

	double lnp = 0.0;
	for (long i = 1; i <= numberOfRows; i ++) {
		double psum = 0.0;
		for (long ic = 1; ic <= my numberOfComponents; ic ++) {
			psum += my mixingProbabilities [ic] * p [i] [ic];
		}
		if (psum > 0.0) {
			lnp += log (psum);
		}
	}

	if (criterion == GaussianMixture_LIKELIHOOD) {
		return lnp;
	}

	double npars = GaussianMixture_getNumberOfParametersInComponent (me), np = npars * my numberOfComponents;
	if (criterion == GaussianMixture_MML) {
		/* Equation (15) in
			Mario A.T. Figueiredo, and Anil K. Jain, Unsupervised Learning of Finite Mixture Models :
			IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, VOL. 24, NO. 3, MARCH 2002

			L(theta,Y)= N/2*sum(m=1..k, log(n*alpha[k]/12)) +k/2*ln(n/12) +k(N+1)/2
				- log (sum(i=1..n, sum(m=1..k, alpha[k]*p(k))))
		*/
		double logmpn = 0.0;
		for (long ic = 1; ic <= my numberOfComponents; ic ++) {
			logmpn += log (my mixingProbabilities [ic]);
		}

		// a rewritten L(theta,Y) is

		return lnp - 0.5 * my numberOfComponents * (npars + 1) * (log (numberOfRows / 12.0) + 1.0)
		       + 0.5 * npars * logmpn;
	} else if (criterion == GaussianMixture_BIC) {
		return 2.0 * lnp - np * log (numberOfRows);
	} else if (criterion == GaussianMixture_AIC) {
		return 2.0 * (lnp - np);
	} else if (criterion == GaussianMixture_AICC) {
		np = npars * my numberOfComponents;
		return 2.0 * (lnp - np * (numberOfRows / (numberOfRows - np - 1.0)));
	}
	return lnp;
}

autoGaussianMixture GaussianMixture_and_TableOfReal_to_GaussianMixture_CEMM (GaussianMixture gm, TableOfReal thee, long minNumberOfComponents, double delta_l, long maxNumberOfIterations, double lambda, int criterion) {
	try {
		const char32 *criterionText = GaussianMixture_criterionText (criterion);
		bool deleteWeakComponents = ( minNumberOfComponents > 0 );
		autoGaussianMixture me = Data_copy (gm);
		autoNUMmatrix<double> p (1, thy numberOfRows + 2, 1, my numberOfComponents + 1);

		double *gsum = p [thy numberOfRows + 1]; // convenience array with sums

		autoCovariance covg = TableOfReal_to_Covariance (thee);

		double npars = GaussianMixture_getNumberOfParametersInComponent (me.get());
		double nparsd2 = ( deleteWeakComponents ? npars / 2.0 : 0.0 );

		// Initial E-step: Update all p's.

		GaussianMixture_and_TableOfReal_getProbabilities (me.get(), thee, 0, p.peek());

		double lnew = GaussianMixture_getLikelihoodValue (me.get(), p.peek(), thy numberOfRows, criterion);

		autoMelderProgress progress (U"Gaussian mixture...");
		autoGaussianMixture best;
		try {
			double lstart = lnew / thy numberOfRows;
			long iter = 0, component;
			double lmax = -1e308, lprev;
			while (my numberOfComponents >= minNumberOfComponents) {
				do {
					iter ++;
					component = 1;
					lprev = lnew;
					while (component <= my numberOfComponents) {
						// M-step for means and covariances
						GaussianMixture_updateProbabilityMarginals (me.get(), p.peek(), thy numberOfRows);
						GaussianMixture_updateCovariance (me.get(), component, thy data, thy numberOfRows, p.peek());
						if (lambda > 0) {
							GaussianMixture_addCovarianceFraction (me.get(), component, covg.get(), lambda);
						}

						// Now check if enough support for a component exists

						double support_im = gsum [component] - nparsd2, support = 0.0;
						for (long ic = 1; ic <= my numberOfComponents; ic ++) {
							double support_ic = gsum [ic] - nparsd2;
							if (support_ic > 0.0) {
								support += support_ic;
							}
						}
						my mixingProbabilities [component] = ( support_im > 0.0 ? support_im : 0.0 );
						if (support > 0.0) {
							my mixingProbabilities [component] /= support;
						}

						NUMdvector_scaleAsProbabilities (my mixingProbabilities, my numberOfComponents);

						if (my mixingProbabilities [component] > 0.0) { // update p for component
							GaussianMixture_and_TableOfReal_getProbabilities (me.get(), thee, component, p.peek());
							component ++;
						} else {
							// "Remove" the component column from p by shifting row values

							GaussianMixture_removeComponent_bookkeeping (me.get(), component, p.peek(), thy numberOfRows);

							// Now numberOfComponents is one less!
							// MelderInfo_writeLine (U"Removed component ", component);
						}
					}

					// L(theta,Y)=N/2 sum(m=1..k, log(n*mixingP[m]/12))+k/2log(n/12)+k/2(N+1)-loglikelihood reduces to:
					// k/2 (N+1){log(n/12)+1}+N/2sum(m=1..k,mixingP[m]) - loglikelihood

					lnew = GaussianMixture_getLikelihoodValue (me.get(), p.peek(), thy numberOfRows, criterion);

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
					for (long ic = 2; ic <= my numberOfComponents; ic ++) {
						if (my mixingProbabilities [ic] < mpmin) {
							mpmin = my mixingProbabilities [ic];
							component = ic;
						}
					}

					GaussianMixture_removeComponent_bookkeeping (me.get(), component, p.peek(), thy numberOfRows);
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

// The numberOfElemnts per covariance needs to be updated later
void GaussianMixture_removeComponent (GaussianMixture me, long component) {
	if (component < 1 || component > my numberOfComponents || my numberOfComponents == 1) {
		return;
	}

	my covariances -> removeItem (component);
	my numberOfComponents --;

	for (long ic = component; ic <= my numberOfComponents; ic ++) {
		my mixingProbabilities [ic] = my mixingProbabilities [ic + 1];
	}

	NUMdvector_scaleAsProbabilities (my mixingProbabilities, my numberOfComponents);
}

autoGaussianMixture TableOfReal_to_GaussianMixture (TableOfReal me, long numberOfComponents, double delta_lnp, long maxNumberOfIterations, double lambda, int storage, int criterion) {
	try {
		if (my numberOfRows < 2 * numberOfComponents) {
			Melder_throw (U"The number of data points must at least be twice the number of components.");
		}

		autoGaussianMixture thee = GaussianMixture_create (numberOfComponents, my numberOfColumns, storage);
		GaussianMixture_setLabelsFromTableOfReal (thee.get(), me);
		GaussianMixture_initialGuess (thee.get(), me, 1.0, 0.05);
		if (maxNumberOfIterations <= 0) {
			return thee;
		}

		GaussianMixture_and_TableOfReal_improveLikelihood (thee.get(), me, delta_lnp, maxNumberOfIterations, lambda, criterion);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no GaussianMixture created.");
	}
}

autoCorrelation GaussianMixture_and_TableOfReal_to_Correlation (GaussianMixture me, TableOfReal thee) {
	try {
		if (my dimension != thy numberOfColumns) {
			Melder_throw (U"Dimensions must be equal.");
		}
		autoClassificationTable ct = GaussianMixture_and_TableOfReal_to_ClassificationTable (me, thee);
		autoCorrelation him = ClassificationTable_to_Correlation_columns (ct.get());
		return him;
	} catch (MelderError) {
		Melder_throw (U"Correlation not created from GaussianMixture & TableOfReal.");
	}
}

double GaussianMixture_getProbabilityAtPosition_string (GaussianMixture me, const char32 *vector) {
	autoNUMvector<double> v (1, my dimension);

	long i = 0;
	for (char32 *token = Melder_firstToken (vector); token != nullptr; token = Melder_nextToken ()) {
		v [++ i] = Melder_atof (token);
		if (i == my dimension) {
			break;
		}
	}
	double p = GaussianMixture_getProbabilityAtPosition (me, v.peek());
	return p;
}

double GaussianMixture_getMarginalProbabilityAtPosition (GaussianMixture me, double *vector, double x) {
	double p = 0;
	for (long im = 1; im <= my numberOfComponents; im++) {
		double pim = Covariance_getMarginalProbabilityAtPosition (my covariances->at [im], vector, x);
		p += my mixingProbabilities [im] * pim;
	}
	return p;
}

double GaussianMixture_getProbabilityAtPosition (GaussianMixture me, double *xpos) {
	double p = 0.0;
	for (long im = 1; im <= my numberOfComponents; im ++) {
		double pim = Covariance_getProbabilityAtPosition (my covariances->at [im], xpos);
		p += my mixingProbabilities [im] * pim;
	}
	return p;
}

autoMatrix GaussianMixture_and_PCA_to_Matrix_density (GaussianMixture me, PCA thee, long d1, long d2, double xmin, double xmax, long nx, double ymin, double ymax, long ny) {
	try {
		if (my dimension != thy dimension) {
			Melder_throw (U"Dimensions must be equal.");
		}
		if (d1 > thy numberOfEigenvalues || d2 > thy numberOfEigenvalues) {
			Melder_throw (U"Direction index too high.");
		}
		autoNUMvector<double> v (1, my dimension);

		if (xmax == xmin || ymax == ymin) {
			double xmind, xmaxd, ymind, ymaxd, nsigmas = 2.0;

			GaussianMixture_and_PCA_getIntervalsAlongDirections (me, thee, d1, d2, nsigmas, &xmind, &xmaxd, &ymind, &ymaxd);
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

		for (long i = 1; i <= ny; i ++) {
			double y = y1 + (i - 1) * dy;
			for (long j = 1; j <= nx; j ++) {
				double x = x1 + (j - 1) * dx;
				for (long k = 1; k <= my dimension; k ++) {
					v [k] = x * thy eigenvectors [d1] [k] + y * thy eigenvectors [d2] [k];
				}
				his z [i] [j] = GaussianMixture_getProbabilityAtPosition (me, v.peek());
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no Matrix density created.");
	}
}

autoTableOfReal GaussianMixture_to_TableOfReal_randomSampling (GaussianMixture me, long numberOfPoints) {
	try {
		Covariance cov = my covariances->at [1];
		autoTableOfReal thee = TableOfReal_create (numberOfPoints, my dimension);
		autoNUMvector<double> buf (1, my dimension);
		NUMstrings_copyElements (cov -> columnLabels, thy columnLabels, 1, my dimension);
		for (long i = 1; i <= numberOfPoints; i ++) {
			char32 *covname;
			GaussianMixture_generateOneVector (me, thy data[i], &covname, buf.peek());
			TableOfReal_setRowLabel (thee.get(), i, covname);
		}
		GaussianMixture_unExpandPCA (me);
		return thee;
	} catch (MelderError) {
		GaussianMixture_unExpandPCA (me); Melder_throw (U"TableOfReal with random ssampling not created.");
	}
}

autoTableOfReal GaussianMixture_and_TableOfReal_to_TableOfReal_BHEPNormalityTests (GaussianMixture me, TableOfReal thee, double h) {
	try {
		long n = thy numberOfRows, d = thy numberOfColumns, nocp1 = my numberOfComponents + 1;

		if (d != my dimension) {
			Melder_throw (U"Dimensions do not agree.");
		}

		// We cannot use a classification table because this could weigh a far-off data point with high probability

		autoNUMmatrix<double> p (1, thy numberOfRows + 1, 1, my numberOfComponents + 1);

		GaussianMixture_and_TableOfReal_getProbabilities (me, thee, 0, p.peek());

		// prob, beta, tnbo, lnmu, lnvar, ndata, ncol
		autoTableOfReal him = TableOfReal_create (my numberOfComponents, 7);

		// labels

		long iprob = 1, ih = 2, itnb = 3, ilnmu = 4, ilnvar = 5, indata = 6, id = 7;
		const char32 *label[8] = { U"", U"p", U"h", U"tnb", U"lnmu", U"lnvar", U"ndata", U"d" };
		for (long icol = 1; icol <= 7; icol ++) {
			TableOfReal_setColumnLabel (him.get(), icol, label [icol]);
		}
		for (long irow = 1; irow <= my numberOfComponents; irow ++) {
			Covariance cov = my covariances->at [irow];
			TableOfReal_setRowLabel (him.get(), irow, Thing_getName (cov));
		}

		for (long icol = 1 ; icol <= my numberOfComponents; icol ++) {
			his data[icol][indata] = p [n + 1] [icol];
		}

		for (long im = 1; im <= my numberOfComponents; im ++) {
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
				SSCP_expandLowerCholesky (cov);
			} catch (MelderError) {
				tnb = 4.0 * nd;
			}

			double djk, djj, sumjk = 0.0, sumj = 0.0;
			double b1 = beta2 / 2.0, b2 = b1 / (1.0 + beta2);

			/* Heinze & Wagner (1997), page 3
				We use d[j][k] = ||Y[j]-Y[k]||^2 = (Y[j]-Y[k])'S^(-1)(Y[j]-Y[k])
				So d[j][k]= d[k][j] and d[j][j] = 0
			*/
			for (long j = 1; j <= n; j ++) {
				double wj = p [j] [nocp1] > 0.0 ? mixingP * p [j] [im] / p [j] [nocp1] : 0.0;
				for (long k = 1; k < j; k ++) {
					djk = NUMmahalanobisDistance_chi (cov -> lowerCholesky, thy data [j], thy data [k], d, d);
					double w = p [k] [nocp1] > 0.0 ? wj * mixingP * p [k] [im] / p [k] [nocp1] : 0.0;
					sumjk += 2.0 * w * exp (-b1 * djk); // factor 2 because d[j][k] == d[k][j]
				}
				sumjk += wj * wj; // for k == j. Is this ok now for probability weighing ????
				djj = NUMmahalanobisDistance_chi (cov -> lowerCholesky, thy data [j], cov -> centroid, d, d);
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
