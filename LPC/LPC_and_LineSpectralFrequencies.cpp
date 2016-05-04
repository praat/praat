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


static void transcendentalToCosineCoefficients_inplace (double *g, int order) {
	/* c[0] = g[order]; c[k]=2*g[M-k]; g <-- c */
	double gm = g [order];
	for (long i = 0; i < order / 2 + 1; i++) {
		double tmp = g [i];
		g [i] = 2.0 * g [order - i];
		g [order - i] = 2.0 * tmp;
	}
	g [0] = gm;
}

static void lpc_to_lsfcoefficients (double *a /*1..n*/, long numberOfCoefficients, double *g1 /*0..*/, long *order_g1, double *g2 /*0..*/, long *order_g2) {
	*order_g1 = *order_g2 = numberOfCoefficients;
	if (numberOfCoefficients % 2 == 0) {
		/* G1(z) <--- Fs(z)/(1+1/z) and G2(z) <--- Fa(z)/(1-1/z) */
		g1 [0] = g2 [0] = 1.0;
		for (long i = 0; i < numberOfCoefficients / 2; i ++) { // palindromic: no need to store more coefficients
			/* Construction of Fs: 
			 * 	g [0] = g[numberOfCoefficients-j] = 1
			 * 	g [i] = a [i] + a [numberOfCoefficients - i] for i=1..numberOfCoefficients/2
			 * 	g [numberOfCoefficients - j] = g [j] for j = 1..numberOfCoefficients/2-1
			 * Division by 1+1/z gives g [i] -= g [i-1]
			 * Construction of Fa: g [i] = a [i] - a [numberOfCoefficients - i];
			 */
			g1 [i + 1] = a [i + 1] + a [numberOfCoefficients - i] - g1 [i]; // Fs(z)/(1+1/z)
			g2 [i + 1] = a [i + 1] - a [numberOfCoefficients - i] + g2 [i]; // Fa(z)/(1-1/z)
		}
	} else { // odd
		// G1(z) <--- Fs(z) and G2(z) <--- Fa(z)/(1-1/z^2)
		(*order_g1) ++;
		(*order_g2) --;
		g1 [0] = g2 [0] = 1.0;
		for (long i = 0; i < *order_g1 / 2; i ++) {
			g1 [i + 1] = a [i + 1] + a [numberOfCoefficients - i]; // Fs(z)
			g2 [i + 1] = a [i + 1] - a [numberOfCoefficients - i] + g2 [i]; // Fa(z)/(1-1/z)
		}
		for (long i = 0; i < *order_g2 / 2; i ++) {
			g2 [i + 1] -= g2 [i]; // Fa(z)/(1+1/z)
		}
	}
}

/* Conversion from Y(w) to a polynomial in x (= cos (w))
 * From: Joseph Rothweiler (1999), "A rootfinding algorithm for line spectral frequencies",
 * 1999 IEEE International Conference on Acoustics, Speech, and Signal Processing, 1999. (Volume:2 )
 */
static void cos2x (double *g, long order) {
	for (long i = 2; i <= order; i ++) {
		for (long j = order; j > i; j--) {
			g [j - 2] -= g[j];
		}
		g [i - 2] -= 2.0 * g [i];
	}
}

typedef struct polyStruct {
	double *c; long order;
} *polystruct;

void dpoly (double x, double *f, double *df, void *closure) {
	polystruct p = (polystruct) closure;
	*f = p -> c [p -> order];
	*df = 0.0;
	for (long i = p -> order - 1; i >= 0; i--) {
		*df = *df * x + *f;
		*f = *f * x + p -> c [i];  // Evaluate polynomial (Horner)
	}	
}


/* Evaluate polynomial c[0]+c[1]*x+...c_order*x^c_order */
static double evaluatePolynomial (double *c, int order, double x) {
	double result = c [order];
	for (long i = order - 1; i >= 0; i--) {
		result = result * x + c [i];  // Evaluate polynomial (Horner)
	}
	return result;
}

static void evaluatePolynomialAndDerivatives (double *c, int order, double x, double *derivative, int numberOfDerivatives) {
	/* Evaluate polynomial c[0]+c[1]*x+...c_order*x^c_order in derivative[0] and derivatives [1..numberOfDerivatives] */
	numberOfDerivatives = numberOfDerivatives > order ? order : numberOfDerivatives;
	
	derivative [0] = c [order];
	for (long j = 1; j <= numberOfDerivatives; j ++) {
		derivative [j] = 0.0;
	}
	
	for (long i = order - 1; i >= 0; i--) {
		long n = (numberOfDerivatives < (order - i) ? numberOfDerivatives : order - i);
		for (long j = n; j >= 1; j--) {
			derivative [j] = derivative [j] * x +  derivative [j - 1];
		}
		derivative [0] = derivative [0] * x + c [i];  // Evaluate polynomial (Horner)
	}
	double fact = 1.0;
	for (long j = 2; j <= numberOfDerivatives; j ++) {
		fact *= j;
		derivative [j] *= fact;
	}
}

/* g[0]+g[1]x + ... g[m]*x^ m = 0 ; m must be even
 * Semenov, Kalyuzhny, Kovtonyuk (2003), Efficient calculation of line spectral frequencies based on new method for solution of transcendental equations,
 * ICASSP 2003, 457--460
 * 		g[0 .. g_order]
 * 		work [0.. g_order + 1 + (numberOfDerivatives + 1) * 5]
 * 		root [1 .. (g_order+1)/2]
 */
static void findRoots (double *g, long g_order, double *roots, long numberOfDerivatives, double *work) {
	if (numberOfDerivatives < 3) {
		Melder_throw (U"Number of derivatives must be at least 3.");
	}
	double xmin = -1.0, xmax = 1.0;
	long numberOfRootsFound = 0;
	double *gabs = work, *fact = gabs + g_order + 1, *p2 = fact + numberOfDerivatives + 1;
	double *derivatives = p2 + numberOfDerivatives + 1, *constraints = derivatives + numberOfDerivatives + 1;
	double *intervals = constraints + numberOfDerivatives + 1;
	struct polyStruct poly;
	poly.c = g; poly.order = g_order;
	
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
		gabs [k] = fabs (g [k]);
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

/* Find roots on (-1,1) of polynomial c[0]+c[1]*x+c[2]*x^2 + ... + c[order]*x^order */
static long findRootsOnGrid (double *c, long order, double *roots, double dx) {
	struct polyStruct poly;
	poly.c = c; poly.order = order;
	double xmin = -1.0, xmax = xmin + dx;
	long numberOfRootsFound = 0;
	while (xmin < 1.0 && numberOfRootsFound != order) {
		double fxmin = evaluatePolynomial (c, order, xmin);
		double fxmax = evaluatePolynomial (c, order, xmax);
		if (fxmin * fxmax <= 0.0) {
			double root;
			NUMnrbis (dpoly, xmin, xmax, &poly, &root);
			roots [++numberOfRootsFound] = root;
			if (numberOfRootsFound > 1 && roots [numberOfRootsFound] == roots [numberOfRootsFound - 1]) {
				numberOfRootsFound--;
			}
		}
		xmin = xmax; xmax += dx;
		xmax = xmax > 1.0 ? 1.0 : xmax;
	}
	return numberOfRootsFound;
}		

static void test () {
		/* f(x)= (x-0.1)(x-0.2)(x-0.3)(x-0.4) */
		long nzeros = 4;
		double zeros[5] = {0.0, 0.1, 0.2, 0.3, 0.4 };
		double roots[5], work[100];
		double c[5] = {0.0024, -0.05, 0.35, -1.0, 1.0}; /* c0*c1*x+c2*x^2+c3*x^3+c4*x^4 */
		double derivatives [5];
		struct polyStruct poly;
		poly.c = c; poly.order = nzeros;
		MelderInfo_open ();
		for (long i = 1; i <= nzeros; i++) {
			double f, df;
			dpoly (zeros[i], &f, &df, &poly);
			evaluatePolynomialAndDerivatives (c, nzeros, zeros[i], derivatives, nzeros);
			MelderInfo_writeLine (U"Zero at ", zeros[i], U" Value = ", derivatives[0], U" ", f, U" ", df);
			for (long j = 1; j <= nzeros; j++) {
				MelderInfo_writeLine (U"   ", derivatives[j], U" (", j, U")");
			}
		}
		//findRoots (c, nzeros, roots, 3, work);
		long nroots = findRootsOnGrid (c, nzeros, roots, 0.02);
		for (long i = 1; i <= nroots; i++) {
			MelderInfo_writeLine (U"Root at ", roots[i]);
		}
		double g1[6]={1,2,3,4,5,6}, g2[5]={1,2,3,4,5};
		transcendentalToCosineCoefficients_inplace (g1, 5);
		MelderInfo_writeLine (U"In place: {1,2,3,4,5,6}");
		for (long i = 0; i <= 5; i++) {
			MelderInfo_write (U" ", g1 [i]);
		}
		transcendentalToCosineCoefficients_inplace (g2, 4);
		MelderInfo_writeLine (U"\nIn place: {1,2,3,4,5}");
		for (long i = 0; i <= 4; i++) {
			MelderInfo_write (U" ", g2 [i]);
		}
		double a1[5] ={1,2,3,4,5}, a2[6]={1,2,3,4,5,6};
		long order_g1, order_g2;
		lpc_to_lsfcoefficients (a1, 4, g1, & order_g1, g2, & order_g2);
		MelderInfo_writeLine (U"\na1[5] ={1,2,3,4,5} order_g1=", order_g1, U" order_g2=", order_g2);
		for (long i = 0; i <= 2; i++) {
			MelderInfo_writeLine (U" ", g1 [i], U" ", g2 [i]);
		}
		lpc_to_lsfcoefficients (a2, 5, g1, & order_g1, g2, & order_g2);
		MelderInfo_writeLine (U"\na2[6]={1,2,3,4,5,6} order_g1=", order_g1, U" order_g2=", order_g2);
		for (long i = 0; i <= 3; i++) {
			MelderInfo_writeLine (U" ", g1 [i], U" ", g2 [i]);
		}
		MelderInfo_close ();
}

autoLineSpectralFrequencies LPC_to_LineSpectralFrequencies (LPC me, int numberOfDerivatives, double precision) {
	try {
		if (numberOfDerivatives == 0) {
			numberOfDerivatives = 6;
		}
		if (precision == 0.0) {
			precision = 1e-6;
		}
		autoLineSpectralFrequencies thee = LineSpectralFrequencies_create (my xmin, my xmax, my nx, my dx, my x1, my maxnCoefficients, my samplingPeriod);
		 test(); return thee;
		/* Rothweiler (1999), A rootfinding algorithm for line spectral frequencies
		 * Sum and difference polynomials:
		 * 	Fs(z) = A(z)+z^(-p-1)A(1/z)
		 * 	Fa(z) = A(z)-z^(-p-1)A(1/z), order(Fs) = order (Fa) = p+1,
		 * 
		 *	where A(z) = 1 - sum(k=1, P, a[k]*z^(-k))
		 * 
		 * The order of Fs and Fa is p+1. These polynomials are palindromic in their coefficients and 
		 * 	have zeros at z=1 and z=-1, respectively.
		 * 
		 * Divide out these zeros:
		 * 	1. p is even:
		 * 		G1(z) = Fs(z)/(1+1/z) ; G2(z) = fa(z)/(1-1/z)
		 *		order (G1) = order (G2) = p
		 *	2. p is odd
		 *		G1(z) = Fs(z) ; G2(z) = Fa(z)/(1+1/z^2), 
		 *		order (G1) = p+1 and order (G2) = p-1
		 * 
		 * G1 and G2 are always palindromic polynomials of even order (2*M1) and (2*M2).
		 * The zeros of G1 and G2 have special properties:
		 * 1. They all lie at the unit circle
		 * 2. They occur in complex pairs (if x[1] = exp(jw) is a zero then exp(-jw) is a zero too)
		 * 3. The zeros of G1 and G2 are interleaved
		 * 4. They are all distinct
		 * 
		 * G1 and G2 can be written as:
		 * 
		 * G(z) = sum (i=0, 2*M, g[i]*exp(-jiw)) =  // where g [i] = g [2M-i]
		 *      = sum (i=0, M-1, g[i]exp(-jiw)+g[2M-i]exp(-j(2M-i)w)) + g[M]exp(-jMw)
		 * 		= exp(-jMw) * {sum (i=0, M-1, g[i]exp(j(M-i)w) + g[2M-i]exp(-j(M-i)w)) + g[M]
		 *      = exp(-jMw) * {sum (i=0, M-1, 2*g[i]*cos((M-i)w)) + g[M]} 
		 *      = exp(-jMw) * sum (k=0, M, c[k]*cos(k*w))
		 *      = exp(-jMw) * Y(w), 
		 * 	where Y(w) = sum (k=0, M, c[k]*cos(k*w)) and c[0]=g[M] and c[k] = 2*g[M-k] for k = 1..M
		 * 
		 * The transcendental functions Y(w) can be transformed to a polynomial in x = cos(w)
		 * 		Y(x) = sum (k=0, M, r[k]*x^(M-k)). The zeros of Y(x) are in [-1, 1],
		 * 
		 *  The roots of this polynomial are real and keep the ordering property 
		 * 	(if x1[i] and x2[i] are the roots of Y1(x) and Y2(x) respectively:
		 *		x1[1] < x2[1] < x1[2] < x2[2] < ... < x1[M] < x2[M]
		 */
		
		autoNUMvector<double> g1 (0L, my maxnCoefficients + 1), g2 (0L, my maxnCoefficients);
		autoNUMvector<double> work (0L, my maxnCoefficients + 1 + 5 * (numberOfDerivatives + 1));
		autoNUMvector<double> roots (1, (my maxnCoefficients + 1) / 2);
		
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			long order_g1, order_g2;
			LPC_Frame lpf = & my d_frames [iframe];
			LineSpectralFrequencies_Frame lsf = & thy d_frames [iframe];
			LineSpectralFrequencies_Frame_init (lsf, lpf -> nCoefficients);
			
			/* Construct Fs in g1, and, Fa in g2 and divide out the zeros 
			 * 	We only need to store coefficients[0..order/2] in g1 and g2!!
			 */
			lpc_to_lsfcoefficients (lpf -> a, lpf -> nCoefficients, g1.peek(), & order_g1, g2.peek(), & order_g2);
			
			long half_order_g1 = order_g1 / 2, half_order_g2 = order_g2 / 2;
			/* Transform from G(z)=sum(i=0, 2*M,exp(-j*i*w)) to Y(w)=exp(-j*M*w) * sum (i=0, M, c[k]*cos(k*w)) */
			
			transcendentalToCosineCoefficients_inplace (g1.peek(), half_order_g1);
			transcendentalToCosineCoefficients_inplace (g2.peek(), half_order_g2);
			
			/* Transform the trancendental to polynomial equation */
			cos2x (g1.peek(), half_order_g1);
			//findRoots (g1.peek(), order_g1 / 2, lsf -> frequencies, numberOfDerivatives, work.peek());
			while (findRootsOnGrid (g1.peek(), half_order_g1, roots.peek(), precision) != half_order_g1) {
				precision *= 0.5;
			}
			for (long i = 1; i <= half_order_g1; i++) {
				lsf -> frequencies [2 * i - 1] = roots [i];
			}
			// the roots of g2 lie inbetween the roots of g1
			struct polyStruct poly;
			poly.c = g2.peek(); poly.order = half_order_g2;
			
			for (long i = 1; i <= half_order_g2; i ++) {
				double xmin = roots[i];
				double xmax = i == half_order_g1 ? 1.0 : roots[i + 1];
				NUMnrbis (dpoly, xmin, xmax, &poly, &lsf -> frequencies [2 * i]);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LineSpectralFrequencies created.");
	}
}

autoLPC LineSpectralFrequencies_to_LPC (LineSpectralFrequencies me) {
	
	
}

/* End of file LPC_and_LineSpectralFrequencies.cpp */
