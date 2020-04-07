#ifndef _GaussianMixture_h_
#define _GaussianMixture_h_
/* GaussianMixture.h
 *
 * Copyright (C) 2010-2020 David Weenink
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
 djmw 20101021 initial version
 djmw 20110306 Latest modification.
*/

#include "ClassificationTable.h"
#include "Matrix.h"
#include "Covariance.h"
#include "TableOfReal_extensions.h"

#include "GaussianMixture_def.h"

#include "GaussianMixture_enums.h"

/*
	Invariants for a Gaussian mixture:
	 all covariances have the same 'dimension' parameter
	 All mixingProbabilities are >= 0 and sum to 1.0
*/
autoGaussianMixture GaussianMixture_create (integer numberOfComponents, integer dimension, kGaussianMixtureStorage storage);
/* Start each function with expand and end with unExpand */

void GaussianMixture_removeUnsupportedComponents (GaussianMixture me);

void GaussianMixture_expandPCA (GaussianMixture me);

void GaussianMixture_unExpandPCA (GaussianMixture me);

void GaussianMixture_drawConcentrationEllipses (GaussianMixture me, Graphics g,
	double scale, bool confidence, char32 *label, bool pcaDirections, integer d1, integer d2,
	double xmin, double xmax, double ymin, double ymax, double fontSize, bool garnish);

void GaussianMixture_PCA_drawConcentrationEllipses (GaussianMixture me, PCA him, Graphics g,
	double scale, bool confidence, char32 *label, integer d1, integer d2,
	double xmin, double xmax, double ymin, double ymax, double fontSize, bool garnish);

void GaussianMixture_drawMarginalPdf (GaussianMixture me, Graphics g, integer d, double xmin, 
	double xmax, double ymin, double ymax, integer npoints, integer nbins, bool garnish);

void GaussianMixture_PCA_drawMarginalPdf (GaussianMixture me, PCA him, Graphics g, integer d, 
	double xmin, double xmax, double ymin, double ymax, integer npoints, integer nbins, bool garnish);

autoGaussianMixture TableOfReal_to_GaussianMixture_fromRowLabels (TableOfReal me, kGaussianMixtureStorage storage);

/*
	Give an initial guess for the centroids and covariances of the GaussianMixture based on the data in the table.
	Position centroids on the nSigma-ellips in the pc1-pc2 plane with some random variation and the covariances as
	a scaled down version of the total covariance.
	The randomly varied position of a centroid on the ellipse is parametrized as:
		x = a * (1 + randomUniform (-ru_range, ru_range)) * cos (alpha)
		y = b * (1 + randomUniform (-ru_range, ru_range)) * sin (alpha).
	where a and b are the axes of the ellipse and 0<= alpha <= 2pi.
*/
void GaussianMixture_initialGuess2 (GaussianMixture me, TableOfReal thee, double nSigmas, double ru_range);
void GaussianMixture_initialGuess (GaussianMixture me, TableOfReal thee);

/*
	Get the component probabilities N(x|mu(k),Sigma(k)).
	Bishop (2007): Pattern recognition and machine learning, Springer page 25: Eq. 1.52
*/
void GaussianMixture_TableOfReal_getComponentProbabilities (GaussianMixture me, TableOfReal thee, integer componentToUpdate, MAT const& probabilities);
autoTableOfReal GaussianMixture_TableOfReal_to_TableOfReal_probabilities (GaussianMixture me, TableOfReal thee);

/*
	Calculate the responsibities: mixingProbability(k) * N(x(n)|mu(k),Sigma(k)) / Normalization.
	Bishop (2007): Pattern recognition and machine learning, Springer page 438: Eq. 9.23
	Invariant:
	The row sum always equals 1.
*/
void GaussianMixture_TableOfReal_getComponentResponsibilities (GaussianMixture me, TableOfReal thee, integer componentToUpdate, MAT const& responsibilities);
autoTableOfReal GaussianMixture_TableOfReal_to_TableOfReal_responsibilities (GaussianMixture me, TableOfReal thee);

conststring32 GaussianMixture_criterionText (kGaussianMixtureCriterion criterion);

autoGaussianMixture TableOfReal_to_GaussianMixture (TableOfReal me, integer numberOfComponents, double delta_lnp, integer maxNumberOfIterations, double lambda, kGaussianMixtureStorage storage, kGaussianMixtureCriterion criterion);

void GaussianMixture_TableOfReal_improveLikelihood (GaussianMixture me, TableOfReal thee, double delta_lnp, integer maxNumberOfIterations, double lambda, kGaussianMixtureCriterion criterion);

/*
	Learn a GaussiamMixture from multivariate data (unsupervised).
	1) it is capable of selecting the number of components and 
	2) unlike the standard expectation-maximization (EM) algorithm, it does not require careful initialization. 
	3) It avoids the possibility of convergence toward a singular estimate at the boundary of the parameter space.
	The Component-wise Expectation Maximization for Mixures algorithm (CEMM) is described at page 387 of:
	M.A.T. Figueiredo & A.K. Jain (2002): "Unsupervised learning of finite mixture models",
	IEEETransactions on pattern analysis and machine intelligence 24: 381--396.
*/
autoGaussianMixture TableOfReal_to_GaussianMixture_CEMM (TableOfReal me, integer minimumNumberOfComponents, integer maximumNumberOfComponents, kGaussianMixtureStorage storage, integer maximumNumberOfIterations, double tolerance, bool info);

autoGaussianMixture GaussianMixture_TableOfReal_to_GaussianMixture_CEMM (GaussianMixture me, TableOfReal thee, integer minimumNumberOfComponents, integer maxNumberOfIterations, double tolerance, bool info);

void GaussianMixture_splitComponent (GaussianMixture me, integer component);

void GaussianMixture_removeComponent (GaussianMixture me, integer component);

autoClassificationTable GaussianMixture_TableOfReal_to_ClassificationTable (GaussianMixture me, TableOfReal thee);

autoTable GaussianMixture_TableOfReal_to_Table_BHEPNormalityTests (GaussianMixture me, TableOfReal thee, double h);

double GaussianMixture_TableOfReal_getLikelihoodValue (GaussianMixture me, TableOfReal thee, kGaussianMixtureCriterion criterion);

double GaussianMixture_getProbabilityAtPosition (GaussianMixture me, constVEC const& v);

double GaussianMixture_getProbabilityAtPosition_string (GaussianMixture me, conststring32 pos);

double GaussianMixture_getMarginalProbabilityAtPosition (GaussianMixture me, constVECVU const& pos, double x);

autoCorrelation GaussianMixture_TableOfReal_to_Correlation (GaussianMixture me, TableOfReal thee);
/* Correlation between components based on the data in the table */

autoCovariance GaussianMixture_to_Covariance_total (GaussianMixture me);

autoCovariance GaussianMixture_to_Covariance_between (GaussianMixture me);

autoCovariance GaussianMixture_to_Covariance_within (GaussianMixture me);


autoCovariance GaussianMixture_extractComponent(GaussianMixture me, integer component);

autoTableOfReal GaussianMixture_extractCentroids (GaussianMixture me);

autoTableOfReal GaussianMixture_extractMixingProbabilities (GaussianMixture me);

autoPCA GaussianMixture_to_PCA (GaussianMixture me);

autoMatrix GaussianMixture_PCA_to_Matrix_density (GaussianMixture me, PCA pca, integer d1, integer d2, double xmin, double xmax, integer nx, double ymin, double ymax, integer ny);

void GaussianMixture_PCA_getIntervalsAlongDirections (GaussianMixture me, PCA thee, integer d1, integer d2, double nsigmas, double *out_xmin, double *out_xmax, double *out_ymin, double *out_ymax);

void GaussianMixture_PCA_getIntervalAlongDirection (GaussianMixture me, PCA thee, integer d, double nsigmas, double *out_xmin, double *out_xmax);

void GaussianMixture_getIntervalAlongDirection (GaussianMixture me, integer d, double nsigmas, double *out_xmin, double *out_xmax);

void GaussianMixture_getIntervalsAlongDirections (GaussianMixture me, integer d1, integer d2, double nsigmas, double *out_xmin, double *out_xmax, double *out_ymin, double *out_ymax);

/* with on demand expand of pca ! */
void GaussianMixture_generateOneVector_inline (GaussianMixture me, VEC const& c, autostring32 *out_covname, VEC const& buf);

autoTableOfReal GaussianMixture_to_TableOfReal_randomSampling (GaussianMixture me, integer numberOfPoints);

#endif /* _GaussianMixture_h_ */
