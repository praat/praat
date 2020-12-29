/* Covariance.cpp
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

#include "Covariance.h"
#include "Eigen.h"
#include "NUMlapack.h"
#include "NUM2.h"
#include "SVD.h"

#define TOVEC(x) (&(x) - 1)

Thing_implement (Covariance, SSCP, 0);
Thing_implement (CovarianceList, SSCPList, 0);

autoTableOfReal Covariance_TableOfReal_mahalanobis (Covariance me, TableOfReal thee, bool useTableCentroid) {
	try {
		Melder_require (my numberOfColumns == thy numberOfColumns,
			U"The dimension of the Covariance and the TableOfReal shoiuld be equal.");
		autoTableOfReal him = TableOfReal_create (thy numberOfRows, 1);
		autoVEC centroid = copy_VEC (my centroid.get());
		autoMAT covari = copy_MAT (my data.get());
		/*
			Mahalanobis distance calculation. S = L.L' -> S**-1 = L**-1' . L**-1
				(x-m)'S**-1 (x-m) = (x-m)'L**-1' . L**-1. (x-m) =
				(L**-1.(x-m))' . (L**-1.(x-m))

			Get inverse of covari in lower triangular part.
		*/
		MATlowerCholeskyInverse_inplace (covari.get(), nullptr);

		if (useTableCentroid)
			columnMeans_VEC_out (centroid.get(), thy data.get());
		for (integer k = 1; k <= thy numberOfRows; k ++) {
			his data [k] [1] = sqrt (NUMmahalanobisDistanceSquared (covari.get(), thy data.row (k), centroid.get()));
			if (thy rowLabels [k])
				TableOfReal_setRowLabel (him.get(), k, thy rowLabels [k].get());
		}
		TableOfReal_setColumnLabel (him.get(), 1, U"d");
		return him;
	} catch (MelderError) {
		Melder_throw (me, U"no Mahalanobis distances created.");
	}
}

/* For nxn matrix only ! */
void Covariance_PCA_generateOneVector_inline (Covariance me, PCA thee, VECVU vec, VEC buf) {
	Melder_require (thy dimension == my numberOfRows,
		U"The PCA must have the same dimension as the Covariance.");
	Melder_require (vec.size == buf.size && my numberOfColumns == buf.size, 
		U"The vectors and the PCA must have the same dimension.");
	/*
		Generate the multi-normal vector elements N(0,sigma)
	*/
	for (integer j = 1; j <= my numberOfColumns; j ++)
		buf [j] = NUMrandomGauss (0.0, sqrt (thy eigenvalues [j]));
	/*
		Rotate back
	*/	
	mul_VEC_out (vec, buf, thy eigenvectors.get());
	vec  +=  my centroid.get();
}

autoTableOfReal Covariance_to_TableOfReal_randomSampling (Covariance me, integer numberOfData) {
	try {
		if (numberOfData <= 0)
			numberOfData = Melder_ifloor (my numberOfObservations);
		autoPCA pca = SSCP_to_PCA (me);
		autoTableOfReal thee = TableOfReal_create (numberOfData, my numberOfColumns);
		autoVEC buf = raw_VEC (my numberOfColumns);
		for (integer i = 1; i <= numberOfData; i ++)
			Covariance_PCA_generateOneVector_inline (me, pca.get(), thy data.row (i), buf.get());
		thy columnLabels.all() <<= my columnLabels.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not random sampled.");
	}
}

autoTableOfReal Covariance_TableOfReal_extractDistanceQuantileRange (Covariance me, TableOfReal thee, double qlow, double qhigh) {
	try {
		autoTableOfReal him = Covariance_TableOfReal_mahalanobis (me, thee, false);

		const double low = TableOfReal_getColumnQuantile (him.get(), 1, qlow);
		const double high = TableOfReal_getColumnQuantile (him.get(), 1, qhigh);
		/*
			Count the number filtered.
			nsel = (qhigh - qlow) * nrows is sometimes one off
		*/
		integer nsel = 0;
		for (integer i = 1; i <= thy numberOfRows; i ++)
			if (low <= his data [i] [1] && his data [i] [1] < high)
				nsel ++;
		Melder_require (nsel > 0,
			U"Not enough data in quantile interval.");
		
		autoTableOfReal r = TableOfReal_create (nsel, thy numberOfColumns);
		r -> columnLabels.all() <<= thy columnLabels.all();

		integer k = 0;
		for (integer i = 1; i <= thy numberOfRows; i ++)
			if (low <= his data [i] [1] && his data [i] [1] < high)
				TableOfReal_copyOneRowWithLabel (thee, r.get(), i, ++ k);
		return r;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal with distance quantile range not created.");
	}
}

autoCovariance TableOfReal_to_Covariance (TableOfReal me) {
	try {
		autoSSCP sscp = TableOfReal_to_SSCP (me, 0, 0, 0, 0);
		autoCovariance thee = SSCP_to_Covariance (sscp.get(), 1);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": covariances not created.");
	}
}

autoCovariance CovarianceList_to_Covariance_within (CovarianceList me) {
	try {
		autoCovariance thee = Data_copy (my at[1]);
		SSCP_reset (thee.get());
		for (integer i = 1; i <= my size; i ++) {
			const Covariance covi = my at [i];
			Melder_require (thy numberOfColumns == covi -> numberOfColumns && thy numberOfRows == covi -> numberOfRows,
				U"The dimensions of item ", i, U" does not conform.");
			thy data.all()  +=  covi -> data.all()  *  (covi -> numberOfObservations - 1.0);
			thy numberOfObservations += covi -> numberOfObservations;
		}
		thy data.all()  *=  1.0 / (thy numberOfObservations - 1.0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Covariance (within) created.");
	}
}

autoCovariance CovarianceList_to_Covariance_between (CovarianceList me) {
	try {
		autoCovariance thee = Data_copy (my at[1]);
		SSCP_reset (thee.get());

		//	First the new centroid,

		for (integer i = 1; i <= my size; i ++) {
			const Covariance covi = my at [i];
			Melder_require (thy numberOfColumns == covi -> numberOfColumns && thy numberOfRows == covi -> numberOfRows,
				U"The dimensions of item ", i, U" does not conform.");
			thy centroid.all()  +=  covi -> centroid.all()  *  covi -> numberOfObservations;
			thy numberOfObservations += covi -> numberOfObservations;
		}
		thy centroid.all()  *=  1.0 / thy numberOfObservations;
		
		autoVEC mean = raw_VEC (thy numberOfColumns);
		autoMAT outer = raw_MAT (thy numberOfColumns, thy numberOfColumns);
		for (integer i = 1; i <= my size; i ++) {
			const Covariance covi = my at [i];
			mean.all() <<= covi -> centroid.all()  -  thy centroid.all();
			outer_MAT_out (outer.all(), mean.all(), mean.all());
			if (thy numberOfRows == 1)
				thy data.row(1)  +=  outer.diagonal()  *  covi -> numberOfObservations;
			else
				thy data.all()  +=  outer.all()  *  covi -> numberOfObservations;   // Y += aX
		}
		thy data.all()  *=  1.0 / (thy numberOfObservations - 1.0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Covariance (between) created.");
	}
}

autoCovariance CovarianceList_to_Covariance_pool (CovarianceList me) { // Morrison sec 3.5, page 100
	try {
		autoCovariance thee = Data_copy (my at [1]);
		SSCP_reset (thee.get());
		for (integer i = 1; i <= my size; i ++) {
			const Covariance covi = my at [i];
			Melder_require (covi -> numberOfRows == thy numberOfRows,
				U"The dimension of item ", i, U" should agree.");
			thy numberOfObservations += covi -> numberOfObservations;
			/*
				Sum the sscp's and weigh the centroid.
			*/
			for (integer k = 1; k <= thy numberOfRows; k ++) // catch 1xn
				thy data.row(k)  +=  covi -> data.row(k)  *  (covi -> numberOfObservations - 1.0);

			thy centroid.all()  +=  covi -> centroid.all()  *  covi -> numberOfObservations;
		}
		thy centroid.all()  *=  1.0 / thy numberOfObservations;
		thy data.all()  *=  1.0 / (thy numberOfObservations - my size);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not pooled.");
	}
}

autoCovariance Covariance_create (integer dimension) {
	try {
		autoCovariance me = Thing_new (Covariance);
		SSCP_init (me.get(), dimension, kSSCPstorage::COMPLETE);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Covariance not created.");
	}
}

autoCovariance Covariance_create_reduceStorage (integer dimension, kSSCPstorage storage) {
	try {
		autoCovariance me = Thing_new (Covariance);
		SSCP_init (me.get(), dimension, storage);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Reduced storage covariance not created.");
	}
}



autoCovariance Covariance_createSimple (conststring32 s_covariances, conststring32 s_centroid, integer numberOfObservations) {
	try {
		autoVEC centroid = newVECfromString (s_centroid);
		autoVEC covariances = newVECfromString (s_covariances);
		integer numberOfCovariances_wanted = centroid.size * (centroid.size + 1) / 2;
		Melder_require (covariances.size == numberOfCovariances_wanted,
			U"The number of covariance matrix elements and the number of centroid elements (d) should conform. "
			"There should be d(d+1)/2 covariance values and d centroid values.");
		
		autoCovariance me = Covariance_create (centroid.size);
		/*
			Construct the full covariance matrix from the upper-diagonal elements
		*/
		integer rowNumber = 1;
		for (integer inum = 1; inum <= covariances.size; inum ++) {
			const integer nmissing = (rowNumber - 1) * rowNumber / 2;
			const integer inumc = inum + nmissing;
			rowNumber = (inumc - 1) / centroid.size + 1;
			integer icol = ((inumc - 1) % centroid.size) + 1;
			my data [rowNumber] [icol] = my data [icol] [rowNumber] = covariances [inum];
			if (icol == centroid.size)
				rowNumber ++;
		}

		// Check if a valid covariance, first check variances then covariances

		for (integer irow = 1; irow <= centroid.size; irow ++)
			Melder_require (my data [irow] [irow] > 0.0,
				U"The diagonal matrix elements should all be positive numbers.");

		for (integer irow = 1; irow <= centroid.size; irow ++)
			for (integer icol = irow + 1; icol <= centroid.size; icol ++)
				Melder_require (fabs (my data [irow] [icol] / sqrt (my data [irow] [irow] * my data [icol] [icol])) <= 1.0,
					U"The covariance in cell [", irow, U",", icol, U"], i.e. input item ",
				(irow - 1) * centroid.size + icol - (irow - 1) * irow / 2, U" is too large.");
		my centroid.all() <<= centroid.all();
		my numberOfObservations = numberOfObservations;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Simple Covariance not created.");
	}
}

autoCovariance SSCP_to_Covariance (SSCP me, integer numberOfConstraints) {
	try {
		Melder_assert (numberOfConstraints >= 0);
		autoCovariance thee = Thing_new (Covariance);
		my structSSCP :: v_copy (thee.get());

		for (integer irow = 1; irow <= my numberOfRows; irow ++)
			for (integer icol = irow; icol <= my numberOfColumns; icol ++)   // a covariance matrix is symmetric
				thy data [icol] [irow] = thy data [irow] [icol] /= my numberOfObservations - numberOfConstraints;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"; Covariance not created.");
	}
}

static autoCovariance Covariances_pool (Covariance me, Covariance thee) {
	try {
		Melder_require (my numberOfRows == thy numberOfRows && my numberOfColumns == thy numberOfColumns,
			U"Matrices should have equal dimensions.");
		autoSSCPList sscps = SSCPList_create ();
		autoSSCP sscp1 = Covariance_to_SSCP (me);
		sscps -> addItem_move (sscp1.move());
		autoSSCP sscp2 = Covariance_to_SSCP (thee);
		sscps -> addItem_move (sscp2.move());
		autoSSCP pool = SSCPList_to_SSCP_pool (sscps.get());
		autoCovariance him = SSCP_to_Covariance (pool.get(), 2);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U"not pooled.");
	}
}

static double traceOfSquaredMatrixProduct (constMAT const& s1, constMAT const& s2) {
	// tr ((s1*s2)^2), s1, s2 are symmetric
	autoMAT m = mul_MAT (s1, s2);
	double trace2 = NUMtrace2 (m.get(), m.get());
	return trace2;
}

double Covariance_getProbabilityAtPosition_string (Covariance me, conststring32 vector_string) {
	autoSTRVEC vector = splitByWhitespace_STRVEC (vector_string);
	autoVEC v = zero_VEC (my numberOfColumns);
	for (integer i = 1; i <= vector.size; i ++) {
		v [i] = Melder_atof (vector [i].get());
		if (i == my numberOfColumns)
			break;
	}
	const double p = Covariance_getProbabilityAtPosition (me, v.get());
	return p;
}

double Covariance_getProbabilityAtPosition (Covariance me, constVEC x) {
	Melder_require (x.size == my numberOfColumns,
		U"The dimensions of the Covariance and the vector should agree.");
	if (NUMisEmpty (my lowerCholeskyInverse.get()))
		SSCP_expandLowerCholeskyInverse (me);
	const double ln2pid = my numberOfColumns * log (NUM2pi);
	const double dsq = NUMmahalanobisDistanceSquared (my lowerCholeskyInverse.get(), x, my centroid.get());
	const double lnN = - 0.5 * (ln2pid + my lnd + dsq);
	const double p = exp (lnN);
	return p;
}

double Covariance_getMarginalProbabilityAtPosition (Covariance me, constVECVU const& vector, double x) {
	double mu, stdev;
	Covariance_getMarginalDensityParameters (me, vector, & mu, & stdev);
	const double dx = (x - mu) / stdev;
	const double p = (NUM1_sqrt2pi / stdev) * exp (- 0.5 * dx * dx);
	return p;
}

/* Precondition ||v|| = 1 */
void Covariance_getMarginalDensityParameters (Covariance me, constVECVU const& v, double *out_mean, double *out_stdev) {
	Melder_assert (v.size == my numberOfColumns);
	if (out_mean)
		*out_mean = NUMinner (v, my centroid.get());
	if (out_stdev) {
		longdouble stdev = 0.0;
		if (my numberOfRows == 1) // 1xn diagonal matrix
			for (integer m = 1; m <= my numberOfColumns; m ++)
				stdev += v [m] * my data [1] [m] * v [m];
		else 
			for (integer k = 1; k <= my numberOfRows; k ++)
				for (integer m = 1; m <= my numberOfColumns; m ++)
					stdev += v [k] * my data [k] [m] * v [m];
		*out_stdev = sqrt (double (stdev));
	}
}

double Covariances_getMultivariateCentroidDifference (Covariance me, Covariance thee, int equalCovariances, double *out_prob, double *out_fisher, double *out_df1, double *out_df2) {
	const integer p = my numberOfRows, N = Melder_ifloor (my numberOfObservations + thy numberOfObservations);
	const integer N1 = Melder_ifloor (my numberOfObservations), n1 = N1 - 1;
	const integer N2 = Melder_ifloor (thy numberOfObservations), n2 = N2 - 1;
	const double df1 = p;
	double df2 = N - p - 1;
	
	Melder_require (df2 >= 1.0, 
		U"Not enough observations (", N, U") for this test.");
	Melder_require (p <= N1 && p <= N2,
		U"The number of observations should be larger than the number of variables.");
	double dif = 0.0;
	for (integer i = 1; i <= p; i ++) {
		const double dist = my centroid [i] - thy centroid [i];
		dif += dist * dist;
	}
	dif = sqrt (dif);
	double fisher = undefined;
	if (equalCovariances) {
		/*
			Morrison, page 141
		*/
		autoCovariance pool = Covariances_pool (me, thee);
		Melder_assert (my data.ncol == p);   // ppgb 20180913
		autoMAT s = copy_MAT (my data.get());
		double lndet;
		MATlowerCholeskyInverse_inplace (s.get(), & lndet);

		const double mahalanobis = NUMmahalanobisDistanceSquared (s.get(), my centroid.get(), thy centroid.get());
		const double hotelling_tsq = mahalanobis * N1 * N2 / N;
		fisher = hotelling_tsq * df2 / ( (N - 2) * df1);
	} else {
		/*
			Krishnamoorthy-Yu (2004): Modified Nel and Van der Merwe test

			Hotelling t^2 = (x1-x2)'*S^-1*(x1 -x2) follows nu*p*Fisher(p,nu-p+1)/(nu-p+1)

			Approximate number of degrees of freedom  (their formula 7, page 164)
			nu = (p+p^2)/((1/n1)(tr (S1*S^-1)^2 + (tr(S1*S^-1))^2)) +(1/n2)(tr (S2*S^-1)^2 + (tr(S2*S^-1))^2)))
			the matrices S1 and S2 are the covariance matrices 'my data' and 'thy data' divided by N1 and N2 respectively.
			S is the pooled covar divided by N.
		*/
		autoMAT s1 = raw_MAT (p, p), s2 = raw_MAT (p, p), s = raw_MAT (p, p);
		for (integer i = 1; i <= p; i ++) {
			for (integer j = 1; j <= p; j ++) {
				s1 [i] [j] = my data [i] [j] / my numberOfObservations;
				s2 [i] [j] = thy data [i] [j] / thy numberOfObservations;
				s [i] [j] = s1 [i] [j] + s2 [i] [j];
			}
		}
		double lndet;
		MATlowerCholeskyInverse_inplace (s.get(), & lndet);
		/*
			Krishan... formula 2, page 162
		*/
		const double hotelling_tsq = NUMmahalanobisDistanceSquared (s.get(), my centroid.get(), thy centroid.get());

		autoMAT si = newMATinverse_fromLowerCholeskyInverse (s.get());
		const double tr_s1sisqr = traceOfSquaredMatrixProduct (s1.get(), si.get());
		const double tr_s1si = NUMtrace2 (s1.get(), si.get());
		const double tr_s2sisqr = traceOfSquaredMatrixProduct (s2.get(), si.get());
		const double tr_s2si = NUMtrace2 (s2.get(), si.get());

		const double nu = (p + p * p) / ( (tr_s1sisqr + tr_s1si * tr_s1si) / n1 + (tr_s2sisqr + tr_s2si * tr_s2si) / n2);
		df2 = nu - p + 1;
		fisher =  hotelling_tsq * (nu - p + 1) / (nu * p);
	}

	if (out_prob)
		*out_prob = NUMfisherQ (fisher, df1, df2);
	if (out_fisher)
		*out_fisher = fisher;
	if (out_df1)
		*out_df1 = df1;
	if (out_df2)
		*out_df2 = df2;
	return dif;
}

/* Schott 2001 */
void Covariances_equality (CovarianceList me, int method, double *out_prob, double *out_chisq, double *out_df) {
	try {

		const integer numberOfMatrices = my size;
		Melder_require (numberOfMatrices > 1,
			U"We need at least two matrices");

		autoCovariance pool = CovarianceList_to_Covariance_pool (me); 
		const double ns = pool -> numberOfObservations - my size;
		const integer p = pool -> numberOfColumns;
		
		double chisq = undefined, df = undefined;
		if (method == 1) {
			/*
				Bartlett (see Morrison page 297)
				The hypothesis H0 : Sigma [1] = .... = Sigma [k] of the equality of the covariance matrices of k p-dimensional
				multinormal populations can be tested against the alternative by a modified generalized likelihood-ratio statistic.
				Let S [i] be the unbiased estimate of Sigma [i] based on n [i] degrees of freedom, where n [i] = N [i]-1 for 
				the usual case of a random sample of N [i] observation vectors from the i-th population. When H0 is true
					S = 1/(sum(i=1..k, n [i])) sum(i=1..k, n [i]*S [i])
				is the pooled estimate of the common covariance matrix. The test statistic is
					M = sum(i=1..k,n [i])*ln|S| - sum(i=1..k, n [i]*ln|S [i]|).
				Box (1949), "A general distribution theory for a class of likelihood criteria", 
				Biomerika, vol 36, pp. 317-346. has shown that if the scale factor
				C^(-1) = 1 - (2p^2+3p-1)/(6(p+1)(k-1)) * (sum(i=1..k, 1/n [i]) - 1 / sum(i=1..k, n [i])) is introduced,
				the quatity M/C is approximately distributed as a chi-squared variate with (k-1)p(p+1)/2 degrees of freedom 
				as the n [i] become large.
				It is well known that this likelihood ratio test is very sensitive to violations of the normality assumption, 
				and so other more robust procedures have been proposed.
			 */
			double lnd;
			try {
				lnd = NUMdeterminant_fromSymmetricMatrix (pool -> data.get());
			} catch (MelderError) {
				Melder_throw (U"Pooled covariance matrix is singular.");
			}

			double nsi = 0.0, m = ns * lnd; // First part of eq (3) page 297

			for (integer i = 1; i <= numberOfMatrices; i ++) {
				const Covariance ci = my at [i];
				try {
					lnd = NUMdeterminant_fromSymmetricMatrix (ci -> data.get());
				} catch (MelderError) {
					Melder_throw (U"Covariance matrix ", i, U" is singular.");
				}
				nsi += 1.0 / (ci -> numberOfObservations - 1);
				m -= (ci -> numberOfObservations - 1) * lnd;  // Last part of eq (3) page 297
			}
			/*
				Eq (4) page 297
			*/
			const double c1 = 1.0 - (2.0 * p * p + 3.0 * p - 1.0) / (6.0 * (p + 1) * (numberOfMatrices - 1)) * (nsi - 1.0 / ns);

			df = (numberOfMatrices - 1.0) * p * (p + 1) / 2.0;
			chisq = m * c1;
		} else if (method == 2) { // Schott (2001) Wald 1
			/*
				T1 = sum(i=1..k, n [i]/n *tr((S [i]*S^-1)^2)- sum(i=1..k, sum(j=1..k, (n [i]/n)*(n [j]/n) *tr(S [i]*S^-1*S [j]*sS^-1))) =
				sum(i=1..k, (ni/n -(ni/n)^2) tr((si*s^-1)^2)
				- 2 * sum (i=1..k, sum(j=1..i-1, (ni/n)*(nj/n) *tr(si*s^-1*sj*s^-1)))
			*/
			double trace = 0.0;
			MATlowerCholeskyInverse_inplace (pool -> data.get(), nullptr);
			autoMAT si = newMATinverse_fromLowerCholeskyInverse (pool -> data.get());
			for (integer i = 1; i <= numberOfMatrices; i ++) {
				const Covariance ci = my at [i];
				const double ni = ci -> numberOfObservations - 1;
				autoMAT s1 = mul_MAT (ci -> data.get(), si.get());
				const double trace_ii = NUMtrace2 (s1.get(), s1.get());
				trace += (ni / ns) * (1 - (ni / ns)) * trace_ii;
				for (integer j = i + 1; j <= numberOfMatrices; j ++) {
					const Covariance cj = my at [j];
					const double nj = cj -> numberOfObservations - 1;
					autoMAT s2 = mul_MAT (cj -> data.get(), si.get());
					const double trace_ij = NUMtrace2 (s1.get(), s2.get());
					trace -= 2.0 * (ni / ns) * (nj / ns) * trace_ij;
				}
			}
			df = (numberOfMatrices - 1) * p * (p + 1) / 2.0;
			chisq = (ns / 2.0) * trace;
		} else {
			return;
		}
		if (out_prob)
			*out_prob = NUMchiSquareQ (chisq, df);
		if (out_df)
			*out_df = df;
		if (out_chisq)
			*out_chisq = chisq;
	} catch (MelderError) {
		Melder_throw (U"Equality coud not be tested.");
	}
}

double Covariance_TableOfReal_normalityTest_BHEP (Covariance me, TableOfReal thee, constVEC const& responsibilities, double *inout_beta, double *out_tnb, double *out_lnmu, double *out_lnvar, bool *out_covarianceIsSingular) {
	try {
		const MAT data = thy data.get();
		Melder_require (data.nrow > data.ncol,
			U"The number of data should be larger than the dimension of the data.");
		Melder_require (my numberOfColumns == data.ncol,
			U"The number of columns of the covariance and the data should be equal.");

		const bool weighting = responsibilities.size > 0;
		bool covarianceIsSingular = false;
		const double d = data.ncol;
		double n = data.nrow, testStatistic;
		if (weighting) {
			Melder_require (responsibilities.size == data.nrow,
				U"The size of the responsibilities vector should equal the number of data.");
			n = NUMsum (responsibilities);
		}

		*inout_beta = ( *inout_beta > 0.0 ? NUMsqrt1_2 / *inout_beta : NUMsqrt1_2 * pow ( (1.0 + 2.0 * d) / 4.0, 1.0 / (d + 4.0)) * pow (n, 1.0 / (d + 4.0)) );
		const double d2 = d / 2.0;
		const double beta2 = *inout_beta * *inout_beta, beta4 = beta2 * beta2, beta8 = beta4 * beta4;
		const double gamma = 1.0 + 2.0 * beta2, gamma2 = gamma * gamma, gamma4 = gamma2 * gamma2; // page 15
		const double delta = 1.0 + beta2 * (4.0 + 3.0 * beta2), delta2 = delta * delta; // page 15
		
		autoVEC buf = raw_VEC (data.nrow);

		try { // in case the covariance matrix is singular and its inverse cannot be determined
			SSCP_expandLowerCholeskyInverse (me);
			const MAT lowerInverse = my lowerCholeskyInverse.get();
			/*
				Heinze & Wagner (1997), on page 3 in the formula for W(n,beta) the residuals 
				y(i)=S^(-1/2)*(x(i)-mean) are used.
				However, we can work with the original data and use squared Mahalanobis distances in this calculation because
				||y(j)-y(k)||^2 = (x(j)-x(k))'S^-1((x(j)-x(k)).
				This expression is symmetric in j and k, so we can do with half the amount of work
			*/
			double doubleSum = 0.0;
			for (integer j = 1; j <= data.nrow - 1; j ++) {
				for (integer k = j + 1; k <= data.nrow; k ++) {
					const double djk_sq = NUMmahalanobisDistanceSquared (lowerInverse, data.row (j), data.row (k));
					buf [k] = exp (-0.5 * beta2 * djk_sq);
				}
				if (weighting)
					doubleSum += 2.0 * responsibilities [j] * NUMinner (buf.part (j + 1, data.nrow), responsibilities.part (j + 1, data.nrow));
				else
					doubleSum += 2.0 * NUMsum (buf.part (j + 1, data.nrow));
			}
			doubleSum += n; // contribution of all the terms in the double sum where j == k
			
			double singleSum = 0.0;	
			for (integer j = 1; j <= data.nrow; j ++) {
				const double djj_sq = NUMmahalanobisDistanceSquared (lowerInverse, data.row(j), my centroid.get());
				buf [j] = exp (-0.5 * beta2 * djj_sq / (1.0 + beta2));
			}
			if (weighting)
				singleSum += NUMinner (buf.get(), responsibilities);
			else
				singleSum += NUMsum (buf.get());
			/*
				The test statistic is n times the W(n,beta) of page 3.
			*/
			testStatistic = (1.0 / n) * doubleSum - 2.0 * pow (1.0 + beta2, - d2) * singleSum + n * pow (gamma, - d2);
		} catch (MelderError) {
			Melder_clearError ();
			testStatistic = 4.0 * n;
			covarianceIsSingular = true;
		}

		const double mu = 1.0 - pow (gamma, -d2) * (1.0 + d * beta2 / gamma + d * (d + 2.0) * beta4 / (2.0 * gamma2));
		const double var = 2.0 * pow (1.0 + 4.0 * beta2, -d2)
			+ 2.0 * pow (gamma,  -d) * (1.0 + 2.0 * d * beta4 / gamma2  + 3.0 * d * (d + 2) * beta8 / (4.0 * gamma4))
			- 4.0 * pow (delta, -d2) * (1.0 + 3.0 * d * beta4 / (2.0 * delta) + d * (d + 2) * beta8 / (2.0 * delta2));
		const double mu2 = mu * mu;
		const double lnmu = log (sqrt (mu2 * mu2 / (mu2 + var)));
		const double lnvar = sqrt (log ((mu2 + var) / mu2));
		if (out_lnmu)
			*out_lnmu = lnmu;
		if (out_lnvar)
			*out_lnvar = lnvar;
		if (out_tnb)
			*out_tnb = testStatistic;
		if (out_covarianceIsSingular)
			*out_covarianceIsSingular = covarianceIsSingular;
		double prob = NUMlogNormalQ (testStatistic, lnmu, lnvar);
		return prob;
	} catch (MelderError) {
		Melder_throw (me, U": Could not determine BHEP statistic.");
	}
}

void Covariance_difference (Covariance me, Covariance thee, double *out_prob, double *out_chisq, double *out_df) {
	const integer p = my numberOfRows;
	integer numberOfObservations = Melder_ifloor (my numberOfObservations);
	double chisq = undefined, df = undefined;
	Melder_require (my numberOfRows == thy numberOfRows,
		U"Matrices should have equal dimensions.");
	if (my numberOfObservations != thy numberOfObservations) {
		numberOfObservations = Melder_ifloor (my numberOfObservations > thy numberOfObservations ?
			thy numberOfObservations : my numberOfObservations) - 1;
		Melder_warning (U"Covariance_difference: number of observations of matrices do not agree.\n"
			U" The minimum  size (", numberOfObservations, U") of the two is used.");
	}
	Melder_require (numberOfObservations > 1,
		U"Number of observations too small.");
	Melder_assert (thy data.ncol == p);
	autoMAT linv = copy_MAT (thy data.get());
	double ln_thee;
	MATlowerCholeskyInverse_inplace (linv.get(), & ln_thee);
	const double ln_me = NUMdeterminant_fromSymmetricMatrix (my data.get());
	/*
		We need trace (A B^-1). We have A and the inverse L^(-1) of the
		cholesky decomposition L^T L of B in the lower triangle + diagonal.
		Always: tr (A B) = tr (B A)
		tr (A B^-1) = tr (A (L L^T)^-1) = tr (A L^-1 (L^T)^-1)
		trace = sum(i=1..p, j=1..p, l=max(i,j)..p, A [i] [j]Lm [l] [j]Lm [l] [i],
		where Lm = L^(-1)
	*/
	double trace = 0.0;
	for (integer i = 1; i <= p; i ++) {
		for (integer j = 1; j <= p; j ++) {
			const integer lp = std::max (j, i);
			for (integer l = lp; l <= p; l ++)
				trace += my data [i] [j] * linv [l] [j] * linv [l] [i];
		}
	}

	const double l = (numberOfObservations - 1) * fabs (ln_thee - ln_me + trace - p);
	chisq = l * fabs (1.0 - (2.0 * p + 1.0 - 2.0 / (p + 1)) / (numberOfObservations - 1) / 6.0);
	df = p * (p + 1) / 2.0;
	
	if (out_prob)
		*out_prob = NUMchiSquareQ (chisq, df);
	if (out_chisq)
		*out_chisq = chisq;
	if (out_df)
		*out_df = df;
}

static void checkOneIndex (TableOfReal me, integer index) {
	Melder_require (index > 0 && index <= my numberOfColumns,
		U"Index should be in interval [1, ", my numberOfColumns, U"].");
}

static void checkTwoIndices (TableOfReal me, integer index1, integer index2) {
	Melder_require (index1 > 0 && index1 <= my numberOfColumns && index2 > 0 && index2 <= my numberOfColumns,
		U"Index should be in interval [1, ", my numberOfColumns, U"].");
	Melder_require (index1 != index2, 
		U"Indices should be different.");
}

void Covariance_getSignificanceOfOneMean (Covariance me, integer index, double mu, double *out_prob, double *out_t, double *out_df) {
	const double var = my data [index] [index];
	double prob = undefined, t = undefined;
	const double df = my numberOfObservations - 1.0;

	checkOneIndex (me, index);

	if (var > 0.0) {
		t = (my centroid [index] - mu) / sqrt (var / my numberOfObservations);
		if (out_prob) prob = 2.0 * NUMstudentQ (fabs (t), df);
	}	
	if (out_prob)
		*out_prob = prob;
	if (out_t)
		*out_t = t;
	if (out_df)
		*out_df = df;
}

void Covariance_getSignificanceOfMeansDifference (Covariance me, integer index1, integer index2, double mu, int paired, int equalVariances, double *out_prob, double *out_t, double *out_df) {
	integer n = Melder_ifloor (my numberOfObservations);

	double prob = undefined, t = undefined;
	double df = 2.0 * (n - 1);

	checkTwoIndices (me, index1, index2);

	const double var1 = my data [index1] [index1];
	const double var2 = my data [index2] [index2];

	double var_pooled = var1 + var2;
	if (var_pooled == 0.0) {
		Melder_warning (U"The pooled variance turned out to be zero. Check your data.");
		goto end;
	}
	if (paired) {
		var_pooled -= 2.0 * my data [index1] [index2];
		df /= 2.0;
	}

	if (var_pooled == 0.0) {
		Melder_warning (U"The pooled variance with the paired correction turned out to be zero. ");
		prob = 0.0;
		goto end;
	}

	t = (my centroid [index1] - my centroid [index2] - mu) / sqrt (var_pooled / n);

	/*
		Return two sided probabilty.
	*/

	if (equalVariances) {
		prob = 2.0 * NUMstudentQ (fabs (t), df);
	} else {
		df = (1.0 + 2.0 * var1 * var2 / (var1 * var1 + var2 * var2)) * (n - 1);
		prob = NUMincompleteBeta (df / 2.0, 0.5, df / (df + t * t));
	}
end:
	if (out_prob)
		*out_prob = prob;
	if (out_t)
		*out_t = t;
	if (out_df)
		*out_df = df;
}

void Covariance_getSignificanceOfOneVariance (Covariance me, integer index, double sigmasq, double *out_prob, double *out_chisq, double *out_df) {
	double var = my data [index] [index];
	double prob = undefined, chisq = undefined;
	double df = my numberOfObservations - 1.0;

	checkOneIndex (me, index);

	if (var > 0.0) {
		chisq = df;
		if (sigmasq > 0.0)
			chisq = df * var / sigmasq;
		if (out_prob)
			prob = NUMchiSquareQ (chisq, df);
	}
	if (out_prob)
		*out_prob = prob;
	if (out_chisq)
		*out_chisq = chisq;
	if (out_df)
		*out_df = df;
}

void Covariance_getSignificanceOfVariancesRatio (Covariance me, integer index1, integer index2, double ratio, double *out_prob, double *out_f, double *out_df) {
	const double df = my numberOfObservations - 1.0;
	double prob = undefined, f = undefined;
	checkTwoIndices (me, index1, index2);

	const double var1 = my data [index1] [index1];
	const double var2 = my data [index2] [index2];

	if (var1 > 0.0 && var2 > 0.0) {
		double ratio2 = (var1 / var2) / ratio;
		f = ratio2;
		if (var2 > var1)
			ratio2 = (var2 / var1) * ratio;
		if (out_prob) {
			prob = 2.0 * NUMfisherQ (ratio2, df, df);
			if (prob > 1.0)
				prob = 2.0 - prob;
		}
	}
	if (out_prob)
		*out_prob = prob;
	if (out_df)
		*out_df = df;
	if (out_f)
		*out_f = f;
}

/* End of file Covariance.cpp */
