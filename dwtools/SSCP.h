#ifndef _SSCP_h_
#define _SSCP_h_
/* SSCP.h
 * 
 * Copyright (C) 1993-2007 David Weenink
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

/*
 djmw 20020327 GPL
 djmw 20070620 Latest modification.
*/

#ifndef _TableOfReal_extensions_h_
	#include "TableOfReal_extensions.h"
#endif
#ifndef _PCA_h_
	#include "PCA.h"
#endif
#ifndef _CCA_h_
	#include "CCA.h"
#endif

#define SSCP_members TableOfReal_members \
	double numberOfObservations;	\
	double *centroid;
#define SSCP_methods TableOfReal_methods
class_create (SSCP, TableOfReal);

#define Covariance_members SSCP_members
#define Covariance_methods SSCP_methods
class_create (Covariance, SSCP);

#define Correlation_members SSCP_members
#define Correlation_methods SSCP_methods
class_create (Correlation, SSCP);

/*
	Ordered collection of SSCP's
	All SSCP's must have the same dimensions and labels.
*/
#define SSCPs_members Ordered_members
#define SSCPs_methods Ordered_methods
class_create (SSCPs, Ordered);

int SSCP_init (I, long dimension);

SSCP SSCP_create (long dimension);

void SSCP_drawConcentrationEllipse (SSCP me, Graphics g, double scale, int confidence,
	long d1, long d2, double xmin, double xmax, double ymin, double ymax, int garnish);

void SSCP_setNumberOfObservations (I, double numberOfObservations);
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

Covariance TableOfReal_to_Covariance (I);

Correlation TableOfReal_to_Correlation (I);
Correlation TableOfReal_to_Correlation_rank (I);

TableOfReal SSCP_to_TableOfReal (SSCP me);
TableOfReal SSCP_extractCentroid (I);

TableOfReal Covariance_to_TableOfReal_randomSampling (Covariance me, long numberOfData);
/* Generate a table with data based on the covariance matrix */

SSCPs TableOfReal_to_SSCPs_byLabel (I);

PCA SSCP_to_PCA (I);

CCA SSCP_to_CCA (I, long ny);

Covariance Covariance_create (long dimension);

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

int Covariance_difference (Covariance me, Covariance thee, double *prob, double *chisq, long *ndf);

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

SSCPs SSCPs_create (void);

SSCP SSCPs_to_SSCP_sum (SSCPs me);
/* Sum the sscp's and weigh each means with it's numberOfObservations. */

SSCP SSCPs_to_SSCP_pool (SSCPs me);

int SSCPs_getHomegeneityOfCovariances_box (SSCPs me, double *probability, 
	double *chisq, long *ndf);
	
SSCPs SSCPs_toTwoDimensions (SSCPs me, double *v1, double *v2);

/* For inheritors */

void SSCPs_drawConcentrationEllipses (SSCPs me, Graphics g, double scale,
	int confidence, wchar_t *label, long d1, long d2, double xmin, double xmax,
	double ymin, double ymax, int fontSize, int garnish);

#endif /* _SSCP_h_ */

/* End of file SSCP.h */
