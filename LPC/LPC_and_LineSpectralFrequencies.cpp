/* LPC_and_LineSpectralFrequencies.cpp
 *
 * Copyright (C) 2016 David Weenink
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
 djmw 20160421  Initial version
*/

#include "LPC_and_LineSpectralFrequencies.h"
#include "NUM2.h"
#include "Polynomial.h"


/* Conversion from Y(w) to a polynomial in x (= 2 cos (w))
 * From: Joseph Rothweiler (1999), "On Polynomial Reduction in the Computation of LSP Frequencies." 
 * 	IEEE Trans. on ASSP 7, 592--594.
 */
static void cos2x (double *g, long order) {
	for (long i = 2; i <= order; i ++) {
		for (long j = order; j > i; j--) {
			g [j - 2] -= g[j];
		}
		g [i - 2] -= 2.0 * g [i];
	}
}

static void Polynomial_fromLPC_Frame_lspsum (Polynomial me, LPC_Frame lpc) {
	/* Fs (z) = A(z) + z^-(p+1) A(1/z) */
	
	long order = lpc -> nCoefficients, g_order = (order + 1) / 2; // orders
	my coefficients [order + 2] = 1.0;
	for (long i = 1; i <= order; i++) {
		my coefficients [order + 2 - i] = lpc -> a [i] + lpc -> a [order + 1 - i];
	}
	my coefficients [1] = 1.0;
	my numberOfCoefficients = order + 2;

	if (order % 2 == 0) { // order even
		Polynomial_divide_firstOrderFactor (me, -1.0, nullptr);
	}
	/* transform to cos(w) terms */
	for (long i = 1; i <= g_order + 1; i++) {
		my coefficients [i] = my coefficients [g_order + i];
	}
	my numberOfCoefficients = g_order + 1;
	/* to Chebychev */
	cos2x (& my coefficients [1], g_order);
}

static void Polynomial_fromLPC_Frame_lspdif (Polynomial me, LPC_Frame lpc) {
	/* Fa (z) = A(z) - z^-(p+1)A(1/z) */
	long order = lpc -> nCoefficients;
	my coefficients [order + 2] = -1.0;
	for (long i = 1; i <= order; i++) {
		my coefficients [order + 2 - i] = -lpc -> a [i] + lpc -> a [order + 1 - i];
	}
	my coefficients [1] = 1.0;
	my numberOfCoefficients = order + 2;

	if (order % 2 == 0) { // Fa(z)/(z-1)
		Polynomial_divide_firstOrderFactor (me, 1.0, nullptr);
	} else { // Fa(z) / (z^2 - 1)
		Polynomial_divide_secondOrderFactor (me, 1.0);
	}
	/* transform to cos(w) terms */
	long g_order = my numberOfCoefficients / 2;
	for (long i = 1; i <= g_order + 1; i++) {
		my coefficients [i] = my coefficients [g_order + i];
	}
	my numberOfCoefficients = g_order + 1;
	/* to Chebychev */
	cos2x (& my coefficients [1], g_order);
}

#if 0
/* g[0]+g[1]x + ... g[m]*x^ m = 0 ; m must be even
 * Semenov, Kalyuzhny, Kovtonyuk (2003), Efficient calculation of line spectral frequencies based on new method for solution of transcendental equations,
 * ICASSP 2003, 457--460
 * 		g[0 .. g_order]
 * 		work [0.. g_order + 1 + (numberOfDerivatives + 1) * 5]
 * 		root [1 .. (g_order+1)/2]
 */
static void Roots_fromPolynomial (Roots me, Polynomial g, long numberOfDerivatives, double *work) {
	if (numberOfDerivatives < 3) {
		Melder_throw (U"Number of derivatives must be at least 3.");
	}
	double xmin = -1.0, xmax = 1.0;
	long numberOfRootsFound = 0;
	long g_order = g -> numberOfCoefficients - 1;
	double *gabs = work, *fact = gabs + g_order + 1, *p2 = fact + numberOfDerivatives + 1;
	double *derivatives = p2 + numberOfDerivatives + 1, *constraints = derivatives + numberOfDerivatives + 1;
	double *intervals = constraints + numberOfDerivatives + 1;
	
	/* Fill vectors with j! and 2^j only once */
	fact [0] = p2 [0] = 1.0;
	for (long j = 1; j <= numberOfDerivatives; j ++) {
		fact [j] = fact [j - 1] * j; // j!
		p2 [j] = p2 [j - 1] * 2.0; // 2^j
	}
	
	/* The constraints M[j] (Semenov et al. eq. (8)) can be calculated by taking absolute values of 
	 * the polynomial coefficients and evaluating the polynomial and the derivatives at x = 1.0
	 */
	for (long k = 0; k <= g_order; k++) {
		gabs [k] = fabs (g -> coefficients [k + 1]);
	}
	evaluatePolynomialAndDerivatives (gabs, g_order, 1.0, constraints, numberOfDerivatives);
	intervals [0] = 1.0;
	while (numberOfRootsFound < g_order || xmin == xmax) {
		double dsum1 = 0.0, dsum2 = 0.0;
		double xmid = (xmin + xmax) / 2.0;
		evaluatePolynomialAndDerivatives (g, g_order, xmid, derivatives, numberOfDerivatives);
		double fxmid = derivatives[0], fdxmin = derivatives[1];
		long j = 1;
		bool rootsOnIntervalPossible_f = true, rootsOnIntervalPossible_df = true;
		while (j <= numberOfDerivatives && (rootsOnIntervalPossible_f || rootsOnIntervalPossible_df)) {
			intervals [j] = intervals [j - 1] * (xmax - xmin);
			long k = j - 1;
			if (j > 1) { // start at first derivative
				dsum1 += fabs (derivatives [k]) * intervals [k] / (p2 [k] * fact [k]);
			}
			if (j > 2) { // start at second derivative
				dsum2 += fabs (derivatives [k]) * intervals [k - 1] / (p2 [k - 1] * fact [k - 1]);
				if (rootsOnIntervalPossible_f) {
					double testValue1 = dsum1 + constraints [j] * intervals [j] / (p2 [j] * fact [j]);
					rootsOnIntervalPossible_f = ! (fxmid + testValue1 < 0 || fxmid - testValue1 > 0);
				}
				if (rootsOnIntervalPossible_df) {
					double testValue2 = dsum2 + constraints [j] * intervals [j - 1] / (p2 [j - 1] * fact [j - 1]);
					rootsOnIntervalPossible_df = ! (fdxmin + testValue2 < 0 || fdxmin - testValue2 > 0);
				}
			}
			j++;
		}
		if (rootsOnIntervalPossible_f) {
			if (rootsOnIntervalPossible_df) { // f(x) uncertain && f'(x) uncertain : bisect
				xmax = xmid;
			} else {// f(x) uncertain; f'(x) certain
				double fxmin = evaluatePolynomial (g, g_order, xmin);
				double fxmax = evaluatePolynomial (g, g_order, xmax);
				if (fxmin * fxmax <= 0.0) {
					double root;
					NUMnrbis (dpoly, xmin, xmax, &poly, &root);
					roots [++numberOfRootsFound] = root;
				} else {
					xmin = xmax; xmax = 1.0;
				}
			}
		} else {
			xmin = xmax; xmax = 1.0;
		}
	}	
}
#endif

static long Roots_fromPolynomial_grid (Roots me, Polynomial thee, double gridSize) {
	Melder_assert (my max >= thy numberOfCoefficients - 1);
	double xmin = thy xmin;
	long numberOfRootsFound = 0;
	while (xmin < thy xmax && numberOfRootsFound < thy numberOfCoefficients - 1) {
		double xmax = xmin + gridSize;
		xmax = xmax > thy xmax ? thy xmax : xmax;
		//double root = Polynomial_findOneRealRoot_nr (thee, xmin, xmax);
		double root = Polynomial_findOneSimpleRealRoot_ridders (thee, xmin, xmax);
		if (isdefined (root) && (numberOfRootsFound == 0 || my v [numberOfRootsFound].re != root)) {
			my v [++numberOfRootsFound].re = root; // root not at border of interval
			my v [numberOfRootsFound].im = 0.0;
		}
		xmin = xmax;
	}
	// make rest of storage undefined (not necessary)
	return numberOfRootsFound;
}

static void LineSpectralFrequencies_Frame_initFromLPC_Frame_grid (LineSpectralFrequencies_Frame me, LPC_Frame thee, Polynomial g1, Polynomial g2, Roots roots, double gridSize, double maximumFrequency) {
	/* Construct Fs and Fa
		* divide out the zeros
		* transform to polynomial equations g1 and g2 of half the order
		*/
	LineSpectralFrequencies_Frame_init (me, thy nCoefficients);
	Polynomial_fromLPC_Frame_lspsum (g1, thee);
	long half_order_g1 = g1 -> numberOfCoefficients - 1;
	Polynomial_fromLPC_Frame_lspdif (g2, thee);
	long half_order_g2 = g2 -> numberOfCoefficients - 1;
	
	long numberOfBisections = 0, numberOfRootsFound = 0;
	while (numberOfRootsFound  < half_order_g1 && numberOfBisections < 10) {
		numberOfRootsFound = Roots_fromPolynomial_grid (roots, g1, gridSize);
		gridSize *= 0.5; numberOfBisections++;
	}
	if (numberOfBisections == 10) {
		Melder_throw (U"Too many bisections.");
	}
	// [g1-> xmin, g1 -> xmax] <==> [nyquistFrequency, 0] i.e. highest root corresponds to lowest frequency
	for (long i = 1; i <= half_order_g1; i++) {
		my frequencies [2 * i - 1] = acos (roots -> v [half_order_g1 + 1 - i].re / 2.0) / NUMpi * maximumFrequency; 
	}
	// the roots of g2 lie inbetween the roots of g1
	for (long i = 1; i <= half_order_g2; i++) {
		double xmax = roots -> v [half_order_g1 + 1 - i].re;
		double xmin = i == half_order_g1 ? g1 -> xmin : roots -> v [half_order_g1 - i].re;
		double root = Polynomial_findOneSimpleRealRoot_ridders (g2, xmin, xmax);
		if (isdefined (root)) {
			my frequencies [2 * i] = acos (root / 2.0) / NUMpi * maximumFrequency;
		} else { 
			my numberOfFrequencies --;
		}	
	}
}

autoLineSpectralFrequencies LPC_to_LineSpectralFrequencies (LPC me, double gridSize) {
	try {
		if (gridSize == 0.0) {
			gridSize = 0.02;
		}
		double nyquistFrequency = 0.5 / my samplingPeriod;
		autoLineSpectralFrequencies thee = LineSpectralFrequencies_create (my xmin, my xmax, my nx, my dx, my x1, my maxnCoefficients, nyquistFrequency);
		autoPolynomial g1 = Polynomial_create (-2.0, 2.0, my maxnCoefficients + 1); // large enough
		autoPolynomial g2 = Polynomial_create (-2.0, 2.0, my maxnCoefficients + 1);
		autoRoots roots = Roots_create ((my maxnCoefficients + 1) / 2);
		
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			LPC_Frame lpf = & my d_frames [iframe];
			LineSpectralFrequencies_Frame lsf = & thy d_frames [iframe];
			/* Construct Fs and Fa
			 * divide out the zeros
			 * transform to polynomial equations g1 and g2 of half the order
			 * find zeros
			 */
			LineSpectralFrequencies_Frame_initFromLPC_Frame_grid (lsf, lpf, g1.get(), g2.get(), roots.get(), gridSize, nyquistFrequency);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LineSpectralFrequencies created.");
	}
}

static void LPC_Frame_initFromLineSpectralFrequencies_Frame (LPC_Frame me, LineSpectralFrequencies_Frame thee, Polynomial fs, Polynomial fa, double maximumFrequency) {
	LPC_Frame_init (me, thy numberOfFrequencies);

	/* Reconstruct Fs (z) */
	long numberOfOmegas = (thy numberOfFrequencies + 1) / 2;
	for (long i = 1; i <= numberOfOmegas; i++) {
		double omega = thy frequencies [2 * i -1] / maximumFrequency * NUMpi;
		my a[i] = -2.0 * cos (omega);
	}
	Polynomial_initFromProductOfSecondOrderTerms (fs, my a, numberOfOmegas);

	/* Reconstruct Fa (z) */
	numberOfOmegas = thy numberOfFrequencies / 2;
	for (long i = 1; i <= numberOfOmegas; i++) {
		double omega = thy frequencies [2 * i] / maximumFrequency * NUMpi;
		my a [i] = -2.0 * cos (omega);
	}
	Polynomial_initFromProductOfSecondOrderTerms (fa, my a, numberOfOmegas);
	
	if (thy numberOfFrequencies % 2 == 0) {
		Polynomial_multiply_firstOrderFactor (fs, -1.0); // * (z + 1)
		Polynomial_multiply_firstOrderFactor (fa, 1.0); // * (z - 1)
	} else {
		Polynomial_multiply_secondOrderFactor (fa, 1.0); // * (z^2 - 1)
	}
	Melder_assert (fs -> numberOfCoefficients == fa -> numberOfCoefficients);
	/* A(z) = (Fs(z) + Fa(z) / 2 */
	for (long i = 1; i <= fs -> numberOfCoefficients - 2; i++) {
		my a [thy numberOfFrequencies - i + 1] = 0.5 * (fs -> coefficients [i+1] + fa -> coefficients [i+1]);
	}
}

autoLPC LineSpectralFrequencies_to_LPC (LineSpectralFrequencies me) {
	try {
		autoLPC thee = LPC_create (my xmin, my xmax, my nx, my dx, my x1, my maximumNumberOfFrequencies,0.5 / my maximumFrequency);
		autoPolynomial fs = Polynomial_create (-1.0, 1.0, my maximumNumberOfFrequencies + 2);
		autoPolynomial fa = Polynomial_create (-1.0, 1.0, my maximumNumberOfFrequencies + 2);
		
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			LineSpectralFrequencies_Frame lsf = & my d_frames [iframe];
			LPC_Frame lpf = & thy d_frames [iframe];
			/* Construct Fs and Fa
			 * A(z) = (Fs(z) + Fa(z))/2
			 */
			LPC_Frame_initFromLineSpectralFrequencies_Frame (lpf, lsf, fs.get(), fa.get(), my maximumFrequency);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC created from LineSpectralFrequencies.");
	}
}

/* End of file LPC_and_LineSpectralFrequencies.cpp */
