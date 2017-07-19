/* NUM2.cpp
 *
 * Copyright (C) 1993-2016 David Weenink
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
 djmw 20040211 Modified NUMstrings_copyElements: if (form[i]==NULL) then {to[i]= NULL}.
 djmw 20040303 Added NUMstring_containsPrintableCharacter.
 djmw 20050406 NUMprocrutus->NUMprocrustes
 djmw 20060319 NUMinverse_cholesky: calculation of determinant is made optional
 djmw 20060517 Added NUMregexp_compile
 djmw 20060518 Treat NULL string as empty string in strs_replace_regexp/literal. Don't accept empty search in str_replace_regexp
 djmw 20060626 Extra NULL argument for ExecRE.
 djmw 20061021 printf expects %ld for 'long int' for 64-bit systems
 djmw 20070302 NUMclipLineWithinRectangle
 djmw 20070614 updated to version 1.30 of regular expressions.
 djmw 20071022 Removed function NUMfvector_moment2.
 djmw 20071201 Melder_warning<n>
 djmw 20080107 Changed assertion to "npoints > 0" in NUMcosinesTable
 djmw 20080110 Corrected some bugs in str_replace_regexp
 djmw 20080122 Bug in str_replace_regexp
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
 djmw 20140318 NUMvector_avevar now returns variance instead of sigma^2
*/

#include "SVD.h"
#include "Eigen.h"
#include "NUMclapack.h"
#ifndef _NUM_h_
	#include "NUM.h"
#endif
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

#define my me ->

#undef MAX
#undef MIN

#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))
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

void NUMdmatrix_printMatlabForm (double **m, long nr, long nc, const char32 *name) {
	long npc = 5;
	ldiv_t n = ldiv (nc, npc);

	MelderInfo_open ();
	MelderInfo_write (name, U"=[");
	for (long i = 1; i <= nr; i++) {
		for (long j = 1; j <= n.quot; j++) {
			for (long k = 1; k <= npc; k++) {
				MelderInfo_write (m[i][ (j - 1) *npc + k], (k < npc ? U", " : U""));
			}
			MelderInfo_write (j < n.quot ? U",\n" : U"");
		}

		for (long k = 1; k <= n.rem; k++) {
			MelderInfo_write (m[i][n.quot * npc + k], (k < n.rem ? U", " : U""));
		}
		MelderInfo_write (i < nr ? U";\n" : U"];\n");
	}
	MelderInfo_close ();
}

void NUMcentreRows (double **a, long rb, long re, long cb, long ce) {
	for (long i = rb; i <= re; i++) {
		double rowmean = 0.0;
		for (long j = cb; j <= ce; j++) {
			rowmean += a[i][j];
		}
		rowmean /= (ce - cb + 1);
		for (long j = cb; j <= ce; j++) {
			a[i][j] -= rowmean;
		}
	}
}

void NUMcentreColumns (double **a, long rb, long re, long cb, long ce, double *centres) {
	for (long j = cb; j <= ce; j++) {
		double colmean = 0.0;
		for (long i = rb; i <= re; i++) {
			colmean += a[i][j];
		}
		colmean /= (re - rb + 1);
		for (long i = rb; i <= re; i++) {
			a[i][j] -= colmean;
		}
		if (centres) {
			centres[j - cb + 1] = colmean;
		}
	}
}

void NUMdoubleCentre (double **a, long rb, long re, long cb, long ce) {
	NUMcentreRows (a, rb, re, cb, ce);
	NUMcentreColumns (a, rb, re, cb, ce, NULL);
}

void NUMnormalizeColumns (double **a, long nr, long nc, double norm) {
	Melder_assert (norm > 0);
	for (long j = 1; j <= nc; j++) {
		double s = 0.0;
		for (long i = 1; i <= nr; i++) {
			s += a[i][j] * a[i][j];
		}
		if (s <= 0.0) {
			continue;
		}
		s = sqrt (norm / s);
		for (long i = 1; i <= nr; i++) {
			a[i][j] *= s;
		}
	}
}

void NUMnormalizeRows (double **a, long nr, long nc, double norm) {
	Melder_assert (norm > 0);
	for (long i = 1; i <= nr; i++) {
		double s = 0.0;
		for (long j = 1; j <= nc; j++) {
			s += a[i][j] * a[i][j];
		}
		if (s <= 0.0) {
			continue;
		}
		s = sqrt (norm / s);
		for (long j = 1; j <= nc; j++) {
			a[i][j] *= s;
		}
	}
}

void NUMnormalize (double **a, long nr, long nc, double norm) {
	Melder_assert (norm > 0);
	double sq = 0.0;
	for (long i = 1; i <= nr; i++) {
		for (long j = 1; j <= nc; j++) {
			sq += a[i][j] * a[i][j];
		}
	}
	if (sq <= 0.0) {
		return;
	}
	norm = sqrt (norm / sq);
	for (long i = 1; i <= nr; i++) {
		for (long j = 1; j <= nc; j++) {
			a[i][j] *= norm;
		}
	}
}

void NUMstandardizeColumns (double **a, long rb, long re, long cb, long ce) {
	long n = re - rb + 1;
	if (n < 2) {
		return;
	}
	for (long j = cb; j <= ce; j++) {
		double ep = 0.0, s = 0.0, sdev, var = 0.0;
		for (long i = rb; i <= re; i++) {
			s += a[i][j];
		}
		double ave = s / n;
		for (long i = rb; i <= re; i++) {
			s = a[i][j] - ave;
			ep += s;
			var += s * s;
		}
		if (ave != 0.0) {
			for (long i = rb; i <= re; i++) {
				a[i][j] -= ave;
			}
		}
		if (var > 0.0) {
			var = (var - ep * ep / n) / (n - 1);
			sdev = sqrt (var);
			for (long i = rb; i <= re; i++) {
				a[i][j] /= sdev;
			}
		}
	}
}

void NUMstandardizeRows (double **a, long rb, long re, long cb, long ce) {
	long n = ce - cb + 1;
	if (n < 2) {
		return;
	}
	for (long i = rb; i <= re; i++) {
		double ep = 0.0, s = 0.0, sdev, var = 0.0;
		for (long j = cb; j <= ce; j++) {
			s += a[i][j];
		}
		double ave = s / n;
		for (long j = cb; j <= ce; j++) {
			s = a[i][j] - ave;
			ep += s;
			var += s * s;
		}
		if (ave != 0.0) {
			for (long j = cb; j <= ce; j++) {
				a[i][j] -= ave;
			}
		}
		if (var > 0.0) {
			var = (var - ep * ep / n) / (n - 1);
			sdev = sqrt (var);
			for (long j = cb; j <= ce; j++) {
				a[i][j] /= sdev;
			}
		}
	}
}

void NUMaverageColumns (double **a, long rb, long re, long cb, long ce) {
	long n = re - rb + 1;
	if (n < 2) {
		return;
	}
	for (long j = cb; j <= ce; j++) {
		double ave = 0.0;
		for (long i = rb; i <= re; i++) {
			ave += a[i][j];
		}
		ave /= n;
		for (long i = rb; i <= re; i++) {
			a[i][j] = ave;
		}
	}

}

void NUMvector_avevar (double *a, long n, double *p_mean, double *p_var) {

	double mean = 0.0;
	for (long i = 1; i <= n; i++) {
		mean += a[i];
	}

	mean /= n;

	if (p_mean) {
		*p_mean = mean;
	}

	if (p_var) {
		double eps = 0.0, var = 0.0;
		if (n > 1) {
			for (long i = 1; i <= n; i++) {
				double s = a[i] - mean;
				eps += s;
				var += s * s;
			}

			var = (var - eps * eps / n);
		} else {
			var = NUMundefined;
		}
		*p_var = var;
	}
}

void NUMcolumn_avevar (double **a, long nr, long nc, long icol, double *p_mean, double *p_var) {

	Melder_assert (nr > 0 && nc > 0 && icol > 0 && icol <= nc);

	double mean = 0.0;
	for (long i = 1; i <= nr; i++) {
		mean += a[i][icol];
	}

	mean /= nr;

	if (p_mean) {
		*p_mean = mean;
	}

	if (p_var) {
		double eps = 0.0, var = 0.0;
		if (nr > 1) {
			for (long i = 1; i <= nr; i++) {
				double s = a[i][icol] - mean;
				eps += s;
				var += s * s;
			}

			var = (var - eps * eps / nr);
		} else {
			var = NUMundefined;
		}
		*p_var = var;
	}
}

void NUMcolumn2_avevar (double **a, long nr, long nc, long icol1, long icol2, double *p_mean1, double *p_var1, double *p_mean2, double *p_var2, double *p_covar) {

	Melder_assert (icol1 > 0 && icol1 <= nc && icol2 > 0 && icol2 <= nc);

	double mean1 = 0.0, mean2 = 0.0;
	for (long i = 1; i <= nr; i++) {
		mean1 += a[i][icol1];
		mean2 += a[i][icol2];
	}

	mean1 /= nr;
	mean2 /= nr;

	if (p_mean1) {
		*p_mean1 = mean1;
	}
	if (p_mean2) {
		*p_mean2 = mean2;
	}

	if (p_var1 || p_var2 || p_covar) {
		double eps1 = 0.0, eps2 = 0.0, var1 = 0.0, var2 = 0.0, covar = 0.0;

		if (nr > 1) {
			for (long i = 1; i <= nr; i++) {
				double s1 = a[i][icol1] - mean1;
				double s2 = a[i][icol2] - mean2;
				eps1 += s1;
				eps2 += s2;
				var1 += s1 * s1;
				var2 += s2 * s2;
				covar += s1 * s2;
			}

			var1 = (var1 - eps1 * eps1 / nr);
			var2 = (var2 - eps2 * eps2 / nr);;
		} else {
			var1 = NUMundefined;
			var2 = NUMundefined;
			covar = NUMundefined;
		}
		if (p_var1) {
			*p_var1 = var1;
		}
		if (p_var2) {
			*p_var2 = var2;
		}
		if (icol1 == icol2) {
			covar = var1;
		}
		if (p_covar) {
			*p_covar = covar;
		}
	}
}

void NUMvector_smoothByMovingAverage (double *xin, long n, long nwindow, double *xout) {
// simple averaging, out of bound values are zero
	for (long i = 1; i <= n; i++) {
		long jfrom = i - nwindow / 2, jto = i + nwindow / 2;
		if ((nwindow % 2) == 0) {
			jto--;
		}
		jfrom = jfrom < 1 ? 1 : jfrom;
		jto = jto > n ? n : jto;
		xout[i] = 0;
		for (long j = jfrom; j <= jto; j++) {
			xout[i] += xin[j];
		}
		xout[i] /= jto - jfrom + 1;
	}
}

void NUMcovarianceFromColumnCentredMatrix (double **x, long nrows, long ncols, long ndf, double **covar) {
	if (ndf < 0 || nrows - ndf < 1 || covar == 0) {
		Melder_throw (U"Invalid arguments.");
	}
	for (long i = 1; i <= ncols; i++) {
		for (long j = i; j <= ncols; j++) {
			double sum = 0.0;
			for (long k = 1; k <= nrows; k++) {
				sum += x[k][i] * x[k][j];
			}
			covar[i][j] = covar[j][i] = sum / (nrows - ndf);
		}
	}
}

double NUMmultivariateKurtosis (double **x, long nrows, long ncols, int method) {
	double kurt = NUMundefined;
	if (nrows < 5) {
		return kurt;
	}
	autoNUMvector<double> mean (1, ncols);
	autoNUMmatrix<double> covar (1, ncols, 1, ncols);

	NUMcentreColumns (x, 1, nrows, 1, ncols, mean.peek());
	NUMcovarianceFromColumnCentredMatrix (x, nrows, ncols, 1, covar.peek());
	if (method == 1) { // Schott (2001, page 33)
		kurt = 0.0;
		for (long l = 1; l <= ncols; l++) {
			double zl = 0.0, wl, sll2 = covar[l][l] * covar[l][l];
			for (long j = 1; j <= nrows; j++) {
				double d = x[j][l] - mean[l], d2 = d * d;
				zl += d2 * d2;
			}
			zl = (zl - 6.0 * sll2) / (nrows - 4);
			wl = (sll2 - zl / nrows) * nrows / (nrows - 1);
			kurt += zl / wl;
		}
		kurt = kurt / (3 * ncols) - 1.0;
	}
	return kurt;
}

void eigenSort (double d[], double **v, long n, int sort) {
	if (sort == 0) {
		return;
	}
	for (long i = 1; i < n; i++) {
		long k;
		double temp = d[k = i];
		if (sort > 0) {
			for (long j = i + 1; j <= n; j++) {
				if (d[j] > temp) {
					temp = d[k = j];
				}
			}
		} else {
			for (long j = i + 1; j <= n; j++) {
				if (d[j] < temp) {
					temp = d[k = j];
				}
			}
		}
		if (k != i) {
			d[k] = d[i];
			d[i] = temp;
			if (v) {
				for (long j = 1; j <= n; j++) {
					temp = v[j][i];
					v[j][i] = v[j][k];
					v[j][k] = temp;
				}
			}
		}
	}
}

int NUMstrcmp (const char *s1, const char *s2) {
	if (! s1 || s1[0] == '\0') {
		if (s2 && s2[0] != '\0') {
			return -1;
		} else {
			return 0;
		}
	} else {
		if (! s2) {
			return +1;
		} else {
			return strcmp (s1, s2);
		}
	}
}

void NUMlocate (double *xx, long n, double x, long *index) {
	long ju = n + 1, jm, jl = 0;
	int ascend = xx[n] >= xx[1];

	while (ju - jl > 1) {
		jm = (ju + jl) / 2;
		if ( (x >= xx[jm]) == ascend) {
			jl = jm;
		} else {
			ju = jm;
		}
	}
	if (x == xx[1]) {
		*index = 1;
	} else if (x == xx[n]) {
		*index = n - 1;
	} else {
		*index = jl;
	}
}

/*
	The following algorithm for monotone regession is on the average
	3.5 times faster than
	Kruskal's algorithm for monotone regression (and much simpler).
	Regression is ascending
*/
void NUMmonotoneRegression (const double x[], long n, double xs[]) {
	double xt = NUMundefined; // Only to stop gcc complaining "may be used unitialized"

	for (long i = 1; i <= n; i++) {
		xs[i] = x[i];
	}

	for (long i = 2; i <= n; i++) {
		if (xs[i] >= xs[i - 1]) {
			continue;
		}
		double sum = xs[i];
		long nt = 1;
		for (long j = 1; j <= i - 1; j++) {
			sum += xs[i - j]; nt++;
			xt = sum / nt; // i >= 2 -> xt always gets a value
			if (j < i - 1 && xt >= xs[i - j - 1]) {
				break;
			}
		}
		for (long j = i - nt + 1; j <= i; j++) {
			xs[j] = xt;
		}
	}
}

double NUMvector_getNorm1 (const double v[], long n) {
	double norm = 0;
	for (long i = 1; i <= n; i++) {
		norm += fabs (v[i]);
	}
	return norm;
}

double NUMvector_getNorm2 (const double v[], long n) {
	double norm = 0.0;
	for (long i = 1; i <= n; i++) {
		norm += v[i] * v[i];
	}
	return sqrt (norm);
}

double NUMvector_normalize1 (double v[], long n) {
	double norm = 0.0;
	for (long i = 1; i <= n; i++) {
		norm += fabs (v[i]);
	}
	if (norm > 0.0) {
		for (long i = 1; i <= n; i++) {
			v[i] /= norm;
		}
	}
	return norm;
}

double NUMvector_normalize2 (double v[], long n) {
	double norm = 0;
	for (long i = 1; i <= n; i++) {
		norm += v[i] * v[i];
	}
	norm = sqrt (norm);
	if (norm > 0) {
		for (long i = 1; i <= n; i++) {
			v[i] /= norm;
		}
	}
	return norm;
}

#undef TINY

void NUMcholeskySolve (double **a, long n, double d[], double b[], double x[]) {
	for (long i = 1; i <= n; i++) { /* Solve L.y=b */
		double sum = b[i];
		for (long k = i - 1; k >= 1; k--) {
			sum -= a[i][k] * x[k];
		}
		x[i] = sum / d[i];
	}
	for (long i = n; i >= 1; i--) { /* Solve L^T.x=y */
		double sum = x[i];
		for (long k = i + 1; k <= n; k++) {
			sum -= a[k][i] * x[k];
		}
		x[i] = sum / d[i];
	}
}

void NUMdeterminant_cholesky (double **a, long n, double *p_lnd) {
	// Save the diagonal
	autoNUMvector<double> d (1, n);
	for (long i = 1; i <= n; i++) {
		d[i] = a[i][i];
	}

	//	 Cholesky decomposition in lower, leave upper intact

	char uplo = 'U';
	long lda = n, info;
	NUMlapack_dpotf2 (&uplo, &n, &a[1][1], &lda, &info);
	if (info != 0) {
		Melder_throw (U"Cannot determine Cholesky decomposition.");
	}

	// Determinant from diagonal, restore diagonal

	double lnd = 0.0;
	for (long i = 1; i <= n; i++) {
		lnd += log (a[i][i]);
		a[i][i] = d[i];
	}
	lnd *= 2.0; // because A = L . L' TODO

	// Restore lower from upper */

	for (long i = 1; i < n; i++) {
		for (long j = i + 1; j <= n; j++) {
			a[j][i] = a[i][j];
		}
	}
	if (p_lnd) {
		*p_lnd = lnd;
	}
}

void NUMlowerCholeskyInverse (double **a, long n, double *lnd) {

	char uplo = 'U', diag = 'N';
	long info;

	// Cholesky decomposition in lower, leave upper intact
	// Fortran storage -> use uplo='U' to get 'L'.

	(void) NUMlapack_dpotf2 (&uplo, &n, &a[1][1], &n, &info);
	if (info != 0) {
		Melder_throw (U"dpotf2 fails.");
	}

	// Determinant from diagonal, diagonal is now sqrt (a[i][i]) !

	if (lnd) {
		*lnd = 0.0;
		for (long i = 1; i <= n; i++) {
			*lnd += log (a[i][i]);
		}
		*lnd *= 2.0; /* because A = L . L' */
	}

	// Get the inverse */

	(void) NUMlapack_dtrtri (&uplo, &diag, &n, &a[1][1], &n, &info);
	if (info != 0) {
		Melder_throw (U"dtrtri fails.");
	}
}

double **NUMinverseFromLowerCholesky (double **m, long n) {
	autoNUMmatrix<double> r (1, n, 1, n);
	for (long i = 1; i <= n; i++) {
		for (long j = 1; j <= i; j++) {
			double sum = 0;
			for (long k = i; k <= n; k++) {
				sum += m[k][i] * m[k][j];
			}
			r[i][j] = r[j][i] = sum;
		}
	}
	return r.transfer();
}

double NUMmahalanobisDistance_chi (double **linv, double *v, double *m, long nr, long n) {
	double chisq = 0;
	if (nr == 1) { // 1xn matrix
		for (long j = 1; j <= n; j++) {
			double t = linv[1][j] * (v[j] - m[j]);
			chisq += t * t;
		}
	} else { // nxn matrix
		for (long i = n; i > 0; i--) {
			double t = 0.0;
			for (long j = 1; j <= i; j++) {
				t += linv[i][j] * (v[j] - m[j]);
			}
			chisq += t * t;
		}
	}
	return chisq;
}

double NUMtrace (double **a, long n) {
	double trace = 0.0;
	for (long i = 1; i <= n; i++) {
		trace += a[i][i];
	}
	return trace;
}

double NUMtrace2 (double **a1, double **a2, long n) {
	double trace = 0.0;
	for (long i = 1; i <= n; i++) {
		for (long k = 1; k <= n; k++) {
			trace += a1[i][k] * a2[k][i];
		}
	}
	return trace;
}

void NUMeigensystem (double **a, long n, double **evec, double eval[]) {
	autoEigen me = Thing_new (Eigen);
	Eigen_initFromSymmetricMatrix (me.get(), a, n);
	if (evec) {
		NUMmatrix_copyElements (my eigenvectors, evec, 1, n, 1, n);
	}
	if (eval) {
		NUMvector_copyElements (my eigenvalues, eval, 1, n);
	}
}

void NUMdominantEigenvector (double **mns, long n, double *q, double *p_lambda, double tolerance) {
	autoNUMvector<double> z (1, n);

	double lambda0, lambda = 0.0;
	for (long k = 1; k <= n; k++) {
		for (long l = 1; l <= n; l++) {
			lambda += q[k] * mns[k][l] * q[l];
		}
	}
	if (lambda == 0.0) {
		Melder_throw (U"Zero matrices ??");
	}

	long iter = 0;
	do {
		double znorm2 = 0.0;
		for (long l = 1; l <= n; l++) {
			z[l] = 0.0;
			for (long k = 1; k <= n; k++) {
				z[l] += mns[l][k] * q[k];
			}
		}

		for (long k = 1; k <= n; k++)  {
			znorm2 += z[k] * z[k];
		}
		znorm2 = sqrt (znorm2);

		for (long k = 1; k <= n; k++) {
			q[k] = z[k] / znorm2;
		}

		lambda0 = lambda; 
		
		lambda = 0.0;
		for (long k = 1; k <= n; k++) {
			for (long l = 1; l <= n; l++) {
				lambda += q[k] * mns[k][l] * q[l];
			}
		}

	} while (fabs (lambda - lambda0) > tolerance || ++iter < 30);
	if (p_lambda) {
		*p_lambda = lambda;
	}
}

void NUMprincipalComponents (double **a, long n, long nComponents, double **pc) {
	autoNUMmatrix<double> evec (1, n, 1, n);
	NUMeigensystem (a, n, evec.peek(), NULL);
	for (long i = 1; i <= n; i++) {
		for (long j = 1; j <= nComponents; j++) {
			double s = 0.0;
			for (long k = 1; k <= n; k++) {
				s += a[k][i] * evec[k][j]; /* times sqrt(eigenvalue) ?? */
			}
			pc[i][j] = s;
		}
	}
}

void NUMdmatrix_projectRowsOnEigenspace (double **data, long numberOfRows, long from_col, double **eigenvectors, long numberOfEigenvectors, long dimension, double **projection, long to_col) {
	/* Input:
	 * 	data[numberOfRows, from_col - 1 + my dimension] 
	 * 		contains the 'numberOfRows' vectors to be projected on the eigenspace. 
	 *  eigenvectors [numberOfEigenvectors][dimension] 
	 * 		the eigenvectors stored as rows
	 * Input/Output
	 * 	projection [numberOfRows, to_colbegin - 1 + numberOfEigenvectors] 
	 * 		the projected vectors from 'data'
	 * 
	 * Project (part of) the vectors in matrix 'data' along the 'numberOfEigenvectors' eigenvectors into the matrix 'projection'.
	 */
	from_col = from_col <= 0 ? 1 : from_col;
	to_col = to_col <= 0 ? 1 : to_col;

	for (long irow = 1; irow <= numberOfRows; irow ++) {
		for (long icol = 1; icol <= numberOfEigenvectors; icol ++) {
			double r = 0.0;
			for (long k = 1; k <= dimension; k ++) {
				r += eigenvectors  [icol] [k] * data [irow] [from_col + k - 1];
			}
			projection [irow] [to_col + icol - 1] = r;
		}
	}
}

void NUMdmatrix_projectColumnsOnEigenspace (double **data, long numberOfColumns, double **eigenvectors, long numberOfEigenvectors, long dimension, double **projection) {
	/* Input:
	 * 	data[dimension, numberOfColumns] 
	 * 		contains the column vectors to be projected on the eigenspace. 
	 *  eigenvectors [numberOfEigenvectors][dimension] 
	 * 		the eigenvectors stored as rows
	 * Input/Output
	 * 	projection [numberOfEigenvectors, numberOfColumns] 
	 * 		the projected vectors from 'data'
	 * 
	 * Project the columnvectors in matrix 'data' along the 'numberOfEigenvectors' eigenvectors into the matrix 'projection'.
	 */

	for (long icol = 1; icol <= numberOfColumns; icol++) {
		for (long irow = 1; irow <= numberOfEigenvectors; irow++) {
			double r = 0.0;
			for (long k = 1; k <= dimension; k ++) {
				r += eigenvectors  [irow] [k] * data [k] [icol];
			}
			projection [irow][icol] = r;
		}
	}
}

void NUMdmatrix_into_principalComponents (double **m, long nrows, long ncols, long numberOfComponents, double **pc) {
	Melder_assert (numberOfComponents > 0 && numberOfComponents <= ncols);
	autoNUMmatrix<double> mc (NUMmatrix_copy (m, 1, nrows, 1, ncols), 1, 1);

	/*NUMcentreColumns (mc, nrows, ncols);*/
	autoSVD svd = SVD_create_d (mc.peek(), nrows, ncols);
	for (long i = 1; i <= nrows; i++) {
		for (long j = 1; j <= numberOfComponents; j++) {
			pc[i][j] = 0.0;
			for (long k = 1; k <= ncols; k++) {
				pc[i][j] += svd -> v[k][j] * m[i][k];
			}
		}
	}
}

void NUMpseudoInverse (double **y, long nr, long nc, double **yinv, double tolerance) {
	autoSVD me = SVD_create_d (y, nr, nc);

	(void) SVD_zeroSmallSingularValues (me.get(), tolerance);
	for (long i = 1; i <= nc; i++) {
		for (long j = 1; j <= nr; j++) {
			double s = 0.0;
			for (long k = 1; k <= nc; k++) {
				if (my d[k] != 0.0) {
					s += my v[i][k] * my u[j][k] / my d[k];
				}
			}
			yinv[i][j] = s;
		}
	}
}

long NUMsolveQuadraticEquation (double a, double b, double c, double *x1, double *x2) {
	return gsl_poly_solve_quadratic (a, b, c, x1, x2);
}

void NUMsolveEquation (double **a, long nr, long nc, double *b, double tolerance, double *result) {
	double tol = tolerance > 0 ? tolerance : NUMfpp -> eps * nr;

	if (nr <= 0 || nc <= 0) {
		Melder_throw (U"Negative dimensions");
	}

	autoSVD me = SVD_create_d (a, nr, nc);
	SVD_zeroSmallSingularValues (me.get(), tol);
	SVD_solve (me.get(), b, result);
}


void NUMsolveEquations (double **a, long nr, long nc, double **b, long ncb, double tolerance, double **x) {
	double tol = tolerance > 0 ? tolerance : NUMfpp -> eps * nr;

	if (nr <= 0 || nc <= 0) {
		Melder_throw (U"Negative dimensions");
	}

	autoSVD me = SVD_create_d (a, nr, nc);
	autoNUMvector<double> bt (1, nr + nc);
	double *xt = & bt[nr];

	SVD_zeroSmallSingularValues (me.get(), tol);

	for (long k = 1; k <= ncb; k++) {
		for (long j = 1; j <= nr; j++) {
			bt[j] = b[j][k];
		}

		SVD_solve (me.get(), bt.peek(), xt);

		for (long j = 1; j <= nc; j++) {
			x[j][k] = xt[j];
		}
	}
}


void NUMsolveNonNegativeLeastSquaresRegression (double **m, long nr, long nc, double *d, double tol, long itermax, double *b) {
	double difsq, difsqp = 0.0;

	for (long iter = 1; iter <= itermax; iter++) {

		// Fix all weights except b[j]

		for (long j = 1; j <= nc; j++) {
			double mjr = 0.0, mjmj = 0.0;
			for (long i = 1; i <= nr; i++) {
				double ri = d[i], mij = m[i][j];
				for (long l = 1; l <= nc; l++) {
					if (l != j) {
						ri -= b[l] * m[i][l];
					}
				}
				mjr += mij * ri;
				mjmj += mij * mij;
			}
			b[j] = mjr / mjmj;
			if (b[j] < 0.0) {
				b[j] = 0.0;
			}
		}

		// Calculate t(b) and compare with previous result.

		difsq = 0.0;
		for (long i = 1; i <= nr; i++) {
			double dmb = d[i];
			for (long j = 1; j <= nc; j++) {
				dmb -= m[i][j] * b[j];
			}
			difsq += dmb * dmb;
		}
		if (fabs (difsq - difsqp) / difsq < tol) {
			break;
		}
		difsqp = difsq;
	}
}

struct nr_struct {
	double *y, *delta;
};

/*
	f (lambda) = sum (y[i]^2 delta[i] / (delta[i]-lambda)^2, i=1..3)
	f'(lambda) = 2 * sum (y[i]^2 delta[i] / (delta[i]-lambda)^3, i=1..3)
*/

static void nr_func (double x, double *f, double *df, void *data) {
	struct nr_struct *me = (struct nr_struct *) data;
	*f = *df = 0.0;
	for (long i = 1; i <= 3; i++) {
		double t1 = (my delta[i] - x);
		double t2 = my y[i] / t1;
		double t3 = t2 * t2 * my delta[i];
		*f  += t3;
		*df += t3 * 2 / t1;
	}
}

void NUMsolveConstrainedLSQuadraticRegression (double **o, const double d[], long n, double *alpha, double *gamma) {
	long n3 = 3, info;
	double eps = 1e-5, t1, t2, t3;

	autoNUMmatrix<double> ftinv (1, n3, 1, n3);
	autoNUMmatrix<double> b (1, n3, 1, n3);
	autoNUMmatrix<double> g (1, n3, 1, n3);
	autoNUMmatrix<double> p (1, n3, 1, n3);
	autoNUMvector<double> delta (1, n3);
	autoNUMmatrix<double> ftinvp (1, n3, 1, n3);
	autoNUMmatrix<double> ptfinv (1, n3, 1, n3);
	autoNUMvector<double> otd (1, n3);
	autoNUMmatrix<double> ptfinvc (1, n3, 1, n3);
	autoNUMvector<double> y (1, n3);
	autoNUMvector<double> w (1, n3);
	autoNUMvector<double> chi (1, n3);
	autoNUMvector<double> diag (1, n3);

	// Construct O'.O     [1..3][1..3].

	for (long i = 1; i <= n3; i++) {
		for (long j = 1; j <= n3; j++) {
			for (long k = 1; k <= n; k++) {
				ftinv[i][j] += o[k][i] * o[k][j];
			}
		}
	}

	// Get lower triangular decomposition from O'.O and
	// get F'^-1 from it (eq. (2)) (F^-1 not done ????)

	char uplo = 'U';
	(void) NUMlapack_dpotf2 (&uplo, &n3, &ftinv[1][1], &n3, &info);
	if (info != 0) {
		Melder_throw (U"dpotf2 fails.");
	}
	ftinv[1][2] = ftinv[1][3] = ftinv[2][3] = 0.0;

	// Construct G and its eigen-decomposition (eq. (4,5))
	// Sort eigenvalues (& eigenvectors) ascending.

	b[3][1] = b[1][3] = -0.5; b[2][2] = 1.0;

	// G = F^-1 B (F')^-1 (eq. 4)

	for (long i = 1; i <= 3; i++) {
		for (long j = 1; j <= 3; j++) {
			for (long k = 1; k <= 3; k++) {
				if (ftinv[k][i] != 0.0) {
					for (long l = 1; l <= 3; l++) {
						g[i][j] += ftinv[k][i] * b[k][l] * ftinv[l][j];
					}
				}
			}
		}
	}

	// G's eigen-decomposition with eigenvalues (assumed ascending). (eq. 5)

	NUMeigensystem (g.peek(), 3, p.peek(), delta.peek());

	NUMsort_d (3, delta.peek()); /* ascending */

	// Construct y = P'.F'.O'.d ==> Solve (F')^-1 . P .y = (O'.d)    (page 632)
	// Get P'F^-1 from the transpose of (F')^-1 . P

	for (long i = 1; i <= 3; i++) {
		for (long j = 1; j <= 3; j++) {
			if (ftinv[i][j] != 0.0) {
				for (long k = 1; k <= 3; k++) {
					ftinvp[i][k] += ftinv[i][j] * p[3 + 1 - j][k]; /* is sorted desc. */
				}
			}
		}
		for (long k = 1; k <= n; k++) {
			otd[i] += o[k][i] * d[k];
		}
	}

	for (long i = 1; i <= 3; i++) {
		for (long j = 1; j <= 3; j++) {
			ptfinvc[j][i] = ptfinv[j][i] = ftinvp[i][j];
		}
	}

	NUMsolveEquation (ftinvp.peek(), 3, 3, otd.peek(), 1e-6, y.peek());

	// The solution (3 cases)

	if (fabs (y[1]) < eps) {
		// Case 1: page 633

		t2 = y[2] / (delta[2] - delta[1]);
		t3 = y[3] / (delta[3] - delta[1]);
		/* +- */
		w[1] = sqrt (- delta[1] * (t2 * t2 * delta[2] + t3 * t3 * delta[3]));
		w[2] = t2 * delta[2];
		w[3] = t3 * delta[3];

		NUMsolveEquation (ptfinv.peek(), 3, 3, w.peek(), 1e-6, chi.peek());

		w[1] = -w[1];
		if (fabs (chi[3] / chi[1]) < eps) {
			NUMsolveEquation (ptfinvc.peek(), 3, 3, w.peek(), 1e-6, chi.peek());
		}
	} else if (fabs (y[2]) < eps) {
		// Case 2: page 633

		t1 = y[1] / (delta[1] - delta[2]);
		t3 = y[3] / (delta[3] - delta[2]);
		w[1] = t1 * delta[1];
		if ( (delta[2] < delta[3] && (t2 = (t1 * t1 * delta[1] + t3 * t3 * delta[3])) < eps)) {
			w[2] = sqrt (- delta[2] * t2); /* +- */
			w[3] = t3 * delta[3];
			NUMsolveEquation (ptfinv.peek(), 3, 3, w.peek(), 1e-6, chi.peek());
			w[2] = -w[2];
			if (fabs (chi[3] / chi[1]) < eps) {
				NUMsolveEquation (ptfinvc.peek(), 3, 3, w.peek(), 1e-6, chi.peek());
			}
		} else if (((delta[2] < delta[3] + eps) || (delta[2] > delta[3] - eps)) && fabs (y[3]) < eps) {
			// choose one value for w[2] from an infinite number

			w[2] = w[1];
			w[3] = sqrt (- t1 * t1 * delta[1] * delta[2] - w[2] * w[2]);
			NUMsolveEquation (ptfinv.peek(), 3, 3, w.peek(), 1e-6, chi.peek());
		}
	} else {
		// Case 3: page 634 use Newton-Raphson root finder

		struct nr_struct me;
		double xlambda, eps2 = (delta[2] - delta[1]) * 1e-6;

		me.y = y.peek(); me.delta = delta.peek();

		NUMnrbis (nr_func, delta[1] + eps, delta[2] - eps2, & me, & xlambda);

		for (long i = 1; i <= 3; i++) {
			w[i] = y[i] / (1 - xlambda / delta[i]);
		}
		NUMsolveEquation (ptfinv.peek(), 3, 3, w.peek(), 1e-6, chi.peek());
	}

	*alpha = chi[1]; *gamma = chi[3];
}

/*
	f (b) = delta - b / (2 alpha) - sum (x[i]^2 / (c[i] - b)^2, i=1..n)
	f'(b) = - 1 / (2 alpha) + 2 * sum (x[i]^2 / (c[i] - b)^3, i=1..n)
*/
struct nr2_struct {
	long m;
	double delta, alpha, *x, *c;
};

static void nr2_func (double b, double *f, double *df, void *data) {
	struct nr2_struct *me = (struct nr2_struct *) data;

	*df = - 0.5 / my alpha;
	*f = my delta + *df * b;
	for (long i = 1; i <= my m; i++) {
		double c1 = (my c[i] - b);
		double c2 = my x[i] / c1;
		double c2sq = c2 * c2;
		*f -= c2sq; *df += 2 * c2sq / c1;
	}
}

void NUMsolveWeaklyConstrainedLinearRegression (double **f, long n, long m, double phi[], double alpha, double delta, double t[]) {
	autoNUMmatrix<double> u (1, m, 1, m);
	autoNUMvector<double> c (1, m);
	autoNUMvector<double> x (1, n);
	autoNUMvector<long> indx (1, m);

	for (long j = 1; j <= m; j++) {
		t[j] = 0.0;
	}

	autoSVD svd = SVD_create_d (f, n, m);

	if (alpha == 0.0) {
		SVD_solve (svd.get(), phi, t);    // standard least squares
	}


	// Step 1: Compute U and C from the eigendecomposition F'F = UCU'
	// Evaluate q, the multiplicity of the smallest eigenvalue in C


	double *sqrtc = svd -> d;
	double **ut = svd -> v;
	NUMindexx (sqrtc, m, indx.peek());

	for (long j = m; j > 0; j--) {
		double tmp = sqrtc [indx[j]];
		c[m - j + 1] = tmp * tmp;
		for (long k = 1; k <= m; k++) {
			u[m - j + 1][k] = ut [indx[j]] [k];
		}
	}

	long q = 1;
	double tol = 1e-6;
	while (q < m && (c[m - q] - c[m]) < tol) {
		q++;
	}

	// step 2: x = U'F'phi

	for (long i = 1; i <= m; i++) {
		for (long j = 1; j <= m; j++) {
			for (long k = 1; k <= n; k++) {
				x[i] += u[j][i] * f[k][j] * phi[k];
			}
		}
	}

	// step 3:

	struct nr2_struct me;
	me.m = m;
	me.delta = delta;
	me.alpha = alpha;
	me.x = x.peek();
	me.c = c.peek();

	double xqsq = 0.0;
	for (long j = m - q + 1; j <= m; j++) {
		xqsq += x[j] * x[j];
	}

	long r = m;
	if (xqsq < tol) { /* xqsq == 0 */
		double fm, df;
		r = m - q;
		me.m = r;
		nr2_func (c[m], &fm, &df, & me);
		if (fm >= 0.0) { /* step 3.b1 */
			x[r + 1] = sqrt (fm);
			for (long j = 1; j <= r; j++) {
				x[j] /= c[j] - c[m];
			}
			for (long j = 1; j <= r + 1; j++) {
				for (long k = 1; k <= r + 1; k++) {
					t[j] += u[j][k] * x[k];
				}
			}
			return;
		}
		// else continue with r = m - q
	}

	// step 3a & 3b2, determine interval lower bound for Newton-Raphson root finder

	double xCx = 0.0;
	for (long j = 1; j <= r; j++) {
		xCx += x[j] * x[j] / c[j];
	}
	double b0, bmin = delta > 0.0 ? - xCx / delta : -2.0 * sqrt (alpha * xCx);
	double eps = (c[m] - bmin) * tol;

	// find the root of d(psi(b)/db in interval (bmin, c[m])

	NUMnrbis (nr2_func, bmin + eps, c[m] - eps, & me, & b0);

	for (long j = 1; j <= r; j++) {
		for (long k = 1; k <= r; k++) {
			t[j] += u[j][k] * x[k] / (c[k] - b0);
		}
	}
}

void NUMProcrustes (double **x, double **y, long nPoints, long nDimensions, double **t, double v[], double *s) {
	bool orthogonal = ! v || ! s; // else similarity transform

	autoNUMmatrix<double> c (1, nDimensions, 1, nDimensions);
	autoNUMmatrix<double> yc (1, nPoints, 1, nDimensions);
	NUMmatrix_copyElements (y, yc.peek(), 1, nPoints, 1, nDimensions);

	/*
		Reference: Borg & Groenen (1997), Modern multidimensional scaling,
		Springer
		1. Calculate C = X'JY (page 346) for similarity transform
			else X'Y for othogonal (page 341)
			JY amounts to centering the columns of Y.
	*/

	if (! orthogonal) {
		NUMcentreColumns (yc.peek(), 1, nPoints, 1, nDimensions, NULL);
	}
	for (long i = 1; i <= nDimensions; i++) {
		for (long j = 1; j <= nDimensions; j++) {
			for (long k = 1; k <= nPoints; k++) {
				c[i][j] += x[k][i] * yc[k][j];
			}
		}
	}

	// 2. Decompose C by SVD:  C = PDQ' (SVD attribute is Q instead of Q'!)

	autoSVD svd = SVD_create_d (c.peek(), nDimensions, nDimensions);
	double trace = 0.0;
	for (long i = 1; i <= nDimensions; i++) {
		trace += svd -> d[i];
	}

	if (trace == 0.0) {
		Melder_throw (U"NUMProcrustes: degenerate configuration(s).");
	}

	// 3. T = QP'

	for (long i = 1; i <= nDimensions; i++) {
		for (long j = 1; j <= nDimensions; j++) {
			t[i][j] = 0.0;
			for (long k = 1; k <= nDimensions; k++) {
				t[i][j] += svd -> v[i][k] * svd -> u[j][k];
			}
		}
	}

	if (! orthogonal) {
		autoNUMmatrix<double> xc (1, nPoints, 1, nDimensions);
		NUMmatrix_copyElements (x, xc.peek(), 1, nPoints, 1, nDimensions);
		autoNUMmatrix<double> yt (1, nPoints, 1, nDimensions);

		// 4. Dilation factor s = (tr X'JYT) / (tr Y'JY)
		// First we need YT.

		for (long i = 1; i <= nPoints; i++) {
			for (long j = 1; j <= nDimensions; j++) {
				for (long k = 1; k <= nDimensions; k++) {
					yt[i][j] += y[i][k] * t[k][j];
				}
			}
		}

		// X'J amount to centering the columns of X

		NUMcentreColumns (xc.peek(), 1, nPoints, 1, nDimensions, NULL);

		// tr X'J YT == tr xc' yt

		double traceXtJYT = 0.0;
		for (long i = 1; i <= nDimensions; i++) {
			for (long j = 1; j <= nPoints; j++) {
				traceXtJYT += xc[j][i] * yt[j][i];
			}
		}
		double traceYtJY = 0.0;
		for (long i = 1; i <= nDimensions; i++) {
			for (long j = 1; j <= nPoints; j++) {
				traceYtJY += y[j][i] * yc[j][i];
			}
		}

		*s = traceXtJYT / traceYtJY;

		// 5. Translation vector tr = (X - sYT)'1 / nPoints

		for (long i = 1; i <= nDimensions; i++) {
			for (long j = 1; j <= nPoints; j++) {
				v[i] += x[j][i] - *s * yt[j][i];
			}
			v[i] /= nPoints;
		}
	}
}


void NUMmspline (double knot[], long nKnots, long order, long i, double x, double *y) {
	long jj, nSplines = nKnots - order;
	if (nSplines <= 0) {
		Melder_throw (U"No splines.");
	}

	// Find the interval where x is located.
	// M-splines of order k have degree k-1.
	// M-splines are zero outside interval [ knot[i], knot[i+order] ).
	// First and last 'order' knots are equal, i.e.,
	// knot[1] = ... = knot[order] && knot[nKnots-order+1] = ... knot[nKnots].

	*y = 0.0;
	if (i > nSplines || order < 1) {
		Melder_throw (U"Combination of order and index not correct.");
	}
	for (jj = order; jj <= nKnots - order + 1; jj++) {
		if (x < knot[jj]) {
			break;
		}
	}
	if (jj < i || (jj > i + order) || jj == order || jj > (nKnots - order + 1)) {
		return;
	}

	// Calculate M[i](x|1,t) according to eq.2.

	long ito = i + order - 1;
	autoNUMvector<double> m (i, ito);
	for (long j = i; j <= ito; j++) {
		if (x >= knot[j] && x < knot[j + 1]) {
			m[j] = 1 / (knot[j + 1] - knot[j]);
		}
	}

	// Iterate to get M[i](x|k,t)

	for (long k = 2; k <= order; k++) {
		for (long j = i; j <= i + order - k; j++) {
			double kj = knot[j], kjpk = knot[j + k];
			if (kjpk > kj) {
				m[j] = k * ((x - kj) * m[j] + (kjpk - x) * m[j + 1]) / ((k - 1) * (kjpk - kj));
			}
		}
	}
	*y = m[i];
}

void NUMispline (double aknot[], long nKnots, long order, long i, double x, double *y) {
	long j, orderp1 = order + 1;

	*y = 0.0;

	for (j = orderp1; j <= nKnots - order; j++) {
		if (x < aknot[j]) {
			break;
		}
	}
	j--;
	if (j < i) {
		return;
	}
	if (j > i + order || (j == nKnots - order && x == aknot[j])) {
		*y = 1.0; return;
	}

	// Equation 5 in Ramsay's article contains some errors!!!
	// 1. the interval selection must be 'j-k <= i <= j' instead of
	//	'j-k+1 <= i <= j'
	// 2. the summation index m starts at 'i+1' instead of 'i'

	for (long m = i + 1; m <= j; m++) {
		double r;
		NUMmspline (aknot, nKnots, orderp1, m, x, &r);
		*y += (aknot[m + orderp1] - aknot[m]) * r;
	}
	*y /= orderp1;
}

double NUMwilksLambda (double *lambda, long from, long to) {
	double result = 1.0;
	for (long i = from; i <= to; i++) {
		result /= (1.0 + lambda[i]);
	}
	return result;
}

double NUMfactln (int n) {
	static double table[101];
	if (n < 0) {
		return NUMundefined;
	}
	if (n <= 1) {
		return 0;
	}
	return n > 100 ? NUMlnGamma (n + 1.0) : table[n] != 0.0 ? table[n] :
	       (table[n] = NUMlnGamma (n + 1.0));
}

void NUMnrbis (void (*f) (double x, double *fx, double *dfx, void *closure), double xmin, double xmax, void *closure, double *root) {
	double df, fx, fh, fl, tmp, xh, xl, tol;
	long itermax = 60;

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
		*root = NUMundefined;
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

	for (long iter = 1; iter <= itermax; iter++) {
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
	double x3, x4, d, root = NUMundefined, tol;
	long itermax = 100;

	double f1 = f (x1, closure);
	if (f1 == 0.0) {
		return x1;
	}
	if (f1 == NUMundefined) {
		return NUMundefined;
	}
	double f2 = f (x2, closure);
	if (f2 == 0.0) {
		return x2;
	}
	if (f2 == NUMundefined) {
		return NUMundefined;
	}
	if ( (f1 < 0.0 && f2 < 0.0) || (f1 > 0.0 && f2 > 0.0)) {
		return NUMundefined;
	}

	for (long iter = 1; iter <= itermax; iter++) {
		x3 = 0.5 * (x1 + x2);
		double f3 = f (x3, closure);
		if (f3 == 0.0) {
			return x3;
		}
		if (f3 == NUMundefined) {
			return NUMundefined;
		}

		// New guess: x4 = x3 + (x3 - x1) * sign(f1 - f2) * f3 / sqrt(f3^2 - f1*f2)

		d = f3 * f3 - f1 * f2;
		if (d < 0.0) {
			Melder_warning (U"d < 0 in ridders (iter = ", iter, U").");
			return NUMundefined;
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
					x1 = x3; f1 = f3;   // retain invariant: f1 > 0, f2 < 0
				} else {
					// f3 <= 0.0
					x2 = x3; f2 = f3;   // retain invariant: f1 > 0, f2 < 0
				}
			} else {
				// rising curve: f1 < 0, f2 > 0 
				if (f3 > 0.0) {
					x2 = x3; f2 = f3;   // retain invariant: f1 < 0, f2 > 0
				} else {
					// f3 < 0.0
					x1 = x3; f1 = f3;   // retain invariant: f1 < 0, f2 > 0
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
						x1 = x3; f1 = f3;   // retain invariant: f1 > 0, f2 < 0
					} else {
						// f3 <= 0.0
						x2 = x3; f2 = f3;   // retain invariant: f1 > 0, f2 < 0
					}
				} else {
					// rising curve: f1 < 0, f2 > 0
					if (f3 > 0.0) {
						x2 = x3; f2 = f3;   // retain invariant: f1 < 0, f2 > 0
					} else {
						// f3 < 0.0
						x1 = x3; f1 = f3;   // retain invariant: f1 < 0, f2 > 0 */
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
				if (f4 == NUMundefined) {
					return NUMundefined;
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
		static long nwarnings = 0;
		nwarnings++;
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
		return NUMundefined;
	}
	double ib = NUMincompleteBeta (0.5 * df, 0.5, df / (df + t * t));
	if (ib == NUMundefined) {
		return NUMundefined;
	}
	ib *= 0.5;
	return t < 0.0 ? ib : 1.0 - ib;
}

double NUMstudentQ (double t, double df) {
	if (df < 1) {
		return NUMundefined;
	}
	double ib = NUMincompleteBeta (0.5 * df, 0.5, df / (df + t * t));
	if (ib == NUMundefined) {
		return NUMundefined;
	}
	ib *= 0.5;
	return t > 0.0 ? ib : 1.0 - ib;
}

double NUMfisherP (double f, double df1, double df2) {
	if (f < 0.0 || df1 < 1.0 || df2 < 1.0) {
		return NUMundefined;
	}
	double ib = NUMincompleteBeta (0.5 * df2, 0.5 * df1, df2 / (df2 + f * df1));
	if (ib == NUMundefined) {
		return NUMundefined;
	}
	return 1.0 - ib;
}

double NUMfisherQ (double f, double df1, double df2) {
	if (f < 0.0 || df1 < 1.0 || df2 < 1.0) {
		return NUMundefined;
	}
	if (Melder_debug == 28) {
		return NUMincompleteBeta (0.5 * df2, 0.5 * df1, df2 / (df2 + f * df1));
	} else {
		double result = gsl_cdf_fdist_Q (f, df1, df2);
		if (isnan (result)) {
			return NUMundefined;
		}
		return result;
	}
}

double NUMinvGaussQ (double p) {
	double pc = p;
	if (p <= 0.0 || p >= 1.0) {
		return NUMundefined;
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
	return q == NUMundefined ? NUMundefined : q - params -> p;
}

double NUMinvStudentQ (double p, double df) {
	struct pdf1_struct params;
	double pc = p > 0.5 ? 1.0 - p : p, xmin, xmax = 1.0, x;

	if (p < 0.0 || p >= 1.0) {
		return NUMundefined;
	}


	// Bracket the function f(x) = NUMstudentQ (x, df) - p.

	for (;;) {
		double q = NUMstudentQ (xmax, df);
		if (q == NUMundefined) {
			return NUMundefined;
		}
		if (q < pc) {
			break;
		}
		xmax *= 2.0;
	}

	xmin = xmax > 1.0 ? xmax / 2.0 : 0.0;

	// Find zero of f(x) with Ridders' method.

	params. df = df;
	params. p = pc;
	x = NUMridders (studentQ_func, xmin, xmax, & params);
	if (x == NUMundefined) {
		return NUMundefined;
	}

	return p > 0.5 ? -x : x;
}

static double chiSquareQ_func (double x, void *voidParams) {
	struct pdf1_struct *params = (struct pdf1_struct *) voidParams;
	double q = NUMchiSquareQ (x, params -> df);
	return q == NUMundefined ? NUMundefined : q - params -> p;
}

double NUMinvChiSquareQ (double p, double df) {
	struct pdf1_struct params;
	double xmin, xmax = 1;

	if (p < 0.0 || p >= 1.0) {
		return NUMundefined;
	}

	// Bracket the function f(x) = NUMchiSquareQ (x, df) - p.

	for (;;) {
		double q = NUMchiSquareQ (xmax, df);
		if (q == NUMundefined) {
			return NUMundefined;
		}
		if (q < p) {
			break;
		}
		xmax *= 2.0;
	}
	xmin = xmax > 1.0 ? xmax / 2.0 : 0.0;

	// Find zero of f(x) with Ridders' method.

	params. df = df;
	params. p = p;
	return NUMridders (chiSquareQ_func, xmin, xmax, & params);
}

static double fisherQ_func (double x, void *voidParams) {
	struct pdf2_struct *params = (struct pdf2_struct *) voidParams;
	double q = NUMfisherQ (x, params -> df1, params -> df2);
	return q == NUMundefined ? NUMundefined : q - params -> p;
}

double NUMinvFisherQ (double p, double df1, double df2) {
	if (p <= 0.0 || p > 1.0 || df1 < 1.0 || df2 < 1.0) {
		return NUMundefined;
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
			if (q == NUMundefined) {
				return NUMundefined;
			}
			if (q < p) {
				break;
			}
			if (top > 0.9e300) {
				return NUMundefined;
			}
			top *= 1e9;
		}
		return NUMridders (fisherQ_func, 0.0, p > 0.5 ? 2.2 : top, & params);
	}
}

double NUMbeta2 (double z, double w) {
	gsl_sf_result result;
	int status = gsl_sf_beta_e (z, w, &result);
	return status == GSL_SUCCESS ? result.val : NUMundefined;
}

double NUMlnBeta (double a, double b) {
	gsl_sf_result result;
	int status = gsl_sf_lnbeta_e (a, b, &result);
	return status == GSL_SUCCESS ? result.val : NUMundefined;
}

double NUMnormalityTest_HenzeZirkler (double **data, long n, long p, double *beta, double *tnb, double *lnmu, double *lnvar) {
	if (*beta <= 0) {
		*beta = (1.0 / sqrt (2.0)) * pow ((1.0 + 2 * p) / 4.0, 1.0 / (p + 4)) * pow (n, 1.0 / (p + 4));
	}
	double p2 = p / 2.0;
	double beta2 = *beta * *beta, beta4 = beta2 * beta2, beta8 = beta4 * beta4;
	double gamma = 1.0 + 2.0 * beta2, gamma2 = gamma * gamma, gamma4 = gamma2 * gamma2;
	double delta = 1.0 + beta2 * (4.0 + 3.0 * beta2), delta2 = delta * delta;
	double prob = NUMundefined;

	*tnb = *lnmu = *lnvar = NUMundefined;

	if (n < 2 || p < 1) {
		return prob;
	}

	autoNUMvector<double> zero (1, p);
	autoNUMmatrix<double> covar (1, p, 1, p);
	autoNUMmatrix<double> x (NUMmatrix_copy (data, 1, n, 1, p), 1, 1);

	NUMcentreColumns (x.peek(), 1, n, 1, p, NULL); // x - xmean

	NUMcovarianceFromColumnCentredMatrix (x.peek(), n, p, 0, covar.peek());

	try {
		NUMlowerCholeskyInverse (covar.peek(), p, NULL);
		double djk, djj, sumjk = 0.0, sumj = 0.0;
		double b1 = beta2 / 2.0, b2 = b1 / (1.0 + beta2);
		/* Heinze & Wagner (1997), page 3
			We use d[j][k] = ||Y[j]-Y[k]||^2 = (Y[j]-Y[k])'S^(-1)(Y[j]-Y[k])
			So d[j][k]= d[k][j] and d[j][j] = 0
		*/
		for (long j = 1; j <= n; j++) {
			for (long k = 1; k < j; k++) {
				djk = NUMmahalanobisDistance_chi (covar.peek(), x[j], x[k], p, p);
				sumjk += 2.0 * exp (-b1 * djk); // factor 2 because d[j][k] == d[k][j]
			}
			sumjk += 1.0; // for k == j
			djj = NUMmahalanobisDistance_chi (covar.peek(), x[j], zero.peek(), p, p);
			sumj += exp (-b2 * djj);
		}
		*tnb = (1.0 / n) * sumjk - 2.0 * pow (1.0 + beta2, - p2) * sumj + n * pow (gamma, - p2); // n *
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
		return NUMundefined;
	}
	return mel < 1000.0 ? mel : 1000.0 * (exp (mel * log10 (2.0) / 1000.0) - 1.0);
}

double NUMhertzToMel3 (double hz) {
	if (hz < 0.0) {
		return NUMundefined;
	}
	return hz < 1000.0 ? hz : 1000.0 * log10 (1.0 + hz / 1000.0) / log10 (2.0);
}

double NUMmelToHertz2 (double mel) {
	if (mel < 0.0) {
		return NUMundefined;
	}
	return 700.0 * (pow (10.0, mel / 2595.0) - 1.0);
}

double NUMhertzToMel2 (double hz) {
	if (hz < 0.0) {
		return NUMundefined;
	}
	return 2595.0 * log10 (1.0 + hz / 700.0);
}

double NUMhertzToBark_traunmueller (double hz) {
	if (hz < 0.0) {
		return NUMundefined;
	}
	return 26.81 * hz / (1960.0 + hz) - 0.53;
}

double NUMbarkToHertz_traunmueller (double bark) {
	if (bark < 0.0 || bark > 26.28) {
		return NUMundefined;
	}
	return 1960.0 * (bark + 0.53) / (26.28 - bark);
}

double NUMbarkToHertz_schroeder (double bark) {
	return 650.0 * sinh (bark / 7.0);
}

double NUMbarkToHertz_zwickerterhardt (double hz) {
	if (hz < 0.0) {
		return NUMundefined;
	}
	return 13.0 * atan (0.00076 * hz) + 3.5 * atan (hz / 7500.0);
}

double NUMhertzToBark_schroeder (double hz) {
	if (hz < 0.0) {
		return NUMundefined;
	}
	double h650 = hz / 650.0;
	return 7.0 * log (h650 + sqrt (1.0 + h650 * h650));
}

double NUMbarkToHertz2 (double bark) {
	if (bark < 0.0) {
		return NUMundefined;
	}
	return 650.0 * sinh (bark / 7.0);
}

double NUMhertzToBark2 (double hz) {
	if (hz < 0) {
		return NUMundefined;
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
/* work[1..n+n+n];
b1 = & work[1];
b2 = & work[n+1];
aa = & work[n+n+1];
for (i=1; i<=n+n+n; i++) work[i]=0;
*/
int NUMburg (double x[], long n, double a[], int m, double *xms) {
	for (long j = 1; j <= m; j++) {
		a[j] = 0.0;
	}

	autoNUMvector<double> b1 (1, n);
	autoNUMvector<double> b2 (1, n);
	autoNUMvector<double> aa (1, m);

	// (3)

	double p = 0.0;
	for (long j = 1; j <= n; j++) {
		p += x[j] * x[j];
	}

	*xms = p / n;
	if (*xms <= 0.0) {
		return 0;    // warning empty
	}

	// (9)

	b1[1] = x[1];
	b2[n - 1] = x[n];
	for (long j = 2; j <= n - 1; j++) {
		b1[j] = b2[j - 1] = x[j];
	}

	for (long i = 1; i <= m; i++) {
		// (7)

		double num = 0.0, denum = 0.0;
		for (long j = 1; j <= n - i; j++) {
			num += b1[j] * b2[j];
			denum += b1[j] * b1[j] + b2[j] * b2[j];
		}

		if (denum <= 0.0) {
			return 0;    // warning ill-conditioned
		}

		a[i] = 2.0 * num / denum;

		// (10)

		*xms *= 1.0 - a[i] * a[i];

		// (5)

		for (long j = 1; j <= i - 1; j++) {
			a[j] = aa[j] - a[i] * aa[i - j];
		}

		if (i < m) {

			// (8)  Watch out: i -> i+1

			for (long j = 1; j <= i; j++) {
				aa[j] = a[j];
			}
			for (long j = 1; j <= n - i - 1; j++) {
				b1[j] -= aa[i] * b2[j];
				b2[j] = b2[j + 1] - aa[i] * b1[j + 1];
			}
		}
	}
	return 1;
}

void NUMdmatrix_to_dBs (double **m, long rb, long re, long cb, long ce, double ref, double factor, double floor) {
	double ref_db, factor10 = factor * 10.0;
	double max = m[rb][cb], min = max;

	Melder_assert (ref > 0 && factor > 0 && rb <= re && cb <= ce);

	for (long i = rb; i <= re; i++) {
		for (long j = cb; j <= ce; j++) {
			if (m[i][j] > max) {
				max = m[i][j];
			} else if (m[i][j] < min) {
				min = m[i][j];
			}
		}
	}

	if (max < 0 || min < 0) {
		Melder_throw (U"NUMdmatrix_to_dBs: all matrix elements must be positive.");
	}
	ref_db = factor10 * log10 (ref);

	for (long i = rb; i <= re; i++) {
		for (long j = cb; j <= ce; j++) {
			double mij = floor;
			if (m[i][j] > 0.0) {
				mij = factor10 * log10 (m[i][j]) - ref_db;
				if (mij < floor) {
					mij = floor;
				}
			}
			m[i][j] = mij;
		}
	}
}

double **NUMcosinesTable (long first, long last, long npoints) {
	Melder_assert (0 < first && first <= last && npoints > 0);
	autoNUMmatrix<double> m (first, last, 1, npoints);
	for (long i = first; i <= last; i++) {
		double f = i * NUMpi / npoints;
		for (long j = 1; j <= npoints; j++) {
			m[i][j] = cos (f * (j - 0.5));
		}
	}
	return m.transfer();
}

void NUMspline (double x[], double y[], long n, double yp1, double ypn, double y2[]) {
	autoNUMvector<double> u (1, n - 1);

	if (yp1 > 0.99e30) {
		y2[1] = u[1] = 0.0;
	} else {
		y2[1] = -0.5;
		u[1] = (3.0 / (x[2] - x[1])) * ( (y[2] - y[1]) / (x[2] - x[1]) - yp1);
	}

	for (long i = 2; i <= n - 1; i++) {
		double sig = (x[i] - x[i - 1]) / (x[i + 1] - x[i - 1]);
		double p = sig * y2[i - 1] + 2.0;
		y2[i] = (sig - 1.0) / p;
		u[i] = (y[i + 1] - y[i]) / (x[i + 1] - x[i]) - (y[i] - y[i - 1]) / (x[i] - x[i - 1]);
		u[i] = (6.0 * u[i] / (x[i + 1] - x[i - 1]) - sig * u[i - 1]) / p;
	}

	double qn, un;
	if (ypn > 0.99e30) {
		qn = un = 0.0;
	} else {
		qn = 0.5;
		un = (3.0 / (x[n] - x[n - 1])) * (ypn - (y[n] - y[n - 1]) / (x[n] - x[n - 1]));
	}

	y2[n] = (un - qn * u[n - 1]) / (qn * y2[n - 1] + 1.0);
	for (long k = n - 1; k >= 1; k--) {
		y2[k] = y2[k] * y2[k + 1] + u[k];
	}
}

void NUMsplint (double xa[], double ya[], double y2a[], long n, double x, double *y) {
	long klo = 1, khi = n;
	while (khi - klo > 1) {
		long k = (khi + klo) >> 1;
		if (xa[k] > x) {
			khi = k;
		} else {
			klo = k;
		}
	}
	double h = xa[khi] - xa[klo];
	if (h == 0.0) {
		Melder_throw (U"NUMsplint: bad input value.");
	}
	double a = (xa[khi] - x) / h;
	double b = (x - xa[klo]) / h;
	*y = a * ya[klo] + b * ya[khi] + ( (a * a * a - a) * y2a[klo] +
	                                   (b * b * b - b) * y2a[khi]) * (h * h) / 6.0;
}

double NUMsinc (const double x) {
	struct gsl_sf_result_struct result;
	int status = gsl_sf_sinc_e (x / NUMpi, &result);
	return status == GSL_SUCCESS ? result. val : NUMundefined;
}

double NUMsincpi (const double x) {
	struct gsl_sf_result_struct result;
	int status = gsl_sf_sinc_e (x, &result);
	return status == GSL_SUCCESS ? result. val : NUMundefined;
}

/* Does the line segment from (x1,y1) to (x2,y2) intersect with the line segment from (x3,y3) to (x4,y4)? */
int NUMdoLineSegmentsIntersect (double x1, double y1, double x2, double y2, double x3, double y3,
                                double x4, double y4) {
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

int NUMgetIntersectionsWithRectangle (double x1, double y1, double x2, double y2,
                                      double xmin, double ymin, double xmax, double ymax, double *xi, double *yi) {
	double x[6], y[6];
	long ni = 0;

	x[1] = x[4] = x[5] = xmin;
	x[2] = x[3] = xmax;
	y[1] = y[2] = y[5] = ymin;
	y[3] = y[4] = ymax;
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

	for (long i = 1; i <= 4; i++) {
		double denom = (x[i + 1] - x[i]) * (y2 - y1) - (y[i + 1] - y[i]) * (x2 - x1);
		double s, t, x3, y3;
		if (denom == 0.0) {
			continue;
		}
		/* We have an intersection. */
		t = ((y[i] - y1) * (x2 - x1) - (x[i] - x1) * (y2 - y1)) / denom;
		if (t < 0 || t >= 1) {
			continue;
		}
		/* Intersection is within rectangle side. */
		x3 = x[i] + t * (x[i + 1] - x[i]);
		y3 = y[i] + t * (y[i + 1] - y[i]);
		/* s must also be valid */
		if (x1 != x2) {
			s = (x3 - x1) / (x2 - x1);
		} else {
			s = (y3 - y1) / (y2 - y1);
		}
		if (s < 0 || s >= 1) {
			continue;
		}
		ni++;
		if (ni > 2) {
			Melder_throw (U"Too many intersections.");
		}
		xi[ni] = x3;
		yi[ni] = y3;
	}
	return ni;
}


bool NUMclipLineWithinRectangle (double xl1, double yl1, double xl2, double yl2, double xr1, double yr1,
                                double xr2, double yr2, double *xo1, double *yo1, double *xo2, double *yo2) {
	int ncrossings = 0;
	bool xswap, yswap;
	double a, b, x, y, t, xc[5], yc[5], xmin, xmax, ymin, ymax;

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
			t = *xo1; *xo1 = *xo2; *xo2 = t;
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
			t = *yo1; *yo1 = *yo2; *yo2 = t;
		}
		return true;
	}

	// Now we know that the line from (x1,y1) to (x2,y2) is neither horizontal nor vertical.
	// Parametrize it as y = ax + b

	a = (yl1 - yl2) / (xl1 - xl2);
	b = yl1 - a * xl1;


	//	To determine the crossings we have to avoid counting the crossings in a corner twice.
	//	Therefore we test the corners inclusive (..<=..<=..) on the vertical borders of the rectangle
	//	and exclusive (..<..<) at the horizontal borders.


	y = a * xr1 + b; // Crossing at y with left border: x = xr1

	if (y >= yr1 && y <= yr2 && xmin < xr1) { // Within vertical range?
		ncrossings++;
		xc[ncrossings] = xr1; yc[ncrossings] = y;
		xc[2] = xmax;
		yc[2] = xl1 > xl2 ? yl1 : yl2;
	}

	x = (yr2 - b) / a; // Crossing at x with top border: y = yr2

	if (x > xr1 && x < xr2 && ymax > yr2) { // Within horizontal range?
		ncrossings++;
		xc[ncrossings] = x; yc[ncrossings] = yr2;
		if (ncrossings == 1) {
			yc[2] = ymin;
			xc[2] = yl1 < yl2 ? xl1 : xl2;
		}
	}

	y = a * xr2 + b; // Crossing at y with right border: x = xr2

	if (y >= yr1 && y <= yr2 && xmax > xr2) { // Within vertical range?
		ncrossings++;
		xc[ncrossings] = xr2; yc[ncrossings] = y;
		if (ncrossings == 1) {
			xc[2] = xmin;
			yc[2] = xl1 < xl2 ? yl1 : yl2;
		}
	}

	x = (yr1 - b) / a; // Crossing at x with bottom border: y = yr1

	if (x > xr1 && x < xr2 && ymin < yr1) {
		ncrossings++;
		xc[ncrossings] = x; yc[ncrossings] = yr1;
		if (ncrossings == 1) {
			yc[2] = ymax;
			xc[2] = yl1 > yl2 ? xl1 : xl2;
		}
	}
	if (ncrossings == 0) {
		return false;
	}
	if (ncrossings == 1 || ncrossings == 2) {
		// if start and endpoint of line are outside rectangle and ncrossings == 1,
		// than the line only touches.
		if (ncrossings == 1 &&
		        (xl1 < xr1 || xl1 > xr2 || yl1 < yr1 || yl1 > yr2) &&
		        (xl2 < xr1 || xl2 > xr2 || yl2 < yr1 || yl2 > yr2)) {
			return true;
		}

		if ( (xc[1] > xc[2] && ! xswap) || (xc[1] < xc[2] && xswap)) {
			t = xc[1]; xc[1] = xc[2]; xc[2] = t;
			t = yc[1]; yc[1] = yc[2]; yc[2] = t;
		}
		*xo1 = xc[1]; *yo1 = yc[1]; *xo2 = xc[2]; *yo2 = yc[2];
	} else {
		Melder_throw (U"Too many crossings found.");
	}
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
Closely modelled after the netlib code by Oleg Keselyov.
*/
double NUMminimize_brent (double (*f) (double x, void *closure), double a, double b, void *closure, double tol, double *fx) {
	double x, v, fv, w, fw;
	const double golden = 1 - NUM_goldenSection;
	const double sqrt_epsilon = sqrt (NUMfpp -> eps);
	long itermax = 60;

	Melder_assert (tol > 0 && a < b);

	/* First step - golden section */

	v = a + golden * (b - a);
	fv = (*f) (v, closure);
	x = v;  w = v;
	*fx = fv;  fw = fv;

	for (long iter = 1; iter <= itermax; iter++) {
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

				v = w;  w = x;  x = t;
				fv = fw;  fw = *fx;  *fx = ft;
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
	probs is probability vector, i.e. all 0 <= probs[i] <= 1 and sum(i=1;i=nprobs, probs[i])= 1
	p is a probability
*/
long NUMgetIndexFromProbability (double *probs, long nprobs, double p) {
	long index = 1;
	double psum = probs[index];
	while (p > psum && index < nprobs) {
		psum += probs[++index];
	}
	return index;
}

// straight line fitting

void NUMlineFit_theil (double *x, double *y, long numberOfPoints, double *p_m, double *p_intercept, bool incompleteMethod) {
	try {
		/* Theil's incomplete method:
		 * Split (x[i],y[i]) as
		 * (x[i],y[i]), (x[N+i],y[N=i], i=1..numberOfPoints/2
		 * m[i] = (y[N+i]-y[i])/(x[N+i]-x[i])
		 * m = median (m[i])
		 * b = median(y[i]-m*x[i])
		 */
		double m, intercept;
		if (numberOfPoints <= 0) {
			m = intercept = NUMundefined;
		} else if (numberOfPoints == 1) {
			intercept = y[1];
			m = 0;
		} else if (numberOfPoints == 2) {
			m = (y[2] - y[1]) / (x[2] - x[1]);
			intercept = y[1] - m * x[1];
		} else {
			long numberOfCombinations;
			autoNUMvector<double> mbs;
			if (incompleteMethod) { // incomplete method
				numberOfCombinations = numberOfPoints / 2;
				mbs.reset (1, numberOfPoints); //
				long n2 = numberOfPoints % 2 == 1 ? numberOfCombinations + 1 : numberOfCombinations;
				for (long i = 1; i <= numberOfCombinations; i++) {
					mbs[i] = (y[n2 + i] - y[i]) / (x[n2 + i] - x[i]);
				}
			} else { // use all combinations
				numberOfCombinations = (numberOfPoints - 1) * numberOfPoints / 2;
				mbs.reset (1, numberOfCombinations);
				long index = 0;
				for (long i = 1; i < numberOfPoints; i++) {
					for (long j = i + 1; j <= numberOfPoints; j++) {
						mbs[++index] = (y[j] - y[i]) / (x[j] - x[i]);
					}
				}
			}
			NUMsort_d (numberOfCombinations, mbs.peek());
			m = NUMquantile (numberOfCombinations, mbs.peek(), 0.5);
			for (long i = 1; i <= numberOfPoints; i++) {
				mbs[i] = y[i] - m * x[i];
			}
			NUMsort_d (numberOfPoints, mbs.peek());
			intercept = NUMquantile (numberOfPoints, mbs.peek(), 0.5);
		}
		if (p_m) {
			*p_m = m;
		}
		if (p_intercept) {
			*p_intercept = intercept;
		}
	} catch (MelderError) {
		Melder_throw (U"No line fit (Theil's method)");
	}
}

void NUMlineFit_LS (double *x, double *y, long numberOfPoints, double *p_m, double *intercept) {
	double sx = 0.0, sy = 0.0;
	for (long i = 1; i <= numberOfPoints; i++) {
		sx += x[i];
		sy += y[i];
	}
	double xmean = sx / numberOfPoints;
	double st2 = 0.0, m = 0.0;
	for (long i = 1; i <= numberOfPoints; i++) {
		double t = x[i] - xmean;
		st2 += t * t;
		m += t * y[i];
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

void NUMlineFit (double *x, double *y, long numberOfPoints, double *m, double *intercept, int method) {
	if (method == 1) {
		NUMlineFit_LS (x, y, numberOfPoints, m, intercept);
	} else if (method == 3) {
		NUMlineFit_theil (x, y, numberOfPoints, m, intercept, false);
	} else {
		NUMlineFit_theil (x, y, numberOfPoints, m, intercept, true);
	}
}

// IEEE: Programs for digital signal processing section 4.3 LPTRN
// lpc[1..n] to rc[1..n]
void NUMlpc_lpc_to_rc (double *lpc, long p, double *rc) {
	autoNUMvector<double> b (1, p);
	autoNUMvector<double> a (NUMvector_copy<double> (lpc, 1, p), 1);
	for (long m = p; m > 0; m--) {
		rc[m] = a[m];
		if (fabs (rc[m]) > 1) {
			Melder_throw (U"Relection coefficient [", m, U"] larger than 1.");
		}
		for (long i = 1; i < m; i++) {
			b[i] = a[i];
		}
		for (long i = 1; i < m; i++) {
			a[i] = (b[i] - rc[m] * b[m - i]) / (1.0 - rc[m] * rc[m]);
		}
	}
}

void NUMlpc_rc_to_area2 (double *rc, long n, double *area);
void NUMlpc_rc_to_area2 (double *rc, long n, double *area) {
	double s = 0.0001; /* 1.0 cm^2 at glottis */
	for (long i = n; i > 0; i--) {
		s *= (1.0 + rc[i]) / (1.0 - rc[i]);
		area[i] = s;
	}
}

void NUMlpc_area_to_lpc2 (double *area, long n, double *lpc);
void NUMlpc_area_to_lpc2 (double *area, long n, double *lpc) {
	// from area to reflection coefficients
	autoNUMvector<double> rc (1, n);
	// normalisation: area[n+1] = 0.0001
	for (long j = n; j > 0; j--) {
		double ar = area[j+1] / area[j];
		rc[j] = (1 - ar) / (1 + ar);
	}
	// LPTRAN works from mouth to lips:
	for (long j = 1; j <= n; j++) {
		lpc[j] = rc[n - j + 1];
	}
	for (long j = 2; j <= n; j++) {
		long nh = j / 2;
		double q = rc[j];
		for (long k = 1; k <= nh; k++) {
			double at = lpc[k] + q * lpc[j - k];
			lpc[j - k] += q * lpc[k];
			lpc[k] = at;
		}
	}
}

void NUMlpc_lpc_to_rc2 (double *lpc, long m, double *rc);
void NUMlpc_lpc_to_rc2 (double *lpc, long m, double *rc) { // klopt nog niet
	NUMvector_copyElements<double> (lpc, rc, 1, m);
	for (long j = 2; j <= m; j++) {
		long jb = m + 1 - j;
		long mh = (jb + 1) / 2;
		double rct = rc[jb+1];
		double d = 1.0 - rct * rct;
		for (long k = 1; k <= mh; k++) {
			rc[k] *= (1 - rct) / d;
		}
	}
}
// area[1] at lips generates n+1 areas from n rc's
void NUMlpc_rc_to_area (double *rc, long m, double *area) {
	area[m+1] = 0.0001; /* 1.0 cm^2 */
	for (long j = 1; j <= m; j++) {
		double ar = (1.0 - rc[m+1-j]) / (1.0 + rc[m+1-j]);
		area[m+1-j] = area[m+2-j] / ar;
	}
}

// returns m-1 reflection coefficients from m areas
void NUMlpc_area_to_rc (double *area, long m, double *rc) {
	for (long j = 1; j <= m - 1; j++) {
		double ar = area[j+1] / area[j];
		rc[j] = (1.0 - ar) / (1.0 + ar);
	}
}

void NUMlpc_rc_to_lpc (double *rc, long m, double *lpc);
void NUMlpc_rc_to_lpc (double *rc, long m, double *lpc) {
	NUMvector_copyElements<double> (rc, lpc, 1, m);
	for (long j = 2; j <= m; j++) {
		for (long k = 1; k <= j / 2; k++) {
			double at = lpc[k] + rc[j] * lpc[j - k];
			lpc[j - k] += rc[j] * lpc[k];
			lpc[k] = at;
		}
	}
}

void NUMlpc_area_to_lpc (double *area, long m, double *lpc) {
	// from area to reflection coefficients
	autoNUMvector<double> rc (1, m);
	// normalisation: area[n+1] = 0.0001
	NUMlpc_area_to_rc (area, m, rc.peek());
	NUMlpc_rc_to_lpc (rc.peek(), m - 1, lpc);
}

void NUMlpc_lpc_to_area (double *lpc, long m, double *area) {
	autoNUMvector<double> rc (1, m);
	NUMlpc_lpc_to_rc (lpc, m, rc.peek());
	NUMlpc_rc_to_area (rc.peek(), m, area);

}

#undef MAX
#undef MIN
#undef SIGN

#define SMALL_MEAN 14
/* If n*p < SMALL_MEAN then use BINV algorithm. The ranlib implementation used cutoff=30;
 * but on my (Brian Gough) computer 14 works better
 */

#define BINV_CUTOFF 110
/* In BINV, do not permit ix too large */

#define FAR_FROM_MEAN 20
/* If ix-n*p is larger than this, then use the "squeeze" algorithm.
 * Ranlib used 20, and this seems to be the best choice on my  (Brian Gough) machine as well.
 */

#define LNFACT(x) gsl_sf_lnfact(x)

inline static double Stirling (double y1)
{
	double y2 = y1 * y1;
	double s = (13860.0 - (462.0 - (132.0 - (99.0 - 140.0 / y2) / y2) / y2) / y2) / y1 / 166320.0;
	return s;
}

// djmw 20121211 replaced calls to gsl_rng_uniform with NUMrandomUniform (0,1)

long NUMrandomBinomial (double p, long n) {
	if (p < 0.0 || p > 1.0 || n < 0) {
		return -100000000;
	}
	long ix;                       /* return value */
	int flipped = 0;

	if (n == 0) {
		return 0;
	}
	if (p > 0.5) {
		p = 1.0 - p;              /* work with small p */
		flipped = 1;
	}

	double q = 1.0 - p;
	double s = p / q;
	double np = n * p;

	/* Inverse cdf logic for small mean (BINV in K+S) */

	if (np < SMALL_MEAN) {
		double f0 = pow (q, n); // djmw gsl_pow_int (q, n);   /* f(x), starting with x=0 */

		while (1) {
			/* This while(1) loop will almost certainly only loop once; but
			* if u=1 to within a few epsilons of machine precision, then it
			* is possible for roundoff to prevent the main loop over ix to
			* achieve its proper value.  following the ranlib implementation,
			* we introduce a check for that situation, and when it occurs,
			* we just try again.
			*/

			double f = f0;
			double u = NUMrandomUniform (0.0, 1.0); //djmw gsl_rng_uniform (rng);

			for (ix = 0; ix <= BINV_CUTOFF; ++ix) {
				if (u < f) {
					goto Finish;
				}
				u -= f;
				/* Use recursion f(x+1) = f(x)*[(n-x)/(x+1)]*[p/(1-p)] */
				f *= s * (n - ix) / (ix + 1.0);
			}

			/* It should be the case that the 'goto Finish' was encountered
			* before this point was ever reached.  But if we have reached
			* this point, then roundoff has prevented u from decreasing
			* all the way to zero.  This can happen only if the initial u
			* was very nearly equal to 1, which is a rare situation.  In
			* that rare situation, we just try again.
			*
			* Note, following the ranlib implementation, we loop ix only to
			* a hardcoded value of SMALL_MEAN_LARGE_N=110; we could have
			* looped to n, and 99.99...% of the time it won't matter.  This
			* choice, I think is a little more robust against the rare
			* roundoff error.  If n>LARGE_N, then it is technically
			* possible for ix>LARGE_N, but it is astronomically rare, and
			* if ix is that large, it is more likely due to roundoff than
			* probability, so better to nip it at LARGE_N than to take a
			* chance that roundoff will somehow conspire to produce an even
			* larger (and more improbable) ix.  If n<LARGE_N, then once
			* ix=n, f=0, and the loop will continue until ix=LARGE_N.
			*/
		}
	} else {
		/* For n >= SMALL_MEAN, we invoke the BTPE algorithm */

		double ffm = np + p;      /* ffm = n*p+p             */
		long m = (long) ffm;        /* m = int floor[n*p+p]    */
		double fm = m;            /* fm = double m;          */
		double xm = fm + 0.5;     /* xm = half integer mean (tip of triangle)  */
		double npq = np * q;      /* npq = n*p*q            */

		/* Compute cumulative area of tri, para, exp tails */

		/* p1: radius of triangle region; since height=1, also: area of region */
		/* p2: p1 + area of parallelogram region */
		/* p3: p2 + area of left tail */
		/* p4: p3 + area of right tail */
		/* pi/p4: probability of i'th area (i=1,2,3,4) */

		/* Note: magic numbers 2.195, 4.6, 0.134, 20.5, 15.3 */
		/* These magic numbers are not adjustable...at least not easily! */

		double p1 = floor (2.195 * sqrt (npq) - 4.6 * q) + 0.5;

		/* xl, xr: left and right edges of triangle */
		double xl = xm - p1;
		double xr = xm + p1;

		/* Parameter of exponential tails */
		/* Left tail:  t(x) = c*exp(-lambda_l*[xl - (x+0.5)]) */
		/* Right tail: t(x) = c*exp(-lambda_r*[(x+0.5) - xr]) */

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

		/* generate random variates, u specifies which region: Tri, Par, Tail */
		u = p4 * NUMrandomUniform (0.0, 1.0); // djmw gsl_rng_uniform (rng) * p4;
		v = NUMrandomUniform (0.0, 1.0); // djmw gsl_rng_uniform (rng);

		if (u <= p1) {
			/* Triangular region */
			ix = (long) (xm - p1 * v + u);
			goto Finish;
		} else if (u <= p2) {
			/* Parallelogram region */
			double x = xl + (u - p1) / c;
			v = v * c + 1.0 - fabs (x - xm) / p1;
			if (v > 1.0 || v <= 0.0) {
				goto TryAgain;
			}
			ix = (long) x;
		} else if (u <= p3) {
			/* Left tail */
			ix = (long) (xl + log (v) / lambda_l);
			if (ix < 0) {
				goto TryAgain;
			}
			v *= ((u - p2) * lambda_l);
		} else {
			/* Right tail */
			ix = (long) (xr - log (v) / lambda_r);
			if (ix > (double) n) {
				goto TryAgain;
			}
			v *= ((u - p3) * lambda_r);
		}

		/* At this point, the goal is to test whether v <= f(x)/f(m)
		*
		*  v <= f(x)/f(m) = (m!(n-m)! / (x!(n-x)!)) * (p/q)^{x-m}
		*
		*/

		/* Here is a direct test using logarithms.  It is a little
		* slower than the various "squeezing" computations below, but
		* if things are working, it should give exactly the same answer
		* (given the same random number seed).  */

		#ifdef DIRECT
		var = log (v);

		accept = LNFACT (m) + LNFACT (n - m) - LNFACT (ix) - LNFACT (n - ix) + (ix - m) * log (p / q);

		#else /* SQUEEZE METHOD */

		/* More efficient determination of whether v < f(x)/f(M) */

		long k = labs (ix - m);

		if (k <= FAR_FROM_MEAN) {
			/*
			* If ix near m (ie, |ix-m|<FAR_FROM_MEAN), then do
			* explicit evaluation using recursion relation for f(x)
			*/
			double g = (n + 1) * s;
			double f = 1.0;

			var = v;

			if (m < ix) {
				for (long i = m + 1; i <= ix; i++) {
					f *= (g / i - s);
				}
			} else if (m > ix) {
				for (long i = ix + 1; i <= m; i++) {
					f /= (g / i - s);
				}
			}

			accept = f;
		} else {
			/* If ix is far from the mean m: k=ABS(ix-m) large */

			var = log (v);

			if (k < npq / 2 - 1) {
				/* "Squeeze" using upper and lower bounds on
				* log(f(x)) The squeeze condition was derived
				* under the condition k < npq/2-1 */
				double amaxp = k / npq * ((k * (k / 3.0 + 0.625) + (1.0 / 6.0)) / npq + 0.5);
				double ynorm = -(k * k / (2.0 * npq));
				if (var < ynorm - amaxp) {
					goto Finish;
				}
				if (var > ynorm + amaxp) {
					goto TryAgain;
				}
			}

			/* Now, again: do the test log(v) vs. log f(x)/f(M) */

			#if USE_EXACT
			/* This is equivalent to the above, but is a little (~20%) slower */
			/* There are five log's vs three above, maybe that's it? */

			accept = LNFACT (m) + LNFACT (n - m) - LNFACT (ix) - LNFACT (n - ix) + (ix - m) * log (p / q);

			#else /* USE STIRLING */
			/* The "#define Stirling" above corresponds to the first five
			* terms in asymptotic formula for
			* log Gamma (y) - (y-0.5)log(y) + y - 0.5 log(2*pi);
			* See Abramowitz and Stegun, eq 6.1.40
			*/

			/* Note below: two Stirling's are added, and two are
			* subtracted.  In both K+S, and in the ranlib
			* implementation, all four are added.  I (jt) believe that
			* is a mistake -- this has been confirmed by personal
			* correspondence w/ Dr. Kachitvichyanukul.  Note, however,
			* the corrections are so small, that I couldn't find an
			* example where it made a difference that could be
			* observed, let alone tested.  In fact, define'ing Stirling
			* to be zero gave identical results!!  In practice, alv is
			* O(1), ranging 0 to -10 or so, while the Stirling
			* correction is typically O(10^{-5}) ...setting the
			* correction to zero gives about a 2% performance boost;
			* might as well keep it just to be pedantic.  */

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

double NUMrandomBinomial_real (double p, long n) {
	if (p < 0.0 || p > 1.0 || n < 0) {
		return NUMundefined;
	} else {
		return (double) NUMrandomBinomial (p, n);
	}
}

void NUMlngamma_complex (double zr, double zi, double *lnr, double *arg) {
	double ln_re = NUMundefined, ln_arg = NUMundefined;
	gsl_sf_result gsl_lnr, gsl_arg;
	if (gsl_sf_lngamma_complex_e (zr, zi, & gsl_lnr, & gsl_arg)) {
		ln_re = gsl_lnr.val; ln_arg = gsl_arg.val;
	}
	if (lnr) {
		*lnr = ln_re;
	}
	if (arg) {
		*arg = ln_arg;
	}
}

bool NUMdmatrix_hasFiniteElements (double **m, long row1, long row2, long col1, long col2) {
	for (long i = row1; i <= row2; i ++) {
		for (long j = col1; j <= col2; j ++) {
			if (! isfinite (m [i] [j])) {
				return false;
			}
		}
	}
	return true;
}

void NUMdmatrix_diagnoseCells (double **m, long rb, long re, long cb, long ce, long maximumNumberOfPositionsToReport) {
	long numberOfInvalids = 0;
	bool firstTime = true;
	for (long i = rb; i <= re; i++) {
		for (long j = cb; j <= ce; j++) {
			if (! isfinite (m [i][j])) {
				numberOfInvalids ++;
				if (firstTime) {
					MelderInfo_writeLine (U"Invalid data at the following [row] [column] positions:");
					firstTime = false;
				}
				if (numberOfInvalids <= maximumNumberOfPositionsToReport) {
					if (numberOfInvalids % 10 != 0) {
						MelderInfo_write (U"[", i, U"][", j, U"]  ");
					} else {
						MelderInfo_writeLine (U"[", i, U"][", j, U"]");
					}
				} else {
					return;
				}
			}
		}
	}
	if (numberOfInvalids == 0) {
		MelderInfo_writeLine (U"All cells have valid data.");
	}
}

/* End of file NUM2.cpp */
