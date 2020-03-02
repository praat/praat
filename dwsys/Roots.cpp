/* Roots.cpp
 *
 * Copyright (C) 2020 David Weenink
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

#include "NUMclapack.h"
#include "NUMmachar.h"
#include "Polynomial.h"
#include "Roots.h"

#include "oo_DESTROY.h"
#include "Roots_def.h"
#include "oo_COPY.h"
#include "Roots_def.h"
#include "oo_EQUAL.h"
#include "Roots_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Roots_def.h"
#include "oo_READ_TEXT.h"
#include "Roots_def.h"
#include "oo_WRITE_TEXT.h"
#include "Roots_def.h"
#include "oo_READ_BINARY.h"
#include "Roots_def.h"
#include "oo_WRITE_BINARY.h"
#include "Roots_def.h"
#include "oo_DESCRIPTION.h"
#include "Roots_def.h"


Thing_implement (Roots, Daata, 1);

void structRoots :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of roots: ", numberOfRoots);
}

autoRoots Roots_create (integer numberOfRoots) {
	try {
		autoRoots me = Thing_new (Roots);
		my numberOfRoots = numberOfRoots;
		my roots = newCOMPVECzero (numberOfRoots);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Roots not created.");
	}
}

integer Roots_getNumberOfRoots (Roots me) {
	return my numberOfRoots;
}

dcomplex Roots_getRoot (Roots me, integer index) {
	Melder_require (index > 0 && index <= my numberOfRoots,
		U"Root index out of range.");
	return my roots [index];
}

void Roots_fixIntoUnitCircle (Roots me) {
	dcomplex z10 { 1.0, 0.0 };
	for (integer iroot = 1; iroot <= my numberOfRoots; iroot ++) {
		if (abs (my roots [iroot]) > 1.0)
			my roots [iroot] = z10 / conj(my roots [iroot]);
	}
}

static void NUMdcvector_extrema_re (COMPVEC const& v, integer lo, integer hi, double *out_min, double *out_max) {
	double min = v [lo].real(), max = v [lo].real();
	for (integer i = lo + 1; i <= hi; i ++)
		if (v [i].real() < min)
			min = v [i].real();
		else if (v [i].real() > max)
			max = v [i].real();
	if (out_min)
		*out_min = min;
	if (out_max)
		*out_max = max;
}

static void NUMdcvector_extrema_im (COMPVEC const& v, integer lo, integer hi, double *out_min, double *out_max) {
	double min = v [lo].imag(), max = v [lo].imag();
	for (integer i = lo + 1; i <= hi; i ++)
		if (v [i].imag() < min)
			min = v [i].imag();
		else if (v [i].imag() > max)
			max = v [i].imag();
	if (out_min)
		*out_min = min;
	if (out_max)
		*out_max = max;
}

void Roots_draw (Roots me, Graphics g, double rmin, double rmax, double imin, double imax,
	conststring32 symbol, double fontSize, bool garnish) {
	const double oldFontSize = Graphics_inqFontSize (g);
	const double eps = 1e-6;

	if (rmax <= rmin)
		NUMdcvector_extrema_re (my roots.get(), 1, my numberOfRoots, & rmin, & rmax);

	double denominator = fabs (rmax) > fabs (rmin) ? fabs (rmax) : fabs (rmin);
	if (denominator == 0.0)
		denominator = 1.0;
	if (fabs ((rmax - rmin) / denominator) < eps) {
		rmin -= 1.0;
		rmax += 1.0;
	}
	if (imax <= imin)
		NUMdcvector_extrema_im (my roots.get(), 1, my numberOfRoots, & imin, & imax);
	denominator = fabs (imax) > fabs (imin) ? fabs (imax) : fabs (imin);
	if (denominator == 0.0)
		denominator = 1.0;
	if (fabs ((imax - imin) / denominator) < eps) {
		imin -= 1;
		imax += 1;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, rmin, rmax, imin, imax);
	Graphics_setFontSize (g, fontSize);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	for (integer i = 1; i <= my numberOfRoots; i ++) {
		const double re = my roots [i].real(), im = my roots [i].imag();
		if (re >= rmin && re <= rmax && im >= imin && im <= imax)
			Graphics_text (g, re, im, symbol);
	}
	Graphics_setFontSize (g, oldFontSize);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		if (rmin * rmax < 0.0)
			Graphics_markLeft (g, 0.0, true, true, true, U"0");
		if (imin * imax < 0.0)
			Graphics_markBottom (g, 0.0, true, true, true, U"0");
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Imaginary part");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textBottom (g, true, U"Real part");
	}
}

autoRoots Polynomial_to_Roots (Polynomial me) {
	try {
		integer np1 = my numberOfCoefficients, n = np1 - 1;
		Melder_require (n > 0,
			U"Cannot find roots of a constant function.");
		/*
			Allocate storage for Hessenberg matrix (n * n) plus real and imaginary
			parts of eigenvalues wr [1..n] and wi [1..n].
		*/
		autoVEC hes = newVECzero (n * n);
		autoVEC wr = newVECraw (n);
		autoVEC wi = newVECraw (n);
		/*
			Fill the upper Hessenberg matrix (storage is Fortran)
			C: [i] [j] -> Fortran: (j-1)*n + i
		*/
		for (integer i = 1; i <= n; i ++) {
			hes [(i - 1) * n + 1] = - (my coefficients [np1 - i] / my coefficients [np1]);
			if (i < n)
				hes [(i - 1) * n + 1 + i] = 1.0;
		}

		// Find out the working storage needed

		char job = 'E', compz = 'N';
		integer ilo = 1, ihi = n, ldh = n, ldz = n, lwork = -1, info;
		double *z = nullptr, wt [1];
		NUMlapack_dhseqr (& job, & compz, & n, & ilo, & ihi, & hes [1], & ldh, & wr [1], & wi [1], z, & ldz, wt, & lwork, & info);
		if (info != 0)
			Melder_require (info > 0,
				U"Programming error. Argument ", info, U" in NUMlapack_dhseqr has illegal value.");
		lwork = Melder_ifloor (wt [0]);
		autoVEC work = newVECraw (lwork);

		// Find eigenvalues.

		NUMlapack_dhseqr (& job, & compz, & n, & ilo, & ihi, & hes [1], & ldh, & wr [1], & wi [1], z, & ldz, & work [1], & lwork, & info);
		integer nrootsfound = n;
		integer ioffset = 0;
		if (info > 0) {
			/*
				if INFO = i, NUMlapack_dhseqr failed to compute all of the eigenvalues. Elements i+1:n of
				WR and WI contain those eigenvalues which have been successfully computed
			*/
			nrootsfound -= info;
			Melder_require (nrootsfound > 0,
				U"No roots found.");
			Melder_warning (U"Calculated only ", nrootsfound, U" roots.");
			ioffset = info;
		} else if (info < 0) {
			Melder_throw (U"Programming error. Argument ", info, U" in NUMlapack_dhseqr has illegal value.");
		}

		autoRoots thee = Roots_create (nrootsfound);
		for (integer i = 1; i <= nrootsfound; i ++) {
			thy roots [i]. real (wr [ioffset + i]);
			thy roots [i]. imag (wi [ioffset + i]);
		}
		Roots_Polynomial_polish (thee.get(), me);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no roots can be calculated.");
	}
}

// workspace.size >= n * (n + 3) 
void Polynomial_into_Roots (Polynomial me, Roots r, VEC const& workspace) {
	try {
		integer np1 = my numberOfCoefficients, n = np1 - 1;
		Melder_require (n > 0,
			U"Cannot find roots of a constant function.");
		/*
			Allocate storage for Hessenberg matrix (n * n) plus real and imaginary
			parts of eigenvalues wr [1..n] and wi [1..n].
		*/
		VEC hes = workspace. part (1, n * n);
		VEC wr = workspace. part (n * n + 1, n * (n + 1));
		VEC wi = workspace. part (n * (n + 1) + 1, n * (n + 2));
		/*
			Fill the upper Hessenberg matrix (storage is Fortran)
			C: [i] [j] -> Fortran: (j-1)*n + i
		*/
		hes <<= 0.0;
		for (integer i = 1; i < n; i ++) {
			hes [(i - 1) * n + 1] = - (my coefficients [np1 - i] / my coefficients [np1]);
			hes [(i - 1) * n + 1 + i] = 1.0;
		}
		hes [(n - 1) * n + 1] = - my coefficients [1] / my coefficients [n + 1];
		/*
			Find out the working storage needed
		*/
		char job = 'E', compz = 'N';
		integer ilo = 1, ihi = n, ldh = n, ldz = n, lwork = -1, info;
		double *z = nullptr, wt [1];
		NUMlapack_dhseqr (& job, & compz, & n, & ilo, & ihi, & hes [1], & ldh, & wr [1], & wi [1], z, & ldz, wt, & lwork, & info);
		if (info != 0)
			Melder_require (info > 0,
				U"Programming error. Argument ", info, U" in NUMlapack_dhseqr has illegal value.");
		lwork = Melder_ifloor (wt [0]);
		Melder_require (lwork <= 2 * n,
			U"insufficient working memory.");
		VEC work = workspace. part (n * (n + 2) + 1, n * (n + 2) + lwork);

		// Find eigenvalues.

		NUMlapack_dhseqr (& job, & compz, & n, & ilo, & ihi, & hes [1], & ldh, & wr [1], & wi [1], z, & ldz, & work [1], & lwork, & info);
		integer nrootsfound = n;
		integer ioffset = 0;
		if (info > 0) {
			/*
				if INFO = i, NUMlapack_dhseqr failed to compute all of the eigenvalues. Elements i+1:n of
				WR and WI contain those eigenvalues which have been successfully computed
			*/
			nrootsfound -= info;
			Melder_require (nrootsfound > 0,
				U"No roots found.");
			Melder_warning (U"Calculated only ", nrootsfound, U" roots.");
			ioffset = info;
		} else if (info < 0) {
			Melder_throw (U"Programming error. Argument ", info, U" in NUMlapack_dhseqr has illegal value.");
		}

		for (integer i = 1; i <= nrootsfound; i ++) {
			r -> roots [i]. real (wr [ioffset + i]);
			r -> roots [i]. imag (wi [ioffset + i]);
		}
		r -> numberOfRoots = nrootsfound;
		Roots_Polynomial_polish (r, me);
	} catch (MelderError) {
		Melder_throw (me, U": no roots can be calculated.");
	}
}

void Roots_sort (Roots me) {
	(void) me;
}

/* Get value and derivative */
static void Polynomial_evaluateWithDerivative_z (Polynomial me, dcomplex *in_z, dcomplex *out_p, dcomplex *out_dp) {
	longdouble pr = my coefficients [my numberOfCoefficients], pi = 0.0;
	longdouble dpr = 0.0, dpi = 0.0, x = in_z->real(), y = in_z->imag();

	for (integer i = my numberOfCoefficients - 1; i > 0; i --) {
		longdouble tr   = dpr;
		dpr  =  dpr * x -  dpi * y + pr;
		dpi  =   tr * y +  dpi * x + pi;
		tr   = pr;
		pr   =   pr * x -   pi * y + my coefficients [i];
		pi   =   tr * y +   pi * x;
	}
	if (out_p)
		*out_p = { (double) pr, (double) pi };
	if (out_dp)
		*out_dp = { (double) dpr, (double) dpi };
}

static void Polynomial_polish_complexroot_nr (Polynomial me, dcomplex *z, integer maxit) {
	if (! NUMfpp)
		NUMmachar ();
	dcomplex zbest = *z;
	double pmin = 1e308;
	for (integer i = 1; i <= maxit; i ++) {
		dcomplex p, dp;
		Polynomial_evaluateWithDerivative_z (me, z, &p, &dp);
		const double fabsp = dcomplex_abs (p);
		if (fabsp > pmin || fabs (fabsp - pmin) < NUMfpp -> eps) {
			/*
				We stop, because the approximation is getting worse.
				Return the previous (hitherto best) value for z.
			*/
			*z = zbest;
			return;
		}
		pmin = fabsp;
		zbest = *z;
		if (dcomplex_abs (dp) == 0.0)
			return;
		const dcomplex dz = dcomplex_div (p, dp);   // Newton-Raphson
		*z = dcomplex_sub (*z, dz);
	}
	// Melder_throw (U"Maximum number of iterations exceeded.");
}

static double Polynomial_polish_realroot (Polynomial me, double x, integer maxit) {
	if (! NUMfpp)
		NUMmachar ();
	double xbest = x, pmin = 1e308;
	for (integer i = 1; i <= maxit; i ++) {
		double p, dp;
		Polynomial_evaluateWithDerivative (me, x, & p, & dp);
		const double fabsp = fabs (p);
		if (fabsp > pmin || fabs (fabsp - pmin) < NUMfpp -> eps) {
			/*
				We stop, because the approximation is getting worse or we cannot get any closer.
				Return the previous (hitherto best) value for x.
			*/
			x = xbest;
			return x;
		}
		pmin = fabsp;
		xbest = x;
		if (fabs (dp) == 0.0)
			return x;
		const double dx = p / dp;   // Newton-Raphson
		x -= dx;
	}
	return x;
	// Melder_throw (U"Maximum number of iterations exceeded.");
}

// Precondition: complex roots occur in pairs (a,bi), (a,-bi) with b>0
void Roots_Polynomial_polish (Roots me, Polynomial thee) {
	const integer maxit = 80;
	integer i = 1;
	while (i <= my numberOfRoots) {
		const double im = my roots [i].imag(), re = my roots [i].real();
		if (im != 0.0) {
			Polynomial_polish_complexroot_nr (thee, & my roots [i], maxit);
			if (i < my numberOfRoots && im == - my roots [i + 1].imag() && re == my roots [i + 1].real()) {
				my roots [i + 1]. real (my roots [i].real());
				my roots [i + 1]. imag (- my roots [i].imag());
				i ++;
			}
		} else {
			my roots [i]. real (Polynomial_polish_realroot (thee, my roots [i].real(), maxit));
		}
		i ++;
	}
}

autoPolynomial Roots_to_Polynomial (Roots me, bool rootsAreReal) {
	try {
		(void) me;
		autoPolynomial thee;
		if (! rootsAreReal)
			throw MelderError();
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Not implemented yet");
	}
}

/* End of file Roots.cpp */
