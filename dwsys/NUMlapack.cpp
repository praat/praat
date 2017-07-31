/* NUMlapack.cpp
 *
 * Copyright (C) 1994-2011 David Weenink
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
 djmw 20020812 GPL header
 djmw 20030205 Latest modification (NUMmacros)
 djmw 20071022 NUMmatricesToUpperTriangularForms now inializes l=0
*/

#include "melder.h"
#include "NUMlapack.h"
#include "NUMmachar.h"

#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))
#define SIGN(x,s) ((s) < 0 ? -fabs (x) : fabs(x))
#define TOVEC(x) (&(x) - 1)

void NUMidentity (double **a, long rb, long re, long cb) {
	for (long i = rb; i <= re; i++) {
		for (long j = cb; j <= cb + (re - rb); j++) {
			a[i][j] = 0;
		}
		a[i][i] = 1;
	}
}

double NUMpythagoras (double a, double b) {
	double absa = fabs (a), absb = fabs (b);
	double w = MAX (absa, absb);
	double z = MIN (absa, absb);
	if (z == 0) {
		return w;
	}
	double t = z / w;
	return w * sqrt (1 + t * t);
}

double NUMnorm2 (long n, double *x, long incx) {
	if (n < 1 || incx < 1) {
		return 0;
	}
	if (n == 1) {
		return fabs (x[1]);
	}

	double scale = 0, ssq = 1;
	for (long i = 1; i <= 1 + (n - 1) * incx; i += incx) {
		if (x[i] != 0) {
			double absxi = fabs (x[i]);
			if (scale < absxi) {
				double tmp = scale / absxi;
				ssq = 1 + ssq * tmp * tmp;
				scale = absxi;
			} else {
				double tmp = absxi / scale;
				ssq += tmp * tmp;
			}
		}
	}
	return scale * sqrt (ssq);
}


double NUMfrobeniusnorm (long m, long n, double **x) {
	if (n < 1 || m < 1) {
		return 0;
	}

	double scale = 0, ssq = 1;
	for (long i = 1; i <= m; i++) {
		for (long j = 1; j <= n; j++) {
			if (x[i][j] != 0) {
				double absxi = fabs (x[i][j]);
				if (scale < absxi) {
					double t = scale / absxi;
					ssq = 1 + ssq * t * t;
					scale = absxi;
				} else {
					double t = absxi / scale;
					ssq  += t * t;
				}
			}
		}
	}
	return scale * sqrt (ssq);
}


double NUMdotproduct (long n, double x[], long incx, double y[], long incy) {
	long ix = 1, iy = 1;
	if (n <= 0) {
		return 0;
	}
	if (incx < 0) {
		ix = (-n + 1) * incx + 1;
	}
	if (incy < 0) {
		iy = (-n + 1) * incy + 1;
	}

	double dot = 0;
	for (long i = 1; i <= n; i++, ix += incx, iy += incy) {
		dot += x[ix] * y[iy];
	}

	return dot;
}

void NUMdaxpy (long n, double da, double x[], long incx, double y[], long incy) {
	long ix = 1, iy = 1;

	if (n <= 0) {
		return;
	}
	if (incx < 0) {
		ix = (-n + 1) * incx + 1;
	}
	if (incy < 0) {
		iy = (-n + 1) * incy + 1;
	}

	for (long i = 1; i <= n; i++, ix += incx, iy += incy) {
		y[iy] += da * x[ix];
	}
}

void NUMvector_scale (long n, double da, double dx[], long incx) {
	if (n < 1 || incx < 1) {
		return;
	}
	for (long i = 1; i <= n * incx; i += incx) {
		dx[i] *= da;
	}
}

void NUMcopyElements (long n, double x[], long incx, double y[], long incy) {
	if (n <= 0) {
		return;
	}
	if (incx == 1 && incy == 1) {
		for (long i = 1; i <= n; i++) {
			y[i] = x[i];
		}
	} else {
		long ix = 1, iy = 1;
		if (incx < 0) {
			ix = (-n + 1) * incx + 1;
		}
		if (incy < 0) {
			iy = (-n + 1) * incy + 1;
		}
		for (long i = 1; i <= n; i++, ix += incx, iy += incy) {
			y[iy] = x[ix];
		}
	}
}

void NUMplaneRotation (long n, double x[], long incx, double y[], long incy,
                       double c, double s) {
	if (n < 1) {
		return;
	}
	if (incx == 1 && incy == 1) {
		for (long i = 1; i <= n; i++) {
			double xt = c * x[i] + s * y[i];
			y[i] = c * y[i] - s * x[i];
			x[i] = xt;
		}
	} else {
		long ix = 1, iy = 1;
		if (incx < 0) {
			ix = (-n + 1) * incx + 1;
		}
		if (incy < 0) {
			iy = (-n + 1) * incy + 1;
		}
		for (long i = 1; i <= n; i++, ix += incx, iy += incy) {
			double xt = c * x[ix] + s * y[iy];
			y[iy] = c * y[iy] - s * x[ix];
			x[ix] = xt;
		}
	}
}

void NUMpermuteColumns (int forward, long m, long n, double **x, long *perm) {
	if (n <= 1) {
		return;
	}

	for (long i = 1; i <= n; i++) {
		perm[i] = - perm[i];
	}

	if (forward) {
		for (long i = 1; i <= n; i++) {
			if (perm[i] > 0) {
				continue;
			}

			long j = i;
			perm[j] = -perm[j];
			long in = perm[j];
			for (;;) {
				if (perm[in] > 0) {
					break;
				}
				for (long ii = 1; ii <= m; ii++) {
					double tmp = x[ii][j];
					x[ii][j] = x[ii][in];
					x[ii][in] = tmp;
				}
				perm[in] = -perm[in];
				j = in;
				in = perm[in];
			}
		}
	} else {
		for (long i = 1; i <= n; i++) {
			if (perm[i] > 0) {
				continue;
			}
			long j = perm[i] = - perm[i];
			for (;;) {
				if (j == i) {
					break;
				}
				for (long ii = 1; ii <= m; ii++) {
					double tmp = x[ii][i];
					x[ii][i] = x[ii][j];
					x[ii][j] = tmp;
				}
				perm[j] = -perm[j];
				j = perm[j];
			}
		}
	}
}

void NUMfindHouseholder (long n, double *alpha, double x[], long incx,
                         double *tau) {
	double xnorm;

	if (n < 2 || (xnorm = NUMnorm2 (n - 1, x, incx)) == 0) {
		*tau = 0; return;
	}

	double beta = NUMpythagoras (*alpha, xnorm);
	if (*alpha > 0) {
		beta = - beta;
	}
	if (! NUMfpp) {
		NUMmachar ();
	}
	double safmin = NUMfpp -> sfmin / NUMfpp -> eps;

	if (fabs (beta) >= safmin) {
		*tau = (beta - *alpha) / beta;
		NUMvector_scale (n - 1, 1 / (*alpha - beta), x, incx);
		*alpha = beta;
	} else {
		// xnorm, beta may be inaccurate; scale x and recompute them

		double rsafmn = 1 / safmin;
		long knt = 0;
		do {
			knt++;
			NUMvector_scale (n - 1, rsafmn, x, incx);
			beta *= rsafmn; *alpha *= rsafmn;
		} while (fabs (beta) < safmin);

		/*
			New beta is at most 1, at least safmin
		*/

		xnorm = NUMnorm2 (n - 1, x, incx);
		beta = NUMpythagoras (*alpha, xnorm);
		if (*alpha > 0) {
			beta = - beta;
		}
		*tau = (beta - *alpha) / beta;
		NUMvector_scale (n - 1, 1 / (*alpha - beta), x, incx);

		/*
			If alpha is subnormal, it may lose relative accuracy
		*/

		*alpha = beta;
		for (long i = 1; i <= knt; i++) {
			*alpha *= safmin;
		}
	}
}

void NUMfindGivens (double f, double g, double *cs, double *sn, double *r) {
	long count;

	if (! NUMfpp) {
		NUMmachar ();
	}
	double safmn2 = pow (NUMfpp -> base, (long) (log (NUMfpp -> sfmin / NUMfpp -> eps) /
	                     log (NUMfpp -> base) / 2.));
	double safmx2 = 1 / safmn2;

	if (g == 0) {
		*cs = 1;
		*sn = 0;
		*r = f;
		return;
	}
	if (f == 0) {
		*cs = 0;
		*sn = 1;
		*r = g;
		return;
	}

	double f1 = f, g1 = g;
	double scale = MAX (fabs (f1), fabs (g1));
	if (scale >= safmx2) {
		count = 0;
		do {
			count++;
			f1 *= safmn2;
			g1 *= safmn2;
			scale = MAX (fabs (f1), fabs (g1));
		} while (scale >= safmx2);

		*r = sqrt (f1 * f1 + g1 * g1);
		*cs = f1 / *r;
		*sn = g1 / *r;
		for (long i = 1; i <= count; i++) {
			*r *= safmx2;
		}
	} else if (scale <= safmn2) {
		count = 0;
		do {
			count++;
			f1 *= safmx2;
			g1 *= safmx2;
			scale = MAX (fabs (f1), fabs (g1));
		} while (scale <= safmn2);

		*r = sqrt (f1 * f1 + g1 * g1);
		*cs = f1 / *r;
		*sn = g1 / *r;
		for (long i = 1; i <= count; i++) {
			*r *= safmn2;
		}
	} else {
		*r = sqrt (f1 * f1 + g1 * g1);
		*cs = f1 / *r;
		*sn = g1 / *r;
	}

	if (fabs (f) > fabs (g) && *cs < 0) {
		*cs = -*cs;
		*sn = -*sn;
		*r = -*r;
	}
}

void NUMapplyFactoredHouseholder (double **c, long rb, long re, long cb,
                                  long ce, double v[], long incv, double tau, int side) {
	long i, j, iv;
	double sum;

	Melder_assert ( (re - rb) >= 0 && (ce - cb) >= 0 && incv != 0);

	if (tau == 0) {
		return;
	}
	if (side == NUM_LEFT) {
		/*
			Form  Q * C:
			1. w := C' * v
			2. C := C - v * w'
		*/
		for (j = cb; j <= ce; j++) {
			for (sum = 0, iv = 1, i = rb; i <= re; i++, iv += incv) {
				sum += c[i][j] * v[iv];
			}
			sum *= tau;
			for (iv = 1, i = rb; i <= re; i++, iv += incv) {
				c[i][j] -= v[iv] * sum;
			}
		}
	} else { /* side == NUM_RIGHT */
		/*
			Form  C * Q
			1. w := C * v
			2. C := C - w * v'
		*/
		for (i = rb; i <= re; i++) {
			for (sum = 0, iv = 1, j = cb; j <= ce; j++, iv += incv) {
				sum += c[i][j] * v[iv];
			}
			sum *= tau;
			for (iv = 1, j = cb; j <= ce; j++, iv += incv) {
				c[i][j] -= sum * v[iv];
			}
		}
	}
}

void NUMapplyFactoredHouseholders (double **c, long rb, long re, long cb,
                                   long ce, double **v, long rbv, long rev, long cbv, long cev, long incv,
                                   double tau[], int side, int trans) {
	int left = side == NUM_LEFT, transpose = trans != NUM_NOTRANSPOSE;
	long mv = rev - rbv + 1, nv = cev - cbv + 1;
	long i_begin, i_end, i_inc, numberOfHouseholders, order_v;
	long m = re - rb + 1, n = ce - cb + 1;
	long rbc = rb, rec = re, cbc = cb, cec = ce;

	if (incv != 1) {	/* by column (QR) */
		numberOfHouseholders = mv > nv ? nv : mv - 1;
		order_v = mv;
	} else { /* by row (RQ) */
		numberOfHouseholders = nv > mv ? mv : nv - 1;
		order_v = nv;
	}

	Melder_assert (m > 0 && n > 0 && mv > 0 && nv > 0);
	Melder_assert (numberOfHouseholders <= MAX (m, n));
	Melder_assert ( (left && m == order_v) || (! left && n == order_v));

	if ( (left && ! transpose) || (! left && transpose)) {
		i_begin = numberOfHouseholders;
		i_end = 0;
		i_inc = -1;
	} else {
		i_begin = 1;
		i_end = numberOfHouseholders + 1;
		i_inc = 1;
	}

	long i = i_begin;
	while (i != i_end) {
		double save, *v1; long vr, vc;
		if (incv == 1) {
			vr = rev - i + 1;
			vc = cev - i + 1;
			if (left) {
				rec = re - i + 1;
			} else {
				cec = ce - i + 1;
			}
			v1 = v[vr];
		} else {
			vr = rbv + i - 1;
			vc = cbv + i - 1;
			if (left) {
				rbc = rb + i - 1;
			} else {
				cbc = cb + i - 1;
			}
			v1 = TOVEC (v[vr][vc]);
		}
		save = v[vr][vc]; v[vr][vc] = 1;
		NUMapplyFactoredHouseholder (c, rbc, rec, cbc, cec, v1, incv, tau[i],
		                             side);
		v[vr][vc] = save;

		i += i_inc;
	}
}

void NUMhouseholderQR (double **a, long rb, long re, long cb, long ce,
                       long lda, double tau[]) {
	long m = re - rb + 1, n = ce - cb + 1;
	long numberOfHouseholders = MIN (m, n);

	Melder_assert (numberOfHouseholders > 0);

	if (numberOfHouseholders == m) {
		tau[m] = 0; numberOfHouseholders--;
	}

	for (long i = 1; i <= numberOfHouseholders; i++) {
		long ri = rb + i - 1, ci = cb + i - 1;

		// Generate elementary reflector H(i) to annihilate "A(i+1:m,i)"

		NUMfindHouseholder (m - i + 1, &a[ri][ci], TOVEC (a[ri + 1][ci]), lda, &tau[i]);

		if (i < n) {
			// Apply H(i) to "A(i:m, i+1:n)" from the left

			double save = a[ri][ci];
			a[ri][ci] = 1;
			NUMapplyFactoredHouseholder (a, ri, re, cb + i, ce, TOVEC (a[ri][ci]), lda, tau[i], NUM_LEFT);
			a[ri][ci] = save;
		}
	}
}


void NUMhouseholderQRwithColumnPivoting (long m, long n, double **a, long lda, long pivot[], double tau[]) {
	long numberOfHouseholders = MIN (m, n);

	Melder_assert (numberOfHouseholders > 0);

	if (! pivot) {
		NUMhouseholderQR (a, 1, m, 1, n, lda, tau); return;
	}

	if (numberOfHouseholders == m) {
		tau[m] = 0; numberOfHouseholders--;
	}

	autoNUMvector<double> colnorm (1, 2 * n);
	long itmp = 1;
	for (long i = 1; i <= n; i++) {
		if (pivot[i] != 0) {
			if (i != itmp) {
				for (long j = 1; j <= m; j++) {
					double tmp = a[j][i];
					a[j][i] = a[j][itmp];
					a[j][itmp] = tmp;
				}
				pivot[i] = pivot[itmp]; pivot[itmp] = i;
			} else {
				pivot[i] = i;
			}
			itmp++;
		} else {
			pivot[i] = i;
		}
	}
	itmp--;

	// Compute the QR factorization and update remaining columns

	if (itmp > 0) {
		long ma = MIN (itmp, m);
		NUMhouseholderQR (a, 1, m, 1, ma, lda, tau);
		if (ma < n) NUMapplyFactoredHouseholders (a, 1, m, ma + 1, n, a, 1, m,
			        1, ma, lda, tau, NUM_LEFT, NUM_TRANSPOSE);
	}

	if (itmp >= numberOfHouseholders) {
		return;    // goto end;
	}

	// Initialize partial column norms. the first n elements of
	// colnorm store the exact column norms.

	for (long i = itmp + 1; i <= n; i++) {
		colnorm[n + i] = colnorm[i] = NUMnorm2 (m - itmp, TOVEC (a[itmp + 1][i]), lda);
	}

	// Compute factorization

	for (long i = itmp + 1; i <= numberOfHouseholders; i++) {
		// Determine ith pivot column and swap if necessary

		double max = colnorm[i]; long pvt = i;
		for (long j = i + 1; j <= n; j++) {
			if (fabs (colnorm[j]) > max) {
				max = fabs (colnorm[j]);
				pvt = j;
			}
		}

		if (pvt != i) {
			for (long j = 1; j <= m; j++) {
				double tmp = a[j][i];
				a[j][i] = a[j][pvt];
				a[j][pvt] = tmp;
			}

			itmp = pivot[pvt];
			pivot[pvt] = pivot[i];
			pivot[i] = itmp;
			colnorm[pvt] = colnorm[i];
			colnorm[n + pvt] = colnorm[n + i];
		}

		// Generate elementary reflector H(i)

		NUMfindHouseholder (m - i + 1, &a[i][i], TOVEC (a[i + 1][i]), lda, &tau[i]);

		if (i < n) {
			// Apply H(i) to A(i:m,i+1:n) from the left

			double save = a[i][i];
			a[i][i] = 1;
			NUMapplyFactoredHouseholder (a, i, m, i + 1, n, TOVEC (a[i][i]), lda, tau[i], NUM_LEFT);
			a[i][i] = save;
		}

		// Update partial column norms.

		for (long j = i + 1; j <= n; j++) {
			if (colnorm[j] == 0) {
				continue;
			}
			double t = a[i][j] / colnorm[j];
			double tmp = 1 - t * t;
			if (tmp < 0) {
				tmp = 0;
			}
			t = colnorm[j] / colnorm[n + j];
			if ( (1 + 0.05 * tmp * t * t) == 1) {
				if (m - i > 0) {
					colnorm[n + j] = colnorm[j] = NUMnorm2 (m - i, TOVEC (a[i + 1][j]), lda);
				} else {
					colnorm[j] = colnorm[n + j] = 0;
				}
			} else {
				colnorm[j] *= sqrt (tmp);
			}
		}
	}
}


void NUMhouseholderRQ (double **a, long rb, long re, long cb, long ce, double tau[]) {
	long m = re - rb + 1, n = ce - cb + 1;
	long numberOfHouseholders = MIN (m, n);

	Melder_assert (numberOfHouseholders > 0);

	if (numberOfHouseholders == n) {
		tau[n] = 0; numberOfHouseholders--;
	}

	for (long i = 1; i <= numberOfHouseholders; i++) {
		// Generate elementary reflector H(i) to annihilate "A(m-i+1,1:n-i)"

		long ri = re - i + 1, ci = ce - i + 1, order = n - i + 1;

		NUMfindHouseholder (order, &a[ri][ci], a[ri], 1, &tau[i]);

		if (i < m) {
			// Apply H(i) to "A(1:m-i,1:n-i+1)" from the right

			double save = a[ri][ci];
			a[ri][ci] = 1;
			NUMapplyFactoredHouseholder (a, rb, re - i, cb, ci, a[ri], 1,
			                             tau[i], NUM_RIGHT);
			a[ri][ci] = save;
		}
	}
}


void NUMparallelVectors (long n, double x[], long incx, double y[], long incy,
                         double *svmin) {
	double a11, c, svmax, tau;

	if (n <= 1) {
		*svmin = 0;
		return;
	}

	// Compute the QR factorization of the n-by-2 matrix ( x y )

	NUMfindHouseholder (n, &x[1], TOVEC (x[1 + incx]), incx, &tau);

	a11 = x[1];
	x[1] = 1;

	c = - tau * NUMdotproduct (n, x, incx, y, incy);
	NUMdaxpy (n, c, x, incx, y, incy);

	NUMfindHouseholder (n - 1, &y[1 + incy], TOVEC (y[1 + 2 * incy]), incy, &tau);

	// Compute the svd of 2-by-2 upper triangular matrix.

	NUMsvcmp22 (a11, y[1], y[1 + incy], svmin, &svmax);
}

void NUMsvdcmp22 (double f, double g, double h, double *svmin, double *svmax,
                  double *snr, double *csr, double *snl, double *csl) {
	double a, clt, crt, slt, srt, tsign;
	double ft = f, fa = fabs (f);
	double ht = h, ha = fabs (h);
	double gt = g, ga = fabs (g);

	long pmax = 1;	// pmax: maximum absolute entry of matrix (f=1, g=2, h=3).

	bool swap = ha > fa;
	if (swap) {
		double tmp = ft;
		ft = ht;
		ht = tmp;
		tmp = fa;
		fa = ha;
		ha = tmp;
		pmax = 3;
	}

	if (ga == 0) {
		// diagonal matrix

		*svmin = ha;
		*svmax = fa;
		clt = crt = 1;
		slt = srt = 0;
	} else {
		int gasmal = 1;
		if (ga > fa) {
			pmax = 2;
			if (! NUMfpp) {
				NUMmachar ();
			}
			if (fa / ga <= NUMfpp -> eps) {
				/* very large ga */
				gasmal = 0;
				*svmax = ga;
				*svmin = ha > 1 ? fa / (ga / ha) : (fa / ga) * ha;
				clt = 1;
				slt = ht / gt;
				srt = 1;
				crt = ft / gt;
			}
		}
		if (gasmal) {
			// normal case

			double m, mm, l, t, tt, r, s, d = fa - ha;

			l = d == fa ? 1 : d / fa;	/* copes with infinite f or h. */

			m = gt / ft;
			mm = m * m;
			t = 2 - l;
			tt = t * t;
			s = sqrt (tt + mm);
			r = l == 0 ? fabs (m) : sqrt (l * l + mm);
			a = 0.5 * (s + r);
			*svmin = ha / a;
			*svmax = fa * a;
			t = mm == 0 ? (l == 0 ? SIGN (2.0, ft) * SIGN (1.0, gt) :
			               gt / SIGN (d, ft) + m / t) : (m / (s + t) + m / (r + l)) * (1 + a);
			l = sqrt (t * t + 4);
			crt = 2 / l;
			srt = t / l;
			clt = (crt + srt * m) / a;
			slt = (ht / ft) * srt / a;
		}
	}
	if (swap) {
		*csl = srt;
		*snl = crt;
		*csr = slt;
		*snr = clt;
	} else {
		*csl = clt;
		*snl = slt;
		*csr = crt;
		*snr = srt;
	}

	// Correct the signs of svmin and svmax

	if (pmax == 1) {
		tsign = SIGN (1.0, *csr) * SIGN (1.0, *csl) * SIGN (1.0, f);
	} else if (pmax == 2) {
		tsign = SIGN (1.0, *snr) * SIGN (1.0, *csl) * SIGN (1.0, g);
	} else { /* pmax == 3 */
		tsign = SIGN (1.0, *snr) * SIGN (1.0, *snl) * SIGN (1.0, h);
	}

	*svmax = SIGN (*svmax, tsign);
	*svmin = SIGN (*svmin, tsign * SIGN (1.0, f) * SIGN (1.0, h));
}

/* Bai & Demmel, page 1472 */
void NUMgsvdcmp22 (int upper, int productsvd, double a1, double a2, double a3,
                   double b1, double b2, double b3, double *csu, double *snu, double *csv,
                   double *snv, double *csq, double *snq) {
	double a, aua11, aua12, aua21, aua22, avb12, avb11;
	double avb21, avb22, b, c, csl, csr, d, g, r, s1, s2;
	double snl, snr, ua11, ua11r, ua12, ua21, ua22;
	double ua22r, vb11, vb11r, vb12, vb21, vb22, vb22r;

	if (upper) {
		// Input matrices A and B are upper triangular matrices
		// Form matrix C = A * adj(B) =  ( a b )
		//							     ( 0 d )

		if (productsvd) {
			/*
				This is to simplistic, will not work.
				Form: C = A.B:
				a = a1 * b1;
				d = a3 * b3;
				b = a1 * b2 + a2 * b3;
				Form C = B.A
			*/
			a = a1 * b1;
			d = a3 * b3;
			b = a2 * b2 + a3 * b2;
		} else {
			a = a1 * b3;
			d = a3 * b1;
			b = a2 * b1 - a1 * b2;
		}

		/*
			the svd of real 2-by-2 triangular C

			( csl -snl )*( a b )*(  csr  snr ) = ( r 0 )
			( snl  csl ) ( 0 d ) ( -snr  csr )   ( 0 t )
		*/

		NUMsvdcmp22 (a, b, d, &s1, &s2, &snr, &csr, &snl, &csl);

		if (fabs (csl) >= fabs (snl) || fabs (csr) >= fabs (snr)) {
			// Compute the (1,1) and (1,2) elements of U' * A and V' * B,
			// and (1,2) element of |U|' * |A| and |V|' * |B|.

			ua11r = csl * a1;
			ua12 = csl * a2 + snl * a3;
			vb11r = csr * b1;
			vb12 = csr * b2 + snr * b3;
			aua12 = fabs (csl) * fabs (a2) + fabs (snl) * fabs (a3);
			avb12 = fabs (csr) * fabs (b2) + fabs (snr) * fabs (b3);
			g = fabs (ua11r) + fabs (ua12);

			// Zero (1,2) elements of U'*A and V' * B

			if (g != 0 && aua12 / g <= avb12 / (fabs (vb11r) + fabs (vb12))) {
				NUMfindGivens (-ua11r, ua12, csq, snq, &r);
			} else {
				NUMfindGivens (-vb11r, vb12, csq, snq, &r);
			}

			*csu = csl;
			*snu = -snl;
			*csv = csr;
			*snv = -snr;
		} else {
			/*
				Compute the (2,1) and (2,2) elements of U' * A and V' * B,
				and (2,2) element of |U|' * |A| and |V|' * |B|.
			*/
			ua21 = -snl * a1;
			ua22 = -snl * a2 + csl * a3;
			vb21 = -snr * b1;
			vb22 = -snr * b2 + csr * b3;
			aua22 = fabs (snl) * fabs (a2) + fabs (csl) * fabs (a3);
			avb22 = fabs (snr) * fabs (b2) + fabs (csr) * fabs (b3);
			g = fabs (ua21) + fabs (ua22);

			// Zero (2,2) elements of U' * A and V' * B, and then swap.

			if (g != 0 && aua22 / g <= avb22 / (fabs (vb21) + fabs (vb22))) {
				NUMfindGivens (-ua21, ua22, csq, snq, &r);
			} else {
				NUMfindGivens (-vb21, vb22, csq, snq, &r);
			}

			*csu = snl;
			*snu = csl;
			*csv = snr;
			*snv = csr;
		}
	} else {
		// Input matrices A and B are lower triangular matrices
		// Form matrix C = A * adj(B) = ( a 0 )
		//							    ( c d )

		if (productsvd) {
			a = a1 * b1;
			d = a3 * b3;
			c = a2 * b1 + a3 * b2;
		} else {
			a = a1 * b3;
			d = a3 * b1;
			c = a2 * b3 - a3 * b2;
		}

		// The svd of real 2-by-2 triangular C
		// ( csl -snl ) * ( a 0 ) * (  csr  snr ) = ( r 0 )
		// ( snl  csl )   ( c d )   ( -snr  csr )   ( 0 t )

		NUMsvdcmp22 (a, c, d, &s1, &s2, &snr, &csr, &snl, &csl);

		if (fabs (csr) >= fabs (snr) || fabs (csl) >= fabs (snl)) {
			// Compute the (2,1) and (2,2) elements of U' * A and V' * B,
			// and (2,1) element of |U|' * |A| and |V|' * |B|.

			ua21 = -snr * a1 + csr * a2;
			ua22r = csr * a3;
			vb21 = -snl * b1 + csl * b2;
			vb22r = csl * b3;
			aua21 = fabs (snr) * fabs (a1) + fabs (csr) * fabs (a2);
			avb21 = fabs (snl) * fabs (b1) + fabs (csl) * fabs (b2);
			g = fabs (ua21) + fabs (ua22r);

			// Zero (2,1) elements of U' * A and V' * B.

			if (g != 0 && aua21 / g <= avb21 / (fabs (vb21) + fabs (vb22r))) {
				NUMfindGivens (ua22r, ua21, csq, snq, &r);
			} else {
				NUMfindGivens (vb22r, vb21, csq, snq, &r);
			}

			*csu = csr;
			*snu = -snr;
			*csv = csl;
			*snv = -snl;
		} else {
			// Compute the (1,1) and (1,2) elements of U' * A and V' * B,
			// and (1,1) element of |U|' * |A| and |V|' * |B|.

			ua11 = csr * a1 + snr * a2;
			ua12 = snr * a3;
			vb11 = csl * b1 + snl * b2;
			vb12 = snl * b3;
			aua11 = fabs (csr) * fabs (a1) + fabs (snr) * fabs (a2);
			avb11 = fabs (csl) * fabs (b1) + fabs (snl) * fabs (b2);
			g = fabs (ua11) + fabs (ua12);

			// Zero (1,1) elements of U' * A and V' * B, and then swap.

			if (g != 0 && aua11 / g <= avb11 / (fabs (vb11) + fabs (vb12))) {
				NUMfindGivens (ua12, ua11, csq, snq, &r);
			} else {
				NUMfindGivens (vb12, vb11, csq, snq, &r);
			}

			*csu = snr;
			*snu = csr;
			*csv = snl;
			*snv = csl;
		}
	}
}


void NUMsvcmp22 (double f, double g, double h, double *svmin, double *svmax) {
	double as, at, au, c;

	double fa = fabs (f), ga = fabs (g), ha = fabs (h);
	double fhmn = MIN (fa, ha), fhmx = MAX (fa, ha);

	if (fhmn == 0) {
		*svmin = 0;

		if (fhmx == 0) {
			*svmax = ga;
		} else {
			double tmp = MIN (fhmx, ga) / MAX (fhmx, ga);
			*svmax = MAX (fhmx, ga) * sqrt (1 + tmp * tmp);
		}
	} else {
		if (ga < fhmx) {
			as = 1 + fhmn / fhmx;
			at = (fhmx - fhmn) / fhmx;
			au = ga / fhmx;
			au *= au;
			c = 2 / (sqrt (as * as + au) + sqrt (at * at + au));
			*svmin = fhmn * c;
			*svmax = fhmx / c;
		} else {
			au = fhmx / ga;
			if (au == 0) {
				// Avoid possible harmful underflow if exponent range
				// asymmetric (true svmin may not underflow even if au underflows)

				*svmin = (fhmn * fhmx) / ga;
				*svmax = ga;
			} else {
				double t1, t2;
				as = 1 + fhmn / fhmx;
				at = (fhmx - fhmn) / fhmx;
				t1 = as * au;
				t2 = at * au;
				c = 1 / (sqrt (1 + t1 * t1) + sqrt (1 + t2 * t2));
				*svmin = (fhmn * c) * au;
				*svmin += *svmin;
				*svmax = ga / (c + c);
			}
		}
	}
}

#define MAXIT 50

void NUMgsvdFromUpperTriangulars (double **a, long m, long n, double **b,
                                  long p, int product, long k, long l, double tola, double tolb,
                                  double *alpha, double *beta, double **u, double **v, double **q, long *ncycle) {
	int upper = 0; long iter, maxmn = MAX (m, n);
	double a1, a2, a3, b1, b2, b3, csq, csu, csv;
	double snq, snu, snv;

	Melder_assert (m > 0 && p > 0 && n > 0 && k <= maxmn && l <= maxmn);

	autoNUMvector<double> work (1, 2 * n);

	for (iter = 1; iter <= MAXIT; iter++) {
		upper = ! upper;

		for (long i = 1; i <= l - 1; i++) {
			for (long j = i + 1; j <= l; j++) {
				a1 = a2 = a3 = 0;
				if (k + i <= m) {
					a1 = a[k + i][n - l + i];
				}
				if (k + j <= m) {
					a3 = a[k + j][n - l + j];
				}

				b1 = b[i][n - l + i]; b3 = b[j][n - l + j];

				if (upper) {
					if (k + i <= m) {
						a2 = a[k + i][n - l + j];
					}
					b2 = b[i][n - l + j];
				} else {
					if (k + j <= m) {
						a2 = a[k + j][n - l + i];
					}
					b2 = b[j][n - l + i];
				}

				NUMgsvdcmp22 (upper, product, a1, a2, a3, b1, b2, b3,
				              &csu, &snu, &csv, &snv, &csq, &snq);

				// Update (k+i)-th and (k+j)-th rows of matrix A: U'*A

				if (k + j <= m) NUMplaneRotation (l, TOVEC (a[k + j][n - l + 1]), 1,
					                                  TOVEC (a[k + i][n - l + 1]), 1, csu, snu);

				// Update i-th and j-th rows of matrix B: V' * B

				NUMplaneRotation (l, TOVEC (b[j][n - l + 1]), 1, TOVEC (b[i][n - l + 1]),
				                  1, csv, snv);

				// Update (n-l+i)-th and (n-l+j)-th columns of matrices A and B: A * Q and B * Q

				NUMplaneRotation (MIN (k + l, m), TOVEC (a[1][n - l + j]), n, TOVEC (a[1][n - l + i]), n, csq, snq);

				NUMplaneRotation (l, TOVEC (b[1][n - l + j]), n, TOVEC (b[1][n - l + i]), n, csq, snq);

				if (upper) {
					if (k + i <= m) {
						a[k + i][n - l + j] = 0;
					}
					b[i][n - l + j] = 0;
				} else {
					if (k + j <= m) {
						a[k + j][n - l + i] = 0;
					}
					b[j][n - l + i] = 0;
				}

				// Update columns of orthogonal matrices U, V, Q, if desired.

				if (u && k + j <= m) {
					NUMplaneRotation (m, TOVEC (u[1][k + j]), m, TOVEC (u[1][k + i]), m, csu, snu);
				}

				if (v) {
					NUMplaneRotation (p, TOVEC (v[1][j]), p, TOVEC (v[1][i]), p, csv, snv);
				}

				if (q) {
					NUMplaneRotation (n, TOVEC (q[1][n - l + j]), n, TOVEC (q[1][n - l + i]), n, csq, snq);
				}
			}
		}
		if (! upper) {
			// The matrices A13 and B13 were lower triangular at the start
			//  of the cycle, and are now upper triangular.

			// Convergence test: test the parallelism of the corresponding rows of A and B.

			double error = 0, svmin;
			for (long i = 1; i <= MIN (l, m - k); i++) {
				long jj = n - l + i;
				for (long j = 1; j <= l - i + 1; j++, jj++) {
					work[j] = a[k + i][jj];
					work[l + j] = b[i][jj];
				}
				NUMparallelVectors (l - i + 1, work.peek(), 1, TOVEC (work[l + 1]), 1,
				                    &svmin);
				if (svmin > error) {
					error = svmin;
				}
			}
			if (fabs (error) <=  MIN (tola, tolb) * n) {
				break;
			}
		}
	}

	// Has the algorithm converged after maxit cycles?

	if (iter == MAXIT + 1) {
		*ncycle = MAXIT;
		Melder_throw (U"No convergence after ", MAXIT, U" iterations.");
	}

	*ncycle = iter;


	// Compute the generalized singular value pairs (alpha, beta), and
	//  set the triangular matrix R to matrix a.

	for (long i = 1; i <= k; i++) {
		alpha[i] = 1;
		beta[i] = 0;
	}

	for (long i = 1; i <= MIN (l, m - k); i++) {
		a1 = a[k + i][n - l + i];
		b1 = b[i][n - l + i];

		if (a1 != 0) {
			double gamma = b1 / a1, rwk;
			if (gamma < 0) {
				/*
					Change sign of i-th row of B and i-th column of V:
						B(i, n-l+i:n) := - B(i, n-l+i:n)
						V(1:p, i) := - V(1:p, i)
				*/

				for (long j = n - l + i; j <= n; j++) {
					b[i][j] = -b[i][j];
				}
				if (v) for (long j = 1; j <= p; j++) {
						v[j][i] = - v[j][i];
					}
			}

			NUMfindGivens (fabs (gamma), 1, &beta[k + i], &alpha[k + i], &rwk);

			if (alpha[k + i] >= beta[k + i]) {
				for (long j = n - l + i; j <= n; j++) {
					a[k + i][j] /= alpha[k + i];
				}
			} else {
				for (long j = n - l + i; j <= n; j++) {
					b[i][j] /= beta[k + i];
					a[k + i][j] = b[i][j];
				}
			}
		} else {
			alpha[k + i] = 0;
			beta[k + i] = 1;
			for (long j = n - l + i; j <= n; j++) {
				a[k + i][j] = b[i][j];
			}
		}
	}

	// Post-assignment

	for (long i = m + 1; i <= k + l; i++) {
		alpha[i] = 0;
		beta[i] = 1;
	}
	if (k + l < n) {
		for (long i = k + l + 1; i <= n; i++) {
			alpha[i] = beta[i] = 0;
		}
	}
}


void NUMmatricesToUpperTriangularForms (double **a, long m, long n, double **b,
                                        long p, double tola, double tolb, long *kk, long *ll, double **u,
                                        double **v, double **q) {
	int forward = 1;
	long i, j, k = 0, l = 0, lda = n, ldb = n;

	Melder_assert (m > 0 && p > 0 && n > 0);

	*kk = *ll = 0;

	autoNUMvector<long> pivot (1, n);
	autoNUMvector<double> tau (1, n);

	if (v) {
		NUMidentity (v, 1, p, 1);
	}
	if (u) {
		NUMidentity (u, 1, m, 1);
	}
	if (q) {
		NUMidentity (q, 1, n, 1);
	}

	/*
		QR with column pivoting of B: B * P = V * ( B11 B12 )
											      (  0   0  ),
		where V = H(1)H(2)...H(min(p,n))
	*/

	NUMhouseholderQRwithColumnPivoting (p, n, b, ldb, pivot.peek(), tau.peek());

	if (v) NUMapplyFactoredHouseholders (v, 1, p, 1, p, b, 1, p, 1, n, ldb,
		                                     tau.peek(), NUM_RIGHT, NUM_NOTRANSPOSE);

	if (q) {
		NUMpermuteColumns (forward, n, n, q, pivot.peek());
	}

	// Update A := A * P

	NUMpermuteColumns (forward, m, n, a, pivot.peek());

	// Determine the effective rank of matrix B and clean up B.

	for (l = 0, i = 1; i <= MIN (p, n); i++) {
		if (fabs (b[i][i]) > tolb) {
			l++;
		}
	}

	for (i = 2; i <= p; i++) {
		for (j = 1; j <= (i <= l ? i - 1 : n); j++) {
			b[i][j] = 0;
		}
	}

	if (p >= l && n != l) {
		// RQ factorization of (B11 B12): ( B11 B12 ) = ( 0 B12 ) * Z

		NUMhouseholderRQ (b, 1, l, 1, n, tau.peek());

		// Update A := A * Z' and Q := Q * Z'
		// Householder vectors are stored in rows of B.

		NUMapplyFactoredHouseholders (a, 1, m, 1, n, b, 1, l, 1, n, 1,
		                              tau.peek(), NUM_RIGHT, NUM_NOTRANSPOSE);
		if (q) NUMapplyFactoredHouseholders (q, 1, n, 1, n, b, 1, l, 1, n, 1,
			                                     tau.peek(), NUM_RIGHT, NUM_NOTRANSPOSE);

		// Clean up B

		for (i = 1; i <= l; i++) {
			for (j = 1; j <= n - l + i - 1; j++) {
				b[i][j] = 0;
			}
		}
	}

	/*
		We are done with B, now proceed with A.

		Let              n-l     l
					A = ( A11    A12 ) m,

		then the following does the complete QR decomposition of A11:

				A11 = U * (  0  T12 ) * P1'
						  (  0   0  )
	*/

	if (n - l > 0) {
		for (i = 1; i <= n - l; i++) {
			pivot[i] = 0;
		}

		NUMhouseholderQRwithColumnPivoting (m, n - l, a, lda, pivot.peek(), tau.peek());

		// Determine the effective rank of A11

		for (k = 0, i = 1; i <= MIN (m, n - l); i++) {
			if (fabs (a[i][i]) > tola) {
				k++;
			}
		}

		// Update A12 := U' * A12, where A12 = A (1:m, n-l+1:n)

		NUMapplyFactoredHouseholders (a, 1, m, n - l + 1, n, a, 1, m, 1, n - l,
		                              lda, tau.peek(), NUM_LEFT, NUM_TRANSPOSE);

		if (u) NUMapplyFactoredHouseholders (u, 1, m, 1, m, a, 1, m, 1, n - l,
			                                     lda, tau.peek(), NUM_RIGHT, NUM_NOTRANSPOSE);

		// Update Q(1:n, 1:n-l)  = Q(1:n, 1:n-l) * P1

		if (q) {
			NUMpermuteColumns (forward, n, n - l, q, pivot.peek());
		}

		// Clean up A: set the strictly lower triangular part of
		// A(1:k, 1:k) = 0, and A(k+1:m, 1:n-l) = 0.

		for (i = 2; i <= k; i++) {
			for (j = 1; j <= i - 1; j++) {
				a[i][j] = 0;
			}
		}
		for (i = k + 1; i <= m; i++) {
			for (j = 1; j <= n - l; j++) {
				a[i][j] = 0;
			}
		}
	}
	if (n - l > k) {
		// RQ factorization of ( T11 T12 ) = ( 0 T12 ) * Z1

		NUMhouseholderRQ (a, 1, k, 1, n - l, tau.peek());

		// Update Q(1:n,1:n-l) = Q(1:n,1:n-l ) * Z1'
		// djmw: was transpose instead of no_transpose

		if (q) NUMapplyFactoredHouseholders (q, 1, n, 1, n - l, a, 1, k, 1, n -
			                                     l, 1, tau.peek(), NUM_RIGHT, NUM_NOTRANSPOSE);

		// Clean up A

		for (i = 1; i <= k; i++) {
			for (j = 1; j <= n - l - k + i - 1; j++) {
				a[i][j] = 0;
			}
		}
	}

	if (m > k) {
		// QR factorization of A (k+1:m, n-l+1:n)

		NUMhouseholderQR (a, k + 1, m, n - l + 1, n, lda, tau.peek());

		// Update U(:,k+1:m) := U(:,k+1:m)*U1

		if (u) NUMapplyFactoredHouseholders (u, 1, m, k + 1, m, a, k + 1, m,
			                                     n - l + 1, n, lda, tau.peek(), NUM_RIGHT, NUM_NOTRANSPOSE);

		// Clean up

		for (j = n - l + 1; j <= n; j++) {
			for (i = j - n + k + l + 1; i <= m; i++) {
				a[i][j] = 0;
			}
		}
	}

	*kk = k; *ll = l;
}


void NUMgsvdcmp (double **a, long m, long n, double **b, long p, int productsvd,
                 long *k, long *l, double *alpha, double *beta, double **u, double **v, double **q, int invertR) {
	long ncycle;

	Melder_assert (m > 0 && n > 0 && p > 0);

	/*
		Get machine precision and set up threshold for determining
		the effective numerical rank of the matrices a and b.
	*/

	if (! NUMfpp) {
		NUMmachar ();
	}

	double anorm = NUMfrobeniusnorm (m, n, a);
	double bnorm = NUMfrobeniusnorm (p, n, b);

	if (anorm == 0 || bnorm == 0) {
		Melder_throw (U"NUMgsvdcmp: empty matrix.");
	}

	double tola = MAX (m, n) * MAX (anorm, NUMfpp -> sfmin) * NUMfpp -> prec;
	double tolb = MAX (p, n) * MAX (bnorm, NUMfpp -> sfmin) * NUMfpp -> prec;

	NUMmatricesToUpperTriangularForms (a, m, n, b, p, tola, tolb, k, l, u, v, q);

	NUMgsvdFromUpperTriangulars (a, m, n, b, p, productsvd, *k, *l, tola,
	                             tolb, alpha, beta, u, v, q, &ncycle);

	if (q && invertR) {
		long i, j, ki, nr = *k + *l;
		int upper, unitDiagonal;

		autoNUMmatrix<double> r (1, nr, 1, nr);
		autoNUMvector<double> norms (1, *l);
		autoNUMvector<double> xrow (n - *k - *l + 1, n);

		if (m - *k - *l >= 0) {
			/*
							 n-k-l  k    l
				( 0 R ) = k (  0   R11  R12 )
						  l (  0    0   R22 )

				R is stored in A (1:k+l, n-k-l+1:n).
			*/

			for (i = 1; i <= nr; i++) {
				for (j = i; j <= nr; j++) {
					r[i][j] = a[i][n - *k - *l + j];
				}
			}
		} else {
			/*
								 n-k-l  k   m-k  k+l-m
				( 0 R ) =     k ( 0    R11  R12  R13  )
							m-k ( 0     0   R22  R23  )
						  k+l-m ( 0     0    0   R33  )

				(R11 R12 R13 ) is stored in A (1:m, n-k-l+1:n), and R33 is
				stored ( 0  R22 R23 ) in B (m-k+1:l, n+m-k-l+1:n).
			*/

			for (i = 1; i <= m; i++) {
				for (j = i; j <= nr; j++) {
					r[i][j] = a[i][n - *k - *l + j];
				}
			}
			for (i = m + 1; i <= nr; i++) {
				for (j = i; j <= nr; j++) {
					r[i][j] = b[i - *k][n - *k - *l + j];
				}
			}
		}

		/*
			Form X = Q * ( I   0    )
					     ( 0 inv(R) ) and store result in Q.
		*/

		NUMtriangularInverse ( (upper = 1), (unitDiagonal = 0), nr, r.peek());

		for (i = 1; i <= n; i++) {
			for (j = n - *k - *l + 1; j <= n; j++) {
				xrow[j] = 0;
				for (ki = n - *k - *l + 1; ki <= j; ki++) {
					xrow[j] += q[i][ki] * r[ki][j];
				}
			}
			for (j = n - *k - *l + 1; j <= n; j++) {
				q[i][j] = xrow[j];
			}
		}

		/*
			Get norms of eigenvectors.
		*/

		for (i = 1; i <= *l; i++) {
			norms[i] = NUMnorm2 (n, TOVEC (q[1][*k + i]), n);
			printf ("%f ", norms[i]);
		}
	}
}

void NUMtriangularInverse (int upper, int unitDiagonal, long n, double **a) {
	long j, i, k;
	double ajj, tmp;

	Melder_assert (n > 0);

	if (upper) {
		for (j = 1; j <= n; j++) {
			if (! unitDiagonal) {
				a[j][j] = 1 / a[j][j];
				ajj = -a[j][j];
			} else {
				ajj = -1;
			}
			/*
				Compute elements 1:j-1 of j-th column.
			*/
			for (k = 1; k <= j - 1; k++) {
				if (a[k][j] == 0) {
					continue;
				}
				tmp = a[k][j];
				for (i = 1; i <= k - 1; i++) {
					a[i][j] += tmp * a[i][k];
				}
				if (! unitDiagonal) {
					a[k][j] *= a[k][k];
				}
			}
			for (k = 1; k <= j - 1; k++) {
				a[k][j] *= ajj;
			}
		}
	} else {
		for (j = n; j >= 1; j--) {
			if (! unitDiagonal) {
				a[j][j] = 1 / a[j][j];
				ajj = -a[j][j];
			} else {
				ajj = -1;
			}
			if (j < n) {
				/*
					Compute elements j+1:n of j-th column.
				*/
				for (k = n; k >= j + 1; k--) {
					if (a[k][j] == 0) {
						continue;
					}
					tmp = a[k][j];
					for (i = n; i >= k + 1; i--) {
						a[i][j] += tmp * a[i][k];
					}
					if (! unitDiagonal) {
						a[k][j] *= a[k][k];
					}
				}
				for (k = 1; k <= n - j; k++) {
					a[j + k][j] *= ajj;
				}
			}
		}
	}
}

void NUMeigencmp22 (double a, double b, double c, double *rt1, double *rt2,
                    double *cs1, double *sn1) {
	long sgn1, sgn2;
	double ab, acmn, acmx, acs, adf, cs, ct, df, rt, sm, tb, tn;

	sm = a + c;
	df = a - c;
	adf = fabs (df);
	tb = b + b;
	ab = fabs (tb);
	if (fabs (a) > fabs (c)) {
		acmx = a;
		acmn = c;
	} else {
		acmx = c;
		acmn = a;
	}
	if (adf > ab) {
		tn = ab / adf;
		rt = adf * sqrt (1 + tn * tn);
	} else if (adf < ab) {
		tn = adf / ab;
		rt = ab * sqrt (1 + tn * tn);
	} else {
		rt = ab * sqrt (2);
	}

	if (sm < 0) {
		*rt1 = 0.5 * (sm - rt);
		sgn1 = -1;
		/*
			Order of execution important.
			To get fully accurate smaller eigenvalue,
			next line needs to be executed in higher precision.
		*/
		*rt2 = (acmx / *rt1) * acmn - (b / *rt1) * b;
	} else if (sm > 0) {
		*rt1 = 0.5 * (sm + rt);
		sgn1 = 1;
		/*
			Order of execution important.
			To get fully accurate smaller eigenvalue,
			next line needs to be executed in higher precision.
		*/
		*rt2 = (acmx / *rt1) * acmn - (b / *rt1) * b;
	} else {
		*rt1 = 0.5 * rt;
		*rt2 = -0.5 * rt;
		sgn1 = 1;
	}

	/*
		Compute the eigenvector
	*/

	if (df >= 0) {
		cs = df + rt;
		sgn2 = 1;
	} else {
		cs = df - rt;
		sgn2 = -1;
	}
	acs = fabs (cs);
	if (acs > ab) {
		ct = -tb / cs;
		*sn1 = 1 / sqrt (1 + ct * ct);
		*cs1 = ct * *sn1;
	} else {
		if (ab == 0) {
			*cs1 = 1;
			*sn1 = 0;
		} else {
			tn = -cs / tb;
			*cs1 = 1 / sqrt (1 + tn * tn);
			*sn1 = tn * *cs1;
		}
	}
	if (sgn1 == sgn2) {
		tn = *cs1;
		*cs1 = -*sn1;
		*sn1 = tn;
	}
}

/* End of file NUMlapack.cpp */
