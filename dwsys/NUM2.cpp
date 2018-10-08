/* NUM2.cpp
 *
 * Copyright (C) 1993-2018 David Weenink, Paul Boersma 2017
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20020819 GPL header
 djmw 20020819 Split nonGLP part off.
 djmw 20001109 Changed stop criteria in NUMsvdcmp and NUMtqli.
 djmw 20020819 Split into GPL and nonGPL part.
 djmw 20021008 Removed SVD_sort.
 djmw 20030619 Removed calls to NRC svd-routines.
 djmw 20030623 Removed tqli en tred calls.
 djmw 20030703 Replaced NUMincompleteBeta with gsl_sf_beta_inc.
 djmw 20030710 NUMminimize_brent now also returns the minimum function value.
 djmw 20030731 NUMridders: better approximation for small d.
			   NUMinvFisherQ better approximation for p < 0.5
 djmw 20030813 Added NUMmad and NUMstatistics_huber.
 djmw 20030825 Replaced gsl_sf_beta_inc with NUMincompleteBeta
 pb   20030828 Improvements for invFisherQ, ridders, studentP, studentQ,
 	invStudentQ, invChiSquareQ: modifications for 'undefined' return values.
 djmw 20030830 Corrected a bug in NUMtriangularfilter_amplitude
 djmw 20031111 Added NUMdmatrix_transpose, NUMdmatrix_printMatlabForm
 djmw 20040105 Added NUMmahalanobisDistance_chi
 djmw 20040303 Added NUMstring_containsPrintableCharacter.
 djmw 20050406 NUMprocrutus->NUMprocrustes
 djmw 20060319 NUMinverse_cholesky: calculation of determinant is made optional
 djmw 20060517 Added NUMregexp_compile
 djmw 20060518 Treat NULL string as empty string in strs_replace_regexp/literal. Don't accept empty search in replace_regexStr
 djmw 20060626 Extra NULL argument for ExecRE.
 djmw 20070302 NUMclipLineWithinRectangle
 djmw 20070614 updated to version 1.30 of regular expressions.
 djmw 20071022 Removed function NUMfvector_moment2.
 djmw 20071201 Melder_warning<n>
 djmw 20080107 Changed assertion to "npoints > 0" in NUMcosinesTable
 djmw 20080110 Corrected some bugs in replace_regexStr
 djmw 20080122 Bug in replace_regexStr
 djmw 20080317 +NUMsinc
 pb   20080410 FisherQ from gsl
 djmw 20090630 NUMlogNormalP/Q from gsl
 djmw 20090707 Rename NUMinverse_cholesky to NUMlowerCholeskyInverse,
 	+NUMcovarianceFromColumnCentredMatrix, +NUMmultivariateKurtosis
 djmw 20100311 +NUMsolveQuadraticEquation
 djmw 20100426 replace wcstok by Melder_wcstok
 djmw 20101209 removed NUMwcscmp is Melder_wcscmp now
 djmw 20110304 Thing_new
 djmw 20111110 use autostringvector
*/

#include "SVD.h"
#include "Eigen.h"
#include "NUMclapack.h"
#include "NUM2.h"
#include "NUMmachar.h"
#include "melder.h"

#include "gsl_randist.h"

#include "gsl_errno.h"
#include "gsl_sf_bessel.h"
#include "gsl_sf_gamma.h"
#include "gsl_sf_erf.h"
#include "gsl_sf_trig.h"
#include "gsl_poly.h"
#include "gsl_cdf.h"

#define SIGN(a,b) ((b < 0) ? -fabs(a) : fabs(a))

struct pdf1_struct {
	double p;
	double df;
};
struct pdf2_struct {
	double p;
	double df1;
	double df2;
};

void MATprintMatlabForm (constMAT m, conststring32 name) {
	integer npc = 5;
	ldiv_t n = ldiv (m.ncol, npc);

	MelderInfo_open ();
	MelderInfo_write (name, U"= [");
	for (integer i = 1; i <= m.nrow; i ++) {
		for (integer j = 1; j <= n.quot; j ++) {
			for (integer k = 1; k <= npc; k ++) {
				MelderInfo_write (m [i] [(j - 1) * npc + k], (k < npc ? U", " : U""));
			}
			MelderInfo_write (j < n.quot ? U",\n" : U"");
		}

		for (integer k = 1; k <= n.rem; k ++) {
			MelderInfo_write (m [i] [n.quot * npc + k], (k < n.rem ? U", " : U""));
		}
		MelderInfo_write (i < m.nrow ? U";\n" : U"];\n");
	}
	MelderInfo_close ();
}

void MATnormalizeColumns_inplace (MAT a, double power, double norm) {
	Melder_assert (norm > 0.0);
	autoVEC column = VECraw (a.nrow);
	for (integer icol = 1; icol <= a.ncol; icol ++) {
		for (integer irow = 1; irow <= column.size; irow ++)
			column [irow] = a [irow] [icol];
		VECnormalize_inplace (column.get(), power, norm);
		for (integer irow = 1; irow <= column.size; irow ++)
			a [irow] [icol] = column [irow];
	}
}

void VECsmoothByMovingAverage_preallocated (VEC out, constVEC in, integer window) {
	Melder_assert (out.size == in.size);
	for (integer i = 1; i <= out.size; i ++) {
		integer jfrom = i - window / 2, jto = i + window / 2;
		if (window % 2 == 0) {
			jto --;
		}
		jfrom = jfrom < 1 ? 1 : jfrom;
		jto = jto > out.size ? out.size : jto;
		out [i] = 0.0;
		for (integer j = jfrom; j <= jto; j ++) {
			out [i] += in [j];
		}
		out [i] /= jto - jfrom + 1;
	}
}

autoMAT MATcovarianceFromColumnCentredMatrix (constMAT x, integer ndf) {
	Melder_require (ndf >= 0 && x.nrow - ndf > 0, U"Invalid arguments.");
	autoMAT covar = MATmtm (x);
	MATmultiply_inplace (covar.get(), 1.0 / (x.nrow - ndf));
	return covar;
}

static void MATweighRows (MAT x, constVEC y) {
	Melder_assert (x.nrow == y.size);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		VECmultiply_inplace (x.row (irow), y [irow]);
}

void MATmtm_weighRows_preallocated (MAT result, constMAT data, constVEC rowWeights) {
	Melder_assert (data.nrow == rowWeights.size);
	Melder_assert (data.ncol = result.ncol);
	Melder_assert (result.nrow == result.ncol);
	MATset (result, 0.0);
	if (true) {
		autoVEC row = VECraw (data.ncol);
		autoMAT outer = MATraw (result.nrow, result.ncol);
		for (integer irow = 1; irow <= data.nrow; irow ++) {
			VECcopy_preallocated (row.get(), data.row (irow));
			MATouter_preallocated (outer, row.get());
			VECsaxpy (asvector (result), asvector (outer.get()), rowWeights [irow]);
		}
	} else {
		autoVEC w = VECraw (rowWeights.size);
		autoMAT d = MATcopy (data);
		for (integer irow = 1; irow <= w.size; irow ++) 
			w [irow] = sqrt (rowWeights [irow]);
		MATweighRows (d.get(), w.get());
		MATmtm_preallocated (result, d.get());
	}
}

inline void MATmul_rows_inplace (MAT x, constVEC v) { // TODO better name??
	Melder_assert (x.nrow == v.size);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		VECmultiply_inplace (x.row (irow), v [irow]);
}

double NUMmultivariateKurtosis (constMAT m, int method) {
	double kurt = undefined;
	if (m.nrow < 5) {
		return kurt;
	}
	autoMAT x = MATcopy (m);
	autoVEC mean = VECraw (x.ncol);
	VECcolumnMeans_preallocated (mean.get(), x.get());
	MATsubtract_inplace (x.get(), mean.get());
	autoMAT covar = MATcovarianceFromColumnCentredMatrix (x.get(), 1);
	
	if (method == 1) { // Schott (2001, page 33)
		kurt = 0.0;
		for (integer l = 1; l <= x.ncol; l ++) {
			double zl = 0.0, wl, sll2 = covar [l] [l] * covar [l] [l];
			for (integer j = 1; j <= x.nrow; j ++) {
				double d = x [j] [l] - mean [l], d2 = d * d;
				zl += d2 * d2;
			}
			zl = (zl - 6.0 * sll2) / (x.nrow - 4);
			wl = (sll2 - zl / x.nrow) * x.nrow / (x.nrow - 1);
			kurt += zl / wl;
		}
		kurt = kurt / (3 * x.ncol) - 1.0;
	}
	return kurt;
}

/*
	The following algorithm for monotone regession is on the average
	3.5 times faster than
	Kruskal's algorithm for monotone regression (and much simpler).
	Regression is ascending
*/
autoVEC VECmonotoneRegression (constVEC x) {
	autoVEC fit = VECcopy (x);
	double xt = undefined;   // only to stop gcc from complaining "may be used uninitialized"
	for (integer i = 2; i <= x.size; i ++) {
		if (fit [i] >= fit [i - 1])
			continue;
		longdouble sum = fit [i];
		integer nt = 1;
		for (integer j = 1; j <= i - 1; j ++) {
			sum += fit [i - j];
			nt ++;
			xt = (double) sum / nt;   // i >= 2 -> xt always gets a value
			if (j < i - 1 && xt >= fit [i - j - 1])
				break;
		}
		for (integer j = i - nt + 1; j <= i; j ++)
			fit [j] = xt;
	}
	return fit;
}

#undef TINY

double MATdeterminant_fromSymmetricMatrix (constMAT m) {
	Melder_assert (m.nrow == m.ncol);
	autoMAT a = MATcopy (m);
	
	//	 Cholesky decomposition in lower, leave upper intact

	char uplo = 'U';
	integer lda = m.nrow, info;
	NUMlapack_dpotf2 (& uplo, & a.nrow, & a [1] [1], & lda, & info);
	Melder_require (info == 0, U"dpotf2 cannot determine Cholesky decomposition.");
	longdouble lnd = 0.0;
	for (integer i = 1; i <= a.nrow; i ++) {
		lnd += log (a [i] [i]);
	}
	lnd *= 2.0; // because A = L . L' TODO
	return (double) lnd;
}

void MATlowerCholeskyInverse_inplace (MAT a, double *out_lnd) {
	Melder_assert (a.nrow == a.ncol);
	char uplo = 'U', diag = 'N';
	integer info;

	// Cholesky decomposition in lower, leave upper intact
	// Fortran storage -> use uplo='U' to get 'L'.

	(void) NUMlapack_dpotf2 (& uplo, & a.nrow, & a [1] [1], & a.nrow, & info);
	Melder_require (info == 0, U"dpotf2 fails.");

	// Determinant from diagonal, diagonal is now sqrt (a [i] [i]) !

	if (out_lnd) {
		longdouble lnd = 0.0;
		for (integer i = 1; i <= a.nrow; i ++) {
			lnd += log (a [i] [i]);
		}
		*out_lnd *= 2.0 * lnd; /* because A = L . L' */
	}

	// Get the inverse */

	(void) NUMlapack_dtrtri (& uplo, & diag, & a.nrow, & a [1] [1], & a.nrow, & info);
	Melder_require (info == 0, U"dtrtri fails.");
}

autoMAT MATinverse_fromLowerCholeskyInverse (constMAT m) {
	Melder_assert (m.nrow == m.ncol);
	autoMAT r = MATraw (m.nrow, m.nrow);
	for (integer i = 1; i <= m.nrow; i ++) {
		for (integer j = 1; j <= i; j ++) {
			longdouble sum = 0.0;
			for (integer k = i; k <= m.nrow; k ++) {
				sum += m [k] [i] * m [k] [j];
			}
			r [i] [j] = r [j] [i] = (double) sum;
		}
	}
	return r;
}

double NUMmahalanobisDistance (constMAT lowerInverse, constVEC v, constVEC m) {
	Melder_assert (lowerInverse.ncol == v.size && v.size == m.size);
	longdouble chisq = 0.0;
	if (lowerInverse.nrow == 1) { // 1xn matrix
		for (integer j = 1; j <= v.size; j ++) {
			double t = lowerInverse [1] [j] * (v [j] - m [j]);
			chisq += t * t;
		}
	} else { // nxn matrix
		for (integer i = v.size; i > 0; i --) {
			double t = 0.0;
			for (integer j = 1; j <= i; j ++) {
				t += lowerInverse [i] [j] * (v [j] - m [j]);
			}
			chisq += t * t;
		}
	}
	return (double) chisq;
}

void NUMdominantEigenvector (constMAT m, VEC inout_q, double *out_lambda, double tolerance) {
	Melder_assert (m.nrow == m.ncol && inout_q.size == m.nrow);

	double lambda0, lambda = NUMvtmv (inout_q, m); //  q'. M . q
	Melder_require (lambda > 0.0, U"Zero matrices ??");
	autoVEC z = VECraw (m.nrow);
	integer iter = 0;
	do {
		lambda0 = lambda;
		VECmul_preallocated (z.get(), m, inout_q);
		VECnormalize_inplace (z.get(), 2.0, 1.0);
		lambda = NUMvtmv (z.get(), m); // z'. M . z

	} while (fabs (lambda - lambda0) > tolerance || ++ iter < 30);
	VECcopy_preallocated (inout_q, z.get());
	if (out_lambda) {
		*out_lambda = (double) lambda;
	}
}

/* Input:
		data [numberOfRows, from_col - 1 + my dimension] 
		contains the 'numberOfRows' vectors to be projected on the eigenspace. 
		eigenvectors [numberOfEigenvectors] [dimension] the eigenvectors stored as rows
	Input/Output
		projection [numberOfRows, to_colbegin - 1 + numberOfEigenvectors] 
		the projected vectors from 'data'

	Project (part of) the vectors in matrix 'data' along the 'numberOfEigenvectors' eigenvectors into the matrix 'projection'.
	*/
void MATprojectRowsOnEigenspace_preallocated (MAT projection, integer toColumn, constMAT data, integer fromColumn, constMAT eigenvectors) {
	Melder_assert (projection.nrow = data.nrow);
	fromColumn = fromColumn <= 0 ? 1 : fromColumn;
	toColumn = toColumn <= 0 ? 1 : toColumn;
	Melder_assert (fromColumn + eigenvectors.ncol - 1 <= data.ncol);
	Melder_assert (toColumn + eigenvectors.ncol - 1 <= projection.ncol);
	for (integer irow = 1; irow <= data.nrow; irow ++)
		for (integer icol = 1; icol <= eigenvectors.nrow; icol ++) {
			longdouble r = 0.0;
			for (integer k = 1; k <= eigenvectors.ncol; k ++) {
				r += eigenvectors [icol] [k] * data [irow] [fromColumn + k - 1];
			}
			projection [irow] [toColumn + icol - 1] = (double) r;
		}
}

void MATprojectColumnsOnEigenspace_preallocated (MAT projection, constMAT data, constMAT eigenvectors) {
	Melder_assert (data.nrow == eigenvectors.ncol && projection.nrow == eigenvectors.nrow);
	for (integer icol = 1; icol <= data.ncol; icol ++)
		for (integer irow = 1; irow <= eigenvectors.nrow; irow ++) {
			longdouble r = 0.0;
			for (integer k = 1; k <= eigenvectors.ncol; k ++) {
				r += eigenvectors [irow] [k] * data [k] [icol];
			}
			projection [irow] [icol] = (double) r;
		}
	// MATmul_tt (data.get(), eigenvectors.get()) ??
}


void NUMdmatrix_into_principalComponents (double **m, integer nrows, integer ncols, integer numberOfComponents, double **pc) {
	Melder_assert (numberOfComponents > 0 && numberOfComponents <= ncols);
	autoMAT mc = MATcopy (MAT (m, nrows, ncols));

	/*MATcentreEachColumn_inplace (mc.get());*/
	autoSVD svd = SVD_createFromGeneralMatrix (mc.get());
	for (integer i = 1; i <= nrows; i ++) {
		for (integer j = 1; j <= numberOfComponents; j ++) {
			longdouble sum = 0.0;
			for (integer k = 1; k <= ncols; k ++) {
				sum += svd -> v [k] [j] * m [i] [k];
			}
			pc [i] [j] = (double) sum;
		}
	}
	// MATmul_tt (svd->v.get(), m.get()); ??
}

integer NUMsolveQuadraticEquation (double a, double b, double c, double *x1, double *x2) {
	return gsl_poly_solve_quadratic (a, b, c, x1, x2);
}

autoVEC NUMsolveEquation (constMAT a, constVEC b, double tolerance) {
	Melder_assert (a.nrow == b.size);
	autoSVD me = SVD_createFromGeneralMatrix (a);
	SVD_zeroSmallSingularValues (me.get(), tolerance);
	autoVEC x = SVD_solve (me.get(), b);
	return x;
}

/*void NUMsolveEquation (double **a, integer nr, integer nc, double *b, double tolerance, double *result) {

	double tol = tolerance > 0 ? tolerance : NUMfpp -> eps * nr;

	Melder_require (nr > 0 && nc > 0, U"The number of rows and the number of columns should at least be 1.");

	autoSVD me = SVD_createFromGeneralMatrix ({a, nr, nc});
	SVD_zeroSmallSingularValues (me.get(), tol);
	SVD_solve (me.get(), b, result);
}*/

autoMAT NUMsolveEquations (constMAT a, constMAT b, double tolerance) {
	Melder_assert (a.nrow == b.nrow);
	double tol = tolerance > 0 ? tolerance : NUMfpp -> eps * a.nrow;
	
	autoSVD me = SVD_createFromGeneralMatrix (a);
	autoMAT x = MATraw (b.nrow, b.ncol);
	autoVEC bt = VECraw (b.nrow);

	SVD_zeroSmallSingularValues (me.get(), tol);

	for (integer k = 1; k <= b.ncol; k ++) {
		for (integer j = 1; j <= b.nrow; j ++) { // copy b[.][k]
			bt [j] = b [j] [k];
		}

		autoVEC xt = SVD_solve (me.get(), bt.get());

		for (integer j = 1; j <= b.nrow; j ++) {
			x [j] [k] = xt [j];
		}
	}
	return x;
}


autoVEC NUMsolveNonNegativeLeastSquaresRegression (constMAT m, constVEC d, double tol, integer itermax) {
	Melder_assert (m.nrow == d.size);
	long nr = m.nrow, nc = m.ncol;
	autoVEC b = VECzero (nc);
	for (integer iter = 1; iter <= itermax; iter ++) {

		// Fix all weights except b [j]

		for (integer j = 1; j <= nc; j ++) {
			longdouble mjr = 0.0, mjmj = 0.0;
			for (integer i = 1; i <= nr; i ++) {
				double ri = d [i], mij = m [i] [j];
				for (integer l = 1; l <= nc; l ++) {
					if (l != j) {
						ri -= b [l] * m [i] [l];
					}
				}
				mjr += mij * ri;
				mjmj += mij * mij;
			}
			b [j] = mjr / mjmj;
			if (b [j] < 0.0) {
				b [j] = 0.0;
			}
		}

		// Calculate t(b) and compare with previous result.

		longdouble difsq = 0.0, difsqp = 0.0;
		for (integer i = 1; i <= nr; i ++) {
			double dmb = d [i];
			for (integer j = 1; j <= nc; j ++) {
				dmb -= m [i] [j] * b [j];
			}
			difsq += dmb * dmb;
		}
		if (fabs (difsq - difsqp) / difsq < tol) {
			break;
		}
		difsqp = difsq;
	}
	return b;
}

struct nr_struct {
	double *y, *delta;
};

/*
	f (lambda) = sum (y [i]^2 delta [i] / (delta [i]-lambda)^2, i=1..3)
	f'(lambda) = 2 * sum (y [i]^2 delta [i] / (delta [i]-lambda)^3, i=1..3)
*/

static void nr_func (double x, double *f, double *df, void *data) {
	struct nr_struct *me = (struct nr_struct *) data;
	*f = *df = 0.0;
	for (integer i = 1; i <= 3; i ++) {
		double t1 = (my delta [i] - x);
		double t2 = my y [i] / t1;
		double t3 = t2 * t2 * my delta [i];
		*f  += t3;
		*df += t3 * 2.0 / t1;
	}
}

void NUMsolveConstrainedLSQuadraticRegression (constMAT o, constVEC d, double *out_alpha, double *out_gamma) {
	Melder_assert (o.ncol == o.nrow && d.size == o.ncol && d.size == 3);
	integer n3 = 3, info;
	double eps = 1e-5, t1, t2, t3;

	autoMAT g = MATzero (n3, n3);
	autoMAT ptfinv = MATzero (n3, n3);

	// Construct O'.O	[1..3] [1..3].

	autoMAT ftinv = MATmtm (o);

	// Get lower triangular decomposition from O'.O and
	// get F'^-1 from it (eq. (2)) (F^-1 not done ????)

	char uplo = 'U';
	(void) NUMlapack_dpotf2 (& uplo, & n3, & ftinv [1] [1], & n3, & info);
	Melder_require (info == 0, U"dpotf2 fails.");
	
	ftinv [1] [2] = ftinv [1] [3] = ftinv [2] [3] = 0.0;

	// Construct G and its eigen-decomposition (eq. (4,5))
	// Sort eigenvalues (& eigenvectors) ascending.
	
	autoMAT b = MATzero (n3, n3);
	b [3] [1] = b [1] [3] = -0.5;
	b [2] [2] = 1.0;

	// G = F^-1 B (F')^-1 (eq. 4)

	for (integer i = 1; i <= 3; i ++) {
		for (integer j = 1; j <= 3; j ++) {
			for (integer k = 1; k <= 3; k ++) {
				if (ftinv [k] [i] != 0.0) {
					for (integer l = 1; l <= 3; l ++) {
						g [i] [j] += ftinv [k] [i] * b [k] [l] * ftinv [l] [j];
					}
				}
			}
		}
	}

	// G's eigen-decomposition with eigenvalues (assumed ascending). (eq. 5)
	autoMAT p;
	autoVEC delta;
	MAT_getEigenSystemFromSymmetricMatrix (g.get(), & p, & delta, true);

	// Construct y = P'.F'.O'.d ==> Solve (F')^-1 . P .y = (O'.d)	(page 632)
	// Get P'F^-1 from the transpose of (F')^-1 . P
	
	autoVEC otd (n3, kTensorInitializationType::ZERO);
	autoMAT ftinvp (n3, n3, kTensorInitializationType::ZERO);
	for (integer i = 1; i <= 3; i ++) {
		for (integer j = 1; j <= 3; j ++) {
			if (ftinv [i] [j] != 0.0) {
				for (integer k = 1; k <= 3; k ++) {
					ftinvp [i] [k] += ftinv [i] [j] * p [j] [k];
				}
			}
		}
		for (integer k = 1; k <= n3; k ++) {
			otd [i] += o [k] [i] * d [k];
		}
	}
	
	autoMAT ptfinvc (n3, n3, kTensorInitializationType::ZERO);

	for (integer i = 1; i <= 3; i ++) {
		for (integer j = 1; j <= 3; j ++) {
			ptfinvc [j] [i] = ptfinv [j] [i] = ftinvp [i] [j];
		}
	}

	autoVEC y = NUMsolveEquation (ftinvp.get(), otd.get(), 1e-6);

	// The solution (3 cases)
	autoVEC w (n3, kTensorInitializationType::ZERO);
	autoVEC chi;
	autoVEC diag (n3, kTensorInitializationType::ZERO);

	if (fabs (y [1]) < eps) {
		// Case 1: page 633

		t2 = y [2] / (delta [2] - delta [1]);
		t3 = y [3] / (delta [3] - delta [1]);
		/* +- */
		w [1] = sqrt (- delta [1] * (t2 * t2 * delta [2] + t3 * t3 * delta [3]));
		w [2] = t2 * delta [2];
		w [3] = t3 * delta [3];

		chi = NUMsolveEquation (ptfinv.get(), w.get(), 1e-6);

		w [1] = -w [1];
		if (fabs (chi [3] / chi [1]) < eps) chi = NUMsolveEquation (ptfinvc.get(), w.get(), 1e-6);
		
	} else if (fabs (y [2]) < eps) {
		// Case 2: page 633

		t1 = y [1] / (delta [1] - delta [2]);
		t3 = y [3] / (delta [3] - delta [2]);
		w [1] = t1 * delta [1];
		if ( (delta [2] < delta [3] && (t2 = (t1 * t1 * delta [1] + t3 * t3 * delta [3])) < eps)) {
			w [2] = sqrt (- delta [2] * t2); /* +- */
			w [3] = t3 * delta [3];
			chi = NUMsolveEquation (ptfinv.get(), w.get(), 1e-6);
			w [2] = -w [2];
			if (fabs (chi [3] / chi [1]) < eps) chi = NUMsolveEquation (ptfinvc.get(), w.get(), 1e-6);

		} else if (((delta [2] < delta [3] + eps) || (delta [2] > delta [3] - eps)) && fabs (y [3]) < eps) {
			// choose one value for w [2] from an infinite number

			w [2] = w [1];
			w [3] = sqrt (- t1 * t1 * delta [1] * delta [2] - w [2] * w [2]);
			chi = NUMsolveEquation (ptfinv.get(), w.get(), 1e-6);
		}
	} else {
		// Case 3: page 634 use Newton-Raphson root finder

		struct nr_struct me;
		double xlambda, eps2 = (delta [2] - delta [1]) * 1e-6;

		me.y = y.at;
		me.delta = delta.at;

		NUMnrbis (nr_func, delta [1] + eps, delta [2] - eps2, & me, & xlambda);

		for (integer i = 1; i <= 3; i++) {
			w [i] = y [i] / (1.0 - xlambda / delta [i]);
		}
		chi = NUMsolveEquation (ptfinv.get(), w.get(), 1e-6);
	}

	if (out_alpha) *out_alpha = chi [1];
	if (out_gamma) *out_gamma = chi [3];
}

/*
	f (b) = delta - b / (2 alpha) - sum (x [i]^2 / (c [i] - b)^2, i=1..n)
	f'(b) = - 1 / (2 alpha) + 2 * sum (x [i]^2 / (c [i] - b)^3, i=1..n)
*/
struct nr2_struct {
	integer m;
	double delta, alpha, *x, *c;
};

static void nr2_func (double b, double *f, double *df, void *data) {
	struct nr2_struct *me = (struct nr2_struct *) data;

	*df = - 0.5 / my alpha;
	*f = my delta + *df * b;
	for (integer i = 1; i <= my m; i ++) {
		double c1 = (my c [i] - b);
		double c2 = my x [i] / c1;
		double c2sq = c2 * c2;
		*f -= c2sq;
		*df += 2 * c2sq / c1;
	}
}

autoVEC NUMsolveWeaklyConstrainedLinearRegression (constMAT f, constVEC phi, double alpha, double delta) {
	// n = f.nrow m=f.ncol
	autoMAT u = MATzero (f.ncol, f.ncol);
	autoVEC c = VECzero (f.ncol);
	autoVEC x = VECzero (f.nrow);
	autoVEC t;
	
	autoSVD svd = SVD_createFromGeneralMatrix (f);

	if (alpha == 0.0) {
		t = SVD_solve (svd.get(), phi);	// standard least squares
	}


	// Step 1: Compute U and C from the eigendecomposition F'F = UCU'
	// Evaluate q, the multiplicity of the smallest eigenvalue in C

	autoINTVEC indx = NUMindexx (svd -> d.get());

	for (integer j = f.ncol; j > 0; j --) {
		double tmp = svd -> d [indx [j]];
		c [f.ncol - j + 1] = tmp * tmp;
		for (integer k = 1; k <= f.ncol; k ++) {
			u [f.ncol - j + 1] [k] = svd -> v [indx [j]] [k];
		}
	}

	integer q = 1;
	double tol = 1e-6;
	while (q < f.ncol && (c [f.ncol - q] - c [f.ncol]) < tol) {
		q ++;
	}

	// step 2: x = U'F'phi

	for (integer i = 1; i <= f.ncol; i ++) {
		for (integer j = 1; j <= f.ncol; j ++) {
			for (integer k = 1; k <= f.nrow; k ++) {
				x [i] += u [j] [i] * f [k] [j] * phi [k];
			}
		}
	}

	// step 3:

	struct nr2_struct me;
	me.m = f.ncol;
	me.delta = delta;
	me.alpha = alpha;
	me.x = x.at;
	me.c = c.at;

	double xqsq = 0.0;
	for (integer j = f.ncol - q + 1; j <= f.ncol; j ++) {
		xqsq += x [j] * x [j];
	}

	integer r = f.ncol;
	if (xqsq < tol) { /* xqsq == 0 */
		double fm, df;
		r = f.ncol - q;
		me.m = r;
		nr2_func (c [f.ncol], &fm, &df, & me);
		if (fm >= 0.0) { /* step 3.b1 */
			x [r + 1] = sqrt (fm);
			for (integer j = 1; j <= r; j ++) {
				x [j] /= c [j] - c [f.ncol];
			}
			for (integer j = 1; j <= r + 1; j ++) {
				for (integer k = 1; k <= r + 1; k ++) {
					t [j] += u [j] [k] * x [k];
				}
			}
			return t;
		}
		// else continue with r = m - q
	}

	// step 3a & 3b2, determine interval lower bound for Newton-Raphson root finder

	double xCx = 0.0;
	for (integer j = 1; j <= r; j ++) {
		xCx += x [j] * x [j] / c [j];
	}
	double b0, bmin = delta > 0.0 ? - xCx / delta : -2.0 * sqrt (alpha * xCx);
	double eps = (c [f.ncol] - bmin) * tol;

	// find the root of d(psi(b)/db in interval (bmin, c [m])

	NUMnrbis (nr2_func, bmin + eps, c [f.ncol] - eps, & me, & b0);

	for (integer j = 1; j <= r; j ++) {
		for (integer k = 1; k <= r; k ++) {
			t [j] += u [j] [k] * x [k] / (c [k] - b0);
		}
	}
	return t;
}


void NUMprocrustes (constMAT x, constMAT y, autoMAT *out_rotation, autoVEC *out_translation, double *out_scale) {
	Melder_assert (x.nrow == y.nrow && x.ncol == y.ncol);
	Melder_assert (x.nrow >= x.ncol);
	bool orthogonal = ! out_translation || ! out_scale; // else similarity transform

	/*
		Reference: Borg & Groenen (1997), Modern multidimensional scaling,
		Springer
		1. Calculate C = X'JY (page 346) for similarity transform
			else X'Y for othogonal (page 341)
			JY amounts to centering the columns of Y.
	*/
	
	autoMAT yc = MATcopy (y);
	if (! orthogonal)
		MATcentreEachColumn_inplace (yc.get());
	autoMAT c = MATmul (constMATVUtranspose (x), yc.get()); // X'(JY)

	// 2. Decompose C by SVD: C = UDV' (our SVD has eigenvectors stored row-wise V!)

	autoSVD svd = SVD_createFromGeneralMatrix (c.get());
	double trace = NUMsum (svd -> d.get());
	Melder_require (trace > 0.0, U"NUMprocrustes: degenerate configuration(s).");

	// 3. T = VU'

	autoMAT rotation = MATmul (svd->v.get(), constMATVUtranspose (svd->u.get()));
	
	if (! orthogonal) {

		// 4. Dilation factor s = (tr X'JYT) / (tr Y'JY)
		// First we need YT.
		
		autoMAT yt = MATmul (y, rotation.get());
		
		// X'J = (JX)' centering the columns of X

		autoMAT xc = MATcopy (x);
		MATcentreEachColumn_inplace (xc.get());

		// tr X'J YT == tr xc' yt

		double traceXtJYT = NUMtrace2_tn (xc.get(), yt.get()); // trace (Xc'.(YT))
		double traceYtJY = NUMtrace2_tn (y, yc.get()); // trace (Y'.Yc)
		longdouble scale = traceXtJYT / traceYtJY;

		// 5. Translation vector tr = (X - sYT)'1 / x.nrow
		if (out_translation) {
			autoVEC translation = VECzero (x.ncol);
			for (integer i = 1; i <= x.ncol; i ++) {
				longdouble productsum = 0.0;
				for (integer j = 1; j <= x.nrow; j ++)
					productsum += x [j] [i] - scale * yt [j] [i];
				translation [i] = productsum / x.nrow;
			}
			*out_translation = translation.move();
		}
		if (out_scale) *out_scale = (double) scale;
	}
	if (out_rotation) *out_rotation = rotation.move();
}


double NUMmspline (constVEC knot, integer order, integer i, double x) {
	integer jj, nSplines = knot.size - order;
	
	double y = 0.0;
	Melder_require (nSplines > 0, U"No splines.");
	Melder_require (order > 0 && i <= nSplines, U"Combination of order and index not correct.");
	/*
		Find the interval where x is located.
		M-splines of order k have degree k-1.
		M-splines are zero outside interval [ knot [i], knot [i+order] ).
		First and last 'order' knots are equal, i.e.,
		knot [1] = ... = knot [order] && knot [knot.size-order+1] = ... knot [knot.size].
	*/
	
	for (jj = order; jj <= knot.size - order + 1; jj ++) {
		if (x < knot [jj]) {
			break;
		}
	}
	if (jj < i || (jj > i + order) || jj == order || jj > (knot.size - order + 1)) {
		return y;
	}

	// Calculate M [i](x|1,t) according to eq.2.

	integer ito = i + order - 1;
	autoNUMvector<double> m (i, ito);
	for (integer j = i; j <= ito; j ++) {
		if (x >= knot [j] && x < knot [j + 1]) {
			m [j] = 1 / (knot [j + 1] - knot [j]);
		}
	}

	// Iterate to get M [i](x|k,t)

	for (integer k = 2; k <= order; k ++) {
		for (integer j = i; j <= i + order - k; j ++) {
			double kj = knot [j], kjpk = knot [j + k];
			if (kjpk > kj) {
				m [j] = k * ((x - kj) * m [j] + (kjpk - x) * m [j + 1]) / ((k - 1) * (kjpk - kj));
			}
		}
	}
	y = m [i];
	return y;
}

double NUMispline (constVEC aknot, integer order, integer i, double x) {
	integer j, orderp1 = order + 1;

	double y = 0.0;

	for (j = orderp1; j <= aknot.size - order; j ++) {
		if (x < aknot [j]) {
			break;
		}
	}
	if (-- j < i) {
		return y;
	}
	if (j > i + order || (j == aknot.size - order && x == aknot [j])) {
		return 1.0;
	}
	/*
		Equation 5 in Ramsay's article contains some errors!!!
		1. the interval selection should be 'j-k <= i <= j' instead of
			j-k+1 <= i <= j'
		2. the summation index m starts at 'i+1' instead of 'i'
	*/
	for (integer m = i + 1; m <= j; m ++) {
		double r = NUMmspline (aknot, orderp1, m, x);
		y += (aknot [m + orderp1] - aknot [m]) * r;
	}
	y /= orderp1;
	return y;
}

double NUMwilksLambda (double *lambda, integer from, integer to) {
	double result = 1.0;
	for (integer i = from; i <= to; i ++) {
		result /= (1.0 + lambda [i]);
	}
	return result;
}

double NUMfactln (int n) {
	static double table [101];
	if (n < 0) {
		return undefined;
	}
	if (n <= 1) {
		return 0.0;
	}
	return n > 100 ? NUMlnGamma (n + 1.0) : table [n] != 0.0 ? table [n] : (table [n] = NUMlnGamma (n + 1.0));
}

void NUMnrbis (void (*f) (double x, double *fx, double *dfx, void *closure), double xmin, double xmax, void *closure, double *root) {
	double df, fx, fh, fl, tmp, xh, xl, tol;
	integer itermax = 1000; // 80 or so could be enough; 60 is too small

	(*f) (xmin, &fl, &df, closure);
	if (fl == 0.0) {
		*root = xmin;
		return;
	}

	(*f) (xmax, &fh, &df, closure);
	if (fh == 0.0) {
		*root = xmax;
		return;
	}

	if ((fl > 0.0 && fh > 0.0) || (fl < 0.0 && fh < 0.0)) {
		*root = undefined;
		return;
	}

	if (fl < 0.0) {
		xl = xmin;
		xh = xmax;
	} else {
		xh = xmin;
		xl = xmax;
	}

	double dxold = fabs (xmax - xmin);
	double dx = dxold;
	*root = 0.5 * (xmin + xmax);
	(*f) (*root, &fx, &df, closure);

	for (integer iter = 1; iter <= itermax; iter ++) {
		if ((((*root - xh) * df - fx) * ((*root - xl) * df - fx) >= 0.0) || (fabs (2.0 * fx) > fabs (dxold * df))) {
			dxold = dx;
			dx = 0.5 * (xh - xl);
			*root = xl + dx;
			if (xl == *root) {
				return;
			}
		} else {
			dxold = dx;
			dx = fx / df;
			tmp = *root;
			*root -= dx;
			if (tmp == *root) {
				return;
			}
		}
		tol = NUMfpp -> eps	* (*root == 0.0 ? 1.0 : fabs (*root));
		if (fabs (dx) < tol) {
			return;
		}

		(*f) (*root, &fx, &df, closure);

		if (fx < 0.0) {
			xl = *root;
		} else {
			xh = *root;
		}
	}
	Melder_warning (U"NUMnrbis: maximum number of iterations (", itermax, U") exceeded.");
}

double NUMridders (double (*f) (double x, void *closure), double x1, double x2, void *closure) {
	/* There is still a problem with this implementation:
		tol may be zero;
	*/
	double x3, x4, d, root = undefined, tol;
	integer itermax = 100;

	double f1 = f (x1, closure);
	if (f1 == 0.0) {
		return x1;
	}
	if (isundef (f1)) {
		return undefined;
	}
	double f2 = f (x2, closure);
	if (f2 == 0.0) {
		return x2;
	}
	if (isundef (f2)) {
		return undefined;
	}
	if ((f1 < 0.0 && f2 < 0.0) || (f1 > 0.0 && f2 > 0.0)) {
		return undefined;
	}

	for (integer iter = 1; iter <= itermax; iter ++) {
		x3 = 0.5 * (x1 + x2);
		double f3 = f (x3, closure);
		if (f3 == 0.0) {
			return x3;
		}
		if (isundef (f3)) {
			return undefined;
		}

		// New guess: x4 = x3 + (x3 - x1) * sign(f1 - f2) * f3 / sqrt(f3^2 - f1*f2)

		d = f3 * f3 - f1 * f2;
		if (d < 0.0) {
			Melder_warning (U"d < 0 in ridders (iter = ", iter, U").");
			return undefined;
		}

		if (d == 0.0) {
			// pb test added because f1 f2 f3 may be 1e-170 or so
			tol = NUMfpp -> eps * (x3 == 0.0 ? 1.0 : fabs (x3));
			if (iter > 1 && fabs (x3 - root) < tol) {
				return root;
			}
			root = x3;

			// Perform bisection.

			if (f1 > 0.0) { 
				// falling curve: f1 > 0, f2 < 0 
				if (f3 > 0.0) {
					x1 = x3; f1 = f3; // retain invariant: f1 > 0, f2 < 0
				} else {
					// f3 <= 0.0
					x2 = x3; f2 = f3; // retain invariant: f1 > 0, f2 < 0
				}
			} else {
				// rising curve: f1 < 0, f2 > 0 
				if (f3 > 0.0) {
					x2 = x3; f2 = f3; // retain invariant: f1 < 0, f2 > 0
				} else {
					// f3 < 0.0
					x1 = x3; f1 = f3; // retain invariant: f1 < 0, f2 > 0
				}
			}
		} else {
			d = sqrt (d);
			if (isnan (d)) {
				// pb: square root of denormalized small number fails on some computers
				tol = NUMfpp -> eps * (x3 == 0.0 ? 1.0 : fabs (x3));
				if (iter > 1 && fabs (x3 - root) < tol) {
					return root;
				}
				root = x3;

				// Perform bisection.

				if (f1 > 0.0) {
					// falling curve: f1 > 0, f2 < 0
					if (f3 > 0.0) {
						x1 = x3; f1 = f3; // retain invariant: f1 > 0, f2 < 0
					} else {
						// f3 <= 0.0
						x2 = x3; f2 = f3; // retain invariant: f1 > 0, f2 < 0
					}
				} else {
					// rising curve: f1 < 0, f2 > 0
					if (f3 > 0.0) {
						x2 = x3; f2 = f3; // retain invariant: f1 < 0, f2 > 0
					} else {
						// f3 < 0.0
						x1 = x3; f1 = f3; // retain invariant: f1 < 0, f2 > 0 */
					}
				}
			} else {
				d = (x3 - x1) * f3 / d;
				x4 = f1 - f2 < 0 ? x3 - d : x3 + d;
				tol = NUMfpp -> eps * (x4 == 0.0 ? 1.0 : fabs (x4));
				if (iter > 1 && fabs (x4 - root) < tol) {
					return root;
				}
				root = x4;
				double f4 = f (x4, closure);
				if (f4 == 0.0) {
					return root;
				}
				if (isundef (f4)) {
					return undefined;
				}
				if ((f1 > f2) == (d > 0.0) /* pb: instead of x3 < x4 */) {
					if (SIGN (f3, f4) != f3) {
						x1 = x3; f1 = f3;
						x2 = x4; f2 = f4;
					} else {
						x1 = x4; f1 = f4;
					}
				} else {
					if (SIGN (f3, f4) != f3) {
						x1 = x4; f1 = f4;
						x2 = x3; f2 = f3;
					} else {
						x2 = x4; f2 = f4;
					}
				}
			}
		}
		if (fabs (x1 - x2) < tol) {
			return root;
		}
	}

	{
		static integer nwarnings = 0;
		nwarnings ++;
		Melder_warning (U"NUMridders: maximum number of iterations (", itermax, U") exceeded.");
	}
	return root;
}

double NUMlogNormalP (double x, double zeta, double sigma) {
	return gsl_cdf_lognormal_P (x, zeta, sigma);
}

double NUMlogNormalQ (double x, double zeta, double sigma) {
	return gsl_cdf_lognormal_Q (x, zeta, sigma);
}

double NUMstudentP (double t, double df) {
	if (df < 1.0) {
		return undefined;
	}
	double ib = NUMincompleteBeta (0.5 * df, 0.5, df / (df + t * t));
	if (isundef (ib)) {
		return undefined;
	}
	ib *= 0.5;
	return t < 0.0 ? ib : 1.0 - ib;
}

double NUMstudentQ (double t, double df) {
	if (df < 1) {
		return undefined;
	}
	double ib = NUMincompleteBeta (0.5 * df, 0.5, df / (df + t * t));
	if (isundef (ib)) {
		return undefined;
	}
	ib *= 0.5;
	return t > 0.0 ? ib : 1.0 - ib;
}

double NUMfisherP (double f, double df1, double df2) {
	if (f < 0.0 || df1 < 1.0 || df2 < 1.0) {
		return undefined;
	}
	double ib = NUMincompleteBeta (0.5 * df2, 0.5 * df1, df2 / (df2 + f * df1));
	if (isundef (ib)) {
		return undefined;
	}
	return 1.0 - ib;
}

double NUMfisherQ (double f, double df1, double df2) {
	if (f < 0.0 || df1 < 1.0 || df2 < 1.0) {
		return undefined;
	}
	if (Melder_debug == 28) {
		return NUMincompleteBeta (0.5 * df2, 0.5 * df1, df2 / (df2 + f * df1));
	} else {
		double result = gsl_cdf_fdist_Q (f, df1, df2);
		if (isnan (result)) {
			return undefined;
		}
		return result;
	}
}

double NUMinvGaussQ (double p) {
	double pc = p;
	if (p <= 0.0 || p >= 1.0) {
		return undefined;
	}
	if (p > 0.5) {
		pc = 1.0 - p;
	}
	double t = sqrt (- 2.0 * log (pc));
	t -= (2.515517 + (0.802853 + 0.010328 * t) * t) /
		 (1.0 + (1.432788 + (0.189269 + 0.001308 * t) * t) * t);
	return p > 0.5 ? -t : t;
}

static double studentQ_func (double x, void *voidParams) {
	struct pdf1_struct *params = (struct pdf1_struct *) voidParams;
	double q = NUMstudentQ (x, params -> df);
	return ( isundef (q) ? undefined : q - params -> p );
}

double NUMinvStudentQ (double p, double df) {
	struct pdf1_struct params;
	double pc = ( p > 0.5 ? 1.0 - p : p ), xmin, xmax = 1.0, x;

	if (p < 0.0 || p >= 1.0) {
		return undefined;
	}

	// Bracket the function f(x) = NUMstudentQ (x, df) - p.

	for (;;) {
		double q = NUMstudentQ (xmax, df);
		if (isundef (q)) {
			return undefined;
		}
		if (q < pc) {
			break;
		}
		xmax *= 2.0;
	}

	xmin = ( xmax > 1.0 ? xmax / 2.0 : 0.0 );

	// Find zero of f(x) with Ridders' method.

	params. df = df;
	params. p = pc;
	x = NUMridders (studentQ_func, xmin, xmax, & params);
	if (isundef (x)) {
		return undefined;
	}

	return ( p > 0.5 ? -x : x );
}

static double chiSquareQ_func (double x, void *voidParams) {
	struct pdf1_struct *params = (struct pdf1_struct *) voidParams;
	double q = NUMchiSquareQ (x, params -> df);
	return ( isundef (q) ? undefined : q - params -> p );
}

double NUMinvChiSquareQ (double p, double df) {
	struct pdf1_struct params;
	double xmin, xmax = 1;

	if (p < 0.0 || p >= 1.0) {
		return undefined;
	}

	// Bracket the function f(x) = NUMchiSquareQ (x, df) - p.

	for (;;) {
		double q = NUMchiSquareQ (xmax, df);
		if (isundef (q)) {
			return undefined;
		}
		if (q < p) {
			break;
		}
		xmax *= 2.0;
	}
	xmin = ( xmax > 1.0 ? xmax / 2.0 : 0.0 );

	// Find zero of f(x) with Ridders' method.

	params. df = df;
	params. p = p;
	return NUMridders (chiSquareQ_func, xmin, xmax, & params);
}

static double fisherQ_func (double x, void *voidParams) {
	struct pdf2_struct *params = (struct pdf2_struct *) voidParams;
	double q = NUMfisherQ (x, params -> df1, params -> df2);
	return ( isundef (q) ? undefined : q - params -> p );
}

double NUMinvFisherQ (double p, double df1, double df2) {
	if (p <= 0.0 || p > 1.0 || df1 < 1.0 || df2 < 1.0) {
		return undefined;
	}
	if (Melder_debug == 29) {
		//if (p == 1.0) return 0.0;
		return gsl_cdf_fdist_Qinv (p, df1, df2);
	} else {
		struct pdf2_struct params;
		double top = 1000.0;
		if (p == 1.0) {
			return 0.0;
		}
		params. p = p;
		params. df1 = df1;
		params. df2 = df2;
		for (;;) {
			double q = NUMfisherQ (top, df1, df2);
			if (isundef (q)) {
				return undefined;
			}
			if (q < p) {
				break;
			}
			if (top > 0.9e300) {
				return undefined;
			}
			top *= 1e9;
		}
		return NUMridders (fisherQ_func, 0.0, p > 0.5 ? 2.2 : top, & params);
	}
}

double NUMbeta2 (double z, double w) {
	gsl_sf_result result;
	int status = gsl_sf_beta_e (z, w, &result);
	return status == GSL_SUCCESS ? result.val : undefined;
}

double NUMlnBeta (double a, double b) {
	gsl_sf_result result;
	int status = gsl_sf_lnbeta_e (a, b, &result);
	return status == GSL_SUCCESS ? result.val : undefined;
}

double NUMnormalityTest_HenzeZirkler (constMAT data, double *beta, double *tnb, double *lnmu, double *lnvar) {
	const integer n = data.nrow, p = data.ncol;
	if (*beta <= 0)
		*beta = (1.0 / sqrt (2.0)) * pow ((1.0 + 2 * p) / 4.0, 1.0 / (p + 4)) * pow (n, 1.0 / (p + 4));
	const double p2 = p / 2.0;
	const double beta2 = *beta * *beta, beta4 = beta2 * beta2, beta8 = beta4 * beta4;
	const double gamma = 1.0 + 2.0 * beta2, gamma2 = gamma * gamma, gamma4 = gamma2 * gamma2;
	const double delta = 1.0 + beta2 * (4.0 + 3.0 * beta2), delta2 = delta * delta;
	double prob = undefined;

	*tnb = *lnmu = *lnvar = undefined;

	if (n < 2 || p < 1)
		return prob;

	autoVEC zero = VECzero (p);
	autoMAT x = MATcopy (data);

	MATcentreEachColumn_inplace (x.get()); // x - xmean

	autoMAT covar = MATcovarianceFromColumnCentredMatrix (x.get(), 0);

	try {
		MATlowerCholeskyInverse_inplace (covar.get(), nullptr);
		longdouble sumjk = 0.0, sumj = 0.0;
		const double b1 = beta2 / 2.0, b2 = b1 / (1.0 + beta2);
		/* Heinze & Wagner (1997), page 3
			We use d [j] [k] = ||Y [j]-Y [k]||^2 = (Y [j]-Y [k])'S^(-1)(Y [j]-Y [k])
			So d [j] [k]= d [k] [j] and d [j] [j] = 0
		*/
		for (integer j = 1; j <= n; j ++) {
			for (integer k = 1; k < j; k ++) {
				const double djk = NUMmahalanobisDistance (covar.get(), x.row (j), x.row(k));
				sumjk += 2.0 * exp (-b1 * djk); // factor 2 because d [j] [k] == d [k] [j]
			}
			sumjk += 1.0; // for k == j
			const double djj = NUMmahalanobisDistance (covar.get(), x.row(j), zero.get());
			sumj += exp (-b2 * djj);
		}
		*tnb = (1.0 / n) * (double) sumjk - 2.0 * pow (1.0 + beta2, - p2) * (double) sumj + n * pow (gamma, - p2); // n *
	} catch (MelderError) {
		Melder_clearError ();
		*tnb = 4.0 * n;
	}

	double mu = 1.0 - pow (gamma, -p2) * (1.0 + p * beta2 / gamma + p * (p + 2) * beta4 / (2.0 * gamma2));
	double var = 2.0 * pow (1.0 + 4.0 * beta2, -p2)
		 + 2.0 * pow (gamma,  -p) * (1.0 + 2.0 * p * beta4 / gamma2  + 3.0 * p * (p + 2) * beta8 / (4.0 * gamma4))
		 - 4.0 * pow (delta, -p2) * (1.0 + 3.0 * p * beta4 / (2.0 * delta) + p * (p + 2) * beta8 / (2.0 * delta2));
	double mu2 = mu * mu;
	*lnmu = log (sqrt (mu2 * mu2 / (mu2 + var)));
	*lnvar = sqrt (log ( (mu2 + var) / mu2));
	prob = NUMlogNormalQ (*tnb, *lnmu, *lnvar);
	return prob;
}

/*************** Hz <--> other freq reps *********************/

double NUMmelToHertz3 (double mel) {
	if (mel < 0.0) {
		return undefined;
	}
	return mel < 1000.0 ? mel : 1000.0 * (exp (mel * log10 (2.0) / 1000.0) - 1.0);
}

double NUMhertzToMel3 (double hz) {
	if (hz < 0.0) {
		return undefined;
	}
	return hz < 1000.0 ? hz : 1000.0 * log10 (1.0 + hz / 1000.0) / log10 (2.0);
}

double NUMmelToHertz2 (double mel) {
	if (mel < 0.0) {
		return undefined;
	}
	return 700.0 * (pow (10.0, mel / 2595.0) - 1.0);
}

double NUMhertzToMel2 (double hz) {
	if (hz < 0.0) {
		return undefined;
	}
	return 2595.0 * log10 (1.0 + hz / 700.0);
}

double NUMhertzToBark_traunmueller (double hz) {
	if (hz < 0.0) {
		return undefined;
	}
	return 26.81 * hz / (1960.0 + hz) - 0.53;
}

double NUMbarkToHertz_traunmueller (double bark) {
	if (bark < 0.0 || bark > 26.28) {
		return undefined;
	}
	return 1960.0 * (bark + 0.53) / (26.28 - bark);
}

double NUMbarkToHertz_schroeder (double bark) {
	return 650.0 * sinh (bark / 7.0);
}

double NUMbarkToHertz_zwickerterhardt (double hz) {
	if (hz < 0.0) {
		return undefined;
	}
	return 13.0 * atan (0.00076 * hz) + 3.5 * atan (hz / 7500.0);
}

double NUMhertzToBark_schroeder (double hz) {
	if (hz < 0.0) {
		return undefined;
	}
	double h650 = hz / 650.0;
	return 7.0 * log (h650 + sqrt (1.0 + h650 * h650));
}

double NUMbarkToHertz2 (double bark) {
	if (bark < 0.0) {
		return undefined;
	}
	return 650.0 * sinh (bark / 7.0);
}

double NUMhertzToBark2 (double hz) {
	if (hz < 0) {
		return undefined;
	}
	double h650 = hz / 650.0;
	return 7.0 * log (h650 + sqrt (1.0 + h650 * h650));
}

double NUMbladonlindblomfilter_amplitude (double zc, double z) {
	double dz = zc - z + 0.474;
	return pow (10.0, 1.581 + 0.75 * dz - 1.75 * sqrt (1.0 + dz * dz));
}

double NUMsekeyhansonfilter_amplitude (double zc, double z) {
	double dz = zc - z - 0.215;
	return pow (10.0, 0.7 - 0.75 * dz - 1.75 * sqrt (0.196 + dz * dz));
}

double NUMtriangularfilter_amplitude (double fl, double fc, double fh, double f) {
	double a = 0.0;
	if (f > fl && f < fh) {
		a = f < fc ? (f - fl) / (fc - fl) : (fh - f) / (fh - fc);

		/* Normalize such that area under the filter is always 1. ???

		a /= 2 * (fh - fl);*/
	}
	return a;
}

double NUMformantfilter_amplitude (double fc, double bw, double f) {
	double dq = (fc * fc - f * f) / (bw * f);
	return 1.0 / (dq * dq + 1.0);
}

/* Childers (1978), Modern Spectrum analysis, IEEE Press, 252-255) */
/* work [1..n+n+n];
b1 = & work [1];
b2 = & work [n+1];
aa = & work [n+n+1];
for (i=1; i<=n+n+n; i ++) work [i]=0;
*/
double NUMburg_preallocated (VEC a, constVEC x) {
	integer n = x.size, m = a.size;
	for (integer j = 1; j <= m; j ++) {
		a [j] = 0.0;
	}

	autoVEC b1 = VECzero (n), b2 = VECzero (n), aa = VECzero (m);

	// (3)

	longdouble p = 0.0;
	for (integer j = 1; j <= n; j ++) {
		p += x [j] * x [j];
	}

	longdouble xms = p / n;
	if (xms <= 0.0) {
		return xms;	// warning empty
	}

	// (9)

	b1 [1] = x [1];
	b2 [n - 1] = x [n];
	for (integer j = 2; j <= n - 1; j ++) {
		b1 [j] = b2 [j - 1] = x [j];
	}

	for (integer i = 1; i <= m; i ++) {
		// (7)

		longdouble num = 0.0, denum = 0.0;
		for (integer j = 1; j <= n - i; j ++) {
			num += b1 [j] * b2 [j];
			denum += b1 [j] * b1 [j] + b2 [j] * b2 [j];
		}

		if (denum <= 0.0) {
			return 0.0;	// warning ill-conditioned
		}

		a [i] = 2.0 * num / denum;

		// (10)

		xms *= 1.0 - a [i] * a [i];

		// (5)

		for (integer j = 1; j <= i - 1; j ++) {
			a [j] = aa [j] - a [i] * aa [i - j];
		}

		if (i < m) {

			// (8) Watch out: i -> i+1

			for (integer j = 1; j <= i; j ++) {
				aa [j] = a [j];
			}
			for (integer j = 1; j <= n - i - 1; j ++) {
				b1 [j] -= aa [i] * b2 [j];
				b2 [j] = b2 [j + 1] - aa [i] * b1 [j + 1];
			}
		}
	}
	return xms;
}

autoVEC NUMburg (constVEC x, integer numberOfPredictionCoefficients, double *out_xms) {
	autoVEC a = VECraw (numberOfPredictionCoefficients);
	double xms = NUMburg_preallocated (a.get(), x);
	if (out_xms) *out_xms = xms;
	return a;
}

void NUMdmatrix_to_dBs (double **m, integer rb, integer re, integer cb, integer ce, double ref, double factor, double floor) {
	double ref_db, factor10 = factor * 10.0;
	double max = m [rb] [cb], min = max;

	Melder_assert (ref > 0 && factor > 0 && rb <= re && cb <= ce);

	for (integer i = rb; i <= re; i ++) {
		for (integer j = cb; j <= ce; j ++) {
			if (m [i] [j] > max) {
				max = m [i] [j];
			} else if (m [i] [j] < min) {
				min = m [i] [j];
			}
		}
	}
	
	Melder_require (min >= 0.0 && max >= 0.0, U"All matrix elements should be positive.");
	
	ref_db = factor10 * log10 (ref);

	for (integer i = rb; i <= re; i ++) {
		for (integer j = cb; j <= ce; j ++) {
			double mij = floor;
			if (m [i] [j] > 0.0) {
				mij = factor10 * log10 (m [i] [j]) - ref_db;
				if (mij < floor) {
					mij = floor;
				}
			}
			m [i] [j] = mij;
		}
	}
}

double **NUMcosinesTable (integer first, integer last, integer npoints) {
	Melder_assert (0 < first && first <= last && npoints > 0);
	autoNUMmatrix<double> m (first, last, 1, npoints);
	for (integer i = first; i <= last; i ++) {
		double f = i * NUMpi / npoints;
		for (integer j = 1; j <= npoints; j ++) {
			m [i] [j] = cos (f * (j - 0.5));
		}
	}
	return m.transfer();
}

void NUMcubicSplineInterpolation_getSecondDerivatives (double x [], double y [], integer n, double yp1, double ypn, double y2 []) {
	autoNUMvector<double> u (1, n - 1);

	if (yp1 > 0.99e30) {
		y2 [1] = u [1] = 0.0;
	} else {
		y2 [1] = -0.5;
		u [1] = (3.0 / (x [2] - x [1])) * ( (y [2] - y [1]) / (x [2] - x [1]) - yp1);
	}

	for (integer i = 2; i <= n - 1; i ++) {
		double sig = (x [i] - x [i - 1]) / (x [i + 1] - x [i - 1]);
		double p = sig * y2 [i - 1] + 2.0;
		y2 [i] = (sig - 1.0) / p;
		u [i] = (y [i + 1] - y [i]) / (x [i + 1] - x [i]) - (y [i] - y [i - 1]) / (x [i] - x [i - 1]);
		u [i] = (6.0 * u [i] / (x [i + 1] - x [i - 1]) - sig * u [i - 1]) / p;
	}

	double qn, un;
	if (ypn > 0.99e30) {
		qn = un = 0.0;
	} else {
		qn = 0.5;
		un = (3.0 / (x [n] - x [n - 1])) * (ypn - (y [n] - y [n - 1]) / (x [n] - x [n - 1]));
	}

	y2 [n] = (un - qn * u [n - 1]) / (qn * y2 [n - 1] + 1.0);
	for (integer k = n - 1; k >= 1; k--) {
		y2 [k] = y2 [k] * y2 [k + 1] + u [k];
	}
}

double NUMcubicSplineInterpolation (double xa [], double ya [], double y2a [], integer n, double x) {
	integer klo = 1, khi = n;
	while (khi - klo > 1) {
		integer k = (khi + klo) >> 1;
		if (xa [k] > x) {
			khi = k;
		} else {
			klo = k;
		}
	}
	double h = xa [khi] - xa [klo];
	Melder_require (h != 0.0, U"NUMcubicSplineInterpolation: bad input value.");
	
	double a = (xa [khi] - x) / h;
	double b = (x - xa [klo]) / h;
	double y = a * ya [klo] + b * ya [khi] + ( (a * a * a - a) * y2a [klo] + (b * b * b - b) * y2a [khi]) * (h * h) / 6.0;
	return y;
}

double NUMsinc (const double x) {
	struct gsl_sf_result_struct result;
	int status = gsl_sf_sinc_e (x / NUMpi, &result);
	return status == GSL_SUCCESS ? result. val : undefined;
}

double NUMsincpi (const double x) {
	struct gsl_sf_result_struct result;
	int status = gsl_sf_sinc_e (x, &result);
	return status == GSL_SUCCESS ? result. val : undefined;
}

/* Does the line segment from (x1,y1) to (x2,y2) intersect with the line segment from (x3,y3) to (x4,y4)? */
int NUMdoLineSegmentsIntersect (double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
	int o11 = NUMgetOrientationOfPoints (x1, y1, x2, y2, x3, y3);
	int o12 = NUMgetOrientationOfPoints (x1, y1, x2, y2, x4, y4);
	int o21 = NUMgetOrientationOfPoints (x3, y3, x4, y4, x1, y1);
	int o22 = NUMgetOrientationOfPoints (x3, y3, x4, y4, x2, y2);
	return ((o11 * o12 < 0) && (o21 * o22 < 0)) || (o11 *o12 *o21 *o22 == 0);
}

int NUMgetOrientationOfPoints (double x1, double y1, double x2, double y2, double x3, double y3) {
	int orientation;
	double dx2 = x2 - x1, dy2 = y2 - y1;
	double dx3 = x3 - x1, dy3 = y3 - y1;
	if (dx2 * dy3 > dy2 * dx3) {
		orientation = 1;
	} else if (dx2 * dy3 < dy2 * dx3) {
		orientation = -1;
	} else {
		if ((dx2 * dx3 < 0) || (dy2 * dy3 < 0)) {
			orientation = -1;
		} else if ((dx2 * dx2 + dy2 * dy2) >= (dx3 * dx3 + dy3 * dy3)) {
			orientation = 0;
		} else {
			orientation = 1;
		}
	}
	return orientation;
}

int NUMgetIntersectionsWithRectangle (double x1, double y1, double x2, double y2, double xmin, double ymin, double xmax, double ymax, double *xi, double *yi) {
	double x [6], y [6];
	integer ni = 0;

	x [1] = x [4] = x [5] = xmin;
	x [2] = x [3] = xmax;
	y [1] = y [2] = y [5] = ymin;
	y [3] = y [4] = ymax;
	/*
		Calculate intersection of line segment through p1=(x1,y1) to p2(x2,y2) with line segment
		through p3=(x3,y3) to p4=(x4,y4).
		Parametrisation of the lines:
		l1 = p1 + s (p2 - p1), s in (-inf,+inf)
		l2 = p3 + t (p4 - p3), t in (-inf,+inf).
		When s and t are in [0,1] we have line segments between the points.
		At the intersection l1 == l2. We get for the x and y coordinates:
			x1 + s (x2 - x1) = x3 + t (x4 - x3).............(1)
			y1 + s (y2 - y1) = y3 + t (y4 - y3).............(2)
		Multiply (1)*(y2 - y1) and (2)*(x2 - x1):
			x1 (y2 - y1) + s (x2 - x1)(y2 - y1) = x3 (y2 - y1) + t (x4 - x3)(y2 - y1).......(3)
			y1 (x2 - x1) + s (y2 - y1)(x2 - x1) = y3 (x2 - x1) + t (y4 - y3)(x2 - x1).......(4)
		(3)-(4) with y21 = y2 -y1, x21 = x2 - x1, x43 = x4 - x3, ...
			x1 y21 - y1 x21 = x3 y21 - y3 x21 +t (x43 y21 - y43 x21)
		Combining:
			y31 x21 - x31 y21 = t (x43 y21 - y43 x21)
		Therefor at the intersection we have:

			t = (y31 x21 - x31 y21) / (x43 y21 - y43 x21)

		If (x43 y21 - y43 x21) == 0
			There is no intersection.
		If (t < 0 || t >= 1)
			No intersection in the segment l2
			To count intersections in a corner only once we have t < 0 instead of t <= 0!
	*/

	for (integer i = 1; i <= 4; i ++) {
		double denom = (x [i + 1] - x [i]) * (y2 - y1) - (y [i + 1] - y [i]) * (x2 - x1);
		double s, t, x3, y3;
		if (denom == 0.0) {
			continue;
		}
		/* We have an intersection. */
		t = ((y [i] - y1) * (x2 - x1) - (x [i] - x1) * (y2 - y1)) / denom;
		if (t < 0 || t >= 1) {
			continue;
		}
		/* Intersection is within rectangle side. */
		x3 = x [i] + t * (x [i + 1] - x [i]);
		y3 = y [i] + t * (y [i + 1] - y [i]);
		/* s must also be valid */
		if (x1 != x2) {
			s = (x3 - x1) / (x2 - x1);
		} else {
			s = (y3 - y1) / (y2 - y1);
		}
		if (s < 0 || s >= 1) {
			continue;
		}
		ni ++;
		Melder_require (ni <= 3, U"Too many intersections.");
		
		xi [ni] = x3;
		yi [ni] = y3;
	}
	return ni;
}

bool NUMclipLineWithinRectangle (double xl1, double yl1, double xl2, double yl2, double xr1, double yr1, double xr2, double yr2, double *xo1, double *yo1, double *xo2, double *yo2) {
	int ncrossings = 0;
	bool xswap, yswap;
	double xc [5], yc [5], xmin, xmax, ymin, ymax;

	*xo1 = xl1; *yo1 = yl1; *xo2 = xl2; *yo2 = yl2;

	// This test first because we expect the majority of the tested segments to be
	// within the rectangle
	if (xl1 >= xr1 && xl1 <= xr2 && yl1 >= yr1 && yl1 <= yr2 &&
			xl2 >= xr1 && xl2 <= xr2 && yl2 >= yr1 && yl2 <= yr2) {
		return true;
	}

	// All lines that are completely outside the rectangle
	if ( (xl1 <= xr1 && xl2 <= xr1) || (xl1 >= xr2 && xl2 >= xr2) ||
			(yl1 <= yr1 && yl2 <= yr1) || (yl1 >= yr2 && yl2 >= yr2)) {
		return false;
	}

	// At least line spans (part of) the rectangle.
	// Get extremes in x and y of the line for easy testing further on.
	if (xl1 < xl2) {
		xmin = xl1; xmax = xl2; xswap = false;
	} else {
		xmin = xl2; xmax = xl1; xswap = true;
	}
	if (yl1 < yl2) {
		ymin = yl1; ymax = yl2; yswap = false;
	} else {
		ymin = yl2; ymax = yl1; yswap = true;
	}
	bool hline = yl1 == yl2, vline = xl1 == xl2;
	if (hline) {
		if (xmin < xr1) {
			*xo1 = xr1;
		}
		if (xmax > xr2) {
			*xo2 = xr2;
		}
		if (xswap) {
			std::swap (*xo1, *xo2);
		}
		return true;
	}
	if (vline) {
		if (ymin < yr1) {
			*yo1 = yr1;
		}
		if (ymax > yr2) {
			*yo2 = yr2;
		}
		if (yswap) {
			std::swap (*yo1, *yo2);
		}
		return true;
	}

	// Now we know that the line from (x1,y1) to (x2,y2) is neither horizontal nor vertical.
	// Parametrize it as y = ax + b

	double a = (yl1 - yl2) / (xl1 - xl2);
	double b = yl1 - a * xl1;


	//	To determine the crossings we have to avoid counting the crossings in a corner twice.
	//	Therefore we test the corners inclusive (..<=..<=..) on the vertical borders of the rectangle
	//	and exclusive (..<..<) at the horizontal borders.


	double y = a * xr1 + b; // Crossing at y with left border: x = xr1

	if (y >= yr1 && y <= yr2 && xmin < xr1) { // Within vertical range?
		ncrossings ++;
		xc [ncrossings] = xr1; yc [ncrossings] = y;
		xc [2] = xmax;
		yc [2] = xl1 > xl2 ? yl1 : yl2;
	}

	double x = (yr2 - b) / a; // Crossing at x with top border: y = yr2

	if (x > xr1 && x < xr2 && ymax > yr2) { // Within horizontal range?
		ncrossings ++;
		xc [ncrossings] = x; yc [ncrossings] = yr2;
		if (ncrossings == 1) {
			yc [2] = ymin;
			xc [2] = yl1 < yl2 ? xl1 : xl2;
		}
	}

	y = a * xr2 + b; // Crossing at y with right border: x = xr2

	if (y >= yr1 && y <= yr2 && xmax > xr2) { // Within vertical range?
		ncrossings ++;
		xc [ncrossings] = xr2; yc [ncrossings] = y;
		if (ncrossings == 1) {
			xc [2] = xmin;
			yc [2] = xl1 < xl2 ? yl1 : yl2;
		}
	}

	x = (yr1 - b) / a; // Crossing at x with bottom border: y = yr1

	if (x > xr1 && x < xr2 && ymin < yr1) {
		ncrossings ++;
		xc [ncrossings] = x; yc [ncrossings] = yr1;
		if (ncrossings == 1) {
			yc [2] = ymax;
			xc [2] = yl1 > yl2 ? xl1 : xl2;
		}
	}
	if (ncrossings == 0) {
		return false;
	}
	Melder_require (ncrossings <= 2, U"Too many crossings found.");

	/*
		if start and endpoint of line are outside rectangle and ncrossings == 1, than the line only touches.
	*/
	if (ncrossings == 1 && (xl1 < xr1 || xl1 > xr2 || yl1 < yr1 || yl1 > yr2) &&
		(xl2 < xr1 || xl2 > xr2 || yl2 < yr1 || yl2 > yr2)) {
		return true;
	}

	if ((xc [1] > xc [2] && ! xswap) || (xc [1] < xc [2] && xswap)) {
		std::swap (xc [1], xc [2]);
		std::swap (yc [1], yc [2]);
	}
	*xo1 = xc [1]; *yo1 = yc [1]; *xo2 = xc [2]; *yo2 = yc [2];
	return true;
}

void NUMgetEllipseBoundingBox (double a, double b, double cospsi, double *width, double *height) {

	Melder_assert (cospsi >= -1 && cospsi <= 1);

	if (cospsi == 1) {

		// a-axis along x-axis

		*width = a;
		*height = b;
	} else if (cospsi == 0) {

		// a-axis along y-axis

		*width = b;
		*height = a;
	} else {
		double psi = acos (cospsi), sn = sin (psi);
		double phi = atan2 (-b * sn, a * cospsi);
		*width = fabs (a * cospsi * cos (phi) - b * sn * sin (phi));
		phi = atan2 (b * cospsi , a * sn);
		*height = fabs (a * sn * cos (phi) + b * cospsi * sin (phi));
	}
}

/*
	Closely modeled after the netlib code by Oleg Keselyov.
*/
double NUMminimize_brent (double (*f) (double x, void *closure), double a, double b, void *closure, double tol, double *fx) {
	double x, v, fv, w, fw;
	const double golden = 1 - NUM_goldenSection;
	const double sqrt_epsilon = sqrt (NUMfpp -> eps);
	integer itermax = 60;

	Melder_assert (tol > 0 && a < b);

	/* First step - golden section */

	v = a + golden * (b - a);
	fv = (*f) (v, closure);
	x = v; w = v;
	*fx = fv; fw = fv;

	for (integer iter = 1; iter <= itermax; iter ++) {
		double range = b - a;
		double middle_range = (a + b) / 2.0;
		double tol_act = sqrt_epsilon * fabs (x) + tol / 3.0;
		double new_step; /* Step at this iteration */

		if (fabs (x - middle_range) + range / 2.0 <= 2.0 * tol_act) {
			return x;
		}

		// Obtain the golden section step

		new_step = golden * (x < middle_range ? b - x : a - x);

		// Decide if the parabolic interpolation can be tried

		if (fabs (x - w) >= tol_act) {
			/*
				Interpolation step is calculated as p/q;
				division operation is delayed until last moment.
			*/

			double t = (x - w) * (*fx - fv);
			double q = (x - v) * (*fx - fw);
			double p = (x - v) * q - (x - w) * t;
			q = 2.0 * (q - t);

			if (q > 0.0) {
				p = -p;
			} else {
				q = -q;
			}

			/*
				If x+p/q falls in [a,b], not too close to a and b,
				and isn't too large, it is accepted.
				If p/q is too large then the golden section procedure can
				reduce [a,b] range.
			*/

			if (fabs (p) < fabs (new_step * q) &&
					p > q * (a - x + 2.0 * tol_act) &&
					p < q * (b - x - 2.0 * tol_act)) {
				new_step = p / q;
			}
		}

		// Adjust the step to be not less than tolerance.

		if (fabs (new_step) < tol_act) {
			new_step = new_step > 0.0 ? tol_act : - tol_act;
		}

		// Obtain the next approximation to min	and reduce the enveloping range

		{
			double t = x + new_step;	// Tentative point for the min
			double ft = (*f) (t, closure);

			/*
				If t is a better approximation, reduce the range so that
				t would fall within it. If x remains the best, reduce the range
				so that x falls within it.
			*/

			if (ft <= *fx) {
				if (t < x) {
					b = x;
				} else {
					a = x;
				}

				v = w; w = x; x = t;
				fv = fw; fw = *fx; *fx = ft;
			} else {
				if (t < x) {
					a = t;
				} else {
					b = t;
				}

				if (ft <= fw || w == x) {
					v = w; w = t;
					fv = fw; fw = ft;
				} else if (ft <= fv || v == x || v == w) {
					v = t;
					fv = ft;
				}
			}
		}
	}
	Melder_warning (U"NUMminimize_brent: maximum number of iterations (", itermax, U") exceeded.");
	return x;
}

/*
	probs is probability vector, i.e. all 0 <= probs [i] <= 1 and sum(i=1;i=nprobs, probs [i])= 1
	p is a probability
*/
integer NUMgetIndexFromProbability (double *probs, integer nprobs, double p) {
	integer index = 1;
	double psum = probs [index];
	while (p > psum && index < nprobs) {
		psum += probs [++ index];
	}
	return index;
}

// straight line fitting

void NUMlineFit_theil (double *x, double *y, integer numberOfPoints,
	double *out_m, double *out_intercept, bool incompleteMethod)
{
	try {
		/* Theil's incomplete method:
			Split (x [i],y [i]) as
			(x [i],y [i]), (x [N+i],y [N=i], i=1..numberOfPoints/2
			m [i] = (y [N+i]-y [i])/(x [N+i]-x [i])
			m = median (m [i])
			b = median(y [i]-m*x [i])
		 */
		double m, intercept;
		if (numberOfPoints <= 0) {
			m = intercept = undefined;
		} else if (numberOfPoints == 1) {
			intercept = y [1];
			m = 0.0;
		} else if (numberOfPoints == 2) {
			m = (y [2] - y [1]) / (x [2] - x [1]);
			intercept = y [1] - m * x [1];
		} else {
			integer numberOfCombinations;
			autoVEC mbs;
			if (incompleteMethod) { // incomplete method
				numberOfCombinations = numberOfPoints / 2;
				mbs = VECzero (numberOfPoints); //
				integer n2 = numberOfPoints % 2 == 1 ? numberOfCombinations + 1 : numberOfCombinations;
				for (integer i = 1; i <= numberOfCombinations; i ++)
					mbs [i] = (y [n2 + i] - y [i]) / (x [n2 + i] - x [i]);
			} else { // use all combinations
				numberOfCombinations = (numberOfPoints - 1) * numberOfPoints / 2;
				mbs = VECzero (numberOfCombinations);
				integer index = 0;
				for (integer i = 1; i < numberOfPoints; i ++)
					for (integer j = i + 1; j <= numberOfPoints; j ++)
						mbs [++ index] = (y [j] - y [i]) / (x [j] - x [i]);
			}
			VECsort_inplace (mbs.subview (1, numberOfCombinations));
			m = NUMquantile (mbs.subview (1, numberOfCombinations), 0.5);
			for (integer i = 1; i <= numberOfPoints; i ++)
				mbs [i] = y [i] - m * x [i];
			VECsort_inplace (mbs.subview (1, numberOfPoints));
			intercept = NUMquantile (mbs.subview (1, numberOfPoints), 0.5);
		}
		if (out_m)
			*out_m = m;
		if (out_intercept)
			*out_intercept = intercept;
	} catch (MelderError) {
		Melder_throw (U"No line fit (Theil's method).");
	}
}

void NUMlineFit_LS (double *x, double *y, integer numberOfPoints, double *p_m, double *intercept) {
	double sx = 0.0, sy = 0.0;
	for (integer i = 1; i <= numberOfPoints; i ++) {
		sx += x [i];
		sy += y [i];
	}
	double xmean = sx / numberOfPoints;
	double st2 = 0.0, m = 0.0;
	for (integer i = 1; i <= numberOfPoints; i ++) {
		double t = x [i] - xmean;
		st2 += t * t;
		m += t * y [i];
	}
	// y = m*x + b
	m /= st2;
	if (intercept) {
		*intercept = (sy - m * sx) / numberOfPoints;
	}
	if (p_m) {
		*p_m = m;
	}
}

void NUMlineFit (double *x, double *y, integer numberOfPoints, double *m, double *intercept, int method) {
	if (method == 1) {
		NUMlineFit_LS (x, y, numberOfPoints, m, intercept);
	} else if (method == 3) {
		NUMlineFit_theil (x, y, numberOfPoints, m, intercept, false);
	} else {
		NUMlineFit_theil (x, y, numberOfPoints, m, intercept, true);
	}
}

// IEEE: Programs for digital signal processing section 4.3 LPTRN
// lpc [1..n] to rc [1..n]
void NUMlpc_lpc_to_rc (double *lpc, integer p, double *rc) {
	autoNUMvector<double> b (1, p);
	autoNUMvector<double> a (NUMvector_copy<double> (lpc, 1, p), 1);
	for (integer m = p; m > 0; m--) {
		rc [m] = a [m];
		Melder_require (fabs (rc [m]) <= 1.0, U"Relection coefficient [", m, U"] larger than 1.");
		for (integer i = 1; i < m; i ++) {
			b [i] = a [i];
		}
		for (integer i = 1; i < m; i ++) {
			a [i] = (b [i] - rc [m] * b [m - i]) / (1.0 - rc [m] * rc [m]);
		}
	}
}

void NUMlpc_rc_to_area2 (double *rc, integer n, double *area);
void NUMlpc_rc_to_area2 (double *rc, integer n, double *area) {
	double s = 0.0001; /* 1.0 cm^2 at glottis */
	for (integer i = n; i > 0; i--) {
		s *= (1.0 + rc [i]) / (1.0 - rc [i]);
		area [i] = s;
	}
}

void NUMlpc_area_to_lpc2 (double *area, integer n, double *lpc);
void NUMlpc_area_to_lpc2 (double *area, integer n, double *lpc) {
	// from area to reflection coefficients
	autoNUMvector<double> rc (1, n);
	// normalisation: area [n+1] = 0.0001
	for (integer j = n; j > 0; j--) {
		double ar = area [j+1] / area [j];
		rc [j] = (1 - ar) / (1 + ar);
	}
	// LPTRAN works from mouth to lips:
	for (integer j = 1; j <= n; j ++) {
		lpc [j] = rc [n - j + 1];
	}
	for (integer j = 2; j <= n; j ++) {
		integer nh = j / 2;
		double q = rc [j];
		for (integer k = 1; k <= nh; k ++) {
			double at = lpc [k] + q * lpc [j - k];
			lpc [j - k] += q * lpc [k];
			lpc [k] = at;
		}
	}
}

void NUMlpc_lpc_to_rc2 (double *lpc, integer m, double *rc);
void NUMlpc_lpc_to_rc2 (double *lpc, integer m, double *rc) { // klopt nog niet
	NUMvector_copyElements<double> (lpc, rc, 1, m);
	for (integer j = 2; j <= m; j ++) {
		integer jb = m + 1 - j;
		integer mh = (jb + 1) / 2;
		double rct = rc [jb+1];
		double d = 1.0 - rct * rct;
		for (integer k = 1; k <= mh; k ++) {
			rc [k] *= (1 - rct) / d;
		}
	}
}
// area [1] at lips generates n+1 areas from n rc's
void NUMlpc_rc_to_area (double *rc, integer m, double *area) {
	area [m+1] = 0.0001; /* 1.0 cm^2 */
	for (integer j = 1; j <= m; j ++) {
		double ar = (1.0 - rc [m+1-j]) / (1.0 + rc [m+1-j]);
		area [m+1-j] = area [m+2-j] / ar;
	}
}

// returns m-1 reflection coefficients from m areas
void NUMlpc_area_to_rc (double *area, integer m, double *rc) {
	for (integer j = 1; j <= m - 1; j ++) {
		double ar = area [j+1] / area [j];
		rc [j] = (1.0 - ar) / (1.0 + ar);
	}
}

void NUMlpc_rc_to_lpc (double *rc, integer m, double *lpc);
void NUMlpc_rc_to_lpc (double *rc, integer m, double *lpc) {
	NUMvector_copyElements<double> (rc, lpc, 1, m);
	for (integer j = 2; j <= m; j ++) {
		for (integer k = 1; k <= j / 2; k ++) {
			double at = lpc [k] + rc [j] * lpc [j - k];
			lpc [j - k] += rc [j] * lpc [k];
			lpc [k] = at;
		}
	}
}

void NUMlpc_area_to_lpc (double *area, integer m, double *lpc) {
	// from area to reflection coefficients
	autoNUMvector<double> rc (1, m);
	// normalisation: area [n+1] = 0.0001
	NUMlpc_area_to_rc (area, m, rc.peek());
	NUMlpc_rc_to_lpc (rc.peek(), m - 1, lpc);
}

void NUMlpc_lpc_to_area (double *lpc, integer m, double *area) {
	autoNUMvector<double> rc (1, m);
	NUMlpc_lpc_to_rc (lpc, m, rc.peek());
	NUMlpc_rc_to_area (rc.peek(), m, area);

}

#undef SIGN

#define SMALL_MEAN 14
/* If n*p < SMALL_MEAN then use BINV algorithm. The ranlib implementation used cutoff=30;
 * but on my (Brian Gough) computer 14 works better
 */

#define BINV_CUTOFF 110
/* In BINV, do not permit ix too large */

#define FAR_FROM_MEAN 20
/* If ix-n*p is larger than this, then use the "squeeze" algorithm.
 * Ranlib used 20, and this seems to be the best choice on my (Brian Gough) machine as well.
 */

#define LNFACT(x) gsl_sf_lnfact(x)

inline static double Stirling (double y1)
{
	double y2 = y1 * y1;
	double s = (13860.0 - (462.0 - (132.0 - (99.0 - 140.0 / y2) / y2) / y2) / y2) / y1 / 166320.0;
	return s;
}

// djmw 20121211 replaced calls to gsl_rng_uniform with NUMrandomUniform (0,1)

integer NUMrandomBinomial (double p, integer n) {
	if (p < 0.0 || p > 1.0 || n < 0) {
		return -100000000;
	}
	integer ix;			// return value
	int flipped = 0;

	if (n == 0) {
		return 0;
	}
	if (p > 0.5) {
		p = 1.0 - p;	// work with small p
		flipped = 1;
	}

	double q = 1.0 - p;
	double s = p / q;
	double np = n * p;

	/* 
		Inverse cdf logic for small mean (BINV in K+S)
	*/

	if (np < SMALL_MEAN) {
		double f0 = pow (q, n); // djmw gsl_pow_int (q, n); f(x), starting with x=0

		while (1) {
			/* 
				This while(1) loop will almost certainly only loop once; but
				if u=1 to within a few epsilons of machine precision, then it
				is possible for roundoff to prevent the main loop over ix to
				achieve its proper value. Following the ranlib implementation,
				we introduce a check for that situation, and when it occurs,
				we just try again.
			*/

			double f = f0;
			double u = NUMrandomUniform (0.0, 1.0); // djmw gsl_rng_uniform (rng);

			for (ix = 0; ix <= BINV_CUTOFF; ++ ix) {
				if (u < f) {
					goto Finish;
				}
				u -= f;
				// Use recursion f(x+1) = f(x)* [(n-x)/(x+1)]* [p/(1-p)]
				f *= s * (n - ix) / (ix + 1.0);
			}

			/* 
				It should be the case that the 'goto Finish' was encountered
				before this point was ever reached. But if we have reached
				this point, then roundoff has prevented u from decreasing
				all the way to zero. This can happen only if the initial u
				was very nearly equal to 1, which is a rare situation. In
				that rare situation, we just try again.

				Note, following the ranlib implementation, we loop ix only to
				a hardcoded value of SMALL_MEAN_LARGE_N=110; we could have
				looped to n, and 99.99...% of the time it won't matter. This
				choice, I think is a little more robust against the rare
				roundoff error. If n>LARGE_N, then it is technically
				possible for ix>LARGE_N, but it is astronomically rare, and
				if ix is that large, it is more likely due to roundoff than
				probability, so better to nip it at LARGE_N than to take a
				chance that roundoff will somehow conspire to produce an even
				larger (and more improbable) ix. If n<LARGE_N, then once
				ix=n, f=0, and the loop will continue until ix=LARGE_N.
			*/
		}
	} else {
		
		/* 
			For n >= SMALL_MEAN, we invoke the BTPE algorithm
		*/

		double ffm = np + p;		// ffm = n*p+p
		integer m = (integer) ffm;	// m = int floor [n*p+p]
		double fm = m;				// fm = double m
		double xm = fm + 0.5;	 	// xm = half integer mean (tip of triangle)
		double npq = np * q;		// npq = n*p*q

		/* 
			Compute cumulative area of tri, para, exp tails

			p1: radius of triangle region; since height=1, also: area of region
			p2: p1 + area of parallelogram region
			p3: p2 + area of left tail
			p4: p3 + area of right tail
			pi/p4: probability of i'th area (i=1,2,3,4)

			Note: magic numbers 2.195, 4.6, 0.134, 20.5, 15.3
			These magic numbers are not adjustable...at least not easily!
		*/
		
		double p1 = Melder_roundDown (2.195 * sqrt (npq) - 4.6 * q) + 0.5;

		// xl, xr: left and right edges of triangle
		double xl = xm - p1;
		double xr = xm + p1;

		/* 
			Parameter of exponential tails
			Left tail:  t(x) = c*exp(-lambda_l* [xl - (x+0.5)])
			Right tail: t(x) = c*exp(-lambda_r* [(x+0.5) - xr])
		*/

		double c = 0.134 + 20.5 / (15.3 + fm);
		double p2 = p1 * (1.0 + c + c);

		double al = (ffm - xl) / (ffm - xl * p);
		double lambda_l = al * (1.0 + 0.5 * al);
		double ar = (xr - ffm) / (xr * q);
		double lambda_r = ar * (1.0 + 0.5 * ar);
		double p3 = p2 + c / lambda_l;
		double p4 = p3 + c / lambda_r;
		double var, accept;
		double u, v; /* random variates */

TryAgain:

		/*
			Generate random variates, u specifies which region: Tri, Par, Tail
		*/
		
		u = p4 * NUMrandomUniform (0.0, 1.0); // djmw gsl_rng_uniform (rng) * p4;
		v = NUMrandomUniform (0.0, 1.0); // djmw gsl_rng_uniform (rng);

		if (u <= p1) {
			// Triangular region
			ix = (integer) (xm - p1 * v + u);
			goto Finish;
		} else if (u <= p2) {
			// Parallelogram region
			double x = xl + (u - p1) / c;
			v = v * c + 1.0 - fabs (x - xm) / p1;
			if (v > 1.0 || v <= 0.0) {
				goto TryAgain;
			}
			ix = (integer) x;
		} else if (u <= p3) {
			// Left tail
			ix = (integer) (xl + log (v) / lambda_l);
			if (ix < 0) {
				goto TryAgain;
			}
			v *= ((u - p2) * lambda_l);
		} else {
			// Right tail
			ix = (integer) (xr - log (v) / lambda_r);
			if (ix > (double) n) {
				goto TryAgain;
			}
			v *= ((u - p3) * lambda_r);
		}

		/* 
			At this point, the goal is to test whether v <= f(x)/f(m)
			v <= f(x)/f(m) = (m!(n-m)! / (x!(n-x)!)) * (p/q)^{x-m}

			Here is a direct test using logarithms. It is a little
			slower than the various "squeezing" computations below, but
			if things are working, it should give exactly the same answer
			(given the same random number seed).
		*/

		#ifdef DIRECT
		var = log (v);

		accept = LNFACT (m) + LNFACT (n - m) - LNFACT (ix) - LNFACT (n - ix) + (ix - m) * log (p / q);

		#else // SQUEEZE METHOD

		/* 
			More efficient determination of whether v < f(x)/f(M)
		 */

		integer k = labs (ix - m);

		if (k <= FAR_FROM_MEAN) {
			/*
				If ix near m (ie, |ix-m|<FAR_FROM_MEAN), then do
				explicit evaluation using recursion relation for f(x)
			*/
			double g = (n + 1) * s;
			double f = 1.0;

			var = v;

			if (m < ix) {
				for (integer i = m + 1; i <= ix; i ++) {
					f *= (g / i - s);
				}
			} else if (m > ix) {
				for (integer i = ix + 1; i <= m; i ++) {
					f /= (g / i - s);
				}
			}

			accept = f;
		} else {
			// If ix is far from the mean m: k=ABS(ix-m) large

			var = log (v);

			if (k < npq / 2 - 1) {
				/* 
					"Squeeze" using upper and lower bounds on
					log(f(x)) The squeeze condition was derived
					under the condition k < npq/2-1
				*/
				double amaxp = k / npq * ((k * (k / 3.0 + 0.625) + (1.0 / 6.0)) / npq + 0.5);
				double ynorm = -(k * k / (2.0 * npq));
				if (var < ynorm - amaxp) {
					goto Finish;
				}
				if (var > ynorm + amaxp) {
					goto TryAgain;
				}
			}

			/* 
				Now, again: do the test log(v) vs. log f(x)/f(M)
			*/

			#if USE_EXACT
			/* 
				This is equivalent to the above, but is a little (~20%) slower
				There are five log's vs three above, maybe that's it?
			*/

			accept = LNFACT (m) + LNFACT (n - m) - LNFACT (ix) - LNFACT (n - ix) + (ix - m) * log (p / q);

			#else 
			/* USE STIRLING:
				The "#define Stirling" above corresponds to the first five
				terms in asymptotic formula for
				log Gamma (y) - (y-0.5)log(y) + y - 0.5 log(2*pi);
				See Abramowitz and Stegun, eq 6.1.40


				Note below: two Stirling's are added, and two are
				subtracted. In both K+S, and in the ranlib
				implementation, all four are added. I (jt) believe that
				is a mistake -- this has been confirmed by personal
				correspondence w/ Dr. Kachitvichyanukul. Note, however,
				the corrections are so small, that I couldn't find an
				example where it made a difference that could be
				observed, let alone tested. In fact, define'ing Stirling
				to be zero gave identical results!! In practice, alv is
				O(1), ranging 0 to -10 or so, while the Stirling
				correction is typically O(10^{-5}) ...setting the
				correction to zero gives about a 2% performance boost;
				might as well keep it just to be pedantic.
			*/

			{
				double x1 = ix + 1.0;
				double w1 = n - ix + 1.0;
				double f1 = fm + 1.0;
				double z1 = n + 1.0 - fm;

				accept = xm * log (f1 / x1) + (n - m + 0.5) * log (z1 / w1) + (ix - m) * log (w1 * p / (x1 * q))
					+ Stirling (f1) + Stirling (z1) - Stirling (x1) - Stirling (w1);
			}
			#endif
			#endif
		}


		if (var <= accept) {
			goto Finish;
		} else {
			goto TryAgain;
		}
	}

Finish:

	return (flipped) ? (n - ix) : ix;
}

double NUMrandomBinomial_real (double p, integer n) {
	if (p < 0.0 || p > 1.0 || n < 0) {
		return undefined;
	} else {
		return (double) NUMrandomBinomial (p, n);
	}
}

void NUMlngamma_complex (double zr, double zi, double *lnr, double *arg) {
	double ln_re = undefined, ln_arg = undefined;
	gsl_sf_result gsl_lnr, gsl_arg;
	if (gsl_sf_lngamma_complex_e (zr, zi, & gsl_lnr, & gsl_arg)) {
		ln_re = gsl_lnr.val;
		ln_arg = gsl_arg.val;
	}
	if (lnr) {
		*lnr = ln_re;
	}
	if (arg) {
		*arg = ln_arg;
	}
}

autoVEC NUMbiharmonic2DSplineInterpolation_getWeights (constVEC x, constVEC y, constVEC z) {
	Melder_assert (x.size == y.size && x.size == z.size);
	autoMAT g (x.size, x.size, kTensorInitializationType :: RAW);
	/*
		1. Calculate the Green matrix G = |point [i]-point [j]|^2 (ln (|point [i]-point [j]|) - 1.0)
		2. Solve z = G.w for w
	*/
	for (integer i = 1; i <= x.size; i ++) {
		for (integer j = i + 1; j <= x.size; j ++) {
			double dx = x [i] - x [j], dy = y [i] - y [j];
			double distanceSquared = dx * dx + dy * dy;
			g [i] [j] = g [j] [i] = distanceSquared * (0.5 * log (distanceSquared) - 1.0); // Green's function
		}
		g [i] [i] = 0.0;
	}
	autoVEC w = NUMsolveEquation (g.get(), z, 0.0);
	return w;
}

double NUMbiharmonic2DSplineInterpolation (constVEC x, constVEC y, constVEC w, double xp, double yp) {
	Melder_assert (x.size == y.size && x.size == w.size);
	longdouble result = 0.0;
	for (integer i = 1; i <= x.size; i ++) {
		double dx = xp - x [i], dy = yp - y [i];
		double d = dx * dx + dy * dy;
		result += w [i] * d * (0.5 * log (d) - 1.0);
	}
	return (double) result;
}

void NUMfixIndicesInRange (integer lowerLimit, integer upperLimit, integer *lowIndex, integer *highIndex) {
	Melder_require (lowerLimit <= upperLimit, U"The lower limit should not exceed the upper limit.");
	if (*highIndex < *lowIndex) {
		*lowIndex = lowerLimit; *highIndex = upperLimit;
	} else if (*highIndex == *lowIndex) {
		Melder_require (*lowIndex >= lowerLimit && *highIndex <= upperLimit, U"Both lower and upper indices are out of range.");
	} else { // low < high
		Melder_require (*lowIndex < upperLimit && *highIndex > lowerLimit, U"Both lower and upper indices are out of range.");
		if (*lowIndex < lowerLimit) {
			*lowIndex = lowerLimit;
		}
		if (*highIndex > upperLimit) {
			*highIndex = upperLimit;
		}
	}
}

void MAT_getEntropies (constMAT m, double *out_h, double *out_hx, 
	double *out_hy,	double *out_hygx, double *out_hxgy, double *out_uygx, double *out_uxgy, double *out_uxy) {
	
	double h = undefined, hx = undefined, hy = undefined;
	double hxgy = undefined, hygx = undefined, uygx = undefined, uxgy = undefined, uxy = undefined;
	
	// Get total sum and check if all elements are not negative.
	
	longdouble totalSum = 0.0;
	for (integer i = 1; i <= m.nrow; i ++) {
		for (integer j = 1; j <= m.ncol; j++) {
			Melder_require (m [i][j] >= 0, U"Matrix elements should not be negative.");
			totalSum += m [i] [j];
		}
	}
	
	if (totalSum > 0.0) {
		longdouble hy_t = 0.0;
		for (integer i = 1; i <= m.nrow; i ++) {
			longdouble rowsum = 0.0;
			for (integer j = 1; j <= m.ncol; j++) rowsum += m [i] [j];
			if (rowsum > 0.0) {
				longdouble p = rowsum / totalSum;
				hy_t -= p * NUMlog2 (p);
			}
		}
		hy = (double) hy_t;
		
		longdouble hx_t = 0.0;
		for (integer j = 1; j <= m.ncol; j ++) {
			longdouble colsum = 0.0;
			for (integer i = 1; i <= m.nrow; i++) colsum += m [i] [j];
			if (colsum > 0.0) {
				longdouble p = colsum / totalSum;
				hx_t -= p * NUMlog2 (p);
			}
		}
		hx = (double) hx_t;
				
		// Total entropy
		longdouble h_t = 0.0;
		for (integer i = 1; i <= m.nrow; i ++) {
			for (integer j = 1; j <= m.ncol; j ++) {
				if (m [i] [j] > 0.0) {
					double p = m [i] [j] / totalSum;
					h_t -= p * NUMlog2 (p);
				}
			}
		}
		h = (double) h_t;
		hygx = h - hx;
		hxgy = h - hy;
		uygx = (hy - hygx) / hy;
		uxgy = (hx - hxgy) / hx;
		uxy = 2.0 * (hx + hy - h) / (hx + hy);
	}
	if (out_h) *out_h = h;
	if (out_hx) *out_hx = hx;
	if (out_hy) *out_hy = hy;
	// Conditional entropies
	if (out_hygx) *out_hygx = hygx;
	if (*out_hxgy) *out_hxgy = hxgy;
	if (*out_uygx) *out_uygx = uygx;
	if (*out_uxgy) *out_uxgy = uxgy;
	if (*out_uxy) *out_uxy = uxy;
}
#undef TINY

double NUMfrobeniusnorm (constMAT x) {
	longdouble scale = 0.0;
	longdouble ssq = 1.0;
	for (integer i = 1; i <= x.nrow; i ++) {
		for (integer j = 1; j <= x.ncol; j ++) {
			if (x [i] [j] != 0.0) {
				longdouble absxi = fabs (x [i] [j]);
				if (scale < absxi) {
					longdouble t = scale / absxi;
					ssq = 1 + ssq * t * t;
					scale = absxi;
				} else {
					longdouble t = absxi / scale;
					ssq += t * t;
				}
			}
		}
	}
	return scale * sqrt ((double) ssq);
}

double NUMtrace (constMAT a) {
	Melder_assert (a.nrow == a.ncol);
	longdouble trace = 0.0;
	for (integer i = 1; i <= a.nrow; i ++) {
		trace += a [i] [i];
	}
	return (double) trace;
}

double NUMtrace2_nn (constMAT x, constMAT y) {
	Melder_assert (x.ncol == y.nrow && x.nrow == y.ncol);
	longdouble trace = 0.0;
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer k = 1; k <= x.ncol; k ++) {
			trace += x [irow] [k] * y [k] [irow];
		}
	}
	return (double) trace;
}

double NUMtrace2_tn (constMAT x, constMAT y) {
	Melder_assert (x.ncol == y.ncol && x.nrow == y.nrow);
	longdouble trace = 0.0;
	for (integer irow = 1; irow <= x.ncol; irow ++) {
		for (integer k = 1; k <= x.nrow; k ++) {
			trace += x [k] [irow] * y [k] [irow];
		}
	}
	return (double) trace;
}

double NUMtrace2_nt (constMAT x, constMAT y) {
	return NUMtrace2_tn (y, x);
}

double NUMtrace2_tt (constMAT x, constMAT y) {
	return NUMtrace2_nn (y, x);
}

void NUMeigencmp22 (double a, double b, double c, double *out_rt1, double *out_rt2, double *out_cs1, double *out_sn1) {
	longdouble sm = a + c, df = a - c, adf = fabs (df);
	longdouble tb = b + b, ab = fabs (tb);
	longdouble acmx = c, acmn = a;
	if (fabs (a) > fabs (c)) {
		acmx = a;
		acmn = c;
	}
	longdouble rt, tn;
	if (adf > ab) {
		tn = ab / adf;
		rt = adf * sqrt (1.0 + tn * tn);
	} else if (adf < ab) {
		tn = adf / ab;
		rt = ab * sqrt (1.0 + tn * tn);
	} else {
		rt = ab * sqrt (2.0);
	}
	
	longdouble rt1, rt2;
	integer sgn1, sgn2;
	if (sm < 0) {
		rt1 = 0.5 * (sm - rt);
		sgn1 = -1;
		/*
			Order of execution important.
			To get fully accurate smaller eigenvalue,
			next line needs to be executed in higher precision.
		*/
		rt2 = (acmx / rt1) * acmn - (b / rt1) * b;
	} else if (sm > 0) {
		rt1 = 0.5 * (sm + rt);
		sgn1 = 1;
		/*
			Order of execution important.
			To get fully accurate smaller eigenvalue,
			next line needs to be executed in higher precision.
		*/
		rt2 = (acmx / rt1) * acmn - (b / rt1) * b;
	} else {
		rt1 = 0.5 * rt;
		rt2 = -0.5 * rt;
		sgn1 = 1;
	}

	// Compute the eigenvector

	longdouble cs;
	if (df >= 0) {
		cs = df + rt;
		sgn2 = 1;
	} else {
		cs = df - rt;
		sgn2 = -1;
	}
	longdouble acs = fabs (cs), cs1, sn1;
	if (acs > ab) {
		longdouble ct = -tb / cs;
		sn1 = 1 / sqrt (1 + ct * ct);
		cs1 = ct * sn1;
	} else {
		if (ab == 0) {
			cs1 = 1;
			sn1 = 0;
		} else {
			tn = -cs / tb;
			cs1 = 1 / sqrt (1 + tn * tn);
			sn1 = tn * cs1;
		}
	}
	if (sgn1 == sgn2) {
		tn = cs1;
		cs1 = -sn1;
		sn1 = tn;
	}
	if (out_rt1) *out_rt1 = (double) rt1;
	if (out_rt2) *out_rt2 = (double) rt2;
	if (out_cs1) *out_cs1 = (double) cs1;
	if (out_sn1) *out_sn1 = (double) sn1;
}

/* End of file NUM2.cpp */
