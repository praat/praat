/* NUMclapack.c */

/* -- LAPACK driver routines (version 3.0) -- Univ. of Tennessee, Univ. of
   California Berkeley, NAG Ltd., Courant Institute, Argonne National Lab,
   and Rice University October 31, 1999 -- translated by f2c (version
   19990503).

	Adapted by David Weenink 20021201

 djmw 20030205 Latest modification
*/
/* #include "blaswrap.h" */
#include "NUMf2c.h"
#include "NUMclapack.h"
#include "NUMcblas.h"
#include "NUM2.h"
#include "melder.h"

/* Table of constant values */

static long c__0 = 0;
static long c__1 = 1;
static long c_n1 = -1;
static long c__2 = 2;
static long c__3 = 3;
static long c__4 = 4;
static long c__6 = 6;
static long c__10 = 10;
static long c__11 = 11;
static double c_b15 = -.125;
static double c_b49 = 1.;
static double c_b72 = -1.;
static double c_b74 = 0.;
static double c_b108 = 1.;
static double c_b416 = 0.;
static double c_b438 = 1.;

#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))

#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define c___ref(a_1,a_2) c__[(a_2)*c_dim1 + a_1]

/* --------------------------------------------------- */

#define u_ref(a_1,a_2) u[(a_2)*u_dim1 + a_1]
#define vt_ref(a_1,a_2) vt[(a_2)*vt_dim1 + a_1]

int NUMlapack_dbdsqr (const char *uplo, long *n, long *ncvt, long *nru, long *ncc, double *d__, double *e, double *vt,
                      long *ldvt, double *u, long *ldu, double *c__, long *ldc, double *work, long *info) {
	/* System generated locals */
	long c_dim1, c_offset, u_dim1, u_offset, vt_dim1, vt_offset, i__1, i__2;
	double d__1, d__2, d__3, d__4;

	/* Local variables */
	static double abse;
	static long idir;
	static double abss;
	static long oldm;
	static double cosl;
	static long isub, iter;
	static double unfl, sinl, cosr, smin, smax, sinr;
	static double f, g, h__;
	static long i__, j, m;
	static double r__;
	static double oldcs;
	static long oldll;
	static double shift, sigmn, oldsn;
	static long maxit;
	static double sminl, sigmx;
	static long lower;
	static double cs;
	static long ll;
	static double sn, mu;
	static double sminoa, thresh;
	static long rotate;
	static double sminlo;
	static long nm1;
	static double tolmul;
	static long nm12, nm13, lll;
	static double eps, sll, tol;

	/* Parameter adjustments */
	--d__;
	--e;
	vt_dim1 = *ldvt;
	vt_offset = 1 + vt_dim1 * 1;
	vt -= vt_offset;
	u_dim1 = *ldu;
	u_offset = 1 + u_dim1 * 1;
	u -= u_offset;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1 * 1;
	c__ -= c_offset;
	--work;

	/* Function Body */
	*info = 0;
	lower = lsame_ (uplo, "L");
	if (!lsame_ (uplo, "U") && !lower) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*ncvt < 0) {
		*info = -3;
	} else if (*nru < 0) {
		*info = -4;
	} else if (*ncc < 0) {
		*info = -5;
	} else if (*ncvt == 0 && *ldvt < 1 || *ncvt > 0 && *ldvt < MAX (1, *n)) {
		*info = -9;
	} else if (*ldu < MAX (1, *nru)) {
		*info = -11;
	} else if (*ncc == 0 && *ldc < 1 || *ncc > 0 && *ldc < MAX (1, *n)) {
		*info = -13;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DBDSQR", &i__1);
		return 0;
	}
	if (*n == 0) {
		return 0;
	}
	if (*n == 1) {
		goto L160;
	}

	/* ROTATE is true if any singular vectors desired, false otherwise */

	rotate = *ncvt > 0 || *nru > 0 || *ncc > 0;

	/* If no singular vectors desired, use qd algorithm */

	if (!rotate) {
		NUMlapack_dlasq1 (n, &d__[1], &e[1], &work[1], info);
		return 0;
	}

	nm1 = *n - 1;
	nm12 = nm1 + nm1;
	nm13 = nm12 + nm1;
	idir = 0;

	/* Get machine constants */

	eps = NUMblas_dlamch ("Epsilon");
	unfl = NUMblas_dlamch ("Safe minimum");

	/* If matrix lower bidiagonal, rotate to be upper bidiagonal by applying
	   Givens rotations on the left */

	if (lower) {
		i__1 = *n - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {
			NUMlapack_dlartg (&d__[i__], &e[i__], &cs, &sn, &r__);
			d__[i__] = r__;
			e[i__] = sn * d__[i__ + 1];
			d__[i__ + 1] = cs * d__[i__ + 1];
			work[i__] = cs;
			work[nm1 + i__] = sn;
			/* L10: */
		}

		/* Update singular vectors if desired */

		if (*nru > 0) {
			NUMlapack_dlasr ("R", "V", "F", nru, n, &work[1], &work[*n], &u[u_offset], ldu);
		}
		if (*ncc > 0) {
			NUMlapack_dlasr ("L", "V", "F", n, ncc, &work[1], &work[*n], &c__[c_offset], ldc);
		}
	}

	/* Compute singular values to relative accuracy TOL (By setting TOL to be
	   negative, algorithm will compute singular values to absolute accuracy
	   ABS(TOL)*norm(input matrix))

	   Computing MAX Computing MIN */
	d__3 = 100., d__4 = pow (eps, c_b15);
	d__1 = 10., d__2 = MIN (d__3, d__4);
	tolmul = MAX (d__1, d__2);
	tol = tolmul * eps;

	/* Compute approximate maximum, minimum singular values */

	smax = 0.;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		/* Computing MAX */
		d__2 = smax, d__3 = (d__1 = d__[i__], fabs (d__1));
		smax = MAX (d__2, d__3);
		/* L20: */
	}
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
		/* Computing MAX */
		d__2 = smax, d__3 = (d__1 = e[i__], fabs (d__1));
		smax = MAX (d__2, d__3);
		/* L30: */
	}
	sminl = 0.;
	if (tol >= 0.) {

		/* Relative accuracy desired */

		sminoa = fabs (d__[1]);
		if (sminoa == 0.) {
			goto L50;
		}
		mu = sminoa;
		i__1 = *n;
		for (i__ = 2; i__ <= i__1; ++i__) {
			mu = (d__2 = d__[i__], fabs (d__2)) * (mu / (mu + (d__1 = e[i__ - 1], fabs (d__1))));
			sminoa = MIN (sminoa, mu);
			if (sminoa == 0.) {
				goto L50;
			}
			/* L40: */
		}
L50:
		sminoa /= sqrt ( (double) (*n));
		/* Computing MAX */
		d__1 = tol * sminoa, d__2 = *n * 6 * *n * unfl;
		thresh = MAX (d__1, d__2);
	} else {

		/* Absolute accuracy desired

		   Computing MAX */
		d__1 = fabs (tol) * smax, d__2 = *n * 6 * *n * unfl;
		thresh = MAX (d__1, d__2);
	}

	/* Prepare for main iteration loop for the singular values (MAXIT is the
	   maximum number of passes through the inner loop permitted before
	   nonconvergence signalled.) */

	maxit = *n * 6 * *n;
	iter = 0;
	oldll = -1;
	oldm = -1;

	/* M points to last element of unconverged part of matrix */

	m = *n;

	/* Begin main iteration loop */

L60:

	/* Check for convergence or exceeding iteration count */

	if (m <= 1) {
		goto L160;
	}
	if (iter > maxit) {
		goto L200;
	}

	/* Find diagonal block of matrix to work on */

	if (tol < 0. && (d__1 = d__[m], fabs (d__1)) <= thresh) {
		d__[m] = 0.;
	}
	smax = (d__1 = d__[m], fabs (d__1));
	smin = smax;
	i__1 = m - 1;
	for (lll = 1; lll <= i__1; ++lll) {
		ll = m - lll;
		abss = (d__1 = d__[ll], fabs (d__1));
		abse = (d__1 = e[ll], fabs (d__1));
		if (tol < 0. && abss <= thresh) {
			d__[ll] = 0.;
		}
		if (abse <= thresh) {
			goto L80;
		}
		smin = MIN (smin, abss);
		/* Computing MAX */
		d__1 = MAX (smax, abss);
		smax = MAX (d__1, abse);
		/* L70: */
	}
	ll = 0;
	goto L90;
L80:
	e[ll] = 0.;

	/* Matrix splits since E(LL) = 0 */

	if (ll == m - 1) {

		/* Convergence of bottom singular value, return to top of loop */

		--m;
		goto L60;
	}
L90:
	++ll;

	/* E(LL) through E(M-1) are nonzero, E(LL-1) is zero */

	if (ll == m - 1) {

		/* 2 by 2 block, handle separately */

		NUMlapack_dlasv2 (&d__[m - 1], &e[m - 1], &d__[m], &sigmn, &sigmx, &sinr, &cosr, &sinl, &cosl);
		d__[m - 1] = sigmx;
		e[m - 1] = 0.;
		d__[m] = sigmn;

		/* Compute singular vectors, if desired */

		if (*ncvt > 0) {
			NUMblas_drot (ncvt, &vt_ref (m - 1, 1), ldvt, &vt_ref (m, 1), ldvt, &cosr, &sinr);
		}
		if (*nru > 0) {
			NUMblas_drot (nru, &u_ref (1, m - 1), &c__1, &u_ref (1, m), &c__1, &cosl, &sinl);
		}
		if (*ncc > 0) {
			NUMblas_drot (ncc, &c___ref (m - 1, 1), ldc, &c___ref (m, 1), ldc, &cosl, &sinl);
		}
		m += -2;
		goto L60;
	}

	/* If working on new submatrix, choose shift direction (from larger end
	   diagonal element towards smaller) */

	if (ll > oldm || m < oldll) {
		if ( (d__1 = d__[ll], fabs (d__1)) >= (d__2 = d__[m], fabs (d__2))) {

			/* Chase bulge from top (big end) to bottom (small end) */

			idir = 1;
		} else {

			/* Chase bulge from bottom (big end) to top (small end) */

			idir = 2;
		}
	}

	/* Apply convergence tests */

	if (idir == 1) {

		/* Run convergence test in forward direction First apply standard
		   test to bottom of matrix */

		if ( (d__2 = e[m - 1], fabs (d__2)) <= fabs (tol) * (d__1 = d__[m], fabs (d__1)) || tol < 0. &&
		        (d__3 = e[m - 1], fabs (d__3)) <= thresh) {
			e[m - 1] = 0.;
			goto L60;
		}

		if (tol >= 0.) {

			/* If relative accuracy desired, apply convergence criterion
			   forward */

			mu = (d__1 = d__[ll], fabs (d__1));
			sminl = mu;
			i__1 = m - 1;
			for (lll = ll; lll <= i__1; ++lll) {
				if ( (d__1 = e[lll], fabs (d__1)) <= tol * mu) {
					e[lll] = 0.;
					goto L60;
				}
				sminlo = sminl;
				mu = (d__2 = d__[lll + 1], fabs (d__2)) * (mu / (mu + (d__1 = e[lll], fabs (d__1))));
				sminl = MIN (sminl, mu);
				/* L100: */
			}
		}

	} else {

		/* Run convergence test in backward direction First apply standard
		   test to top of matrix */

		if ( (d__2 = e[ll], fabs (d__2)) <= fabs (tol) * (d__1 = d__[ll], fabs (d__1)) || tol < 0. &&
		        (d__3 = e[ll], fabs (d__3)) <= thresh) {
			e[ll] = 0.;
			goto L60;
		}

		if (tol >= 0.) {

			/* If relative accuracy desired, apply convergence criterion
			   backward */

			mu = (d__1 = d__[m], fabs (d__1));
			sminl = mu;
			i__1 = ll;
			for (lll = m - 1; lll >= i__1; --lll) {
				if ( (d__1 = e[lll], fabs (d__1)) <= tol * mu) {
					e[lll] = 0.;
					goto L60;
				}
				sminlo = sminl;
				mu = (d__2 = d__[lll], fabs (d__2)) * (mu / (mu + (d__1 = e[lll], fabs (d__1))));
				sminl = MIN (sminl, mu);
				/* L110: */
			}
		}
	}
	oldll = ll;
	oldm = m;

	/* Compute shift.  First, test if shifting would ruin relative accuracy,
	   and if so set the shift to zero.

	   Computing MAX */
	d__1 = eps, d__2 = tol * .01;
	if (tol >= 0. && *n * tol * (sminl / smax) <= MAX (d__1, d__2)) {

		/* Use a zero shift to avoid loss of relative accuracy */

		shift = 0.;
	} else {

		/* Compute the shift from 2-by-2 block at end of matrix */

		if (idir == 1) {
			sll = (d__1 = d__[ll], fabs (d__1));
			NUMlapack_dlas2 (&d__[m - 1], &e[m - 1], &d__[m], &shift, &r__);
		} else {
			sll = (d__1 = d__[m], fabs (d__1));
			NUMlapack_dlas2 (&d__[ll], &e[ll], &d__[ll + 1], &shift, &r__);
		}

		/* Test if shift negligible, and if so set to zero */

		if (sll > 0.) {
			/* Computing 2nd power */
			d__1 = shift / sll;
			if (d__1 * d__1 < eps) {
				shift = 0.;
			}
		}
	}

	/* Increment iteration count */

	iter = iter + m - ll;

	/* If SHIFT = 0, do simplified QR iteration */

	if (shift == 0.) {
		if (idir == 1) {

			/* Chase bulge from top to bottom Save cosines and sines for
			   later singular vector updates */

			cs = 1.;
			oldcs = 1.;
			i__1 = m - 1;
			for (i__ = ll; i__ <= i__1; ++i__) {
				d__1 = d__[i__] * cs;
				NUMlapack_dlartg (&d__1, &e[i__], &cs, &sn, &r__);
				if (i__ > ll) {
					e[i__ - 1] = oldsn * r__;
				}
				d__1 = oldcs * r__;
				d__2 = d__[i__ + 1] * sn;
				NUMlapack_dlartg (&d__1, &d__2, &oldcs, &oldsn, &d__[i__]);
				work[i__ - ll + 1] = cs;
				work[i__ - ll + 1 + nm1] = sn;
				work[i__ - ll + 1 + nm12] = oldcs;
				work[i__ - ll + 1 + nm13] = oldsn;
				/* L120: */
			}
			h__ = d__[m] * cs;
			d__[m] = h__ * oldcs;
			e[m - 1] = h__ * oldsn;

			/* Update singular vectors */

			if (*ncvt > 0) {
				i__1 = m - ll + 1;
				NUMlapack_dlasr ("L", "V", "F", &i__1, ncvt, &work[1], &work[*n], &vt_ref (ll, 1), ldvt);
			}
			if (*nru > 0) {
				i__1 = m - ll + 1;
				NUMlapack_dlasr ("R", "V", "F", nru, &i__1, &work[nm12 + 1], &work[nm13 + 1], &u_ref (1, ll),
				                 ldu);
			}
			if (*ncc > 0) {
				i__1 = m - ll + 1;
				NUMlapack_dlasr ("L", "V", "F", &i__1, ncc, &work[nm12 + 1], &work[nm13 + 1], &c___ref (ll, 1),
				                 ldc);
			}

			/* Test convergence */

			if ( (d__1 = e[m - 1], fabs (d__1)) <= thresh) {
				e[m - 1] = 0.;
			}

		} else {

			/* Chase bulge from bottom to top Save cosines and sines for
			   later singular vector updates */

			cs = 1.;
			oldcs = 1.;
			i__1 = ll + 1;
			for (i__ = m; i__ >= i__1; --i__) {
				d__1 = d__[i__] * cs;
				NUMlapack_dlartg (&d__1, &e[i__ - 1], &cs, &sn, &r__);
				if (i__ < m) {
					e[i__] = oldsn * r__;
				}
				d__1 = oldcs * r__;
				d__2 = d__[i__ - 1] * sn;
				NUMlapack_dlartg (&d__1, &d__2, &oldcs, &oldsn, &d__[i__]);
				work[i__ - ll] = cs;
				work[i__ - ll + nm1] = -sn;
				work[i__ - ll + nm12] = oldcs;
				work[i__ - ll + nm13] = -oldsn;
				/* L130: */
			}
			h__ = d__[ll] * cs;
			d__[ll] = h__ * oldcs;
			e[ll] = h__ * oldsn;

			/* Update singular vectors */

			if (*ncvt > 0) {
				i__1 = m - ll + 1;
				NUMlapack_dlasr ("L", "V", "B", &i__1, ncvt, &work[nm12 + 1], &work[nm13 + 1], &vt_ref (ll, 1),
				                 ldvt);
			}
			if (*nru > 0) {
				i__1 = m - ll + 1;
				NUMlapack_dlasr ("R", "V", "B", nru, &i__1, &work[1], &work[*n], &u_ref (1, ll), ldu);
			}
			if (*ncc > 0) {
				i__1 = m - ll + 1;
				NUMlapack_dlasr ("L", "V", "B", &i__1, ncc, &work[1], &work[*n], &c___ref (ll, 1), ldc);
			}

			/* Test convergence */

			if ( (d__1 = e[ll], fabs (d__1)) <= thresh) {
				e[ll] = 0.;
			}
		}
	} else {

		/* Use nonzero shift */

		if (idir == 1) {

			/* Chase bulge from top to bottom Save cosines and sines for
			   later singular vector updates */

			f = ( (d__1 = d__[ll], fabs (d__1)) - shift) * (d_sign (&c_b49, &d__[ll]) + shift / d__[ll]);
			g = e[ll];
			i__1 = m - 1;
			for (i__ = ll; i__ <= i__1; ++i__) {
				NUMlapack_dlartg (&f, &g, &cosr, &sinr, &r__);
				if (i__ > ll) {
					e[i__ - 1] = r__;
				}
				f = cosr * d__[i__] + sinr * e[i__];
				e[i__] = cosr * e[i__] - sinr * d__[i__];
				g = sinr * d__[i__ + 1];
				d__[i__ + 1] = cosr * d__[i__ + 1];
				NUMlapack_dlartg (&f, &g, &cosl, &sinl, &r__);
				d__[i__] = r__;
				f = cosl * e[i__] + sinl * d__[i__ + 1];
				d__[i__ + 1] = cosl * d__[i__ + 1] - sinl * e[i__];
				if (i__ < m - 1) {
					g = sinl * e[i__ + 1];
					e[i__ + 1] = cosl * e[i__ + 1];
				}
				work[i__ - ll + 1] = cosr;
				work[i__ - ll + 1 + nm1] = sinr;
				work[i__ - ll + 1 + nm12] = cosl;
				work[i__ - ll + 1 + nm13] = sinl;
				/* L140: */
			}
			e[m - 1] = f;

			/* Update singular vectors */

			if (*ncvt > 0) {
				i__1 = m - ll + 1;
				NUMlapack_dlasr ("L", "V", "F", &i__1, ncvt, &work[1], &work[*n], &vt_ref (ll, 1), ldvt);
			}
			if (*nru > 0) {
				i__1 = m - ll + 1;
				NUMlapack_dlasr ("R", "V", "F", nru, &i__1, &work[nm12 + 1], &work[nm13 + 1], &u_ref (1, ll),
				                 ldu);
			}
			if (*ncc > 0) {
				i__1 = m - ll + 1;
				NUMlapack_dlasr ("L", "V", "F", &i__1, ncc, &work[nm12 + 1], &work[nm13 + 1], &c___ref (ll, 1),
				                 ldc);
			}

			/* Test convergence */

			if ( (d__1 = e[m - 1], fabs (d__1)) <= thresh) {
				e[m - 1] = 0.;
			}

		} else {

			/* Chase bulge from bottom to top Save cosines and sines for
			   later singular vector updates */

			f = ( (d__1 = d__[m], fabs (d__1)) - shift) * (d_sign (&c_b49, &d__[m]) + shift / d__[m]);
			g = e[m - 1];
			i__1 = ll + 1;
			for (i__ = m; i__ >= i__1; --i__) {
				NUMlapack_dlartg (&f, &g, &cosr, &sinr, &r__);
				if (i__ < m) {
					e[i__] = r__;
				}
				f = cosr * d__[i__] + sinr * e[i__ - 1];
				e[i__ - 1] = cosr * e[i__ - 1] - sinr * d__[i__];
				g = sinr * d__[i__ - 1];
				d__[i__ - 1] = cosr * d__[i__ - 1];
				NUMlapack_dlartg (&f, &g, &cosl, &sinl, &r__);
				d__[i__] = r__;
				f = cosl * e[i__ - 1] + sinl * d__[i__ - 1];
				d__[i__ - 1] = cosl * d__[i__ - 1] - sinl * e[i__ - 1];
				if (i__ > ll + 1) {
					g = sinl * e[i__ - 2];
					e[i__ - 2] = cosl * e[i__ - 2];
				}
				work[i__ - ll] = cosr;
				work[i__ - ll + nm1] = -sinr;
				work[i__ - ll + nm12] = cosl;
				work[i__ - ll + nm13] = -sinl;
				/* L150: */
			}
			e[ll] = f;

			/* Test convergence */

			if ( (d__1 = e[ll], fabs (d__1)) <= thresh) {
				e[ll] = 0.;
			}

			/* Update singular vectors if desired */

			if (*ncvt > 0) {
				i__1 = m - ll + 1;
				NUMlapack_dlasr ("L", "V", "B", &i__1, ncvt, &work[nm12 + 1], &work[nm13 + 1], &vt_ref (ll, 1),
				                 ldvt);
			}
			if (*nru > 0) {
				i__1 = m - ll + 1;
				NUMlapack_dlasr ("R", "V", "B", nru, &i__1, &work[1], &work[*n], &u_ref (1, ll), ldu);
			}
			if (*ncc > 0) {
				i__1 = m - ll + 1;
				NUMlapack_dlasr ("L", "V", "B", &i__1, ncc, &work[1], &work[*n], &c___ref (ll, 1), ldc);
			}
		}
	}

	/* QR iteration finished, go back and check convergence */

	goto L60;

	/* All singular values converged, so make them positive */

L160:
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		if (d__[i__] < 0.) {
			d__[i__] = -d__[i__];

			/* Change sign of singular vectors, if desired */

			if (*ncvt > 0) {
				NUMblas_dscal (ncvt, &c_b72, &vt_ref (i__, 1), ldvt);
			}
		}
		/* L170: */
	}

	/* Sort the singular values into decreasing order (insertion sort on
	   singular values, but only one transposition per singular vector) */

	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {

		/* Scan for smallest D(I) */

		isub = 1;
		smin = d__[1];
		i__2 = *n + 1 - i__;
		for (j = 2; j <= i__2; ++j) {
			if (d__[j] <= smin) {
				isub = j;
				smin = d__[j];
			}
			/* L180: */
		}
		if (isub != *n + 1 - i__) {

			/* Swap singular values and vectors */

			d__[isub] = d__[*n + 1 - i__];
			d__[*n + 1 - i__] = smin;
			if (*ncvt > 0) {
				NUMblas_dswap (ncvt, &vt_ref (isub, 1), ldvt, &vt_ref (*n + 1 - i__, 1), ldvt);
			}
			if (*nru > 0) {
				NUMblas_dswap (nru, &u_ref (1, isub), &c__1, &u_ref (1, *n + 1 - i__), &c__1);
			}
			if (*ncc > 0) {
				NUMblas_dswap (ncc, &c___ref (isub, 1), ldc, &c___ref (*n + 1 - i__, 1), ldc);
			}
		}
		/* L190: */
	}
	goto L220;

	/* Maximum number of iterations exceeded, failure to converge */

L200:
	*info = 0;
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
		if (e[i__] != 0.) {
			++ (*info);
		}
		/* L210: */
	}
L220:
	return 0;
}								/* NUMlapack_dbdsqr */

#undef vt_ref
#undef u_ref


int NUMlapack_dgebd2 (long *m, long *n, double *a, long *lda, double *d__, double *e, double *tauq,
                      double *taup, double *work, long *info) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3, i__4;

	/* Local variables */
	static long i__;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--d__;
	--e;
	--tauq;
	--taup;
	--work;

	/* Function Body */
	*info = 0;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *m)) {
		*info = -4;
	}
	if (*info < 0) {
		i__1 = - (*info);
		xerbla_ ("DGEBD2", &i__1);
		return 0;
	}

	if (*m >= *n) {

		/* Reduce to upper bidiagonal form */

		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {

			/* Generate elementary reflector H(i) to annihilate A(i+1:m,i)

			   Computing MIN */
			i__2 = i__ + 1;
			i__3 = *m - i__ + 1;
			NUMlapack_dlarfg (&i__3, &a_ref (i__, i__), &a_ref (MIN (i__2, *m), i__), &c__1, &tauq[i__]);
			d__[i__] = a_ref (i__, i__);
			a_ref (i__, i__) = 1.;

			/* Apply H(i) to A(i:m,i+1:n) from the left */

			i__2 = *m - i__ + 1;
			i__3 = *n - i__;
			NUMlapack_dlarf ("Left", &i__2, &i__3, &a_ref (i__, i__), &c__1, &tauq[i__], &a_ref (i__, i__ + 1),
			                 lda, &work[1]);
			a_ref (i__, i__) = d__[i__];

			if (i__ < *n) {

				/* Generate elementary reflector G(i) to annihilate
				   A(i,i+2:n)

				   Computing MIN */
				i__2 = i__ + 2;
				i__3 = *n - i__;
				NUMlapack_dlarfg (&i__3, &a_ref (i__, i__ + 1), &a_ref (i__, MIN (i__2, *n)), lda, &taup[i__]);
				e[i__] = a_ref (i__, i__ + 1);
				a_ref (i__, i__ + 1) = 1.;

				/* Apply G(i) to A(i+1:m,i+1:n) from the right */

				i__2 = *m - i__;
				i__3 = *n - i__;
				NUMlapack_dlarf ("Right", &i__2, &i__3, &a_ref (i__, i__ + 1), lda, &taup[i__], &a_ref (i__ + 1,
				                 i__ + 1), lda, &work[1]);
				a_ref (i__, i__ + 1) = e[i__];
			} else {
				taup[i__] = 0.;
			}
			/* L10: */
		}
	} else {

		/* Reduce to lower bidiagonal form */

		i__1 = *m;
		for (i__ = 1; i__ <= i__1; ++i__) {

			/* Generate elementary reflector G(i) to annihilate A(i,i+1:n)

			   Computing MIN */
			i__2 = i__ + 1;
			i__3 = *n - i__ + 1;
			NUMlapack_dlarfg (&i__3, &a_ref (i__, i__), &a_ref (i__, MIN (i__2, *n)), lda, &taup[i__]);
			d__[i__] = a_ref (i__, i__);
			a_ref (i__, i__) = 1.;

			/* Apply G(i) to A(i+1:m,i:n) from the right

			   Computing MIN */
			i__2 = i__ + 1;
			i__3 = *m - i__;
			i__4 = *n - i__ + 1;
			NUMlapack_dlarf ("Right", &i__3, &i__4, &a_ref (i__, i__), lda, &taup[i__], &a_ref (MIN (i__2, *m),
			                 i__), lda, &work[1]);
			a_ref (i__, i__) = d__[i__];

			if (i__ < *m) {

				/* Generate elementary reflector H(i) to annihilate
				   A(i+2:m,i)

				   Computing MIN */
				i__2 = i__ + 2;
				i__3 = *m - i__;
				NUMlapack_dlarfg (&i__3, &a_ref (i__ + 1, i__), &a_ref (MIN (i__2, *m), i__), &c__1,
				                  &tauq[i__]);
				e[i__] = a_ref (i__ + 1, i__);
				a_ref (i__ + 1, i__) = 1.;

				/* Apply H(i) to A(i+1:m,i+1:n) from the left */

				i__2 = *m - i__;
				i__3 = *n - i__;
				NUMlapack_dlarf ("Left", &i__2, &i__3, &a_ref (i__ + 1, i__), &c__1, &tauq[i__],
				                 &a_ref (i__ + 1, i__ + 1), lda, &work[1]);
				a_ref (i__ + 1, i__) = e[i__];
			} else {
				tauq[i__] = 0.;
			}
			/* L20: */
		}
	}
	return 0;
}								/* NUMlapack_dgebd2 */


int NUMlapack_dgebak (const char *job, const char *side, long *n, long *ilo, long *ihi, double *scale, long *m,
                      double *v, long *ldv, long *info) {
	/* System generated locals */
	long v_dim1, v_offset, i__1;

	/* Local variables */
	static long i__, k;
	static double s;
	static int leftv;
	static long ii;
	static int rightv;

#define v_ref(a_1,a_2) v[(a_2)*v_dim1 + a_1]

	--scale;
	v_dim1 = *ldv;
	v_offset = 1 + v_dim1 * 1;
	v -= v_offset;

	/* Function Body */
	rightv = lsame_ (side, "R");
	leftv = lsame_ (side, "L");

	*info = 0;
	if (!lsame_ (job, "N") && !lsame_ (job, "P") && !lsame_ (job, "S") && !lsame_ (job, "B")) {
		*info = -1;
	} else if (!rightv && !leftv) {
		*info = -2;
	} else if (*n < 0) {
		*info = -3;
	} else if (*ilo < 1 || *ilo > MAX (1, *n)) {
		*info = -4;
	} else if (*ihi < MIN (*ilo, *n) || *ihi > *n) {
		*info = -5;
	} else if (*m < 0) {
		*info = -7;
	} else if (*ldv < MAX (1, *n)) {
		*info = -9;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("NUMlapack_dgebak ", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*n == 0) {
		return 0;
	}
	if (*m == 0) {
		return 0;
	}
	if (lsame_ (job, "N")) {
		return 0;
	}

	if (*ilo == *ihi) {
		goto L30;
	}

	/* Backward balance */

	if (lsame_ (job, "S") || lsame_ (job, "B")) {

		if (rightv) {
			i__1 = *ihi;
			for (i__ = *ilo; i__ <= i__1; ++i__) {
				s = scale[i__];
				NUMblas_dscal (m, &s, &v_ref (i__, 1), ldv);
				/* L10: */
			}
		}

		if (leftv) {
			i__1 = *ihi;
			for (i__ = *ilo; i__ <= i__1; ++i__) {
				s = 1. / scale[i__];
				NUMblas_dscal (m, &s, &v_ref (i__, 1), ldv);
				/* L20: */
			}
		}

	}

	/* Backward permutation

	   For I = ILO-1 step -1 until 1, IHI+1 step 1 until N do -- */

L30:
	if (lsame_ (job, "P") || lsame_ (job, "B")) {
		if (rightv) {
			i__1 = *n;
			for (ii = 1; ii <= i__1; ++ii) {
				i__ = ii;
				if (i__ >= *ilo && i__ <= *ihi) {
					goto L40;
				}
				if (i__ < *ilo) {
					i__ = *ilo - ii;
				}
				k = (long) scale[i__];
				if (k == i__) {
					goto L40;
				}
				NUMblas_dswap (m, &v_ref (i__, 1), ldv, &v_ref (k, 1), ldv);
L40:
				;
			}
		}

		if (leftv) {
			i__1 = *n;
			for (ii = 1; ii <= i__1; ++ii) {
				i__ = ii;
				if (i__ >= *ilo && i__ <= *ihi) {
					goto L50;
				}
				if (i__ < *ilo) {
					i__ = *ilo - ii;
				}
				k = (long) scale[i__];
				if (k == i__) {
					goto L50;
				}
				NUMblas_dswap (m, &v_ref (i__, 1), ldv, &v_ref (k, 1), ldv);
L50:
				;
			}
		}
	}

	return 0;
}								/* NUMlapack_dgebak */

#undef v_ref

int NUMlapack_dgebal (const char *job, long *n, double *a, long *lda, long *ilo, long *ihi, double *scale,
                      long *info) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2;
	double d__1, d__2;

	/* Local variables */
	static long iexc;
	static double c__, f, g;
	static long i__, j, k, l, m;
	static double r__, s;
	static double sfmin1, sfmin2, sfmax1, sfmax2, ca, ra;
	static int noconv;
	static long ica, ira;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--scale;

	/* Function Body */
	*info = 0;
	if (!lsame_ (job, "N") && !lsame_ (job, "P") && !lsame_ (job, "S") && !lsame_ (job, "B")) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *n)) {
		*info = -4;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("NUMlapack_dgebal ", &i__1);
		return 0;
	}

	k = 1;
	l = *n;

	if (*n == 0) {
		goto L210;
	}

	if (lsame_ (job, "N")) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
			scale[i__] = 1.;
			/* L10: */
		}
		goto L210;
	}

	if (lsame_ (job, "S")) {
		goto L120;
	}

	/* Permutation to isolate eigenvalues if possible */

	goto L50;

	/* Row and column exchange. */

L20:
	scale[m] = (double) j;
	if (j == m) {
		goto L30;
	}

	NUMblas_dswap (&l, &a_ref (1, j), &c__1, &a_ref (1, m), &c__1);
	i__1 = *n - k + 1;
	NUMblas_dswap (&i__1, &a_ref (j, k), lda, &a_ref (m, k), lda);

L30:
	switch (iexc) {
		case 1:
			goto L40;
		case 2:
			goto L80;
	}

	/* Search for rows isolating an eigenvalue and push them down. */

L40:
	if (l == 1) {
		goto L210;
	}
	--l;

L50:
	for (j = l; j >= 1; --j) {

		i__1 = l;
		for (i__ = 1; i__ <= i__1; ++i__) {
			if (i__ == j) {
				goto L60;
			}
			if (a_ref (j, i__) != 0.) {
				goto L70;
			}
L60:
			;
		}

		m = l;
		iexc = 1;
		goto L20;
L70:
		;
	}

	goto L90;

	/* Search for columns isolating an eigenvalue and push them left. */

L80:
	++k;

L90:
	i__1 = l;
	for (j = k; j <= i__1; ++j) {

		i__2 = l;
		for (i__ = k; i__ <= i__2; ++i__) {
			if (i__ == j) {
				goto L100;
			}
			if (a_ref (i__, j) != 0.) {
				goto L110;
			}
L100:
			;
		}

		m = k;
		iexc = 2;
		goto L20;
L110:
		;
	}

L120:
	i__1 = l;
	for (i__ = k; i__ <= i__1; ++i__) {
		scale[i__] = 1.;
		/* L130: */
	}

	if (lsame_ (job, "P")) {
		goto L210;
	}

	/* Balance the submatrix in rows K to L.

	   Iterative loop for norm reduction */

	sfmin1 = NUMblas_dlamch ("S") / NUMblas_dlamch ("P");
	sfmax1 = 1. / sfmin1;
	sfmin2 = sfmin1 * 8.;
	sfmax2 = 1. / sfmin2;
L140:
	noconv = FALSE;

	i__1 = l;
	for (i__ = k; i__ <= i__1; ++i__) {
		c__ = 0.;
		r__ = 0.;

		i__2 = l;
		for (j = k; j <= i__2; ++j) {
			if (j == i__) {
				goto L150;
			}
			c__ += (d__1 = a_ref (j, i__), fabs (d__1));
			r__ += (d__1 = a_ref (i__, j), fabs (d__1));
L150:
			;
		}
		ica = NUMblas_idamax (&l, &a_ref (1, i__), &c__1);
		ca = (d__1 = a_ref (ica, i__), fabs (d__1));
		i__2 = *n - k + 1;
		ira = NUMblas_idamax (&i__2, &a_ref (i__, k), lda);
		ra = (d__1 = a_ref (i__, ira + k - 1), fabs (d__1));

		/* Guard against zero C or R due to underflow. */

		if (c__ == 0. || r__ == 0.) {
			goto L200;
		}
		g = r__ / 8.;
		f = 1.;
		s = c__ + r__;
L160:
		/* Computing MAX */
		d__1 = MAX (f, c__);
		/* Computing MIN */
		d__2 = MIN (r__, g);
		if (c__ >= g || MAX (d__1, ca) >= sfmax2 || MIN (d__2, ra) <= sfmin2) {
			goto L170;
		}
		f *= 8.;
		c__ *= 8.;
		ca *= 8.;
		r__ /= 8.;
		g /= 8.;
		ra /= 8.;
		goto L160;

L170:
		g = c__ / 8.;
L180:
		/* Computing MIN */
		d__1 = MIN (f, c__), d__1 = MIN (d__1, g);
		if (g < r__ || MAX (r__, ra) >= sfmax2 || MIN (d__1, ca) <= sfmin2) {
			goto L190;
		}
		f /= 8.;
		c__ /= 8.;
		g /= 8.;
		ca /= 8.;
		r__ *= 8.;
		ra *= 8.;
		goto L180;

		/* Now balance. */

L190:
		if (c__ + r__ >= s * .95) {
			goto L200;
		}
		if (f < 1. && scale[i__] < 1.) {
			if (f * scale[i__] <= sfmin1) {
				goto L200;
			}
		}
		if (f > 1. && scale[i__] > 1.) {
			if (scale[i__] >= sfmax1 / f) {
				goto L200;
			}
		}
		g = 1. / f;
		scale[i__] *= f;
		noconv = TRUE;

		i__2 = *n - k + 1;
		NUMblas_dscal (&i__2, &g, &a_ref (i__, k), lda);
		NUMblas_dscal (&l, &f, &a_ref (1, i__), &c__1);

L200:
		;
	}

	if (noconv) {
		goto L140;
	}

L210:
	*ilo = k;
	*ihi = l;

	return 0;
}								/* NUMlapack_dgebal */


int NUMlapack_dgebrd (long *m, long *n, double *a, long *lda, double *d__, double *e, double *tauq,
                      double *taup, double *work, long *lwork, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__3 = 3;
	static long c__2 = 2;
	static double c_b21 = -1.;
	static double c_b22 = 1.;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3, i__4;

	/* Local variables */
	static long i__, j;
	static long nbmin, iinfo, minmn;
	static long nb;
	static long nx;
	static double ws;
	static long ldwrkx, ldwrky, lwkopt;
	static long lquery;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--d__;
	--e;
	--tauq;
	--taup;
	--work;

	/* Function Body */
	*info = 0;
	/* Computing MAX */
	i__1 = 1, i__2 = NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, n, &c_n1, &c_n1, 6, 1);
	nb = MAX (i__1, i__2);
	lwkopt = (*m + *n) * nb;
	work[1] = (double) lwkopt;
	lquery = *lwork == -1;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *m)) {
		*info = -4;
	} else {					/* if(complicated condition) */
		/* Computing MAX */
		i__1 = MAX (1, *m);
		if (*lwork < MAX (i__1, *n) && !lquery) {
			*info = -10;
		}
	}
	if (*info < 0) {
		i__1 = - (*info);
		xerbla_ ("DGEBRD", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	minmn = MIN (*m, *n);
	if (minmn == 0) {
		work[1] = 1.;
		return 0;
	}

	ws = (double) MAX (*m, *n);
	ldwrkx = *m;
	ldwrky = *n;

	if (nb > 1 && nb < minmn) {

		/* Set the crossover point NX.

		   Computing MAX */
		i__1 = nb, i__2 = NUMlapack_ilaenv (&c__3, "DGEBRD", " ", m, n, &c_n1, &c_n1, 6, 1);
		nx = MAX (i__1, i__2);

		/* Determine when to switch from blocked to unblocked code. */

		if (nx < minmn) {
			ws = (double) ( (*m + *n) * nb);
			if ( (double) (*lwork) < ws) {

				/* Not enough work space for the optimal NB, consider using a
				   smaller block size. */

				nbmin = NUMlapack_ilaenv (&c__2, "DGEBRD", " ", m, n, &c_n1, &c_n1, 6, 1);
				if (*lwork >= (*m + *n) * nbmin) {
					nb = *lwork / (*m + *n);
				} else {
					nb = 1;
					nx = minmn;
				}
			}
		}
	} else {
		nx = minmn;
	}

	i__1 = minmn - nx;
	i__2 = nb;
	for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {

		/* Reduce rows and columns i:i+nb-1 to bidiagonal form and return the
		   matrices X and Y which are needed to update the unreduced part of
		   the matrix */

		i__3 = *m - i__ + 1;
		i__4 = *n - i__ + 1;
		NUMlapack_dlabrd (&i__3, &i__4, &nb, &a_ref (i__, i__), lda, &d__[i__], &e[i__], &tauq[i__], &taup[i__],
		                  &work[1], &ldwrkx, &work[ldwrkx * nb + 1], &ldwrky);

		/* Update the trailing submatrix A(i+nb:m,i+nb:n), using an update of
		   the form A := A - V*Y' - X*U' */

		i__3 = *m - i__ - nb + 1;
		i__4 = *n - i__ - nb + 1;
		NUMblas_dgemm ("No transpose", "Transpose", &i__3, &i__4, &nb, &c_b21, &a_ref (i__ + nb, i__), lda,
		               &work[ldwrkx * nb + nb + 1], &ldwrky, &c_b22, &a_ref (i__ + nb, i__ + nb), lda);
		i__3 = *m - i__ - nb + 1;
		i__4 = *n - i__ - nb + 1;
		NUMblas_dgemm ("No transpose", "No transpose", &i__3, &i__4, &nb, &c_b21, &work[nb + 1], &ldwrkx,
		               &a_ref (i__, i__ + nb), lda, &c_b22, &a_ref (i__ + nb, i__ + nb), lda);

		/* Copy diagonal and off-diagonal elements of B back into A */

		if (*m >= *n) {
			i__3 = i__ + nb - 1;
			for (j = i__; j <= i__3; ++j) {
				a_ref (j, j) = d__[j];
				a_ref (j, j + 1) = e[j];
				/* L10: */
			}
		} else {
			i__3 = i__ + nb - 1;
			for (j = i__; j <= i__3; ++j) {
				a_ref (j, j) = d__[j];
				a_ref (j + 1, j) = e[j];
				/* L20: */
			}
		}
		/* L30: */
	}

	/* Use unblocked code to reduce the remainder of the matrix */

	i__2 = *m - i__ + 1;
	i__1 = *n - i__ + 1;
	NUMlapack_dgebd2 (&i__2, &i__1, &a_ref (i__, i__), lda, &d__[i__], &e[i__], &tauq[i__], &taup[i__],
	                  &work[1], &iinfo);
	work[1] = ws;
	return 0;
}								/* NUMlapack_dgebrd */


int NUMlapack_dgeev (const char *jobvl, const char *jobvr, long *n, double *a, long *lda, double *wr, double *wi,
                     double *vl, long *ldvl, double *vr, long *ldvr, double *work, long *lwork, long *info) {
	/* Table of constant values */
	static long c__8 = 8;
	static long c_n1 = -1;

	/* System generated locals */
	long a_dim1, a_offset, vl_dim1, vl_offset, vr_dim1, vr_offset, i__1, i__2, i__3, i__4;
	double d__1, d__2;

	/* Local variables */
	static long ibal;
	static char side[1];
	static long maxb;
	static double anrm;
	static long ierr, itau;
	static long iwrk, nout;
	static long i__, k;
	static double r__;
	static double cs;
	static int scalea;
	static double cscale;
	static double sn;
	static int select[1];
	static double bignum;
	static long minwrk, maxwrk;
	static int wantvl;
	static double smlnum;
	static long hswork;
	static int lquery, wantvr;
	static long ihi;
	static double scl;
	static long ilo;
	static double dum[1], eps;

#define vl_ref(a_1,a_2) vl[(a_2)*vl_dim1 + a_1]
#define vr_ref(a_1,a_2) vr[(a_2)*vr_dim1 + a_1]

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--wr;
	--wi;
	vl_dim1 = *ldvl;
	vl_offset = 1 + vl_dim1 * 1;
	vl -= vl_offset;
	vr_dim1 = *ldvr;
	vr_offset = 1 + vr_dim1 * 1;
	vr -= vr_offset;
	--work;

	/* Function Body */
	*info = 0;
	lquery = *lwork == -1;
	wantvl = lsame_ (jobvl, "V");
	wantvr = lsame_ (jobvr, "V");
	if (!wantvl && !lsame_ (jobvl, "N")) {
		*info = -1;
	} else if (!wantvr && !lsame_ (jobvr, "N")) {
		*info = -2;
	} else if (*n < 0) {
		*info = -3;
	} else if (*lda < MAX (1, *n)) {
		*info = -5;
	} else if (*ldvl < 1 || wantvl && *ldvl < *n) {
		*info = -9;
	} else if (*ldvr < 1 || wantvr && *ldvr < *n) {
		*info = -11;
	}

	/* Compute workspace (Note: Comments in the code beginning "Workspace:"
	   describe the minimal amount of workspace needed at that point in the
	   code, as well as the preferred amount for good performance. NB refers
	   to the optimal block size for the immediately following subroutine, as
	   returned by ILAENV. HSWORK refers to the workspace preferred by
	   NUMlapack_dhseqr , as calculated below. HSWORK is computed assuming
	   ILO=1 and IHI=N, the worst case.) */

	minwrk = 1;
	if (*info == 0 && (*lwork >= 1 || lquery)) {
		maxwrk =
		    (*n << 1) + *n * NUMlapack_ilaenv (&c__1, "NUMlapack_dgehrd ", " ", n, &c__1, n, &c__0, 6, 1);
		if (!wantvl && !wantvr) {
			/* Computing MAX */
			i__1 = 1, i__2 = *n * 3;
			minwrk = MAX (i__1, i__2);
			/* Computing MAX */
			i__1 = NUMlapack_ilaenv (&c__8, "NUMlapack_dhseqr ", "EN", n, &c__1, n, &c_n1, 6, 2);
			maxb = MAX (i__1, 2);
			/* Computing MIN Computing MAX */
			i__3 = 2, i__4 = NUMlapack_ilaenv (&c__4, "NUMlapack_dhseqr ", "EN", n, &c__1, n, &c_n1, 6, 2);
			i__1 = MIN (maxb, *n), i__2 = MAX (i__3, i__4);
			k = MIN (i__1, i__2);
			/* Computing MAX */
			i__1 = k * (k + 2), i__2 = *n << 1;
			hswork = MAX (i__1, i__2);
			/* Computing MAX */
			i__1 = maxwrk, i__2 = *n + 1, i__1 = MAX (i__1, i__2), i__2 = *n + hswork;
			maxwrk = MAX (i__1, i__2);
		} else {
			/* Computing MAX */
			i__1 = 1, i__2 = *n << 2;
			minwrk = MAX (i__1, i__2);
			/* Computing MAX */
			i__1 = maxwrk, i__2 =
			           (*n << 1) + (*n - 1) * NUMlapack_ilaenv (&c__1, "DOR" "GHR", " ", n, &c__1, n, &c_n1, 6, 1);
			maxwrk = MAX (i__1, i__2);
			/* Computing MAX */
			i__1 = NUMlapack_ilaenv (&c__8, "NUMlapack_dhseqr ", "SV", n, &c__1, n, &c_n1, 6, 2);
			maxb = MAX (i__1, 2);
			/* Computing MIN Computing MAX */
			i__3 = 2, i__4 = NUMlapack_ilaenv (&c__4, "NUMlapack_dhseqr ", "SV", n, &c__1, n, &c_n1, 6, 2);
			i__1 = MIN (maxb, *n), i__2 = MAX (i__3, i__4);
			k = MIN (i__1, i__2);
			/* Computing MAX */
			i__1 = k * (k + 2), i__2 = *n << 1;
			hswork = MAX (i__1, i__2);
			/* Computing MAX */
			i__1 = maxwrk, i__2 = *n + 1, i__1 = MAX (i__1, i__2), i__2 = *n + hswork;
			maxwrk = MAX (i__1, i__2);
			/* Computing MAX */
			i__1 = maxwrk, i__2 = *n << 2;
			maxwrk = MAX (i__1, i__2);
		}
		work[1] = (double) maxwrk;
	}
	if (*lwork < minwrk && !lquery) {
		*info = -13;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("NUMlapack_dgeev ", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*n == 0) {
		return 0;
	}

	/* Get machine constants */

	eps = NUMblas_dlamch ("P");
	smlnum = NUMblas_dlamch ("S");
	bignum = 1. / smlnum;
	NUMlapack_dlabad (&smlnum, &bignum);
	smlnum = sqrt (smlnum) / eps;
	bignum = 1. / smlnum;

	/* Scale A if max element outside range [SMLNUM,BIGNUM] */

	anrm = NUMlapack_dlange ("M", n, n, &a[a_offset], lda, dum);
	scalea = FALSE;
	if (anrm > 0. && anrm < smlnum) {
		scalea = TRUE;
		cscale = smlnum;
	} else if (anrm > bignum) {
		scalea = TRUE;
		cscale = bignum;
	}
	if (scalea) {
		NUMlapack_dlascl ("G", &c__0, &c__0, &anrm, &cscale, n, n, &a[a_offset], lda, &ierr);
	}

	/* Balance the matrix (Workspace: need N) */

	ibal = 1;
	NUMlapack_dgebal ("B", n, &a[a_offset], lda, &ilo, &ihi, &work[ibal], &ierr);

	/* Reduce to upper Hessenberg form (Workspace: need 3*N, prefer 2*N+N*NB)
	 */

	itau = ibal + *n;
	iwrk = itau + *n;
	i__1 = *lwork - iwrk + 1;
	NUMlapack_dgehrd (n, &ilo, &ihi, &a[a_offset], lda, &work[itau], &work[iwrk], &i__1, &ierr);

	if (wantvl) {

		/* Want left eigenvectors Copy Householder vectors to VL */

		* (unsigned char *) side = 'L';
		NUMlapack_dlacpy ("L", n, n, &a[a_offset], lda, &vl[vl_offset], ldvl);

		/* Generate orthogonal matrix in VL (Workspace: need 3*N-1, prefer
		   2*N+(N-1)*NB) */

		i__1 = *lwork - iwrk + 1;
		NUMlapack_dorghr (n, &ilo, &ihi, &vl[vl_offset], ldvl, &work[itau], &work[iwrk], &i__1, &ierr);

		/* Perform QR iteration, accumulating Schur vectors in VL (Workspace:
		   need N+1, prefer N+HSWORK (see comments) ) */

		iwrk = itau;
		i__1 = *lwork - iwrk + 1;
		NUMlapack_dhseqr ("S", "V", n, &ilo, &ihi, &a[a_offset], lda, &wr[1], &wi[1], &vl[vl_offset], ldvl,
		                  &work[iwrk], &i__1, info);

		if (wantvr) {

			/* Want left and right eigenvectors Copy Schur vectors to VR */

			* (unsigned char *) side = 'B';
			NUMlapack_dlacpy ("F", n, n, &vl[vl_offset], ldvl, &vr[vr_offset], ldvr);
		}

	} else if (wantvr) {

		/* Want right eigenvectors Copy Householder vectors to VR */

		* (unsigned char *) side = 'R';
		NUMlapack_dlacpy ("L", n, n, &a[a_offset], lda, &vr[vr_offset], ldvr);

		/* Generate orthogonal matrix in VR (Workspace: need 3*N-1, prefer
		   2*N+(N-1)*NB) */

		i__1 = *lwork - iwrk + 1;
		NUMlapack_dorghr (n, &ilo, &ihi, &vr[vr_offset], ldvr, &work[itau], &work[iwrk], &i__1, &ierr);

		/* Perform QR iteration, accumulating Schur vectors in VR (Workspace:
		   need N+1, prefer N+HSWORK (see comments) ) */

		iwrk = itau;
		i__1 = *lwork - iwrk + 1;
		NUMlapack_dhseqr ("S", "V", n, &ilo, &ihi, &a[a_offset], lda, &wr[1], &wi[1], &vr[vr_offset], ldvr,
		                  &work[iwrk], &i__1, info);

	} else {

		/* Compute eigenvalues only (Workspace: need N+1, prefer N+HSWORK
		   (see comments) ) */

		iwrk = itau;
		i__1 = *lwork - iwrk + 1;
		NUMlapack_dhseqr ("E", "N", n, &ilo, &ihi, &a[a_offset], lda, &wr[1], &wi[1], &vr[vr_offset], ldvr,
		                  &work[iwrk], &i__1, info);
	}

	/* If INFO > 0 from NUMlapack_dhseqr , then quit */

	if (*info > 0) {
		goto L50;
	}

	if (wantvl || wantvr) {

		/* Compute left and/or right eigenvectors (Workspace: need 4*N) */

		NUMlapack_dtrevc (side, "B", select, n, &a[a_offset], lda, &vl[vl_offset], ldvl, &vr[vr_offset],
		                  ldvr, n, &nout, &work[iwrk], &ierr);
	}

	if (wantvl) {

		/* Undo balancing of left eigenvectors (Workspace: need N) */

		NUMlapack_dgebak ("B", "L", n, &ilo, &ihi, &work[ibal], n, &vl[vl_offset], ldvl, &ierr);

		/* Normalize left eigenvectors and make largest component real */

		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
			if (wi[i__] == 0.) {
				scl = 1. / NUMblas_dnrm2 (n, &vl_ref (1, i__), &c__1);
				NUMblas_dscal (n, &scl, &vl_ref (1, i__), &c__1);
			} else if (wi[i__] > 0.) {
				d__1 = NUMblas_dnrm2 (n, &vl_ref (1, i__), &c__1);
				d__2 = NUMblas_dnrm2 (n, &vl_ref (1, i__ + 1), &c__1);
				scl = 1. / NUMlapack_dlapy2 (&d__1, &d__2);
				NUMblas_dscal (n, &scl, &vl_ref (1, i__), &c__1);
				NUMblas_dscal (n, &scl, &vl_ref (1, i__ + 1), &c__1);
				i__2 = *n;
				for (k = 1; k <= i__2; ++k) {
					/* Computing 2nd power */
					d__1 = vl_ref (k, i__);
					/* Computing 2nd power */
					d__2 = vl_ref (k, i__ + 1);
					work[iwrk + k - 1] = d__1 * d__1 + d__2 * d__2;
					/* L10: */
				}
				k = NUMblas_idamax (n, &work[iwrk], &c__1);
				NUMlapack_dlartg (&vl_ref (k, i__), &vl_ref (k, i__ + 1), &cs, &sn, &r__);
				NUMblas_drot (n, &vl_ref (1, i__), &c__1, &vl_ref (1, i__ + 1), &c__1, &cs, &sn);
				vl_ref (k, i__ + 1) = 0.;
			}
			/* L20: */
		}
	}

	if (wantvr) {

		/* Undo balancing of right eigenvectors (Workspace: need N) */

		NUMlapack_dgebak ("B", "R", n, &ilo, &ihi, &work[ibal], n, &vr[vr_offset], ldvr, &ierr);

		/* Normalize right eigenvectors and make largest component real */

		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
			if (wi[i__] == 0.) {
				scl = 1. / NUMblas_dnrm2 (n, &vr_ref (1, i__), &c__1);
				NUMblas_dscal (n, &scl, &vr_ref (1, i__), &c__1);
			} else if (wi[i__] > 0.) {
				d__1 = NUMblas_dnrm2 (n, &vr_ref (1, i__), &c__1);
				d__2 = NUMblas_dnrm2 (n, &vr_ref (1, i__ + 1), &c__1);
				scl = 1. / NUMlapack_dlapy2 (&d__1, &d__2);
				NUMblas_dscal (n, &scl, &vr_ref (1, i__), &c__1);
				NUMblas_dscal (n, &scl, &vr_ref (1, i__ + 1), &c__1);
				i__2 = *n;
				for (k = 1; k <= i__2; ++k) {
					/* Computing 2nd power */
					d__1 = vr_ref (k, i__);
					/* Computing 2nd power */
					d__2 = vr_ref (k, i__ + 1);
					work[iwrk + k - 1] = d__1 * d__1 + d__2 * d__2;
					/* L30: */
				}
				k = NUMblas_idamax (n, &work[iwrk], &c__1);
				NUMlapack_dlartg (&vr_ref (k, i__), &vr_ref (k, i__ + 1), &cs, &sn, &r__);
				NUMblas_drot (n, &vr_ref (1, i__), &c__1, &vr_ref (1, i__ + 1), &c__1, &cs, &sn);
				vr_ref (k, i__ + 1) = 0.;
			}
			/* L40: */
		}
	}

	/* Undo scaling if necessary */

L50:
	if (scalea) {
		i__1 = *n - *info;
		/* Computing MAX */
		i__3 = *n - *info;
		i__2 = MAX (i__3, 1);
		NUMlapack_dlascl ("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wr[*info + 1], &i__2, &ierr);
		i__1 = *n - *info;
		/* Computing MAX */
		i__3 = *n - *info;
		i__2 = MAX (i__3, 1);
		NUMlapack_dlascl ("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wi[*info + 1], &i__2, &ierr);
		if (*info > 0) {
			i__1 = ilo - 1;
			NUMlapack_dlascl ("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wr[1], n, &ierr);
			i__1 = ilo - 1;
			NUMlapack_dlascl ("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wi[1], n, &ierr);
		}
	}

	work[1] = (double) maxwrk;
	return 0;
}								/* NUMlapack_dgeev */

#undef vr_ref
#undef vl_ref


int NUMlapack_dgehd2 (long *n, long *ilo, long *ihi, double *a, long *lda, double *tau, double *work,
                      long *info) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;

	/* Local variables */
	static long i__;
	static double aii;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	if (*n < 0) {
		*info = -1;
	} else if (*ilo < 1 || *ilo > MAX (1, *n)) {
		*info = -2;
	} else if (*ihi < MIN (*ilo, *n) || *ihi > *n) {
		*info = -3;
	} else if (*lda < MAX (1, *n)) {
		*info = -5;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("NUMlapack_dgehd2 ", &i__1);
		return 0;
	}

	i__1 = *ihi - 1;
	for (i__ = *ilo; i__ <= i__1; ++i__) {

		/* Compute elementary reflector H(i) to annihilate A(i+2:ihi,i)

		   Computing MIN */
		i__2 = i__ + 2;
		i__3 = *ihi - i__;
		NUMlapack_dlarfg (&i__3, &a_ref (i__ + 1, i__), &a_ref (MIN (i__2, *n), i__), &c__1, &tau[i__]);
		aii = a_ref (i__ + 1, i__);
		a_ref (i__ + 1, i__) = 1.;

		/* Apply H(i) to A(1:ihi,i+1:ihi) from the right */

		i__2 = *ihi - i__;
		NUMlapack_dlarf ("Right", ihi, &i__2, &a_ref (i__ + 1, i__), &c__1, &tau[i__], &a_ref (1, i__ + 1),
		                 lda, &work[1]);

		/* Apply H(i) to A(i+1:ihi,i+1:n) from the left */

		i__2 = *ihi - i__;
		i__3 = *n - i__;
		NUMlapack_dlarf ("Left", &i__2, &i__3, &a_ref (i__ + 1, i__), &c__1, &tau[i__], &a_ref (i__ + 1,
		                 i__ + 1), lda, &work[1]);

		a_ref (i__ + 1, i__) = aii;
		/* L10: */
	}

	return 0;
}								/* NUMlapack_dgehd2 */


int NUMlapack_dgehrd (long *n, long *ilo, long *ihi, double *a, long *lda, double *tau, double *work,
                      long *lwork, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__3 = 3;
	static long c__2 = 2;
	static long c__65 = 65;
	static double c_b25 = -1.;
	static double c_b26 = 1.;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3, i__4;

	/* Local variables */
	static long i__;
	static double t[4160] /* was [65][64] */ ;
	static long nbmin, iinfo;
	static long ib;
	static double ei;
	static long nb, nh;
	static long nx;
	static long ldwork, lwkopt;
	static int lquery;
	static long iws;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	/* Computing MIN */
	i__1 = 64, i__2 = NUMlapack_ilaenv (&c__1, "NUMlapack_dgehrd ", " ", n, ilo, ihi, &c_n1, 6, 1);
	nb = MIN (i__1, i__2);
	lwkopt = *n * nb;
	work[1] = (double) lwkopt;
	lquery = *lwork == -1;
	if (*n < 0) {
		*info = -1;
	} else if (*ilo < 1 || *ilo > MAX (1, *n)) {
		*info = -2;
	} else if (*ihi < MIN (*ilo, *n) || *ihi > *n) {
		*info = -3;
	} else if (*lda < MAX (1, *n)) {
		*info = -5;
	} else if (*lwork < MAX (1, *n) && !lquery) {
		*info = -8;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("NUMlapack_dgehrd ", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Set elements 1:ILO-1 and IHI:N-1 of TAU to zero */

	i__1 = *ilo - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
		tau[i__] = 0.;
		/* L10: */
	}
	i__1 = *n - 1;
	for (i__ = MAX (1, *ihi); i__ <= i__1; ++i__) {
		tau[i__] = 0.;
		/* L20: */
	}

	/* Quick return if possible */

	nh = *ihi - *ilo + 1;
	if (nh <= 1) {
		work[1] = 1.;
		return 0;
	}

	/* Determine the block size.

	   Computing MIN */
	i__1 = 64, i__2 = NUMlapack_ilaenv (&c__1, "NUMlapack_dgehrd ", " ", n, ilo, ihi, &c_n1, 6, 1);
	nb = MIN (i__1, i__2);
	nbmin = 2;
	iws = 1;
	if (nb > 1 && nb < nh) {

		/* Determine when to cross over from blocked to unblocked code (last
		   block is always handled by unblocked code).

		   Computing MAX */
		i__1 = nb, i__2 = NUMlapack_ilaenv (&c__3, "NUMlapack_dgehrd ", " ", n, ilo, ihi, &c_n1, 6, 1);
		nx = MAX (i__1, i__2);
		if (nx < nh) {

			/* Determine if workspace is large enough for blocked code. */

			iws = *n * nb;
			if (*lwork < iws) {

				/* Not enough workspace to use optimal NB: determine the
				   minimum value of NB, and reduce NB or force use of
				   unblocked code.

				   Computing MAX */
				i__1 = 2, i__2 =
				           NUMlapack_ilaenv (&c__2, "NUMlapack_dgehrd ", " ", n, ilo, ihi, &c_n1, 6, 1);
				nbmin = MAX (i__1, i__2);
				if (*lwork >= *n * nbmin) {
					nb = *lwork / *n;
				} else {
					nb = 1;
				}
			}
		}
	}
	ldwork = *n;

	if (nb < nbmin || nb >= nh) {

		/* Use unblocked code below */

		i__ = *ilo;

	} else {

		/* Use blocked code */

		i__1 = *ihi - 1 - nx;
		i__2 = nb;
		for (i__ = *ilo; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
			/* Computing MIN */
			i__3 = nb, i__4 = *ihi - i__;
			ib = MIN (i__3, i__4);

			/* Reduce columns i:i+ib-1 to Hessenberg form, returning the
			   matrices V and T of the block reflector H = I - V*T*V' which
			   performs the reduction, and also the matrix Y = A*V*T */

			NUMlapack_dlahrd (ihi, &i__, &ib, &a_ref (1, i__), lda, &tau[i__], t, &c__65, &work[1], &ldwork);

			/* Apply the block reflector H to A(1:ihi,i+ib:ihi) from the
			   right, computing A := A - Y * V'. V(i+ib,ib-1) must be set to
			   1. */

			ei = a_ref (i__ + ib, i__ + ib - 1);
			a_ref (i__ + ib, i__ + ib - 1) = 1.;
			i__3 = *ihi - i__ - ib + 1;
			NUMblas_dgemm ("No transpose", "Transpose", ihi, &i__3, &ib, &c_b25, &work[1], &ldwork,
			               &a_ref (i__ + ib, i__), lda, &c_b26, &a_ref (1, i__ + ib), lda);
			a_ref (i__ + ib, i__ + ib - 1) = ei;

			/* Apply the block reflector H to A(i+1:ihi,i+ib:n) from the left
			 */

			i__3 = *ihi - i__;
			i__4 = *n - i__ - ib + 1;
			NUMlapack_dlarfb ("Left", "Transpose", "Forward", "Columnwise", &i__3, &i__4, &ib,
			                  &a_ref (i__ + 1, i__), lda, t, &c__65, &a_ref (i__ + 1, i__ + ib), lda, &work[1], &ldwork);
			/* L30: */
		}
	}

	/* Use unblocked code to reduce the rest of the matrix */

	NUMlapack_dgehd2 (n, &i__, ihi, &a[a_offset], lda, &tau[1], &work[1], &iinfo);
	work[1] = (double) iws;

	return 0;
}								/* NUMlapack_dgehrd */

int NUMlapack_dgelq2 (long *m, long *n, double *a, long *lda, double *tau, double *work, long *info) {
	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;

	/* Local variables */
	static long i__, k;
	static double aii;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *m)) {
		*info = -4;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGELQ2", &i__1);
		return 0;
	}

	k = MIN (*m, *n);

	i__1 = k;
	for (i__ = 1; i__ <= i__1; ++i__) {

		/* Generate elementary reflector H(i) to annihilate A(i,i+1:n)

		   Computing MIN */
		i__2 = i__ + 1;
		i__3 = *n - i__ + 1;
		NUMlapack_dlarfg (&i__3, &a_ref (i__, i__), &a_ref (i__, MIN (i__2, *n)), lda, &tau[i__]);
		if (i__ < *m) {

			/* Apply H(i) to A(i+1:m,i:n) from the right */

			aii = a_ref (i__, i__);
			a_ref (i__, i__) = 1.;
			i__2 = *m - i__;
			i__3 = *n - i__ + 1;
			NUMlapack_dlarf ("Right", &i__2, &i__3, &a_ref (i__, i__), lda, &tau[i__], &a_ref (i__ + 1, i__),
			                 lda, &work[1]);
			a_ref (i__, i__) = aii;
		}
		/* L10: */
	}
	return 0;
}								/* NUMlapack_dgelq2 */

int NUMlapack_dgelqf (long *m, long *n, double *a, long *lda, double *tau, double *work, long *lwork,
                      long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__3 = 3;
	static long c__2 = 2;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3, i__4;

	/* Local variables */
	static long i__, k, nbmin, iinfo;
	static long ib, nb;
	static long nx;
	static long ldwork, lwkopt;
	static long lquery;
	static long iws;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	nb = NUMlapack_ilaenv (&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
	lwkopt = *m * nb;
	work[1] = (double) lwkopt;
	lquery = *lwork == -1;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *m)) {
		*info = -4;
	} else if (*lwork < MAX (1, *m) && !lquery) {
		*info = -7;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGELQF", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	k = MIN (*m, *n);
	if (k == 0) {
		work[1] = 1.;
		return 0;
	}

	nbmin = 2;
	nx = 0;
	iws = *m;
	if (nb > 1 && nb < k) {

		/* Determine when to cross over from blocked to unblocked code.

		   Computing MAX */
		i__1 = 0, i__2 = NUMlapack_ilaenv (&c__3, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
		nx = MAX (i__1, i__2);
		if (nx < k) {

			/* Determine if workspace is large enough for blocked code. */

			ldwork = *m;
			iws = ldwork * nb;
			if (*lwork < iws) {

				/* Not enough workspace to use optimal NB: reduce NB and
				   determine the minimum value of NB. */

				nb = *lwork / ldwork;
				/* Computing MAX */
				i__1 = 2, i__2 = NUMlapack_ilaenv (&c__2, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
				nbmin = MAX (i__1, i__2);
			}
		}
	}

	if (nb >= nbmin && nb < k && nx < k) {

		/* Use blocked code initially */

		i__1 = k - nx;
		i__2 = nb;
		for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
			/* Computing MIN */
			i__3 = k - i__ + 1;
			ib = MIN (i__3, nb);

			/* Compute the LQ factorization of the current block
			   A(i:i+ib-1,i:n) */

			i__3 = *n - i__ + 1;
			NUMlapack_dgelq2 (&ib, &i__3, &a_ref (i__, i__), lda, &tau[i__], &work[1], &iinfo);
			if (i__ + ib <= *m) {

				/* Form the triangular factor of the block reflector H = H(i)
				   H(i+1) . . . H(i+ib-1) */

				i__3 = *n - i__ + 1;
				NUMlapack_dlarft ("Forward", "Rowwise", &i__3, &ib, &a_ref (i__, i__), lda, &tau[i__], &work[1],
				                  &ldwork);

				/* Apply H to A(i+ib:m,i:n) from the right */

				i__3 = *m - i__ - ib + 1;
				i__4 = *n - i__ + 1;
				NUMlapack_dlarfb ("Right", "No transpose", "Forward", "Rowwise", &i__3, &i__4, &ib, &a_ref (i__,
				                  i__), lda, &work[1], &ldwork, &a_ref (i__ + ib, i__), lda, &work[ib + 1], &ldwork);
			}
			/* L10: */
		}
	} else {
		i__ = 1;
	}

	/* Use unblocked code to factor the last or only block. */

	if (i__ <= k) {
		i__2 = *m - i__ + 1;
		i__1 = *n - i__ + 1;
		NUMlapack_dgelq2 (&i__2, &i__1, &a_ref (i__, i__), lda, &tau[i__], &work[1], &iinfo);
	}

	work[1] = (double) iws;
	return 0;
}								/* NUMlapack_dgelqf */

#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]

int NUMlapack_dgelss (long *m, long *n, long *nrhs, double *a, long *lda, double *b, long *ldb, double *s,
                      double *rcond, long *rank, double *work, long *lwork, long *info) {
	/* System generated locals */
	long a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3, i__4;
	double d__1;

	/* Local variables */
	static double anrm, bnrm;
	static long itau;
	static double vdum[1];
	static long i__;
	static long iascl, ibscl;
	static long chunk;
	static double sfmin;
	static long minmn;
	static long maxmn, itaup, itauq, mnthr, iwork;
	static long bl, ie, il;
	static long mm;
	static long bdspac;
	static double bignum;
	static long ldwork;
	static long minwrk, maxwrk;
	static double smlnum;
	static long lquery;
	static double eps, thr;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1 * 1;
	b -= b_offset;
	--s;
	--work;

	/* Function Body */
	*info = 0;
	minmn = MIN (*m, *n);
	maxmn = MAX (*m, *n);
	mnthr = NUMlapack_ilaenv (&c__6, "DGELSS", " ", m, n, nrhs, &c_n1, 6, 1);
	lquery = *lwork == -1;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*nrhs < 0) {
		*info = -3;
	} else if (*lda < MAX (1, *m)) {
		*info = -5;
	} else if (*ldb < MAX (1, maxmn)) {
		*info = -7;
	}

	/* Compute workspace (Note: Comments in the code beginning "Workspace:"
	   describe the minimal amount of workspace needed at that point in the
	   code, as well as the preferred amount for good performance. NB refers
	   to the optimal block size for the immediately following subroutine, as
	   returned by ILAENV.) */

	minwrk = 1;
	if (*info == 0 && (*lwork >= 1 || lquery)) {
		maxwrk = 0;
		mm = *m;
		if (*m >= *n && *m >= mnthr) {

			/* Path 1a - overdetermined, with many more rows than columns */

			mm = *n;
			/* Computing MAX */
			i__1 = maxwrk, i__2 = *n + *n * NUMlapack_ilaenv (&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
			maxwrk = MAX (i__1, i__2);
			/* Computing MAX */
			i__1 = maxwrk, i__2 =
			           *n + *nrhs * NUMlapack_ilaenv (&c__1, "DORMQR", "LT", m, nrhs, n, &c_n1, 6, 2);
			maxwrk = MAX (i__1, i__2);
		}
		if (*m >= *n) {

			/* Path 1 - overdetermined or exactly determined

			   Compute workspace needed for DBDSQR

			   Computing MAX */
			i__1 = 1, i__2 = *n * 5;
			bdspac = MAX (i__1, i__2);
			/* Computing MAX */
			i__1 = maxwrk, i__2 =
			           *n * 3 + (mm + *n) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", &mm, n, &c_n1, &c_n1, 6, 1);
			maxwrk = MAX (i__1, i__2);
			/* Computing MAX */
			i__1 = maxwrk, i__2 =
			           *n * 3 + *nrhs * NUMlapack_ilaenv (&c__1, "DORMBR", "QLT", &mm, nrhs, n, &c_n1, 6, 3);
			maxwrk = MAX (i__1, i__2);
			/* Computing MAX */
			i__1 = maxwrk, i__2 =
			           *n * 3 + (*n - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", n, n, n, &c_n1, 6, 1);
			maxwrk = MAX (i__1, i__2);
			maxwrk = MAX (maxwrk, bdspac);
			/* Computing MAX */
			i__1 = maxwrk, i__2 = *n * *nrhs;
			maxwrk = MAX (i__1, i__2);
			/* Computing MAX */
			i__1 = *n * 3 + mm, i__2 = *n * 3 + *nrhs, i__1 = MAX (i__1, i__2);
			minwrk = MAX (i__1, bdspac);
			maxwrk = MAX (minwrk, maxwrk);
		}
		if (*n > *m) {

			/* Compute workspace needed for DBDSQR

			   Computing MAX */
			i__1 = 1, i__2 = *m * 5;
			bdspac = MAX (i__1, i__2);
			/* Computing MAX */
			i__1 = *m * 3 + *nrhs, i__2 = *m * 3 + *n, i__1 = MAX (i__1, i__2);
			minwrk = MAX (i__1, bdspac);
			if (*n >= mnthr) {

				/* Path 2a - underdetermined, with many more columns than
				   rows */

				maxwrk = *m + *m * NUMlapack_ilaenv (&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
				/* Computing MAX */
				i__1 = maxwrk, i__2 =
				           *m * *m + (*m << 2) + (*m << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, m, &c_n1,
				                   &c_n1, 6, 1);
				maxwrk = MAX (i__1, i__2);
				/* Computing MAX */
				i__1 = maxwrk, i__2 =
				           *m * *m + (*m << 2) + *nrhs * NUMlapack_ilaenv (&c__1, "DORMBR", "QLT", m, nrhs, m, &c_n1,
				                   6, 3);
				maxwrk = MAX (i__1, i__2);
				/* Computing MAX */
				i__1 = maxwrk, i__2 =
				           *m * *m + (*m << 2) + (*m - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", m, m, m, &c_n1, 6,
				                   1);
				maxwrk = MAX (i__1, i__2);
				/* Computing MAX */
				i__1 = maxwrk, i__2 = *m * *m + *m + bdspac;
				maxwrk = MAX (i__1, i__2);
				if (*nrhs > 1) {
					/* Computing MAX */
					i__1 = maxwrk, i__2 = *m * *m + *m + *m * *nrhs;
					maxwrk = MAX (i__1, i__2);
				} else {
					/* Computing MAX */
					i__1 = maxwrk, i__2 = *m * *m + (*m << 1);
					maxwrk = MAX (i__1, i__2);
				}
				/* Computing MAX */
				i__1 = maxwrk, i__2 =
				           *m + *nrhs * NUMlapack_ilaenv (&c__1, "DORMLQ", "LT", n, nrhs, m, &c_n1, 6, 2);
				maxwrk = MAX (i__1, i__2);
			} else {

				/* Path 2 - underdetermined */

				maxwrk = *m * 3 + (*n + *m) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, n, &c_n1, &c_n1, 6, 1);
				/* Computing MAX */
				i__1 = maxwrk, i__2 =
				           *m * 3 + *nrhs * NUMlapack_ilaenv (&c__1, "DORMBR", "QLT", m, nrhs, m, &c_n1, 6, 3);
				maxwrk = MAX (i__1, i__2);
				/* Computing MAX */
				i__1 = maxwrk, i__2 =
				           *m * 3 + *m * NUMlapack_ilaenv (&c__1, "DORGBR", "P", m, n, m, &c_n1, 6, 1);
				maxwrk = MAX (i__1, i__2);
				maxwrk = MAX (maxwrk, bdspac);
				/* Computing MAX */
				i__1 = maxwrk, i__2 = *n * *nrhs;
				maxwrk = MAX (i__1, i__2);
			}
		}
		maxwrk = MAX (minwrk, maxwrk);
		work[1] = (double) maxwrk;
	}

	minwrk = MAX (minwrk, 1);
	if (*lwork < minwrk && !lquery) {
		*info = -12;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGELSS", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*m == 0 || *n == 0) {
		*rank = 0;
		return 0;
	}

	/* Get machine parameters */

	eps = NUMblas_dlamch ("P");
	sfmin = NUMblas_dlamch ("S");
	smlnum = sfmin / eps;
	bignum = 1. / smlnum;
	NUMlapack_dlabad (&smlnum, &bignum);

	/* Scale A if max element outside range [SMLNUM,BIGNUM] */

	anrm = NUMlapack_dlange ("M", m, n, &a[a_offset], lda, &work[1]);
	iascl = 0;
	if (anrm > 0. && anrm < smlnum) {

		/* Scale matrix norm up to SMLNUM */

		NUMlapack_dlascl ("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda, info);
		iascl = 1;
	} else if (anrm > bignum) {

		/* Scale matrix norm down to BIGNUM */

		NUMlapack_dlascl ("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda, info);
		iascl = 2;
	} else if (anrm == 0.) {

		/* Matrix all zero. Return zero solution. */

		i__1 = MAX (*m, *n);
		NUMlapack_dlaset ("F", &i__1, nrhs, &c_b74, &c_b74, &b[b_offset], ldb);
		NUMlapack_dlaset ("F", &minmn, &c__1, &c_b74, &c_b74, &s[1], &c__1);
		*rank = 0;
		goto L70;
	}

	/* Scale B if max element outside range [SMLNUM,BIGNUM] */

	bnrm = NUMlapack_dlange ("M", m, nrhs, &b[b_offset], ldb, &work[1]);
	ibscl = 0;
	if (bnrm > 0. && bnrm < smlnum) {

		/* Scale matrix norm up to SMLNUM */

		NUMlapack_dlascl ("G", &c__0, &c__0, &bnrm, &smlnum, m, nrhs, &b[b_offset], ldb, info);
		ibscl = 1;
	} else if (bnrm > bignum) {

		/* Scale matrix norm down to BIGNUM */

		NUMlapack_dlascl ("G", &c__0, &c__0, &bnrm, &bignum, m, nrhs, &b[b_offset], ldb, info);
		ibscl = 2;
	}

	/* Overdetermined case */

	if (*m >= *n) {

		/* Path 1 - overdetermined or exactly determined */

		mm = *m;
		if (*m >= mnthr) {

			/* Path 1a - overdetermined, with many more rows than columns */

			mm = *n;
			itau = 1;
			iwork = itau + *n;

			/* Compute A=Q*R (Workspace: need 2*N, prefer N+N*NB) */

			i__1 = *lwork - iwork + 1;
			NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__1, info);

			/* Multiply B by transpose(Q) (Workspace: need N+NRHS, prefer
			   N+NRHS*NB) */

			i__1 = *lwork - iwork + 1;
			NUMlapack_dormqr ("L", "T", m, nrhs, n, &a[a_offset], lda, &work[itau], &b[b_offset], ldb,
			                  &work[iwork], &i__1, info);

			/* Zero out below R */

			if (*n > 1) {
				i__1 = *n - 1;
				i__2 = *n - 1;
				NUMlapack_dlaset ("L", &i__1, &i__2, &c_b74, &c_b74, &a_ref (2, 1), lda);
			}
		}

		ie = 1;
		itauq = ie + *n;
		itaup = itauq + *n;
		iwork = itaup + *n;

		/* Bidiagonalize R in A (Workspace: need 3*N+MM, prefer
		   3*N+(MM+N)*NB) */

		i__1 = *lwork - iwork + 1;
		NUMlapack_dgebrd (&mm, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup], &work[iwork],
		                  &i__1, info);

		/* Multiply B by transpose of left bidiagonalizing vectors of R
		   (Workspace: need 3*N+NRHS, prefer 3*N+NRHS*NB) */

		i__1 = *lwork - iwork + 1;
		NUMlapack_dormbr ("Q", "L", "T", &mm, nrhs, n, &a[a_offset], lda, &work[itauq], &b[b_offset], ldb,
		                  &work[iwork], &i__1, info);

		/* Generate right bidiagonalizing vectors of R in A (Workspace: need
		   4*N-1, prefer 3*N+(N-1)*NB) */

		i__1 = *lwork - iwork + 1;
		NUMlapack_dorgbr ("P", n, n, n, &a[a_offset], lda, &work[itaup], &work[iwork], &i__1, info);
		iwork = ie + *n;

		/* Perform bidiagonal QR iteration multiply B by transpose of left
		   singular vectors compute right singular vectors in A (Workspace:
		   need BDSPAC) */

		NUMlapack_dbdsqr ("U", n, n, &c__0, nrhs, &s[1], &work[ie], &a[a_offset], lda, vdum, &c__1,
		                  &b[b_offset], ldb, &work[iwork], info);
		if (*info != 0) {
			goto L70;
		}

		/* Multiply B by reciprocals of singular values

		   Computing MAX */
		d__1 = *rcond * s[1];
		thr = MAX (d__1, sfmin);
		if (*rcond < 0.) {
			/* Computing MAX */
			d__1 = eps * s[1];
			thr = MAX (d__1, sfmin);
		}
		*rank = 0;
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
			if (s[i__] > thr) {
				NUMlapack_drscl (nrhs, &s[i__], &b_ref (i__, 1), ldb);
				++ (*rank);
			} else {
				NUMlapack_dlaset ("F", &c__1, nrhs, &c_b74, &c_b74, &b_ref (i__, 1), ldb);
			}
			/* L10: */
		}

		/* Multiply B by right singular vectors (Workspace: need N, prefer
		   N*NRHS) */

		if (*lwork >= *ldb * *nrhs && *nrhs > 1) {
			NUMblas_dgemm ("T", "N", n, nrhs, n, &c_b108, &a[a_offset], lda, &b[b_offset], ldb, &c_b74, &work[1],
			               ldb);
			NUMlapack_dlacpy ("G", n, nrhs, &work[1], ldb, &b[b_offset], ldb);
		} else if (*nrhs > 1) {
			chunk = *lwork / *n;
			i__1 = *nrhs;
			i__2 = chunk;
			for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
				/* Computing MIN */
				i__3 = *nrhs - i__ + 1;
				bl = MIN (i__3, chunk);
				NUMblas_dgemm ("T", "N", n, &bl, n, &c_b108, &a[a_offset], lda, &b_ref (1, i__), ldb, &c_b74,
				               &work[1], n);
				NUMlapack_dlacpy ("G", n, &bl, &work[1], n, &b_ref (1, i__), ldb);
				/* L20: */
			}
		} else {
			NUMblas_dgemv ("T", n, n, &c_b108, &a[a_offset], lda, &b[b_offset], &c__1, &c_b74, &work[1], &c__1);
			NUMblas_dcopy (n, &work[1], &c__1, &b[b_offset], &c__1);
		}

	} else {					/* if(complicated condition) */
		/* Computing MAX */
		i__2 = *m, i__1 = (*m << 1) - 4, i__2 = MAX (i__2, i__1), i__2 = MAX (i__2, *nrhs), i__1 =
		        *n - *m * 3;
		if (*n >= mnthr && *lwork >= (*m << 2) + *m * *m + MAX (i__2, i__1)) {

			/* Path 2a - underdetermined, with many more columns than rows
			   and sufficient workspace for an efficient algorithm */

			ldwork = *m;
			/* Computing MAX Computing MAX */
			i__3 = *m, i__4 = (*m << 1) - 4, i__3 = MAX (i__3, i__4), i__3 = MAX (i__3, *nrhs), i__4 =
			        *n - *m * 3;
			i__2 = (*m << 2) + *m * *lda + MAX (i__3, i__4), i__1 = *m * *lda + *m + *m * *nrhs;
			if (*lwork >= MAX (i__2, i__1)) {
				ldwork = *lda;
			}
			itau = 1;
			iwork = *m + 1;

			/* Compute A=L*Q (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, info);
			il = iwork;

			/* Copy L to WORK(IL), zeroing out above it */

			NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &work[il], &ldwork);
			i__2 = *m - 1;
			i__1 = *m - 1;
			NUMlapack_dlaset ("U", &i__2, &i__1, &c_b74, &c_b74, &work[il + ldwork], &ldwork);
			ie = il + ldwork * *m;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

			/* Bidiagonalize L in WORK(IL) (Workspace: need M*M+5*M, prefer
			   M*M+4*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			NUMlapack_dgebrd (m, m, &work[il], &ldwork, &s[1], &work[ie], &work[itauq], &work[itaup],
			                  &work[iwork], &i__2, info);

			/* Multiply B by transpose of left bidiagonalizing vectors of L
			   (Workspace: need M*M+4*M+NRHS, prefer M*M+4*M+NRHS*NB) */

			i__2 = *lwork - iwork + 1;
			NUMlapack_dormbr ("Q", "L", "T", m, nrhs, m, &work[il], &ldwork, &work[itauq], &b[b_offset], ldb,
			                  &work[iwork], &i__2, info);

			/* Generate right bidiagonalizing vectors of R in WORK(IL)
			   (Workspace: need M*M+5*M-1, prefer M*M+4*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			NUMlapack_dorgbr ("P", m, m, m, &work[il], &ldwork, &work[itaup], &work[iwork], &i__2, info);
			iwork = ie + *m;

			/* Perform bidiagonal QR iteration, computing right singular
			   vectors of L in WORK(IL) and multiplying B by transpose of
			   left singular vectors (Workspace: need M*M+M+BDSPAC) */

			NUMlapack_dbdsqr ("U", m, m, &c__0, nrhs, &s[1], &work[ie], &work[il], &ldwork, &a[a_offset], lda,
			                  &b[b_offset], ldb, &work[iwork], info);
			if (*info != 0) {
				goto L70;
			}

			/* Multiply B by reciprocals of singular values

			   Computing MAX */
			d__1 = *rcond * s[1];
			thr = MAX (d__1, sfmin);
			if (*rcond < 0.) {
				/* Computing MAX */
				d__1 = eps * s[1];
				thr = MAX (d__1, sfmin);
			}
			*rank = 0;
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
				if (s[i__] > thr) {
					NUMlapack_drscl (nrhs, &s[i__], &b_ref (i__, 1), ldb);
					++ (*rank);
				} else {
					NUMlapack_dlaset ("F", &c__1, nrhs, &c_b74, &c_b74, &b_ref (i__, 1), ldb);
				}
				/* L30: */
			}
			iwork = ie;

			/* Multiply B by right singular vectors of L in WORK(IL)
			   (Workspace: need M*M+2*M, prefer M*M+M+M*NRHS) */

			if (*lwork >= *ldb * *nrhs + iwork - 1 && *nrhs > 1) {
				NUMblas_dgemm ("T", "N", m, nrhs, m, &c_b108, &work[il], &ldwork, &b[b_offset], ldb, &c_b74,
				               &work[iwork], ldb);
				NUMlapack_dlacpy ("G", m, nrhs, &work[iwork], ldb, &b[b_offset], ldb);
			} else if (*nrhs > 1) {
				chunk = (*lwork - iwork + 1) / *m;
				i__2 = *nrhs;
				i__1 = chunk;
				for (i__ = 1; i__1 < 0 ? i__ >= i__2 : i__ <= i__2; i__ += i__1) {
					/* Computing MIN */
					i__3 = *nrhs - i__ + 1;
					bl = MIN (i__3, chunk);
					NUMblas_dgemm ("T", "N", m, &bl, m, &c_b108, &work[il], &ldwork, &b_ref (1, i__), ldb, &c_b74,
					               &work[iwork], n);
					NUMlapack_dlacpy ("G", m, &bl, &work[iwork], n, &b_ref (1, i__), ldb);
					/* L40: */
				}
			} else {
				NUMblas_dgemv ("T", m, m, &c_b108, &work[il], &ldwork, &b_ref (1, 1), &c__1, &c_b74, &work[iwork],
				               &c__1);
				NUMblas_dcopy (m, &work[iwork], &c__1, &b_ref (1, 1), &c__1);
			}

			/* Zero out below first M rows of B */

			i__1 = *n - *m;
			NUMlapack_dlaset ("F", &i__1, nrhs, &c_b74, &c_b74, &b_ref (*m + 1, 1), ldb);
			iwork = itau + *m;

			/* Multiply transpose(Q) by B (Workspace: need M+NRHS, prefer
			   M+NRHS*NB) */

			i__1 = *lwork - iwork + 1;
			NUMlapack_dormlq ("L", "T", n, nrhs, m, &a[a_offset], lda, &work[itau], &b[b_offset], ldb,
			                  &work[iwork], &i__1, info);

		} else {

			/* Path 2 - remaining underdetermined cases */

			ie = 1;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

			/* Bidiagonalize A (Workspace: need 3*M+N, prefer 3*M+(M+N)*NB) */

			i__1 = *lwork - iwork + 1;
			NUMlapack_dgebrd (m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
			                  &work[iwork], &i__1, info);

			/* Multiply B by transpose of left bidiagonalizing vectors
			   (Workspace: need 3*M+NRHS, prefer 3*M+NRHS*NB) */

			i__1 = *lwork - iwork + 1;
			NUMlapack_dormbr ("Q", "L", "T", m, nrhs, n, &a[a_offset], lda, &work[itauq], &b[b_offset], ldb,
			                  &work[iwork], &i__1, info);

			/* Generate right bidiagonalizing vectors in A (Workspace: need
			   4*M, prefer 3*M+M*NB) */

			i__1 = *lwork - iwork + 1;
			NUMlapack_dorgbr ("P", m, n, m, &a[a_offset], lda, &work[itaup], &work[iwork], &i__1, info);
			iwork = ie + *m;

			/* Perform bidiagonal QR iteration, computing right singular
			   vectors of A in A and multiplying B by transpose of left
			   singular vectors (Workspace: need BDSPAC) */

			NUMlapack_dbdsqr ("L", m, n, &c__0, nrhs, &s[1], &work[ie], &a[a_offset], lda, vdum, &c__1,
			                  &b[b_offset], ldb, &work[iwork], info);
			if (*info != 0) {
				goto L70;
			}

			/* Multiply B by reciprocals of singular values

			   Computing MAX */
			d__1 = *rcond * s[1];
			thr = MAX (d__1, sfmin);
			if (*rcond < 0.) {
				/* Computing MAX */
				d__1 = eps * s[1];
				thr = MAX (d__1, sfmin);
			}
			*rank = 0;
			i__1 = *m;
			for (i__ = 1; i__ <= i__1; ++i__) {
				if (s[i__] > thr) {
					NUMlapack_drscl (nrhs, &s[i__], &b_ref (i__, 1), ldb);
					++ (*rank);
				} else {
					NUMlapack_dlaset ("F", &c__1, nrhs, &c_b74, &c_b74, &b_ref (i__, 1), ldb);
				}
				/* L50: */
			}

			/* Multiply B by right singular vectors of A (Workspace: need N,
			   prefer N*NRHS) */

			if (*lwork >= *ldb * *nrhs && *nrhs > 1) {
				NUMblas_dgemm ("T", "N", n, nrhs, m, &c_b108, &a[a_offset], lda, &b[b_offset], ldb, &c_b74, &work[1],
				               ldb);
				NUMlapack_dlacpy ("F", n, nrhs, &work[1], ldb, &b[b_offset], ldb);
			} else if (*nrhs > 1) {
				chunk = *lwork / *n;
				i__1 = *nrhs;
				i__2 = chunk;
				for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
					/* Computing MIN */
					i__3 = *nrhs - i__ + 1;
					bl = MIN (i__3, chunk);
					NUMblas_dgemm ("T", "N", n, &bl, m, &c_b108, &a[a_offset], lda, &b_ref (1, i__), ldb, &c_b74,
					               &work[1], n);
					NUMlapack_dlacpy ("F", n, &bl, &work[1], n, &b_ref (1, i__), ldb);
					/* L60: */
				}
			} else {
				NUMblas_dgemv ("T", m, n, &c_b108, &a[a_offset], lda, &b[b_offset], &c__1, &c_b74, &work[1], &c__1);
				NUMblas_dcopy (n, &work[1], &c__1, &b[b_offset], &c__1);
			}
		}
	}

	/* Undo scaling */

	if (iascl == 1) {
		NUMlapack_dlascl ("G", &c__0, &c__0, &anrm, &smlnum, n, nrhs, &b[b_offset], ldb, info);
		NUMlapack_dlascl ("G", &c__0, &c__0, &smlnum, &anrm, &minmn, &c__1, &s[1], &minmn, info);
	} else if (iascl == 2) {
		NUMlapack_dlascl ("G", &c__0, &c__0, &anrm, &bignum, n, nrhs, &b[b_offset], ldb, info);
		NUMlapack_dlascl ("G", &c__0, &c__0, &bignum, &anrm, &minmn, &c__1, &s[1], &minmn, info);
	}
	if (ibscl == 1) {
		NUMlapack_dlascl ("G", &c__0, &c__0, &smlnum, &bnrm, n, nrhs, &b[b_offset], ldb, info);
	} else if (ibscl == 2) {
		NUMlapack_dlascl ("G", &c__0, &c__0, &bignum, &bnrm, n, nrhs, &b[b_offset], ldb, info);
	}

L70:
	work[1] = (double) maxwrk;
	return 0;
}								/* NUMlapack_dgelss */

#undef b_ref

int NUMlapack_dgeqpf (long *m, long *n, double *a, long *lda, long *jpvt, double *tau, double *work, long *info) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;
	double d__1, d__2;

	/* Local variables */
	static double temp;
	static double temp2;
	static long i__, j;
	static long itemp;
	static long ma, mn;
	static double aii;
	static long pvt;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--jpvt;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *m)) {
		*info = -4;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGEQPF", &i__1);
		return 0;
	}

	mn = MIN (*m, *n);

	/* Move initial columns up front */

	itemp = 1;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		if (jpvt[i__] != 0) {
			if (i__ != itemp) {
				NUMblas_dswap (m, &a_ref (1, i__), &c__1, &a_ref (1, itemp), &c__1);
				jpvt[i__] = jpvt[itemp];
				jpvt[itemp] = i__;
			} else {
				jpvt[i__] = i__;
			}
			++itemp;
		} else {
			jpvt[i__] = i__;
		}
		/* L10: */
	}
	--itemp;

	/* Compute the QR factorization and update remaining columns */

	if (itemp > 0) {
		ma = MIN (itemp, *m);
		NUMlapack_dgeqr2 (m, &ma, &a[a_offset], lda, &tau[1], &work[1], info);
		if (ma < *n) {
			i__1 = *n - ma;
			NUMlapack_dorm2r ("Left", "Transpose", m, &i__1, &ma, &a[a_offset], lda, &tau[1], &a_ref (1,
			                  ma + 1), lda, &work[1], info);
		}
	}

	if (itemp < mn) {

		/* Initialize partial column norms. The first n elements of work
		   store the exact column norms. */

		i__1 = *n;
		for (i__ = itemp + 1; i__ <= i__1; ++i__) {
			i__2 = *m - itemp;
			work[i__] = NUMblas_dnrm2 (&i__2, &a_ref (itemp + 1, i__), &c__1);
			work[*n + i__] = work[i__];
			/* L20: */
		}

		/* Compute factorization */

		i__1 = mn;
		for (i__ = itemp + 1; i__ <= i__1; ++i__) {

			/* Determine ith pivot column and swap if necessary */

			i__2 = *n - i__ + 1;
			pvt = i__ - 1 + NUMblas_idamax (&i__2, &work[i__], &c__1);

			if (pvt != i__) {
				NUMblas_dswap (m, &a_ref (1, pvt), &c__1, &a_ref (1, i__), &c__1);
				itemp = jpvt[pvt];
				jpvt[pvt] = jpvt[i__];
				jpvt[i__] = itemp;
				work[pvt] = work[i__];
				work[*n + pvt] = work[*n + i__];
			}

			/* Generate elementary reflector H(i) */

			if (i__ < *m) {
				i__2 = *m - i__ + 1;
				NUMlapack_dlarfg (&i__2, &a_ref (i__, i__), &a_ref (i__ + 1, i__), &c__1, &tau[i__]);
			} else {
				NUMlapack_dlarfg (&c__1, &a_ref (*m, *m), &a_ref (*m, *m), &c__1, &tau[*m]);
			}

			if (i__ < *n) {

				/* Apply H(i) to A(i:m,i+1:n) from the left */

				aii = a_ref (i__, i__);
				a_ref (i__, i__) = 1.;
				i__2 = *m - i__ + 1;
				i__3 = *n - i__;
				NUMlapack_dlarf ("LEFT", &i__2, &i__3, &a_ref (i__, i__), &c__1, &tau[i__], &a_ref (i__,
				                 i__ + 1), lda, &work[ (*n << 1) + 1]);
				a_ref (i__, i__) = aii;
			}

			/* Update partial column norms */

			i__2 = *n;
			for (j = i__ + 1; j <= i__2; ++j) {
				if (work[j] != 0.) {
					/* Computing 2nd power */
					d__2 = (d__1 = a_ref (i__, j), fabs (d__1)) / work[j];
					temp = 1. - d__2 * d__2;
					temp = MAX (temp, 0.);
					/* Computing 2nd power */
					d__1 = work[j] / work[*n + j];
					temp2 = temp * .05 * (d__1 * d__1) + 1.;
					if (temp2 == 1.) {
						if (*m - i__ > 0) {
							i__3 = *m - i__;
							work[j] = NUMblas_dnrm2 (&i__3, &a_ref (i__ + 1, j), &c__1);
							work[*n + j] = work[j];
						} else {
							work[j] = 0.;
							work[*n + j] = 0.;
						}
					} else {
						work[j] *= sqrt (temp);
					}
				}
				/* L30: */
			}

			/* L40: */
		}
	}
	return 0;
}								/* NUMlapack_dgeqpf */

int NUMlapack_dgeqr2 (long *m, long *n, double *a, long *lda, double *tau, double *work, long *info) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;

	/* Local variables */
	static long i__, k;
	static double aii;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *m)) {
		*info = -4;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGEQR2", &i__1);
		return 0;
	}

	k = MIN (*m, *n);

	i__1 = k;
	for (i__ = 1; i__ <= i__1; ++i__) {

		/* Generate elementary reflector H(i) to annihilate A(i+1:m,i)

		   Computing MIN */
		i__2 = i__ + 1;
		i__3 = *m - i__ + 1;
		NUMlapack_dlarfg (&i__3, &a_ref (i__, i__), &a_ref (MIN (i__2, *m), i__), &c__1, &tau[i__]);
		if (i__ < *n) {

			/* Apply H(i) to A(i:m,i+1:n) from the left */

			aii = a_ref (i__, i__);
			a_ref (i__, i__) = 1.;
			i__2 = *m - i__ + 1;
			i__3 = *n - i__;
			NUMlapack_dlarf ("Left", &i__2, &i__3, &a_ref (i__, i__), &c__1, &tau[i__], &a_ref (i__, i__ + 1),
			                 lda, &work[1]);
			a_ref (i__, i__) = aii;
		}
		/* L10: */
	}
	return 0;
}								/* NUMlapack_dgeqr2 */

int NUMlapack_dgeqrf (long *m, long *n, double *a, long *lda, double *tau, double *work, long *lwork,
                      long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__3 = 3;
	static long c__2 = 2;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3, i__4;

	/* Local variables */
	static long i__, k, nbmin, iinfo;
	static long ib, nb;
	static long nx;
	static long ldwork, lwkopt;
	static long lquery;
	static long iws;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	nb = NUMlapack_ilaenv (&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
	lwkopt = *n * nb;
	work[1] = (double) lwkopt;
	lquery = *lwork == -1;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *m)) {
		*info = -4;
	} else if (*lwork < MAX (1, *n) && !lquery) {
		*info = -7;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGEQRF", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	k = MIN (*m, *n);
	if (k == 0) {
		work[1] = 1.;
		return 0;
	}

	nbmin = 2;
	nx = 0;
	iws = *n;
	if (nb > 1 && nb < k) {

		/* Determine when to cross over from blocked to unblocked code.

		   Computing MAX */
		i__1 = 0, i__2 = NUMlapack_ilaenv (&c__3, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
		nx = MAX (i__1, i__2);
		if (nx < k) {

			/* Determine if workspace is large enough for blocked code. */

			ldwork = *n;
			iws = ldwork * nb;
			if (*lwork < iws) {

				/* Not enough workspace to use optimal NB: reduce NB and
				   determine the minimum value of NB. */

				nb = *lwork / ldwork;
				/* Computing MAX */
				i__1 = 2, i__2 = NUMlapack_ilaenv (&c__2, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
				nbmin = MAX (i__1, i__2);
			}
		}
	}

	if (nb >= nbmin && nb < k && nx < k) {

		/* Use blocked code initially */

		i__1 = k - nx;
		i__2 = nb;
		for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
			/* Computing MIN */
			i__3 = k - i__ + 1;
			ib = MIN (i__3, nb);

			/* Compute the QR factorization of the current block
			   A(i:m,i:i+ib-1) */

			i__3 = *m - i__ + 1;
			NUMlapack_dgeqr2 (&i__3, &ib, &a_ref (i__, i__), lda, &tau[i__], &work[1], &iinfo);
			if (i__ + ib <= *n) {

				/* Form the triangular factor of the block reflector H = H(i)
				   H(i+1) . . . H(i+ib-1) */

				i__3 = *m - i__ + 1;
				NUMlapack_dlarft ("Forward", "Columnwise", &i__3, &ib, &a_ref (i__, i__), lda, &tau[i__],
				                  &work[1], &ldwork);

				/* Apply H' to A(i:m,i+ib:n) from the left */

				i__3 = *m - i__ + 1;
				i__4 = *n - i__ - ib + 1;
				NUMlapack_dlarfb ("Left", "Transpose", "Forward", "Columnwise", &i__3, &i__4, &ib, &a_ref (i__,
				                  i__), lda, &work[1], &ldwork, &a_ref (i__, i__ + ib), lda, &work[ib + 1], &ldwork);
			}
			/* L10: */
		}
	} else {
		i__ = 1;
	}

	/* Use unblocked code to factor the last or only block. */

	if (i__ <= k) {
		i__2 = *m - i__ + 1;
		i__1 = *n - i__ + 1;
		NUMlapack_dgeqr2 (&i__2, &i__1, &a_ref (i__, i__), lda, &tau[i__], &work[1], &iinfo);
	}

	work[1] = (double) iws;
	return 0;
}								/* NUMlapack_dgeqrf */

int NUMlapack_dgerq2 (long *m, long *n, double *a, long *lda, double *tau, double *work, long *info) {
	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2;

	/* Local variables */
	static long i__, k;
	static double aii;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *m)) {
		*info = -4;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGERQ2", &i__1);
		return 0;
	}

	k = MIN (*m, *n);

	for (i__ = k; i__ >= 1; --i__) {

		/* Generate elementary reflector H(i) to annihilate
		   A(m-k+i,1:n-k+i-1) */

		i__1 = *n - k + i__;
		NUMlapack_dlarfg (&i__1, &a_ref (*m - k + i__, *n - k + i__), &a_ref (*m - k + i__, 1), lda, &tau[i__]);

		/* Apply H(i) to A(1:m-k+i-1,1:n-k+i) from the right */

		aii = a_ref (*m - k + i__, *n - k + i__);
		a_ref (*m - k + i__, *n - k + i__) = 1.;
		i__1 = *m - k + i__ - 1;
		i__2 = *n - k + i__;
		NUMlapack_dlarf ("Right", &i__1, &i__2, &a_ref (*m - k + i__, 1), lda, &tau[i__], &a[a_offset], lda,
		                 &work[1]);
		a_ref (*m - k + i__, *n - k + i__) = aii;
		/* L10: */
	}
	return 0;
}								/* NUMlapack_dgerq2 */

int NUMlapack_dgesv (long *n, long *nrhs, double *a, long *lda, long *ipiv, double *b, long *ldb, long *info) {
	/* System generated locals */
	long a_dim1, a_offset, b_dim1, b_offset, i__1;

	/* Local variables */

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--ipiv;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1 * 1;
	b -= b_offset;

	/* Function Body */
	*info = 0;
	if (*n < 0) {
		*info = -1;
	} else if (*nrhs < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *n)) {
		*info = -4;
	} else if (*ldb < MAX (1, *n)) {
		*info = -7;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGESV ", &i__1);
		return 0;
	}

	/* Compute the LU factorization of A. */

	NUMlapack_dgetrf (n, n, &a[a_offset], lda, &ipiv[1], info);
	if (*info == 0) {

		/* Solve the system A*X = B, overwriting B with X. */

		NUMlapack_dgetrs ("No transpose", n, nrhs, &a[a_offset], lda, &ipiv[1], &b[b_offset], ldb, info);
	}
	return 0;
}								/* NUMlapack_dgesv */

#define u_ref(a_1,a_2) u[(a_2)*u_dim1 + a_1]
#define vt_ref(a_1,a_2) vt[(a_2)*vt_dim1 + a_1]

int NUMlapack_dgesvd (const char *jobu, const char *jobvt, long *m, long *n, double *a, long *lda, double *s, double *u,
                      long *ldu, double *vt, long *ldvt, double *work, long *lwork, long *info) {
	/* System generated locals */
	const char *a__1[2];
	long a_dim1, a_offset, u_dim1, u_offset, vt_dim1, vt_offset, i__1[2], i__2, i__3, i__4;
	char ch__1[2];

	/* Local variables */
	static long iscl;
	static double anrm;
	static long ierr, itau, ncvt, nrvt, i__;
	static long chunk, minmn, wrkbl, itaup, itauq, mnthr, iwork;
	static long wntua, wntva, wntun, wntuo, wntvn, wntvo, wntus, wntvs;
	static long ie;
	static long ir, bdspac, iu;
	static double bignum;
	static long ldwrkr, minwrk, ldwrku, maxwrk;
	static double smlnum;
	static long lquery, wntuas, wntvas;
	static long blk, ncu;
	static double dum[1], eps;
	static long nru;

	/* Parameter adjustments */
	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--s;
	u_dim1 = *ldu;
	u_offset = 1 + u_dim1 * 1;
	u -= u_offset;
	vt_dim1 = *ldvt;
	vt_offset = 1 + vt_dim1 * 1;
	vt -= vt_offset;
	--work;

	/* Function Body */
	*info = 0;
	minmn = MIN (*m, *n);
	/* Writing concatenation */
	i__1[0] = 1, a__1[0] = jobu;
	i__1[1] = 1, a__1[1] = jobvt;
	s_cat (ch__1, a__1, i__1, &c__2, 2);
	mnthr = NUMlapack_ilaenv (&c__6, "DGESVD", ch__1, m, n, &c__0, &c__0, 6, 2);
	wntua = lsame_ (jobu, "A");
	wntus = lsame_ (jobu, "S");
	wntuas = wntua || wntus;
	wntuo = lsame_ (jobu, "O");
	wntun = lsame_ (jobu, "N");
	wntva = lsame_ (jobvt, "A");
	wntvs = lsame_ (jobvt, "S");
	wntvas = wntva || wntvs;
	wntvo = lsame_ (jobvt, "O");
	wntvn = lsame_ (jobvt, "N");
	minwrk = 1;
	lquery = *lwork == -1;

	if (! (wntua || wntus || wntuo || wntun)) {
		*info = -1;
	} else if (! (wntva || wntvs || wntvo || wntvn) || wntvo && wntuo) {
		*info = -2;
	} else if (*m < 0) {
		*info = -3;
	} else if (*n < 0) {
		*info = -4;
	} else if (*lda < MAX (1, *m)) {
		*info = -6;
	} else if (*ldu < 1 || wntuas && *ldu < *m) {
		*info = -9;
	} else if (*ldvt < 1 || wntva && *ldvt < *n || wntvs && *ldvt < minmn) {
		*info = -11;
	}

	/* Compute workspace (Note: Comments in the code beginning "Workspace:"
	   describe the minimal amount of workspace needed at that point in the
	   code, as well as the preferred amount for good performance. NB refers
	   to the optimal block size for the immediately following subroutine, as
	   returned by ILAENV.) */

	if (*info == 0 && (*lwork >= 1 || lquery) && *m > 0 && *n > 0) {
		if (*m >= *n) {

			/* Compute space needed for DBDSQR */

			bdspac = *n * 5;
			if (*m >= mnthr) {
				if (wntun) {

					/* Path 1 (M much larger than N, JOBU='N') */

					maxwrk = *n + *n * NUMlapack_ilaenv (&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = maxwrk, i__3 =
					           *n * 3 + (*n << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, 6, 1);
					maxwrk = MAX (i__2, i__3);
					if (wntvo || wntvas) {
						/* Computing MAX */
						i__2 = maxwrk, i__3 =
						           *n * 3 + (*n - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", n, n, n, &c_n1, 6, 1);
						maxwrk = MAX (i__2, i__3);
					}
					maxwrk = MAX (maxwrk, bdspac);
					/* Computing MAX */
					i__2 = *n << 2;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntuo && wntvn) {

					/* Path 2 (M much larger than N, JOBU='O', JOBVT='N') */

					wrkbl = *n + *n * NUMlapack_ilaenv (&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n + *n * NUMlapack_ilaenv (&c__1, "DORGQR", " ", m, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + *n * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					/* Computing MAX */
					i__2 = *n * *n + wrkbl, i__3 = *n * *n + *m * *n + *n;
					maxwrk = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = *n * 3 + *m;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntuo && wntvas) {

					/* Path 3 (M much larger than N, JOBU='O', JOBVT='S' or
					   'A') */

					wrkbl = *n + *n * NUMlapack_ilaenv (&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n + *n * NUMlapack_ilaenv (&c__1, "DORGQR", " ", m, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + *n * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					/* Computing MAX */
					i__2 = *n * *n + wrkbl, i__3 = *n * *n + *m * *n + *n;
					maxwrk = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = *n * 3 + *m;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntus && wntvn) {

					/* Path 4 (M much larger than N, JOBU='S', JOBVT='N') */

					wrkbl = *n + *n * NUMlapack_ilaenv (&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n + *n * NUMlapack_ilaenv (&c__1, "DORGQR", " ", m, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + *n * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					maxwrk = *n * *n + wrkbl;
					/* Computing MAX */
					i__2 = *n * 3 + *m;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntus && wntvo) {

					/* Path 5 (M much larger than N, JOBU='S', JOBVT='O') */

					wrkbl = *n + *n * NUMlapack_ilaenv (&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n + *n * NUMlapack_ilaenv (&c__1, "DORGQR", " ", m, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + *n * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					maxwrk = (*n << 1) * *n + wrkbl;
					/* Computing MAX */
					i__2 = *n * 3 + *m;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntus && wntvas) {

					/* Path 6 (M much larger than N, JOBU='S', JOBVT='S' or
					   'A') */

					wrkbl = *n + *n * NUMlapack_ilaenv (&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n + *n * NUMlapack_ilaenv (&c__1, "DORGQR", " ", m, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + *n * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					maxwrk = *n * *n + wrkbl;
					/* Computing MAX */
					i__2 = *n * 3 + *m;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntua && wntvn) {

					/* Path 7 (M much larger than N, JOBU='A', JOBVT='N') */

					wrkbl = *n + *n * NUMlapack_ilaenv (&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n + *m * NUMlapack_ilaenv (&c__1, "DORGQR", " ", m, m, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + *n * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					maxwrk = *n * *n + wrkbl;
					/* Computing MAX */
					i__2 = *n * 3 + *m;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntua && wntvo) {

					/* Path 8 (M much larger than N, JOBU='A', JOBVT='O') */

					wrkbl = *n + *n * NUMlapack_ilaenv (&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n + *m * NUMlapack_ilaenv (&c__1, "DORGQR", " ", m, m, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + *n * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					maxwrk = (*n << 1) * *n + wrkbl;
					/* Computing MAX */
					i__2 = *n * 3 + *m;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntua && wntvas) {

					/* Path 9 (M much larger than N, JOBU='A', JOBVT='S' or
					   'A') */

					wrkbl = *n + *n * NUMlapack_ilaenv (&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n + *m * NUMlapack_ilaenv (&c__1, "DORGQR", " ", m, m, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + *n * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *n * 3 + (*n - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", n, n, n, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					maxwrk = *n * *n + wrkbl;
					/* Computing MAX */
					i__2 = *n * 3 + *m;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				}
			} else {

				/* Path 10 (M at least N, but not much larger) */

				maxwrk = *n * 3 + (*m + *n) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, n, &c_n1, &c_n1, 6, 1);
				if (wntus || wntuo) {
					/* Computing MAX */
					i__2 = maxwrk, i__3 =
					           *n * 3 + *n * NUMlapack_ilaenv (&c__1, "DORG" "BR", "Q", m, n, n, &c_n1, 6, 1);
					maxwrk = MAX (i__2, i__3);
				}
				if (wntua) {
					/* Computing MAX */
					i__2 = maxwrk, i__3 =
					           *n * 3 + *m * NUMlapack_ilaenv (&c__1, "DORG" "BR", "Q", m, m, n, &c_n1, 6, 1);
					maxwrk = MAX (i__2, i__3);
				}
				if (!wntvn) {
					/* Computing MAX */
					i__2 = maxwrk, i__3 =
					           *n * 3 + (*n - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", n, n, n, &c_n1, 6, 1);
					maxwrk = MAX (i__2, i__3);
				}
				maxwrk = MAX (maxwrk, bdspac);
				/* Computing MAX */
				i__2 = *n * 3 + *m;
				minwrk = MAX (i__2, bdspac);
				maxwrk = MAX (maxwrk, minwrk);
			}
		} else {

			/* Compute space needed for DBDSQR */

			bdspac = *m * 5;
			if (*n >= mnthr) {
				if (wntvn) {

					/* Path 1t(N much larger than M, JOBVT='N') */

					maxwrk = *m + *m * NUMlapack_ilaenv (&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = maxwrk, i__3 =
					           *m * 3 + (*m << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, 6, 1);
					maxwrk = MAX (i__2, i__3);
					if (wntuo || wntuas) {
						/* Computing MAX */
						i__2 = maxwrk, i__3 =
						           *m * 3 + *m * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", m, m, m, &c_n1, 6, 1);
						maxwrk = MAX (i__2, i__3);
					}
					maxwrk = MAX (maxwrk, bdspac);
					/* Computing MAX */
					i__2 = *m << 2;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntvo && wntun) {

					/* Path 2t(N much larger than M, JOBU='N', JOBVT='O') */

					wrkbl = *m + *m * NUMlapack_ilaenv (&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m + *m * NUMlapack_ilaenv (&c__1, "DORGLQ", " ", m, n, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					/* Computing MAX */
					i__2 = *m * *m + wrkbl, i__3 = *m * *m + *m * *n + *m;
					maxwrk = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = *m * 3 + *n;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntvo && wntuas) {

					/* Path 3t(N much larger than M, JOBU='S' or 'A',
					   JOBVT='O') */

					wrkbl = *m + *m * NUMlapack_ilaenv (&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m + *m * NUMlapack_ilaenv (&c__1, "DORGLQ", " ", m, n, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + *m * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					/* Computing MAX */
					i__2 = *m * *m + wrkbl, i__3 = *m * *m + *m * *n + *m;
					maxwrk = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = *m * 3 + *n;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntvs && wntun) {

					/* Path 4t(N much larger than M, JOBU='N', JOBVT='S') */

					wrkbl = *m + *m * NUMlapack_ilaenv (&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m + *m * NUMlapack_ilaenv (&c__1, "DORGLQ", " ", m, n, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					maxwrk = *m * *m + wrkbl;
					/* Computing MAX */
					i__2 = *m * 3 + *n;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntvs && wntuo) {

					/* Path 5t(N much larger than M, JOBU='O', JOBVT='S') */

					wrkbl = *m + *m * NUMlapack_ilaenv (&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m + *m * NUMlapack_ilaenv (&c__1, "DORGLQ", " ", m, n, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + *m * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					maxwrk = (*m << 1) * *m + wrkbl;
					/* Computing MAX */
					i__2 = *m * 3 + *n;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntvs && wntuas) {

					/* Path 6t(N much larger than M, JOBU='S' or 'A',
					   JOBVT='S') */

					wrkbl = *m + *m * NUMlapack_ilaenv (&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m + *m * NUMlapack_ilaenv (&c__1, "DORGLQ", " ", m, n, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + *m * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					maxwrk = *m * *m + wrkbl;
					/* Computing MAX */
					i__2 = *m * 3 + *n;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntva && wntun) {

					/* Path 7t(N much larger than M, JOBU='N', JOBVT='A') */

					wrkbl = *m + *m * NUMlapack_ilaenv (&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m + *n * NUMlapack_ilaenv (&c__1, "DORGLQ", " ", n, n, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					maxwrk = *m * *m + wrkbl;
					/* Computing MAX */
					i__2 = *m * 3 + *n;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntva && wntuo) {

					/* Path 8t(N much larger than M, JOBU='O', JOBVT='A') */

					wrkbl = *m + *m * NUMlapack_ilaenv (&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m + *n * NUMlapack_ilaenv (&c__1, "DORGLQ", " ", n, n, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + *m * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					maxwrk = (*m << 1) * *m + wrkbl;
					/* Computing MAX */
					i__2 = *m * 3 + *n;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				} else if (wntva && wntuas) {

					/* Path 9t(N much larger than M, JOBU='S' or 'A',
					   JOBVT='A') */

					wrkbl = *m + *m * NUMlapack_ilaenv (&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1, 6, 1);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m + *n * NUMlapack_ilaenv (&c__1, "DORGLQ", " ", n, n, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m << 1) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + (*m - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "P", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					/* Computing MAX */
					i__2 = wrkbl, i__3 =
					           *m * 3 + *m * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", m, m, m, &c_n1, 6, 1);
					wrkbl = MAX (i__2, i__3);
					wrkbl = MAX (wrkbl, bdspac);
					maxwrk = *m * *m + wrkbl;
					/* Computing MAX */
					i__2 = *m * 3 + *n;
					minwrk = MAX (i__2, bdspac);
					maxwrk = MAX (maxwrk, minwrk);
				}
			} else {

				/* Path 10t(N greater than M, but not much larger) */

				maxwrk = *m * 3 + (*m + *n) * NUMlapack_ilaenv (&c__1, "DGEBRD", " ", m, n, &c_n1, &c_n1, 6, 1);
				if (wntvs || wntvo) {
					/* Computing MAX */
					i__2 = maxwrk, i__3 =
					           *m * 3 + *m * NUMlapack_ilaenv (&c__1, "DORG" "BR", "P", m, n, m, &c_n1, 6, 1);
					maxwrk = MAX (i__2, i__3);
				}
				if (wntva) {
					/* Computing MAX */
					i__2 = maxwrk, i__3 =
					           *m * 3 + *n * NUMlapack_ilaenv (&c__1, "DORG" "BR", "P", n, n, m, &c_n1, 6, 1);
					maxwrk = MAX (i__2, i__3);
				}
				if (!wntun) {
					/* Computing MAX */
					i__2 = maxwrk, i__3 =
					           *m * 3 + (*m - 1) * NUMlapack_ilaenv (&c__1, "DORGBR", "Q", m, m, m, &c_n1, 6, 1);
					maxwrk = MAX (i__2, i__3);
				}
				maxwrk = MAX (maxwrk, bdspac);
				/* Computing MAX */
				i__2 = *m * 3 + *n;
				minwrk = MAX (i__2, bdspac);
				maxwrk = MAX (maxwrk, minwrk);
			}
		}
		work[1] = (double) maxwrk;
	}

	if (*lwork < minwrk && !lquery) {
		*info = -13;
	}
	if (*info != 0) {
		i__2 = - (*info);
		xerbla_ ("DGESVD", &i__2);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*m == 0 || *n == 0) {
		if (*lwork >= 1) {
			work[1] = 1.;
		}
		return 0;
	}

	/* Get machine constants */

	eps = NUMblas_dlamch ("P");
	smlnum = sqrt (NUMblas_dlamch ("S")) / eps;
	bignum = 1. / smlnum;

	/* Scale A if max element outside range [SMLNUM,BIGNUM] */

	anrm = NUMlapack_dlange ("M", m, n, &a[a_offset], lda, dum);
	iscl = 0;
	if (anrm > 0. && anrm < smlnum) {
		iscl = 1;
		NUMlapack_dlascl ("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda, &ierr);
	} else if (anrm > bignum) {
		iscl = 1;
		NUMlapack_dlascl ("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda, &ierr);
	}

	if (*m >= *n) {

		/* A has at least as many rows as columns. If A has sufficiently more
		   rows than columns, first reduce using the QR decomposition (if
		   sufficient workspace available) */

		if (*m >= mnthr) {

			if (wntun) {

				/* Path 1 (M much larger than N, JOBU='N') No left singular
				   vectors to be computed */

				itau = 1;
				iwork = itau + *n;

				/* Compute A=Q*R (Workspace: need 2*N, prefer N+N*NB) */

				i__2 = *lwork - iwork + 1;
				NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

				/* Zero out below R */

				i__2 = *n - 1;
				i__3 = *n - 1;
				NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &a_ref (2, 1), lda);
				ie = 1;
				itauq = ie + *n;
				itaup = itauq + *n;
				iwork = itaup + *n;

				/* Bidiagonalize R in A (Workspace: need 4*N, prefer
				   3*N+2*N*NB) */

				i__2 = *lwork - iwork + 1;
				NUMlapack_dgebrd (n, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
				                  &work[iwork], &i__2, &ierr);
				ncvt = 0;
				if (wntvo || wntvas) {

					/* If right singular vectors desired, generate P'.
					   (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dorgbr ("P", n, n, n, &a[a_offset], lda, &work[itaup], &work[iwork], &i__2,
					                  &ierr);
					ncvt = *n;
				}
				iwork = ie + *n;

				/* Perform bidiagonal QR iteration, computing right singular
				   vectors of A in A if desired (Workspace: need BDSPAC) */

				NUMlapack_dbdsqr ("U", n, &ncvt, &c__0, &c__0, &s[1], &work[ie], &a[a_offset], lda, dum, &c__1,
				                  dum, &c__1, &work[iwork], info);

				/* If right singular vectors desired in VT, copy them there */

				if (wntvas) {
					NUMlapack_dlacpy ("F", n, n, &a[a_offset], lda, &vt[vt_offset], ldvt);
				}

			} else if (wntuo && wntvn) {

				/* Path 2 (M much larger than N, JOBU='O', JOBVT='N') N left
				   singular vectors to be overwritten on A and no right
				   singular vectors to be computed

				   Computing MAX */
				i__2 = *n << 2;
				if (*lwork >= *n * *n + MAX (i__2, bdspac)) {

					/* Sufficient workspace for a fast algorithm */

					ir = 1;
					/* Computing MAX */
					i__2 = wrkbl, i__3 = *lda * *n + *n;
					if (*lwork >= MAX (i__2, i__3) + *lda * *n) {

						/* WORK(IU) is LDA by N, WORK(IR) is LDA by N */

						ldwrku = *lda;
						ldwrkr = *lda;
					} else {	/* if(complicated condition) */
						/* Computing MAX */
						i__2 = wrkbl, i__3 = *lda * *n + *n;
						if (*lwork >= MAX (i__2, i__3) + *n * *n) {

							/* WORK(IU) is LDA by N, WORK(IR) is N by N */

							ldwrku = *lda;
							ldwrkr = *n;
						} else {

							/* WORK(IU) is LDWRKU by N, WORK(IR) is N by N */

							ldwrku = (*lwork - *n * *n - *n) / *n;
							ldwrkr = *n;
						}
					}
					itau = ir + ldwrkr * *n;
					iwork = itau + *n;

					/* Compute A=Q*R (Workspace: need N*N+2*N, prefer
					   N*N+N+N*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

					/* Copy R to WORK(IR) and zero out below it */

					NUMlapack_dlacpy ("U", n, n, &a[a_offset], lda, &work[ir], &ldwrkr);
					i__2 = *n - 1;
					i__3 = *n - 1;
					NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &work[ir + 1], &ldwrkr);

					/* Generate Q in A (Workspace: need N*N+2*N, prefer
					   N*N+N+N*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dorgqr (m, n, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
					ie = itau;
					itauq = ie + *n;
					itaup = itauq + *n;
					iwork = itaup + *n;

					/* Bidiagonalize R in WORK(IR) (Workspace: need N*N+4*N,
					   prefer N*N+3*N+2*N*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dgebrd (n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &work[itauq], &work[itaup],
					                  &work[iwork], &i__2, &ierr);

					/* Generate left vectors bidiagonalizing R (Workspace:
					   need N*N+4*N, prefer N*N+3*N+N*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dorgbr ("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq], &work[iwork], &i__2,
					                  &ierr);
					iwork = ie + *n;

					/* Perform bidiagonal QR iteration, computing left
					   singular vectors of R in WORK(IR) (Workspace: need
					   N*N+BDSPAC) */

					NUMlapack_dbdsqr ("U", n, &c__0, n, &c__0, &s[1], &work[ie], dum, &c__1, &work[ir], &ldwrkr,
					                  dum, &c__1, &work[iwork], info);
					iu = ie + *n;

					/* Multiply Q in A by left singular vectors of R in
					   WORK(IR), storing result in WORK(IU) and copying to A
					   (Workspace: need N*N+2*N, prefer N*N+M*N+N) */

					i__2 = *m;
					i__3 = ldwrku;
					for (i__ = 1; i__3 < 0 ? i__ >= i__2 : i__ <= i__2; i__ += i__3) {
						/* Computing MIN */
						i__4 = *m - i__ + 1;
						chunk = MIN (i__4, ldwrku);
						NUMblas_dgemm ("N", "N", &chunk, n, n, &c_b438, &a_ref (i__, 1), lda, &work[ir], &ldwrkr,
						               &c_b416, &work[iu], &ldwrku);
						NUMlapack_dlacpy ("F", &chunk, n, &work[iu], &ldwrku, &a_ref (i__, 1), lda);
						/* L10: */
					}

				} else {

					/* Insufficient workspace for a fast algorithm */

					ie = 1;
					itauq = ie + *n;
					itaup = itauq + *n;
					iwork = itaup + *n;

					/* Bidiagonalize A (Workspace: need 3*N+M, prefer
					   3*N+(M+N)*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dgebrd (m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
					                  &work[iwork], &i__3, &ierr);

					/* Generate left vectors bidiagonalizing A (Workspace:
					   need 4*N, prefer 3*N+N*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dorgbr ("Q", m, n, n, &a[a_offset], lda, &work[itauq], &work[iwork], &i__3,
					                  &ierr);
					iwork = ie + *n;

					/* Perform bidiagonal QR iteration, computing left
					   singular vectors of A in A (Workspace: need BDSPAC) */

					NUMlapack_dbdsqr ("U", n, &c__0, m, &c__0, &s[1], &work[ie], dum, &c__1, &a[a_offset], lda,
					                  dum, &c__1, &work[iwork], info);

				}

			} else if (wntuo && wntvas) {

				/* Path 3 (M much larger than N, JOBU='O', JOBVT='S' or 'A')
				   N left singular vectors to be overwritten on A and N right
				   singular vectors to be computed in VT

				   Computing MAX */
				i__3 = *n << 2;
				if (*lwork >= *n * *n + MAX (i__3, bdspac)) {

					/* Sufficient workspace for a fast algorithm */

					ir = 1;
					/* Computing MAX */
					i__3 = wrkbl, i__2 = *lda * *n + *n;
					if (*lwork >= MAX (i__3, i__2) + *lda * *n) {

						/* WORK(IU) is LDA by N and WORK(IR) is LDA by N */

						ldwrku = *lda;
						ldwrkr = *lda;
					} else {	/* if(complicated condition) */
						/* Computing MAX */
						i__3 = wrkbl, i__2 = *lda * *n + *n;
						if (*lwork >= MAX (i__3, i__2) + *n * *n) {

							/* WORK(IU) is LDA by N and WORK(IR) is N by N */

							ldwrku = *lda;
							ldwrkr = *n;
						} else {

							/* WORK(IU) is LDWRKU by N and WORK(IR) is N by N
							 */

							ldwrku = (*lwork - *n * *n - *n) / *n;
							ldwrkr = *n;
						}
					}
					itau = ir + ldwrkr * *n;
					iwork = itau + *n;

					/* Compute A=Q*R (Workspace: need N*N+2*N, prefer
					   N*N+N+N*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__3, &ierr);

					/* Copy R to VT, zeroing out below it */

					NUMlapack_dlacpy ("U", n, n, &a[a_offset], lda, &vt[vt_offset], ldvt);
					i__3 = *n - 1;
					i__2 = *n - 1;
					NUMlapack_dlaset ("L", &i__3, &i__2, &c_b416, &c_b416, &vt_ref (2, 1), ldvt);

					/* Generate Q in A (Workspace: need N*N+2*N, prefer
					   N*N+N+N*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dorgqr (m, n, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__3, &ierr);
					ie = itau;
					itauq = ie + *n;
					itaup = itauq + *n;
					iwork = itaup + *n;

					/* Bidiagonalize R in VT, copying result to WORK(IR)
					   (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dgebrd (n, n, &vt[vt_offset], ldvt, &s[1], &work[ie], &work[itauq], &work[itaup],
					                  &work[iwork], &i__3, &ierr);
					NUMlapack_dlacpy ("L", n, n, &vt[vt_offset], ldvt, &work[ir], &ldwrkr);

					/* Generate left vectors bidiagonalizing R in WORK(IR)
					   (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dorgbr ("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq], &work[iwork], &i__3,
					                  &ierr);

					/* Generate right vectors bidiagonalizing R in VT
					   (Workspace: need N*N+4*N-1, prefer N*N+3*N+(N-1)*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dorgbr ("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], &work[iwork], &i__3,
					                  &ierr);
					iwork = ie + *n;

					/* Perform bidiagonal QR iteration, computing left
					   singular vectors of R in WORK(IR) and computing right
					   singular vectors of R in VT (Workspace: need
					   N*N+BDSPAC) */

					NUMlapack_dbdsqr ("U", n, n, n, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt, &work[ir],
					                  &ldwrkr, dum, &c__1, &work[iwork], info);
					iu = ie + *n;

					/* Multiply Q in A by left singular vectors of R in
					   WORK(IR), storing result in WORK(IU) and copying to A
					   (Workspace: need N*N+2*N, prefer N*N+M*N+N) */

					i__3 = *m;
					i__2 = ldwrku;
					for (i__ = 1; i__2 < 0 ? i__ >= i__3 : i__ <= i__3; i__ += i__2) {
						/* Computing MIN */
						i__4 = *m - i__ + 1;
						chunk = MIN (i__4, ldwrku);
						NUMblas_dgemm ("N", "N", &chunk, n, n, &c_b438, &a_ref (i__, 1), lda, &work[ir], &ldwrkr,
						               &c_b416, &work[iu], &ldwrku);
						NUMlapack_dlacpy ("F", &chunk, n, &work[iu], &ldwrku, &a_ref (i__, 1), lda);
						/* L20: */
					}

				} else {

					/* Insufficient workspace for a fast algorithm */

					itau = 1;
					iwork = itau + *n;

					/* Compute A=Q*R (Workspace: need 2*N, prefer N+N*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

					/* Copy R to VT, zeroing out below it */

					NUMlapack_dlacpy ("U", n, n, &a[a_offset], lda, &vt[vt_offset], ldvt);
					i__2 = *n - 1;
					i__3 = *n - 1;
					NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &vt_ref (2, 1), ldvt);

					/* Generate Q in A (Workspace: need 2*N, prefer N+N*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dorgqr (m, n, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
					ie = itau;
					itauq = ie + *n;
					itaup = itauq + *n;
					iwork = itaup + *n;

					/* Bidiagonalize R in VT (Workspace: need 4*N, prefer
					   3*N+2*N*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dgebrd (n, n, &vt[vt_offset], ldvt, &s[1], &work[ie], &work[itauq], &work[itaup],
					                  &work[iwork], &i__2, &ierr);

					/* Multiply Q in A by left vectors bidiagonalizing R
					   (Workspace: need 3*N+M, prefer 3*N+M*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dormbr ("Q", "R", "N", m, n, n, &vt[vt_offset], ldvt, &work[itauq], &a[a_offset],
					                  lda, &work[iwork], &i__2, &ierr);

					/* Generate right vectors bidiagonalizing R in VT
					   (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dorgbr ("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], &work[iwork], &i__2,
					                  &ierr);
					iwork = ie + *n;

					/* Perform bidiagonal QR iteration, computing left
					   singular vectors of A in A and computing right
					   singular vectors of A in VT (Workspace: need BDSPAC) */

					NUMlapack_dbdsqr ("U", n, n, m, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt, &a[a_offset],
					                  lda, dum, &c__1, &work[iwork], info);

				}

			} else if (wntus) {

				if (wntvn) {

					/* Path 4 (M much larger than N, JOBU='S', JOBVT='N') N
					   left singular vectors to be computed in U and no right
					   singular vectors to be computed

					   Computing MAX */
					i__2 = *n << 2;
					if (*lwork >= *n * *n + MAX (i__2, bdspac)) {

						/* Sufficient workspace for a fast algorithm */

						ir = 1;
						if (*lwork >= wrkbl + *lda * *n) {

							/* WORK(IR) is LDA by N */

							ldwrkr = *lda;
						} else {

							/* WORK(IR) is N by N */

							ldwrkr = *n;
						}
						itau = ir + ldwrkr * *n;
						iwork = itau + *n;

						/* Compute A=Q*R (Workspace: need N*N+2*N, prefer
						   N*N+N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

						/* Copy R to WORK(IR), zeroing out below it */

						NUMlapack_dlacpy ("U", n, n, &a[a_offset], lda, &work[ir], &ldwrkr);
						i__2 = *n - 1;
						i__3 = *n - 1;
						NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &work[ir + 1], &ldwrkr);

						/* Generate Q in A (Workspace: need N*N+2*N, prefer
						   N*N+N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgqr (m, n, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						ie = itau;
						itauq = ie + *n;
						itaup = itauq + *n;
						iwork = itaup + *n;

						/* Bidiagonalize R in WORK(IR) (Workspace: need
						   N*N+4*N, prefer N*N+3*N+2*N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);

						/* Generate left vectors bidiagonalizing R in
						   WORK(IR) (Workspace: need N*N+4*N, prefer
						   N*N+3*N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *n;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of R in WORK(IR) (Workspace: need
						   N*N+BDSPAC) */

						NUMlapack_dbdsqr ("U", n, &c__0, n, &c__0, &s[1], &work[ie], dum, &c__1, &work[ir],
						                  &ldwrkr, dum, &c__1, &work[iwork], info);

						/* Multiply Q in A by left singular vectors of R in
						   WORK(IR), storing result in U (Workspace: need
						   N*N) */

						NUMblas_dgemm ("N", "N", m, n, n, &c_b438, &a[a_offset], lda, &work[ir], &ldwrkr, &c_b416,
						               &u[u_offset], ldu);

					} else {

						/* Insufficient workspace for a fast algorithm */

						itau = 1;
						iwork = itau + *n;

						/* Compute A=Q*R, copying result to U (Workspace:
						   need 2*N, prefer N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);

						/* Generate Q in U (Workspace: need 2*N, prefer
						   N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgqr (m, n, n, &u[u_offset], ldu, &work[itau], &work[iwork], &i__2, &ierr);
						ie = itau;
						itauq = ie + *n;
						itaup = itauq + *n;
						iwork = itaup + *n;

						/* Zero out below R in A */

						i__2 = *n - 1;
						i__3 = *n - 1;
						NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &a_ref (2, 1), lda);

						/* Bidiagonalize R in A (Workspace: need 4*N, prefer
						   3*N+2*N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (n, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
						                  &work[iwork], &i__2, &ierr);

						/* Multiply Q in U by left vectors bidiagonalizing R
						   (Workspace: need 3*N+M, prefer 3*N+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dormbr ("Q", "R", "N", m, n, n, &a[a_offset], lda, &work[itauq], &u[u_offset],
						                  ldu, &work[iwork], &i__2, &ierr);
						iwork = ie + *n;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of A in U (Workspace: need
						   BDSPAC) */

						NUMlapack_dbdsqr ("U", n, &c__0, m, &c__0, &s[1], &work[ie], dum, &c__1, &u[u_offset],
						                  ldu, dum, &c__1, &work[iwork], info);

					}

				} else if (wntvo) {

					/* Path 5 (M much larger than N, JOBU='S', JOBVT='O') N
					   left singular vectors to be computed in U and N right
					   singular vectors to be overwritten on A

					   Computing MAX */
					i__2 = *n << 2;
					if (*lwork >= (*n << 1) * *n + MAX (i__2, bdspac)) {

						/* Sufficient workspace for a fast algorithm */

						iu = 1;
						if (*lwork >= wrkbl + (*lda << 1) * *n) {

							/* WORK(IU) is LDA by N and WORK(IR) is LDA by N */

							ldwrku = *lda;
							ir = iu + ldwrku * *n;
							ldwrkr = *lda;
						} else if (*lwork >= wrkbl + (*lda + *n) * *n) {

							/* WORK(IU) is LDA by N and WORK(IR) is N by N */

							ldwrku = *lda;
							ir = iu + ldwrku * *n;
							ldwrkr = *n;
						} else {

							/* WORK(IU) is N by N and WORK(IR) is N by N */

							ldwrku = *n;
							ir = iu + ldwrku * *n;
							ldwrkr = *n;
						}
						itau = ir + ldwrkr * *n;
						iwork = itau + *n;

						/* Compute A=Q*R (Workspace: need 2*N*N+2*N, prefer
						   2*N*N+N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

						/* Copy R to WORK(IU), zeroing out below it */

						NUMlapack_dlacpy ("U", n, n, &a[a_offset], lda, &work[iu], &ldwrku);
						i__2 = *n - 1;
						i__3 = *n - 1;
						NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &work[iu + 1], &ldwrku);

						/* Generate Q in A (Workspace: need 2*N*N+2*N, prefer
						   2*N*N+N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgqr (m, n, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						ie = itau;
						itauq = ie + *n;
						itaup = itauq + *n;
						iwork = itaup + *n;

						/* Bidiagonalize R in WORK(IU), copying result to
						   WORK(IR) (Workspace: need 2*N*N+4*N, prefer
						   2*N*N+3*N+2*N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (n, n, &work[iu], &ldwrku, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("U", n, n, &work[iu], &ldwrku, &work[ir], &ldwrkr);

						/* Generate left bidiagonalizing vectors in WORK(IU)
						   (Workspace: need 2*N*N+4*N, prefer 2*N*N+3*N+N*NB)
						 */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", n, n, n, &work[iu], &ldwrku, &work[itauq], &work[iwork], &i__2,
						                  &ierr);

						/* Generate right bidiagonalizing vectors in WORK(IR)
						   (Workspace: need 2*N*N+4*N-1, prefer
						   2*N*N+3*N+(N-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", n, n, n, &work[ir], &ldwrkr, &work[itaup], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *n;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of R in WORK(IU) and computing
						   right singular vectors of R in WORK(IR)
						   (Workspace: need 2*N*N+BDSPAC) */

						NUMlapack_dbdsqr ("U", n, n, n, &c__0, &s[1], &work[ie], &work[ir], &ldwrkr, &work[iu],
						                  &ldwrku, dum, &c__1, &work[iwork], info);

						/* Multiply Q in A by left singular vectors of R in
						   WORK(IU), storing result in U (Workspace: need
						   N*N) */

						NUMblas_dgemm ("N", "N", m, n, n, &c_b438, &a[a_offset], lda, &work[iu], &ldwrku, &c_b416,
						               &u[u_offset], ldu);

						/* Copy right singular vectors of R to A (Workspace:
						   need N*N) */

						NUMlapack_dlacpy ("F", n, n, &work[ir], &ldwrkr, &a[a_offset], lda);

					} else {

						/* Insufficient workspace for a fast algorithm */

						itau = 1;
						iwork = itau + *n;

						/* Compute A=Q*R, copying result to U (Workspace:
						   need 2*N, prefer N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);

						/* Generate Q in U (Workspace: need 2*N, prefer
						   N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgqr (m, n, n, &u[u_offset], ldu, &work[itau], &work[iwork], &i__2, &ierr);
						ie = itau;
						itauq = ie + *n;
						itaup = itauq + *n;
						iwork = itaup + *n;

						/* Zero out below R in A */

						i__2 = *n - 1;
						i__3 = *n - 1;
						NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &a_ref (2, 1), lda);

						/* Bidiagonalize R in A (Workspace: need 4*N, prefer
						   3*N+2*N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (n, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
						                  &work[iwork], &i__2, &ierr);

						/* Multiply Q in U by left vectors bidiagonalizing R
						   (Workspace: need 3*N+M, prefer 3*N+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dormbr ("Q", "R", "N", m, n, n, &a[a_offset], lda, &work[itauq], &u[u_offset],
						                  ldu, &work[iwork], &i__2, &ierr);

						/* Generate right vectors bidiagonalizing R in A
						   (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", n, n, n, &a[a_offset], lda, &work[itaup], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *n;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of A in U and computing right
						   singular vectors of A in A (Workspace: need
						   BDSPAC) */

						NUMlapack_dbdsqr ("U", n, n, m, &c__0, &s[1], &work[ie], &a[a_offset], lda,
						                  &u[u_offset], ldu, dum, &c__1, &work[iwork], info);

					}

				} else if (wntvas) {

					/* Path 6 (M much larger than N, JOBU='S', JOBVT='S' or
					   'A') N left singular vectors to be computed in U and N
					   right singular vectors to be computed in VT

					   Computing MAX */
					i__2 = *n << 2;
					if (*lwork >= *n * *n + MAX (i__2, bdspac)) {

						/* Sufficient workspace for a fast algorithm */

						iu = 1;
						if (*lwork >= wrkbl + *lda * *n) {

							/* WORK(IU) is LDA by N */

							ldwrku = *lda;
						} else {

							/* WORK(IU) is N by N */

							ldwrku = *n;
						}
						itau = iu + ldwrku * *n;
						iwork = itau + *n;

						/* Compute A=Q*R (Workspace: need N*N+2*N, prefer
						   N*N+N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

						/* Copy R to WORK(IU), zeroing out below it */

						NUMlapack_dlacpy ("U", n, n, &a[a_offset], lda, &work[iu], &ldwrku);
						i__2 = *n - 1;
						i__3 = *n - 1;
						NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &work[iu + 1], &ldwrku);

						/* Generate Q in A (Workspace: need N*N+2*N, prefer
						   N*N+N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgqr (m, n, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						ie = itau;
						itauq = ie + *n;
						itaup = itauq + *n;
						iwork = itaup + *n;

						/* Bidiagonalize R in WORK(IU), copying result to VT
						   (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (n, n, &work[iu], &ldwrku, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("U", n, n, &work[iu], &ldwrku, &vt[vt_offset], ldvt);

						/* Generate left bidiagonalizing vectors in WORK(IU)
						   (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", n, n, n, &work[iu], &ldwrku, &work[itauq], &work[iwork], &i__2,
						                  &ierr);

						/* Generate right bidiagonalizing vectors in VT
						   (Workspace: need N*N+4*N-1, prefer
						   N*N+3*N+(N-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *n;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of R in WORK(IU) and computing
						   right singular vectors of R in VT (Workspace: need
						   N*N+BDSPAC) */

						NUMlapack_dbdsqr ("U", n, n, n, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt,
						                  &work[iu], &ldwrku, dum, &c__1, &work[iwork], info);

						/* Multiply Q in A by left singular vectors of R in
						   WORK(IU), storing result in U (Workspace: need
						   N*N) */

						NUMblas_dgemm ("N", "N", m, n, n, &c_b438, &a[a_offset], lda, &work[iu], &ldwrku, &c_b416,
						               &u[u_offset], ldu);

					} else {

						/* Insufficient workspace for a fast algorithm */

						itau = 1;
						iwork = itau + *n;

						/* Compute A=Q*R, copying result to U (Workspace:
						   need 2*N, prefer N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);

						/* Generate Q in U (Workspace: need 2*N, prefer
						   N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgqr (m, n, n, &u[u_offset], ldu, &work[itau], &work[iwork], &i__2, &ierr);

						/* Copy R to VT, zeroing out below it */

						NUMlapack_dlacpy ("U", n, n, &a[a_offset], lda, &vt[vt_offset], ldvt);
						i__2 = *n - 1;
						i__3 = *n - 1;
						NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &vt_ref (2, 1), ldvt);
						ie = itau;
						itauq = ie + *n;
						itaup = itauq + *n;
						iwork = itaup + *n;

						/* Bidiagonalize R in VT (Workspace: need 4*N, prefer
						   3*N+2*N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (n, n, &vt[vt_offset], ldvt, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);

						/* Multiply Q in U by left bidiagonalizing vectors in
						   VT (Workspace: need 3*N+M, prefer 3*N+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dormbr ("Q", "R", "N", m, n, n, &vt[vt_offset], ldvt, &work[itauq],
						                  &u[u_offset], ldu, &work[iwork], &i__2, &ierr);

						/* Generate right bidiagonalizing vectors in VT
						   (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *n;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of A in U and computing right
						   singular vectors of A in VT (Workspace: need
						   BDSPAC) */

						NUMlapack_dbdsqr ("U", n, n, m, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt,
						                  &u[u_offset], ldu, dum, &c__1, &work[iwork], info);

					}

				}

			} else if (wntua) {

				if (wntvn) {

					/* Path 7 (M much larger than N, JOBU='A', JOBVT='N') M
					   left singular vectors to be computed in U and no right
					   singular vectors to be computed

					   Computing MAX */
					i__2 = *n + *m, i__3 = *n << 2, i__2 = MAX (i__2, i__3);
					if (*lwork >= *n * *n + MAX (i__2, bdspac)) {

						/* Sufficient workspace for a fast algorithm */

						ir = 1;
						if (*lwork >= wrkbl + *lda * *n) {

							/* WORK(IR) is LDA by N */

							ldwrkr = *lda;
						} else {

							/* WORK(IR) is N by N */

							ldwrkr = *n;
						}
						itau = ir + ldwrkr * *n;
						iwork = itau + *n;

						/* Compute A=Q*R, copying result to U (Workspace:
						   need N*N+2*N, prefer N*N+N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);

						/* Copy R to WORK(IR), zeroing out below it */

						NUMlapack_dlacpy ("U", n, n, &a[a_offset], lda, &work[ir], &ldwrkr);
						i__2 = *n - 1;
						i__3 = *n - 1;
						NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &work[ir + 1], &ldwrkr);

						/* Generate Q in U (Workspace: need N*N+N+M, prefer
						   N*N+N+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgqr (m, m, n, &u[u_offset], ldu, &work[itau], &work[iwork], &i__2, &ierr);
						ie = itau;
						itauq = ie + *n;
						itaup = itauq + *n;
						iwork = itaup + *n;

						/* Bidiagonalize R in WORK(IR) (Workspace: need
						   N*N+4*N, prefer N*N+3*N+2*N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);

						/* Generate left bidiagonalizing vectors in WORK(IR)
						   (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *n;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of R in WORK(IR) (Workspace: need
						   N*N+BDSPAC) */

						NUMlapack_dbdsqr ("U", n, &c__0, n, &c__0, &s[1], &work[ie], dum, &c__1, &work[ir],
						                  &ldwrkr, dum, &c__1, &work[iwork], info);

						/* Multiply Q in U by left singular vectors of R in
						   WORK(IR), storing result in A (Workspace: need
						   N*N) */

						NUMblas_dgemm ("N", "N", m, n, n, &c_b438, &u[u_offset], ldu, &work[ir], &ldwrkr, &c_b416,
						               &a[a_offset], lda);

						/* Copy left singular vectors of A from A to U */

						NUMlapack_dlacpy ("F", m, n, &a[a_offset], lda, &u[u_offset], ldu);

					} else {

						/* Insufficient workspace for a fast algorithm */

						itau = 1;
						iwork = itau + *n;

						/* Compute A=Q*R, copying result to U (Workspace:
						   need 2*N, prefer N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);

						/* Generate Q in U (Workspace: need N+M, prefer
						   N+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgqr (m, m, n, &u[u_offset], ldu, &work[itau], &work[iwork], &i__2, &ierr);
						ie = itau;
						itauq = ie + *n;
						itaup = itauq + *n;
						iwork = itaup + *n;

						/* Zero out below R in A */

						i__2 = *n - 1;
						i__3 = *n - 1;
						NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &a_ref (2, 1), lda);

						/* Bidiagonalize R in A (Workspace: need 4*N, prefer
						   3*N+2*N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (n, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
						                  &work[iwork], &i__2, &ierr);

						/* Multiply Q in U by left bidiagonalizing vectors in
						   A (Workspace: need 3*N+M, prefer 3*N+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dormbr ("Q", "R", "N", m, n, n, &a[a_offset], lda, &work[itauq], &u[u_offset],
						                  ldu, &work[iwork], &i__2, &ierr);
						iwork = ie + *n;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of A in U (Workspace: need
						   BDSPAC) */

						NUMlapack_dbdsqr ("U", n, &c__0, m, &c__0, &s[1], &work[ie], dum, &c__1, &u[u_offset],
						                  ldu, dum, &c__1, &work[iwork], info);

					}

				} else if (wntvo) {

					/* Path 8 (M much larger than N, JOBU='A', JOBVT='O') M
					   left singular vectors to be computed in U and N right
					   singular vectors to be overwritten on A

					   Computing MAX */
					i__2 = *n + *m, i__3 = *n << 2, i__2 = MAX (i__2, i__3);
					if (*lwork >= (*n << 1) * *n + MAX (i__2, bdspac)) {

						/* Sufficient workspace for a fast algorithm */

						iu = 1;
						if (*lwork >= wrkbl + (*lda << 1) * *n) {

							/* WORK(IU) is LDA by N and WORK(IR) is LDA by N */

							ldwrku = *lda;
							ir = iu + ldwrku * *n;
							ldwrkr = *lda;
						} else if (*lwork >= wrkbl + (*lda + *n) * *n) {

							/* WORK(IU) is LDA by N and WORK(IR) is N by N */

							ldwrku = *lda;
							ir = iu + ldwrku * *n;
							ldwrkr = *n;
						} else {

							/* WORK(IU) is N by N and WORK(IR) is N by N */

							ldwrku = *n;
							ir = iu + ldwrku * *n;
							ldwrkr = *n;
						}
						itau = ir + ldwrkr * *n;
						iwork = itau + *n;

						/* Compute A=Q*R, copying result to U (Workspace:
						   need 2*N*N+2*N, prefer 2*N*N+N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);

						/* Generate Q in U (Workspace: need 2*N*N+N+M, prefer
						   2*N*N+N+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgqr (m, m, n, &u[u_offset], ldu, &work[itau], &work[iwork], &i__2, &ierr);

						/* Copy R to WORK(IU), zeroing out below it */

						NUMlapack_dlacpy ("U", n, n, &a[a_offset], lda, &work[iu], &ldwrku);
						i__2 = *n - 1;
						i__3 = *n - 1;
						NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &work[iu + 1], &ldwrku);
						ie = itau;
						itauq = ie + *n;
						itaup = itauq + *n;
						iwork = itaup + *n;

						/* Bidiagonalize R in WORK(IU), copying result to
						   WORK(IR) (Workspace: need 2*N*N+4*N, prefer
						   2*N*N+3*N+2*N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (n, n, &work[iu], &ldwrku, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("U", n, n, &work[iu], &ldwrku, &work[ir], &ldwrkr);

						/* Generate left bidiagonalizing vectors in WORK(IU)
						   (Workspace: need 2*N*N+4*N, prefer 2*N*N+3*N+N*NB)
						 */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", n, n, n, &work[iu], &ldwrku, &work[itauq], &work[iwork], &i__2,
						                  &ierr);

						/* Generate right bidiagonalizing vectors in WORK(IR)
						   (Workspace: need 2*N*N+4*N-1, prefer
						   2*N*N+3*N+(N-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", n, n, n, &work[ir], &ldwrkr, &work[itaup], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *n;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of R in WORK(IU) and computing
						   right singular vectors of R in WORK(IR)
						   (Workspace: need 2*N*N+BDSPAC) */

						NUMlapack_dbdsqr ("U", n, n, n, &c__0, &s[1], &work[ie], &work[ir], &ldwrkr, &work[iu],
						                  &ldwrku, dum, &c__1, &work[iwork], info);

						/* Multiply Q in U by left singular vectors of R in
						   WORK(IU), storing result in A (Workspace: need
						   N*N) */

						NUMblas_dgemm ("N", "N", m, n, n, &c_b438, &u[u_offset], ldu, &work[iu], &ldwrku, &c_b416,
						               &a[a_offset], lda);

						/* Copy left singular vectors of A from A to U */

						NUMlapack_dlacpy ("F", m, n, &a[a_offset], lda, &u[u_offset], ldu);

						/* Copy right singular vectors of R from WORK(IR) to
						   A */

						NUMlapack_dlacpy ("F", n, n, &work[ir], &ldwrkr, &a[a_offset], lda);

					} else {

						/* Insufficient workspace for a fast algorithm */

						itau = 1;
						iwork = itau + *n;

						/* Compute A=Q*R, copying result to U (Workspace:
						   need 2*N, prefer N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);

						/* Generate Q in U (Workspace: need N+M, prefer
						   N+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgqr (m, m, n, &u[u_offset], ldu, &work[itau], &work[iwork], &i__2, &ierr);
						ie = itau;
						itauq = ie + *n;
						itaup = itauq + *n;
						iwork = itaup + *n;

						/* Zero out below R in A */

						i__2 = *n - 1;
						i__3 = *n - 1;
						NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &a_ref (2, 1), lda);

						/* Bidiagonalize R in A (Workspace: need 4*N, prefer
						   3*N+2*N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (n, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
						                  &work[iwork], &i__2, &ierr);

						/* Multiply Q in U by left bidiagonalizing vectors in
						   A (Workspace: need 3*N+M, prefer 3*N+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dormbr ("Q", "R", "N", m, n, n, &a[a_offset], lda, &work[itauq], &u[u_offset],
						                  ldu, &work[iwork], &i__2, &ierr);

						/* Generate right bidiagonalizing vectors in A
						   (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", n, n, n, &a[a_offset], lda, &work[itaup], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *n;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of A in U and computing right
						   singular vectors of A in A (Workspace: need
						   BDSPAC) */

						NUMlapack_dbdsqr ("U", n, n, m, &c__0, &s[1], &work[ie], &a[a_offset], lda,
						                  &u[u_offset], ldu, dum, &c__1, &work[iwork], info);

					}

				} else if (wntvas) {

					/* Path 9 (M much larger than N, JOBU='A', JOBVT='S' or
					   'A') M left singular vectors to be computed in U and N
					   right singular vectors to be computed in VT

					   Computing MAX */
					i__2 = *n + *m, i__3 = *n << 2, i__2 = MAX (i__2, i__3);
					if (*lwork >= *n * *n + MAX (i__2, bdspac)) {

						/* Sufficient workspace for a fast algorithm */

						iu = 1;
						if (*lwork >= wrkbl + *lda * *n) {

							/* WORK(IU) is LDA by N */

							ldwrku = *lda;
						} else {

							/* WORK(IU) is N by N */

							ldwrku = *n;
						}
						itau = iu + ldwrku * *n;
						iwork = itau + *n;

						/* Compute A=Q*R, copying result to U (Workspace:
						   need N*N+2*N, prefer N*N+N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);

						/* Generate Q in U (Workspace: need N*N+N+M, prefer
						   N*N+N+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgqr (m, m, n, &u[u_offset], ldu, &work[itau], &work[iwork], &i__2, &ierr);

						/* Copy R to WORK(IU), zeroing out below it */

						NUMlapack_dlacpy ("U", n, n, &a[a_offset], lda, &work[iu], &ldwrku);
						i__2 = *n - 1;
						i__3 = *n - 1;
						NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &work[iu + 1], &ldwrku);
						ie = itau;
						itauq = ie + *n;
						itaup = itauq + *n;
						iwork = itaup + *n;

						/* Bidiagonalize R in WORK(IU), copying result to VT
						   (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (n, n, &work[iu], &ldwrku, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("U", n, n, &work[iu], &ldwrku, &vt[vt_offset], ldvt);

						/* Generate left bidiagonalizing vectors in WORK(IU)
						   (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", n, n, n, &work[iu], &ldwrku, &work[itauq], &work[iwork], &i__2,
						                  &ierr);

						/* Generate right bidiagonalizing vectors in VT
						   (Workspace: need N*N+4*N-1, prefer
						   N*N+3*N+(N-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *n;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of R in WORK(IU) and computing
						   right singular vectors of R in VT (Workspace: need
						   N*N+BDSPAC) */

						NUMlapack_dbdsqr ("U", n, n, n, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt,
						                  &work[iu], &ldwrku, dum, &c__1, &work[iwork], info);

						/* Multiply Q in U by left singular vectors of R in
						   WORK(IU), storing result in A (Workspace: need
						   N*N) */

						NUMblas_dgemm ("N", "N", m, n, n, &c_b438, &u[u_offset], ldu, &work[iu], &ldwrku, &c_b416,
						               &a[a_offset], lda);

						/* Copy left singular vectors of A from A to U */

						NUMlapack_dlacpy ("F", m, n, &a[a_offset], lda, &u[u_offset], ldu);

					} else {

						/* Insufficient workspace for a fast algorithm */

						itau = 1;
						iwork = itau + *n;

						/* Compute A=Q*R, copying result to U (Workspace:
						   need 2*N, prefer N+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgeqrf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);

						/* Generate Q in U (Workspace: need N+M, prefer
						   N+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgqr (m, m, n, &u[u_offset], ldu, &work[itau], &work[iwork], &i__2, &ierr);

						/* Copy R from A to VT, zeroing out below it */

						NUMlapack_dlacpy ("U", n, n, &a[a_offset], lda, &vt[vt_offset], ldvt);
						i__2 = *n - 1;
						i__3 = *n - 1;
						NUMlapack_dlaset ("L", &i__2, &i__3, &c_b416, &c_b416, &vt_ref (2, 1), ldvt);
						ie = itau;
						itauq = ie + *n;
						itaup = itauq + *n;
						iwork = itaup + *n;

						/* Bidiagonalize R in VT (Workspace: need 4*N, prefer
						   3*N+2*N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (n, n, &vt[vt_offset], ldvt, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);

						/* Multiply Q in U by left bidiagonalizing vectors in
						   VT (Workspace: need 3*N+M, prefer 3*N+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dormbr ("Q", "R", "N", m, n, n, &vt[vt_offset], ldvt, &work[itauq],
						                  &u[u_offset], ldu, &work[iwork], &i__2, &ierr);

						/* Generate right bidiagonalizing vectors in VT
						   (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *n;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of A in U and computing right
						   singular vectors of A in VT (Workspace: need
						   BDSPAC) */

						NUMlapack_dbdsqr ("U", n, n, m, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt,
						                  &u[u_offset], ldu, dum, &c__1, &work[iwork], info);

					}

				}

			}

		} else {

			/* M .LT. MNTHR

			   Path 10 (M at least N, but not much larger) Reduce to
			   bidiagonal form without QR decomposition */

			ie = 1;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

			/* Bidiagonalize A (Workspace: need 3*N+M, prefer 3*N+(M+N)*NB) */

			i__2 = *lwork - iwork + 1;
			NUMlapack_dgebrd (m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
			                  &work[iwork], &i__2, &ierr);
			if (wntuas) {

				/* If left singular vectors desired in U, copy result to U
				   and generate left bidiagonalizing vectors in U (Workspace:
				   need 3*N+NCU, prefer 3*N+NCU*NB) */

				NUMlapack_dlacpy ("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);
				if (wntus) {
					ncu = *n;
				}
				if (wntua) {
					ncu = *m;
				}
				i__2 = *lwork - iwork + 1;
				NUMlapack_dorgbr ("Q", m, &ncu, n, &u[u_offset], ldu, &work[itauq], &work[iwork], &i__2, &ierr);
			}
			if (wntvas) {

				/* If right singular vectors desired in VT, copy result to VT
				   and generate right bidiagonalizing vectors in VT
				   (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

				NUMlapack_dlacpy ("U", n, n, &a[a_offset], lda, &vt[vt_offset], ldvt);
				i__2 = *lwork - iwork + 1;
				NUMlapack_dorgbr ("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], &work[iwork], &i__2, &ierr);
			}
			if (wntuo) {

				/* If left singular vectors desired in A, generate left
				   bidiagonalizing vectors in A (Workspace: need 4*N, prefer
				   3*N+N*NB) */

				i__2 = *lwork - iwork + 1;
				NUMlapack_dorgbr ("Q", m, n, n, &a[a_offset], lda, &work[itauq], &work[iwork], &i__2, &ierr);
			}
			if (wntvo) {

				/* If right singular vectors desired in A, generate right
				   bidiagonalizing vectors in A (Workspace: need 4*N-1,
				   prefer 3*N+(N-1)*NB) */

				i__2 = *lwork - iwork + 1;
				NUMlapack_dorgbr ("P", n, n, n, &a[a_offset], lda, &work[itaup], &work[iwork], &i__2, &ierr);
			}
			iwork = ie + *n;
			if (wntuas || wntuo) {
				nru = *m;
			}
			if (wntun) {
				nru = 0;
			}
			if (wntvas || wntvo) {
				ncvt = *n;
			}
			if (wntvn) {
				ncvt = 0;
			}
			if (!wntuo && !wntvo) {

				/* Perform bidiagonal QR iteration, if desired, computing
				   left singular vectors in U and computing right singular
				   vectors in VT (Workspace: need BDSPAC) */

				NUMlapack_dbdsqr ("U", n, &ncvt, &nru, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt,
				                  &u[u_offset], ldu, dum, &c__1, &work[iwork], info);
			} else if (!wntuo && wntvo) {

				/* Perform bidiagonal QR iteration, if desired, computing
				   left singular vectors in U and computing right singular
				   vectors in A (Workspace: need BDSPAC) */

				NUMlapack_dbdsqr ("U", n, &ncvt, &nru, &c__0, &s[1], &work[ie], &a[a_offset], lda, &u[u_offset],
				                  ldu, dum, &c__1, &work[iwork], info);
			} else {

				/* Perform bidiagonal QR iteration, if desired, computing
				   left singular vectors in A and computing right singular
				   vectors in VT (Workspace: need BDSPAC) */

				NUMlapack_dbdsqr ("U", n, &ncvt, &nru, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt,
				                  &a[a_offset], lda, dum, &c__1, &work[iwork], info);
			}

		}

	} else {

		/* A has more columns than rows. If A has sufficiently more columns
		   than rows, first reduce using the LQ decomposition (if sufficient
		   workspace available) */

		if (*n >= mnthr) {

			if (wntvn) {

				/* Path 1t(N much larger than M, JOBVT='N') No right singular
				   vectors to be computed */

				itau = 1;
				iwork = itau + *m;

				/* Compute A=L*Q (Workspace: need 2*M, prefer M+M*NB) */

				i__2 = *lwork - iwork + 1;
				NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

				/* Zero out above L */

				i__2 = *m - 1;
				i__3 = *m - 1;
				NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &a_ref (1, 2), lda);
				ie = 1;
				itauq = ie + *m;
				itaup = itauq + *m;
				iwork = itaup + *m;

				/* Bidiagonalize L in A (Workspace: need 4*M, prefer
				   3*M+2*M*NB) */

				i__2 = *lwork - iwork + 1;
				NUMlapack_dgebrd (m, m, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
				                  &work[iwork], &i__2, &ierr);
				if (wntuo || wntuas) {

					/* If left singular vectors desired, generate Q
					   (Workspace: need 4*M, prefer 3*M+M*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dorgbr ("Q", m, m, m, &a[a_offset], lda, &work[itauq], &work[iwork], &i__2,
					                  &ierr);
				}
				iwork = ie + *m;
				nru = 0;
				if (wntuo || wntuas) {
					nru = *m;
				}

				/* Perform bidiagonal QR iteration, computing left singular
				   vectors of A in A if desired (Workspace: need BDSPAC) */

				NUMlapack_dbdsqr ("U", m, &c__0, &nru, &c__0, &s[1], &work[ie], dum, &c__1, &a[a_offset], lda,
				                  dum, &c__1, &work[iwork], info);

				/* If left singular vectors desired in U, copy them there */

				if (wntuas) {
					NUMlapack_dlacpy ("F", m, m, &a[a_offset], lda, &u[u_offset], ldu);
				}

			} else if (wntvo && wntun) {

				/* Path 2t(N much larger than M, JOBU='N', JOBVT='O') M right
				   singular vectors to be overwritten on A and no left
				   singular vectors to be computed

				   Computing MAX */
				i__2 = *m << 2;
				if (*lwork >= *m * *m + MAX (i__2, bdspac)) {

					/* Sufficient workspace for a fast algorithm */

					ir = 1;
					/* Computing MAX */
					i__2 = wrkbl, i__3 = *lda * *n + *m;
					if (*lwork >= MAX (i__2, i__3) + *lda * *m) {

						/* WORK(IU) is LDA by N and WORK(IR) is LDA by M */

						ldwrku = *lda;
						chunk = *n;
						ldwrkr = *lda;
					} else {	/* if(complicated condition) */
						/* Computing MAX */
						i__2 = wrkbl, i__3 = *lda * *n + *m;
						if (*lwork >= MAX (i__2, i__3) + *m * *m) {

							/* WORK(IU) is LDA by N and WORK(IR) is M by M */

							ldwrku = *lda;
							chunk = *n;
							ldwrkr = *m;
						} else {

							/* WORK(IU) is M by CHUNK and WORK(IR) is M by M */

							ldwrku = *m;
							chunk = (*lwork - *m * *m - *m) / *m;
							ldwrkr = *m;
						}
					}
					itau = ir + ldwrkr * *m;
					iwork = itau + *m;

					/* Compute A=L*Q (Workspace: need M*M+2*M, prefer
					   M*M+M+M*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

					/* Copy L to WORK(IR) and zero out above it */

					NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &work[ir], &ldwrkr);
					i__2 = *m - 1;
					i__3 = *m - 1;
					NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &work[ir + ldwrkr], &ldwrkr);

					/* Generate Q in A (Workspace: need M*M+2*M, prefer
					   M*M+M+M*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dorglq (m, n, m, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
					ie = itau;
					itauq = ie + *m;
					itaup = itauq + *m;
					iwork = itaup + *m;

					/* Bidiagonalize L in WORK(IR) (Workspace: need M*M+4*M,
					   prefer M*M+3*M+2*M*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dgebrd (m, m, &work[ir], &ldwrkr, &s[1], &work[ie], &work[itauq], &work[itaup],
					                  &work[iwork], &i__2, &ierr);

					/* Generate right vectors bidiagonalizing L (Workspace:
					   need M*M+4*M-1, prefer M*M+3*M+(M-1)*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dorgbr ("P", m, m, m, &work[ir], &ldwrkr, &work[itaup], &work[iwork], &i__2,
					                  &ierr);
					iwork = ie + *m;

					/* Perform bidiagonal QR iteration, computing right
					   singular vectors of L in WORK(IR) (Workspace: need
					   M*M+BDSPAC) */

					NUMlapack_dbdsqr ("U", m, m, &c__0, &c__0, &s[1], &work[ie], &work[ir], &ldwrkr, dum, &c__1,
					                  dum, &c__1, &work[iwork], info);
					iu = ie + *m;

					/* Multiply right singular vectors of L in WORK(IR) by Q
					   in A, storing result in WORK(IU) and copying to A
					   (Workspace: need M*M+2*M, prefer M*M+M*N+M) */

					i__2 = *n;
					i__3 = chunk;
					for (i__ = 1; i__3 < 0 ? i__ >= i__2 : i__ <= i__2; i__ += i__3) {
						/* Computing MIN */
						i__4 = *n - i__ + 1;
						blk = MIN (i__4, chunk);
						NUMblas_dgemm ("N", "N", m, &blk, m, &c_b438, &work[ir], &ldwrkr, &a_ref (1, i__), lda,
						               &c_b416, &work[iu], &ldwrku);
						NUMlapack_dlacpy ("F", m, &blk, &work[iu], &ldwrku, &a_ref (1, i__), lda);
						/* L30: */
					}

				} else {

					/* Insufficient workspace for a fast algorithm */

					ie = 1;
					itauq = ie + *m;
					itaup = itauq + *m;
					iwork = itaup + *m;

					/* Bidiagonalize A (Workspace: need 3*M+N, prefer
					   3*M+(M+N)*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dgebrd (m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
					                  &work[iwork], &i__3, &ierr);

					/* Generate right vectors bidiagonalizing A (Workspace:
					   need 4*M, prefer 3*M+M*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dorgbr ("P", m, n, m, &a[a_offset], lda, &work[itaup], &work[iwork], &i__3,
					                  &ierr);
					iwork = ie + *m;

					/* Perform bidiagonal QR iteration, computing right
					   singular vectors of A in A (Workspace: need BDSPAC) */

					NUMlapack_dbdsqr ("L", m, n, &c__0, &c__0, &s[1], &work[ie], &a[a_offset], lda, dum, &c__1,
					                  dum, &c__1, &work[iwork], info);

				}

			} else if (wntvo && wntuas) {

				/* Path 3t(N much larger than M, JOBU='S' or 'A', JOBVT='O')
				   M right singular vectors to be overwritten on A and M left
				   singular vectors to be computed in U

				   Computing MAX */
				i__3 = *m << 2;
				if (*lwork >= *m * *m + MAX (i__3, bdspac)) {

					/* Sufficient workspace for a fast algorithm */

					ir = 1;
					/* Computing MAX */
					i__3 = wrkbl, i__2 = *lda * *n + *m;
					if (*lwork >= MAX (i__3, i__2) + *lda * *m) {

						/* WORK(IU) is LDA by N and WORK(IR) is LDA by M */

						ldwrku = *lda;
						chunk = *n;
						ldwrkr = *lda;
					} else {	/* if(complicated condition) */
						/* Computing MAX */
						i__3 = wrkbl, i__2 = *lda * *n + *m;
						if (*lwork >= MAX (i__3, i__2) + *m * *m) {

							/* WORK(IU) is LDA by N and WORK(IR) is M by M */

							ldwrku = *lda;
							chunk = *n;
							ldwrkr = *m;
						} else {

							/* WORK(IU) is M by CHUNK and WORK(IR) is M by M */

							ldwrku = *m;
							chunk = (*lwork - *m * *m - *m) / *m;
							ldwrkr = *m;
						}
					}
					itau = ir + ldwrkr * *m;
					iwork = itau + *m;

					/* Compute A=L*Q (Workspace: need M*M+2*M, prefer
					   M*M+M+M*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__3, &ierr);

					/* Copy L to U, zeroing about above it */

					NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &u[u_offset], ldu);
					i__3 = *m - 1;
					i__2 = *m - 1;
					NUMlapack_dlaset ("U", &i__3, &i__2, &c_b416, &c_b416, &u_ref (1, 2), ldu);

					/* Generate Q in A (Workspace: need M*M+2*M, prefer
					   M*M+M+M*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dorglq (m, n, m, &a[a_offset], lda, &work[itau], &work[iwork], &i__3, &ierr);
					ie = itau;
					itauq = ie + *m;
					itaup = itauq + *m;
					iwork = itaup + *m;

					/* Bidiagonalize L in U, copying result to WORK(IR)
					   (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dgebrd (m, m, &u[u_offset], ldu, &s[1], &work[ie], &work[itauq], &work[itaup],
					                  &work[iwork], &i__3, &ierr);
					NUMlapack_dlacpy ("U", m, m, &u[u_offset], ldu, &work[ir], &ldwrkr);

					/* Generate right vectors bidiagonalizing L in WORK(IR)
					   (Workspace: need M*M+4*M-1, prefer M*M+3*M+(M-1)*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dorgbr ("P", m, m, m, &work[ir], &ldwrkr, &work[itaup], &work[iwork], &i__3,
					                  &ierr);

					/* Generate left vectors bidiagonalizing L in U
					   (Workspace: need M*M+4*M, prefer M*M+3*M+M*NB) */

					i__3 = *lwork - iwork + 1;
					NUMlapack_dorgbr ("Q", m, m, m, &u[u_offset], ldu, &work[itauq], &work[iwork], &i__3,
					                  &ierr);
					iwork = ie + *m;

					/* Perform bidiagonal QR iteration, computing left
					   singular vectors of L in U, and computing right
					   singular vectors of L in WORK(IR) (Workspace: need
					   M*M+BDSPAC) */

					NUMlapack_dbdsqr ("U", m, m, m, &c__0, &s[1], &work[ie], &work[ir], &ldwrkr, &u[u_offset],
					                  ldu, dum, &c__1, &work[iwork], info);
					iu = ie + *m;

					/* Multiply right singular vectors of L in WORK(IR) by Q
					   in A, storing result in WORK(IU) and copying to A
					   (Workspace: need M*M+2*M, prefer M*M+M*N+M)) */

					i__3 = *n;
					i__2 = chunk;
					for (i__ = 1; i__2 < 0 ? i__ >= i__3 : i__ <= i__3; i__ += i__2) {
						/* Computing MIN */
						i__4 = *n - i__ + 1;
						blk = MIN (i__4, chunk);
						NUMblas_dgemm ("N", "N", m, &blk, m, &c_b438, &work[ir], &ldwrkr, &a_ref (1, i__), lda,
						               &c_b416, &work[iu], &ldwrku);
						NUMlapack_dlacpy ("F", m, &blk, &work[iu], &ldwrku, &a_ref (1, i__), lda);
						/* L40: */
					}

				} else {

					/* Insufficient workspace for a fast algorithm */

					itau = 1;
					iwork = itau + *m;

					/* Compute A=L*Q (Workspace: need 2*M, prefer M+M*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

					/* Copy L to U, zeroing out above it */

					NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &u[u_offset], ldu);
					i__2 = *m - 1;
					i__3 = *m - 1;
					NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &u_ref (1, 2), ldu);

					/* Generate Q in A (Workspace: need 2*M, prefer M+M*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dorglq (m, n, m, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
					ie = itau;
					itauq = ie + *m;
					itaup = itauq + *m;
					iwork = itaup + *m;

					/* Bidiagonalize L in U (Workspace: need 4*M, prefer
					   3*M+2*M*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dgebrd (m, m, &u[u_offset], ldu, &s[1], &work[ie], &work[itauq], &work[itaup],
					                  &work[iwork], &i__2, &ierr);

					/* Multiply right vectors bidiagonalizing L by Q in A
					   (Workspace: need 3*M+N, prefer 3*M+N*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dormbr ("P", "L", "T", m, n, m, &u[u_offset], ldu, &work[itaup], &a[a_offset],
					                  lda, &work[iwork], &i__2, &ierr);

					/* Generate left vectors bidiagonalizing L in U
					   (Workspace: need 4*M, prefer 3*M+M*NB) */

					i__2 = *lwork - iwork + 1;
					NUMlapack_dorgbr ("Q", m, m, m, &u[u_offset], ldu, &work[itauq], &work[iwork], &i__2,
					                  &ierr);
					iwork = ie + *m;

					/* Perform bidiagonal QR iteration, computing left
					   singular vectors of A in U and computing right
					   singular vectors of A in A (Workspace: need BDSPAC) */

					NUMlapack_dbdsqr ("U", m, n, m, &c__0, &s[1], &work[ie], &a[a_offset], lda, &u[u_offset],
					                  ldu, dum, &c__1, &work[iwork], info);

				}

			} else if (wntvs) {

				if (wntun) {

					/* Path 4t(N much larger than M, JOBU='N', JOBVT='S') M
					   right singular vectors to be computed in VT and no
					   left singular vectors to be computed

					   Computing MAX */
					i__2 = *m << 2;
					if (*lwork >= *m * *m + MAX (i__2, bdspac)) {

						/* Sufficient workspace for a fast algorithm */

						ir = 1;
						if (*lwork >= wrkbl + *lda * *m) {

							/* WORK(IR) is LDA by M */

							ldwrkr = *lda;
						} else {

							/* WORK(IR) is M by M */

							ldwrkr = *m;
						}
						itau = ir + ldwrkr * *m;
						iwork = itau + *m;

						/* Compute A=L*Q (Workspace: need M*M+2*M, prefer
						   M*M+M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

						/* Copy L to WORK(IR), zeroing out above it */

						NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &work[ir], &ldwrkr);
						i__2 = *m - 1;
						i__3 = *m - 1;
						NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &work[ir + ldwrkr], &ldwrkr);

						/* Generate Q in A (Workspace: need M*M+2*M, prefer
						   M*M+M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorglq (m, n, m, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						ie = itau;
						itauq = ie + *m;
						itaup = itauq + *m;
						iwork = itaup + *m;

						/* Bidiagonalize L in WORK(IR) (Workspace: need
						   M*M+4*M, prefer M*M+3*M+2*M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (m, m, &work[ir], &ldwrkr, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);

						/* Generate right vectors bidiagonalizing L in
						   WORK(IR) (Workspace: need M*M+4*M, prefer
						   M*M+3*M+(M-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", m, m, m, &work[ir], &ldwrkr, &work[itaup], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *m;

						/* Perform bidiagonal QR iteration, computing right
						   singular vectors of L in WORK(IR) (Workspace: need
						   M*M+BDSPAC) */

						NUMlapack_dbdsqr ("U", m, m, &c__0, &c__0, &s[1], &work[ie], &work[ir], &ldwrkr, dum,
						                  &c__1, dum, &c__1, &work[iwork], info);

						/* Multiply right singular vectors of L in WORK(IR)
						   by Q in A, storing result in VT (Workspace: need
						   M*M) */

						NUMblas_dgemm ("N", "N", m, n, m, &c_b438, &work[ir], &ldwrkr, &a[a_offset], lda, &c_b416,
						               &vt[vt_offset], ldvt);

					} else {

						/* Insufficient workspace for a fast algorithm */

						itau = 1;
						iwork = itau + *m;

						/* Compute A=L*Q (Workspace: need 2*M, prefer M+M*NB)
						 */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

						/* Copy result to VT */

						NUMlapack_dlacpy ("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

						/* Generate Q in VT (Workspace: need 2*M, prefer
						   M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorglq (m, n, m, &vt[vt_offset], ldvt, &work[itau], &work[iwork], &i__2,
						                  &ierr);
						ie = itau;
						itauq = ie + *m;
						itaup = itauq + *m;
						iwork = itaup + *m;

						/* Zero out above L in A */

						i__2 = *m - 1;
						i__3 = *m - 1;
						NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &a_ref (1, 2), lda);

						/* Bidiagonalize L in A (Workspace: need 4*M, prefer
						   3*M+2*M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (m, m, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
						                  &work[iwork], &i__2, &ierr);

						/* Multiply right vectors bidiagonalizing L by Q in
						   VT (Workspace: need 3*M+N, prefer 3*M+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dormbr ("P", "L", "T", m, n, m, &a[a_offset], lda, &work[itaup],
						                  &vt[vt_offset], ldvt, &work[iwork], &i__2, &ierr);
						iwork = ie + *m;

						/* Perform bidiagonal QR iteration, computing right
						   singular vectors of A in VT (Workspace: need
						   BDSPAC) */

						NUMlapack_dbdsqr ("U", m, n, &c__0, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt, dum,
						                  &c__1, dum, &c__1, &work[iwork], info);

					}

				} else if (wntuo) {

					/* Path 5t(N much larger than M, JOBU='O', JOBVT='S') M
					   right singular vectors to be computed in VT and M left
					   singular vectors to be overwritten on A

					   Computing MAX */
					i__2 = *m << 2;
					if (*lwork >= (*m << 1) * *m + MAX (i__2, bdspac)) {

						/* Sufficient workspace for a fast algorithm */

						iu = 1;
						if (*lwork >= wrkbl + (*lda << 1) * *m) {

							/* WORK(IU) is LDA by M and WORK(IR) is LDA by M */

							ldwrku = *lda;
							ir = iu + ldwrku * *m;
							ldwrkr = *lda;
						} else if (*lwork >= wrkbl + (*lda + *m) * *m) {

							/* WORK(IU) is LDA by M and WORK(IR) is M by M */

							ldwrku = *lda;
							ir = iu + ldwrku * *m;
							ldwrkr = *m;
						} else {

							/* WORK(IU) is M by M and WORK(IR) is M by M */

							ldwrku = *m;
							ir = iu + ldwrku * *m;
							ldwrkr = *m;
						}
						itau = ir + ldwrkr * *m;
						iwork = itau + *m;

						/* Compute A=L*Q (Workspace: need 2*M*M+2*M, prefer
						   2*M*M+M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

						/* Copy L to WORK(IU), zeroing out below it */

						NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &work[iu], &ldwrku);
						i__2 = *m - 1;
						i__3 = *m - 1;
						NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &work[iu + ldwrku], &ldwrku);

						/* Generate Q in A (Workspace: need 2*M*M+2*M, prefer
						   2*M*M+M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorglq (m, n, m, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						ie = itau;
						itauq = ie + *m;
						itaup = itauq + *m;
						iwork = itaup + *m;

						/* Bidiagonalize L in WORK(IU), copying result to
						   WORK(IR) (Workspace: need 2*M*M+4*M, prefer
						   2*M*M+3*M+2*M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (m, m, &work[iu], &ldwrku, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, m, &work[iu], &ldwrku, &work[ir], &ldwrkr);

						/* Generate right bidiagonalizing vectors in WORK(IU)
						   (Workspace: need 2*M*M+4*M-1, prefer
						   2*M*M+3*M+(M-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", m, m, m, &work[iu], &ldwrku, &work[itaup], &work[iwork], &i__2,
						                  &ierr);

						/* Generate left bidiagonalizing vectors in WORK(IR)
						   (Workspace: need 2*M*M+4*M, prefer 2*M*M+3*M+M*NB)
						 */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", m, m, m, &work[ir], &ldwrkr, &work[itauq], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *m;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of L in WORK(IR) and computing
						   right singular vectors of L in WORK(IU)
						   (Workspace: need 2*M*M+BDSPAC) */

						NUMlapack_dbdsqr ("U", m, m, m, &c__0, &s[1], &work[ie], &work[iu], &ldwrku, &work[ir],
						                  &ldwrkr, dum, &c__1, &work[iwork], info);

						/* Multiply right singular vectors of L in WORK(IU)
						   by Q in A, storing result in VT (Workspace: need
						   M*M) */

						NUMblas_dgemm ("N", "N", m, n, m, &c_b438, &work[iu], &ldwrku, &a[a_offset], lda, &c_b416,
						               &vt[vt_offset], ldvt);

						/* Copy left singular vectors of L to A (Workspace:
						   need M*M) */

						NUMlapack_dlacpy ("F", m, m, &work[ir], &ldwrkr, &a[a_offset], lda);

					} else {

						/* Insufficient workspace for a fast algorithm */

						itau = 1;
						iwork = itau + *m;

						/* Compute A=L*Q, copying result to VT (Workspace:
						   need 2*M, prefer M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

						/* Generate Q in VT (Workspace: need 2*M, prefer
						   M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorglq (m, n, m, &vt[vt_offset], ldvt, &work[itau], &work[iwork], &i__2,
						                  &ierr);
						ie = itau;
						itauq = ie + *m;
						itaup = itauq + *m;
						iwork = itaup + *m;

						/* Zero out above L in A */

						i__2 = *m - 1;
						i__3 = *m - 1;
						NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &a_ref (1, 2), lda);

						/* Bidiagonalize L in A (Workspace: need 4*M, prefer
						   3*M+2*M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (m, m, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
						                  &work[iwork], &i__2, &ierr);

						/* Multiply right vectors bidiagonalizing L by Q in
						   VT (Workspace: need 3*M+N, prefer 3*M+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dormbr ("P", "L", "T", m, n, m, &a[a_offset], lda, &work[itaup],
						                  &vt[vt_offset], ldvt, &work[iwork], &i__2, &ierr);

						/* Generate left bidiagonalizing vectors of L in A
						   (Workspace: need 4*M, prefer 3*M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", m, m, m, &a[a_offset], lda, &work[itauq], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *m;

						/* Perform bidiagonal QR iteration, compute left
						   singular vectors of A in A and compute right
						   singular vectors of A in VT (Workspace: need
						   BDSPAC) */

						NUMlapack_dbdsqr ("U", m, n, m, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt,
						                  &a[a_offset], lda, dum, &c__1, &work[iwork], info);

					}

				} else if (wntuas) {

					/* Path 6t(N much larger than M, JOBU='S' or 'A',
					   JOBVT='S') M right singular vectors to be computed in
					   VT and M left singular vectors to be computed in U

					   Computing MAX */
					i__2 = *m << 2;
					if (*lwork >= *m * *m + MAX (i__2, bdspac)) {

						/* Sufficient workspace for a fast algorithm */

						iu = 1;
						if (*lwork >= wrkbl + *lda * *m) {

							/* WORK(IU) is LDA by N */

							ldwrku = *lda;
						} else {

							/* WORK(IU) is LDA by M */

							ldwrku = *m;
						}
						itau = iu + ldwrku * *m;
						iwork = itau + *m;

						/* Compute A=L*Q (Workspace: need M*M+2*M, prefer
						   M*M+M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);

						/* Copy L to WORK(IU), zeroing out above it */

						NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &work[iu], &ldwrku);
						i__2 = *m - 1;
						i__3 = *m - 1;
						NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &work[iu + ldwrku], &ldwrku);

						/* Generate Q in A (Workspace: need M*M+2*M, prefer
						   M*M+M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorglq (m, n, m, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						ie = itau;
						itauq = ie + *m;
						itaup = itauq + *m;
						iwork = itaup + *m;

						/* Bidiagonalize L in WORK(IU), copying result to U
						   (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (m, m, &work[iu], &ldwrku, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, m, &work[iu], &ldwrku, &u[u_offset], ldu);

						/* Generate right bidiagonalizing vectors in WORK(IU)
						   (Workspace: need M*M+4*M-1, prefer
						   M*M+3*M+(M-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", m, m, m, &work[iu], &ldwrku, &work[itaup], &work[iwork], &i__2,
						                  &ierr);

						/* Generate left bidiagonalizing vectors in U
						   (Workspace: need M*M+4*M, prefer M*M+3*M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", m, m, m, &u[u_offset], ldu, &work[itauq], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *m;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of L in U and computing right
						   singular vectors of L in WORK(IU) (Workspace: need
						   M*M+BDSPAC) */

						NUMlapack_dbdsqr ("U", m, m, m, &c__0, &s[1], &work[ie], &work[iu], &ldwrku,
						                  &u[u_offset], ldu, dum, &c__1, &work[iwork], info);

						/* Multiply right singular vectors of L in WORK(IU)
						   by Q in A, storing result in VT (Workspace: need
						   M*M) */

						NUMblas_dgemm ("N", "N", m, n, m, &c_b438, &work[iu], &ldwrku, &a[a_offset], lda, &c_b416,
						               &vt[vt_offset], ldvt);

					} else {

						/* Insufficient workspace for a fast algorithm */

						itau = 1;
						iwork = itau + *m;

						/* Compute A=L*Q, copying result to VT (Workspace:
						   need 2*M, prefer M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

						/* Generate Q in VT (Workspace: need 2*M, prefer
						   M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorglq (m, n, m, &vt[vt_offset], ldvt, &work[itau], &work[iwork], &i__2,
						                  &ierr);

						/* Copy L to U, zeroing out above it */

						NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &u[u_offset], ldu);
						i__2 = *m - 1;
						i__3 = *m - 1;
						NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &u_ref (1, 2), ldu);
						ie = itau;
						itauq = ie + *m;
						itaup = itauq + *m;
						iwork = itaup + *m;

						/* Bidiagonalize L in U (Workspace: need 4*M, prefer
						   3*M+2*M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (m, m, &u[u_offset], ldu, &s[1], &work[ie], &work[itauq], &work[itaup],
						                  &work[iwork], &i__2, &ierr);

						/* Multiply right bidiagonalizing vectors in U by Q
						   in VT (Workspace: need 3*M+N, prefer 3*M+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dormbr ("P", "L", "T", m, n, m, &u[u_offset], ldu, &work[itaup],
						                  &vt[vt_offset], ldvt, &work[iwork], &i__2, &ierr);

						/* Generate left bidiagonalizing vectors in U
						   (Workspace: need 4*M, prefer 3*M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", m, m, m, &u[u_offset], ldu, &work[itauq], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *m;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of A in U and computing right
						   singular vectors of A in VT (Workspace: need
						   BDSPAC) */

						NUMlapack_dbdsqr ("U", m, n, m, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt,
						                  &u[u_offset], ldu, dum, &c__1, &work[iwork], info);

					}

				}

			} else if (wntva) {

				if (wntun) {

					/* Path 7t(N much larger than M, JOBU='N', JOBVT='A') N
					   right singular vectors to be computed in VT and no
					   left singular vectors to be computed

					   Computing MAX */
					i__2 = *n + *m, i__3 = *m << 2, i__2 = MAX (i__2, i__3);
					if (*lwork >= *m * *m + MAX (i__2, bdspac)) {

						/* Sufficient workspace for a fast algorithm */

						ir = 1;
						if (*lwork >= wrkbl + *lda * *m) {

							/* WORK(IR) is LDA by M */

							ldwrkr = *lda;
						} else {

							/* WORK(IR) is M by M */

							ldwrkr = *m;
						}
						itau = ir + ldwrkr * *m;
						iwork = itau + *m;

						/* Compute A=L*Q, copying result to VT (Workspace:
						   need M*M+2*M, prefer M*M+M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

						/* Copy L to WORK(IR), zeroing out above it */

						NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &work[ir], &ldwrkr);
						i__2 = *m - 1;
						i__3 = *m - 1;
						NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &work[ir + ldwrkr], &ldwrkr);

						/* Generate Q in VT (Workspace: need M*M+M+N, prefer
						   M*M+M+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorglq (n, n, m, &vt[vt_offset], ldvt, &work[itau], &work[iwork], &i__2,
						                  &ierr);
						ie = itau;
						itauq = ie + *m;
						itaup = itauq + *m;
						iwork = itaup + *m;

						/* Bidiagonalize L in WORK(IR) (Workspace: need
						   M*M+4*M, prefer M*M+3*M+2*M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (m, m, &work[ir], &ldwrkr, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);

						/* Generate right bidiagonalizing vectors in WORK(IR)
						   (Workspace: need M*M+4*M-1, prefer
						   M*M+3*M+(M-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", m, m, m, &work[ir], &ldwrkr, &work[itaup], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *m;

						/* Perform bidiagonal QR iteration, computing right
						   singular vectors of L in WORK(IR) (Workspace: need
						   M*M+BDSPAC) */

						NUMlapack_dbdsqr ("U", m, m, &c__0, &c__0, &s[1], &work[ie], &work[ir], &ldwrkr, dum,
						                  &c__1, dum, &c__1, &work[iwork], info);

						/* Multiply right singular vectors of L in WORK(IR)
						   by Q in VT, storing result in A (Workspace: need
						   M*M) */

						NUMblas_dgemm ("N", "N", m, n, m, &c_b438, &work[ir], &ldwrkr, &vt[vt_offset], ldvt, &c_b416,
						               &a[a_offset], lda);

						/* Copy right singular vectors of A from A to VT */

						NUMlapack_dlacpy ("F", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

					} else {

						/* Insufficient workspace for a fast algorithm */

						itau = 1;
						iwork = itau + *m;

						/* Compute A=L*Q, copying result to VT (Workspace:
						   need 2*M, prefer M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

						/* Generate Q in VT (Workspace: need M+N, prefer
						   M+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorglq (n, n, m, &vt[vt_offset], ldvt, &work[itau], &work[iwork], &i__2,
						                  &ierr);
						ie = itau;
						itauq = ie + *m;
						itaup = itauq + *m;
						iwork = itaup + *m;

						/* Zero out above L in A */

						i__2 = *m - 1;
						i__3 = *m - 1;
						NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &a_ref (1, 2), lda);

						/* Bidiagonalize L in A (Workspace: need 4*M, prefer
						   3*M+2*M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (m, m, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
						                  &work[iwork], &i__2, &ierr);

						/* Multiply right bidiagonalizing vectors in A by Q
						   in VT (Workspace: need 3*M+N, prefer 3*M+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dormbr ("P", "L", "T", m, n, m, &a[a_offset], lda, &work[itaup],
						                  &vt[vt_offset], ldvt, &work[iwork], &i__2, &ierr);
						iwork = ie + *m;

						/* Perform bidiagonal QR iteration, computing right
						   singular vectors of A in VT (Workspace: need
						   BDSPAC) */

						NUMlapack_dbdsqr ("U", m, n, &c__0, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt, dum,
						                  &c__1, dum, &c__1, &work[iwork], info);

					}

				} else if (wntuo) {

					/* Path 8t(N much larger than M, JOBU='O', JOBVT='A') N
					   right singular vectors to be computed in VT and M left
					   singular vectors to be overwritten on A

					   Computing MAX */
					i__2 = *n + *m, i__3 = *m << 2, i__2 = MAX (i__2, i__3);
					if (*lwork >= (*m << 1) * *m + MAX (i__2, bdspac)) {

						/* Sufficient workspace for a fast algorithm */

						iu = 1;
						if (*lwork >= wrkbl + (*lda << 1) * *m) {

							/* WORK(IU) is LDA by M and WORK(IR) is LDA by M */

							ldwrku = *lda;
							ir = iu + ldwrku * *m;
							ldwrkr = *lda;
						} else if (*lwork >= wrkbl + (*lda + *m) * *m) {

							/* WORK(IU) is LDA by M and WORK(IR) is M by M */

							ldwrku = *lda;
							ir = iu + ldwrku * *m;
							ldwrkr = *m;
						} else {

							/* WORK(IU) is M by M and WORK(IR) is M by M */

							ldwrku = *m;
							ir = iu + ldwrku * *m;
							ldwrkr = *m;
						}
						itau = ir + ldwrkr * *m;
						iwork = itau + *m;

						/* Compute A=L*Q, copying result to VT (Workspace:
						   need 2*M*M+2*M, prefer 2*M*M+M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

						/* Generate Q in VT (Workspace: need 2*M*M+M+N,
						   prefer 2*M*M+M+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorglq (n, n, m, &vt[vt_offset], ldvt, &work[itau], &work[iwork], &i__2,
						                  &ierr);

						/* Copy L to WORK(IU), zeroing out above it */

						NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &work[iu], &ldwrku);
						i__2 = *m - 1;
						i__3 = *m - 1;
						NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &work[iu + ldwrku], &ldwrku);
						ie = itau;
						itauq = ie + *m;
						itaup = itauq + *m;
						iwork = itaup + *m;

						/* Bidiagonalize L in WORK(IU), copying result to
						   WORK(IR) (Workspace: need 2*M*M+4*M, prefer
						   2*M*M+3*M+2*M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (m, m, &work[iu], &ldwrku, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, m, &work[iu], &ldwrku, &work[ir], &ldwrkr);

						/* Generate right bidiagonalizing vectors in WORK(IU)
						   (Workspace: need 2*M*M+4*M-1, prefer
						   2*M*M+3*M+(M-1)*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", m, m, m, &work[iu], &ldwrku, &work[itaup], &work[iwork], &i__2,
						                  &ierr);

						/* Generate left bidiagonalizing vectors in WORK(IR)
						   (Workspace: need 2*M*M+4*M, prefer 2*M*M+3*M+M*NB)
						 */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", m, m, m, &work[ir], &ldwrkr, &work[itauq], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *m;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of L in WORK(IR) and computing
						   right singular vectors of L in WORK(IU)
						   (Workspace: need 2*M*M+BDSPAC) */

						NUMlapack_dbdsqr ("U", m, m, m, &c__0, &s[1], &work[ie], &work[iu], &ldwrku, &work[ir],
						                  &ldwrkr, dum, &c__1, &work[iwork], info);

						/* Multiply right singular vectors of L in WORK(IU)
						   by Q in VT, storing result in A (Workspace: need
						   M*M) */

						NUMblas_dgemm ("N", "N", m, n, m, &c_b438, &work[iu], &ldwrku, &vt[vt_offset], ldvt, &c_b416,
						               &a[a_offset], lda);

						/* Copy right singular vectors of A from A to VT */

						NUMlapack_dlacpy ("F", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

						/* Copy left singular vectors of A from WORK(IR) to A
						 */

						NUMlapack_dlacpy ("F", m, m, &work[ir], &ldwrkr, &a[a_offset], lda);

					} else {

						/* Insufficient workspace for a fast algorithm */

						itau = 1;
						iwork = itau + *m;

						/* Compute A=L*Q, copying result to VT (Workspace:
						   need 2*M, prefer M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

						/* Generate Q in VT (Workspace: need M+N, prefer
						   M+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorglq (n, n, m, &vt[vt_offset], ldvt, &work[itau], &work[iwork], &i__2,
						                  &ierr);
						ie = itau;
						itauq = ie + *m;
						itaup = itauq + *m;
						iwork = itaup + *m;

						/* Zero out above L in A */

						i__2 = *m - 1;
						i__3 = *m - 1;
						NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &a_ref (1, 2), lda);

						/* Bidiagonalize L in A (Workspace: need 4*M, prefer
						   3*M+2*M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (m, m, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
						                  &work[iwork], &i__2, &ierr);

						/* Multiply right bidiagonalizing vectors in A by Q
						   in VT (Workspace: need 3*M+N, prefer 3*M+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dormbr ("P", "L", "T", m, n, m, &a[a_offset], lda, &work[itaup],
						                  &vt[vt_offset], ldvt, &work[iwork], &i__2, &ierr);

						/* Generate left bidiagonalizing vectors in A
						   (Workspace: need 4*M, prefer 3*M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", m, m, m, &a[a_offset], lda, &work[itauq], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *m;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of A in A and computing right
						   singular vectors of A in VT (Workspace: need
						   BDSPAC) */

						NUMlapack_dbdsqr ("U", m, n, m, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt,
						                  &a[a_offset], lda, dum, &c__1, &work[iwork], info);

					}

				} else if (wntuas) {

					/* Path 9t(N much larger than M, JOBU='S' or 'A',
					   JOBVT='A') N right singular vectors to be computed in
					   VT and M left singular vectors to be computed in U

					   Computing MAX */
					i__2 = *n + *m, i__3 = *m << 2, i__2 = MAX (i__2, i__3);
					if (*lwork >= *m * *m + MAX (i__2, bdspac)) {

						/* Sufficient workspace for a fast algorithm */

						iu = 1;
						if (*lwork >= wrkbl + *lda * *m) {

							/* WORK(IU) is LDA by M */

							ldwrku = *lda;
						} else {

							/* WORK(IU) is M by M */

							ldwrku = *m;
						}
						itau = iu + ldwrku * *m;
						iwork = itau + *m;

						/* Compute A=L*Q, copying result to VT (Workspace:
						   need M*M+2*M, prefer M*M+M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

						/* Generate Q in VT (Workspace: need M*M+M+N, prefer
						   M*M+M+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorglq (n, n, m, &vt[vt_offset], ldvt, &work[itau], &work[iwork], &i__2,
						                  &ierr);

						/* Copy L to WORK(IU), zeroing out above it */

						NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &work[iu], &ldwrku);
						i__2 = *m - 1;
						i__3 = *m - 1;
						NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &work[iu + ldwrku], &ldwrku);
						ie = itau;
						itauq = ie + *m;
						itaup = itauq + *m;
						iwork = itaup + *m;

						/* Bidiagonalize L in WORK(IU), copying result to U
						   (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (m, m, &work[iu], &ldwrku, &s[1], &work[ie], &work[itauq],
						                  &work[itaup], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("L", m, m, &work[iu], &ldwrku, &u[u_offset], ldu);

						/* Generate right bidiagonalizing vectors in WORK(IU)
						   (Workspace: need M*M+4*M, prefer M*M+3*M+(M-1)*NB)
						 */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("P", m, m, m, &work[iu], &ldwrku, &work[itaup], &work[iwork], &i__2,
						                  &ierr);

						/* Generate left bidiagonalizing vectors in U
						   (Workspace: need M*M+4*M, prefer M*M+3*M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", m, m, m, &u[u_offset], ldu, &work[itauq], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *m;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of L in U and computing right
						   singular vectors of L in WORK(IU) (Workspace: need
						   M*M+BDSPAC) */

						NUMlapack_dbdsqr ("U", m, m, m, &c__0, &s[1], &work[ie], &work[iu], &ldwrku,
						                  &u[u_offset], ldu, dum, &c__1, &work[iwork], info);

						/* Multiply right singular vectors of L in WORK(IU)
						   by Q in VT, storing result in A (Workspace: need
						   M*M) */

						NUMblas_dgemm ("N", "N", m, n, m, &c_b438, &work[iu], &ldwrku, &vt[vt_offset], ldvt, &c_b416,
						               &a[a_offset], lda);

						/* Copy right singular vectors of A from A to VT */

						NUMlapack_dlacpy ("F", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

					} else {

						/* Insufficient workspace for a fast algorithm */

						itau = 1;
						iwork = itau + *m;

						/* Compute A=L*Q, copying result to VT (Workspace:
						   need 2*M, prefer M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgelqf (m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2, &ierr);
						NUMlapack_dlacpy ("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

						/* Generate Q in VT (Workspace: need M+N, prefer
						   M+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorglq (n, n, m, &vt[vt_offset], ldvt, &work[itau], &work[iwork], &i__2,
						                  &ierr);

						/* Copy L to U, zeroing out above it */

						NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &u[u_offset], ldu);
						i__2 = *m - 1;
						i__3 = *m - 1;
						NUMlapack_dlaset ("U", &i__2, &i__3, &c_b416, &c_b416, &u_ref (1, 2), ldu);
						ie = itau;
						itauq = ie + *m;
						itaup = itauq + *m;
						iwork = itaup + *m;

						/* Bidiagonalize L in U (Workspace: need 4*M, prefer
						   3*M+2*M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dgebrd (m, m, &u[u_offset], ldu, &s[1], &work[ie], &work[itauq], &work[itaup],
						                  &work[iwork], &i__2, &ierr);

						/* Multiply right bidiagonalizing vectors in U by Q
						   in VT (Workspace: need 3*M+N, prefer 3*M+N*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dormbr ("P", "L", "T", m, n, m, &u[u_offset], ldu, &work[itaup],
						                  &vt[vt_offset], ldvt, &work[iwork], &i__2, &ierr);

						/* Generate left bidiagonalizing vectors in U
						   (Workspace: need 4*M, prefer 3*M+M*NB) */

						i__2 = *lwork - iwork + 1;
						NUMlapack_dorgbr ("Q", m, m, m, &u[u_offset], ldu, &work[itauq], &work[iwork], &i__2,
						                  &ierr);
						iwork = ie + *m;

						/* Perform bidiagonal QR iteration, computing left
						   singular vectors of A in U and computing right
						   singular vectors of A in VT (Workspace: need
						   BDSPAC) */

						NUMlapack_dbdsqr ("U", m, n, m, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt,
						                  &u[u_offset], ldu, dum, &c__1, &work[iwork], info);

					}

				}

			}

		} else {

			/* N .LT. MNTHR

			   Path 10t(N greater than M, but not much larger) Reduce to
			   bidiagonal form without LQ decomposition */

			ie = 1;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

			/* Bidiagonalize A (Workspace: need 3*M+N, prefer 3*M+(M+N)*NB) */

			i__2 = *lwork - iwork + 1;
			NUMlapack_dgebrd (m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &work[itaup],
			                  &work[iwork], &i__2, &ierr);
			if (wntuas) {

				/* If left singular vectors desired in U, copy result to U
				   and generate left bidiagonalizing vectors in U (Workspace:
				   need 4*M-1, prefer 3*M+(M-1)*NB) */

				NUMlapack_dlacpy ("L", m, m, &a[a_offset], lda, &u[u_offset], ldu);
				i__2 = *lwork - iwork + 1;
				NUMlapack_dorgbr ("Q", m, m, n, &u[u_offset], ldu, &work[itauq], &work[iwork], &i__2, &ierr);
			}
			if (wntvas) {

				/* If right singular vectors desired in VT, copy result to VT
				   and generate right bidiagonalizing vectors in VT
				   (Workspace: need 3*M+NRVT, prefer 3*M+NRVT*NB) */

				NUMlapack_dlacpy ("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);
				if (wntva) {
					nrvt = *n;
				}
				if (wntvs) {
					nrvt = *m;
				}
				i__2 = *lwork - iwork + 1;
				NUMlapack_dorgbr ("P", &nrvt, n, m, &vt[vt_offset], ldvt, &work[itaup], &work[iwork], &i__2,
				                  &ierr);
			}
			if (wntuo) {

				/* If left singular vectors desired in A, generate left
				   bidiagonalizing vectors in A (Workspace: need 4*M-1,
				   prefer 3*M+(M-1)*NB) */

				i__2 = *lwork - iwork + 1;
				NUMlapack_dorgbr ("Q", m, m, n, &a[a_offset], lda, &work[itauq], &work[iwork], &i__2, &ierr);
			}
			if (wntvo) {

				/* If right singular vectors desired in A, generate right
				   bidiagonalizing vectors in A (Workspace: need 4*M, prefer
				   3*M+M*NB) */

				i__2 = *lwork - iwork + 1;
				NUMlapack_dorgbr ("P", m, n, m, &a[a_offset], lda, &work[itaup], &work[iwork], &i__2, &ierr);
			}
			iwork = ie + *m;
			if (wntuas || wntuo) {
				nru = *m;
			}
			if (wntun) {
				nru = 0;
			}
			if (wntvas || wntvo) {
				ncvt = *n;
			}
			if (wntvn) {
				ncvt = 0;
			}
			if (!wntuo && !wntvo) {

				/* Perform bidiagonal QR iteration, if desired, computing
				   left singular vectors in U and computing right singular
				   vectors in VT (Workspace: need BDSPAC) */

				NUMlapack_dbdsqr ("L", m, &ncvt, &nru, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt,
				                  &u[u_offset], ldu, dum, &c__1, &work[iwork], info);
			} else if (!wntuo && wntvo) {

				/* Perform bidiagonal QR iteration, if desired, computing
				   left singular vectors in U and computing right singular
				   vectors in A (Workspace: need BDSPAC) */

				NUMlapack_dbdsqr ("L", m, &ncvt, &nru, &c__0, &s[1], &work[ie], &a[a_offset], lda, &u[u_offset],
				                  ldu, dum, &c__1, &work[iwork], info);
			} else {

				/* Perform bidiagonal QR iteration, if desired, computing
				   left singular vectors in A and computing right singular
				   vectors in VT (Workspace: need BDSPAC) */

				NUMlapack_dbdsqr ("L", m, &ncvt, &nru, &c__0, &s[1], &work[ie], &vt[vt_offset], ldvt,
				                  &a[a_offset], lda, dum, &c__1, &work[iwork], info);
			}

		}

	}

	/* If DBDSQR failed to converge, copy unconverged superdiagonals to WORK(
	   2:MINMN ) */

	if (*info != 0) {
		if (ie > 2) {
			i__2 = minmn - 1;
			for (i__ = 1; i__ <= i__2; ++i__) {
				work[i__ + 1] = work[i__ + ie - 1];
				/* L50: */
			}
		}
		if (ie < 2) {
			for (i__ = minmn - 1; i__ >= 1; --i__) {
				work[i__ + 1] = work[i__ + ie - 1];
				/* L60: */
			}
		}
	}

	/* Undo scaling if necessary */

	if (iscl == 1) {
		if (anrm > bignum) {
			NUMlapack_dlascl ("G", &c__0, &c__0, &bignum, &anrm, &minmn, &c__1, &s[1], &minmn, &ierr);
		}
		if (*info != 0 && anrm > bignum) {
			i__2 = minmn - 1;
			NUMlapack_dlascl ("G", &c__0, &c__0, &bignum, &anrm, &i__2, &c__1, &work[2], &minmn, &ierr);
		}
		if (anrm < smlnum) {
			NUMlapack_dlascl ("G", &c__0, &c__0, &smlnum, &anrm, &minmn, &c__1, &s[1], &minmn, &ierr);
		}
		if (*info != 0 && anrm < smlnum) {
			i__2 = minmn - 1;
			NUMlapack_dlascl ("G", &c__0, &c__0, &smlnum, &anrm, &i__2, &c__1, &work[2], &minmn, &ierr);
		}
	}

	/* Return optimal workspace in WORK(1) */

	work[1] = (double) maxwrk;
	return 0;
}								/* NUMlapack_dgesvd */

#undef vt_ref
#undef u_ref

int NUMlapack_dgetf2 (long *m, long *n, double *a, long *lda, long *ipiv, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static double c_b6 = -1.;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;
	double d__1;

	/* Local variables */
	static long j;
	static long jp;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--ipiv;

	/* Function Body */
	*info = 0;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *m)) {
		*info = -4;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGETF2", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*m == 0 || *n == 0) {
		return 0;
	}

	i__1 = MIN (*m, *n);
	for (j = 1; j <= i__1; ++j) {

		/* Find pivot and test for singularity. */

		i__2 = *m - j + 1;
		jp = j - 1 + NUMblas_idamax (&i__2, &a_ref (j, j), &c__1);
		ipiv[j] = jp;
		if (a_ref (jp, j) != 0.) {

			/* Apply the interchange to columns 1:N. */

			if (jp != j) {
				NUMblas_dswap (n, &a_ref (j, 1), lda, &a_ref (jp, 1), lda);
			}

			/* Compute elements J+1:M of J-th column. */

			if (j < *m) {
				i__2 = *m - j;
				d__1 = 1. / a_ref (j, j);
				NUMblas_dscal (&i__2, &d__1, &a_ref (j + 1, j), &c__1);
			}

		} else if (*info == 0) {

			*info = j;
		}

		if (j < MIN (*m, *n)) {

			/* Update trailing submatrix. */

			i__2 = *m - j;
			i__3 = *n - j;
			NUMblas_dger (&i__2, &i__3, &c_b6, &a_ref (j + 1, j), &c__1, &a_ref (j, j + 1), lda, &a_ref (j + 1,
			              j + 1), lda);
		}
		/* L10: */
	}
	return 0;
}								/* NUMlapack_dgetf2 */

int NUMlapack_dgetri (long *n, double *a, long *lda, long *ipiv, double *work, long *lwork, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__2 = 2;
	static double c_b20 = -1.;
	static double c_b22 = 1.;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;

	/* Local variables */
	static long i__, j;
	static long nbmin;
	static long jb, nb, jj, jp, nn;
	static long ldwork;
	static long lwkopt;
	static long lquery;
	static long iws;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--ipiv;
	--work;

	/* Function Body */
	*info = 0;
	nb = NUMlapack_ilaenv (&c__1, "DGETRI", " ", n, &c_n1, &c_n1, &c_n1, 6, 1);
	lwkopt = *n * nb;
	work[1] = (double) lwkopt;
	lquery = *lwork == -1;
	if (*n < 0) {
		*info = -1;
	} else if (*lda < MAX (1, *n)) {
		*info = -3;
	} else if (*lwork < MAX (1, *n) && !lquery) {
		*info = -6;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGETRI", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*n == 0) {
		return 0;
	}

	/* Form inv(U).  If INFO > 0 from DTRTRI, then U is singular, and the
	   inverse is not computed. */

	NUMlapack_dtrtri ("Upper", "Non-unit", n, &a[a_offset], lda, info);
	if (*info > 0) {
		return 0;
	}

	nbmin = 2;
	ldwork = *n;
	if (nb > 1 && nb < *n) {
		/* Computing MAX */
		i__1 = ldwork * nb;
		iws = MAX (i__1, 1);
		if (*lwork < iws) {
			nb = *lwork / ldwork;
			/* Computing MAX */
			i__1 = 2, i__2 = NUMlapack_ilaenv (&c__2, "DGETRI", " ", n, &c_n1, &c_n1, &c_n1, 6, 1);
			nbmin = MAX (i__1, i__2);
		}
	} else {
		iws = *n;
	}

	/* Solve the equation inv(A)*L = inv(U) for inv(A). */

	if (nb < nbmin || nb >= *n) {

		/* Use unblocked code. */

		for (j = *n; j >= 1; --j) {

			/* Copy current column of L to WORK and replace with zeros. */

			i__1 = *n;
			for (i__ = j + 1; i__ <= i__1; ++i__) {
				work[i__] = a_ref (i__, j);
				a_ref (i__, j) = 0.;
				/* L10: */
			}

			/* Compute current column of inv(A). */

			if (j < *n) {
				i__1 = *n - j;
				NUMblas_dgemv ("No transpose", n, &i__1, &c_b20, &a_ref (1, j + 1), lda, &work[j + 1], &c__1, &c_b22,
				               &a_ref (1, j), &c__1);
			}
			/* L20: */
		}
	} else {

		/* Use blocked code. */

		nn = (*n - 1) / nb * nb + 1;
		i__1 = -nb;
		for (j = nn; i__1 < 0 ? j >= 1 : j <= 1; j += i__1) {
			/* Computing MIN */
			i__2 = nb, i__3 = *n - j + 1;
			jb = MIN (i__2, i__3);

			/* Copy current block column of L to WORK and replace with zeros.
			 */

			i__2 = j + jb - 1;
			for (jj = j; jj <= i__2; ++jj) {
				i__3 = *n;
				for (i__ = jj + 1; i__ <= i__3; ++i__) {
					work[i__ + (jj - j) * ldwork] = a_ref (i__, jj);
					a_ref (i__, jj) = 0.;
					/* L30: */
				}
				/* L40: */
			}

			/* Compute current block column of inv(A). */

			if (j + jb <= *n) {
				i__2 = *n - j - jb + 1;
				NUMblas_dgemm ("No transpose", "No transpose", n, &jb, &i__2, &c_b20, &a_ref (1, j + jb), lda,
				               &work[j + jb], &ldwork, &c_b22, &a_ref (1, j), lda);
			}
			NUMblas_dtrsm ("Right", "Lower", "No transpose", "Unit", n, &jb, &c_b22, &work[j], &ldwork, &a_ref (1,
			               j), lda);
			/* L50: */
		}
	}

	/* Apply column interchanges. */

	for (j = *n - 1; j >= 1; --j) {
		jp = ipiv[j];
		if (jp != j) {
			NUMblas_dswap (n, &a_ref (1, j), &c__1, &a_ref (1, jp), &c__1);
		}
		/* L60: */
	}

	work[1] = (double) iws;
	return 0;
}								/* NUMlapack_dgetri */

int NUMlapack_dgetrf (long *m, long *n, double *a, long *lda, long *ipiv, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static double c_b16 = 1.;
	static double c_b19 = -1.;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5;

	/* Local variables */
	static long i__, j;
	static long iinfo;
	static long jb, nb;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--ipiv;

	/* Function Body */
	*info = 0;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *m)) {
		*info = -4;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGETRF", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*m == 0 || *n == 0) {
		return 0;
	}

	/* Determine the block size for this environment. */

	nb = NUMlapack_ilaenv (&c__1, "DGETRF", " ", m, n, &c_n1, &c_n1, 6, 1);
	if (nb <= 1 || nb >= MIN (*m, *n)) {

		/* Use unblocked code. */

		NUMlapack_dgetf2 (m, n, &a[a_offset], lda, &ipiv[1], info);
	} else {

		/* Use blocked code. */

		i__1 = MIN (*m, *n);
		i__2 = nb;
		for (j = 1; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {
			/* Computing MIN */
			i__3 = MIN (*m, *n) - j + 1;
			jb = MIN (i__3, nb);

			/* Factor diagonal and subdiagonal blocks and test for exact
			   singularity. */

			i__3 = *m - j + 1;
			NUMlapack_dgetf2 (&i__3, &jb, &a_ref (j, j), lda, &ipiv[j], &iinfo);

			/* Adjust INFO and the pivot indices. */

			if (*info == 0 && iinfo > 0) {
				*info = iinfo + j - 1;
			}
			/* Computing MIN */
			i__4 = *m, i__5 = j + jb - 1;
			i__3 = MIN (i__4, i__5);
			for (i__ = j; i__ <= i__3; ++i__) {
				ipiv[i__] = j - 1 + ipiv[i__];
				/* L10: */
			}

			/* Apply interchanges to columns 1:J-1. */

			i__3 = j - 1;
			i__4 = j + jb - 1;
			NUMlapack_dlaswp (&i__3, &a[a_offset], lda, &j, &i__4, &ipiv[1], &c__1);

			if (j + jb <= *n) {

				/* Apply interchanges to columns J+JB:N. */

				i__3 = *n - j - jb + 1;
				i__4 = j + jb - 1;
				NUMlapack_dlaswp (&i__3, &a_ref (1, j + jb), lda, &j, &i__4, &ipiv[1], &c__1);

				/* Compute block row of U. */

				i__3 = *n - j - jb + 1;
				NUMblas_dtrsm ("Left", "Lower", "No transpose", "Unit", &jb, &i__3, &c_b16, &a_ref (j, j), lda,
				               &a_ref (j, j + jb), lda);
				if (j + jb <= *m) {

					/* Update trailing submatrix. */

					i__3 = *m - j - jb + 1;
					i__4 = *n - j - jb + 1;
					NUMblas_dgemm ("No transpose", "No transpose", &i__3, &i__4, &jb, &c_b19, &a_ref (j + jb, j),
					               lda, &a_ref (j, j + jb), lda, &c_b16, &a_ref (j + jb, j + jb), lda);
				}
			}
			/* L20: */
		}
	}
	return 0;
}								/* NUMlapack_dgetrf */

int NUMlapack_dgetrs (const char *trans, long *n, long *nrhs, double *a, long *lda, long *ipiv, double *b, long *ldb,
                      long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static double c_b12 = 1.;
	static long c_n1 = -1;

	/* System generated locals */
	long a_dim1, a_offset, b_dim1, b_offset, i__1;

	/* Local variables */
	static long notran;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--ipiv;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1 * 1;
	b -= b_offset;

	/* Function Body */
	*info = 0;
	notran = lsame_ (trans, "N");
	if (!notran && !lsame_ (trans, "T") && !lsame_ (trans, "C")) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*nrhs < 0) {
		*info = -3;
	} else if (*lda < MAX (1, *n)) {
		*info = -5;
	} else if (*ldb < MAX (1, *n)) {
		*info = -8;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGETRS", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*n == 0 || *nrhs == 0) {
		return 0;
	}

	if (notran) {

		/* Solve A * X = B.

		   Apply row interchanges to the right hand sides. */

		NUMlapack_dlaswp (nrhs, &b[b_offset], ldb, &c__1, n, &ipiv[1], &c__1);

		/* Solve L*X = B, overwriting B with X. */

		NUMblas_dtrsm ("Left", "Lower", "No transpose", "Unit", n, nrhs, &c_b12, &a[a_offset], lda, &b[b_offset],
		               ldb);

		/* Solve U*X = B, overwriting B with X. */

		NUMblas_dtrsm ("Left", "Upper", "No transpose", "Non-unit", n, nrhs, &c_b12, &a[a_offset], lda, &b[b_offset],
		               ldb);
	} else {

		/* Solve A' * X = B.

		   Solve U'*X = B, overwriting B with X. */

		NUMblas_dtrsm ("Left", "Upper", "Transpose", "Non-unit", n, nrhs, &c_b12, &a[a_offset], lda, &b[b_offset],
		               ldb);

		/* Solve L'*X = B, overwriting B with X. */

		NUMblas_dtrsm ("Left", "Lower", "Transpose", "Unit", n, nrhs, &c_b12, &a[a_offset], lda, &b[b_offset], ldb);

		/* Apply row interchanges to the solution vectors. */

		NUMlapack_dlaswp (nrhs, &b[b_offset], ldb, &c__1, n, &ipiv[1], &c_n1);
	}

	return 0;
}								/* NUMlapack_dgetrs */

int NUMlapack_dggsvd (const char *jobu, const char *jobv, const char *jobq, long *m, long *n, long *p, long *k, long *l,
                      double *a, long *lda, double *b, long *ldb, double *alpha, double *beta, double *u, long *ldu, double *v,
                      long *ldv, double *q, long *ldq, double *work, long *iwork, long *info) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, u_dim1, u_offset, v_dim1, v_offset, i__1, i__2;

	/* Local variables */
	static long ibnd;
	static double tola;
	static long isub;
	static double tolb, unfl, temp, smax;
	static long i__, j;
	static double anorm, bnorm;
	static long wantq, wantu, wantv;
	static long ncycle;
	static double ulp;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1 * 1;
	b -= b_offset;
	--alpha;
	--beta;
	u_dim1 = *ldu;
	u_offset = 1 + u_dim1 * 1;
	u -= u_offset;
	v_dim1 = *ldv;
	v_offset = 1 + v_dim1 * 1;
	v -= v_offset;
	q_dim1 = *ldq;
	q_offset = 1 + q_dim1 * 1;
	q -= q_offset;
	--work;
	--iwork;

	/* Function Body */
	wantu = lsame_ (jobu, "U");
	wantv = lsame_ (jobv, "V");
	wantq = lsame_ (jobq, "Q");

	*info = 0;
	if (! (wantu || lsame_ (jobu, "N"))) {
		*info = -1;
	} else if (! (wantv || lsame_ (jobv, "N"))) {
		*info = -2;
	} else if (! (wantq || lsame_ (jobq, "N"))) {
		*info = -3;
	} else if (*m < 0) {
		*info = -4;
	} else if (*n < 0) {
		*info = -5;
	} else if (*p < 0) {
		*info = -6;
	} else if (*lda < MAX (1, *m)) {
		*info = -10;
	} else if (*ldb < MAX (1, *p)) {
		*info = -12;
	} else if (*ldu < 1 || wantu && *ldu < *m) {
		*info = -16;
	} else if (*ldv < 1 || wantv && *ldv < *p) {
		*info = -18;
	} else if (*ldq < 1 || wantq && *ldq < *n) {
		*info = -20;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGGSVD", &i__1);
		return 0;
	}

	/* Compute the Frobenius norm of matrices A and B */

	anorm = NUMlapack_dlange ("1", m, n, &a[a_offset], lda, &work[1]);
	bnorm = NUMlapack_dlange ("1", p, n, &b[b_offset], ldb, &work[1]);

	/* Get machine precision and set up threshold for determining the
	   effective numerical rank of the matrices A and B. */

	ulp = NUMblas_dlamch ("Precision");
	unfl = NUMblas_dlamch ("Safe Minimum");
	tola = MAX (*m, *n) * MAX (anorm, unfl) * ulp;
	tolb = MAX (*p, *n) * MAX (bnorm, unfl) * ulp;

	/* Preprocessing */

	NUMlapack_dggsvp (jobu, jobv, jobq, m, p, n, &a[a_offset], lda, &b[b_offset], ldb, &tola, &tolb, k, l,
	                  &u[u_offset], ldu, &v[v_offset], ldv, &q[q_offset], ldq, &iwork[1], &work[1], &work[*n + 1], info);

	/* Compute the GSVD of two upper "triangular" matrices */

	NUMlapack_dtgsja (jobu, jobv, jobq, m, p, n, k, l, &a[a_offset], lda, &b[b_offset], ldb, &tola, &tolb,
	                  &alpha[1], &beta[1], &u[u_offset], ldu, &v[v_offset], ldv, &q[q_offset], ldq, &work[1], &ncycle,
	                  info);

	/* Sort the singular values and store the pivot indices in IWORK Copy
	   ALPHA to WORK, then sort ALPHA in WORK */

	NUMblas_dcopy (n, &alpha[1], &c__1, &work[1], &c__1);
	/* Computing MIN */
	i__1 = *l, i__2 = *m - *k;
	ibnd = MIN (i__1, i__2);
	i__1 = ibnd;
	for (i__ = 1; i__ <= i__1; ++i__) {

		/* Scan for largest ALPHA(K+I) */

		isub = i__;
		smax = work[*k + i__];
		i__2 = ibnd;
		for (j = i__ + 1; j <= i__2; ++j) {
			temp = work[*k + j];
			if (temp > smax) {
				isub = j;
				smax = temp;
			}
			/* L10: */
		}
		if (isub != i__) {
			work[*k + isub] = work[*k + i__];
			work[*k + i__] = smax;
			iwork[*k + i__] = *k + isub;
		} else {
			iwork[*k + i__] = *k + i__;
		}
		/* L20: */
	}

	return 0;
}								/* NUMlapack_dggsvd */

#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]
#define u_ref(a_1,a_2) u[(a_2)*u_dim1 + a_1]
#define v_ref(a_1,a_2) v[(a_2)*v_dim1 + a_1]

int NUMlapack_dggsvp (const char *jobu, const char *jobv, const char *jobq, long *m, long *p, long *n, double *a, long *lda,
                      double *b, long *ldb, double *tola, double *tolb, long *k, long *l, double *u, long *ldu, double *v,
                      long *ldv, double *q, long *ldq, long *iwork, double *tau, double *work, long *info) {
	/* Table of constant values */
	static double c_b12 = 0.;
	static double c_b22 = 1.;

	/* System generated locals */
	long a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, u_dim1, u_offset, v_dim1, v_offset, i__1, i__2,
	     i__3;
	double d__1;

	/* Local variables */
	static long i__, j;
	static long wantq, wantu, wantv;
	static long forwrd;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1 * 1;
	b -= b_offset;
	u_dim1 = *ldu;
	u_offset = 1 + u_dim1 * 1;
	u -= u_offset;
	v_dim1 = *ldv;
	v_offset = 1 + v_dim1 * 1;
	v -= v_offset;
	q_dim1 = *ldq;
	q_offset = 1 + q_dim1 * 1;
	q -= q_offset;
	--iwork;
	--tau;
	--work;

	/* Function Body */
	wantu = lsame_ (jobu, "U");
	wantv = lsame_ (jobv, "V");
	wantq = lsame_ (jobq, "Q");
	forwrd = TRUE;

	*info = 0;
	if (! (wantu || lsame_ (jobu, "N"))) {
		*info = -1;
	} else if (! (wantv || lsame_ (jobv, "N"))) {
		*info = -2;
	} else if (! (wantq || lsame_ (jobq, "N"))) {
		*info = -3;
	} else if (*m < 0) {
		*info = -4;
	} else if (*p < 0) {
		*info = -5;
	} else if (*n < 0) {
		*info = -6;
	} else if (*lda < MAX (1, *m)) {
		*info = -8;
	} else if (*ldb < MAX (1, *p)) {
		*info = -10;
	} else if (*ldu < 1 || wantu && *ldu < *m) {
		*info = -16;
	} else if (*ldv < 1 || wantv && *ldv < *p) {
		*info = -18;
	} else if (*ldq < 1 || wantq && *ldq < *n) {
		*info = -20;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DGGSVP", &i__1);
		return 0;
	}

	/* QR with column pivoting of B: B*P = V*( S11 S12 ) ( 0 0 ) */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		iwork[i__] = 0;
		/* L10: */
	}
	NUMlapack_dgeqpf (p, n, &b[b_offset], ldb, &iwork[1], &tau[1], &work[1], info);

	/* Update A := A*P */

	NUMlapack_dlapmt (&forwrd, m, n, &a[a_offset], lda, &iwork[1]);

	/* Determine the effective rank of matrix B. */

	*l = 0;
	i__1 = MIN (*p, *n);
	for (i__ = 1; i__ <= i__1; ++i__) {
		if ( (d__1 = b_ref (i__, i__), fabs (d__1)) > *tolb) {
			++ (*l);
		}
		/* L20: */
	}

	if (wantv) {

		/* Copy the details of V, and form V. */

		NUMlapack_dlaset ("Full", p, p, &c_b12, &c_b12, &v[v_offset], ldv);
		if (*p > 1) {
			i__1 = *p - 1;
			NUMlapack_dlacpy ("Lower", &i__1, n, &b_ref (2, 1), ldb, &v_ref (2, 1), ldv);
		}
		i__1 = MIN (*p, *n);
		NUMlapack_dorg2r (p, p, &i__1, &v[v_offset], ldv, &tau[1], &work[1], info);
	}

	/* Clean up B */

	i__1 = *l - 1;
	for (j = 1; j <= i__1; ++j) {
		i__2 = *l;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
			b_ref (i__, j) = 0.;
			/* L30: */
		}
		/* L40: */
	}
	if (*p > *l) {
		i__1 = *p - *l;
		NUMlapack_dlaset ("Full", &i__1, n, &c_b12, &c_b12, &b_ref (*l + 1, 1), ldb);
	}

	if (wantq) {

		/* Set Q = I and Update Q := Q*P */

		NUMlapack_dlaset ("Full", n, n, &c_b12, &c_b22, &q[q_offset], ldq);
		NUMlapack_dlapmt (&forwrd, n, n, &q[q_offset], ldq, &iwork[1]);
	}

	if (*p >= *l && *n != *l) {

		/* RQ factorization of (S11 S12): ( S11 S12 ) = ( 0 S12 )*Z */

		NUMlapack_dgerq2 (l, n, &b[b_offset], ldb, &tau[1], &work[1], info);

		/* Update A := A*Z' */

		NUMlapack_dormr2 ("Right", "Transpose", m, n, l, &b[b_offset], ldb, &tau[1], &a[a_offset], lda,
		                  &work[1], info);

		if (wantq) {

			/* Update Q := Q*Z' */

			NUMlapack_dormr2 ("Right", "Transpose", n, n, l, &b[b_offset], ldb, &tau[1], &q[q_offset], ldq,
			                  &work[1], info);
		}

		/* Clean up B */

		i__1 = *n - *l;
		NUMlapack_dlaset ("Full", l, &i__1, &c_b12, &c_b12, &b[b_offset], ldb);
		i__1 = *n;
		for (j = *n - *l + 1; j <= i__1; ++j) {
			i__2 = *l;
			for (i__ = j - *n + *l + 1; i__ <= i__2; ++i__) {
				b_ref (i__, j) = 0.;
				/* L50: */
			}
			/* L60: */
		}

	}

	/* Let N-L L A = ( A11 A12 ) M,

	   then the following does the complete QR decomposition of A11:

	   A11 = U*( 0 T12 )*P1' ( 0 0 ) */

	i__1 = *n - *l;
	for (i__ = 1; i__ <= i__1; ++i__) {
		iwork[i__] = 0;
		/* L70: */
	}
	i__1 = *n - *l;
	NUMlapack_dgeqpf (m, &i__1, &a[a_offset], lda, &iwork[1], &tau[1], &work[1], info);

	/* Determine the effective rank of A11 */

	*k = 0;
	/* Computing MIN */
	i__2 = *m, i__3 = *n - *l;
	i__1 = MIN (i__2, i__3);
	for (i__ = 1; i__ <= i__1; ++i__) {
		if ( (d__1 = a_ref (i__, i__), fabs (d__1)) > *tola) {
			++ (*k);
		}
		/* L80: */
	}

	/* Update A12 := U'*A12, where A12 = A( 1:M, N-L+1:N )

	   Computing MIN */
	i__2 = *m, i__3 = *n - *l;
	i__1 = MIN (i__2, i__3);
	NUMlapack_dorm2r ("Left", "Transpose", m, l, &i__1, &a[a_offset], lda, &tau[1], &a_ref (1, *n - *l + 1),
	                  lda, &work[1], info);

	if (wantu) {

		/* Copy the details of U, and form U */

		NUMlapack_dlaset ("Full", m, m, &c_b12, &c_b12, &u[u_offset], ldu);
		if (*m > 1) {
			i__1 = *m - 1;
			i__2 = *n - *l;
			NUMlapack_dlacpy ("Lower", &i__1, &i__2, &a_ref (2, 1), lda, &u_ref (2, 1), ldu);
		}
		/* Computing MIN */
		i__2 = *m, i__3 = *n - *l;
		i__1 = MIN (i__2, i__3);
		NUMlapack_dorg2r (m, m, &i__1, &u[u_offset], ldu, &tau[1], &work[1], info);
	}

	if (wantq) {

		/* Update Q( 1:N, 1:N-L ) = Q( 1:N, 1:N-L )*P1 */

		i__1 = *n - *l;
		NUMlapack_dlapmt (&forwrd, n, &i__1, &q[q_offset], ldq, &iwork[1]);
	}

	/* Clean up A: set the strictly lower triangular part of A(1:K, 1:K) = 0,
	   and A( K+1:M, 1:N-L ) = 0. */

	i__1 = *k - 1;
	for (j = 1; j <= i__1; ++j) {
		i__2 = *k;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
			a_ref (i__, j) = 0.;
			/* L90: */
		}
		/* L100: */
	}
	if (*m > *k) {
		i__1 = *m - *k;
		i__2 = *n - *l;
		NUMlapack_dlaset ("Full", &i__1, &i__2, &c_b12, &c_b12, &a_ref (*k + 1, 1), lda);
	}

	if (*n - *l > *k) {

		/* RQ factorization of ( T11 T12 ) = ( 0 T12 )*Z1 */

		i__1 = *n - *l;
		NUMlapack_dgerq2 (k, &i__1, &a[a_offset], lda, &tau[1], &work[1], info);

		if (wantq) {

			/* Update Q( 1:N,1:N-L ) = Q( 1:N,1:N-L )*Z1' */

			i__1 = *n - *l;
			NUMlapack_dormr2 ("Right", "Transpose", n, &i__1, k, &a[a_offset], lda, &tau[1], &q[q_offset], ldq,
			                  &work[1], info);
		}

		/* Clean up A */

		i__1 = *n - *l - *k;
		NUMlapack_dlaset ("Full", k, &i__1, &c_b12, &c_b12, &a[a_offset], lda);
		i__1 = *n - *l;
		for (j = *n - *l - *k + 1; j <= i__1; ++j) {
			i__2 = *k;
			for (i__ = j - *n + *l + *k + 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = 0.;
				/* L110: */
			}
			/* L120: */
		}

	}

	if (*m > *k) {

		/* QR factorization of A( K+1:M,N-L+1:N ) */

		i__1 = *m - *k;
		NUMlapack_dgeqr2 (&i__1, l, &a_ref (*k + 1, *n - *l + 1), lda, &tau[1], &work[1], info);

		if (wantu) {

			/* Update U(:,K+1:M) := U(:,K+1:M)*U1 */

			i__1 = *m - *k;
			/* Computing MIN */
			i__3 = *m - *k;
			i__2 = MIN (i__3, *l);
			NUMlapack_dorm2r ("Right", "No transpose", m, &i__1, &i__2, &a_ref (*k + 1, *n - *l + 1), lda,
			                  &tau[1], &u_ref (1, *k + 1), ldu, &work[1], info);
		}

		/* Clean up */

		i__1 = *n;
		for (j = *n - *l + 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = j - *n + *k + *l + 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = 0.;
				/* L130: */
			}
			/* L140: */
		}

	}
	return 0;
}								/* NUMlapack_dggsvp */

#undef v_ref
#undef u_ref
#undef b_ref


int NUMlapack_dhseqr (const char *job, const char *compz, long *n, long *ilo, long *ihi, double *h__, long *ldh,
                      double *wr, double *wi, double *z__, long *ldz, double *work, long *lwork, long *info) {
	/* Table of constant values */
	static double c_b9 = 0.;
	static double c_b10 = 1.;
	static long c__4 = 4;
	static long c_n1 = -1;
	static long c__2 = 2;
	static long c__8 = 8;
	static long c__15 = 15;
	static int c_false = FALSE;
	static long c__1 = 1;

	/* System generated locals */
	const char *a__1[2];
	long h_dim1, h_offset, z_dim1, z_offset, i__1, i__2, i__3[2], i__4, i__5;
	double d__1, d__2;
	char ch__1[2];

	/* Local variables */
	static long maxb;
	static double absw;
	static long ierr;
	static double unfl, temp, ovfl;
	static long i__, j, k, l;
	static double s[225] /* was [15][15] */ , v[16];
	static long itemp;
	static long i1, i2;
	static int initz, wantt, wantz;
	static long ii, nh;
	static long nr, ns;
	static long nv;
	static double vv[16];
	static double smlnum;
	static int lquery;
	static long itn;
	static double tau;
	static long its;
	static double ulp, tst1;

#define h___ref(a_1,a_2) h__[(a_2)*h_dim1 + a_1]
#define s_ref(a_1,a_2) s[(a_2)*15 + a_1 - 16]
#define z___ref(a_1,a_2) z__[(a_2)*z_dim1 + a_1]

	h_dim1 = *ldh;
	h_offset = 1 + h_dim1 * 1;
	h__ -= h_offset;
	--wr;
	--wi;
	z_dim1 = *ldz;
	z_offset = 1 + z_dim1 * 1;
	z__ -= z_offset;
	--work;

	/* Function Body */
	wantt = lsame_ (job, "S");
	initz = lsame_ (compz, "I");
	wantz = initz || lsame_ (compz, "V");

	*info = 0;
	work[1] = (double) MAX (1, *n);
	lquery = *lwork == -1;
	if (!lsame_ (job, "E") && !wantt) {
		*info = -1;
	} else if (!lsame_ (compz, "N") && !wantz) {
		*info = -2;
	} else if (*n < 0) {
		*info = -3;
	} else if (*ilo < 1 || *ilo > MAX (1, *n)) {
		*info = -4;
	} else if (*ihi < MIN (*ilo, *n) || *ihi > *n) {
		*info = -5;
	} else if (*ldh < MAX (1, *n)) {
		*info = -7;
	} else if (*ldz < 1 || wantz && *ldz < MAX (1, *n)) {
		*info = -11;
	} else if (*lwork < MAX (1, *n) && !lquery) {
		*info = -13;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("NUMlapack_dhseqr ", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Initialize Z, if necessary */

	if (initz) {
		NUMlapack_dlaset ("Full", n, n, &c_b9, &c_b10, &z__[z_offset], ldz);
	}

	/* Store the eigenvalues isolated by NUMlapack_dgebal. */

	i__1 = *ilo - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
		wr[i__] = h___ref (i__, i__);
		wi[i__] = 0.;
		/* L10: */
	}
	i__1 = *n;
	for (i__ = *ihi + 1; i__ <= i__1; ++i__) {
		wr[i__] = h___ref (i__, i__);
		wi[i__] = 0.;
		/* L20: */
	}

	/* Quick return if possible. */

	if (*n == 0) {
		return 0;
	}
	if (*ilo == *ihi) {
		wr[*ilo] = h___ref (*ilo, *ilo);
		wi[*ilo] = 0.;
		return 0;
	}

	/* Set rows and columns ILO to IHI to zero below the first subdiagonal. */

	i__1 = *ihi - 2;
	for (j = *ilo; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = j + 2; i__ <= i__2; ++i__) {
			h___ref (i__, j) = 0.;
			/* L30: */
		}
		/* L40: */
	}
	nh = *ihi - *ilo + 1;

	/* Determine the order of the multi-shift QR algorithm to be used.

	   Writing concatenation */
	i__3[0] = 1, a__1[0] = job;
	i__3[1] = 1, a__1[1] = compz;
	s_cat ( (char *) ch__1, a__1, i__3, &c__2, 2);
	ns = NUMlapack_ilaenv (&c__4, "NUMlapack_dhseqr ", ch__1, n, ilo, ihi, &c_n1, 6, 2);
	/* Writing concatenation */
	i__3[0] = 1, a__1[0] = job;
	i__3[1] = 1, a__1[1] = compz;
	s_cat (ch__1, a__1, i__3, &c__2, 2);
	maxb = NUMlapack_ilaenv (&c__8, "NUMlapack_dhseqr ", ch__1, n, ilo, ihi, &c_n1, 6, 2);
	if (ns <= 2 || ns > nh || maxb >= nh) {

		/* Use the standard double-shift algorithm */

		NUMlapack_dlahqr (&wantt, &wantz, n, ilo, ihi, &h__[h_offset], ldh, &wr[1], &wi[1], ilo, ihi,
		                  &z__[z_offset], ldz, info);
		return 0;
	}
	maxb = MAX (3, maxb);
	/* Computing MIN */
	i__1 = MIN (ns, maxb);
	ns = MIN (i__1, 15);

	/* Now 2 < NS <= MAXB < NH.

	   Set machine-dependent constants for the stopping criterion. If norm(H)
	   <= sqrt(OVFL), overflow should not occur. */

	unfl = NUMblas_dlamch ("Safe minimum");
	ovfl = 1. / unfl;
	NUMlapack_dlabad (&unfl, &ovfl);
	ulp = NUMblas_dlamch ("Precision");
	smlnum = unfl * (nh / ulp);

	/* I1 and I2 are the indices of the first row and last column of H to
	   which transformations must be applied. If eigenvalues only are being
	   computed, I1 and I2 are set inside the main loop. */

	if (wantt) {
		i1 = 1;
		i2 = *n;
	}

	/* ITN is the total number of multiple-shift QR iterations allowed. */

	itn = nh * 30;

	/* The main loop begins here. I is the loop index and decreases from IHI
	   to ILO in steps of at most MAXB. Each iteration of the loop works with
	   the active submatrix in rows and columns L to I. Eigenvalues I+1 to IHI
	   have already converged. Either L = ILO or H(L,L-1) is negligible so
	   that the matrix splits. */

	i__ = *ihi;
L50:
	l = *ilo;
	if (i__ < *ilo) {
		goto L170;
	}

	/* Perform multiple-shift QR iterations on rows and columns ILO to I
	   until a submatrix of order at most MAXB splits off at the bottom
	   because a subdiagonal element has become negligible. */

	i__1 = itn;
	for (its = 0; its <= i__1; ++its) {

		/* Look for a single small subdiagonal element. */

		i__2 = l + 1;
		for (k = i__; k >= i__2; --k) {
			tst1 = (d__1 = h___ref (k - 1, k - 1), fabs (d__1)) + (d__2 = h___ref (k, k), fabs (d__2));
			if (tst1 == 0.) {
				i__4 = i__ - l + 1;
				tst1 = NUMlapack_dlanhs ("1", &i__4, &h___ref (l, l), ldh, &work[1]);
			}
			/* Computing MAX */
			d__2 = ulp * tst1;
			if ( (d__1 = h___ref (k, k - 1), fabs (d__1)) <= MAX (d__2, smlnum)) {
				goto L70;
			}
			/* L60: */
		}
L70:
		l = k;
		if (l > *ilo) {

			/* H(L,L-1) is negligible. */

			h___ref (l, l - 1) = 0.;
		}

		/* Exit from loop if a submatrix of order <= MAXB has split off. */

		if (l >= i__ - maxb + 1) {
			goto L160;
		}

		/* Now the active submatrix is in rows and columns L to I. If
		   eigenvalues only are being computed, only the active submatrix
		   need be transformed. */

		if (!wantt) {
			i1 = l;
			i2 = i__;
		}

		if (its == 20 || its == 30) {

			/* Exceptional shifts. */

			i__2 = i__;
			for (ii = i__ - ns + 1; ii <= i__2; ++ii) {
				wr[ii] = ( (d__1 = h___ref (ii, ii - 1), fabs (d__1)) + (d__2 =
				               h___ref (ii, ii), fabs (d__2))) * 1.5;
				wi[ii] = 0.;
				/* L80: */
			}
		} else {

			/* Use eigenvalues of trailing submatrix of order NS as shifts. */

			NUMlapack_dlacpy ("Full", &ns, &ns, &h___ref (i__ - ns + 1, i__ - ns + 1), ldh, s, &c__15);
			NUMlapack_dlahqr (&c_false, &c_false, &ns, &c__1, &ns, s, &c__15, &wr[i__ - ns + 1],
			                  &wi[i__ - ns + 1], &c__1, &ns, &z__[z_offset], ldz, &ierr);
			if (ierr > 0) {

				/* If NUMlapack_dlahqr failed to compute all NS eigenvalues,
				   use the unconverged diagonal elements as the remaining
				   shifts. */

				i__2 = ierr;
				for (ii = 1; ii <= i__2; ++ii) {
					wr[i__ - ns + ii] = s_ref (ii, ii);
					wi[i__ - ns + ii] = 0.;
					/* L90: */
				}
			}
		}

		/* Form the first column of (G-w(1)) (G-w(2)) . . . (G-w(ns)) where G
		   is the Hessenberg submatrix H(L:I,L:I) and w is the vector of
		   shifts (stored in WR and WI). The result is stored in the local
		   array V. */

		v[0] = 1.;
		i__2 = ns + 1;
		for (ii = 2; ii <= i__2; ++ii) {
			v[ii - 1] = 0.;
			/* L100: */
		}
		nv = 1;
		i__2 = i__;
		for (j = i__ - ns + 1; j <= i__2; ++j) {
			if (wi[j] >= 0.) {
				if (wi[j] == 0.) {

					/* real shift */

					i__4 = nv + 1;
					NUMblas_dcopy (&i__4, v, &c__1, vv, &c__1);
					i__4 = nv + 1;
					d__1 = -wr[j];
					NUMblas_dgemv ("No transpose", &i__4, &nv, &c_b10, &h___ref (l, l), ldh, vv, &c__1,
					               &d__1, v, &c__1);
					++nv;
				} else if (wi[j] > 0.) {

					/* complex conjugate pair of shifts */

					i__4 = nv + 1;
					NUMblas_dcopy (&i__4, v, &c__1, vv, &c__1);
					i__4 = nv + 1;
					d__1 = wr[j] * -2.;
					NUMblas_dgemv ("No transpose", &i__4, &nv, &c_b10, &h___ref (l, l), ldh, v, &c__1, &d__1,
					               vv, &c__1);
					i__4 = nv + 1;
					itemp = NUMblas_idamax (&i__4, vv, &c__1);
					/* Computing MAX */
					d__2 = (d__1 = vv[itemp - 1], fabs (d__1));
					temp = 1. / MAX (d__2, smlnum);
					i__4 = nv + 1;
					NUMblas_dscal (&i__4, &temp, vv, &c__1);
					absw = NUMlapack_dlapy2 (&wr[j], &wi[j]);
					temp = temp * absw * absw;
					i__4 = nv + 2;
					i__5 = nv + 1;
					NUMblas_dgemv ("No transpose", &i__4, &i__5, &c_b10, &h___ref (l, l), ldh, vv, &c__1,
					               &temp, v, &c__1);
					nv += 2;
				}

				/* Scale V(1:NV) so that MAX (fabs (V(i))) = 1. If V is zero,
				   reset it to the unit vector. */

				itemp = NUMblas_idamax (&nv, v, &c__1);
				temp = (d__1 = v[itemp - 1], fabs (d__1));
				if (temp == 0.) {
					v[0] = 1.;
					i__4 = nv;
					for (ii = 2; ii <= i__4; ++ii) {
						v[ii - 1] = 0.;
						/* L110: */
					}
				} else {
					temp = MAX (temp, smlnum);
					d__1 = 1. / temp;
					NUMblas_dscal (&nv, &d__1, v, &c__1);
				}
			}
			/* L120: */
		}

		/* Multiple-shift QR step */

		i__2 = i__ - 1;
		for (k = l; k <= i__2; ++k) {

			/* The first iteration of this loop determines a reflection G
			   from the vector V and applies it from left and right to H,
			   thus creating a nonzero bulge below the subdiagonal.

			   Each subsequent iteration determines a reflection G to restore
			   the Hessenberg form in the (K-1)th column, and thus chases the
			   bulge one step toward the bottom of the active submatrix. NR
			   is the order of G.

			   Computing MIN */
			i__4 = ns + 1, i__5 = i__ - k + 1;
			nr = MIN (i__4, i__5);
			if (k > l) {
				NUMblas_dcopy (&nr, &h___ref (k, k - 1), &c__1, v, &c__1);
			}
			NUMlapack_dlarfg (&nr, v, &v[1], &c__1, &tau);
			if (k > l) {
				h___ref (k, k - 1) = v[0];
				i__4 = i__;
				for (ii = k + 1; ii <= i__4; ++ii) {
					h___ref (ii, k - 1) = 0.;
					/* L130: */
				}
			}
			v[0] = 1.;

			/* Apply G from the left to transform the rows of the matrix in
			   columns K to I2. */

			i__4 = i2 - k + 1;
			NUMlapack_dlarfx ("Left", &nr, &i__4, v, &tau, &h___ref (k, k), ldh, &work[1]);

			/* Apply G from the right to transform the columns of the matrix
			   in rows I1 to MIN (K+NR,I).

			   Computing MIN */
			i__5 = k + nr;
			i__4 = MIN (i__5, i__) - i1 + 1;
			NUMlapack_dlarfx ("Right", &i__4, &nr, v, &tau, &h___ref (i1, k), ldh, &work[1]);

			if (wantz) {

				/* Accumulate transformations in the matrix Z */

				NUMlapack_dlarfx ("Right", &nh, &nr, v, &tau, &z___ref (*ilo, k), ldz, &work[1]);
			}
			/* L140: */
		}

		/* L150: */
	}

	/* Failure to converge in remaining number of iterations */

	*info = i__;
	return 0;

L160:

	/* A submatrix of order <= MAXB in rows and columns L to I has split off.
	   Use the double-shift QR algorithm to handle it. */

	NUMlapack_dlahqr (&wantt, &wantz, n, &l, &i__, &h__[h_offset], ldh, &wr[1], &wi[1], ilo, ihi,
	                  &z__[z_offset], ldz, info);
	if (*info > 0) {
		return 0;
	}

	/* Decrement number of remaining iterations, and return to start of the
	   main loop with a new value of I. */

	itn -= its;
	i__ = l - 1;
	goto L50;

L170:
	work[1] = (double) MAX (1, *n);
	return 0;
}								/* NUMlapack_dhseqr */

#undef z___ref
#undef s_ref
#undef h___ref

int NUMlapack_dlabad (double *smal, double *large) {

	if (log10 (*large) > 2e3) {
		*smal = sqrt (*smal);
		*large = sqrt (*large);
	}

	return 0;
}								/* NUMlapack_dlabad */

#define x_ref(a_1,a_2) x[(a_2)*x_dim1 + a_1]
#define y_ref(a_1,a_2) y[(a_2)*y_dim1 + a_1]

int NUMlapack_dlabrd (long *m, long *n, long *nb, double *a, long *lda, double *d__, double *e, double *tauq,
                      double *taup, double *x, long *ldx, double *y, long *ldy) {
	/* Table of constant values */
	static double c_b4 = -1.;
	static double c_b5 = 1.;
	static long c__1 = 1;
	static double c_b16 = 0.;

	/* System generated locals */
	long a_dim1, a_offset, x_dim1, x_offset, y_dim1, y_offset, i__1, i__2, i__3;

	/* Local variables */
	static long i__;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--d__;
	--e;
	--tauq;
	--taup;
	x_dim1 = *ldx;
	x_offset = 1 + x_dim1 * 1;
	x -= x_offset;
	y_dim1 = *ldy;
	y_offset = 1 + y_dim1 * 1;
	y -= y_offset;

	/* Function Body */
	if (*m <= 0 || *n <= 0) {
		return 0;
	}

	if (*m >= *n) {

		/* Reduce to upper bidiagonal form */

		i__1 = *nb;
		for (i__ = 1; i__ <= i__1; ++i__) {

			/* Update A(i:m,i) */

			i__2 = *m - i__ + 1;
			i__3 = i__ - 1;
			NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b4, &a_ref (i__, 1), lda, &y_ref (i__, 1), ldy, &c_b5,
			               &a_ref (i__, i__), &c__1);
			i__2 = *m - i__ + 1;
			i__3 = i__ - 1;
			NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b4, &x_ref (i__, 1), ldx, &a_ref (1, i__), &c__1, &c_b5,
			               &a_ref (i__, i__), &c__1);

			/* Generate reflection Q(i) to annihilate A(i+1:m,i)

			   Computing MIN */
			i__2 = i__ + 1;
			i__3 = *m - i__ + 1;
			NUMlapack_dlarfg (&i__3, &a_ref (i__, i__), &a_ref (MIN (i__2, *m), i__), &c__1, &tauq[i__]);
			d__[i__] = a_ref (i__, i__);
			if (i__ < *n) {
				a_ref (i__, i__) = 1.;

				/* Compute Y(i+1:n,i) */

				i__2 = *m - i__ + 1;
				i__3 = *n - i__;
				NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b5, &a_ref (i__, i__ + 1), lda, &a_ref (i__, i__),
				               &c__1, &c_b16, &y_ref (i__ + 1, i__), &c__1);
				i__2 = *m - i__ + 1;
				i__3 = i__ - 1;
				NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b5, &a_ref (i__, 1), lda, &a_ref (i__, i__), &c__1,
				               &c_b16, &y_ref (1, i__), &c__1);
				i__2 = *n - i__;
				i__3 = i__ - 1;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b4, &y_ref (i__ + 1, 1), ldy, &y_ref (1, i__), &c__1,
				               &c_b5, &y_ref (i__ + 1, i__), &c__1);
				i__2 = *m - i__ + 1;
				i__3 = i__ - 1;
				NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b5, &x_ref (i__, 1), ldx, &a_ref (i__, i__), &c__1,
				               &c_b16, &y_ref (1, i__), &c__1);
				i__2 = i__ - 1;
				i__3 = *n - i__;
				NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b4, &a_ref (1, i__ + 1), lda, &y_ref (1, i__), &c__1,
				               &c_b5, &y_ref (i__ + 1, i__), &c__1);
				i__2 = *n - i__;
				NUMblas_dscal (&i__2, &tauq[i__], &y_ref (i__ + 1, i__), &c__1);

				/* Update A(i,i+1:n) */

				i__2 = *n - i__;
				NUMblas_dgemv ("No transpose", &i__2, &i__, &c_b4, &y_ref (i__ + 1, 1), ldy, &a_ref (i__, 1), lda,
				               &c_b5, &a_ref (i__, i__ + 1), lda);
				i__2 = i__ - 1;
				i__3 = *n - i__;
				NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b4, &a_ref (1, i__ + 1), lda, &x_ref (i__, 1), ldx,
				               &c_b5, &a_ref (i__, i__ + 1), lda);

				/* Generate reflection P(i) to annihilate A(i,i+2:n)

				   Computing MIN */
				i__2 = i__ + 2;
				i__3 = *n - i__;
				NUMlapack_dlarfg (&i__3, &a_ref (i__, i__ + 1), &a_ref (i__, MIN (i__2, *n)), lda, &taup[i__]);
				e[i__] = a_ref (i__, i__ + 1);
				a_ref (i__, i__ + 1) = 1.;

				/* Compute X(i+1:m,i) */

				i__2 = *m - i__;
				i__3 = *n - i__;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b5, &a_ref (i__ + 1, i__ + 1), lda, &a_ref (i__,
				               i__ + 1), lda, &c_b16, &x_ref (i__ + 1, i__), &c__1);
				i__2 = *n - i__;
				NUMblas_dgemv ("Transpose", &i__2, &i__, &c_b5, &y_ref (i__ + 1, 1), ldy, &a_ref (i__, i__ + 1), lda,
				               &c_b16, &x_ref (1, i__), &c__1);
				i__2 = *m - i__;
				NUMblas_dgemv ("No transpose", &i__2, &i__, &c_b4, &a_ref (i__ + 1, 1), lda, &x_ref (1, i__), &c__1,
				               &c_b5, &x_ref (i__ + 1, i__), &c__1);
				i__2 = i__ - 1;
				i__3 = *n - i__;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b5, &a_ref (1, i__ + 1), lda, &a_ref (i__, i__ + 1),
				               lda, &c_b16, &x_ref (1, i__), &c__1);
				i__2 = *m - i__;
				i__3 = i__ - 1;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b4, &x_ref (i__ + 1, 1), ldx, &x_ref (1, i__), &c__1,
				               &c_b5, &x_ref (i__ + 1, i__), &c__1);
				i__2 = *m - i__;
				NUMblas_dscal (&i__2, &taup[i__], &x_ref (i__ + 1, i__), &c__1);
			}
			/* L10: */
		}
	} else {

		/* Reduce to lower bidiagonal form */

		i__1 = *nb;
		for (i__ = 1; i__ <= i__1; ++i__) {

			/* Update A(i,i:n) */

			i__2 = *n - i__ + 1;
			i__3 = i__ - 1;
			NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b4, &y_ref (i__, 1), ldy, &a_ref (i__, 1), lda, &c_b5,
			               &a_ref (i__, i__), lda);
			i__2 = i__ - 1;
			i__3 = *n - i__ + 1;
			NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b4, &a_ref (1, i__), lda, &x_ref (i__, 1), ldx, &c_b5,
			               &a_ref (i__, i__), lda);

			/* Generate reflection P(i) to annihilate A(i,i+1:n)

			   Computing MIN */
			i__2 = i__ + 1;
			i__3 = *n - i__ + 1;
			NUMlapack_dlarfg (&i__3, &a_ref (i__, i__), &a_ref (i__, MIN (i__2, *n)), lda, &taup[i__]);
			d__[i__] = a_ref (i__, i__);
			if (i__ < *m) {
				a_ref (i__, i__) = 1.;

				/* Compute X(i+1:m,i) */

				i__2 = *m - i__;
				i__3 = *n - i__ + 1;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b5, &a_ref (i__ + 1, i__), lda, &a_ref (i__, i__),
				               lda, &c_b16, &x_ref (i__ + 1, i__), &c__1);
				i__2 = *n - i__ + 1;
				i__3 = i__ - 1;
				NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b5, &y_ref (i__, 1), ldy, &a_ref (i__, i__), lda,
				               &c_b16, &x_ref (1, i__), &c__1);
				i__2 = *m - i__;
				i__3 = i__ - 1;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b4, &a_ref (i__ + 1, 1), lda, &x_ref (1, i__), &c__1,
				               &c_b5, &x_ref (i__ + 1, i__), &c__1);
				i__2 = i__ - 1;
				i__3 = *n - i__ + 1;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b5, &a_ref (1, i__), lda, &a_ref (i__, i__), lda,
				               &c_b16, &x_ref (1, i__), &c__1);
				i__2 = *m - i__;
				i__3 = i__ - 1;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b4, &x_ref (i__ + 1, 1), ldx, &x_ref (1, i__), &c__1,
				               &c_b5, &x_ref (i__ + 1, i__), &c__1);
				i__2 = *m - i__;
				NUMblas_dscal (&i__2, &taup[i__], &x_ref (i__ + 1, i__), &c__1);

				/* Update A(i+1:m,i) */

				i__2 = *m - i__;
				i__3 = i__ - 1;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b4, &a_ref (i__ + 1, 1), lda, &y_ref (i__, 1), ldy,
				               &c_b5, &a_ref (i__ + 1, i__), &c__1);
				i__2 = *m - i__;
				NUMblas_dgemv ("No transpose", &i__2, &i__, &c_b4, &x_ref (i__ + 1, 1), ldx, &a_ref (1, i__), &c__1,
				               &c_b5, &a_ref (i__ + 1, i__), &c__1);

				/* Generate reflection Q(i) to annihilate A(i+2:m,i)

				   Computing MIN */
				i__2 = i__ + 2;
				i__3 = *m - i__;
				NUMlapack_dlarfg (&i__3, &a_ref (i__ + 1, i__), &a_ref (MIN (i__2, *m), i__), &c__1,
				                  &tauq[i__]);
				e[i__] = a_ref (i__ + 1, i__);
				a_ref (i__ + 1, i__) = 1.;

				/* Compute Y(i+1:n,i) */

				i__2 = *m - i__;
				i__3 = *n - i__;
				NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b5, &a_ref (i__ + 1, i__ + 1), lda, &a_ref (i__ + 1,
				               i__), &c__1, &c_b16, &y_ref (i__ + 1, i__), &c__1);
				i__2 = *m - i__;
				i__3 = i__ - 1;
				NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b5, &a_ref (i__ + 1, 1), lda, &a_ref (i__ + 1, i__),
				               &c__1, &c_b16, &y_ref (1, i__), &c__1);
				i__2 = *n - i__;
				i__3 = i__ - 1;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b4, &y_ref (i__ + 1, 1), ldy, &y_ref (1, i__), &c__1,
				               &c_b5, &y_ref (i__ + 1, i__), &c__1);
				i__2 = *m - i__;
				NUMblas_dgemv ("Transpose", &i__2, &i__, &c_b5, &x_ref (i__ + 1, 1), ldx, &a_ref (i__ + 1, i__),
				               &c__1, &c_b16, &y_ref (1, i__), &c__1);
				i__2 = *n - i__;
				NUMblas_dgemv ("Transpose", &i__, &i__2, &c_b4, &a_ref (1, i__ + 1), lda, &y_ref (1, i__), &c__1,
				               &c_b5, &y_ref (i__ + 1, i__), &c__1);
				i__2 = *n - i__;
				NUMblas_dscal (&i__2, &tauq[i__], &y_ref (i__ + 1, i__), &c__1);
			}
			/* L20: */
		}
	}
	return 0;
}								/* NUMlapack_dlabrd */

#undef y_ref
#undef x_ref

#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]

int NUMlapack_dlacpy (const char *uplo, long *m, long *n, double *a, long *lda, double *b, long *ldb) {
	/* System generated locals */
	long a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;

	/* Local variables */
	static long i__, j;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1 * 1;
	b -= b_offset;

	/* Function Body */
	if (lsame_ (uplo, "U")) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			i__2 = MIN (j, *m);
			for (i__ = 1; i__ <= i__2; ++i__) {
				b_ref (i__, j) = a_ref (i__, j);
				/* L10: */
			}
			/* L20: */
		}
	} else if (lsame_ (uplo, "L")) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = j; i__ <= i__2; ++i__) {
				b_ref (i__, j) = a_ref (i__, j);
				/* L30: */
			}
			/* L40: */
		}
	} else {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
				b_ref (i__, j) = a_ref (i__, j);
				/* L50: */
			}
			/* L60: */
		}
	}
	return 0;
}								/* NUMlapack_dlacpy */

#undef b_ref


int NUMlapack_dladiv (double *a, double *b, double *c, double *d, double *p, double *q) {
	static double e, f;

	if (fabs (*d) < fabs (*c)) {
		e = *d / *c;
		f = *c + *d * e;
		*p = (*a + *b * e) / f;
		*q = (*b - *a * e) / f;
	} else {
		e = *c / *d;
		f = *d + *c * e;
		*p = (*b + *a * e) / f;
		*q = (- (*a) + *b * e) / f;
	}

	return 0;
}								/* NUMlapack_dladiv */


int NUMlapack_dlae2 (double *a, double *b, double *c__, double *rt1, double *rt2) {
	/* System generated locals */
	double d__1;

	/* Local variables */
	static double acmn, acmx, ab, df, tb, sm, rt, adf;
	sm = *a + *c__;
	df = *a - *c__;
	adf = fabs (df);
	tb = *b + *b;
	ab = fabs (tb);
	// djmw 20110721 changed abs(*a) to fabs(*a)
	if (fabs (*a) > fabs (*c__)) {
		acmx = *a;
		acmn = *c__;
	} else {
		acmx = *c__;
		acmn = *a;
	}
	if (adf > ab) {
		/* Computing 2nd power */
		d__1 = ab / adf;
		rt = adf * sqrt (d__1 * d__1 + 1.);
	} else if (adf < ab) {
		/* Computing 2nd power */
		d__1 = adf / ab;
		rt = ab * sqrt (d__1 * d__1 + 1.);
	} else {

		/* Includes case AB=ADF=0 */

		rt = ab * sqrt (2.);
	}
	if (sm < 0.) {
		*rt1 = (sm - rt) * .5;

		/* Order of execution important. To get fully accurate smaller
		   eigenvalue, next line needs to be executed in higher precision. */

		*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
	} else if (sm > 0.) {
		*rt1 = (sm + rt) * .5;

		/* Order of execution important. To get fully accurate smaller
		   eigenvalue, next line needs to be executed in higher precision. */

		*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
	} else {

		/* Includes case RT1 = RT2 = 0 */

		*rt1 = rt * .5;
		*rt2 = rt * -.5;
	}
	return 0;
}								/* NUMlapack_dlae2 */

int NUMlapack_dlaev2 (double *a, double *b, double *c__, double *rt1, double *rt2, double *cs1, double *sn1) {
	/* System generated locals */
	double d__1;

	/* Local variables */
	static double acmn, acmx, ab, df, cs, ct, tb, sm, tn, rt, adf, acs;
	static long sgn1, sgn2;

	sm = *a + *c__;
	df = *a - *c__;
	adf = fabs (df);
	tb = *b + *b;
	ab = fabs (tb);
	if (fabs (*a) > fabs (*c__)) {
		acmx = *a;
		acmn = *c__;
	} else {
		acmx = *c__;
		acmn = *a;
	}
	if (adf > ab) {
		/* Computing 2nd power */
		d__1 = ab / adf;
		rt = adf * sqrt (d__1 * d__1 + 1.);
	} else if (adf < ab) {
		/* Computing 2nd power */
		d__1 = adf / ab;
		rt = ab * sqrt (d__1 * d__1 + 1.);
	} else {

		/* Includes case AB=ADF=0 */

		rt = ab * sqrt (2.);
	}
	if (sm < 0.) {
		*rt1 = (sm - rt) * .5;
		sgn1 = -1;

		/* Order of execution important. To get fully accurate smaller
		   eigenvalue, next line needs to be executed in higher precision. */

		*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
	} else if (sm > 0.) {
		*rt1 = (sm + rt) * .5;
		sgn1 = 1;

		/* Order of execution important. To get fully accurate smaller
		   eigenvalue, next line needs to be executed in higher precision. */

		*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
	} else {

		/* Includes case RT1 = RT2 = 0 */

		*rt1 = rt * .5;
		*rt2 = rt * -.5;
		sgn1 = 1;
	}

	/* Compute the eigenvector */

	if (df >= 0.) {
		cs = df + rt;
		sgn2 = 1;
	} else {
		cs = df - rt;
		sgn2 = -1;
	}
	acs = fabs (cs);
	if (acs > ab) {
		ct = -tb / cs;
		*sn1 = 1. / sqrt (ct * ct + 1.);
		*cs1 = ct * *sn1;
	} else {
		if (ab == 0.) {
			*cs1 = 1.;
			*sn1 = 0.;
		} else {
			tn = -cs / tb;
			*cs1 = 1. / sqrt (tn * tn + 1.);
			*sn1 = tn * *cs1;
		}
	}
	if (sgn1 == sgn2) {
		tn = *cs1;
		*cs1 = - (*sn1);
		*sn1 = tn;
	}
	return 0;
}								/* NUMlapack_dlaev2 */

int NUMlapack_dlags2 (long *upper, double *a1, double *a2, double *a3, double *b1, double *b2, double *b3,
                      double *csu, double *snu, double *csv, double *snv, double *csq, double *snq) {
	/* System generated locals */
	double d__1;

	/* Local variables */
	static double aua11, aua12, aua21, aua22, avb11, avb12, avb21, avb22;
	double ua11r, ua22r, vb11r, vb22r, a, b, c__, d__, r__, s1, s2;
	static double ua11, ua12, ua21, ua22, vb11, vb12, vb21, vb22, csl, csr, snl, snr;

	if (*upper) {

		/* Input matrices A and B are upper triangular matrices

		   Form matrix C = A*adj(B) = ( a b ) ( 0 d ) */

		a = *a1 * *b3;
		d__ = *a3 * *b1;
		b = *a2 * *b1 - *a1 * *b2;

		/* The SVD of real 2-by-2 triangular C

		   ( CSL -SNL )*( A B )*( CSR SNR ) = ( R 0 ) ( SNL CSL ) ( 0 D ) (
		   -SNR CSR ) ( 0 T ) */

		NUMlapack_dlasv2 (&a, &b, &d__, &s1, &s2, &snr, &csr, &snl, &csl);

		if (fabs (csl) >= fabs (snl) || fabs (csr) >= fabs (snr)) {

			/* Compute the (1,1) and (1,2) elements of U'*A and V'*B, and
			   (1,2) element of |U|'*|A| and |V|'*|B|. */

			ua11r = csl * *a1;
			ua12 = csl * *a2 + snl * *a3;

			vb11r = csr * *b1;
			vb12 = csr * *b2 + snr * *b3;

			aua12 = fabs (csl) * fabs (*a2) + fabs (snl) * fabs (*a3);
			avb12 = fabs (csr) * fabs (*b2) + fabs (snr) * fabs (*b3);

			/* zero (1,2) elements of U'*A and V'*B */

			if (fabs (ua11r) + fabs (ua12) != 0.) {
				if (aua12 / (fabs (ua11r) + fabs (ua12)) <= avb12 / (fabs (vb11r) + fabs (vb12))) {
					d__1 = -ua11r;
					NUMlapack_dlartg (&d__1, &ua12, csq, snq, &r__);
				} else {
					d__1 = -vb11r;
					NUMlapack_dlartg (&d__1, &vb12, csq, snq, &r__);
				}
			} else {
				d__1 = -vb11r;
				NUMlapack_dlartg (&d__1, &vb12, csq, snq, &r__);
			}

			*csu = csl;
			*snu = -snl;
			*csv = csr;
			*snv = -snr;

		} else {

			/* Compute the (2,1) and (2,2) elements of U'*A and V'*B, and
			   (2,2) element of |U|'*|A| and |V|'*|B|. */

			ua21 = -snl * *a1;
			ua22 = -snl * *a2 + csl * *a3;

			vb21 = -snr * *b1;
			vb22 = -snr * *b2 + csr * *b3;

			aua22 = fabs (snl) * fabs (*a2) + fabs (csl) * fabs (*a3);
			avb22 = fabs (snr) * fabs (*b2) + fabs (csr) * fabs (*b3);

			/* zero (2,2) elements of U'*A and V'*B, and then swap. */

			if (fabs (ua21) + fabs (ua22) != 0.) {
				if (aua22 / (fabs (ua21) + fabs (ua22)) <= avb22 / (fabs (vb21) + fabs (vb22))) {
					d__1 = -ua21;
					NUMlapack_dlartg (&d__1, &ua22, csq, snq, &r__);
				} else {
					d__1 = -vb21;
					NUMlapack_dlartg (&d__1, &vb22, csq, snq, &r__);
				}
			} else {
				d__1 = -vb21;
				NUMlapack_dlartg (&d__1, &vb22, csq, snq, &r__);
			}

			*csu = snl;
			*snu = csl;
			*csv = snr;
			*snv = csr;

		}

	} else {

		/* Input matrices A and B are lower triangular matrices Form matrix C
		   = A*adj(B) = ( a 0 ) ( c d ) */

		a = *a1 * *b3;
		d__ = *a3 * *b1;
		c__ = *a2 * *b3 - *a3 * *b2;

		/* The SVD of real 2-by-2 triangular C

		   ( CSL -SNL )*( A 0 )*( CSR SNR ) = ( R 0 ) ( SNL CSL ) ( C D ) (
		   -SNR CSR ) ( 0 T ) */

		NUMlapack_dlasv2 (&a, &c__, &d__, &s1, &s2, &snr, &csr, &snl, &csl);

		if (fabs (csr) >= fabs (snr) || fabs (csl) >= fabs (snl)) {

			/* Compute the (2,1) and (2,2) elements of U'*A and V'*B, and
			   (2,1) element of |U|'*|A| and |V|'*|B|. */

			ua21 = -snr * *a1 + csr * *a2;
			ua22r = csr * *a3;

			vb21 = -snl * *b1 + csl * *b2;
			vb22r = csl * *b3;

			aua21 = fabs (snr) * fabs (*a1) + fabs (csr) * fabs (*a2);
			avb21 = fabs (snl) * fabs (*b1) + fabs (csl) * fabs (*b2);

			/* zero (2,1) elements of U'*A and V'*B. */

			if (fabs (ua21) + fabs (ua22r) != 0.) {
				if (aua21 / (fabs (ua21) + fabs (ua22r)) <= avb21 / (fabs (vb21) + fabs (vb22r))) {
					NUMlapack_dlartg (&ua22r, &ua21, csq, snq, &r__);
				} else {
					NUMlapack_dlartg (&vb22r, &vb21, csq, snq, &r__);
				}
			} else {
				NUMlapack_dlartg (&vb22r, &vb21, csq, snq, &r__);
			}

			*csu = csr;
			*snu = -snr;
			*csv = csl;
			*snv = -snl;

		} else {

			/* Compute the (1,1) and (1,2) elements of U'*A and V'*B, and
			   (1,1) element of |U|'*|A| and |V|'*|B|. */

			ua11 = csr * *a1 + snr * *a2;
			ua12 = snr * *a3;

			vb11 = csl * *b1 + snl * *b2;
			vb12 = snl * *b3;

			aua11 = fabs (csr) * fabs (*a1) + fabs (snr) * fabs (*a2);
			avb11 = fabs (csl) * fabs (*b1) + fabs (snl) * fabs (*b2);

			/* zero (1,1) elements of U'*A and V'*B, and then swap. */

			if (fabs (ua11) + fabs (ua12) != 0.) {
				if (aua11 / (fabs (ua11) + fabs (ua12)) <= avb11 / (fabs (vb11) + fabs (vb12))) {
					NUMlapack_dlartg (&ua12, &ua11, csq, snq, &r__);
				} else {
					NUMlapack_dlartg (&vb12, &vb11, csq, snq, &r__);
				}
			} else {
				NUMlapack_dlartg (&vb12, &vb11, csq, snq, &r__);
			}

			*csu = snr;
			*snu = csr;
			*csv = snl;
			*snv = csl;
		}
	}
	return 0;
}								/* NUMlapack_dlags2 */


int NUMlapack_dlahqr (int *wantt, int *wantz, long *n, long *ilo, long *ihi, double *h__, long *ldh,
                      double *wr, double *wi, long *iloz, long *ihiz, double *z__, long *ldz, long *info) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long h_dim1, h_offset, z_dim1, z_offset, i__1, i__2, i__3, i__4;
	double d__1, d__2;

	/* Local variables */
	static double h43h34, disc, unfl, ovfl;
	static double work[1];
	static long i__, j, k, l, m;
	static double s, v[3];
	static long i1, i2;
	static double t1, t2, t3, v1, v2, v3;
	static double h00, h10, h11, h12, h21, h22, h33, h44;
	static long nh;
	static double cs;
	static long nr;
	static double sn;
	static long nz;
	static double smlnum, ave, h33s, h44s;
	static long itn, its;
	static double ulp, sum, tst1;

#define h___ref(a_1,a_2) h__[(a_2)*h_dim1 + a_1]
#define z___ref(a_1,a_2) z__[(a_2)*z_dim1 + a_1]

	h_dim1 = *ldh;
	h_offset = 1 + h_dim1 * 1;
	h__ -= h_offset;
	--wr;
	--wi;
	z_dim1 = *ldz;
	z_offset = 1 + z_dim1 * 1;
	z__ -= z_offset;

	/* Function Body */
	*info = 0;

	/* Quick return if possible */

	if (*n == 0) {
		return 0;
	}
	if (*ilo == *ihi) {
		wr[*ilo] = h___ref (*ilo, *ilo);
		wi[*ilo] = 0.;
		return 0;
	}

	nh = *ihi - *ilo + 1;
	nz = *ihiz - *iloz + 1;

	/* Set machine-dependent constants for the stopping criterion. If norm(H)
	   <= sqrt(OVFL), overflow should not occur. */

	unfl = NUMblas_dlamch ("Safe minimum");
	ovfl = 1. / unfl;
	NUMlapack_dlabad (&unfl, &ovfl);
	ulp = NUMblas_dlamch ("Precision");
	smlnum = unfl * (nh / ulp);

	/* I1 and I2 are the indices of the first row and last column of H to
	   which transformations must be applied. If eigenvalues only are being
	   computed, I1 and I2 are set inside the main loop. */

	if (*wantt) {
		i1 = 1;
		i2 = *n;
	}

	/* ITN is the total number of QR iterations allowed. */

	itn = nh * 30;

	/* The main loop begins here. I is the loop index and decreases from IHI
	   to ILO in steps of 1 or 2. Each iteration of the loop works with the
	   active submatrix in rows and columns L to I. Eigenvalues I+1 to IHI
	   have already converged. Either L = ILO or H(L,L-1) is negligible so
	   that the matrix splits. */

	i__ = *ihi;
L10:
	l = *ilo;
	if (i__ < *ilo) {
		goto L150;
	}

	/* Perform QR iterations on rows and columns ILO to I until a submatrix
	   of order 1 or 2 splits off at the bottom because a subdiagonal element
	   has become negligible. */

	i__1 = itn;
	for (its = 0; its <= i__1; ++its) {

		/* Look for a single small subdiagonal element. */

		i__2 = l + 1;
		for (k = i__; k >= i__2; --k) {
			tst1 = (d__1 = h___ref (k - 1, k - 1), fabs (d__1)) + (d__2 = h___ref (k, k), fabs (d__2));
			if (tst1 == 0.) {
				i__3 = i__ - l + 1;
				tst1 = NUMlapack_dlanhs ("1", &i__3, &h___ref (l, l), ldh, work);
			}
			/* Computing MAX */
			d__2 = ulp * tst1;
			if ( (d__1 = h___ref (k, k - 1), fabs (d__1)) <= MAX (d__2, smlnum)) {
				goto L30;
			}
			/* L20: */
		}
L30:
		l = k;
		if (l > *ilo) {

			/* H(L,L-1) is negligible */

			h___ref (l, l - 1) = 0.;
		}

		/* Exit from loop if a submatrix of order 1 or 2 has split off. */

		if (l >= i__ - 1) {
			goto L140;
		}

		/* Now the active submatrix is in rows and columns L to I. If
		   eigenvalues only are being computed, only the active submatrix
		   need be transformed. */

		if (! (*wantt)) {
			i1 = l;
			i2 = i__;
		}

		if (its == 10 || its == 20) {

			/* Exceptional shift. */

			s = (d__1 = h___ref (i__, i__ - 1), fabs (d__1)) + (d__2 =
			            h___ref (i__ - 1, i__ - 2), fabs (d__2));
			h44 = s * .75 + h___ref (i__, i__);
			h33 = h44;
			h43h34 = s * -.4375 * s;
		} else {

			/* Prepare to use Francis' double shift (i.e. 2nd degree
			   generalized Rayleigh quotient) */

			h44 = h___ref (i__, i__);
			h33 = h___ref (i__ - 1, i__ - 1);
			h43h34 = h___ref (i__, i__ - 1) * h___ref (i__ - 1, i__);
			s = h___ref (i__ - 1, i__ - 2) * h___ref (i__ - 1, i__ - 2);
			disc = (h33 - h44) * .5;
			disc = disc * disc + h43h34;
			if (disc > 0.) {

				/* Real roots: use Wilkinson's shift twice */

				disc = sqrt (disc);
				ave = (h33 + h44) * .5;
				if (fabs (h33) - fabs (h44) > 0.) {
					h33 = h33 * h44 - h43h34;
					h44 = h33 / (d_sign (&disc, &ave) + ave);
				} else {
					h44 = d_sign (&disc, &ave) + ave;
				}
				h33 = h44;
				h43h34 = 0.;
			}
		}

		/* Look for two consecutive small subdiagonal elements. */

		i__2 = l;
		for (m = i__ - 2; m >= i__2; --m) {
			/* Determine the effect of starting the double-shift QR iteration
			   at row M, and see if this would make H(M,M-1) negligible. */

			h11 = h___ref (m, m);
			h22 = h___ref (m + 1, m + 1);
			h21 = h___ref (m + 1, m);
			h12 = h___ref (m, m + 1);
			h44s = h44 - h11;
			h33s = h33 - h11;
			v1 = (h33s * h44s - h43h34) / h21 + h12;
			v2 = h22 - h11 - h33s - h44s;
			v3 = h___ref (m + 2, m + 1);
			s = fabs (v1) + fabs (v2) + fabs (v3);
			v1 /= s;
			v2 /= s;
			v3 /= s;
			v[0] = v1;
			v[1] = v2;
			v[2] = v3;
			if (m == l) {
				goto L50;
			}
			h00 = h___ref (m - 1, m - 1);
			h10 = h___ref (m, m - 1);
			tst1 = fabs (v1) * (fabs (h00) + fabs (h11) + fabs (h22));
			if (fabs (h10) * (fabs (v2) + fabs (v3)) <= ulp * tst1) {
				goto L50;
			}
			/* L40: */
		}
L50:

		/* Double-shift QR step */

		i__2 = i__ - 1;
		for (k = m; k <= i__2; ++k) {

			/* The first iteration of this loop determines a reflection G
			   from the vector V and applies it from left and right to H,
			   thus creating a nonzero bulge below the subdiagonal.

			   Each subsequent iteration determines a reflection G to restore
			   the Hessenberg form in the (K-1)th column, and thus chases the
			   bulge one step toward the bottom of the active submatrix. NR
			   is the order of G.

			   Computing MIN */
			i__3 = 3, i__4 = i__ - k + 1;
			nr = MIN (i__3, i__4);
			if (k > m) {
				NUMblas_dcopy (&nr, &h___ref (k, k - 1), &c__1, v, &c__1);
			}
			NUMlapack_dlarfg (&nr, v, &v[1], &c__1, &t1);
			if (k > m) {
				h___ref (k, k - 1) = v[0];
				h___ref (k + 1, k - 1) = 0.;
				if (k < i__ - 1) {
					h___ref (k + 2, k - 1) = 0.;
				}
			} else if (m > l) {
				h___ref (k, k - 1) = -h___ref (k, k - 1);
			}
			v2 = v[1];
			t2 = t1 * v2;
			if (nr == 3) {
				v3 = v[2];
				t3 = t1 * v3;

				/* Apply G from the left to transform the rows of the matrix
				   in columns K to I2. */

				i__3 = i2;
				for (j = k; j <= i__3; ++j) {
					sum = h___ref (k, j) + v2 * h___ref (k + 1, j) + v3 * h___ref (k + 2, j);
					h___ref (k, j) = h___ref (k, j) - sum * t1;
					h___ref (k + 1, j) = h___ref (k + 1, j) - sum * t2;
					h___ref (k + 2, j) = h___ref (k + 2, j) - sum * t3;
					/* L60: */
				}

				/* Apply G from the right to transform the columns of the
				   matrix in rows I1 to MIN (K+3,I).

				   Computing MIN */
				i__4 = k + 3;
				i__3 = MIN (i__4, i__);
				for (j = i1; j <= i__3; ++j) {
					sum = h___ref (j, k) + v2 * h___ref (j, k + 1) + v3 * h___ref (j, k + 2);
					h___ref (j, k) = h___ref (j, k) - sum * t1;
					h___ref (j, k + 1) = h___ref (j, k + 1) - sum * t2;
					h___ref (j, k + 2) = h___ref (j, k + 2) - sum * t3;
					/* L70: */
				}

				if (*wantz) {

					/* Accumulate transformations in the matrix Z */

					i__3 = *ihiz;
					for (j = *iloz; j <= i__3; ++j) {
						sum = z___ref (j, k) + v2 * z___ref (j, k + 1) + v3 * z___ref (j, k + 2);
						z___ref (j, k) = z___ref (j, k) - sum * t1;
						z___ref (j, k + 1) = z___ref (j, k + 1) - sum * t2;
						z___ref (j, k + 2) = z___ref (j, k + 2) - sum * t3;
						/* L80: */
					}
				}
			} else if (nr == 2) {

				/* Apply G from the left to transform the rows of the matrix
				   in columns K to I2. */

				i__3 = i2;
				for (j = k; j <= i__3; ++j) {
					sum = h___ref (k, j) + v2 * h___ref (k + 1, j);
					h___ref (k, j) = h___ref (k, j) - sum * t1;
					h___ref (k + 1, j) = h___ref (k + 1, j) - sum * t2;
					/* L90: */
				}

				/* Apply G from the right to transform the columns of the
				   matrix in rows I1 to MIN (K+3,I). */

				i__3 = i__;
				for (j = i1; j <= i__3; ++j) {
					sum = h___ref (j, k) + v2 * h___ref (j, k + 1);
					h___ref (j, k) = h___ref (j, k) - sum * t1;
					h___ref (j, k + 1) = h___ref (j, k + 1) - sum * t2;
					/* L100: */
				}

				if (*wantz) {

					/* Accumulate transformations in the matrix Z */

					i__3 = *ihiz;
					for (j = *iloz; j <= i__3; ++j) {
						sum = z___ref (j, k) + v2 * z___ref (j, k + 1);
						z___ref (j, k) = z___ref (j, k) - sum * t1;
						z___ref (j, k + 1) = z___ref (j, k + 1) - sum * t2;
						/* L110: */
					}
				}
			}
			/* L120: */
		}

		/* L130: */
	}

	/* Failure to converge in remaining number of iterations */

	*info = i__;
	return 0;

L140:

	if (l == i__) {

		/* H(I,I-1) is negligible: one eigenvalue has converged. */

		wr[i__] = h___ref (i__, i__);
		wi[i__] = 0.;
	} else if (l == i__ - 1) {

		/* H(I-1,I-2) is negligible: a pair of eigenvalues have converged.

		   Transform the 2-by-2 submatrix to standard Schur form, and compute
		   and store the eigenvalues. */

		NUMlapack_dlanv2 (&h___ref (i__ - 1, i__ - 1), &h___ref (i__ - 1, i__), &h___ref (i__, i__ - 1),
		                  &h___ref (i__, i__), &wr[i__ - 1], &wi[i__ - 1], &wr[i__], &wi[i__], &cs, &sn);

		if (*wantt) {

			/* Apply the transformation to the rest of H. */

			if (i2 > i__) {
				i__1 = i2 - i__;
				NUMblas_drot (&i__1, &h___ref (i__ - 1, i__ + 1), ldh, &h___ref (i__, i__ + 1), ldh, &cs,
				              &sn);
			}
			i__1 = i__ - i1 - 1;
			NUMblas_drot (&i__1, &h___ref (i1, i__ - 1), &c__1, &h___ref (i1, i__), &c__1, &cs, &sn);
		}
		if (*wantz) {

			/* Apply the transformation to Z. */

			NUMblas_drot (&nz, &z___ref (*iloz, i__ - 1), &c__1, &z___ref (*iloz, i__), &c__1, &cs, &sn);
		}
	}

	/* Decrement number of remaining iterations, and return to start of the
	   main loop with new value of I. */

	itn -= its;
	i__ = l - 1;
	goto L10;

L150:
	return 0;
}								/* NUMlapack_dlahqr */

#undef z___ref
#undef h___ref


int NUMlapack_dlahrd (long *n, long *k, long *nb, double *a, long *lda, double *tau, double *t, long *ldt,
                      double *y, long *ldy) {
	/* Table of constant values */
	static double c_b4 = -1.;
	static double c_b5 = 1.;
	static long c__1 = 1;
	static double c_b38 = 0.;

	/* System generated locals */
	long a_dim1, a_offset, t_dim1, t_offset, y_dim1, y_offset, i__1, i__2, i__3;
	double d__1;

	/* Local variables */
	static long i__;
	static double ei;

#define t_ref(a_1,a_2) t[(a_2)*t_dim1 + a_1]
#define y_ref(a_1,a_2) y[(a_2)*y_dim1 + a_1]

	--tau;
	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	t_dim1 = *ldt;
	t_offset = 1 + t_dim1 * 1;
	t -= t_offset;
	y_dim1 = *ldy;
	y_offset = 1 + y_dim1 * 1;
	y -= y_offset;

	/* Function Body */
	if (*n <= 1) {
		return 0;
	}

	i__1 = *nb;
	for (i__ = 1; i__ <= i__1; ++i__) {
		if (i__ > 1) {

			/* Update A(1:n,i)

			   Compute i-th column of A - Y * V' */

			i__2 = i__ - 1;
			NUMblas_dgemv ("No transpose", n, &i__2, &c_b4, &y[y_offset], ldy, &a_ref (*k + i__ - 1, 1), lda,
			               &c_b5, &a_ref (1, i__), &c__1);

			/* Apply I - V * T' * V' to this column (call it b) from the
			   left, using the last column of T as workspace

			   Let V = ( V1 ) and b = ( b1 ) (first I-1 rows) ( V2 ) ( b2 )

			   where V1 is unit lower triangular

			   w := V1' * b1 */

			i__2 = i__ - 1;
			NUMblas_dcopy (&i__2, &a_ref (*k + 1, i__), &c__1, &t_ref (1, *nb), &c__1);
			i__2 = i__ - 1;
			NUMblas_dtrmv ("Lower", "Transpose", "Unit", &i__2, &a_ref (*k + 1, 1), lda, &t_ref (1, *nb),
			               &c__1);

			/* w := w + V2'*b2 */

			i__2 = *n - *k - i__ + 1;
			i__3 = i__ - 1;
			NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b5, &a_ref (*k + i__, 1), lda, &a_ref (*k + i__,
			               i__), &c__1, &c_b5, &t_ref (1, *nb), &c__1);

			/* w := T'*w */

			i__2 = i__ - 1;
			NUMblas_dtrmv ("Upper", "Transpose", "Non-unit", &i__2, &t[t_offset], ldt, &t_ref (1, *nb),
			               &c__1);

			/* b2 := b2 - V2*w */

			i__2 = *n - *k - i__ + 1;
			i__3 = i__ - 1;
			NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b4, &a_ref (*k + i__, 1), lda, &t_ref (1, *nb),
			               &c__1, &c_b5, &a_ref (*k + i__, i__), &c__1);

			/* b1 := b1 - V1*w */

			i__2 = i__ - 1;
			NUMblas_dtrmv ("Lower", "No transpose", "Unit", &i__2, &a_ref (*k + 1, 1), lda, &t_ref (1, *nb),
			               &c__1);
			i__2 = i__ - 1;
			NUMblas_daxpy (&i__2, &c_b4, &t_ref (1, *nb), &c__1, &a_ref (*k + 1, i__), &c__1);

			a_ref (*k + i__ - 1, i__ - 1) = ei;
		}

		/* Generate the elementary reflector H(i) to annihilate A(k+i+1:n,i)
		   Computing MIN */
		i__2 = *k + i__ + 1;
		i__3 = *n - *k - i__ + 1;
		NUMlapack_dlarfg (&i__3, &a_ref (*k + i__, i__), &a_ref (MIN (i__2, *n), i__), &c__1, &tau[i__]);
		ei = a_ref (*k + i__, i__);
		a_ref (*k + i__, i__) = 1.;

		/* Compute Y(1:n,i) */

		i__2 = *n - *k - i__ + 1;
		NUMblas_dgemv ("No transpose", n, &i__2, &c_b5, &a_ref (1, i__ + 1), lda, &a_ref (*k + i__, i__),
		               &c__1, &c_b38, &y_ref (1, i__), &c__1);
		i__2 = *n - *k - i__ + 1;
		i__3 = i__ - 1;
		NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b5, &a_ref (*k + i__, 1), lda, &a_ref (*k + i__, i__),
		               &c__1, &c_b38, &t_ref (1, i__), &c__1);
		i__2 = i__ - 1;
		NUMblas_dgemv ("No transpose", n, &i__2, &c_b4, &y[y_offset], ldy, &t_ref (1, i__), &c__1, &c_b5,
		               &y_ref (1, i__), &c__1);
		NUMblas_dscal (n, &tau[i__], &y_ref (1, i__), &c__1);

		/* Compute T(1:i,i) */

		i__2 = i__ - 1;
		d__1 = -tau[i__];
		NUMblas_dscal (&i__2, &d__1, &t_ref (1, i__), &c__1);
		i__2 = i__ - 1;
		NUMblas_dtrmv ("Upper", "No transpose", "Non-unit", &i__2, &t[t_offset], ldt, &t_ref (1, i__),
		               &c__1);
		t_ref (i__, i__) = tau[i__];

		/* L10: */
	}
	a_ref (*k + *nb, *nb) = ei;

	return 0;
}								/* NUMlapack_dlahrd */

#undef y_ref
#undef t_ref


int NUMlapack_dlaln2 (int *ltrans, long *na, long *nw, double *smin, double *ca, double *a, long *lda,
                      double *d1, double *d2, double *b, long *ldb, double *wr, double *wi, double *x, long *ldx, double *scale,
                      double *xnorm, long *info) {
	/* Initialized data */
	static int zswap[4] = { FALSE, FALSE, TRUE, TRUE };
	static int rswap[4] = { FALSE, TRUE, FALSE, TRUE };
	static long ipivot[16] /* was [4][4] */  = { 1, 2, 3, 4, 2, 1, 4, 3, 3, 4, 1, 2,
	        4, 3, 2, 1
	                                           };
	/* System generated locals */
	long a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset;
	double d__1, d__2, d__3, d__4, d__5, d__6;
	static double equiv_0[4], equiv_1[4];

	/* Local variables */
	static double bbnd, cmax, ui11r, ui12s, temp, ur11r, ur12s;
	static long j;
	static double u22abs;
	static long icmax;
	static double bnorm, cnorm, smini;

#define ci (equiv_0)
#define cr (equiv_1)
	static double bignum, bi1, bi2, br1, br2, smlnum, xi1, xi2, xr1, xr2, ci21, ci22, cr21, cr22, li21, csi,
	       ui11, lr21, ui12, ui22;
#define civ (equiv_0)
	static double csr, ur11, ur12, ur22;

#define crv (equiv_1)
#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]
#define x_ref(a_1,a_2) x[(a_2)*x_dim1 + a_1]
#define ci_ref(a_1,a_2) ci[(a_2)*2 + a_1 - 3]
#define cr_ref(a_1,a_2) cr[(a_2)*2 + a_1 - 3]
#define ipivot_ref(a_1,a_2) ipivot[(a_2)*4 + a_1 - 5]

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1 * 1;
	b -= b_offset;
	x_dim1 = *ldx;
	x_offset = 1 + x_dim1 * 1;
	x -= x_offset;

	/* Function Body

	   Compute BIGNUM */

	smlnum = 2. * NUMblas_dlamch ("Safe minimum");
	bignum = 1. / smlnum;
	smini = MAX (*smin, smlnum);

	/* Don't check for input errors */

	*info = 0;

	/* Standard Initializations */

	*scale = 1.;

	if (*na == 1) {

		/* 1 x 1 (i.e., scalar) system C X = B */

		if (*nw == 1) {

			/* Real 1x1 system.

			   C = ca A - w D */

			csr = *ca * a_ref (1, 1) - *wr * *d1;
			cnorm = fabs (csr);

			/* If | C | < SMINI, use C = SMINI */

			if (cnorm < smini) {
				csr = smini;
				cnorm = smini;
				*info = 1;
			}

			/* Check scaling for X = B / C */

			bnorm = (d__1 = b_ref (1, 1), fabs (d__1));
			if (cnorm < 1. && bnorm > 1.) {
				if (bnorm > bignum * cnorm) {
					*scale = 1. / bnorm;
				}
			}

			/* Compute X */

			x_ref (1, 1) = b_ref (1, 1) * *scale / csr;
			*xnorm = (d__1 = x_ref (1, 1), fabs (d__1));
		} else {

			/* Complex 1x1 system (w is complex)

			   C = ca A - w D */

			csr = *ca * a_ref (1, 1) - *wr * *d1;
			csi = - (*wi) * *d1;
			cnorm = fabs (csr) + fabs (csi);

			/* If | C | < SMINI, use C = SMINI */

			if (cnorm < smini) {
				csr = smini;
				csi = 0.;
				cnorm = smini;
				*info = 1;
			}

			/* Check scaling for X = B / C */

			bnorm = (d__1 = b_ref (1, 1), fabs (d__1)) + (d__2 = b_ref (1, 2), fabs (d__2));
			if (cnorm < 1. && bnorm > 1.) {
				if (bnorm > bignum * cnorm) {
					*scale = 1. / bnorm;
				}
			}

			/* Compute X */

			d__1 = *scale * b_ref (1, 1);
			d__2 = *scale * b_ref (1, 2);
			NUMlapack_dladiv (&d__1, &d__2, &csr, &csi, &x_ref (1, 1), &x_ref (1, 2));
			*xnorm = (d__1 = x_ref (1, 1), fabs (d__1)) + (d__2 = x_ref (1, 2), fabs (d__2));
		}

	} else {

		/* 2x2 System

		   Compute the real part of C = ca A - w D (or ca A' - w D ) */

		cr_ref (1, 1) = *ca * a_ref (1, 1) - *wr * *d1;
		cr_ref (2, 2) = *ca * a_ref (2, 2) - *wr * *d2;
		if (*ltrans) {
			cr_ref (1, 2) = *ca * a_ref (2, 1);
			cr_ref (2, 1) = *ca * a_ref (1, 2);
		} else {
			cr_ref (2, 1) = *ca * a_ref (2, 1);
			cr_ref (1, 2) = *ca * a_ref (1, 2);
		}

		if (*nw == 1) {

			/* Real 2x2 system (w is real)

			   Find the largest element in C */

			cmax = 0.;
			icmax = 0;

			for (j = 1; j <= 4; ++j) {
				if ( (d__1 = crv[j - 1], fabs (d__1)) > cmax) {
					cmax = (d__1 = crv[j - 1], fabs (d__1));
					icmax = j;
				}
				/* L10: */
			}

			/* If norm(C) < SMINI, use SMINI*identity. */

			if (cmax < smini) {
				/* Computing MAX */
				d__3 = (d__1 = b_ref (1, 1), fabs (d__1)), d__4 = (d__2 = b_ref (2, 1), fabs (d__2));
				bnorm = MAX (d__3, d__4);
				if (smini < 1. && bnorm > 1.) {
					if (bnorm > bignum * smini) {
						*scale = 1. / bnorm;
					}
				}
				temp = *scale / smini;
				x_ref (1, 1) = temp * b_ref (1, 1);
				x_ref (2, 1) = temp * b_ref (2, 1);
				*xnorm = temp * bnorm;
				*info = 1;
				return 0;
			}

			/* Gaussian elimination with complete pivoting. */

			ur11 = crv[icmax - 1];
			cr21 = crv[ipivot_ref (2, icmax) - 1];
			ur12 = crv[ipivot_ref (3, icmax) - 1];
			cr22 = crv[ipivot_ref (4, icmax) - 1];
			ur11r = 1. / ur11;
			lr21 = ur11r * cr21;
			ur22 = cr22 - ur12 * lr21;

			/* If smaller pivot < SMINI, use SMINI */

			if (fabs (ur22) < smini) {
				ur22 = smini;
				*info = 1;
			}
			if (rswap[icmax - 1]) {
				br1 = b_ref (2, 1);
				br2 = b_ref (1, 1);
			} else {
				br1 = b_ref (1, 1);
				br2 = b_ref (2, 1);
			}
			br2 -= lr21 * br1;
			/* Computing MAX */
			d__2 = (d__1 = br1 * (ur22 * ur11r), fabs (d__1)), d__3 = fabs (br2);
			bbnd = MAX (d__2, d__3);
			if (bbnd > 1. && fabs (ur22) < 1.) {
				if (bbnd >= bignum * fabs (ur22)) {
					*scale = 1. / bbnd;
				}
			}

			xr2 = br2 * *scale / ur22;
			xr1 = *scale * br1 * ur11r - xr2 * (ur11r * ur12);
			if (zswap[icmax - 1]) {
				x_ref (1, 1) = xr2;
				x_ref (2, 1) = xr1;
			} else {
				x_ref (1, 1) = xr1;
				x_ref (2, 1) = xr2;
			}
			/* Computing MAX */
			d__1 = fabs (xr1), d__2 = fabs (xr2);
			*xnorm = MAX (d__1, d__2);

			/* Further scaling if norm(A) norm(X) > overflow */

			if (*xnorm > 1. && cmax > 1.) {
				if (*xnorm > bignum / cmax) {
					temp = cmax / bignum;
					x_ref (1, 1) = temp * x_ref (1, 1);
					x_ref (2, 1) = temp * x_ref (2, 1);
					*xnorm = temp * *xnorm;
					*scale = temp * *scale;
				}
			}
		} else {

			/* Complex 2x2 system (w is complex)

			   Find the largest element in C */

			ci_ref (1, 1) = - (*wi) * *d1;
			ci_ref (2, 1) = 0.;
			ci_ref (1, 2) = 0.;
			ci_ref (2, 2) = - (*wi) * *d2;
			cmax = 0.;
			icmax = 0;

			for (j = 1; j <= 4; ++j) {
				if ( (d__1 = crv[j - 1], fabs (d__1)) + (d__2 = civ[j - 1], fabs (d__2)) > cmax) {
					cmax = (d__1 = crv[j - 1], fabs (d__1)) + (d__2 = civ[j - 1], fabs (d__2));
					icmax = j;
				}
				/* L20: */
			}

			/* If norm(C) < SMINI, use SMINI*identity. */

			if (cmax < smini) {
				/* Computing MAX */
				d__5 = (d__1 = b_ref (1, 1), fabs (d__1)) + (d__2 = b_ref (1, 2), fabs (d__2)), d__6 = (d__3 =
				            b_ref (2, 1), fabs (d__3)) + (d__4 = b_ref (2, 2), fabs (d__4));
				bnorm = MAX (d__5, d__6);
				if (smini < 1. && bnorm > 1.) {
					if (bnorm > bignum * smini) {
						*scale = 1. / bnorm;
					}
				}
				temp = *scale / smini;
				x_ref (1, 1) = temp * b_ref (1, 1);
				x_ref (2, 1) = temp * b_ref (2, 1);
				x_ref (1, 2) = temp * b_ref (1, 2);
				x_ref (2, 2) = temp * b_ref (2, 2);
				*xnorm = temp * bnorm;
				*info = 1;
				return 0;
			}

			/* Gaussian elimination with complete pivoting. */

			ur11 = crv[icmax - 1];
			ui11 = civ[icmax - 1];
			cr21 = crv[ipivot_ref (2, icmax) - 1];
			ci21 = civ[ipivot_ref (2, icmax) - 1];
			ur12 = crv[ipivot_ref (3, icmax) - 1];
			ui12 = civ[ipivot_ref (3, icmax) - 1];
			cr22 = crv[ipivot_ref (4, icmax) - 1];
			ci22 = civ[ipivot_ref (4, icmax) - 1];
			if (icmax == 1 || icmax == 4) {

				/* Code when off-diagonals of pivoted C are real */

				if (fabs (ur11) > fabs (ui11)) {
					temp = ui11 / ur11;
					/* Computing 2nd power */
					d__1 = temp;
					ur11r = 1. / (ur11 * (d__1 * d__1 + 1.));
					ui11r = -temp * ur11r;
				} else {
					temp = ur11 / ui11;
					/* Computing 2nd power */
					d__1 = temp;
					ui11r = -1. / (ui11 * (d__1 * d__1 + 1.));
					ur11r = -temp * ui11r;
				}
				lr21 = cr21 * ur11r;
				li21 = cr21 * ui11r;
				ur12s = ur12 * ur11r;
				ui12s = ur12 * ui11r;
				ur22 = cr22 - ur12 * lr21;
				ui22 = ci22 - ur12 * li21;
			} else {

				/* Code when diagonals of pivoted C are real */

				ur11r = 1. / ur11;
				ui11r = 0.;
				lr21 = cr21 * ur11r;
				li21 = ci21 * ur11r;
				ur12s = ur12 * ur11r;
				ui12s = ui12 * ur11r;
				ur22 = cr22 - ur12 * lr21 + ui12 * li21;
				ui22 = -ur12 * li21 - ui12 * lr21;
			}
			u22abs = fabs (ur22) + fabs (ui22);

			/* If smaller pivot < SMINI, use SMINI */

			if (u22abs < smini) {
				ur22 = smini;
				ui22 = 0.;
				*info = 1;
			}
			if (rswap[icmax - 1]) {
				br2 = b_ref (1, 1);
				br1 = b_ref (2, 1);
				bi2 = b_ref (1, 2);
				bi1 = b_ref (2, 2);
			} else {
				br1 = b_ref (1, 1);
				br2 = b_ref (2, 1);
				bi1 = b_ref (1, 2);
				bi2 = b_ref (2, 2);
			}
			br2 = br2 - lr21 * br1 + li21 * bi1;
			bi2 = bi2 - li21 * br1 - lr21 * bi1;
			/* Computing MAX */
			// djmw 20110721 changed abs(br2) to fabs(br2)
			d__1 = (fabs (br1) + fabs (bi1)) * (u22abs * (fabs (ur11r) + fabs (ui11r))), d__2 =
			           fabs (br2) + fabs (bi2);
			bbnd = MAX (d__1, d__2);
			if (bbnd > 1. && u22abs < 1.) {
				if (bbnd >= bignum * u22abs) {
					*scale = 1. / bbnd;
					br1 = *scale * br1;
					bi1 = *scale * bi1;
					br2 = *scale * br2;
					bi2 = *scale * bi2;
				}
			}

			NUMlapack_dladiv (&br2, &bi2, &ur22, &ui22, &xr2, &xi2);
			xr1 = ur11r * br1 - ui11r * bi1 - ur12s * xr2 + ui12s * xi2;
			xi1 = ui11r * br1 + ur11r * bi1 - ui12s * xr2 - ur12s * xi2;
			if (zswap[icmax - 1]) {
				x_ref (1, 1) = xr2;
				x_ref (2, 1) = xr1;
				x_ref (1, 2) = xi2;
				x_ref (2, 2) = xi1;
			} else {
				x_ref (1, 1) = xr1;
				x_ref (2, 1) = xr2;
				x_ref (1, 2) = xi1;
				x_ref (2, 2) = xi2;
			}
			/* Computing MAX */
			d__1 = fabs (xr1) + fabs (xi1), d__2 = fabs (xr2) + fabs (xi2);
			*xnorm = MAX (d__1, d__2);

			/* Further scaling if norm(A) norm(X) > overflow */

			if (*xnorm > 1. && cmax > 1.) {
				if (*xnorm > bignum / cmax) {
					temp = cmax / bignum;
					x_ref (1, 1) = temp * x_ref (1, 1);
					x_ref (2, 1) = temp * x_ref (2, 1);
					x_ref (1, 2) = temp * x_ref (1, 2);
					x_ref (2, 2) = temp * x_ref (2, 2);
					*xnorm = temp * *xnorm;
					*scale = temp * *scale;
				}
			}
		}
	}

	return 0;
}								/* NUMlapack_NUMlapack_dlaln2 */

#undef ipivot_ref
#undef cr_ref
#undef ci_ref
#undef x_ref
#undef b_ref
#undef crv
#undef civ
#undef cr
#undef ci


double NUMlapack_dlange (const char *norm, long *m, long *n, double *a, long *lda, double *work) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2;
	double ret_val, d__1, d__2, d__3;

	/* Local variables */
	static long i__, j;
	static double scale;
	static double value;
	static double sum;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--work;

	/* Function Body */
	if (MIN (*m, *n) == 0) {
		value = 0.;
	} else if (lsame_ (norm, "M")) {

		/* Find MAX(abs(A(i,j))). */

		value = 0.;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
				/* Computing MAX */
				d__2 = value, d__3 = (d__1 = a_ref (i__, j), fabs (d__1));
				value = MAX (d__2, d__3);
				/* L10: */
			}
			/* L20: */
		}
	} else if (lsame_ (norm, "O") || * (unsigned char *) norm == '1') {

		/* Find norm1(A). */

		value = 0.;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			sum = 0.;
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
				sum += (d__1 = a_ref (i__, j), fabs (d__1));
				/* L30: */
			}
			value = MAX (value, sum);
			/* L40: */
		}
	} else if (lsame_ (norm, "I")) {

		/* Find normI(A). */

		i__1 = *m;
		for (i__ = 1; i__ <= i__1; ++i__) {
			work[i__] = 0.;
			/* L50: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
				work[i__] += (d__1 = a_ref (i__, j), fabs (d__1));
				/* L60: */
			}
			/* L70: */
		}
		value = 0.;
		i__1 = *m;
		for (i__ = 1; i__ <= i__1; ++i__) {
			/* Computing MAX */
			d__1 = value, d__2 = work[i__];
			value = MAX (d__1, d__2);
			/* L80: */
		}
	} else if (lsame_ (norm, "F") || lsame_ (norm, "E")) {

		/* Find normF(A). */

		scale = 0.;
		sum = 1.;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			NUMlapack_dlassq (m, &a_ref (1, j), &c__1, &scale, &sum);
			/* L90: */
		}
		value = scale * sqrt (sum);
	}

	ret_val = value;
	return ret_val;
}								/* NUMlapack_dlange */


double NUMlapack_dlanhs (const char *norm, long *n, double *a, long *lda, double *work) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3, i__4;
	double ret_val, d__1, d__2, d__3;

	/* Local variables */
	static long i__, j;
	static double scale;
	static double value;
	static double sum;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--work;

	/* Function Body */
	if (*n == 0) {
		value = 0.;
	} else if (lsame_ (norm, "M")) {

		/* Find MAX (fabs (A(i,j))). */

		value = 0.;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			/* Computing MIN */
			i__3 = *n, i__4 = j + 1;
			i__2 = MIN (i__3, i__4);
			for (i__ = 1; i__ <= i__2; ++i__) {
				/* Computing MAX */
				d__2 = value, d__3 = (d__1 = a_ref (i__, j), fabs (d__1));
				value = MAX (d__2, d__3);
				/* L10: */
			}
			/* L20: */
		}
	} else if (lsame_ (norm, "O") || * (unsigned char *) norm == '1') {

		/* Find norm1(A). */

		value = 0.;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			sum = 0.;
			/* Computing MIN */
			i__3 = *n, i__4 = j + 1;
			i__2 = MIN (i__3, i__4);
			for (i__ = 1; i__ <= i__2; ++i__) {
				sum += (d__1 = a_ref (i__, j), fabs (d__1));
				/* L30: */
			}
			value = MAX (value, sum);
			/* L40: */
		}
	} else if (lsame_ (norm, "I")) {

		/* Find normI(A). */

		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
			work[i__] = 0.;
			/* L50: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			/* Computing MIN */
			i__3 = *n, i__4 = j + 1;
			i__2 = MIN (i__3, i__4);
			for (i__ = 1; i__ <= i__2; ++i__) {
				work[i__] += (d__1 = a_ref (i__, j), fabs (d__1));
				/* L60: */
			}
			/* L70: */
		}
		value = 0.;
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
			/* Computing MAX */
			d__1 = value, d__2 = work[i__];
			value = MAX (d__1, d__2);
			/* L80: */
		}
	} else if (lsame_ (norm, "F") || lsame_ (norm, "E")) {

		/* Find normF(A). */

		scale = 0.;
		sum = 1.;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			/* Computing MIN */
			i__3 = *n, i__4 = j + 1;
			i__2 = MIN (i__3, i__4);
			NUMlapack_dlassq (&i__2, &a_ref (1, j), &c__1, &scale, &sum);
			/* L90: */
		}
		value = scale * sqrt (sum);
	}

	ret_val = value;
	return ret_val;
}								/* NUMlapack_dlanhs */


double NUMlapack_dlanst (const char *norm, long *n, double *d__, double *e) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long i__1;
	double ret_val, d__1, d__2, d__3, d__4, d__5;
	static long i__;
	static double scale;
	static double anorm;
	static double sum;

	--e;
	--d__;

	/* Function Body */
	if (*n <= 0) {
		anorm = 0.;
	} else if (lsame_ (norm, "M")) {

		/* Find max(abs(A(i,j))). */

		anorm = (d__1 = d__[*n], fabs (d__1));
		i__1 = *n - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {
			/* Computing MAX */
			d__2 = anorm, d__3 = (d__1 = d__[i__], fabs (d__1));
			anorm = MAX (d__2, d__3);
			/* Computing MAX */
			d__2 = anorm, d__3 = (d__1 = e[i__], fabs (d__1));
			anorm = MAX (d__2, d__3);
			/* L10: */
		}
	} else if (lsame_ (norm, "O") || * (unsigned char *) norm == '1' || lsame_ (norm, "I")) {

		/* Find norm1(A). */

		if (*n == 1) {
			anorm = fabs (d__[1]);
		} else {
			/* Computing MAX */
			d__3 = fabs (d__[1]) + fabs (e[1]), d__4 = (d__1 = e[*n - 1], fabs (d__1)) + (d__2 =
			            d__[*n], fabs (d__2));
			anorm = MAX (d__3, d__4);
			i__1 = *n - 1;
			for (i__ = 2; i__ <= i__1; ++i__) {
				/* Computing MAX */
				d__4 = anorm, d__5 = (d__1 = d__[i__], fabs (d__1)) + (d__2 = e[i__], fabs (d__2)) + (d__3 =
				                         e[i__ - 1], fabs (d__3));
				anorm = MAX (d__4, d__5);
				/* L20: */
			}
		}
	} else if (lsame_ (norm, "F") || lsame_ (norm, "E")) {

		/* Find normF(A). */

		scale = 0.;
		sum = 1.;
		if (*n > 1) {
			i__1 = *n - 1;
			NUMlapack_dlassq (&i__1, &e[1], &c__1, &scale, &sum);
			sum *= 2;
		}
		NUMlapack_dlassq (n, &d__[1], &c__1, &scale, &sum);
		anorm = scale * sqrt (sum);
	}

	ret_val = anorm;
	return ret_val;
}								/* NUMlapack_dlanst */

double NUMlapack_dlansy (const char *norm, const char *uplo, long *n, double *a, long *lda, double *work) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2;
	double ret_val, d__1, d__2, d__3;

	/* Local variables */
	static double absa;
	static long i__, j;
	static double scale;
	static double value;
	static double sum;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--work;

	/* Function Body */
	if (*n == 0) {
		value = 0.;
	} else if (lsame_ (norm, "M")) {

		/* Find max(abs(A(i,j))). */

		value = 0.;
		if (lsame_ (uplo, "U")) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = j;
				for (i__ = 1; i__ <= i__2; ++i__) {
					/* Computing MAX */
					d__2 = value, d__3 = (d__1 = a_ref (i__, j), fabs (d__1));
					value = MAX (d__2, d__3);
					/* L10: */
				}
				/* L20: */
			}
		} else {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *n;
				for (i__ = j; i__ <= i__2; ++i__) {
					/* Computing MAX */
					d__2 = value, d__3 = (d__1 = a_ref (i__, j), fabs (d__1));
					value = MAX (d__2, d__3);
					/* L30: */
				}
				/* L40: */
			}
		}
	} else if (lsame_ (norm, "I") || lsame_ (norm, "O") || * (unsigned char *) norm == '1') {

		/* Find normI(A) ( = norm1(A), since A is symmetric). */

		value = 0.;
		if (lsame_ (uplo, "U")) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				sum = 0.;
				i__2 = j - 1;
				for (i__ = 1; i__ <= i__2; ++i__) {
					absa = (d__1 = a_ref (i__, j), fabs (d__1));
					sum += absa;
					work[i__] += absa;
					/* L50: */
				}
				work[j] = sum + (d__1 = a_ref (j, j), fabs (d__1));
				/* L60: */
			}
			i__1 = *n;
			for (i__ = 1; i__ <= i__1; ++i__) {
				/* Computing MAX */
				d__1 = value, d__2 = work[i__];
				value = MAX (d__1, d__2);
				/* L70: */
			}
		} else {
			i__1 = *n;
			for (i__ = 1; i__ <= i__1; ++i__) {
				work[i__] = 0.;
				/* L80: */
			}
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				sum = work[j] + (d__1 = a_ref (j, j), fabs (d__1));
				i__2 = *n;
				for (i__ = j + 1; i__ <= i__2; ++i__) {
					absa = (d__1 = a_ref (i__, j), fabs (d__1));
					sum += absa;
					work[i__] += absa;
					/* L90: */
				}
				value = MAX (value, sum);
				/* L100: */
			}
		}
	} else if (lsame_ (norm, "F") || lsame_ (norm, "E")) {

		/* Find normF(A). */

		scale = 0.;
		sum = 1.;
		if (lsame_ (uplo, "U")) {
			i__1 = *n;
			for (j = 2; j <= i__1; ++j) {
				i__2 = j - 1;
				NUMlapack_dlassq (&i__2, &a_ref (1, j), &c__1, &scale, &sum);
				/* L110: */
			}
		} else {
			i__1 = *n - 1;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *n - j;
				NUMlapack_dlassq (&i__2, &a_ref (j + 1, j), &c__1, &scale, &sum);
				/* L120: */
			}
		}
		sum *= 2;
		i__1 = *lda + 1;
		NUMlapack_dlassq (n, &a[a_offset], &i__1, &scale, &sum);
		value = scale * sqrt (sum);
	}

	ret_val = value;
	return ret_val;
}								/* NUMlapack_dlansy */


int NUMlapack_dlanv2 (double *a, double *b, double *c__, double *d__, double *rt1r, double *rt1i,
                      double *rt2r, double *rt2i, double *cs, double *sn) {
	/* Table of constant values */
	static double c_b4 = 1.;

	/* System generated locals */
	double d__1, d__2;

	/* Local variables */
	static double temp, p, scale, bcmax, z__, bcmis, sigma;
	static double aa, bb, cc, dd;
	static double cs1, sn1, sab, sac, eps, tau;

	eps = NUMblas_dlamch ("P");
	if (*c__ == 0.) {
		*cs = 1.;
		*sn = 0.;
		goto L10;

	} else if (*b == 0.) {

		/* Swap rows and columns */

		*cs = 0.;
		*sn = 1.;
		temp = *d__;
		*d__ = *a;
		*a = temp;
		*b = - (*c__);
		*c__ = 0.;
		goto L10;
	} else if (*a - *d__ == 0. && d_sign (&c_b4, b) != d_sign (&c_b4, c__)) {
		*cs = 1.;
		*sn = 0.;
		goto L10;
	} else {

		temp = *a - *d__;
		p = temp * .5;
		/* Computing MAX */
		d__1 = fabs (*b), d__2 = fabs (*c__);
		bcmax = MAX (d__1, d__2);
		/* Computing MIN */
		d__1 = fabs (*b), d__2 = fabs (*c__);
		bcmis = MIN (d__1, d__2) * d_sign (&c_b4, b) * d_sign (&c_b4, c__);
		/* Computing MAX */
		d__1 = fabs (p);
		scale = MAX (d__1, bcmax);
		z__ = p / scale * p + bcmax / scale * bcmis;

		/* If Z is of the order of the machine accuracy, postpone the
		   decision on the nature of eigenvalues */

		if (z__ >= eps * 4.) {

			/* Real eigenvalues. Compute A and D. */

			d__1 = sqrt (scale) * sqrt (z__);
			z__ = p + d_sign (&d__1, &p);
			*a = *d__ + z__;
			*d__ -= bcmax / z__ * bcmis;

			/* Compute B and the rotation matrix */

			tau = NUMlapack_dlapy2 (c__, &z__);
			*cs = z__ / tau;
			*sn = *c__ / tau;
			*b -= *c__;
			*c__ = 0.;
		} else {

			/* Complex eigenvalues, or real (almost) equal eigenvalues. Make
			   diagonal elements equal. */

			sigma = *b + *c__;
			tau = NUMlapack_dlapy2 (&sigma, &temp);
			*cs = sqrt ( (fabs (sigma) / tau + 1.) * .5);
			*sn = - (p / (tau * *cs)) * d_sign (&c_b4, &sigma);

			/* Compute [ AA BB ] = [ A B ] [ CS -SN ] [ CC DD ] [ C D ] [ SN
			   CS ] */

			aa = *a * *cs + *b * *sn;
			bb = - (*a) * *sn + *b * *cs;
			cc = *c__ * *cs + *d__ * *sn;
			dd = - (*c__) * *sn + *d__ * *cs;

			/* Compute [ A B ] = [ CS SN ] [ AA BB ] [ C D ] [-SN CS ] [ CC
			   DD ] */

			*a = aa * *cs + cc * *sn;
			*b = bb * *cs + dd * *sn;
			*c__ = -aa * *sn + cc * *cs;
			*d__ = -bb * *sn + dd * *cs;

			temp = (*a + *d__) * .5;
			*a = temp;
			*d__ = temp;

			if (*c__ != 0.) {
				if (*b != 0.) {
					if (d_sign (&c_b4, b) == d_sign (&c_b4, c__)) {

						/* Real eigenvalues: reduce to upper triangular form */

						sab = sqrt ( (fabs (*b)));
						sac = sqrt ( (fabs (*c__)));
						d__1 = sab * sac;
						p = d_sign (&d__1, c__);
						tau = 1. / sqrt ( (d__1 = *b + *c__, fabs (d__1)));
						*a = temp + p;
						*d__ = temp - p;
						*b -= *c__;
						*c__ = 0.;
						cs1 = sab * tau;
						sn1 = sac * tau;
						temp = *cs * cs1 - *sn * sn1;
						*sn = *cs * sn1 + *sn * cs1;
						*cs = temp;
					}
				} else {
					*b = - (*c__);
					*c__ = 0.;
					temp = *cs;
					*cs = - (*sn);
					*sn = temp;
				}
			}
		}

	}

L10:

	/* Store eigenvalues in (RT1R,RT1I) and (RT2R,RT2I). */

	*rt1r = *a;
	*rt2r = *d__;
	if (*c__ == 0.) {
		*rt1i = 0.;
		*rt2i = 0.;
	} else {
		*rt1i = sqrt ( (fabs (*b))) * sqrt ( (fabs (*c__)));
		*rt2i = - (*rt1i);
	}
	return 0;
}								/* NUMlapack_dlanv2 */


int NUMlapack_dlapll (long *n, double *x, long *incx, double *y, long *incy, double *ssmin) {
	/* System generated locals */
	long i__1;

	/* Local variables */
	static double c__;
	static double ssmax, a11, a12, a22;
	static double tau;

	--y;
	--x;

	/* Function Body */
	if (*n <= 1) {
		*ssmin = 0.;
		return 0;
	}

	/* Compute the QR factorization of the N-by-2 matrix ( X Y ) */

	NUMlapack_dlarfg (n, &x[1], &x[*incx + 1], incx, &tau);
	a11 = x[1];
	x[1] = 1.;

	c__ = -tau * NUMblas_ddot (n, &x[1], incx, &y[1], incy);
	NUMblas_daxpy (n, &c__, &x[1], incx, &y[1], incy);

	i__1 = *n - 1;
	NUMlapack_dlarfg (&i__1, &y[*incy + 1], &y[ (*incy << 1) + 1], incy, &tau);

	a12 = y[1];
	a22 = y[*incy + 1];

	/* Compute the SVD of 2-by-2 Upper triangular matrix. */

	NUMlapack_dlas2 (&a11, &a12, &a22, ssmin, &ssmax);

	return 0;
}

/* NUMlapack_dlapll */

#define x_ref(a_1,a_2) x[(a_2)*x_dim1 + a_1]

int NUMlapack_dlapmt (long *forwrd, long *m, long *n, double *x, long *ldx, long *k) {
	/* System generated locals */
	long x_dim1, x_offset, i__1, i__2;

	/* Local variables */
	static double temp;
	static long i__, j, ii, in;

	x_dim1 = *ldx;
	x_offset = 1 + x_dim1 * 1;
	x -= x_offset;
	--k;

	/* Function Body */
	if (*n <= 1) {
		return 0;
	}

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		k[i__] = -k[i__];
		/* L10: */
	}

	if (*forwrd) {

		/* Forward permutation */

		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {

			if (k[i__] > 0) {
				goto L40;
			}

			j = i__;
			k[j] = -k[j];
			in = k[j];

L20:
			if (k[in] > 0) {
				goto L40;
			}

			i__2 = *m;
			for (ii = 1; ii <= i__2; ++ii) {
				temp = x_ref (ii, j);
				x_ref (ii, j) = x_ref (ii, in);
				x_ref (ii, in) = temp;
				/* L30: */
			}

			k[in] = -k[in];
			j = in;
			in = k[in];
			goto L20;

L40:

			/* L50: */
			;
		}

	} else {

		/* Backward permutation */

		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {

			if (k[i__] > 0) {
				goto L80;
			}

			k[i__] = -k[i__];
			j = k[i__];
L60:
			if (j == i__) {
				goto L80;
			}

			i__2 = *m;
			for (ii = 1; ii <= i__2; ++ii) {
				temp = x_ref (ii, i__);
				x_ref (ii, i__) = x_ref (ii, j);
				x_ref (ii, j) = temp;
				/* L70: */
			}

			k[j] = -k[j];
			j = k[j];
			goto L60;

L80:

			/* L90: */
			;
		}

	}

	return 0;
}								/* NUMlapack_dlapmt */

#undef x_ref

double NUMlapack_dlapy2 (double *x, double *y) {
	/* System generated locals */
	double ret_val, d__1;

	/* Local variables */
	static double xabs, yabs, w, z__;

	xabs = fabs (*x);
	yabs = fabs (*y);
	w = MAX (xabs, yabs);
	z__ = MIN (xabs, yabs);
	if (z__ == 0.) {
		ret_val = w;
	} else {
		/* Computing 2nd power */
		d__1 = z__ / w;
		ret_val = w * sqrt (d__1 * d__1 + 1.);
	}
	return ret_val;
}								/* NUMlapack_dlapy2 */

#define work_ref(a_1,a_2) work[(a_2)*work_dim1 + a_1]
#define v_ref(a_1,a_2) v[(a_2)*v_dim1 + a_1]

int NUMlapack_dlarfb (const char *side, const char *trans, const char *direct, const char *storev, long *m, long *n, long *k, double *v,
                      long *ldv, double *t, long *ldt, double *c__, long *ldc, double *work, long *ldwork) {
	/* Table of constant values */
	static long c__1 = 1;
	static double c_b14 = 1.;
	static double c_b25 = -1.;

	/* System generated locals */
	long c_dim1, c_offset, t_dim1, t_offset, v_dim1, v_offset;
	long work_dim1, work_offset, i__1, i__2;

	/* Local variables */
	static long i__, j;
	static char transt[1];

	v_dim1 = *ldv;
	v_offset = 1 + v_dim1 * 1;
	v -= v_offset;
	t_dim1 = *ldt;
	t_offset = 1 + t_dim1 * 1;
	t -= t_offset;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1 * 1;
	c__ -= c_offset;
	work_dim1 = *ldwork;
	work_offset = 1 + work_dim1 * 1;
	work -= work_offset;

	/* Function Body */
	if (*m <= 0 || *n <= 0) {
		return 0;
	}

	if (lsame_ (trans, "N")) {
		* (unsigned char *) transt = 'T';
	} else {
		* (unsigned char *) transt = 'N';
	}

	if (lsame_ (storev, "C")) {

		if (lsame_ (direct, "F")) {

			/* Let V = ( V1 ) (first K rows) ( V2 ) where V1 is unit lower
			   triangular. */

			if (lsame_ (side, "L")) {

				/* Form H * C or H' * C where C = ( C1 ) ( C2 )

				   W := C' * V = (C1'*V1 + C2'*V2) (stored in WORK)

				   W := C1' */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					NUMblas_dcopy (n, &c___ref (j, 1), ldc, &work_ref (1, j), &c__1);
					/* L10: */
				}

				/* W := W * V1 */

				NUMblas_dtrmm ("Right", "Lower", "No transpose", "Unit", n, k, &c_b14, &v[v_offset], ldv,
				               &work[work_offset], ldwork);
				if (*m > *k) {

					/* W := W + C2'*V2 */

					i__1 = *m - *k;
					NUMblas_dgemm ("Transpose", "No transpose", n, k, &i__1, &c_b14, &c___ref (*k + 1, 1), ldc,
					               &v_ref (*k + 1, 1), ldv, &c_b14, &work[work_offset], ldwork);
				}

				/* W := W * T' or W * T */

				NUMblas_dtrmm ("Right", "Upper", transt, "Non-unit", n, k, &c_b14, &t[t_offset], ldt,
				               &work[work_offset], ldwork);

				/* C := C - V * W' */

				if (*m > *k) {

					/* C2 := C2 - V2 * W' */

					i__1 = *m - *k;
					NUMblas_dgemm ("No transpose", "Transpose", &i__1, n, k, &c_b25, &v_ref (*k + 1, 1), ldv,
					               &work[work_offset], ldwork, &c_b14, &c___ref (*k + 1, 1), ldc);
				}

				/* W := W * V1' */

				NUMblas_dtrmm ("Right", "Lower", "Transpose", "Unit", n, k, &c_b14, &v[v_offset], ldv,
				               &work[work_offset], ldwork);

				/* C1 := C1 - W' */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *n;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (j, i__) = c___ref (j, i__) - work_ref (i__, j);
						/* L20: */
					}
					/* L30: */
				}

			} else if (lsame_ (side, "R")) {

				/* Form C * H or C * H' where C = ( C1 C2 )

				   W := C * V = (C1*V1 + C2*V2) (stored in WORK)

				   W := C1 */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					NUMblas_dcopy (m, &c___ref (1, j), &c__1, &work_ref (1, j), &c__1);
					/* L40: */
				}

				/* W := W * V1 */

				NUMblas_dtrmm ("Right", "Lower", "No transpose", "Unit", m, k, &c_b14, &v[v_offset], ldv,
				               &work[work_offset], ldwork);
				if (*n > *k) {

					/* W := W + C2 * V2 */

					i__1 = *n - *k;
					NUMblas_dgemm ("No transpose", "No transpose", m, k, &i__1, &c_b14, &c___ref (1, *k + 1), ldc,
					               &v_ref (*k + 1, 1), ldv, &c_b14, &work[work_offset], ldwork);
				}

				/* W := W * T or W * T' */

				NUMblas_dtrmm ("Right", "Upper", trans, "Non-unit", m, k, &c_b14, &t[t_offset], ldt,
				               &work[work_offset], ldwork);

				/* C := C - W * V' */

				if (*n > *k) {

					/* C2 := C2 - W * V2' */

					i__1 = *n - *k;
					NUMblas_dgemm ("No transpose", "Transpose", m, &i__1, k, &c_b25, &work[work_offset], ldwork,
					               &v_ref (*k + 1, 1), ldv, &c_b14, &c___ref (1, *k + 1), ldc);
				}

				/* W := W * V1' */

				NUMblas_dtrmm ("Right", "Lower", "Transpose", "Unit", m, k, &c_b14, &v[v_offset], ldv,
				               &work[work_offset], ldwork);

				/* C1 := C1 - W */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (i__, j) = c___ref (i__, j) - work_ref (i__, j);
						/* L50: */
					}
					/* L60: */
				}
			}

		} else {

			/* Let V = ( V1 ) ( V2 ) (last K rows) where V2 is unit upper
			   triangular. */

			if (lsame_ (side, "L")) {

				/* Form H * C or H' * C where C = ( C1 ) ( C2 )

				   W := C' * V = (C1'*V1 + C2'*V2) (stored in WORK)

				   W := C2' */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					NUMblas_dcopy (n, &c___ref (*m - *k + j, 1), ldc, &work_ref (1, j), &c__1);
					/* L70: */
				}

				/* W := W * V2 */

				NUMblas_dtrmm ("Right", "Upper", "No transpose", "Unit", n, k, &c_b14, &v_ref (*m - *k + 1, 1), ldv,
				               &work[work_offset], ldwork);
				if (*m > *k) {

					/* W := W + C1'*V1 */

					i__1 = *m - *k;
					NUMblas_dgemm ("Transpose", "No transpose", n, k, &i__1, &c_b14, &c__[c_offset], ldc,
					               &v[v_offset], ldv, &c_b14, &work[work_offset], ldwork);
				}

				/* W := W * T' or W * T */

				NUMblas_dtrmm ("Right", "Lower", transt, "Non-unit", n, k, &c_b14, &t[t_offset], ldt,
				               &work[work_offset], ldwork);

				/* C := C - V * W' */

				if (*m > *k) {

					/* C1 := C1 - V1 * W' */

					i__1 = *m - *k;
					NUMblas_dgemm ("No transpose", "Transpose", &i__1, n, k, &c_b25, &v[v_offset], ldv,
					               &work[work_offset], ldwork, &c_b14, &c__[c_offset], ldc);
				}

				/* W := W * V2' */

				NUMblas_dtrmm ("Right", "Upper", "Transpose", "Unit", n, k, &c_b14, &v_ref (*m - *k + 1, 1), ldv,
				               &work[work_offset], ldwork);

				/* C2 := C2 - W' */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *n;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (*m - *k + j, i__) = c___ref (*m - *k + j, i__) - work_ref (i__, j);
						/* L80: */
					}
					/* L90: */
				}

			} else if (lsame_ (side, "R")) {

				/* Form C * H or C * H' where C = ( C1 C2 )

				   W := C * V = (C1*V1 + C2*V2) (stored in WORK)

				   W := C2 */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					NUMblas_dcopy (m, &c___ref (1, *n - *k + j), &c__1, &work_ref (1, j), &c__1);
					/* L100: */
				}

				/* W := W * V2 */

				NUMblas_dtrmm ("Right", "Upper", "No transpose", "Unit", m, k, &c_b14, &v_ref (*n - *k + 1, 1), ldv,
				               &work[work_offset], ldwork);
				if (*n > *k) {

					/* W := W + C1 * V1 */

					i__1 = *n - *k;
					NUMblas_dgemm ("No transpose", "No transpose", m, k, &i__1, &c_b14, &c__[c_offset], ldc,
					               &v[v_offset], ldv, &c_b14, &work[work_offset], ldwork);
				}

				/* W := W * T or W * T' */

				NUMblas_dtrmm ("Right", "Lower", trans, "Non-unit", m, k, &c_b14, &t[t_offset], ldt,
				               &work[work_offset], ldwork);

				/* C := C - W * V' */

				if (*n > *k) {

					/* C1 := C1 - W * V1' */

					i__1 = *n - *k;
					NUMblas_dgemm ("No transpose", "Transpose", m, &i__1, k, &c_b25, &work[work_offset], ldwork,
					               &v[v_offset], ldv, &c_b14, &c__[c_offset], ldc);
				}

				/* W := W * V2' */

				NUMblas_dtrmm ("Right", "Upper", "Transpose", "Unit", m, k, &c_b14, &v_ref (*n - *k + 1, 1), ldv,
				               &work[work_offset], ldwork);

				/* C2 := C2 - W */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (i__, *n - *k + j) = c___ref (i__, *n - *k + j) - work_ref (i__, j);
						/* L110: */
					}
					/* L120: */
				}
			}
		}

	} else if (lsame_ (storev, "R")) {

		if (lsame_ (direct, "F")) {

			/* Let V = ( V1 V2 ) (V1: first K columns) where V1 is unit upper
			   triangular. */

			if (lsame_ (side, "L")) {

				/* Form H * C or H' * C where C = ( C1 ) ( C2 )

				   W := C' * V' = (C1'*V1' + C2'*V2') (stored in WORK)

				   W := C1' */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					NUMblas_dcopy (n, &c___ref (j, 1), ldc, &work_ref (1, j), &c__1);
					/* L130: */
				}

				/* W := W * V1' */

				NUMblas_dtrmm ("Right", "Upper", "Transpose", "Unit", n, k, &c_b14, &v[v_offset], ldv,
				               &work[work_offset], ldwork);
				if (*m > *k) {

					/* W := W + C2'*V2' */

					i__1 = *m - *k;
					NUMblas_dgemm ("Transpose", "Transpose", n, k, &i__1, &c_b14, &c___ref (*k + 1, 1), ldc,
					               &v_ref (1, *k + 1), ldv, &c_b14, &work[work_offset], ldwork);
				}

				/* W := W * T' or W * T */

				NUMblas_dtrmm ("Right", "Upper", transt, "Non-unit", n, k, &c_b14, &t[t_offset], ldt,
				               &work[work_offset], ldwork);

				/* C := C - V' * W' */

				if (*m > *k) {

					/* C2 := C2 - V2' * W' */

					i__1 = *m - *k;
					NUMblas_dgemm ("Transpose", "Transpose", &i__1, n, k, &c_b25, &v_ref (1, *k + 1), ldv,
					               &work[work_offset], ldwork, &c_b14, &c___ref (*k + 1, 1), ldc);
				}

				/* W := W * V1 */

				NUMblas_dtrmm ("Right", "Upper", "No transpose", "Unit", n, k, &c_b14, &v[v_offset], ldv,
				               &work[work_offset], ldwork);

				/* C1 := C1 - W' */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *n;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (j, i__) = c___ref (j, i__) - work_ref (i__, j);
						/* L140: */
					}
					/* L150: */
				}

			} else if (lsame_ (side, "R")) {

				/* Form C * H or C * H' where C = ( C1 C2 )

				   W := C * V' = (C1*V1' + C2*V2') (stored in WORK)

				   W := C1 */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					NUMblas_dcopy (m, &c___ref (1, j), &c__1, &work_ref (1, j), &c__1);
					/* L160: */
				}

				/* W := W * V1' */

				NUMblas_dtrmm ("Right", "Upper", "Transpose", "Unit", m, k, &c_b14, &v[v_offset], ldv,
				               &work[work_offset], ldwork);
				if (*n > *k) {

					/* W := W + C2 * V2' */

					i__1 = *n - *k;
					NUMblas_dgemm ("No transpose", "Transpose", m, k, &i__1, &c_b14, &c___ref (1, *k + 1), ldc,
					               &v_ref (1, *k + 1), ldv, &c_b14, &work[work_offset], ldwork);
				}

				/* W := W * T or W * T' */

				NUMblas_dtrmm ("Right", "Upper", trans, "Non-unit", m, k, &c_b14, &t[t_offset], ldt,
				               &work[work_offset], ldwork);

				/* C := C - W * V */

				if (*n > *k) {

					/* C2 := C2 - W * V2 */

					i__1 = *n - *k;
					NUMblas_dgemm ("No transpose", "No transpose", m, &i__1, k, &c_b25, &work[work_offset], ldwork,
					               &v_ref (1, *k + 1), ldv, &c_b14, &c___ref (1, *k + 1), ldc);
				}

				/* W := W * V1 */

				NUMblas_dtrmm ("Right", "Upper", "No transpose", "Unit", m, k, &c_b14, &v[v_offset], ldv,
				               &work[work_offset], ldwork);

				/* C1 := C1 - W */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (i__, j) = c___ref (i__, j) - work_ref (i__, j);
						/* L170: */
					}
					/* L180: */
				}

			}

		} else {

			/* Let V = ( V1 V2 ) (V2: last K columns) where V2 is unit lower
			   triangular. */

			if (lsame_ (side, "L")) {

				/* Form H * C or H' * C where C = ( C1 ) ( C2 )

				   W := C' * V' = (C1'*V1' + C2'*V2') (stored in WORK)

				   W := C2' */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					NUMblas_dcopy (n, &c___ref (*m - *k + j, 1), ldc, &work_ref (1, j), &c__1);
					/* L190: */
				}

				/* W := W * V2' */

				NUMblas_dtrmm ("Right", "Lower", "Transpose", "Unit", n, k, &c_b14, &v_ref (1, *m - *k + 1), ldv,
				               &work[work_offset], ldwork);
				if (*m > *k) {

					/* W := W + C1'*V1' */

					i__1 = *m - *k;
					NUMblas_dgemm ("Transpose", "Transpose", n, k, &i__1, &c_b14, &c__[c_offset], ldc, &v[v_offset],
					               ldv, &c_b14, &work[work_offset], ldwork);
				}

				/* W := W * T' or W * T */

				NUMblas_dtrmm ("Right", "Lower", transt, "Non-unit", n, k, &c_b14, &t[t_offset], ldt,
				               &work[work_offset], ldwork);

				/* C := C - V' * W' */

				if (*m > *k) {

					/* C1 := C1 - V1' * W' */

					i__1 = *m - *k;
					NUMblas_dgemm ("Transpose", "Transpose", &i__1, n, k, &c_b25, &v[v_offset], ldv,
					               &work[work_offset], ldwork, &c_b14, &c__[c_offset], ldc);
				}

				/* W := W * V2 */

				NUMblas_dtrmm ("Right", "Lower", "No transpose", "Unit", n, k, &c_b14, &v_ref (1, *m - *k + 1), ldv,
				               &work[work_offset], ldwork);

				/* C2 := C2 - W' */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *n;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (*m - *k + j, i__) = c___ref (*m - *k + j, i__) - work_ref (i__, j);
						/* L200: */
					}
					/* L210: */
				}

			} else if (lsame_ (side, "R")) {

				/* Form C * H or C * H' where C = ( C1 C2 )

				   W := C * V' = (C1*V1' + C2*V2') (stored in WORK)

				   W := C2 */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					NUMblas_dcopy (m, &c___ref (1, *n - *k + j), &c__1, &work_ref (1, j), &c__1);
					/* L220: */
				}

				/* W := W * V2' */

				NUMblas_dtrmm ("Right", "Lower", "Transpose", "Unit", m, k, &c_b14, &v_ref (1, *n - *k + 1), ldv,
				               &work[work_offset], ldwork);
				if (*n > *k) {

					/* W := W + C1 * V1' */

					i__1 = *n - *k;
					NUMblas_dgemm ("No transpose", "Transpose", m, k, &i__1, &c_b14, &c__[c_offset], ldc,
					               &v[v_offset], ldv, &c_b14, &work[work_offset], ldwork);
				}

				/* W := W * T or W * T' */

				NUMblas_dtrmm ("Right", "Lower", trans, "Non-unit", m, k, &c_b14, &t[t_offset], ldt,
				               &work[work_offset], ldwork);

				/* C := C - W * V */

				if (*n > *k) {

					/* C1 := C1 - W * V1 */

					i__1 = *n - *k;
					NUMblas_dgemm ("No transpose", "No transpose", m, &i__1, k, &c_b25, &work[work_offset], ldwork,
					               &v[v_offset], ldv, &c_b14, &c__[c_offset], ldc);
				}

				/* W := W * V2 */

				NUMblas_dtrmm ("Right", "Lower", "No transpose", "Unit", m, k, &c_b14, &v_ref (1, *n - *k + 1), ldv,
				               &work[work_offset], ldwork);

				/* C1 := C1 - W */

				i__1 = *k;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (i__, *n - *k + j) = c___ref (i__, *n - *k + j) - work_ref (i__, j);
						/* L230: */
					}
					/* L240: */
				}

			}

		}
	}

	return 0;
}								/* NUMlapack_dlarfb */

#undef v_ref
#undef work_ref

int NUMlapack_dlarf (const char *side, long *m, long *n, double *v, long *incv, double *tau, double *c__, long *ldc,
                     double *work) {
	/* Table of constant values */
	static double c_b4 = 1.;
	static double c_b5 = 0.;
	static long c__1 = 1;

	/* System generated locals */
	long c_dim1, c_offset;
	double d__1;

	/* Local variables */

	--v;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1 * 1;
	c__ -= c_offset;
	--work;

	/* Function Body */
	if (lsame_ (side, "L")) {

		/* Form H * C */

		if (*tau != 0.) {

			/* w := C' * v */

			NUMblas_dgemv ("Transpose", m, n, &c_b4, &c__[c_offset], ldc, &v[1], incv, &c_b5, &work[1], &c__1);

			/* C := C - v * w' */

			d__1 = - (*tau);
			NUMblas_dger (m, n, &d__1, &v[1], incv, &work[1], &c__1, &c__[c_offset], ldc);
		}
	} else {

		/* Form C * H */

		if (*tau != 0.) {

			/* w := C * v */

			NUMblas_dgemv ("No transpose", m, n, &c_b4, &c__[c_offset], ldc, &v[1], incv, &c_b5, &work[1], &c__1);

			/* C := C - w * v' */

			d__1 = - (*tau);
			NUMblas_dger (m, n, &d__1, &work[1], &c__1, &v[1], incv, &c__[c_offset], ldc);
		}
	}
	return 0;
}								/* NUMlapack_dlarf */

int NUMlapack_dlarfg (long *n, double *alpha, double *x, long *incx, double *tau) {
	/* System generated locals */
	long i__1;
	double d__1;

	/* Local variables */
	static double beta;
	static long j;
	static double xnorm;
	static double safmin, rsafmn;
	static long knt;

	--x;

	/* Function Body */
	if (*n <= 1) {
		*tau = 0.;
		return 0;
	}

	i__1 = *n - 1;
	xnorm = NUMblas_dnrm2 (&i__1, &x[1], incx);

	if (xnorm == 0.) {

		/* H = I */

		*tau = 0.;
	} else {

		/* general case */

		d__1 = NUMlapack_dlapy2 (alpha, &xnorm);
		beta = -d_sign (&d__1, alpha);
		safmin = NUMblas_dlamch ("S") / NUMblas_dlamch ("E");
		if (fabs (beta) < safmin) {

			/* XNORM, BETA may be inaccurate; scale X and recompute them */

			rsafmn = 1. / safmin;
			knt = 0;
L10:
			++knt;
			i__1 = *n - 1;
			NUMblas_dscal (&i__1, &rsafmn, &x[1], incx);
			beta *= rsafmn;
			*alpha *= rsafmn;
			if (fabs (beta) < safmin) {
				goto L10;
			}

			/* New BETA is at most 1, at least SAFMIN */

			i__1 = *n - 1;
			xnorm = NUMblas_dnrm2 (&i__1, &x[1], incx);
			d__1 = NUMlapack_dlapy2 (alpha, &xnorm);
			beta = -d_sign (&d__1, alpha);
			*tau = (beta - *alpha) / beta;
			i__1 = *n - 1;
			d__1 = 1. / (*alpha - beta);
			NUMblas_dscal (&i__1, &d__1, &x[1], incx);

			/* If ALPHA is subnormal, it may lose relative accuracy */

			*alpha = beta;
			i__1 = knt;
			for (j = 1; j <= i__1; ++j) {
				*alpha *= safmin;
				/* L20: */
			}
		} else {
			*tau = (beta - *alpha) / beta;
			i__1 = *n - 1;
			d__1 = 1. / (*alpha - beta);
			NUMblas_dscal (&i__1, &d__1, &x[1], incx);
			*alpha = beta;
		}
	}

	return 0;
}								/* NUMlapack_dlarfg */

#define t_ref(a_1,a_2) t[(a_2)*t_dim1 + a_1]
#define v_ref(a_1,a_2) v[(a_2)*v_dim1 + a_1]

int NUMlapack_dlarft (const char *direct, const char *storev, long *n, long *k, double *v, long *ldv, double *tau,
                      double *t, long *ldt) {
	/* Table of constant values */
	static long c__1 = 1;
	static double c_b8 = 0.;

	/* System generated locals */
	long t_dim1, t_offset, v_dim1, v_offset, i__1, i__2, i__3;
	double d__1;

	/* Local variables */
	static long i__, j;
	static double vii;

	v_dim1 = *ldv;
	v_offset = 1 + v_dim1 * 1;
	v -= v_offset;
	--tau;
	t_dim1 = *ldt;
	t_offset = 1 + t_dim1 * 1;
	t -= t_offset;

	/* Function Body */
	if (*n == 0) {
		return 0;
	}

	if (lsame_ (direct, "F")) {
		i__1 = *k;
		for (i__ = 1; i__ <= i__1; ++i__) {
			if (tau[i__] == 0.) {

				/* H(i) = I */

				i__2 = i__;
				for (j = 1; j <= i__2; ++j) {
					t_ref (j, i__) = 0.;
					/* L10: */
				}
			} else {

				/* general case */

				vii = v_ref (i__, i__);
				v_ref (i__, i__) = 1.;
				if (lsame_ (storev, "C")) {

					/* T(1:i-1,i) := - tau(i) * V(i:n,1:i-1)' * V(i:n,i) */

					i__2 = *n - i__ + 1;
					i__3 = i__ - 1;
					d__1 = -tau[i__];
					NUMblas_dgemv ("Transpose", &i__2, &i__3, &d__1, &v_ref (i__, 1), ldv, &v_ref (i__, i__), &c__1,
					               &c_b8, &t_ref (1, i__), &c__1);
				} else {

					/* T(1:i-1,i) := - tau(i) * V(1:i-1,i:n) * V(i,i:n)' */

					i__2 = i__ - 1;
					i__3 = *n - i__ + 1;
					d__1 = -tau[i__];
					NUMblas_dgemv ("No transpose", &i__2, &i__3, &d__1, &v_ref (1, i__), ldv, &v_ref (i__, i__), ldv,
					               &c_b8, &t_ref (1, i__), &c__1);
				}
				v_ref (i__, i__) = vii;

				/* T(1:i-1,i) := T(1:i-1,1:i-1) * T(1:i-1,i) */

				i__2 = i__ - 1;
				NUMblas_dtrmv ("Upper", "No transpose", "Non-unit", &i__2, &t[t_offset], ldt, &t_ref (1, i__),
				               &c__1);
				t_ref (i__, i__) = tau[i__];
			}
			/* L20: */
		}
	} else {
		for (i__ = *k; i__ >= 1; --i__) {
			if (tau[i__] == 0.) {

				/* H(i) = I */

				i__1 = *k;
				for (j = i__; j <= i__1; ++j) {
					t_ref (j, i__) = 0.;
					/* L30: */
				}
			} else {

				/* general case */

				if (i__ < *k) {
					if (lsame_ (storev, "C")) {
						vii = v_ref (*n - *k + i__, i__);
						v_ref (*n - *k + i__, i__) = 1.;

						/* T(i+1:k,i) := - tau(i) * V(1:n-k+i,i+1:k)' *
						   V(1:n-k+i,i) */

						i__1 = *n - *k + i__;
						i__2 = *k - i__;
						d__1 = -tau[i__];
						NUMblas_dgemv ("Transpose", &i__1, &i__2, &d__1, &v_ref (1, i__ + 1), ldv, &v_ref (1, i__),
						               &c__1, &c_b8, &t_ref (i__ + 1, i__), &c__1);
						v_ref (*n - *k + i__, i__) = vii;
					} else {
						vii = v_ref (i__, *n - *k + i__);
						v_ref (i__, *n - *k + i__) = 1.;

						/* T(i+1:k,i) := - tau(i) * V(i+1:k,1:n-k+i) *
						   V(i,1:n-k+i)' */

						i__1 = *k - i__;
						i__2 = *n - *k + i__;
						d__1 = -tau[i__];
						NUMblas_dgemv ("No transpose", &i__1, &i__2, &d__1, &v_ref (i__ + 1, 1), ldv, &v_ref (i__,
						               1), ldv, &c_b8, &t_ref (i__ + 1, i__), &c__1);
						v_ref (i__, *n - *k + i__) = vii;
					}

					/* T(i+1:k,i) := T(i+1:k,i+1:k) * T(i+1:k,i) */

					i__1 = *k - i__;
					NUMblas_dtrmv ("Lower", "No transpose", "Non-unit", &i__1, &t_ref (i__ + 1, i__ + 1), ldt,
					               &t_ref (i__ + 1, i__), &c__1);
				}
				t_ref (i__, i__) = tau[i__];
			}
			/* L40: */
		}
	}
	return 0;
}								/* NUMlapack_dlarft */

#undef v_ref
#undef t_ref

int NUMlapack_dlartg (double *f, double *g, double *cs, double *sn, double *r__) {
	/* Initialized data */
	static long first = TRUE;

	/* System generated locals */
	long i__1;
	double d__1, d__2;

	/* Local variables */
	static long i__;
	static double scale;
	static long count;
	static double f1, g1, safmn2, safmx2;
	static double safmin, eps;

	if (first) {
		first = FALSE;
		safmin = NUMblas_dlamch ("S");
		eps = NUMblas_dlamch ("E");
		d__1 = NUMblas_dlamch ("B");
		i__1 = (long) (log (safmin / eps) / log (NUMblas_dlamch ("B")) / 2.);
		safmn2 = pow_di (&d__1, &i__1);
		safmx2 = 1. / safmn2;
	}
	if (*g == 0.) {
		*cs = 1.;
		*sn = 0.;
		*r__ = *f;
	} else if (*f == 0.) {
		*cs = 0.;
		*sn = 1.;
		*r__ = *g;
	} else {
		f1 = *f;
		g1 = *g;
		/* Computing MAX */
		d__1 = fabs (f1), d__2 = fabs (g1);
		scale = MAX (d__1, d__2);
		if (scale >= safmx2) {
			count = 0;
L10:
			++count;
			f1 *= safmn2;
			g1 *= safmn2;
			/* Computing MAX */
			d__1 = fabs (f1), d__2 = fabs (g1);
			scale = MAX (d__1, d__2);
			if (scale >= safmx2) {
				goto L10;
			}
			/* Computing 2nd power */
			d__1 = f1;
			/* Computing 2nd power */
			d__2 = g1;
			*r__ = sqrt (d__1 * d__1 + d__2 * d__2);
			*cs = f1 / *r__;
			*sn = g1 / *r__;
			i__1 = count;
			for (i__ = 1; i__ <= i__1; ++i__) {
				*r__ *= safmx2;
				/* L20: */
			}
		} else if (scale <= safmn2) {
			count = 0;
L30:
			++count;
			f1 *= safmx2;
			g1 *= safmx2;
			/* Computing MAX */
			d__1 = fabs (f1), d__2 = fabs (g1);
			scale = MAX (d__1, d__2);
			if (scale <= safmn2) {
				goto L30;
			}
			/* Computing 2nd power */
			d__1 = f1;
			/* Computing 2nd power */
			d__2 = g1;
			*r__ = sqrt (d__1 * d__1 + d__2 * d__2);
			*cs = f1 / *r__;
			*sn = g1 / *r__;
			i__1 = count;
			for (i__ = 1; i__ <= i__1; ++i__) {
				*r__ *= safmn2;
				/* L40: */
			}
		} else {
			/* Computing 2nd power */
			d__1 = f1;
			/* Computing 2nd power */
			d__2 = g1;
			*r__ = sqrt (d__1 * d__1 + d__2 * d__2);
			*cs = f1 / *r__;
			*sn = g1 / *r__;
		}
		if (fabs (*f) > fabs (*g) && *cs < 0.) {
			*cs = - (*cs);
			*sn = - (*sn);
			*r__ = - (*r__);
		}
	}
	return 0;
}								/* NUMlapack_dlartg */

int NUMlapack_dlarfx (const char *side, long *m, long *n, double *v, double *tau, double *c__, long *ldc,
                      double *work) {
	/* Table of constant values */
	static double c_b14 = 1.;
	static long c__1 = 1;
	static double c_b16 = 0.;

	/* System generated locals */
	long c_dim1, c_offset, i__1;
	double d__1;

	/* Local variables */
	static long j;
	static double t1, t2, t3, t4, t5, t6, t7, t8, t9, v1, v2, v3, v4, v5, v6, v7, v8, v9, t10, v10, sum;

	--v;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1 * 1;
	c__ -= c_offset;
	--work;

	/* Function Body */
	if (*tau == 0.) {
		return 0;
	}
	if (lsame_ (side, "L")) {

		/* Form H * C, where H has order m. */

		switch (*m) {
			case 1:
				goto L10;
			case 2:
				goto L30;
			case 3:
				goto L50;
			case 4:
				goto L70;
			case 5:
				goto L90;
			case 6:
				goto L110;
			case 7:
				goto L130;
			case 8:
				goto L150;
			case 9:
				goto L170;
			case 10:
				goto L190;
		}

		/* Code for general M

		   w := C'*v */

		NUMblas_dgemv ("Transpose", m, n, &c_b14, &c__[c_offset], ldc, &v[1], &c__1, &c_b16, &work[1],
		               &c__1);

		/* C := C - tau * v * w' */

		d__1 = - (*tau);
		NUMblas_dger (m, n, &d__1, &v[1], &c__1, &work[1], &c__1, &c__[c_offset], ldc);
		goto L410;
L10:

		/* Special code for 1 x 1 Householder */

		t1 = 1. - *tau * v[1] * v[1];
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			c___ref (1, j) = t1 * c___ref (1, j);
			/* L20: */
		}
		goto L410;
L30:

		/* Special code for 2 x 2 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			sum = v1 * c___ref (1, j) + v2 * c___ref (2, j);
			c___ref (1, j) = c___ref (1, j) - sum * t1;
			c___ref (2, j) = c___ref (2, j) - sum * t2;
			/* L40: */
		}
		goto L410;
L50:

		/* Special code for 3 x 3 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			sum = v1 * c___ref (1, j) + v2 * c___ref (2, j) + v3 * c___ref (3, j);
			c___ref (1, j) = c___ref (1, j) - sum * t1;
			c___ref (2, j) = c___ref (2, j) - sum * t2;
			c___ref (3, j) = c___ref (3, j) - sum * t3;
			/* L60: */
		}
		goto L410;
L70:

		/* Special code for 4 x 4 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			sum = v1 * c___ref (1, j) + v2 * c___ref (2, j) + v3 * c___ref (3, j) + v4 * c___ref (4, j);
			c___ref (1, j) = c___ref (1, j) - sum * t1;
			c___ref (2, j) = c___ref (2, j) - sum * t2;
			c___ref (3, j) = c___ref (3, j) - sum * t3;
			c___ref (4, j) = c___ref (4, j) - sum * t4;
			/* L80: */
		}
		goto L410;
L90:

		/* Special code for 5 x 5 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		v5 = v[5];
		t5 = *tau * v5;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			sum =
			    v1 * c___ref (1, j) + v2 * c___ref (2, j) + v3 * c___ref (3, j) + v4 * c___ref (4,
			            j) + v5 * c___ref (5, j);
			c___ref (1, j) = c___ref (1, j) - sum * t1;
			c___ref (2, j) = c___ref (2, j) - sum * t2;
			c___ref (3, j) = c___ref (3, j) - sum * t3;
			c___ref (4, j) = c___ref (4, j) - sum * t4;
			c___ref (5, j) = c___ref (5, j) - sum * t5;
			/* L100: */
		}
		goto L410;
L110:

		/* Special code for 6 x 6 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		v5 = v[5];
		t5 = *tau * v5;
		v6 = v[6];
		t6 = *tau * v6;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			sum =
			    v1 * c___ref (1, j) + v2 * c___ref (2, j) + v3 * c___ref (3, j) + v4 * c___ref (4,
			            j) + v5 * c___ref (5, j) + v6 * c___ref (6, j);
			c___ref (1, j) = c___ref (1, j) - sum * t1;
			c___ref (2, j) = c___ref (2, j) - sum * t2;
			c___ref (3, j) = c___ref (3, j) - sum * t3;
			c___ref (4, j) = c___ref (4, j) - sum * t4;
			c___ref (5, j) = c___ref (5, j) - sum * t5;
			c___ref (6, j) = c___ref (6, j) - sum * t6;
			/* L120: */
		}
		goto L410;
L130:

		/* Special code for 7 x 7 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		v5 = v[5];
		t5 = *tau * v5;
		v6 = v[6];
		t6 = *tau * v6;
		v7 = v[7];
		t7 = *tau * v7;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			sum =
			    v1 * c___ref (1, j) + v2 * c___ref (2, j) + v3 * c___ref (3, j) + v4 * c___ref (4,
			            j) + v5 * c___ref (5, j) + v6 * c___ref (6, j) + v7 * c___ref (7, j);
			c___ref (1, j) = c___ref (1, j) - sum * t1;
			c___ref (2, j) = c___ref (2, j) - sum * t2;
			c___ref (3, j) = c___ref (3, j) - sum * t3;
			c___ref (4, j) = c___ref (4, j) - sum * t4;
			c___ref (5, j) = c___ref (5, j) - sum * t5;
			c___ref (6, j) = c___ref (6, j) - sum * t6;
			c___ref (7, j) = c___ref (7, j) - sum * t7;
			/* L140: */
		}
		goto L410;
L150:

		/* Special code for 8 x 8 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		v5 = v[5];
		t5 = *tau * v5;
		v6 = v[6];
		t6 = *tau * v6;
		v7 = v[7];
		t7 = *tau * v7;
		v8 = v[8];
		t8 = *tau * v8;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			sum =
			    v1 * c___ref (1, j) + v2 * c___ref (2, j) + v3 * c___ref (3, j) + v4 * c___ref (4,
			            j) + v5 * c___ref (5, j) + v6 * c___ref (6, j) + v7 * c___ref (7, j) + v8 * c___ref (8, j);
			c___ref (1, j) = c___ref (1, j) - sum * t1;
			c___ref (2, j) = c___ref (2, j) - sum * t2;
			c___ref (3, j) = c___ref (3, j) - sum * t3;
			c___ref (4, j) = c___ref (4, j) - sum * t4;
			c___ref (5, j) = c___ref (5, j) - sum * t5;
			c___ref (6, j) = c___ref (6, j) - sum * t6;
			c___ref (7, j) = c___ref (7, j) - sum * t7;
			c___ref (8, j) = c___ref (8, j) - sum * t8;
			/* L160: */
		}
		goto L410;
L170:

		/* Special code for 9 x 9 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		v5 = v[5];
		t5 = *tau * v5;
		v6 = v[6];
		t6 = *tau * v6;
		v7 = v[7];
		t7 = *tau * v7;
		v8 = v[8];
		t8 = *tau * v8;
		v9 = v[9];
		t9 = *tau * v9;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			sum =
			    v1 * c___ref (1, j) + v2 * c___ref (2, j) + v3 * c___ref (3, j) + v4 * c___ref (4,
			            j) + v5 * c___ref (5, j) + v6 * c___ref (6, j) + v7 * c___ref (7, j) + v8 * c___ref (8,
			                    j) + v9 * c___ref (9, j);
			c___ref (1, j) = c___ref (1, j) - sum * t1;
			c___ref (2, j) = c___ref (2, j) - sum * t2;
			c___ref (3, j) = c___ref (3, j) - sum * t3;
			c___ref (4, j) = c___ref (4, j) - sum * t4;
			c___ref (5, j) = c___ref (5, j) - sum * t5;
			c___ref (6, j) = c___ref (6, j) - sum * t6;
			c___ref (7, j) = c___ref (7, j) - sum * t7;
			c___ref (8, j) = c___ref (8, j) - sum * t8;
			c___ref (9, j) = c___ref (9, j) - sum * t9;
			/* L180: */
		}
		goto L410;
L190:

		/* Special code for 10 x 10 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		v5 = v[5];
		t5 = *tau * v5;
		v6 = v[6];
		t6 = *tau * v6;
		v7 = v[7];
		t7 = *tau * v7;
		v8 = v[8];
		t8 = *tau * v8;
		v9 = v[9];
		t9 = *tau * v9;
		v10 = v[10];
		t10 = *tau * v10;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			sum =
			    v1 * c___ref (1, j) + v2 * c___ref (2, j) + v3 * c___ref (3, j) + v4 * c___ref (4,
			            j) + v5 * c___ref (5, j) + v6 * c___ref (6, j) + v7 * c___ref (7, j) + v8 * c___ref (8,
			                    j) + v9 * c___ref (9, j) + v10 * c___ref (10, j);
			c___ref (1, j) = c___ref (1, j) - sum * t1;
			c___ref (2, j) = c___ref (2, j) - sum * t2;
			c___ref (3, j) = c___ref (3, j) - sum * t3;
			c___ref (4, j) = c___ref (4, j) - sum * t4;
			c___ref (5, j) = c___ref (5, j) - sum * t5;
			c___ref (6, j) = c___ref (6, j) - sum * t6;
			c___ref (7, j) = c___ref (7, j) - sum * t7;
			c___ref (8, j) = c___ref (8, j) - sum * t8;
			c___ref (9, j) = c___ref (9, j) - sum * t9;
			c___ref (10, j) = c___ref (10, j) - sum * t10;
			/* L200: */
		}
		goto L410;
	} else {

		/* Form C * H, where H has order n. */

		switch (*n) {
			case 1:
				goto L210;
			case 2:
				goto L230;
			case 3:
				goto L250;
			case 4:
				goto L270;
			case 5:
				goto L290;
			case 6:
				goto L310;
			case 7:
				goto L330;
			case 8:
				goto L350;
			case 9:
				goto L370;
			case 10:
				goto L390;
		}

		/* Code for general N

		   w := C * v */

		NUMblas_dgemv ("No transpose", m, n, &c_b14, &c__[c_offset], ldc, &v[1], &c__1, &c_b16, &work[1],
		               &c__1);

		/* C := C - tau * w * v' */

		d__1 = - (*tau);
		NUMblas_dger (m, n, &d__1, &work[1], &c__1, &v[1], &c__1, &c__[c_offset], ldc);
		goto L410;
L210:

		/* Special code for 1 x 1 Householder */

		t1 = 1. - *tau * v[1] * v[1];
		i__1 = *m;
		for (j = 1; j <= i__1; ++j) {
			c___ref (j, 1) = t1 * c___ref (j, 1);
			/* L220: */
		}
		goto L410;
L230:

		/* Special code for 2 x 2 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		i__1 = *m;
		for (j = 1; j <= i__1; ++j) {
			sum = v1 * c___ref (j, 1) + v2 * c___ref (j, 2);
			c___ref (j, 1) = c___ref (j, 1) - sum * t1;
			c___ref (j, 2) = c___ref (j, 2) - sum * t2;
			/* L240: */
		}
		goto L410;
L250:

		/* Special code for 3 x 3 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		i__1 = *m;
		for (j = 1; j <= i__1; ++j) {
			sum = v1 * c___ref (j, 1) + v2 * c___ref (j, 2) + v3 * c___ref (j, 3);
			c___ref (j, 1) = c___ref (j, 1) - sum * t1;
			c___ref (j, 2) = c___ref (j, 2) - sum * t2;
			c___ref (j, 3) = c___ref (j, 3) - sum * t3;
			/* L260: */
		}
		goto L410;
L270:

		/* Special code for 4 x 4 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		i__1 = *m;
		for (j = 1; j <= i__1; ++j) {
			sum = v1 * c___ref (j, 1) + v2 * c___ref (j, 2) + v3 * c___ref (j, 3) + v4 * c___ref (j, 4);
			c___ref (j, 1) = c___ref (j, 1) - sum * t1;
			c___ref (j, 2) = c___ref (j, 2) - sum * t2;
			c___ref (j, 3) = c___ref (j, 3) - sum * t3;
			c___ref (j, 4) = c___ref (j, 4) - sum * t4;
			/* L280: */
		}
		goto L410;
L290:

		/* Special code for 5 x 5 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		v5 = v[5];
		t5 = *tau * v5;
		i__1 = *m;
		for (j = 1; j <= i__1; ++j) {
			sum =
			    v1 * c___ref (j, 1) + v2 * c___ref (j, 2) + v3 * c___ref (j, 3) + v4 * c___ref (j,
			            4) + v5 * c___ref (j, 5);
			c___ref (j, 1) = c___ref (j, 1) - sum * t1;
			c___ref (j, 2) = c___ref (j, 2) - sum * t2;
			c___ref (j, 3) = c___ref (j, 3) - sum * t3;
			c___ref (j, 4) = c___ref (j, 4) - sum * t4;
			c___ref (j, 5) = c___ref (j, 5) - sum * t5;
			/* L300: */
		}
		goto L410;
L310:

		/* Special code for 6 x 6 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		v5 = v[5];
		t5 = *tau * v5;
		v6 = v[6];
		t6 = *tau * v6;
		i__1 = *m;
		for (j = 1; j <= i__1; ++j) {
			sum =
			    v1 * c___ref (j, 1) + v2 * c___ref (j, 2) + v3 * c___ref (j, 3) + v4 * c___ref (j,
			            4) + v5 * c___ref (j, 5) + v6 * c___ref (j, 6);
			c___ref (j, 1) = c___ref (j, 1) - sum * t1;
			c___ref (j, 2) = c___ref (j, 2) - sum * t2;
			c___ref (j, 3) = c___ref (j, 3) - sum * t3;
			c___ref (j, 4) = c___ref (j, 4) - sum * t4;
			c___ref (j, 5) = c___ref (j, 5) - sum * t5;
			c___ref (j, 6) = c___ref (j, 6) - sum * t6;
			/* L320: */
		}
		goto L410;
L330:

		/* Special code for 7 x 7 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		v5 = v[5];
		t5 = *tau * v5;
		v6 = v[6];
		t6 = *tau * v6;
		v7 = v[7];
		t7 = *tau * v7;
		i__1 = *m;
		for (j = 1; j <= i__1; ++j) {
			sum =
			    v1 * c___ref (j, 1) + v2 * c___ref (j, 2) + v3 * c___ref (j, 3) + v4 * c___ref (j,
			            4) + v5 * c___ref (j, 5) + v6 * c___ref (j, 6) + v7 * c___ref (j, 7);
			c___ref (j, 1) = c___ref (j, 1) - sum * t1;
			c___ref (j, 2) = c___ref (j, 2) - sum * t2;
			c___ref (j, 3) = c___ref (j, 3) - sum * t3;
			c___ref (j, 4) = c___ref (j, 4) - sum * t4;
			c___ref (j, 5) = c___ref (j, 5) - sum * t5;
			c___ref (j, 6) = c___ref (j, 6) - sum * t6;
			c___ref (j, 7) = c___ref (j, 7) - sum * t7;
			/* L340: */
		}
		goto L410;
L350:

		/* Special code for 8 x 8 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		v5 = v[5];
		t5 = *tau * v5;
		v6 = v[6];
		t6 = *tau * v6;
		v7 = v[7];
		t7 = *tau * v7;
		v8 = v[8];
		t8 = *tau * v8;
		i__1 = *m;
		for (j = 1; j <= i__1; ++j) {
			sum =
			    v1 * c___ref (j, 1) + v2 * c___ref (j, 2) + v3 * c___ref (j, 3) + v4 * c___ref (j,
			            4) + v5 * c___ref (j, 5) + v6 * c___ref (j, 6) + v7 * c___ref (j, 7) + v8 * c___ref (j, 8);
			c___ref (j, 1) = c___ref (j, 1) - sum * t1;
			c___ref (j, 2) = c___ref (j, 2) - sum * t2;
			c___ref (j, 3) = c___ref (j, 3) - sum * t3;
			c___ref (j, 4) = c___ref (j, 4) - sum * t4;
			c___ref (j, 5) = c___ref (j, 5) - sum * t5;
			c___ref (j, 6) = c___ref (j, 6) - sum * t6;
			c___ref (j, 7) = c___ref (j, 7) - sum * t7;
			c___ref (j, 8) = c___ref (j, 8) - sum * t8;
			/* L360: */
		}
		goto L410;
L370:

		/* Special code for 9 x 9 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		v5 = v[5];
		t5 = *tau * v5;
		v6 = v[6];
		t6 = *tau * v6;
		v7 = v[7];
		t7 = *tau * v7;
		v8 = v[8];
		t8 = *tau * v8;
		v9 = v[9];
		t9 = *tau * v9;
		i__1 = *m;
		for (j = 1; j <= i__1; ++j) {
			sum =
			    v1 * c___ref (j, 1) + v2 * c___ref (j, 2) + v3 * c___ref (j, 3) + v4 * c___ref (j,
			            4) + v5 * c___ref (j, 5) + v6 * c___ref (j, 6) + v7 * c___ref (j, 7) + v8 * c___ref (j,
			                    8) + v9 * c___ref (j, 9);
			c___ref (j, 1) = c___ref (j, 1) - sum * t1;
			c___ref (j, 2) = c___ref (j, 2) - sum * t2;
			c___ref (j, 3) = c___ref (j, 3) - sum * t3;
			c___ref (j, 4) = c___ref (j, 4) - sum * t4;
			c___ref (j, 5) = c___ref (j, 5) - sum * t5;
			c___ref (j, 6) = c___ref (j, 6) - sum * t6;
			c___ref (j, 7) = c___ref (j, 7) - sum * t7;
			c___ref (j, 8) = c___ref (j, 8) - sum * t8;
			c___ref (j, 9) = c___ref (j, 9) - sum * t9;
			/* L380: */
		}
		goto L410;
L390:

		/* Special code for 10 x 10 Householder */

		v1 = v[1];
		t1 = *tau * v1;
		v2 = v[2];
		t2 = *tau * v2;
		v3 = v[3];
		t3 = *tau * v3;
		v4 = v[4];
		t4 = *tau * v4;
		v5 = v[5];
		t5 = *tau * v5;
		v6 = v[6];
		t6 = *tau * v6;
		v7 = v[7];
		t7 = *tau * v7;
		v8 = v[8];
		t8 = *tau * v8;
		v9 = v[9];
		t9 = *tau * v9;
		v10 = v[10];
		t10 = *tau * v10;
		i__1 = *m;
		for (j = 1; j <= i__1; ++j) {
			sum =
			    v1 * c___ref (j, 1) + v2 * c___ref (j, 2) + v3 * c___ref (j, 3) + v4 * c___ref (j,
			            4) + v5 * c___ref (j, 5) + v6 * c___ref (j, 6) + v7 * c___ref (j, 7) + v8 * c___ref (j,
			                    8) + v9 * c___ref (j, 9) + v10 * c___ref (j, 10);
			c___ref (j, 1) = c___ref (j, 1) - sum * t1;
			c___ref (j, 2) = c___ref (j, 2) - sum * t2;
			c___ref (j, 3) = c___ref (j, 3) - sum * t3;
			c___ref (j, 4) = c___ref (j, 4) - sum * t4;
			c___ref (j, 5) = c___ref (j, 5) - sum * t5;
			c___ref (j, 6) = c___ref (j, 6) - sum * t6;
			c___ref (j, 7) = c___ref (j, 7) - sum * t7;
			c___ref (j, 8) = c___ref (j, 8) - sum * t8;
			c___ref (j, 9) = c___ref (j, 9) - sum * t9;
			c___ref (j, 10) = c___ref (j, 10) - sum * t10;
			/* L400: */
		}
		goto L410;
	}
L410:
	return 0;
}								/* NUMlapack_dlarfx */


int NUMlapack_dlas2 (double *f, double *g, double *h__, double *ssmin, double *ssmax) {
	/* System generated locals */
	double d__1, d__2;

	/* Local variables */
	static double fhmn, fhmx, c__, fa, ga, ha, as, at, au;

	fa = fabs (*f);
	ga = fabs (*g);
	ha = fabs (*h__);
	fhmn = MIN (fa, ha);
	fhmx = MAX (fa, ha);
	if (fhmn == 0.) {
		*ssmin = 0.;
		if (fhmx == 0.) {
			*ssmax = ga;
		} else {
			/* Computing 2nd power */
			d__1 = MIN (fhmx, ga) / MAX (fhmx, ga);
			*ssmax = MAX (fhmx, ga) * sqrt (d__1 * d__1 + 1.);
		}
	} else {
		if (ga < fhmx) {
			as = fhmn / fhmx + 1.;
			at = (fhmx - fhmn) / fhmx;
			/* Computing 2nd power */
			d__1 = ga / fhmx;
			au = d__1 * d__1;
			c__ = 2. / (sqrt (as * as + au) + sqrt (at * at + au));
			*ssmin = fhmn * c__;
			*ssmax = fhmx / c__;
		} else {
			au = fhmx / ga;
			if (au == 0.) {

				/* Avoid possible harmful underflow if exponent range
				   asymmetric (true SSMIN may not underflow even if AU
				   underflows) */

				*ssmin = fhmn * fhmx / ga;
				*ssmax = ga;
			} else {
				as = fhmn / fhmx + 1.;
				at = (fhmx - fhmn) / fhmx;
				/* Computing 2nd power */
				d__1 = as * au;
				/* Computing 2nd power */
				d__2 = at * au;
				c__ = 1. / (sqrt (d__1 * d__1 + 1.) + sqrt (d__2 * d__2 + 1.));
				*ssmin = fhmn * c__ * au;
				*ssmin += *ssmin;
				*ssmax = ga / (c__ + c__);
			}
		}
	}
	return 0;
}								/* NUMlapack_dlas2 */

int NUMlapack_dlascl (const char *type__, long *kl, long *ku, double *cfrom, double *cto, long *m, long *n, double *a,
                      long *lda, long *info) {
	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5;

	/* Local variables */
	static long done;
	static double ctoc;
	static long i__, j;
	static long itype, k1, k2, k3, k4;
	static double cfrom1;
	static double cfromc;
	static double bignum, smlnum, mul, cto1;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;

	/* Function Body */
	*info = 0;

	if (lsame_ (type__, "G")) {
		itype = 0;
	} else if (lsame_ (type__, "L")) {
		itype = 1;
	} else if (lsame_ (type__, "U")) {
		itype = 2;
	} else if (lsame_ (type__, "H")) {
		itype = 3;
	} else if (lsame_ (type__, "B")) {
		itype = 4;
	} else if (lsame_ (type__, "Q")) {
		itype = 5;
	} else if (lsame_ (type__, "Z")) {
		itype = 6;
	} else {
		itype = -1;
	}

	if (itype == -1) {
		*info = -1;
	} else if (*cfrom == 0.) {
		*info = -4;
	} else if (*m < 0) {
		*info = -6;
	} else if (*n < 0 || itype == 4 && *n != *m || itype == 5 && *n != *m) {
		*info = -7;
	} else if (itype <= 3 && *lda < MAX (1, *m)) {
		*info = -9;
	} else if (itype >= 4) {
		/* Computing MAX */
		i__1 = *m - 1;
		if (*kl < 0 || *kl > MAX (i__1, 0)) {
			*info = -2;
		} else {				/* if(complicated condition) */
			/* Computing MAX */
			i__1 = *n - 1;
			if (*ku < 0 || *ku > MAX (i__1, 0) || (itype == 4 || itype == 5) && *kl != *ku) {
				*info = -3;
			} else if (itype == 4 && *lda < *kl + 1 || itype == 5 && *lda < *ku + 1 || itype == 6 &&
			           *lda < (*kl << 1) + *ku + 1) {
				*info = -9;
			}
		}
	}

	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DLASCL", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*n == 0 || *m == 0) {
		return 0;
	}

	/* Get machine parameters */

	smlnum = NUMblas_dlamch ("S");
	bignum = 1. / smlnum;

	cfromc = *cfrom;
	ctoc = *cto;

L10:
	cfrom1 = cfromc * smlnum;
	cto1 = ctoc / bignum;
	if (fabs (cfrom1) > fabs (ctoc) && ctoc != 0.) {
		mul = smlnum;
		done = FALSE;
		cfromc = cfrom1;
	} else if (fabs (cto1) > fabs (cfromc)) {
		mul = bignum;
		done = FALSE;
		ctoc = cto1;
	} else {
		mul = ctoc / cfromc;
		done = TRUE;
	}

	if (itype == 0) {

		/* Full matrix */

		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = a_ref (i__, j) * mul;
				/* L20: */
			}
			/* L30: */
		}

	} else if (itype == 1) {

		/* Lower triangular matrix */

		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = j; i__ <= i__2; ++i__) {
				a_ref (i__, j) = a_ref (i__, j) * mul;
				/* L40: */
			}
			/* L50: */
		}

	} else if (itype == 2) {

		/* Upper triangular matrix */

		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			i__2 = MIN (j, *m);
			for (i__ = 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = a_ref (i__, j) * mul;
				/* L60: */
			}
			/* L70: */
		}

	} else if (itype == 3) {

		/* Upper Hessenberg matrix */

		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			/* Computing MIN */
			i__3 = j + 1;
			i__2 = MIN (i__3, *m);
			for (i__ = 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = a_ref (i__, j) * mul;
				/* L80: */
			}
			/* L90: */
		}

	} else if (itype == 4) {

		/* Lower half of a symmetric band matrix */

		k3 = *kl + 1;
		k4 = *n + 1;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			/* Computing MIN */
			i__3 = k3, i__4 = k4 - j;
			i__2 = MIN (i__3, i__4);
			for (i__ = 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = a_ref (i__, j) * mul;
				/* L100: */
			}
			/* L110: */
		}

	} else if (itype == 5) {

		/* Upper half of a symmetric band matrix */

		k1 = *ku + 2;
		k3 = *ku + 1;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			/* Computing MAX */
			i__2 = k1 - j;
			i__3 = k3;
			for (i__ = MAX (i__2, 1); i__ <= i__3; ++i__) {
				a_ref (i__, j) = a_ref (i__, j) * mul;
				/* L120: */
			}
			/* L130: */
		}

	} else if (itype == 6) {

		/* Band matrix */

		k1 = *kl + *ku + 2;
		k2 = *kl + 1;
		k3 = (*kl << 1) + *ku + 1;
		k4 = *kl + *ku + 1 + *m;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			/* Computing MAX */
			i__3 = k1 - j;
			/* Computing MIN */
			i__4 = k3, i__5 = k4 - j;
			i__2 = MIN (i__4, i__5);
			for (i__ = MAX (i__3, k2); i__ <= i__2; ++i__) {
				a_ref (i__, j) = a_ref (i__, j) * mul;
				/* L140: */
			}
			/* L150: */
		}

	}

	if (!done) {
		goto L10;
	}

	return 0;
}								/* NUMlapack_dlascl */

int NUMlapack_dlaset (const char *uplo, long *m, long *n, double *alpha, double *beta, double *a, long *lda) {
	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;

	/* Local variables */
	static long i__, j;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;

	/* Function Body */
	if (lsame_ (uplo, "U")) {

		/* Set the strictly upper triangular or trapezoidal part of the array
		   to ALPHA. */

		i__1 = *n;
		for (j = 2; j <= i__1; ++j) {
			/* Computing MIN */
			i__3 = j - 1;
			i__2 = MIN (i__3, *m);
			for (i__ = 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = *alpha;
				/* L10: */
			}
			/* L20: */
		}

	} else if (lsame_ (uplo, "L")) {

		/* Set the strictly lower triangular or trapezoidal part of the array
		   to ALPHA. */

		i__1 = MIN (*m, *n);
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = j + 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = *alpha;
				/* L30: */
			}
			/* L40: */
		}

	} else {

		/* Set the leading m-by-n submatrix to ALPHA. */

		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = *alpha;
				/* L50: */
			}
			/* L60: */
		}
	}

	/* Set the first MIN(M,N) diagonal elements to BETA. */

	i__1 = MIN (*m, *n);
	for (i__ = 1; i__ <= i__1; ++i__) {
		a_ref (i__, i__) = *beta;
		/* L70: */
	}

	return 0;
}								/* NUMlapack_dlaset */

int NUMlapack_dlasq1 (long *n, double *d__, double *e, double *work, long *info) {
	/* System generated locals */
	long i__1, i__2;
	double d__1, d__2, d__3;

	/* Local variables */
	static long i__;
	static double scale;
	static long iinfo;
	static double sigmn;
	static double sigmx;
	static double safmin;
	static double eps;

	/* Parameter adjustments */
	--work;
	--e;
	--d__;

	/* Function Body */
	*info = 0;
	if (*n < 0) {
		*info = -2;
		i__1 = - (*info);
		xerbla_ ("DLASQ1", &i__1);
		return 0;
	} else if (*n == 0) {
		return 0;
	} else if (*n == 1) {
		d__[1] = fabs (d__[1]);
		return 0;
	} else if (*n == 2) {
		NUMlapack_dlas2 (&d__[1], &e[1], &d__[2], &sigmn, &sigmx);
		d__[1] = sigmx;
		d__[2] = sigmn;
		return 0;
	}

	/* Estimate the largest singular value. */

	sigmx = 0.;
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
		d__[i__] = (d__1 = d__[i__], fabs (d__1));
		/* Computing MAX */
		d__2 = sigmx, d__3 = (d__1 = e[i__], fabs (d__1));
		sigmx = MAX (d__2, d__3);
		/* L10: */
	}
	d__[*n] = (d__1 = d__[*n], fabs (d__1));

	/* Early return if SIGMX is zero (matrix is already diagonal). */

	if (sigmx == 0.) {
		NUMlapack_dlasrt ("D", n, &d__[1], &iinfo);
		return 0;
	}

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		/* Computing MAX */
		d__1 = sigmx, d__2 = d__[i__];
		sigmx = MAX (d__1, d__2);
		/* L20: */
	}

	/* Copy D and E into WORK (in the Z format) and scale (squaring the input
	   data makes scaling by a power of the radix pointless). */

	eps = NUMblas_dlamch ("Precision");
	safmin = NUMblas_dlamch ("Safe minimum");
	scale = sqrt (eps / safmin);
	NUMblas_dcopy (n, &d__[1], &c__1, &work[1], &c__2);
	i__1 = *n - 1;
	NUMblas_dcopy (&i__1, &e[1], &c__1, &work[2], &c__2);
	i__1 = (*n << 1) - 1;
	i__2 = (*n << 1) - 1;
	NUMlapack_dlascl ("G", &c__0, &c__0, &sigmx, &scale, &i__1, &c__1, &work[1], &i__2, &iinfo);

	/* Compute the q's and e's. */

	i__1 = (*n << 1) - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
		/* Computing 2nd power */
		d__1 = work[i__];
		work[i__] = d__1 * d__1;
		/* L30: */
	}
	work[*n * 2] = 0.;

	NUMlapack_dlasq2 (n, &work[1], info);

	if (*info == 0) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
			d__[i__] = sqrt (work[i__]);
			/* L40: */
		}
		NUMlapack_dlascl ("G", &c__0, &c__0, &scale, &sigmx, n, &c__1, &d__[1], n, &iinfo);
	}

	return 0;

}								/* NUMlapack_dlasq1 */

int NUMlapack_dlasq2 (long *n, double *z__, long *info) {
	/* System generated locals */
	long i__1, i__2, i__3;
	double d__1, d__2;

	/* Local variables */
	static long ieee;
	static long nbig;
	static double dmin__, emin, emax;
	static long ndiv, iter;
	static double qmin, temp, qmax, zmax;
	static long splt;
	static double d__, e;
	static long k;
	static double s, t;
	static long nfail;
	static double desig, trace, sigma;
	static long iinfo, i0, i4, n0;
	static long pp, iwhila, iwhilb;
	static double oldemn, safmin;
	static double eps, tol;
	static long ipn4;
	static double tol2;

	/* Parameter adjustments */
	--z__;

	/* Function Body */
	*info = 0;
	eps = NUMblas_dlamch ("Precision");
	safmin = NUMblas_dlamch ("Safe minimum");
	tol = eps * 100.;
	/* Computing 2nd power */
	d__1 = tol;
	tol2 = d__1 * d__1;

	if (*n < 0) {
		*info = -1;
		xerbla_ ("DLASQ2", &c__1);
		return 0;
	} else if (*n == 0) {
		return 0;
	} else if (*n == 1) {

		/* 1-by-1 case. */

		if (z__[1] < 0.) {
			*info = -201;
			xerbla_ ("DLASQ2", &c__2);
		}
		return 0;
	} else if (*n == 2) {

		/* 2-by-2 case. */

		if (z__[2] < 0. || z__[3] < 0.) {
			*info = -2;
			xerbla_ ("DLASQ2", &c__2);
			return 0;
		} else if (z__[3] > z__[1]) {
			d__ = z__[3];
			z__[3] = z__[1];
			z__[1] = d__;
		}
		z__[5] = z__[1] + z__[2] + z__[3];
		if (z__[2] > z__[3] * tol2) {
			t = (z__[1] - z__[3] + z__[2]) * .5;
			s = z__[3] * (z__[2] / t);
			if (s <= t) {
				s = z__[3] * (z__[2] / (t * (sqrt (s / t + 1.) + 1.)));
			} else {
				s = z__[3] * (z__[2] / (t + sqrt (t) * sqrt (t + s)));
			}
			t = z__[1] + (s + z__[2]);
			z__[3] *= z__[1] / t;
			z__[1] = t;
		}
		z__[2] = z__[3];
		z__[6] = z__[2] + z__[1];
		return 0;
	}

	/* Check for negative data and compute sums of q's and e's. */

	z__[*n * 2] = 0.;
	emin = z__[2];
	qmax = 0.;
	zmax = 0.;
	d__ = 0.;
	e = 0.;

	i__1 = *n - 1 << 1;
	for (k = 1; k <= i__1; k += 2) {
		if (z__[k] < 0.) {
			*info = - (k + 200);
			xerbla_ ("DLASQ2", &c__2);
			return 0;
		} else if (z__[k + 1] < 0.) {
			*info = - (k + 201);
			xerbla_ ("DLASQ2", &c__2);
			return 0;
		}
		d__ += z__[k];
		e += z__[k + 1];
		/* Computing MAX */
		d__1 = qmax, d__2 = z__[k];
		qmax = MAX (d__1, d__2);
		/* Computing MIN */
		d__1 = emin, d__2 = z__[k + 1];
		emin = MIN (d__1, d__2);
		/* Computing MAX */
		d__1 = MAX (qmax, zmax), d__2 = z__[k + 1];
		zmax = MAX (d__1, d__2);
		/* L10: */
	}
	if (z__[ (*n << 1) - 1] < 0.) {
		*info = - ( (*n << 1) + 199);
		xerbla_ ("DLASQ2", &c__2);
		return 0;
	}
	d__ += z__[ (*n << 1) - 1];
	/* Computing MAX */
	d__1 = qmax, d__2 = z__[ (*n << 1) - 1];
	qmax = MAX (d__1, d__2);
	zmax = MAX (qmax, zmax);

	/* Check for diagonality. */

	if (e == 0.) {
		i__1 = *n;
		for (k = 2; k <= i__1; ++k) {
			z__[k] = z__[ (k << 1) - 1];
			/* L20: */
		}
		NUMlapack_dlasrt ("D", n, &z__[1], &iinfo);
		z__[ (*n << 1) - 1] = d__;
		return 0;
	}

	trace = d__ + e;

	/* Check for zero data. */

	if (trace == 0.) {
		z__[ (*n << 1) - 1] = 0.;
		return 0;
	}

	/* Check whether the machine is IEEE conformable. */

	ieee = NUMlapack_ilaenv (&c__10, "DLASQ2", "N", &c__1, &c__2, &c__3, &c__4, 6, 1) == 1 &&
	       NUMlapack_ilaenv (&c__11, "DLASQ2", "N", &c__1, &c__2, &c__3, &c__4, 6, 1) == 1;

	/* Rearrange data for locality: Z=(q1,qq1,e1,ee1,q2,qq2,e2,ee2,...). */

	for (k = *n << 1; k >= 2; k += -2) {
		z__[k * 2] = 0.;
		z__[ (k << 1) - 1] = z__[k];
		z__[ (k << 1) - 2] = 0.;
		z__[ (k << 1) - 3] = z__[k - 1];
		/* L30: */
	}

	i0 = 1;
	n0 = *n;

	/* Reverse the qd-array, if warranted. */

	if (z__[ (i0 << 2) - 3] * 1.5 < z__[ (n0 << 2) - 3]) {
		ipn4 = i0 + n0 << 2;
		i__1 = i0 + n0 - 1 << 1;
		for (i4 = i0 << 2; i4 <= i__1; i4 += 4) {
			temp = z__[i4 - 3];
			z__[i4 - 3] = z__[ipn4 - i4 - 3];
			z__[ipn4 - i4 - 3] = temp;
			temp = z__[i4 - 1];
			z__[i4 - 1] = z__[ipn4 - i4 - 5];
			z__[ipn4 - i4 - 5] = temp;
			/* L40: */
		}
	}

	/* Initial split checking via dqd and Li's test. */

	pp = 0;

	for (k = 1; k <= 2; ++k) {

		d__ = z__[ (n0 << 2) + pp - 3];
		i__1 = (i0 << 2) + pp;
		for (i4 = (n0 - 1 << 2) + pp; i4 >= i__1; i4 += -4) {
			if (z__[i4 - 1] <= tol2 * d__) {
				z__[i4 - 1] = 0.;
				d__ = z__[i4 - 3];
			} else {
				d__ = z__[i4 - 3] * (d__ / (d__ + z__[i4 - 1]));
			}
			/* L50: */
		}

		/* dqd maps Z to ZZ plus Li's test. */

		emin = z__[ (i0 << 2) + pp + 1];
		d__ = z__[ (i0 << 2) + pp - 3];
		i__1 = (n0 - 1 << 2) + pp;
		for (i4 = (i0 << 2) + pp; i4 <= i__1; i4 += 4) {
			z__[i4 - (pp << 1) - 2] = d__ + z__[i4 - 1];
			if (z__[i4 - 1] <= tol2 * d__) {
				z__[i4 - 1] = 0.;
				z__[i4 - (pp << 1) - 2] = d__;
				z__[i4 - (pp << 1)] = 0.;
				d__ = z__[i4 + 1];
			} else if (safmin * z__[i4 + 1] < z__[i4 - (pp << 1) - 2] &&
			           safmin * z__[i4 - (pp << 1) - 2] < z__[i4 + 1]) {
				temp = z__[i4 + 1] / z__[i4 - (pp << 1) - 2];
				z__[i4 - (pp << 1)] = z__[i4 - 1] * temp;
				d__ *= temp;
			} else {
				z__[i4 - (pp << 1)] = z__[i4 + 1] * (z__[i4 - 1] / z__[i4 - (pp << 1) - 2]);
				d__ = z__[i4 + 1] * (d__ / z__[i4 - (pp << 1) - 2]);
			}
			/* Computing MIN */
			d__1 = emin, d__2 = z__[i4 - (pp << 1)];
			emin = MIN (d__1, d__2);
			/* L60: */
		}
		z__[ (n0 << 2) - pp - 2] = d__;

		/* Now find qmax. */

		qmax = z__[ (i0 << 2) - pp - 2];
		i__1 = (n0 << 2) - pp - 2;
		for (i4 = (i0 << 2) - pp + 2; i4 <= i__1; i4 += 4) {
			/* Computing MAX */
			d__1 = qmax, d__2 = z__[i4];
			qmax = MAX (d__1, d__2);
			/* L70: */
		}

		/* Prepare for the next iteration on K. */

		pp = 1 - pp;
		/* L80: */
	}

	iter = 2;
	nfail = 0;
	ndiv = n0 - i0 << 1;

	i__1 = *n + 1;
	for (iwhila = 1; iwhila <= i__1; ++iwhila) {
		if (n0 < 1) {
			goto L150;
		}

		/* While array unfinished do

		   E(N0) holds the value of SIGMA when submatrix in I0:N0 splits from
		   the rest of the array, but is negated. */

		desig = 0.;
		if (n0 == *n) {
			sigma = 0.;
		} else {
			sigma = -z__[ (n0 << 2) - 1];
		}
		if (sigma < 0.) {
			*info = 1;
			return 0;
		}

		/* Find last unreduced submatrix's top index I0, find QMAX and EMIN.
		   Find Gershgorin-type bound if Q's much greater than E's. */

		emax = 0.;
		if (n0 > i0) {
			emin = (d__1 = z__[ (n0 << 2) - 5], fabs (d__1));
		} else {
			emin = 0.;
		}
		qmin = z__[ (n0 << 2) - 3];
		qmax = qmin;
		for (i4 = n0 << 2; i4 >= 8; i4 += -4) {
			if (z__[i4 - 5] <= 0.) {
				goto L100;
			}
			if (qmin >= emax * 4.) {
				/* Computing MIN */
				d__1 = qmin, d__2 = z__[i4 - 3];
				qmin = MIN (d__1, d__2);
				/* Computing MAX */
				d__1 = emax, d__2 = z__[i4 - 5];
				emax = MAX (d__1, d__2);
			}
			/* Computing MAX */
			d__1 = qmax, d__2 = z__[i4 - 7] + z__[i4 - 5];
			qmax = MAX (d__1, d__2);
			/* Computing MIN */
			d__1 = emin, d__2 = z__[i4 - 5];
			emin = MIN (d__1, d__2);
			/* L90: */
		}
		i4 = 4;

L100:
		i0 = i4 / 4;

		/* Store EMIN for passing to DLASQ3. */

		z__[ (n0 << 2) - 1] = emin;

		/* Put -(initial shift) into DMIN.

		   Computing MAX */
		d__1 = 0., d__2 = qmin - sqrt (qmin) * 2. * sqrt (emax);
		dmin__ = -MAX (d__1, d__2);

		/* Now I0:N0 is unreduced. PP = 0 for ping, PP = 1 for pong. */

		pp = 0;

		nbig = (n0 - i0 + 1) * 30;
		i__2 = nbig;
		for (iwhilb = 1; iwhilb <= i__2; ++iwhilb) {
			if (i0 > n0) {
				goto L130;
			}

			/* While submatrix unfinished take a good dqds step. */

			NUMlapack_dlasq3 (&i0, &n0, &z__[1], &pp, &dmin__, &sigma, &desig, &qmax, &nfail, &iter, &ndiv,
			                  &ieee);

			pp = 1 - pp;

			/* When EMIN is very small check for splits. */

			if (pp == 0 && n0 - i0 >= 3) {
				if (z__[n0 * 4] <= tol2 * qmax || z__[ (n0 << 2) - 1] <= tol2 * sigma) {
					splt = i0 - 1;
					qmax = z__[ (i0 << 2) - 3];
					emin = z__[ (i0 << 2) - 1];
					oldemn = z__[i0 * 4];
					i__3 = n0 - 3 << 2;
					for (i4 = i0 << 2; i4 <= i__3; i4 += 4) {
						if (z__[i4] <= tol2 * z__[i4 - 3] || z__[i4 - 1] <= tol2 * sigma) {
							z__[i4 - 1] = -sigma;
							splt = i4 / 4;
							qmax = 0.;
							emin = z__[i4 + 3];
							oldemn = z__[i4 + 4];
						} else {
							/* Computing MAX */
							d__1 = qmax, d__2 = z__[i4 + 1];
							qmax = MAX (d__1, d__2);
							/* Computing MIN */
							d__1 = emin, d__2 = z__[i4 - 1];
							emin = MIN (d__1, d__2);
							/* Computing MIN */
							d__1 = oldemn, d__2 = z__[i4];
							oldemn = MIN (d__1, d__2);
						}
						/* L110: */
					}
					z__[ (n0 << 2) - 1] = emin;
					z__[n0 * 4] = oldemn;
					i0 = splt + 1;
				}
			}

			/* L120: */
		}

		*info = 2;
		return 0;

		/* end IWHILB */

L130:

		/* L140: */
		;
	}

	*info = 3;
	return 0;

	/* end IWHILA */

L150:

	/* Move q's to the front. */

	i__1 = *n;
	for (k = 2; k <= i__1; ++k) {
		z__[k] = z__[ (k << 2) - 3];
		/* L160: */
	}

	/* Sort and compute sum of eigenvalues. */

	NUMlapack_dlasrt ("D", n, &z__[1], &iinfo);

	e = 0.;
	for (k = *n; k >= 1; --k) {
		e += z__[k];
		/* L170: */
	}

	/* Store trace, sum(eigenvalues) and information on performance. */

	z__[ (*n << 1) + 1] = trace;
	z__[ (*n << 1) + 2] = e;
	z__[ (*n << 1) + 3] = (double) iter;
	/* Computing 2nd power */
	i__1 = *n;
	z__[ (*n << 1) + 4] = (double) ndiv / (double) (i__1 * i__1);
	z__[ (*n << 1) + 5] = nfail * 100. / (double) iter;
	return 0;
}								/* NUMlapack_dlasq2 */

int NUMlapack_dlasq3 (long *i0, long *n0, double *z__, long *pp, double *dmin__, double *sigma, double *desig,
                      double *qmax, long *nfail, long *iter, long *ndiv, long *ieee) {
	/* Initialized data */
	static long ttype = 0;
	static double dmin1 = 0.;
	static double dmin2 = 0.;
	static double dn = 0.;
	static double dn1 = 0.;
	static double dn2 = 0.;
	static double tau = 0.;

	/* System generated locals */
	long i__1;
	double d__1, d__2;

	/* Local variables */
	static double temp, s, t;
	static long j4;
	static long nn;
	static double safmin, eps, tol;
	static long n0in, ipn4;
	static double tol2;

	--z__;

	/* Function Body */

	n0in = *n0;
	eps = NUMblas_dlamch ("Precision");
	safmin = NUMblas_dlamch ("Safe minimum");
	tol = eps * 100.;
	/* Computing 2nd power */
	d__1 = tol;
	tol2 = d__1 * d__1;

	/* Check for deflation. */

L10:

	if (*n0 < *i0) {
		return 0;
	}
	if (*n0 == *i0) {
		goto L20;
	}
	nn = (*n0 << 2) + *pp;
	if (*n0 == *i0 + 1) {
		goto L40;
	}

	/* Check whether E(N0-1) is negligible, 1 eigenvalue. */

	if (z__[nn - 5] > tol2 * (*sigma + z__[nn - 3]) && z__[nn - (*pp << 1) - 4] > tol2 * z__[nn - 7]) {
		goto L30;
	}

L20:

	z__[ (*n0 << 2) - 3] = z__[ (*n0 << 2) + *pp - 3] + *sigma;
	-- (*n0);
	goto L10;

	/* Check whether E(N0-2) is negligible, 2 eigenvalues. */

L30:

	if (z__[nn - 9] > tol2 * *sigma && z__[nn - (*pp << 1) - 8] > tol2 * z__[nn - 11]) {
		goto L50;
	}

L40:

	if (z__[nn - 3] > z__[nn - 7]) {
		s = z__[nn - 3];
		z__[nn - 3] = z__[nn - 7];
		z__[nn - 7] = s;
	}
	if (z__[nn - 5] > z__[nn - 3] * tol2) {
		t = (z__[nn - 7] - z__[nn - 3] + z__[nn - 5]) * .5;
		s = z__[nn - 3] * (z__[nn - 5] / t);
		if (s <= t) {
			s = z__[nn - 3] * (z__[nn - 5] / (t * (sqrt (s / t + 1.) + 1.)));
		} else {
			s = z__[nn - 3] * (z__[nn - 5] / (t + sqrt (t) * sqrt (t + s)));
		}
		t = z__[nn - 7] + (s + z__[nn - 5]);
		z__[nn - 3] *= z__[nn - 7] / t;
		z__[nn - 7] = t;
	}
	z__[ (*n0 << 2) - 7] = z__[nn - 7] + *sigma;
	z__[ (*n0 << 2) - 3] = z__[nn - 3] + *sigma;
	*n0 += -2;
	goto L10;

L50:

	/* Reverse the qd-array, if warranted. */

	if (*dmin__ <= 0. || *n0 < n0in) {
		if (z__[ (*i0 << 2) + *pp - 3] * 1.5 < z__[ (*n0 << 2) + *pp - 3]) {
			ipn4 = *i0 + *n0 << 2;
			i__1 = *i0 + *n0 - 1 << 1;
			for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
				temp = z__[j4 - 3];
				z__[j4 - 3] = z__[ipn4 - j4 - 3];
				z__[ipn4 - j4 - 3] = temp;
				temp = z__[j4 - 2];
				z__[j4 - 2] = z__[ipn4 - j4 - 2];
				z__[ipn4 - j4 - 2] = temp;
				temp = z__[j4 - 1];
				z__[j4 - 1] = z__[ipn4 - j4 - 5];
				z__[ipn4 - j4 - 5] = temp;
				temp = z__[j4];
				z__[j4] = z__[ipn4 - j4 - 4];
				z__[ipn4 - j4 - 4] = temp;
				/* L60: */
			}
			if (*n0 - *i0 <= 4) {
				z__[ (*n0 << 2) + *pp - 1] = z__[ (*i0 << 2) + *pp - 1];
				z__[ (*n0 << 2) - *pp] = z__[ (*i0 << 2) - *pp];
			}
			/* Computing MIN */
			d__1 = dmin2, d__2 = z__[ (*n0 << 2) + *pp - 1];
			dmin2 = MIN (d__1, d__2);
			/* Computing MIN */
			d__1 = z__[ (*n0 << 2) + *pp - 1], d__2 = z__[ (*i0 << 2) + *pp - 1], d__1 =
			            MIN (d__1, d__2), d__2 = z__[ (*i0 << 2) + *pp + 3];
			z__[ (*n0 << 2) + *pp - 1] = MIN (d__1, d__2);
			/* Computing MIN */
			d__1 = z__[ (*n0 << 2) - *pp], d__2 = z__[ (*i0 << 2) - *pp], d__1 = MIN (d__1, d__2), d__2 =
			        z__[ (*i0 << 2) - *pp + 4];
			z__[ (*n0 << 2) - *pp] = MIN (d__1, d__2);
			/* Computing MAX */
			d__1 = *qmax, d__2 = z__[ (*i0 << 2) + *pp - 3], d__1 = MAX (d__1, d__2), d__2 =
			                         z__[ (*i0 << 2) + *pp + 1];
			*qmax = MAX (d__1, d__2);
			*dmin__ = 0.;
		}
	}

	/* L70:

	   Computing MIN */
	d__1 = z__[ (*n0 << 2) + *pp - 1], d__2 = z__[ (*n0 << 2) + *pp - 9], d__1 = MIN (d__1, d__2), d__2 =
	            dmin2 + z__[ (*n0 << 2) - *pp];
	if (*dmin__ < 0. || safmin * *qmax < MIN (d__1, d__2)) {

		/* Choose a shift. */

		NUMlapack_dlasq4 (i0, n0, &z__[1], pp, &n0in, dmin__, &dmin1, &dmin2, &dn, &dn1, &dn2, &tau, &ttype);

		/* Call dqds until DMIN > 0. */

L80:

		NUMlapack_dlasq5 (i0, n0, &z__[1], pp, &tau, dmin__, &dmin1, &dmin2, &dn, &dn1, &dn2, ieee);

		*ndiv += *n0 - *i0 + 2;
		++ (*iter);

		/* Check status. */

		if (*dmin__ >= 0. && dmin1 > 0.) {

			/* Success. */

			goto L100;

		} else if (*dmin__ < 0. && dmin1 > 0. && z__[ (*n0 - 1 << 2) - *pp] < tol * (*sigma + dn1) &&
		           fabs (dn) < tol * *sigma) {

			/* Convergence hidden by negative DN. */

			z__[ (*n0 - 1 << 2) - *pp + 2] = 0.;
			*dmin__ = 0.;
			goto L100;
		} else if (*dmin__ < 0.) {

			/* TAU too big. Select new TAU and try again. */

			++ (*nfail);
			if (ttype < -22) {

				/* Failed twice. Play it safe. */

				tau = 0.;
			} else if (dmin1 > 0.) {

				/* Late failure. Gives excellent shift. */

				tau = (tau + *dmin__) * (1. - eps * 2.);
				ttype += -11;
			} else {

				/* Early failure. Divide by 4. */

				tau *= .25;
				ttype += -12;
			}
			goto L80;
		} else if (*dmin__ != *dmin__) {

			/* NaN. */

			tau = 0.;
			goto L80;
		} else {

			/* Possible underflow. Play it safe. */

			goto L90;
		}
	}

	/* Risk of underflow. */

L90:
	NUMlapack_dlasq6 (i0, n0, &z__[1], pp, dmin__, &dmin1, &dmin2, &dn, &dn1, &dn2);
	*ndiv += *n0 - *i0 + 2;
	++ (*iter);
	tau = 0.;

L100:
	if (tau < *sigma) {
		*desig += tau;
		t = *sigma + *desig;
		*desig -= t - *sigma;
	} else {
		t = *sigma + tau;
		*desig = *sigma - (t - tau) + *desig;
	}
	*sigma = t;

	return 0;
}								/* NUMlapack_dlasq3 */

int NUMlapack_dlasq4 (long *i0, long *n0, double *z__, long *pp, long *n0in, double *dmin__, double *dmin1,
                      double *dmin2, double *dn, double *dn1, double *dn2, double *tau, long *ttype) {
	/* Initialized data */

	static double g = 0.;

	/* System generated locals */
	long i__1;
	double d__1, d__2;

	/* Local variables */
	static double s, a2, b1, b2;
	static long i4, nn, np;
	static double gam, gap1, gap2;

	/* Parameter adjustments */
	--z__;

	/* Function Body

	   A negative DMIN forces the shift to take that absolute value TTYPE
	   records the type of shift. */

	if (*dmin__ <= 0.) {
		*tau = - (*dmin__);
		*ttype = -1;
		return 0;
	}

	nn = (*n0 << 2) + *pp;
	if (*n0in == *n0) {

		/* No eigenvalues deflated. */

		if (*dmin__ == *dn || *dmin__ == *dn1) {

			b1 = sqrt (z__[nn - 3]) * sqrt (z__[nn - 5]);
			b2 = sqrt (z__[nn - 7]) * sqrt (z__[nn - 9]);
			a2 = z__[nn - 7] + z__[nn - 5];

			/* Cases 2 and 3. */

			if (*dmin__ == *dn && *dmin1 == *dn1) {
				gap2 = *dmin2 - a2 - *dmin2 * .25;
				if (gap2 > 0. && gap2 > b2) {
					gap1 = a2 - *dn - b2 / gap2 * b2;
				} else {
					gap1 = a2 - *dn - (b1 + b2);
				}
				if (gap1 > 0. && gap1 > b1) {
					/* Computing MAX */
					d__1 = *dn - b1 / gap1 * b1, d__2 = *dmin__ * .5;
					s = MAX (d__1, d__2);
					*ttype = -2;
				} else {
					s = 0.;
					if (*dn > b1) {
						s = *dn - b1;
					}
					if (a2 > b1 + b2) {
						/* Computing MIN */
						d__1 = s, d__2 = a2 - (b1 + b2);
						s = MIN (d__1, d__2);
					}
					/* Computing MAX */
					d__1 = s, d__2 = *dmin__ * .333;
					s = MAX (d__1, d__2);
					*ttype = -3;
				}
			} else {

				/* Case 4. */

				*ttype = -4;
				s = *dmin__ * .25;
				if (*dmin__ == *dn) {
					gam = *dn;
					a2 = 0.;
					if (z__[nn - 5] > z__[nn - 7]) {
						return 0;
					}
					b2 = z__[nn - 5] / z__[nn - 7];
					np = nn - 9;
				} else {
					np = nn - (*pp << 1);
					b2 = z__[np - 2];
					gam = *dn1;
					if (z__[np - 4] > z__[np - 2]) {
						return 0;
					}
					a2 = z__[np - 4] / z__[np - 2];
					if (z__[nn - 9] > z__[nn - 11]) {
						return 0;
					}
					b2 = z__[nn - 9] / z__[nn - 11];
					np = nn - 13;
				}

				/* Approximate contribution to norm squared from I < NN-1. */

				a2 += b2;
				i__1 = (*i0 << 2) - 1 + *pp;
				for (i4 = np; i4 >= i__1; i4 += -4) {
					if (b2 == 0.) {
						goto L20;
					}
					b1 = b2;
					if (z__[i4] > z__[i4 - 2]) {
						return 0;
					}
					b2 *= z__[i4] / z__[i4 - 2];
					a2 += b2;
					if (MAX (b2, b1) * 100. < a2 || .563 < a2) {
						goto L20;
					}
					/* L10: */
				}
L20:
				a2 *= 1.05;

				/* Rayleigh quotient residual bound. */

				if (a2 < .563) {
					s = gam * (1. - sqrt (a2)) / (a2 + 1.);
				}
			}
		} else if (*dmin__ == *dn2) {

			/* Case 5. */

			*ttype = -5;
			s = *dmin__ * .25;

			/* Compute contribution to norm squared from I > NN-2. */

			np = nn - (*pp << 1);
			b1 = z__[np - 2];
			b2 = z__[np - 6];
			gam = *dn2;
			if (z__[np - 8] > b2 || z__[np - 4] > b1) {
				return 0;
			}
			a2 = z__[np - 8] / b2 * (z__[np - 4] / b1 + 1.);

			/* Approximate contribution to norm squared from I < NN-2. */

			if (*n0 - *i0 > 2) {
				b2 = z__[nn - 13] / z__[nn - 15];
				a2 += b2;
				i__1 = (*i0 << 2) - 1 + *pp;
				for (i4 = nn - 17; i4 >= i__1; i4 += -4) {
					if (b2 == 0.) {
						goto L40;
					}
					b1 = b2;
					if (z__[i4] > z__[i4 - 2]) {
						return 0;
					}
					b2 *= z__[i4] / z__[i4 - 2];
					a2 += b2;
					if (MAX (b2, b1) * 100. < a2 || .563 < a2) {
						goto L40;
					}
					/* L30: */
				}
L40:
				a2 *= 1.05;
			}

			if (a2 < .563) {
				s = gam * (1. - sqrt (a2)) / (a2 + 1.);
			}
		} else {

			/* Case 6, no information to guide us. */

			if (*ttype == -6) {
				g += (1. - g) * .333;
			} else if (*ttype == -18) {
				g = .083250000000000005;
			} else {
				g = .25;
			}
			s = g * *dmin__;
			*ttype = -6;
		}

	} else if (*n0in == *n0 + 1) {

		/* One eigenvalue just deflated. Use DMIN1, DN1 for DMIN and DN. */

		if (*dmin1 == *dn1 && *dmin2 == *dn2) {

			/* Cases 7 and 8. */

			*ttype = -7;
			s = *dmin1 * .333;
			if (z__[nn - 5] > z__[nn - 7]) {
				return 0;
			}
			b1 = z__[nn - 5] / z__[nn - 7];
			b2 = b1;
			if (b2 == 0.) {
				goto L60;
			}
			i__1 = (*i0 << 2) - 1 + *pp;
			for (i4 = (*n0 << 2) - 9 + *pp; i4 >= i__1; i4 += -4) {
				a2 = b1;
				if (z__[i4] > z__[i4 - 2]) {
					return 0;
				}
				b1 *= z__[i4] / z__[i4 - 2];
				b2 += b1;
				if (MAX (b1, a2) * 100. < b2) {
					goto L60;
				}
				/* L50: */
			}
L60:
			b2 = sqrt (b2 * 1.05);
			/* Computing 2nd power */
			d__1 = b2;
			a2 = *dmin1 / (d__1 * d__1 + 1.);
			gap2 = *dmin2 * .5 - a2;
			if (gap2 > 0. && gap2 > b2 * a2) {
				/* Computing MAX */
				d__1 = s, d__2 = a2 * (1. - a2 * 1.01 * (b2 / gap2) * b2);
				s = MAX (d__1, d__2);
			} else {
				/* Computing MAX */
				d__1 = s, d__2 = a2 * (1. - b2 * 1.01);
				s = MAX (d__1, d__2);
				*ttype = -8;
			}
		} else {

			/* Case 9. */

			s = *dmin1 * .25;
			if (*dmin1 == *dn1) {
				s = *dmin1 * .5;
			}
			*ttype = -9;
		}

	} else if (*n0in == *n0 + 2) {

		/* Two eigenvalues deflated. Use DMIN2, DN2 for DMIN and DN.

		   Cases 10 and 11. */

		if (*dmin2 == *dn2 && z__[nn - 5] * 2. < z__[nn - 7]) {
			*ttype = -10;
			s = *dmin2 * .333;
			if (z__[nn - 5] > z__[nn - 7]) {
				return 0;
			}
			b1 = z__[nn - 5] / z__[nn - 7];
			b2 = b1;
			if (b2 == 0.) {
				goto L80;
			}
			i__1 = (*i0 << 2) - 1 + *pp;
			for (i4 = (*n0 << 2) - 9 + *pp; i4 >= i__1; i4 += -4) {
				if (z__[i4] > z__[i4 - 2]) {
					return 0;
				}
				b1 *= z__[i4] / z__[i4 - 2];
				b2 += b1;
				if (b1 * 100. < b2) {
					goto L80;
				}
				/* L70: */
			}
L80:
			b2 = sqrt (b2 * 1.05);
			/* Computing 2nd power */
			d__1 = b2;
			a2 = *dmin2 / (d__1 * d__1 + 1.);
			gap2 = z__[nn - 7] + z__[nn - 9] - sqrt (z__[nn - 11]) * sqrt (z__[nn - 9]) - a2;
			if (gap2 > 0. && gap2 > b2 * a2) {
				/* Computing MAX */
				d__1 = s, d__2 = a2 * (1. - a2 * 1.01 * (b2 / gap2) * b2);
				s = MAX (d__1, d__2);
			} else {
				/* Computing MAX */
				d__1 = s, d__2 = a2 * (1. - b2 * 1.01);
				s = MAX (d__1, d__2);
			}
		} else {
			s = *dmin2 * .25;
			*ttype = -11;
		}
	} else if (*n0in > *n0 + 2) {

		/* Case 12, more than two eigenvalues deflated. No information. */

		s = 0.;
		*ttype = -12;
	}

	*tau = s;
	return 0;
}								/* NUMlapack_dlasq4 */

int NUMlapack_dlasq5 (long *i0, long *n0, double *z__, long *pp, double *tau, double *dmin__, double *dmin1,
                      double *dmin2, double *dn, double *dnm1, double *dnm2, long *ieee) {
	/* System generated locals */
	long i__1;
	double d__1, d__2;

	/* Local variables */
	static double emin, temp, d__;
	static long j4, j4p2;

	--z__;

	/* Function Body */
	if (*n0 - *i0 - 1 <= 0) {
		return 0;
	}

	j4 = (*i0 << 2) + *pp - 3;
	emin = z__[j4 + 4];
	d__ = z__[j4] - *tau;
	*dmin__ = d__;
	*dmin1 = -z__[j4];

	if (*ieee) {

		/* Code for IEEE arithmetic. */

		if (*pp == 0) {
			i__1 = *n0 - 3 << 2;
			for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
				z__[j4 - 2] = d__ + z__[j4 - 1];
				temp = z__[j4 + 1] / z__[j4 - 2];
				d__ = d__ * temp - *tau;
				*dmin__ = MIN (*dmin__, d__);
				z__[j4] = z__[j4 - 1] * temp;
				/* Computing MIN */
				d__1 = z__[j4];
				emin = MIN (d__1, emin);
				/* L10: */
			}
		} else {
			i__1 = *n0 - 3 << 2;
			for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
				z__[j4 - 3] = d__ + z__[j4];
				temp = z__[j4 + 2] / z__[j4 - 3];
				d__ = d__ * temp - *tau;
				*dmin__ = MIN (*dmin__, d__);
				z__[j4 - 1] = z__[j4] * temp;
				/* Computing MIN */
				d__1 = z__[j4 - 1];
				emin = MIN (d__1, emin);
				/* L20: */
			}
		}

		/* Unroll last two steps. */

		*dnm2 = d__;
		*dmin2 = *dmin__;
		j4 = (*n0 - 2 << 2) - *pp;
		j4p2 = j4 + (*pp << 1) - 1;
		z__[j4 - 2] = *dnm2 + z__[j4p2];
		z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
		*dnm1 = z__[j4p2 + 2] * (*dnm2 / z__[j4 - 2]) - *tau;
		*dmin__ = MIN (*dmin__, *dnm1);

		*dmin1 = *dmin__;
		j4 += 4;
		j4p2 = j4 + (*pp << 1) - 1;
		z__[j4 - 2] = *dnm1 + z__[j4p2];
		z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
		*dn = z__[j4p2 + 2] * (*dnm1 / z__[j4 - 2]) - *tau;
		*dmin__ = MIN (*dmin__, *dn);

	} else {

		/* Code for non IEEE arithmetic. */

		if (*pp == 0) {
			i__1 = *n0 - 3 << 2;
			for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
				z__[j4 - 2] = d__ + z__[j4 - 1];
				if (d__ < 0.) {
					return 0;
				} else {
					z__[j4] = z__[j4 + 1] * (z__[j4 - 1] / z__[j4 - 2]);
					d__ = z__[j4 + 1] * (d__ / z__[j4 - 2]) - *tau;
				}
				*dmin__ = MIN (*dmin__, d__);
				/* Computing MIN */
				d__1 = emin, d__2 = z__[j4];
				emin = MIN (d__1, d__2);
				/* L30: */
			}
		} else {
			i__1 = *n0 - 3 << 2;
			for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
				z__[j4 - 3] = d__ + z__[j4];
				if (d__ < 0.) {
					return 0;
				} else {
					z__[j4 - 1] = z__[j4 + 2] * (z__[j4] / z__[j4 - 3]);
					d__ = z__[j4 + 2] * (d__ / z__[j4 - 3]) - *tau;
				}
				*dmin__ = MIN (*dmin__, d__);
				/* Computing MIN */
				d__1 = emin, d__2 = z__[j4 - 1];
				emin = MIN (d__1, d__2);
				/* L40: */
			}
		}

		/* Unroll last two steps. */

		*dnm2 = d__;
		*dmin2 = *dmin__;
		j4 = (*n0 - 2 << 2) - *pp;
		j4p2 = j4 + (*pp << 1) - 1;
		z__[j4 - 2] = *dnm2 + z__[j4p2];
		if (*dnm2 < 0.) {
			return 0;
		} else {
			z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
			*dnm1 = z__[j4p2 + 2] * (*dnm2 / z__[j4 - 2]) - *tau;
		}
		*dmin__ = MIN (*dmin__, *dnm1);

		*dmin1 = *dmin__;
		j4 += 4;
		j4p2 = j4 + (*pp << 1) - 1;
		z__[j4 - 2] = *dnm1 + z__[j4p2];
		if (*dnm1 < 0.) {
			return 0;
		} else {
			z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
			*dn = z__[j4p2 + 2] * (*dnm1 / z__[j4 - 2]) - *tau;
		}
		*dmin__ = MIN (*dmin__, *dn);

	}

	z__[j4 + 2] = *dn;
	z__[ (*n0 << 2) - *pp] = emin;
	return 0;
}								/* NUMlapack_dlasq5 */

int NUMlapack_dlasq6 (long *i0, long *n0, double *z__, long *pp, double *dmin__, double *dmin1, double *dmin2,
                      double *dn, double *dnm1, double *dnm2) {
	/* System generated locals */
	long i__1;
	double d__1, d__2;

	/* Local variables */
	static double emin, temp, d__;
	static long j4;
	static double safmin;
	static long j4p2;

	/* Parameter adjustments */
	--z__;

	/* Function Body */
	if (*n0 - *i0 - 1 <= 0) {
		return 0;
	}

	safmin = NUMblas_dlamch ("Safe minimum");
	j4 = (*i0 << 2) + *pp - 3;
	emin = z__[j4 + 4];
	d__ = z__[j4];
	*dmin__ = d__;

	if (*pp == 0) {
		i__1 = *n0 - 3 << 2;
		for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
			z__[j4 - 2] = d__ + z__[j4 - 1];
			if (z__[j4 - 2] == 0.) {
				z__[j4] = 0.;
				d__ = z__[j4 + 1];
				*dmin__ = d__;
				emin = 0.;
			} else if (safmin * z__[j4 + 1] < z__[j4 - 2] && safmin * z__[j4 - 2] < z__[j4 + 1]) {
				temp = z__[j4 + 1] / z__[j4 - 2];
				z__[j4] = z__[j4 - 1] * temp;
				d__ *= temp;
			} else {
				z__[j4] = z__[j4 + 1] * (z__[j4 - 1] / z__[j4 - 2]);
				d__ = z__[j4 + 1] * (d__ / z__[j4 - 2]);
			}
			*dmin__ = MIN (*dmin__, d__);
			/* Computing MIN */
			d__1 = emin, d__2 = z__[j4];
			emin = MIN (d__1, d__2);
			/* L10: */
		}
	} else {
		i__1 = *n0 - 3 << 2;
		for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
			z__[j4 - 3] = d__ + z__[j4];
			if (z__[j4 - 3] == 0.) {
				z__[j4 - 1] = 0.;
				d__ = z__[j4 + 2];
				*dmin__ = d__;
				emin = 0.;
			} else if (safmin * z__[j4 + 2] < z__[j4 - 3] && safmin * z__[j4 - 3] < z__[j4 + 2]) {
				temp = z__[j4 + 2] / z__[j4 - 3];
				z__[j4 - 1] = z__[j4] * temp;
				d__ *= temp;
			} else {
				z__[j4 - 1] = z__[j4 + 2] * (z__[j4] / z__[j4 - 3]);
				d__ = z__[j4 + 2] * (d__ / z__[j4 - 3]);
			}
			*dmin__ = MIN (*dmin__, d__);
			/* Computing MIN */
			d__1 = emin, d__2 = z__[j4 - 1];
			emin = MIN (d__1, d__2);
			/* L20: */
		}
	}

	/* Unroll last two steps. */

	*dnm2 = d__;
	*dmin2 = *dmin__;
	j4 = (*n0 - 2 << 2) - *pp;
	j4p2 = j4 + (*pp << 1) - 1;
	z__[j4 - 2] = *dnm2 + z__[j4p2];
	if (z__[j4 - 2] == 0.) {
		z__[j4] = 0.;
		*dnm1 = z__[j4p2 + 2];
		*dmin__ = *dnm1;
		emin = 0.;
	} else if (safmin * z__[j4p2 + 2] < z__[j4 - 2] && safmin * z__[j4 - 2] < z__[j4p2 + 2]) {
		temp = z__[j4p2 + 2] / z__[j4 - 2];
		z__[j4] = z__[j4p2] * temp;
		*dnm1 = *dnm2 * temp;
	} else {
		z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
		*dnm1 = z__[j4p2 + 2] * (*dnm2 / z__[j4 - 2]);
	}
	*dmin__ = MIN (*dmin__, *dnm1);

	*dmin1 = *dmin__;
	j4 += 4;
	j4p2 = j4 + (*pp << 1) - 1;
	z__[j4 - 2] = *dnm1 + z__[j4p2];
	if (z__[j4 - 2] == 0.) {
		z__[j4] = 0.;
		*dn = z__[j4p2 + 2];
		*dmin__ = *dn;
		emin = 0.;
	} else if (safmin * z__[j4p2 + 2] < z__[j4 - 2] && safmin * z__[j4 - 2] < z__[j4p2 + 2]) {
		temp = z__[j4p2 + 2] / z__[j4 - 2];
		z__[j4] = z__[j4p2] * temp;
		*dn = *dnm1 * temp;
	} else {
		z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
		*dn = z__[j4p2 + 2] * (*dnm1 / z__[j4 - 2]);
	}
	*dmin__ = MIN (*dmin__, *dn);

	z__[j4 + 2] = *dn;
	z__[ (*n0 << 2) - *pp] = emin;
	return 0;
}								/* NUMlapack_dlasq6 */

int NUMlapack_dlasr (const char *side, const char *pivot, const char *direct, long *m, long *n, double *c__, double *s, double *a,
                     long *lda) {
	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2;

	/* Local variables */
	static long info;
	static double temp;
	static long i__, j;
	static double ctemp, stemp;

	--c__;
	--s;
	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;

	/* Function Body */
	info = 0;
	if (! (lsame_ (side, "L") || lsame_ (side, "R"))) {
		info = 1;
	} else if (! (lsame_ (pivot, "V") || lsame_ (pivot, "T") || lsame_ (pivot, "B"))) {
		info = 2;
	} else if (! (lsame_ (direct, "F") || lsame_ (direct, "B"))) {
		info = 3;
	} else if (*m < 0) {
		info = 4;
	} else if (*n < 0) {
		info = 5;
	} else if (*lda < MAX (1, *m)) {
		info = 9;
	}
	if (info != 0) {
		xerbla_ ("DLASR ", &info);
		return 0;
	}

	/* Quick return if possible */

	if (*m == 0 || *n == 0) {
		return 0;
	}
	if (lsame_ (side, "L")) {

		/* Form P * A */

		if (lsame_ (pivot, "V")) {
			if (lsame_ (direct, "F")) {
				i__1 = *m - 1;
				for (j = 1; j <= i__1; ++j) {
					ctemp = c__[j];
					stemp = s[j];
					if (ctemp != 1. || stemp != 0.) {
						i__2 = *n;
						for (i__ = 1; i__ <= i__2; ++i__) {
							temp = a_ref (j + 1, i__);
							a_ref (j + 1, i__) = ctemp * temp - stemp * a_ref (j, i__);
							a_ref (j, i__) = stemp * temp + ctemp * a_ref (j, i__);
							/* L10: */
						}
					}
					/* L20: */
				}
			} else if (lsame_ (direct, "B")) {
				for (j = *m - 1; j >= 1; --j) {
					ctemp = c__[j];
					stemp = s[j];
					if (ctemp != 1. || stemp != 0.) {
						i__1 = *n;
						for (i__ = 1; i__ <= i__1; ++i__) {
							temp = a_ref (j + 1, i__);
							a_ref (j + 1, i__) = ctemp * temp - stemp * a_ref (j, i__);
							a_ref (j, i__) = stemp * temp + ctemp * a_ref (j, i__);
							/* L30: */
						}
					}
					/* L40: */
				}
			}
		} else if (lsame_ (pivot, "T")) {
			if (lsame_ (direct, "F")) {
				i__1 = *m;
				for (j = 2; j <= i__1; ++j) {
					ctemp = c__[j - 1];
					stemp = s[j - 1];
					if (ctemp != 1. || stemp != 0.) {
						i__2 = *n;
						for (i__ = 1; i__ <= i__2; ++i__) {
							temp = a_ref (j, i__);
							a_ref (j, i__) = ctemp * temp - stemp * a_ref (1, i__);
							a_ref (1, i__) = stemp * temp + ctemp * a_ref (1, i__);
							/* L50: */
						}
					}
					/* L60: */
				}
			} else if (lsame_ (direct, "B")) {
				for (j = *m; j >= 2; --j) {
					ctemp = c__[j - 1];
					stemp = s[j - 1];
					if (ctemp != 1. || stemp != 0.) {
						i__1 = *n;
						for (i__ = 1; i__ <= i__1; ++i__) {
							temp = a_ref (j, i__);
							a_ref (j, i__) = ctemp * temp - stemp * a_ref (1, i__);
							a_ref (1, i__) = stemp * temp + ctemp * a_ref (1, i__);
							/* L70: */
						}
					}
					/* L80: */
				}
			}
		} else if (lsame_ (pivot, "B")) {
			if (lsame_ (direct, "F")) {
				i__1 = *m - 1;
				for (j = 1; j <= i__1; ++j) {
					ctemp = c__[j];
					stemp = s[j];
					if (ctemp != 1. || stemp != 0.) {
						i__2 = *n;
						for (i__ = 1; i__ <= i__2; ++i__) {
							temp = a_ref (j, i__);
							a_ref (j, i__) = stemp * a_ref (*m, i__) + ctemp * temp;
							a_ref (*m, i__) = ctemp * a_ref (*m, i__) - stemp * temp;
							/* L90: */
						}
					}
					/* L100: */
				}
			} else if (lsame_ (direct, "B")) {
				for (j = *m - 1; j >= 1; --j) {
					ctemp = c__[j];
					stemp = s[j];
					if (ctemp != 1. || stemp != 0.) {
						i__1 = *n;
						for (i__ = 1; i__ <= i__1; ++i__) {
							temp = a_ref (j, i__);
							a_ref (j, i__) = stemp * a_ref (*m, i__) + ctemp * temp;
							a_ref (*m, i__) = ctemp * a_ref (*m, i__) - stemp * temp;
							/* L110: */
						}
					}
					/* L120: */
				}
			}
		}
	} else if (lsame_ (side, "R")) {

		/* Form A * P' */

		if (lsame_ (pivot, "V")) {
			if (lsame_ (direct, "F")) {
				i__1 = *n - 1;
				for (j = 1; j <= i__1; ++j) {
					ctemp = c__[j];
					stemp = s[j];
					if (ctemp != 1. || stemp != 0.) {
						i__2 = *m;
						for (i__ = 1; i__ <= i__2; ++i__) {
							temp = a_ref (i__, j + 1);
							a_ref (i__, j + 1) = ctemp * temp - stemp * a_ref (i__, j);
							a_ref (i__, j) = stemp * temp + ctemp * a_ref (i__, j);
							/* L130: */
						}
					}
					/* L140: */
				}
			} else if (lsame_ (direct, "B")) {
				for (j = *n - 1; j >= 1; --j) {
					ctemp = c__[j];
					stemp = s[j];
					if (ctemp != 1. || stemp != 0.) {
						i__1 = *m;
						for (i__ = 1; i__ <= i__1; ++i__) {
							temp = a_ref (i__, j + 1);
							a_ref (i__, j + 1) = ctemp * temp - stemp * a_ref (i__, j);
							a_ref (i__, j) = stemp * temp + ctemp * a_ref (i__, j);
							/* L150: */
						}
					}
					/* L160: */
				}
			}
		} else if (lsame_ (pivot, "T")) {
			if (lsame_ (direct, "F")) {
				i__1 = *n;
				for (j = 2; j <= i__1; ++j) {
					ctemp = c__[j - 1];
					stemp = s[j - 1];
					if (ctemp != 1. || stemp != 0.) {
						i__2 = *m;
						for (i__ = 1; i__ <= i__2; ++i__) {
							temp = a_ref (i__, j);
							a_ref (i__, j) = ctemp * temp - stemp * a_ref (i__, 1);
							a_ref (i__, 1) = stemp * temp + ctemp * a_ref (i__, 1);
							/* L170: */
						}
					}
					/* L180: */
				}
			} else if (lsame_ (direct, "B")) {
				for (j = *n; j >= 2; --j) {
					ctemp = c__[j - 1];
					stemp = s[j - 1];
					if (ctemp != 1. || stemp != 0.) {
						i__1 = *m;
						for (i__ = 1; i__ <= i__1; ++i__) {
							temp = a_ref (i__, j);
							a_ref (i__, j) = ctemp * temp - stemp * a_ref (i__, 1);
							a_ref (i__, 1) = stemp * temp + ctemp * a_ref (i__, 1);
							/* L190: */
						}
					}
					/* L200: */
				}
			}
		} else if (lsame_ (pivot, "B")) {
			if (lsame_ (direct, "F")) {
				i__1 = *n - 1;
				for (j = 1; j <= i__1; ++j) {
					ctemp = c__[j];
					stemp = s[j];
					if (ctemp != 1. || stemp != 0.) {
						i__2 = *m;
						for (i__ = 1; i__ <= i__2; ++i__) {
							temp = a_ref (i__, j);
							a_ref (i__, j) = stemp * a_ref (i__, *n) + ctemp * temp;
							a_ref (i__, *n) = ctemp * a_ref (i__, *n) - stemp * temp;
							/* L210: */
						}
					}
					/* L220: */
				}
			} else if (lsame_ (direct, "B")) {
				for (j = *n - 1; j >= 1; --j) {
					ctemp = c__[j];
					stemp = s[j];
					if (ctemp != 1. || stemp != 0.) {
						i__1 = *m;
						for (i__ = 1; i__ <= i__1; ++i__) {
							temp = a_ref (i__, j);
							a_ref (i__, j) = stemp * a_ref (i__, *n) + ctemp * temp;
							a_ref (i__, *n) = ctemp * a_ref (i__, *n) - stemp * temp;
							/* L230: */
						}
					}
					/* L240: */
				}
			}
		}
	}

	return 0;
}								/* NUMlapack_dlasr */

#define stack_ref(a_1,a_2) stack[(a_2)*2 + a_1 - 3]

int NUMlapack_dlasrt (const char *id, long *n, double *d__, long *info) {
	/* System generated locals */
	long i__1, i__2;

	/* Local variables */
	static long endd, i__, j;
	static long stack[64] /* was [2][32] */ ;
	static double dmnmx, d1, d2, d3;
	static long start;
	static long stkpnt, dir;
	static double tmp;

	--d__;

	/* Function Body */
	*info = 0;
	dir = -1;
	if (lsame_ (id, "D")) {
		dir = 0;
	} else if (lsame_ (id, "I")) {
		dir = 1;
	}
	if (dir == -1) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DLASRT", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*n <= 1) {
		return 0;
	}

	stkpnt = 1;
	stack_ref (1, 1) = 1;
	stack_ref (2, 1) = *n;
L10:
	start = stack_ref (1, stkpnt);
	endd = stack_ref (2, stkpnt);
	--stkpnt;
	if (endd - start <= 20 && endd - start > 0) {

		/* Do Insertion sort on D( START:ENDD ) */

		if (dir == 0) {

			/* Sort into decreasing order */

			i__1 = endd;
			for (i__ = start + 1; i__ <= i__1; ++i__) {
				i__2 = start + 1;
				for (j = i__; j >= i__2; --j) {
					if (d__[j] > d__[j - 1]) {
						dmnmx = d__[j];
						d__[j] = d__[j - 1];
						d__[j - 1] = dmnmx;
					} else {
						goto L30;
					}
					/* L20: */
				}
L30:
				;
			}

		} else {

			/* Sort into increasing order */

			i__1 = endd;
			for (i__ = start + 1; i__ <= i__1; ++i__) {
				i__2 = start + 1;
				for (j = i__; j >= i__2; --j) {
					if (d__[j] < d__[j - 1]) {
						dmnmx = d__[j];
						d__[j] = d__[j - 1];
						d__[j - 1] = dmnmx;
					} else {
						goto L50;
					}
					/* L40: */
				}
L50:
				;
			}

		}

	} else if (endd - start > 20) {

		/* Partition D( START:ENDD ) and stack parts, largest one first

		   Choose partition entry as median of 3 */

		d1 = d__[start];
		d2 = d__[endd];
		i__ = (start + endd) / 2;
		d3 = d__[i__];
		if (d1 < d2) {
			if (d3 < d1) {
				dmnmx = d1;
			} else if (d3 < d2) {
				dmnmx = d3;
			} else {
				dmnmx = d2;
			}
		} else {
			if (d3 < d2) {
				dmnmx = d2;
			} else if (d3 < d1) {
				dmnmx = d3;
			} else {
				dmnmx = d1;
			}
		}

		if (dir == 0) {

			/* Sort into decreasing order */

			i__ = start - 1;
			j = endd + 1;
L60:
L70:
			--j;
			if (d__[j] < dmnmx) {
				goto L70;
			}
L80:
			++i__;
			if (d__[i__] > dmnmx) {
				goto L80;
			}
			if (i__ < j) {
				tmp = d__[i__];
				d__[i__] = d__[j];
				d__[j] = tmp;
				goto L60;
			}
			if (j - start > endd - j - 1) {
				++stkpnt;
				stack_ref (1, stkpnt) = start;
				stack_ref (2, stkpnt) = j;
				++stkpnt;
				stack_ref (1, stkpnt) = j + 1;
				stack_ref (2, stkpnt) = endd;
			} else {
				++stkpnt;
				stack_ref (1, stkpnt) = j + 1;
				stack_ref (2, stkpnt) = endd;
				++stkpnt;
				stack_ref (1, stkpnt) = start;
				stack_ref (2, stkpnt) = j;
			}
		} else {

			/* Sort into increasing order */

			i__ = start - 1;
			j = endd + 1;
L90:
L100:
			--j;
			if (d__[j] > dmnmx) {
				goto L100;
			}
L110:
			++i__;
			if (d__[i__] < dmnmx) {
				goto L110;
			}
			if (i__ < j) {
				tmp = d__[i__];
				d__[i__] = d__[j];
				d__[j] = tmp;
				goto L90;
			}
			if (j - start > endd - j - 1) {
				++stkpnt;
				stack_ref (1, stkpnt) = start;
				stack_ref (2, stkpnt) = j;
				++stkpnt;
				stack_ref (1, stkpnt) = j + 1;
				stack_ref (2, stkpnt) = endd;
			} else {
				++stkpnt;
				stack_ref (1, stkpnt) = j + 1;
				stack_ref (2, stkpnt) = endd;
				++stkpnt;
				stack_ref (1, stkpnt) = start;
				stack_ref (2, stkpnt) = j;
			}
		}
	}
	if (stkpnt > 0) {
		goto L10;
	}
	return 0;
}								/* NUMlapack_dlasrt */

#undef stack_ref

int NUMlapack_dlassq (long *n, double *x, long *incx, double *scale, double *sumsq) {
	/* System generated locals */
	long i__1, i__2;
	double d__1;

	/* Local variables */
	static double absxi;
	static long ix;

	--x;

	/* Function Body */
	if (*n > 0) {
		i__1 = (*n - 1) * *incx + 1;
		i__2 = *incx;
		for (ix = 1; i__2 < 0 ? ix >= i__1 : ix <= i__1; ix += i__2) {
			if (x[ix] != 0.) {
				absxi = (d__1 = x[ix], fabs (d__1));
				if (*scale < absxi) {
					/* Computing 2nd power */
					d__1 = *scale / absxi;
					*sumsq = *sumsq * (d__1 * d__1) + 1;
					*scale = absxi;
				} else {
					/* Computing 2nd power */
					d__1 = absxi / *scale;
					*sumsq += d__1 * d__1;
				}
			}
			/* L10: */
		}
	}
	return 0;
}								/* NUMlapack_dlassq */

int NUMlapack_dlasv2 (double *f, double *g, double *h__, double *ssmin, double *ssmax, double *snr, double *csr,
                      double *snl, double *csl) {
	/* Table of constant values */
	static double c_b3 = 2.;
	static double c_b4 = 1.;

	/* System generated locals */
	double d__1;

	/* Local variables */
	static long pmax;
	static double temp;
	static long swap;
	static double a, d__, l, m, r__, s, t, tsign, fa, ga, ha;
	static double ft, gt, ht, mm;
	static long gasmal;
	static double tt, clt, crt, slt, srt;

	ft = *f;
	fa = fabs (ft);
	ht = *h__;
	ha = fabs (*h__);

	/* PMAX points to the maximum absolute element of matrix PMAX = 1 if F
	   largest in absolute values PMAX = 2 if G largest in absolute values
	   PMAX = 3 if H largest in absolute values */

	pmax = 1;
	swap = ha > fa;
	if (swap) {
		pmax = 3;
		temp = ft;
		ft = ht;
		ht = temp;
		temp = fa;
		fa = ha;
		ha = temp;

		/* Now FA .ge. HA */

	}
	gt = *g;
	ga = fabs (gt);
	if (ga == 0.) {

		/* Diagonal matrix */

		*ssmin = ha;
		*ssmax = fa;
		clt = 1.;
		crt = 1.;
		slt = 0.;
		srt = 0.;
	} else {
		gasmal = TRUE;
		if (ga > fa) {
			pmax = 2;
			if (fa / ga < NUMblas_dlamch ("EPS")) {

				/* Case of very large GA */

				gasmal = FALSE;
				*ssmax = ga;
				if (ha > 1.) {
					*ssmin = fa / (ga / ha);
				} else {
					*ssmin = fa / ga * ha;
				}
				clt = 1.;
				slt = ht / gt;
				srt = 1.;
				crt = ft / gt;
			}
		}
		if (gasmal) {

			/* Normal case */

			d__ = fa - ha;
			if (d__ == fa) {

				/* Copes with infinite F or H */

				l = 1.;
			} else {
				l = d__ / fa;
			}

			/* Note that 0 .le. L .le. 1 */

			m = gt / ft;

			/* Note that abs(M) .le. 1/macheps */

			t = 2. - l;

			/* Note that T .ge. 1 */

			mm = m * m;
			tt = t * t;
			s = sqrt (tt + mm);

			/* Note that 1 .le. S .le. 1 + 1/macheps */

			if (l == 0.) {
				r__ = fabs (m);
			} else {
				r__ = sqrt (l * l + mm);
			}

			/* Note that 0 .le. R .le. 1 + 1/macheps */

			a = (s + r__) * .5;

			/* Note that 1 .le. A .le. 1 + abs(M) */

			*ssmin = ha / a;
			*ssmax = fa * a;
			if (mm == 0.) {

				/* Note that M is very tiny */

				if (l == 0.) {
					t = d_sign (&c_b3, &ft) * d_sign (&c_b4, &gt);
				} else {
					t = gt / d_sign (&d__, &ft) + m / t;
				}
			} else {
				t = (m / (s + t) + m / (r__ + l)) * (a + 1.);
			}
			l = sqrt (t * t + 4.);
			crt = 2. / l;
			srt = t / l;
			clt = (crt + srt * m) / a;
			slt = ht / ft * srt / a;
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

	/* Correct signs of SSMAX and SSMIN */

	if (pmax == 1) {
		tsign = d_sign (&c_b4, csr) * d_sign (&c_b4, csl) * d_sign (&c_b4, f);
	}
	if (pmax == 2) {
		tsign = d_sign (&c_b4, snr) * d_sign (&c_b4, csl) * d_sign (&c_b4, g);
	}
	if (pmax == 3) {
		tsign = d_sign (&c_b4, snr) * d_sign (&c_b4, snl) * d_sign (&c_b4, h__);
	}
	*ssmax = d_sign (ssmax, &tsign);
	d__1 = tsign * d_sign (&c_b4, f) * d_sign (&c_b4, h__);
	*ssmin = d_sign (ssmin, &d__1);
	return 0;
}								/* NUMlapack_dlasv2 */

int NUMlapack_dlaswp (long *n, double *a, long *lda, long *k1, long *k2, long *ipiv, long *incx) {
	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3, i__4;

	/* Local variables */
	static double temp;
	static long i__, j, k, i1, i2, n32, ip, ix, ix0, inc;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--ipiv;

	/* Function Body */
	if (*incx > 0) {
		ix0 = *k1;
		i1 = *k1;
		i2 = *k2;
		inc = 1;
	} else if (*incx < 0) {
		ix0 = (1 - *k2) * *incx + 1;
		i1 = *k2;
		i2 = *k1;
		inc = -1;
	} else {
		return 0;
	}

	n32 = *n / 32 << 5;
	if (n32 != 0) {
		i__1 = n32;
		for (j = 1; j <= i__1; j += 32) {
			ix = ix0;
			i__2 = i2;
			i__3 = inc;
			for (i__ = i1; i__3 < 0 ? i__ >= i__2 : i__ <= i__2; i__ += i__3) {
				ip = ipiv[ix];
				if (ip != i__) {
					i__4 = j + 31;
					for (k = j; k <= i__4; ++k) {
						temp = a_ref (i__, k);
						a_ref (i__, k) = a_ref (ip, k);
						a_ref (ip, k) = temp;
						/* L10: */
					}
				}
				ix += *incx;
				/* L20: */
			}
			/* L30: */
		}
	}
	if (n32 != *n) {
		++n32;
		ix = ix0;
		i__1 = i2;
		i__3 = inc;
		for (i__ = i1; i__3 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__3) {
			ip = ipiv[ix];
			if (ip != i__) {
				i__2 = *n;
				for (k = n32; k <= i__2; ++k) {
					temp = a_ref (i__, k);
					a_ref (i__, k) = a_ref (ip, k);
					a_ref (ip, k) = temp;
					/* L40: */
				}
			}
			ix += *incx;
			/* L50: */
		}
	}
	return 0;
}								/* NUMlapack_dlaswp */

#define w_ref(a_1,a_2) w[(a_2)*w_dim1 + a_1]

int NUMlapack_dlatrd (const char *uplo, long *n, long *nb, double *a, long *lda, double *e, double *tau, double *w,
                      long *ldw) {
	/* Table of constant values */
	static double c_b5 = -1.;
	static double c_b6 = 1.;
	static long c__1 = 1;
	static double c_b16 = 0.;

	/* System generated locals */
	long a_dim1, a_offset, w_dim1, w_offset, i__1, i__2, i__3;

	/* Local variables */
	static long i__;
	static double alpha;
	static long iw;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--e;
	--tau;
	w_dim1 = *ldw;
	w_offset = 1 + w_dim1 * 1;
	w -= w_offset;

	/* Function Body */
	if (*n <= 0) {
		return 0;
	}

	if (lsame_ (uplo, "U")) {

		/* Reduce last NB columns of upper triangle */

		i__1 = *n - *nb + 1;
		for (i__ = *n; i__ >= i__1; --i__) {
			iw = i__ - *n + *nb;
			if (i__ < *n) {

				/* Update A(1:i,i) */

				i__2 = *n - i__;
				NUMblas_dgemv ("No transpose", &i__, &i__2, &c_b5, &a_ref (1, i__ + 1), lda, &w_ref (i__, iw + 1),
				               ldw, &c_b6, &a_ref (1, i__), &c__1);
				i__2 = *n - i__;
				NUMblas_dgemv ("No transpose", &i__, &i__2, &c_b5, &w_ref (1, iw + 1), ldw, &a_ref (i__, i__ + 1),
				               lda, &c_b6, &a_ref (1, i__), &c__1);
			}
			if (i__ > 1) {

				/* Generate elementary reflector H(i) to annihilate
				   A(1:i-2,i) */

				i__2 = i__ - 1;
				NUMlapack_dlarfg (&i__2, &a_ref (i__ - 1, i__), &a_ref (1, i__), &c__1, &tau[i__ - 1]);
				e[i__ - 1] = a_ref (i__ - 1, i__);
				a_ref (i__ - 1, i__) = 1.;

				/* Compute W(1:i-1,i) */

				i__2 = i__ - 1;
				NUMblas_dsymv ("Upper", &i__2, &c_b6, &a[a_offset], lda, &a_ref (1, i__), &c__1, &c_b16, &w_ref (1,
				               iw), &c__1);
				if (i__ < *n) {
					i__2 = i__ - 1;
					i__3 = *n - i__;
					NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b6, &w_ref (1, iw + 1), ldw, &a_ref (1, i__), &c__1,
					               &c_b16, &w_ref (i__ + 1, iw), &c__1);
					i__2 = i__ - 1;
					i__3 = *n - i__;
					NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b5, &a_ref (1, i__ + 1), lda, &w_ref (i__ + 1,
					               iw), &c__1, &c_b6, &w_ref (1, iw), &c__1);
					i__2 = i__ - 1;
					i__3 = *n - i__;
					NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b6, &a_ref (1, i__ + 1), lda, &a_ref (1, i__),
					               &c__1, &c_b16, &w_ref (i__ + 1, iw), &c__1);
					i__2 = i__ - 1;
					i__3 = *n - i__;
					NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b5, &w_ref (1, iw + 1), ldw, &w_ref (i__ + 1,
					               iw), &c__1, &c_b6, &w_ref (1, iw), &c__1);
				}
				i__2 = i__ - 1;
				NUMblas_dscal (&i__2, &tau[i__ - 1], &w_ref (1, iw), &c__1);
				i__2 = i__ - 1;
				alpha = tau[i__ - 1] * -.5 * NUMblas_ddot (&i__2, &w_ref (1, iw), &c__1, &a_ref (1, i__), &c__1);
				i__2 = i__ - 1;
				NUMblas_daxpy (&i__2, &alpha, &a_ref (1, i__), &c__1, &w_ref (1, iw), &c__1);
			}

			/* L10: */
		}
	} else {

		/* Reduce first NB columns of lower triangle */

		i__1 = *nb;
		for (i__ = 1; i__ <= i__1; ++i__) {

			/* Update A(i:n,i) */

			i__2 = *n - i__ + 1;
			i__3 = i__ - 1;
			NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b5, &a_ref (i__, 1), lda, &w_ref (i__, 1), ldw, &c_b6,
			               &a_ref (i__, i__), &c__1);
			i__2 = *n - i__ + 1;
			i__3 = i__ - 1;
			NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b5, &w_ref (i__, 1), ldw, &a_ref (i__, 1), lda, &c_b6,
			               &a_ref (i__, i__), &c__1);
			if (i__ < *n) {

				/* Generate elementary reflector H(i) to annihilate
				   A(i+2:n,i)

				   Computing MIN */
				i__2 = i__ + 2;
				i__3 = *n - i__;
				NUMlapack_dlarfg (&i__3, &a_ref (i__ + 1, i__), &a_ref (MIN (i__2, *n), i__), &c__1, &tau[i__]);
				e[i__] = a_ref (i__ + 1, i__);
				a_ref (i__ + 1, i__) = 1.;

				/* Compute W(i+1:n,i) */

				i__2 = *n - i__;
				NUMblas_dsymv ("Lower", &i__2, &c_b6, &a_ref (i__ + 1, i__ + 1), lda, &a_ref (i__ + 1, i__), &c__1,
				               &c_b16, &w_ref (i__ + 1, i__), &c__1);
				i__2 = *n - i__;
				i__3 = i__ - 1;
				NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b6, &w_ref (i__ + 1, 1), ldw, &a_ref (i__ + 1, i__),
				               &c__1, &c_b16, &w_ref (1, i__), &c__1);
				i__2 = *n - i__;
				i__3 = i__ - 1;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b5, &a_ref (i__ + 1, 1), lda, &w_ref (1, i__), &c__1,
				               &c_b6, &w_ref (i__ + 1, i__), &c__1);
				i__2 = *n - i__;
				i__3 = i__ - 1;
				NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b6, &a_ref (i__ + 1, 1), lda, &a_ref (i__ + 1, i__),
				               &c__1, &c_b16, &w_ref (1, i__), &c__1);
				i__2 = *n - i__;
				i__3 = i__ - 1;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b5, &w_ref (i__ + 1, 1), ldw, &w_ref (1, i__), &c__1,
				               &c_b6, &w_ref (i__ + 1, i__), &c__1);
				i__2 = *n - i__;
				NUMblas_dscal (&i__2, &tau[i__], &w_ref (i__ + 1, i__), &c__1);
				i__2 = *n - i__;
				alpha =
				    tau[i__] * -.5 * NUMblas_ddot (&i__2, &w_ref (i__ + 1, i__), &c__1, &a_ref (i__ + 1, i__),
				                                   &c__1);
				i__2 = *n - i__;
				NUMblas_daxpy (&i__2, &alpha, &a_ref (i__ + 1, i__), &c__1, &w_ref (i__ + 1, i__), &c__1);
			}

			/* L20: */
		}
	}

	return 0;
}								/* NUMlapack_dlatrd */

#undef w_ref

int NUMlapack_dorg2l (long *m, long *n, long *k, double *a, long *lda, double *tau, double *work, long *info) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;
	double d__1;

	/* Local variables */
	static long i__, j, l;
	static long ii;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0 || *n > *m) {
		*info = -2;
	} else if (*k < 0 || *k > *n) {
		*info = -3;
	} else if (*lda < MAX (1, *m)) {
		*info = -5;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORG2L", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*n <= 0) {
		return 0;
	}

	/* Initialise columns 1:n-k to columns of the unit matrix */

	i__1 = *n - *k;
	for (j = 1; j <= i__1; ++j) {
		i__2 = *m;
		for (l = 1; l <= i__2; ++l) {
			a_ref (l, j) = 0.;
			/* L10: */
		}
		a_ref (*m - *n + j, j) = 1.;
		/* L20: */
	}

	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
		ii = *n - *k + i__;

		/* Apply H(i) to A(1:m-k+i,1:n-k+i) from the left */

		a_ref (*m - *n + ii, ii) = 1.;
		i__2 = *m - *n + ii;
		i__3 = ii - 1;
		NUMlapack_dlarf ("Left", &i__2, &i__3, &a_ref (1, ii), &c__1, &tau[i__], &a[a_offset], lda, &work[1]);
		i__2 = *m - *n + ii - 1;
		d__1 = -tau[i__];
		NUMblas_dscal (&i__2, &d__1, &a_ref (1, ii), &c__1);
		a_ref (*m - *n + ii, ii) = 1. - tau[i__];

		/* Set A(m-k+i+1:m,n-k+i) to zero */

		i__2 = *m;
		for (l = *m - *n + ii + 1; l <= i__2; ++l) {
			a_ref (l, ii) = 0.;
			/* L30: */
		}
		/* L40: */
	}
	return 0;
}								/* NUMlapack_dorg2l */

int NUMlapack_dorg2r (long *m, long *n, long *k, double *a, long *lda, double *tau, double *work, long *info) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2;
	double d__1;

	/* Local variables */
	static long i__, j, l;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0 || *n > *m) {
		*info = -2;
	} else if (*k < 0 || *k > *n) {
		*info = -3;
	} else if (*lda < MAX (1, *m)) {
		*info = -5;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORG2R", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*n <= 0) {
		return 0;
	}

	/* Initialise columns k+1:n to columns of the unit matrix */

	i__1 = *n;
	for (j = *k + 1; j <= i__1; ++j) {
		i__2 = *m;
		for (l = 1; l <= i__2; ++l) {
			a_ref (l, j) = 0.;
			/* L10: */
		}
		a_ref (j, j) = 1.;
		/* L20: */
	}

	for (i__ = *k; i__ >= 1; --i__) {

		/* Apply H(i) to A(i:m,i:n) from the left */

		if (i__ < *n) {
			a_ref (i__, i__) = 1.;
			i__1 = *m - i__ + 1;
			i__2 = *n - i__;
			NUMlapack_dlarf ("Left", &i__1, &i__2, &a_ref (i__, i__), &c__1, &tau[i__], &a_ref (i__, i__ + 1),
			                 lda, &work[1]);
		}
		if (i__ < *m) {
			i__1 = *m - i__;
			d__1 = -tau[i__];
			NUMblas_dscal (&i__1, &d__1, &a_ref (i__ + 1, i__), &c__1);
		}
		a_ref (i__, i__) = 1. - tau[i__];

		/* Set A(1:i-1,i) to zero */

		i__1 = i__ - 1;
		for (l = 1; l <= i__1; ++l) {
			a_ref (l, i__) = 0.;
			/* L30: */
		}
		/* L40: */
	}
	return 0;
}								/* NUMlapack_dorg2r */

int NUMlapack_dorgbr (const char *vect, long *m, long *n, long *k, double *a, long *lda, double *tau, double *work,
                      long *lwork, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;

	/* Local variables */
	static long i__, j;
	static long iinfo;
	static long wantq;
	static long nb, mn;
	static long lwkopt;
	static long lquery;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	wantq = lsame_ (vect, "Q");
	mn = MIN (*m, *n);
	lquery = *lwork == -1;
	if (!wantq && !lsame_ (vect, "P")) {
		*info = -1;
	} else if (*m < 0) {
		*info = -2;
	} else if (*n < 0 || wantq && (*n > *m || *n < MIN (*m, *k)) || !wantq && (*m > *n || *m < MIN (*n, *k))) {
		*info = -3;
	} else if (*k < 0) {
		*info = -4;
	} else if (*lda < MAX (1, *m)) {
		*info = -6;
	} else if (*lwork < MAX (1, mn) && !lquery) {
		*info = -9;
	}

	if (*info == 0) {
		if (wantq) {
			nb = NUMlapack_ilaenv (&c__1, "DORGQR", " ", m, n, k, &c_n1, 6, 1);
		} else {
			nb = NUMlapack_ilaenv (&c__1, "DORGLQ", " ", m, n, k, &c_n1, 6, 1);
		}
		lwkopt = MAX (1, mn) * nb;
		work[1] = (double) lwkopt;
	}

	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORGBR", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*m == 0 || *n == 0) {
		work[1] = 1.;
		return 0;
	}

	if (wantq) {

		/* Form Q, determined by a call to DGEBRD to reduce an m-by-k matrix */

		if (*m >= *k) {

			/* If m >= k, assume m >= n >= k */

			NUMlapack_dorgqr (m, n, k, &a[a_offset], lda, &tau[1], &work[1], lwork, &iinfo);

		} else {

			/* If m < k, assume m = n

			   Shift the vectors which define the elementary reflectors one
			   column to the right, and set the first row and column of Q to
			   those of the unit matrix */

			for (j = *m; j >= 2; --j) {
				a_ref (1, j) = 0.;
				i__1 = *m;
				for (i__ = j + 1; i__ <= i__1; ++i__) {
					a_ref (i__, j) = a_ref (i__, j - 1);
					/* L10: */
				}
				/* L20: */
			}
			a_ref (1, 1) = 1.;
			i__1 = *m;
			for (i__ = 2; i__ <= i__1; ++i__) {
				a_ref (i__, 1) = 0.;
				/* L30: */
			}
			if (*m > 1) {

				/* Form Q(2:m,2:m) */

				i__1 = *m - 1;
				i__2 = *m - 1;
				i__3 = *m - 1;
				NUMlapack_dorgqr (&i__1, &i__2, &i__3, &a_ref (2, 2), lda, &tau[1], &work[1], lwork, &iinfo);
			}
		}
	} else {

		/* Form P', determined by a call to DGEBRD to reduce a k-by-n matrix */

		if (*k < *n) {

			/* If k < n, assume k <= m <= n */

			NUMlapack_dorglq (m, n, k, &a[a_offset], lda, &tau[1], &work[1], lwork, &iinfo);

		} else {

			/* If k >= n, assume m = n

			   Shift the vectors which define the elementary reflectors one
			   row downward, and set the first row and column of P' to those
			   of the unit matrix */

			a_ref (1, 1) = 1.;
			i__1 = *n;
			for (i__ = 2; i__ <= i__1; ++i__) {
				a_ref (i__, 1) = 0.;
				/* L40: */
			}
			i__1 = *n;
			for (j = 2; j <= i__1; ++j) {
				for (i__ = j - 1; i__ >= 2; --i__) {
					a_ref (i__, j) = a_ref (i__ - 1, j);
					/* L50: */
				}
				a_ref (1, j) = 0.;
				/* L60: */
			}
			if (*n > 1) {

				/* Form P'(2:n,2:n) */

				i__1 = *n - 1;
				i__2 = *n - 1;
				i__3 = *n - 1;
				NUMlapack_dorglq (&i__1, &i__2, &i__3, &a_ref (2, 2), lda, &tau[1], &work[1], lwork, &iinfo);
			}
		}
	}
	work[1] = (double) lwkopt;
	return 0;
}								/* NUMlapack_dorgbr */


int NUMlapack_dorghr (long *n, long *ilo, long *ihi, double *a, long *lda, double *tau, double *work,
                      long *lwork, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2;

	/* Local variables */
	static long i__, j, iinfo, nb, nh;
	static long lwkopt;
	static int lquery;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	nh = *ihi - *ilo;
	lquery = *lwork == -1;
	if (*n < 0) {
		*info = -1;
	} else if (*ilo < 1 || *ilo > MAX (1, *n)) {
		*info = -2;
	} else if (*ihi < MIN (*ilo, *n) || *ihi > *n) {
		*info = -3;
	} else if (*lda < MAX (1, *n)) {
		*info = -5;
	} else if (*lwork < MAX (1, nh) && !lquery) {
		*info = -8;
	}

	if (*info == 0) {
		nb = NUMlapack_ilaenv (&c__1, "DORGQR", " ", &nh, &nh, &nh, &c_n1, 6, 1);
		lwkopt = MAX (1, nh) * nb;
		work[1] = (double) lwkopt;
	}

	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORGHR", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*n == 0) {
		work[1] = 1.;
		return 0;
	}

	/* Shift the vectors which define the elementary reflectors one column to
	   the right, and set the first ilo and the last n-ihi rows and columns
	   to those of the unit matrix */

	i__1 = *ilo + 1;
	for (j = *ihi; j >= i__1; --j) {
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
			a_ref (i__, j) = 0.;
			/* L10: */
		}
		i__2 = *ihi;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
			a_ref (i__, j) = a_ref (i__, j - 1);
			/* L20: */
		}
		i__2 = *n;
		for (i__ = *ihi + 1; i__ <= i__2; ++i__) {
			a_ref (i__, j) = 0.;
			/* L30: */
		}
		/* L40: */
	}
	i__1 = *ilo;
	for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
			a_ref (i__, j) = 0.;
			/* L50: */
		}
		a_ref (j, j) = 1.;
		/* L60: */
	}
	i__1 = *n;
	for (j = *ihi + 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
			a_ref (i__, j) = 0.;
			/* L70: */
		}
		a_ref (j, j) = 1.;
		/* L80: */
	}

	if (nh > 0) {

		/* Generate Q(ilo+1:ihi,ilo+1:ihi) */

		NUMlapack_dorgqr (&nh, &nh, &nh, &a_ref (*ilo + 1, *ilo + 1), lda, &tau[*ilo], &work[1], lwork,
		                  &iinfo);
	}
	work[1] = (double) lwkopt;
	return 0;
}								/* NUMlapack_dorghr */


int NUMlapack_dorgl2 (long *m, long *n, long *k, double *a, long *lda, double *tau, double *work, long *info) {
	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2;
	double d__1;

	/* Local variables */
	static long i__, j, l;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	if (*m < 0) {
		*info = -1;
	} else if (*n < *m) {
		*info = -2;
	} else if (*k < 0 || *k > *m) {
		*info = -3;
	} else if (*lda < MAX (1, *m)) {
		*info = -5;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORGL2", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*m <= 0) {
		return 0;
	}

	if (*k < *m) {

		/* Initialise rows k+1:m to rows of the unit matrix */

		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (l = *k + 1; l <= i__2; ++l) {
				a_ref (l, j) = 0.;
				/* L10: */
			}
			if (j > *k && j <= *m) {
				a_ref (j, j) = 1.;
			}
			/* L20: */
		}
	}

	for (i__ = *k; i__ >= 1; --i__) {

		/* Apply H(i) to A(i:m,i:n) from the right */

		if (i__ < *n) {
			if (i__ < *m) {
				a_ref (i__, i__) = 1.;
				i__1 = *m - i__;
				i__2 = *n - i__ + 1;
				NUMlapack_dlarf ("Right", &i__1, &i__2, &a_ref (i__, i__), lda, &tau[i__], &a_ref (i__ + 1,
				                 i__), lda, &work[1]);
			}
			i__1 = *n - i__;
			d__1 = -tau[i__];
			NUMblas_dscal (&i__1, &d__1, &a_ref (i__, i__ + 1), lda);
		}
		a_ref (i__, i__) = 1. - tau[i__];

		/* Set A(i,1:i-1) to zero */

		i__1 = i__ - 1;
		for (l = 1; l <= i__1; ++l) {
			a_ref (i__, l) = 0.;
			/* L30: */
		}
		/* L40: */
	}
	return 0;
}								/* NUMlapack_dorgl2 */

int NUMlapack_dorglq (long *m, long *n, long *k, double *a, long *lda, double *tau, double *work, long *lwork,
                      long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__3 = 3;
	static long c__2 = 2;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;

	/* Local variables */
	static long i__, j, l, nbmin, iinfo;
	static long ib, nb, ki, kk;
	static long nx;
	static long ldwork, lwkopt;
	static long lquery;
	static long iws;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	nb = NUMlapack_ilaenv (&c__1, "DORGLQ", " ", m, n, k, &c_n1, 6, 1);
	lwkopt = MAX (1, *m) * nb;
	work[1] = (double) lwkopt;
	lquery = *lwork == -1;
	if (*m < 0) {
		*info = -1;
	} else if (*n < *m) {
		*info = -2;
	} else if (*k < 0 || *k > *m) {
		*info = -3;
	} else if (*lda < MAX (1, *m)) {
		*info = -5;
	} else if (*lwork < MAX (1, *m) && !lquery) {
		*info = -8;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORGLQ", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*m <= 0) {
		work[1] = 1.;
		return 0;
	}

	nbmin = 2;
	nx = 0;
	iws = *m;
	if (nb > 1 && nb < *k) {

		/* Determine when to cross over from blocked to unblocked code.

		   Computing MAX */
		i__1 = 0, i__2 = NUMlapack_ilaenv (&c__3, "DORGLQ", " ", m, n, k, &c_n1, 6, 1);
		nx = MAX (i__1, i__2);
		if (nx < *k) {

			/* Determine if workspace is large enough for blocked code. */

			ldwork = *m;
			iws = ldwork * nb;
			if (*lwork < iws) {

				/* Not enough workspace to use optimal NB: reduce NB and
				   determine the minimum value of NB. */

				nb = *lwork / ldwork;
				/* Computing MAX */
				i__1 = 2, i__2 = NUMlapack_ilaenv (&c__2, "DORGLQ", " ", m, n, k, &c_n1, 6, 1);
				nbmin = MAX (i__1, i__2);
			}
		}
	}

	if (nb >= nbmin && nb < *k && nx < *k) {

		/* Use blocked code after the last block. The first kk rows are
		   handled by the block method. */

		ki = (*k - nx - 1) / nb * nb;
		/* Computing MIN */
		i__1 = *k, i__2 = ki + nb;
		kk = MIN (i__1, i__2);

		/* Set A(kk+1:m,1:kk) to zero. */

		i__1 = kk;
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = kk + 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = 0.;
				/* L10: */
			}
			/* L20: */
		}
	} else {
		kk = 0;
	}

	/* Use unblocked code for the last or only block. */

	if (kk < *m) {
		i__1 = *m - kk;
		i__2 = *n - kk;
		i__3 = *k - kk;
		NUMlapack_dorgl2 (&i__1, &i__2, &i__3, &a_ref (kk + 1, kk + 1), lda, &tau[kk + 1], &work[1], &iinfo);
	}

	if (kk > 0) {

		/* Use blocked code */

		i__1 = -nb;
		for (i__ = ki + 1; i__1 < 0 ? i__ >= 1 : i__ <= 1; i__ += i__1) {
			/* Computing MIN */
			i__2 = nb, i__3 = *k - i__ + 1;
			ib = MIN (i__2, i__3);
			if (i__ + ib <= *m) {

				/* Form the triangular factor of the block reflector H = H(i)
				   H(i+1) . . . H(i+ib-1) */

				i__2 = *n - i__ + 1;
				NUMlapack_dlarft ("Forward", "Rowwise", &i__2, &ib, &a_ref (i__, i__), lda, &tau[i__], &work[1],
				                  &ldwork);

				/* Apply H' to A(i+ib:m,i:n) from the right */

				i__2 = *m - i__ - ib + 1;
				i__3 = *n - i__ + 1;
				NUMlapack_dlarfb ("Right", "Transpose", "Forward", "Rowwise", &i__2, &i__3, &ib, &a_ref (i__,
				                  i__), lda, &work[1], &ldwork, &a_ref (i__ + ib, i__), lda, &work[ib + 1], &ldwork);
			}

			/* Apply H' to columns i:n of current block */

			i__2 = *n - i__ + 1;
			NUMlapack_dorgl2 (&ib, &i__2, &ib, &a_ref (i__, i__), lda, &tau[i__], &work[1], &iinfo);

			/* Set columns 1:i-1 of current block to zero */

			i__2 = i__ - 1;
			for (j = 1; j <= i__2; ++j) {
				i__3 = i__ + ib - 1;
				for (l = i__; l <= i__3; ++l) {
					a_ref (l, j) = 0.;
					/* L30: */
				}
				/* L40: */
			}
			/* L50: */
		}
	}

	work[1] = (double) iws;
	return 0;
}								/* NUMlapack_dorglq */

int NUMlapack_dorgql (long *m, long *n, long *k, double *a, long *lda, double *tau, double *work, long *lwork,
                      long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__3 = 3;
	static long c__2 = 2;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3, i__4;

	/* Local variables */
	static long i__, j, l, nbmin, iinfo;
	static long ib, nb, kk;
	static long nx;
	static long ldwork, lwkopt;
	static long lquery;
	static long iws;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	nb = NUMlapack_ilaenv (&c__1, "DORGQL", " ", m, n, k, &c_n1, 6, 1);
	lwkopt = MAX (1, *n) * nb;
	work[1] = (double) lwkopt;
	lquery = *lwork == -1;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0 || *n > *m) {
		*info = -2;
	} else if (*k < 0 || *k > *n) {
		*info = -3;
	} else if (*lda < MAX (1, *m)) {
		*info = -5;
	} else if (*lwork < MAX (1, *n) && !lquery) {
		*info = -8;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORGQL", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*n <= 0) {
		work[1] = 1.;
		return 0;
	}

	nbmin = 2;
	nx = 0;
	iws = *n;
	if (nb > 1 && nb < *k) {

		/* Determine when to cross over from blocked to unblocked code.

		   Computing MAX */
		i__1 = 0, i__2 = NUMlapack_ilaenv (&c__3, "DORGQL", " ", m, n, k, &c_n1, 6, 1);
		nx = MAX (i__1, i__2);
		if (nx < *k) {

			/* Determine if workspace is large enough for blocked code. */

			ldwork = *n;
			iws = ldwork * nb;
			if (*lwork < iws) {

				/* Not enough workspace to use optimal NB: reduce NB and
				   determine the minimum value of NB. */

				nb = *lwork / ldwork;
				/* Computing MAX */
				i__1 = 2, i__2 = NUMlapack_ilaenv (&c__2, "DORGQL", " ", m, n, k, &c_n1, 6, 1);
				nbmin = MAX (i__1, i__2);
			}
		}
	}

	if (nb >= nbmin && nb < *k && nx < *k) {

		/* Use blocked code after the first block. The last kk columns are
		   handled by the block method.

		   Computing MIN */
		i__1 = *k, i__2 = (*k - nx + nb - 1) / nb * nb;
		kk = MIN (i__1, i__2);

		/* Set A(m-kk+1:m,1:n-kk) to zero. */

		i__1 = *n - kk;
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = *m - kk + 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = 0.;
				/* L10: */
			}
			/* L20: */
		}
	} else {
		kk = 0;
	}

	/* Use unblocked code for the first or only block. */

	i__1 = *m - kk;
	i__2 = *n - kk;
	i__3 = *k - kk;
	NUMlapack_dorg2l (&i__1, &i__2, &i__3, &a[a_offset], lda, &tau[1], &work[1], &iinfo);

	if (kk > 0) {

		/* Use blocked code */

		i__1 = *k;
		i__2 = nb;
		for (i__ = *k - kk + 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
			/* Computing MIN */
			i__3 = nb, i__4 = *k - i__ + 1;
			ib = MIN (i__3, i__4);
			if (*n - *k + i__ > 1) {

				/* Form the triangular factor of the block reflector H =
				   H(i+ib-1) . . . H(i+1) H(i) */

				i__3 = *m - *k + i__ + ib - 1;
				NUMlapack_dlarft ("Backward", "Columnwise", &i__3, &ib, &a_ref (1, *n - *k + i__), lda,
				                  &tau[i__], &work[1], &ldwork);

				/* Apply H to A(1:m-k+i+ib-1,1:n-k+i-1) from the left */

				i__3 = *m - *k + i__ + ib - 1;
				i__4 = *n - *k + i__ - 1;
				NUMlapack_dlarfb ("Left", "No transpose", "Backward", "Columnwise", &i__3, &i__4, &ib,
				                  &a_ref (1, *n - *k + i__), lda, &work[1], &ldwork, &a[a_offset], lda, &work[ib + 1],
				                  &ldwork);
			}

			/* Apply H to rows 1:m-k+i+ib-1 of current block */

			i__3 = *m - *k + i__ + ib - 1;
			NUMlapack_dorg2l (&i__3, &ib, &ib, &a_ref (1, *n - *k + i__), lda, &tau[i__], &work[1], &iinfo);

			/* Set rows m-k+i+ib:m of current block to zero */

			i__3 = *n - *k + i__ + ib - 1;
			for (j = *n - *k + i__; j <= i__3; ++j) {
				i__4 = *m;
				for (l = *m - *k + i__ + ib; l <= i__4; ++l) {
					a_ref (l, j) = 0.;
					/* L30: */
				}
				/* L40: */
			}
			/* L50: */
		}
	}

	work[1] = (double) iws;
	return 0;
}								/* NUMlapack_dorgql */

int NUMlapack_dorgqr (long *m, long *n, long *k, double *a, long *lda, double *tau, double *work, long *lwork,
                      long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__3 = 3;
	static long c__2 = 2;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;

	/* Local variables */
	static long i__, j, l, nbmin, iinfo;
	static long ib, nb, ki, kk;
	static long nx;
	static long ldwork, lwkopt;
	static long lquery;
	static long iws;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	nb = NUMlapack_ilaenv (&c__1, "DORGQR", " ", m, n, k, &c_n1, 6, 1);
	lwkopt = MAX (1, *n) * nb;
	work[1] = (double) lwkopt;
	lquery = *lwork == -1;
	if (*m < 0) {
		*info = -1;
	} else if (*n < 0 || *n > *m) {
		*info = -2;
	} else if (*k < 0 || *k > *n) {
		*info = -3;
	} else if (*lda < MAX (1, *m)) {
		*info = -5;
	} else if (*lwork < MAX (1, *n) && !lquery) {
		*info = -8;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORGQR", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*n <= 0) {
		work[1] = 1.;
		return 0;
	}

	nbmin = 2;
	nx = 0;
	iws = *n;
	if (nb > 1 && nb < *k) {

		/* Determine when to cross over from blocked to unblocked code.

		   Computing MAX */
		i__1 = 0, i__2 = NUMlapack_ilaenv (&c__3, "DORGQR", " ", m, n, k, &c_n1, 6, 1);
		nx = MAX (i__1, i__2);
		if (nx < *k) {

			/* Determine if workspace is large enough for blocked code. */

			ldwork = *n;
			iws = ldwork * nb;
			if (*lwork < iws) {

				/* Not enough workspace to use optimal NB: reduce NB and
				   determine the minimum value of NB. */

				nb = *lwork / ldwork;
				/* Computing MAX */
				i__1 = 2, i__2 = NUMlapack_ilaenv (&c__2, "DORGQR", " ", m, n, k, &c_n1, 6, 1);
				nbmin = MAX (i__1, i__2);
			}
		}
	}

	if (nb >= nbmin && nb < *k && nx < *k) {

		/* Use blocked code after the last block. The first kk columns are
		   handled by the block method. */

		ki = (*k - nx - 1) / nb * nb;
		/* Computing MIN */
		i__1 = *k, i__2 = ki + nb;
		kk = MIN (i__1, i__2);

		/* Set A(1:kk,kk+1:n) to zero. */

		i__1 = *n;
		for (j = kk + 1; j <= i__1; ++j) {
			i__2 = kk;
			for (i__ = 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = 0.;
				/* L10: */
			}
			/* L20: */
		}
	} else {
		kk = 0;
	}

	/* Use unblocked code for the last or only block. */

	if (kk < *n) {
		i__1 = *m - kk;
		i__2 = *n - kk;
		i__3 = *k - kk;
		NUMlapack_dorg2r (&i__1, &i__2, &i__3, &a_ref (kk + 1, kk + 1), lda, &tau[kk + 1], &work[1], &iinfo);
	}

	if (kk > 0) {

		/* Use blocked code */

		i__1 = -nb;
		for (i__ = ki + 1; i__1 < 0 ? i__ >= 1 : i__ <= 1; i__ += i__1) {
			/* Computing MIN */
			i__2 = nb, i__3 = *k - i__ + 1;
			ib = MIN (i__2, i__3);
			if (i__ + ib <= *n) {

				/* Form the triangular factor of the block reflector H = H(i)
				   H(i+1) . . . H(i+ib-1) */

				i__2 = *m - i__ + 1;
				NUMlapack_dlarft ("Forward", "Columnwise", &i__2, &ib, &a_ref (i__, i__), lda, &tau[i__],
				                  &work[1], &ldwork);

				/* Apply H to A(i:m,i+ib:n) from the left */

				i__2 = *m - i__ + 1;
				i__3 = *n - i__ - ib + 1;
				NUMlapack_dlarfb ("Left", "No transpose", "Forward", "Columnwise", &i__2, &i__3, &ib,
				                  &a_ref (i__, i__), lda, &work[1], &ldwork, &a_ref (i__, i__ + ib), lda, &work[ib + 1],
				                  &ldwork);
			}

			/* Apply H to rows i:m of current block */

			i__2 = *m - i__ + 1;
			NUMlapack_dorg2r (&i__2, &ib, &ib, &a_ref (i__, i__), lda, &tau[i__], &work[1], &iinfo);

			/* Set rows 1:i-1 of current block to zero */

			i__2 = i__ + ib - 1;
			for (j = i__; j <= i__2; ++j) {
				i__3 = i__ - 1;
				for (l = 1; l <= i__3; ++l) {
					a_ref (l, j) = 0.;
					/* L30: */
				}
				/* L40: */
			}
			/* L50: */
		}
	}

	work[1] = (double) iws;
	return 0;
}								/* NUMlapack_dorgqr */

int NUMlapack_dorgtr (const char *uplo, long *n, double *a, long *lda, double *tau, double *work, long *lwork,
                      long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;

	/* Local variables */
	static long i__, j;
	static long iinfo;
	static long upper;
	static long nb;
	static long lwkopt;
	static long lquery;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	lquery = *lwork == -1;
	upper = lsame_ (uplo, "U");
	if (!upper && !lsame_ (uplo, "L")) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *n)) {
		*info = -4;
	} else {					/* if(complicated condition) */
		/* Computing MAX */
		i__1 = 1, i__2 = *n - 1;
		if (*lwork < MAX (i__1, i__2) && !lquery) {
			*info = -7;
		}
	}

	if (*info == 0) {
		if (upper) {
			i__1 = *n - 1;
			i__2 = *n - 1;
			i__3 = *n - 1;
			nb = NUMlapack_ilaenv (&c__1, "DORGQL", " ", &i__1, &i__2, &i__3, &c_n1, 6, 1);
		} else {
			i__1 = *n - 1;
			i__2 = *n - 1;
			i__3 = *n - 1;
			nb = NUMlapack_ilaenv (&c__1, "DORGQR", " ", &i__1, &i__2, &i__3, &c_n1, 6, 1);
		}
		/* Computing MAX */
		i__1 = 1, i__2 = *n - 1;
		lwkopt = MAX (i__1, i__2) * nb;
		work[1] = (double) lwkopt;
	}

	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORGTR", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*n == 0) {
		work[1] = 1.;
		return 0;
	}

	if (upper) {

		/* Q was determined by a call to DSYTRD with UPLO = 'U'

		   Shift the vectors which define the elementary reflectors one
		   column to the left, and set the last row and column of Q to those
		   of the unit matrix */

		i__1 = *n - 1;
		for (j = 1; j <= i__1; ++j) {
			i__2 = j - 1;
			for (i__ = 1; i__ <= i__2; ++i__) {
				a_ref (i__, j) = a_ref (i__, j + 1);
				/* L10: */
			}
			a_ref (*n, j) = 0.;
			/* L20: */
		}
		i__1 = *n - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {
			a_ref (i__, *n) = 0.;
			/* L30: */
		}
		a_ref (*n, *n) = 1.;

		/* Generate Q(1:n-1,1:n-1) */

		i__1 = *n - 1;
		i__2 = *n - 1;
		i__3 = *n - 1;
		NUMlapack_dorgql (&i__1, &i__2, &i__3, &a[a_offset], lda, &tau[1], &work[1], lwork, &iinfo);

	} else {

		/* Q was determined by a call to DSYTRD with UPLO = 'L'.

		   Shift the vectors which define the elementary reflectors one
		   column to the right, and set the first row and column of Q to
		   those of the unit matrix */

		for (j = *n; j >= 2; --j) {
			a_ref (1, j) = 0.;
			i__1 = *n;
			for (i__ = j + 1; i__ <= i__1; ++i__) {
				a_ref (i__, j) = a_ref (i__, j - 1);
				/* L40: */
			}
			/* L50: */
		}
		a_ref (1, 1) = 1.;
		i__1 = *n;
		for (i__ = 2; i__ <= i__1; ++i__) {
			a_ref (i__, 1) = 0.;
			/* L60: */
		}
		if (*n > 1) {

			/* Generate Q(2:n,2:n) */

			i__1 = *n - 1;
			i__2 = *n - 1;
			i__3 = *n - 1;
			NUMlapack_dorgqr (&i__1, &i__2, &i__3, &a_ref (2, 2), lda, &tau[1], &work[1], lwork, &iinfo);
		}
	}
	work[1] = (double) lwkopt;
	return 0;
}								/* NUMlapack_dorgtr */

int NUMlapack_dorm2r (const char *side, const char *trans, long *m, long *n, long *k, double *a, long *lda, double *tau,
                      double *c__, long *ldc, double *work, long *info) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, c_dim1, c_offset, i__1, i__2;

	/* Local variables */
	static long left;
	static long i__;
	static long i1, i2, i3, ic, jc, mi, ni, nq;
	static long notran;
	static double aii;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1 * 1;
	c__ -= c_offset;
	--work;

	/* Function Body */
	*info = 0;
	left = lsame_ (side, "L");
	notran = lsame_ (trans, "N");

	/* NQ is the order of Q */

	if (left) {
		nq = *m;
	} else {
		nq = *n;
	}
	if (!left && !lsame_ (side, "R")) {
		*info = -1;
	} else if (!notran && !lsame_ (trans, "T")) {
		*info = -2;
	} else if (*m < 0) {
		*info = -3;
	} else if (*n < 0) {
		*info = -4;
	} else if (*k < 0 || *k > nq) {
		*info = -5;
	} else if (*lda < MAX (1, nq)) {
		*info = -7;
	} else if (*ldc < MAX (1, *m)) {
		*info = -10;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORM2R", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*m == 0 || *n == 0 || *k == 0) {
		return 0;
	}

	if (left && !notran || !left && notran) {
		i1 = 1;
		i2 = *k;
		i3 = 1;
	} else {
		i1 = *k;
		i2 = 1;
		i3 = -1;
	}

	if (left) {
		ni = *n;
		jc = 1;
	} else {
		mi = *m;
		ic = 1;
	}

	i__1 = i2;
	i__2 = i3;
	for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
		if (left) {

			/* H(i) is applied to C(i:m,1:n) */

			mi = *m - i__ + 1;
			ic = i__;
		} else {

			/* H(i) is applied to C(1:m,i:n) */

			ni = *n - i__ + 1;
			jc = i__;
		}

		/* Apply H(i) */

		aii = a_ref (i__, i__);
		a_ref (i__, i__) = 1.;
		NUMlapack_dlarf (side, &mi, &ni, &a_ref (i__, i__), &c__1, &tau[i__], &c___ref (ic, jc), ldc, &work[1]);
		a_ref (i__, i__) = aii;
		/* L10: */
	}
	return 0;
}								/* NUMlapack_dorm2r */

int NUMlapack_dormbr (const char *vect, const char *side, const char *trans, long *m, long *n, long *k, double *a, long *lda,
                      double *tau, double *c__, long *ldc, double *work, long *lwork, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__2 = 2;

	/* System generated locals */
	const char *a__1[2];
	long a_dim1, a_offset, c_dim1, c_offset, i__1, i__2, i__3[2];
	char ch__1[2];

	/* Local variables */
	static long left;
	static long iinfo, i1, i2, nb, mi, ni, nq, nw;
	static long notran;
	static long applyq;
	static char transt[1];
	static long lwkopt;
	static long lquery;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1 * 1;
	c__ -= c_offset;
	--work;

	/* Function Body */
	*info = 0;
	applyq = lsame_ (vect, "Q");
	left = lsame_ (side, "L");
	notran = lsame_ (trans, "N");
	lquery = *lwork == -1;

	/* NQ is the order of Q or P and NW is the minimum dimension of WORK */

	if (left) {
		nq = *m;
		nw = *n;
	} else {
		nq = *n;
		nw = *m;
	}
	if (!applyq && !lsame_ (vect, "P")) {
		*info = -1;
	} else if (!left && !lsame_ (side, "R")) {
		*info = -2;
	} else if (!notran && !lsame_ (trans, "T")) {
		*info = -3;
	} else if (*m < 0) {
		*info = -4;
	} else if (*n < 0) {
		*info = -5;
	} else if (*k < 0) {
		*info = -6;
	} else {					/* if(complicated condition) */
		/* Computing MAX */
		i__1 = 1, i__2 = MIN (nq, *k);
		if (applyq && *lda < MAX (1, nq) || !applyq && *lda < MAX (i__1, i__2)) {
			*info = -8;
		} else if (*ldc < MAX (1, *m)) {
			*info = -11;
		} else if (*lwork < MAX (1, nw) && !lquery) {
			*info = -13;
		}
	}

	if (*info == 0) {
		if (applyq) {
			if (left) {
				/* Writing concatenation */
				i__3[0] = 1, a__1[0] = side;
				i__3[1] = 1, a__1[1] = trans;
				s_cat (ch__1, a__1, i__3, &c__2, 2);
				i__1 = *m - 1;
				i__2 = *m - 1;
				nb = NUMlapack_ilaenv (&c__1, "DORMQR", ch__1, &i__1, n, &i__2, &c_n1, 6, 2);
			} else {
				/* Writing concatenation */
				i__3[0] = 1, a__1[0] = side;
				i__3[1] = 1, a__1[1] = trans;
				s_cat (ch__1, a__1, i__3, &c__2, 2);
				i__1 = *n - 1;
				i__2 = *n - 1;
				nb = NUMlapack_ilaenv (&c__1, "DORMQR", ch__1, m, &i__1, &i__2, &c_n1, 6, 2);
			}
		} else {
			if (left) {
				/* Writing concatenation */
				i__3[0] = 1, a__1[0] = side;
				i__3[1] = 1, a__1[1] = trans;
				s_cat (ch__1, a__1, i__3, &c__2, 2);
				i__1 = *m - 1;
				i__2 = *m - 1;
				nb = NUMlapack_ilaenv (&c__1, "DORMLQ", ch__1, &i__1, n, &i__2, &c_n1, 6, 2);
			} else {
				/* Writing concatenation */
				i__3[0] = 1, a__1[0] = side;
				i__3[1] = 1, a__1[1] = trans;
				s_cat (ch__1, a__1, i__3, &c__2, 2);
				i__1 = *n - 1;
				i__2 = *n - 1;
				nb = NUMlapack_ilaenv (&c__1, "DORMLQ", ch__1, m, &i__1, &i__2, &c_n1, 6, 2);
			}
		}
		lwkopt = MAX (1, nw) * nb;
		work[1] = (double) lwkopt;
	}

	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORMBR", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	work[1] = 1.;
	if (*m == 0 || *n == 0) {
		return 0;
	}

	if (applyq) {

		/* Apply Q */

		if (nq >= *k) {

			/* Q was determined by a call to DGEBRD with nq >= k */

			NUMlapack_dormqr (side, trans, m, n, k, &a[a_offset], lda, &tau[1], &c__[c_offset], ldc, &work[1],
			                  lwork, &iinfo);
		} else if (nq > 1) {

			/* Q was determined by a call to DGEBRD with nq < k */

			if (left) {
				mi = *m - 1;
				ni = *n;
				i1 = 2;
				i2 = 1;
			} else {
				mi = *m;
				ni = *n - 1;
				i1 = 1;
				i2 = 2;
			}
			i__1 = nq - 1;
			NUMlapack_dormqr (side, trans, &mi, &ni, &i__1, &a_ref (2, 1), lda, &tau[1], &c___ref (i1, i2), ldc,
			                  &work[1], lwork, &iinfo);
		}
	} else {

		/* Apply P */

		if (notran) {
			* (unsigned char *) transt = 'T';
		} else {
			* (unsigned char *) transt = 'N';
		}
		if (nq > *k) {

			/* P was determined by a call to DGEBRD with nq > k */

			NUMlapack_dormlq (side, transt, m, n, k, &a[a_offset], lda, &tau[1], &c__[c_offset], ldc, &work[1],
			                  lwork, &iinfo);
		} else if (nq > 1) {

			/* P was determined by a call to DGEBRD with nq <= k */

			if (left) {
				mi = *m - 1;
				ni = *n;
				i1 = 2;
				i2 = 1;
			} else {
				mi = *m;
				ni = *n - 1;
				i1 = 1;
				i2 = 2;
			}
			i__1 = nq - 1;
			NUMlapack_dormlq (side, transt, &mi, &ni, &i__1, &a_ref (1, 2), lda, &tau[1], &c___ref (i1, i2),
			                  ldc, &work[1], lwork, &iinfo);
		}
	}
	work[1] = (double) lwkopt;
	return 0;
}								/* NUMlapack_dormbr */

int NUMlapack_dorml2 (const char *side, const char *trans, long *m, long *n, long *k, double *a, long *lda, double *tau,
                      double *c__, long *ldc, double *work, long *info) {
	/* System generated locals */
	long a_dim1, a_offset, c_dim1, c_offset, i__1, i__2;

	/* Local variables */
	static long left;
	static long i__;
	static long i1, i2, i3, ic, jc, mi, ni, nq;
	static long notran;
	static double aii;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1 * 1;
	c__ -= c_offset;
	--work;

	/* Function Body */
	*info = 0;
	left = lsame_ (side, "L");
	notran = lsame_ (trans, "N");

	/* NQ is the order of Q */

	if (left) {
		nq = *m;
	} else {
		nq = *n;
	}
	if (!left && !lsame_ (side, "R")) {
		*info = -1;
	} else if (!notran && !lsame_ (trans, "T")) {
		*info = -2;
	} else if (*m < 0) {
		*info = -3;
	} else if (*n < 0) {
		*info = -4;
	} else if (*k < 0 || *k > nq) {
		*info = -5;
	} else if (*lda < MAX (1, *k)) {
		*info = -7;
	} else if (*ldc < MAX (1, *m)) {
		*info = -10;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORML2", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*m == 0 || *n == 0 || *k == 0) {
		return 0;
	}

	if (left && notran || !left && !notran) {
		i1 = 1;
		i2 = *k;
		i3 = 1;
	} else {
		i1 = *k;
		i2 = 1;
		i3 = -1;
	}

	if (left) {
		ni = *n;
		jc = 1;
	} else {
		mi = *m;
		ic = 1;
	}

	i__1 = i2;
	i__2 = i3;
	for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
		if (left) {

			/* H(i) is applied to C(i:m,1:n) */

			mi = *m - i__ + 1;
			ic = i__;
		} else {

			/* H(i) is applied to C(1:m,i:n) */

			ni = *n - i__ + 1;
			jc = i__;
		}

		/* Apply H(i) */

		aii = a_ref (i__, i__);
		a_ref (i__, i__) = 1.;
		NUMlapack_dlarf (side, &mi, &ni, &a_ref (i__, i__), lda, &tau[i__], &c___ref (ic, jc), ldc, &work[1]);
		a_ref (i__, i__) = aii;
		/* L10: */
	}
	return 0;

}								/* NUMlapack_dorml2 */

int NUMlapack_dormlq (const char *side, const char *trans, long *m, long *n, long *k, double *a, long *lda, double *tau,
                      double *c__, long *ldc, double *work, long *lwork, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__2 = 2;
	static long c__65 = 65;

	/* System generated locals */
	char *a__1[2];
	long a_dim1, a_offset, c_dim1, c_offset, i__1, i__2, i__3[2], i__4, i__5;
	char ch__1[2];

	/* Local variables */
	static long left;
	static long i__;
	static double t[4160] /* was [65][64] */ ;
	static long nbmin, iinfo, i1, i2, i3;
	static long ib, ic, jc, nb, mi, ni;
	static long nq, nw;
	static long notran;
	static long ldwork;
	static char transt[1];
	static long lwkopt;
	static long lquery;
	static long iws;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1 * 1;
	c__ -= c_offset;
	--work;

	/* Function Body */
	*info = 0;
	left = lsame_ (side, "L");
	notran = lsame_ (trans, "N");
	lquery = *lwork == -1;

	/* NQ is the order of Q and NW is the minimum dimension of WORK */

	if (left) {
		nq = *m;
		nw = *n;
	} else {
		nq = *n;
		nw = *m;
	}
	if (!left && !lsame_ (side, "R")) {
		*info = -1;
	} else if (!notran && !lsame_ (trans, "T")) {
		*info = -2;
	} else if (*m < 0) {
		*info = -3;
	} else if (*n < 0) {
		*info = -4;
	} else if (*k < 0 || *k > nq) {
		*info = -5;
	} else if (*lda < MAX (1, *k)) {
		*info = -7;
	} else if (*ldc < MAX (1, *m)) {
		*info = -10;
	} else if (*lwork < MAX (1, nw) && !lquery) {
		*info = -12;
	}

	if (*info == 0) {

		/* Determine the block size.  NB may be at most NBMAX, where NBMAX is
		   used to define the local array T.

		   Computing MIN Writing concatenation */
		i__3[0] = 1, a__1[0] = (char *) side;
		i__3[1] = 1, a__1[1] = (char *) trans;
		s_cat (ch__1, (const char **) a__1, i__3, &c__2, 2);
		i__1 = 64, i__2 = NUMlapack_ilaenv (&c__1, "DORMLQ", ch__1, m, n, k, &c_n1, 6, 2);
		nb = MIN (i__1, i__2);
		lwkopt = MAX (1, nw) * nb;
		work[1] = (double) lwkopt;
	}

	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORMLQ", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*m == 0 || *n == 0 || *k == 0) {
		work[1] = 1.;
		return 0;
	}

	nbmin = 2;
	ldwork = nw;
	if (nb > 1 && nb < *k) {
		iws = nw * nb;
		if (*lwork < iws) {
			nb = *lwork / ldwork;
			/* Computing MAX Writing concatenation */
			i__3[0] = 1, a__1[0] = (char *) side;
			i__3[1] = 1, a__1[1] = (char *) trans;
			s_cat (ch__1, (const char **) a__1, i__3, &c__2, 2);
			i__1 = 2, i__2 = NUMlapack_ilaenv (&c__2, "DORMLQ", ch__1, m, n, k, &c_n1, 6, 2);
			nbmin = MAX (i__1, i__2);
		}
	} else {
		iws = nw;
	}

	if (nb < nbmin || nb >= *k) {

		/* Use unblocked code */

		NUMlapack_dorml2 (side, trans, m, n, k, &a[a_offset], lda, &tau[1], &c__[c_offset], ldc, &work[1],
		                  &iinfo);
	} else {

		/* Use blocked code */

		if (left && notran || !left && !notran) {
			i1 = 1;
			i2 = *k;
			i3 = nb;
		} else {
			i1 = (*k - 1) / nb * nb + 1;
			i2 = 1;
			i3 = -nb;
		}

		if (left) {
			ni = *n;
			jc = 1;
		} else {
			mi = *m;
			ic = 1;
		}

		if (notran) {
			* (unsigned char *) transt = 'T';
		} else {
			* (unsigned char *) transt = 'N';
		}

		i__1 = i2;
		i__2 = i3;
		for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
			/* Computing MIN */
			i__4 = nb, i__5 = *k - i__ + 1;
			ib = MIN (i__4, i__5);

			/* Form the triangular factor of the block reflector H = H(i)
			   H(i+1) . . . H(i+ib-1) */

			i__4 = nq - i__ + 1;
			NUMlapack_dlarft ("Forward", "Rowwise", &i__4, &ib, &a_ref (i__, i__), lda, &tau[i__], t, &c__65);
			if (left) {

				/* H or H' is applied to C(i:m,1:n) */

				mi = *m - i__ + 1;
				ic = i__;
			} else {

				/* H or H' is applied to C(1:m,i:n) */

				ni = *n - i__ + 1;
				jc = i__;
			}

			/* Apply H or H' */

			NUMlapack_dlarfb (side, transt, "Forward", "Rowwise", &mi, &ni, &ib, &a_ref (i__, i__), lda, t,
			                  &c__65, &c___ref (ic, jc), ldc, &work[1], &ldwork);
			/* L10: */
		}
	}
	work[1] = (double) lwkopt;
	return 0;
}								/* NUMlapack_dormlq */

int NUMlapack_dormqr (const char *side, const char *trans, long *m, long *n, long *k, double *a, long *lda, double *tau,
                      double *c__, long *ldc, double *work, long *lwork, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__2 = 2;
	static long c__65 = 65;

	/* System generated locals */
	char *a__1[2];
	long a_dim1, a_offset, c_dim1, c_offset, i__1, i__2, i__3[2], i__4, i__5;
	char ch__1[2];

	/* Local variables */
	static long left;
	static long i__;
	static double t[4160] /* was [65][64] */ ;
	static long nbmin, iinfo, i1, i2, i3;
	static long ib, ic, jc, nb, mi, ni;
	static long nq, nw;
	static long notran;
	static long ldwork, lwkopt;
	static long lquery;
	static long iws;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1 * 1;
	c__ -= c_offset;
	--work;

	/* Function Body */
	*info = 0;
	left = lsame_ (side, "L");
	notran = lsame_ (trans, "N");
	lquery = *lwork == -1;

	/* NQ is the order of Q and NW is the minimum dimension of WORK */

	if (left) {
		nq = *m;
		nw = *n;
	} else {
		nq = *n;
		nw = *m;
	}
	if (!left && !lsame_ (side, "R")) {
		*info = -1;
	} else if (!notran && !lsame_ (trans, "T")) {
		*info = -2;
	} else if (*m < 0) {
		*info = -3;
	} else if (*n < 0) {
		*info = -4;
	} else if (*k < 0 || *k > nq) {
		*info = -5;
	} else if (*lda < MAX (1, nq)) {
		*info = -7;
	} else if (*ldc < MAX (1, *m)) {
		*info = -10;
	} else if (*lwork < MAX (1, nw) && !lquery) {
		*info = -12;
	}

	if (*info == 0) {

		/* Determine the block size.  NB may be at most NBMAX, where NBMAX is
		   used to define the local array T.

		   Computing MIN Writing concatenation */
		i__3[0] = 1, a__1[0] = (char *) side;
		i__3[1] = 1, a__1[1] = (char *) trans;
		s_cat (ch__1, (const char **) a__1, i__3, &c__2, 2);
		i__1 = 64, i__2 = NUMlapack_ilaenv (&c__1, "DORMQR", ch__1, m, n, k, &c_n1, 6, 2);
		nb = MIN (i__1, i__2);
		lwkopt = MAX (1, nw) * nb;
		work[1] = (double) lwkopt;
	}

	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORMQR", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*m == 0 || *n == 0 || *k == 0) {
		work[1] = 1.;
		return 0;
	}

	nbmin = 2;
	ldwork = nw;
	if (nb > 1 && nb < *k) {
		iws = nw * nb;
		if (*lwork < iws) {
			nb = *lwork / ldwork;
			/* Computing MAX Writing concatenation */
			i__3[0] = 1, a__1[0] = (char *) side;
			i__3[1] = 1, a__1[1] = (char *) trans;
			s_cat (ch__1, (const char **) a__1, i__3, &c__2, 2);
			i__1 = 2, i__2 = NUMlapack_ilaenv (&c__2, "DORMQR", ch__1, m, n, k, &c_n1, 6, 2);
			nbmin = MAX (i__1, i__2);
		}
	} else {
		iws = nw;
	}

	if (nb < nbmin || nb >= *k) {

		/* Use unblocked code */

		NUMlapack_dorm2r (side, trans, m, n, k, &a[a_offset], lda, &tau[1], &c__[c_offset], ldc, &work[1],
		                  &iinfo);
	} else {

		/* Use blocked code */

		if (left && !notran || !left && notran) {
			i1 = 1;
			i2 = *k;
			i3 = nb;
		} else {
			i1 = (*k - 1) / nb * nb + 1;
			i2 = 1;
			i3 = -nb;
		}

		if (left) {
			ni = *n;
			jc = 1;
		} else {
			mi = *m;
			ic = 1;
		}

		i__1 = i2;
		i__2 = i3;
		for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
			/* Computing MIN */
			i__4 = nb, i__5 = *k - i__ + 1;
			ib = MIN (i__4, i__5);

			/* Form the triangular factor of the block reflector H = H(i)
			   H(i+1) . . . H(i+ib-1) */

			i__4 = nq - i__ + 1;
			NUMlapack_dlarft ("Forward", "Columnwise", &i__4, &ib, &a_ref (i__, i__), lda, &tau[i__], t,
			                  &c__65);
			if (left) {

				/* H or H' is applied to C(i:m,1:n) */

				mi = *m - i__ + 1;
				ic = i__;
			} else {

				/* H or H' is applied to C(1:m,i:n) */

				ni = *n - i__ + 1;
				jc = i__;
			}

			/* Apply H or H' */

			NUMlapack_dlarfb (side, trans, "Forward", "Columnwise", &mi, &ni, &ib, &a_ref (i__, i__), lda, t,
			                  &c__65, &c___ref (ic, jc), ldc, &work[1], &ldwork);
			/* L10: */
		}
	}
	work[1] = (double) lwkopt;
	return 0;
}								/* NUMlapack_dormqr */

int NUMlapack_dormr2 (const char *side, const char *trans, long *m, long *n, long *k, double *a, long *lda, double *tau,
                      double *c__, long *ldc, double *work, long *info) {
	/* System generated locals */
	long a_dim1, a_offset, c_dim1, c_offset, i__1, i__2;

	/* Local variables */
	static long left;
	static long i__;
	static long i1, i2, i3, mi, ni, nq;
	static long notran;
	static double aii;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--tau;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1 * 1;
	c__ -= c_offset;
	--work;

	/* Function Body */
	*info = 0;
	left = lsame_ (side, "L");
	notran = lsame_ (trans, "N");

	/* NQ is the order of Q */

	if (left) {
		nq = *m;
	} else {
		nq = *n;
	}
	if (!left && !lsame_ (side, "R")) {
		*info = -1;
	} else if (!notran && !lsame_ (trans, "T")) {
		*info = -2;
	} else if (*m < 0) {
		*info = -3;
	} else if (*n < 0) {
		*info = -4;
	} else if (*k < 0 || *k > nq) {
		*info = -5;
	} else if (*lda < MAX (1, *k)) {
		*info = -7;
	} else if (*ldc < MAX (1, *m)) {
		*info = -10;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DORMR2", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*m == 0 || *n == 0 || *k == 0) {
		return 0;
	}

	if (left && !notran || !left && notran) {
		i1 = 1;
		i2 = *k;
		i3 = 1;
	} else {
		i1 = *k;
		i2 = 1;
		i3 = -1;
	}

	if (left) {
		ni = *n;
	} else {
		mi = *m;
	}

	i__1 = i2;
	i__2 = i3;
	for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
		if (left) {

			/* H(i) is applied to C(1:m-k+i,1:n) */

			mi = *m - *k + i__;
		} else {

			/* H(i) is applied to C(1:m,1:n-k+i) */

			ni = *n - *k + i__;
		}

		/* Apply H(i) */

		aii = a_ref (i__, nq - *k + i__);
		a_ref (i__, nq - *k + i__) = 1.;
		NUMlapack_dlarf (side, &mi, &ni, &a_ref (i__, 1), lda, &tau[i__], &c__[c_offset], ldc, &work[1]);
		a_ref (i__, nq - *k + i__) = aii;
		/* L10: */
	}
	return 0;
}								/* NUMlapack_dormr2 */

int NUMlapack_dpotf2 (const char *uplo, long *n, double *a, long *lda, long *info) {
	/* Table of constant values */
	static double c_b10 = -1.;
	static double c_b12 = 1.;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;
	double d__1;

	/* Local variables */
	static long j;
	static int upper;
	static double ajj;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;

	/* Function Body */
	*info = 0;
	upper = lsame_ (uplo, "U");
	if (!upper && !lsame_ (uplo, "L")) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *n)) {
		*info = -4;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DPOTF2", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*n == 0) {
		return 0;
	}

	if (upper) {

		/* Compute the Cholesky factorization A = U'*U. */

		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {

			/* Compute U(J,J) and test for non-positive-definiteness. */

			i__2 = j - 1;
			ajj = a_ref (j, j) - NUMblas_ddot (&i__2, &a_ref (1, j), &c__1, &a_ref (1, j), &c__1);
			if (ajj <= 0.) {
				a_ref (j, j) = ajj;
				goto L30;
			}
			ajj = sqrt (ajj);
			a_ref (j, j) = ajj;

			/* Compute elements J+1:N of row J. */

			if (j < *n) {
				i__2 = j - 1;
				i__3 = *n - j;
				NUMblas_dgemv ("Transpose", &i__2, &i__3, &c_b10, &a_ref (1, j + 1), lda, &a_ref (1, j),
				               &c__1, &c_b12, &a_ref (j, j + 1), lda);
				i__2 = *n - j;
				d__1 = 1. / ajj;
				NUMblas_dscal (&i__2, &d__1, &a_ref (j, j + 1), lda);
			}
			/* L10: */
		}
	} else {

		/* Compute the Cholesky factorization A = L*L'. */

		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {

			/* Compute L(J,J) and test for non-positive-definiteness. */

			i__2 = j - 1;
			ajj = a_ref (j, j) - NUMblas_ddot (&i__2, &a_ref (j, 1), lda, &a_ref (j, 1), lda);
			if (ajj <= 0.) {
				a_ref (j, j) = ajj;
				goto L30;
			}
			ajj = sqrt (ajj);
			a_ref (j, j) = ajj;

			/* Compute elements J+1:N of column J. */

			if (j < *n) {
				i__2 = *n - j;
				i__3 = j - 1;
				NUMblas_dgemv ("No transpose", &i__2, &i__3, &c_b10, &a_ref (j + 1, 1), lda, &a_ref (j, 1),
				               lda, &c_b12, &a_ref (j + 1, j), &c__1);
				i__2 = *n - j;
				d__1 = 1. / ajj;
				NUMblas_dscal (&i__2, &d__1, &a_ref (j + 1, j), &c__1);
			}
			/* L20: */
		}
	}
	goto L40;

L30:
	*info = j;

L40:
	return 0;
}				/* NUMlapack_dpotf2_ */

int NUMlapack_drscl (long *n, double *sa, double *sx, long *incx) {
	static double cden;
	static long done;
	static double cnum, cden1, cnum1;
	static double bignum, smlnum, mul;

	--sx;

	/* Function Body */
	if (*n <= 0) {
		return 0;
	}

	/* Get machine parameters */

	smlnum = NUMblas_dlamch ("S");
	bignum = 1. / smlnum;
	NUMlapack_dlabad (&smlnum, &bignum);

	/* Initialize the denominator to SA and the numerator to 1. */

	cden = *sa;
	cnum = 1.;

L10:
	cden1 = cden * smlnum;
	cnum1 = cnum / bignum;
	if (fabs (cden1) > fabs (cnum) && cnum != 0.) {

		/* Pre-multiply X by SMLNUM if CDEN is large compared to CNUM. */

		mul = smlnum;
		done = FALSE;
		cden = cden1;
	} else if (fabs (cnum1) > fabs (cden)) {

		/* Pre-multiply X by BIGNUM if CDEN is small compared to CNUM. */

		mul = bignum;
		done = FALSE;
		cnum = cnum1;
	} else {

		/* Multiply X by CNUM / CDEN and return. */

		mul = cnum / cden;
		done = TRUE;
	}

	/* Scale the vector X by MUL */

	NUMblas_dscal (n, &mul, &sx[1], incx);

	if (!done) {
		goto L10;
	}

	return 0;
}								/* NUMlapack_drscl */

#define z___ref(a_1,a_2) z__[(a_2)*z_dim1 + a_1]

int NUMlapack_dsteqr (const char *compz, long *n, double *d__, double *e, double *z__, long *ldz, double *work,
                      long *info) {
	/* Table of constant values */
	static double c_b9 = 0.;
	static double c_b10 = 1.;
	static long c__0 = 0;
	static long c__1 = 1;
	static long c__2 = 2;

	/* System generated locals */
	long z_dim1, z_offset, i__1, i__2;
	double d__1, d__2;

	/* Local variables */
	static long lend, jtot;
	static double b, c__, f, g;
	static long i__, j, k, l, m;
	static double p, r__, s;
	static double anorm;
	static long l1;
	static long lendm1, lendp1;
	static long ii;
	static long mm, iscale;
	static double safmin;
	static double safmax;
	static long lendsv;
	static double ssfmin;
	static long nmaxit, icompz;
	static double ssfmax;
	static long lm1, mm1, nm1;
	static double rt1, rt2, eps;
	static long lsv;
	static double tst, eps2;

	--d__;
	--e;
	z_dim1 = *ldz;
	z_offset = 1 + z_dim1 * 1;
	z__ -= z_offset;
	--work;

	/* Function Body */
	*info = 0;

	if (lsame_ (compz, "N")) {
		icompz = 0;
	} else if (lsame_ (compz, "V")) {
		icompz = 1;
	} else if (lsame_ (compz, "I")) {
		icompz = 2;
	} else {
		icompz = -1;
	}
	if (icompz < 0) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*ldz < 1 || icompz > 0 && *ldz < MAX (1, *n)) {
		*info = -6;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DSTEQR", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*n == 0) {
		return 0;
	}

	if (*n == 1) {
		if (icompz == 2) {
			z___ref (1, 1) = 1.;
		}
		return 0;
	}

	/* Determine the unit roundoff and over/underflow thresholds. */

	eps = NUMblas_dlamch ("E");
	/* Computing 2nd power */
	d__1 = eps;
	eps2 = d__1 * d__1;
	safmin = NUMblas_dlamch ("S");
	safmax = 1. / safmin;
	ssfmax = sqrt (safmax) / 3.;
	ssfmin = sqrt (safmin) / eps2;

	/* Compute the eigenvalues and eigenvectors of the tridiagonal matrix. */

	if (icompz == 2) {
		NUMlapack_dlaset ("Full", n, n, &c_b9, &c_b10, &z__[z_offset], ldz);
	}

	nmaxit = *n * 30;
	jtot = 0;

	/* Determine where the matrix splits and choose QL or QR iteration for
	   each block, according to whether top or bottom diagonal element is
	   smaller. */

	l1 = 1;
	nm1 = *n - 1;

L10:
	if (l1 > *n) {
		goto L160;
	}
	if (l1 > 1) {
		e[l1 - 1] = 0.;
	}
	if (l1 <= nm1) {
		i__1 = nm1;
		for (m = l1; m <= i__1; ++m) {
			tst = (d__1 = e[m], fabs (d__1));
			if (tst == 0.) {
				goto L30;
			}
			if (tst <= sqrt ( (d__1 = d__[m], fabs (d__1))) * sqrt ( (d__2 = d__[m + 1], fabs (d__2))) * eps) {
				e[m] = 0.;
				goto L30;
			}
			/* L20: */
		}
	}
	m = *n;

L30:
	l = l1;
	lsv = l;
	lend = m;
	lendsv = lend;
	l1 = m + 1;
	if (lend == l) {
		goto L10;
	}

	/* Scale submatrix in rows and columns L to LEND */

	i__1 = lend - l + 1;
	anorm = NUMlapack_dlanst ("I", &i__1, &d__[l], &e[l]);
	iscale = 0;
	if (anorm == 0.) {
		goto L10;
	}
	if (anorm > ssfmax) {
		iscale = 1;
		i__1 = lend - l + 1;
		NUMlapack_dlascl ("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &d__[l], n, info);
		i__1 = lend - l;
		NUMlapack_dlascl ("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &e[l], n, info);
	} else if (anorm < ssfmin) {
		iscale = 2;
		i__1 = lend - l + 1;
		NUMlapack_dlascl ("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &d__[l], n, info);
		i__1 = lend - l;
		NUMlapack_dlascl ("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &e[l], n, info);
	}

	/* Choose between QL and QR iteration */

	if ( (d__1 = d__[lend], fabs (d__1)) < (d__2 = d__[l], fabs (d__2))) {
		lend = lsv;
		l = lendsv;
	}

	if (lend > l) {

		/* QL Iteration

		   Look for small subdiagonal element. */

L40:
		if (l != lend) {
			lendm1 = lend - 1;
			i__1 = lendm1;
			for (m = l; m <= i__1; ++m) {
				/* Computing 2nd power */
				d__2 = (d__1 = e[m], fabs (d__1));
				tst = d__2 * d__2;
				if (tst <= eps2 * (d__1 = d__[m], fabs (d__1)) * (d__2 = d__[m + 1], fabs (d__2)) + safmin) {
					goto L60;
				}
				/* L50: */
			}
		}

		m = lend;

L60:
		if (m < lend) {
			e[m] = 0.;
		}
		p = d__[l];
		if (m == l) {
			goto L80;
		}

		/* If remaining matrix is 2-by-2, use DLAE2 or SLAEV2 to compute its
		   eigensystem. */

		if (m == l + 1) {
			if (icompz > 0) {
				NUMlapack_dlaev2 (&d__[l], &e[l], &d__[l + 1], &rt1, &rt2, &c__, &s);
				work[l] = c__;
				work[*n - 1 + l] = s;
				NUMlapack_dlasr ("R", "V", "B", n, &c__2, &work[l], &work[*n - 1 + l], &z___ref (1, l), ldz);
			} else {
				NUMlapack_dlae2 (&d__[l], &e[l], &d__[l + 1], &rt1, &rt2);
			}
			d__[l] = rt1;
			d__[l + 1] = rt2;
			e[l] = 0.;
			l += 2;
			if (l <= lend) {
				goto L40;
			}
			goto L140;
		}

		if (jtot == nmaxit) {
			goto L140;
		}
		++jtot;

		/* Form shift. */

		g = (d__[l + 1] - p) / (e[l] * 2.);
		r__ = NUMlapack_dlapy2 (&g, &c_b10);
		g = d__[m] - p + e[l] / (g + d_sign (&r__, &g));

		s = 1.;
		c__ = 1.;
		p = 0.;

		/* Inner loop */

		mm1 = m - 1;
		i__1 = l;
		for (i__ = mm1; i__ >= i__1; --i__) {
			f = s * e[i__];
			b = c__ * e[i__];
			NUMlapack_dlartg (&g, &f, &c__, &s, &r__);
			if (i__ != m - 1) {
				e[i__ + 1] = r__;
			}
			g = d__[i__ + 1] - p;
			r__ = (d__[i__] - g) * s + c__ * 2. * b;
			p = s * r__;
			d__[i__ + 1] = g + p;
			g = c__ * r__ - b;

			/* If eigenvectors are desired, then save rotations. */

			if (icompz > 0) {
				work[i__] = c__;
				work[*n - 1 + i__] = -s;
			}

			/* L70: */
		}

		/* If eigenvectors are desired, then apply saved rotations. */

		if (icompz > 0) {
			mm = m - l + 1;
			NUMlapack_dlasr ("R", "V", "B", n, &mm, &work[l], &work[*n - 1 + l], &z___ref (1, l), ldz);
		}

		d__[l] -= p;
		e[l] = g;
		goto L40;

		/* Eigenvalue found. */

L80:
		d__[l] = p;

		++l;
		if (l <= lend) {
			goto L40;
		}
		goto L140;

	} else {

		/* QR Iteration

		   Look for small superdiagonal element. */

L90:
		if (l != lend) {
			lendp1 = lend + 1;
			i__1 = lendp1;
			for (m = l; m >= i__1; --m) {
				/* Computing 2nd power */
				d__2 = (d__1 = e[m - 1], fabs (d__1));
				tst = d__2 * d__2;
				if (tst <= eps2 * (d__1 = d__[m], fabs (d__1)) * (d__2 = d__[m - 1], fabs (d__2)) + safmin) {
					goto L110;
				}
				/* L100: */
			}
		}

		m = lend;

L110:
		if (m > lend) {
			e[m - 1] = 0.;
		}
		p = d__[l];
		if (m == l) {
			goto L130;
		}

		/* If remaining matrix is 2-by-2, use DLAE2 or SLAEV2 to compute its
		   eigensystem. */

		if (m == l - 1) {
			if (icompz > 0) {
				NUMlapack_dlaev2 (&d__[l - 1], &e[l - 1], &d__[l], &rt1, &rt2, &c__, &s);
				work[m] = c__;
				work[*n - 1 + m] = s;
				NUMlapack_dlasr ("R", "V", "F", n, &c__2, &work[m], &work[*n - 1 + m], &z___ref (1, l - 1),
				                 ldz);
			} else {
				NUMlapack_dlae2 (&d__[l - 1], &e[l - 1], &d__[l], &rt1, &rt2);
			}
			d__[l - 1] = rt1;
			d__[l] = rt2;
			e[l - 1] = 0.;
			l += -2;
			if (l >= lend) {
				goto L90;
			}
			goto L140;
		}

		if (jtot == nmaxit) {
			goto L140;
		}
		++jtot;

		/* Form shift. */

		g = (d__[l - 1] - p) / (e[l - 1] * 2.);
		r__ = NUMlapack_dlapy2 (&g, &c_b10);
		g = d__[m] - p + e[l - 1] / (g + d_sign (&r__, &g));

		s = 1.;
		c__ = 1.;
		p = 0.;

		/* Inner loop */

		lm1 = l - 1;
		i__1 = lm1;
		for (i__ = m; i__ <= i__1; ++i__) {
			f = s * e[i__];
			b = c__ * e[i__];
			NUMlapack_dlartg (&g, &f, &c__, &s, &r__);
			if (i__ != m) {
				e[i__ - 1] = r__;
			}
			g = d__[i__] - p;
			r__ = (d__[i__ + 1] - g) * s + c__ * 2. * b;
			p = s * r__;
			d__[i__] = g + p;
			g = c__ * r__ - b;

			/* If eigenvectors are desired, then save rotations. */

			if (icompz > 0) {
				work[i__] = c__;
				work[*n - 1 + i__] = s;
			}

			/* L120: */
		}

		/* If eigenvectors are desired, then apply saved rotations. */

		if (icompz > 0) {
			mm = l - m + 1;
			NUMlapack_dlasr ("R", "V", "F", n, &mm, &work[m], &work[*n - 1 + m], &z___ref (1, m), ldz);
		}

		d__[l] -= p;
		e[lm1] = g;
		goto L90;

		/* Eigenvalue found. */

L130:
		d__[l] = p;

		--l;
		if (l >= lend) {
			goto L90;
		}
		goto L140;

	}

	/* Undo scaling if necessary */

L140:
	if (iscale == 1) {
		i__1 = lendsv - lsv + 1;
		NUMlapack_dlascl ("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &d__[lsv], n, info);
		i__1 = lendsv - lsv;
		NUMlapack_dlascl ("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &e[lsv], n, info);
	} else if (iscale == 2) {
		i__1 = lendsv - lsv + 1;
		NUMlapack_dlascl ("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &d__[lsv], n, info);
		i__1 = lendsv - lsv;
		NUMlapack_dlascl ("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &e[lsv], n, info);
	}

	/* Check for no convergence to an eigenvalue after a total of N*MAXIT
	   iterations. */

	if (jtot < nmaxit) {
		goto L10;
	}
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
		if (e[i__] != 0.) {
			++ (*info);
		}
		/* L150: */
	}
	goto L190;

	/* Order eigenvalues and eigenvectors. */

L160:
	if (icompz == 0) {

		/* Use Quick Sort */

		NUMlapack_dlasrt ("I", n, &d__[1], info);

	} else {

		/* Use Selection Sort to minimize swaps of eigenvectors */

		i__1 = *n;
		for (ii = 2; ii <= i__1; ++ii) {
			i__ = ii - 1;
			k = i__;
			p = d__[i__];
			i__2 = *n;
			for (j = ii; j <= i__2; ++j) {
				if (d__[j] < p) {
					k = j;
					p = d__[j];
				}
				/* L170: */
			}
			if (k != i__) {
				d__[k] = d__[i__];
				d__[i__] = p;
				NUMblas_dswap (n, &z___ref (1, i__), &c__1, &z___ref (1, k), &c__1);
			}
			/* L180: */
		}
	}

L190:
	return 0;
}								/* NUMlapack_dsteqr */

#undef z___ref

int NUMlapack_dsterf (long *n, double *d__, double *e, long *info) {
	/* Table of constant values */
	static long c__0 = 0;
	static long c__1 = 1;
	static double c_b32 = 1.;

	/* System generated locals */
	long i__1;
	double d__1, d__2, d__3;

	/* Local variables */
	static double oldc;
	static long lend, jtot;
	static double c__;
	static long i__, l, m;
	static double p, gamma, r__, s, alpha, sigma, anorm;
	static long l1;
	static double bb;
	static long iscale;
	static double oldgam, safmin;
	static double safmax;
	static long lendsv;
	static double ssfmin;
	static long nmaxit;
	static double ssfmax, rt1, rt2, eps, rte;
	static long lsv;
	static double eps2;

	--e;
	--d__;

	/* Function Body */
	*info = 0;

	/* Quick return if possible */

	if (*n < 0) {
		*info = -1;
		i__1 = - (*info);
		xerbla_ ("DSTERF", &i__1);
		return 0;
	}
	if (*n <= 1) {
		return 0;
	}

	/* Determine the unit roundoff for this environment. */

	eps = NUMblas_dlamch ("E");
	/* Computing 2nd power */
	d__1 = eps;
	eps2 = d__1 * d__1;
	safmin = NUMblas_dlamch ("S");
	safmax = 1. / safmin;
	ssfmax = sqrt (safmax) / 3.;
	ssfmin = sqrt (safmin) / eps2;

	/* Compute the eigenvalues of the tridiagonal matrix. */

	nmaxit = *n * 30;
	sigma = 0.;
	jtot = 0;

	/* Determine where the matrix splits and choose QL or QR iteration for
	   each block, according to whether top or bottom diagonal element is
	   smaller. */

	l1 = 1;

L10:
	if (l1 > *n) {
		goto L170;
	}
	if (l1 > 1) {
		e[l1 - 1] = 0.;
	}
	i__1 = *n - 1;
	for (m = l1; m <= i__1; ++m) {
		if ( (d__3 = e[m], fabs (d__3)) <= sqrt ( (d__1 = d__[m], fabs (d__1))) * sqrt ( (d__2 =
		            d__[m + 1], fabs (d__2))) * eps) {
			e[m] = 0.;
			goto L30;
		}
		/* L20: */
	}
	m = *n;

L30:
	l = l1;
	lsv = l;
	lend = m;
	lendsv = lend;
	l1 = m + 1;
	if (lend == l) {
		goto L10;
	}

	/* Scale submatrix in rows and columns L to LEND */

	i__1 = lend - l + 1;
	anorm = NUMlapack_dlanst ("I", &i__1, &d__[l], &e[l]);
	iscale = 0;
	if (anorm > ssfmax) {
		iscale = 1;
		i__1 = lend - l + 1;
		NUMlapack_dlascl ("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &d__[l], n, info);
		i__1 = lend - l;
		NUMlapack_dlascl ("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &e[l], n, info);
	} else if (anorm < ssfmin) {
		iscale = 2;
		i__1 = lend - l + 1;
		NUMlapack_dlascl ("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &d__[l], n, info);
		i__1 = lend - l;
		NUMlapack_dlascl ("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &e[l], n, info);
	}

	i__1 = lend - 1;
	for (i__ = l; i__ <= i__1; ++i__) {
		/* Computing 2nd power */
		d__1 = e[i__];
		e[i__] = d__1 * d__1;
		/* L40: */
	}

	/* Choose between QL and QR iteration */

	if ( (d__1 = d__[lend], fabs (d__1)) < (d__2 = d__[l], fabs (d__2))) {
		lend = lsv;
		l = lendsv;
	}

	if (lend >= l) {

		/* QL Iteration

		   Look for small subdiagonal element. */

L50:
		if (l != lend) {
			i__1 = lend - 1;
			for (m = l; m <= i__1; ++m) {
				if ( (d__2 = e[m], fabs (d__2)) <= eps2 * (d__1 = d__[m] * d__[m + 1], fabs (d__1))) {
					goto L70;
				}
				/* L60: */
			}
		}
		m = lend;

L70:
		if (m < lend) {
			e[m] = 0.;
		}
		p = d__[l];
		if (m == l) {
			goto L90;
		}

		/* If remaining matrix is 2 by 2, use DLAE2 to compute its
		   eigenvalues. */

		if (m == l + 1) {
			rte = sqrt (e[l]);
			NUMlapack_dlae2 (&d__[l], &rte, &d__[l + 1], &rt1, &rt2);
			d__[l] = rt1;
			d__[l + 1] = rt2;
			e[l] = 0.;
			l += 2;
			if (l <= lend) {
				goto L50;
			}
			goto L150;
		}

		if (jtot == nmaxit) {
			goto L150;
		}
		++jtot;

		/* Form shift. */

		rte = sqrt (e[l]);
		sigma = (d__[l + 1] - p) / (rte * 2.);
		r__ = NUMlapack_dlapy2 (&sigma, &c_b32);
		sigma = p - rte / (sigma + d_sign (&r__, &sigma));

		c__ = 1.;
		s = 0.;
		gamma = d__[m] - sigma;
		p = gamma * gamma;

		/* Inner loop */

		i__1 = l;
		for (i__ = m - 1; i__ >= i__1; --i__) {
			bb = e[i__];
			r__ = p + bb;
			if (i__ != m - 1) {
				e[i__ + 1] = s * r__;
			}
			oldc = c__;
			c__ = p / r__;
			s = bb / r__;
			oldgam = gamma;
			alpha = d__[i__];
			gamma = c__ * (alpha - sigma) - s * oldgam;
			d__[i__ + 1] = oldgam + (alpha - gamma);
			if (c__ != 0.) {
				p = gamma * gamma / c__;
			} else {
				p = oldc * bb;
			}
			/* L80: */
		}

		e[l] = s * p;
		d__[l] = sigma + gamma;
		goto L50;

		/* Eigenvalue found. */

L90:
		d__[l] = p;

		++l;
		if (l <= lend) {
			goto L50;
		}
		goto L150;

	} else {

		/* QR Iteration

		   Look for small superdiagonal element. */

L100:
		i__1 = lend + 1;
		for (m = l; m >= i__1; --m) {
			if ( (d__2 = e[m - 1], fabs (d__2)) <= eps2 * (d__1 = d__[m] * d__[m - 1], fabs (d__1))) {
				goto L120;
			}
			/* L110: */
		}
		m = lend;

L120:
		if (m > lend) {
			e[m - 1] = 0.;
		}
		p = d__[l];
		if (m == l) {
			goto L140;
		}

		/* If remaining matrix is 2 by 2, use DLAE2 to compute its
		   eigenvalues. */

		if (m == l - 1) {
			rte = sqrt (e[l - 1]);
			NUMlapack_dlae2 (&d__[l], &rte, &d__[l - 1], &rt1, &rt2);
			d__[l] = rt1;
			d__[l - 1] = rt2;
			e[l - 1] = 0.;
			l += -2;
			if (l >= lend) {
				goto L100;
			}
			goto L150;
		}

		if (jtot == nmaxit) {
			goto L150;
		}
		++jtot;

		/* Form shift. */

		rte = sqrt (e[l - 1]);
		sigma = (d__[l - 1] - p) / (rte * 2.);
		r__ = NUMlapack_dlapy2 (&sigma, &c_b32);
		sigma = p - rte / (sigma + d_sign (&r__, &sigma));

		c__ = 1.;
		s = 0.;
		gamma = d__[m] - sigma;
		p = gamma * gamma;

		/* Inner loop */

		i__1 = l - 1;
		for (i__ = m; i__ <= i__1; ++i__) {
			bb = e[i__];
			r__ = p + bb;
			if (i__ != m) {
				e[i__ - 1] = s * r__;
			}
			oldc = c__;
			c__ = p / r__;
			s = bb / r__;
			oldgam = gamma;
			alpha = d__[i__ + 1];
			gamma = c__ * (alpha - sigma) - s * oldgam;
			d__[i__] = oldgam + (alpha - gamma);
			if (c__ != 0.) {
				p = gamma * gamma / c__;
			} else {
				p = oldc * bb;
			}
			/* L130: */
		}

		e[l - 1] = s * p;
		d__[l] = sigma + gamma;
		goto L100;

		/* Eigenvalue found. */

L140:
		d__[l] = p;

		--l;
		if (l >= lend) {
			goto L100;
		}
		goto L150;

	}

	/* Undo scaling if necessary */

L150:
	if (iscale == 1) {
		i__1 = lendsv - lsv + 1;
		NUMlapack_dlascl ("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &d__[lsv], n, info);
	}
	if (iscale == 2) {
		i__1 = lendsv - lsv + 1;
		NUMlapack_dlascl ("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &d__[lsv], n, info);
	}

	/* Check for no convergence to an eigenvalue after a total of N*MAXIT
	   iterations. */

	if (jtot < nmaxit) {
		goto L10;
	}
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
		if (e[i__] != 0.) {
			++ (*info);
		}
		/* L160: */
	}
	goto L180;

	/* Sort eigenvalues in increasing order. */

L170:
	NUMlapack_dlasrt ("I", n, &d__[1], info);

L180:
	return 0;
}								/* NUMlapack_dsterf */

int NUMlapack_dsyev (const char *jobz, const char *uplo, long *n, double *a, long *lda, double *w, double *work,
                     long *lwork, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__0 = 0;
	static double c_b17 = 1.;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2;
	double d__1;

	/* Local variables */
	static long inde;
	static double anrm;
	static long imax;
	static double rmin, rmax;
	static long lopt;
	static double sigma;
	static long iinfo;
	static long lower, wantz;
	static long nb;
	static long iscale;
	static double safmin;
	static double bignum;
	static long indtau;
	static long indwrk;
	static long llwork;
	static double smlnum;
	static long lwkopt;
	static long lquery;
	static double eps;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--w;
	--work;

	/* Function Body */
	wantz = lsame_ (jobz, "V");
	lower = lsame_ (uplo, "L");
	lquery = *lwork == -1;

	*info = 0;
	if (! (wantz || lsame_ (jobz, "N"))) {
		*info = -1;
	} else if (! (lower || lsame_ (uplo, "U"))) {
		*info = -2;
	} else if (*n < 0) {
		*info = -3;
	} else if (*lda < MAX (1, *n)) {
		*info = -5;
	} else {					/* if(complicated condition) */
		/* Computing MAX */
		i__1 = 1, i__2 = *n * 3 - 1;
		if (*lwork < MAX (i__1, i__2) && !lquery) {
			*info = -8;
		}
	}

	if (*info == 0) {
		nb = NUMlapack_ilaenv (&c__1, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1, 6, 1);
		/* Computing MAX */
		i__1 = 1, i__2 = (nb + 2) * *n;
		lwkopt = MAX (i__1, i__2);
		work[1] = (double) lwkopt;
	}

	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DSYEV ", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*n == 0) {
		work[1] = 1.;
		return 0;
	}

	if (*n == 1) {
		w[1] = a_ref (1, 1);
		work[1] = 3.;
		if (wantz) {
			a_ref (1, 1) = 1.;
		}
		return 0;
	}

	/* Get machine constants. */

	safmin = NUMblas_dlamch ("Safe minimum");
	eps = NUMblas_dlamch ("Precision");
	smlnum = safmin / eps;
	bignum = 1. / smlnum;
	rmin = sqrt (smlnum);
	rmax = sqrt (bignum);

	/* Scale matrix to allowable range, if necessary. */

	anrm = NUMlapack_dlansy ("M", uplo, n, &a[a_offset], lda, &work[1]);
	iscale = 0;
	if (anrm > 0. && anrm < rmin) {
		iscale = 1;
		sigma = rmin / anrm;
	} else if (anrm > rmax) {
		iscale = 1;
		sigma = rmax / anrm;
	}
	if (iscale == 1) {
		NUMlapack_dlascl (uplo, &c__0, &c__0, &c_b17, &sigma, n, n, &a[a_offset], lda, info);
	}

	/* Call DSYTRD to reduce symmetric matrix to tridiagonal form. */

	inde = 1;
	indtau = inde + *n;
	indwrk = indtau + *n;
	llwork = *lwork - indwrk + 1;
	NUMlapack_dsytrd (uplo, n, &a[a_offset], lda, &w[1], &work[inde], &work[indtau], &work[indwrk], &llwork,
	                  &iinfo);
	lopt = (long) ( (*n << 1) + work[indwrk]);

	/* For eigenvalues only, call DSTERF.  For eigenvectors, first call
	   DORGTR to generate the orthogonal matrix, then call DSTEQR. */

	if (!wantz) {
		NUMlapack_dsterf (n, &w[1], &work[inde], info);
	} else {
		NUMlapack_dorgtr (uplo, n, &a[a_offset], lda, &work[indtau], &work[indwrk], &llwork, &iinfo);
		NUMlapack_dsteqr (jobz, n, &w[1], &work[inde], &a[a_offset], lda, &work[indtau], info);
	}

	/* If matrix was scaled, then rescale eigenvalues appropriately. */

	if (iscale == 1) {
		if (*info == 0) {
			imax = *n;
		} else {
			imax = *info - 1;
		}
		d__1 = 1. / sigma;
		NUMblas_dscal (&imax, &d__1, &w[1], &c__1);
	}

	/* Set WORK(1) to optimal workspace size. */

	work[1] = (double) lwkopt;

	return 0;
}								/* NUMlapack_dsyev */

int NUMlapack_dsytd2 (const char *uplo, long *n, double *a, long *lda, double *d__, double *e, double *tau,
                      long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static double c_b8 = 0.;
	static double c_b14 = -1.;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;

	/* Local variables */
	static double taui;
	static long i__;
	static double alpha;
	static long upper;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--d__;
	--e;
	--tau;

	/* Function Body */
	*info = 0;
	upper = lsame_ (uplo, "U");
	if (!upper && !lsame_ (uplo, "L")) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *n)) {
		*info = -4;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DSYTD2", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*n <= 0) {
		return 0;
	}

	if (upper) {

		/* Reduce the upper triangle of A */

		for (i__ = *n - 1; i__ >= 1; --i__) {

			/* Generate elementary reflector H(i) = I - tau * v * v' to
			   annihilate A(1:i-1,i+1) */

			NUMlapack_dlarfg (&i__, &a_ref (i__, i__ + 1), &a_ref (1, i__ + 1), &c__1, &taui);
			e[i__] = a_ref (i__, i__ + 1);

			if (taui != 0.) {

				/* Apply H(i) from both sides to A(1:i,1:i) */

				a_ref (i__, i__ + 1) = 1.;

				/* Compute x := tau * A * v storing x in TAU(1:i) */

				NUMblas_dsymv (uplo, &i__, &taui, &a[a_offset], lda, &a_ref (1, i__ + 1), &c__1, &c_b8, &tau[1],
				               &c__1);

				/* Compute w := x - 1/2 * tau * (x'*v) * v */

				alpha = taui * -.5 * NUMblas_ddot (&i__, &tau[1], &c__1, &a_ref (1, i__ + 1), &c__1);
				NUMblas_daxpy (&i__, &alpha, &a_ref (1, i__ + 1), &c__1, &tau[1], &c__1);

				/* Apply the transformation as a rank-2 update: A := A - v *
				   w' - w * v' */

				NUMblas_dsyr2 (uplo, &i__, &c_b14, &a_ref (1, i__ + 1), &c__1, &tau[1], &c__1, &a[a_offset], lda);

				a_ref (i__, i__ + 1) = e[i__];
			}
			d__[i__ + 1] = a_ref (i__ + 1, i__ + 1);
			tau[i__] = taui;
			/* L10: */
		}
		d__[1] = a_ref (1, 1);
	} else {

		/* Reduce the lower triangle of A */

		i__1 = *n - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {

			/* Generate elementary reflector H(i) = I - tau * v * v' to
			   annihilate A(i+2:n,i)

			   Computing MIN */
			i__2 = i__ + 2;
			i__3 = *n - i__;
			NUMlapack_dlarfg (&i__3, &a_ref (i__ + 1, i__), &a_ref (MIN (i__2, *n), i__), &c__1, &taui);
			e[i__] = a_ref (i__ + 1, i__);

			if (taui != 0.) {

				/* Apply H(i) from both sides to A(i+1:n,i+1:n) */

				a_ref (i__ + 1, i__) = 1.;

				/* Compute x := tau * A * v storing y in TAU(i:n-1) */

				i__2 = *n - i__;
				NUMblas_dsymv (uplo, &i__2, &taui, &a_ref (i__ + 1, i__ + 1), lda, &a_ref (i__ + 1, i__), &c__1,
				               &c_b8, &tau[i__], &c__1);

				/* Compute w := x - 1/2 * tau * (x'*v) * v */

				i__2 = *n - i__;
				alpha = taui * -.5 * NUMblas_ddot (&i__2, &tau[i__], &c__1, &a_ref (i__ + 1, i__), &c__1);
				i__2 = *n - i__;
				NUMblas_daxpy (&i__2, &alpha, &a_ref (i__ + 1, i__), &c__1, &tau[i__], &c__1);

				/* Apply the transformation as a rank-2 update: A := A - v *
				   w' - w * v' */

				i__2 = *n - i__;
				NUMblas_dsyr2 (uplo, &i__2, &c_b14, &a_ref (i__ + 1, i__), &c__1, &tau[i__], &c__1, &a_ref (i__ + 1,
				               i__ + 1), lda);

				a_ref (i__ + 1, i__) = e[i__];
			}
			d__[i__] = a_ref (i__, i__);
			tau[i__] = taui;
			/* L20: */
		}
		d__[*n] = a_ref (*n, *n);
	}

	return 0;
}								/* NUMlapack_dsytd2 */

int NUMlapack_dsytrd (const char *uplo, long *n, double *a, long *lda, double *d__, double *e, double *tau,
                      double *work, long *lwork, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__3 = 3;
	static long c__2 = 2;
	static double c_b22 = -1.;
	static double c_b23 = 1.;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2, i__3;

	/* Local variables */
	static long i__, j;
	static long nbmin, iinfo;
	static long upper;
	static long nb, kk, nx;
	static long ldwork, lwkopt;
	static long lquery;
	static long iws;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--d__;
	--e;
	--tau;
	--work;

	/* Function Body */
	*info = 0;
	upper = lsame_ (uplo, "U");
	lquery = *lwork == -1;
	if (!upper && !lsame_ (uplo, "L")) {
		*info = -1;
	} else if (*n < 0) {
		*info = -2;
	} else if (*lda < MAX (1, *n)) {
		*info = -4;
	} else if (*lwork < 1 && !lquery) {
		*info = -9;
	}

	if (*info == 0) {

		/* Determine the block size. */

		nb = NUMlapack_ilaenv (&c__1, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1, 6, 1);
		lwkopt = *n * nb;
		work[1] = (double) lwkopt;
	}

	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DSYTRD", &i__1);
		return 0;
	} else if (lquery) {
		return 0;
	}

	/* Quick return if possible */

	if (*n == 0) {
		work[1] = 1.;
		return 0;
	}

	nx = *n;
	iws = 1;
	if (nb > 1 && nb < *n) {

		/* Determine when to cross over from blocked to unblocked code (last
		   block is always handled by unblocked code).

		   Computing MAX */
		i__1 = nb, i__2 = NUMlapack_ilaenv (&c__3, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1, 6, 1);
		nx = MAX (i__1, i__2);
		if (nx < *n) {

			/* Determine if workspace is large enough for blocked code. */

			ldwork = *n;
			iws = ldwork * nb;
			if (*lwork < iws) {

				/* Not enough workspace to use optimal NB: determine the
				   minimum value of NB, and reduce NB or force use of
				   unblocked code by setting NX = N.

				   Computing MAX */
				i__1 = *lwork / ldwork;
				nb = MAX (i__1, 1);
				nbmin = NUMlapack_ilaenv (&c__2, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1, 6, 1);
				if (nb < nbmin) {
					nx = *n;
				}
			}
		} else {
			nx = *n;
		}
	} else {
		nb = 1;
	}

	if (upper) {

		/* Reduce the upper triangle of A. Columns 1:kk are handled by the
		   unblocked method. */

		kk = *n - (*n - nx + nb - 1) / nb * nb;
		i__1 = kk + 1;
		i__2 = -nb;
		for (i__ = *n - nb + 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {

			/* Reduce columns i:i+nb-1 to tridiagonal form and form the
			   matrix W which is needed to update the unreduced part of the
			   matrix */

			i__3 = i__ + nb - 1;
			NUMlapack_dlatrd (uplo, &i__3, &nb, &a[a_offset], lda, &e[1], &tau[1], &work[1], &ldwork);

			/* Update the unreduced submatrix A(1:i-1,1:i-1), using an update
			   of the form: A := A - V*W' - W*V' */

			i__3 = i__ - 1;
			NUMblas_dsyr2k (uplo, "No transpose", &i__3, &nb, &c_b22, &a_ref (1, i__), lda, &work[1], &ldwork,
			                &c_b23, &a[a_offset], lda);

			/* Copy superdiagonal elements back into A, and diagonal elements
			   into D */

			i__3 = i__ + nb - 1;
			for (j = i__; j <= i__3; ++j) {
				a_ref (j - 1, j) = e[j - 1];
				d__[j] = a_ref (j, j);
				/* L10: */
			}
			/* L20: */
		}

		/* Use unblocked code to reduce the last or only block */

		NUMlapack_dsytd2 (uplo, &kk, &a[a_offset], lda, &d__[1], &e[1], &tau[1], &iinfo);
	} else {

		/* Reduce the lower triangle of A */

		i__2 = *n - nx;
		i__1 = nb;
		for (i__ = 1; i__1 < 0 ? i__ >= i__2 : i__ <= i__2; i__ += i__1) {

			/* Reduce columns i:i+nb-1 to tridiagonal form and form the
			   matrix W which is needed to update the unreduced part of the
			   matrix */

			i__3 = *n - i__ + 1;
			NUMlapack_dlatrd (uplo, &i__3, &nb, &a_ref (i__, i__), lda, &e[i__], &tau[i__], &work[1], &ldwork);

			/* Update the unreduced submatrix A(i+ib:n,i+ib:n), using an
			   update of the form: A := A - V*W' - W*V' */

			i__3 = *n - i__ - nb + 1;
			NUMblas_dsyr2k (uplo, "No transpose", &i__3, &nb, &c_b22, &a_ref (i__ + nb, i__), lda, &work[nb + 1],
			                &ldwork, &c_b23, &a_ref (i__ + nb, i__ + nb), lda);

			/* Copy subdiagonal elements back into A, and diagonal elements
			   into D */

			i__3 = i__ + nb - 1;
			for (j = i__; j <= i__3; ++j) {
				a_ref (j + 1, j) = e[j];
				d__[j] = a_ref (j, j);
				/* L30: */
			}
			/* L40: */
		}

		/* Use unblocked code to reduce the last or only block */

		i__1 = *n - i__ + 1;
		NUMlapack_dsytd2 (uplo, &i__1, &a_ref (i__, i__), lda, &d__[i__], &e[i__], &tau[i__], &iinfo);
	}

	work[1] = (double) lwkopt;
	return 0;
}								/* NUMlapack_dsytrd */

#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]
#define q_ref(a_1,a_2) q[(a_2)*q_dim1 + a_1]
#define u_ref(a_1,a_2) u[(a_2)*u_dim1 + a_1]
#define v_ref(a_1,a_2) v[(a_2)*v_dim1 + a_1]

int NUMlapack_dtgsja (const char *jobu, const char *jobv, const char *jobq, long *m, long *p, long *n, long *k, long *l,
                      double *a, long *lda, double *b, long *ldb, double *tola, double *tolb, double *alpha, double *beta,
                      double *u, long *ldu, double *v, long *ldv, double *q, long *ldq, double *work, long *ncycle, long *info) {
	/* Table of constant values */
	static double c_b13 = 0.;
	static double c_b14 = 1.;
	static long c__1 = 1;
	static double c_b43 = -1.;

	/* System generated locals */
	long a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, u_dim1, u_offset, v_dim1, v_offset, i__1, i__2,
	     i__3, i__4;
	double d__1;

	/* Local variables */
	static long i__, j;
	static double gamma;
	static double a1;
	static long initq;
	static double a2, a3, b1;
	static long initu, initv, wantq, upper;
	static double b2, b3;
	static long wantu, wantv;
	static double error, ssmin;
	static long kcycle;
	static double csq, csu, csv, snq, rwk, snu, snv;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1 * 1;
	b -= b_offset;
	--alpha;
	--beta;
	u_dim1 = *ldu;
	u_offset = 1 + u_dim1 * 1;
	u -= u_offset;
	v_dim1 = *ldv;
	v_offset = 1 + v_dim1 * 1;
	v -= v_offset;
	q_dim1 = *ldq;
	q_offset = 1 + q_dim1 * 1;
	q -= q_offset;
	--work;

	/* Function Body */
	initu = lsame_ (jobu, "I");
	wantu = initu || lsame_ (jobu, "U");

	initv = lsame_ (jobv, "I");
	wantv = initv || lsame_ (jobv, "V");

	initq = lsame_ (jobq, "I");
	wantq = initq || lsame_ (jobq, "Q");

	*info = 0;
	if (! (initu || wantu || lsame_ (jobu, "N"))) {
		*info = -1;
	} else if (! (initv || wantv || lsame_ (jobv, "N"))) {
		*info = -2;
	} else if (! (initq || wantq || lsame_ (jobq, "N"))) {
		*info = -3;
	} else if (*m < 0) {
		*info = -4;
	} else if (*p < 0) {
		*info = -5;
	} else if (*n < 0) {
		*info = -6;
	} else if (*lda < MAX (1, *m)) {
		*info = -10;
	} else if (*ldb < MAX (1, *p)) {
		*info = -12;
	} else if (*ldu < 1 || wantu && *ldu < *m) {
		*info = -18;
	} else if (*ldv < 1 || wantv && *ldv < *p) {
		*info = -20;
	} else if (*ldq < 1 || wantq && *ldq < *n) {
		*info = -22;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DTGSJA", &i__1);
		return 0;
	}

	/* Initialize U, V and Q, if necessary */

	if (initu) {
		NUMlapack_dlaset ("Full", m, m, &c_b13, &c_b14, &u[u_offset], ldu);
	}
	if (initv) {
		NUMlapack_dlaset ("Full", p, p, &c_b13, &c_b14, &v[v_offset], ldv);
	}
	if (initq) {
		NUMlapack_dlaset ("Full", n, n, &c_b13, &c_b14, &q[q_offset], ldq);
	}

	/* Loop until convergence */

	upper = FALSE;
	for (kcycle = 1; kcycle <= 40; ++kcycle) {

		upper = !upper;

		i__1 = *l - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {
			i__2 = *l;
			for (j = i__ + 1; j <= i__2; ++j) {

				a1 = 0.;
				a2 = 0.;
				a3 = 0.;
				if (*k + i__ <= *m) {
					a1 = a_ref (*k + i__, *n - *l + i__);
				}
				if (*k + j <= *m) {
					a3 = a_ref (*k + j, *n - *l + j);
				}

				b1 = b_ref (i__, *n - *l + i__);
				b3 = b_ref (j, *n - *l + j);

				if (upper) {
					if (*k + i__ <= *m) {
						a2 = a_ref (*k + i__, *n - *l + j);
					}
					b2 = b_ref (i__, *n - *l + j);
				} else {
					if (*k + j <= *m) {
						a2 = a_ref (*k + j, *n - *l + i__);
					}
					b2 = b_ref (j, *n - *l + i__);
				}

				NUMlapack_dlags2 (&upper, &a1, &a2, &a3, &b1, &b2, &b3, &csu, &snu, &csv, &snv, &csq, &snq);

				/* Update (K+I)-th and (K+J)-th rows of matrix A: U'*A */

				if (*k + j <= *m) {
					NUMblas_drot (l, &a_ref (*k + j, *n - *l + 1), lda, &a_ref (*k + i__, *n - *l + 1), lda, &csu,
					              &snu);
				}

				/* Update I-th and J-th rows of matrix B: V'*B */

				NUMblas_drot (l, &b_ref (j, *n - *l + 1), ldb, &b_ref (i__, *n - *l + 1), ldb, &csv, &snv);

				/* Update (N-L+I)-th and (N-L+J)-th columns of matrices A and
				   B: A*Q and B*Q

				   Computing MIN */
				i__4 = *k + *l;
				i__3 = MIN (i__4, *m);
				NUMblas_drot (&i__3, &a_ref (1, *n - *l + j), &c__1, &a_ref (1, *n - *l + i__), &c__1, &csq, &snq);

				NUMblas_drot (l, &b_ref (1, *n - *l + j), &c__1, &b_ref (1, *n - *l + i__), &c__1, &csq, &snq);

				if (upper) {
					if (*k + i__ <= *m) {
						a_ref (*k + i__, *n - *l + j) = 0.;
					}
					b_ref (i__, *n - *l + j) = 0.;
				} else {
					if (*k + j <= *m) {
						a_ref (*k + j, *n - *l + i__) = 0.;
					}
					b_ref (j, *n - *l + i__) = 0.;
				}

				/* Update orthogonal matrices U, V, Q, if desired. */

				if (wantu && *k + j <= *m) {
					NUMblas_drot (m, &u_ref (1, *k + j), &c__1, &u_ref (1, *k + i__), &c__1, &csu, &snu);
				}

				if (wantv) {
					NUMblas_drot (p, &v_ref (1, j), &c__1, &v_ref (1, i__), &c__1, &csv, &snv);
				}

				if (wantq) {
					NUMblas_drot (n, &q_ref (1, *n - *l + j), &c__1, &q_ref (1, *n - *l + i__), &c__1, &csq, &snq);
				}

				/* L10: */
			}
			/* L20: */
		}

		if (!upper) {

			/* The matrices A13 and B13 were lower triangular at the start of
			   the cycle, and are now upper triangular.

			   Convergence test: test the parallelism of the corresponding
			   rows of A and B. */

			error = 0.;
			/* Computing MIN */
			i__2 = *l, i__3 = *m - *k;
			i__1 = MIN (i__2, i__3);
			for (i__ = 1; i__ <= i__1; ++i__) {
				i__2 = *l - i__ + 1;
				NUMblas_dcopy (&i__2, &a_ref (*k + i__, *n - *l + i__), lda, &work[1], &c__1);
				i__2 = *l - i__ + 1;
				NUMblas_dcopy (&i__2, &b_ref (i__, *n - *l + i__), ldb, &work[*l + 1], &c__1);
				i__2 = *l - i__ + 1;
				NUMlapack_dlapll (&i__2, &work[1], &c__1, &work[*l + 1], &c__1, &ssmin);
				error = MAX (error, ssmin);
				/* L30: */
			}

			if (fabs (error) <= MIN (*tola, *tolb)) {
				goto L50;
			}
		}

		/* End of cycle loop

		   L40: */
	}

	/* The algorithm has not converged after MAXIT cycles. */

	*info = 1;
	goto L100;

L50:

	/* If ERROR <= MIN(TOLA,TOLB), then the algorithm has converged. Compute
	   the generalized singular value pairs (ALPHA, BETA), and set the
	   triangular matrix R to array A. */

	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
		alpha[i__] = 1.;
		beta[i__] = 0.;
		/* L60: */
	}

	/* Computing MIN */
	i__2 = *l, i__3 = *m - *k;
	i__1 = MIN (i__2, i__3);
	for (i__ = 1; i__ <= i__1; ++i__) {

		a1 = a_ref (*k + i__, *n - *l + i__);
		b1 = b_ref (i__, *n - *l + i__);

		if (a1 != 0.) {
			gamma = b1 / a1;

			/* change sign if necessary */

			if (gamma < 0.) {
				i__2 = *l - i__ + 1;
				NUMblas_dscal (&i__2, &c_b43, &b_ref (i__, *n - *l + i__), ldb);
				if (wantv) {
					NUMblas_dscal (p, &c_b43, &v_ref (1, i__), &c__1);
				}
			}

			d__1 = fabs (gamma);
			NUMlapack_dlartg (&d__1, &c_b14, &beta[*k + i__], &alpha[*k + i__], &rwk);

			if (alpha[*k + i__] >= beta[*k + i__]) {
				i__2 = *l - i__ + 1;
				d__1 = 1. / alpha[*k + i__];
				NUMblas_dscal (&i__2, &d__1, &a_ref (*k + i__, *n - *l + i__), lda);
			} else {
				i__2 = *l - i__ + 1;
				d__1 = 1. / beta[*k + i__];
				NUMblas_dscal (&i__2, &d__1, &b_ref (i__, *n - *l + i__), ldb);
				i__2 = *l - i__ + 1;
				NUMblas_dcopy (&i__2, &b_ref (i__, *n - *l + i__), ldb, &a_ref (*k + i__, *n - *l + i__), lda);
			}

		} else {

			alpha[*k + i__] = 0.;
			beta[*k + i__] = 1.;
			i__2 = *l - i__ + 1;
			NUMblas_dcopy (&i__2, &b_ref (i__, *n - *l + i__), ldb, &a_ref (*k + i__, *n - *l + i__), lda);

		}

		/* L70: */
	}

	/* Post-assignment */

	i__1 = *k + *l;
	for (i__ = *m + 1; i__ <= i__1; ++i__) {
		alpha[i__] = 0.;
		beta[i__] = 1.;
		/* L80: */
	}

	if (*k + *l < *n) {
		i__1 = *n;
		for (i__ = *k + *l + 1; i__ <= i__1; ++i__) {
			alpha[i__] = 0.;
			beta[i__] = 0.;
			/* L90: */
		}
	}

L100:
	*ncycle = kcycle;
	return 0;
}								/* NUMlapack_dtgsja */

#undef v_ref
#undef u_ref
#undef q_ref
#undef b_ref



int NUMlapack_dtrevc (const char *side, const char *howmny, int *select, long *n, double *t, long *ldt, double *vl,
                      long *ldvl, double *vr, long *ldvr, long *mm, long *m, double *work, long *info) {
	/* Table of constant values */
	static int c_false = FALSE;
	static long c__1 = 1;
	static double c_b22 = 1.;
	static double c_b25 = 0.;
	static long c__2 = 2;
	static int c_true = TRUE;

	/* System generated locals */
	long t_dim1, t_offset, vl_dim1, vl_offset, vr_dim1, vr_offset, i__1, i__2, i__3;
	double d__1, d__2, d__3, d__4, d__5, d__6;

	/* Local variables */
	static double beta, emax;
	static int pair;
	static int allv;
	static long ierr;
	static double unfl, ovfl, smin;
	static int over;
	static double vmax;
	static long jnxt, i__, j, k;
	static double scale, x[4] /* was [2][2] */ ;
	static double remax;
	static int leftv, bothv;
	static double vcrit;
	static int somev;
	static long j1, j2, n2;
	static double xnorm;
	static long ii, ki;
	static long ip, is;
	static double wi;
	static double wr;
	static double bignum;
	static int rightv;
	static double smlnum, rec, ulp;

#define t_ref(a_1,a_2) t[(a_2)*t_dim1 + a_1]
#define x_ref(a_1,a_2) x[(a_2)*2 + a_1 - 3]
#define vl_ref(a_1,a_2) vl[(a_2)*vl_dim1 + a_1]
#define vr_ref(a_1,a_2) vr[(a_2)*vr_dim1 + a_1]

	--select;
	t_dim1 = *ldt;
	t_offset = 1 + t_dim1 * 1;
	t -= t_offset;
	vl_dim1 = *ldvl;
	vl_offset = 1 + vl_dim1 * 1;
	vl -= vl_offset;
	vr_dim1 = *ldvr;
	vr_offset = 1 + vr_dim1 * 1;
	vr -= vr_offset;
	--work;

	/* Function Body */
	bothv = lsame_ (side, "B");
	rightv = lsame_ (side, "R") || bothv;
	leftv = lsame_ (side, "L") || bothv;

	allv = lsame_ (howmny, "A");
	over = lsame_ (howmny, "B");
	somev = lsame_ (howmny, "S");

	*info = 0;
	if (!rightv && !leftv) {
		*info = -1;
	} else if (!allv && !over && !somev) {
		*info = -2;
	} else if (*n < 0) {
		*info = -4;
	} else if (*ldt < MAX (1, *n)) {
		*info = -6;
	} else if (*ldvl < 1 || leftv && *ldvl < *n) {
		*info = -8;
	} else if (*ldvr < 1 || rightv && *ldvr < *n) {
		*info = -10;
	} else {

		/* Set M to the number of columns required to store the selected
		   eigenvectors, standardize the array SELECT if necessary, and test
		   MM. */

		if (somev) {
			*m = 0;
			pair = FALSE;
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (pair) {
					pair = FALSE;
					select[j] = FALSE;
				} else {
					if (j < *n) {
						if (t_ref (j + 1, j) == 0.) {
							if (select[j]) {
								++ (*m);
							}
						} else {
							pair = TRUE;
							if (select[j] || select[j + 1]) {
								select[j] = TRUE;
								*m += 2;
							}
						}
					} else {
						if (select[*n]) {
							++ (*m);
						}
					}
				}
				/* L10: */
			}
		} else {
			*m = *n;
		}

		if (*mm < *m) {
			*info = -11;
		}
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("NUMlapack_dtrevc", &i__1);
		return 0;
	}

	/* Quick return if possible. */

	if (*n == 0) {
		return 0;
	}

	/* Set the constants to control overflow. */

	unfl = NUMblas_dlamch ("Safe minimum");
	ovfl = 1. / unfl;
	NUMlapack_dlabad (&unfl, &ovfl);
	ulp = NUMblas_dlamch ("Precision");
	smlnum = unfl * (*n / ulp);
	bignum = (1. - ulp) / smlnum;

	/* Compute 1-norm of each column of strictly upper triangular part of T
	   to control overflow in triangular solver. */

	work[1] = 0.;
	i__1 = *n;
	for (j = 2; j <= i__1; ++j) {
		work[j] = 0.;
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
			work[j] += (d__1 = t_ref (i__, j), fabs (d__1));
			/* L20: */
		}
		/* L30: */
	}

	/* Index IP is used to specify the real or complex eigenvalue: IP = 0,
	   real eigenvalue, 1, first of conjugate complex pair: (wr,wi) -1,
	   second of conjugate complex pair: (wr,wi) */

	n2 = *n << 1;

	if (rightv) {

		/* Compute right eigenvectors. */

		ip = 0;
		is = *m;
		for (ki = *n; ki >= 1; --ki) {

			if (ip == 1) {
				goto L130;
			}
			if (ki == 1) {
				goto L40;
			}
			if (t_ref (ki, ki - 1) == 0.) {
				goto L40;
			}
			ip = -1;

L40:
			if (somev) {
				if (ip == 0) {
					if (!select[ki]) {
						goto L130;
					}
				} else {
					if (!select[ki - 1]) {
						goto L130;
					}
				}
			}

			/* Compute the KI-th eigenvalue (WR,WI). */

			wr = t_ref (ki, ki);
			wi = 0.;
			if (ip != 0) {
				wi = sqrt ( (d__1 = t_ref (ki, ki - 1), fabs (d__1))) * sqrt ( (d__2 =
				            t_ref (ki - 1, ki), fabs (d__2)));
			}
			/* Computing MAX */
			d__1 = ulp * (fabs (wr) + fabs (wi));
			smin = MAX (d__1, smlnum);

			if (ip == 0) {

				/* Real right eigenvector */

				work[ki + *n] = 1.;

				/* Form right-hand side */

				i__1 = ki - 1;
				for (k = 1; k <= i__1; ++k) {
					work[k + *n] = -t_ref (k, ki);
					/* L50: */
				}

				/* Solve the upper quasi-triangular system: (T(1:KI-1,1:KI-1)
				   - WR)*X = SCALE*WORK. */

				jnxt = ki - 1;
				for (j = ki - 1; j >= 1; --j) {
					if (j > jnxt) {
						goto L60;
					}
					j1 = j;
					j2 = j;
					jnxt = j - 1;
					if (j > 1) {
						if (t_ref (j, j - 1) != 0.) {
							j1 = j - 1;
							jnxt = j - 2;
						}
					}

					if (j1 == j2) {

						/* 1-by-1 diagonal block */

						NUMlapack_dlaln2 (&c_false, &c__1, &c__1, &smin, &c_b22, &t_ref (j, j), ldt, &c_b22,
						                  &c_b22, &work[j + *n], n, &wr, &c_b25, x, &c__2, &scale, &xnorm, &ierr);

						/* Scale X(1,1) to avoid overflow when updating the
						   right-hand side. */

						if (xnorm > 1.) {
							if (work[j] > bignum / xnorm) {
								x_ref (1, 1) = x_ref (1, 1) / xnorm;
								scale /= xnorm;
							}
						}

						/* Scale if necessary */

						if (scale != 1.) {
							NUMblas_dscal (&ki, &scale, &work[*n + 1], &c__1);
						}
						work[j + *n] = x_ref (1, 1);

						/* Update right-hand side */

						i__1 = j - 1;
						d__1 = -x_ref (1, 1);
						NUMblas_daxpy (&i__1, &d__1, &t_ref (1, j), &c__1, &work[*n + 1], &c__1);

					} else {

						/* 2-by-2 diagonal block */

						NUMlapack_dlaln2 (&c_false, &c__2, &c__1, &smin, &c_b22, &t_ref (j - 1, j - 1), ldt,
						                  &c_b22, &c_b22, &work[j - 1 + *n], n, &wr, &c_b25, x, &c__2, &scale, &xnorm,
						                  &ierr);

						/* Scale X(1,1) and X(2,1) to avoid overflow when
						   updating the right-hand side. */

						if (xnorm > 1.) {
							/* Computing MAX */
							d__1 = work[j - 1], d__2 = work[j];
							beta = MAX (d__1, d__2);
							if (beta > bignum / xnorm) {
								x_ref (1, 1) = x_ref (1, 1) / xnorm;
								x_ref (2, 1) = x_ref (2, 1) / xnorm;
								scale /= xnorm;
							}
						}

						/* Scale if necessary */

						if (scale != 1.) {
							NUMblas_dscal (&ki, &scale, &work[*n + 1], &c__1);
						}
						work[j - 1 + *n] = x_ref (1, 1);
						work[j + *n] = x_ref (2, 1);

						/* Update right-hand side */

						i__1 = j - 2;
						d__1 = -x_ref (1, 1);
						NUMblas_daxpy (&i__1, &d__1, &t_ref (1, j - 1), &c__1, &work[*n + 1], &c__1);
						i__1 = j - 2;
						d__1 = -x_ref (2, 1);
						NUMblas_daxpy (&i__1, &d__1, &t_ref (1, j), &c__1, &work[*n + 1], &c__1);
					}
L60:
					;
				}

				/* Copy the vector x or Q*x to VR and normalize. */

				if (!over) {
					NUMblas_dcopy (&ki, &work[*n + 1], &c__1, &vr_ref (1, is), &c__1);

					ii = NUMblas_idamax (&ki, &vr_ref (1, is), &c__1);
					remax = 1. / (d__1 = vr_ref (ii, is), fabs (d__1));
					NUMblas_dscal (&ki, &remax, &vr_ref (1, is), &c__1);

					i__1 = *n;
					for (k = ki + 1; k <= i__1; ++k) {
						vr_ref (k, is) = 0.;
						/* L70: */
					}
				} else {
					if (ki > 1) {
						i__1 = ki - 1;
						NUMblas_dgemv ("N", n, &i__1, &c_b22, &vr[vr_offset], ldvr, &work[*n + 1], &c__1,
						               &work[ki + *n], &vr_ref (1, ki), &c__1);
					}

					ii = NUMblas_idamax (n, &vr_ref (1, ki), &c__1);
					remax = 1. / (d__1 = vr_ref (ii, ki), fabs (d__1));
					NUMblas_dscal (n, &remax, &vr_ref (1, ki), &c__1);
				}

			} else {

				/* Complex right eigenvector.

				   Initial solve [ (T(KI-1,KI-1) T(KI-1,KI) ) - (WR + I*
				   WI)]*X = 0. [ (T(KI,KI-1) T(KI,KI) ) ] */

				if ( (d__1 = t_ref (ki - 1, ki), fabs (d__1)) >= (d__2 = t_ref (ki, ki - 1), fabs (d__2))) {
					work[ki - 1 + *n] = 1.;
					work[ki + n2] = wi / t_ref (ki - 1, ki);
				} else {
					work[ki - 1 + *n] = -wi / t_ref (ki, ki - 1);
					work[ki + n2] = 1.;
				}
				work[ki + *n] = 0.;
				work[ki - 1 + n2] = 0.;

				/* Form right-hand side */

				i__1 = ki - 2;
				for (k = 1; k <= i__1; ++k) {
					work[k + *n] = -work[ki - 1 + *n] * t_ref (k, ki - 1);
					work[k + n2] = -work[ki + n2] * t_ref (k, ki);
					/* L80: */
				}

				/* Solve upper quasi-triangular system: (T(1:KI-2,1:KI-2) -
				   (WR+i*WI))*X = SCALE*(WORK+i*WORK2) */

				jnxt = ki - 2;
				for (j = ki - 2; j >= 1; --j) {
					if (j > jnxt) {
						goto L90;
					}
					j1 = j;
					j2 = j;
					jnxt = j - 1;
					if (j > 1) {
						if (t_ref (j, j - 1) != 0.) {
							j1 = j - 1;
							jnxt = j - 2;
						}
					}

					if (j1 == j2) {

						/* 1-by-1 diagonal block */

						NUMlapack_dlaln2 (&c_false, &c__1, &c__2, &smin, &c_b22, &t_ref (j, j), ldt, &c_b22,
						                  &c_b22, &work[j + *n], n, &wr, &wi, x, &c__2, &scale, &xnorm, &ierr);

						/* Scale X(1,1) and X(1,2) to avoid overflow when
						   updating the right-hand side. */

						if (xnorm > 1.) {
							if (work[j] > bignum / xnorm) {
								x_ref (1, 1) = x_ref (1, 1) / xnorm;
								x_ref (1, 2) = x_ref (1, 2) / xnorm;
								scale /= xnorm;
							}
						}

						/* Scale if necessary */

						if (scale != 1.) {
							NUMblas_dscal (&ki, &scale, &work[*n + 1], &c__1);
							NUMblas_dscal (&ki, &scale, &work[n2 + 1], &c__1);
						}
						work[j + *n] = x_ref (1, 1);
						work[j + n2] = x_ref (1, 2);

						/* Update the right-hand side */

						i__1 = j - 1;
						d__1 = -x_ref (1, 1);
						NUMblas_daxpy (&i__1, &d__1, &t_ref (1, j), &c__1, &work[*n + 1], &c__1);
						i__1 = j - 1;
						d__1 = -x_ref (1, 2);
						NUMblas_daxpy (&i__1, &d__1, &t_ref (1, j), &c__1, &work[n2 + 1], &c__1);

					} else {

						/* 2-by-2 diagonal block */

						NUMlapack_dlaln2 (&c_false, &c__2, &c__2, &smin, &c_b22, &t_ref (j - 1, j - 1), ldt,
						                  &c_b22, &c_b22, &work[j - 1 + *n], n, &wr, &wi, x, &c__2, &scale, &xnorm, &ierr);

						/* Scale X to avoid overflow when updating the
						   right-hand side. */

						if (xnorm > 1.) {
							/* Computing MAX */
							d__1 = work[j - 1], d__2 = work[j];
							beta = MAX (d__1, d__2);
							if (beta > bignum / xnorm) {
								rec = 1. / xnorm;
								x_ref (1, 1) = x_ref (1, 1) * rec;
								x_ref (1, 2) = x_ref (1, 2) * rec;
								x_ref (2, 1) = x_ref (2, 1) * rec;
								x_ref (2, 2) = x_ref (2, 2) * rec;
								scale *= rec;
							}
						}

						/* Scale if necessary */

						if (scale != 1.) {
							NUMblas_dscal (&ki, &scale, &work[*n + 1], &c__1);
							NUMblas_dscal (&ki, &scale, &work[n2 + 1], &c__1);
						}
						work[j - 1 + *n] = x_ref (1, 1);
						work[j + *n] = x_ref (2, 1);
						work[j - 1 + n2] = x_ref (1, 2);
						work[j + n2] = x_ref (2, 2);

						/* Update the right-hand side */

						i__1 = j - 2;
						d__1 = -x_ref (1, 1);
						NUMblas_daxpy (&i__1, &d__1, &t_ref (1, j - 1), &c__1, &work[*n + 1], &c__1);
						i__1 = j - 2;
						d__1 = -x_ref (2, 1);
						NUMblas_daxpy (&i__1, &d__1, &t_ref (1, j), &c__1, &work[*n + 1], &c__1);
						i__1 = j - 2;
						d__1 = -x_ref (1, 2);
						NUMblas_daxpy (&i__1, &d__1, &t_ref (1, j - 1), &c__1, &work[n2 + 1], &c__1);
						i__1 = j - 2;
						d__1 = -x_ref (2, 2);
						NUMblas_daxpy (&i__1, &d__1, &t_ref (1, j), &c__1, &work[n2 + 1], &c__1);
					}
L90:
					;
				}

				/* Copy the vector x or Q*x to VR and normalize. */

				if (!over) {
					NUMblas_dcopy (&ki, &work[*n + 1], &c__1, &vr_ref (1, is - 1), &c__1);
					NUMblas_dcopy (&ki, &work[n2 + 1], &c__1, &vr_ref (1, is), &c__1);

					emax = 0.;
					i__1 = ki;
					for (k = 1; k <= i__1; ++k) {
						/* Computing MAX */
						d__3 = emax, d__4 = (d__1 = vr_ref (k, is - 1), fabs (d__1)) + (d__2 =
						                        vr_ref (k, is), fabs (d__2));
						emax = MAX (d__3, d__4);
						/* L100: */
					}

					remax = 1. / emax;
					NUMblas_dscal (&ki, &remax, &vr_ref (1, is - 1), &c__1);
					NUMblas_dscal (&ki, &remax, &vr_ref (1, is), &c__1);

					i__1 = *n;
					for (k = ki + 1; k <= i__1; ++k) {
						vr_ref (k, is - 1) = 0.;
						vr_ref (k, is) = 0.;
						/* L110: */
					}

				} else {

					if (ki > 2) {
						i__1 = ki - 2;
						NUMblas_dgemv ("N", n, &i__1, &c_b22, &vr[vr_offset], ldvr, &work[*n + 1], &c__1,
						               &work[ki - 1 + *n], &vr_ref (1, ki - 1), &c__1);
						i__1 = ki - 2;
						NUMblas_dgemv ("N", n, &i__1, &c_b22, &vr[vr_offset], ldvr, &work[n2 + 1], &c__1,
						               &work[ki + n2], &vr_ref (1, ki), &c__1);
					} else {
						NUMblas_dscal (n, &work[ki - 1 + *n], &vr_ref (1, ki - 1), &c__1);
						NUMblas_dscal (n, &work[ki + n2], &vr_ref (1, ki), &c__1);
					}

					emax = 0.;
					i__1 = *n;
					for (k = 1; k <= i__1; ++k) {
						/* Computing MAX */
						d__3 = emax, d__4 = (d__1 = vr_ref (k, ki - 1), fabs (d__1)) + (d__2 =
						                        vr_ref (k, ki), fabs (d__2));
						emax = MAX (d__3, d__4);
						/* L120: */
					}
					remax = 1. / emax;
					NUMblas_dscal (n, &remax, &vr_ref (1, ki - 1), &c__1);
					NUMblas_dscal (n, &remax, &vr_ref (1, ki), &c__1);
				}
			}

			--is;
			if (ip != 0) {
				--is;
			}
L130:
			if (ip == 1) {
				ip = 0;
			}
			if (ip == -1) {
				ip = 1;
			}
			/* L140: */
		}
	}

	if (leftv) {

		/* Compute left eigenvectors. */

		ip = 0;
		is = 1;
		i__1 = *n;
		for (ki = 1; ki <= i__1; ++ki) {

			if (ip == -1) {
				goto L250;
			}
			if (ki == *n) {
				goto L150;
			}
			if (t_ref (ki + 1, ki) == 0.) {
				goto L150;
			}
			ip = 1;

L150:
			if (somev) {
				if (!select[ki]) {
					goto L250;
				}
			}

			/* Compute the KI-th eigenvalue (WR,WI). */

			wr = t_ref (ki, ki);
			wi = 0.;
			if (ip != 0) {
				wi = sqrt ( (d__1 = t_ref (ki, ki + 1), fabs (d__1))) * sqrt ( (d__2 =
				            t_ref (ki + 1, ki), fabs (d__2)));
			}
			/* Computing MAX */
			d__1 = ulp * (fabs (wr) + fabs (wi));
			smin = MAX (d__1, smlnum);

			if (ip == 0) {

				/* Real left eigenvector. */

				work[ki + *n] = 1.;

				/* Form right-hand side */

				i__2 = *n;
				for (k = ki + 1; k <= i__2; ++k) {
					work[k + *n] = -t_ref (ki, k);
					/* L160: */
				}

				/* Solve the quasi-triangular system: (T(KI+1:N,KI+1:N) -
				   WR)'*X = SCALE*WORK */

				vmax = 1.;
				vcrit = bignum;

				jnxt = ki + 1;
				i__2 = *n;
				for (j = ki + 1; j <= i__2; ++j) {
					if (j < jnxt) {
						goto L170;
					}
					j1 = j;
					j2 = j;
					jnxt = j + 1;
					if (j < *n) {
						if (t_ref (j + 1, j) != 0.) {
							j2 = j + 1;
							jnxt = j + 2;
						}
					}

					if (j1 == j2) {

						/* 1-by-1 diagonal block

						   Scale if necessary to avoid overflow when forming
						   the right-hand side. */

						if (work[j] > vcrit) {
							rec = 1. / vmax;
							i__3 = *n - ki + 1;
							NUMblas_dscal (&i__3, &rec, &work[ki + *n], &c__1);
							vmax = 1.;
							vcrit = bignum;
						}

						i__3 = j - ki - 1;
						work[j + *n] -=
						    NUMblas_ddot (&i__3, &t_ref (ki + 1, j), &c__1, &work[ki + 1 + *n], &c__1);

						/* Solve (T(J,J)-WR)'*X = WORK */

						NUMlapack_dlaln2 (&c_false, &c__1, &c__1, &smin, &c_b22, &t_ref (j, j), ldt, &c_b22,
						                  &c_b22, &work[j + *n], n, &wr, &c_b25, x, &c__2, &scale, &xnorm, &ierr);

						/* Scale if necessary */

						if (scale != 1.) {
							i__3 = *n - ki + 1;
							NUMblas_dscal (&i__3, &scale, &work[ki + *n], &c__1);
						}
						work[j + *n] = x_ref (1, 1);
						/* Computing MAX */
						d__2 = (d__1 = work[j + *n], fabs (d__1));
						vmax = MAX (d__2, vmax);
						vcrit = bignum / vmax;

					} else {

						/* 2-by-2 diagonal block

						   Scale if necessary to avoid overflow when forming
						   the right-hand side.

						   Computing MAX */
						d__1 = work[j], d__2 = work[j + 1];
						beta = MAX (d__1, d__2);
						if (beta > vcrit) {
							rec = 1. / vmax;
							i__3 = *n - ki + 1;
							NUMblas_dscal (&i__3, &rec, &work[ki + *n], &c__1);
							vmax = 1.;
							vcrit = bignum;
						}

						i__3 = j - ki - 1;
						work[j + *n] -=
						    NUMblas_ddot (&i__3, &t_ref (ki + 1, j), &c__1, &work[ki + 1 + *n], &c__1);

						i__3 = j - ki - 1;
						work[j + 1 + *n] -=
						    NUMblas_ddot (&i__3, &t_ref (ki + 1, j + 1), &c__1, &work[ki + 1 + *n], &c__1);

						/* Solve [T(J,J)-WR T(J,J+1) ]'* X = SCALE*( WORK1 )
						   [T(J+1,J) T(J+1,J+1)-WR] ( WORK2 ) */

						NUMlapack_dlaln2 (&c_true, &c__2, &c__1, &smin, &c_b22, &t_ref (j, j), ldt, &c_b22,
						                  &c_b22, &work[j + *n], n, &wr, &c_b25, x, &c__2, &scale, &xnorm, &ierr);

						/* Scale if necessary */

						if (scale != 1.) {
							i__3 = *n - ki + 1;
							NUMblas_dscal (&i__3, &scale, &work[ki + *n], &c__1);
						}
						work[j + *n] = x_ref (1, 1);
						work[j + 1 + *n] = x_ref (2, 1);

						/* Computing MAX */
						d__3 = (d__1 = work[j + *n], fabs (d__1)), d__4 = (d__2 =
						            work[j + 1 + *n], fabs (d__2)), d__3 = MAX (d__3, d__4);
						vmax = MAX (d__3, vmax);
						vcrit = bignum / vmax;

					}
L170:
					;
				}

				/* Copy the vector x or Q*x to VL and normalize. */

				if (!over) {
					i__2 = *n - ki + 1;
					NUMblas_dcopy (&i__2, &work[ki + *n], &c__1, &vl_ref (ki, is), &c__1);

					i__2 = *n - ki + 1;
					ii = NUMblas_idamax (&i__2, &vl_ref (ki, is), &c__1) + ki - 1;
					remax = 1. / (d__1 = vl_ref (ii, is), fabs (d__1));
					i__2 = *n - ki + 1;
					NUMblas_dscal (&i__2, &remax, &vl_ref (ki, is), &c__1);

					i__2 = ki - 1;
					for (k = 1; k <= i__2; ++k) {
						vl_ref (k, is) = 0.;
						/* L180: */
					}

				} else {

					if (ki < *n) {
						i__2 = *n - ki;
						NUMblas_dgemv ("N", n, &i__2, &c_b22, &vl_ref (1, ki + 1), ldvl, &work[ki + 1 + *n],
						               &c__1, &work[ki + *n], &vl_ref (1, ki), &c__1);
					}

					ii = NUMblas_idamax (n, &vl_ref (1, ki), &c__1);
					remax = 1. / (d__1 = vl_ref (ii, ki), fabs (d__1));
					NUMblas_dscal (n, &remax, &vl_ref (1, ki), &c__1);

				}

			} else {

				/* Complex left eigenvector.

				   Initial solve: ((T(KI,KI) T(KI,KI+1) )' - (WR - I* WI))*X
				   = 0. ((T(KI+1,KI) T(KI+1,KI+1)) ) */

				if ( (d__1 = t_ref (ki, ki + 1), fabs (d__1)) >= (d__2 = t_ref (ki + 1, ki), fabs (d__2))) {
					work[ki + *n] = wi / t_ref (ki, ki + 1);
					work[ki + 1 + n2] = 1.;
				} else {
					work[ki + *n] = 1.;
					work[ki + 1 + n2] = -wi / t_ref (ki + 1, ki);
				}
				work[ki + 1 + *n] = 0.;
				work[ki + n2] = 0.;

				/* Form right-hand side */

				i__2 = *n;
				for (k = ki + 2; k <= i__2; ++k) {
					work[k + *n] = -work[ki + *n] * t_ref (ki, k);
					work[k + n2] = -work[ki + 1 + n2] * t_ref (ki + 1, k);
					/* L190: */
				}

				/* Solve complex quasi-triangular system: ( T(KI+2,N:KI+2,N)
				   - (WR-i*WI) )*X = WORK1+i*WORK2 */

				vmax = 1.;
				vcrit = bignum;

				jnxt = ki + 2;
				i__2 = *n;
				for (j = ki + 2; j <= i__2; ++j) {
					if (j < jnxt) {
						goto L200;
					}
					j1 = j;
					j2 = j;
					jnxt = j + 1;
					if (j < *n) {
						if (t_ref (j + 1, j) != 0.) {
							j2 = j + 1;
							jnxt = j + 2;
						}
					}

					if (j1 == j2) {

						/* 1-by-1 diagonal block

						   Scale if necessary to avoid overflow when forming
						   the right-hand side elements. */

						if (work[j] > vcrit) {
							rec = 1. / vmax;
							i__3 = *n - ki + 1;
							NUMblas_dscal (&i__3, &rec, &work[ki + *n], &c__1);
							i__3 = *n - ki + 1;
							NUMblas_dscal (&i__3, &rec, &work[ki + n2], &c__1);
							vmax = 1.;
							vcrit = bignum;
						}

						i__3 = j - ki - 2;
						work[j + *n] -=
						    NUMblas_ddot (&i__3, &t_ref (ki + 2, j), &c__1, &work[ki + 2 + *n], &c__1);
						i__3 = j - ki - 2;
						work[j + n2] -=
						    NUMblas_ddot (&i__3, &t_ref (ki + 2, j), &c__1, &work[ki + 2 + n2], &c__1);

						/* Solve (T(J,J)-(WR-i*WI))*(X11+i*X12)= WK+I*WK2 */

						d__1 = -wi;
						NUMlapack_dlaln2 (&c_false, &c__1, &c__2, &smin, &c_b22, &t_ref (j, j), ldt, &c_b22,
						                  &c_b22, &work[j + *n], n, &wr, &d__1, x, &c__2, &scale, &xnorm, &ierr);

						/* Scale if necessary */

						if (scale != 1.) {
							i__3 = *n - ki + 1;
							NUMblas_dscal (&i__3, &scale, &work[ki + *n], &c__1);
							i__3 = *n - ki + 1;
							NUMblas_dscal (&i__3, &scale, &work[ki + n2], &c__1);
						}
						work[j + *n] = x_ref (1, 1);
						work[j + n2] = x_ref (1, 2);
						/* Computing MAX */
						d__3 = (d__1 = work[j + *n], fabs (d__1)), d__4 = (d__2 =
						            work[j + n2], fabs (d__2)), d__3 = MAX (d__3, d__4);
						vmax = MAX (d__3, vmax);
						vcrit = bignum / vmax;

					} else {

						/* 2-by-2 diagonal block

						   Scale if necessary to avoid overflow when forming
						   the right-hand side elements.

						   Computing MAX */
						d__1 = work[j], d__2 = work[j + 1];
						beta = MAX (d__1, d__2);
						if (beta > vcrit) {
							rec = 1. / vmax;
							i__3 = *n - ki + 1;
							NUMblas_dscal (&i__3, &rec, &work[ki + *n], &c__1);
							i__3 = *n - ki + 1;
							NUMblas_dscal (&i__3, &rec, &work[ki + n2], &c__1);
							vmax = 1.;
							vcrit = bignum;
						}

						i__3 = j - ki - 2;
						work[j + *n] -=
						    NUMblas_ddot (&i__3, &t_ref (ki + 2, j), &c__1, &work[ki + 2 + *n], &c__1);

						i__3 = j - ki - 2;
						work[j + n2] -=
						    NUMblas_ddot (&i__3, &t_ref (ki + 2, j), &c__1, &work[ki + 2 + n2], &c__1);

						i__3 = j - ki - 2;
						work[j + 1 + *n] -=
						    NUMblas_ddot (&i__3, &t_ref (ki + 2, j + 1), &c__1, &work[ki + 2 + *n], &c__1);

						i__3 = j - ki - 2;
						work[j + 1 + n2] -=
						    NUMblas_ddot (&i__3, &t_ref (ki + 2, j + 1), &c__1, &work[ki + 2 + n2], &c__1);

						/* Solve 2-by-2 complex linear equation ([T(j,j)
						   T(j,j+1) ]'-(wr-i*wi)*I)*X = SCALE*B ([T(j+1,j)
						   T(j+1,j+1)] ) */

						d__1 = -wi;
						NUMlapack_dlaln2 (&c_true, &c__2, &c__2, &smin, &c_b22, &t_ref (j, j), ldt, &c_b22,
						                  &c_b22, &work[j + *n], n, &wr, &d__1, x, &c__2, &scale, &xnorm, &ierr);

						/* Scale if necessary */

						if (scale != 1.) {
							i__3 = *n - ki + 1;
							NUMblas_dscal (&i__3, &scale, &work[ki + *n], &c__1);
							i__3 = *n - ki + 1;
							NUMblas_dscal (&i__3, &scale, &work[ki + n2], &c__1);
						}
						work[j + *n] = x_ref (1, 1);
						work[j + n2] = x_ref (1, 2);
						work[j + 1 + *n] = x_ref (2, 1);
						work[j + 1 + n2] = x_ref (2, 2);
						/* Computing MAX */
						d__5 = (d__1 = x_ref (1, 1), fabs (d__1)), d__6 = (d__2 =
						            x_ref (1, 2), fabs (d__2)), d__5 = MAX (d__5, d__6), d__6 = (d__3 =
						                        x_ref (2, 1), fabs (d__3)), d__5 = MAX (d__5, d__6), d__6 = (d__4 =
						                                    x_ref (2, 2), fabs (d__4)), d__5 = MAX (d__5, d__6);
						vmax = MAX (d__5, vmax);
						vcrit = bignum / vmax;

					}
L200:
					;
				}

				/* Copy the vector x or Q*x to VL and normalize.

				   L210: */
				if (!over) {
					i__2 = *n - ki + 1;
					NUMblas_dcopy (&i__2, &work[ki + *n], &c__1, &vl_ref (ki, is), &c__1);
					i__2 = *n - ki + 1;
					NUMblas_dcopy (&i__2, &work[ki + n2], &c__1, &vl_ref (ki, is + 1), &c__1);

					emax = 0.;
					i__2 = *n;
					for (k = ki; k <= i__2; ++k) {
						/* Computing MAX */
						d__3 = emax, d__4 = (d__1 = vl_ref (k, is), fabs (d__1)) + (d__2 =
						                        vl_ref (k, is + 1), fabs (d__2));
						emax = MAX (d__3, d__4);
						/* L220: */
					}
					remax = 1. / emax;
					i__2 = *n - ki + 1;
					NUMblas_dscal (&i__2, &remax, &vl_ref (ki, is), &c__1);
					i__2 = *n - ki + 1;
					NUMblas_dscal (&i__2, &remax, &vl_ref (ki, is + 1), &c__1);

					i__2 = ki - 1;
					for (k = 1; k <= i__2; ++k) {
						vl_ref (k, is) = 0.;
						vl_ref (k, is + 1) = 0.;
						/* L230: */
					}
				} else {
					if (ki < *n - 1) {
						i__2 = *n - ki - 1;
						NUMblas_dgemv ("N", n, &i__2, &c_b22, &vl_ref (1, ki + 2), ldvl, &work[ki + 2 + *n],
						               &c__1, &work[ki + *n], &vl_ref (1, ki), &c__1);
						i__2 = *n - ki - 1;
						NUMblas_dgemv ("N", n, &i__2, &c_b22, &vl_ref (1, ki + 2), ldvl, &work[ki + 2 + n2],
						               &c__1, &work[ki + 1 + n2], &vl_ref (1, ki + 1), &c__1);
					} else {
						NUMblas_dscal (n, &work[ki + *n], &vl_ref (1, ki), &c__1);
						NUMblas_dscal (n, &work[ki + 1 + n2], &vl_ref (1, ki + 1), &c__1);
					}

					emax = 0.;
					i__2 = *n;
					for (k = 1; k <= i__2; ++k) {
						/* Computing MAX */
						d__3 = emax, d__4 = (d__1 = vl_ref (k, ki), fabs (d__1)) + (d__2 =
						                        vl_ref (k, ki + 1), fabs (d__2));
						emax = MAX (d__3, d__4);
						/* L240: */
					}
					remax = 1. / emax;
					NUMblas_dscal (n, &remax, &vl_ref (1, ki), &c__1);
					NUMblas_dscal (n, &remax, &vl_ref (1, ki + 1), &c__1);
				}
			}

			++is;
			if (ip != 0) {
				++is;
			}
L250:
			if (ip == -1) {
				ip = 0;
			}
			if (ip == 1) {
				ip = -1;
			}

			/* L260: */
		}

	}

	return 0;
}								/* NUMlapack_dtrevc */

#undef vr_ref
#undef vl_ref
#undef x_ref
#undef t_ref

int NUMlapack_dtrti2 (const char *uplo, const char *diag, long *n, double *a, long *lda, long *info) {
	/* Table of constant values */
	static long c__1 = 1;

	/* System generated locals */
	long a_dim1, a_offset, i__1, i__2;

	/* Local variables */
	static long j;
	static long upper;
	static long nounit;
	static double ajj;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;

	/* Function Body */
	*info = 0;
	upper = lsame_ (uplo, "U");
	nounit = lsame_ (diag, "N");
	if (!upper && !lsame_ (uplo, "L")) {
		*info = -1;
	} else if (!nounit && !lsame_ (diag, "U")) {
		*info = -2;
	} else if (*n < 0) {
		*info = -3;
	} else if (*lda < MAX (1, *n)) {
		*info = -5;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DTRTI2", &i__1);
		return 0;
	}

	if (upper) {

		/* Compute inverse of upper triangular matrix. */

		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			if (nounit) {
				a_ref (j, j) = 1. / a_ref (j, j);
				ajj = -a_ref (j, j);
			} else {
				ajj = -1.;
			}

			/* Compute elements 1:j-1 of j-th column. */

			i__2 = j - 1;
			NUMblas_dtrmv ("Upper", "No transpose", diag, &i__2, &a[a_offset], lda, &a_ref (1, j), &c__1);
			i__2 = j - 1;
			NUMblas_dscal (&i__2, &ajj, &a_ref (1, j), &c__1);
			/* L10: */
		}
	} else {

		/* Compute inverse of lower triangular matrix. */

		for (j = *n; j >= 1; --j) {
			if (nounit) {
				a_ref (j, j) = 1. / a_ref (j, j);
				ajj = -a_ref (j, j);
			} else {
				ajj = -1.;
			}
			if (j < *n) {

				/* Compute elements j+1:n of j-th column. */

				i__1 = *n - j;
				NUMblas_dtrmv ("Lower", "No transpose", diag, &i__1, &a_ref (j + 1, j + 1), lda, &a_ref (j + 1, j),
				               &c__1);
				i__1 = *n - j;
				NUMblas_dscal (&i__1, &ajj, &a_ref (j + 1, j), &c__1);
			}
			/* L20: */
		}
	}

	return 0;
}								/* NUMlapack_dtrti2 */

int NUMlapack_dtrtri (const char *uplo, const char *diag, long *n, double *a, long *lda, long *info) {
	/* Table of constant values */
	static long c__1 = 1;
	static long c_n1 = -1;
	static long c__2 = 2;
	static double c_b18 = 1.;
	static double c_b22 = -1.;

	/* System generated locals */
	char *a__1[2];
	long a_dim1, a_offset, i__1, i__2[2], i__3, i__4, i__5;
	char ch__1[2];

	/* Local variables */
	static long j;
	static long upper;
	static long jb, nb, nn;
	static long nounit;

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;

	/* Function Body */
	*info = 0;
	upper = lsame_ (uplo, "U");
	nounit = lsame_ (diag, "N");
	if (!upper && !lsame_ (uplo, "L")) {
		*info = -1;
	} else if (!nounit && !lsame_ (diag, "U")) {
		*info = -2;
	} else if (*n < 0) {
		*info = -3;
	} else if (*lda < MAX (1, *n)) {
		*info = -5;
	}
	if (*info != 0) {
		i__1 = - (*info);
		xerbla_ ("DTRTRI", &i__1);
		return 0;
	}

	/* Quick return if possible */

	if (*n == 0) {
		return 0;
	}

	/* Check for singularity if non-unit. */

	if (nounit) {
		i__1 = *n;
		for (*info = 1; *info <= i__1; ++ (*info)) {
			if (a_ref (*info, *info) == 0.) {
				return 0;
			}
			/* L10: */
		}
		*info = 0;
	}

	/* Determine the block size for this environment.

	   Writing concatenation */
	i__2[0] = 1, a__1[0] = (char *) uplo;
	i__2[1] = 1, a__1[1] = (char *) diag;
	s_cat (ch__1, (const char **) a__1, i__2, &c__2, 2);
	nb = NUMlapack_ilaenv (&c__1, "DTRTRI", ch__1, n, &c_n1, &c_n1, &c_n1, 6, 2);
	if (nb <= 1 || nb >= *n) {

		/* Use unblocked code */

		NUMlapack_dtrti2 (uplo, diag, n, &a[a_offset], lda, info);
	} else {

		/* Use blocked code */

		if (upper) {

			/* Compute inverse of upper triangular matrix */

			i__1 = *n;
			i__3 = nb;
			for (j = 1; i__3 < 0 ? j >= i__1 : j <= i__1; j += i__3) {
				/* Computing MIN */
				i__4 = nb, i__5 = *n - j + 1;
				jb = MIN (i__4, i__5);

				/* Compute rows 1:j-1 of current block column */

				i__4 = j - 1;
				NUMblas_dtrmm ("Left", "Upper", "No transpose", diag, &i__4, &jb, &c_b18, &a[a_offset], lda,
				               &a_ref (1, j), lda);
				i__4 = j - 1;
				NUMblas_dtrsm ("Right", "Upper", "No transpose", diag, &i__4, &jb, &c_b22, &a_ref (j, j), lda,
				               &a_ref (1, j), lda);

				/* Compute inverse of current diagonal block */

				NUMlapack_dtrti2 ("Upper", diag, &jb, &a_ref (j, j), lda, info);
				/* L20: */
			}
		} else {

			/* Compute inverse of lower triangular matrix */

			nn = (*n - 1) / nb * nb + 1;
			i__3 = -nb;
			for (j = nn; i__3 < 0 ? j >= 1 : j <= 1; j += i__3) {
				/* Computing MIN */
				i__1 = nb, i__4 = *n - j + 1;
				jb = MIN (i__1, i__4);
				if (j + jb <= *n) {

					/* Compute rows j+jb:n of current block column */

					i__1 = *n - j - jb + 1;
					NUMblas_dtrmm ("Left", "Lower", "No transpose", diag, &i__1, &jb, &c_b18, &a_ref (j + jb,
					               j + jb), lda, &a_ref (j + jb, j), lda);
					i__1 = *n - j - jb + 1;
					NUMblas_dtrsm ("Right", "Lower", "No transpose", diag, &i__1, &jb, &c_b22, &a_ref (j, j), lda,
					               &a_ref (j + jb, j), lda);
				}

				/* Compute inverse of current diagonal block */

				NUMlapack_dtrti2 ("Lower", diag, &jb, &a_ref (j, j), lda, info);
				/* L30: */
			}
		}
	}
	return 0;
}								/* NUMlapack_dtrtri */

long NUMlapack_ieeeck (long *ispec, float *zero, float *one) {
	/* System generated locals */
	long ret_val;

	/* Local variables */
	static float neginf, posinf, negzro, newzro, nan1, nan2, nan3, nan4, nan5, nan6;

	ret_val = 1;

	posinf = *one / *zero;
	if (posinf <= *one) {
		ret_val = 0;
		return ret_val;
	}

	neginf = - (*one) / *zero;
	if (neginf >= *zero) {
		ret_val = 0;
		return ret_val;
	}

	negzro = *one / (neginf + *one);
	if (negzro != *zero) {
		ret_val = 0;
		return ret_val;
	}

	neginf = *one / negzro;
	if (neginf >= *zero) {
		ret_val = 0;
		return ret_val;
	}

	newzro = negzro + *zero;
	if (newzro != *zero) {
		ret_val = 0;
		return ret_val;
	}

	posinf = *one / newzro;
	if (posinf <= *one) {
		ret_val = 0;
		return ret_val;
	}

	neginf *= posinf;
	if (neginf >= *zero) {
		ret_val = 0;
		return ret_val;
	}

	posinf *= posinf;
	if (posinf <= *one) {
		ret_val = 0;
		return ret_val;
	}

	/* Return if we were only asked to check infinity arithmetic */

	if (*ispec == 0) {
		return ret_val;
	}

	nan1 = posinf + neginf;

	nan2 = posinf / neginf;

	nan3 = posinf / posinf;

	nan4 = posinf * *zero;

	nan5 = neginf * negzro;

	nan6 = nan5 * 0.f;

	if (nan1 == nan1) {
		ret_val = 0;
		return ret_val;
	}

	if (nan2 == nan2) {
		ret_val = 0;
		return ret_val;
	}

	if (nan3 == nan3) {
		ret_val = 0;
		return ret_val;
	}

	if (nan4 == nan4) {
		ret_val = 0;
		return ret_val;
	}

	if (nan5 == nan5) {
		ret_val = 0;
		return ret_val;
	}

	if (nan6 == nan6) {
		ret_val = 0;
		return ret_val;
	}

	return ret_val;
}								/* NUMlapack_ieeeck */

long NUMlapack_ilaenv (long *ispec, const char *name__, const char *opts, long *n1, long *n2, long *n3, long *n4,
                       long name_len, long opts_len) {
	/* Table of constant values */
	static long c__0 = 0;
	static float c_b162 = 0.f;
	static float c_b163 = 1.f;
	static long c__1 = 1;

	/* System generated locals */
	long ret_val;

	/* Local variables */
	static long i__;
	static long cname, sname;
	static long nbmin;
	static char c1[1], c2[2], c3[3], c4[2];
	static long ic, nb;
	static long iz, nx;
	static char subnam[6];

	(void) opts;
	(void) n3;
	(void) opts_len;
	switch (*ispec) {
		case 1:
			goto L100;
		case 2:
			goto L100;
		case 3:
			goto L100;
		case 4:
			goto L400;
		case 5:
			goto L500;
		case 6:
			goto L600;
		case 7:
			goto L700;
		case 8:
			goto L800;
		case 9:
			goto L900;
		case 10:
			goto L1000;
		case 11:
			goto L1100;
	}

	/* Invalid value for ISPEC */

	ret_val = -1;
	return ret_val;

L100:

	/* Convert NAME to upper case if the first character is lower case. */

	ret_val = 1;
	s_copy (subnam, (char *) name__, 6, name_len);
	ic = * (unsigned char *) subnam;
	iz = 'Z';
	if (iz == 90 || iz == 122) {

		/* ASCII character set */

		if (ic >= 97 && ic <= 122) {
			* (unsigned char *) subnam = (char) (ic - 32);
			for (i__ = 2; i__ <= 6; ++i__) {
				ic = * (unsigned char *) &subnam[i__ - 1];
				if (ic >= 97 && ic <= 122) {
					* (unsigned char *) &subnam[i__ - 1] = (char) (ic - 32);
				}
				/* L10: */
			}
		}

	} else if (iz == 233 || iz == 169) {

		/* EBCDIC character set */

		if (ic >= 129 && ic <= 137 || ic >= 145 && ic <= 153 || ic >= 162 && ic <= 169) {
			* (unsigned char *) subnam = (char) (ic + 64);
			for (i__ = 2; i__ <= 6; ++i__) {
				ic = * (unsigned char *) &subnam[i__ - 1];
				if (ic >= 129 && ic <= 137 || ic >= 145 && ic <= 153 || ic >= 162 && ic <= 169) {
					* (unsigned char *) &subnam[i__ - 1] = (char) (ic + 64);
				}
				/* L20: */
			}
		}

	} else if (iz == 218 || iz == 250) {

		/* Prime machines: ASCII+128 */

		if (ic >= 225 && ic <= 250) {
			* (unsigned char *) subnam = (char) (ic - 32);
			for (i__ = 2; i__ <= 6; ++i__) {
				ic = * (unsigned char *) &subnam[i__ - 1];
				if (ic >= 225 && ic <= 250) {
					* (unsigned char *) &subnam[i__ - 1] = (char) (ic - 32);
				}
				/* L30: */
			}
		}
	}

	* (unsigned char *) c1 = * (unsigned char *) subnam;
	sname = * (unsigned char *) c1 == 'S' || * (unsigned char *) c1 == 'D';
	cname = * (unsigned char *) c1 == 'C' || * (unsigned char *) c1 == 'Z';
	if (! (cname || sname)) {
		return ret_val;
	}
	s_copy (c2, subnam + 1, 2, 2);
	s_copy (c3, subnam + 3, 3, 3);
	s_copy (c4, c3 + 1, 2, 2);

	switch (*ispec) {
		case 1:
			goto L110;
		case 2:
			goto L200;
		case 3:
			goto L300;
	}

L110:

	/* ISPEC = 1: block size

	   In these examples, separate code is provided for setting NB for real
	   and complex.  We assume that NB will take the same value in single or
	   double precision. */

	nb = 1;

	if (s_cmp (c2, "GE", 2, 2) == 0) {
		if (s_cmp (c3, "TRF", 3, 3) == 0) {
			if (sname) {
				nb = 64;
			} else {
				nb = 64;
			}
		} else if (s_cmp (c3, "QRF", 3, 3) == 0 || s_cmp (c3, "RQF", 3, 3) == 0 || s_cmp (c3, "LQF", 3, 3) == 0
		           || s_cmp (c3, "QLF", 3, 3) == 0) {
			if (sname) {
				nb = 32;
			} else {
				nb = 32;
			}
		} else if (s_cmp (c3, "HRD", 3, 3) == 0) {
			if (sname) {
				nb = 32;
			} else {
				nb = 32;
			}
		} else if (s_cmp (c3, "BRD", 3, 3) == 0) {
			if (sname) {
				nb = 32;
			} else {
				nb = 32;
			}
		} else if (s_cmp (c3, "TRI", 3, 3) == 0) {
			if (sname) {
				nb = 64;
			} else {
				nb = 64;
			}
		}
	} else if (s_cmp (c2, "PO", 2, 2) == 0) {
		if (s_cmp (c3, "TRF", 3, 3) == 0) {
			if (sname) {
				nb = 64;
			} else {
				nb = 64;
			}
		}
	} else if (s_cmp (c2, "SY", 2, 2) == 0) {
		if (s_cmp (c3, "TRF", 3, 3) == 0) {
			if (sname) {
				nb = 64;
			} else {
				nb = 64;
			}
		} else if (sname && s_cmp (c3, "TRD", 3, 3) == 0) {
			nb = 32;
		} else if (sname && s_cmp (c3, "GST", 3, 3) == 0) {
			nb = 64;
		}
	} else if (cname && s_cmp (c2, "HE", 2, 2) == 0) {
		if (s_cmp (c3, "TRF", 3, 3) == 0) {
			nb = 64;
		} else if (s_cmp (c3, "TRD", 3, 3) == 0) {
			nb = 32;
		} else if (s_cmp (c3, "GST", 3, 3) == 0) {
			nb = 64;
		}
	} else if (sname && s_cmp (c2, "OR", 2, 2) == 0) {
		if (* (unsigned char *) c3 == 'G') {
			if (s_cmp (c4, "QR", 2, 2) == 0 || s_cmp (c4, "RQ", 2, 2) == 0 || s_cmp (c4, "LQ", 2, 2) == 0 ||
			        s_cmp (c4, "QL", 2, 2) == 0 || s_cmp (c4, "HR", 2, 2) == 0 || s_cmp (c4, "TR", 2, 2) == 0 ||
			        s_cmp (c4, "BR", 2, 2) == 0) {
				nb = 32;
			}
		} else if (* (unsigned char *) c3 == 'M') {
			if (s_cmp (c4, "QR", 2, 2) == 0 || s_cmp (c4, "RQ", 2, 2) == 0 || s_cmp (c4, "LQ", 2, 2) == 0 ||
			        s_cmp (c4, "QL", 2, 2) == 0 || s_cmp (c4, "HR", 2, 2) == 0 || s_cmp (c4, "TR", 2, 2) == 0 ||
			        s_cmp (c4, "BR", 2, 2) == 0) {
				nb = 32;
			}
		}
	} else if (cname && s_cmp (c2, "UN", 2, 2) == 0) {
		if (* (unsigned char *) c3 == 'G') {
			if (s_cmp (c4, "QR", 2, 2) == 0 || s_cmp (c4, "RQ", 2, 2) == 0 || s_cmp (c4, "LQ", 2, 2) == 0 ||
			        s_cmp (c4, "QL", 2, 2) == 0 || s_cmp (c4, "HR", 2, 2) == 0 || s_cmp (c4, "TR", 2, 2) == 0 ||
			        s_cmp (c4, "BR", 2, 2) == 0) {
				nb = 32;
			}
		} else if (* (unsigned char *) c3 == 'M') {
			if (s_cmp (c4, "QR", 2, 2) == 0 || s_cmp (c4, "RQ", 2, 2) == 0 || s_cmp (c4, "LQ", 2, 2) == 0 ||
			        s_cmp (c4, "QL", 2, 2) == 0 || s_cmp (c4, "HR", 2, 2) == 0 || s_cmp (c4, "TR", 2, 2) == 0 ||
			        s_cmp (c4, "BR", 2, 2) == 0) {
				nb = 32;
			}
		}
	} else if (s_cmp (c2, "GB", 2, 2) == 0) {
		if (s_cmp (c3, "TRF", 3, 3) == 0) {
			if (sname) {
				if (*n4 <= 64) {
					nb = 1;
				} else {
					nb = 32;
				}
			} else {
				if (*n4 <= 64) {
					nb = 1;
				} else {
					nb = 32;
				}
			}
		}
	} else if (s_cmp (c2, "PB", 2, 2) == 0) {
		if (s_cmp (c3, "TRF", 3, 3) == 0) {
			if (sname) {
				if (*n2 <= 64) {
					nb = 1;
				} else {
					nb = 32;
				}
			} else {
				if (*n2 <= 64) {
					nb = 1;
				} else {
					nb = 32;
				}
			}
		}
	} else if (s_cmp (c2, "TR", 2, 2) == 0) {
		if (s_cmp (c3, "TRI", 3, 3) == 0) {
			if (sname) {
				nb = 64;
			} else {
				nb = 64;
			}
		}
	} else if (s_cmp (c2, "LA", 2, 2) == 0) {
		if (s_cmp (c3, "UUM", 3, 3) == 0) {
			if (sname) {
				nb = 64;
			} else {
				nb = 64;
			}
		}
	} else if (sname && s_cmp (c2, "ST", 2, 2) == 0) {
		if (s_cmp (c3, "EBZ", 3, 3) == 0) {
			nb = 1;
		}
	}
	ret_val = nb;
	return ret_val;

L200:

	/* ISPEC = 2: minimum block size */

	nbmin = 2;
	if (s_cmp (c2, "GE", 2, 2) == 0) {
		if (s_cmp (c3, "QRF", 3, 3) == 0 || s_cmp (c3, "RQF", 3, 3) == 0 || s_cmp (c3, "LQF", 3, 3) == 0 ||
		        s_cmp (c3, "QLF", 3, 3) == 0) {
			if (sname) {
				nbmin = 2;
			} else {
				nbmin = 2;
			}
		} else if (s_cmp (c3, "HRD", 3, 3) == 0) {
			if (sname) {
				nbmin = 2;
			} else {
				nbmin = 2;
			}
		} else if (s_cmp (c3, "BRD", 3, 3) == 0) {
			if (sname) {
				nbmin = 2;
			} else {
				nbmin = 2;
			}
		} else if (s_cmp (c3, "TRI", 3, 3) == 0) {
			if (sname) {
				nbmin = 2;
			} else {
				nbmin = 2;
			}
		}
	} else if (s_cmp (c2, "SY", 2, 2) == 0) {
		if (s_cmp (c3, "TRF", 3, 3) == 0) {
			if (sname) {
				nbmin = 8;
			} else {
				nbmin = 8;
			}
		} else if (sname && s_cmp (c3, "TRD", 3, 3) == 0) {
			nbmin = 2;
		}
	} else if (cname && s_cmp (c2, "HE", 2, 2) == 0) {
		if (s_cmp (c3, "TRD", 3, 3) == 0) {
			nbmin = 2;
		}
	} else if (sname && s_cmp (c2, "OR", 2, 2) == 0) {
		if (* (unsigned char *) c3 == 'G') {
			if (s_cmp (c4, "QR", 2, 2) == 0 || s_cmp (c4, "RQ", 2, 2) == 0 || s_cmp (c4, "LQ", 2, 2) == 0 ||
			        s_cmp (c4, "QL", 2, 2) == 0 || s_cmp (c4, "HR", 2, 2) == 0 || s_cmp (c4, "TR", 2, 2) == 0 ||
			        s_cmp (c4, "BR", 2, 2) == 0) {
				nbmin = 2;
			}
		} else if (* (unsigned char *) c3 == 'M') {
			if (s_cmp (c4, "QR", 2, 2) == 0 || s_cmp (c4, "RQ", 2, 2) == 0 || s_cmp (c4, "LQ", 2, 2) == 0 ||
			        s_cmp (c4, "QL", 2, 2) == 0 || s_cmp (c4, "HR", 2, 2) == 0 || s_cmp (c4, "TR", 2, 2) == 0 ||
			        s_cmp (c4, "BR", 2, 2) == 0) {
				nbmin = 2;
			}
		}
	} else if (cname && s_cmp (c2, "UN", 2, 2) == 0) {
		if (* (unsigned char *) c3 == 'G') {
			if (s_cmp (c4, "QR", 2, 2) == 0 || s_cmp (c4, "RQ", 2, 2) == 0 || s_cmp (c4, "LQ", 2, 2) == 0 ||
			        s_cmp (c4, "QL", 2, 2) == 0 || s_cmp (c4, "HR", 2, 2) == 0 || s_cmp (c4, "TR", 2, 2) == 0 ||
			        s_cmp (c4, "BR", 2, 2) == 0) {
				nbmin = 2;
			}
		} else if (* (unsigned char *) c3 == 'M') {
			if (s_cmp (c4, "QR", 2, 2) == 0 || s_cmp (c4, "RQ", 2, 2) == 0 || s_cmp (c4, "LQ", 2, 2) == 0 ||
			        s_cmp (c4, "QL", 2, 2) == 0 || s_cmp (c4, "HR", 2, 2) == 0 || s_cmp (c4, "TR", 2, 2) == 0 ||
			        s_cmp (c4, "BR", 2, 2) == 0) {
				nbmin = 2;
			}
		}
	}
	ret_val = nbmin;
	return ret_val;

L300:

	/* ISPEC = 3: crossover point */

	nx = 0;
	if (s_cmp (c2, "GE", 2, 2) == 0) {
		if (s_cmp (c3, "QRF", 3, 3) == 0 || s_cmp (c3, "RQF", 3, 3) == 0 || s_cmp (c3, "LQF", 3, 3) == 0 ||
		        s_cmp (c3, "QLF", 3, 3) == 0) {
			if (sname) {
				nx = 128;
			} else {
				nx = 128;
			}
		} else if (s_cmp (c3, "HRD", 3, 3) == 0) {
			if (sname) {
				nx = 128;
			} else {
				nx = 128;
			}
		} else if (s_cmp (c3, "BRD", 3, 3) == 0) {
			if (sname) {
				nx = 128;
			} else {
				nx = 128;
			}
		}
	} else if (s_cmp (c2, "SY", 2, 2) == 0) {
		if (sname && s_cmp (c3, "TRD", 3, 3) == 0) {
			nx = 32;
		}
	} else if (cname && s_cmp (c2, "HE", 2, 2) == 0) {
		if (s_cmp (c3, "TRD", 3, 3) == 0) {
			nx = 32;
		}
	} else if (sname && s_cmp (c2, "OR", 2, 2) == 0) {
		if (* (unsigned char *) c3 == 'G') {
			if (s_cmp (c4, "QR", 2, 2) == 0 || s_cmp (c4, "RQ", 2, 2) == 0 || s_cmp (c4, "LQ", 2, 2) == 0 ||
			        s_cmp (c4, "QL", 2, 2) == 0 || s_cmp (c4, "HR", 2, 2) == 0 || s_cmp (c4, "TR", 2, 2) == 0 ||
			        s_cmp (c4, "BR", 2, 2) == 0) {
				nx = 128;
			}
		}
	} else if (cname && s_cmp (c2, "UN", 2, 2) == 0) {
		if (* (unsigned char *) c3 == 'G') {
			if (s_cmp (c4, "QR", 2, 2) == 0 || s_cmp (c4, "RQ", 2, 2) == 0 || s_cmp (c4, "LQ", 2, 2) == 0 ||
			        s_cmp (c4, "QL", 2, 2) == 0 || s_cmp (c4, "HR", 2, 2) == 0 || s_cmp (c4, "TR", 2, 2) == 0 ||
			        s_cmp (c4, "BR", 2, 2) == 0) {
				nx = 128;
			}
		}
	}
	ret_val = nx;
	return ret_val;

L400:

	/* ISPEC = 4: number of shifts (used by xHSEQR) */

	ret_val = 6;
	return ret_val;

L500:

	/* ISPEC = 5: minimum column dimension (not used) */

	ret_val = 2;
	return ret_val;

L600:

	/* ISPEC = 6: crossover point for SVD (used by xGELSS and xGESVD) */

	ret_val = (long) ( (float) MIN (*n1, *n2) * 1.6f);
	return ret_val;

L700:

	/* ISPEC = 7: number of processors (not used) */

	ret_val = 1;
	return ret_val;

L800:

	/* ISPEC = 8: crossover point for multishift (used by xHSEQR) */

	ret_val = 50;
	return ret_val;

L900:

	/* ISPEC = 9: maximum size of the subproblems at the bottom of the
	   computation tree in the divide-and-conquer algorithm (used by xGELSD
	   and xGESDD) */

	ret_val = 25;
	return ret_val;

L1000:

	/* ISPEC = 10: ieee NaN arithmetic can be trusted not to trap

	   ILAENV = 0 */
	ret_val = 1;
	if (ret_val == 1) {
		ret_val = NUMlapack_ieeeck (&c__0, &c_b162, &c_b163);
	}
	return ret_val;

L1100:

	/* ISPEC = 11: infinity arithmetic can be trusted not to trap

	   ILAENV = 0 */
	ret_val = 1;
	if (ret_val == 1) {
		ret_val = NUMlapack_ieeeck (&c__1, &c_b162, &c_b163);
	}
	return ret_val;
}								/* NUMlapack_ilaenv */

#undef a_ref
#undef c___ref

#undef MAX
#undef MIN

/* End of file NUMclapack.c */
