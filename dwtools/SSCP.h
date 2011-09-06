#ifndef _SSCP_h_
#define _SSCP_h_
/* SSCP.h
 *
 * Copyright (C) 1993-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "TableOfReal_extensions.h"
#include "PCA.h"
#include "CCA.h"

#include "SSCP_def.h"
oo_CLASS_CREATE (SSCP, TableOfReal);

Thing_define (Covariance, SSCP) {
};

Thing_define (Correlation, SSCP) {
};

/*
	Ordered collection of SSCP's
	All SSCP's must have the same dimensions and labels.
*/
Thing_define (SSCPs, Ordered) {
};

void SSCP_init (I, long dimension, long storage);

SSCP SSCP_create (long dimension);

void SSCP_drawConcentrationEllipse (SSCP me, Graphics g, double scale, int confidence,
	long d1, long d2, double xmin, double xmax, double ymin, double ymax, int garnish);

void SSCP_setNumberOfObservations (I, double numberOfObservations);
void SSCP_setCentroid (I, long component, double value); // only SSCP & Covariance
void SSCP_setValue (I, long row, long col, double value); // only SSCP & Covariance

double SSCP_getNumberOfObservations (I);
double SSCP_getDegreesOfFreedom (I);
double SSCP_getTotalVariance (I);
double SSCP_getCumulativeContributionOfComponents (I, long from, long to);
double SSCP_getLnDeterminant (I);
double SSCP_getConcentrationEllipseArea(I, double scale, int confidence, long d1, long d2);
double SSCP_getFractionVariation (I, long from, long to);

SSCP TableOfReal_to_SSCP (I, long rowb, long rowe, long colb, long cole);

TableOfReal SSCP_and_TableOfReal_extractDistanceQuantileRange (SSCP me, thou,
	double qlow, double qhigh);
TableOfReal Covariance_and_TableOfReal_extractDistanceQuantileRange (Covariance me, thou,
	double qlow, double qhigh);
/*
	Select from a TableOfReal the rows whose Mahalanobis distance to the centroid
	(from the SSCP) is in the quantile [qlow, qhigh].
*/

TableOfReal Covariance_and_TableOfReal_mahalanobis (Covariance me, thou, bool useTableCentroid);
/*
	Calculate the Mahalanobis distance: sqrt ((x-m)'S**-1 (x-m))
	use the m-vector (centroid) from the covariance unless useTableColumnMeans is true.
*/


Covariance TableOfReal_to_Covariance (I);

Correlation TableOfReal_to_Correlation (I);
Correlation TableOfReal_to_Correlation_rank (I);

TableOfReal SSCP_to_TableOfReal (SSCP me);
TableOfReal SSCP_extractCentroid (I);

TableOfReal Covariance_to_TableOfReal_randomSampling (Covariance me, long numberOfData);
/* Generate a table with data based on the covariance matrix */

void Covariance_and_PCA_generateOneVector (Covariance me, PCA thee, double *vec, double *buf);
/*
	A convenience function to avoid the calculation of the PCA each time we want to generate a random vector
	The PCA must be the result of a previous SSCP_to_PCA call !
	The covariance matrix must not be singular or diagonal!
	Returns the random sampled vector in vec (which must be of size my numberOfColumns).
	Preconditions:
		1. Covariance may not be in diagonal representation (1 row)
		2. Dimensions of me and PCA agree
		3. vec is of length my numberOfColumns
		4. buf, a vector of length my numberOfColumns, is needed so the routine cannot fail
*/

SSCPs TableOfReal_to_SSCPs_byLabel (I);

PCA SSCP_to_PCA (I);

void SSCP_expandPCA (I);
void SSCP_unExpandPCA (I);

CCA SSCP_to_CCA (I, long ny);

Covariance Covariance_create (long dimension);
Covariance Covariance_createSimple (wchar_t *covars, wchar_t *centroid, long numberOfObservations);
Covariance Covariance_create_reduceStorage (long dimension, long storage);
/*
	storage 0 or >= dimension: complete matrix
	storage 1: only diagonal
	storage 2: diagonal + 1 off-diagonal [i,i+1]
	storage 3: diagonal + off-diagonal [i,i+1] + off-diagonal [i,i+2]
    ....
    storage dimension : complete matrix
    See also SSCP_expand () for usage.
*/

Correlation Correlation_create (long dimension);

TableOfReal Correlation_confidenceIntervals (Correlation me,
	double confidenceLevel, long numberOfTests, int method);
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
void Covariance_getMarginalDensityParameters (Covariance me, double *vector, double *mu, double *stdev);
double Covariance_getMarginalProbabilityAtPosition (Covariance me, double *vector, double x);

double Covariance_getProbabilityAtPosition_string (Covariance me, wchar_t *xpos);
double Covariance_getProbabilityAtPosition (Covariance me, double *x);
/* evaluate the pdf(x,mu,Sigma) at x */

Covariance SSCP_to_Covariance (SSCP me, long numberOfConstraints);

SSCP Covariance_to_SSCP (Covariance me);

void SSCP_testDiagonality_bartlett (SSCP me, long numberOfContraints,
	double *chisq, double *probability);
void Correlation_testDiagonality_bartlett (Correlation me,
	long numberOfContraints, double *chisq, double *probability);
/* Test whether matrices are diagonal matrices, Morrison, page 118 */

Correlation SSCP_to_Correlation (I);

Configuration SSCP_to_Configuration (I, long numberOfDimensions);

Configuration Covariance_to_Configuration (Covariance me, long numberOfDimensions);

Configuration Correlation_to_Configuration (Correlation me, long numberOfDimensions);

void Covariance_difference (Covariance me, Covariance thee, double *prob, double *chisq, long *ndf);

void Covariance_getSignificanceOfOneMean (Covariance me, long index, double mu,
	double *probability, double *t, double *ndf);

void Covariance_getSignificanceOfMeansDifference (Covariance me,
	long index1, long index2, double mu, int paired, int equalVariances,
	double *probability, double *t, double *ndf);

void Covariance_getSignificanceOfOneVariance (Covariance me, long index,
	double sigmasq, double *probability, double *chisq, long *ndf);

void Covariance_getSignificanceOfVariancesRatio (Covariance me,
	long index1, long index2, double ratio, double *probability,
	double *f, long *ndf);

double Covariances_getMultivariateCentroidDifference (Covariance me, Covariance thee, int equalCovariances, double *prob, double *fisher, double *df1, double *df2);
/* Are the centroids of me and thee different?
	Assumption: the two covariances are equal. (we pool me and thee, dimension p).
	Two sample test of Morrison (1990), page 141:
	T^2 = n1*n2/(n1+n2) (m1-m2)'S^-1(m1-m2) # weighted Mahalanobis distance
	f = T^2 * (n1+n2-p-1)/((n1+n2-2)*p)
	f has Fisher distribution with p and n1+n2-p-1 degrees of freedom.
*/

void Covariances_equality (Collection me, int method, double *prob, double *chisq, double *df);
/*
	Equality of covariance.
	method = 1 : Bartlett (Morrison, 1990)
	method = 2 : Wald (Schott, 2001)
*/

SSCPs SSCPs_create ();

SSCP SSCPs_to_SSCP_sum (SSCPs me);
/* Sum the sscp's and weigh each means with it's numberOfObservations. */

SSCP SSCPs_to_SSCP_pool (SSCPs me);

void SSCPs_getHomegeneityOfCovariances_box (SSCPs me, double *probability,
	double *chisq, long *ndf);

SSCP SSCP_toTwoDimensions (I, double *v1, double *v2);
SSCPs SSCPs_toTwoDimensions (SSCPs me, double *v1, double *v2);
SSCPs SSCPs_extractTwoDimensions (SSCPs me, long d1, long d2);

/* For inheritors */

void SSCPs_drawConcentrationEllipses (SSCPs me, Graphics g, double scale,
	int confidence, const wchar_t *label, long d1, long d2, double xmin, double xmax,
	double ymin, double ymax, int fontSize, int garnish);

void SSCPs_getEllipsesBoundingBoxCoordinates (SSCPs me, double scale, int confidence,
	double *xmin, double *xmax, double *ymin, double *ymax);

void SSCP_expand (I);
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

void SSCP_unExpand (I);
/* Use only if the memory is really needed! */

void SSCP_expandLowerCholesky (I); // create lower square root of covariance matrix
void SSCP_unExpandLowerCholesky (I);

#endif /* _SSCP_h_ */
