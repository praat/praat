#ifndef _Covariance_h_
#define _Covariance_h_
/* Covariance.h
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

#include "CCA.h"
#include "PCA.h"
#include "SSCP.h"
#include "TableOfReal_extensions.h"

Thing_define (Covariance, SSCP) {
};

Collection_define (CovarianceList, OrderedOf, Covariance) {
	SSCPList asSSCPList () {
		return reinterpret_cast<SSCPList> (this);
	}
};

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
autoTableOfReal Covariance_TableOfReal_scaledResiduals (Covariance me, TableOfReal thee, bool useTableCentroid);

autoCovariance TableOfReal_to_Covariance (TableOfReal me);

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

autoCovariance Covariance_create (integer dimension);

autoCovariance Covariance_createSimple (conststring32 covars, conststring32 centroid, integer numberOfObservations);

autoCovariance Covariance_create_reduceStorage (integer dimension, kSSCPstorage storage);
/*
	storage full: complete matrix
	storage diagonal: only diagonal
	
    See also SSCP_expand () for usage.
*/

/* Precondition ||vector|| = 1 */
void Covariance_getMarginalDensityParameters (Covariance me, constVECVU const& vector, double *out_mean, double *out_stdev);

double Covariance_getMarginalProbabilityAtPosition (Covariance me, constVECVU const& vector, double x);

double Covariance_getProbabilityAtPosition_string (Covariance me, conststring32 xpos);

double Covariance_getProbabilityAtPosition (Covariance me, constVEC x);
/* evaluate the pdf(x,mu,Sigma) at x */

autoCovariance SSCP_to_Covariance (SSCP me, integer numberOfConstraints);

autoSSCP Covariance_to_SSCP (Covariance me);

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

double Covariance_TableOfReal_normalityTest_BHEP (Covariance me, TableOfReal data, constVEC const& responsibilities, double *inout_beta, double *out_tnb, double *out_lnmu, double *out_lnvar, bool *out_covarianceIsSingular);
/*
	Multivariate normality test of nxp data matrix according to the method described in 
		Henze & Wagner (1997), A new approach to the BHEP tests for multivariate normality, 
		Journal of Multivariate Analysis 62, 1-23.
	The test statistic is returned in tnb, together with the lognormal mean 'lnmu' and the lognormal variance 'lnvar'.
*/

autoCovariance CovarianceList_to_Covariance_pool (CovarianceList me);
autoCovariance CovarianceList_to_Covariance_between (CovarianceList me);
autoCovariance CovarianceList_to_Covariance_within (CovarianceList me);

#endif // _Covariance_h_
