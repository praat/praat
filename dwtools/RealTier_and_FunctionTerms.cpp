/* RealTier_and_FunctionTerms.cpp
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

#include "ChebyshevTerms.h"
#include "LegendreTerms.h"
#include "Polynomial.h"
#include "RealTier_and_FunctionTerms.h"


#define FITTER_PARAMETER_FREE 0
#define FITTER_PARAMETER_FIXED 1

void FunctionTerms_RealTier_fit (FunctionTerms me, RealTier thee, INTVEC freeze, double tol, int ic, autoCovariance *c) {
	try {
		const integer numberOfData = thy points.size;
		const integer numberOfParameters = my numberOfCoefficients;
		Melder_require (numberOfData > 1,
			U"The number of data point should be larger than 1.");

		autoFunctionTerms frozen = Data_copy (me);
		autoVEC terms = newVECzero (my numberOfCoefficients);
		autoVEC p = newVECzero (numberOfParameters);
		autoVEC y_residual = newVECraw (numberOfData);
		autoCovariance ac;
		if (ic)
			ac = Covariance_create (numberOfParameters);

		integer k = 1;
		integer numberOfFreeParameters = numberOfParameters;
		for (integer j = 1; j <= my numberOfCoefficients; j ++)
			if (freeze.size > 0 && freeze [j])
				numberOfFreeParameters--;
			else {
				p [k ++] = my coefficients [j];
				frozen -> coefficients [j] = 0.0;
			}
		
		Melder_require (numberOfFreeParameters > 0,
			U"No free parameters left.");

		autoSVD svd = SVD_create (numberOfData, numberOfFreeParameters);

		const double sigma = RealTier_getStandardDeviation_points (thee, my xmin, my xmax);
		
		Melder_require (isdefined (sigma),
			U"Not enough data points in fit interval.");

		for (integer i = 1; i <= numberOfData; i ++) {
			/*
				Only 'residual variance' must be explained by the model
				Evaluate only with the frozen parameters
			*/
			RealPoint point = thy points.at [i];
			const double x = point -> number;
			const double y = point -> value;
			const double y_frozen = ( numberOfFreeParameters == numberOfParameters ? 0.0 :
				FunctionTerms_evaluate (frozen.get(), x));

			y_residual [i] = (y - y_frozen) / sigma;

			FunctionTerms_evaluateTerms (me, x, terms.get ());
			k = 0;
			for (integer j = 1; j <= my numberOfCoefficients; j ++)
				if (freeze.size == 0 || ! freeze [j])
					svd -> u [i] [++ k] = terms [j] / sigma;
		}
		/*
			SVD and evaluation of the singular values
		*/
		if (tol > 0.0)
			SVD_setTolerance (svd.get(), tol);

		SVD_compute (svd.get());
		autoVEC result = SVD_solve (svd.get(), y_residual.get());
		/*
			Put fitted values at correct position
		*/
		k = 1;
		for (integer j = 1; j <= my numberOfCoefficients; j ++)
			if (freeze.size == 0 || ! freeze [j])
				my coefficients [j] = result [k ++];
		if (ic)
			svdcvm (ac -> data.get(), svd -> v.get(), numberOfFreeParameters, freeze, svd -> d.get());
		if (c)
			*c = ac.move();
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no fit.");
	}
}


autoPolynomial RealTier_to_Polynomial (RealTier me, integer degree, double tol, int ic, autoCovariance *cvm) {
	try {
		autoPolynomial thee = Polynomial_create (my xmin, my xmax, degree);
		autoINTVEC nul;
		FunctionTerms_RealTier_fit (thee.get(), me, nul.get(), tol, ic, cvm);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Polynomial fitted.");
	}
}

autoLegendreTerms RealTier_to_LegendreTerms (RealTier me, integer degree, double tol, int ic, autoCovariance *cvm) {
	try {
		autoLegendreTerms thee = LegendreTerms_create (my xmin, my xmax, degree);
		autoINTVEC nul;
		FunctionTerms_RealTier_fit (thee.get(), me, nul.get(), tol, ic, cvm);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LegendreTerms fitted.");
	}
}

autoChebyshevSeries RealTier_to_ChebyshevSeries (RealTier me, integer degree, double tol, int ic, autoCovariance *cvm) {
	try {
		autoChebyshevSeries thee = ChebyshevSeries_create (my xmin, my xmax, degree);
		autoINTVEC nul;
		FunctionTerms_RealTier_fit (thee.get(), me, nul.get(), tol, ic, cvm);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U":no ChebyshevSeries fitted.");
	};
}

/* End of file RealTier_and_FunctionTerms.cpp */
