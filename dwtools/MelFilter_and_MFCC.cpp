/* MelFilter_and_MFCC.cpp
 *
 * Copyright (C) 1993-2013 David Weenink
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
 djmw 2001
 djmw 20020813 GPL header
 djmw 20130207 Latest modification
*/

#include "MelFilter_and_MFCC.h"
#include "NUM2.h"

#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))

static double **NUMcosinesTable (long  n) {
	autoNUMmatrix<double> costab (1, n, 1, n);
	for (long k = 1; k <= n; k++) {
		for (long j = 1; j <= n; j++) {
			costab[k][j] = cos (NUMpi * (k - 1) * (j - 0.5) / n);
		}
	}
	return costab.transfer();
}

// x[1..n] : input
// y[1..n] : output
static void NUMcosineTransform (double *x, double *y, long n, double **cosinesTable) {
	for (long k = 1; k <= n; k++) {
		y[k] = 0;
		for (long j = 1; j <= n; j++) {
			y[k] += x[j] * cosinesTable[k][j];
		}
	}
}


// x: input
// y: output
static void NUMinverseCosineTransform (double *x, double *y, long n, double **cosinesTable) {
	for (long j = 1; j <= n; j++) {
		y[j] = 0.5 * x[1] * cosinesTable[1][j];
		for (long k = 2; k <= n; k++) {
			y[j] += x[k] * cosinesTable[k][j];
		}
		y[j] *= 2.0 / n;
	}
}

double testCosineTransform (long n) {
	try {
		autoNUMvector<double> x (1, n);
		autoNUMvector<double> y (1, n);
		autoNUMvector<double> x2 (1, n);
		autoNUMmatrix<double> cosinesTable (NUMcosinesTable (n), 1, 1);
		for (long i = 1 ; i <= n; i++) {
			x[i] = NUMrandomUniform (0, 70);
		}
		NUMcosineTransform (x.peek(), y.peek(), n, cosinesTable.peek());
		NUMinverseCosineTransform (y.peek(), x2.peek(), n, cosinesTable.peek());
		double delta = 0;
		for (long i =1 ; i <= n; i++) {
			double dif = x[i] - x2[i];
			delta += dif * dif;
		}
		delta = sqrt (delta);
		return delta;
	} catch (MelderError) {
		Melder_throw ("Test cosine transform error");
	}
}

MFCC MelFilter_to_MFCC (MelFilter me, long numberOfCoefficients) {
	try {
		autoNUMmatrix<double> cosinesTable (NUMcosinesTable (my ny), 1, 1);
		autoNUMvector<double> x (1, my ny);
		autoNUMvector<double> y (1, my ny);
		
		double fmax_mel = my y1 + (my ny - 1) * my dy;
		numberOfCoefficients = numberOfCoefficients > my ny - 1 ? my ny - 1 : numberOfCoefficients;
		Melder_assert (numberOfCoefficients > 0);
		// 20130220 new interpretation of maximumNumberOfCoefficients necessary for inverse transform 
		autoMFCC thee = MFCC_create (my xmin, my xmax, my nx, my dx, my x1, my ny - 1, my ymin, my ymax);
		for (long frame = 1; frame <= my nx; frame++) {
			CC_Frame cf = (CC_Frame) & thy frame[frame];
			for (long i = 1; i <= my ny; i++) {
				x[i] = my z[i][frame];
			}
			NUMcosineTransform (x.peek(), y.peek(), my ny, cosinesTable.peek());
			CC_Frame_init (cf, numberOfCoefficients);
			for (long i = 1; i <= numberOfCoefficients; i++) {
				cf -> c[i] = y[i + 1];
			}
			cf -> c0 = y[1];
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no MFCC created.");
	}
}

MelFilter MFCC_to_MelFilter (MFCC me, long first, long last) {
	try {
		long nf = my maximumNumberOfCoefficients + 1;
		autoNUMmatrix<double> cosinesTable (NUMcosinesTable (nf), 1, 1);
		autoNUMvector<double> x (1, nf);
		autoNUMvector<double> y (1, nf);

		if (first >= last) {
			first = 0; last = nf - 1;
		}

		if (first < 0 || last > nf - 1) {
			Melder_throw ("MFCC_to_MelFilter: coefficients must be in interval [0,", my maximumNumberOfCoefficients, "].");
		}
		double df = (my fmax - my fmin) / (nf + 1);
		autoMelFilter thee = MelFilter_create (my xmin, my xmax, my nx, my dx, my x1, my fmin, my fmax, nf, df, df);

		for (long frame = 1; frame <= my nx; frame++) {
			CC_Frame cf = (CC_Frame) & my frame[frame];
			long iend = MIN (last, cf -> numberOfCoefficients);
			x[1] = first == 0 ? cf -> c0 : 0;
			for (long i = 1; i <= my maximumNumberOfCoefficients; i++) {
				x[i + 1] = i < first || i > iend ? 0 : cf -> c[i];
			}
			NUMinverseCosineTransform (x.peek(), y.peek(), nf, cosinesTable.peek());
			for (long i = 1; i <= nf; i++) {
				
				thy z[i][frame] = y[i];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no MelFilter created.");
	}
}


MelFilter MFCC_to_MelFilter2 (MFCC me, long first_cc, long last_cc, double f1_mel, double df_mel) {
	try {
		int use_c0 = 0;
		long nf = ((my fmax - my fmin) / df_mel + 0.5);
		double fmin = MAX (f1_mel - df_mel, 0), fmax = f1_mel + (nf + 1) * df_mel;

		if (nf < 1) {
			Melder_throw ("MFCC_to_MelFilter: the position of the first filter, the distance between the filters, "
			"and, the maximum do not result in a positive number of filters.");
		}

		// Default values

		if (first_cc == 0) {
			first_cc = 1;
			use_c0 = 1;
		}
		if (last_cc == 0) {
			last_cc = my maximumNumberOfCoefficients;
		}

		// Be strict

		if (last_cc < first_cc || first_cc < 1 || last_cc > my maximumNumberOfCoefficients) {
			Melder_throw ("MFCC_to_MelFilter: coefficients must be in interval [1,", my maximumNumberOfCoefficients, "].");
		}
		autoNUMmatrix<double> dct (NUMcosinesTable (first_cc, last_cc, nf), first_cc, 1); // TODO ??
		//if ((dct = NUMcosinesTable (first_cc, last_cc, nf)) == NULL) return NULL;

		autoMelFilter thee = MelFilter_create (my xmin, my xmax, my nx, my dx, my x1, fmin, fmax, nf, df_mel, f1_mel);

		for (long frame = 1; frame <= my nx; frame++) {
			CC_Frame cf = (CC_Frame) & my frame[frame];
			long ie = MIN (last_cc, cf -> numberOfCoefficients);
			for (long j = 1; j <= nf; j++) {
				double t = 0;
				for (long i = first_cc; i <= ie; i++) {
					t += cf -> c[i] * dct[i][j];
				}

				// The inverse CT has a factor 1/N

				t /= nf;
				if (use_c0) {
					t +=  cf -> c0;
				}
				thy z[j][frame] = t;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no MelFilter created.");
	}
}

#undef MAX
#undef MIN

/* End of file MelFilter_and_MFCC.cpp */
