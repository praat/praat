/*ChebyshevSeries.cpp
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

#include "ChebyshevSeries.h"
#include "NUM2.h"

/****** ChebyshevSeries ******************************************/

Thing_implement (ChebyshevSeries, FunctionSeries, 0);

/*
	p(x) = sum (k=1..numberOfCoefficients, c [k]*T [k](x')) - c [1] / 2;
	Numerical evaluation via Clenshaw's recurrence equation (NRC: 5.8.11)
	d [m+1] = d [m] = 0;
	d [j] = 2 x' d [j+1] - d [j+2] + c [j];
	p(x) = d [0] = x' d [1] - d [2] + c [0] / 2;
	x' = (2 * x - xmin - xmax) / (xmax - xmin)
*/
double structChebyshevSeries :: v_evaluate (double x) {
	if (x < our xmin || x > our xmax)
		return undefined;

	double d1 = 0.0, d2 = 0.0;
	if (numberOfCoefficients > 1) {
		/*
			Transform x from domain [xmin, xmax] to domain [-1, 1]
		*/
		x = (2.0 * x - our xmin - our xmax) / (our xmax - our xmin);
		const double twox = 2.0 * x;
		for (integer i = our numberOfCoefficients; i > 1; i --) {
			const double tmp = d1;
			d1 = twox * d1 - d2 + our coefficients [i];
			d2 = tmp;
		}
	}
	return x * d1 - d2 + our coefficients [1];
}

/*
	T [n](x) = 2*x*T [n-1] - T [n-2](x)  n >= 2
*/
void structChebyshevSeries :: v_evaluateTerms (double x, VEC terms) {
	Melder_assert (terms.size == numberOfCoefficients);
	if (x < our xmin || x > our xmax) {
		terms  <<=  undefined;
		return;
	}
	terms [1] = 1.0;
	if (numberOfCoefficients > 1) {
		/*
			Transform x from domain [xmin, xmax] to domain [-1, 1]
		*/
		terms [2] = x = (2.0 * x - xmin - xmax) / (xmax - xmin);

		for (integer i = 3; i <= numberOfCoefficients; i ++)
			terms [i] = 2.0 * x * terms [i - 1] - terms [i - 2];
	}
}

void structChebyshevSeries :: v_getExtrema (double x1, double x2, double *out_xmin, double *out_ymin, double *out_xmax, double *out_ymax) {
	try {
		autoPolynomial p = ChebyshevSeries_to_Polynomial (this);
		FunctionSeries_getExtrema (p.get(), x1, x2, out_xmin, out_ymin, out_xmax, out_ymax);
	} catch (MelderError) {
		Melder_throw (this, U"Extrema cannot be calculated");
	}
}

autoChebyshevSeries ChebyshevSeries_create (double lxmin, double lxmax, integer numberOfPolynomials) {
	try {
		autoChebyshevSeries me = Thing_new (ChebyshevSeries);
		FunctionSeries_init (me.get(), lxmin, lxmax, numberOfPolynomials);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ChebyshevSeries not created.");
	}
}

autoChebyshevSeries ChebyshevSeries_createFromString (double lxmin, double lxmax, conststring32 s) {
	try {
		autoChebyshevSeries me = Thing_new (ChebyshevSeries);
		FunctionSeries_initFromString (me.get(), lxmin, lxmax, s, false);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ChebyshevSeries not created from string.");
	};
}

autoPolynomial ChebyshevSeries_to_Polynomial (ChebyshevSeries me) {
	try {
		const double xmin = -1.0, xmax = 1.0;

		autoPolynomial thee = Polynomial_create (xmin, xmax, my numberOfCoefficients - 1);

		thy coefficients [1] = my coefficients [1] /* * p [1] */;
		if (my numberOfCoefficients == 1)
			return thee;

		thy coefficients [2] = my coefficients [2];
		if (my numberOfCoefficients > 2) {
			autoVEC pn = zero_VEC (my numberOfCoefficients);
			autoVEC pnm1 = zero_VEC (my numberOfCoefficients);
			autoVEC pnm2 = zero_VEC (my numberOfCoefficients);

			// Start the recursion: T [2] = x; T [1] = 1;

			pnm1 [2] = 1.0;
			pnm2 [1] = 1.0;
			const double a = 2.0, b = 0.0, c = -1.0;
			for (integer n = 2; n <= my numberOfCoefficients - 1; n ++) {
				NUMpolynomial_recurrence (pn.part (1, n + 1), a, b, c, pnm1.get (), pnm2.get());
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
		Melder_throw (me, U"; not converted to Polynomial.");
	};
}

/* End of file ChebyshevSeries.cpp */
