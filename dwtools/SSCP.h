#ifndef _SSCP_h_
#define _SSCP_h_
/* SSCP.h
 *
 * Copyright (C) 1993-2019 David Weenink
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

#include "TableOfReal_extensions.h"
#include "PCA.h"
#include "CCA.h"

#include "SSCP_def.h"

Thing_define (Covariance, SSCP) {
};

Thing_define (Correlation, SSCP) {
};

/*
	Ordered collection of SSCP's
	All SSCP's must have the same dimensions and labels.
*/
Collection_define (SSCPList, OrderedOf, SSCP) {
};

Collection_define (CovarianceList, OrderedOf, Covariance) {
	SSCPList asSSCPList () {
		return reinterpret_cast<SSCPList> (this);
	}
};

void SSCP_init (SSCP me, integer dimension, integer storage);

autoSSCP SSCP_create (integer dimension);

void SSCP_reset (SSCP me);

void SSCP_drawTwoDimensionalEllipse_inside (SSCP me, Graphics g, double scale, conststring32 label, double fontSize);

double SSCP_getEllipseScalefactor (SSCP me, double scale, bool confidence);

void SSCP_drawConcentrationEllipse (SSCP me, Graphics g, double scale, int confidence,
	integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, int garnish);

void SSCP_setNumberOfObservations (SSCP me, double numberOfObservations);

void SSCP_setCentroid (SSCP me, integer component, double value); // only SSCP & Covariance

void SSCP_setValue (SSCP me, integer row, integer col, double value); // only SSCP & Covariance

double SSCP_getNumberOfObservations (SSCP me);

double SSCP_getDegreesOfFreedom (SSCP me);

double SSCP_getTotalVariance (SSCP me);

double SSCP_getCumulativeContributionOfComponents (SSCP me, integer from, integer to);

double SSCP_getLnDeterminant (SSCP me);

double SSCP_getConcentrationEllipseArea(SSCP me, double scale, bool confidence, integer d1, integer d2);

double SSCP_getFractionVariation (SSCP me, integer from, integer to);

autoSSCP TableOfReal_to_SSCP (TableOfReal me, integer rowb, integer rowe, integer colb, integer cole);
autoSSCP TableOfReal_to_SSCP_rowWeights (TableOfReal me, integer rowb, integer rowe, integer colb, integer cole, integer weightColumnNumber);

autoTableOfReal SSCP_TableOfReal_extractDistanceQuantileRange (SSCP me, TableOfReal thee, double qlow, double qhigh);

autoTableOfReal Covariance_TableOfReal_extractDistanceQuantileRange (Covariance me, TableOfReal thee, double qlow, double qhigh);
/*
	Select from a TableOfReal the rows whose Mahalanobis distance to the centroid
	(from the SSCP) is in the quantile [qlow, qhigh].
*/

autoTableOfReal Covariance_TableOfReal_mahalanobis (Covariance me, TableOfReal thee, bool useTableCentroid);
/*
	Calculate the Mahalanobis distance: sqrt ((x-m)'S**-1 (x-m))
	use the m-vector (centroid) from the covariance unless useTableColumnMeans is true.
*/

autoCovariance TableOfReal_to_Covariance (TableOfReal me);

autoCorrelation TableOfReal_to_Correlation (TableOfReal me);

autoCorrelation TableOfReal_to_Correlation_rank (TableOfReal me);

autoTableOfReal SSCP_to_TableOfReal (SSCP me);

autoTableOfReal SSCP_extractCentroid (SSCP me);

autoTableOfReal Covariance_to_TableOfReal_randomSampling (Covariance me, integer numberOfData);
/* Generate a table with data based on the covariance matrix */

void Covariance_PCA_generateOneVector_inline (Covariance me, PCA thee, VECVU vec, VEC buf);
/*
	A convenience function to avoid the calculation of the PCA each time we want to generate a random vector
	The PCA must be the result of a previous SSCP_to_PCA call !
	The covariance matrix must not be singular or diagonal!
	Returns the random sampled vector in vec (which should be of size my numberOfColumns).
	Preconditions:
		1. Covariance may not be in diagonal representation (1 row)
		2. Dimensions of me and PCA agree
		3. vec is of length my numberOfColumns
		4. buf, a vector of length my numberOfColumns, is needed so the routine cannot fail
*/

autoSSCPList TableOfReal_to_SSCPList_byLabel (TableOfReal me);

autoPCA SSCP_to_PCA (SSCP me);

void SSCP_expandPCA (SSCP me);

void SSCP_unExpandPCA (SSCP me);

autoCCA SSCP_to_CCA (SSCP me, integer ny);

autoCovariance Covariance_create (integer dimension);

autoCovariance Covariance_createSimple (conststring32 covars, conststring32 centroid, integer numberOfObservations);

autoCovariance Covariance_create_reduceStorage (integer dimension, integer storage);
/*
	storage 0 or >= dimension: complete matrix
	storage 1: only diagonal
	storage 2: diagonal + 1 off-diagonal [i,i+1]
	storage 3: diagonal + off-diagonal [i,i+1] + off-diagonal [i,i+2]
    ....
    storage dimension : complete matrix
    See also SSCP_expand () for usage.
*/

autoCorrelation Correlation_create (integer dimension);

autoCorrelation Correlation_createSimple (conststring32 s_correlations, conststring32 s_centroid, integer numberOfObservations);

autoTableOfReal Correlation_confidenceIntervals (Correlation me, double confidenceLevel, integer numberOfTests, int method);
/*
	if (method == 1)
		Confidence intervals by Ruben's approximation
	if (method == 2)
		Obtain large-sample conservative multiple tests and intervals by the
		Bonferroni inequality and the Fisher z transformation.

	Put upper value of confidence intervals in upper matrix and lower
	values of confidence intervals in lower part of resulting table.
	Diagonal values are 1 (and represent both upper and lower c.i.).
*/

/* Precondition ||vector|| = 1 */
void Covariance_getMarginalDensityParameters (Covariance me, constVECVU const& vector, double *p_mu, double *p_stdev);

double Covariance_getMarginalProbabilityAtPosition (Covariance me, constVECVU const& vector, double x);

double Covariance_getProbabilityAtPosition_string (Covariance me, conststring32 xpos);

double Covariance_getProbabilityAtPosition (Covariance me, constVEC x);
/* evaluate the pdf(x,mu,Sigma) at x */

autoCovariance SSCP_to_Covariance (SSCP me, integer numberOfConstraints);

autoSSCP Covariance_to_SSCP (Covariance me);

void SSCP_getDiagonality_bartlett (SSCP me, integer numberOfContraints, double *out_chisq, double *out_prob, double *out_df);

void Correlation_testDiagonality_bartlett (Correlation me, integer numberOfContraints, double *out_chisq, double *out_prob, double *out_df);
/* Test if a Correlation matrix is diagonal, Morrison pp. 116-118 */

autoCorrelation SSCP_to_Correlation (SSCP me);

void Covariance_difference (Covariance me, Covariance thee, double *out_prob, double *out_chisq, double *out_df);

void Covariance_getSignificanceOfOneMean (Covariance me, integer index, double mu,	double *out_probability, double *out_t, double *out_df);

void Covariance_getSignificanceOfMeansDifference (Covariance me, integer index1, integer index2, double mu, int paired, int equalVariances,	double *out_probability, double *out_t, double *out_ndf);

void Covariance_getSignificanceOfOneVariance (Covariance me, integer index, double sigmasq, double *out_probability, double *out_chisq, double *out_ndf);

void Covariance_getSignificanceOfVariancesRatio (Covariance me, integer index1, integer index2, double ratio, double *out_probability, double *out_f, double *out_df);

double Covariances_getMultivariateCentroidDifference (Covariance me, Covariance thee, int equalCovariances, double *out_prob, double *out_fisher, double *out_df1, double *out_df2);
/* Are the centroids of me and thee different?
	Assumption: the two covariances are equal. (we pool me and thee, dimension p).
	Two sample test of Morrison (1990), page 141:
	T^2 = n1*n2/(n1+n2) (m1-m2)'S^-1(m1-m2) # weighted Mahalanobis distance
	f = T^2 * (n1+n2-p-1)/((n1+n2-2)*p)
	f has Fisher distribution with p and n1+n2-p-1 degrees of freedom.
*/

void Covariances_equality (CovarianceList me, int method, double *out_prob, double *out_chisq, double *out_df);
/*
	Equality of covariance.
	method = 1 : Bartlett (Morrison, 1990)
	method = 2 : Wald (Schott, 2001)
*/

autoCovariance CovarianceList_to_Covariance_pool (CovarianceList me);
autoCovariance CovarianceList_to_Covariance_between (CovarianceList me);
autoCovariance CovarianceList_to_Covariance_within (CovarianceList me);


autoSSCP SSCPList_to_SSCP_sum (SSCPList me);
/* Sum the sscp's and weigh each means with it's numberOfObservations. */

autoSSCP SSCPList_to_SSCP_pool (SSCPList me);

void SSCPList_getHomegeneityOfCovariances_box (SSCPList me, double *out_probability, double *out_chisq, double *out_df);

autoSSCP SSCP_toTwoDimensions (SSCP me, constVECVU const& v1, constVECVU const& v2);

autoSSCPList SSCPList_toTwoDimensions (SSCPList me, constVECVU const& v1, constVECVU const& v2);

autoSSCPList SSCPList_extractTwoDimensions (SSCPList me, integer d1, integer d2);

/* For inheritors */

void SSCPList_drawConcentrationEllipses (SSCPList me, Graphics g, double scale,
	bool confidence, conststring32 label, integer d1, integer d2, double xmin, double xmax,
	double ymin, double ymax, double fontSize, bool garnish);

void SSCPList_getEllipsesBoundingBoxCoordinates (SSCPList me, double scale, bool confidence,
	double *xmin, double *xmax, double *ymin, double *ymax);

void SSCP_expand (SSCP me);
/*
	Expand a reduced storage SSCP. For efficiency reasons, the expanded matrix is kept in memory.
	Successive calls to SSCP_expand don't change anything unless
	Before using one of the Covariance functions defined here on a reduced matrix we
	first have to expand it to normal size.

	Covariance me = Covariance_create_reduceStorage (dimension, 1); // diagonal only
	...
	SSCP_expand (me);
	PCA thee = SSCP_to_PCA (me);
*/

void SSCP_unExpand (SSCP me);
/* Use only if the memory is really needed! */

void SSCP_expandLowerCholeskyInverse (SSCP me); // create lower square root of covariance matrix

void SSCP_unExpandLowerCholesky (SSCP me);

/* End of file SSCP.h */
#endif
