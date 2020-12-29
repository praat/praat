/* LegendreSeries.cpp
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

#include "LegendreSeries.h"
#include "NUM2.h"

Thing_implement (LegendreSeries, FunctionSeries, 0);

double structLegendreSeries :: v_evaluate (double x) {
	if (x < our xmin || x > our xmax)
		return undefined;
	/*
		Transform x from domain [xmin, xmax] to domain [-1, 1]
	*/
	double p = our coefficients [1];
	double pim1 = x = (2 * x - our xmin - our xmax) / (our xmax - our xmin);
	if (numberOfCoefficients > 1) {
		const double twox = 2.0 * x;
		double pim2 = 1, f2 = x, d = 1.0;
		p += our coefficients [2] * pim1;
		for (integer i = 3; i <= our numberOfCoefficients; i ++) {
			const double f1 = d ++;
			f2 += twox;
			const double pi = (f2 * pim1 - f1 * pim2) / d;
			p += our coefficients [i] * pi;
			pim2 = pim1; 
			pim1 = pi;
		}
	}
	return p;
}

void structLegendreSeries :: v_evaluateTerms (double x, VEC terms) {
	Melder_assert (terms.size == numberOfCoefficients);
	if (x < our xmin || x > our xmax) {
		terms  <<=  undefined;
		return;
	}

	// Transform x from domain [xmin, xmax] to domain [-1, 1]

	x = (2.0 * x - our xmin - our xmax) / (our xmax - our xmin);

	terms [1] = 1.0;
	if (our numberOfCoefficients > 1) {
		const double twox = 2.0 * x;
		double f2 = x, d = 1.0;
		terms [2] = x;
		for (integer i = 3; i <= numberOfCoefficients; i ++) {
			const double f1 = d ++;
			f2 += twox;
			terms [i] = (f2 * terms [i - 1] - f1 * terms [i - 2]) / d;
		}
	}
}

void structLegendreSeries :: v_getExtrema (double x1, double x2, double *out_xmin, double *out_ymin, double *out_xmax, double *out_ymax) {
	try {
		autoPolynomial p = LegendreSeries_to_Polynomial (this);
		FunctionSeries_getExtrema (p.get(), x1, x2, out_xmin, out_ymin, out_xmax, out_ymax);
	} catch (MelderError) {
		structFunctionSeries :: v_getExtrema (x1, x2, out_xmin, out_ymin, out_xmax, out_ymax);
		Melder_clearError ();
	}
}

autoLegendreSeries LegendreSeries_create (double xmin, double xmax, integer numberOfPolynomials) {
	try {
		autoLegendreSeries me = Thing_new (LegendreSeries);
		FunctionSeries_init (me.get(), xmin, xmax, numberOfPolynomials);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LegendreSeries not created.");
	}
}

autoLegendreSeries LegendreSeries_createFromString (double xmin, double xmax, conststring32 s) {
	try {
		autoLegendreSeries me = Thing_new (LegendreSeries);
		FunctionSeries_initFromString (me.get(), xmin, xmax, s, false);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LegendreSeries not created from string.");
	}
}

autoLegendreSeries LegendreSeries_getDerivative (LegendreSeries me) {
	try {
		autoLegendreSeries thee = LegendreSeries_create (my xmin, my xmax, my numberOfCoefficients - 1);

		for (integer n = 1; n <= my numberOfCoefficients - 1; n ++) {
			/*
				P [n]'(x) = Sum (k=0..nonNegative, (2n - 4k - 1) P [n-2k-1](x))
			*/
			integer n2 = n - 1;
			for (integer k = 0; n2 >= 0; k ++, n2 -= 2)
				thy coefficients [n2 + 1] += (2 * n - 4 * k - 1) * my coefficients [n + 1];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no derivative created.");
	}
}

autoPolynomial LegendreSeries_to_Polynomial (LegendreSeries me) {
	try {
		const double xmin = -1, xmax = 1;
		autoPolynomial thee = Polynomial_create (xmin, xmax, my numberOfCoefficients - 1);

		thy coefficients [1] = my coefficients [1];   /* * p [1] */
		if (my numberOfCoefficients == 1)
			return thee;

		thy coefficients [2] = my coefficients [2];   /* * p [2] */
		if (my numberOfCoefficients > 2) {
			autoVEC pn = zero_VEC (my numberOfCoefficients);
			autoVEC pnm1 = zero_VEC (my numberOfCoefficients);
			autoVEC pnm2 = zero_VEC (my numberOfCoefficients);

			// Start the recursion: P [1] = x; P [0] = 1;

			pnm1 [2] = 1.0;
			pnm2 [1] = 1.0;
			for (integer n = 2; n <= my numberOfCoefficients - 1; n ++) {
				const double a = (2.0 * n - 1.0) / (double) n;
				const double c = - (n - 1.0) / (double) n;
				NUMpolynomial_recurrence (pn.part (1, n + 1), a, 0, c, pnm1.get(), pnm2.get());
				if (my coefficients [n + 1] != 0.0)
					for (integer j = 1; j <= n + 1; j ++)
						thy coefficients [j] += my coefficients [n + 1] * pn [j];
				autoVEC pn_old = std::move (pn);
				pn = std::move (pnm2);
				pnm2 = std::move (pnm1);
				pnm1 = std::move (pn_old);
			}
		}
		if (my xmin != xmin || my xmax != xmax)
			thee = Polynomial_scaleX (thee.get(), my xmin, my xmax);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Polynomial.");
	}
}

/* end of file LegendreSeries.cpp */
