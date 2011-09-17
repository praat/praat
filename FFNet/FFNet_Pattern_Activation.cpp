/* FFNet_Pattern_Activation.cpp
 *
 * Copyright (C) 1994-2011 David Weenink
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
 djmw 19960826
 djmw 20020712 GPL header
 djmw 20030701 Removed non-GPL minimizations
 djmw 20040416 More precise error messages.
 djmw 20041118 Added FFNet_Pattern_Categories_getCosts.
*/

#include "Graphics.h"
#include "FFNet_Pattern_Activation.h"

static double func (Data object, const double p[]) {
	FFNet me = (FFNet) object;
	Minimizer thee = my minimizer;
	double fp = 0;

	for (long j = 1, k = 1; k <= my nWeights; k++) {
		my dw[k] = 0.0;
		if (my wSelected[k]) {
			my w[k] = p[j++];
		}
	}
	for (long i = 1; i <= my nPatterns; i++) {
		FFNet_propagate (me, my inputPattern[i], NULL);
		fp += FFNet_computeError (me, my targetActivation[i]);
		FFNet_computeDerivative (me);
		/* derivative (cumulative) */
		for (long k = 1; k <= my nWeights; k++) {
			my dw[k] += my dwi[k];
		}
	}
	thy funcCalls++;
	return fp;
}

static void dfunc_optimized (Data object, const double p[], double dp[]) {
	FFNet me = (FFNet) object;
	(void) p;

	long j = 1;
	for (long k = 1; k <= my nWeights; k++) {
		if (my wSelected[k]) {
			dp[j++] = my dw[k];
		}
	}
}

static void _FFNet_Pattern_Activation_checkDimensions (FFNet me, Pattern p, Activation a) {
	if (my nInputs != p -> nx) Melder_throw (L"The Pattern and the FFNet do not match.\n"
		        "The number of colums in the Pattern must equal the number of inputs in the FFNet.");
	if (my nOutputs != a -> nx) Melder_throw (L"The Activation and the FFNet do not match.\n"
		        "The number of colums in the Activation must equal the number of outputs in the FFNet.");
	if (p -> ny != a -> ny) Melder_throw (L"The Pattern and the Activation do not match.\n"
		                                      "The number of rows in the Pattern must equal the number of rows in the Activation.");
	if (! _Pattern_checkElements (p)) Melder_throw (L"The elements in the Pattern are not all "
		        "in the interval [0, 1].\nThe input of the neural net can only process values that are between 0 "
		        "and 1.\nYou could use a \"Formula...\" to scale the Pattern values first.");
	if (! _Activation_checkElements (a)) Melder_throw (L"The elements in the Activation are not "
		        "all in the interval [0, 1].\nThe output of the neural net can only process values that are "
		        "between 0 and 1.\nYou could use \"Formula...\" to scale the Activation values first.");
}

static void _FFNet_Pattern_Activation_learn (FFNet me, Pattern pattern,
        Activation activation, long maxNumOfEpochs, double tolerance,
        Any parameters, int costFunctionType, int reset) {
	try {
		_FFNet_Pattern_Activation_checkDimensions (me, pattern, activation);
		Minimizer_setParameters (my minimizer, parameters);

		// Link the things to be learned

		my nPatterns = pattern -> ny;
		my inputPattern = pattern -> z;
		my targetActivation = activation -> z;
		FFNet_setCostFunction (me, costFunctionType);

		if (reset) {
			autoNUMvector<double> wbuf (1, my dimension);
			long k = 1;
			for (long i = 1; i <= my nWeights; i++) {
				if (my wSelected[i]) {
					wbuf[k++] = my w[i];
				}
			}
			Minimizer_reset (my minimizer, wbuf.peek());
		}

		Minimizer_minimize (my minimizer, maxNumOfEpochs, tolerance, 1);

		// Unlink

		my nPatterns = 0;
		my inputPattern = NULL;
		my targetActivation = NULL;
	} catch (MelderError) {
		my nPatterns = 0;
		my inputPattern = 0;
		my targetActivation = 0;
	}
}


void FFNet_Pattern_Activation_learnSD (FFNet me, Pattern p, Activation a,
                                       long maxNumOfEpochs, double tolerance, Any parameters, int costFunctionType) {
	int resetMinimizer = 0;
	/* Did we choose another minimizer */
	if (my minimizer != 0 && ! Thing_member (my minimizer, classSteepestDescentMinimizer)) {
		forget (my minimizer);
		resetMinimizer = 1;
	}
	/* create the minimizer if it doesn't exist */
	if (my minimizer == 0) {
		resetMinimizer = 1;
		my minimizer = (Minimizer) SteepestDescentMinimizer_create (my dimension, me, func, dfunc_optimized);
	}
	_FFNet_Pattern_Activation_learn (me, p, a, maxNumOfEpochs,
	                                 tolerance, parameters, costFunctionType, resetMinimizer);
}

void FFNet_Pattern_Activation_learnSM (FFNet me, Pattern p, Activation a,
                                       long maxNumOfEpochs, double tolerance, Any parameters, int costFunctionType) {
	int resetMinimizer = 0;
	/*
		Did we choose another minimizer
	*/
	if (my minimizer != 0 && ! Thing_member (my minimizer, classVDSmagtMinimizer)) {
		forget (my minimizer);
		resetMinimizer = 1;
	}
	/* create the minimizer if it doesn't exist */
	if (my minimizer == 0) {
		resetMinimizer = 1;
		my minimizer = (Minimizer) VDSmagtMinimizer_create (my dimension, me, func, dfunc_optimized);
	}
	_FFNet_Pattern_Activation_learn (me, p, a, maxNumOfEpochs,
	                                 tolerance, parameters, costFunctionType, resetMinimizer);
}

double FFNet_Pattern_Activation_getCosts_total (FFNet me, Pattern p, Activation a, int costFunctionType) {
	try {
		_FFNet_Pattern_Activation_checkDimensions (me, p, a);
		FFNet_setCostFunction (me, costFunctionType);

		double cost = 0;
		for (long i = 1; i <= p -> ny; i++) {
			FFNet_propagate (me, p -> z[i], NULL);
			cost += FFNet_computeError (me, a -> z[i]);
		}
		return cost;
	} catch (MelderError) {
		return NUMundefined;
	}
}

double FFNet_Pattern_Activation_getCosts_average (FFNet me, Pattern p, Activation a, int costFunctionType) {
	double costs = FFNet_Pattern_Activation_getCosts_total (me, p, a, costFunctionType);
	return costs == NUMundefined ? NUMundefined : costs / p -> ny;
}

Activation FFNet_Pattern_to_Activation (FFNet me, Pattern p, long layer) {
	try {
		if (layer < 1 || layer > my nLayers) {
			layer = my nLayers;
		}
		if (my nInputs != p -> nx) Melder_throw ("The Pattern and the FFNet do not match. "
			        "The number of colums in the Pattern (", p -> nx, ") must equal the number of inputs "
			        "in the FFNet (", my nInputs, ").");
		if (! _Pattern_checkElements (p)) Melder_throw
			("The elements in the Activation are not all in the interval [0, 1].\n"
			 "The output units of the neural net can only process values that are between 0 and 1.\n"
			 "You could use \"Formula...\" to scale the Activation values first.");

		long nPatterns = p -> ny;
		autoActivation thee = Activation_create (nPatterns, my nUnitsInLayer[layer]);

		for (long i = 1; i <= nPatterns; i++) {
			FFNet_propagateToLayer (me, p -> z[i], thy z[i], layer);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Activation created.");
	}
}

/* End of file FFNet_Pattern_Activation.cpp */
